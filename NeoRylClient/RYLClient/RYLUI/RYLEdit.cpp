#include "RYLEdit.h"
#include "RYLFontTexture.h"
#include "RYLImageManager.h"
#include "RYLRectangle.h"

#include "GMTexture.h"
#include "GMFont.h"
#include "GMUIScreen.h"

//-- Zergra From.--//
#include "IME/GMIMEPACK.h"
#include "RYLIMEVIEW.h"
#include "GMMemory.h"
//-- Zergra To. --//


CRYLEdit::CRYLEdit( CGMUICustomDlg *pParentDlg ) : CGMUICustomEdit( pParentDlg )
{
	m_dwColor		= 0xFF000000 ;
	m_dwLineColor	= 0xFFFFFF00 ;
	m_pBackImage	= NULL ;
	m_pFontTexture	= NULL ;

   //-- Zergra From.--//
   m_pImeView     = NULL;
   //-- Zergra To. --//

	if ( !m_pFontTexture )
	{
		CGMFont::Instance()->CreateFontTexture( ( CGMFontTexture** )&m_pFontTexture ) ;
	}
}

CRYLEdit::~CRYLEdit()
{
   //-- Zergra From.--//
   GM_DELETE( m_pImeView );
   //-- Zergra To. --//

	GM_DELETE( m_pFontTexture ) ;
	m_pBackImage = NULL ;
}

VOID CRYLEdit::DefaultDraw (RECT r)
{
	CRYLRectangle::Instance()->Draw( r.left, r.top, r.right-r.left, r.bottom-r.top, m_dwAlpha, m_dwColor ) ;	
}

VOID CRYLEdit::DoDraw( CGMUIObject* Sender, RECT r )
{
	CGMUICustomEdit::DoDraw( Sender, r ) ;

   // if candidate window opened below function return non-zero value
   if( m_pImePack->GetCandidateCount() )
   {
      if( !m_pImeView )
      {
         m_pImeView = new CRYLIMEView;
         m_pImeView->SetWidth( 128 );
         m_pImeView->SetGMUIEdit( dynamic_cast<CGMUICustomEdit*>( this ) );
         
         // set position( left, down of edit box )
         m_pImeView->SetLeft( r.left );
         m_pImeView->SetTop( r.bottom );
      }

      if( !m_pImeView->GetVisible() )
         m_pImeView->SetVisible( TRUE );

      m_pImeView->GMUIPaint( m_pImeView->GetLeft(), m_pImeView->GetTop() );
   }
   else
   {
      GM_DELETE( m_pImeView );
   }
	
	if ( !m_pBackImage )
	{
		DefaultDraw( r ) ;
	}
	else
	{
		m_pBackImage->Draw( r.left, r.top ) ;
	}
}

VOID CRYLEdit::DoCharDraw( CGMUIObject* Sender, RECT r, LPSTR str )
{
   if( !str )
      return;

	if( strcmp( str, "" ) == 0 )
		return;

	CGMFont::Instance()->DrawText( 0, r.left, r.top, str, m_dwFontColor ) ;

   //-- Zergra From.--//
   // place for specialized text drawing routine( for example, concerned with composition string, attribute, etc )
   //-- Zergra To. --//
}

VOID CRYLEdit::DoUnderLineDraw( CGMUIObject* Sender, RECT r )
{
	CGMUICustomEdit::DoUnderLineDraw( Sender, r ) ;
	CRYLRectangle::Instance()->Draw( r.left, r.top, r.right-r.left, r.bottom - r.top, 255, m_dwFontColor ) ;
}

VOID CRYLEdit::DoSelectLineDraw( CGMUIObject* Sender, RECT r )
{
	CGMUICustomEdit::DoSelectLineDraw( Sender, r ) ;
	CRYLRectangle::Instance()->Draw( r.left, r.top, r.right-r.left, r.bottom - r.top, 255, 0x0071260B ) ;
}

VOID CRYLEdit::SetFontColor( DWORD dwColor )
{
	CGMUICustomEdit::SetFontColor( dwColor ) ;
	if ( m_pFontTexture )
		m_pFontTexture->SetFontColor( dwColor ) ;
}

VOID CRYLEdit::SetLineColor( DWORD dwColor )
{
	m_dwLineColor = dwColor;
}

DWORD CRYLEdit::GetLineColor()
{
	return m_dwLineColor ;
}

VOID CRYLEdit::SetSize( CGMUIObject* Sender )
{
	CGMUICustomEdit::SetSize( Sender ) ;
	//m_pFontTexture->SetSize (CAUXUICustomEdit::GetWidth (), 16); // --! 16일단 고정
}

VOID CRYLEdit::AttachBackTexture( CGMTexture* pGMTexture )
{
	m_pBackImage = pGMTexture ;
}

VOID CRYLEdit::DetachBackTexture ()
{
	m_pBackImage = NULL ;
}

