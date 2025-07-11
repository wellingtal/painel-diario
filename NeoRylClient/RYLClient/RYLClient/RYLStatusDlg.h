//=================================================================
// Name : RYLStatusDlg.h
// Data : 2003. 12. 24
// Desc : ���� ������ �������ͽ� ���̾�α�
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

	CRYLButton*			m_pHideButton ;				// Hide ��ư
	CRYLButton*			m_pExtensionButton ;		// Ȯ�� ��ư
	CRYLLabel*			m_pLevelLabel ;				// ����
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

	CRYLProgressBar*	m_pHPProgressBar ;			// HP ���α׷�����
	CRYLProgressBar*	m_pMPProgressBar ;			// MP ���α׷�����
	CRYLProgressBar*	m_pExpProgressBar ;			// Exp ���α׷�����

	CGMUICustomPanel*	m_pSlotOne ;				// Ȯ�� ����1 ATT&DEF
	CGMUICustomPanel*	m_pSlotTwo ;				// Ȯ�� ����2 ATR&DFR

	INT					m_nHeight ;	
	int					m_nExtensionLevel;			// 0�� HP��, 1�� att&def ����, 2�� ���δ� (0 -> 1 -> 2 -> 0)

	VOID				UpdateExtension();			// EntensionLevel�� ���� ���� (��)Ȱ��ȭ ��Ű�� �Լ�


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