//================================================================
//
//	Name : RYLJoystickConfigureDlg.h 
//	Desc : 2006.4.18
//  Date : 조이스틱 설정 창.
//
//================================================================
#ifndef __RYL_JOYSTICKCONFIGUREDLG_H__
#define __RYL_JOYSTICKCONFIGUREDLG_H__

#include "RYLDialog.h"

class CRYLTabButton;
class CGMUICustomPanel;

namespace RYLJOYSTICKCONFIGUREDLG
{
	const DWORD	TAB_BUTTON		= 0x00000000 ;	
	const DWORD	JOYSTICK_PANEL	= 0x00000001 ;
} ;

class CRYLJoystickConfigureDlg : public CRYLDialog
{
public:

	CRYLTabButton*		m_pTabButton;
	CGMUICustomPanel*	m_pJoystickPanel;

public:

	CRYLJoystickConfigureDlg();
	virtual ~CRYLJoystickConfigureDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	VOID SetVisible(BOOL bVisible );
	
	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y );

	CGMUICustomPanel*	GetRYLJoystickPanel(void)	{ return m_pJoystickPanel; }
	

public:
	
	static CRYLJoystickConfigureDlg*	s_pInstance;
	static CRYLJoystickConfigureDlg*	Instance()		{ return s_pInstance; }
};

#endif