//================================================================
//
//	Name : RYLImage.h 
//	Desc : RYL UI Image
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLIMAGE_H__
#define __RYLIMAGE_H__

#include "RYLTextureManager.h"
#include "GMTexture.h"
#include "BaseGraphicsLayer.h"
#include <boost/pool/poolfwd.hpp>

#define FVF_RHWDIFTEX	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

typedef struct tagTRHWDiffTexVertex 
{
    D3DXVECTOR4		Position ;
    DWORD			Color ;
	FLOAT			tv, tu ;
}TRHWDiffTexVertex ;

typedef struct tagTImageData 
{
	INT			m_iImageWidth;
	INT			m_iImageHeight;
	HRYLTexture	phRYLTexture;
} TImageData;


class CGMImage ;
//------------------------------------------------------------------------------
//					CNAImage
//------------------------------------------------------------------------------
class CRYLImage : public CGMTexture
{
public:

	VOID Clear() ;

	INT			GetWidth()						{ return m_iWidth ; }
	INT			GetHeight()						{ return m_iHeight ; }
	D3DFORMAT	GetFormat()						{ return m_Format ; }
	VOID		SetFormat( D3DFORMAT Format )	{ if (m_phRYLTexture == NULL) m_Format = Format ; }

	HRESULT		LoadFromFile( LPSTR pFileName ) ;
	HRESULT		LoadFromImage( CGMImage* pGMImage ) ;

	HRESULT		Begin() ;
	HRESULT		End() ;

	LPSTR		GetFileName() ;
	VOID		SetDepth( FLOAT fDepth ) ;

	virtual HRESULT		InitDeviceObjects() ;
	virtual HRESULT		DeleteDeviceObjects() ;
	virtual VOID		Draw( INT X, INT Y ) ;
	virtual VOID		DrawRect( RECT rt, bool bInverse = false ) ;
	virtual VOID		DrawRect2( RECT rt, bool bInverse = false ) ;//hz guigui added this for oposite draw hp/mp bars
	virtual HRESULT		Draw( INT X, INT Y, DWORD Alpha, DWORD Color ) ;
	virtual HRESULT		DrawWH( INT X, INT Y, INT Width, INT Height, DWORD AlphaLT, DWORD ColorLT, DWORD AlphaRT, DWORD ColorRT, DWORD AlphaLB, DWORD ColorLB, DWORD AlphaRB, DWORD ColorRB ) ;
	virtual HRESULT		DrawScale( INT X, INT Y, FLOAT WScale, FLOAT HScale, DWORD Alpha, DWORD Color ) ;

    virtual LONG        Release();

	HRESULT SetSingleTexture( CGMImage* pGMImage ) ;
	HRESULT SetManyTexture( CGMImage* pGMImage ) ;

	HRESULT ReSetRenderState() ;
	HRESULT ReImageRenderState() ;

	HRESULT	CreateVB() ;
	HRESULT	ReleaseVB() ;

	HRESULT CreateTexture( INT Width, INT Height, HRYLTexture *phRYLTexture ) ;
	HRESULT ReleaseTexture( HRYLTexture phRYLTexture ) ;

protected:

	CRYLImage() ;
	virtual ~CRYLImage() ;

	virtual VOID SetTextureBits ( INT X, INT Y, 
								  INT SourWidth, INT SourHeight, VOID* pSourBits, INT SourBitsSize, 
								  INT DestWidth, INT DestHeight, VOID* pDestBits, INT DestBitsSize ) ;
private:

	static BOOL     s_bBegin;

    INT				m_iWCount, m_iHCount ;
	INT				m_iTextureCount ;
	DWORD			m_dwMaxSize ;
	FLOAT			m_fDepth ;
	INT				m_iWidth, m_iHeight ;
	D3DFORMAT		m_Format ;

	TImageData	   *m_pArrImageData ;
	CGMString		m_pFileName ;

	LPDIRECT3DVERTEXBUFFER8 m_hVB ;
	HRYLTexture				m_phRYLTexture ;

    friend class boost::object_pool<CRYLImage>;
};


#endif