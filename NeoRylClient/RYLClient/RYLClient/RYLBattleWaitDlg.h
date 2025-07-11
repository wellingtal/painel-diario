//=================================================================
// Name : RYLBattleWaitDlg.h
// Data : 2004. 07. 27
// Desc : 전투대기 다이얼로그
//=================================================================
#ifndef __RYL_RYLBATTLEWAITDLG_H__
#define __RYL_RYLBATTLEWAITDLG_H__

#include "RYLDialog.h"

namespace RYLBATTLEWAITDLG
{
	const DWORD		CHANGEPOS_BUTTON					= 0x00000000 ;
	const DWORD		OK_BUTTON							= 0x00000001 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLProgressBar ;

class CRYLBattleWaitDlg : public CRYLDialog
{
public :
	CRYLLabel*			m_pTitle ;							// 전투대기
	CRYLLabel*			m_pWaitingRankingLabel ;			// 대기순위
	CRYLLabel*			m_pWaitingRankingValueLabel ;		// 대기순위 값
	CRYLLabel*			m_pWaitingTimeLabel ;				// 대기시간
	CRYLLabel*			m_pWaitingTimeValueLabel ;			// 대기시간 값
	CRYLLabel*			m_pProgressStateLabel ;				// 진행상황
	CRYLLabel*			m_pProgressStateValueLabel ;		// 진행상황 값
	CRYLProgressBar*	m_pProgressStateProgrss ;			// 진행상황 프로그래스바
	CRYLButton*			m_pChangePositionButton ;			// 위치 변경
	CRYLButton*			m_pOKButton ;						// 확인

	FLOAT				m_fProgressTime ; 
	LONG				m_lScreenFade ;
	ULONG				m_dwFadeMode ;
	BOOL				m_bShow ;
	BOOL				m_bRespawn;
	BOOL				m_bRespawnWait ;
	BOOL				m_bWarQuit ;

	unsigned long		m_dwTownID ;


	BOOL				m_bPortal;							// 포탈 관련.
	BOOL				m_bClearPortal;						// 포탈 삭제.
			
public :
	CRYLBattleWaitDlg() ;
	virtual ~CRYLBattleWaitDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void	SetData( unsigned long	dwTownID, unsigned short wWaitOrder,	unsigned char cHour, unsigned char cMin, unsigned char cSec, bool bCount ) ;
	void	InitDlg( BOOL bCounting ) ;

	void	SetPortal(BOOL bPortal);
	BOOL	GetPortal();
	void	SetClearPortal(BOOL bClearPortal);
	BOOL	GetClearPortal();
} ;

#endif //__RYL_BATTLEGROUNDDLG_H__
