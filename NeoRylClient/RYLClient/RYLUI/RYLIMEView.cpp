#include "RYLImeView.h"

#include "GMUIScreen.h"
#include "RYLImage.h"
#include "RYLFontTexture.h"
#include "GMFont.h"

#include "RYLImageManager.h"
#include "GMMemory.h"

CRYLIMEView::CRYLIMEView() : CGMUICustomIMEView()
{
	m_pBackImage = NULL ;

	for ( INT i = 0 ; i < IMEVIEW_BUTTON_COUNT ; i ++ )
	{
		m_paLeftButtonImage[ i ] = NULL ;
		m_paRightButtonImage[ i ] = NULL ;
	}
	for ( INT i = 0 ; i < IMEVIEW_VIEWFONT_COUNT ; i ++ )
	{
		CGMFont::Instance()->CreateFontTexture( ( CGMFontTexture**)&m_paFontTexture[ i ] ) ;
		m_paFontTexture[ i ]->SetSize( 32, 32 ) ;
	}
}

CRYLIMEView::~CRYLIMEView()
{
	m_pBackImage = NULL ;

	for ( INT i = 0 ; i < IMEVIEW_BUTTON_COUNT ; i ++ )
	{
		m_paLeftButtonImage[ i ] = NULL ;
		m_paRightButtonImage[ i ] = NULL ;
	}
	for ( INT i = 0 ; i < IMEVIEW_VIEWFONT_COUNT ; i ++ )
	{
		GM_DELETE( m_paFontTexture[ i ] ) ;
	}
}

VOID CRYLIMEView::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_pBackImage )
	{
		m_pBackImage->Draw( r.left, r.top ) ;
	}

	CGMUICustomIMEView::DoDraw( Sender, r, State ) ;
}

VOID CRYLIMEView::DoLeftButtonDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

	if ( !m_paLeftButtonImage[ State ] )
	{
		switch ( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_LEFT_BUTTON_UP ;
				break ;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_LEFT_BUTTON_DOWN ;
				break ;
			case BTN_STATE_ENTER :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_LEFT_BUTTON_ENTER ;
				break ;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_LEFT_BUTTON_DISIBLE ;
				break ;
		}

		m_paLeftButtonImage[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}


	if ( m_paLeftButtonImage[ State ] )
		m_paLeftButtonImage[ State ]->Draw( r.left, r.top ) ;

	CGMUICustomIMEView::DoLeftButtonDraw( Sender, r, State ) ;
}

VOID CRYLIMEView::DoRightButtonDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

	if ( !m_paRightButtonImage[ State ] )
	{
		switch( State )
		{
			case BTN_STATE_UP :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_RIGHT_BUTTON_UP ;
				break ;
			case BTN_STATE_DOWN :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_RIGHT_BUTTON_DOWN ;
				break ;
			case BTN_STATE_ENTER :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_RIGHT_BUTTON_ENTER ;
				break ;
			case BTN_STATE_DISIBLE :
				DefaultCtrlType = CRYLImageCabinet::EDCT_IMEVIEW_RIGHT_BUTTON_DISIBLE ;
				break ;
		}

		m_paRightButtonImage[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
	}

	if ( m_paRightButtonImage[ State ] )
		m_paRightButtonImage[ State ]->Draw( r.left, r.top ) ;

	CGMUICustomIMEView::DoRightButtonDraw( Sender, r, State ) ;
}

VOID CRYLIMEView::DoDrawImeChar( CGMUIObject* Sender, INT Index, RECT r, LPSTR pText, INT State )
{
	CGMUICustomIMEView::DoDrawImeChar( Sender, Index, r, pText, State ) ;

   switch( State )
	{
		case LB_NONE :
         CGMFont::Instance()->DrawText( 0, r.left, r.top, pText, 0xffffffff );
			break ;
		case LB_SELECT :
         CGMFont::Instance()->DrawText( 0, r.left, r.top, pText, 0xFF00ffff ) ;
			break ;
	}

	// 그냥 그려두 되지만..우선은 텍스쳐로 그린다.

	/*n = Index % IMEVIEW_VIEWFONT_COUNT ;

	switch( State )
	{
		case LB_NONE :
			m_paFontTexture[ n ]->SetFontColor( 0xFF000000 ) ;
			break ;
		case LB_SELECT :
			m_paFontTexture[ n ]->SetFontColor( 0xFF0000FF ) ;
			break ;
	}
	ptmp = m_paFontTexture[ n ]->GetText() ;
	
	if ( ptmp == NULL )
	{
		m_paFontTexture[ n ]->SetText( pText ) ;
	}
	else
	{
		if ( strcmp( ptmp, pText ) != 0 )
			m_paFontTexture[n]->SetText( pText ) ;
	}
	
	m_paFontTexture[ n ]->Draw( r.left, r.top ) ;*/
}

VOID CRYLIMEView::DoLeftButtonClick( CGMUIObject* Sender, INT X, INT Y )
{
	CGMUICustomIMEView::DoLeftButtonClick( Sender, X, Y ) ;
}

VOID CRYLIMEView::DoRightButtonClick( CGMUIObject* Sender, INT X, INT Y )
{
	CGMUICustomIMEView::DoRightButtonClick( Sender, X, Y ) ;
}

VOID CRYLIMEView::DoCharClick( CGMUIObject* Sender, LPSTR aText, INT Index, INT X, INT Y )
{
	CGMUICustomIMEView::DoCharClick( Sender, aText, Index, X, Y ) ;
}

VOID CRYLIMEView::AttachLeftButtonTexture( EImeViewButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paLeftButtonImage[ StateType ] = pGMTexture ;
}

VOID CRYLIMEView::DetachLeftButtonTexture( EImeViewButtonImageStateType StateType )
{
	m_paLeftButtonImage[ StateType ] = NULL ;
}

VOID CRYLIMEView::AttachRightButtonTexture( EImeViewButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paRightButtonImage[ StateType ] = pGMTexture ;
}

VOID CRYLIMEView::DetachRightButtonTexture( EImeViewButtonImageStateType StateType )
{
	m_paRightButtonImage[ StateType ] = NULL ;
}

VOID CRYLIMEView::AttachBackTexture( CGMTexture* pGMTexture )
{
	m_pBackImage = pGMTexture ;
}

VOID CRYLIMEView::DetachBackTexture ()
{
	m_pBackImage = NULL ;
}


