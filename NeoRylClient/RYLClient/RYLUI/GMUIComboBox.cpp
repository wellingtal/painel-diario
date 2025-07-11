#include "GMUIComboBox.h"
#include "GMUICustomDlg.h"
#include "GMUIUtil.h"

#include "GMTexture.h"
#include "GMFontTexture.h"

#include "RYLStringTable.h"
#include "GMMemory.h"


CGMUIComboBox::CGMUIComboBox( CGMUICustomDlg *pParentDlg ) : CGMUICustomComboBox( pParentDlg )
{
	m_pCaptionTexture = NULL ;

	for ( INT i = 0 ; i < GMUIComboButton_Image_Count ; i ++ )
	{
		m_paButtonTexture[ i ] = NULL ;
	}
}

CGMUIComboBox::~CGMUIComboBox()
{
	for ( INT i = 0 ; i < GMUIComboButton_Image_Count ; i ++ )
	{
		m_paButtonTexture[ i ] = NULL ;
	}
	GM_DELETE( m_pCaptionTexture ) ;
	ClearItem() ;
}

VOID CGMUIComboBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	INT h, w, ww ;
	
	ww = 0 ;
	if ( m_paButtonTexture[ ComboButtonImageStateType_Up ] )
		ww = m_paButtonTexture[ComboButtonImageStateType_Up]->GetWidth() ;
	
	w = ( m_iWidth - ww - GetGMUITextWidth( m_strText ) ) / 2 ;
	h = GetGMUITextHeight( "노" ) ;
	h = ( ( m_iHeight - h ) / 2 ) + 1 ;

	SetRect( &r, r.left + w, r.top + h, r.left + w, r.bottom + h ) ;

	if ( m_pCaptionTexture )  
	{
		if ( m_pCaptionTexture->GetFontColor () != m_dwFontColor )
			m_pCaptionTexture->SetFontColor( m_dwFontColor ) ;
		
		m_pCaptionTexture->Draw( r.left, r.top ) ;
	}
	CGMUICustomComboBox::DoDraw( Sender, r, State ) ;
}

VOID CGMUIComboBox::SelectMenuData( INT nIndex )
{
	CGMUICustomComboBox::SelectMenuData( nIndex ) ;

	SCOMBOBOXDATA *pdata = NULL ;
	CGMUICustomComboBox::ItemGet( nIndex, ( VOID ** )&pdata ) ;
	
	if ( pdata )  
		CGMUICustomComboBox::SetText( pdata->Text ) ;
}

VOID CGMUIComboBox::Change( CGMUIObject* Sender )
{
	CGMUICustomComboBox::Change( Sender ) ;
	
	INT n = m_iHeight ;

	if ( m_paButtonTexture[ ComboButtonImageStateType_Up ] )
		n = m_paButtonTexture[ ComboButtonImageStateType_Up ]->GetWidth() ;

	if ( !m_pCaptionTexture )
	{
		CGMFont::Instance()->CreateFontTexture( &m_pCaptionTexture ) ;
		m_pCaptionTexture->SetSize( m_iWidth - n - 2, m_iHeight ) ;
	}
	m_pCaptionTexture->SetText( m_strText ) ;
}

VOID CGMUIComboBox::SetSize( CGMUIObject* Sender )
{
	CGMUICustomComboBox::SetSize( Sender ) ;
}

VOID CGMUIComboBox::DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	INT		h = 5 ;
	INT		MergeX = 0 ;

	SCOMBOBOXDATA *pdata = NULL ;
	CGMUICustomComboBox::ItemGet( Index, ( VOID ** )&pdata ) ;

	switch( State )
	{
		case  LB_SELECT :
			pdata->pFontTexture->SetColor( pdata->SelectColor ) ;
			break ;
		case  LB_NONE   :
			pdata->pFontTexture->SetColor( pdata->TextColor ) ;
			break ;
	}

	pdata->pFontTexture->SetVisible( TRUE ) ;
	pdata->pFontTexture->Draw( r.left + MergeX, r.top + h ) ;
	CGMUICustomComboBox::DrawItem( Sender, Index, r, State ) ;
}

VOID CGMUIComboBox::DoBtnDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUICustomComboBox::DoBtnDraw( Sender, r, State ) ;

	if ( m_paButtonTexture[ State ] )
	{
		m_paButtonTexture[ State ]->Draw( r.left, r.top ) ;
	}
}

VOID CGMUIComboBox::AddItem( SCOMBOBOXDATA *pData )
{
	CGMUICustomComboBox::ItemAdd( pData ) ;
}

VOID CGMUIComboBox::AddItem( LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor )
{
	SCOMBOBOXDATA *pdata;

	pdata = new SCOMBOBOXDATA ;
	//	주의 !! - By Vincent 2005. 10. 12
	//	memset 으로 초기화를 하게되면 pdata->Text 의 CGMString 객체의
	//	생성자 호출시에 할당안 1 바이트를 해제할수 없습니다. 
//	memset( pdata, 0, sizeof( SCOMBOBOXDATA ) ) ;
	pdata->pFontTexture = NULL ;
	pdata->TextColor = 0 ;
	pdata->SelectColor = 0 ;
	pdata->BackColor = 0 ;


	CGMFont::Instance()->CreateFontTexture( &pdata->pFontTexture ) ;

	INT w = m_iWidth - ( m_pComboMenu->GetItemMerginX() * 2 ) ;
	if ( m_paButtonTexture[ ComboButtonImageStateType_Up ] )
		w -= m_paButtonTexture[ ComboButtonImageStateType_Up ]->GetWidth() ;

	//pdata->pFontTexture->SetSize( w, 16 ) ; // 고정 16 --!
	INT iFoneWidth = CRYLStringTable::GetStringWidth( pText ) ;
	pdata->pFontTexture->SetSize( iFoneWidth - w - 2, m_iHeight ) ;
	//pdata->pFontTexture->ClearByColor( dwBackColor ) ; 
	pdata->pFontTexture->SetText( pText ) ;

	pdata->Text			= pText ;
	pdata->SelectColor	= dwSelectColor ;
	pdata->BackColor	= dwBackColor ;
	pdata->TextColor	= dwTextColor ;

	CGMUICustomComboBox::ItemAdd( pdata ) ;
}

