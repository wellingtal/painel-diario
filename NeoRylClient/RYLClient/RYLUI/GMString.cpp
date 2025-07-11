#include "GMString.h"
#include "GMMemory.h"


//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
static HWND Focus = 0;

INT		StrScanWidth( CGMString& OutStr, CGMString& DestStr, CGMString& SourStr, INT Width ) 
{
	INT			ilen, x ;
	LPSTR		pbuf ;
	LPSTR		pDestbuf ;
	HDC			hdc ;
	CHAR		c, str[ 3 ] ;
	

	if ( Focus == 0 )
		Focus = ::GetFocus() ;
	hdc = GetDC( Focus ) ;

	x = 0 ;
	ilen = strlen( SourStr ) ;

	INT			idx		= 0 ;
	BOOL		bUni	= FALSE ;
	
	while( TRUE )
	{
		if ( idx >= ilen )		break ;

		c = SourStr[ idx ] ;
		if ( Width < x ) 
		{
			pbuf = new CHAR[ idx + 1 ] ;
			strncpy( pbuf, SourStr, idx ) ;
			pbuf[idx] = '\0';
			OutStr = pbuf;
			GM_DELETE_ARRAY (pbuf);

			LPSTR	pSourStr ;
			pSourStr = SourStr ;
			pDestbuf = new CHAR[ ilen - idx + 1 ] ;
			strncpy( pDestbuf, pSourStr + idx, ilen - idx ) ;
			pDestbuf[ ilen - idx ] = '\0' ;
			DestStr = pDestbuf ;
			GM_DELETE_ARRAY( pDestbuf ) ;
			return idx ;
		}

		if ( ( c < 127 ) && ( c > 0 ) )
		{
			str[ 0 ] = SourStr[ idx ] ;
			idx ++ ;
			str[ 1 ] = '\0' ;
			bUni = FALSE ;
		}
		else
		{
			str[ 0 ] = SourStr[ idx ] ;
			idx ++ ;
			str[ 1 ] = SourStr[ idx ] ;
			idx ++ ;
			str[ 2 ] = '\0' ;
			bUni = TRUE ;
		}
		x = x + GetWindowsTextWidth( hdc, str ) ;
	}
	OutStr  = SourStr ;
	SourStr = "" ;
	DestStr = "" ;

	return ilen ;
}

INT		strscan3( CGMString& OutStr, CGMString& DestStr, CHAR* pSourStr, CHAR Divider)
{
	INT		ilen ;
	LPSTR	pbuf ;
	LPSTR	pDestbuf ;

	ilen = strlen( pSourStr ) ;
	for ( INT i = 0 ; i < ilen ; i ++ )
	{
		if ( pSourStr[ i ] == Divider )
		{
			pbuf = new CHAR[ i + 1 ] ;
			strncpy( pbuf, pSourStr, i ) ;
			pbuf[ i ] = '\0' ;
			OutStr = pbuf ;
			GM_DELETE_ARRAY( pbuf ) ;
   
			pDestbuf = new CHAR[ ilen - i ] ;
			strncpy( pDestbuf, pSourStr + i + 1, ilen - i -1 ) ;
			pDestbuf[ ilen - i - 1 ] = '\0' ;
			DestStr = pDestbuf ;
			GM_DELETE_ARRAY( pDestbuf ) ;
			
			return i + 1 ; // Divider까지 포함..
		}
	}
	OutStr	= pSourStr ;
	DestStr = "" ;

	return ilen;
}

