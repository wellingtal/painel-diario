//================================================================
//
//	Name : GMString.h 
//	Desc : Gama Soft String Class
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMSTRING_H__
#define __GMSTRING_H__

#include "GMBase.h"

class CGMString 
{
private :
	enum ECONSTANTS
	{
		MAX_LOCAL_BUFFER_LEN = 128
	} ;

	CHAR		m_szBuffer[ MAX_LOCAL_BUFFER_LEN ] ;
	LPSTR		m_pstr ;
	INT			m_ilen ;

	VOID		SetString( INT iLen, LPCSTR pText ) ;
	VOID		AddString( LPCSTR pText ) ;
	VOID		DeleteString() ;
		
public:
	CGMString() ;
	CGMString( LPCSTR pText ) ;
	CGMString( const CGMString& str ) ;
	virtual ~CGMString() ;

	INT			Length() const ;
	VOID		LowerCase() { _strlwr( m_pstr ) ; } ;
	VOID		UpperCase() { _strupr( m_pstr ) ; } ;

	CGMString&	operator = ( const CGMString & str) ;
	CGMString&	operator = ( LPCSTR pText ) ; 
	CGMString&	operator + ( LPCSTR pText ) ; 
	CHAR&		operator [] ( INT iIndex ) const ;

	operator LPSTR() const ;

	friend BOOL operator >  ( const CGMString& str1, const CGMString& str2 ) ;
	friend BOOL operator <  ( const CGMString& str1, const CGMString& str2 ) ;
	friend BOOL operator == ( const CGMString& str1, const CGMString& str2 ) ;
	friend BOOL operator == ( const CGMString& str1, LPCSTR pText ) ;
	friend BOOL operator != ( const CGMString& str1, LPCSTR pText ) ;
} ;

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
LPSTR	LowerCase( LPSTR pStr ) ;
LPSTR	UpperCase( LPSTR pStr ) ;
INT		GetWindowsTextWidth( HDC hdc, LPSTR pstr ) ;
INT		GetWindowsTextHeight( HDC hdc, LPSTR pstr ) ;
VOID	GetWindowsTextExtent( HDC hdc, LPSTR pstr, INT& X, INT& Y ) ;

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
INT		strscan( CGMString& OutStr, LPSTR pSourStr, CHAR Divider ) ;
INT		strscan( CHAR** ppOutStr, LPSTR pSourStr, CHAR Divider ) ;

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
INT		strscan3( CHAR** ppOutStr, CHAR** ppDestStr, CHAR* pSourStr, CHAR Divider ) ;
INT		strscan3( CGMString& OutStr, CGMString& DestStr, CHAR* pSourStr, CHAR Divider);

//------------------------------------------------------------------------------
//                  strscanwidth
//------------------------------------------------------------------------------
INT		StrScanWidth( CGMString& OutStr, CGMString& DestStr, CGMString& SourStr, INT Width ) ;

#endif //__GM_STRING_H__