//================================================================
//
//	Name : RYLFontTexture.h 
//	Desc : RYL Font Texture ¤Ñ¤Ñ;
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __RYLFONTTEXTURE_H__
#define __RYLFONTTEXTURE_H__

#include "GMFontTexture.h"

class CRYLFontTexture : public CGMFontTexture
{
private:
	DWORD m_hFontTex;

public:
	CRYLFontTexture ();
	virtual ~CRYLFontTexture ();
	virtual VOID SetSize( INT Width, INT Height, BOOL boClear = TRUE ) ;
	virtual VOID SetText( LPSTR pText, LPSTR pFontName = NULL ) ;
	virtual VOID Draw( INT X, INT Y ) ;
};

#endif //__RYLFONTTEXTURE_H__