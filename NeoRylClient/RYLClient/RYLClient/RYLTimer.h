//=================================================================
// Name : RYL Timer.h
// Data : 2003. 11. 10
// Desc : RYL Timer 
//			- Client에서 쓰는 시스템 타이머
//=================================================================

#ifndef __RYL_TIMER_H__
#define __RYL_TIMER_H__

#include <windows.h>
#include <time.h>

class CRYLTimer
{
public :
	DWORD  m_nShowInterfaceTimerID ;
	DWORD  m_nSceneManagerTimerID ;
	DWORD  m_nWeatherTimerID ;
	DWORD  m_dwServerTime ;				// 서버에서 주는 서버 시간(틱) 이걸로 밤/낮을 구분한다.
	DWORD  m_dwStartTime ; 				// 클라이언트가 켜진 시간(틱)
	DWORD  m_dwPartyInfoTimerID ;
	DWORD  m_dwSpeedHackCheckTimerID ;	
	DWORD  m_dwUpdateFlushTimerID ;
	DWORD  m_dwCheckStatusTimerID ;
	DWORD  m_dwPingTimerID ;
	DWORD  m_dwCursorTimer ;
	DWORD  m_dwMoveUpdateTimerID ;
	DWORD  m_dwSkillCheckSumTimerID ;
	DWORD  m_dwMovieBarTimerID ;
	DWORD  m_dwChinaBillingTimeID ;		// 중국 결제 시간
    	
	// 공성관련 타이머
	DWORD  m_dwTireTimerID ;			// 타이어 타이머
	DWORD  m_dwBombTimerID ;			// 포탄 타이머
	
	time_t		m_ttCurrentTime ;
	tm			m_tmEndTime ;

public :
	CRYLTimer() ;
	virtual ~CRYLTimer() ;

	HRESULT		InitTimer() ;

	DWORD  GetShowInterfaceTimerID()		{ return m_nShowInterfaceTimerID ; }
	DWORD  GetSceneManagerTimerID()			{ return m_nSceneManagerTimerID ; } 
	DWORD  GetWeatherTimerID()				{ return m_nWeatherTimerID ; }
	DWORD  GetServerTime()					{ return m_dwServerTime ; }
	DWORD  GetStartTime()					{ return m_dwStartTime ; }
	DWORD  GetPartyInfoTimerID()			{ return m_dwPartyInfoTimerID ; }
	DWORD  GetSpeedHackCheckTimerID()		{ return m_dwSpeedHackCheckTimerID ; }	
	DWORD  GetUpdateFlushTimerID()			{ return m_dwUpdateFlushTimerID ; }
	DWORD  GetCheckStatusTimerID()			{ return m_dwCheckStatusTimerID ; }
	DWORD  GetPingTimerID()					{ return m_dwPingTimerID ; }
	DWORD  GetCursorTimer()					{ return m_dwCursorTimer ; }
	DWORD  GetMoveUpdateTimer()				{ return m_dwMoveUpdateTimerID ; }
	DWORD  GetSkillCheckSumTimerID()		{ return m_dwSkillCheckSumTimerID ; }
	DWORD  GetMovieBarTimer()				{ return m_dwMovieBarTimerID ; }
	DWORD  GetChinaBillingTimeID()			{ return m_dwChinaBillingTimeID ; }
	
	// 공성관련 타이머
	DWORD  GetTireTimerID()					{ return m_dwTireTimerID ; }
	DWORD  GetBombTimerID()					{ return m_dwBombTimerID ; } 
	    
	VOID	SetStartTime( DWORD dwTime )		{ m_dwStartTime = dwTime ; }
	VOID	SetServerTime( DWORD dwTime )		{ m_dwServerTime = dwTime ; }

	// Signton
	/*
public :
	static CRYLTimer*	s_pInstance ;
	static CRYLTimer*	Instance()	{ return s_pInstance ; } ;
	*/
} ;

extern CRYLTimer	GRYLTimer ;

#endif //__RYL_TIMER_H__