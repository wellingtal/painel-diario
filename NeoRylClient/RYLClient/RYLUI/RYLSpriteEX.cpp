#include "RYLSpriteEx.h"
#include "RYLSprite.h"
#include "Texture.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"


CRYLSpriteEx::CRYLSpriteEx( CGMUICustomDlg *pParentDlg ) : CGMUILabel( pParentDlg )
{
	m_pSprite = NULL ;
	m_pTexture = NULL ;
	m_bAlpha = FALSE;
	m_lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CGMUIObject::SetGMUICtrlType( GMUISPRITE ) ;
}

CRYLSpriteEx::~CRYLSpriteEx()
{
	GM_DELETE( m_pSprite ) ;
	GM_DELETE( m_pTexture ) ;
}

VOID CRYLSpriteEx::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	CGMUILabel::DoDraw( Sender, r, State ) ;

	if ( m_pSprite )
	{
		if ( m_pSprite->m_lpTexture )
		{
			if( m_bAlpha )
			{
				m_lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
				m_lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
				m_lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

				m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
				m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

				m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
				m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
				m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;
			}

			m_pSprite->SetPosition( r.left, r.top ) ;



			m_pSprite->Render( m_lpD3DDevice ) ;
		}
	}
}

VOID CRYLSpriteEx::AttachImageTexture( CRYLSprite* pSprite )
{
	m_pSprite = pSprite ;
}

VOID CRYLSpriteEx::DetachImageTexture()
{
	m_pSprite = NULL ;
}