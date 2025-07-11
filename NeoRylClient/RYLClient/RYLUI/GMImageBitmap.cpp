#include "GMImageBitmap.h"
#include "GMStream.h"
#include "GMMemory.h"

VOID	WinVRGB( WORD n, WORD* r, WORD* g, WORD* b )
{
	*b = n & 0x1F;
	*g = (n >> 5) & 0x1F;
	*r = (n >> 10) & 0x1F;
}

WORD	WinRGB( WORD r, WORD g, WORD b )
{
	return  b + (g << 5) + (r << 10 ) ;
}

INT WidthBytes( int Width )
{
	return ( ( ( Width * 8 ) + 31 ) / 32 ) * 4 ;
}

VOID Change8BitTo16Bit( VOID* pSour, VOID* pDest, INT SourSize, TImgPalette Palette )
{
	BYTE* ps;
	WORD* pd;
	WORD r;
	WORD g;
	WORD b;
	WORD t;

	ps = ( BYTE* )pSour ;
	pd = ( WORD* )pDest ;

	for ( INT i = 0; i < SourSize; i++)
	{
		b = Palette[ *ps ].Red ;
		g = Palette[ *ps ].Green ;
		r = Palette[ *ps ].Blue ;
		t = r >> 3 ;

		if ( ( r != 0 ) && ( t == 0 ) )			r = 1 ;
		else									r = t ;

		t = g >> 3 ;
		if ( ( g != 0 ) && ( t == 0 ) )			g = 1 ;
		else									g = t ;

		t = b >> 3;
		if ( ( b != 0 ) && ( t == 0 ) )			b = 1 ;
		else									b = t ;

		*pd = WinRGB( r, g, b ) ;
		ps = ps + 1 ;
		pd = pd + 1 ;
	}
}

VOID Change16BitTo24Bit( VOID* pSour, VOID* pDest, INT SourSize )
{
	WORD *ps ;
	BYTE *pd ;
	WORD r ;
	WORD g ;
	WORD b ;

	ps = ( WORD* )pSour ;
	pd = ( BYTE* )pDest ;

	for ( INT i = 0 ; i < SourSize ; i ++ )
	{
		WinVRGB (*ps, &r, &g, &b);
		*pd = b * 8;
		 pd = pd + 1;
		*pd = g * 8;
		 pd = pd + 1;
		*pd = r * 8;
		 pd = pd + 1;
		 ps = ps + 1;
	}

}

VOID Change24BitTo16Bit( VOID* pSour, VOID* pDest, INT SourSize )
{
	BYTE *ps;
	WORD *pd;
	WORD r;
	WORD g;
	WORD b;

	ps = (BYTE*)pSour;
	pd = (WORD*)pDest;

	for ( INT i = 0 ; i < SourSize ; i ++ )
	{
		b = *ps / 8;
		ps++;
		g = *ps / 8;
		ps++;
		r = *ps / 8;
		ps++;

		*pd = WinRGB (r, g, b);
		pd++;
	}
}

VOID Change32BitTo16Bit( VOID* pSour, VOID* pDest, INT SourSize )
{
	BYTE *ps;
	WORD *pd;
	WORD r;
	WORD g;
	WORD b;
	WORD a;

	ps = (BYTE*)pSour;
	pd = (WORD*)pDest;

	for ( INT i = 0; i < SourSize; i++ )
	{
		b = *ps / 8;
		ps++;
		g = *ps / 8;
		ps++;
		r = *ps / 8;
		ps++;
		// 나중에 알파두 넣어보지 1Bit
		a = *ps / 8;
		ps++;

		*pd = WinRGB (r, g, b);
		pd++;
	}
}

VOID FlipBits( VOID* pSour, INT wbytes, INT Height )
{
	int size;
	BYTE * tmpBits; 
	BYTE * tb;
	BYTE * ps;
	size = wbytes * Height;

	tmpBits = new BYTE [size];

	memcpy (tmpBits, pSour, size);
	
	tb = tmpBits;
	tb = tb + size - wbytes;
	ps = (BYTE*)pSour;

	for ( INT i = 0; i < Height; i++ )
	{
		memcpy (ps, tb, wbytes);
		tb = tb - wbytes;
		ps = ps + wbytes;
	}

	delete [] tmpBits;
}

VOID BmpInfoToImgLibPal( INT ColorCount, TImgPalette Palette, BITMAPINFOHEADER bif, SIMGCOLOR *pILib)
{
	INT	n ;

	if ((ColorCount != 0) && (ColorCount != 256))
	{
		for (n = 0; n < ColorCount; n++ )
		{
			Palette[n].Red = pILib[n].Red; 
			Palette[n].Green = pILib[n].Green; 
			Palette[n].Blue = pILib[n].Blue;
		}
	}
	else 
	{
		for (n = 0; n < 256; n++)
		{
			Palette[n].Red = pILib[n].Red;
			Palette[n].Green = pILib[n].Green;
			Palette[n].Blue = pILib[n].Blue;
		}
	}
}


CGMImageBitmap::CGMImageBitmap () : CGMImage ()
{
	m_GMImageBitmapType = AIBT_NONE;
}

CGMImageBitmap::~CGMImageBitmap ()
{
	m_GMImageBitmapType = AIBT_NONE;

	CGMImage::Clear();
}

