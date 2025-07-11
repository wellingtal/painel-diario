//================================================================
//
//	Name : RYLGuildFrameDlg.h 
//	Desc : 길드 다이얼로그
//  Date : 2003. 12. 15
//
//================================================================
#ifndef __RYL_GUILDFRAMEDLG_H__
#define __RYL_GUILDFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CRYLDialog ;
class CGMUICustomPanel ;

namespace RYLGUILDDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
	const DWORD		GUILDINFO_PANEL			= 0x00000001 ;
	const DWORD		GUILDPROPERTY_PANEL		= 0x00000002 ;
	const DWORD		GUILDMEMBER_PANEL		= 0x00000003 ;
	const DWORD		GUILDTACTICS_PANEL		= 0x00000004 ;
} ;

class CRYLGuildFrameDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pGuildInfoPanel ;
	CGMUICustomPanel*	m_pGuildPropertyPanel ;
	CGMUICustomPanel*	m_pGuildMemberPanel ;
	CGMUICustomPanel*	m_pGuildTacticsPanel ;
	
	CRYLDialog*			m_AuthoritySettingDlg ;
	CRYLDialog*			m_pGuildMarkDlg ;
	CRYLDialog*			m_pHostilityDlg ;

	DWORD		m_dwCresteGuildResult ;
	CHAR		m_szCreateGuildName[ 128 ] ;
	DWORD		m_dwResCheckGuildName ;
	
public :
	CRYLGuildFrameDlg() ;
	virtual ~CRYLGuildFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	UpdateGuildMessageBox() ;
	VOID	CheckGuildStipulation() ;

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton ; } ;
	CGMUICustomPanel*	GetGuildInfoPanel()		{ return m_pGuildInfoPanel ; } ;
	CGMUICustomPanel*	GetGuildPropertyPanel()	{ return m_pGuildPropertyPanel ; } ;
	CGMUICustomPanel*	GetGuildMemberPanel()	{ return m_pGuildMemberPanel ; } ; 
	CGMUICustomPanel*	GetGuildTacticsPanel()	{ return m_pGuildTacticsPanel ; } ;

	CRYLDialog*			GetAuthoritySettingDlg() { return m_AuthoritySettingDlg ; } ;
	CRYLDialog*			GetGuildMarkDlg()		 { return m_pGuildMarkDlg ; }
	CRYLDialog*			GetHostilityDlg()		 { return m_pHostilityDlg ; }	
} ;

#endif // __RYL_GUILDFRAMEDLG_H__