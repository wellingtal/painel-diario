#include "RYLRectangle.h"
#include "GMUIUtil.h"
#include "GMMemory.h"

CRYLRectangle *CRYLRectangle::s_pInstance = NULL ;

CRYLRectangle::CRYLRectangle()
{
	m_bBegin	= FALSE ;
	m_fDepth	= 0.0f ;
	m_hVB		= NULL ;
}

CRYLRectangle::~CRYLRectangle()
{
	ReleaseVB() ;
}

VOID CRYLRectangle::SetDepth( FLOAT fDepth )
{
	m_fDepth = fDepth ;
}

HRESULT CRYLRectangle::ReSetRenderState()
{
	//CAZRenderStateManager::Instance()->PopRenderState(3);
	//CAZTextureStateManager::Instance()->PopTextureState(4) ;

	m_bBegin = FALSE ;

	return S_OK ;
}

HRESULT CRYLRectangle::ReRectangleRenderState()
{
	if ( m_hVB == NULL )
	{
		CreateVB() ;
	}

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;
	
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ) ;

	lpD3DDevice->SetStreamSource( 0, m_hVB, sizeof( TRHWDiffVertex ) ) ;
	lpD3DDevice->SetVertexShader( NULL ) ;
	lpD3DDevice->SetVertexShader( FVF_RHWDIF ) ;

	return S_OK ;
}

HRESULT CRYLRectangle::Begin()
{
	ReRectangleRenderState() ;
	m_bBegin = TRUE ;

	return S_OK ;
}

HRESULT CRYLRectangle::End()
{
	ReSetRenderState() ;

	return S_OK ;
}

HRESULT	CRYLRectangle::CreateVB() 
{
	if ( !m_hVB )
	{		
		if ( SUCCEEDED( BaseGraphicsLayer::GetDevice()->CreateVertexBuffer( 4 * sizeof( TRHWDiffVertex ), 
																			D3DUSAGE_WRITEONLY, 
																			FVF_RHWDIF,  
																			D3DPOOL_MANAGED,
																			&m_hVB ) ) )
		{
			return S_OK ;
		}
		else
		{
			GMASSERT( FALSE, "CRYLRectangle::CreateVertexBuffer Failed" ) ;
			return E_FAIL ;
		}
	}
	return S_OK ;
}

HRESULT	CRYLRectangle::ReleaseVB() 
{
	/*
	if (m_hVB)
	{
		if ( SUCCEEDED( BaseGraphicsLayer::GetDevice()->ReleaseVertexBuffer( m_hVB ) ) )
		{
			m_hVB = NULL ;
			return S_OK ;
		}
	}
	*/

	GM_RELEASE( m_hVB ) ;

	return S_OK ;
}

HRESULT CRYLRectangle::CreateInstance()
{
	GMASSERT( !s_pInstance, "CRYLRectangle::s_pInstance Failed" ) ;
	s_pInstance = new CRYLRectangle();
	GMASSERT( s_pInstance, "CRYLRectangle::s_pInstance Failed" ) ;

	return S_OK ;
}

HRESULT CRYLRectangle::DeleteInstance()
{
	GMASSERT( s_pInstance, "CRYLRectangle::s_pInstance Failed" ) ;
	GM_DELETE( s_pInstance ) ;

	return S_OK ;
}

BOOL CRYLRectangle::IsInited()
{
	return s_pInstance != NULL; 
}

CRYLRectangle * CRYLRectangle::Instance()
{
	GMASSERT( s_pInstance, "CRYLRectangle::s_pInstance Failed" ) ;
	return s_pInstance; 
}

HRESULT CRYLRectangle::InitDeviceObjects()
{
	if ( m_hVB == NULL )
		CreateVB() ;

	return S_OK ;
}

HRESULT CRYLRectangle::DeleteDeviceObjects()
{
	ReleaseVB() ;
	return S_OK ;
}

