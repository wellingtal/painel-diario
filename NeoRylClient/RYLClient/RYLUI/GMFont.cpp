#include "GMFont.h"
#include "GMFontTexture.h"
#include "RYLFontTexture.h"
#include "GMMemory.h"


CGMFont* CGMFont::s_pInstance = NULL ;

CGMFont::CGMFont()
{
	m_hGMFontLibrary				= 0 ; 
	m_bBegin						= FALSE ;

	m_fGMFontInitDeviceObjects		= NULL ; 
	m_fGMFontDeleteDeviceObjects	= NULL ; 
	m_fGMFontRestoreDeviceObjects	= NULL ; 
	m_fGMFontInvalidateDeviceObjects= NULL ; 
	m_fGMFontFinalCleanup			= NULL ; 

	m_fGMCreateFont					= NULL ; 
	m_fGMReleaseFont				= NULL ; 
	m_fGMDrawText					= NULL ; 
	m_fGMDrawAlphaText				= NULL ; 
	m_fGMDrawBorderText				= NULL ;

	m_fGMCreateStringTexture		= NULL ; 
	m_fGMReleaseStringTexture		= NULL ; 
	m_fGMSetString					= NULL ; 
	m_fGMDrawString					= NULL ; 
	m_fGMDrawAlphaString			= NULL ; 
	m_fGMDrawBorderString			= NULL ;  

	m_fGMTextWidth					= NULL ;
	m_fGMTextHeight					= NULL ;

	LoadLibrary() ;
}

CGMFont::~CGMFont()
{
	FreeLibrary() ;
}

HRESULT CGMFont::CreateInstance() 
{
	GMASSERT( !s_pInstance, "CGMFont::Instance Failed!" ) ;
	s_pInstance = new CGMFont() ;
	GMASSERT( s_pInstance, "CGMFont::Instance Failed!" ) ;
	
	return S_OK ;
}

HRESULT CGMFont::DeleteInstance() 
{
	GMASSERT( s_pInstance, "CGMFont::Instance Failed!" ) ;
	GM_DELETE( s_pInstance ) ;

	return S_OK ;
}

HRESULT CGMFont::FontBegin()
{
	m_bBegin = TRUE;
	return S_OK ;
	
	GMASSERT( s_pInstance, "CGMFont::Instance Failed!" ) ;
	GMASSERT( !m_bBegin, "CGMFont::m_bBegin Failed!" ) ;

	LPDIRECT3DDEVICE8 lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	
	lpD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE , TRUE ) ;
	lpD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF); 
	lpD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ); 

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD) ;
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE   ) ;
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_ZERO  ) ;				

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_MODULATE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1  , D3DTA_TEXTURE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP    , D3DTOP_SELECTARG1 ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1 , D3DTA_DIFFUSE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2  , D3DTA_CURRENT ) ; 
	m_bBegin = TRUE;
	return S_OK;
}

HRESULT CGMFont::FontEnd()
{
	LPDIRECT3DDEVICE8 lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD) ;
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE   ) ;
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_ZERO  ) ;				

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_MODULATE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1  , D3DTA_TEXTURE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP    , D3DTOP_SELECTARG1 ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1 , D3DTA_DIFFUSE ) ; 
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2  , D3DTA_CURRENT ) ; 

	m_bBegin = FALSE;

	return S_OK;
}


HRESULT CGMFont::LoadLibrary()
{
	//m_hazFontLibrary = ::LoadLibrary("Lib\\3DEngine\\Service\\Font Service\\AZFont.dll");
	m_hGMFontLibrary = ::LoadLibrary("GMFont.dll");
	GMASSERT( m_hGMFontLibrary, "CGMFont::m_hGMFontLibrary Failed!" ) ;

	m_fGMFontInitDeviceObjects		= ( FGMFontInitDeviceObjects )GetProcAddress( m_hGMFontLibrary, "InitDeviceObjects" ) ;
	m_fGMFontDeleteDeviceObjects	= ( FGMFontDeleteDeviceObjects )GetProcAddress( m_hGMFontLibrary, "DeleteDeviceObjects" ) ;
	m_fGMFontRestoreDeviceObjects	= ( FGMFontRestoreDeviceObjects )GetProcAddress( m_hGMFontLibrary, "RestoreDeviceObjects" ) ;
	m_fGMFontInvalidateDeviceObjects= ( FGMFontInvalidateDeviceObjects )GetProcAddress( m_hGMFontLibrary, "InvalidateDeviceObjects" ) ; 
	m_fGMFontFinalCleanup			= ( FGMFontCleanup )GetProcAddress( m_hGMFontLibrary, "FinalCleanup" ) ;

	m_fGMCreateFont					= ( FGMCreateFont )GetProcAddress( m_hGMFontLibrary, "CreateFont" ) ; 
	m_fGMReleaseFont				= ( FGMReleaseFont )GetProcAddress( m_hGMFontLibrary, "ReleaseFont" ) ; 
	m_fGMDrawText					= ( FGMDrawText )GetProcAddress( m_hGMFontLibrary, "DrawText" ) ;
	m_fGMDrawAlphaText				= ( FGMDrawAlphaText )GetProcAddress( m_hGMFontLibrary, "DrawAlphaText" ) ;
	m_fGMDrawBorderText				= ( FGMDrawBorderText )GetProcAddress( m_hGMFontLibrary, "DrawTextOutBorder" ) ;

	m_fGMCreateStringTexture		= ( FGMCreateStringTexture )GetProcAddress( m_hGMFontLibrary, "CreateStringTexture" ) ; 
	m_fGMReleaseStringTexture		= ( FGMReleaseStringTexture )GetProcAddress( m_hGMFontLibrary, "ReleaseStringTexture" ) ; 
	m_fGMSetString					= ( FGMSetString )GetProcAddress( m_hGMFontLibrary, "SetString" ) ; 
	m_fGMDrawString					= ( FGMDrawString )GetProcAddress( m_hGMFontLibrary, "DrawStringImage" ) ; 
	m_fGMDrawAlphaString			= ( FGMDrawAlphaString )GetProcAddress( m_hGMFontLibrary, "DrawStringAlphaImage" ) ; 
	m_fGMDrawBorderString			= ( FGMDrawBorderString )GetProcAddress( m_hGMFontLibrary, "DrawStringBorderImage" ) ; 

	m_fGMTextWidth					= ( FGMTextWidth )GetProcAddress( m_hGMFontLibrary, "TextWidth" ) ;
	m_fGMTextHeight					= ( FGMTextHeight )GetProcAddress( m_hGMFontLibrary, "TextHeight" ) ; 

	return S_OK ;
}

