#include <winsock2.h>
#include "GMImageDib.h"
#include "GMStream.h"
#include "GMImageBitmap.h"
#include "GMImageTarga.h"
#include "RenderOption.h"
#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

CGMImageDib::CGMImageDib() : CGMImage()
{
	m_szFileName[ 0 ] = '\0' ;
	m_GMImageDefaultData.Width			= 0 ;
	m_GMImageDefaultData.Height			= 0 ;
	m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit ;
	m_GMImageFileFormatType				= EAIFFT_NONE ;
}

CGMImageDib::~CGMImageDib()
{
	m_GMImageDefaultData.Width			= 0 ;
	m_GMImageDefaultData.Height			= 0 ;
	m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit ;
	m_GMImageFileFormatType				= EAIFFT_NONE ;
}
 
VOID CGMImageDib::Clear ()
{
   m_GMImageDefaultData.Width			= 0 ;
   m_GMImageDefaultData.Height			= 0 ;
   m_GMImageDefaultData.ImageBitFormat	= EAIF_16Bit ;
   m_GMImageFileFormatType				= EAIFFT_NONE ;
}

BOOL CGMImageDib::GetEmpty ()
{
	return m_pBits == NULL ;
}

INT CGMImageDib::LoadFromFile( LPSTR pFileName )
{
	CGMFileStream *pStream ;
	EGMIMAGEFILEFORMATTYPE ImageFileFormatType ;

	pStream = new CGMFileStream ;
	ImageFileFormatType = GetGMImageFileFormatType( pFileName ) ;
	strcpy( m_szFileName, pFileName ) ;
	if ( pStream->Create( pFileName, O_LOAD ) == -1 )
	{
		return -1 ;
	}
	Load( ImageFileFormatType, pStream ) ;
	GM_DELETE( pStream ) ;
    return 0;
}

VOID CGMImageDib::SaveToFile( LPSTR pFileName )  
{
	CGMFileStream *pStream ;
	EGMIMAGEFILEFORMATTYPE ImageFileFormatType ;

	pStream = new CGMFileStream ;
	ImageFileFormatType = GetGMImageFileFormatType( pFileName ) ;
	pStream->Create( pFileName, O_SAVE ) ;
	Save( ImageFileFormatType, pStream ) ;
	GM_DELETE( pStream ) ;
}

VOID CGMImageDib::ClearbyColor( DWORD dwColor )
{
	INT i, count;
	WORD a, r, g, b;
	DWORD dwA, dwR, dwG, dwB;
	PWORD pw;
	PDWORD pdw;
	WORD wColor;
	if (!m_pBits)
		return;

	
	if (m_GMImageDefaultData.ImageBitFormat == EAIF_16Bit)
	{
		dwA = dwColor >> 24;
		dwR = dwColor & 0xFF;
		dwG = (dwColor & 0xFF00) >> 8;
		dwB = (dwColor & 0xFF0000) >> 16;

		a = WORD (dwA / 8);
		r = WORD (dwR / 8);
		g = WORD (dwG / 8);
		b = WORD (dwB / 8);
		wColor = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
		pw = (PWORD)m_pBits;
		count = m_GMImageDefaultData.Width * m_GMImageDefaultData.Height;
		for (i=0;i<count; i++)
		{
			*pw = wColor;
			pw++;
		}
		return;
	}

	if (m_GMImageDefaultData.ImageBitFormat == EAIF_32Bit)
	{
		pdw = (PDWORD)m_pBits;
		count = m_GMImageDefaultData.Width * m_GMImageDefaultData.Height;
		for (i=0;i<count; i++)
		{
			*pdw = dwColor;
			pdw++;
		}
		return;
	}
}

VOID	CGMImageDib::MakeFrameRect( INT iStartX, INT iStartY, 
									INT iRectWidth, INT iRectHeight, 
									DWORD dwOutColor, DWORD dwInColor, INT iPixel )
{
	DrawFrameRect( iStartX, iStartY, iRectWidth, iRectHeight, dwOutColor ) ;
	DrawFrameRect( iStartX + iPixel, iStartY + iPixel, iRectWidth - ( iPixel * 2 ), iRectHeight - ( iPixel * 2 ), dwInColor ) ;
}

