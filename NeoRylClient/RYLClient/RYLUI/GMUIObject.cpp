#include <winsock2.h>
#include "GMUIObject.h"
#include "GMUICustomDlg.h"
#include "GMUITooltip.h"
#include "GMUITooltipBox.h"
#include "GMUITooltipManager.h"
#include "RYLGameData.h"
#include "GMMemory.h"

CGMUIObject::CGMUIObject() : CGMObject ()
{
	m_bDown			= FALSE ;
	m_hParentHandle = 0;
	m_dwAlpha		= 255 ;

	m_dwColor			= 0xFFFFFFFF ;
	m_dwFontColor		= 0xFF000000 ;
	m_dwBorderFontColor = 0x00FF0000 ;

	m_bVisible		= TRUE ;
	m_bEnable		= TRUE ;
	m_bShowHint		= FALSE ;
	m_bEntered		= FALSE ;
	m_bFocus		= FALSE	;

	m_iWidth = m_iHeight = 0 ;
	m_iLeft  = m_iTop    = 0 ;

	m_hGMUICtrlType		= GMUICtrlNONE ;
	m_pParent			= NULL ;
	m_pSecondParent		= NULL ;
	m_pGMUIParentDlg   = NULL ;

	OnMouseDownEvent	= NULL ;
	OnMouseMoveEvent	= NULL ;
	OnMouseUpEvent		= NULL ;
	OnClickEvent		= NULL ;
	OnDblClickEvent		= NULL ;
	OnShowEvent			= NULL ;
	OnHideEvent			= NULL ;
	OnEnableEvent		= NULL ;
	OnDisibleEvent		= NULL ;
	OnMouseEnterEvent	= NULL ;
	OnMouseLeaveEvent	= NULL ;
	OnKeyDown			= NULL ;
	OnKeyUp				= NULL ;
	OnSizeEvent			= NULL ;

	m_iControlState		= BTN_STATE_UP ;

	m_bClick			= FALSE ;
	m_bDisableEmptyCheck = FALSE ;

	m_lpTooltip			= NULL ;
}

CGMUIObject::~CGMUIObject()
{
	if (CGMUIScreen::IsInited() )
	{
		if(CGMUIScreen::GetInstance()->GetGMUICaptureObject() == this)
			CGMUIScreen::GetInstance()->GMUIObjectReleaseCapture();
	}
	m_pGMUIParentDlg   = NULL ;
}

CGMUICustomDlg* CGMUIObject::GetParentDlg ()
{
	return m_pGMUIParentDlg ;
}

VOID CGMUIObject::SetParentDlg( CGMUICustomDlg* pGMUICustomDlg )
{
	if ( m_pGMUIParentDlg != pGMUICustomDlg )
	{
		m_pGMUIParentDlg = pGMUICustomDlg ;
		DoCustomDlgChange( this ) ;
	}
}

VOID CGMUIObject::SetParent( CGMUIObject* pParent )
{
	m_pParent = pParent ;
}

VOID CGMUIObject::SetSecondParent( CGMUIObject* pParent ) 
{
	m_pSecondParent = pParent ;
}

CGMUIObject* CGMUIObject::GetParent()
{
	return m_pParent ;
}

CGMUIObject*	CGMUIObject::GetSecondParent()
{
	return	m_pSecondParent ;
}

VOID CGMUIObject::SetAlpha( DWORD dwAlpha )
{
	if ( m_dwAlpha != dwAlpha )
	{
		m_dwAlpha = dwAlpha ;
		DoAlphaChange( this ) ;
	}
}

DWORD CGMUIObject::GetAlpha()
{
	return m_dwAlpha ;
}

VOID CGMUIObject::SetGMUICtrlType( INT GMUICtrlType)
{
	m_hGMUICtrlType = GMUICtrlType;
}

int CGMUIObject::GetGMUICtrlType ()
{
	return m_hGMUICtrlType ;
}

VOID CGMUIObject::DoAlphaChange( CGMUIObject* Sender )
{

}

VOID CGMUIObject::SetHint( LPSTR pHint )
{
	m_strHint = pHint ;
}

LPSTR CGMUIObject::GetHint()
{
	return m_strHint ;
}












VOID CGMUIObject::MouseEnter(CGMUIObject* Sender)
{
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable )
	{
		 return ;
	}

	if (OnMouseEnterEvent != NULL)
	{
		OnMouseEnterEvent (Sender);
	}
	m_bEntered = TRUE;
}

