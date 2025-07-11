//================================================================
//
//	Name : RYLSocialActionPanel.h 
//	Desc : 소셜 액션
//  Date : 2005. 01. 19
//
//================================================================
#ifndef __RYL_SOCIALACTIONPANEL_H__
#define __RYL_SOCIALACTIONPANEL_H__

#include "GMUICustomPanel.h"
#include <d3d8.h>
#include <vector>

class CRYLLabel ;
class CRYLButton ;
class CItemInstance ;

class CRYLSocialActionSlot : public CGMUICustomPanel
{
public :
	CRYLLabel*			m_pActionNameLabel ;				// 액션 이름
	CItemInstance*		m_itmActuonSlot ;
	
public :
	CRYLSocialActionSlot( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSocialActionSlot() ;

	void	MakeSkillSlot( char* lpszActionName ) ;
} ;

//======================================================================
// social action slot
namespace RYLSOCIALACTIONPANEL
{
	const DWORD			PREVACTION_BUTTON		= 0x00000000 ;
	const DWORD			NEXTACTION_BUTTON		= 0x00000001 ;
} ;

class CRYLSocialActionPanel : public CGMUICustomPanel
{
private :
	enum Const
	{
		MAX_SLOT_PER_PAGE		= 9,			// 페이지 당 최대 슬롯 수

		MAX_ACTION				= 4,
		ACTION_TYPE				= 5,

		MAX_ACTIONEX			= 5,
	};

	std::vector< CRYLSocialActionSlot* >	m_vecActionSlot ;
	CRYLButton*								m_pPrevActionButton ;
	CRYLButton*								m_pNextActionButton ;
	unsigned long							m_dwSlotHeightMargin ;
	unsigned long							m_dwStartSlot ;
	unsigned short							m_wActionTable[ MAX_ACTION ][ ACTION_TYPE ] ;

	unsigned short							m_wActionExTable[ MAX_ACTIONEX ][ ACTION_TYPE ] ;
public :

	CRYLSocialActionPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSocialActionPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void	InitSocialActionAbilityEx(int nAbilityValue);
	void	ClearSocialAction() ;

	void	InitSocialAction() ;
	void	PrevActionSlotPage() ;
	void	NextActionSlotPage() ;
	void	ActionSlotAlign() ; 
	void    UpdateActionSlotPosition() ;
} ;

#endif //__RYL_SOCIALACTIONPANEL_H__