VOID	CGMImageDib::DrawFrameRect( INT iX, INT iY, INT iRectWidth, INT iRectHeight, DWORD dwColor )
{
	INT count;
	WORD a, r, g, b;
	DWORD dwA, dwR, dwG, dwB;
	PWORD pw;
	PDWORD pdw;
	WORD wColor;
	if (!m_pBits)
		return;

	
	if (m_GMImageDefaultData.ImageBitFormat == EAIF_16Bit)
	{
		/*
		dwA = dwColor >> 24;
		dwR = dwColor & 0xFF;
		dwG = (dwColor & 0xFF00) >> 8;
		dwB = (dwColor & 0xFF0000) >> 16;

		a = WORD (0xff / 8);
		r = WORD (dwR / 8);
		g = WORD (dwG / 8);
		b = WORD (dwB / 8);
		wColor = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
		pw = (PWORD)m_pBits;
		
		count = m_GMImageDefaultData.Width * m_GMImageDefaultData.Height;
		for (i=0;i<count; i++)
		{
			*pw = wColor;
			pw++;
		}
		*/
		
		pw = (PWORD)m_pBits;
		PWORD		pDest ;
		INT iSize = m_GMImageDefaultData.ImageBitFormat ;
		
		pDest = (PWORD)m_pBits;
		pDest = pDest + ( ( iY * m_GMImageDefaultData.Width ) + iX ) ;
		INT iCount = 0 ;

		for ( int h = 0 ; h < iRectHeight; h++)
		{
			pw = pDest + ( m_GMImageDefaultData.Width * iCount ) ;
			for ( int w = 0 ; w < iRectWidth ; w++)
			{
				dwA = dwColor >> 24;
				dwR = dwColor & 0xFF;
				dwG = (dwColor & 0xFF00) >> 8;
				dwB = (dwColor & 0xFF0000) >> 16;

				a = WORD (0xff / 8);
				r = WORD (dwR / 8);
				g = WORD (dwG / 8);
				b = WORD (dwB / 8);
				wColor = (a <<= 15) | (b <<= 10) | (g <<= 5) | r;
				
				*pw = wColor ;
				++pw; 
			}
			++ iCount ;
		}
		return ;
	}

	if (m_GMImageDefaultData.ImageBitFormat == EAIF_32Bit)
	{
		pdw = (PDWORD)m_pBits;
		
		count = m_GMImageDefaultData.Width * m_GMImageDefaultData.Height;
		
		PDWORD		pDest ;
		INT iSize = GetIamgePixelSize(m_GMImageDefaultData.ImageBitFormat) ;
		
		pDest = (PDWORD)m_pBits;
		pDest = pDest + ( ( iY * m_GMImageDefaultData.Width ) + iX ) ;
		INT iCount = 0 ;

		for ( int h = 0 ; h < iRectHeight; h++)
		{
			pdw = pDest + ( m_GMImageDefaultData.Width * iCount ) ;
			for ( int w = 0 ; w < iRectWidth ; w++)
			{
				*pdw = dwColor ;
				++pdw; 
			}
			++ iCount ;
		}
	}
}

VOID CGMImageDib::SetSize( INT nWidth, INT nHeight, BOOL boClearColor, EGMIMAGEBITFORMAT ImageBitFormat ) 
{
	int imgsize;
	if ( (nWidth <= 0) || (nHeight <= 0))
		return;
	//=====================================================
	// Low texture도 32bit로 생성
	if ( BaseGraphicsLayer::m_iGraphicCards == 0 || 
		 BaseGraphicsLayer::m_iGraphicCards == 1 )
	{
		m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit;
	}
	else
	{
		m_GMImageDefaultData.ImageBitFormat = EAIF_32Bit;
	}
	
	imgsize = CGMImage::GetIamgePixelSize (m_GMImageDefaultData.ImageBitFormat);

	if(!imgsize)
	{
		return;
	}

	if ((nWidth == m_GMImageDefaultData.Width) && (nHeight == m_GMImageDefaultData.Height))
	{
		memset (m_pBits, 0, m_GMImageDefaultData.Width * m_GMImageDefaultData.Height * imgsize);
		return;
	}

	GM_DELETE_ARRAY (m_pBits);
	m_GMImageDefaultData.Width = nWidth;
	m_GMImageDefaultData.Height = nHeight;
	m_pBits = new BYTE [m_GMImageDefaultData.Width * m_GMImageDefaultData.Height * imgsize];

	if (boClearColor)
		memset (m_pBits, 0, m_GMImageDefaultData.Width * m_GMImageDefaultData.Height * imgsize);
}

