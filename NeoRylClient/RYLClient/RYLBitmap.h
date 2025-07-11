#ifndef __RYLBITMAL_H__
#define __RYLBITMAL_H__

#include <windows.h>

class CRYLBitmap
{
private:
	WORD*				m_pwBitmapPixels ;
	INT					m_iWidth ;
	INT					m_iHeight ;
	INT					m_iSize ;
	BYTE*				m_buffer ;
	
public:
	CRYLBitmap() ;
	virtual ~CRYLBitmap() ;
	
	VOID Clear() ;
	INT		Load( LPCTSTR pFileName, BYTE** pOutBits ) ;
	WORD*	GetImgBits()		{ return m_pwBitmapPixels ; }
	BYTE*	GetByteBits()		{ return m_buffer ; }
} ;

#endif //__RYLBITMAL_H__