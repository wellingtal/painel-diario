#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "GMTexture.h"
#include "GMImageDib.h"
#include "GMMemory.h"

CRYLButton::CRYLButton( CGMUICustomDlg *pParentDlg ) : CGMUICaptionButton( pParentDlg )
{
}

CRYLButton::~CRYLButton()
{
}

VOID CRYLButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

	if ( !m_paButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP:
				DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_NORMAL;
				break;
			case BTN_STATE_DOWN:
				DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_DOWN;
				break;
			case BTN_STATE_ENTER:
				DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_UP;
				break;
			case BTN_STATE_DISIBLE:
				DefaultCtrlType = CRYLImageCabinet::EDCT_BUTTON_DISIBLE;
				break;
		}

		m_paButtonTexture[State] = (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, DefaultCtrlType);
		m_iWidth = m_paButtonTexture[State]->GetWidth ();
		m_iHeight = m_paButtonTexture[State]->GetHeight ();
	}

	CGMUICaptionButton::DoDraw (Sender, r, State);
}

VOID CRYLButton::LoadFromFile( LPSTR pFileName, BOOL boDefault )
{
	CGMImageDib* pDib;
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	pDib = new CGMImageDib() ;
	pDib->LoadFromFile( pFileName ) ;

	INT w = ( pDib->GetWidth () / GMUIButton_Image_Count ) ;
	INT x = 0;

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( w, pDib->GetHeight(), FALSE, EAIF_32Bit ) ;

	for ( INT i = 0 ; i < GMUIButton_Image_Count ; i ++ )
	{
		pdestDib->ClearbyColor( 0x00000000 ) ;
		pdestDib->Draw( -x, 0, pDib ) ;
		x += w ;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pdestDib ) ;

		if ( boDefault )
		{
			switch( i )
			{
				case 0 :	pImage->SetInstance( CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;				break ;
				case 1 :	pImage->SetInstance( CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;				break ;
				case 2 :	pImage->SetInstance( CRYLImageCabinet::EDCT_BUTTON_UP ) ;			break ;
				case 3 :	pImage->SetInstance( CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;			break ;
			}
			
		}
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
		

		m_paButtonTexture[i] = pImage ;
	}
	m_iWidth = w;
	m_iHeight = pdestDib->GetHeight();

	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID	CRYLButton::AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	if(iInstance>=GMUIButton_Image_Count)
	{
		return;
	}

	CGMImageDib*	pdestDib	= NULL;
	CRYLImage*		pImage		= NULL;
	
	m_iWidth  = lpRect->right - lpRect->left ;
	m_iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;

	if(pdestDib)
	{
		pdestDib->SetSize( m_iWidth, m_iHeight, FALSE, EAIF_32Bit ) ;
		pdestDib->ClearbyColor( 0x00000000 ) ;
		pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;
	}

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;

	if(pImage)
	{
		pImage->LoadFromImage( pdestDib ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
		m_paButtonTexture[ iInstance ] = pImage ;
	}
	GM_DELETE( pdestDib ) ;
}