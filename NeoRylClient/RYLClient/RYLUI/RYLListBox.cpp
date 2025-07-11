#include "RYLListBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "GMTexture.h"
#include "GMImageDib.h"
#include "GMMemory.h"

CRYLListBox::CRYLListBox( CGMUICustomDlg *pParentDlg, BOOL bScrollEnable ) : CGMUIListBox( pParentDlg, bScrollEnable )
{
	m_pListBoxImage = NULL ;
}

CRYLListBox::~CRYLListBox ()
{
}

VOID CRYLListBox::DoTopDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;
	
	if ( !m_paTopButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_NORMAL ;
				break ;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_DOWN ;
				break ;
			case BTN_STATE_ENTER :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_UP ;
				break ;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_LISTBOX_TOP_BUTTON_DISIBLE ;
				break ;
		}
		m_paTopButtonTexture[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}

	CGMUIListBox::DoTopDraw( Sender, r, State ) ;
}

VOID CRYLListBox::DoTrackDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

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

	CGMUIListBox::DoTrackDraw( Sender, r, State ) ;
}

VOID CRYLListBox::DoBottomDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

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

	CGMUIListBox::DoBottomDraw( Sender, r, State ) ;
}

VOID CRYLListBox::LoadTopImage( LPSTR pFileName )
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
				AttachTopImageButtonTexture( CGMUIListBox::ButtonImageStateType_Normal, pImage ) ;
				break ;
			case 1 :
				AttachTopImageButtonTexture( CGMUIListBox::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachTopImageButtonTexture( CGMUIListBox::ButtonImageStateType_Up, pImage ) ;
				break ; 
			case 3 :
				AttachTopImageButtonTexture( CGMUIListBox::ButtonImageStateType_Disible, pImage ) ;
				break ;
		}
	}
	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID CRYLListBox::LoadBottomImage( LPSTR pFileName )
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
				AttachBottomImageButtonTexture( CGMUIListBox::ButtonImageStateType_Normal, pImage ) ;
				break ;
			case 1 :
				AttachBottomImageButtonTexture( CGMUIListBox::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachBottomImageButtonTexture( CGMUIListBox::ButtonImageStateType_Up, pImage ) ;
				break ;
			case 3 :
				AttachBottomImageButtonTexture( CGMUIListBox::ButtonImageStateType_Disible, pImage ) ;
				break ;
		}
	}
	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID CRYLListBox::LoadTrackImage( LPSTR pFileName )
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
				AttachTrackImageButtonTexture( CGMUIListBox::ButtonImageStateType_Normal, pImage ) ;
				break ;
			case 1 :
				AttachTrackImageButtonTexture( CGMUIListBox::ButtonImageStateType_Down, pImage ) ;
				break ;
			case 2 :
				AttachTrackImageButtonTexture( CGMUIListBox::ButtonImageStateType_Up, pImage ) ;
				break ;
			case 3 :
				AttachTrackImageButtonTexture( CGMUIListBox::ButtonImageStateType_Disible, pImage ) ;
				break ;
		}
	}
	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID	CRYLListBox::AttachTopButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
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
}

VOID	CRYLListBox::AttachTrackButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
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
}

VOID	CRYLListBox::AttachBottomButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
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
}

VOID	CRYLListBox::DoDraw( CGMUIObject* Sender, RECT r, INT State ) 
{
	if ( m_pListBoxImage )
		m_pListBoxImage->Draw( r.left, r.top ) ;

	CGMUIListBox::DoDraw( Sender, r, State ) ;
}