HRESULT CGMFont::FreeLibrary()
{
	GMASSERT( ( m_hGMFontLibrary != 0 ), "CGMFont::m_hazFontLibrary != 0 Failed" ) ;
	::FreeLibrary( m_hGMFontLibrary ) ;
	m_hGMFontLibrary = 0;

	m_fGMFontInitDeviceObjects		 = NULL ;  
	m_fGMFontDeleteDeviceObjects	 = NULL ;  
	m_fGMFontRestoreDeviceObjects	 = NULL ; 
	m_fGMFontInvalidateDeviceObjects = NULL ; 
	m_fGMFontFinalCleanup			 = NULL ; 

	m_fGMCreateFont					 = NULL ; 
	m_fGMReleaseFont				 = NULL ; 
	m_fGMDrawText					 = NULL ; 
	m_fGMDrawAlphaText				 = NULL ; 

	m_fGMCreateStringTexture		 = NULL ; 
	m_fGMReleaseStringTexture		 = NULL ; 
	m_fGMSetString					 = NULL ; 
	m_fGMDrawString					 = NULL ;  
	m_fGMDrawAlphaString			 = NULL ; 

	return S_OK ;
}


HRESULT CGMFont::InitDeviceObjects() 
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMFontInitDeviceObjects, "CGMFont::m_fGMFontInitDeviceObjects Failed!" ) ;
	return m_fGMFontInitDeviceObjects( BaseGraphicsLayer::GetDevice() ) ;
}

HRESULT CGMFont::DeleteDeviceObjects()
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMFontDeleteDeviceObjects, "CGMFont::m_fGMFontDeleteDeviceObjects Failed!" ) ;
	
	return m_fGMFontDeleteDeviceObjects();
}

HRESULT CGMFont::RestoreDeviceObjects()
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMFontRestoreDeviceObjects, "CGMFont::m_fGMFontRestoreDeviceObjects Failed!" ) ;
	
	return m_fGMFontRestoreDeviceObjects();
}

HRESULT CGMFont::InvalidateDeviceObjects()
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMFontInvalidateDeviceObjects, "CGMFont::m_fGMFontInvalidateDeviceObjects Failed!" ) ;
	
	return m_fGMFontInvalidateDeviceObjects();
}

HRESULT CGMFont::Clear()
{
	if( !this ) return 0;	// By Belldandy : 생성도 안되고 클리어시 뻑남

	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMFontFinalCleanup, "CGMFont::m_fGMFontFinalCleanup Failed!" ) ;
	
	return m_fGMFontFinalCleanup() ;
}

HRESULT CGMFont::CreateFont(DWORD* phFont,LPCTSTR szFontName,INT nFontSize) 
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMCreateFont, "CGMFont::m_fGMCreateFont Failed!" ) ;

	return m_fGMCreateFont( phFont, szFontName, nFontSize ) ;
}

HRESULT CGMFont::ReleaseFont(DWORD hFont) 
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMReleaseFont, "CGMFont::m_fGMReleaseFont Failed!" ) ;
	
	return m_fGMReleaseFont( hFont ) ;
}

HRESULT CGMFont::DrawText(DWORD hFont, INT X, INT Y, LPCTSTR szStr, DWORD dwColor) 
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	
	HRESULT hr; 
	BOOL bBegin = GetBegin() ;
	if ( !bBegin )
		FontBegin() ;

	GMASSERT( m_fGMDrawText, "CGMFont::m_fGMDrawText Failed!" ) ;
	hr = m_fGMDrawText(hFont, X, Y, szStr, dwColor) ;
	
	if (!bBegin)
		FontEnd() ;
	return hr;
}

