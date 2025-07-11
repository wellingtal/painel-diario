#include "stdafx.h"

#include "Session.h"
#include "../IOCP/Overlapped.h"
#include "../IOCP/CompletionHandler.h"
#include "../Winsock/SocketFactory.h"
#include "../Address/INET_Addr.h"

#include "../Dispatch/Dispatch.h"
#include "../Dispatch/ServerRequest.h"
#include "../../Stream/Buffer/Buffer.h"
#include "../../Stream/Buffer/BufferFactory.h"

#include "../../Log/ServerLog.h"

#include <iostream>

// SP : SessionPointer
// DP : DispatchPointer

namespace SessionState
{
    enum Type
    {   
        CONNECTED,			// 연결 성공.
        DISCONNECTED,		// 접속이 해제되었음.
        MAX_SESSION_STATE
    };
};

namespace SessionFlag
{
    // 8bit SessionFlag
    enum Type
    {
        CONNECT_CALLED          =   (  1 <<  0 ),   // 연결 오픈 처리를 했음
        DISCONNECT_PROCESSED    =   (  1 <<  1 ),   // 연결 종료 처리를 했음
        SHUTDOWN_CALLED         =   (  1 <<  2 ),   // Shutdown을 호출했음
		SHUTDOWN_PROCESSED		=	(  1 <<  3 ),   // Shutdown 처리를 했음.

        SUSPEND_RECV            =   (  1 <<  4 )    // 플래그가 세팅되어 있으면 Recv를 멈춤
    };
};

namespace SessionConst
{
	enum Data
	{
		MAX_RECV_BUFFER		= 32768
	};
}

CSession::CSession(CSessionPolicy& SessionPolicy)
:	m_hSocket(INVALID_SOCKET),
    m_nRefCount(0),
    m_SessionPolicy(SessionPolicy),

    m_dwRecvPending(0),
    m_dwSendPending(0),   

    m_cCurrentStatus(SessionState::CONNECTED), 
    m_cFlags(0),
    m_usPadding(0x4343)
{
    m_SessionPolicy.AddRef();

	m_lpRecvBuffer	= CREATE_BUFFER(SessionPolicy.GetBufferFactory(), SessionConst::MAX_RECV_BUFFER);
	m_lpDispatch	= SessionPolicy.GetDispatchFactory().CreateDispatch(*this);
}


CSession::~CSession()
{	
	{
		SessionLock::Syncronize sync(m_SessionLock);

		InternalCloseSocket();
		
		SAFE_RELEASE_BUFFER(m_lpRecvBuffer);
		m_SessionPolicy.GetDispatchFactory().DeleteDispatch(m_lpDispatch);

		m_usPadding = 0x4444;
	}

    m_SessionPolicy.Release();
}


// false 리턴시 Accept Pending을 함.
bool CSession::Process()
{
	if(!m_lpDispatch)
	{
		ERRLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/CloseSocket - Failed Dispatch Pointer",
			this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

		return false;
	}

    SessionState::Type  eState          = SessionState::MAX_SESSION_STATE;
    unsigned char       cFlags          = 0;

    bool bCloseSocket		= false;
	bool bCallConnected		= false;
	bool bCallDisconnected	= false;

    {
        SessionLock::Syncronize sync(m_SessionLock);

        eState      = static_cast<SessionState::Type>(m_cCurrentStatus);
        
        switch(m_cCurrentStatus)
        {
        case SessionState::CONNECTED:

			if(SessionFlag::CONNECT_CALLED != (m_cFlags & SessionFlag::CONNECT_CALLED))
			{
				bCallConnected = true;
                m_cFlags |= SessionFlag::CONNECT_CALLED;
			}

            if (!(m_cFlags & SessionFlag::SUSPEND_RECV) && 0 == m_dwRecvPending)
            {
                // 데이터를 받을 수 있으면 데이터를 받는다.
                InternalRecv();
            }
            break;

        case SessionState::DISCONNECTED:

            if(SessionFlag::DISCONNECT_PROCESSED != (m_cFlags & SessionFlag::DISCONNECT_PROCESSED))
            {
				bCallDisconnected = true;
                m_Statistics.m_DisconnectedTime = time(0);
                m_cFlags |= SessionFlag::DISCONNECT_PROCESSED;
            }
            else if(INVALID_SOCKET == m_hSocket &&
                0 == m_dwRecvPending &&
                0 == m_dwSendPending &&
                0 == m_nRefCount)
            {                
                InternalCloseSocket();

				DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/Eliminate Session",
					this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

                return false;
            }

            break;
        }        

		cFlags = m_cFlags;

        ++m_nRefCount;
    }

    // 연결 오픈 처리를 함.
    if(bCallConnected)
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/Connection Open - Process connected",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

        m_lpDispatch->Connected();

        m_Statistics.m_ConnectedTime = time(0);
    }

    //  받은 패킷을 Dispatch함.
    if(SessionState::CONNECTED == eState && !m_lpDispatch->Dispatch())
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/CloseSocket - Failed Dispatch",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

        bCloseSocket = true;
    }

    // 연결 닫기 처리를 함.
    if(bCallDisconnected)
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ Connection Closed - Process disconnect",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);
		
        m_lpDispatch->Disconnected();
    }

    {
        SessionLock::Syncronize sync(m_SessionLock);

        // 전송 처리를 한다.
        InternalSend();
        
		// 전송이 전부 끝나고 Shutdown을 호출했었으면 Shutdown 처리를 함.
		if((m_cFlags & SessionFlag::SHUTDOWN_CALLED) && 
			m_SendQueue.empty() && INVALID_SOCKET != m_hSocket)
		{
			if(shutdown(m_hSocket, SD_SEND))
			{
				m_cFlags |= SessionFlag::SHUTDOWN_PROCESSED;
			}
		}

        --m_nRefCount;

        if(bCloseSocket)
        {
            InternalCloseSocket();
        }
    }

    return true;
}


