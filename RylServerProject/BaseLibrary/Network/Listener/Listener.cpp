#include "stdafx.h"
#include "Listener.h"

#include "../Session/Session.h"
#include "../Session/SessionMgr.h"
#include "../Dispatch/Dispatch.h"
#include "../Winsock/SocketFactory.h"

#include "../IOCP/CompletionHandler.h"
#include "../IOCP/Overlapped.h"

#include "../../Stream/Buffer/Buffer.h"
#include "../../Stream/Buffer/BufferFactory.h"

#include "../../Utility/Debug/DebugUtils.h"
#include "../../Utility/Resource/EnsureCleanup.h"

#include "../../Log/ServerLog.h"

#include <mmsystem.h>
#include <iostream>


enum LISTENER_CONSTANT
{
    DEFAULT_PENDING_NUM = 10
};



CListener::CListener(CCompletionHandler& SocketHandler, 					 
					 CSessionPolicy& SessionPolicy,
					 CSessionMgr& SessionMgr,
					 CValidateConnection* lpValidateConnection)
:   m_hListen(INVALID_SOCKET),    
    m_dwMaxPending(DEFAULT_PENDING_NUM),
	m_dwCurrentPending(0),    
	m_dwTotalPendingCount(0),
	m_dwTotalAcceptCompleteCount(0),
    m_SocketHandler(SocketHandler),	
    m_SessionPolicy(SessionPolicy),
	m_SessionMgr(SessionMgr),
	m_lpValidateConnection(lpValidateConnection)
{    
	m_SessionPolicy.AddRef();

	if(0 != m_lpValidateConnection)
	{
		m_lpValidateConnection->AddRef();
	}
}


CListener::~CListener()
{
	{
        SessionLock::Syncronize sync(m_ListenerLock);
		InternalCloseListen();
	}

	WaitForPendingComplete(5000);

	m_SessionPolicy.Release();

	if(0 != m_lpValidateConnection)
	{
		m_lpValidateConnection->Release();
	}
}


bool CListener::Initialize(INET_Addr& addrListen, unsigned long dwMaxPending)
{
    if(!m_SessionPolicy.IsValid())
    {
		ERRLOG0(g_SessionLog, "CListener initialized failed : Invalid SessionPolicy");
        return false;
    }
	else
	{
        SessionLock::Syncronize sync(m_ListenerLock);
		
		InternalCloseListen();

		const sockaddr_in& addr_in = addrListen.get_addr_in();

		m_hListen = m_SessionPolicy.GetSocketFactory().CreateListenSocket(
			(0 != addr_in.sin_addr.S_un.S_addr) ? inet_ntoa(addr_in.sin_addr) : 0,
			addrListen.get_port_in());

		if(SOCKET_ERROR == m_hListen)
		{
			ERRLOG1(g_SessionLog, "CListener initialized failed : Create listen socket failed : %d",
				WSAGetLastError());
			return false;
		}

		// 리슨 소켓을 Handler에 Attach한다.
		if(!m_SocketHandler.AttachToHander(
			reinterpret_cast<HANDLE>(m_hListen), reinterpret_cast<ULONG_PTR>(this)))
		{
			ERRLOG1(g_SessionLog, "CListener initialized failed : Attach to Handler failed : %d",
				WSAGetLastError());
			return false;
		}

		// Pending을 몇개 해 놓는다.
		if(m_dwMaxPending < dwMaxPending)
		{
			m_dwMaxPending = dwMaxPending;
		}

		dwMaxPending = m_dwMaxPending;
	}
	
	for(unsigned long dwCount = 0; dwCount < dwMaxPending; ++dwCount)
	{
		PendingAccept();
	}

    return true;
}


void CListener::InternalCloseListen()
{
	if(INVALID_SOCKET != m_hListen)
	{
		if(SOCKET_ERROR == closesocket(m_hListen))
		{
			ERRLOG1(g_SessionLog, "closesocket failed : %d", WSAGetLastError());			
		}

		m_hListen = INVALID_SOCKET;
	}
}

