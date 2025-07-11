#include "GMUIListBox.h"
#include "GMUICustomDlg.h"
#include "GMUIScreen.h"
#include "GMUIUtil.h"

#include "GMTexture.h"
#include "GMFontTexture.h"
#include "GMMemory.h"
 

//------------------------------------------------------------------------------
CGMUIListBox::CGMUIListBox( CGMUICustomDlg *pGMUIDlg, BOOL bScrollEnable ) : CGMUICustomListBox( pGMUIDlg, bScrollEnable )
{
	for ( INT i = 0 ; i < GMUIListBox_Button_Image_Count; i++ )
	{
		m_paTopButtonTexture[ i ]		= NULL ;
		m_paBottomButtonTexture[ i ]	= NULL ;
		m_paTrackButtonTexture[ i ]		= NULL ;
	}
}

CGMUIListBox::~CGMUIListBox()
{
	for ( INT i = 0 ; i < GMUIListBox_Button_Image_Count ; i ++ )
	{
		m_paTopButtonTexture[ i ]		= NULL ;
		m_paBottomButtonTexture[ i ]	= NULL ;
		m_paTrackButtonTexture[ i ]		= NULL ;
	}

	ClearItem() ;
}

VOID CGMUIListBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUICustomListBox::DoDraw( Sender, r, State ) ;
}

VOID CGMUIListBox::DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	INT		x = r.left ;
	INT		y = r.top ;

	// 차후에 각각의 object의 alpha를 따로 지원할것
	GMUILISTBOXITEM *pdata = NULL ;
	GetData( Index, ( VOID ** )&pdata ) ;	
	if ( pdata == NULL ) 
		return ;
/*
	if (m_pAUXUIForm->GetAlpha() != 255)
	{
		pdata->FontTexture->SetAlpha (m_pAUXUIForm->GetAlpha());
	}
*/
	switch( State )
	{
		case  LB_SELECT :
			pdata->pFontTexture->SetFontColor( pdata->SelectColor ) ;
			break;
		case  LB_NONE   :
			pdata->pFontTexture->SetFontColor( pdata->TextColor ) ;
			
			break;
	}
	pdata->pFontTexture->Draw( x, y ) ;

	CGMUICustomListBox::DoDrawItem( Sender, Index, r, State ) ;
}

VOID CGMUIListBox::DoTopDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_paTopButtonTexture[ State ] )
		m_paTopButtonTexture[ State ]->Draw( r.left, r.top ) ;
	
	CGMUICustomListBox::DoTopDraw( Sender, r, State ) ;
}

VOID CGMUIListBox::DoTrackDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_paTrackButtonTexture[ State ] )
		m_paTrackButtonTexture[ State ]->Draw( r.left - 2, r.top ) ;
	CGMUICustomListBox::DoTrackDraw( Sender, r, State ) ;
}

VOID CGMUIListBox::DoBottomDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_paBottomButtonTexture[ State ] )
		m_paBottomButtonTexture[ State ]->Draw( r.left, r.top ) ;
	CGMUICustomListBox::DoBottomDraw( Sender, r, State ) ;
}

VOID CGMUIListBox::AddItem( LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor )
{
	GMUILISTBOXITEM*	pdata ;
	pdata = new GMUILISTBOXITEM ;

	CGMFont::Instance()->CreateFontTexture( &pdata->pFontTexture ) ;

	//pdata->pFontTexture->SetSize (GetItemWidth(), GetItemHeight(), FALSE);
	pdata->pFontTexture->SetSize( GetItemWidth(), GetItemHeight() ) ; // 임시
//	pdata->FontTexture->ClearByColor (dwBackColor);
	pdata->pFontTexture->SetText( pText ) ;

	pdata->Text			= pText ;
	pdata->SelectColor	= dwSelectColor ;
	pdata->BackColor	= dwBackColor ;
	pdata->TextColor	= dwTextColor ;
	AddItem( pdata ) ;
}

VOID CGMUIListBox::AddItem( GMUILISTBOXITEM *pData )
{
	CGMUICustomListBox::AddData( pData ) ;
}

LPSTR CGMUIListBox::GetItem( INT Index )
{
	GMUILISTBOXITEM* pData = NULL ;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pData ) ;

	if ( !pData )
		return NULL ;
	
	return pData->Text ;
}

VOID CGMUIListBox::GetItem( INT Index, GMUILISTBOXITEM **Data )
{
	CGMUICustomListBox::GetData( Index, ( VOID ** )Data ) ;
}

VOID CGMUIListBox::DeleteItem( INT Index )
{
	GMUILISTBOXITEM *pdata = NULL ;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pdata ) ;
	GM_DELETE( pdata->pFontTexture ) ;
	GM_DELETE( pdata ) ;
	CGMUICustomListBox::DeleteData( Index ) ;
}

VOID CGMUIListBox::SetItem( INT Index, GMUILISTBOXITEM *pData )
{
	CGMUICustomListBox::SetData( Index, pData ) ;
	pData->pFontTexture->SetText( pData->Text ) ;
}

VOID CGMUIListBox::SetItem( INT Index, LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor )
{
	GMUILISTBOXITEM *pdata = NULL ;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pdata ) ;

	pdata->Text			= pText ;
	pdata->SelectColor	= dwSelectColor ;
	pdata->BackColor	= dwBackColor ;
	pdata->TextColor	= dwTextColor ;
	pdata->pFontTexture->SetText( pdata->Text ) ;
}

VOID CGMUIListBox::SetItem( INT Index, LPSTR pText )
{
	GMUILISTBOXITEM *pdata	= NULL ;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pdata ) ;
	pdata->Text = pText ;
	pdata->pFontTexture->SetText( pdata->Text ) ;
}

VOID	CGMUIListBox::SetItemColor( INT Index, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) 
{
	GMUILISTBOXITEM *pdata = NULL ;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pdata ) ;

	pdata->SelectColor	= dwSelectColor ;
	pdata->BackColor	= dwBackColor ;
	pdata->TextColor	= dwTextColor ;
}

VOID CGMUIListBox::ClearItem()
{
	GMUILISTBOXITEM *pdata = NULL ;
	for ( INT i = 0 ; i < CGMUICustomListBox::Count() ; i ++ )
	{
		pdata = NULL ;
		CGMUICustomListBox::GetData( i, ( VOID ** )&pdata ) ;
		GM_DELETE( pdata->pFontTexture ) ;
		GM_DELETE( pdata ) ;
	}
	CGMUICustomListBox::ClearData() ;
}


VOID CGMUIListBox::AttachTopImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture)
{
	m_paTopButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize();
}

VOID CGMUIListBox::DetachTopImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paTopButtonTexture[ StateType ] = NULL ;
}

VOID CGMUIListBox::AttachBottomImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paBottomButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID CGMUIListBox::DetachBottomImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paBottomButtonTexture[ StateType ] = NULL ;
}

VOID CGMUIListBox::AttachTrackImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paTrackButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID CGMUIListBox::DetachTrackImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paTrackButtonTexture[ StateType ] = NULL ;
}


