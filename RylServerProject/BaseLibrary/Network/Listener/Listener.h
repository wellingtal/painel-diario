#ifndef _CLISTENER_H_
#define _CLISTENER_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <list>

#include <boost/pool/object_pool.hpp>

#include "../../Thread/Lock.h"
#include "../Address/INET_Addr.h"

#pragma comment(lib, "winmm")
#pragma comment(lib, "mswsock")


// 전방 참조
class CBuffer;
class CSession;
class CSessionMgr;
class CSessionPolicy;
class CSocketFactory;
class CCompletionHandler;

// IP나, 기타 상태 등을 전제로 접속을 차단하는 방침을 만든다.
class CValidateConnection
{
public:

	LONG AddRef();
	LONG Release();
	
	virtual bool operator () (INET_Addr& localAddr, INET_Addr& remoteAddr) = 0;

protected:

	CValidateConnection() : m_nRefCount(1) { }
	virtual ~CValidateConnection() { }

private:

	LONG m_nRefCount;
};

class CListener
{
public:

    CListener(CCompletionHandler& SocketHandler,
		CSessionPolicy& SessionPolicy, 		
		CSessionMgr& SessionMgr,
		CValidateConnection* lpValidateConnection);

    virtual ~CListener();

    bool Initialize(const char* lpListenAddress, unsigned short usPort, 
		unsigned long dwMaxPending = 0)
    {
        return Initialize(INET_Addr(lpListenAddress, usPort), dwMaxPending);
    }

    bool Initialize(INET_Addr& addrListen, unsigned long dwMaxPending = 0);

	bool PendingAccept();

	void ProcessAccept(BOOL bResult, SOCKET hSocket, 
		CBuffer* lpBuffer, unsigned long dwProcessedBytes);
	
	unsigned long GetPendingAcceptNum();
	CSessionPolicy& GetPolicy() { return m_SessionPolicy; }

    friend std::ostream& Log(std::ostream& strm, CListener* lpListener) { return (0 != lpListener) ? lpListener->PrintStatistics(strm) : strm; }
    friend std::ostream& Log(std::ostream& strm, CListener& Listener)   { return Listener.PrintStatistics(strm); }

private:

	void InternalProcess();
	void InternalPending();
	
	void InternalDestroy();
	void InternalCloseListen();	
	
	void WaitForPendingComplete(unsigned long dwTime);

    std::ostream& PrintStatistics(std::ostream& strm);
    int InternalPrintStatistics(char* szBuffer, int nBufferLen);
	    
    //--------------------------------------------------- 
	typedef CCSLock		SessionLock;

	SessionLock			m_ListenerLock;	// 리스너 자체 변수를 건드릴 때 사용하는 락.
	CACHE_PAD(ListenerLockPad, sizeof(SessionLock));

    SOCKET              m_hListen;
    INET_Addr           m_ListenAddr;

    CCompletionHandler& m_SocketHandler;	
    CSessionPolicy&     m_SessionPolicy;
	CSessionMgr&		m_SessionMgr;
        
    unsigned long       m_dwMaxPending;
    unsigned long       m_dwCurrentPending;
    unsigned long		m_dwTotalPendingCount;
	unsigned long		m_dwTotalAcceptCompleteCount;

	CValidateConnection*	m_lpValidateConnection;
};


#endif