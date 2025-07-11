#include "SessionMgr.h"
#include "Session.h"

#include <boost/pool/object_pool.hpp>

CSessionMgr::CSessionMgr()
:	m_lpSessionPool(new boost::pool<>(sizeof(CSession)))
{
	
}


CSessionMgr::~CSessionMgr()
{
	// 기타 제거작업.
	Destroy(5000);

	delete m_lpSessionPool;
}

void CSessionMgr::Add(CSession* lpSession)
{
	if(0 != lpSession)
	{
        SessionLock::Syncronize sync(m_AddLock);
		m_to_be_added.push_back(lpSession);
	}
}


void CSessionMgr::InternalProcess()
{
	{
		SessionLock::Syncronize sync(m_AddLock);	
		m_current.splice(m_current.end(), m_to_be_added);
	}

	SessionSet::iterator pos = m_current.begin();
	SessionSet::iterator end = m_current.end();
	
	CSession* lpSession = 0;

	for(;pos != end;)
	{
		lpSession = *pos;

		if(!lpSession->Process())
		{
			pos = m_current.erase(pos);
			DeleteSession(lpSession);
		}
		else
		{
			++pos;
		}
	}
}


void CSessionMgr::Destroy(unsigned long dwWaitTime)
{
	SessionLock::Syncronize process_sync(m_ProcessLock);
	
	{
		SessionLock::Syncronize sync(m_AddLock);	
		m_current.splice(m_current.end(), m_to_be_added);
	}

	SessionSet::iterator pos = m_current.begin();
	SessionSet::iterator end = m_current.end();
	
	CSession* lpSession = 0;

	for(;pos != end; ++pos)
	{
		lpSession = *pos;		
		lpSession->CloseSession();
	}

	pos = m_current.begin();
	end = m_current.end();

	unsigned long dwTime = timeGetTime();

	while(!m_current.empty()
		&& timeGetTime() < dwTime + dwWaitTime)
	{
        InternalProcess();
		Sleep(10);		
	}
}


CSession* CSessionMgr::CreateSession(CSessionPolicy& SessionPolicy)
{
	SessionLock::Syncronize sync(m_CreationLock);

	void* ptr = m_lpSessionPool->malloc();

	return ptr ? new (ptr) CSession(SessionPolicy) : 0;
}


void CSessionMgr::DeleteSession(CSession* lpSession)
{
	SessionLock::Syncronize sync(m_CreationLock);

	if(0 != m_lpSessionPool && 0 != lpSession)
	{
		lpSession->~CSession();
		m_lpSessionPool->free(lpSession);
	}
}

