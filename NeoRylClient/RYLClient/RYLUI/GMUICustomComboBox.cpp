#include "GMUICustomComboBox.h"
#include "GMUICustomDlg.h"
#include "GMUIScreen.h"
#include "GMMemory.h"

CGMUICustomComboBox::CGMUICustomComboBox( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pComboButton	= NULL ;
	m_pComboMenu	= NULL ;

	OnChanageEvent	= NULL ;
	OnDrawItemEvent = NULL ;

	CGMUICustomPanel::SetGMUICtrlType( GMUICtrlComboBox ) ;

	InitGMUICtrl() ;
}

CGMUICustomComboBox::~CGMUICustomComboBox()
{
	FinalGMUICtrl() ;
}

VOID CGMUICustomComboBox::SetMenuPos ()
{
	if ( m_pComboMenu == NULL )
	{
		return ;
	}

	INT iWidth = CGMUICustomPanel::GetWidth() ;
    m_pComboMenu->SetWidth( iWidth ) ;

	INT n = m_pComboMenu->GetMenuPosStyle() ;
	
	m_pComboMenu->SetLeft( CGMUICustomPanel::GetLeft() + ( ( CGMUICustomPanel::GetWidth() - m_pComboMenu->GetWidth() ) / 2 ) ) ;
	 
	if ( n == 0 )
	{
		m_pComboMenu->SetTop( CGMUICustomPanel::GetTop() - m_pComboMenu->GetHeight() ) ; 
	}
	else
	{
		m_pComboMenu->SetTop( CGMUICustomPanel::GetTop() + CGMUICustomPanel::GetHeight() ) ;
	}
}

VOID CGMUICustomComboBox::SetButtonPos()
{
	if (m_pComboButton != NULL)
	{
		m_pComboButton->SetLeft( CGMUICustomPanel::GetWidth() - m_pComboButton->GetWidth() + 4 ) ;
	
		INT h = ( m_iHeight - m_pComboButton->GetWidth() ) / 2 ;
		m_pComboButton->SetTop( h + 2 ) ;
	}
}

VOID CGMUICustomComboBox::InitGMUICtrl()
{
	// combobutton
	m_pComboButton = new CGMUIComboButton( m_pGMUIParentDlg ) ;
	m_pComboButton->SetParent( this ) ;
	m_pComboButton->SetWidth( 16 ) ;
	m_pComboButton->SetHeight( 16 ) ;

	CGMUICustomPanel::AddChild( m_pComboButton ) ;
	SetButtonPos() ;

	// combomenu
	m_pComboMenu = new CComboMenu( m_pGMUIParentDlg ) ;
	m_pComboMenu->SetParent( this ) ;

	if ( m_pGMUIParentDlg )
		m_pGMUIParentDlg->AddAlwaysTopObject( m_pComboMenu ) ;
}

VOID CGMUICustomComboBox::FinalGMUICtrl()
{
	GM_DELETE( m_pComboButton ) ;
	GM_DELETE( m_pComboMenu ) ;
}

VOID CGMUICustomComboBox::ViewMenu()
{
	if ( m_pComboMenu->ItemCount () > 0 )
	{
		if ( m_pComboMenu->GetVisible() == FALSE )
		{
			m_pComboMenu->SetVisible( TRUE ) ;
			ShowMenu( m_pComboMenu ) ;
		}
		else
		{
			m_pComboMenu->SetVisible( FALSE ) ;
		}
	}

	CGMUIScreen::GetInstance()->GMUIObjectReleaseCapture() ;
}

VOID CGMUICustomComboBox::ShowMenu( CGMUIObject* Sender )
{
}

VOID CGMUICustomComboBox::HideMenu( CGMUIObject* Sender )
{
}

CComboMenu *CGMUICustomComboBox::GetMenu()
{
	return m_pComboMenu ;
}

CGMUIComboButton *CGMUICustomComboBox::GetButton()
{
	return m_pComboButton ;
}

