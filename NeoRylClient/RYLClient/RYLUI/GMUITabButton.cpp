#include "GMUITabButton.h"
#include "GMMemory.h"

CGMUITabButton::CGMUITabButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomTabButton( pParentDlg ) 
{
}

CGMUITabButton::~CGMUITabButton() 
{
}

VOID CGMUITabButton::AttachTabButtonTexture( ETabButtonImageStateType StateType, CGMTexture* pGMTexture ) 
{
}

VOID CGMUITabButton::DetachTabButtonTexture( ETabButtonImageStateType StateType ) 
{
}

VOID CGMUITabButton::DoDraw( CGMUIObject* Sender, RECT r, INT State ) 
{
	CGMUICustomTabButton::DoDraw( Sender, r, State ) ;
}

VOID CGMUITabButton::SetVisible( BOOL bVisible ) 
{
	CGMUICustomTabButton::SetVisible( bVisible ) ;

	/*
	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i ++ )
	{
		if ( m_paCheckBoxTexture[ i ] )
			m_paCheckBoxTexture[ i ]->SetVisible( bVisible ) ;
	}
	*/
}

