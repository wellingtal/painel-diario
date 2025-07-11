#include "GMUICustomImeView.h"
#include "GMUIScreen.h"
#include "GMUICustomDlg.h"
#include "GMUICustomEdit.h"
#include "GMUIUtil.h"

#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "RYLButton.h"

#include "RYLImage.h"
#include "RYLImageManager.h"

//-- Zergra From.--//
#include "IME/GMIMEPACK.h"
#include "GMMemory.h"
//-- Zergra To. --//

CGMUICustomIMEView::CGMUICustomIMEView() : CGMUIObject()
{
	m_pGMUIParentDlg		   = NULL;
	m_pImeViewRightButton   = NULL;
	m_pImeViewLeftButton	   = NULL;
	m_pGMUIEdit				   = NULL;

	m_strImeBuffer			   = NULL;
	m_iImeBufferViewIndex   = 0;
	m_iImeBufferIndex		   = 0;
	m_iImeCharViewCount	   = 0;

	CGMUIObject::SetLeft( 0 );
	CGMUIObject::SetTop( 0 );
	CGMUIObject::SetWidth( 80 );
	CGMUIObject::SetHeight( 16 );
	CGMUIObject::SetVisible( FALSE );

	SetGMUICtrlType( GMUICtrlImeView );
	InitGMUICtrl();

	
		
	CGMImageDib*	pImageDIB = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 216, 17, &pImageDIB ) ;
	pImageDIB->MakeFrameRect( 0, 0, 216, 17, 0xffB99E6B, 0xc0000000,  1 ) ;

	m_pIMEBackGroundImage  = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pIMEBackGroundImage ->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pIMEBackGroundImage  ) ;
	GM_DELETE( pImageDIB ) ;
}

CGMUICustomIMEView::~CGMUICustomIMEView()
{
	if ( m_strImeBuffer != NULL )
	{
		GM_DELETE_ARRAY( m_strImeBuffer );
	}

	FinalGMUICtrl();
}

VOID CGMUICustomIMEView::InitGMUICtrl()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ; // was RYL_COMPONENT01_256x256

	m_pImeViewRightButton = new CGMUICustomImeViewRightButton( m_pGMUIParentDlg );
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pImeViewRightButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pImeViewRightButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pImeViewRightButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pImeViewRightButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pImeViewRightButton->SetParent( this );


	m_pImeViewLeftButton = new CGMUICustomImeViewLeftButton( m_pGMUIParentDlg );
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pImeViewLeftButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pImeViewLeftButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pImeViewLeftButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pImeViewLeftButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pImeViewLeftButton->SetParent( this );

	GMUICtrlSetSize();
}

VOID CGMUICustomIMEView::FinalGMUICtrl()
{
	GM_DELETE( m_pImeViewRightButton );
	GM_DELETE( m_pImeViewLeftButton );
}

VOID CGMUICustomIMEView::GMUICtrlSetSize()
{
	if ( m_pImeViewLeftButton )
	{
		m_pImeViewLeftButton->SetWidth( GetHeight() );
		m_pImeViewLeftButton->SetHeight( GetHeight() );
		m_pImeViewLeftButton->SetLeft( GetLeft() );
		m_pImeViewLeftButton->SetTop( GetTop() );
	}

	if ( m_pImeViewRightButton )
	{
		m_pImeViewRightButton->SetWidth( GetHeight() );
		m_pImeViewRightButton->SetHeight(GetHeight() );
		m_pImeViewRightButton->SetLeft( GetLeft() + ( GetWidth() - m_pImeViewLeftButton->GetWidth() ) );
		m_pImeViewRightButton->SetTop( GetTop() );
	}
}

VOID CGMUICustomIMEView::SetGMUIEdit( CGMUICustomEdit *pGMUIEdit )
{
	m_pGMUIEdit = pGMUIEdit;

   GMIMEPACK* pImePack = const_cast<GMIMEPACK*>( m_pGMUIEdit->GetImePack() );
   m_iImeCharViewCount = pImePack->GetCandidateOnceRetrieveCount();
}

