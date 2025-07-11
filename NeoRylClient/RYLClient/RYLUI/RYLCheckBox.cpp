#include "RYLCheckBox.h"
#include "RYLFontTexture.h"
#include "RYLImageManager.h"
#include "GMUIUtil.h"
#include "GMFont.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "GMMemory.h"

CRYLCheckBox::CRYLCheckBox( CGMUICustomDlg *pParentDlg ) : CGMUICheckBox( pParentDlg )
{
	m_pCaptionTexture	= NULL ;
	m_pCaption			= NULL ;
	m_iOffsetX			= 0 ;
	m_iOffsetY			= 0 ;
}

CRYLCheckBox::~CRYLCheckBox ()
{
	m_pCaption		= NULL ;
	m_iOffsetX		= 0 ;
	m_iOffsetY		= 0 ;
	
	GM_DELETE( m_pCaptionTexture ) ;
}

VOID CRYLCheckBox::DoDraw( CGMUIObject* Sender, RECT r,	INT State )
{
	INT iState = 0 ;
	if ( !m_bChecked )		
		iState = 0 ;
	else					
		iState = 1 ;

	if ( State == BTN_STATE_DISIBLE )
		iState = 2 ;

	if ( State == BTN_STATE_CHECKED_DISIBLE )
		iState = 4 ;

	if ( State == BTN_STATE_NCHECKED_DISIBLE )
		iState = 5 ;

	// 렌더링시 안드러올수있는 경우...
	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i ++ )
	{
		if ( m_paCheckBoxTexture[ i ] )
			m_paCheckBoxTexture[ i ]->SetVisible( FALSE ) ;
	}

	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;
	if ( !m_paCheckBoxTexture[ iState ] )
	{
		switch( iState )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_CHECKBOX_BUTTON_UP ;
				break;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_CHECKBOX_BUTTON_DOWN ;
				break;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_CHECKBOX_BUTTON_DISIBLE ;
		}
		m_paCheckBoxTexture[ iState ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
		m_iOffsetX = m_paCheckBoxTexture[ iState ]->GetWidth () + 2 ;
	}

	CGMUICheckBox::DoDraw( Sender, r, State ) ;
	
	if ( m_pCaptionTexture )
		m_pCaptionTexture->Draw( r.left+ m_iOffsetX, r.top+ m_iOffsetY ) ;
}

VOID CRYLCheckBox::SetVisible( BOOL bVisible )
{
	CGMUICheckBox::SetVisible( bVisible ) ;
	
	if ( m_pCaptionTexture )
		m_pCaptionTexture->SetVisible( bVisible ) ;
}

VOID CRYLCheckBox::SetCaption( LPSTR pCaption, BOOL bOffsetX, BOOL bOffsetY, DWORD dwFontColor )
{
	INT			w ;
	m_pCaption = pCaption ;

	if ( !m_pCaptionTexture )
		m_pCaptionTexture = new CRYLFontTexture() ;

	w = GetGMUITextWidth( m_pCaption ) ;
	m_pCaptionTexture->SetSize( w, 16 ) ;
	m_pCaptionTexture->SetText( m_pCaption ) ;
	m_pCaptionTexture->SetFontColor( dwFontColor ) ;

	if ( bOffsetX )
	{
		if ( m_paCheckBoxTexture[ CheckBoxImageStateType_Up ] )
			m_iOffsetX = (m_paCheckBoxTexture[ CheckBoxImageStateType_Up ]->GetWidth() - w ) / 2 ;
	}
	else
	{
		if ( m_paCheckBoxTexture[ CheckBoxImageStateType_Up ] )
			m_iOffsetX = m_paCheckBoxTexture[ CheckBoxImageStateType_Up ]->GetWidth() + 2 ;
	}

	if ( bOffsetY )
	{
		INT iHeight = GetGMUITextHeight( m_pCaption ) ;
		if ( m_paCheckBoxTexture[ CheckBoxImageStateType_Up ] )
			m_iOffsetY = ( m_paCheckBoxTexture[ CheckBoxImageStateType_Up ]->GetHeight() - iHeight ) / 2 ;
	}
	else
	{
		m_iOffsetY = GetGMUITextHeight( m_pCaption ) ;
		m_iOffsetY = ( m_iHeight - m_iOffsetY ) / 2 + 1;		
	}
	
	
}

LPSTR CRYLCheckBox::GetCaption ()
{
	return m_pCaption;
}

VOID CRYLCheckBox::LoadFromFile( LPSTR pFileName, BOOL boDefault )
{
	CGMImageDib* pDib;
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	pDib = new CGMImageDib() ;
	pDib->LoadFromFile( pFileName ) ;

	INT w = ( pDib->GetWidth () / GMUICHECKBOX_IMAGE_COUNT ) ;
	INT x = 0;

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( w, pDib->GetHeight(), FALSE, EAIF_32Bit ) ;

	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i ++ )
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
		

		m_paCheckBoxTexture[i] = pImage ;
	}
	m_iWidth = w;
	m_iHeight = pdestDib->GetHeight();

	GM_DELETE( pdestDib ) ;
	GM_DELETE( pDib ) ;
}

VOID	CRYLCheckBox::AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
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
	
	m_paCheckBoxTexture[ iInstance ] = pImage ;
	GM_DELETE( pdestDib ) ;
}

VOID	CRYLCheckBox::AttachButtonImage( CGMTexture* pTexture, INT iInstance ) 
{
	m_paCheckBoxTexture[ iInstance ] = pTexture ;
}

VOID	CRYLCheckBox::DefaultEvent( CRYLCheckBox* pCheckBox ) 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	SetRect( &rt, 181, 220, 197, 236 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	pCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
}