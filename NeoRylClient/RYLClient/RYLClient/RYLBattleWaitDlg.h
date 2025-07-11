//=================================================================
// Name : RYLBattleWaitDlg.h
// Data : 2004. 07. 27
// Desc : ������� ���̾�α�
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
	CRYLLabel*			m_pTitle ;							// �������
	CRYLLabel*			m_pWaitingRankingLabel ;			// ������
	CRYLLabel*			m_pWaitingRankingValueLabel ;		// ������ ��
	CRYLLabel*			m_pWaitingTimeLabel ;				// ���ð�
	CRYLLabel*			m_pWaitingTimeValueLabel ;			// ���ð� ��
	CRYLLabel*			m_pProgressStateLabel ;				// �����Ȳ
	CRYLLabel*			m_pProgressStateValueLabel ;		// �����Ȳ ��
	CRYLProgressBar*	m_pProgressStateProgrss ;			// �����Ȳ ���α׷�����
	CRYLButton*			m_pChangePositionButton ;			// ��ġ ����
	CRYLButton*			m_pOKButton ;						// Ȯ��

	FLOAT				m_fProgressTime ; 
	LONG				m_lScreenFade ;
	ULONG				m_dwFadeMode ;
	BOOL				m_bShow ;
	BOOL				m_bRespawn;
	BOOL				m_bRespawnWait ;
	BOOL				m_bWarQuit ;

	unsigned long		m_dwTownID ;


	BOOL				m_bPortal;							// ��Ż ����.
	BOOL				m_bClearPortal;						// ��Ż ����.
			
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
