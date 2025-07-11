#include "stdafx.h"
#include "MultiDispatchStorage.h"

#include <Log/ServerLog.h>
#include <Network/Session/Session.h>
#include <Network/Dispatch/Dispatch.h>

bool CMultiDispatch::SetDispatch(unsigned long dispatchKey, CPacketDispatch* lpDispatch)
{
	if(0 == lpDispatch)
	{
		DETLOG1(g_Log, "this:0x%p/Setting null dispatch", this);    
	}
	else
	{
		LockType::Syncronize sync(m_DispatchLock);

		DispatchMap::iterator find = m_DispatchMap.find(dispatchKey);
		DispatchMap::iterator end  = m_DispatchMap.end();

		if(find == end)
		{
			lpDispatch->GetSession().AddRef();
			m_DispatchMap.insert(find, DispatchMap::value_type(dispatchKey, lpDispatch));
			DETLOG2(g_Log, "Setting new dispatch(Key:%u/DP:0x%p)", dispatchKey, lpDispatch);
			return true;
		}
		else
		{
			CPacketDispatch* lpOldDispatch = find->second;			

			if(0 != lpOldDispatch)
			{
				DETLOG1(g_Log, "DP:0x%p/Already dispatch exist. cannot set dispatch", lpOldDispatch);
			}
		}
	}

	return false;		
}

void CMultiDispatch::ForceSetDispatch(unsigned long dispatchKey, CPacketDispatch* lpDispatch)
{
	if(0 == lpDispatch)
	{
		DETLOG1(g_Log, "this:0x%p/Setting null dispatch", this);    
	}
	else
	{
		LockType::Syncronize sync(m_DispatchLock);

		DispatchMap::iterator find = m_DispatchMap.find(dispatchKey);
		DispatchMap::iterator end  = m_DispatchMap.end();

		CPacketDispatch* lpOldDispatch = 0;
		
		if(find == end)
		{			
			m_DispatchMap.insert(find, DispatchMap::value_type(dispatchKey, lpDispatch));
		}
		else
		{
			lpOldDispatch = find->second;
			find->second = lpDispatch;

			if(0 != lpOldDispatch)
			{
				DETLOG1(g_Log, "DP:0x%p/Already dispatch exist. close now", lpOldDispatch);
				lpOldDispatch->GetSession().CloseSession();					
				lpOldDispatch->GetSession().Release();
			}
		}
		
		DETLOG3(g_Log, "DP:0x%p/Setting new dispatch(Key:%u/DP:0x%p)", 
			lpOldDispatch, dispatchKey, lpDispatch);		

		lpDispatch->GetSession().AddRef();
	}
}

void CMultiDispatch::RemoveDispatch(unsigned long dispatchKey)
{
	LockType::Syncronize sync(m_DispatchLock); 
	InternalRemoveDispatch(dispatchKey);
}


size_t CMultiDispatch::GetDispatchNum() 
{ 
	LockType::Syncronize sync(m_DispatchLock);
	return m_DispatchMap.size();
}

bool CMultiDispatch::IsEmpty() 
{ 
    LockType::Syncronize sync(m_DispatchLock); 
    return m_DispatchMap.empty(); 
}

CMultiDispatch::CMultiDispatch()
{

}

CMultiDispatch::~CMultiDispatch()
{
	LockType::Syncronize sync(m_DispatchLock);

	if(!m_DispatchMap.empty())
	{
		DispatchMap::iterator pos = m_DispatchMap.begin();
		DispatchMap::iterator end = m_DispatchMap.end();

		for(;pos != end; ++pos)
		{
			ERRLOG1(g_Log, "this:0x%p/Dispatch is not removed until destroy", pos->second);				
		}

		m_DispatchMap.clear();
	}
}


void CMultiDispatch::InternalRemoveDispatch(unsigned long dispatchKey)
{
	DispatchMap::iterator find = m_DispatchMap.find(dispatchKey);
	DispatchMap::iterator end  = m_DispatchMap.end();

	if(find == end)
	{
		ERRLOG1(g_Log, "Reset dispatch failed. Invalid dispatch key(%u)", dispatchKey);
	}
	else
	{
		CPacketDispatch* lpDispatch = find->second;

		DETLOG2(g_Log, "Reset dispatch(Key:%u/DP:0x%p)", dispatchKey, lpDispatch);
		lpDispatch->GetSession().Release();
		m_DispatchMap.erase(find);
	}
}

CPacketDispatch* CMultiDispatch::GetDispatch(unsigned long dispatchKey)
{
	LockType::Syncronize sync(m_DispatchLock);

	DispatchMap::iterator find	= m_DispatchMap.find(dispatchKey);
	DispatchMap::iterator end	= m_DispatchMap.end();

	if(find != end)
	{
		CPacketDispatch* lpDispatch = find->second;
		lpDispatch->GetSession().AddRef();
		return lpDispatch;
	}

	return 0;
}


CMultiDispatch::Storage::Storage(CMultiDispatch& multiDispatch, unsigned long dispatchKey) 
:	m_lpDispatch(multiDispatch.GetDispatch(dispatchKey)),
	m_MultiDispatch(multiDispatch)
{

}


CMultiDispatch::Storage::~Storage() 
{
	if(0 != m_lpDispatch) 
	{ 
		m_lpDispatch->GetSession().Release(); 
		m_lpDispatch = 0; 
	}
}


CPacketDispatch* CMultiDispatch::Storage::ReloadDispatch(unsigned long dispatchKey)
{
    if(0 != m_lpDispatch)
	{
		m_lpDispatch->GetSession().Release();
	}

	m_lpDispatch = m_MultiDispatch.GetDispatch(dispatchKey);
    return m_lpDispatch;
}
