#include "BaseDataDefine.h"
#include "RYLImage.h"
#include "GMUIUtil.h"

#include "GMImage.h"
#include "GMImageDib.h"

#include "BaseGraphicsLayer.h"
#include "RYLTextureManager.h"
#include "RYLImageManager.h"
#include "GMMemory.h"


BOOL CRYLImage::s_bBegin = FALSE;

CRYLImage::CRYLImage()
{
    m_iWCount		= 0 ;
	m_iHCount		= 0 ;
	m_iTextureCount = 0 ;
	m_dwMaxSize		= 256 ;
	m_pArrImageData = NULL ;

	m_fDepth		= 0.0f ;
	m_iWidth		= 0 ;
	m_iHeight		= 0 ;
	m_Format		= D3DFMT_A1R5G5B5 ;
	m_phRYLTexture	= NULL ;
	m_hVB			= NULL ;
}

CRYLImage::~CRYLImage()
{
	Clear() ;
}

VOID CRYLImage::Clear()
{
	for ( INT i = 0 ;i < m_iTextureCount ; i ++ )
	{
		ReleaseTexture( m_pArrImageData[i].phRYLTexture ) ;
	}
	GM_DELETE_ARRAY( m_pArrImageData ) ;

	ReleaseVB() ;

    m_iWCount		= 0 ;
	m_iHCount		= 0 ;
	m_iTextureCount = 0 ;
	m_dwMaxSize		= 256 ;
	m_fDepth		= 0.0f ;
	m_iWidth		= 0 ;
	m_iHeight		= 0 ;
	m_Format		= D3DFMT_A1R5G5B5 ;
}

LONG CRYLImage::Release()
{
    CRYLImageManager::Instance()->DeleteRYLImage( this ) ;
    return 0;
}

HRESULT CRYLImage::ReSetRenderState ()
{
	//CAZTextureStateManager::Instance()->PopTextureState(5);
	//CAZRenderStateManager::Instance()->PopRenderState(3);
	/*
	if (m_iTextureCount > 1)
		CAZTextureStateManager::Instance()->PopTextureState(10);
	else
		CAZTextureStateManager::Instance()->PopTextureState(10);
	*/
	s_bBegin = FALSE ;

	return S_OK ;
}

HRESULT CRYLImage::ReImageRenderState ()
{
	if (m_hVB == NULL)
	{
		CreateVB();
	}

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;
	
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;
	
/*
	if (m_iTextureCount > 1)
	{
		CAZTextureStateManager::Instance()->PushTextureState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
		CAZTextureStateManager::Instance()->PushTextureState(0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);
	}
	else
	{
		CAZTextureStateManager::Instance()->PushTextureState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		CAZTextureStateManager::Instance()->PushTextureState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}
*/
	/*
	CAZTextureStateManager::Instance()->PushTextureState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    CAZTextureStateManager::Instance()->PushTextureState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    CAZTextureStateManager::Instance()->PushTextureState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	*/
	lpD3DDevice->SetStreamSource( 0, m_hVB, sizeof( TRHWDiffTexVertex ) );
	lpD3DDevice->SetVertexShader( NULL ) ;
	lpD3DDevice->SetVertexShader( FVF_RHWDIFTEX ) ;

	return S_OK ;
}

HRESULT	CRYLImage::CreateVB()
{
	if ( !m_hVB )
	{		
		if ( SUCCEEDED( BaseGraphicsLayer::GetDevice()->CreateVertexBuffer( 4 * sizeof( TRHWDiffTexVertex ), 
																		    D3DUSAGE_WRITEONLY, 
																			FVF_RHWDIFTEX,  
																			D3DPOOL_MANAGED,
																			&m_hVB ) ) )
		{
			return S_OK ;
		}
		else
		{
			GMASSERT( FALSE, "CRYLImage::CreateVertexBuffer Failed" ) ;
			return E_FAIL ;
		}
	}
	return S_OK ;
}

