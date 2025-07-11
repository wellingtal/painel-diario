#include "stdafx.h"
#include "SingleDispatchStorage.h"

#include <Thread/Lock.h>
#include <Log/ServerLog.h>
#include <Network/Session/Session.h>
#include <Network/Dispatch/Dispatch.h>

class CPacketDispatch;


void CSingleDispatch::SetDispatch(CPacketDispatch* lpDispatch)
{
	if(0 != lpDispatch)
	{
        LockType::Syncronize sync(m_DispatchLock);

		if(0 != m_lpDispatch)
		{
			DETLOG1(g_Log, "this:0x%p/Already dispatch exist. close now", this);				
			m_lpDispatch->GetSession().CloseSession();				
			m_lpDispatch->GetSession().Release();
		}
		
		DETLOG2(g_Log, "DP:0x%p/Setting new dispatch(DP:0x%p)", m_lpDispatch, lpDispatch);
		m_lpDispatch = lpDispatch;
		m_lpDispatch->GetSession().AddRef();
	}
}

void CSingleDispatch::RemoveDispatch(CPacketDispatch* lpDispatch)
{
	LockType::Syncronize sync(m_DispatchLock); 
	InternalRemoveDispatch(lpDispatch);
}

size_t CSingleDispatch::GetDispatchNum() 
{ 
	LockType::Syncronize sync(m_DispatchLock);
	return 0 != m_lpDispatch ? 1 : 0;
}

bool CSingleDispatch::IsEmpty() 
{ 
    LockType::Syncronize sync(m_DispatchLock);
    return 0 != m_lpDispatch ? false : true;
}


CSingleDispatch::CSingleDispatch() 
:	m_lpDispatch(0)
{ 

}
	
CSingleDispatch::~CSingleDispatch()
{
	LockType::Syncronize sync(m_DispatchLock);

	if (0 != m_lpDispatch)
	{
		ERRLOG1(g_Log, "DP:0x%p/Dispatch is not removed until destroy", m_lpDispatch);
        InternalRemoveDispatch(m_lpDispatch);
	}
}

void CSingleDispatch::InternalRemoveDispatch(CPacketDispatch* lpDispatch)
{
	if (lpDispatch == m_lpDispatch)
	{
		DETLOG1(g_Log, "DP:0x%p/Reset dispatch", m_lpDispatch);
		m_lpDispatch->GetSession().Release();
		m_lpDispatch = 0;
	}
	else
	{
		ERRLOG2(g_Log, "this:0x%p/Reset dispatch failed. Invalid dispatch(0x%p)",
			m_lpDispatch, lpDispatch);
	}		
}

CPacketDispatch* CSingleDispatch::GetDispatch()
{
	LockType::Syncronize sync(m_DispatchLock);

	if(0 != m_lpDispatch)
	{
		m_lpDispatch->GetSession().AddRef();			
	}

	return m_lpDispatch;
}



CSingleDispatch::Storage::Storage(CSingleDispatch& singleDispatch)
:	m_lpDispatch(singleDispatch.GetDispatch()),
	m_SingleDispatch(singleDispatch)
{
	
}


CSingleDispatch::Storage::~Storage() 
{ 
	if(0 != m_lpDispatch) 
	{
		m_lpDispatch->GetSession().Release();
		m_lpDispatch = 0; 
	}
}


CPacketDispatch* CSingleDispatch::Storage::ReloadDispatch()
{
    if(0 != m_lpDispatch)
	{
		m_lpDispatch->GetSession().Release();
	}

	m_lpDispatch = m_SingleDispatch.GetDispatch();
    return m_lpDispatch;
}


