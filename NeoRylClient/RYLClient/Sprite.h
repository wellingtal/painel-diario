// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__6D416805_02FD_4174_9D7E_4FE127EBEB62__INCLUDED_)
#define AFX_SPRITE_H__6D416805_02FD_4174_9D7E_4FE127EBEB62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <windows.h>
#include <texture.h>

class CSprite  
{
	public:
//		unsigned long m_dwTextureID;	// 텍스쳐 아이디
		long m_nPutX,m_nPutY;			// 화면상 좌표
		RECT m_rcRender;
		bool m_bShow;
		CTexture *m_lpTexture;
		float m_fRate;
		BOOL m_bHorizon;
//		LPDIRECT3DVERTEXBUFFER8 m_lpVB;

		CSprite();
		~CSprite();

		void SetPosition(long m_lPosX, long m_lPosY)	{ m_nPutX = m_lPosX; m_nPutY = m_lPosY; }
		long GetSizeX(void)								{ return (m_rcRender.right - m_rcRender.left); }
		long GetSizeY(void)								{ return (m_rcRender.bottom - m_rcRender.top); }
		void SetDirection(BOOL bHorizon)				{ m_bHorizon = bHorizon; }
		void SetRate(float fRate)
		{
			m_fRate = fRate;
			if(m_fRate < 0.0f) m_fRate = 0.0f; else if(m_fRate > 1.0f) m_fRate = 1.0f;
		}

		BOOL Create(long nPutX, long nPutY, long nTexX1, long nTexY1, long nTexX2, long nTexY2, CTexture *lpTexture);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned char Alpha = 0xFF, long fx = 0, long fy = 0);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice, float fDirection, unsigned char Alpha = 0xFF) ;
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned char Alpha, LPRECT pRect ) ;

		CTexture*	GetTextureEnable()				{ if ( this ) return m_lpTexture ; }
		VOID		SetRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ; 

};

#endif // !defined(AFX_SPRITE_H__6D416805_02FD_4174_9D7E_4FE127EBEB62__INCLUDED_)
