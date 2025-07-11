#include "stdafx.h"
#include "CommandQueue.h"

#include "Command.h"
#include <Thread/ThreadMgr.h>
#include <Log/ServerLog.h>

CCommandQueueThread::CCommandQueueThread(long nMaxQueueSize)
{
	MsgQueueLock::Syncronize sync(m_Lock);

	m_hHandles[StopperIndex]	= CreateEvent(0, TRUE, FALSE, 0);
	m_hHandles[SemaphoreIndex]	= CreateSemaphore(0, 0, nMaxQueueSize, 0);
}

CCommandQueueThread::~CCommandQueueThread()
{
	CThreadMgr::Stop(this);

	MsgQueueLock::Syncronize sync(m_Lock);

	CommandList::iterator pos = m_CommandList.begin();
	CommandList::iterator end = m_CommandList.end();

	for(;pos != end; ++pos)
	{
		(*pos)->Destroy();
	}

	m_CommandList.clear();

	CloseHandle(m_hHandles[StopperIndex]);
	CloseHandle(m_hHandles[SemaphoreIndex]);
}

bool CCommandQueueThread::IsValid()
{
	MsgQueueLock::Syncronize sync(m_Lock);

	return INVALID_HANDLE_VALUE != m_hHandles[StopperIndex] && 
		INVALID_HANDLE_VALUE != m_hHandles[SemaphoreIndex];
}			 


bool CCommandQueueThread::Add(CCommand* lpCommand)
{
	MsgQueueLock::Syncronize sync(m_Lock);

	m_CommandList.push_back(lpCommand);
	BOOL bResult = ReleaseSemaphore(m_hHandles[SemaphoreIndex], 1, 0);
	if(!bResult)
	{
		m_CommandList.pop_back();
	}

	return 0 != bResult;
}

unsigned int CCommandQueueThread::Run()
{
	bool bExit = false;

	while(!bExit)
	{
		CCommand* lpCommand = 0;

        switch(WaitForMultipleObjects(MaxIndex, m_hHandles, FALSE, INFINITE))
		{
		case StopperIndex:
			bExit = true;
			break;

		case SemaphoreIndex:
			{
				MsgQueueLock::Syncronize sync(m_Lock);

				if(!m_CommandList.empty())
				{
                    lpCommand = m_CommandList.front();
					m_CommandList.pop_front();
				}
			}

			if(0 != lpCommand)
			{
				lpCommand->DoProcess();
				lpCommand->Destroy();
				lpCommand = 0;
			}
			break;

		case WAIT_FAILED:

			ERRLOG1(g_Log, "Err:%d/Error from WaitForMultipleObject", GetLastError());
			bExit = true;
			break;
		}
	}

	return 0;
}

BOOL CCommandQueueThread::End()
{
	return SetEvent(m_hHandles[StopperIndex]);
}
