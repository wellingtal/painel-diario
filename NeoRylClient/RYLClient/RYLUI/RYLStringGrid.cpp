#include "RYLStringGrid.h"
#include "GMUICustomDlg.h"
#include "GMUIScreen.h"
#include "GMUIUtil.h"

#include "GMTexture.h"
#include "GMFontTexture.h"

#include "RYLGridLabel.h"
#include "RYLImageManager.h"

#include "GMImageDib.h"
#include "RYLImage.h"

#include "GMBase.h"
#include "GMMemory.h"
 
//------------------------------------------------------------------------------
CRYLStringGrid::CRYLStringGrid( CGMUICustomDlg *pGMUIDlg, BOOL bScrollEnable ) : CGMUICustomListBox( pGMUIDlg, bScrollEnable )
{
	m_iColCount = 0 ;

	for ( INT i = 0 ; i < GMUIListBox_Button_Image_Count ; i ++ )
	{
		m_paTopButtonTexture[ i ]		= NULL ;
		m_paBottomButtonTexture[ i ]	= NULL ;
		m_paTrackButtonTexture[ i ]		= NULL ;
	}

	m_dwSelectedItemColor = 0xFFFFFF00;
}

CRYLStringGrid::~CRYLStringGrid()
{
	for ( INT i = 0 ; i < GMUIListBox_Button_Image_Count ; i ++ )
	{
		m_paTopButtonTexture[ i ]		= NULL ;
		m_paBottomButtonTexture[ i ]	= NULL ;
		m_paTrackButtonTexture[ i ]		= NULL ;
	}

	ClearItem () ;
}

VOID CRYLStringGrid::SetColCount( INT ColCount )
{
	m_iColCount = ColCount ;
}

VOID CRYLStringGrid::AddItem( INT Col, LPSTR pText, INT Width, DWORD dwColor, int iPosX )
{
	CRYLGridLabel* pGridLabel = NULL ;

	if ( Col == 0 )
	{
		pGridLabel = new CRYLGridLabel( m_iColCount ) ;
		CGMUICustomListBox::AddData( pGridLabel ) ;
	}
	CGMUICustomListBox::GetData( CGMUICustomListBox::Count() - 1, ( VOID ** )&pGridLabel ) ;
	
	if ( !pGridLabel )
    {
		GMASSERT( FALSE, "CRYLStringGRid::AddItem Failed!" ) ;
    }

	pGridLabel->SetSize( Col, Width, iPosX ) ;
	pGridLabel->SetData( Col, pText ) ;
	pGridLabel->SetFontColor( Col, dwColor ) ;
}

LPSTR CRYLStringGrid::GetItem( INT Index, INT Col )
{
	CRYLGridLabel* pGridLabel = NULL ;

	CGMUICustomListBox::GetData( Index, ( VOID **)&pGridLabel ) ;
	if ( !pGridLabel )
		return NULL ;

	return pGridLabel->GetData( Col ) ;
}

VOID CRYLStringGrid::DeleteItem( INT Index )
{
	CRYLGridLabel* pGridLabel = NULL ;

	CGMUICustomListBox::GetData( Index, ( VOID **)&pGridLabel ) ;
	GM_DELETE( pGridLabel ) ;
	CGMUICustomListBox::DeleteData( Index ) ;
}

VOID CRYLStringGrid::SetItem( INT Index, INT Col, LPSTR pText )
{
	CRYLGridLabel* pGridLabel = NULL ;

	CGMUICustomListBox::GetData( Index, ( VOID **)&pGridLabel ) ;
	if ( pGridLabel )
		pGridLabel->SetData( Col, pText ) ;
}

VOID CRYLStringGrid::ClearItem ()
{
	for ( INT i = 0 ; i < CGMUICustomListBox::Count() ; i ++ )
	{
		CRYLGridLabel* pGridLabel = NULL ;
		CGMUICustomListBox::GetData( i, ( VOID ** )&pGridLabel ) ;
		GM_DELETE( pGridLabel ) ;
	}
	CGMUICustomListBox::ClearData() ;
}

