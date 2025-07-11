#include "RYLTexture.h"
#include "RenderOption.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

CRYLTexture::CRYLTexture( D3DFORMAT Format, INT Width, INT Height )
{
	m_pd3dTexture					= NULL ;
	m_RYLTextureParameters.uiWidth	= Width ;
	m_RYLTextureParameters.uiHeight	= Height ;
	
	//=====================================================
	// Low textureµµ D3DFMT_A1R5G5B5·Î »ý¼º
	if ( BaseGraphicsLayer::m_iGraphicCards == 0 || 
		 BaseGraphicsLayer::m_iGraphicCards == 1 )
	{
		m_RYLTextureParameters.Format	= D3DFMT_A1R5G5B5 ;
		m_RYLTextureParameters.uiLevels = 1 ; // ¹Ó¸ã ·¹º§
	}
	else
	{
		m_RYLTextureParameters.Format	= D3DFMT_A8R8G8B8 ;
		m_RYLTextureParameters.uiLevels = 0 ; // ¹Ó¸ã ·¹º§		
	}
		
	CreateTexture() ;
}

CRYLTexture::~CRYLTexture()
{
	ReleaseTexture() ;
}

HRESULT CRYLTexture::CreateTexture()
{
	LPDIRECT3DTEXTURE8 pTempTexture ;
	HRESULT hr ;

	if ( m_pd3dTexture )   
	{
		ReleaseTexture() ;
	}
	
	hr = BaseGraphicsLayer::GetDevice()->CreateTexture ( m_RYLTextureParameters.uiWidth, 
														 m_RYLTextureParameters.uiHeight, 
														 m_RYLTextureParameters.uiLevels, 
														 0, 
														 m_RYLTextureParameters.Format, 
														 D3DPOOL_MANAGED, 
														 &pTempTexture ) ;

	m_pd3dTexture = LPDIRECT3DBASETEXTURE8( pTempTexture ) ;

	return hr ;
}

HRESULT CRYLTexture::ReleaseTexture ()
{
    GM_RELEASE( m_pd3dTexture ) ;
	return S_OK ;
}

HRESULT CRYLTexture::LockRect (BYTE** ppBits, INT& Pitch)
{
	D3DLOCKED_RECT D3Rect ;

	if(!m_pd3dTexture)
	{
		return E_FAIL ;
	}

	if ( FAILED( LPDIRECT3DTEXTURE8( m_pd3dTexture )->LockRect( 0, &D3Rect, 0, 0 ) ) )
	{
		*ppBits = NULL ;
		Pitch	= 0;
		GMASSERT( FALSE, "RYLTexture::LockRect Failed" ) ;
		return E_FAIL ;
	}

	*ppBits = ( PBYTE )D3Rect.pBits ;
	Pitch = D3Rect.Pitch ;

	return S_OK ;
}

HRESULT CRYLTexture::UnlockRect ()
{
	if(!m_pd3dTexture)
	{
		return E_FAIL ;
	}

	if ( FAILED( LPDIRECT3DTEXTURE8( m_pd3dTexture )->UnlockRect( 0 ) ) )
	{
		GMASSERT( FALSE, "RYLTexture::UnLockRect Failed" ) ;
		return E_FAIL ;
	}

	return S_OK ;
}

D3DFORMAT CRYLTexture::GetFormat ()
{
	return m_RYLTextureParameters.Format ;
}

int CRYLTexture::GetWidth ()
{
	return m_RYLTextureParameters.uiWidth ;
}

int CRYLTexture::GetHeight ()
{
	return m_RYLTextureParameters.uiHeight ;
}

HRESULT CRYLTexture::InitDeviceObjects()
{
	return S_OK ;
}

HRESULT CRYLTexture::DeleteDeviceObjects()
{
	return S_OK ;
}

HRESULT CRYLTexture::InvalidateDeviceObjects()
{
	return S_OK ;
}

HRESULT CRYLTexture::RestoreDeviceObjects()
{
	return S_OK ;
}