HRESULT	CRYLImage::ReleaseVB()
{
	GM_RELEASE( m_hVB ) ;
	/*if (m_hVB)
	{
		if ( SUCCEEDED( BaseGraphicsLayer::GetDevice()->ReleaseVertexBuffer( m_hVB ) ) )
		{
			m_hVB = NULL ;
			return S_OK ;
		}
	}*/

	return S_OK ;
}

HRESULT CRYLImage::CreateTexture( INT Width, INT Height, HRYLTexture *phRYLTexture )
{
	*phRYLTexture = CRYLTextureManager::Instance()->CreateRYLTexture( m_Format, GetTextureSize( Width ), GetTextureSize( Height ) ) ;
	// 차후에 ..
	//return CNATextureManager::Instance()->CreateTexture (m_Format, Width, Height, phRYLTexture);
	return S_OK ;
}

HRESULT CRYLImage::ReleaseTexture( HRYLTexture phRYLTexture )
{
    CRYLTextureManager::Instance()->DeleteRYLTexture ( phRYLTexture ) ;

	// 차후에 ..
	//return CNATextureManager::Instance()->ReleaseTexture (&phRYLTexture);
	return S_OK;
}

HRESULT CRYLImage::SetSingleTexture( CGMImage* pGMImage )
{
	VOID*	pSourBits ;
	PBYTE	pDestBits ;
	INT		SourBitsSize, DestBitsSize ;
	INT		iTexWidth ;

	CreateTexture( pGMImage->GetWidth(), pGMImage->GetHeight(), &m_pArrImageData[ 0 ].phRYLTexture ) ;
	pGMImage->GetBits( &pSourBits, SourBitsSize ) ;

	m_pArrImageData[ 0 ].m_iImageWidth  = pGMImage->GetWidth() ;
	m_pArrImageData[ 0 ].m_iImageHeight = pGMImage->GetHeight() ;

	DestBitsSize = 2 ;
	if ( m_Format != D3DFMT_A1R5G5B5 )
		DestBitsSize = 4 ;

	m_pArrImageData[ 0 ].phRYLTexture->LockRect( &pDestBits, iTexWidth ) ;

	SetTextureBits( 0, 0, pGMImage->GetWidth(), pGMImage->GetHeight(), pSourBits, SourBitsSize,
					m_pArrImageData[0].phRYLTexture->GetWidth(), m_pArrImageData[0].phRYLTexture->GetHeight(), pDestBits, DestBitsSize) ;

	m_pArrImageData[ 0 ].phRYLTexture->UnlockRect() ;

	return S_OK ;
}

HRESULT CRYLImage::SetManyTexture( CGMImage* pGMImage )
{
	INT			x, y, xw, yh, m, n, idx, iTexWidth ;
    INT			iWidth, iHeight ;
	INT			SourBitsSize, DestBitsSize ;
	PBYTE		pSourBits, pDestBits ;

	DestBitsSize = 2 ;
	if ( m_Format != D3DFMT_A1R5G5B5 )
		DestBitsSize = 4;

	if(!pGMImage)
	{
		return S_FALSE;
	}

	pGMImage->GetBits( ( VOID ** )&pSourBits, SourBitsSize ) ;

	y	= 0 ;
	yh	= m_iHeight ;
	idx = 0 ;

	for ( m = 0 ; m < m_iHCount ; m ++ )
	{
		yh = yh - m_dwMaxSize ;

		if ( yh < 0 )
			iHeight = m_iHeight % m_dwMaxSize ;
		else
			iHeight = m_dwMaxSize ;

		xw = m_iWidth ;
		x = 0;

		for ( n = 0 ; n < m_iWCount ; n ++ )
		{
			xw = xw - m_dwMaxSize ;
			if ( xw < 0 )
				iWidth = m_iWidth % m_dwMaxSize ;
			else
				iWidth = m_dwMaxSize ;

			CreateTexture( iWidth, iHeight, &m_pArrImageData[ idx ].phRYLTexture ) ;

			m_pArrImageData[idx].m_iImageWidth  = iWidth ;
			m_pArrImageData[idx].m_iImageHeight = iHeight ;
			m_pArrImageData[idx].phRYLTexture->LockRect( &pDestBits, iTexWidth ) ;

			SetTextureBits ( x, y, pGMImage->GetWidth(), pGMImage->GetHeight(), pSourBits, SourBitsSize,
							 m_pArrImageData[ idx ].phRYLTexture->GetWidth(), m_pArrImageData[ idx ].phRYLTexture->GetHeight(), pDestBits, DestBitsSize ) ;

			m_pArrImageData[ idx ].phRYLTexture->UnlockRect() ;
			if ( m_iWCount > 1 )
				x = x + m_dwMaxSize ;

			idx ++ ;
		}

		if ( m_iHCount > 1 )
			y = y + m_dwMaxSize ;
	}
	return S_OK ;
}

