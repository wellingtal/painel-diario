//================================================================
//
//	Name : GMImageBitmap.h 
//	Desc : Gama Soft DIB Image Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __GMIMAGEDIB_H__
#define __GMIMAGEDIB_H__

#include "GMImage.h"

enum EGMIMAGEDIBDRAWTYPE
{
	EAIDT_NONE, 
	EAIDT_AlphaBlending, 
	EAIDT_Screen, 
	EAIDT_ColorDodge
};
 
class CGMFileStream ;

class CGMImageDib : public CGMImage
{
private:
	EGMIMAGEFILEFORMATTYPE m_GMImageFileFormatType ;
	char				   m_szFileName[ MAX_PATH ] ;

protected:
	VOID Load( EGMIMAGEFILEFORMATTYPE FormatType, CGMFileStream *pStream ) ;
    VOID Save( EGMIMAGEFILEFORMATTYPE FormatType, CGMFileStream *pStream ) ;

public:
	CGMImageDib();
	virtual ~CGMImageDib() ;
	virtual VOID Clear() ;

	BOOL GetEmpty() ;

	virtual INT LoadFromFile( LPSTR pFileName ) ;
	virtual VOID SaveToFile( LPSTR pFileName ) ; 

	VOID ClearbyColor( DWORD dwColor ) ;
	VOID MakeFrameRect( INT iStartX, INT iStartY, INT iRectWidth, INT iRectHeight, DWORD dwOutColor, DWORD dwInColor, INT iPixel ) ;
	VOID DrawFrameRect( INT iX, INT iY, INT iRectWidth, INT iRectHeight, DWORD dwColor ) ;
	VOID SetSize( INT nWidth, INT nHeight, BOOL boClearColor = TRUE, EGMIMAGEBITFORMAT ImageBitFormat = EAIF_16Bit ) ;

	VOID Draw( INT x, INT y, CGMImageDib* pDib, EGMIMAGEDIBDRAWTYPE DrawType = EAIDT_NONE ) ;
	VOID Draw( HDC DC, INT x, INT y ) ;
	VOID DrawRect( LPRECT lpRect, INT iX, INT iY, CGMImageDib* pDib, EGMIMAGEDIBDRAWTYPE DrawType = EAIDT_NONE, bool bColorKey = false, bool bAlphaEnable = false ) ;

	VOID StretchDraw( CGMImageDib* pDib ) ;
	VOID StretchDraw( HDC DC ) ;
	
	VOID SetImageBits( INT X, INT Y, 
					   INT SourWidth, INT SourHeight, 
					   VOID* pSourBits, INT SourBitsSize,
					   INT X2, INT Y2,
					   INT DestWidth, INT DestHeight, 
					   VOID* pDestBits, INT DestBitsSize ) ;
	
	VOID SetImageBitsEx( INT X, INT Y, 
						 INT RealSourWidth, INT RealSourHeight,			     
						 INT SourWidth, INT SourHeight, 
						 VOID* pSourBits, INT SourBitsSize,
					     INT  X2, INT Y2,
					     INT DestWidth, INT DestHeight, 
					     VOID* pDestBits, INT DestBitsSize,
						 bool bColorKey = false, bool bAlphaEnable = false ) ;

	char* GetFileName()		{ return m_szFileName ; }
};

#endif // __GMIMAGEDIB_H__