#include "GMUICustomDlg.h"
#include "GMList.h"
#include "GMUIUtil.h"
#include "BaseGraphicsLayer.h"
#include "GMUITooltipManager.h"
#include "GUITooltipManager.h"
#include "RYLMessageBoxManager.h"
#include "GMMemory.h"
 
CGMUICustomDlg::CGMUICustomDlg() : CGMUIObject()
{
	m_pCtrlList    = new CGMList ;
	m_pTopCtrlList = new CGMList ;

	m_iZOrder		= 0 ; 	
	m_boMouseDown	= FALSE ;
	m_bDocking		= FALSE ;

	m_pGMUIParentDlg = this ;

	m_bDocking	= TRUE ;
	m_bMoving	= TRUE ;
	m_iDockSize = 3 ;

	OnStartDrawEvent = NULL ;
	OnEndDrawEvent   = NULL ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlDIALOG ) ;
	CGMUIObject::SetVisible( FALSE ) ;

	CGMUIScreen::GetInstance()->AddDlg( this ) ;
	SetRect( &m_MovingRect, 0, 0, m_iWidth, m_iHeight ) ;

	m_dwDocking		= -1 ;
}	

CGMUICustomDlg::~CGMUICustomDlg()
{
	CGMUIScreen::GetInstance()->DeleteDlg( this ) ;
	GM_DELETE( m_pCtrlList ) ;
	GM_DELETE( m_pTopCtrlList ) ;

	OnStartDrawEvent	= NULL ;
	OnEndDrawEvent		= NULL ;
}

VOID CGMUICustomDlg::SetDockScreenLenth( INT Value )
{
	m_iDockSize = Value ;
}


VOID CGMUICustomDlg::AddAlwaysTopObject( CGMUIObject *pD2Ctrl )
{
	if ( pD2Ctrl != NULL )
	{
		m_pTopCtrlList->Add( pD2Ctrl ) ;
	}
}

HRESULT CGMUICustomDlg::AttachGMUIControl( CGMUIObject *pGMUIObject )
{
	if ( ! pGMUIObject )
		return FALSE ;

	if ( pGMUIObject->GetGMUICtrlType() == GMUICtrlEDITBOX )  
	{
		CGMUIScreen::GetInstance()->AddGMUIEdit( ( CGMUICustomEdit * )pGMUIObject ) ;
	}

	m_pCtrlList->Add( pGMUIObject ) ;

	return S_OK ;
}

HRESULT CGMUICustomDlg::DetachGMUIControl( CGMUIObject *pGMUIObject )
{
	if ( !pGMUIObject )
		return E_FAIL ;

	CGMUIObject *pUIObject ;

	for ( INT i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, ( VOID ** )&pUIObject ) ;

		if ( pUIObject == pGMUIObject )
		{
			m_pTopCtrlList->Delete( i ) ;
			return S_OK ; 
		}
	}

	for ( INT i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pUIObject ) ;
		
		if ( pUIObject == pGMUIObject )
		{
			m_pCtrlList->Delete( i ) ;
			return S_OK ; 
		}
	}

	return E_FAIL ;
}

VOID CGMUICustomDlg::AllHideCtrl ()
{
	INT  i ;
	CGMUIObject *pGMUIObject = NULL ;

	for ( i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		pGMUIObject->SetVisible( FALSE ) ;
	}

	pGMUIObject = NULL ;
	for ( i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, (void **)&pGMUIObject ) ;
		pGMUIObject->SetVisible( FALSE ) ;
	}
}

VOID CGMUICustomDlg::AllShowCtrl()
{
	CGMUIObject *pGMUIObject = NULL ;

	for ( INT i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		pGMUIObject->SetVisible( TRUE ) ;
	}
}

VOID CGMUICustomDlg::SetCaption( LPSTR pszCaption )
{
	m_Caption = pszCaption ;
}

LPSTR CGMUICustomDlg::GetCaption()
{
	return m_Caption ;
}