VOID CGMImageDib::Draw( INT x, INT y, CGMImageDib* pDib, EGMIMAGEDIBDRAWTYPE DrawType )
{
	VOID*	pSour;
	INT		SourBitsSize;

	if (!pDib) 
		return;

	pDib->GetBits(&pSour, SourBitsSize);

	SetImageBits (x, y, pDib->GetWidth (), pDib->GetHeight (), pSour, SourBitsSize,
		0, 0, m_GMImageDefaultData.Width, m_GMImageDefaultData.Height, m_pBits, GetIamgePixelSize(m_GMImageDefaultData.ImageBitFormat));
}

VOID CGMImageDib::DrawRect( LPRECT lpRect, INT iX, INT iY, CGMImageDib* pDib, EGMIMAGEDIBDRAWTYPE DrawType,
							bool bColorKey, bool bAlphaEnable )
{
	VOID*	pSour;
	INT		SourBitsSize;

	if ( !pDib )  
		return ;

	if(!m_pBits)
		return ;

	pDib->GetBits(&pSour, SourBitsSize);
	/*
	if ( lpRect->right - lpRect->left == pDib->GetWidth() &&
		 lpRect->bottom - lpRect->top == pDib->GetHeight () )
	{
	*/
		SetImageBitsEx( lpRect->left, lpRect->top, 
						pDib->GetWidth(), pDib->GetHeight(),		    
						lpRect->right - lpRect->left, 
						lpRect->bottom - lpRect->top, 
						pSour, 
						SourBitsSize,
						iX, iY, 
						m_GMImageDefaultData.Width, 
						m_GMImageDefaultData.Height, 
						m_pBits, 
						GetIamgePixelSize(m_GMImageDefaultData.ImageBitFormat),
						bColorKey, bAlphaEnable ) ;
	/*
	}
	else
	{
		static char szBuff[ MAX_PATH ] ;
		sprintf( szBuff, "%s Load Filed!", pDib->GetFileName() ) ;
		MessageBox( NULL, szBuff, "Error", MB_OK ) ;
		
		CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
		PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
	}
	*/
}

VOID CGMImageDib::SetImageBitsEx( INT X, INT Y, 
								  INT RealSourWidth, INT RealSourHeight,
								  INT SourWidth, INT SourHeight, 
								  VOID* pSourBits, INT SourBitsSize,
								  INT X2, INT Y2,
								  INT DestWidth, INT DestHeight, 
								  VOID* pDestBits, INT DestBitsSize,
								  bool bColorKey, bool bAlphaEnable ) 
{
	PBYTE		pSour, ps;
	PBYTE		pDest, pd;
	PWORD		pwd, pws;
	PDWORD		pdwd, pdws;
	WORD		a, r, g, b;
	DWORD		dwA, dwR, dwG, dwB;
	int			h, w;
	RECT		sr, dr ;

	SetRect( &sr, X, Y, X + SourWidth, Y + SourHeight ) ;
	SetRect( &dr, X2, Y2, X2 + DestWidth, Y2 + DestHeight ) ;

	pSour = (PBYTE) pSourBits;
	pDest = (PBYTE) pDestBits;

	pDest = pDest + ( ( dr.left + dr.top * DestWidth ) * DestBitsSize ) ;
	pSour = pSour + ( ( sr.left + sr.top * RealSourWidth ) * SourBitsSize ) ;

	for (h = sr.top; h < sr.bottom; h++)
	{
		pd = pDest;
		ps = pSour;
		for (w = sr.left; w < sr.right; w++)
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

							if ( ( bColorKey ) &&
								 ( dwB == 0 ) &&
								 ( dwG == 0 ) &&
								 ( dwR == 0 ) )
							{
								break ;
							}
							
							a = WORD (0xff / 8);
							r = WORD (dwR / 8);
							g = WORD (dwG / 8);
							b = WORD (dwB / 8);

							if ( bAlphaEnable )
							{
								DWORD dwDstA, dwDstB, dwDstG, dwDstR ;
								WORD wDstA, wDstB, wDstG, wDstR ;

								dwDstA = *pwd >> 24;
								dwDstB = *pwd & 0xFF;
								dwDstG = (*pwd & 0xFF00) >> 8;
								dwDstR = (*pwd & 0xFF0000) >> 16;

								wDstA = WORD (0xff / 8);
								wDstR = WORD (dwDstG / 8);
								wDstG = WORD (dwDstG / 8);
								wDstB = WORD (dwDstB / 8);

								r = ( r >> 1 ) + ( wDstA >> 1 ) ;
								g = ( g >> 1 ) + ( wDstG >> 1 ) ;
								b = ( b >> 1 ) + ( wDstB >> 1 ) ;

								*pwd = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
							}
							else
							{
								*pwd = (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
							}
							break;
						case 4:
							pdws = (PDWORD) ps;
							pdwd = (PDWORD) pd;
							dwA = *pdws >> 24;
							if ( ( bColorKey ) && ( dwA == 0x00 ) )
							{
								break ;
							}	
							//--! 나주에 꼭 빼기..
							//dwA = 0xFFFFFFFF;
							dwB = *pdws & 0xFF;
							dwG = (*pdws & 0xFF00) >> 8;
							dwR = (*pdws & 0xFF0000) >> 16;
							
							if ( bAlphaEnable && pdwd != 0x00000000 )
							{
								DWORD dwDstA, dwDstB, dwDstG, dwDstR ;

								dwDstA = *pdwd >> 24;
								dwDstB = *pdwd & 0xFF;
								dwDstG = (*pdwd & 0xFF00) >> 8;
								dwDstR = (*pdwd & 0xFF0000) >> 16;

								dwB = ( dwB ) + ( dwDstB ) ;
								dwG = ( dwG ) + ( dwDstG ) ;
								dwR = ( dwR ) + ( dwDstR ) ;

								*pdwd = ( dwA <<= 24) | (dwR <<= 16) | (dwG <<= 8) | dwB;
							}
							else
							{
								*pdwd = (dwA <<= 24) | (dwR <<= 16) | (dwG <<= 8) | dwB;
							}							
							break;
					}
					break;
			}
			ps += SourBitsSize; 
			pd += DestBitsSize; 
		}
		pSour = pSour + (RealSourWidth * SourBitsSize);
		pDest = pDest + (DestWidth * DestBitsSize);
	}
}

