#include "RYLFontTexture.h"
#include "GMFont.h"
#include "GMMemory.h"

CRYLFontTexture::CRYLFontTexture () : CGMFontTexture ()
{
	m_hFontTex	= 0 ;	
}

CRYLFontTexture::~CRYLFontTexture ()
{
	if ( m_hFontTex != 0 )
		CGMFont::Instance()->ReleaseStringTexture( m_hFontTex ) ;
}

VOID CRYLFontTexture::Draw( INT X, INT Y )
{
	CGMFont::Instance()->DrawString( m_hFontTex, X, Y, m_dwFontColor, m_FontDrawType, m_Alpha, m_dwBorderColor ) ;
}

VOID CRYLFontTexture::SetSize( INT Width, INT Height, BOOL boClear )
{
	if ( ( Width <= 0 ) || ( Height <= 0 ) )
	{
		return ;
	}

	if ( ( m_iWidth != Width ) || ( m_iHeight != Height ) )
	{
		m_iWidth	= Width ;
		m_iHeight	= Height ;
		CGMFont::Instance()->ReleaseStringTexture( m_hFontTex ) ;
		CGMFont::Instance()->CreateStringTexture( &m_hFontTex, m_iWidth, m_iHeight ) ;
	}
}

VOID CRYLFontTexture::SetText( LPSTR pText, LPSTR pFontName)
{
	// Font는 나중에
	CGMFontTexture::SetText( pText ) ;
	CGMFont::Instance()->SetString (0, m_hFontTex, pText);
}
