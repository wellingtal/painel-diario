#include "GMUICustomListBox.h"
#include "GMUICustomDlg.h"
#include "GMList.h"
#include "GMMemory.h"
 

//------------------------------------------------------------------------------
CGMUICustomListBox::CGMUICustomListBox( CGMUICustomDlg *pParentDlg, BOOL bScrollEnable ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pDataList = new CGMList ;

	m_iOrY				= 0 ;
	m_iMouseX			= 0 ;
	m_iMouseY			= 0 ;
	m_iItemHeight		= 14 ;
    m_iItemMerginX		= 2 ;
    m_iItemMerginY		= 2 ;
	m_iViewItemCount	= 0 ;
	m_iViewIndex		= 0 ;
	m_iOverIndex		= 0 ;
	m_iItemIndex		= -1 ;
	
	CGMUIObject::SetGMUICtrlType( GMUICtrlListBox ) ;

	m_pListBoxTopButton		= NULL ;
	m_pListBoxTrackButton	= NULL ;
	m_pListBoxBottomButton	= NULL ;

	OnDrawItemEvent		= NULL ;
	OnDrawEvent			= NULL ;
	OnTopDrawEvent		= NULL ;
	OnTrackDrawEvent	= NULL ;
	OnBottomDrawEvent	= NULL ;

	InitGMUICtrl( bScrollEnable ) ;
	m_bScrollEnable		= bScrollEnable ;
	SetTrackPos( m_iMouseY ) ;
}

CGMUICustomListBox::~CGMUICustomListBox()
{
	FinalGMUICtrl() ;
	ClearData() ;

	GM_DELETE( m_pDataList ) ;
	
	OnDrawItemEvent		= NULL ;
	OnDrawEvent			= NULL ;
	OnTopDrawEvent		= NULL ;
	OnTrackDrawEvent	= NULL ;
	OnBottomDrawEvent	= NULL ;
}

VOID CGMUICustomListBox::InitGMUICtrl( BOOL bScrollEnable )
{
	m_pListBoxTopButton = new CListBoxTopButton( m_pGMUIParentDlg ) ;
	m_pListBoxTopButton->SetParent( this ) ;
	
	m_pListBoxTrackButton = new CListBoxTrackButton( m_pGMUIParentDlg ) ;
	m_pListBoxTrackButton->SetParent( this ) ;
	
	m_pListBoxBottomButton = new CListBoxBottomButton( m_pGMUIParentDlg ) ;
	m_pListBoxBottomButton->SetParent( this ) ;
	
	m_iScrollTrackTopPos = m_pListBoxTopButton->GetHeight() ;
	m_iScrollTrackingPos = m_pListBoxTopButton->GetHeight() ;

	if ( bScrollEnable )
	{
		CGMUICustomPanel::AddChild( m_pListBoxTopButton ) ;
		CGMUICustomPanel::AddChild( m_pListBoxTrackButton ) ;
		CGMUICustomPanel::AddChild( m_pListBoxBottomButton ) ;
	}
}

VOID CGMUICustomListBox::FinalGMUICtrl()
{
	GM_DELETE( m_pListBoxTopButton ) ;
	GM_DELETE( m_pListBoxTrackButton ) ;
	GM_DELETE( m_pListBoxBottomButton ) ;
}

CListBoxTopButton *CGMUICustomListBox::GetTopButton()
{
	return m_pListBoxTopButton ;
}

CListBoxTrackButton *CGMUICustomListBox::GetTrackButton()
{
	return m_pListBoxTrackButton ;
}

CListBoxBottomButton *CGMUICustomListBox::GetBottomButton()
{
	return m_pListBoxBottomButton ;
}


VOID CGMUICustomListBox::SetTopButtonSize( INT Width, INT Height )
{
	if ( m_pListBoxTopButton != NULL )
	{
		m_pListBoxTopButton->SetWidth( Width ) ;
		m_pListBoxTopButton->SetHeight( Height ) ;
	}
}

VOID CGMUICustomListBox::SetTrackButtonSize( INT Width, INT Height )
{
	if ( m_pListBoxTrackButton != NULL )
	{
		m_pListBoxTrackButton->SetWidth( Width ) ;
		m_pListBoxTrackButton->SetHeight( Height ) ;
	}
}

VOID CGMUICustomListBox::SetBottomButtonSize( INT Width, INT Height )
{
	if ( m_pListBoxBottomButton != NULL )
	{
		m_pListBoxBottomButton->SetWidth( Width ) ;
		m_pListBoxBottomButton->SetHeight( Height ) ;
	}
}

