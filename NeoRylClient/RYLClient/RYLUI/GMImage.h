//================================================================
//
//	Name : GMImage.h 
//	Desc : Gama Soft Image Base Class
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMIMAGE_H__
#define __GMIMAGE_H__

#include "GMObject.h"

#define GMIMAGE_EXT_FILE_FORMAT_TYPE_TABLE_COUNT	4


enum EGMIMAGEFILEFORMATTYPE
{
	EAIFFT_NONE,
	EAIFFT_DIB,
	EAIFFT_BMP,
	EAIFFT_TGA
};
 
enum EGMIMAGEBITFORMAT
{
	EAIF_NONE		= 0, 
	EAIF_8Bit		= 8, 
	EAIF_16Bit		= 16, 
	EAIF_24Bit		= 24, 
	EAIF_32Bit		= 32
};

typedef struct tagSGMIMAGEDEFAULTDATA {
	INT			Width ;
	INT			Height ;
	INT			offsetX ;
	INT			offsetY ;
	EGMIMAGEBITFORMAT ImageBitFormat ;
} SGMIMAGEDEFAULTDATA ;

class CGMFileStream ;
 
class CGMImage : public CGMObject 
{
protected :
	VOID*				m_pBits ;
	SGMIMAGEDEFAULTDATA	m_GMImageDefaultData ;
    VOID DrawEX( HDC DC, INT x, INT y, VOID* pBits ) ;

public:
	CGMImage() ;
	virtual ~CGMImage();
	
	virtual VOID Clear() ;

	VOID GetBits( VOID **pOutBits, INT &OutSize ) ;

	INT GetWidth() ;
	INT GetHeight() ;
	EGMIMAGEBITFORMAT GetImageBitFormat ();

	virtual INT		LoadFromFile( LPSTR pFileName ) ;
	virtual VOID	LoadFromStream( CGMFileStream *pStream ) ;
	virtual VOID	LoadFromStream( CGMFileStream *pStream, VOID **pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) ; 

	virtual VOID	SaveToFile( LPSTR pFileName ) ; 
	virtual VOID	SaveToStream( CGMFileStream *pStream ) ;
	virtual VOID	SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits ) ; 

	INT		GetIamgePixelSize( EGMIMAGEBITFORMAT ImageFormat ) ;
};


EGMIMAGEFILEFORMATTYPE GetGMImageFileFormatType( LPSTR pFileName ) ;

#endif