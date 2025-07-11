//================================================================
//
//	Name : RYLTextureManager.h 
//	Desc : RYL Texture Manager Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLTEXTUREMANAGER_H__
#define __RYLTEXTUREMANAGER_H__

#include "RYLTexture.h"
#include <boost/pool/object_pool.hpp>

class CRYLFormatList ;

class CRYLTextureManager
{
public:

    static CRYLTextureManager*	s_pInstance ;
	static CRYLTextureManager*	Instance() ;
	static BOOL					IsInited() ;
	static HRESULT				CreateInstance() ;
	static HRESULT				DeleteInstance() ;
	
public:

	CRYLTextureManager() ;
	virtual ~CRYLTextureManager () ;

    CRYLTexture* CreateRYLTexture( D3DFORMAT Format, INT Width, INT Height );
    void DeleteRYLTexture(CRYLTexture* lpRYLTexture);

	HRESULT		Clear() ;
	HRESULT		CreateTexture( D3DFORMAT Format, INT Width, INT Height, HRYLTexture *pphRYLTexture ) ;
	HRESULT		ReleaseTexture( HRYLTexture *pphRYLTexture ) ;

	HRESULT		InitDeviceObjects() ;
	HRESULT		DeleteDeviceObjects() ;
	HRESULT		RestoreDeviceObjects()		{ return S_OK ; }
	HRESULT		InvalidateDeviceObjects()	{ return S_OK ; }

private:

    CRYLFormatList* pRYLFormat1555List ;
	CRYLFormatList* pRYLFormat8888List ;
	CRYLFormatList* pRYLFormat4444List ;
    
    boost::object_pool<CRYLTexture> m_RYLTexturePool;
};

#endif