VOID CGMUIObject::MouseLeave(CGMUIObject* Sender)
{
	if(!Sender)
	{
		return;
	}

	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
	{
		 return ;
	}
	
	if (OnMouseLeaveEvent != NULL)
	{
		OnMouseLeaveEvent (Sender);
	}
	m_bEntered = FALSE;
}


VOID CGMUIObject::Click (CGMUIObject* Sender, int X, int Y)
{
	if(!Sender)//hz guigui wasnt here
	{
		m_bClick = TRUE ;
		m_bClick = FALSE ;
		return;
	}
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
		 return ;

	if (OnClickEvent)
	{
		OnClickEvent (Sender, X, Y);
		m_bClick = TRUE ;
		CRYLGameData::Instance()->PlayClickSound() ;
	}

	if (m_pParent)
	{
		m_pParent->Click (this, X, Y);
		m_bClick = TRUE ;
	}
}

VOID CGMUIObject::DblClick (CGMUIObject* Sender, int Button, int Shift, int X, int Y)
{
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
		 return ;

	if (OnDblClickEvent)
	{
		OnDblClickEvent (Sender, Button, Shift, X, Y);
		m_bClick = TRUE ;
	}
	if (m_pParent)
	{
		m_pParent->DblClick (this, Button, Shift, X, Y);
		m_bClick = TRUE ;
	}
}

VOID CGMUIObject::MouseDown (CGMUIObject* Sender, int Button, int Shift, int X, int Y)
{
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
		 return ;

	if (OnMouseDownEvent)
	{
		OnMouseDownEvent (Sender, Button, Shift, X, Y);
	}
	GMUIObjectCapture();

	if (m_pParent)
	{
		m_pParent->MouseDown (this, Button, Shift, X, Y);
	}
	m_bDown = TRUE;
//	m_bClick = TRUE ;
}

VOID CGMUIObject::MouseMove (CGMUIObject* Sender, int Shift, int X, int Y)
{
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
		 return ;

	RECT r;
	CGMUICustomDlg *ptmpDlg;
	SetRect (&r, m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight);

	if (m_bShowHint)
	{
		if (m_hGMUICtrlType == GMUICtrlDIALOG)
		{
			ptmpDlg = (CGMUICustomDlg *)Sender;
			SetRect (&r, 0, 0, m_iWidth, m_iHeight);
			ptmpDlg->SetCurrentHint (this, r, m_strHint);
		}
		else
		{
			if (m_pGMUIParentDlg)
			{
				ptmpDlg = m_pGMUIParentDlg;
				ptmpDlg->SetCurrentHint (this, r, m_strHint);
			}
		}
	}

	/*
	if (m_pGMUIParentDlg)
	{
		ptmpDlg = m_pGMUIParentDlg;
		ptmpDlg->SetCurrentHint (this, r, m_strHint);
	}
	*/

	if (OnMouseMoveEvent)
	{
		OnMouseMoveEvent (Sender, Shift, X, Y);
	}

	if (m_pParent)
	{
		m_pParent->MouseMove (this, Shift, X, Y);
	}
	m_bClick = FALSE ;
}

VOID CGMUIObject::MouseUp (CGMUIObject* Sender, int Button, int Shift, int X, int Y)
{
	if ( m_iControlState == BTN_STATE_DISIBLE ||
		 m_iControlState == BTN_STATE_CHECKED_DISIBLE ||
		 m_iControlState == BTN_STATE_NCHECKED_DISIBLE || !m_bEnable)
		 return ;

	if (OnMouseUpEvent)
	{
		OnMouseUpEvent (Sender, Button, Shift, X, Y);
	}

	GMUIObjectReleaseCapture();

	if (m_pParent)
	{
		m_pParent->MouseUp (this, Button, Shift, X, Y);
	}
	m_bDown = FALSE;
	m_bClick = FALSE ;
}

INT CGMUIObject::GetButtonState ()
{
	m_iControlState = BTN_STATE_UP;
	
	if (CGMUIObject::GetEnable())
	{
		if (m_bDown) 
		{
			m_iControlState = BTN_STATE_DOWN;
		}
		else   
		{
			if (CGMUIObject::GetEntered())
			{
				m_iControlState = BTN_STATE_ENTER;
			}
			else
			{
				m_iControlState = BTN_STATE_UP;
			}
		}
	}
	else
	{
		m_iControlState = BTN_STATE_DISIBLE ;

		if ( m_hGMUICtrlType == GMUICtrlCHECKBOX )
		{
			if ( m_bDisableEmptyCheck )
				m_iControlState = BTN_STATE_CHECKED_DISIBLE ;
			else
				m_iControlState = BTN_STATE_NCHECKED_DISIBLE ;
		}
	}
	return m_iControlState ;
}