VOID CGMUIComboBox::GetItem( INT Index, SCOMBOBOXDATA **Data)
{
	CGMUICustomComboBox::ItemGet( Index, ( VOID ** )Data ) ;
}

VOID CGMUIComboBox::DeleteItem( INT Index )
{
	SCOMBOBOXDATA *pdata = NULL ;
	CGMUICustomComboBox::ItemGet( Index, ( VOID **)&pdata ) ;
	GM_DELETE( pdata->pFontTexture ) ;
	GM_DELETE( pdata ) ;

	CGMUICustomComboBox::ItemDelete( Index ) ;
}

VOID CGMUIComboBox::SetItem( INT Index, LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor )
{
	SCOMBOBOXDATA *pdata = NULL ;

	CGMUICustomComboBox::ItemGet( Index, ( VOID ** )&pdata ) ;
	pdata->pFontTexture->SetText( pText ) ;
	pdata->Text			= pText ;
	pdata->SelectColor	= dwSelectColor ;
	pdata->BackColor	= dwBackColor ;
	pdata->TextColor	= dwTextColor ;
}

VOID CGMUIComboBox::SetItem( INT Index, SCOMBOBOXDATA *pData )
{
	CGMUICustomComboBox::ItemSet( Index, pData ) ;
}

VOID CGMUIComboBox::ClearItem()
{
	SCOMBOBOXDATA *pdata = NULL ;
	
	for ( INT i = 0 ; i < CGMUICustomComboBox::ItemCount() ; i ++ )
	{
		pdata = NULL ;
		CGMUICustomComboBox::ItemGet( i, ( VOID **)&pdata ) ;
		GM_DELETE( pdata->pFontTexture ) ;
		GM_DELETE( pdata) ;
	}

	CGMUICustomComboBox::ItemClear() ;
}

VOID CGMUIComboBox::SetVisible( BOOL bVisible )
{
	CGMUICustomComboBox::SetVisible( bVisible ) ;

	if (m_pCaptionTexture) 
		m_pCaptionTexture->SetVisible( bVisible ) ;
	
	for ( INT i = 0 ; i < GMUIComboButton_Image_Count ; i ++ )
	{
		if ( m_paButtonTexture[ i ] )
			m_paButtonTexture[ i ]->SetVisible( bVisible ) ;
	}
}

VOID CGMUIComboBox::DrawItemBegin( CGMUIObject* Sender )
{
	CGMUICustomComboBox::DrawItemBegin( Sender ) ;
}

VOID CGMUIComboBox::DrawItemEnd( CGMUIObject* Sender )
{
	CGMUICustomComboBox::DrawItemEnd( Sender ) ;
}

VOID CGMUIComboBox::ShowMenu( CGMUIObject* Sender )
{
	CGMUICustomComboBox::ShowMenu( Sender ) ;
	SCOMBOBOXDATA *pdata = NULL ;

	for ( INT i = 0 ; i < CGMUICustomComboBox::ItemCount() ; i ++ )
	{
		pdata = NULL ;
		CGMUICustomComboBox::ItemGet( i, ( VOID ** )&pdata ) ;
		pdata->pFontTexture->SetFontColor( 0xffffffff ) ;
		pdata->pFontTexture->SetShadowColor( 0xffff0000 ) ;
		pdata->pFontTexture->SetVisible( TRUE ) ;
	}
}

VOID CGMUIComboBox::HideMenu( CGMUIObject* Sender )
{
	SCOMBOBOXDATA *pdata = NULL ;
	
	for ( INT i = 0 ; i < CGMUICustomComboBox::ItemCount() ; i ++ )
	{
		pdata = NULL;
		CGMUICustomComboBox::ItemGet( i, ( VOID ** )&pdata ) ;
		pdata->pFontTexture->SetVisible( FALSE ) ;
	}
	CGMUICustomComboBox::HideMenu( Sender ) ;
}

VOID CGMUIComboBox::SetItemIndex( CGMUIObject* Sender )
{
}

VOID CGMUIComboBox::AttachComboButtonTexture( EComboButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paButtonTexture[ StateType ] = pGMTexture ;
}

VOID CGMUIComboBox::DetachComboButtonTexture( EComboButtonImageStateType StateType )
{
	m_paButtonTexture[ StateType ] = NULL ;
}

VOID CGMUIComboBox::SetFontColor( DWORD dwColor )
{
	CGMUICustomComboBox::SetFontColor( dwColor ) ;

	if ( m_pCaptionTexture )
		m_pCaptionTexture->SetColor( m_dwFontColor ) ;
}

