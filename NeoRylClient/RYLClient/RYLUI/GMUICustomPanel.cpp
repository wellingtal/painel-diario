#include "GMUICustomPanel.h"
#include "GMUICustomDlg.h"
#include "GMUIUtil.h"
#include "GMTexture.h"
#include "GMMemory.h"

 
CGMUICustomPanel::CGMUICustomPanel( CGMUICustomDlg *pParentDlg ) : CGMUIObject ()
{
	m_pGMUIParentDlg = pParentDlg ;
	m_pPanelImage    = NULL ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlPanel ) ;
}

CGMUICustomPanel::~CGMUICustomPanel()
{
	m_ChildVector.clear() ;
}

VOID CGMUICustomPanel::Click( CGMUIObject* Sender, INT X, INT Y )
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) || ( !m_pGMUIParentDlg ) )
		return;

//	if ( Sender->GetGMUICtrlType() == GMUICtrlPanel )
//	{
		CGMUIObject *pGMUICtrl ;
		for ( int i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
		{
			pGMUICtrl = m_ChildVector[ i ] ;
			
			if ( pGMUICtrl->GetVisible() )
			{
				if ( InArea( pGMUICtrl->GetLeft(), pGMUICtrl->GetTop(), pGMUICtrl->GetWidth(), pGMUICtrl->GetHeight(), X, Y ) == 0 )
				{
					if ( ( Sender->GetGMUICtrlType() == GMUICtrlTabButton ) &&
						 ( pGMUICtrl->GetGMUICtrlType() == GMUICtrlCHECKBOX ) )
						CGMUIObject::Click( this, X, Y ) ;
					else
						pGMUICtrl->Click( pGMUICtrl, X - pGMUICtrl->GetLeft(), Y - pGMUICtrl->GetTop() ) ;
					return ;
				}
			}
		}
//	}
//	else
		CGMUIObject::Click( this, X, Y ) ;
}

VOID CGMUICustomPanel::DblClick (CGMUIObject* Sender, int Button, int Shift, int X, int Y) 
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) || ( !m_pGMUIParentDlg ) )
		return;

	CGMUIObject *pGMUICtrl ;
	for ( int i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
	{
		pGMUICtrl = m_ChildVector[ i ] ;

		if ( pGMUICtrl->GetVisible() )
		{
			if ( InArea( pGMUICtrl->GetLeft(), pGMUICtrl->GetTop(), pGMUICtrl->GetWidth(), pGMUICtrl->GetHeight(), X, Y ) == 0 )
			{
				if ( ( Sender->GetGMUICtrlType() == GMUICtrlTabButton ) &&
					( pGMUICtrl->GetGMUICtrlType() == GMUICtrlCHECKBOX ) )
					CGMUIObject::DblClick( this, Button, Shift, X, Y ) ;
				else
					pGMUICtrl->DblClick( pGMUICtrl, Button, Shift, X - pGMUICtrl->GetLeft(), Y - pGMUICtrl->GetTop() ) ;
				return ;
			}
		}
	}
	
	CGMUIObject::DblClick( this, Button, Shift, X, Y ) ;
}

VOID CGMUICustomPanel::AddChild( CGMUIObject *pGMUICtrl )
{
	m_ChildVector.insert( m_ChildVector.end(), pGMUICtrl ) ;
}

VOID CGMUICustomPanel::AddChildEx( CGMUIObject *pGMUICtrl )
{
	m_ChildVector.insert( m_ChildVector.begin(), pGMUICtrl ) ;
}

VOID CGMUICustomPanel::DeleteChild( CGMUIObject *pGMUICtrl )
{
	for ( INT i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
	{
		if ( pGMUICtrl == m_ChildVector[ i ] )
		{
			m_ChildVector.erase( m_ChildVector.begin() + i ) ;
			return;
		}
	}
}

VOID CGMUICustomPanel::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) || ( !m_pGMUIParentDlg ) )
		return;
	
	CGMUIObject *pGMUICtrl ;
	for ( int i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
	{
		pGMUICtrl = m_ChildVector[ i ] ;
		
		if ( pGMUICtrl->GetVisible() )
		{
			if ( InArea( pGMUICtrl->GetLeft(), pGMUICtrl->GetTop(), pGMUICtrl->GetWidth(), pGMUICtrl->GetHeight(), X, Y ) == 0 )
			{
				pGMUICtrl->MouseDown( pGMUICtrl, Button, Shift, X - pGMUICtrl->GetLeft(), Y - pGMUICtrl->GetTop() ) ;
				return ;
			}
		}
	}

	CGMUIObject::MouseDown( this, Button, Shift, X, Y ) ;
}

