#include "GMUICustomTabButton.h"
#include "GMUICustomDlg.h"
#include "BaseDataDefine.h"

#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLImageManager.h"

#include "GMUIUtil.h"
#include "GMMemory.h"

CGMUICustomTabButton::CGMUICustomTabButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;	
	OnDrawTabButton = NULL ;
	m_iSXMargin = 0 ;
	m_iFocusTabButton = 0 ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlTabButton ) ;
}

CGMUICustomTabButton::~CGMUICustomTabButton() 
{
	FinalGMUICtrl() ;
	OnDrawTabButton = NULL ;
}

VOID	CGMUICustomTabButton::FinalGMUICtrl() 
{
	for ( std::vector< LPTABBUTTONINFO >::iterator iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++ iter )
	{
		GM_DELETE( ( *iter )->m_pTabButton ) ;
		GM_DELETE( ( *iter )->m_pTabLabel ) ;

		GM_DELETE( ( *iter ) ) ;
	}

	m_vecTabButton.clear() ;
}

VOID		CGMUICustomTabButton::SetTabVisible(INT iIndex, BOOL boVisible )
{
	INT nCnt = 0;

	std::vector< LPTABBUTTONINFO >::iterator iter ;

	m_iSXMargin = 0;

	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter, nCnt++)
	{
		LPTABBUTTONINFO pTabInfo = ( *iter );		

		if(nCnt==iIndex)
		{
			pTabInfo->m_pTabButton->SetVisible(boVisible);
			return;
		}
	}
}