BOOL CGMUICustomIMEView::GMUICtrlDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	INT		MyX = X + GetLeft();
	INT		MyY = Y + GetTop();
	if ( InArea( m_pImeViewLeftButton->GetLeft(), m_pImeViewLeftButton->GetTop(), m_pImeViewLeftButton->GetWidth(), m_pImeViewLeftButton->GetHeight(), MyX, MyY ) == 0 )
	{
		m_pImeViewLeftButton->MouseDown( Sender, Button, Shift, X, Y );

      if ( m_pGMUIEdit != NULL )
	   {
         GMIMEPACK* pImePack = const_cast<GMIMEPACK*>( m_pGMUIEdit->GetImePack() );
         pImePack->SetCandidateListPrev();
      }
		return TRUE;
	}

	if ( InArea( m_pImeViewRightButton->GetLeft(), m_pImeViewRightButton->GetTop(), m_pImeViewRightButton->GetWidth(), m_pImeViewRightButton->GetHeight(), MyX, MyY) == 0)
	{
		m_pImeViewRightButton->MouseDown (Sender, Button, Shift, X, Y);

      if ( m_pGMUIEdit != NULL )
	   {
         GMIMEPACK* pImePack = const_cast<GMIMEPACK*>( m_pGMUIEdit->GetImePack() );
         pImePack->SetCandidateListNext();
      }
		return TRUE;
	}

	return FALSE;
}

VOID CGMUICustomIMEView::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( !GetVisible() || !GetEnable() )		{ return; }
	
	if ( GMUICtrlDown( Sender, Button, Shift, X, Y ) )
	{
		return;
	}
	else 
	{
		CGMUIObject::MouseDown( this, Button, Shift, X, Y );
	}

   GMIMEPACK* pImePack = const_cast<GMIMEPACK*>( m_pGMUIEdit->GetImePack() );

	if ( m_iImeCharViewCount > static_cast<INT>( pImePack->GetCandidateOnceRetrieveCount() ) )
	{
		m_iImeCharViewCount = pImePack->GetCandidateOnceRetrieveCount();
	}

	/*if ( m_pGMUIEdit != NULL )
	{
		DoCharClick( this, str, m_iImeBufferIndex, X, Y );
	}*/

	this->SetVisible( FALSE );
}

BOOL	CGMUICustomIMEView::GMUICtrlUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	//----------------------------------------------------------
	//		예외적으로 Form이 없기 때문에 Leave아 Enter
	//		메시지를 바로날림 SetCurrentHint;
	//----------------------------------------------------------
	INT		MyX = X + CGMUIObject::GetLeft();
	INT		MyY = Y + CGMUIObject::GetTop();

	if ( InArea( m_pImeViewLeftButton->GetLeft(), m_pImeViewLeftButton->GetTop(), m_pImeViewLeftButton->GetWidth(), m_pImeViewLeftButton->GetHeight(), MyX, MyY ) == 0 )
	{
		m_pImeViewLeftButton->MouseUp( Sender, Button, Shift, X, Y );
		return TRUE;
	}

	if ( InArea( m_pImeViewRightButton->GetLeft(), m_pImeViewRightButton->GetTop(), m_pImeViewRightButton->GetWidth(), m_pImeViewRightButton->GetHeight(), MyX, MyY ) == 0 )
	{
		m_pImeViewRightButton->MouseUp( Sender, Button, Shift, X, Y );
		return TRUE;
	}
	return FALSE;
}

VOID CGMUICustomIMEView::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y )
{
	if ( !CGMUIObject::GetVisible() || !CGMUIObject::GetEnable() )			{ return; }
	

	if ( GMUICtrlUp( Sender, Shift, Button, X, Y ) )
	{
		return;
	}
	else
	{
		CGMUIObject::MouseUp( Sender, Button, Shift, X, Y );
		this->SetVisible( FALSE );
	}
}

VOID CGMUICustomIMEView::KeyDown( CGMUIObject* Sender, WORD key, INT Shift )
{
	CGMUIObject::KeyDown( Sender, key, Shift );
}

