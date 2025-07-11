//================================================================================
//
//	Name : HumanStoneStatueRespawn.h
//	Date : 2003. 09. 25
//  Desc : 명예의 석상관련 리스폰 윈도우
//================================================================================

#ifndef __HUMANSTONESTATUERESPAWN_H__
#define __HUMANSTONESTATUERESPAWN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUIWindow.h"
#include "RYLGameData.h"

typedef struct __STONESTATUERESPAWNINFO
{
	unsigned short usTurn_Out ;
	unsigned short usWaitNum_Out ;
	unsigned short usLeftTime_Out ;
	unsigned short usHumanNum_Out ;
	unsigned short usAkhanNum_Out ;
}STONESTATUERESPAWNINFO, *LPSTONESTATUERESPAWNINFO ;

enum STONESTATUESTATE { NEUTRALITY_NORMAL = 0,
						NEUTRALITY_ATTACKING = 1,
						HUMAN_RESPAWN = 2,
						HUMAN_RESPAWN_ATTACKING=3, 
						HUMAN_NORMAL=4,
						HUMAN_ATTACKING=5,
						AKHAN_RESPAWN=6,
						AKHAN_RESPAWN_ATTACKING=7,
						AKHAN_NORMAL=8,
						AKHAN_ATTACKING=9 } ;

//--------------------------------------------------------------------------------------------
// desc : 석상 윈도우 [ 3개 ]
//--------------------------------------------------------------------------------------------

typedef struct __STONSTATUEINFO
{
	INT		wNowHP ;
	INT		iStoneStatueState ;
	BOOL	bStoneStatueStateFadeable ;
	BOOL	bRespawnable ;
	long			lScreenFade ;
	unsigned long	dwFadeMode ;

	__STONSTATUEINFO()
	{
		wNowHP					  = 0 ;
		iStoneStatueState		  = NEUTRALITY_NORMAL ;
		bStoneStatueStateFadeable = TRUE ;	
		bRespawnable			  = FALSE ;	
		lScreenFade				  = 0xFFFFFFFF ;
		dwFadeMode				  = FADE_IN ;
	}
}STONSTATUEINFO, *LPSTONSTATUEINFO ;

class CHumanStoneStatue : public CGUIWindow  
{
private :
	STONSTATUEINFO	m_StonStatueInfo[ 3 ] ;
	INT				m_iCurrentHuman ;
	INT				m_iCurrentAkhan ;
		
public :
	CHumanStoneStatue() ;
	virtual ~CHumanStoneStatue() ;

	BOOL	Init(unsigned short x, unsigned short y);
	void	InitValue(void) {} ;
	BOOL	Update(BOOL &bClick, BOOL &bEdge);
	void	ShowWindow(BOOL bShow) ;
	void	Render(LPDIRECT3DDEVICE8 lpD3DDevice);

	void	FadeIn( INT Index )	;
	void	FadeOut( INT Index ) ;

	VOID	SetStoneStatueState( INT Index, INT iKind, INT iState, INT wNowHP, DWORD dwMileage, WORD wHumanNum, WORD wAkhanNum ) ;
												   
												   
												   
												   
											       
												   
												   
} ;

#endif // __HUMANSTONESTATUERESPAWN_H__