VOID CGMUIObject::KeyDown (CGMUIObject* Sender, WORD key, int Shift)
{
	if (OnKeyDown != NULL)
	{
		OnKeyDown (Sender, key, Shift);
	}
	if (m_pParent != NULL)
	{
		m_pParent->KeyDown (this, key, Shift);
	}
}

VOID CGMUIObject::KeyUp (CGMUIObject* Sender, WORD key, int Shift)
{
	if (OnKeyUp != NULL)
	{
		OnKeyUp (Sender, key, Shift);
	}
	if (m_pParent != NULL)
	{
		m_pParent->KeyUp (this, key, Shift);
	}
}

VOID CGMUIObject::Show (CGMUIObject* Sender)
{
	if (OnShowEvent != NULL)
	{
		OnShowEvent (Sender);
	}

	if (m_pParent != NULL)
	{
		m_pParent->Show (this);
	}
}

VOID CGMUIObject::Hide (CGMUIObject* Sender)
{
	if (OnHideEvent != NULL)
	{
		OnHideEvent (Sender);
	}

	if (m_pParent != NULL)
	{
		m_pParent->Hide (this);
	}
}

VOID CGMUIObject::SetSize (CGMUIObject* Sender)
{
	if (OnSizeEvent = NULL)
	{
		OnSizeEvent (Sender);
	}
	if (m_pParent != NULL)
	{
		m_pParent->SetSize (this);
	}
}

RECT CGMUIObject::GetGMUIObjectRect ()
{
	RECT r;
	SetRect (&r, m_iLeft, m_iTop, m_iLeft+m_iWidth, m_iTop+m_iHeight);
	return r;
}

VOID CGMUIObject::GMUIPaint( INT xx, INT yy)
{
}

VOID CGMUIObject::GMUIObjectCapture ()
{
	if (m_pGMUIParentDlg)
	{
		if (CGMUIScreen::IsInited() )
			CGMUIScreen::GetInstance()->GMUIObjectSetCapture (this, m_iLeft, m_iTop);
	}
}

VOID CGMUIObject::GMUIObjectReleaseCapture ()
{
	if (m_pGMUIParentDlg)
	{
		if (CGMUIScreen::IsInited() )
			CGMUIScreen::GetInstance()->GMUIObjectReleaseCapture ();
	}
}

VOID CGMUIObject::DoCustomDlgChange (CGMUIObject* Sender)
{
}

VOID	CGMUIObject::SetVisible( BOOL boVisible ) 
{ 
	if ( boVisible ) 
	{ 
		m_bVisible = TRUE ; 
		Show (this);	
	}
	else           
	{ 
		m_bVisible = FALSE;	
		Hide (this);	
	} 
}


VOID	CGMUIObject::SetEnable( BOOL boEnable ) 
{
	if (boEnable)
	{
		m_bEnable = TRUE;
		if (OnEnableEvent != NULL)
		{
			OnEnableEvent (this);
		}
	}
	else 
	{
		m_bEnable = FALSE;
		if (OnDisibleEvent != NULL)
		{
			OnDisibleEvent (this);
		}
	}
}

VOID	CGMUIObject::SetTooltip( CGMUITooltipManager *lpTooltipManager, LPCTSTR strTooltip, DWORD dwKey, DWORD dwCtrlType )
{
	if ( !m_lpTooltip )
	{
		m_lpTooltip = new CGMUITooltipBox ;
		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltip, dwKey, dwCtrlType ) ;
		INT iTop    = m_iTop ;
		INT iBottom = m_iTop + m_iHeight ;
		if ( dwCtrlType == TOOLTIP_PANEL )
		{
			iTop += 36 ;
			iBottom += 36 ;
		}
		( ( CGMUITooltipBox* )m_lpTooltip )->SetRect( m_iLeft, iTop, m_iLeft + m_iWidth, iBottom ) ;
		lpTooltipManager->AddTooltip(m_lpTooltip);
	} 
	else
	{
		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltip, dwKey, dwCtrlType ) ;
	}
}