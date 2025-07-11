//================================================================
//
//	Name : GMTexture.h 
//	Desc : GM Font Texture
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __GMFONTTEXTURE_H__
#define __GMFONTTEXTURE_H__

#include "GMTexture.h"

class CGMFontTexture : public CGMTexture
{
protected :
	CGMString	m_Text ;

	DWORD		m_dwClearColor ;
	DWORD		m_dwFontColor ;
	DWORD		m_dwBorderColor ;
	DWORD		m_dwShadowColor ;

public: 
    enum EFontDrawType
    {
		FontDrawType_NONE,
		FontDrawType_Alpha,
		FontDrawType_Border,
		FontDrawType_Shadow
    } ;
	EFontDrawType m_FontDrawType ;

public: 
	CGMFontTexture() ;
	virtual ~CGMFontTexture() ;

	virtual VOID	Clear() {} 

	virtual VOID	SetSize( INT Width, INT Height, BOOL boClear = TRUE ) {} 
	virtual VOID	SetText( LPSTR pText, LPSTR pFontName = NULL )				{ m_Text = pText ; }
	virtual LPSTR	GetText()													{ return m_Text ; } 
	virtual VOID	ClearByColor( DWORD dwColor )								{ m_dwClearColor = dwColor ; }
	virtual VOID	SetFontColor( DWORD dwFontColor )							{ m_dwFontColor = dwFontColor ; }
	virtual DWORD	GetFontColor()												{ return m_dwFontColor ; }
	virtual VOID	SetBorderColor( DWORD dwBorderColor )						{ m_dwBorderColor = dwBorderColor ; }
	virtual DWORD	GetBorderColor()											{ return m_dwBorderColor ; } 
	virtual VOID	SetShadowColor( DWORD dwShadowColor )						{ m_dwShadowColor = dwShadowColor ; }
	virtual DWORD	GetShadowColor()											{ return m_dwShadowColor ; }
	virtual VOID	SetFontDrawType( EFontDrawType DrawType )					{ m_FontDrawType = DrawType ; }
};

#endif // __GMFONTTEXTURE_H__