HRESULT CGMFont::DrawAlphaText(DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;

	HRESULT hr; 
	BOOL bBegin = GetBegin() ;
	if (!bBegin)
		FontBegin() ;

	GMASSERT( m_fGMDrawAlphaText, "CGMFont::m_fGMDrawAlphaText Failed!" ) ;
	hr = m_fGMDrawAlphaText(hFont, X, Y, szStr, alpha, dwColor) ;
	if (!bBegin)
		FontEnd() ;
	return hr;
}

HRESULT CGMFont::DrawBorderText(DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor, DWORD dwBorderColor)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	
	HRESULT hr; 
	BOOL bBegin = GetBegin() ;
	if (!bBegin)
		FontBegin() ;

	GMASSERT( m_fGMDrawText, "CGMFont::m_fGMDrawText Failed!" ) ;
	hr = m_fGMDrawBorderText(hFont, X, Y, szStr, alpha, dwColor, dwBorderColor) ;
	if (!bBegin)
		FontEnd() ;
	return hr;
}

HRESULT CGMFont::CreateStringTexture(DWORD* phTexture, INT Width, INT Height)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMCreateStringTexture, "CGMFont::m_fGMCreateStringTexture Failed!" ) ;
	
	return m_fGMCreateStringTexture(phTexture, Width, Height);
}

HRESULT CGMFont::ReleaseStringTexture(DWORD& hTEXTURE)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMReleaseStringTexture, "CGMFont::m_fGMReleaseStringTexture Failed!" ) ;
	
	if ( m_fGMReleaseStringTexture( hTEXTURE ) == S_OK )
		hTEXTURE = 0 ;

	return S_OK ;
}

HRESULT CGMFont::SetString(DWORD hFont, DWORD hTexture, LPCTSTR szStr)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	GMASSERT( m_fGMSetString, "CGMFont::m_fGMSetString Failed!" ) ;
	
	return m_fGMSetString(hFont, hTexture, szStr);
}

HRESULT CGMFont::DrawString(DWORD hTexture, INT X, INT Y, DWORD dwColor, INT DrawType, INT Alpha, DWORD dwBorderColor)
{
	assert(s_pInstance);
	HRESULT hr; 
	BOOL bBegin = GetBegin() ;
	if (!bBegin)
		FontBegin() ;

	switch (DrawType)
	{
		case CGMFontTexture::FontDrawType_NONE:
			GMASSERT( m_fGMDrawString, "CGMFont::m_fGMDrawString Failed!" ) ;
			hr = m_fGMDrawString(hTexture, X, Y, dwColor); //	was that line
//			hr = m_fGMDrawBorderString(hTexture, X, Y, 1.0f, 1.0f, Alpha, dwColor, dwBorderColor) ;  // add that line
			break;

		case CGMFontTexture::FontDrawType_Alpha:
			GMASSERT( m_fGMDrawAlphaString, "CGMFont::m_fGMDrawAlphaString Failed!" ) ;
			hr = m_fGMDrawAlphaString(hTexture, X, Y, 1.0f, 1.0f, Alpha, dwColor);
			break;

		case CGMFontTexture::FontDrawType_Border:
			GMASSERT( m_fGMDrawBorderString, "CGMFont::m_fGMDrawBorderString Failed!" ) ;
			hr = m_fGMDrawBorderString(hTexture, X, Y, 1.0f, 1.0f, Alpha, dwColor, dwBorderColor) ;
			break;
	}

	if (!bBegin)
		FontEnd() ;
	return hr;
}

DWORD CGMFont::TextWidth (DWORD hFont, LPCTSTR szStr)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;

	DWORD dwW;
	BOOL bBegin = GetBegin() ;
	if (!bBegin)
		FontBegin() ;

	GMASSERT( m_fGMTextWidth, "CGMFont::m_fGMTextWidth Failed!" ) ;
	dwW = m_fGMTextWidth (hFont, szStr);

	if (!bBegin)
		FontEnd() ;
	return dwW;
}


DWORD CGMFont::TextHeight (DWORD hFont, LPCTSTR szStr)
{
	GMASSERT( s_pInstance, "CGMFont::s_pInstance Failed!" ) ;
	DWORD dwW;
	BOOL bBegin = GetBegin() ;
	if (!bBegin)
		FontBegin() ;

	GMASSERT( m_fGMTextHeight, "CGMFont::m_fGMTextHeight Failed!" ) ;
	dwW = m_fGMTextHeight (hFont, szStr);

	if (!bBegin)
		FontEnd() ;
	return dwW;
}

VOID CGMFont::CreateFontTexture( CGMFontTexture** ppOutFontTexture)
{
	CRYLFontTexture* pFontTexture;
	pFontTexture = new CRYLFontTexture();
	*ppOutFontTexture = pFontTexture;
}