VOID CGMImageBitmap::Clear ()
{
	m_GMImageBitmapType = AIBT_NONE;

	CGMImage::Clear();
}

VOID CGMImageBitmap::LoadFromStream( CGMFileStream *pStream ) 
{
	CGMImage::LoadFromStream( pStream ) ;
	Clear() ;
	Load( pStream, &m_pBits, m_GMImageDefaultData ) ;
}

VOID CGMImageBitmap::LoadFromStream( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData) 
{
	CGMImage::LoadFromStream( pStream, pOutBits, GMImageDefaultData ) ;
	Clear() ;
	Load( pStream, pOutBits, GMImageDefaultData ) ;
}

VOID CGMImageBitmap::SaveToStream( CGMFileStream *pStream ) 
{
	CGMImage::SaveToStream( pStream ) ;
	Save( pStream, m_GMImageDefaultData, m_pBits ) ;
}

VOID CGMImageBitmap::SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits )  
{
	CGMImage::SaveToStream( pStream, m_GMImageDefaultData, m_pBits ) ;
	Save( pStream, GMImageDefaultData, pBits ) ;
}

VOID CGMImageBitmap::Load( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData )
{
	BITMAPFILEHEADER bmpheader ;
	BITMAPINFOHEADER bmpinfoheader ;
	int HeaderSize;
	int ImageSize;
	void* p;
	void* pbuf;
	TImgPalette Palette;
	SIMGCOLOR* pImgLib;

	pStream->ReadBuffer (&bmpheader, sizeof(BITMAPFILEHEADER));
	if (bmpheader.bfType != 0x4D42)
	{
		assert (FALSE);
		Clear();
		return;
	}

	HeaderSize = bmpheader.bfOffBits - sizeof(BITMAPFILEHEADER);
	HeaderSize = HeaderSize - sizeof(BITMAPINFOHEADER);
	pStream->ReadBuffer (&bmpinfoheader, sizeof(BITMAPINFOHEADER));

    GMImageDefaultData.Width = bmpinfoheader.biWidth;
    GMImageDefaultData.Height = abs (bmpinfoheader.biHeight);

	int n;
	n = HeaderSize / sizeof(SIMGCOLOR);

	pImgLib = new SIMGCOLOR[n];
	pStream->ReadBuffer (pImgLib, HeaderSize);

	ImageSize = bmpinfoheader.biSizeImage;

	if (ImageSize == 0)
	{
		ImageSize = (bmpinfoheader.biWidth * bmpinfoheader.biBitCount + 7) / 8 * abs (bmpinfoheader.biHeight);
	}

	if (!pOutBits)
		assert (FALSE);

	pbuf = new WORD [ImageSize];
	pStream->ReadBuffer (pbuf, ImageSize);

	switch(bmpinfoheader.biBitCount)
	{
		case 8:
			p = pbuf;
			pbuf = new WORD [GMImageDefaultData.Width * GMImageDefaultData.Height * 2];
			BmpInfoToImgLibPal (bmpinfoheader.biClrUsed, Palette, bmpinfoheader, pImgLib);
			Change8BitTo16Bit (p, pbuf, GMImageDefaultData.Width * GMImageDefaultData.Height, Palette);
			GM_DELETE_ARRAY (p);
			break;			
		case 16:
			break;
		case 24:
			p = pbuf;
			pbuf = new WORD [GMImageDefaultData.Width * GMImageDefaultData.Height * 2];
			Change24BitTo16Bit (p, pbuf, GMImageDefaultData.Width * GMImageDefaultData.Height);
			GM_DELETE_ARRAY (p);
			break;
		case 32:
            p = pbuf;
            pbuf = new WORD [GMImageDefaultData.Width * GMImageDefaultData.Height * 2];
            Change32BitTo16Bit (p, pbuf, GMImageDefaultData.Width*GMImageDefaultData.Height);
			GM_DELETE_ARRAY (p);
			break;
	}

	if (bmpinfoheader.biHeight > 0)
	{
		FlipBits (pbuf, GMImageDefaultData.Width*2, GMImageDefaultData.Height);
	}

	GM_DELETE_ARRAY (pImgLib);
	*pOutBits = pbuf;
}

VOID CGMImageBitmap::Save (CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits)
{
	//나중에 --!
	/*
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFO bmi;
	int iHeaderSize;

	iHeaderSize = sizeof (BITMAPINFO);
	bmi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_iWidth;
    bmi.bmiHeader.biHeight = -m_iHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 16;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

	if (bmi.bmiHeader.biHeight < 0)
	{
		FlipBits (m_pBits, m_iWidth *2, m_iHeight);
		bmi.bmiHeader.biHeight = - bmi.bmiHeader.biHeight;
	}
	memset (&bmpHeader, 0, sizeof(BITMAPFILEHEADER));
	bmpHeader.bfType = 0x4D42;
	bmpHeader.bfSize = 1078 + m_iWidth * m_iHeight * 2 - 1024;
	bmpHeader.bfOffBits = 1078 - 1024;

	pStream->WriteBuffer (&bmpHeader, sizeof (BITMAPFILEHEADER));
	pStream->WriteBuffer (&bmi, sizeof (BITMAPINFO));
	pStream->WriteBuffer (m_pBits, m_iWidth * m_iHeight * 2);
	*/
}


