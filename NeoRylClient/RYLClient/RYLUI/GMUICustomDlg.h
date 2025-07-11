//================================================================
//
//	Name : GMUICustomDlg.h 
//	Desc : Gama Soft Custom Dialog Class
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMUICUSTOMDLG_H__
#define __GMUICUSTOMDLG_H__

#include "GMUIObject.h"
#include "GMUIScreen.h"
#include "GMUITooltipManager.h"

class CGMList;
 
class CGMUICustomDlg : public CGMUIObject
{
private :

	INT			m_iOrX ;
	INT			m_iOrY ;

	CGMList*	m_pCtrlList ;
	CGMList*	m_pTopCtrlList ;
	BOOL		m_bMoving ;

	DWORD		m_dwDocking ;


protected :

	INT			m_iZOrder ;
	RECT		m_MovingRect ;
	BOOL		m_boMouseDown ;
	INT			m_iDockSize ;
	BOOL		m_bDocking ;
	CGMString	m_Caption ;
	

public:

	OnDraw		OnStartDrawEvent ;
	OnDraw		OnEndDrawEvent ;

	CGMUITooltipManager m_TooltipManager;


public:

	CGMUICustomDlg() ;
	virtual ~CGMUICustomDlg() ;

	VOID		AllHideCtrl() ;
	VOID		AllShowCtrl() ;

	VOID		SetDockScreenLenth( INT Value ) ;
	VOID		SetDlgMoving( BOOL Value )			{ m_bMoving = Value ;	}
	BOOL		GetDlgMoving() 						{ return m_bMoving ;	} 
	VOID		GetDlgPosition( CGMUIObject *pD2Object, INT *pX, INT *pY ) ;

	INT			GetMouseX() 	{ return CGMUIScreen::GetInstance()->GetMouseX() - GetLeft() ;	}
	INT			GetMouseY() 	{ return CGMUIScreen::GetInstance()->GetMouseY() - GetTop() ;	}

	VOID		AddAlwaysTopObject( CGMUIObject *pD2Ctrl ) ;

	virtual VOID		SetCaption( LPSTR pszCaption ) ;
	virtual LPSTR		GetCaption() ;
	virtual VOID		ShowModal ();
	virtual VOID		SetZOrder( INT aOrder )		{ m_iZOrder = aOrder ;	}
	virtual INT			GetZOrder()					{ return m_iZOrder ;	}
	virtual VOID		SetVisible( BOOL bVisible ) ;
	virtual BOOL		GetVisible() ;
	virtual HRESULT		AttachGMUIControl( CGMUIObject *pGMUIObject ) ;
	virtual HRESULT		DetachGMUIControl( CGMUIObject *pGMUIObject ) ;
	virtual VOID		SetCurrentHint( CGMUIObject *pGMUIObject, RECT HintR, LPSTR pHint ) ;
	virtual VOID		SetSize( CGMUIObject* Sender ) ;

	virtual VOID		Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID		DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID		MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y);
	virtual VOID		MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y);
	virtual VOID		MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y);

	virtual VOID		DoStartDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID		DoEndDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID		GMUIPaint( INT xx, INT yy ) ;

	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) = 0 ;

	DWORD		GetDockingValue()			{ return m_dwDocking ; }
} ;

#endif // __GMUICUSTOMDLG_H__