bool CSession::Dispatch(unsigned long dwReceivedBytes)
{
    // 패킷 처리를 하고, 처리하지 못한 양을 앞으로 갖다 붙인다.
    m_lpRecvBuffer->wr_ptr(dwReceivedBytes);
    unsigned long dwDispatchSize = static_cast<unsigned long>(m_lpRecvBuffer->length());

    // 받은 바이트 수가 0(접속 완료) 이거나, 패킷 처리에 실패하면 Socket을 Close한다.
	bool bResult = (0 != dwReceivedBytes && 
        m_lpDispatch->ParsePacket(m_lpRecvBuffer->rd_ptr(), &dwDispatchSize));
    
    m_lpRecvBuffer->rd_ptr(dwDispatchSize);
    m_lpRecvBuffer->pop_read_data();

    SessionLock::Syncronize sync(m_SessionLock);

    m_Statistics.m_dwTotalReceived += dwReceivedBytes;
    --m_dwRecvPending;
    
    if (!bResult)
    {
        DETLOG7(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/shutdown - %s : dwReceivedBytes:%d, ErrorCode:%d",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket,
            (0 == dwReceivedBytes) ? "Disconnected" : "Failed To Dispatch Packet",
            dwReceivedBytes, WSAGetLastError());

		InternalCloseSocket();
    }

    return bResult;
}

void CSession::InternalCloseSocket()
{    
    if(INVALID_SOCKET != m_hSocket)
    {
        const char* szRemoteAddr = m_RemoteAddr.get_addr_string();

        if(SOCKET_ERROR == shutdown(m_hSocket, SD_SEND))
        {
            DETLOG5(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ Shutdown failed - ErrorCode:%d",
                this, m_lpDispatch, szRemoteAddr, m_hSocket, WSAGetLastError());
        }

        if(SOCKET_ERROR == closesocket(m_hSocket))
        {
            DETLOG5(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket failed - ErrorCode:%d",
                this, m_lpDispatch, szRemoteAddr, m_hSocket, WSAGetLastError());
        }

        const int   MAX_BUFFER = 1024;
        char        szBuffer[MAX_BUFFER];

        if(0 < InternalPrintStatistics(szBuffer, MAX_BUFFER))
        {
            DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/ InternalCloseSocket - Statistics : %s", 
                this, m_lpDispatch, szRemoteAddr, szBuffer);
        }

        m_cFlags |= SessionFlag::SHUTDOWN_PROCESSED;
        m_hSocket = INVALID_SOCKET;
    }

    m_cCurrentStatus = SessionState::DISCONNECTED;
}


bool CSession::Shutdown()
{
    SessionLock::Syncronize sync(m_SessionLock);
	m_cFlags |= SessionFlag::SHUTDOWN_CALLED;
    return false;
}

