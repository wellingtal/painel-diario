#include "stdafx.h"
#include "ServerRequest.h"

#include <Network/Session/Session.h>
#include <Network/Dispatch/Dispatch.h>

#include <Log/ServerLog.h>


#define REQUEST_LOG(x) x


enum RequestConst
{
    CANNOT_ADD_REQUEST = (1 << 0)
};


CServerRequest& CServerRequest::GetInstance()
{
	static CServerRequest serverRequest;
	return serverRequest;
}


CServerRequest::Result::Result(unsigned long dwRequestKey, bool bRemove)
:	m_bRemove(bRemove)
{
	m_lpSrcDispatch = bRemove 
		// Request를 얻어온 후 제거한다.
		? CServerRequest::GetInstance().PopRequest(dwRequestKey)

		// Request를 얻어온 후 제거하지 않는다.
		: CServerRequest::GetInstance().GetRequest(dwRequestKey);
}


CServerRequest::Result::~Result()
{
    if(0 != m_lpSrcDispatch && m_bRemove) 
    {
        m_lpSrcDispatch->GetSession().Release();
    }
}


CSession* CServerRequest::Result::GetSession()
{ 
	return (0 != m_lpSrcDispatch) ? &m_lpSrcDispatch->GetSession() : 0;
}


void CServerRequest::RequestInfo::ProcessRemove()
{
    if(m_lpSrcDispatch)
	{
		if(m_lpTimeoutRequest)
		{
			m_lpTimeoutRequest(m_lpSrcDispatch);
		}

		m_lpSrcDispatch->GetSession().Release();
	}

	if(m_lpDstDispatch)
	{
		m_lpDstDispatch->GetSession().Release();
	}
}


// return value : RequestKey
unsigned long CServerRequest::AddRequest(CPacketDispatch* lpSrcDispatch, 
										 CPacketDispatch* lpDstDispatch, 
										 unsigned long dwDurationSec,
										 TimeoutRequest lpTimeoutRequest)
{
    if( 0 != lpSrcDispatch && 
		0 != lpDstDispatch &&
		0 != dwDurationSec)
    {
		LockType::Syncronize sync(m_RequestLock);

		if(!(m_dwRequestFlags & CANNOT_ADD_REQUEST))
		{
			CSession& SrcSession = lpSrcDispatch->GetSession();
			CSession& DstSession = lpDstDispatch->GetSession();
			
			if(0 == m_dwRequestCounter) 
			{ 
				++m_dwRequestCounter;
			}

			unsigned long dwRequestCounter = m_dwRequestCounter++;
	        
			if(m_RequestMap.insert(std::make_pair(dwRequestCounter, 
				RequestInfo(lpSrcDispatch, lpDstDispatch, dwDurationSec, lpTimeoutRequest))).second)
			{
				SrcSession.AddRef();
				DstSession.AddRef();

                REQUEST_LOG(DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Request:%d/DstDP:0x%p/Request Added",
                    &lpSrcDispatch->GetSession(), lpSrcDispatch, 
                    lpSrcDispatch->GetSession().GetRemoteAddr().get_addr_string(),
                    dwRequestCounter, lpDstDispatch));

				return dwRequestCounter;
			}
		}
    }

    return 0;
}

void CServerRequest::RemoveRequest(unsigned long dwRequestKey)
{
    m_RequestLock.Lock();

    RequestMap::iterator pos = m_RequestMap.find(dwRequestKey);
    RequestMap::iterator end = m_RequestMap.end();

	RequestInfo info;

	if(pos != end)
	{
		info = pos->second;

        REQUEST_LOG(DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Request:%d/DstDP:0x%p/Request Removed(By RequestKey)",
            &info.m_lpSrcDispatch->GetSession(), info.m_lpSrcDispatch, 
            info.m_lpSrcDispatch->GetSession().GetRemoteAddr().get_addr_string(),
            pos->first, info.m_lpDstDispatch));

		pos = m_RequestMap.erase(pos);
    }

	m_RequestLock.Unlock();

	info.ProcessRemove();
}