VOID CGMUICustomComboBox::SelectMenuData( INT nIndex )
{

}

VOID CGMUICustomComboBox::SetText( LPSTR pText )
{
	m_strText = pText ;
	Change( this ) ;
}

LPSTR CGMUICustomComboBox::GetText()
{
	return m_strText ;
}

VOID CGMUICustomComboBox::SetItemHeight( INT Value )
{
	m_pComboMenu->SetItemHeight( Value ) ;
}

INT CGMUICustomComboBox::GetItemHeight()
{
	return m_pComboMenu->GetItemHeight() ;
}

VOID CGMUICustomComboBox::SetItemMerginX( INT Value )
{
	m_pComboMenu->SetItemMerginX( Value ) ;
}

INT CGMUICustomComboBox::GetItemMerginX ()
{
	return m_pComboMenu->GetItemMerginX() ;
}

VOID CGMUICustomComboBox::SetItemMerginY( INT Value )
{
	m_pComboMenu->SetItemMerginY( Value ) ;
}

INT CGMUICustomComboBox::GetItemMerginY()
{
	return m_pComboMenu->GetItemMerginY() ;
}

VOID CGMUICustomComboBox::ItemClear()
{
	if ( m_pComboMenu != NULL )
	{
		m_pComboMenu->ItemClear() ;
	}
}

VOID CGMUICustomComboBox::ItemAdd( LPVOID Value )
{
	if (m_pComboMenu != NULL)
	{
		m_pComboMenu->ItemAdd( Value ) ;
	}
}

VOID CGMUICustomComboBox::ItemGet( INT Index, VOID **ppValue )
{
	if ( m_pComboMenu != NULL )
	{
		m_pComboMenu->ItemGet( Index, ppValue ) ;
	}
	else
	{
		*ppValue = NULL ;
	}
}

VOID CGMUICustomComboBox::ItemDelete( INT Index )
{
	if ( m_pComboMenu != NULL )
	{
		m_pComboMenu->ItemDelete( Index ) ;
	}
}

VOID CGMUICustomComboBox::ItemSet( INT Index, LPVOID pValue)
{
	if ( m_pComboMenu != NULL )
	{
		m_pComboMenu->ItemSet( Index, pValue ) ;
	}
}

INT CGMUICustomComboBox::ItemCount()
{
	if ( m_pComboMenu != NULL )
	{
		return m_pComboMenu->ItemCount() ;
	}
	return -1 ;
}

VOID CGMUICustomComboBox::Click( CGMUIObject* Sender, INT X, INT Y )
{
	if ( ( !CGMUICustomPanel::GetVisible() ) || ( !CGMUICustomPanel::GetEnable() ) )
	{
		return ;
	}
	
	CGMUICustomPanel::Click( this, X, Y ) ;
}

VOID CGMUICustomComboBox::MouseDown(CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	if ( ( !CGMUICustomPanel::GetVisible() ) || ( !CGMUICustomPanel::GetEnable() ) )
	{
		return ;
	}

	CGMUICustomPanel::MouseDown( this, Button, Shift, X, Y ) ;
}

VOID CGMUICustomComboBox::MouseMove(CGMUIObject* Sender, INT Shift, INT X, INT Y)
{
	if ( (Sender != this) || ( !CGMUICustomPanel::GetVisible() ) || ( !CGMUICustomPanel::GetEnable() ) )
	{
		return ;
	}

	CGMUICustomPanel::MouseMove( this, Shift, X, Y ) ;
}

VOID CGMUICustomComboBox::MouseUp(CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	if ( (Sender != this) || ( !CGMUICustomPanel::GetVisible() ) || ( !CGMUICustomPanel::GetEnable() ) )
	{
		return ;
	}

	CGMUICustomPanel::MouseUp( this, Button, Shift, X, Y ) ;

	ViewMenu() ;
}

