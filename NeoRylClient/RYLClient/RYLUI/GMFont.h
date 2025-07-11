//================================================================
//
//	Name : RYLFont.h 
//	Desc : RYL Font Class
//  Date : 2003. 09. 18
//
//================================================================

#ifndef __RYLFONT_H__
#define __RYLFONT_H__

#include "GMBase.h"
#include "BaseGraphicsLayer.h"

class CGMFontTexture;
class CGMFont
{
protected:
	BOOL		m_bBegin ; 

public:
	static CGMFont*		s_pInstance ;
	static HRESULT		CreateInstance() ;
	static HRESULT		DeleteInstance() ;
	static BOOL			IsInited()			{ return s_pInstance != NULL; }
	static CGMFont*		Instance()			{ GMASSERT( s_pInstance, "CGMFont::Instance Faild" ) ; return s_pInstance ; }

public:
	CGMFont() ;
	virtual ~CGMFont() ;

	typedef HRESULT		( __stdcall *FGMFontInitDeviceObjects )( LPDIRECT3DDEVICE8 pd3dDevice ) ;
	typedef HRESULT		( __stdcall *FGMFontDeleteDeviceObjects )() ;			// 장치삭제
	typedef HRESULT		( __stdcall *FGMFontRestoreDeviceObjects )() ;			// 객체 복원
	typedef HRESULT		( __stdcall *FGMFontInvalidateDeviceObjects )() ;		// 객체 삭제
	typedef HRESULT		( __stdcall *FGMFontCleanup )() ;

	typedef HRESULT		( __stdcall *FGMCreateFont )( DWORD* phFont,LPCTSTR szFontName,INT nFontSize ) ;
	typedef HRESULT		( __stdcall *FGMReleaseFont )( DWORD hFont ) ;
	typedef HRESULT		( __stdcall *FGMDrawText )( DWORD hFont, INT X, INT Y, LPCTSTR szStr, DWORD dwColor ) ;
	typedef HRESULT		( __stdcall *FGMDrawAlphaText )( DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor ) ;
	typedef HRESULT		( __stdcall *FGMDrawBorderText )( DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor, DWORD dwBorderColor ) ;

	typedef HRESULT		( __stdcall *FGMCreateStringTexture )( DWORD* phTexture, INT Width, INT Height ) ;
	typedef HRESULT		( __stdcall *FGMReleaseStringTexture )( DWORD hTEXTURE ) ;
	typedef HRESULT		( __stdcall *FGMSetString )( DWORD hFont, DWORD hTexture, LPCTSTR szStr ) ;
	typedef HRESULT		( __stdcall *FGMDrawString )( DWORD hTexture, INT X, INT Y, DWORD dwColor ) ;
	typedef HRESULT		( __stdcall *FGMDrawAlphaString )( DWORD hTexture, INT X, INT Y, FLOAT fScaleX, FLOAT fScaleY, INT Alpha, DWORD dwColor ) ;
	typedef HRESULT		( __stdcall *FGMDrawBorderString )( DWORD hTexture, INT X, INT Y, FLOAT fScaleX, FLOAT fScaleY, DWORD Alpha, DWORD dwColor, DWORD dwBorderColor ) ;

	typedef DWORD		( __stdcall *FGMTextWidth )( DWORD hFont, LPCTSTR szStr ) ;
	typedef DWORD		( __stdcall *FGMTextHeight )( DWORD hFont, LPCTSTR szStr ) ;

protected:
	HINSTANCE						m_hGMFontLibrary;

	FGMFontInitDeviceObjects		m_fGMFontInitDeviceObjects; 
	FGMFontDeleteDeviceObjects		m_fGMFontDeleteDeviceObjects;
	FGMFontRestoreDeviceObjects		m_fGMFontRestoreDeviceObjects;
	FGMFontInvalidateDeviceObjects	m_fGMFontInvalidateDeviceObjects;
	FGMFontCleanup					m_fGMFontFinalCleanup;

	FGMCreateFont					m_fGMCreateFont;
	FGMReleaseFont					m_fGMReleaseFont;
	FGMDrawText						m_fGMDrawText;
	FGMDrawAlphaText				m_fGMDrawAlphaText;
	FGMDrawBorderText				m_fGMDrawBorderText ;

	FGMCreateStringTexture			m_fGMCreateStringTexture;
	FGMReleaseStringTexture			m_fGMReleaseStringTexture;
	FGMSetString					m_fGMSetString;
	FGMDrawString					m_fGMDrawString; 
	FGMDrawAlphaString				m_fGMDrawAlphaString;
	FGMDrawBorderString				m_fGMDrawBorderString;

	FGMTextWidth					m_fGMTextWidth;
	FGMTextHeight					m_fGMTextHeight;

public:
	HRESULT		LoadLibrary() ;
	HRESULT		FreeLibrary() ;

	HRESULT		InitDeviceObjects() ;
	HRESULT		DeleteDeviceObjects() ;
	HRESULT		RestoreDeviceObjects() ;
	HRESULT		InvalidateDeviceObjects() ;
	HRESULT		Clear() ;
	
	HRESULT		CreateFont( DWORD* phFont,LPCTSTR szFontName,INT nFontSize) ;
	HRESULT		ReleaseFont( DWORD hFont) ;

	HRESULT		DrawText( DWORD hFont, INT X, INT Y, LPCTSTR szStr, DWORD dwColor ) ;
	HRESULT		DrawAlphaText( DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor ) ;
	HRESULT		DrawBorderText( DWORD hFont, INT X, INT Y, LPCTSTR szStr, INT alpha, DWORD dwColor, DWORD dwBorderColor ) ;

	HRESULT		CreateStringTexture( DWORD* phTexture, INT Width, INT Height ) ;
	HRESULT		ReleaseStringTexture( DWORD& hTexture ) ;

	HRESULT		SetString( DWORD hFont, DWORD hTexture, LPCTSTR szStr ) ;

	HRESULT		DrawString( DWORD hTexture, INT X, INT Y, DWORD dwColor, INT DrawType = 0, INT Alpha = 255, DWORD dwBorderColor = 0x00000000 ) ; // was  INT Alpha = 255, DWORD dwBorderColor = 0x0000000

	DWORD		TextWidth( DWORD hFont, LPCTSTR szStr ) ;
	DWORD		TextHeight( DWORD hFont, LPCTSTR szStr ) ;

	VOID		CreateFontTexture( CGMFontTexture **ppOutFontTexture ) ;

	HRESULT		FontBegin() ;
	HRESULT		FontEnd() ;	
	BOOL		GetBegin()		{ GMASSERT( s_pInstance, "CGMFont::Instance Failed" ) ; return m_bBegin ; }
};


#endif