#ifndef _CSESSION_H_
#define _CSESSION_H_

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <iosfwd>

#include "SessionPolicy.h"

#include "../Address/INET_Addr.h"
#include "../IOCP/Overlapped.h"
#include "../../Thread/Lock.h"
#include "../../Stream/Buffer/BufferQueue.h"

#pragma comment(lib, "ws2_32")

// 전방 참조
class CBuffer;
class CBufferFactory;

class CPacketDispatch;
class CDispatchFactory;

class CCompletionHandler;
class CSocketBaseFactory;


struct SessionStatistics
{
    unsigned long   m_dwTotalReceived;      // 세션이 받은 양.
    unsigned long   m_dwTotalSendPending;   // 세션에서 pending한 양.
    unsigned long   m_dwTotalSendCompleted; // Send완료된 양.
    time_t          m_ConnectedTime;        // Connect된 시간.
    time_t          m_DisconnectedTime;     // Disconnected된 시간.

    SessionStatistics::SessionStatistics()
        :   m_dwTotalReceived(0), m_dwTotalSendPending(0), m_dwTotalSendCompleted(0), 
            m_ConnectedTime(0), m_DisconnectedTime(0)
    {

    }

    void Initialize() 
    {
        m_dwTotalReceived = m_dwTotalSendPending = m_dwTotalSendCompleted = 0;
        m_ConnectedTime = m_DisconnectedTime = 0;
    }
};                      
                        

class CSession
{
public:

    CSession(CSessionPolicy& SessionPolicy);
	~CSession();

    enum SendResult
    {
        SEND_SUCCEEDED,
        SEND_FAILED,
        SEND_NEXT_TURN,    
    };

    LONG AddRef();
    LONG Release();

    bool IsConnected();
    void CloseSession();

    bool CalledConnected();
    bool CalledDisconnected();

	void SetAddress(INET_Addr& remoteAddr, INET_Addr& localAddr);
	INET_Addr& GetRemoteAddr() { return m_RemoteAddr; }
	INET_Addr& GetLocalAddr() { return m_LocalAddr; }

    CPacketDispatch* GetDispatch()      { return m_lpDispatch; }
    CSessionPolicy&  GetPolicy()		{ return m_SessionPolicy;    }
	
    const SessionStatistics& GetStatistics() { return m_Statistics; }

    bool SendPending(CBuffer* lpBuffer);
    void SendCompleted(BOOL bResult, unsigned long dwSendedBytes);

	bool Process();
	bool Dispatch(unsigned long dwReceivedBytes);

    bool Recv() { SessionLock::Syncronize sync(m_SessionLock); return InternalRecv(); }
    bool RecvFrom() { SessionLock::Syncronize sync(m_SessionLock); return InternalRecvFrom(); }
    bool Send() { SessionLock::Syncronize sync(m_SessionLock); return InternalSend(); }

    bool Connect(CCompletionHandler& Handler, LPSTR lpConnectAddress, unsigned short usPort);
    bool Shutdown();

    void SuspendRecv();
    void ResumeRecv();

    friend std::ostream& Log(std::ostream& strm, CSession* lpSession) { return (0 != lpSession) ? lpSession->PrintStatistics(strm) : strm; }
    friend std::ostream& Log(std::ostream& strm, CSession& Session)   { return Session.PrintStatistics(strm); }

private:

	bool isValid() const { return (0 != m_lpRecvBuffer && 0 != m_lpDispatch); }
	void Socket(SOCKET hSocket) { m_hSocket = hSocket; }
	SOCKET Socket() { return m_hSocket; }

    bool InternalRecv();
    bool InternalRecvFrom();

    bool InternalSend();
    SendResult InternalSend(CBuffer* lpBuffer);
    SendResult InternalSendTo(CBuffer* lpBuffer);

    void InternalCloseSocket();
    
    // Desc : 로그 출력
    std::ostream& PrintStatistics(std::ostream& strm);
    int InternalPrintStatistics(char* szBuffer, int nBufferLen);

    // -------------------------------------------------------------------------
    // 멤버 변수 선언

    typedef CCSLock     SessionLock;            // Critical Section Lock (24byte)

    SessionLock         m_SessionLock;          // Multithread환경을 위한 Lock
    CACHE_PAD(LockPadding, sizeof(SessionLock));// Cache-align을 위한 Padding

	// Shared class
    CSessionPolicy&     m_SessionPolicy;        // Session Policy

	// Per-session data
	CPacketDispatch*    m_lpDispatch;     // 세션의 PacketDispatch클래스

	SOCKET	        	m_hSocket;              // Socket Descriptor
	
	INET_Addr			m_RemoteAddr;			// 상대방의 주소
    INET_Addr			m_LocalAddr;			// 나의 주소

	CBuffer*			m_lpRecvBuffer;         // Recv Buffer
	CBufferQueue        m_SendQueue;            // SendQueue

    unsigned long       m_dwRecvPending;        // Recv PendingCount
    unsigned long       m_dwSendPending;        // Send PendingCount

    LONG                m_nRefCount;            // 세션 리퍼런스 카운트

	SessionStatistics   m_Statistics;           // 세션 통계

	unsigned char		m_cCurrentStatus;       // 세션 상태
	unsigned char		m_cFlags;               // 세션 flag
    unsigned short      m_usPadding;            // 패딩값

	friend class CListener;
	friend class CIOCPNet;
};

#endif
