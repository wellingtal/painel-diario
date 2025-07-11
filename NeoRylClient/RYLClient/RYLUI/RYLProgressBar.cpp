#include "RYLProgressBar.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMFont.h"
#include "RYLFontTexture.h"
#include "GMUIUtil.h"
#include "GMMemory.h"

CRYLProgressBar::CRYLProgressBar( CGMUICustomDlg *pParentDlg ) : CGMUICustomLabel( pParentDlg )
{
	m_iPosition		= 0 ;
	m_iValueMax		= 100 ;
	m_iValueMin		= 0 ;
	m_pBackImage	= NULL ;
	m_pBarImage		= NULL ;
	m_pFontTexture	= NULL ; 
}

CRYLProgressBar::~CRYLProgressBar()
{
	m_iPosition		= 0 ;
	m_iValueMax		= 100 ;
	m_iValueMin		= 0 ;
	m_pBackImage	= NULL ;
	m_pBarImage		= NULL ;
	GM_DELETE( m_pFontTexture ) ;
}

VOID CRYLProgressBar::SetMax( DWORD iMax )
{
	m_iValueMax = iMax ;
}

INT CRYLProgressBar::GetMax ()
{
	return m_iValueMax ;
}

VOID CRYLProgressBar::SetMin( DWORD iMin )
{
	if ( iMin < 0 )
		return ;
	m_iValueMin = iMin ;
}

INT CRYLProgressBar::GetMin()
{
	return m_iValueMin ;
}

VOID CRYLProgressBar::SetPosition( DWORD iPosition )
{
	if ( ( iPosition < m_iValueMin ) || ( iPosition > m_iValueMax ) )
		return ;

	m_iPosition = iPosition ;
}

INT CRYLProgressBar::GetPosition()
{
	return m_iPosition ;
}

VOID CRYLProgressBar::Change( CGMUIObject* Sender )
{
	if ( !m_pFontTexture )
		CGMFont::Instance()->CreateFontTexture( ( CGMFontTexture** )&m_pFontTexture ) ;

	m_pFontTexture->SetSize( m_iWidth, 16 ) ;
	m_pFontTexture->SetText( GetCaption() ) ;
}

VOID CRYLProgressBar::SetFontColor( DWORD dwColor )
{
	if ( m_pFontTexture )
		m_pFontTexture->SetColor( dwColor ) ;
}

VOID CRYLProgressBar::SetColor( DWORD dwColor )
{
	if ( m_pBackImage )
		m_pBackImage->SetColor( dwColor ) ;
}

VOID CRYLProgressBar::AttachBackImageTexture( CGMTexture* pGMTexture )
{
	m_pBackImage = ( CRYLImage * )pGMTexture ;
}

VOID CRYLProgressBar::DetachBackImageTexture ()
{
	m_pBackImage = NULL ;
}

VOID CRYLProgressBar::AttachBarImageTexture( CGMTexture* pGMTexture )
{
	m_pBarImage = ( CRYLImage * )pGMTexture ;
}

VOID CRYLProgressBar::DetachBarImageTexture ()
{
	m_pBarImage = NULL ;
}

VOID CRYLProgressBar::DefaultBackDraw()
{
	// юс╫ц 
	//m_pBackImage = CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, 0xFF000000 ) ;
}

VOID CRYLProgressBar::DefaultBarDraw ()
{
	//
	m_pBarImage = CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, 0xFF0000FF ) ;
}

VOID CRYLProgressBar::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{

	if(((r.bottom-r.top)==108)&&((r.right-r.left)==112))//if its hp or mp bar
	{
			LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
			
		if ( m_iPosition == 0 || m_iValueMax == 0 )
			return ;

		INT		per = m_iPosition * m_iWidth / m_iValueMax ;
		RECT	dr ;
		INT		h, w ;

		if ( !m_pBackImage )  
			DefaultBackDraw() ;

		if ( m_pBackImage )  
			m_pBackImage->Draw( r.left, r.top ) ;

		dr = r ;
		//SetRect( &dr, r.left, r.bottom +20 , r.right, r.bottom - per ) ;//hz guigui
		SetRect( &dr, r.left , r.bottom , r.right, r.bottom + per ) ;//hz guigui

		if ( !m_pBarImage )
			DefaultBarDraw() ;

		if ( m_pBarImage )
			m_pBarImage->DrawRect2( dr ) ;//hz guigui ws drawrect

		w = GetGMUITextWidth( GetCaption() ) ;
		h = GetGMUITextHeight( GetCaption() ) ;
		w = ( m_iWidth - w ) / 2 ;
		h = ( m_iHeight - h ) / 2 ;
	
		if ( m_pFontTexture )
			m_pFontTexture->Draw( r.left + w, r.top + h ) ;
	}
	else//if its anything else
	{
		LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

		if ( m_iPosition == 0 || m_iValueMax == 0 )
			return ;

		INT		per = m_iPosition * m_iWidth / m_iValueMax ;
		RECT	dr ;
		INT		h, w ;

		if ( !m_pBackImage )  
			DefaultBackDraw() ;

		if ( m_pBackImage )  
			m_pBackImage->Draw( r.left, r.top ) ;

		dr = r ;
		SetRect( &dr, r.left, r.top, r.left + per, r.bottom ) ;//hz guigui


		if ( !m_pBarImage )
			DefaultBarDraw() ;

		if ( m_pBarImage )
			m_pBarImage->DrawRect( dr ) ;//hz guigui ws drawrect

		w = GetGMUITextWidth( GetCaption() ) ;
		h = GetGMUITextHeight( GetCaption() ) ;
		w = ( m_iWidth - w ) / 2 ;
		h = ( m_iHeight - h ) / 2 ;
	
		if ( m_pFontTexture )
			m_pFontTexture->Draw( r.left + w, r.top + h ) ;
	}
}