VOID CGMUICustomComboBox::Show(CGMUIObject* Sender)
{
	if ( Sender != this ) 
	{
		return ;
	}
	
	CGMUICustomPanel::Show( this ) ;
	SetButtonPos() ;
	SetMenuPos() ;
}

VOID CGMUICustomComboBox::Hide( CGMUIObject* Sender )
{
	if ( Sender != this ) 
	{
		return ;
	}
	
	CGMUICustomPanel::Hide( this ) ;
}

VOID CGMUICustomComboBox::Change( CGMUIObject* Sender )
{
	if ( Sender != this ) 
	{
		return ;
	}
	
	if ( OnChanageEvent != NULL )
	{
		OnChanageEvent( Sender ) ;
	}
}

VOID CGMUICustomComboBox::DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	if ( OnDrawItemEvent != NULL )
	{
		OnDrawItemEvent( Sender, Index, r, State ) ;
	}
}

VOID CGMUICustomComboBox::DrawItemBegin( CGMUIObject* Sender )
{
}

VOID CGMUICustomComboBox::DrawItemEnd( CGMUIObject* Sender )
{
}

VOID CGMUICustomComboBox::SetLeft( INT X )
{
	CGMUICustomPanel::SetLeft( X ) ;
	SetButtonPos() ;
	SetMenuPos() ;
}

VOID CGMUICustomComboBox::SetTop( INT Y )
{
	CGMUICustomPanel::SetTop( Y ) ;
	SetButtonPos() ;
	SetMenuPos() ;
}


VOID CGMUICustomComboBox::SetWidth( INT Width )
{
	CGMUICustomPanel::SetWidth( Width ) ;

	SetButtonPos() ;
	SetMenuPos() ;
}

VOID CGMUICustomComboBox::SetHeight( INT Height )
{
	CGMUICustomPanel::SetHeight( Height ) ;

	SetButtonPos() ;
	SetMenuPos() ;
}

VOID CGMUICustomComboBox::SetVisible( BOOL bVisible )
{
	CGMUICustomPanel::SetVisible( bVisible ) ;
	
	if ( !bVisible )
	{
		m_pComboMenu->SetVisible( FALSE ) ;
	}
}

VOID CGMUICustomComboBox::SetEnable( BOOL bEnable )
{
	if ( !bEnable )
	{
		m_pComboMenu->SetVisible( FALSE ) ;
	}
}

VOID CGMUICustomComboBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
}

VOID CGMUICustomComboBox::DoBtnDraw( CGMUIObject* Sender, RECT r, INT State )
{
}

VOID CGMUICustomComboBox::GMUIPaint( INT xx, INT yy )
{
	RECT r ;

	SetRect( &r, xx, yy, xx + CGMUICustomPanel::GetWidth(), yy + CGMUICustomPanel::GetHeight() ) ;

	if ( !CGMUICustomPanel::GetVisible() ) 
	{
		return ;
	}

	DoDraw( this, r, CGMUICustomPanel::GetButtonState() ) ;

	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
}

VOID CGMUICustomComboBox::SetItemIndex( CGMUIObject* Sender )
{
}

//------------------------------------------------------------------------------
//					CGMUIComboButton : public CGMUICustomButton
//------------------------------------------------------------------------------
CGMUIComboButton::CGMUIComboButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomButton( pParentDlg )
{

}

CGMUIComboButton::~CGMUIComboButton()
{

}

VOID CGMUIComboButton::SetParent( LPVOID pGMUIObject )
{
	m_pGMUICustomComboBox = ( CGMUICustomComboBox * )pGMUIObject ;
}

VOID CGMUIComboButton::GetParent( VOID **ppGMUIObject )
{
	*ppGMUIObject = ( LPVOID )m_pGMUICustomComboBox ;
}

VOID CGMUIComboButton::Click( CGMUIObject* Sender, INT X, INT Y )
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) ) 
	{
		return ;
	}
	
	CGMUICustomButton::Click( Sender, X, Y ) ;
}