BOOL CGMUICustomIMEView::GMUICtrlMove( CGMUIObject* Sender, INT Shift, INT X, INT Y)
{
	//----------------------------------------------------------
	//		예외적으로 Form이 없기 때문에 Leave아 Enter
	//		메시지를 바로날림 SetCurrentHint;
	//----------------------------------------------------------
	if ( InArea( m_pImeViewLeftButton->GetLeft(), m_pImeViewLeftButton->GetTop(), m_pImeViewLeftButton->GetWidth(), m_pImeViewLeftButton->GetHeight(), X, Y ) == 0 )
	{
		m_pImeViewLeftButton->MouseMove( Sender, Shift, X, Y );
		RECT r = m_pImeViewLeftButton->GetGMUIObjectRect();
		CGMUIScreen::GetInstance()->SetCurrentHint( m_pImeViewLeftButton, r, m_strHint );

		return TRUE;
	}

	if ( InArea( m_pImeViewRightButton->GetLeft(), m_pImeViewRightButton->GetTop(), m_pImeViewRightButton->GetWidth(), m_pImeViewRightButton->GetHeight(), X, Y ) == 0 )
	{
		m_pImeViewRightButton->MouseMove( Sender, Shift, X, Y );
		RECT r = m_pImeViewRightButton->GetGMUIObjectRect();
		CGMUIScreen::GetInstance()->SetCurrentHint( m_pImeViewRightButton, r, m_strHint );
		return TRUE;
	}
	return FALSE;
}

VOID CGMUICustomIMEView::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
	//----------------------------------------------------------
	//		예외적으로 Form이 없기 때문에 Leave아 Enter
	//		메시지를 바로날림 SetCurrentHint;
	//----------------------------------------------------------

	if ( !CGMUIObject::GetVisible() || !CGMUIObject::GetEnable() )		{ return; }
	
	if ( GMUICtrlMove( Sender, Shift, X, Y ) )
	{
		return;
	}
	else
	{
		RECT r = CGMUIObject::GetGMUIObjectRect();
		CGMUIScreen::GetInstance()->SetCurrentHint( this, r, m_strHint );
	}

	/*INT		len = strlen (m_strImeBuffer);
	INT		n = m_iImeBufferViewIndex * 2;
	INT		ww = CGMUIObject::GetWidth() - m_pImeViewLeftButton->GetWidth() - m_pImeViewRightButton->GetWidth();
	INT		cw = 0;
	INT		oldcw = 0;
	INT		i = m_iImeBufferViewIndex;
	CHAR	str[ 3 ];
	INT		myx = X - CGMUIObject::GetLeft();
	
	while( len > n )
	{ 
		str[ 0 ] = m_strImeBuffer[ n ];
		str[ 1 ] = m_strImeBuffer[ n + 1 ];
		str[ 2 ] = '\0';

		cw = cw + GetGMUITextWidth( str );
		
		if ( ww < cw )
		{
			break;
		}
		if ( ( ( myx - m_pImeViewLeftButton->GetWidth() ) >= oldcw ) && ( ( myx - m_pImeViewLeftButton->GetWidth() ) < cw ) )
		{
			m_iImeBufferIndex = i;
			break;
		}

		oldcw = cw;
		n = n + 2;
		i ++;
	}*/
}

VOID CGMUICustomIMEView::SetVisible( BOOL boVisible )
{
	/*GMUICtrlSetSize();
	m_iImeBufferViewIndex = 0;
	m_iImeBufferIndex = 0;
	CGMUIScreen::s_bFirstIme = FALSE;*/

   CGMUIObject::SetVisible (boVisible);

	if ( boVisible == FALSE )
	{
		m_pGMUIEdit = NULL;
	}
}

VOID CGMUICustomIMEView::DoLeftButtonClick( CGMUIObject* Sender, INT X, INT Y )
{
}

VOID CGMUICustomIMEView::DoRightButtonClick( CGMUIObject* Sender, INT X, INT Y )
{
}

VOID CGMUICustomIMEView::DoCharClick( CGMUIObject* Sender, LPSTR aText, INT Index, INT X, INT Y )
{
}

VOID CGMUICustomIMEView::DoDrawImeChar( CGMUIObject* Sender, INT Index, RECT r, LPSTR pText, INT State )
{
}