VOID CGMUICustomDlg::GetDlgPosition( CGMUIObject *pGMUIObject, INT *pX, INT *pY )
{
	INT xx = 0 ;
	INT yy = 0 ;
	
	CGMUIScreen::GetInstance()->GetScreenPosition( this, &xx, &yy ) ;

	*pX = xx + pGMUIObject->GetLeft() ;
	*pY = yy + pGMUIObject->GetTop() ;
}

VOID CGMUICustomDlg::SetSize( CGMUIObject* Sender )
{
	CGMUIObject::SetSize( Sender ) ;
	SetRect( &m_MovingRect, 0, 0, m_iWidth, m_iHeight ) ;
}

VOID CGMUICustomDlg::Click (CGMUIObject* Sender, INT X, INT Y)
{
	INT		i ;
	CGMUIObject *pGMUIObject ;
	for (i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, (VOID **)&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->Click( pGMUIObject, X - pGMUIObject->GetLeft(), Y - pGMUIObject->GetTop() ) ;
				return ;
			}
		}
	}

	for ( i= 0 ; i < m_pCtrlList->Count() ;i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				if ( pGMUIObject->GetGMUICtrlType() != GMUISPRITE )
				{
					pGMUIObject->Click( pGMUIObject, X - pGMUIObject->GetLeft(), Y - pGMUIObject->GetTop() ) ;
					return ;
				}
			}
		}
	}

// TODO : 라지맵 클릭시에 리스폰 버튼을 비활성화 시켜주길래 주석처리 해봄 - by Vincent
//	for ( i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
//	{
//		m_pTopCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
//		if ( pGMUIObject->GetVisible() )
//		{
//			pGMUIObject->SetVisible( FALSE ) ;
//		}
//	}

	CGMUIObject::Click( this, X, Y ) ;
}

VOID CGMUICustomDlg::DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	INT			i ;
	PVOID		tmp ;
	CGMUIObject *D2Object ;

	for ( i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, &tmp ) ;
		D2Object = ( CGMUIObject * )tmp ;

		if ( D2Object->GetVisible() )
		{
			if ( InArea( D2Object->GetLeft(), D2Object->GetTop(), D2Object->GetWidth(), D2Object->GetHeight(), X, Y ) == 0 )
			{
				D2Object->DblClick( D2Object, Button, Shift, X - D2Object->GetLeft(), Y - D2Object->GetTop() ) ;
				return ;
			}
		}
	}


	for ( i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, &tmp ) ;
		D2Object = ( CGMUIObject * )tmp ;

		if ( D2Object->GetVisible() )
		{
			if ( InArea( D2Object->GetLeft(), D2Object->GetTop(), D2Object->GetWidth(), D2Object->GetHeight(), X, Y ) == 0 )
			{
				D2Object->DblClick( D2Object, Button, Shift, X - D2Object->GetLeft(), Y - D2Object->GetTop() ) ;
				return ;
			}
		}
	}

	CGMUIObject::DblClick( this, Button, Shift, X, Y ) ;
}

VOID CGMUICustomDlg::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( !g_MessageBoxManager.GetIsExistModal() )
		CGMUIScreen::GetInstance()->SetViewDlg( this ) ;

	INT		i ;
	PVOID	tmp ;
	CGMUIObject *pGMUIObject ;
	
	for (i = 0 ; i < m_pTopCtrlList->Count() ;i ++ )
	{
		m_pTopCtrlList->Get( i, &tmp ) ;
		pGMUIObject = ( CGMUIObject * )tmp ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseDown( pGMUIObject, Button, Shift, X - pGMUIObject->GetLeft(), Y - pGMUIObject->GetTop() ) ;
				return ;
			}
		}
	}

	for ( i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, &tmp ) ;
		pGMUIObject = ( CGMUIObject * )tmp ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseDown( pGMUIObject, Button, Shift, X - pGMUIObject->GetLeft(), Y - pGMUIObject->GetTop() ) ;
				return ;
			}
		}
	}
	//inherited
	CGMUIObject::MouseDown( this, Button, Shift, X, Y ) ;

	// Form Move
	if ( !m_bMoving )		return;
	
	if (InArea( m_MovingRect.left, m_MovingRect.top, m_MovingRect.right, m_MovingRect.bottom, X, Y ) == 0 )
	{
		m_boMouseDown = TRUE ;
	}

	if ( CGMUIScreen::IsInited() ) 
	{
		m_iOrX = CGMUIScreen::GetInstance()->GetMouseX() ;
		m_iOrY = CGMUIScreen::GetInstance()->GetMouseY() ;
	}
}