LONG CSession::AddRef()
{
    SessionLock::Syncronize sync(m_SessionLock);
    return ++m_nRefCount;
}


LONG CSession::Release()
{
    SessionLock::Syncronize sync(m_SessionLock);
    return --m_nRefCount;
}


void CSession::CloseSession()
{
	SessionLock::Syncronize sync(m_SessionLock);

    DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/ InternalCloseSocket - SessionClose Called : %d", 
        this, m_lpDispatch, m_RemoteAddr.get_addr_string(), WSAGetLastError());

	InternalCloseSocket();
}


bool CSession::IsConnected()
{
	SessionLock::Syncronize sync(m_SessionLock);
	return (SessionState::CONNECTED == m_cCurrentStatus && m_hSocket != INVALID_SOCKET);
}


bool CSession::CalledConnected()
{
    SessionLock::Syncronize sync(m_SessionLock);
    return SessionFlag::CONNECT_CALLED == (m_cFlags & SessionFlag::CONNECT_CALLED);
}

bool CSession::CalledDisconnected()
{
    SessionLock::Syncronize sync(m_SessionLock);
    return SessionFlag::DISCONNECT_PROCESSED == (m_cFlags & SessionFlag::DISCONNECT_PROCESSED);
}

void CSession::SendCompleted(BOOL bResult, unsigned long dwSendedBytes)
{
    SessionLock::Syncronize sync(m_SessionLock);

    --m_dwSendPending;
    m_Statistics.m_dwTotalSendCompleted += dwSendedBytes;

    if(!bResult)
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/ InternalCloseSocket - Send Completion Error : %d", 
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), WSAGetLastError());

        InternalCloseSocket();
    }
    else
    {
        InternalSend();
    }
}


void CSession::SetAddress(INET_Addr& remoteAddr, INET_Addr& localAddr)
{
	m_RemoteAddr = remoteAddr;
	m_LocalAddr = localAddr;
}


std::ostream& CSession::PrintStatistics(std::ostream& strm)
{
    SessionLock::Syncronize sync(m_SessionLock);

    const int   MAX_BUFFER = 1024;
    char        szBuffer[MAX_BUFFER];

    int nLen = InternalPrintStatistics(szBuffer, MAX_BUFFER);

    if(0 < nLen)
    {
        strm << szBuffer;
    }

    return strm;
}


int CSession::InternalPrintStatistics(char* szBuffer, int nBufferLen)
{
    const char* szMode  = 0;
    
    switch(m_cCurrentStatus)
    {
    case SessionState::CONNECTED:       szMode = "CONNECTED";       break;
    case SessionState::DISCONNECTED:    szMode = "DISCONNECTED";    break;
    default:                            szMode = "Unknown Mode";    break;
    }

    int nLen = _snprintf(szBuffer, nBufferLen - 1, 
        "[m_hSocket:0x%08x][%15s][DiffTime(Sec):%8f]"
        "[m_dwTotalReceived:%5d][m_dwTotalSendPending:%5d][m_dwTotalSendCompleted:%5d]"
        "[m_dwSendPending:%d][m_dwRecvPending:%d][m_nRefCount:%2d][flags:0x%02x]",
        
        m_hSocket, szMode, difftime(time(0), m_Statistics.m_ConnectedTime), 
        m_Statistics.m_dwTotalReceived, m_Statistics.m_dwTotalSendPending, 
        m_Statistics.m_dwTotalSendCompleted, 
        m_dwSendPending, m_dwRecvPending,        
        m_nRefCount, m_cFlags);

    szBuffer[nBufferLen - 1] = 0;
    return nLen;
}