void CServerRequest::RemoveRequest(CPacketDispatch* lpDispatch)
{
    LockType::Syncronize sync(m_RequestLock);

	RequestMap::iterator pos = m_RequestMap.begin();
	RequestMap::iterator end = m_RequestMap.end();

	for(; pos != end;)
	{
		RequestInfo& info = pos->second;

		if(lpDispatch == info.m_lpSrcDispatch ||
			lpDispatch == info.m_lpDstDispatch)
		{
            REQUEST_LOG(DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Request:%d/DstDP:0x%p/Request Removed(By TargetPtr)",
                &info.m_lpSrcDispatch->GetSession(), info.m_lpSrcDispatch, 
                info.m_lpSrcDispatch->GetSession().GetRemoteAddr().get_addr_string(),
                pos->first, info.m_lpDstDispatch));

			info.ProcessRemove();
            pos = m_RequestMap.erase(pos);
		}
		else
		{
			++pos;
		}
	}
}


void CServerRequest::RemoveTimeoutRequest()
{
	LockType::Syncronize sync(m_RequestLock);

	RequestMap::iterator pos = m_RequestMap.begin();
	RequestMap::iterator end = m_RequestMap.end();

	for(; pos != end;)
	{
		RequestInfo& info = pos->second;

		if(0 == --(info.m_dwDurationSec))
		{
            REQUEST_LOG(DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Request:%d/DstDP:0x%p/Request Removed(By Timeout)",
                &info.m_lpSrcDispatch->GetSession(), info.m_lpSrcDispatch, 
                info.m_lpSrcDispatch->GetSession().GetRemoteAddr().get_addr_string(),
                pos->first, info.m_lpDstDispatch));

			info.ProcessRemove();
			pos = m_RequestMap.erase(pos);
		}
		else
		{
			++pos;
		}
	}
}


void CServerRequest::RemoveAllRequest()
{
	LockType::Syncronize sync(m_RequestLock);

	RequestMap::iterator pos = m_RequestMap.begin();
	RequestMap::iterator end = m_RequestMap.end();
	
	for(; pos != end; ++pos)
	{
		pos->second.ProcessRemove();		
	}

	m_RequestMap.clear();
}


void CServerRequest::RequestOn()
{
	LockType::Syncronize sync(m_RequestLock);
	m_dwRequestFlags &= ~CANNOT_ADD_REQUEST;
}

void CServerRequest::RequestOff()
{
	LockType::Syncronize sync(m_RequestLock);
	m_dwRequestFlags |= CANNOT_ADD_REQUEST;
}


CServerRequest::CServerRequest()
{


}

CServerRequest::~CServerRequest()
{
	RequestOff();
	RemoveAllRequest();
}


CPacketDispatch* CServerRequest::GetRequest(unsigned long dwRequestKey)
{
    LockType::Syncronize sync(m_RequestLock);

    RequestMap::iterator pos = m_RequestMap.find(dwRequestKey);
	return (pos != m_RequestMap.end()) ? pos->second.m_lpSrcDispatch : 0;
}


CPacketDispatch* CServerRequest::PopRequest(unsigned long dwRequestKey)
{
    LockType::Syncronize sync(m_RequestLock);

    RequestMap::iterator pos = m_RequestMap.find(dwRequestKey);

    if(pos != m_RequestMap.end())
	{
		RequestInfo info = pos->second;

        REQUEST_LOG(DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Request:%d/DstDP:0x%p/Request Removed(By Pop Request)",
            &info.m_lpSrcDispatch->GetSession(), info.m_lpSrcDispatch, 
            info.m_lpSrcDispatch->GetSession().GetRemoteAddr().get_addr_string(),
            pos->first, info.m_lpDstDispatch));
       
        m_RequestMap.erase(pos);
		return info.m_lpSrcDispatch;        
    }
	
	return 0;
}