INT		strscan3( CHAR** ppOutStr, CHAR** ppDestStr, CHAR* pSourStr, CHAR Divider ) 
{
	INT		ilen ;
	LPSTR	pbuf		= NULL ;
	LPSTR	pDestbuf	= NULL ;

	ilen = strlen( pSourStr ) ;
	for ( INT i = 0 ;i < ilen ; i ++ )
	{
		if ( pSourStr[ i ] == Divider )
		{
			pbuf = new CHAR[ i + 1 ] ;
			strncpy( pbuf, pSourStr, i ) ;
			pbuf[ i ] = '\0' ;
			*ppOutStr = pbuf ;
			pDestbuf = new CHAR[ ilen - i ] ;
			strncpy( pDestbuf, pSourStr + i + 1, ilen - i - 1 ) ;
			pDestbuf[ ilen - i - 1 ] = '\0' ;
			*ppDestStr = pDestbuf ;
			return i + 1 ; // Divider까지 포함..
		}
	}

	pDestbuf = new CHAR[ ilen + 1] ;
	strcpy( pDestbuf, pSourStr ) ;
	pDestbuf[ ilen ] = '\0' ;
	*ppDestStr	= pDestbuf ;
	*ppOutStr	= NULL ;

	return ilen ;
}

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
INT		strscan( CHAR** ppOutStr, LPSTR pSourStr, CHAR Divider ) 
{
	INT		ilen ;
	LPSTR	pbuf ;

	ilen = strlen( pSourStr ) ;
	for ( INT i = 0 ; i < ilen ; i ++ )
	{
		if ( pSourStr[ i ] == Divider )
		{
			pbuf = new CHAR[ i + 1 ] ;
			strncpy( pbuf, pSourStr, i ) ;
			pbuf[ i ] = '\0' ;
			*ppOutStr = pbuf ;

			return i ;
		}
	}
	return ilen ;
}

INT		strscan( CGMString& OutStr, LPSTR pSourStr, CHAR Divider ) 
{
	INT		ilen ;
	LPSTR	pbuf ;

	ilen = strlen( pSourStr ) ;
	for ( INT i = 0 ;i < ilen ; i ++ )
	{
		if ( pSourStr[ i ] == Divider )
		{
			pbuf = new CHAR[ i + 1 ] ;
			strncpy( pbuf, pSourStr, i ) ;
			pbuf[ i ] = '\0' ;
			OutStr = pbuf ;
			GM_DELETE_ARRAY( pbuf ) ;
			return i ;
		}
	}

	return ilen ;
}

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
LPSTR	LowerCase( LPSTR pStr ) 
{
	return _strlwr( pStr ) ;
}

LPSTR	UpperCase( LPSTR pStr )
{
	return _strupr( pStr ) ;
}

//------------------------------------------------------------------------------
//					String Function
//------------------------------------------------------------------------------
INT		GetWindowsTextWidth( HDC hdc, LPSTR pstr )
{
	SIZE	size ;
	GetTextExtentPoint32( hdc, pstr, strlen( pstr ), &size ) ;

	return size.cx ;
}

INT		GetWindowsTextHeight( HDC hdc, LPSTR pstr ) 
{
	SIZE	size ;
	GetTextExtentPoint32( hdc, pstr, strlen( pstr ), &size ) ;

	return size.cy ;
}

VOID	GetWindowsTextExtent( HDC hdc, LPSTR pstr, INT& X, INT& Y ) 
{
	SIZE size ;
	GetTextExtentPoint32( hdc, pstr, strlen( pstr ), &size ) ;
	X = size.cx ;
	Y = size.cy ;
}

//------------------------------------------------------------------------------
//					CGMString
//------------------------------------------------------------------------------

VOID	CGMString::SetString( INT iLen, LPCSTR pText )
{
	GMASSERT( NULL != pText, "CGMString::SetString() Error!" ) ;

	m_ilen = iLen ;

	if ( m_ilen >= ECONSTANTS::MAX_LOCAL_BUFFER_LEN )
	{
		m_pstr = new CHAR[ m_ilen + 1 ] ;
	}
	else
	{
		m_pstr = m_szBuffer ;
	}

	strcpy( m_pstr, pText ) ;
}

