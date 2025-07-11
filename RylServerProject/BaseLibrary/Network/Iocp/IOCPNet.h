#ifndef _CIOCPMGR_H_
#define _CIOCPMGR_H_

#include <map>
#include <list>
#include "../../Thread/Lock.h"

// 전방 참조
class CListener;
class CSession;
class CSessionPolicy;
class CCompletionHandler;
class CThreadMgr;
class CSessionMgr;
class CValidateConnection;

class CIOCPNet
{
public:

    CIOCPNet();
    ~CIOCPNet();

    bool Initialize();
    bool Destroy();
    
    bool AddListener(CSessionPolicy* lpSessionPolicy, 		
		const char* lpListenAddress, unsigned short usPort, 		
		unsigned long dwMaxListenPeding = 10,
		CValidateConnection* lpValidateConnection = 0);

	bool Connect(CSessionPolicy* lpSessionPolicy, 
		const char* lpConnectAddress, unsigned short usPort);

	void Process();

	unsigned long GetAcceptPendingNum();
	unsigned long GetSessionNum();
    
private:
	
    void DestroyListener();
	
    typedef std::list<CListener*>					ListenerList;
	typedef std::map<CSessionPolicy*, unsigned int>	SessionPolicyMap;
    
    typedef CCSLock         SessionLock;
    
    SessionLock             m_IOCPLock;
    CACHE_PAD(SessionLockPad, sizeof(SessionLock));

    CCompletionHandler*     m_lpSocketHandler;
    CThreadMgr*             m_lpThreadMgr;
	CSessionMgr*			m_lpSessionMgr;

	ListenerList			m_ListenerList;
	SessionPolicyMap		m_SessionPolicyMap;
    unsigned long           m_dwFlags;
};

#endif