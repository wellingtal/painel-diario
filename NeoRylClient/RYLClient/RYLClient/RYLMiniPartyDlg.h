//=================================================================
// Name : RYLMiniPartyDlg.h
// Data : 2003. 01. 29
// Desc : �̴���Ƽâ
//=================================================================
#ifndef __RYL_MINIPARTYDLG_H__
#define __RYL_MINIPARTYDLG_H__

#include "RYLDialog.h"
#include "GMUICustomPanel.h"

namespace RYLMINIPARTYDLG
{
	const DWORD		ENCHANT_BUTTON			= 0x00000111 ;
	const DWORD		SHORT_BUTTON			= 0x00000112 ;
} ;

class CRYLPicture ;
class CRYLImage ;
class CRYLProgressBar ;
class CRYLLabel ;

class CRYLMiniPartyMemberSlot : public CGMUICustomPanel
{
public :

	CRYLLabel*			m_pMemberLabel ;				// ��Ƽ��� �̸�
	CRYLProgressBar*	m_pHPProgrss ;					// ü�� ���α׷�����
	CRYLProgressBar*	m_pMPProgrss ;					// ���� ���α׷�����
	INT					m_iMemberSlotIndex ;			// ������� �ε���
	BOOL				m_bShowShort;
	
	CRYLImage*			m_pNormalLargeImage ;				// �Ϲ� ���
	CRYLImage*			m_pLeaderLargeImage ;				// ���� ��� - ��ȣ��ų �޾������� ���
	CRYLImage*			m_pDamageLargeImage ;				// ������ ���� ���
	CRYLImage*			m_pNormalSmallImage ;				// �Ϲ� ���
	CRYLImage*			m_pLeaderSmallImage ;				// ���� ��� - ��ȣ��ų �޾������� ���
	CRYLImage*			m_pDamageSmallImage ;				// ������ ���� ���
    CRYLPicture*        m_pOtherZoneImage ;

public :

	CRYLMiniPartyMemberSlot( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLMiniPartyMemberSlot() ;

	VOID		MakePartyMemberSlot( LPRECT lprt ) ;
	VOID		CreateMPProgressBar() ;
    
	VOID		SetPartyMemberState( INT iState ) ;
	VOID		SetPartyMemberName( CHAR* pszName ) ;
	VOID		SetHPProgress( UINT uiCurrentHP, UINT uiMaxHP ) ;
	VOID		SetMPProgress( UINT uiCurrentMP, UINT uiMaxMP ) ;
    VOID        SetOtherZone( );

	VOID		SetEnableShort(BOOL bShowShort) ;
} ;


//============================================================================
// Mini Party Dialog
class CRYLMiniPartyDlg : public CRYLDialog
{
private :
	CRYLMiniPartyMemberSlot*	m_pMemberPartySlot[ 10 ] ;
	CGMUICustomPanel*			m_pButtomSlotLargeImage ;
	CGMUICustomPanel*			m_pButtomSlotSmallImage ;
	INT							m_iSlotYMargin ;
	CRYLButton*					m_pEnchantButton ;			// æƮ ������ ��ư
	CRYLButton*					m_pShortButton ;			// �̸� ���� ��ư

	CRYLImage*					m_pLongCaption;
	CRYLImage*					m_pLongWindow;
	CRYLImage*					m_pShortCaption;
	CRYLImage*					m_pShortWindow;

public :
	BOOL m_bShowEnchant;
	BOOL m_bShowShortName;
	
public :
	CRYLMiniPartyDlg() ;
	virtual ~CRYLMiniPartyDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	ProcessDlgSize() ;

} ;

#endif //__RYL_MINIPARTYDLG_H__
