#include "GMImageTarga.h"	
#include "GMStream.h"
#include "GMMemory.h"

CGMImageTarga::CGMImageTarga () : CGMImage()
{
	m_BitsPerSample		= 0 ;
	m_SamplesPerPixel	= 0 ;
	m_BitsPerPixel		= 0 ;
	m_ColorScheme		= csUnknown ;
	m_Compression		= ctUnknown ;
}	

CGMImageTarga::~CGMImageTarga ()
{
	m_BitsPerSample		= 0 ;
	m_SamplesPerPixel	= 0 ;
	m_BitsPerPixel		= 0 ;
	m_ColorScheme		= csUnknown ;
	m_Compression		= ctUnknown ;
}

VOID CGMImageTarga::Clear ()
{
	m_BitsPerSample		= 0 ;
	m_SamplesPerPixel	= 0 ;
	m_BitsPerPixel		= 0 ;
	m_ColorScheme		= csUnknown ;
	m_Compression		= ctUnknown ;

	CGMImage::Clear();
}

VOID CGMImageTarga::LoadFromStream( CGMFileStream *pStream ) 
{
	CGMImage::LoadFromStream( pStream ) ;
	Clear() ;
	Load( pStream, &m_pBits, m_GMImageDefaultData ) ;
}

VOID CGMImageTarga::LoadFromStream( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) 
{
	CGMImage::LoadFromStream( pStream, pOutBits, GMImageDefaultData ) ;
	Clear() ;
	Load( pStream, pOutBits, GMImageDefaultData ) ;
}

VOID CGMImageTarga::SaveToStream( CGMFileStream *pStream ) 
{
	CGMImage::SaveToStream( pStream ) ;
	Save( pStream, m_GMImageDefaultData, m_pBits ) ;
}

VOID CGMImageTarga::SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits )  
{
	CGMImage::SaveToStream( pStream, m_GMImageDefaultData, m_pBits ) ;
	Save( pStream, GMImageDefaultData, pBits ) ;
}


VOID CGMImageTarga::Load( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData )
{
	SGMIMAGETARGAHEADER		Header ;
	INT						i, iLineSize, pos ;
	BOOL					FlipV ;
	PBYTE					pBits ;
	//BYTE*					pbuf ;


	if ( !pOutBits )
		return ;
	pos = pStream->GetPostion() ;
	if ( ReadImageProperties( pStream, 0 ) == FALSE )
	{
		return ;
	}
	pStream->SeekBuffer(pos, FP_BEGIN);
	pStream->ReadBuffer(&Header, sizeof(SGMIMAGETARGAHEADER) );
	FlipV = ((Header.ImageDescriptor & 0x20) != 0 );
	Header.ImageDescriptor = Header.ImageDescriptor & 0xF;

	// skip image ID
	pStream->SeekBuffer(Header.IDLength, FP_CURRENT);
	GMImageDefaultData.Width = Header.Width;
	GMImageDefaultData.Height = Header.Height;

	iLineSize = GMImageDefaultData.Width * (Header.PixelSize / 8);

	switch (Header.ImageType)
	{ // 압축되지않은 TrueColor의 tga만 로드 // 차후에 압축과 다른 포맷두 추가하자..온제하나..음냐..
		case TARGA_EMPTY_IMAGE :// nothing to do here
			break;
		case TARGA_INDEXED_IMAGE :
			break;
		case TARGA_TRUECOLOR_IMAGE :
			GMImageDefaultData.ImageBitFormat = (EGMIMAGEBITFORMAT)Header.PixelSize;
			m_pTempBits = new BYTE [iLineSize * GMImageDefaultData.Height];
			pBits = m_pTempBits;

			if (FlipV)
			{
				for (i=0;i<GMImageDefaultData.Height; i++)
				{
					pStream->ReadBuffer (pBits, iLineSize);
					pBits = pBits + iLineSize;
				}
			}
			else
			{
				pBits = pBits + (GMImageDefaultData.Height * iLineSize);
				for (i=0;i<GMImageDefaultData.Height; i++)
				{
					pBits = pBits - iLineSize;
					pStream->ReadBuffer (pBits, iLineSize);
				}
			}
			break;
		case TARGA_BW_IMAGE :
			break;
		case TARGA_INDEXED_RLE_IMAGE :
			break;
		case TARGA_TRUECOLOR_RLE_IMAGE : 
			break;
		case TARGA_BW_RLE_IMAGE :
			break;
	}
	*pOutBits = m_pTempBits;
}

VOID CGMImageTarga::Save( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits )
{
}

BOOL CGMImageTarga::ReadImageProperties(CGMFileStream *pStream, DWORD ImageIndex)
{
	SGMIMAGETARGAHEADER Header;
	if (!pStream)
		return FALSE;

	pStream->ReadBuffer (&Header, sizeof(SGMIMAGETARGAHEADER));
	Header.ImageDescriptor = Header.ImageDescriptor & 0xF;

	m_GMImageDefaultData.Width = Header.Width;
	m_GMImageDefaultData.Height = Header.Height;

    m_BitsPerSample = 8;
	switch (Header.PixelSize)
	{
		case 8:
			if ( (Header.ImageType & TARGA_BW_IMAGE) || (Header.ImageType & TARGA_BW_RLE_IMAGE) )
			{
				m_ColorScheme = csG;
			}
			else
			{
				m_ColorScheme = csIndexed;
			}
			m_SamplesPerPixel = 1;
			break;
		case 15: case 16: // actually, 16 bit are meant being 15 bit
			m_ColorScheme = csRGB;
			m_BitsPerSample = 5;
			m_SamplesPerPixel = 3;
			break;
		case 24:
			m_ColorScheme = csRGB;
			m_SamplesPerPixel = 3;
			break;
		case 32:
			m_ColorScheme = csRGBA;
			m_SamplesPerPixel = 4;
			break;
	}

	m_BitsPerPixel = m_SamplesPerPixel * m_BitsPerSample;
	if ( (Header.ImageType == TARGA_BW_RLE_IMAGE) || (Header.ImageType == TARGA_INDEXED_RLE_IMAGE) || (Header.ImageType == TARGA_TRUECOLOR_RLE_IMAGE) )
	{
		m_Compression = ctRLE;
	}
	else
	{
		m_Compression = ctNone;
	}

	m_GMImageDefaultData.Width = Header.Width;
	m_GMImageDefaultData.Height = Header.Height;
	return TRUE;
}