VOID CRYLStringGrid::AttachTopImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paTopButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID CRYLStringGrid::DetachTopImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paTopButtonTexture[ StateType ] = NULL ;
}

VOID CRYLStringGrid::AttachBottomImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paBottomButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID CRYLStringGrid::DetachBottomImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paBottomButtonTexture[ StateType	] = NULL ;
}

VOID CRYLStringGrid::AttachTrackImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paTrackButtonTexture[ StateType ] = pGMTexture ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID CRYLStringGrid::DetachTrackImageButtonTexture( EListBoxButtonImageStateType StateType )
{
	m_paTrackButtonTexture[ StateType ] = NULL ;
}
VOID CRYLStringGrid::DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	CRYLGridLabel* pGridLabel = NULL;
	CGMUICustomListBox::GetData( Index, ( VOID ** )&pGridLabel ) ;

	if ( pGridLabel )
	{
		switch( State )
		{
			case  LB_SELECT :
				pGridLabel->SetViewFontColor( m_dwSelectedItemColor ) ;
				break ;
			case  LB_NONE   :
				pGridLabel->SetViewFontColor() ;
				break ;
		}
	}

	if ( pGridLabel )
	{
		pGridLabel->Paint( r.left, r.top ) ;
	}

	CGMUICustomListBox::DoDrawItem( Sender, Index, r, State ) ;
}

VOID CRYLStringGrid::DoTopDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;
	if ( !m_paTopButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP:
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_NORMAL;
				break;
			case BTN_STATE_DOWN:
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_DOWN;
				break;
			case BTN_STATE_ENTER:
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_UP;
				break;
			case BTN_STATE_DISIBLE:
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_DISIBLE;
				break;
		}

		m_paTopButtonTexture[ State ] = (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, DefaultCtrlType);
	}

	if (m_paTopButtonTexture[ State ])
		m_paTopButtonTexture[ State ]->Draw( r.left, r.top ) ;
	CGMUICustomListBox::DoTopDraw( Sender, r, State ) ;
}

VOID CRYLStringGrid::DoTrackDraw( CGMUIObject* Sender, RECT r, INT State)
{
	CRYLImageCabinet::EDefaultCtrlType	DefaultCtrlType ;
	
	if ( !m_paTrackButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TRACK_BUTTON_NORMAL ;
				break ;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TRACK_BUTTON_DOWN ;
				break ;
			case BTN_STATE_ENTER :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TRACK_BUTTON_UP ;
				break ;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TRACK_BUTTON_DISIBLE ; 
				break ;
		}

		m_paTrackButtonTexture[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}

	if ( m_paTrackButtonTexture[ State ] )
		m_paTrackButtonTexture[ State ]->Draw( r.left, r.top ) ;
	CGMUICustomListBox::DoTrackDraw( Sender, r, State ) ;
}

VOID CRYLStringGrid::DoBottomDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType	DefaultCtrlType ;
	
	if ( !m_paBottomButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_BOTTOM_BUTTON_NORMAL ;
				break ;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_BOTTOM_BUTTON_DOWN ;
				break ;
			case BTN_STATE_ENTER :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_BOTTOM_BUTTON_UP ;
				break ;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_BOTTOM_BUTTON_DISIBLE ;
				break ;
		}

		m_paBottomButtonTexture[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}

	if ( m_paBottomButtonTexture[ State ] )
		m_paBottomButtonTexture[ State ]->Draw( r.left, r.top ) ;
	CGMUICustomListBox::DoBottomDraw( Sender, r, State ) ;
}

