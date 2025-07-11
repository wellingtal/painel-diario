//================================================================
//
//	Name : GMUIObject.h 
//	Desc : 이벤트 및 속성값
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMUIOBJECT_H__
#define __GMUIOBJECT_H__

#include "GMObject.h"
#include "GMObjectDef.h"

class CGMUICustomDlg;

class CGMUITooltip ;
class CGMUITooltipBox ;
class CGMUITooltipManager ;

class CGMUIObject : public CGMObject
{
private:
	BOOL		m_bVisible ;
	BOOL		m_bEnable ;
	BOOL		m_bEntered ;
	BOOL		m_bShowHint ;
	BOOL		m_bFocus ;

	INT			m_hGMUICtrlType ;

	INT			m_iLeft ;
	INT			m_iTop ;
	INT			m_iControlState ;
    
protected :
	CGMUICustomDlg* m_pGMUIParentDlg;
	CGMUIObject*	m_pParent ;
	CGMUIObject*	m_pSecondParent ;
	CGMUITooltip*	m_lpTooltip ;

	INT			m_iWidth ;
	INT			m_iHeight ;

	DWORD		m_dwAlpha ;
	HWND		m_hParentHandle ;

	BOOL		m_bDown ;
	CGMString	m_strHint ;

	DWORD		m_dwColor ;
	DWORD		m_dwFontColor ;
	DWORD		m_dwBorderFontColor ;

	BOOL		m_bClick ;
	BOOL		m_bDisableEmptyCheck ;

public:
	OnMouseDown			OnMouseDownEvent ;
	OnMouseMove			OnMouseMoveEvent ;
	OnMouseUp			OnMouseUpEvent ;
	OnClick				OnClickEvent ;
	OnDblClick			OnDblClickEvent ;
	OnNotifyEvent		OnShowEvent ;
	OnNotifyEvent		OnHideEvent ;
	OnNotifyEvent		OnEnableEvent ;
	OnNotifyEvent		OnDisibleEvent ;
	OnNotifyEvent		OnMouseEnterEvent ;
	OnNotifyEvent		OnMouseLeaveEvent ;
	OnKeyProc			OnKeyDown ;
	OnKeyProc			OnKeyUp ;
	OnNotifyEvent		OnSizeEvent ;

	INT			GetButtonState() ;
	VOID		SetButtonState( INT iControlState ) { m_iControlState = iControlState ; }
public :
	CGMUIObject() ;
	virtual ~CGMUIObject() ;

	VOID		GMUIObjectCapture ();
	VOID		GMUIObjectReleaseCapture ();

	CGMUICustomDlg*	GetParentDlg() ;
	VOID SetParentDlg( CGMUICustomDlg* pGMUIParentDlg ) ;
	
	VOID		SetGMUICtrlType( INT GMUICtrlType ) ;
	INT			GetGMUICtrlType() ;

	RECT		GetGMUIObjectRect () ;
	
	virtual	VOID			SetParent( CGMUIObject* pParent ) ;
	virtual	CGMUIObject*	GetParent() ;

	virtual CGMUIObject*	GetSecondParent() ;
	virtual VOID			SetSecondParent( CGMUIObject* pParent ) ;

	virtual	VOID		 SetAlpha( DWORD dwAlpha ) ;
	virtual	DWORD		 GetAlpha() ;

	//============================================================
	// HInt : 현제 Text만 지원, 나중에 CGMObject로 받음... ㅡㅡ;
	//============================================================
	virtual VOID		SetHint( LPSTR pHint ) ;
	virtual LPSTR		GetHint() ;

	//============================================================
	//
	// Access Method
	//
	//============================================================
	virtual VOID		SetColor( DWORD dwColor )				{ m_dwColor = dwColor ; }
	virtual DWORD		GetColor()								{ return m_dwColor ; }
	virtual VOID		SetFontColor( DWORD dwColor )			{ m_dwFontColor = dwColor ; }
	virtual DWORD		GetFontColor()							{ return m_dwFontColor ; }
	virtual VOID		SetBorderFontColor( DWORD dwColor )		{ m_dwBorderFontColor = dwColor ; }
	virtual DWORD		GetBorderFontColor()					{ return m_dwBorderFontColor ; }
	virtual VOID		SetLeft( INT X )						{ m_iLeft = X ; } 
	virtual INT			GetLeft()								{ return m_iLeft ; }
	virtual VOID		SetTop( INT Y )							{ m_iTop = Y ; }
	virtual INT			GetTop()								{ return m_iTop ; }
	virtual VOID		SetWidth( INT Width )					{ if ( m_iWidth != Width )	{ m_iWidth = Width ; SetSize( this ) ; } }
	virtual INT			GetWidth()								{ return m_iWidth ; }
	virtual VOID		SetHeight( INT Height )					{ if ( m_iHeight != Height ){ m_iHeight = Height ; SetSize( this ) ; } } 
	virtual INT			GetHeight()								{ return m_iHeight ; }
	virtual VOID		SetVisible( BOOL boVisible ) ;
	virtual BOOL		GetVisible()							{ return m_bVisible ; }
	virtual VOID		SetEnable( BOOL boEnable ) ;
	virtual BOOL		GetEnable()								{ return m_bEnable; }
	virtual BOOL		GetEntered()							{ return m_bEntered; }
	virtual VOID		SetDown( BOOL bDown )					{ m_bDown = bDown ; }		
	virtual BOOL		GetDown()								{ return m_bDown ; }
	virtual VOID		SetParentHandle( HWND hWnd )			{ m_hParentHandle = hWnd; }
	virtual HWND		GetParentHandle()						{ return m_hParentHandle ; }
	virtual VOID		SetShowHint( BOOL bShow )				{ m_bShowHint = bShow ; }
	virtual BOOL		GetShowHint()							{ return m_bShowHint ; }
	virtual BOOL		GetIsClick()							{ return m_bClick ; }
	virtual VOID		SetDisableEmptyCheck( BOOL bChecked )	{ m_bDisableEmptyCheck = bChecked ; m_bEnable = FALSE ; }
	virtual BOOL		GetDisableEmptyCheck()					{ return m_bDisableEmptyCheck ; }



	//============================================================
	//
	// Input Access Method
	//
	//============================================================
	virtual VOID		Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID		DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID		MouseEnter( CGMUIObject* Sender ) ;
	virtual VOID		MouseLeave( CGMUIObject* Sender ) ;
	virtual VOID		MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID		MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID		MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID		KeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ;
	virtual VOID		KeyUp( CGMUIObject* Sender, WORD key, INT Shift ) ;
	
	virtual VOID		Show( CGMUIObject* Sender ) ;
	virtual VOID		Hide( CGMUIObject* Sender ) ;

	virtual VOID		SetSize( CGMUIObject* Sender ) ;

	virtual VOID		DoAlphaChange( CGMUIObject* Sender ) ;
	virtual VOID		DoCustomDlgChange( CGMUIObject* Sender ) ;
	virtual VOID		GMUIPaint( INT xx, INT yy ) ;

	virtual VOID		SetTooltip( CGMUITooltipManager *lpTooltipManager, LPCTSTR strTooltip, DWORD dwKey, DWORD dwCtrlType = 0L ) ;

	CGMUITooltip*		GetTooltip()			{ return m_lpTooltip ; }
} ;


// inline
#include "GMUIObject.inl"

#endif //__GMUIOBJECT_H__