void CListener::WaitForPendingComplete(unsigned long dwTime)
{
	unsigned long dwStartTime = timeGetTime();	

	for(;;)
	{
		long diffTime = timeGetTime() - dwStartTime;
		
		if(dwTime < static_cast<unsigned long>(std::abs(diffTime)))
		{
			break;
		}

		{
			SessionLock::Syncronize sync(m_ListenerLock);
			if(0 == m_dwCurrentPending)
			{
				break;
			}
		}

		Sleep(10);
	}
}

bool CListener::PendingAccept()
{
	SessionLock::Syncronize sync(m_ListenerLock);

	if(INVALID_SOCKET == m_hListen)
	{
		return false;
	}

	unsigned long	dwAddressLen	= sizeof(SOCKADDR_IN) + 16;
	unsigned long	dwBytesReceived	= 0;

    SOCKET		 hSocket      = m_SessionPolicy.GetSocketFactory().CreateSocket();
	CBuffer*	 lpAddrBuffer = CREATE_BUFFER(m_SessionPolicy.GetBufferFactory(), dwAddressLen * 2);
	COverlapped* lpOverlapped = m_SessionPolicy.GetOverlappedFactory().CreateAccept(this, hSocket, lpAddrBuffer);

    if(SOCKET_ERROR == AcceptEx(m_hListen, hSocket, lpAddrBuffer->wr_ptr(),
        0, dwAddressLen, dwAddressLen, &dwBytesReceived, lpOverlapped))
    {            
        if(WSA_IO_PENDING != WSAGetLastError())
        {
			// 버퍼는 내부에서 삭제됨.
            m_SessionPolicy.GetOverlappedFactory().DeleteOverlapped(lpOverlapped);
			closesocket(hSocket);
			return false;
        }
    }
	
	++m_dwCurrentPending;
	++m_dwTotalPendingCount;
	return true;
}


void CListener::ProcessAccept(BOOL bResult, SOCKET hSocket,
							  CBuffer* lpBuffer, unsigned long dwProcessedBytes)
{
	INET_Addr localAddr;
	INET_Addr remoteAddr;

	if(bResult)
	{
		// 소켓 옵션 설정.
		if(SOCKET_ERROR == setsockopt(hSocket, SOL_SOCKET,
			SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_hListen, sizeof(m_hListen)))
		{
			bResult = FALSE;
		}
		else
		{
			SOCKADDR* lpLocalAddr = 0;
			SOCKADDR* lpRemoteAddr = 0;

			int	nLocalSockAddrLen = 0;
			int	nRemoteSockAddrLen = 0;
		        
			unsigned long dwAddressLen = sizeof(SOCKADDR_IN) + 16;

            // 주소 분석 및 주소 객체 생성.
			GetAcceptExSockaddrs(lpBuffer->rd_ptr(), 0, dwAddressLen, dwAddressLen, 
				&lpLocalAddr, &nLocalSockAddrLen, &lpRemoteAddr, &nRemoteSockAddrLen);

			if(0 != lpLocalAddr)
			{
                localAddr.set_addr(*lpLocalAddr, nLocalSockAddrLen);				
			}

			if(0 != lpRemoteAddr)
			{
				remoteAddr.set_addr(*lpRemoteAddr, nRemoteSockAddrLen);
			}
		}
	}

	if(bResult && 0 != m_lpValidateConnection 
		&& !(*m_lpValidateConnection)(localAddr, remoteAddr))
	{
		bResult = FALSE;
	}

	// 세션 생성
	CSession* lpSession = 0;
	
	if(bResult)
	{
		lpSession = m_SessionMgr.CreateSession(m_SessionPolicy);

		if(0 == lpSession || !lpSession->isValid())
		{
			bResult = FALSE;
		}
	}

	if(bResult && !m_SocketHandler.AttachToHander(
		reinterpret_cast<HANDLE>(hSocket), 
		reinterpret_cast<ULONG_PTR>(lpSession)))
	{
		bResult = FALSE;
	}

	if(bResult)
	{
		lpSession->Socket(hSocket);
		lpSession->SetAddress(remoteAddr, localAddr);

		DETLOG3(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/ Accept Session Complete.", 
			lpSession, lpSession->GetDispatch(), remoteAddr.get_addr_string());		

		lpSession->InternalRecv();

		m_SessionMgr.Add(lpSession);		
	}
	else
	{
		m_SessionMgr.DeleteSession(lpSession);		
		lpSession = 0;

		// pending failed
		closesocket(hSocket);		
	}

	{
        SessionLock::Syncronize sync(m_ListenerLock);
		--m_dwCurrentPending;
		++m_dwTotalAcceptCompleteCount;
	}

	PendingAccept();
}