VOID CGMUIComboButton::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) ) 
	{
		return ;
	}

	CGMUICustomButton::MouseDown( Sender, Button, Shift, X, Y ) ;
}

VOID CGMUIComboButton::MouseMove (CGMUIObject* Sender, INT Shift, INT X, INT Y)
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) ) 
	{
		return ;
	}

	CGMUICustomButton::MouseMove( Sender, Shift, X, Y ) ;
}

VOID CGMUIComboButton::MouseUp (CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) ) 
	{
		return ;
	}

	CGMUICustomButton::MouseUp( Sender, Button, Shift, X, Y ) ;
	
	if ( m_pGMUICustomComboBox != NULL )
	{
		m_pGMUICustomComboBox->ViewMenu() ;
	}
}

VOID CGMUIComboButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	m_pGMUICustomComboBox->DoBtnDraw( Sender, r, State ) ;
}



//////////////////////////////////////////////////////////////////////////////////////
CComboMenu::CComboMenu( CGMUICustomDlg *pParentDlg ) : CGMUICustomMenu( pParentDlg )
{
	m_pGMUICustomComboBox = NULL ;
}

CComboMenu::~CComboMenu()
{
}

VOID CComboMenu::SetParent( LPVOID pGMUIObject )
{
	m_pGMUICustomComboBox = ( CGMUICustomComboBox * )pGMUIObject ;
}

VOID CComboMenu::GetParent( VOID **ppGMUIObject )
{
	*ppGMUIObject = ( VOID *)m_pGMUICustomComboBox ;
}

VOID CComboMenu::Click( CGMUIObject* Sender, INT X, INT Y )
{
	CGMUICustomMenu::Click( Sender, X, Y ) ;
	SetVisible( FALSE ) ;
}

VOID CComboMenu::Show( CGMUIObject* Sender )
{
	if ( m_pGMUICustomComboBox != NULL )
	{
		m_pGMUICustomComboBox->SetMenuPos() ;
		m_pGMUICustomComboBox->ShowMenu( this ) ;
	}

	CGMUICustomMenu::Show( Sender ) ;
	CGMUIObject::GMUIObjectCapture() ;
}

VOID CComboMenu::Hide( CGMUIObject* Sender )
{
	CGMUIObject::GMUIObjectReleaseCapture() ;

	if ( m_pGMUICustomComboBox != NULL )
	{
		if ( GetItemIndex() > -1 )
		{
			m_pGMUICustomComboBox->SelectMenuData( CGMUICustomMenu::GetItemIndex() ) ;
		}
		m_pGMUICustomComboBox->HideMenu( this ) ;
	}

	CGMUICustomMenu::Hide( Sender ) ;
}

VOID CComboMenu::DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	CGMUICustomMenu::DrawItem( Sender, Index, r, State ) ;
	if ( m_pGMUICustomComboBox != NULL )
	{
		m_pGMUICustomComboBox->DrawItem( Sender, Index, r, State ) ;
	}
}

VOID CComboMenu::DrawItemBegin( CGMUIObject* Sender )
{
	CGMUICustomMenu::DrawItemBegin( Sender ) ;
	if ( m_pGMUICustomComboBox )
		m_pGMUICustomComboBox->DrawItemBegin( Sender ) ;
}

VOID CComboMenu::DrawItemEnd( CGMUIObject* Sender )
{
	CGMUICustomMenu::DrawItemEnd( Sender ) ;
	if ( m_pGMUICustomComboBox )
		m_pGMUICustomComboBox->DrawItemEnd( Sender ) ;
}

VOID CComboMenu::SetVisible( BOOL bVisible )
{
	CGMUICustomMenu::SetVisible( bVisible ) ;
}


VOID CComboMenu::SetItemIndex( CGMUIObject* Sender )
{
	if ( m_pGMUICustomComboBox )
		m_pGMUICustomComboBox->SetItemIndex( Sender ) ;
}


