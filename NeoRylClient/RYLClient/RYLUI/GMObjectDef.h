//================================================================
//
//	Name : GMObjectDef.h 
//	Desc : UI관련 Data Type 정의
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMOBJECTDEF_H__
#define __GMOBJECTDEF_H__

#include "GMBase.h"

class	CGMUIObject ;
//----------------------------------
//		Define MENU
//----------------------------------
const DWORD		MENU_POS_STYLE_TOP			= 0x00000000 ;
const DWORD		MENU_POS_STYLE_BOTTOM		= 0x00000001 ;

//----------------------------------
//		Define CtrlType
//----------------------------------
const DWORD		GMUICtrlNONE				= 0x00000000 ;
const DWORD		GMUICtrlDIALOG				= 0x00000001 ;
const DWORD		GMUICtrlBUTTON				= 0x00000002 ;
const DWORD		GMUICtrlCHECKBOX			= 0x00000003 ;
const DWORD		GMUICtrlEDITBOX				= 0x00000004 ;
const DWORD		GMUICtrlLABEL				= 0x00000005 ;
const DWORD		GMUICtrlMENU				= 0x00000006 ;
const DWORD		GMUICtrlComboBox			= 0x00000007 ;
const DWORD		GMUICtrlImeView				= 0x00000008 ;
const DWORD		GMUICtrlListBox				= 0x00000009 ;
const DWORD		GMUICtrlPanel				= 0x00000010 ;
const DWORD		GMUICtrlTabButton			= 0x00000011 ;
const DWORD		GMUISPRITE					= 0x00000012 ;
const DWORD		GMUICtrlMessageBox			= 0x00000013 ;

//----------------------------------
//		Define ListBox Selection
//----------------------------------
const DWORD     LB_NONE						= 0x00000000 ;
const DWORD		LB_SELECT					= 0x00000001 ;

//----------------------------------
//		Define MouseButton
//----------------------------------
const DWORD		MB_LEFTCLICK				= 0x00000000 ;
const DWORD     MB_RIGHTCLICK				= 0x00000001 ;
const DWORD		MB_MIDDLECLICK				= 0x00000002 ; 

//----------------------------------
//		Define Label Alignment
//----------------------------------
const DWORD		LAB_CENTER					= 0x00000000 ;	
const DWORD		LAB_LEFT					= 0x00000001 ;
const DWORD		LAB_RIGHT					= 0x00000002 ;

//----------------------------------
//		Define Key state
//----------------------------------
const DWORD		KEY_NONE					= 0x00000000 ;
const DWORD		KEY_SHIFT					= 0x00000001 ;
const DWORD		KEY_ALT						= 0x00000002 ;
const DWORD		KEY_CTRL					= 0x00000003 ;
const DWORD		KEY_LEFT					= 0x00000004 ;
const DWORD		KEY_RIGHT					= 0x00000005 ;
const DWORD		KEY_MIDDLE					= 0x00000006 ;
const DWORD		KEY_DOUBLE					= 0x00000007 ;

//----------------------------------
//		Define Button State
//----------------------------------
const DWORD		BTN_STATE_UP				= 0x00000000 ;
const DWORD		BTN_STATE_DOWN				= 0x00000001 ;
const DWORD		BTN_STATE_ENTER				= 0x00000002 ; 
const DWORD		BTN_STATE_DISIBLE			= 0x00000003 ;
const DWORD		BTN_STATE_CHECKED_DISIBLE	= 0x00000004 ;
const DWORD		BTN_STATE_NCHECKED_DISIBLE	= 0x00000005 ;


//----------------------------------
//		
//----------------------------------
// WM_CREATE Message func
typedef VOID	( *OnCreate )( HWND hWnd ) ;
// WM_DESTROY Message func
typedef VOID	( *OnDestroy )() ;
// Mouse Message func
typedef VOID	( *OnMouseDown )( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
typedef VOID	( *OnMouseMove )( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
typedef VOID	( *OnMouseUp )	( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
typedef VOID	( *OnClick )	( CGMUIObject* Sender, INT X, INT Y ) ;
typedef VOID	( *OnDblClick )	( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

typedef VOID	( *OnNotifyEvent )( CGMUIObject* Sender ) ;
typedef VOID	( *OnDraw )( CGMUIObject* Sender, RECT r ) ;
typedef VOID	( *OnButtonDraw )( CGMUIObject* Sender, RECT r, INT State ) ;
// ListBox
typedef VOID	( *OnDrawItem )( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;

typedef INT		( *OnWinProc )( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
typedef VOID	( *OnKeyProc )( CGMUIObject* Sender, WORD key, INT Shift ) ;
typedef VOID	( *OnDrawImeChar )( CGMUIObject* Sender, INT Index, RECT r, LPSTR Text, INT State ) ;
typedef VOID	( *OnDrawChar )( CGMUIObject* Sender, RECT r, LPSTR str ) ;
typedef VOID	( *OnCharClick )( CGMUIObject* Sender, LPSTR aText, INT Index, INT X, INT Y ) ;
typedef VOID	( *OnDrawButton )( CGMUIObject* Sender, RECT r, BOOL Down, BOOL MouseEnter ) ;



#endif // __GMOBJECTDEF_H__