unsigned long CListener::GetPendingAcceptNum()
{
    SessionLock::Syncronize sync(m_ListenerLock);
	return m_dwCurrentPending;
}


std::ostream& CListener::PrintStatistics(std::ostream& strm)
{
    SessionLock::Syncronize sync(m_ListenerLock);

    const int   MAX_BUFFER = 1024;
    char        szBuffer[MAX_BUFFER];

    int nLen = InternalPrintStatistics(szBuffer, MAX_BUFFER);

    if(0 < nLen)
    {
        strm << szBuffer;
    }

    return strm;
}


int CListener::InternalPrintStatistics(char* szBuffer, int nBufferLen)
{
	return _snprintf(szBuffer, nBufferLen, 
		"[ListenSocket:0x%08x][MaxPending:%d][CurrentPending:%d]"
		"[TotalPendingCount:%d][TotalAcceptCompleteCount:%d]",
		m_hListen, m_dwMaxPending, m_dwCurrentPending, 
		m_dwTotalPendingCount, m_dwTotalAcceptCompleteCount);
}


/*
bool CListener::LogListenerStatus()
{
    using namespace std;
            
    unsigned int nStateNum[CSession::MAX_SESSION_STATE];
    std::fill_n(nStateNum, unsigned int(CSession::MAX_SESSION_STATE), 0);

    const char* szDelimiter = "// ------------------------------------------------------------------------------- ";

    char szProgramName[MAX_PATH];
    char szFileName[MAX_PATH];
    char szTime[MAX_PATH];

    SYSTEMTIME  systemTime;
    GetLocalTime(&systemTime);

    DbgUtils::SetProgramName(szProgramName, MAX_PATH);

    _snprintf(szTime, MAX_PATH, "%04d%02d%02d-%02d%02d%02d", 
        systemTime.wYear, systemTime.wMonth, systemTime.wDay, 
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    _snprintf(szFileName, MAX_PATH, "%s/ListenerLog-%s.txt", szProgramName, szTime);

    fstream             logfile(szFileName, ios::out | ios::app);
    fstream::char_type  endline = logfile.widen('\n');

    logfile << szDelimiter << endline;

    SessionSet::iterator pos = m_SessionSet.begin();
    SessionSet::iterator end = m_SessionSet.end();

    for(; pos != end; ++pos)
    {
        CSession* lpSession = *pos;
        logfile << lpSession << endline;
        ++nStateNum[lpSession->m_cCurrentStatus];
    }
    
    logfile << endline << szDelimiter << endline
        << "Time           : " << szTime << endline << endline
        << "UNINITIALIZED  : " << nStateNum[CSession::UNINITIALIZED]  << endline
        << "ACCEPT_PENDING : " << nStateNum[CSession::ACCEPT_PENDING] << endline
        << "CONNECTED      : " << nStateNum[CSession::CONNECTED]      << endline
        << "DISCONNECTED   : " << nStateNum[CSession::DISCONNECTED]   << endline
        << "Total          : " << m_dwMaxConnections << endl;
        
    return true;
}
*/


LONG CValidateConnection::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

LONG CValidateConnection::Release()
{
    LONG nRefCount = InterlockedDecrement(&m_nRefCount);
    if(0 == nRefCount)
    {
        delete this;
    }

    return nRefCount;
}
