#include "GMList.h"
#include "GMMemory.h"
 
VOID	GMListBufferMove( VOID **sour, VOID **dest, INT Count ) 
{
	LPSTR	pS ;
	LPSTR	pD ;
	INT		i ;
	pS = ( LPSTR ) sour ;
	pD = ( LPSTR ) dest ;

	if (pS == pD)
	{
		return ;
	}
	
	if ( ULONG( pD ) > ULONG( pS ) )
	{
		for ( i = Count - 1 ; i >= 0 ; i -- )
		{
			pD[ i ] = pS[ i ] ;
		}
	}
	else
	{
		for ( i = 0 ; i < Count ; i ++ )
		{
			pD[ i ] = pS[ i ] ;
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////
CGMList::CGMList()
{
	m_iCount	= 0 ;
	m_iCapacity = 0 ;
	m_pList		= NULL ;
}

CGMList::~CGMList()
{
	Clear() ;
}

INT		CGMList::Add( LPVOID p )
{
	INT		n ;
	n = m_iCount ;

	if ( n == m_iCapacity )
	{
		Grow() ;
	}
	m_pList[ n ] = p ;
	m_iCount ++ ;

	return m_iCount ;
}

VOID	CGMList::Clear()
{	
	SetCount( 0 ) ;
	SetCapacity( 0 ) ;
}

INT		CGMList::Delete( INT Index )
{
	LPVOID Temp ;
	
	if ( ( Index < 0 ) || ( Index >= m_iCount ) )
	{
		return 1 ;
	}
	
	Temp = m_pList[ Index ] ;
	m_iCount = m_iCount - 1 ;

	if (Index < m_iCount )
	{
		GMListBufferMove( &m_pList[ Index + 1 ], &m_pList[ Index ], ( m_iCount - Index ) * sizeof( VOID** ) ) ;
	}

	return 0 ;
}

INT		CGMList::Exchange( INT Index1, INT Index2 )
{
	LPVOID Item ;

	if ( ( Index1 < 0 ) || ( Index1 >= m_iCount ) )  { return 1 ; }
	if ( ( Index2 < 0 ) || ( Index2 >= m_iCount ) )  { return 1 ; }
	Item = m_pList[ Index1 ] ;
	m_pList[ Index1 ] = m_pList[ Index2 ] ;
	m_pList[ Index2 ] = Item ;
	
	return 0 ;
}

INT		CGMList::First( LPVOID p )
{
	LPVOID tmp ;
	
	Get( 0, &tmp ) ;
	p = tmp ;

	return 0 ;
}

INT		CGMList::Last( LPVOID p )
{
	LPVOID tmp ;
	
	Get( m_iCount - 1, &tmp ) ;
	p = tmp ;

	return m_iCount;
}

INT		CGMList::IndexOf( LPVOID p )
{
	INT n = 0 ;
	while ( ( n < m_iCount ) && ( m_pList[ n ] ) != p )
	{
		n ++ ;
	}
	if ( n == m_iCount )
	{
		return -1 ;	
	}
	return n ;
}

INT		CGMList::Insert( INT Index, LPVOID p )
{
	if ( ( Index < 0 ) || ( Index > m_iCount ) )
	{
		return -1 ;
	}
	if ( m_iCount == m_iCapacity )
	{
		Grow() ;
	}

	if ( Index < m_iCount )
	{
		GMListBufferMove( &m_pList[ Index ], &m_pList[ Index + 1 ], ( m_iCount - Index ) * sizeof( VOID** ) ) ;
	}
	m_pList[ Index ] = p ;
	m_iCount ++ ;

	return 0 ;
}

INT		CGMList::Move( INT CurIndex, INT NewIndex )
{
	LPVOID Item ;

	if ( CurIndex != NewIndex )
	{
		if ( ( NewIndex < 0 ) || ( NewIndex >= m_iCount ) )
		{
			return -1 ;
		}
		Get( CurIndex, &Item ) ;
		m_pList[ CurIndex ] = NULL ;
		Delete( CurIndex ) ;

		LPVOID tmp = NULL  ;

		Insert( NewIndex, &tmp ) ;
		m_pList[ NewIndex ] = Item ;
	}

	return 0 ;
}

INT		CGMList::Remove( LPVOID p )
{
	INT  n = IndexOf( p ) ;

	if ( n >= 0 )
	{
		Delete( n ) ;
	}
	return n ;
}

VOID	CGMList::Pack()
{
	for ( INT i = m_iCount ; i = 0 ; i -- )
	{
		if ( m_pList[ i ] == NULL )
		{
			Delete( i ) ;
		}
	}
}

VOID	CGMList::Get( INT Index, VOID **p )
{
	if ( ( Index < 0 ) || ( Index >= m_iCount ) )
	{
		return ;
	}
	*p = m_pList[ Index ] ;
}
 
VOID	CGMList::Grow()
{
	INT		Delta ;
	
	if ( m_iCapacity > 64 )
	{
		Delta = m_iCapacity / 4 ;
	}
	else
	{
		if ( m_iCapacity > 8 )
		{
			Delta = 16 ;
		}
		else 
		{
			Delta = 4 ;
		}
	}
	SetCapacity( m_iCapacity + Delta ) ;
}

INT		CGMList::Put( INT Index, LPVOID p )
{
	LPVOID tmp ;
	
	if ( ( Index < 0 ) || ( Index >= m_iCount ) )
	{
		return -1 ;
	}

	if ( p != m_pList[ Index ] )
	{
		tmp = m_pList[ Index ] ;
		m_pList[ Index ] = p ;
	}

	return 0 ;
}

INT		CGMList::SetCapacity( INT NewCapacity )
{
	if ( ( NewCapacity < m_iCount ) || ( NewCapacity > MAX_LISTSIZE ) )
	{
		return -1 ;
	}

	if ( NewCapacity != m_iCapacity )
	{
		m_pList = ( VOID **)realloc( m_pList, NewCapacity * sizeof( VOID** ) ) ;
		m_iCapacity = NewCapacity ;
	}

	return 0 ;
}

INT		CGMList::SetCount( INT NewCount)
{
	if ( ( NewCount < 0 ) || ( NewCount > MAX_LISTSIZE ) )
	{
		return -1 ;
	}
	if ( NewCount > m_iCapacity )
	{
		SetCapacity( NewCount ) ;
	}
	if ( NewCount > m_iCount )
	{
		memset( m_pList[m_iCount], 0, ( NewCount - m_iCount ) * sizeof( VOID** ) ) ;
	}
	else
	{
		for ( INT i = m_iCount - 1 ; i >= NewCount ; i -- )
		{
			Delete( i ) ;
		}
	}

	m_iCount = NewCount ;

	return 0 ;
}
