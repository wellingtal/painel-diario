//=================================================================
// Name : RYL Timer.h
// Data : 2003. 11. 10
// Desc : RYL Timer 
//			- Client���� ���� �ý��� Ÿ�̸�
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
	DWORD  m_dwServerTime ;				// �������� �ִ� ���� �ð�(ƽ) �̰ɷ� ��/���� �����Ѵ�.
	DWORD  m_dwStartTime ; 				// Ŭ���̾�Ʈ�� ���� �ð�(ƽ)
	DWORD  m_dwPartyInfoTimerID ;
	DWORD  m_dwSpeedHackCheckTimerID ;	
	DWORD  m_dwUpdateFlushTimerID ;
	DWORD  m_dwCheckStatusTimerID ;
	DWORD  m_dwPingTimerID ;
	DWORD  m_dwCursorTimer ;
	DWORD  m_dwMoveUpdateTimerID ;
	DWORD  m_dwSkillCheckSumTimerID ;
	DWORD  m_dwMovieBarTimerID ;
	DWORD  m_dwChinaBillingTimeID ;		// �߱� ���� �ð�
    	
	// �������� Ÿ�̸�
	DWORD  m_dwTireTimerID ;			// Ÿ�̾� Ÿ�̸�
	DWORD  m_dwBombTimerID ;			// ��ź Ÿ�̸�
	
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
	
	// �������� Ÿ�̸�
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