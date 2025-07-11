#include "GMUIButton.h"
#include "GMTexture.h"
#include "GMUITooltipManager.h"
#include "GMMemory.h"

CGMUIButton::CGMUIButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomButton( pParentDlg )
{
	m_bAutoSize = FALSE ;
	
	for ( INT i = 0 ;i < GMUIButton_Image_Count ; i ++ )
	{
		m_paButtonTexture[ i ] = NULL ;
	}
}
 
CGMUIButton::~CGMUIButton()
{
	for ( INT i = 0 ;i < GMUIButton_Image_Count ; i ++ )
	{
		m_paButtonTexture[ i ] = NULL ;
	}
}

VOID CGMUIButton::AttachButtonTexture( EButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	m_paButtonTexture[ StateType ] = pGMTexture ;
}

VOID CGMUIButton::DetachButtonTexture( EButtonImageStateType StateType )
{
	m_paButtonTexture[ StateType ] = NULL ;
}

BOOL CGMUIButton::GetIsButtonTexture( EButtonImageStateType StateType )
{
	return m_paButtonTexture[ StateType ] != NULL ;
}

VOID CGMUIButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_paButtonTexture[ State ] )
	{
		m_paButtonTexture[ State ]->Draw( r.left, r.top ) ;
	}

	CGMUICustomButton::DoDraw( Sender, r, State ) ;
}

VOID CGMUIButton::SetVisible( BOOL bVisible )
{
	CGMUICustomButton::SetVisible( bVisible ) ;
	
	for ( INT i= 0 ; i < GMUIButton_Image_Count ; i ++ )
	{
		if ( m_paButtonTexture[ i ] )
			m_paButtonTexture[ i ]->SetVisible( bVisible ) ;
	}
}

BOOL CGMUIButton::DoAutoSize()
{
	if ( m_bAutoSize )
	{
		if (m_paButtonTexture[ ButtonImageStateType_Up ] )
		{
			m_iWidth  = m_paButtonTexture[ ButtonImageStateType_Up ]->GetWidth() ; 	
			m_iHeight = m_paButtonTexture[ ButtonImageStateType_Up ]->GetHeight() ;	
			return TRUE ;
		}
	}
	return FALSE ;
}

VOID CGMUIButton::SetAutoSize( BOOL bAutoSize )
{
	if ( m_bAutoSize != bAutoSize )
	{
		m_bAutoSize = bAutoSize ;
		DoAutoSize() ;
	}
}

BOOL CGMUIButton::GetAutoSize()
{
	return m_bAutoSize ;
}


