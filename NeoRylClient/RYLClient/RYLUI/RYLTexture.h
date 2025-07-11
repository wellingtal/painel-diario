//================================================================
//
//	Name : RYLTexture.h 
//	Desc : RYL Texture Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLTEXTURE_H__
#define __RYLTEXTURE_H__

#include "GMObjectDef.h"
#include "BaseGraphicsLayer.h"
#include <boost/pool/poolfwd.hpp>

typedef enum _ERYLTexture_Size_Type
{
	RYLTEXTURESIZETYPE_16	= 16,
	RYLTEXTURESIZETYPE_32	= 32,
	RYLTEXTURESIZETYPE_64	= 64,
	RYLTEXTURESIZETYPE_128	= 128,
	RYLTEXTURESIZETYPE_256	= 256
} ERYLTexture_Size_Type ;


typedef struct tagTRYLTextureParameters 
{
	UINT		uiWidth ;  // 텍스쳐의 크기 : 가로 픽셀수 
	UINT		uiHeight ; // 텍스쳐의 크긔 : 세로 픽셀수 
	UINT		uiLevels ; // 밉멥 레벨
	D3DFORMAT	Format ; 

}TRYLTextureParameters ;

class CRYLTexture
{
public:

    HRESULT		LockRect( BYTE** ppBits, INT& Pitch ) ;
	HRESULT		UnlockRect() ;

	D3DFORMAT	GetFormat() ;
	INT			GetWidth() ;
	INT			GetHeight() ;

	HRESULT		InitDeviceObjects() ;
	HRESULT		DeleteDeviceObjects() ;
	HRESULT		InvalidateDeviceObjects() ;
	HRESULT		RestoreDeviceObjects() ;

	operator LPDIRECT3DTEXTURE8() const { return LPDIRECT3DTEXTURE8( m_pd3dTexture ) ; }

protected:

	CRYLTexture( D3DFORMAT Format, INT Width, INT Height ) ;
	~CRYLTexture() ;

	HRESULT CreateTexture() ;
	HRESULT ReleaseTexture() ;

	LPDIRECT3DBASETEXTURE8	m_pd3dTexture ;
	TRYLTextureParameters	m_RYLTextureParameters ;

    friend class boost::object_pool<CRYLTexture>;
};

typedef CRYLTexture* HRYLTexture ;



#endif //__RYLTEXTURE_H__