VOID CGMUICustomIMEView::DrawCandidateString( INT xx, INT yy )
{
	//-- Zergra From.--//
   UINT  unPosX = xx + m_pImeViewLeftButton->GetWidth();

   DoDrawImeCharBegin( this );   

   GMIMEPACK* pImePack = const_cast<GMIMEPACK*>( m_pGMUIEdit->GetImePack() );

   UINT unLeftStartPos  = 0;

   SetLeftButtonPos( unPosX + unLeftStartPos, GetTop() );

   for( UINT i = 0 ; i < pImePack->GetCandidateList().size() ; ++i )
   {
      vector<string> vtCandidtateString = pImePack->GetCandidateList();

      char     strTemp[10] = { 0, };
      itoa( i + 1, strTemp, 10 );

      string   strCurrentString = strTemp;
      strCurrentString += vtCandidtateString[ i ];

      UINT     unWidth     = GetGMUITextWidth( const_cast<char*>( strCurrentString.c_str() ) );
      UINT     unHeight    = GetGMUITextHeight( const_cast<char*>( strCurrentString.c_str() ) );
      UINT     unHeightGap = ( CGMUIObject::GetHeight() - unHeight ) / 2;

      RECT  rtDrawRegion = { GetHeight() + unLeftStartPos + unPosX, yy + unHeightGap, GetHeight() + unLeftStartPos + unPosX + unWidth, yy + unHeight };

      unLeftStartPos += ( unWidth + 2 );

      DoDrawImeChar( this, i, rtDrawRegion, const_cast<char*>( strCurrentString.c_str() ), LB_NONE );
   }

   unLeftStartPos += 2;

   SetRightButtonPos( GetHeight() + unPosX + unLeftStartPos, GetTop() );

   DoDrawImeCharEnd( this );
   //-- Zergra To. --//
}


VOID CGMUICustomIMEView::DoDraw (CGMUIObject* Sender, RECT r, int State)
{
}

VOID CGMUICustomIMEView::DoLeftButtonDraw (CGMUIObject* Sender, RECT r, int State)
{
}

VOID CGMUICustomIMEView::DoRightButtonDraw (CGMUIObject* Sender, RECT r, int State)
{
}

VOID CGMUICustomIMEView::GMUIPaint( INT xx, INT yy ) 
{
	INT		n = 0;
	RECT	r;

	xx = CGMUIObject::GetLeft();
	yy = CGMUIObject::GetTop();
	if( !CGMUIObject::GetVisible() )		return;

	m_pIMEBackGroundImage->Draw( xx+m_pImeViewLeftButton->GetWidth() - 2, yy - 2 ) ;
   SetRect (&r, xx+m_pImeViewLeftButton->GetWidth(), 
				 yy, 
				 xx+m_pImeViewLeftButton->GetWidth() + (CGMUIObject::GetWidth() - m_pImeViewLeftButton->GetWidth() - m_pImeViewRightButton->GetWidth()), 
				 yy+CGMUIObject::GetHeight() );

	DoDraw( this, r, CGMUIObject::GetButtonState() );

	// DrawChar
	DrawCandidateString( xx, yy );

	// 예외적 xx yy
	m_pImeViewRightButton->GMUIPaint( xx, yy );
	m_pImeViewLeftButton->GMUIPaint( xx, yy );
	
	CGMUIObject::GMUIPaint( xx, yy );
}

int CGMUICustomIMEView::GetImeCharViewCount ()
{
	return m_iImeCharViewCount;
}