LPSTR CRYLImage::GetFileName ()
{
	return m_pFileName;
}

HRESULT CRYLImage::LoadFromFile( LPSTR pFileName )
{
	CGMImageDib* pGMImageDib ; 

	if ( strcmp( m_pFileName, pFileName ) != 0 )
		m_pFileName = pFileName ;
	else
		return E_FAIL ;
	pGMImageDib = new CGMImageDib();
	pGMImageDib->LoadFromFile( pFileName ) ;

	LoadFromImage( pGMImageDib ) ;

	GM_DELETE( pGMImageDib ) ;
	
	return S_OK;
}

HRESULT CRYLImage::LoadFromImage( CGMImage* pGMImage)
{
	GMASSERT( pGMImage, "CRYLImage::LoadFromImage Failed" ) ;
	
	if ( m_iTextureCount > 0 )  
		Clear() ;

	if ( pGMImage->GetImageBitFormat() == EAIF_16Bit )
		m_Format = D3DFMT_A1R5G5B5 ;
	if ( pGMImage->GetImageBitFormat() == EAIF_32Bit )
		m_Format = D3DFMT_A8R8G8B8 ;

	m_iWidth  = pGMImage->GetWidth() ;
	m_iHeight = pGMImage->GetHeight() ;

	m_iWCount = m_iWidth / m_dwMaxSize ;
	if ( ( m_iWidth % m_dwMaxSize ) > 0 )
		m_iWCount ++ ;
	m_iHCount = m_iHeight / m_dwMaxSize ;
	if ( ( m_iHeight % m_dwMaxSize ) > 0 )
		m_iHCount ++ ;
	
	m_iTextureCount = m_iWCount * m_iHCount ;
	if ( m_iTextureCount > 0 )
	{
		m_pArrImageData = new TImageData[ m_iTextureCount ] ;
	}

	if ( m_iTextureCount > 1 )
	{
		SetManyTexture( pGMImage ) ;
	}
	else
	{
		SetSingleTexture( pGMImage ) ;
	}

	return S_OK;
}

VOID CRYLImage::SetDepth (FLOAT fDepth)
{
	m_fDepth = fDepth;
}

HRESULT CRYLImage::InitDeviceObjects()
{
	if (m_hVB == NULL)
		CreateVB();
	return S_OK;
}

HRESULT CRYLImage::DeleteDeviceObjects()
{
	ReleaseVB();
	return S_OK;
}

HRESULT CRYLImage::Begin()
{
	ReImageRenderState();
	s_bBegin = TRUE;
	return S_OK;
}

HRESULT CRYLImage::End()
{
	ReSetRenderState();
	return S_OK;
}