VOID		CGMUICustomTabButton::RestoreTabButton( INT iIndex, LPCTSTR lpStr, INT iKind )
{		
	INT nCnt = 0;

	std::vector< LPTABBUTTONINFO >::iterator iter ;

	m_iSXMargin = 0;

	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter, nCnt++)
	{
		LPTABBUTTONINFO pTabInfo = ( *iter );

		pTabInfo->m_pTabButton->SetLeft( this->GetLeft() + m_iSXMargin );
				
		if(nCnt==iIndex)
		{
			RECT rt ;
			CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

			switch ( iKind )
			{
			case 2 :
				{
					SetRect( &rt, 0, 232, 42, 253 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
					SetRect( &rt, 84, 232, 126, 253 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
					SetRect( &rt, 42, 232, 84, 253 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
					SetRect( &rt, 0, 232, 42, 253 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
					
					m_iSXMargin += 42 ;
				}
				break ;
			case 3 :
				{
					SetRect( &rt, 0, 209, 54, 230 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
					SetRect( &rt, 108, 209, 162, 230 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
					SetRect( &rt, 54, 209, 108, 230 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
					SetRect( &rt, 0, 209, 54, 230 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
					m_iSXMargin += 54 ;
				}
				break ;
			case 4 :
				{
					SetRect( &rt, 0, 186, 66, 207 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
					SetRect( &rt, 132, 186, 198, 207 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
					SetRect( &rt, 66, 186, 132, 207 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
					SetRect( &rt, 0, 186, 66, 207 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
					m_iSXMargin += 66 ;
				}
				break ;
			case 5 :
				{
					SetRect( &rt, 0, 163, 78, 184 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
					SetRect( &rt, 156, 163, 234, 184 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
					SetRect( &rt, 78, 163, 156, 184 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
					SetRect( &rt, 0, 163, 78, 184 ) ;
					pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
					m_iSXMargin += 78 ;
				}
				break ;
			}

			pTabInfo->m_pTabButton->SetCaption( ( CHAR* )lpStr, TRUE ) ;			
		}	
		else
		{
			m_iSXMargin += pTabInfo->m_pTabButton->GetWidth();
		}
	}		
}

VOID		CGMUICustomTabButton::AttachTabButton( LPCTSTR lpStr, INT iKind ) 
{
	if(!lpStr)
	{
		return;
	}

	LPTABBUTTONINFO pTabInfo = new TABBUTTONINFO ;
	
	pTabInfo->m_pTabButton = new CTabButton( m_pGMUIParentDlg)  ;
	pTabInfo->m_pTabButton->SetParent( this ) ;
	pTabInfo->m_pTabButton->SetLeft( this->GetLeft() + m_iSXMargin );
		
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	if(!pSprite)
	{
		if(pTabInfo)
		{
			if(pTabInfo->m_pTabButton)
			{
				delete pTabInfo->m_pTabButton;
			}

			delete pTabInfo;					
		}
		return;
	}

	switch ( iKind )
	{
	case 2 :
		{
			SetRect( &rt, 0, 232, 42, 253 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
			SetRect( &rt, 84, 232, 126, 253 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
			SetRect( &rt, 42, 232, 84, 253 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
			SetRect( &rt, 0, 232, 42, 253 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
			m_iSXMargin += 42 ;
		}
		break ;
	case 3 :
		{
			SetRect( &rt, 0, 209, 54, 230 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
			SetRect( &rt, 108, 209, 162, 230 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
			SetRect( &rt, 54, 209, 108, 230 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
			SetRect( &rt, 0, 209, 54, 230 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
			m_iSXMargin += 54 ;
		}
		break ;
	case 4 :
		{
			SetRect( &rt, 0, 186, 66, 207 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
			SetRect( &rt, 132, 186, 198, 207 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
			SetRect( &rt, 66, 186, 132, 207 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
			SetRect( &rt, 0, 186, 66, 207 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
			m_iSXMargin += 66 ;
		}
		break ;
	case 5 :
		{
			SetRect( &rt, 0, 163, 78, 184 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
			SetRect( &rt, 156, 163, 234, 184 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
			SetRect( &rt, 78, 163, 156, 184 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
			SetRect( &rt, 0, 163, 78, 184 ) ;
			pTabInfo->m_pTabButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
			m_iSXMargin += 78 ;
		}
		break ;
	}
	

	pTabInfo->m_pTabButton->SetCaption( ( CHAR* )lpStr, TRUE ) ;
	
	CGMUICustomPanel::AddChild( pTabInfo->m_pTabButton ) ;

	m_vecTabButton.push_back( pTabInfo ) ;
	
	if ( !m_vecTabButton.empty() )
	{
		m_vecTabButton[ 0 ]->m_pTabButton->SetChecked( TRUE ) ;
	}
}

VOID		CGMUICustomTabButton::AttachTabButton( INT ISpriteIndex,
												   LPRECT lprtEnable,
												   LPRECT lprtDisable ) 
{
	LPTABBUTTONINFO pTabInfo = new TABBUTTONINFO ;
	
	pTabInfo->m_pTabButton = new CTabButton( m_pGMUIParentDlg)  ;
	pTabInfo->m_pTabButton->SetParent( this ) ;
	pTabInfo->m_pTabButton->SetWidth( lprtEnable->right - lprtEnable->left ) ;
	pTabInfo->m_pTabButton->SetHeight( lprtEnable->bottom - lprtEnable->top ) ;
	pTabInfo->m_pTabButton->SetLeft( this->GetLeft() + m_iSXMargin );
		
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ISpriteIndex ) ;

	pTabInfo->m_pTabButton->AttachButtonImage( pSprite, lprtDisable, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	pTabInfo->m_pTabButton->AttachButtonImage( pSprite, lprtEnable, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	pTabInfo->m_pTabButton->AttachButtonImage( pSprite, lprtEnable, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	pTabInfo->m_pTabButton->AttachButtonImage( pSprite, lprtDisable, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_iSXMargin = lprtEnable->right - lprtEnable->left ;

	CGMUICustomPanel::AddChild( pTabInfo->m_pTabButton ) ;
	m_vecTabButton.push_back( pTabInfo ) ;
	
	if ( !m_vecTabButton.empty() )
	{
		m_vecTabButton[ 0 ]->m_pTabButton->SetChecked( TRUE ) ;
	}
}

VOID		CGMUICustomTabButton::DetachTabButton( INT iIndex ) 
{
	INT nCnt = 0;

	std::vector< LPTABBUTTONINFO >::iterator iter ;
	
	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter, nCnt++)
	{
		LPTABBUTTONINFO pTabInfo = ( *iter );

		if(nCnt==iIndex)
		{
			if(pTabInfo)
			{
				CGMUICustomPanel::DeleteChild( pTabInfo->m_pTabButton ) ;

				m_vecTabButton.erase( iter ) ;

				delete pTabInfo;
				pTabInfo = NULL;
				return;
			}
		}
	}
}

VOID	CGMUICustomTabButton::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )
	{
		return ;
	}
	
	CGMUICustomPanel::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID	CGMUICustomTabButton::SetSize( CGMUIObject* Sender ) 
{

}

VOID	CGMUICustomTabButton::GMUIPaint( INT xx, INT yy )
{
	RECT r ;

	if ( !CGMUIObject::GetVisible() ) 
		return ;

	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () ) ;

	DoDraw( this, r, CGMUIObject::GetButtonState() ) ;

	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
}

VOID	CGMUICustomTabButton::DoDraw( CGMUIObject* Sender, RECT r, INT State ) 
{
}

VOID	CGMUICustomTabButton::TabButtonUp() 
{
	if ( m_vecTabButton.empty() ) return ;

	std::vector< LPTABBUTTONINFO >::iterator iter ;

	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter )
	{
//		if ( ( *iter )->m_pTabButton->GetChecked() )
//		{
			( *iter )->m_pTabButton->SetChecked( FALSE ) ;
//		}
	}
}

INT		CGMUICustomTabButton::GetFocusTabButton()	
{
	INT index = 0 ;
	std::vector< LPTABBUTTONINFO >::iterator iter ;
	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter, index ++ )
	{
		if ( ( *iter )->m_pTabButton->GetChecked() )
		{
			m_iFocusTabButton = index ;
		}
	}

	return m_iFocusTabButton ;
}

VOID	CGMUICustomTabButton::SetFocusTabButton( INT iIndex )	
{
	INT index = 0 ;
	std::vector< LPTABBUTTONINFO >::iterator iter ;
	for ( iter = m_vecTabButton.begin() ; iter != m_vecTabButton.end() ; ++iter, index ++ )
	{
		( *iter )->m_pTabButton->SetChecked( FALSE ) ;
	}
	
	m_vecTabButton[ iIndex ]->m_pTabButton->SetChecked( TRUE ) ;
	m_iFocusTabButton = index ;
}

//=============================================================================================================
CTabButton::CTabButton( CGMUICustomDlg *pParentDlg ) : CRYLCheckBox( pParentDlg )
{
	m_pGMUICustomTabButton = NULL ;
}

CTabButton::~CTabButton() 
{
}

VOID	CTabButton::SetParent( LPVOID pGMUIObject ) 
{
	m_pGMUICustomTabButton = ( CGMUICustomTabButton * )pGMUIObject ;
}


VOID	CTabButton::GetParent( VOID **ppGMUIObject ) 
{
	*ppGMUIObject = ( CGMUIObject * )m_pGMUICustomTabButton ;
}

VOID	CTabButton::SetSize( CGMUIObject* Sender ) 
{
	if ( m_pGMUICustomTabButton != NULL )
	{
		m_pGMUICustomTabButton->SetSize( this ) ;
	}
}

VOID	CTabButton::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( m_pGMUICustomTabButton != NULL )
	{
		if ( Button == MB_LEFTCLICK && 0 == InArea( GetLeft(), GetTop(), GetWidth(), GetHeight(), X, Y ) )  
		{
			m_pGMUICustomTabButton->TabButtonUp() ;
		}
	}

	CGMUICustomCheckBox::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID	CTabButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	/*
	if ( m_pGMUICustomTabButton != NULL )
	{
		m_pGMUICustomTabButton->TabButtonDraw( Sender, r, State ) ;
	}
	*/

	CRYLCheckBox::DoDraw( Sender, r, State ) ;
}

VOID	CTabButton::AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance )
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

//===========================================================================================================
CTabLabel::CTabLabel( CGMUICustomDlg *pParentDlg ) : CRYLLabel( pParentDlg )
{
	m_pGMUICustomTabButton = NULL ;
}

CTabLabel::~CTabLabel() 
{
}

VOID	CTabLabel::SetParent( LPVOID pGMUIObject ) 
{
	m_pGMUICustomTabButton = ( CGMUICustomTabButton * )pGMUIObject ;
}

VOID	CTabLabel::GetParent( VOID **ppGMUIObject ) 
{
	*ppGMUIObject = ( CGMUIObject * )m_pGMUICustomTabButton ;
}

VOID	CTabLabel::SetSize( CGMUIObject* Sender ) 
{
	if ( m_pGMUICustomTabButton != NULL )
	{
		m_pGMUICustomTabButton->SetSize( this ) ;
	}
}

VOID	CTabLabel::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( m_pGMUICustomTabButton != NULL )
	{
		if ( Button == MB_LEFTCLICK )  
		{
			m_pGMUICustomTabButton->TabButtonUp() ;
		}
	}

	CGMUICustomLabel::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID	CTabLabel::DoDraw( CGMUIObject* Sender, RECT r, INT State ) 
{
	CRYLLabel::DoDraw( Sender, r, State ) ;
}
