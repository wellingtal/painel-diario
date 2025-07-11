//================================================================
//
//	Name : GMTexture.h 
//	Desc : GM Texture
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __GMTEXTURE_H__
#define __GMTEXTURE_H__

#include "GMObject.h"

class CGMTexture : public CGMObject
{
protected :

    INT			m_iWidth, m_iHeight ;
	DWORD		m_dwColor ;
	BYTE		m_Alpha ;
	BOOL		m_boVisible ;

	CGMTexture() ;
	virtual ~CGMTexture() ;

public:

    virtual LONG    Release() { delete this; return 0; }

	virtual VOID	Clear() ;

	virtual VOID	SetAlpha( BYTE Alpha )			{ m_Alpha = Alpha ; }
	virtual BYTE	GetAlpha()						{ return m_Alpha ; }
	
	virtual INT		GetWidth()						{ return m_iWidth ; }
	virtual INT		GetHeight()						{ return m_iHeight ; }

	virtual VOID	SetColor (DWORD dwColor)		{ m_dwColor = dwColor ; }

	virtual VOID	SetVisible (BOOL boVisible)		{ m_boVisible = boVisible ; }
	virtual BOOL	GetVisible ()					{ return m_boVisible ; }

	virtual VOID	Draw(INT X, INT Y)				{ }
	virtual VOID	DrawRect(RECT r)				{ }
};

#endif // __GMTEXTURE_H__
