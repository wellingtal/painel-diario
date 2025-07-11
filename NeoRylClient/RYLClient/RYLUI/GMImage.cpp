#include "GMImage.h"
#include "GMStream.h"
#include "GMString.h"
#include "GMMemory.h"

CGMImage::CGMImage () : CGMObject ()
{
	m_pBits								= NULL ;
	m_GMImageDefaultData.Width			= 0 ;
	m_GMImageDefaultData.Height			= 0 ;
	m_GMImageDefaultData.offsetX		= 0 ;
	m_GMImageDefaultData.offsetY		= 0 ;
	m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit ;
}
 
CGMImage::~CGMImage ()
{
	m_GMImageDefaultData.Width			= 0 ;
	m_GMImageDefaultData.Height			= 0 ;
	m_GMImageDefaultData.offsetX		= 0 ;
	m_GMImageDefaultData.offsetY		= 0 ;
	m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit ;
	GM_DELETE_ARRAY( m_pBits ) ;
}

VOID CGMImage::Clear ()
{
	GM_DELETE_ARRAY( m_pBits ) ;
	m_GMImageDefaultData.Width			= 0 ;
	m_GMImageDefaultData.Height			= 0 ;
	m_GMImageDefaultData.offsetX		= 0 ;
	m_GMImageDefaultData.offsetY		= 0 ;
	m_GMImageDefaultData.ImageBitFormat = EAIF_16Bit ;
}

VOID CGMImage::GetBits( VOID **pOutBits, INT &OutSize )
{
	*pOutBits = m_pBits ;
	OutSize = GetIamgePixelSize( m_GMImageDefaultData.ImageBitFormat ) ;
}

INT CGMImage::GetWidth ()
{
	return m_GMImageDefaultData.Width ;
}

INT CGMImage::GetHeight ()
{
   return m_GMImageDefaultData.Height ;
}

EGMIMAGEBITFORMAT CGMImage::GetImageBitFormat ()
{
   return m_GMImageDefaultData.ImageBitFormat;
}

INT CGMImage::LoadFromFile( LPSTR pFileName )
{
	Clear ();

	CGMFileStream *pStream = NULL ;

	pStream = new CGMFileStream ;
	INT n = pStream->Create( pFileName, O_LOAD ) ;

	if ( n == -1 )
	{
		GM_DELETE( pStream ) ;
		return -1 ;
	}

	LoadFromStream( pStream ) ;

	GM_DELETE( pStream ) ;

	return n ;
}

VOID CGMImage::LoadFromStream( CGMFileStream *pStream )
{
}

VOID CGMImage::LoadFromStream( CGMFileStream *pStream, VOID **pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData )
{
}

VOID CGMImage::SaveToFile( LPSTR pFileName )   
{
	CGMFileStream *pStream = NULL ;

	pStream = new CGMFileStream ;
	INT n = pStream->Create( pFileName, O_SAVE ) ;

	if (n == -1)
	{
		GM_DELETE( pStream ) ;
		return ;
	}

	SaveToStream( pStream ) ;
	GM_DELETE( pStream ) ;

	return ;
}

VOID CGMImage::SaveToStream( CGMFileStream *pStream )
{
}

VOID CGMImage::SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits)
{
}

INT CGMImage::GetIamgePixelSize( EGMIMAGEBITFORMAT ImageFormat)
{
	INT  n ;

	switch (ImageFormat)
	{
		case EAIF_NONE:			n = 0;					break ;
		case EAIF_8Bit:			n = sizeof( BYTE ) ;	break ;
		case EAIF_16Bit:		n = sizeof( WORD ) ;	break ;
		case EAIF_24Bit:		n = 0; 					break ;//³ªÁß¿¡
		case EAIF_32Bit:		n = sizeof( DWORD ) ;	break ;
	}
	return n ;
}

VOID CGMImage::DrawEX( HDC DC, INT x, INT y, VOID* pBits )
{
	if ( ( pBits == NULL) || ( DC == 0 ) )
		return ;

	if ( ( m_GMImageDefaultData.Width <= 0 ) || ( m_GMImageDefaultData.Height <= 0 ) )
		return ;

    BITMAPINFO bmi ;
    ZeroMemory( &bmi.bmiHeader,  sizeof( BITMAPINFOHEADER ) ) ;
    bmi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER ) ;
    bmi.bmiHeader.biWidth       =  m_GMImageDefaultData.Width ;
    bmi.bmiHeader.biHeight      = -m_GMImageDefaultData.Height ;
    bmi.bmiHeader.biPlanes      = 1 ;
    bmi.bmiHeader.biCompression = BI_RGB ;
    bmi.bmiHeader.biBitCount    = m_GMImageDefaultData.ImageBitFormat ;

	SetDIBitsToDevice( DC, x, y, m_GMImageDefaultData.Width, m_GMImageDefaultData.Height, 0, 0, 0, m_GMImageDefaultData.Height, pBits, &bmi, DIB_RGB_COLORS ) ; 
}



EGMIMAGEFILEFORMATTYPE GetGMImageFileFormatType( LPSTR pFileName)
{
	LPCTSTR		IMAGEFILEFORMATTYPETABLE[] = { "", ".dib", ".bmp", ".gmi"};
	TCHAR		strtmp[ 5 ] ;
	LPSTR		ptmp;
	
	ptmp = ExtractFileExt( pFileName ) ;
	strcpy( strtmp, ptmp ) ;
	LowerCase( strtmp ) ;

	for ( INT i = 0 ; i < GMIMAGE_EXT_FILE_FORMAT_TYPE_TABLE_COUNT ; i ++ )
	{
		if ( strcmp( IMAGEFILEFORMATTYPETABLE[i], strtmp ) == 0 )
		{
			return ( EGMIMAGEFILEFORMATTYPE )i ;
		}
	}
	
	return EAIFFT_NONE ;
}