//-- Zergra From.--//
//-----------------------------------------------------------------------------------------------------
// CGMUICustomIMEView::SetLeftButtonPos()
void CGMUICustomIMEView::SetLeftButtonPos( INT nLeft, INT nTop )
{
   if ( m_pImeViewLeftButton )
	{
		m_pImeViewLeftButton->SetLeft( nLeft );
      m_pImeViewLeftButton->SetTop( nTop );
	}
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomIMEView::SetRightButtonPos()
void CGMUICustomIMEView::SetRightButtonPos( INT nLeft, INT nTop )
{
   if ( m_pImeViewRightButton )
	{
		m_pImeViewRightButton->SetLeft( nLeft );
      m_pImeViewRightButton->SetTop( nTop );
	}
}
//-- Zergra To. --//

VOID CGMUICustomIMEView::DoDrawImeCharBegin (CGMUIObject* Sender)
{
}

VOID CGMUICustomIMEView::DoDrawImeCharEnd (CGMUIObject* Sender)
{
}



//------------------------------------------------------------------------------
//					CGMUICustomImeViewLeftButton : public CGMUICustomButton
//------------------------------------------------------------------------------
CGMUICustomImeViewLeftButton::CGMUICustomImeViewLeftButton(CGMUICustomDlg *pGMUIDlg) : CRYLButton( pGMUIDlg )
{
	m_pGMUICustomImeView = NULL;
}

CGMUICustomImeViewLeftButton::~CGMUICustomImeViewLeftButton()
{
	m_pGMUICustomImeView = NULL;
}


VOID CGMUICustomImeViewLeftButton::SetParent (CGMUIObject* pGMUIObject)
{
	m_pGMUICustomImeView = (CGMUICustomIMEView *)pGMUIObject;
}

VOID CGMUICustomImeViewLeftButton::GetParent (CGMUIObject** GMUIObject)
{
	*GMUIObject = (CGMUIObject *)m_pGMUICustomImeView;
}

VOID CGMUICustomImeViewLeftButton::SetSize (CGMUIObject* Sender)
{
}

VOID CGMUICustomImeViewLeftButton::Click (CGMUIObject* Sender, INT X, INT Y)
{
	CGMUICustomButton::Click (Sender, X, Y);
	if (m_pGMUICustomImeView)
		m_pGMUICustomImeView->DoLeftButtonClick (this, X, Y);
}

VOID CGMUICustomImeViewLeftButton::DoDraw (CGMUIObject* Sender, RECT r, INT State)
{
	CRYLButton::DoDraw (Sender, r, State);	
	//m_pGMUICustomImeView->DoLeftButtonDraw (Sender, r, State);
}

VOID CGMUICustomImeViewLeftButton::GMUIPaint( INT xx, INT yy)
{
	//----------------------------------------------------------
	//		버튼중에서 유일하게 예외적
	//		xx, yy가 Screen에서 구함
	//----------------------------------------------------------
	RECT r;
	xx = CGMUICustomButton::GetLeft();
	yy = CGMUICustomButton::GetTop();
	SetRect (&r, xx, yy, xx + CGMUICustomButton::GetWidth(), yy + CGMUICustomButton::GetHeight());
	if (! CGMUIObject::GetVisible()) {return;}

	DoDraw (this, r, CGMUIObject::GetButtonState () );

	CGMUIObject::GMUIPaint(xx, yy);
}
//------------------------------------------------------------------------------
//					CGMUICustomImeViewRightButton : public CGMUICustomButton
//------------------------------------------------------------------------------
CGMUICustomImeViewRightButton::CGMUICustomImeViewRightButton(CGMUICustomDlg *pGMUIDlg ) : CRYLButton (pGMUIDlg)
{
	m_pGMUICustomImeView = NULL;
}

CGMUICustomImeViewRightButton::~CGMUICustomImeViewRightButton()
{
	m_pGMUICustomImeView = NULL;
}


VOID CGMUICustomImeViewRightButton::SetParent (CGMUIObject* pGMUIObject)
{
	m_pGMUICustomImeView = (CGMUICustomIMEView *)pGMUIObject;
}

VOID CGMUICustomImeViewRightButton::GetParent (CGMUIObject** GMUIObject)
{
	*GMUIObject = (CGMUIObject *)m_pGMUICustomImeView;
}

VOID CGMUICustomImeViewRightButton::SetSize (CGMUIObject* Sender)
{
}

VOID CGMUICustomImeViewRightButton::Click (CGMUIObject* Sender, INT X, INT Y)
{
	CGMUICustomButton::Click (Sender, X, Y);
	if (m_pGMUICustomImeView)
		m_pGMUICustomImeView->DoRightButtonClick (this, X, Y);
}

VOID CGMUICustomImeViewRightButton::DoDraw (CGMUIObject* Sender, RECT r, INT State)
{
	CRYLButton::DoDraw (Sender, r, State);	
	//m_pGMUICustomImeView->DoRightButtonDraw (Sender, r, State);
}

VOID CGMUICustomImeViewRightButton::GMUIPaint( INT xx, INT yy)
{
	//----------------------------------------------------------
	//		버튼중에서 유일하게 예외적
	//		xx, yy가 Screen에서 구함
	//----------------------------------------------------------
	RECT r;
	xx = CGMUICustomButton::GetLeft();
	yy = CGMUICustomButton::GetTop();
	SetRect (&r, xx, yy, xx + CGMUICustomButton::GetWidth(), yy + CGMUICustomButton::GetHeight());
	if (! CGMUIObject::GetVisible()) {return;}

	DoDraw (this, r, CGMUIObject::GetButtonState () );

	CGMUIObject::GMUIPaint(xx, yy);
}