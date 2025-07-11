//=================================================================
// Name : RYLMiniPartyDlg.h
// Data : 2003. 01. 29
// Desc : 미니파티창
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

	CRYLLabel*			m_pMemberLabel ;				// 파티멤버 이름
	CRYLProgressBar*	m_pHPProgrss ;					// 체력 프로그래스바
	CRYLProgressBar*	m_pMPProgrss ;					// 마법 프로그래스바
	INT					m_iMemberSlotIndex ;			// 멤버슬롯 인덱스
	BOOL				m_bShowShort;
	
	CRYLImage*			m_pNormalLargeImage ;				// 일반 멤버
	CRYLImage*			m_pLeaderLargeImage ;				// 리더 멤버 - 우호스킬 받았을때도 사용
	CRYLImage*			m_pDamageLargeImage ;				// 데미지 입은 멤버
	CRYLImage*			m_pNormalSmallImage ;				// 일반 멤버
	CRYLImage*			m_pLeaderSmallImage ;				// 리더 멤버 - 우호스킬 받았을때도 사용
	CRYLImage*			m_pDamageSmallImage ;				// 데미지 입은 멤버
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
	CRYLButton*					m_pEnchantButton ;			// 챈트 아이콘 버튼
	CRYLButton*					m_pShortButton ;			// 이름 단축 버튼

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
