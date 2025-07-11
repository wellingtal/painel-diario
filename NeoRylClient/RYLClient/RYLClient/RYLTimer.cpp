#include "RYLTimer.h"
#include "FrameTimer.h"
#include "GMMemory.h"

//CRYLTimer*			CRYLTimer::s_pInstance = NULL ;
CRYLTimer			GRYLTimer ;

CRYLTimer::CRYLTimer() 
{
	m_nShowInterfaceTimerID		= 0L ;
	m_nSceneManagerTimerID		= 0L ;
	m_nWeatherTimerID			= 0L ;
	m_dwServerTime				= 0L ;	// �������� �ִ� ���� �ð�(ƽ) �̰ɷ� ��/���� �����Ѵ�.
	m_dwStartTime				= 0L ;	// Ŭ���̾�Ʈ�� ���� �ð�(ƽ)
	m_dwPartyInfoTimerID		= 0L ;
	m_dwSpeedHackCheckTimerID	= 0L ;
	m_dwUpdateFlushTimerID		= 0L ;
	m_dwCheckStatusTimerID		= 0L ;
	m_dwPingTimerID				= 0L ;
	m_dwCursorTimer				= 0L ;
	m_dwMoveUpdateTimerID		= 0L ;
	m_dwTireTimerID				= 0L ;
	m_dwBombTimerID				= 0L ;
	m_dwSkillCheckSumTimerID	= 0L ;
	m_dwMovieBarTimerID			= 0L ;
	m_dwChinaBillingTimeID	    = 0L ;
//	s_pInstance = this ;
}

CRYLTimer::~CRYLTimer() 
{
}

HRESULT		CRYLTimer::InitTimer() 
{
	CFrameTimer::Create() ;

	m_nShowInterfaceTimerID		= CFrameTimer::Regist( 500.0f ) ;
	m_nSceneManagerTimerID		= CFrameTimer::Regist( 35.0f ) ;
	m_nWeatherTimerID			= CFrameTimer::Regist( 10.0f ) ;
	m_dwPartyInfoTimerID		= CFrameTimer::Regist( 1.0f ) ;
	m_dwSpeedHackCheckTimerID	= CFrameTimer::Regist( 1 / 10.0f ) ;
	m_dwUpdateFlushTimerID		= CFrameTimer::Regist( 10.0f ) ;
	m_dwCheckStatusTimerID		= CFrameTimer::Regist( 1 / 30.0f ) ;
	m_dwPingTimerID				= CFrameTimer::Regist( 1 / 10.0f ) ;
	m_dwCursorTimer				= CFrameTimer::Regist( 10.0f ) ;
	m_dwMoveUpdateTimerID		= CFrameTimer::Regist( 4.0f ) ;
	m_dwMovieBarTimerID			= CFrameTimer::Regist( 150.0f ) ;
	m_dwSkillCheckSumTimerID	= CFrameTimer::Regist( 1 / 10.0f ) ;

	m_dwTireTimerID				= CFrameTimer::Regist( 120.0f ) ;
	m_dwBombTimerID				= CFrameTimer::Regist( 1 / 4.0f ) ;
	
	m_dwChinaBillingTimeID      = CFrameTimer::Regist( 1 / 60.0f ) ;

	return S_OK ;
}