HRESULT CRYLImage::DrawWH( INT X, INT Y, INT Width, INT Height, 
						   DWORD AlphaLT, DWORD ColorLT, DWORD AlphaRT, DWORD ColorRT, 
						   DWORD AlphaLB, DWORD ColorLB, DWORD AlphaRB, DWORD ColorRB ) 
{
	INT		addx, addy, tw, th, iw, ih ;
	BYTE*	pData ;
	FLOAT	fu, fv ;
	TRHWDiffTexVertex *pVB = NULL ;

	if ( m_pArrImageData == NULL )		
		return E_FAIL ;

	if ( m_pArrImageData[ 0 ].phRYLTexture == NULL ) 
		return E_FAIL ;

	if ( s_bBegin == FALSE )
		ReImageRenderState() ;

	addx = 0 ;
	addy = 0 ;

	iw = Width ;
	ih = Height ;
	tw = m_pArrImageData[ 0 ].phRYLTexture->GetWidth() ;
	th = m_pArrImageData[ 0 ].phRYLTexture->GetHeight() ;
	if ( SUCCEEDED( m_hVB->Lock(0, 0, &pData, 0 ) ) )
	{
		pVB = ( TRHWDiffTexVertex * )pData ;

		fv = 1 ;
		fu = 1 ;

		pVB[0].Position = D3DXVECTOR4 (X+addx, Y+addy, m_fDepth, 1.0f) ; // left top
		pVB[0].Color	= Color32toColor32 (AlphaLT, ColorLT);
		pVB[0].tu		= 0;
		pVB[0].tv		= 0;

		pVB[1].Position = D3DXVECTOR4 (X+addx+iw, Y+addy, m_fDepth, 1.0f) ; // right top
		pVB[1].Color	= Color32toColor32 (AlphaRT, ColorRT);
		pVB[1].tu		= 0;
		pVB[1].tv		= fv;

		pVB[2].Position = D3DXVECTOR4 (X+addx, Y+addy+ih, m_fDepth, 1.0f) ; // left bottom
		pVB[2].Color	= Color32toColor32 (AlphaLB, ColorLB);
		pVB[2].tu		= fu;
		pVB[2].tv		= 0;

		pVB[3].Position = D3DXVECTOR4 (X+addx+iw, Y+addy+ih, m_fDepth, 1.0f) ; // right bottom
		pVB[3].Color	= Color32toColor32 (AlphaRB, ColorRB);
		pVB[3].tu		= fu;
		pVB[3].tv		= fv;

		if ( FAILED(m_hVB->Unlock()))
		{
			ReSetRenderState();
			return S_FALSE;
		}
	}

	if ( m_pArrImageData[0].phRYLTexture )
		BaseGraphicsLayer::GetDevice()->SetTexture( 0, *m_pArrImageData[0].phRYLTexture ) ;

	BaseGraphicsLayer::GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) ;

	if ( s_bBegin == FALSE )
		ReSetRenderState() ;

	return S_OK ;

}

VOID CRYLImage::Draw (INT X, INT Y)
{
	Draw (X, Y, 255, 0xFFFFFFFF);
}

