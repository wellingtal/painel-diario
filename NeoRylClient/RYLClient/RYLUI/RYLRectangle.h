//================================================================
//
//	Name : RYLRectangle.h 
//	Desc : RYL UI Rectangle Class
//  Date : 2003. 09. 19
//
//=================================================================
#ifndef __RYLRECTANGLE_H__
#define __RYLRECTANGLE_H__

#include "BaseGraphicsLayer.h"

typedef struct tagTRHWDiffVertex 
{
	D3DXVECTOR4		Position ;
    DWORD			Color ;
} TRHWDiffVertex ;

#define FVF_RHWDIF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

class CRYLRectangle
{
private :
	BOOL			m_bBegin ;
	FLOAT			m_fDepth ;
	
	LPDIRECT3DVERTEXBUFFER8 m_hVB ;

public:
	static CRYLRectangle*  s_pInstance ;
	static HRESULT		   CreateInstance() ;
	static HRESULT		   DeleteInstance() ;
	static BOOL			   IsInited() ;
	static CRYLRectangle*  Instance() ;

public:
	CRYLRectangle();
	virtual ~CRYLRectangle();

	HRESULT		InitDeviceObjects() ;
	HRESULT		DeleteDeviceObjects() ;
	HRESULT		RestoreDeviceObjects()		{ return S_OK ; }
	HRESULT		InvalidateDeviceObjects()	{ return S_OK ; }

	VOID		SetDepth( FLOAT fDepth ) ;

	HRESULT		Begin() ;
	HRESULT		End() ;

	HRESULT		Draw( INT X, INT Y, INT Width, INT Height, DWORD Alpha, DWORD Color ) ;
	HRESULT		DrawA( INT X, INT Y, INT Width, INT Height, DWORD AlphaLT, DWORD AlphaRT, DWORD AlphaLB, DWORD AlphaRB, DWORD Color);
	HRESULT		DrawAC( INT X, INT Y, INT Width, INT Height, DWORD AlphaLT, DWORD ColorLT, DWORD AlphaRT, DWORD ColorRT, DWORD AlphaLB, DWORD ColorLB, DWORD AlphaRB, DWORD ColorRB ) ;
	HRESULT		DrawRECT( INT X, INT Y, INT Width, INT Height, INT Thickness, DWORD AlphaLT, DWORD AlphaRT, DWORD AlphaLB, DWORD AlphaRB, DWORD Color ) ;

private :
	HRESULT		ReSetRenderState() ;
	HRESULT		ReRectangleRenderState() ;
	HRESULT		CreateVB() ;
	HRESULT		ReleaseVB() ;
};


#endif