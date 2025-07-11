//================================================================
//
//	Name : RYLTextureList.h 
//	Desc : RYL Texture LIst Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLTEXTURELIST_H__
#define __RYLTEXTURELIST_H__

#include "RYLTexture.h"

class CGMList;
//------------------------------------------------------------------------------
//					CRYLTextureList
//------------------------------------------------------------------------------
class CRYLTextureList
{
public:
	
    CRYLTextureList( D3DFORMAT FormatType, ERYLTexture_Size_Type WSizeType ) ;
	virtual ~CRYLTextureList() ;

	HRESULT Clear () ;

	HRESULT CreateTexture( ERYLTexture_Size_Type HSizeType, HRYLTexture *pphNATexture ) ;
	HRESULT ReleaseTexture( HRYLTexture *pphRYLTexture ) ;

	HRESULT InitDeviceObjects() ;
	HRESULT DeleteDeviceObjects() ;

private :

	INT			IsUsedTexture( HRYLTexture phRYLTexture ) ;
	HRYLTexture IsUnUsedTexture( ERYLTexture_Size_Type HSizeType ) ;

    ERYLTexture_Size_Type	m_WSizeType ;
	D3DFORMAT				m_FormatType ;

	CGMList*				m_pUsedList ;
	CGMList*				m_pUnUsedList ;
};

#endif //__RYLTEXTURELIST_H__