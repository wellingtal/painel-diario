#include "RYLTextureManager.h"
#include "RYLFormatList.h"
#include "GMUIUtil.h"
#include "GMMemory.h"

CRYLTextureManager *CRYLTextureManager::s_pInstance = NULL ;

CRYLTextureManager::CRYLTextureManager()
: m_RYLTexturePool(512)
{
	pRYLFormat1555List = NULL ;
	pRYLFormat8888List = NULL ;
	pRYLFormat4444List = NULL ;
}

CRYLTextureManager::~CRYLTextureManager()
{
	Clear() ;
}

HRESULT CRYLTextureManager::Clear ()
{
	GM_DELETE( pRYLFormat1555List ) ;
	GM_DELETE( pRYLFormat8888List ) ;
	GM_DELETE( pRYLFormat4444List ) ;

	return S_OK ;
}

HRESULT CRYLTextureManager::CreateInstance ()
{
	GMASSERT( !s_pInstance, "CRYLTextureManager::CreateInstance() Failed" ) ;
	s_pInstance = new CRYLTextureManager() ;
	GMASSERT( s_pInstance, "CRYLTextureManager::CreateInstance() Failed" ) ;

	return S_OK ;
}

HRESULT CRYLTextureManager::DeleteInstance()
{
	GMASSERT( s_pInstance, "CRYLTextureManager::DeleteInstance() Failed" ) ;
	GM_DELETE( s_pInstance ) ;
	return S_OK ;
}

BOOL CRYLTextureManager::IsInited ()
{
	return s_pInstance != NULL ;  
}

CRYLTextureManager *CRYLTextureManager::Instance()
{
	GMASSERT( s_pInstance, "CRYLTextureManager::Instance() Failed" ) ;
	return s_pInstance; 
}

HRESULT CRYLTextureManager::CreateTexture( D3DFORMAT Format, INT Width, INT Height, HRYLTexture *pphRYLTexture )
{
	switch( Format )
	{
		case D3DFMT_A8R8G8B8 :
			if (pRYLFormat8888List == NULL)
			{
				pRYLFormat8888List = new CRYLFormatList (Format);
			}
			pRYLFormat8888List->CreateTexture ( ( ERYLTexture_Size_Type )GetTextureSize( Width ), 
												( ERYLTexture_Size_Type )GetTextureSize( Height ), 
												pphRYLTexture ) ;
			break ;

		case D3DFMT_A1R5G5B5 :
			if ( pRYLFormat1555List == NULL )
			{
				pRYLFormat1555List = new CRYLFormatList( Format ) ;
			}
			pRYLFormat1555List->CreateTexture( ( ERYLTexture_Size_Type )GetTextureSize( Width ), 
											   ( ERYLTexture_Size_Type )GetTextureSize( Height ), 
												pphRYLTexture ) ;
			break ;

		case D3DFMT_A4R4G4B4 :
			if ( pRYLFormat4444List == NULL )
			{
				pRYLFormat4444List = new CRYLFormatList( Format ) ;
			}
			pRYLFormat4444List->CreateTexture( ( ERYLTexture_Size_Type )GetTextureSize( Width ), 
											   ( ERYLTexture_Size_Type )GetTextureSize( Height ), 
											   pphRYLTexture ) ;
			break ;
	}
	return S_OK ;
}

HRESULT CRYLTextureManager::ReleaseTexture( HRYLTexture *pphRYLTexture )
{
	if ( *pphRYLTexture == NULL )
	{
		return E_FAIL ;
	}
	switch ( ( *pphRYLTexture)->GetFormat() )
	{
		case D3DFMT_A8R8G8B8 :
			if ( pRYLFormat8888List != NULL )
				pRYLFormat8888List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case D3DFMT_A1R5G5B5:
			if ( pRYLFormat1555List != NULL)
				pRYLFormat1555List->ReleaseTexture( pphRYLTexture ) ;
			break ;
		case D3DFMT_A4R4G4B4 :
			if ( pRYLFormat4444List != NULL )
				pRYLFormat4444List->ReleaseTexture( pphRYLTexture ) ;
			break ;
	}
	return S_OK ;
}

CRYLTexture* CRYLTextureManager::CreateRYLTexture( D3DFORMAT Format, INT Width, INT Height )
{
    return m_RYLTexturePool.construct( Format, Width, Height );
}

void CRYLTextureManager::DeleteRYLTexture(CRYLTexture* lpRYLTexture)
{
    m_RYLTexturePool.destroy(lpRYLTexture);
}


HRESULT CRYLTextureManager::InitDeviceObjects ()
{
	return S_OK;
}

HRESULT CRYLTextureManager::DeleteDeviceObjects ()
{
	return S_OK;
}