//hz guigui added for oposite dehp
VOID CRYLImage::DrawRect2 (RECT r, bool bInverse )
{
	INT addx, addy, idx, m, n, tw, th, iw, ih, X, Y, W, H;
	FLOAT fw, fh;
	DWORD dwColor;
	BYTE *pData ;
	FLOAT fu, fv;
	TRHWDiffTexVertex *pVB = NULL;

	X = r.left;
	Y = r.top;
	W = r.right - r.left;
	H = r.bottom - r.top;

	if (m_pArrImageData == NULL) return;
	dwColor = Color32toColor32 (255, 0xFFFFFFFF);

	if (s_bBegin == FALSE)
		ReImageRenderState();

	addy = 0 ;


	idx = 0;
	for (m=0;m<m_iHCount;m++)
	{
		addx = 0 ;
		
		for (n=0;n<m_iWCount;n++)
		{
			if (m_pArrImageData[idx].phRYLTexture == NULL) // @@ err
				continue;
			
			iw = m_pArrImageData[idx].m_iImageWidth;
			ih = m_pArrImageData[idx].m_iImageHeight;
			tw = m_pArrImageData[idx].phRYLTexture->GetWidth();
			th = m_pArrImageData[idx].phRYLTexture->GetHeight();
			if (W <= addx)
				continue;
			if (W < addx + iw)
				iw = W - addx;
			if (H < addy + ih)
				ih = H - addy;

			if (SUCCEEDED(m_hVB->Lock(0, 0, &pData, 0 )))
			{
				pVB = (TRHWDiffTexVertex *) pData;

				fv = ((FLOAT)iw / (FLOAT)tw );
				fu = ((FLOAT)ih / (FLOAT)th );

				
				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[0].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left top
				pVB[0].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[1].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right top
				pVB[1].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy-(FLOAT)ih;
				pVB[2].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left bottom
				pVB[2].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy-(FLOAT)ih;
				pVB[3].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right bottom
				pVB[3].Color	= dwColor;
					
				if ( !bInverse )
				{
					pVB[0].tu		= 0;
					pVB[0].tv		= 0;
					pVB[1].tu		= 0;
					pVB[1].tv		= fv;
					pVB[2].tu		= fu;
					pVB[2].tv		= 0;
					pVB[3].tu		= fu;
					pVB[3].tv		= fv;
				}
				else
				{
					pVB[0].tu		= fu;
					pVB[0].tv		= 0;
					pVB[1].tu		= fu;
					pVB[1].tv		= fv;
					pVB[2].tu		= 0;
					pVB[2].tv		= 0;
					pVB[3].tu		= 0;
					pVB[3].tv		= fv;
				}
				
				if ( FAILED(m_hVB->Unlock()))
				{
					ReSetRenderState();
					return;
				}
			}

			if (m_pArrImageData[idx].phRYLTexture)
				BaseGraphicsLayer::GetDevice()->SetTexture (0, *m_pArrImageData[idx].phRYLTexture);

			BaseGraphicsLayer::GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			addx = addx + tw;
			idx = idx + 1;
		}
		if (H <= addx)
			continue;
		addy = addy + th;
	}
	if (s_bBegin == FALSE)
		ReSetRenderState();
	return;
}

//hz end

VOID CRYLImage::DrawRect (RECT r, bool bInverse )
{
	INT addx, addy, idx, m, n, tw, th, iw, ih, X, Y, W, H;
	FLOAT fw, fh;
	DWORD dwColor;
	BYTE *pData ;
	FLOAT fu, fv;
	TRHWDiffTexVertex *pVB = NULL;

	X = r.left;
	Y = r.top;
	W = r.right - r.left;
	H = r.bottom - r.top;

	if (m_pArrImageData == NULL) return;
	dwColor = Color32toColor32 (255, 0xFFFFFFFF);

	if (s_bBegin == FALSE)
		ReImageRenderState();

	addy = 0 ;


	idx = 0;
	for (m=0;m<m_iHCount;m++)
	{
		addx = 0 ;
		
		for (n=0;n<m_iWCount;n++)
		{
			if (m_pArrImageData[idx].phRYLTexture == NULL) // @@ err
				continue;
			
			iw = m_pArrImageData[idx].m_iImageWidth;
			ih = m_pArrImageData[idx].m_iImageHeight;
			tw = m_pArrImageData[idx].phRYLTexture->GetWidth();
			th = m_pArrImageData[idx].phRYLTexture->GetHeight();
			if (W <= addx)
				continue;
			if (W < addx + iw)
				iw = W - addx;
			if (H < addy + ih)
				ih = H - addy;

			if (SUCCEEDED(m_hVB->Lock(0, 0, &pData, 0 )))
			{
				pVB = (TRHWDiffTexVertex *) pData;

				fv = ((FLOAT)iw / (FLOAT)tw );
				fu = ((FLOAT)ih / (FLOAT)th );

				
				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[0].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left top
				pVB[0].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[1].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right top
				pVB[1].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy+(FLOAT)ih;
				pVB[2].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left bottom
				pVB[2].Color	= dwColor;
				

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy+(FLOAT)ih;
				pVB[3].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right bottom
				pVB[3].Color	= dwColor;
					
				if ( !bInverse )
				{
					pVB[0].tu		= 0;
					pVB[0].tv		= 0;
					pVB[1].tu		= 0;
					pVB[1].tv		= fv;
					pVB[2].tu		= fu;
					pVB[2].tv		= 0;
					pVB[3].tu		= fu;
					pVB[3].tv		= fv;
				}
				else
				{
					pVB[0].tu		= fu;
					pVB[0].tv		= 0;
					pVB[1].tu		= fu;
					pVB[1].tv		= fv;
					pVB[2].tu		= 0;
					pVB[2].tv		= 0;
					pVB[3].tu		= 0;
					pVB[3].tv		= fv;
				}
				
				if ( FAILED(m_hVB->Unlock()))
				{
					ReSetRenderState();
					return;
				}
			}

			if (m_pArrImageData[idx].phRYLTexture)
				BaseGraphicsLayer::GetDevice()->SetTexture (0, *m_pArrImageData[idx].phRYLTexture);

			BaseGraphicsLayer::GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			addx = addx + tw;
			idx = idx + 1;
		}
		if (H <= addx)
			continue;
		addy = addy + th;
	}
	if (s_bBegin == FALSE)
		ReSetRenderState();
	return;
}

