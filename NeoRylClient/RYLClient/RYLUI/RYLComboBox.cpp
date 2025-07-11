#include "RYLComboBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMTexture.h"
#include "GMImageDib.h"
#include "RYLImageManager.h"
#include "GMMemory.h"

CRYLComboBox::CRYLComboBox( CGMUICustomDlg *pParentDlg ) : CGMUIComboBox( pParentDlg )
{
	m_pImage	= NULL ;
	m_dwColor	= 0xC4000000 ;
}

CRYLComboBox::~CRYLComboBox()
{
	m_pImage = NULL;
}

VOID CRYLComboBox::DefaultDraw ()
{
	m_pImage = CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, m_dwColor ) ;
}

VOID CRYLComboBox::DoBtnDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType	DefaultCtrlType ;

	if ( !m_paButtonTexture[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_COMBOBOX_BUTTON_UP ;
				break ;
			case BTN_STATE_DOWN:
				DefaultCtrlType = CRYLImageCabinet::EDCT_COMBOBOX_BUTTON_DOWN ;
				break ;
			case BTN_STATE_DISIBLE:
				DefaultCtrlType = CRYLImageCabinet::EDCT_COMBOBOX_BUTTON_DISIBLE ;
				break ;
			case BTN_STATE_ENTER:
				DefaultCtrlType = CRYLImageCabinet::EDCT_COMBOBOX_BUTTON_ENTER ;
				break ;
		}

		m_paButtonTexture[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}
	
	if ( m_paButtonTexture[ State ] )
		m_paButtonTexture[ State ]->SetVisible( TRUE ) ;
	
	CGMUIComboBox::DoBtnDraw( Sender, r, State ) ;
}

VOID CRYLComboBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( !m_pImage )
		DefaultDraw() ;

	if ( m_pImage )
		m_pImage->Draw( r.left, r.top ) ;

	CGMUIComboBox::DoDraw( Sender, r, State ) ;
}

VOID	CRYLComboBox::AttachButtonImageEx( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
{
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	m_iWidth  = lpRect->right - lpRect->left ;
	m_iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( m_iWidth, m_iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, pDib ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	m_paButtonTexture[ iInstance ] = pImage ;
	GM_DELETE( pdestDib ) ;
}