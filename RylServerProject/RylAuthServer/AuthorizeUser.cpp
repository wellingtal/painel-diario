#include "stdafx.h"
#include "AuthorizeUser.h"
#include "AuthorizeThread.h"
#include "AuthClientDispatch.h"
#include <BaseLibrary/Network/Session/Session.h>

#include <Log/ServerLog.h>

#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

CSyncronizedObjectQueue::CSyncronizedObjectQueue()
:   m_nMaxSize(LONG_MAX), m_bStop(false)
{
	m_hHandles[StopperIndex]	= CreateEvent(0, TRUE, FALSE, 0);
	m_hHandles[SemaphoreIndex]	= CreateSemaphore(0, 0, LONG_MAX, 0);
}

CSyncronizedObjectQueue::~CSyncronizedObjectQueue()
{
    clear();

	CCSLock::Syncronize sync(m_DataLock);
	
	CloseHandle(m_hHandles[StopperIndex]);
	CloseHandle(m_hHandles[SemaphoreIndex]);
}

bool CSyncronizedObjectQueue::enqueue(CAuthInfo* lpCommand, bool bPendHead)
{
	CCSLock::Syncronize sync(m_DataLock);
	
    if(0 == lpCommand || m_nMaxSize < m_CommandQueue.size())
    {        
        return false;
    }

	if(bPendHead) { m_CommandQueue.push_front(lpCommand); }
	else { m_CommandQueue.push_back(lpCommand); }

	BOOL bResult = ReleaseSemaphore(m_hHandles[SemaphoreIndex], 1, 0);
	if(!bResult)
	{
		if(bPendHead) 
        {
            m_CommandQueue.pop_front();
        }
		else 
        {
            m_CommandQueue.pop_back();
        }
        
        return false;
	}

	return 0 != bResult;
}

CAuthInfo* CSyncronizedObjectQueue::dequeue(unsigned long dwMilliSecond)
{
	CAuthInfo* lpCommand = 0;

    switch(WaitForMultipleObjects(MaxIndex, m_hHandles, FALSE, dwMilliSecond))
	{
	case StopperIndex:
		break;

	case SemaphoreIndex:
		{
			CCSLock::Syncronize sync(m_DataLock);

			if(!m_CommandQueue.empty())
			{
                lpCommand = m_CommandQueue.front();
				m_CommandQueue.pop_front();
			}
		}
		break;

	case WAIT_FAILED:

		ERRLOG1(g_Log, "Err:%d/Error from WaitForMultipleObject", GetLastError());
		break;
	}

	return lpCommand;
}

void CSyncronizedObjectQueue::remove(const char* szUserAccount)
{
    CCSLock::Syncronize sync(m_DataLock);
   
	CommandQueue::iterator pos = m_CommandQueue.begin();
	CommandQueue::iterator end = m_CommandQueue.end();

	for(; pos != end;)
	{
        CAuthInfo* lpAuthInfo = *pos;

        if(0 == strncmp(lpAuthInfo->GetUserAccount(), 
            szUserAccount, PktAU::ID_LEN))
        {
            pos = m_CommandQueue.erase(pos);
            delete lpAuthInfo;
        }
        else
        {
            ++pos;
        }
	}
}

void CSyncronizedObjectQueue::stop()
{
    CCSLock::Syncronize sync(m_DataLock);
    m_bStop = true;
    SetEvent(m_hHandles[StopperIndex]);
}

bool CSyncronizedObjectQueue::is_stopped()
{
    CCSLock::Syncronize sync(m_DataLock);
    return m_bStop;
}

void CSyncronizedObjectQueue::clear()
{
   	CCSLock::Syncronize sync(m_DataLock);
	
	CommandQueue::iterator pos = m_CommandQueue.begin();
	CommandQueue::iterator end = m_CommandQueue.end();

	for(; pos != end; ++pos)
	{
		delete *pos;
	}

	m_CommandQueue.clear();
}


