//=================================================================
// Name : RYLStatusDlg.h
// Data : 2003. 12. 24
// Desc : 메인 윈도우 스테이터스 다이얼로그
//=================================================================
#ifndef __RYL_STATUSDLG_H__
#define __RYL_STATUSDLG_H__

#include "RYLDialog.h"

namespace RYLSTATUSDLG
{
	const DWORD		HIDE_BUTTON					= 0x00000000 ;
	const DWORD		EXTENSION_BUTTON			= 0x00000001 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLProgressBar ;
class CGMUICustomPanel ;

class CRYLStatusDlg : public CRYLDialog
{
private:

	CRYLButton*			m_pHideButton ;				// Hide 버튼
	CRYLButton*			m_pExtensionButton ;		// 확장 버튼
	CRYLLabel*			m_pLevelLabel ;				// 레벨
	CRYLLabel*			m_pHPLebel ;
	CRYLLabel*			m_pMPLebel ;

	// new label by shadow

	CRYLLabel*		    m_pExperienceBalueLabel ;		// EXP
	CRYLLabel*		    m_pNationLabel;					// NATION

	CRYLLabel*		    m_pStrikingPowerLabel ;			// DAMAGE
	CRYLLabel*		    m_pHitRatioLabel ;				// ACCUARY
	CRYLLabel*		    m_pCriticalLabel ;				// CRITICAL

	CRYLLabel*		    m_pDefensivePowerLabel ;		// DEFENCE
	CRYLLabel*		    m_pMagicResistLabel ;			// MAGIC DEFENCE
	CRYLLabel*		    m_pEvasionLabel ;				// EVASION
	CRYLLabel*		    m_pArmorRateValueLabel ;		// ARMOR

	CRYLLabel*		    m_pBlockPowerLabel ;			// BLOCK
	CRYLLabel*		    m_pMagicPowerLabel ;			// MAGIC POWER
	CRYLLabel*			m_pHPRECLabel;                  // HP-REC LABEL

	CRYLProgressBar*	m_pHPProgressBar ;			// HP 프로그레스바
	CRYLProgressBar*	m_pMPProgressBar ;			// MP 프로그레스바
	CRYLProgressBar*	m_pExpProgressBar ;			// Exp 프로그레스바

	CGMUICustomPanel*	m_pSlotOne ;				// 확장 슬롯1 ATT&DEF
	CGMUICustomPanel*	m_pSlotTwo ;				// 확장 슬롯2 ATR&DFR

	INT					m_nHeight ;	
	int					m_nExtensionLevel;			// 0은 HP만, 1은 att&def 슬롯, 2는 전부다 (0 -> 1 -> 2 -> 0)

	VOID				UpdateExtension();			// EntensionLevel에 따라서 슬롯 (비)활성화 시키는 함수


public :

	CRYLStatusDlg( INT nLeft, INT nTop, INT nLevel ) ;
	virtual ~CRYLStatusDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	int GetExtensionLevel(void)			{ return m_nExtensionLevel;		}
} ;

#endif //__RYL_STATUSDLG_H__