VOID CGMUICustomDlg::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	INT		i, sw, sh ;
	INT		myX, myY ;
	CGMUIObject *pGMUIObject ;
	//inherited
	CGMUIObject::MouseMove( this, Shift, X, Y ) ;
	// Form Move
	if ( m_boMouseDown )
	{
		m_dwAlpha = 100 ;
		if ( CGMUIScreen::IsInited() ) 
		{
			myX = CGMUIScreen::GetInstance()->GetMouseX() ;
			myY = CGMUIScreen::GetInstance()->GetMouseY() ;

			CGMUIObject::SetLeft( CGMUIObject::GetLeft() + ( myX - m_iOrX ) ) ;
			m_iOrX = myX ;
			CGMUIObject::SetTop( CGMUIObject::GetTop() + (myY - m_iOrY ) ) ;
			m_iOrY = myY ;
			
			m_dwDocking = -1 ;
			if ( m_bDocking )
			{
				sw = 0 ;
				sh = 0 ;
				if ( CGMUIScreen::IsInited() ) 
				{
					CGMUIScreen::GetInstance()->GetSize( &sw, &sh ) ;
					// left
					if (CGMUIObject::GetLeft() < 0 + m_iDockSize)							 { CGMUIObject::SetLeft( 0 ) ; m_dwDocking = 1 ;}
					// top
					if (CGMUIObject::GetTop() < 0 + m_iDockSize)							 { CGMUIObject::SetTop( 0 ) ;  m_dwDocking = 2 ;}
					// right
					if (CGMUIObject::GetLeft() + CGMUIObject::GetWidth() > sw - m_iDockSize) { CGMUIObject::SetLeft( sw - CGMUIObject::GetWidth() ) ; m_dwDocking = 3; }
					// bottom
					if (CGMUIObject::GetTop() + CGMUIObject::GetHeight() > sh - m_iDockSize) { CGMUIObject::SetTop( sh - CGMUIObject::GetHeight() ) ; m_dwDocking = 4; }
				}
			}
			return ;
		}
	}

	// TopCtrl
	for ( i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			pGMUIObject->MouseLeave( pGMUIObject ) ;
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseEnter( pGMUIObject ) ;
				pGMUIObject->MouseMove( pGMUIObject, Shift, X, Y ) ;
				//return ;
			}
		}
	}

	// Ctrl
	for ( i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			pGMUIObject->MouseLeave( pGMUIObject ) ;
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseEnter( pGMUIObject ) ;
				if ( !g_TooltipManager.m_ttRenderTarget )
				{
					GUITooltipMgr.m_ttRenderTarget = pGMUIObject->GetTooltip();
				}
				else
				{
					GUITooltipMgr.m_ttRenderTarget = NULL ;
				}
				pGMUIObject->MouseMove( pGMUIObject, Shift, X, Y ) ;
				//return ;
			}
		}
	}
}

VOID CGMUICustomDlg::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	INT			 i ;
	CGMUIObject *pGMUIObject ;
	//inherited
	CGMUIObject::MouseUp( this, Button, Shift, X, Y ) ;

	if ( m_boMouseDown )
	{
		m_boMouseDown = FALSE ;
		m_dwAlpha	  = 255 ;
		return ;
	}

	for ( i = 0 ; i < m_pTopCtrlList->Count() ; i ++ )
	{
		m_pTopCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseUp( pGMUIObject, Button, Shift, X, Y ) ;
				return ;
			}
		}
	}

	for ( i = 0 ; i < m_pCtrlList->Count() ; i ++ )
	{
		m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
		if ( pGMUIObject->GetVisible() )
		{
			if ( InArea( pGMUIObject->GetLeft(), pGMUIObject->GetTop(), pGMUIObject->GetWidth(), pGMUIObject->GetHeight(), X, Y ) == 0 )
			{
				pGMUIObject->MouseUp( pGMUIObject, Button, Shift, X, Y ) ;
				return ;
			}
		}
	}
}