CAuthInfo::CAuthInfo(CAuthClientDispatch* lpDispatch, unsigned long dwSessionID, 
                     const char* szUserAccount, const char* szUserPassword, 
                     size_t nMaxUserAccountSize, size_t nMaxUserPasswordSize, 
                     unsigned short usFlag, unsigned short usCnFlag)
                     :  m_lpDispatch(lpDispatch), m_dwSessionID(dwSessionID), 
                        m_usFlag(usFlag), m_usCnFlag(usCnFlag)
{
    m_dwAuthStartTime = timeGetTime();

    nMaxUserAccountSize  = min(size_t(PktAU::ID_LEN), nMaxUserAccountSize);
    nMaxUserPasswordSize = min(size_t(PktAU::PASS_LEN), nMaxUserPasswordSize);

    memcpy(m_szUserAccount, szUserAccount, nMaxUserAccountSize);
    memcpy(m_szUserPassword, szUserPassword, nMaxUserPasswordSize);
    
	m_bLogin = TRUE;

    if(0 != m_lpDispatch)
    {
		static_cast<CPacketDispatch*>(m_lpDispatch)->GetSession().AddRef();
    }
}

CAuthInfo::CAuthInfo(CAuthClientDispatch* lpDispatch, const char* szUserAccount, size_t nMaxUserAccountSize)
{
    m_dwAuthStartTime = timeGetTime();

    nMaxUserAccountSize  = min(size_t(PktAU::ID_LEN), nMaxUserAccountSize);
    memcpy(m_szUserAccount, szUserAccount, nMaxUserAccountSize);
    
	m_bLogin = FALSE;
	m_lpDispatch = NULL;
}


CAuthInfo::~CAuthInfo()
{
    if(0 != m_lpDispatch)
    {
        static_cast<CPacketDispatch*>(m_lpDispatch)->GetSession().Release();
    }
}


CAuthorizeUser& CAuthorizeUser::GetInstance()
{
	static CAuthorizeUser authorizeUser;
	return authorizeUser;
}


CAuthorizeUser::CAuthorizeUser()
{

}

CAuthorizeUser::~CAuthorizeUser()
{
    Destroy();
}

bool CAuthorizeUser::Initialize(unsigned long nThreadNum, unsigned long dwMaxAuthorizeListSize)
{
    m_AuthInfoQueue.set_max_size(dwMaxAuthorizeListSize);

    for(; 0 != nThreadNum; --nThreadNum)
    {
        if(!m_AuthorizeThreadMgr.RegisterAndRun(CAuthorizeThread::Create()))
        {
            return false;
        }
    }

    return true;
}

void CAuthorizeUser::ConnectAll()
{
	int iNum = m_AuthorizeThreadMgr.GetNum();
	int iMaxNum = m_AuthorizeThreadMgr.GetMaxNum();

	for(int i = 0; i < iMaxNum; ++i)
	{
		CAuthorizeThread* pAuth = static_cast<CAuthorizeThread*>(m_AuthorizeThreadMgr.GetThread(i));

		if(pAuth == NULL)
			continue;

		pAuth->ConnectAll();
		iNum--;

		if(iNum < 0)
			break;
	}

}

void CAuthorizeUser::Destroy()
{
    m_AuthInfoQueue.stop();
    m_AuthorizeThreadMgr.JoinThread();

    m_AuthInfoQueue.clear();
}

CAuthInfo* CAuthorizeUser::GetAuthorizeInfo(unsigned long dwMilliSecond)
{
    return m_AuthInfoQueue.dequeue(dwMilliSecond);
}


bool CAuthorizeUser::AddAuthorizeUser(CAuthInfo* lpAuthInfo)
{
    return m_AuthInfoQueue.enqueue(lpAuthInfo);    
}

void CAuthorizeUser::RemovePending(const char* szUserAccount)
{
    return m_AuthInfoQueue.remove(szUserAccount);
}
