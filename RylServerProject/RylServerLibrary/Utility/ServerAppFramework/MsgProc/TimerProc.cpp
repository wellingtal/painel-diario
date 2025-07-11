#include "stdafx.h"
#include "TimerProc.h"

const TCHAR* CTimerProcMgr::ms_this = TEXT("CTimerProcMgr");

const CTimerProcMgr::TIMER_ID	CTimerProcMgr::INVALID_TIMER_ID = 0xFFFFFFFF;
const CTimerProcMgr::PROC_ID	CTimerProcMgr::INVALID_PROC_ID  = 0xFFFFFFFF;


CTimerProcMgr::CTimerProcMgr()
:	m_nProcIDCounter(1),
	m_nTimerIDCounter(1)
{


}

CTimerProcMgr::~CTimerProcMgr()
{
	ClearAll();
}


CTimerProcMgr::TIMER_ID CTimerProcMgr::CreateTimer(HWND hOwnerWnd, unsigned long dwInterval)
{
	TIMER_ID nTimerID = GetTimerID(hOwnerWnd, dwInterval);

	if(INVALID_TIMER_ID == nTimerID)
	{
		std::pair<TimerMap::iterator, bool> result_pair; 
		result_pair.second = false;

		do
		{
			nTimerID = m_nTimerIDCounter++;

			result_pair = m_TimerMap.insert(
				TimerMap::value_type(nTimerID, InternalTimerData(hOwnerWnd, dwInterval)));

			if(result_pair.second)
			{
				if(0 == SetTimer(hOwnerWnd, nTimerID, dwInterval, CTimerProcMgr::TimerProc))
				{					
					nTimerID = INVALID_TIMER_ID;
					m_TimerMap.erase(result_pair.first);
					break;
				}
				else
				{
					SetProp(hOwnerWnd, ms_this, this);
				}
			}

		} while(!result_pair.second);
	}

	return nTimerID;
}

bool CTimerProcMgr::RemoveTimer(TIMER_ID nTimerID)
{
	TimerMap::iterator find = m_TimerMap.find(nTimerID);
	TimerMap::iterator end = m_TimerMap.end();

	if(find != end)
	{
		InternalTimerData& timerData = find->second;
		KillTimer(timerData.m_hOwnerWnd, find->first);

		TimerProcessList::iterator process_pos = timerData.m_timerProcessList.begin();
		TimerProcessList::iterator process_end = timerData.m_timerProcessList.end();

		for(;process_pos != process_end; ++process_pos)
		{
			delete process_pos->m_lpTimerProc;
		}

		timerData.m_timerProcessList.clear();

		m_TimerMap.erase(find);
		return true;
	}

	return false;
}


CTimerProcMgr::TIMER_ID CTimerProcMgr::GetTimerID(HWND hOwnerWnd, unsigned long dwInterval)
{
	TimerMap::iterator pos = m_TimerMap.begin();
	TimerMap::iterator end = m_TimerMap.end();

	for(; pos != end; ++pos)
	{
		InternalTimerData& timerData = pos->second;

		if(timerData.m_hOwnerWnd == hOwnerWnd &&
			timerData.m_dwInterval == dwInterval)
		{
			return pos->first;
		}
	}

	return INVALID_TIMER_ID;
}


CTimerProcMgr::PROC_ID CTimerProcMgr::AddProc(TIMER_ID nTimerID, CTimerProc* lpTimerProc)
{
	TimerMap::iterator find = m_TimerMap.find(nTimerID);
	TimerMap::iterator end = m_TimerMap.end();

	if(find != end)
	{
		InternalTimerData& timerData = find->second;

		PROC_ID nProcID = m_nProcIDCounter++;
		timerData.m_timerProcessList.push_back(InternalProcessData(nProcID, lpTimerProc));

		return nProcID;
	}

	return INVALID_PROC_ID;
}


bool CTimerProcMgr::RemoveProc(PROC_ID nProcID)
{
	TimerMap::iterator pos = m_TimerMap.begin();
	TimerMap::iterator end = m_TimerMap.end();

	TimerProcessList::iterator process_pos;
	TimerProcessList::iterator process_end;

	for(; pos != end; ++pos)
	{
		InternalTimerData& timerData = pos->second;

		process_pos = timerData.m_timerProcessList.begin();
		process_end = timerData.m_timerProcessList.end();

		for(; process_pos != process_end; ++process_pos)
		{
			InternalProcessData& processData = *process_pos;

			if(processData.m_nProcessID == nProcID)
			{
				delete processData.m_lpTimerProc;
				timerData.m_timerProcessList.erase(process_pos);
				return true;
			}
		}
	}

	return false;
}


void CTimerProcMgr::ClearAll()
{
	TimerMap::iterator pos = m_TimerMap.begin();
	TimerMap::iterator end = m_TimerMap.end();

	for(;pos != end; ++pos)
	{
		InternalTimerData& timerData = pos->second;
		KillTimer(timerData.m_hOwnerWnd, pos->first);
        
		TimerProcessList::iterator process_pos = timerData.m_timerProcessList.begin();
		TimerProcessList::iterator process_end = timerData.m_timerProcessList.end();

		for(;process_pos != process_end; ++process_pos)
		{
			delete process_pos->m_lpTimerProc;
		}

		timerData.m_timerProcessList.clear();
	}

	m_TimerMap.clear();
}


VOID CALLBACK CTimerProcMgr::TimerProc(HWND hWnd, UINT uMsg, 
									   UINT_PTR nTimerID, DWORD dwTime)
{
	CTimerProcMgr* lpTimerProcMgr = 
		reinterpret_cast<CTimerProcMgr*>(GetProp(hWnd, ms_this));

	if(0 != lpTimerProcMgr && uMsg == WM_TIMER)
	{
		TimerMap::iterator find = lpTimerProcMgr->m_TimerMap.find(nTimerID);
		TimerMap::iterator end = lpTimerProcMgr->m_TimerMap.end();

		if(find != end)
		{
			InternalTimerData& timerData = find->second;

			TimerProcessList::iterator process_pos = timerData.m_timerProcessList.begin();
			TimerProcessList::iterator process_end = timerData.m_timerProcessList.end();

			for(; process_pos != process_end; ++process_pos)
			{
				InternalProcessData& processData = *process_pos;

				if(0 != processData.m_lpTimerProc)
				{
					(*processData.m_lpTimerProc)(hWnd, uMsg, nTimerID, dwTime);
				}
			}
		}
	}
}