bool CSession::InternalRecv()
{
    if (m_cFlags & SessionFlag::SUSPEND_RECV)
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ Recv suspending now",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

        return true;
    }
    else if (0 != m_dwRecvPending)
    {
        DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ Recv pending now",
            this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);

        return true;
    }

    unsigned long dwReceived = 0;
    unsigned long dwFlags    = 0;

    WSABUF wsaBuf;
    
    wsaBuf.buf = m_lpRecvBuffer->wr_ptr();
    wsaBuf.len = static_cast<u_long>(m_lpRecvBuffer->remain());

    COverlapped* lpOverlapped = m_SessionPolicy.GetOverlappedFactory().CreateRecv(this, m_lpRecvBuffer);

    const char* lpErrorFmtString = 0;

    if(0 != lpOverlapped)
    {
        if(SOCKET_ERROR == WSARecv(m_hSocket, &wsaBuf, 1, 
            &dwReceived, &dwFlags, lpOverlapped, 0))
        {            
            if(WSA_IO_PENDING != WSAGetLastError())
            {
                lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/InternalCloseSocket"
                    " - WSARecv Error - ErrorCode:%d";
            }
        }
    }
    else
    {
        lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/InternalCloseSocket"
            " - WSARecv Error - Create RecvOverlapped Failed - ErrorCode:%d";
    }

    if(0 != lpErrorFmtString)
    {
        DETLOG5(g_SessionLog, lpErrorFmtString, this, m_lpDispatch, 
            m_RemoteAddr.get_addr_string(), m_hSocket, WSAGetLastError());

        InternalCloseSocket();

        m_SessionPolicy.GetOverlappedFactory().DeleteOverlapped(lpOverlapped);
        return false;
    }

    ++m_dwRecvPending;
    return true;
}


bool CSession::InternalRecvFrom()
{
    if(0 != m_dwRecvPending)
    {
        //DETLOG4(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ RecvFrom pending now",
        //    this, m_lpDispatch, m_RemoteAddr.get_addr_string(), m_hSocket);
        return true;
    }

    if(INVALID_SOCKET == m_hSocket)
    {
        return false;
    }

    unsigned long dwReceived = 0;
    unsigned long dwFlags = 0;

    WSABUF wsaBuf;

    wsaBuf.buf = m_lpRecvBuffer->wr_ptr();
    wsaBuf.len = static_cast<u_long>(m_lpRecvBuffer->remain());

    COverlapped* lpOverlapped = 
        m_SessionPolicy.GetOverlappedFactory().CreateRecv(this, m_lpRecvBuffer);

    const char* lpErrorFmtString = 0; 

    if(0 != lpOverlapped)
    {
        INET_Addr& addr = m_lpRecvBuffer->get_addr();

        if(SOCKET_ERROR == WSARecvFrom(m_hSocket, &wsaBuf, 1, &dwReceived, &dwFlags, 
            &addr.get_addr(), addr.get_size_ptr(), lpOverlapped, 0))
        {
            if(WSA_IO_PENDING != WSAGetLastError())
            {
                lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket"
                    " - WSARecvFrom Error - ErrorCode:%d";
            }
        }
    }
    else
    {
        lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/InternalCloseSocket"
            " - WSARecvFrom Error - Create RecvOverlapped Failed - ErrorCode:%d";
    }

    if(0 != lpErrorFmtString)
    {
        DETLOG5(g_SessionLog, lpErrorFmtString, this, m_lpDispatch, 
            m_RemoteAddr.get_addr_string(), m_hSocket, WSAGetLastError());

        InternalCloseSocket();
        
        if(0 != lpOverlapped)
        {
            m_SessionPolicy.GetOverlappedFactory().DeleteOverlapped(lpOverlapped);
        }
        
        return false;
    }

    ++m_dwRecvPending;
    return true;
}


CSession::SendResult CSession::InternalSend(CBuffer* lpBuffer)
{
    if(0 != m_dwSendPending)
    {
        return SEND_NEXT_TURN;
    }

    if(INVALID_SOCKET == m_hSocket)
    {
        return SEND_FAILED;
    }

    const char*     lpErrorFmtString = 0;
    COverlapped*    lpOverlapped = 0;

    if(0 != lpBuffer)
    {        
        lpOverlapped = m_SessionPolicy.GetOverlappedFactory().CreateSend(this, lpBuffer);

        if(0 != lpOverlapped)
        {
            unsigned long dwSentBytes = 0;

            WSABUF wsaBuf;

            wsaBuf.buf = lpBuffer->rd_ptr();
            wsaBuf.len = static_cast<u_long>(lpBuffer->length());

            m_Statistics.m_dwTotalSendPending += wsaBuf.len;

            if(SOCKET_ERROR == WSASend(m_hSocket, &wsaBuf, 1, 
                &dwSentBytes, 0, lpOverlapped, 0))
            {                
                if(WSA_IO_PENDING != WSAGetLastError())
                {
                    lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ "
                        "InternalCloseSocket - WSASend Error - ErrorCode:%d";
                }
            }
        }
        else
        {
            lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket"
                " - WSASend Error - Cannot Create SendOverlapped - ErrorCode:%d";
        }
    }
    else
    {
        lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket"
            " - WSASend Error - lpBuffer is 0 - ErrorCode:%d";
    }

    if(0 != lpErrorFmtString)
    {
        DETLOG5(g_SessionLog, lpErrorFmtString, this, m_lpDispatch, 
            m_RemoteAddr.get_addr_string(), m_hSocket, WSAGetLastError());

        InternalCloseSocket();

        if(0 != lpOverlapped)
        {
            lpOverlapped->SetBuffer(0);
            m_SessionPolicy.GetOverlappedFactory().DeleteOverlapped(lpOverlapped);
        }
       
        return SEND_FAILED;
    }

    ++m_dwSendPending;
	return SEND_SUCCEEDED;
}