HRESULT CRYLImage::Draw (int X, int Y, DWORD Alpha, DWORD Color)
{
	int addx, addy, idx, m, n, tw, th, iw, ih;
	FLOAT fw, fh;
	DWORD dwColor;
	BYTE *pData ;
	FLOAT fu, fv;
	TRHWDiffTexVertex *pVB = NULL;
	if (m_pArrImageData == NULL) return S_FALSE;
	dwColor = Color32toColor32 (Alpha, Color);

	if (s_bBegin == FALSE)
		ReImageRenderState();

	addy = 0;
	idx = 0;
	for (m=0;m<m_iHCount;m++)
	{
		addx = 0;
		for (n=0;n<m_iWCount;n++)
		{
			if (m_pArrImageData[idx].phRYLTexture == NULL) // @@ err
				continue;

			iw = m_pArrImageData[idx].m_iImageWidth;
			ih = m_pArrImageData[idx].m_iImageHeight;
			tw = m_pArrImageData[idx].phRYLTexture->GetWidth();
			th = m_pArrImageData[idx].phRYLTexture->GetHeight();
			if (SUCCEEDED(m_hVB->Lock(0, 0, &pData, 0 )))
			{
				pVB = (TRHWDiffTexVertex *) pData;

				fv = ((FLOAT)iw / (FLOAT)tw );
				fu = ((FLOAT)ih / (FLOAT)th );

				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[0].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left top
				pVB[0].Color	= dwColor;
				pVB[0].tu		= 0;
				pVB[0].tv		= 0;

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy;
				pVB[1].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right top
				pVB[1].Color	= dwColor;
				pVB[1].tu		= 0;
				pVB[1].tv		= fv;

				fw = (FLOAT)X+(FLOAT)addx;
				fh = (FLOAT)Y+(FLOAT)addy+(FLOAT)ih;
				pVB[2].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // left bottom
				pVB[2].Color	= dwColor;
				pVB[2].tu		= fu;
				pVB[2].tv		= 0;

				fw = (FLOAT)X+(FLOAT)addx+(FLOAT)iw;
				fh = (FLOAT)Y+(FLOAT)addy+(FLOAT)ih;
				pVB[3].Position = D3DXVECTOR4 (fw-0.5f, fh-0.5f, m_fDepth, 1.0f) ; // right bottom
				pVB[3].Color	= dwColor;
				pVB[3].tu		= fu;
				pVB[3].tv		= fv;

				if ( FAILED(m_hVB->Unlock()))
				{
					ReSetRenderState();
					return S_FALSE;
				}
			}

			if (m_pArrImageData[idx].phRYLTexture)
				BaseGraphicsLayer::GetDevice()->SetTexture (0, *m_pArrImageData[idx].phRYLTexture);
			/*
			CTexture::SetPath(EFFECTTEXTUREPATH);
			static CTexture *pTex = NULL;
			if(pTex == NULL) {
				pTex = new CTexture;
				pTex->Load("4.dds");

			}
			BaseGraphicsLayer::GetDevice()->SetTexture (0,pTex->GetTexture());
			*/
			BaseGraphicsLayer::GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			addx = addx + tw;
			idx = idx + 1;
		}
		addy = addy + th;
	}
	if (s_bBegin == FALSE)
		ReSetRenderState();
	return S_OK;
}