VOID CGMUICustomDlg::SetVisible( BOOL bVisible )
{
	CGMUIObject *pGMUIObject ;
	BOOL		 bChanged = FALSE ;

	if ( bVisible == FALSE )
	{
		if ( CGMUIScreen::IsInited() ) 
			CGMUIScreen::GetInstance()->SetModal( NULL ) ;
	}

	if ( bVisible != CGMUIObject::GetVisible() )
	{
		bChanged = TRUE ;
	}

	if ( bChanged )
	{
		if ( CGMUIScreen::IsInited() ) 
		{
			CGMUIScreen::GetInstance()->SetViewDlg( this ) ;
			/*
			if ( CGMUIObject::GetVisible() )
			{
			}
			else
			{
			}
			*/
		}
	}

	if ( bChanged )
	{
		for ( INT i = 0 ; i < m_pCtrlList->Count() ; i ++ )
		{
			m_pCtrlList->Get( i, ( VOID ** )&pGMUIObject ) ;
			pGMUIObject->SetVisible( pGMUIObject->GetVisible() ) ;
		}
	}

	//inherited
	CGMUIObject::SetVisible( bVisible ) ;
}

BOOL CGMUICustomDlg::GetVisible ()
{
	return CGMUIObject::GetVisible() ;
}


VOID CGMUICustomDlg::SetCurrentHint( CGMUIObject *pGMUIObject, RECT HintR, LPSTR pHint )
{
	if ( CGMUIScreen::IsInited() )
	{
		HintR.left   = HintR.left   + CGMUIObject::GetLeft() ;
		HintR.right  = HintR.right  + CGMUIObject::GetLeft() ;
		HintR.top	 = HintR.top    + CGMUIObject::GetTop() ;
		HintR.bottom = HintR.bottom + CGMUIObject::GetTop() ;
		CGMUIScreen::GetInstance()->SetCurrentHint( pGMUIObject, HintR, pHint ) ;
	}
}

VOID CGMUICustomDlg::DoStartDraw (CGMUIObject* Sender, RECT r)
{
	if ( OnStartDrawEvent != NULL )
	{
		OnStartDrawEvent( Sender, r ) ;
	}
}

VOID CGMUICustomDlg::DoEndDraw( CGMUIObject* Sender, RECT r )
{
	if ( OnEndDrawEvent = NULL )
	{
		OnEndDrawEvent( Sender, r ) ;
	}
}

VOID CGMUICustomDlg::ShowModal ()
{
	SetVisible( TRUE ) ;

	if ( CGMUIScreen::IsInited() ) 
		CGMUIScreen::GetInstance()->SetModal( this ) ;
}

VOID CGMUICustomDlg::GMUIPaint( INT xx, INT yy)
{
	RECT r ;
	SetRect( &r, xx, yy, xx + m_iWidth, yy + m_iHeight ) ;

	DoStartDraw( this, r ) ;

	INT		n ;
	LPVOID   tmp ;
	CGMUIObject *pGMUIObject;

	for ( n = 0 ; n < m_pCtrlList->Count() ; n ++ )
	{
		tmp = NULL ;
		m_pCtrlList->Get( n, &tmp ) ;
		pGMUIObject = ( CGMUIObject * )tmp ;
		GetDlgPosition( pGMUIObject, &xx, &yy ) ;

		if ( pGMUIObject->GetVisible() )
		{
			pGMUIObject->GMUIPaint( xx, yy ) ;
		}
	}

	for ( n = 0 ; n < m_pTopCtrlList->Count() ; n ++ )
	{
		tmp = NULL ;
		m_pTopCtrlList->Get( n, &tmp ) ;
		pGMUIObject = ( CGMUIObject * )tmp ;

		GetDlgPosition( pGMUIObject, &xx, &yy ) ;
		
		if (pGMUIObject->GetVisible() )
		{
			pGMUIObject->GMUIPaint( xx, yy ) ;
		}
	}	

	DoEndDraw( this, r ) ;

	CGMUIObject::GMUIPaint( xx, yy ) ;

	//m_TooltipManager.Update( GetLeft(), GetTop() ) ;
}

