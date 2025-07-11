#ifndef _NETWORK_SESSION_MGR_H_
#define _NETWORK_SESSION_MGR_H_

#include "../../Thread/Lock.h"

#include <list>
#include <boost/pool/poolfwd.hpp>


// 전방 참조
class CSession;
class CSessionPolicy;

class CSessionMgr
{
public:

	CSessionMgr();
	~CSessionMgr();

	void Add(CSession* lpSession);
	void Process() { SessionLock::Syncronize process_sync(m_ProcessLock); InternalProcess(); }
	void Destroy(unsigned long dwWaitTime = INFINITE);

	size_t GetSessionNum() { SessionLock::Syncronize process_sync(m_ProcessLock); return m_current.size(); }

	CSession* CreateSession(CSessionPolicy& SessionPolicy);
	void DeleteSession(CSession* lpSession);

private:

	void InternalProcess();	

	typedef CCSLock					SessionLock;
	typedef std::list<CSession*>	SessionSet;

	SessionLock	m_AddLock;
	SessionSet	m_to_be_added;

	CACHE_PAD(AddLockPad, sizeof(SessionSet));

	SessionLock	m_ProcessLock;
	SessionSet	m_current;
		
	CACHE_PAD(ProcessLockPad, sizeof(SessionSet));

	SessionLock		m_CreationLock;
	boost::pool<>*	m_lpSessionPool;
};

#endif