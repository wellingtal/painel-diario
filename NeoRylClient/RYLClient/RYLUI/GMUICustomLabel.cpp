#include "GMUICustomLabel.h"
#include "GMUICustomDlg.h"
#include "GMMemory.h"

//------------------------------------------------------------------------------
CGMUICustomLabel::CGMUICustomLabel( CGMUICustomDlg *pParentDlg ) : CGMUIObject()
{
	m_pGMUIParentDlg	= pParentDlg ;
	OnChangeEvent		= NULL ;
	OnDrawEvent			= NULL ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlLABEL ) ;
}

CGMUICustomLabel::~CGMUICustomLabel()
{
	OnChangeEvent	= NULL ;
	OnDrawEvent		= NULL ;
}
 
VOID CGMUICustomLabel::SetCaption( LPSTR pText )
{
	if ( strcmp( m_Caption, pText ) != 0 )
	{
		m_Caption = pText ;
		Change( this ) ;
	}
}

LPSTR CGMUICustomLabel::GetCaption()
{
	return m_Caption ;
}

VOID CGMUICustomLabel::Change( CGMUIObject* Sender )
{
	if ( OnChangeEvent )
	{
		OnChangeEvent( Sender ) ;
	}
}

VOID CGMUICustomLabel::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( OnDrawEvent )
	{
		OnDrawEvent (Sender, r, State);
	}
}

VOID CGMUICustomLabel::GMUIPaint( INT xx, INT yy )
{
	RECT r ;

	if ( !CGMUIObject::GetVisible() ) 
		return ;

	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () ) ;

	DoDraw( this, r, CGMUIObject::GetButtonState() ) ;

	CGMUIObject::GMUIPaint( xx, yy ) ;
}