VOID CGMUICustomPanel::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	INT			xx, yy;
	CGMUIObject *pGMUICtrl;

	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) || ( !m_pGMUIParentDlg ) )   
		return ;
	
	for ( INT i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
	{
		pGMUICtrl = m_ChildVector[ i ] ;
		if ( pGMUICtrl->GetVisible() )
		{
			xx = X - CGMUIObject::GetLeft() ;
			yy = Y - CGMUIObject::GetTop() ;

			pGMUICtrl->MouseLeave( pGMUICtrl ) ;
			if ( InArea( pGMUICtrl->GetLeft(), pGMUICtrl->GetTop(), pGMUICtrl->GetWidth(), pGMUICtrl->GetHeight(), xx, yy ) ==  0 )
			{
				pGMUICtrl->MouseEnter( pGMUICtrl ) ;
				GUITooltipMgr.m_ttRenderTarget = pGMUICtrl->GetTooltip();		 
				pGMUICtrl->MouseMove( pGMUICtrl, Shift, X - pGMUICtrl->GetLeft(), Y - pGMUICtrl->GetTop() ) ;
				break ;
			}
		}
	}

	CGMUIObject::MouseMove( this, Shift, X, Y ) ;
}

VOID CGMUICustomPanel::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) || ( !m_pGMUIParentDlg ) ) 
		return ;

	INT		xx, yy ;

	CGMUIObject *pGMUICtrl ;
	
	for ( INT i = 0 ; i < (int)m_ChildVector.size() ; i ++ )
	{
		pGMUICtrl = m_ChildVector[ i ] ;

		if ( pGMUICtrl->GetVisible() )
		{
			xx = X - ( CGMUIObject::GetLeft() + m_pGMUIParentDlg->GetLeft() ) ;
			yy = Y - ( CGMUIObject::GetTop() + m_pGMUIParentDlg->GetTop() ) ;

			if ( InArea( pGMUICtrl->GetLeft(), pGMUICtrl->GetTop(), pGMUICtrl->GetWidth(), pGMUICtrl->GetHeight(), xx, yy ) == 0 )
			{
				pGMUICtrl->MouseUp( pGMUICtrl, Button, Shift, X - pGMUICtrl->GetLeft(), Y - pGMUICtrl->GetTop() ) ;
				return ;
			}
		}
	}
	
	CGMUIObject::MouseUp( this, Button, Shift, X, Y ) ;
}

VOID	CGMUICustomPanel::AttachPanelImage( CGMTexture* pTexture ) 
{
	m_pPanelImage =  pTexture ;
}

VOID CGMUICustomPanel::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_pPanelImage )
		m_pPanelImage->Draw( r.left, r.top ) ;
}

VOID CGMUICustomPanel::GetChildPosition( CGMUIObject *pGMUIObject, INT *X, INT *Y )
{
	INT		xx = 0 ;
	INT		yy = 0 ;

	if ( m_pGMUIParentDlg ) 
		m_pGMUIParentDlg->GetDlgPosition( this, &xx, &yy ) ;

	*X = xx + pGMUIObject->GetLeft() ;
	*Y = yy + pGMUIObject->GetTop() ;
}

VOID CGMUICustomPanel::GMUIPaint( INT xx, INT yy )
{
	RECT		 r ;
	CGMUIObject *pGMUICtrl ;
	INT			 ix, iy ;

	if ( !GetVisible() )  
		return ;

	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth(), yy + CGMUIObject::GetHeight() ) ;

	DoDraw( this, r, CGMUIObject::GetButtonState() ) ;

	for ( INT i = m_ChildVector.size() - 1 ; i >= 0  ; i -- )
	{
		pGMUICtrl = m_ChildVector[ i ] ;
		if ( pGMUICtrl->GetVisible() )
		{
			GetChildPosition( pGMUICtrl, &ix, &iy ) ;
			pGMUICtrl->GMUIPaint( ix, iy ) ;
		}
	}
}