HRESULT CRYLImage::DrawScale (int X, int Y, FLOAT WScale, FLOAT HScale, DWORD Alpha, DWORD Color)
{
	return S_OK;
}

VOID CRYLImage::SetTextureBits (
     					  int X, int Y, 
						int SourWidth, int SourHeight, void* pSourBits, int SourBitsSize, 
						int DestWidth, int DestHeight, void* pDestBits, int DestBitsSize)
{
	PBYTE pSour, ps;
	PBYTE pDest, pd;
	PWORD pwd, pws;
	PDWORD pdwd, pdws;
	WORD a, r, g, b;
	DWORD dwA, dwR, dwG, dwB;
	int h, w;
	RECT sr, dr, ir;

	SetRect (&dr, X, Y, X+DestWidth, Y+DestHeight);
	SetRect (&sr, 0, 0, SourWidth, SourHeight);
		
	if (IntersectRect (&ir, &sr, &dr) == 0)
	{
		return;
	}	

	pSour = (PBYTE) pSourBits;
	pDest = (PBYTE) pDestBits;

	pSour = pSour + ((ir.left+ir.top*SourWidth) * SourBitsSize);
	pDest = pDest + (((ir.left-X)+(ir.top-Y)*DestWidth) * DestBitsSize);

	for (h = ir.top; h <ir.bottom; h++)
	{
		pd = pDest;
		ps = pSour;
		for (w = ir.left; w <ir.right; w++)
		{
			switch (SourBitsSize)
			{
 				case 2:
					switch (DestBitsSize)
					{
						case 2:
							pws = (PWORD) ps;
							pwd = (PWORD) pd;
							if (*pws != 0x0000) a = 255;
							else a = 0;
							b = *pws & 0x1F;
							g = (*pws >>= 5) & 0x1F;
							r = (*pws >>= 5) & 0x1F;
							*pwd = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
							break;
						case 4:
							pws = (PWORD) ps;
							pdwd = (PDWORD) pd;

							if (*pws != 0x0000) dwA = 0xFFFFFFFF;
							else dwA = 0;
							b = *pws & 0x1F;
							g = (*pws >>= 5) & 0x1F;
							r = (*pws >>= 5) & 0x1F;

							dwR = r * 8;
							dwG = g * 8;
							dwB = b * 8;
							*pdwd = (dwA <<= 24) | (dwR <<= 16) | (dwG <<= 8) | dwB;
							break;
					}
					break;
				case 4:
					switch (DestBitsSize)
					{
						case 2:
							pdws = (PDWORD) ps;
							pwd = (PWORD) pd;
							dwA = *pdws >> 24;
							dwB = *pdws & 0xFF;
							dwG = (*pdws & 0xFF00) >> 8;
							dwR = (*pdws & 0xFF0000) >> 16;

							a = WORD (dwA / 8);
							r = WORD (dwR / 8);
							g = WORD (dwG / 8);
							b = WORD (dwB / 8);
							*pwd = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
							break;
						case 4:
							pdws = (PDWORD) ps;
							pdwd = (PDWORD) pd;
							dwA = *pdws >> 24;
							dwB = *pdws & 0xFF;
							dwG = (*pdws & 0xFF00) >> 8;
							dwR = (*pdws & 0xFF0000) >> 16;
							*pdwd = (dwA <<= 24) | (dwR <<= 16) | (dwG <<= 8) | dwB;
							break;
					}
					break;
			}
			ps += SourBitsSize; 
			pd += DestBitsSize; 
		}
		pSour = pSour + (SourWidth * SourBitsSize);
		pDest = pDest + (DestWidth * DestBitsSize);
	}
}
