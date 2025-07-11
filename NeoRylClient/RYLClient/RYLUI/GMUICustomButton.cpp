#include "GMUICustomDlg.h"
#include "GMUICustomButton.h"
#include "GMMemory.h"

 
//------------------------------------------------------------------------------
//					CGMUICustomButton : public CGMUIObject
//------------------------------------------------------------------------------
CGMUICustomButton::CGMUICustomButton( CGMUICustomDlg *pParentDlg ) : CGMUIObject()
{
	m_pGMUIParentDlg = pParentDlg ;
	CGMUIObject::SetWidth( 80 ) ;
	CGMUIObject::SetHeight( 16 ) ;
	CGMUIObject::SetLeft( 0 ) ;
	CGMUIObject::SetTop( 0 ) ;

	OnDrawEvent = NULL ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlBUTTON ) ;
}

CGMUICustomButton::~CGMUICustomButton()
{
	OnDrawEvent = NULL ;
}

VOID CGMUICustomButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( OnDrawEvent != NULL )
	{
		OnDrawEvent( Sender, r, State ) ;
	}
}

VOID CGMUICustomButton::GMUIPaint( INT xx, INT yy )
{
	CGMUIObject::GMUIPaint( xx, yy ) ;

	RECT r;
	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () ) ;

	if (! CGMUIObject::GetVisible())		return ;

	DoDraw( this, r, CGMUIObject::GetButtonState() ) ;
}

