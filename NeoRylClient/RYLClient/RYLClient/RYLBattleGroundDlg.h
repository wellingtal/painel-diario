//=================================================================
// Name : RYLBattleGroundDlg.h
// Data : 2003. 01. 08
// Desc : 배틀그라운드 다이얼로그
//=================================================================
#ifndef __RYL_BATTLEGROUNDDLG_H__
#define __RYL_BATTLEGROUNDDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <Network/Packet/PacketStruct/CharStatusPacket.h>

namespace RYLBATTLEGROUNDDLG
{
	const DWORD		CHANGEPOS_BUTTON					= 0x00000000 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLProgressBar ;

typedef struct __STONESTATUERESPAWNINFOEX
{
	unsigned short usTurn_Out ;
	unsigned short usWaitNum_Out ;
	unsigned short usLeftTime_Out ;
	unsigned short usHumanNum_Out ;
	unsigned short usAkhanNum_Out ;
}STONESTATUERESPAWNINFOEX, *LPSTONESTATUERESPAWNINFOEX ;

class CRYLBattleGroundDlg : public CRYLDialog
{
public :
	CRYLLabel*			m_pWaitingRankingLabel ;			// 대기순위
	CRYLLabel*			m_pWaitingRankingValueLabel ;		// 대기순위 값
	CRYLLabel*			m_pCurrentHumanLabel ;				// 현재휴먼
	CRYLLabel*			m_pCurrentHumanValueLabel ;			// 현재휴먼 값
	CRYLLabel*			m_pCurrentAkhanLabel ;				// 현재아칸
	CRYLLabel*			m_pCurrentAkhanValueLabel ;			// 현재아칸 값
	CRYLLabel*			m_pWaitingTimeLabel ;				// 대기시간
	CRYLLabel*			m_pWaitingTimeValueLabel ;			// 대기시간 값
	CRYLLabel*			m_pProgressStateLabel ;				// 진행상황
	CRYLLabel*			m_pProgressStateValueLabel ;		// 진행상황 값
	CRYLProgressBar*	m_pProgressStateProgrss ;			// 진행상황 프로그래스바
	CRYLButton*			m_pChangePositionButton ;			// 위치 변경

	FLOAT				m_fProgressTime ; 
	LONG				m_lScreenFade ;
	ULONG				m_dwFadeMode ;
	BOOL				m_bShow ;
	BOOL				m_bRespawn;
	BOOL				m_bRespawnWait ;
	BOOL				m_bWarQuit ;

	LPSTONESTATUERESPAWNINFOEX		m_pStoneStatueRespawn ;
			
public :
	CRYLBattleGroundDlg() ;
	virtual ~CRYLBattleGroundDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	SetMyRanking( LPSTR szMyRanking, LPSTR szTotalRanking ) ;	
	VOID	SetLeftoverTime( LPSTR szLeftoverTime )	;	
	VOID	SetCurrentHumanNum( LPSTR szCurrentHumanNum ) ;	
	VOID	SetCurrentAkhanNum( LPSTR szCurrentAkhanNum ) ;

	VOID	SetStoneStatueRespawnInfo( LPSTONESTATUERESPAWNINFOEX	pStoneStatueRespawn ) ;

	VOID    SetRespawnWait( BOOL bWait ) { m_bRespawnWait = bWait ; } ;
	BOOL	GetRespawnWait()			 { return m_bRespawnWait ; }
	VOID	SetWarQuit( BOOL bQuit )     { m_bWarQuit = bQuit ; } ;
	VOID	InitRespawnPoint() ;
} ;

#endif //__RYL_BATTLEGROUNDDLG_H__
