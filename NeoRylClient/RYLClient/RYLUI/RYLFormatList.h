//================================================================
//
//	Name : RYLFormatList.h 
//	Desc : RYL Image FormatList
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLFORMATLIST_H__
#define __RYLFORMATLIST_H__

#include "RYLTexture.h"

class CRYLTextureList ;
//------------------------------------------------------------------------------
//					CNAFormatList
//------------------------------------------------------------------------------
class CRYLFormatList
{
private:
	D3DFORMAT			m_FormatType ;
	CRYLTextureList*	pRYLTexture16List ;
	CRYLTextureList*	pRYLTexture32List ;
	CRYLTextureList*	pRYLTexture64List ;
	CRYLTextureList*	pRYLTexture128List ;
	CRYLTextureList*	pRYLTexture256List ;

public:
	CRYLFormatList( D3DFORMAT FormatType ) ;
	~CRYLFormatList() ; 

	HRESULT		CreateTexture( ERYLTexture_Size_Type WSizeType, ERYLTexture_Size_Type HSizeType, HRYLTexture *pphRYLTexture ) ;
	HRESULT		ReleaseTexture( HRYLTexture *pphRYLTexture ) ;
	
	HRESULT		InitDeviceObjects() ;
	HRESULT		DeleteDeviceObjects() ;
};

#endif //__RYLFORMATLIST_H__