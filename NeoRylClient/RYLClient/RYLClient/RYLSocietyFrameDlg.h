//================================================================
//
//	Name : RYLSocietyFrameDlg.h 
//	Desc : 파티, 친구, 거부, 파티 찾기 다이얼로그
//  Date : 2003. 12. 15
//
//================================================================
#ifndef __RYL_SOCIETYFRAMEDLG_H__
#define __RYL_SOCIETYFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CGMUICustomPanel ;

namespace RYLSOCIETYDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
} ;

class CRYLSocietyFrameDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pPartyPanel ;
	CGMUICustomPanel*	m_pFriendPanel ;
	CGMUICustomPanel*	m_pRefusalPanel ;
	CGMUICustomPanel*	m_pPartyFindPanel ;
	
public :
	CRYLSocietyFrameDlg() ;
	virtual ~CRYLSocietyFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID	SetWhisperCharacter( char* lpszName ) ;

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton ; } ;
	CGMUICustomPanel*	GetPartyPanel()			{ return m_pPartyPanel ; } ;
	CGMUICustomPanel*	GetFriendPanel()		{ return m_pFriendPanel ; } ;
	CGMUICustomPanel*	GetRefusalPanel()		{ return m_pRefusalPanel ; } ;
	CGMUICustomPanel*	GetPartyFindPanel()		{ return m_pPartyFindPanel ; } ;
} ;

#endif // __RYL_SOCIETYFRAMEDLG_H__