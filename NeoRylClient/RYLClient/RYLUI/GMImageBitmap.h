//================================================================
//
//	Name : GMImageBitmap.h 
//	Desc : Gama Soft Bitmap Image Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __GMIMAGEBITMAP_H__
#define __GMIMAGEBITMAP_H__

#include "GMImage.h"

enum EGMIMAGEBITMAPTYPE
{
	AIBT_NONE	= 0, 
	AIBT_8		= 8, 
	AIBT_16		= 16, 
	AIBT_24		= 24, 
	AIBT_32		= 32
};

typedef struct tagSIMGCOLOR 
{
	BYTE	Red ;  
	BYTE	Green ;  
	BYTE	Blue ; 
	BOOL	Used ;
} SIMGCOLOR; 

typedef SIMGCOLOR TImgPalette[ 256 ] ;

class CGMFileStream ;

class CGMImageBitmap : public CGMImage
{
private :
	EGMIMAGEBITMAPTYPE m_GMImageBitmapType ;

	VOID Load( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) ;
	VOID Save( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits ) ;

public:
	CGMImageBitmap() ;
	virtual ~CGMImageBitmap() ;

	virtual VOID	Clear() ;

	virtual VOID	LoadFromStream( CGMFileStream *pStream ) ; 
	virtual VOID	LoadFromStream( CGMFileStream *pStream, VOID** pOutBits, SGMIMAGEDEFAULTDATA& GMImageDefaultData ) ; 

	virtual VOID	SaveToStream( CGMFileStream *pStream); 
	virtual VOID	SaveToStream( CGMFileStream *pStream, SGMIMAGEDEFAULTDATA GMImageDefaultData, VOID* pBits); 
} ;

#endif //__GMIMAGEBITMAP_H__

