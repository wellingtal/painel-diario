#include "GMUICustomMenu.h"
#include "GMUICustomDlg.h"
#include "GMTexture.h"
#include "GMList.h"
#include "GMMemory.h"
 
CGMUICustomMenu::CGMUICustomMenu( CGMUICustomDlg *pParentDlg ) : CGMUIObject()
{
	m_pGMUIParentDlg = pParentDlg ;

	m_iMenuPosStyle = MENU_POS_STYLE_BOTTOM ;

	m_iItemHeight	= 16 ;
    m_iItemMerginX	= 2 ;
    m_iItemMerginY	= 2 ;

	m_pDataList		= new CGMList ;
	m_iWidth		= 80 ;
	m_iHeight		= 16 ;

	CGMUIObject::SetVisible( FALSE ) ;

	m_iItemIndex	= -1 ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlMENU ) ;

	OnDrawItemEvent = NULL ;
}

CGMUICustomMenu::~CGMUICustomMenu()
{
	ItemClear() ;
	GM_DELETE( m_pDataList ) ;
}

VOID CGMUICustomMenu::SetItemHeight( INT Height )
{
	m_iItemHeight = Height ;
}

INT CGMUICustomMenu::GetItemHeight()
{
	return m_iItemHeight ;
}

VOID CGMUICustomMenu::SetItemMerginX( INT nItemMerginX )
{
	m_iItemMerginX = nItemMerginX ;
}

INT CGMUICustomMenu::GetItemMerginX()
{
    return m_iItemMerginX ;
}

VOID CGMUICustomMenu::SetItemMerginY( INT nItemMerginY )
{
	m_iItemMerginY = nItemMerginY ;
}

INT CGMUICustomMenu::GetItemMerginY()
{
	return m_iItemMerginY ;
}


INT CGMUICustomMenu::GetMenuPosStyle()
{
	return m_iMenuPosStyle ;
}

VOID CGMUICustomMenu::SetMenuPosStyle( INT PosStyle )
{
	m_iMenuPosStyle = PosStyle ;
}

VOID CGMUICustomMenu::ItemClear()
{
	m_pDataList->Clear() ;
}

VOID CGMUICustomMenu::ItemAdd( LPVOID pValue )
{
	if ( m_pDataList ) 
		m_pDataList->Add( pValue ) ;

	CGMUIObject::SetHeight( m_iItemHeight * m_pDataList->Count() ) ;
}

VOID CGMUICustomMenu::ItemGet( INT Index, VOID **ppValue )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() -1 ) )
	{
		*ppValue = NULL ;
		return ;
	}
	m_pDataList->Get( Index, ppValue ) ;
}

VOID CGMUICustomMenu::ItemDelete( INT Index )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() -1 ) )
	{
		return ;
	}
	m_pDataList->Delete( Index ) ;
}

VOID CGMUICustomMenu::ItemSet( INT Index, LPVOID pValue )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() - 1 ) )
	{
		return ;
	}
	LPVOID tmp ;
	m_pDataList->Get( Index, &tmp ) ;
	tmp = pValue ;
}

INT CGMUICustomMenu::ItemCount ()
{
	return m_pDataList->Count() ;
}

INT CGMUICustomMenu::GetItemIndex ()
{
	return m_iItemIndex ;
}

INT CGMUICustomMenu::SetItemIndex( INT Index )
{
	// @@ Â÷ÈÄ¿¡
	return 0 ;
}

void CGMUICustomMenu::SelectItemIndex( int iIndex ) 
{
	m_iItemIndex = iIndex ;
}

VOID CGMUICustomMenu::SetVisible( BOOL Value )
{
	CGMUIObject::SetVisible( Value ) ;
}

VOID CGMUICustomMenu::Click( CGMUIObject* Sender, INT X, INT Y )
{
	if ( ( Sender != this ) || ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )				
	{
		return ;
	}
	
	CGMUIObject::Click( this, X, Y ) ;
}

VOID CGMUICustomMenu::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( ( Sender != this ) || ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )				
	{
		return ;
	}

	CGMUIObject::MouseDown( this, Button, Shift, X, Y ) ;

	INT idx = ( Y / m_iItemHeight ) ; 
	
	if ( ( idx > -1 ) && ( idx < m_pDataList->Count() - 1 ) )
	{
		m_iItemIndex = idx ;
	}
}

VOID CGMUICustomMenu::SetItemIndex( CGMUIObject* Sender )
{
}

VOID CGMUICustomMenu::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	if ( ( Sender != this ) || ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )				
	{
		return ;
	}

	INT iItemIndex = ( ( Y - CGMUIObject::GetTop() ) / m_iItemHeight ) ;
	if ( m_iItemIndex != iItemIndex )
	{
		m_iItemIndex = iItemIndex ;
		SetItemIndex( this ) ;
	}
	
	CGMUIObject::MouseMove( this, Shift, X, Y ) ;
}

VOID CGMUICustomMenu::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( ( Sender != this ) || ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )				
	{
		return ;
	}

	CGMUIObject::MouseUp( this, Button, Shift, X, Y ) ;
}

VOID CGMUICustomMenu::Show (CGMUIObject* Sender)
{
	if ( Sender != this ) 
	{
		return ; 
	}

	CGMUIObject::Show( this ) ;
}

VOID CGMUICustomMenu::Hide( CGMUIObject* Sender )
{
	if ( Sender != this ) 
	{
		return ;
	}
	
	CGMUIObject::Hide( this ) ;
}

VOID CGMUICustomMenu::DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	if ( Sender != this ) 
	{
		return ;
	}
	
	if ( OnDrawItemEvent != NULL )
	{
		OnDrawItemEvent( Sender, Index, r, State ) ;
	}
}

VOID CGMUICustomMenu::GMUIPaint( INT xx, INT yy )
{
	RECT	r ;
	
	if ( !CGMUIObject::GetVisible() ) 
		return ;

	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth(), yy + CGMUIObject::GetHeight() ) ;
	
	INT		x = xx ; 
	INT		y = yy ;
	INT		isel ;

	DoDraw( this, r, 0 ) ;

	DrawItemBegin( this ) ;
	
	for (INT i = 0 ; i < m_pDataList->Count() ; i ++ )
	{
		SetRect( &r, x, y, x + CGMUIObject::GetWidth(), y + m_iItemHeight ) ;
		isel = LB_NONE ;
		if ( i == m_iItemIndex )
		{
			isel = LB_SELECT ;
		}

		DrawItem( this, i, r, isel ) ;
			
		y = y + m_iItemHeight ;
	}

	DrawItemEnd( this ) ;
	CGMUIObject::GMUIPaint( xx, yy ) ;
}


VOID CGMUICustomMenu::DrawItemBegin( CGMUIObject* Sender )
{
}

VOID CGMUICustomMenu::DrawItemEnd( CGMUIObject* Sender )
{
}

VOID	CGMUICustomMenu::AttachMenuImage( CGMTexture* pTexture ) 
{
	m_pMenuImage =  pTexture ;
}

VOID	CGMUICustomMenu::DoDraw( CGMUIObject* Sender, RECT r, INT State ) 
{
	if ( m_pMenuImage )
		m_pMenuImage->Draw( r.left - 3, r.top ) ;
}