INT CGMUICustomListBox::GetScrollTrackingPos()
{
	if ( m_pDataList->Count() < m_iViewItemCount )
	{
		return m_pListBoxTopButton->GetHeight() ;	
	}

	INT n	= CGMUIObject::GetHeight() - ( m_pListBoxTrackButton->GetHeight() + 
										   m_pListBoxTopButton->GetHeight() + 
										   m_pListBoxBottomButton->GetHeight() ) ;
	INT in	= m_pDataList->Count() - m_iViewItemCount ;
	
	if ( in == 0 )
	{
		in = 1 ;
	}
	return ( ( m_iViewIndex * n ) / in ) + m_pListBoxTopButton->GetHeight() ;
}

VOID CGMUICustomListBox::DecItem( INT Value )
{
	m_iViewIndex = m_iViewIndex - Value ;
	
	if ( m_iViewIndex <= 0 )
	{
		m_iViewIndex = 0 ;
		m_iScrollTrackingPos = m_pListBoxTopButton->GetHeight() ;
		return ;
	}

	m_iScrollTrackingPos = GetScrollTrackingPos() ;
}

VOID CGMUICustomListBox::IncItem( INT Value )
{
	if ( m_pDataList->Count() < m_iViewItemCount )
		return ;

	m_iViewIndex = m_iViewIndex + Value ;
	if ( m_iViewIndex + m_iViewItemCount > m_pDataList->Count() -1 )
	{
		m_iViewIndex = m_pDataList->Count() - m_iViewItemCount ;
		m_iScrollTrackingPos = CGMUIObject::GetHeight() - ( m_pListBoxTrackButton->GetHeight() + 
															m_pListBoxBottomButton->GetHeight() ) ;
		return ;
	}

	m_iScrollTrackingPos = GetScrollTrackingPos() ;
}

BOOL CGMUICustomListBox::GetScrollBarInPos( INT X, INT Y )
{
	return ( ( X > m_iItemMerginX ) && 
		     ( X < (CGMUIObject::GetWidth() - m_pListBoxTrackButton->GetWidth() - m_iItemMerginX ) ) && 
		     ( Y > m_iItemMerginY ) && 
		     ( Y < CGMUIObject::GetHeight() - m_iItemMerginY ) ) ;
}

VOID CGMUICustomListBox::TopButtonDown()
{
	DecItem( 1 ) ;
	SetTrackPos( m_iMouseY ) ;
}

VOID CGMUICustomListBox::TrackButtonDown()
{
	m_iOrY = m_iMouseY - m_iScrollTrackingPos ;
}

VOID CGMUICustomListBox::BottomButtonDown()
{
	IncItem( 1 ) ;
	SetTrackPos( m_iMouseY ) ;
}

VOID CGMUICustomListBox::TrackButtonMove(INT yy)
{
	SetTrackPos( yy ) ;
}

VOID CGMUICustomListBox::MouseDown (CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )
	{
		return ;
	}
	
	// Get Item Index
	if ( GetScrollBarInPos( X, Y ) )
	{
		m_iItemIndex = m_iViewIndex + ( Y - m_iItemMerginY ) / m_iItemHeight ;
		if ( m_iItemIndex > m_pDataList->Count() - 1 )
		{
			m_iItemIndex = m_pDataList->Count() -1 ;
		}
	}
	
	if ( m_pDataList->Count() -1 < m_iViewItemCount )
	{
		m_iViewIndex = 0 ;
		m_iScrollTrackingPos = m_pListBoxTopButton->GetHeight() ;
	}

	CGMUICustomPanel::MouseDown( Sender, Button, Shift, X, Y ) ;
}

INT CGMUICustomListBox::GetViewIndex ()
{
	if ( m_pDataList->Count() == 0 )
		return 0 ;

	INT n = CGMUIObject::GetHeight() - ( m_pListBoxTrackButton->GetHeight() + 
										 m_pListBoxTopButton->GetHeight() + 
										 m_pListBoxBottomButton->GetHeight() ) ;

	return ( ( m_pDataList->Count() - m_iViewItemCount ) * ( m_iScrollTrackingPos - m_pListBoxTopButton->GetHeight() ) ) / n ;
}


