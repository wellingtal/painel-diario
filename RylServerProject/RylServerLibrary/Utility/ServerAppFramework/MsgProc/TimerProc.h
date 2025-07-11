#ifndef _GM_NETWORK_TIMER_PROC_
#define _GM_NETWORK_TIMER_PROC_

#include <winsock2.h>
#include <windows.h>
#include <list>
#include <map>

class CTimerProc
{
public:
	virtual ~CTimerProc() { }
	virtual VOID operator () (HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) = 0;
};


class CTimerProcMgr
{
public:

	CTimerProcMgr();
	~CTimerProcMgr();

	typedef unsigned int TIMER_ID;
	typedef unsigned int PROC_ID;

	static const TIMER_ID	INVALID_TIMER_ID;
	static const PROC_ID	INVALID_PROC_ID;

	TIMER_ID CreateTimer(HWND hOwnerWnd, unsigned long dwInterval);
	bool RemoveTimer(TIMER_ID nTimerID);

	TIMER_ID GetTimerID(HWND hOwnerWnd, unsigned long dwInterval);

	PROC_ID AddProc(TIMER_ID nTimerID, CTimerProc* lpTimerProc);
	bool RemoveProc(PROC_ID nProcID);

	void ClearAll();

private:

	static VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR nTimerID, DWORD dwTime);

	struct InternalProcessData
	{
		PROC_ID		m_nProcessID;
        CTimerProc*	m_lpTimerProc;

		InternalProcessData(PROC_ID nProcessID, CTimerProc* lpTimerProc)
			:	m_nProcessID(nProcessID), m_lpTimerProc(lpTimerProc)
		{

		}
	};

	typedef std::list<InternalProcessData>	TimerProcessList;
	
	struct InternalTimerData
	{
		HWND				m_hOwnerWnd;
		unsigned long		m_dwInterval;
		TimerProcessList	m_timerProcessList;

		InternalTimerData(HWND hOwnerWnd, unsigned long dwInterval)
			:	m_hOwnerWnd(hOwnerWnd), m_dwInterval(dwInterval)
		{

		}
	};

	// Key : TimerID
	typedef std::map<TIMER_ID, InternalTimerData>	TimerMap;
	
	static const TCHAR* ms_this;
	
	TimerMap	m_TimerMap;
	PROC_ID		m_nProcIDCounter;
	TIMER_ID	m_nTimerIDCounter;
};

#endif