#include "RYLBitmap.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h> 
#include "GMMemory.h"

void MarkFlipBits (void * pSour, int wbytes, int Height)
{
	int size;
	BYTE * tmpBits; 
	BYTE * tb;
	BYTE * ps;
	size = wbytes * Height;

	tmpBits = new BYTE [size];

	memcpy (tmpBits, pSour, size);
	
	tb = tmpBits;
	tb = tb + size - wbytes;
	ps = (BYTE*)pSour;

	int i;
	for (i = 0; i < Height; i++)
	{
		memcpy (ps, tb, wbytes);
		tb = tb - wbytes;
		ps = ps + wbytes;
	}

	delete [] tmpBits;
}

CRYLBitmap::CRYLBitmap () 
{
	m_iWidth = m_iHeight = 0 ;
	m_pwBitmapPixels = NULL ;
	m_buffer = NULL ;
}

CRYLBitmap::~CRYLBitmap ()
{
}

VOID CRYLBitmap::Clear ()
{
}

INT CRYLBitmap::Load( LPCTSTR pFileName, BYTE** pOutBits )
{
    HANDLE hFile = CreateFile( pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	if ( hFile == INVALID_HANDLE_VALUE )
		return 0 ;

	BITMAPFILEHEADER bmfh ;
	unsigned long nRead ;
	if ( ReadFile( hFile, &bmfh, sizeof( BITMAPFILEHEADER ), &nRead, NULL ) == FALSE )
	{
		CloseHandle( hFile ) ;
		return 0 ;
	}

	if ( bmfh.bfType != ( *( WORD * )"BM" ) )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 1220 ] ) ;
		CloseHandle( hFile ) ;
		return 0 ;
	}

	BITMAPINFOHEADER bmih ;
	if ( ReadFile( hFile, &bmih, sizeof( BITMAPINFOHEADER ), &nRead, NULL ) ==  FALSE )
	{
		CloseHandle( hFile ) ;
		return 0 ;
	}

	if ( bmih.biBitCount != 24 )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 1221 ] ) ;
		CloseHandle( hFile ) ;
		return 0 ;
	}

	m_iWidth			= bmih.biWidth ;
	m_iHeight			= bmih.biHeight ;
	m_iSize				= m_iWidth * m_iHeight ;

	if ( m_iWidth != 18 || m_iHeight != 12 )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 1222 ] ) ;
		CloseHandle( hFile ) ;
		return 0 ;
	}

	DWORD dwWidth = ( bmih.biWidth * 3 ) + ( bmih.biWidth % 4 ) ;
	DWORD dwSize  = dwWidth * bmih.biHeight ;
	BYTE* temp    = new BYTE[ dwSize ] ;
	//WORD* dwTemp  = new WORD[ m_iWidth * m_iHeight ] ;

	SetFilePointer( hFile, bmfh.bfOffBits, NULL, FILE_BEGIN ) ;
	if ( ReadFile( hFile, temp, dwSize, &nRead, NULL ) == FALSE )
	{
		delete [] temp ;
		CloseHandle( hFile ) ;
		return 0 ;
	}

	CloseHandle( hFile ) ;

	m_buffer = new BYTE[ m_iWidth * m_iHeight * 2 + 1 ];
	
	m_buffer[ 0 ]  = 0xff ;
	m_pwBitmapPixels = (WORD *)( m_buffer + 1);
		
	int iLine = m_iHeight ;
	BYTE *pSrc	= temp ;
	WORD *pDest	= m_pwBitmapPixels ;
	INT iCount  = 0 ;

	while( iLine )
	{
		iLine -- ;
		
		for ( int iPx = 0 ; iPx < m_iWidth ; iPx ++, pSrc += 3 )
		{
			pDest[ iCount ] = ( WORD )( ( ( ( WORD )pSrc[ 2 ] & 0xf8 ) << 8 ) |
									    ( ( ( WORD )pSrc[ 1 ] & 0xfc ) << 3 ) |
									    ( ( ( WORD )pSrc[ 0 ] & 0xf8 ) >> 3 ) ) ;
			++ iCount ;
		}

		pSrc += m_iWidth % 4 ;
	}

	MarkFlipBits( pDest, m_iWidth * 2, m_iHeight ) ;

	if ( temp )
	{
		delete [] temp ;
		temp = NULL ;
	}

	*pOutBits = m_buffer ;

	return 1 ;
}
