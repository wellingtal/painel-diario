#include "GMUILabel.h"
#include "GMUIUtil.h"
#include "GMFontTexture.h"
#include "GMMemory.h"


CGMUILabel::CGMUILabel( CGMUICustomDlg *pParentDlg ) : CGMUICustomLabel( pParentDlg )
{
	m_iOffsetX = 0 ;
	m_iOffsetY = 0 ;
	m_bAutoSize = FALSE ;
	m_eAlign = NONE ;
	CGMFont::Instance()->CreateFontTexture( &m_pFontTexture ) ;
}

CGMUILabel::~CGMUILabel()
{
	GM_DELETE( m_pFontTexture ) ;
}

VOID CGMUILabel::Change( CGMUIObject* Sender )
{
	CGMUICustomLabel::Change( Sender ) ;
	m_pFontTexture->SetSize( m_iWidth, m_iHeight ) ;
	DoAutoSize() ;
	SetAlign( m_eAlign ) ;
}

VOID CGMUILabel::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUICustomLabel::DoDraw( Sender, r, State ) ;

	if ( m_pFontTexture )
		m_pFontTexture->Draw( r.left + m_iOffsetX, r.top + m_iOffsetY ) ;
}

VOID CGMUILabel::SetFontColor( DWORD dwColor )
{
	CGMUICustomLabel::SetFontColor( dwColor ) ;

	if ( m_pFontTexture )
		m_pFontTexture->SetFontColor( m_dwFontColor ) ;
}

VOID CGMUILabel::SetBorderFontColor( DWORD dwColor )
{
	CGMUICustomLabel::SetBorderFontColor( dwColor ) ;
	if ( m_pFontTexture )
		m_pFontTexture->SetBorderColor( m_dwBorderFontColor ) ;
}

VOID CGMUILabel::SetVisible( BOOL bVisible )
{
	CGMUICustomLabel::SetVisible( bVisible ) ;
}

BOOL CGMUILabel::DoAutoSize()
{
	if ( !m_pFontTexture )
		return FALSE ;

	if ( m_bAutoSize )
	{
		if ( m_pFontTexture )
		{
			m_iWidth	= GetGMUITextWidth( GetCaption() ) ;
			m_iHeight	= GetGMUITextHeight( GetCaption() ) ;
			m_iOffsetX	= 0 ;
			m_iOffsetY	= 0 ;
			m_pFontTexture->SetSize( m_iWidth, m_iHeight ) ;
			m_pFontTexture->SetText( GetCaption() ) ;
			return TRUE;
		}
		m_iOffsetX = GetGMUITextWidth( GetCaption() ) ;
		m_iOffsetY = GetGMUITextHeight( GetCaption() ) ;

		m_iOffsetX = ( m_iWidth - m_iOffsetX ) / 2 ;
		m_iOffsetY = ( m_iHeight - m_iOffsetY ) / 2 ;
	}
	else
	{
		m_iOffsetX = 0 ;
		m_iOffsetY = 0 ;
		m_pFontTexture->SetText( GetCaption() ) ;
	}
	return FALSE ;
}

VOID CGMUILabel::SetAutoSize( BOOL bAutoSize )
{
	if ( m_bAutoSize != bAutoSize )
	{
		m_bAutoSize = bAutoSize ;
		DoAutoSize() ;
	}
}

BOOL CGMUILabel::GetAutoSize()
{
	return m_bAutoSize ;
}

RECT		CGMUILabel::SetAlign( RYL_ALIGN eAlign ) 
{
	RECT rt ;
	unsigned short wFontWidth = GetGMUITextWidth( GetCaption() ) ;
	unsigned short wAlignWidth = m_rtAline.right - m_rtAline.left ;

	unsigned short wFontHeight = GetGMUITextHeight( GetCaption() ) ;
	unsigned short wAlignHeight = m_rtAline.bottom - m_rtAline.top ;
	
	m_eAlign = eAlign ;

	switch( eAlign )
	{
	case LEFT :		
		{
			rt.left = m_rtAline.left ;										 
			rt.top  = m_rtAline.top + ( ( wAlignHeight - wFontHeight ) / 2 ) ;
		}
		break ;
	
	case RIGHT :	
		{
			rt.left = m_rtAline.left + ( wAlignWidth - wFontWidth ) ;		
			rt.top  = m_rtAline.top + ( ( wAlignHeight - wFontHeight ) / 2 ) ;
		}
		break ;
	
	case CENTER :	
		{
			rt.left = m_rtAline.left + ( ( wAlignWidth - wFontWidth ) / 2 ) ;
			rt.top  = m_rtAline.top + ( ( wAlignHeight - wFontHeight ) / 2 ) ;
		}
		break ;

	case NONE :		
		{
			rt.left   = m_rtAline.left = GetLeft() ;										 
			rt.top    = m_rtAline.top  = GetTop() ;
			rt.right  = m_rtAline.right = GetLeft() + wFontWidth ;
			rt.bottom = m_rtAline.bottom = GetTop() + wFontHeight ;
		}
		break ;
	}

	rt.right = rt.left + wFontWidth ;
	rt.bottom = rt.top + wFontHeight ;
	
	SetLeft( rt.left ) ;
	SetTop( rt.top ) ;

	return rt ;
}

void	CGMUILabel::SetAlignRect( RECT rt ) 
{
	m_rtAline = rt ;
}