VOID CGMImageDib::Draw( HDC DC, INT x, INT y )
{
	CGMImage::DrawEX( DC, x, y, m_pBits ) ;
}

VOID CGMImageDib::StretchDraw( CGMImageDib* pDib )
{
}

VOID CGMImageDib::StretchDraw( HDC DC )
{
}

VOID CGMImageDib::Load( EGMIMAGEFILEFORMATTYPE FormatType, CGMFileStream *pStream )
{
	GM_DELETE_ARRAY(m_pBits);

	CGMImage* pGMImage;
	pGMImage = NULL;

	switch (FormatType)
	{
		case EAIFFT_NONE:
			break;
		case EAIFFT_DIB:
			break;
		case EAIFFT_BMP:
			pGMImage = new CGMImageBitmap();
			pGMImage->LoadFromStream( pStream, &m_pBits, m_GMImageDefaultData);
			break;
		case EAIFFT_TGA:
			pGMImage = new CGMImageTarga();
			pGMImage->LoadFromStream( pStream, &m_pBits, m_GMImageDefaultData);
			break;
	}
	if (pGMImage)
		GM_DELETE (pGMImage);
}

VOID CGMImageDib::Save( EGMIMAGEFILEFORMATTYPE FormatType, CGMFileStream *pStream )
{
	CGMImage* pGMImage;
	pGMImage = NULL;

	switch (FormatType)
	{
		case EAIFFT_NONE:
			break;
		case EAIFFT_DIB:
			break;
		case EAIFFT_BMP:
			pGMImage = new CGMImageBitmap();
			pGMImage->SaveToStream (pStream, m_GMImageDefaultData, m_pBits);
			break;
		case EAIFFT_TGA:
			break;
	}
	if (pGMImage)
		GM_DELETE (pGMImage);
}

VOID CGMImageDib::SetImageBits( INT X, INT Y, 
								INT SourWidth, INT SourHeight, 
								VOID* pSourBits, INT SourBitsSize,
								INT X2, INT Y2,
								INT DestWidth, INT DestHeight, 
								VOID* pDestBits, INT DestBitsSize ) 
{
	PBYTE pSour, ps;
	PBYTE pDest, pd;
	PWORD pwd, pws;
	PDWORD pdwd, pdws;
	WORD a, r, g, b;
	DWORD dwA, dwR, dwG, dwB;
	int h, w;
	RECT sr, dr, ir;

	SetRect (&dr, X, Y, X+SourWidth, Y+SourHeight);
	SetRect (&sr, X2, Y2, DestWidth, DestHeight);
	if (IntersectRect (&ir, &sr, &dr) == 0) 
		return;

	pSour = (PBYTE) pSourBits;
	pDest = (PBYTE) pDestBits;

	pDest = pDest + ((ir.left+ir.top*DestWidth) * DestBitsSize);
	pSour = pSour + (((ir.left-X)+(ir.top-Y)*SourWidth) * SourBitsSize);

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
							//--! 나주에 꼭 빼기..
							//dwA = 0xFFFFFFFF;
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


