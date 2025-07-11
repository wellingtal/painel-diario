#include "GMUICaptionButton.h"
#include "GMUIUtil.h"
#include "GMFontTexture.h"
#include "GMMemory.h"


CGMUICaptionButton::CGMUICaptionButton( CGMUICustomDlg *pParentDlg ) : CGMUIButton( pParentDlg )
{
	m_pFontTexture	= NULL ;
	m_iOffsetX		= 0 ;
	m_iOffsetY		= 0 ;
	m_bCaptionFlag	= FALSE ;
}
 
CGMUICaptionButton::~CGMUICaptionButton()
{
	GM_DELETE( m_pFontTexture ) ;
}

VOID CGMUICaptionButton::SetCaption( LPSTR pCaption)
{
	if ( strcmp( m_Caption, pCaption ) != 0 )
	{
		m_Caption = pCaption ;
		Change( this ) ;
	}
}

LPSTR CGMUICaptionButton::GetCaption ()
{
	return m_Caption ;
}

VOID CGMUICaptionButton::Change( CGMUIObject* Sender )
{
	if ( !m_pFontTexture )
		CGMFont::Instance()->CreateFontTexture( &m_pFontTexture ) ;

	m_bCaptionFlag = TRUE ;
}

VOID CGMUICaptionButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUIButton::DoDraw( Sender, r, State ) ;
	if ( m_bCaptionFlag )
	{
		SetCaptionChange() ;
	}
	if ( m_pFontTexture )
	{
		INT iLeft, iTop ;
		if ( GetButtonState() == BTN_STATE_DOWN )
		{
			iLeft = r.left + m_iOffsetX + 1 ;
			iTop  = r.top + m_iOffsetY + 1 ;
		}
		else
		{
			iLeft = r.left + m_iOffsetX ;
			iTop  = r.top + m_iOffsetY ;
		}
		if ( GetButtonState() == BTN_STATE_DISIBLE ||
			 GetDisableEmptyCheck() == BTN_STATE_CHECKED_DISIBLE ||
			 GetDisableEmptyCheck() == BTN_STATE_NCHECKED_DISIBLE )
		{
			m_pFontTexture->SetFontColor( 0xff808080 ) ;
		}
		else
		{
			m_pFontTexture->SetFontColor( 0xffffffff ) ;
		}
		m_pFontTexture->Draw( iLeft, iTop ) ;
	}
}

VOID CGMUICaptionButton::SetVisible( BOOL bVisible )
{
	CGMUIButton::SetVisible( bVisible ) ;
	if ( m_pFontTexture )
		m_pFontTexture->SetVisible( bVisible ) ;
}

VOID CGMUICaptionButton::SetFontColor( DWORD dwColor )
{
	CGMUIButton::SetFontColor( dwColor ) ;
	if ( m_pFontTexture )
		m_pFontTexture->SetFontColor( m_dwFontColor ) ;
}

VOID CGMUICaptionButton::SetBorderFontColor( DWORD dwColor )
{
	CGMUIButton::SetBorderFontColor( dwColor ) ;
	if ( m_pFontTexture )
		m_pFontTexture->SetBorderColor( m_dwBorderFontColor ) ;
}

VOID CGMUICaptionButton::SetCaptionChange()
{
	INT w = GetGMUITextWidth( m_Caption ) ;
	INT h = GetGMUITextHeight( m_Caption ) ;
	m_pFontTexture->SetSize( w, h ) ;
	m_pFontTexture->SetText( m_Caption ) ;

	m_iOffsetX = ( m_iWidth - m_pFontTexture->GetWidth() ) / 2 ;
	m_iOffsetY = ( m_iHeight - m_pFontTexture->GetHeight() ) / 2 + 1 ;
	m_bCaptionFlag = FALSE ;
}

BOOL CGMUICaptionButton::DoAutoSize ()
{
	SetCaptionChange() ;

	return FALSE ;
}