#include "GMUICheckBox.h"
#include "GMTexture.h"
#include "GMMemory.h"

CGMUICheckBox::CGMUICheckBox( CGMUICustomDlg *pParentDlg ) : CGMUICustomCheckBox( pParentDlg )
{
	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i++ )
	{
		m_paCheckBoxTexture[ i ] = NULL ;
	}
}
 
CGMUICheckBox::~CGMUICheckBox()
{
	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i ++ )
	{
		m_paCheckBoxTexture[ i ] = NULL ;
	}
}

VOID CGMUICheckBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	INT iState = 0 ;
	
	if ( !m_bChecked )		iState = 0 ;
	else					iState = 1 ;

	if ( State == BTN_STATE_DISIBLE )
		iState = 2 ;

	if ( State == BTN_STATE_CHECKED_DISIBLE )
		iState = 4 ;

	if ( State == BTN_STATE_NCHECKED_DISIBLE )
		iState = 5 ;

	if ( m_paCheckBoxTexture[ iState ] )
	{
		m_paCheckBoxTexture[ iState ]->SetVisible( TRUE ) ;
		m_paCheckBoxTexture[ iState ]->Draw( r.left, r.top ) ;
	}
	CGMUICustomCheckBox::DoDraw( Sender, r, iState ) ;
}

VOID CGMUICheckBox::SetVisible( BOOL bVisible )
{
	CGMUICustomCheckBox::SetVisible( bVisible ) ;

	for ( INT i = 0 ; i < GMUICHECKBOX_IMAGE_COUNT ; i ++ )
	{
		if ( m_paCheckBoxTexture[ i ] )
			m_paCheckBoxTexture[ i ]->SetVisible( bVisible ) ;
	}
}