VOID CGMUICustomListBox::SetViewIndex( INT iIndex )
{
	m_iViewIndex = iIndex;

	INT n = CGMUIObject::GetHeight() - ( m_pListBoxTrackButton->GetHeight() + 
										 m_pListBoxTopButton->GetHeight() + 
										 m_pListBoxBottomButton->GetHeight() ) ;

	m_iScrollTrackingPos = ( n * m_iViewIndex ) / ( m_pDataList->Count() - m_iViewItemCount ) + m_pListBoxTopButton->GetHeight() ;
	m_pListBoxTrackButton->SetTop( m_iScrollTrackingPos ) ;
}


VOID CGMUICustomListBox::SetTrackPos( INT Y )
{
	INT		MyY = Y;
	INT		n ;

	if ( m_pListBoxTrackButton->GetDown() )
	{
		m_iScrollTrackingPos = MyY - m_iOrY ;

		if ( m_iScrollTrackingPos < m_pListBoxTopButton->GetHeight() )
		{
			m_iScrollTrackingPos = m_pListBoxTopButton->GetHeight() ;
		}

		n = m_pListBoxBottomButton->GetTop() - ( m_pListBoxBottomButton->GetHeight() + 7 ) ;
		if ( m_iScrollTrackingPos > n )
		{
			m_iScrollTrackingPos = n ;
		}
		if ( m_pDataList->Count() -1 > m_iViewItemCount )
		{
			m_iViewIndex = GetViewIndex() ;
		}
	}
	m_pListBoxTrackButton->SetTop( m_iScrollTrackingPos ) ;
}

VOID CGMUICustomListBox::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	m_iMouseX = X ;
	m_iMouseY = Y ;

	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) ) 
	{
		return ;
	}
	
	if ( GetScrollBarInPos( X - GetLeft(), Y - GetTop() ) )
	{
	    m_iOverIndex = m_iViewIndex + ( Y - m_iItemMerginY) / m_iItemHeight ;
	}

	CGMUICustomPanel::MouseMove( Sender, Shift, X, Y ) ;
}

VOID CGMUICustomListBox::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	m_iMouseX = X ;
	m_iMouseY = Y ;

	if ( ( !CGMUIObject::GetVisible() ) || ( !CGMUIObject::GetEnable() ) )
	{
		return ;
	}
	
	CGMUICustomPanel::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID CGMUICustomListBox::SetSize( CGMUIObject* Sender )
{
	CGMUIObject::SetSize( this ) ;
	SetGMUICtrlSize() ;
}

VOID CGMUICustomListBox::SetGMUICtrlSize ()
{
	/*
	if ( !m_bScrollEnable )
		return ;
	*/

	INT n ;

	n = ( GetWidth() - m_pListBoxTopButton->GetWidth() ) ;
	m_pListBoxTopButton->SetLeft( n ) ;
	m_pListBoxTopButton->SetTop( 0 ) ;

	n = ( GetWidth() - m_pListBoxTrackButton->GetWidth() ) ;
	m_pListBoxTrackButton->SetLeft( n ) ;
	m_pListBoxTrackButton->SetTop( m_pListBoxTopButton->GetTop() + m_pListBoxTopButton->GetHeight() ) ;

	n = ( GetWidth() - m_pListBoxBottomButton->GetWidth() ) ;
	m_pListBoxBottomButton->SetLeft( n ) ;
	n = ( GetHeight() - m_pListBoxBottomButton->GetHeight() ) ;
	m_pListBoxBottomButton->SetTop( n ) ;
}

INT CGMUICustomListBox::GetItemIndex ()
{
	return m_iItemIndex ;
}

VOID	
CGMUICustomListBox::SelectItemIndex( INT Index )
{
	//if( Index < m_iViewItemCount )
		m_iItemIndex = Index;
	return;
}


INT CGMUICustomListBox::SetItemIndex( INT Index )
{
	if ( Index > m_iViewItemCount )
	{
		m_iViewIndex = Index - m_iViewItemCount ;
		if ( m_iViewIndex > ( m_pDataList->Count() - m_iViewItemCount ) )
		{
			m_iViewIndex = ( m_pDataList->Count() - m_iViewItemCount ) ;
		}
	}
	else
	{
		m_iViewIndex = 0 ;
	}

	m_iScrollTrackingPos = GetScrollTrackingPos() ;
	SetTrackPos( m_iMouseY ) ;

	return 0 ;
}

VOID CGMUICustomListBox::SetItemHeight( INT Value )
{
	m_iItemHeight = Value ;
}

INT CGMUICustomListBox::GetItemHeight()
{
	return m_iItemHeight ;
}

INT CGMUICustomListBox::GetItemWidth()
{
	return CGMUIObject::GetWidth() - m_iItemMerginX - m_pListBoxTrackButton->GetWidth() ;
}

