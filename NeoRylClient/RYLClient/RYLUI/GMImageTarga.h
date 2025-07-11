//================================================================
//
//	Name : GMImageBitmap.h 
//	Desc : Gama Soft Targa Image Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __GMIMAGETARGA_H__
#define __GMIMAGETARGA_H__

#include "GMImage.h"

//------------------------------------------------------------------------------
//					Targa Compression Type
//------------------------------------------------------------------------------
#define TARGA_NO_COLORMAP			0
#define TARGA_COLORMAP				1

#define TARGA_EMPTY_IMAGE			0
#define TARGA_INDEXED_IMAGE			1
#define TARGA_TRUECOLOR_IMAGE		2
#define TARGA_BW_IMAGE				3
#define TARGA_INDEXED_RLE_IMAGE		9
#define TARGA_TRUECOLOR_RLE_IMAGE	10
#define TARGA_BW_RLE_IMAGE			11

//------------------------------------------------------------------------------
//					Targa Compression Type
//------------------------------------------------------------------------------
enum ETARGA_COMPRESSION_TYPE {
	ctUnknown,     // compression type is unknown
	ctNone,        // no compression at all
	ctRLE,         // run length encoding
	ctPackedBits,  // Macintosh packed bits
	ctLZW,         // Lempel-Zif-Welch
	ctFax3,        // CCITT T.4 (1d), also known as fax group 3
	ctFaxRLE,      // modified Huffman (CCITT T.4 derivative)
	ctFax4,        // CCITT T.6, also known as fax group 4
	ctFaxRLEW,     // CCITT T.4 with word alignment
	ctLZ77,        // Hufman inflate/deflate
	ctJPEG,        // TIF JPEG compression (new version)
	ctOJPEG,       // TIF JPEG compression (old version)
	ctThunderscan, // TIF thunderscan compression
	ctNext,
	ctIT8CTPAD,
	ctIT8LW,
	ctIT8MP,
	ctIT8BL,
	ctPixarFilm,
	ctPixarLog,
	ctDCS,
	ctJBIG,
	ctPCDHuffmann  // PhotoCD Hufman compression
};

//------------------------------------------------------------------------------
//					Targa Color Type
//------------------------------------------------------------------------------
enum ETARGA_COLOR_SCHEME {
	csUnknown,   // not (yet) defined color scheme
	csIndexed,   // any palette format
	csG,         // gray scale
	csGA,        // gray scale with alpha channel
	csRGB,       // red, green, blue
	csRGBA,      // RGB with alpha channel
	csBGR,       // RGB in reversed order (used under Windows)
	csBGRA,      // BGR with alpha channel (alpha is always the last component)
	csCMY,       // cyan, magenta, yellow (used mainly for printing processes)
	csCMYK,      // CMY with black
	csCIELab,    // CIE color format using luminance and chromaticities
	csYCbCr,     // another format using luminance and chromaticities
	csPhotoYCC   // a modified YCbCr version used for photo CDs
};

#pragma pack (1)
typedef struct tagSGMIMAGETARGAHEADER {
	BYTE	IDLength, ColorMapType, ImageType ;
	WORD	ColorMapOrigin, ColorMapSize ;
	BYTE	ColorMapEntrySize ;
	WORD	XOrigin, YOrigin, Width, Height ;
	BYTE	PixelSize ;
	BYTE	ImageDescriptor ;
}SGMIMAGETARGAHEADER ;  
#pragma pack (8)

class CGMFileStream ;

class CGMImageTarga : public CGMImage
{
private :
	BYTE m_BitsPerSample, m_SamplesPerPixel, m_BitsPerPixel ;

	ETARGA_COLOR_SCHEME			m_ColorScheme ;
	ETARGA_COMPRESSION_TYPE		m_Compression ;
	BYTE*						m_pTempBits ;

	VOID Load( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) ;
	VOID Save( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits ) ;

public:
	CGMImageTarga() ;
	virtual ~CGMImageTarga() ;
	virtual VOID Clear() ;

	virtual VOID LoadFromStream( CGMFileStream *pStream); 
	virtual VOID LoadFromStream( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) ; 

	virtual VOID SaveToStream( CGMFileStream *pStream ) ; 
	virtual VOID SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits ) ; 

	BOOL ReadImageProperties( CGMFileStream *pStream, DWORD ImageIndex ) ;
};

#endif