HRESULT CRYLRectangle::Draw( INT X, INT Y, INT Width, INT Height, DWORD Alpha, DWORD Color )
{
	DWORD			 dwColor ;
	BYTE			*pData ;
	TRHWDiffVertex  *pVB = NULL ;

	dwColor = Color32toColor32( Alpha, Color ) ;

	if ( m_bBegin == FALSE )
		ReRectangleRenderState() ;

	if ( SUCCEEDED( m_hVB->Lock( 0, 0, &pData, 0 ) ) )
	{
		pVB = ( TRHWDiffVertex * )pData ;

		pVB[ 0 ].Position = D3DXVECTOR4( X, Y, m_fDepth, 1.0f ) ; // left top
		pVB[ 0 ].Color	  = dwColor ;

		pVB[ 1 ].Position = D3DXVECTOR4( X + Width, Y, m_fDepth, 1.0f ) ; // right top
		pVB[ 1 ].Color	  = dwColor ;

		pVB[ 2 ].Position = D3DXVECTOR4( X, Y + Height, m_fDepth, 1.0f ) ; // left bottom
		pVB[ 2 ].Color	  = dwColor ;
 
		pVB[ 3 ].Position = D3DXVECTOR4( X + Width, Y + Height, m_fDepth, 1.0f ) ; // right bottom
		pVB[ 3 ].Color	  = dwColor ;

		if ( FAILED( m_hVB->Unlock() ) ) 
		{
			ReSetRenderState() ;
			return E_FAIL ;
		}
	}

	BaseGraphicsLayer::GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) ;
	// Draw
	if ( m_bBegin == FALSE )
		ReSetRenderState() ;

	return S_OK ;
}
// test
HRESULT CRYLRectangle::DrawA( INT X, INT Y, INT Width, INT Height, DWORD AlphaLT, DWORD AlphaRT, DWORD AlphaLB, DWORD AlphaRB, DWORD Color)
{
	DWORD			 dwColor ;
	BYTE			*pData ;
	TRHWDiffVertex  *pVB = NULL ;

	if ( m_bBegin == FALSE )
		ReRectangleRenderState() ;

	if ( SUCCEEDED( m_hVB->Lock( 0, 0, &pData, 0 ) ) )
	{
		pVB = ( TRHWDiffVertex * )pData ;

		pVB[ 0 ].Position	= D3DXVECTOR4( X, Y, m_fDepth, 1.0f ) ;			// left top
		dwColor				= Color32toColor32( AlphaLT, Color ) ;
		pVB[ 0 ].Color		= dwColor ;

		pVB[ 1 ].Position	= D3DXVECTOR4( X + Width, Y, m_fDepth, 1.0f ) ; // right top
		dwColor				= Color32toColor32( AlphaRT, Color ) ;
		pVB[ 1 ].Color		= dwColor ;

		pVB[ 2 ].Position	= D3DXVECTOR4( X, Y+Height, m_fDepth, 1.0f ) ; // left bottom
		dwColor				= Color32toColor32( AlphaLB, Color ) ;
		pVB[ 2 ].Color		= dwColor ;

		pVB[ 3 ].Position	= D3DXVECTOR4( X + Width, Y + Height, m_fDepth, 1.0f ) ; // right bottom
		dwColor				= Color32toColor32 (AlphaRB, Color);
		pVB[ 3 ].Color	    = dwColor ;

		if ( FAILED(m_hVB->Unlock() ) )
		{
			ReSetRenderState() ;
			return E_FAIL ;
		}
	}

	BaseGraphicsLayer::GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) ;
	// Draw
	if ( m_bBegin == FALSE ) 
		ReSetRenderState() ;

	return S_OK ;
}
// test
HRESULT CRYLRectangle::DrawAC( INT X, INT Y, INT Width, INT Height, DWORD AlphaLT, DWORD ColorLT, DWORD AlphaRT, 
									 DWORD ColorRT, DWORD AlphaLB, DWORD ColorLB, DWORD AlphaRB, DWORD ColorRB ) 

{
	DWORD				 dwColor ;
	BYTE				*pData ;
	TRHWDiffVertex		*pVB = NULL ;

	if ( m_bBegin == FALSE )
		ReRectangleRenderState() ;

	if ( SUCCEEDED( m_hVB->Lock( 0, 0, &pData, 0 ) ) )
	{
		pVB = ( TRHWDiffVertex * ) pData ;

		pVB[ 0 ].Position	= D3DXVECTOR4( X, Y, m_fDepth, 1.0f ) ; // left top
		dwColor				= Color32toColor32( AlphaLT, ColorLT ) ;
		pVB[ 0 ].Color		= dwColor ;

		pVB[ 1 ].Position	= D3DXVECTOR4( X + Width, Y, m_fDepth, 1.0f ) ; // right top
		dwColor				= Color32toColor32( AlphaRT, ColorRT ) ;
		pVB[ 1 ].Color		= dwColor ;

		pVB[ 2 ].Position	= D3DXVECTOR4( X, Y + Height, m_fDepth, 1.0f ) ; // left bottom
		dwColor				= Color32toColor32( AlphaLB, ColorLB ) ;
		pVB[ 2 ].Color		= dwColor ;

		pVB[ 3 ].Position	= D3DXVECTOR4( X + Width, Y + Height, m_fDepth, 1.0f ) ; // right bottom
		dwColor				= Color32toColor32( AlphaRB, ColorRB ) ;
		pVB[ 3 ].Color		= dwColor ;

		if ( FAILED( m_hVB->Unlock() ) )
		{
			ReSetRenderState() ;
			return E_FAIL ;
		}
	}

	BaseGraphicsLayer::GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) ;
	// Draw
	if ( m_bBegin == FALSE )
		ReSetRenderState() ;

	return S_OK ;
}

HRESULT CRYLRectangle::DrawRECT( INT X, INT Y, INT Width, INT Height, INT Thickness, DWORD AlphaLT, DWORD AlphaRT, DWORD AlphaLB, DWORD AlphaRB, DWORD Color ) 
{
	// Top
	DrawA (X, Y, Width, Thickness, AlphaLT, AlphaRT, AlphaLT, AlphaRT, Color);
	// Left
	DrawA (X, Y, Thickness, Height, AlphaLT, AlphaLB, AlphaLT, AlphaLB, Color);
	// Right
	DrawA (X+Width-Thickness, Y, Thickness, Height, AlphaRT, AlphaRB, AlphaRT, AlphaRB, Color);
	// Bottom
	DrawA (X, Y+Height-Thickness, Width, Thickness, AlphaLB, AlphaLB, AlphaRB, AlphaRB, Color);
	return S_OK;
}