VOID CGMUICustomListBox::SetItemMerginX( INT nX )
{
    m_iItemMerginX = nX ;
}

INT CGMUICustomListBox::GetItemMerginX()
{
	return m_iItemMerginX ;
}

VOID CGMUICustomListBox::SetItemMerginY( INT nY )
{
    m_iItemMerginY = nY ;
}

INT CGMUICustomListBox::GetItemMerginY ()
{
	return m_iItemMerginY ;
}


VOID CGMUICustomListBox::ClearData()
{
	m_pDataList->Clear() ;
	m_iOverIndex = 0 ;
	m_iItemIndex = -1 ;
}

VOID CGMUICustomListBox::AddData( LPVOID Value )
{
	m_pDataList->Add( Value ) ;
}

VOID CGMUICustomListBox::GetData( INT Index, VOID **ppValue )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() - 1 ) )
	{
		*ppValue = NULL ;
		return ;
	}
	
	LPVOID	tmp ;
	m_pDataList->Get( Index, &tmp ) ;
	*ppValue = tmp ;
}

VOID CGMUICustomListBox::DeleteData( INT Index )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() - 1 ) )
	{
		return ;
	}
	m_pDataList->Delete( Index ) ;
}

VOID CGMUICustomListBox::SetData( INT Index, LPVOID pValue )
{
	if ( ( Index < 0 ) || ( Index > m_pDataList->Count() - 1 ) )
	{
		return ;
	}

	LPVOID	tmp = NULL ;
	
	m_pDataList->Get( Index, &tmp ) ;
	tmp = pValue ;
}

INT CGMUICustomListBox::Count ()
{
	return m_pDataList->Count() ;
}

VOID CGMUICustomListBox::DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State )
{
	if ( OnDrawItemEvent != NULL )
	{
		OnDrawItemEvent( Sender, Index, r, State ) ;
	}
}

VOID CGMUICustomListBox::SetCalcViewItemCount() 
{
	m_iViewItemCount = 
        int((float)(CGMUIObject::GetHeight() - (m_iItemMerginY * 2)) / (float)m_iItemHeight + 0.5) ;
}

VOID CGMUICustomListBox::DrawItems( INT xx, INT yy )
{
	SetCalcViewItemCount() ;

	INT		x, y ;
	RECT	r ;
	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () ) ;

	x = m_iItemMerginX + xx ; 
	y = m_iItemMerginY + yy ;

	INT i ;

	DrawItemBegin( this ) ;
	
	for ( i = m_iViewIndex ; i < m_iViewItemCount + m_iViewIndex ; i ++ )
	{
		if ( i > m_pDataList->Count() - 1 )
		{
			return ;
		}
		INT ist = LB_NONE ;
	    
		if ( i == m_iItemIndex )
		{
			ist = LB_SELECT ;
		}

		SetRect( &r, x, y, x + CGMUIObject::GetWidth() - m_pListBoxTrackButton->GetWidth(), y + m_iItemHeight ) ;
		DoDrawItem( this, i, r, ist ) ;
		y = y + m_iItemHeight ;
	}
	DrawItemEnd( this ) ;
}

VOID CGMUICustomListBox::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_bScrollEnable )
	{
		if ( OnDrawEvent != NULL )
		{
			OnDrawEvent( Sender, r, State ) ;
		}
	}
}

VOID CGMUICustomListBox::DoTopDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_bScrollEnable )
	{
		if ( OnTopDrawEvent != NULL )
		{
			OnTopDrawEvent( Sender, r, State ) ;
		}
	}
}

VOID CGMUICustomListBox::DoTrackDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_bScrollEnable )
	{
		if ( OnTrackDrawEvent != NULL )
		{
			OnTrackDrawEvent( Sender, r, State ) ;
		}
	}
}

VOID CGMUICustomListBox::DoBottomDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_bScrollEnable )
	{
		if ( OnBottomDrawEvent != NULL )
		{
			OnBottomDrawEvent( Sender, r, State ) ;
		}
	}
}

VOID CGMUICustomListBox::GMUIPaint( INT xx, INT yy )
{
	RECT r ;
	SetRect( &r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () ) ;

	DoDraw( this, r, CGMUIObject::GetButtonState () ) ;

	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
	DrawItems( xx, yy ) ;
}

VOID CGMUICustomListBox::DrawItemBegin( CGMUIObject* Sender )
{

}