VOID CRYLStringGrid::LoadTopImage( LPSTR pFileName )
{
	INT				w, x ;
	CGMImageDib*	pDib ;
	CGMImageDib*	pdestDib ;
	CRYLImage*		pImage ;

	pDib = new CGMImageDib() ;
	pDib->LoadFromFile( pFileName ) ;
	w = ( pDib->GetWidth () / 4 ) ;

	x = 0 ;
	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( w, pDib->GetHeight(), FALSE, EAIF_32Bit ) ;

	for ( INT i = 0 ; i < 4 ; i ++ )
	{
		pdestDib->ClearbyColor( 0x00000000 ) ;
		pdestDib->Draw( -x, 0, pDib ) ;
		x += w;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pdestDib ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

		switch ( i )
		{
			case 0 :
				AttachTopImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Up, pImage ) ;
				break ;
			case 1 :
				AttachTopImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachTopImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Disible, pImage ) ;
				break ;
			case 3 :
				AttachTopImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Enter, pImage ) ;
				break ;
		}
	}
	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID CRYLStringGrid::LoadBottomImage (char* pFileName)
{
	INT				w, x ;
	CGMImageDib*	pDib ;
	CGMImageDib*	pdestDib ;
	CRYLImage*		pImage ;

	pDib = new CGMImageDib() ;  
	pDib->LoadFromFile( pFileName ) ;
	w = ( pDib->GetWidth () / 4 ) ;

	x = 0 ;
	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( w, pDib->GetHeight(), FALSE, EAIF_32Bit ) ;

	for ( INT i = 0 ; i < 4 ; i ++ )
	{
		pdestDib->ClearbyColor( 0x00000000 ) ;
		pdestDib->Draw( -x, 0, pDib ) ;
		x += w ;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pdestDib)  ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

		switch( i )
		{
			case 0 :
				AttachBottomImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Up, pImage ) ;
				break  ;
			case 1 :
				AttachBottomImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachBottomImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Disible, pImage ) ;
				break ;
			case 3 :
				AttachBottomImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Enter, pImage ) ;
				break ;
		}
	}
	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID CRYLStringGrid::LoadTrackImage( LPSTR pFileName )
{
	INT				w, x ;
	CGMImageDib*	pDib ;
	CGMImageDib*	pdestDib ;
	CRYLImage*		pImage ;

	pDib = new CGMImageDib() ;
	pDib->LoadFromFile( pFileName ) ;
	w = ( pDib->GetWidth () / 4 ) ;

	x = 0 ;
	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( w, pDib->GetHeight(), FALSE, EAIF_32Bit ) ;

	for ( INT i = 0 ; i < 4 ; i ++ )
	{
		pdestDib->ClearbyColor( 0x00000000 ) ;
		pdestDib->Draw( -x, 0, pDib ) ;
		x += w ;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pdestDib ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

		switch( i )
		{
			case 0 :
				AttachTrackImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Up, pImage ) ;
				break ;
			case 1 :
				AttachTrackImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachTrackImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Disible, pImage ) ;
				break ;
			case 3 :
				AttachTrackImageButtonTexture( CRYLStringGrid::ButtonImageStateType_Enter, pImage ) ;
				break ;
		}
	}

	GM_DELETE( pdestDib );
	GM_DELETE( pDib ) ;  
}

VOID	CRYLStringGrid::AttachTopButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	INT iWidth  = lpRect->right - lpRect->left ;
	INT iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( iWidth, iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	m_paTopButtonTexture[ iInstance ] = pImage ;
	GM_DELETE( pdestDib ) ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID	CRYLStringGrid::AttachTrackButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	INT iWidth  = lpRect->right - lpRect->left ;
	INT iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( iWidth, iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	m_paTrackButtonTexture[ iInstance ] = pImage ;
	GM_DELETE( pdestDib ) ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}

VOID	CRYLStringGrid::AttachBottomButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	INT iWidth  = lpRect->right - lpRect->left ;
	INT iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( iWidth, iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	m_paBottomButtonTexture[ iInstance ] = pImage ;
	GM_DELETE( pdestDib ) ;
	CGMUICustomListBox::SetGMUICtrlSize() ;
}
