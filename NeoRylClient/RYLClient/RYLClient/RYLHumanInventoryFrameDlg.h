//================================================================
//
//	Name : RYLHumanInventoryFrameDlg.h 
//	Desc : 휴먼 인벤토리 다이얼로그
//  Date : 2003. 12. 27
//
//================================================================
#ifndef __RYL_HUMANINVENTORYFRAMEDLG_H__
#define __RYL_HUMANINVENTORYFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CGMUICustomPanel ;

namespace RYLHUMANINVENTORYFRAMEDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
	const DWORD		HUMANINVEN_PANEL		= 0x00000001 ;
	const DWORD		SKILL_PANEL				= 0x00000002 ;
	const DWORD		ABILITY_PANEL			= 0x00000003 ;
	const DWORD		SOCIALACTION_PANEL		= 0x00000004 ;
} ;

class CRYLHumanInventoryFrameDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pHumanInvenPanel ;
	CGMUICustomPanel*	m_pAkhanInvenPanel ;
	CGMUICustomPanel*	m_pSkillPanel ;
	CGMUICustomPanel*	m_pAbilityPanel ;
	CGMUICustomPanel*	m_pSocialActionPanel ;
	LPDIRECT3DDEVICE8   m_lpD3DDevice ;
	
public :
	CRYLHumanInventoryFrameDlg() ;
	virtual ~CRYLHumanInventoryFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	ShowInventory() ;
	VOID	ShowSkill() ;
	VOID	ShowAbility() ;
	VOID	ShowSocialAction() ;

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton ; } ;
	CGMUICustomPanel*	GetHumanInvenPanel()	{ return m_pHumanInvenPanel ; } ;
	CGMUICustomPanel*	GetAkhanInvenPanel()	{ return m_pAkhanInvenPanel ; } ;
	CGMUICustomPanel*	GetSkillPanel()			{ return m_pSkillPanel ; } ;
	CGMUICustomPanel*	GetAbilityPanel()		{ return m_pAbilityPanel ; } ;
	CGMUICustomPanel*	GetSocialActionPanel()	{ return m_pSocialActionPanel ; } ;
} ;

#endif // __RYL_HUMANINVENTORYFRAMEDLG_H__