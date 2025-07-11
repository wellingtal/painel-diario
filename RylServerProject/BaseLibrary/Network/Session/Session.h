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

// ���� ����
class CBuffer;
class CBufferFactory;

class CPacketDispatch;
class CDispatchFactory;

class CCompletionHandler;
class CSocketBaseFactory;


struct SessionStatistics
{
    unsigned long   m_dwTotalReceived;      // ������ ���� ��.
    unsigned long   m_dwTotalSendPending;   // ���ǿ��� pending�� ��.
    unsigned long   m_dwTotalSendCompleted; // Send�Ϸ�� ��.
    time_t          m_ConnectedTime;        // Connect�� �ð�.
    time_t          m_DisconnectedTime;     // Disconnected�� �ð�.

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
    
    // Desc : �α� ���
    std::ostream& PrintStatistics(std::ostream& strm);
    int InternalPrintStatistics(char* szBuffer, int nBufferLen);

    // -------------------------------------------------------------------------
    // ��� ���� ����

    typedef CCSLock     SessionLock;            // Critical Section Lock (24byte)

    SessionLock         m_SessionLock;          // Multithreadȯ���� ���� Lock
    CACHE_PAD(LockPadding, sizeof(SessionLock));// Cache-align�� ���� Padding

	// Shared class
    CSessionPolicy&     m_SessionPolicy;        // Session Policy

	// Per-session data
	CPacketDispatch*    m_lpDispatch;     // ������ PacketDispatchŬ����

	SOCKET	        	m_hSocket;              // Socket Descriptor
	
	INET_Addr			m_RemoteAddr;			// ������ �ּ�
    INET_Addr			m_LocalAddr;			// ���� �ּ�

	CBuffer*			m_lpRecvBuffer;         // Recv Buffer
	CBufferQueue        m_SendQueue;            // SendQueue

    unsigned long       m_dwRecvPending;        // Recv PendingCount
    unsigned long       m_dwSendPending;        // Send PendingCount

    LONG                m_nRefCount;            // ���� ���۷��� ī��Ʈ

	SessionStatistics   m_Statistics;           // ���� ���

	unsigned char		m_cCurrentStatus;       // ���� ����
	unsigned char		m_cFlags;               // ���� flag
    unsigned short      m_usPadding;            // �е���

	friend class CListener;
	friend class CIOCPNet;
};

#endif