VOID	CGMString::AddString( LPCSTR pText )
{
	GMASSERT( NULL != pText, "CGMString::AddString() Error!" ) ;

	INT iAddLen = (INT)strlen( pText ) ;
	if ( 0 == iAddLen )
		return ;

	if ( m_ilen + iAddLen >= ECONSTANTS::MAX_LOCAL_BUFFER_LEN )
	{
		CHAR* pTemp = new CHAR[ m_ilen + iAddLen + 1 ] ;
		strcpy( pTemp, m_pstr ) ;
		strcpy( &pTemp[ m_ilen ], pText ) ;

		DeleteString() ;
		m_pstr = pTemp ;
	}
	else
	{
		strcat( m_pstr, pText ) ;
	}

	m_ilen = m_ilen + iAddLen ;
}

VOID	CGMString::DeleteString()
{
	if ( m_ilen >= ECONSTANTS::MAX_LOCAL_BUFFER_LEN )
	{
		GM_DELETE_ARRAY( m_pstr ) ;
	}

	m_pstr = NULL ;
	m_ilen = 0 ;
}

CGMString::CGMString()
{
	m_ilen		= 0 ;
	m_pstr		= m_szBuffer ;
	m_pstr[ 0 ] = '\0' ;
}

CGMString::CGMString( LPCSTR pText ) 
{
	GMASSERT( NULL != pText, "CGMString::CGMString( LPSTR pText ) Error!" ) ;

	SetString( (INT)strlen( pText ), pText ) ;
}

CGMString::CGMString( const CGMString& str )
{
	SetString( str.m_ilen, str.m_pstr ) ;
}

CGMString::~CGMString()
{
	DeleteString() ;
}

INT		CGMString::Length() const
{
	return m_ilen ;
}

CGMString&	CGMString::operator = ( const CGMString& str )
{
	if ( this == &str )
		return *this ;

	DeleteString() ;
	SetString( str.m_ilen, str.m_pstr ) ;

	return *this ;
}

CGMString&	CGMString::operator = ( LPCSTR pText ) 
{
	GMASSERT( NULL != pText, "CGMString::operator = ( LPCSTR pText ) Error!" ) ;

	if ( NULL != m_pstr && 0 == strcmp( m_pstr, pText ) )
	{
		return *this ;
	}

	DeleteString() ;
	SetString( (INT)strlen( pText ), pText ) ;

	return *this ;
}

CGMString::operator LPSTR() const
{ 
	if ( !this )
		return NULL ;

	return m_pstr ;
}

CGMString&	CGMString::operator + ( LPCSTR pText )
{
	GMASSERT( NULL != pText, "CGMString::operator + ( LPCSTR pText ) Error!" ) ;

	AddString( pText ) ;

	return *this ;
}

CHAR&		CGMString::operator [] ( INT iIndex ) const 
{
	if ( ( iIndex < 0 ) || ( iIndex > m_ilen ) )
	{
		GMASSERT( FALSE, "CGMString::operator [] Error!" ) ;
	}

	return m_pstr[ iIndex ] ;
}

BOOL 		operator > ( const CGMString& str1, const CGMString& str2 )
{
	if ( strcmp( str1.m_pstr, str2.m_pstr ) > 0 )
		return TRUE ;
	else
		return FALSE ;
}

BOOL 		operator < ( const CGMString& str1, const CGMString& str2 )
{
	if ( strcmp( str1.m_pstr, str2.m_pstr ) < 0 )
		return TRUE ;
	else
		return FALSE ;
}

BOOL 		operator == ( const CGMString& str1, const CGMString& str2 )
{
	if ( strcmp( str1.m_pstr, str2.m_pstr ) == 0 )
		return TRUE ;
	else
		return FALSE ;
}

BOOL 		operator == ( const CGMString& str1, LPCSTR pText )
{
	if ( !pText )
    {
		GMASSERT( FALSE, "CGMString::operator == Error!" ) ;
    }
	
	if ( strcmp( str1.m_pstr, pText ) == 0 )
		return TRUE ;
	else
		return FALSE;
}

BOOL 		operator != ( const CGMString& str1, LPCSTR pText )
{
	if ( !pText )
    {
		GMASSERT( FALSE, "CGMString::operator != Error!" ) ;
    }

	if ( strcmp( str1.m_pstr, pText ) == 0 )
		return FALSE ;
	else
		return TRUE ;
}