CSession::SendResult CSession::InternalSendTo(CBuffer* lpBuffer)
{
    if(0 != m_dwSendPending)
    {
        return SEND_NEXT_TURN;
    }

    if(INVALID_SOCKET == m_hSocket)
    {
        return SEND_FAILED;
    }

    const char*     lpErrorFmtString = 0;
    COverlapped*    lpOverlapped = 0;

    if(0 != lpBuffer)
    {        
        lpOverlapped = m_SessionPolicy.GetOverlappedFactory().CreateSend(this, lpBuffer);

        if(0 != lpOverlapped)
        {
            unsigned long dwSentBytes = 0;

            WSABUF wsaBuf;

            wsaBuf.buf = lpBuffer->rd_ptr();
            wsaBuf.len = static_cast<u_long>(lpBuffer->length());

            m_Statistics.m_dwTotalSendPending += wsaBuf.len;

            INET_Addr& addr = lpBuffer->get_addr();

            if(SOCKET_ERROR == WSASendTo(m_hSocket, &wsaBuf, 1, 
                &dwSentBytes, 0, &addr.get_addr(), addr.get_size(), lpOverlapped, 0))
            {                
                if(WSA_IO_PENDING != WSAGetLastError())
                {
                    lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ "
                        "InternalCloseSocket - WSASendTo Error - ErrorCode:%d";
                }
            }
        }
        else
        {
            lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket"
                " - WSASendTo Error - Cannot Create SendOverlapped - ErrorCode:%d";
        }
    }
    else
    {
        lpErrorFmtString = "SP:0x%p/DP:0x%p/IP:%15s/SOCKET:0x%p/ InternalCloseSocket"
            " - WSASendTo Error - lpBuffer is 0 - ErrorCode:%d";
    }

    if(0 != lpErrorFmtString)
    {
        DETLOG5(g_SessionLog, lpErrorFmtString, this, m_lpDispatch, 
            m_RemoteAddr.get_addr_string(), m_hSocket, WSAGetLastError());

        InternalCloseSocket();

        if(0 != lpOverlapped)
        {
            lpOverlapped->SetBuffer(0);
            m_SessionPolicy.GetOverlappedFactory().DeleteOverlapped(lpOverlapped);
        }
        
        return SEND_FAILED;
    }

    ++m_dwSendPending;
	return SEND_SUCCEEDED;
}

bool CSession::SendPending(CBuffer* lpBuffer)
{
    SessionLock::Syncronize sync(m_SessionLock);

	if(!(SessionFlag::SHUTDOWN_CALLED & m_cFlags) && INVALID_SOCKET != m_hSocket)
	{
		m_SendQueue.enqueue(lpBuffer);
		return true;
	}
	
	return false;	
}

bool CSession::InternalSend()
{        
    SendResult eSendResult = SEND_SUCCEEDED;

    if(0 == m_dwSendPending)
    {
        CBuffer* lpBuffer = m_SendQueue.dequeue();
    
        if(0 != lpBuffer)
        {
            eSendResult = (0 == lpBuffer->get_addr().get_size()) ?
                InternalSend(lpBuffer) : InternalSendTo(lpBuffer);

            if(SEND_SUCCEEDED != eSendResult)
            {
                m_SendQueue.enqueue(lpBuffer, true);
            }
        }
    }

    return (SEND_FAILED != eSendResult);
}

void CSession::SuspendRecv()
{
    SessionLock::Syncronize sync(m_SessionLock);
    m_cFlags |= SessionFlag::SUSPEND_RECV;
}

void CSession::ResumeRecv()
{
    SessionLock::Syncronize sync(m_SessionLock);
    m_cFlags &= ~SessionFlag::SUSPEND_RECV;
}
