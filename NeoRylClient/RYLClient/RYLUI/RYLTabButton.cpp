#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "GMTexture.h"
#include "GMImageDib.h"
#include "GMMemory.h"

CRYLTabButton::CRYLTabButton( CGMUICustomDlg *pParentDlg ) : CGMUITabButton( pParentDlg )
{
}

CRYLTabButton::~CRYLTabButton()
{
}

VOID CRYLTabButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	/*
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;
	
	for ( std::vector< LPTABBUTTONINFO >::iterator iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++ iter )
	{
		if ( !( *iter )->m_pTabButton->m_paCheckBoxTexture[ State ] )
		{
			switch( State )
			{
				case BTN_STATE_UP :
					DefaultCtrlType = CRYLImageCabinet::EDCT_TAB_BUTTON_OPEN ;
					break ;
				case BTN_STATE_DOWN :
					DefaultCtrlType = CRYLImageCabinet::EDCT_TAB_BUTTON_OPEN ;
					break ;
				case BTN_STATE_ENTER :
					DefaultCtrlType = CRYLImageCabinet::EDCT_TAB_BUTTON_CLOSE ;
					break ;
				case BTN_STATE_DISIBLE :
					DefaultCtrlType = CRYLImageCabinet::EDCT_TAB_BUTTON_CLOSE ;
					break ;
			}

			( *iter )->m_pTabButton->m_paCheckBoxTexture[ State ] = ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ;
		}
	}
	*/
	CGMUITabButton::DoDraw( Sender, r, State ) ;
}