VOID CGMUICustomListBox::DrawItemEnd( CGMUIObject* Sender )
{

}


//------------------------------------------------------------------------------
CListBoxTopButton::CListBoxTopButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomButton( pParentDlg )
{
	m_pGMUICustomListBox = NULL ;
	CGMUICustomButton::SetWidth( 13 ) ;
	CGMUICustomButton::SetHeight( 13 ) ;
}

CListBoxTopButton::~CListBoxTopButton()
{
	m_pGMUICustomListBox = NULL ;
}

VOID CListBoxTopButton::SetParent( LPVOID pGMUIObject )
{
	m_pGMUICustomListBox = ( CGMUICustomListBox * )pGMUIObject ;
}

VOID CListBoxTopButton::GetParent( VOID **ppGMUIObject )
{
	*ppGMUIObject = ( CGMUIObject * )m_pGMUICustomListBox ;
}

VOID CListBoxTopButton::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		if ( Button == MB_LEFTCLICK ) 
			m_pGMUICustomListBox->TopButtonDown() ;
	}
	CGMUICustomButton::MouseDown( Sender, Button, Shift, X, Y ) ;
}

VOID CListBoxTopButton::SetSize( CGMUIObject* Sender )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->SetSize( this ) ;
	}
}

VOID CListBoxTopButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->DoTopDraw( Sender, r, State ) ;
	}
}


//------------------------------------------------------------------------------
CListBoxTrackButton::CListBoxTrackButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomButton( pParentDlg ) 
{
	m_boTracking		 = FALSE ;
	m_pGMUICustomListBox = NULL ;
	SetWidth( 8 ) ;
	SetHeight( 21 ) ;
}

CListBoxTrackButton::~CListBoxTrackButton()
{
	m_pGMUICustomListBox = NULL ;
}

VOID CListBoxTrackButton::SetParent( VOID *pGMUIObject )
{
	m_pGMUICustomListBox = ( CGMUICustomListBox * )pGMUIObject ;
}

VOID CListBoxTrackButton::GetParent( VOID **ppGMUIObject )
{
	*ppGMUIObject = ( CGMUIObject * )m_pGMUICustomListBox ;
}

VOID CListBoxTrackButton::SetSize( CGMUIObject* Sender )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->SetSize( this ) ;
	}
}

VOID CListBoxTrackButton::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		if ( Button == MB_LEFTCLICK )  
		{
			m_pGMUICustomListBox->TrackButtonDown() ;
			m_boTracking = TRUE ;
		}
	}
	CGMUICustomButton::MouseDown( Sender, Button, Shift, X, Y ) ;
} 

VOID CListBoxTrackButton::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		if ( m_boTracking )
		{
			m_pGMUICustomListBox->TrackButtonMove( Y ) ;
		}
	}
	CGMUICustomButton::MouseMove( Sender, Shift, X, Y ) ;
}

VOID CListBoxTrackButton::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	m_boTracking = FALSE ;
	CGMUICustomButton::MouseUp( Sender, Button, Shift, X, Y ) ;
}

VOID CListBoxTrackButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->DoTrackDraw( Sender, r, State ) ;
	}
}

//------------------------------------------------------------------------------
CListBoxBottomButton::CListBoxBottomButton( CGMUICustomDlg *pParentDlg ) : CGMUICustomButton( pParentDlg )
{
	m_pGMUICustomListBox = NULL ;
	SetWidth( 13 ) ;
	SetHeight( 13 ) ;
}

CListBoxBottomButton::~CListBoxBottomButton()
{
	m_pGMUICustomListBox = NULL ;
}

VOID CListBoxBottomButton::SetParent( LPVOID pGMUIObject )
{
	m_pGMUICustomListBox = ( CGMUICustomListBox * )pGMUIObject ;
}

VOID CListBoxBottomButton::GetParent( VOID **ppGMUIObject )
{
	*ppGMUIObject = ( CGMUIObject * )m_pGMUICustomListBox ;
}

VOID CListBoxBottomButton::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		if ( Button == MB_LEFTCLICK ) 
			m_pGMUICustomListBox->BottomButtonDown() ;
	}
	CGMUICustomButton::MouseDown( Sender, Button, Shift, X, Y ) ;
}

VOID CListBoxBottomButton::SetSize( CGMUIObject* Sender )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->SetSize( this ) ;
	}
}

VOID CListBoxBottomButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( m_pGMUICustomListBox != NULL )
	{
		m_pGMUICustomListBox->DoBottomDraw( Sender, r, State ) ;
	}
}
