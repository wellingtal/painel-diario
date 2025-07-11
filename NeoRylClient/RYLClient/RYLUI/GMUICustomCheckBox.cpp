#include "GMUICustomCheckBox.h"
#include "GMUIUtil.h"
#include "RYLDialog.h"
#include "GMMemory.h"
 
CGMUICustomCheckBox::CGMUICustomCheckBox( CGMUICustomDlg *pParentDlg ) : CGMUIObject()
{
	m_pGMUIParentDlg = pParentDlg ;	
	CGMUIObject::SetWidth( 80 ) ;
	CGMUIObject::SetHeight( 16 ) ; 

	m_bChecked		= FALSE ;
	OnChangeEvent	= NULL ;
	OnDrawEvent		= NULL ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlCHECKBOX ) ;
}

CGMUICustomCheckBox::~CGMUICustomCheckBox()
{
	OnChangeEvent	= NULL ;
	OnDrawEvent		= NULL ;
}

BOOL CGMUICustomCheckBox::GetChecked()
{
	return m_bChecked ;
}

VOID CGMUICustomCheckBox::SetChecked( BOOL bChecked )
{
	m_bChecked = bChecked ;
	Change( this ) ;
}

VOID CGMUICustomCheckBox::Change( CGMUIObject* Sender )
{
	if ( OnChangeEvent != NULL )
	{
		OnChangeEvent( Sender ) ;
	}
}

VOID CGMUICustomCheckBox::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( Button == MB_RIGHTCLICK )
		return;

	if ( !CGMUIObject::GetEnable() )
		return ;

	INT itmpY = Y ;
	if ( this->GetSecondParent() )
	{
		if ( this->GetSecondParent()->GetGMUICtrlType() == GMUICtrlPanel )
			itmpY = Y - 21 ;
	}

	if ( ( ( CRYLDialog* )this->GetParentDlg() )->GetFormType() == CRYLDialog::ERYLDLG_TYPE::RYLDLG_FRAME_TYPE_NONE )
	{
		itmpY += 15 ;
	}
	
	if ( InArea( GetLeft(), GetTop(), GetWidth(), GetHeight(), X, itmpY ) == 0 )
	{
		m_bChecked = !m_bChecked ;
		Change( this ) ;
	}
	CGMUIObject::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID CGMUICustomCheckBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( OnDrawEvent != NULL )
	{
		OnDrawEvent( Sender, r, State ) ;
	}
}

VOID CGMUICustomCheckBox::GMUIPaint( INT xx, INT yy )
{
	if ( !CGMUIObject::GetVisible() ) 
		return ;

	RECT r ;
	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth(), yy + CGMUIObject::GetHeight() ) ;

	DoDraw( this, r, CGMUIObject::GetButtonState () ) ;

	CGMUIObject::GMUIPaint(xx, yy);
}

 