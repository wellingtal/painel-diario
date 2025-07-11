#include "GMStream.h"
#include <stdio.h>
#include <io.h>
#include <sys/stat.h> 
#include "GMMemory.h"

CGMStream::CGMStream()
{
	m_hFileHandle  = 0 ;
	m_iFilePostion = 0 ;
}
 
CGMStream::~CGMStream()
{
	if ( m_hFileHandle != 0 )
	{
		close( m_hFileHandle ) ;
	}

	m_hFileHandle = 0 ;
}

INT CGMStream::GetPostion () 
{
	return m_iFilePostion ;
}

INT CGMStream::Create( LPSTR pFileName, INT OpenMode )
{
	TCHAR	szDirectory[ 256 ] ;
	UINT	cchPath ;
	cchPath = ::GetCurrentDirectory( sizeof( szDirectory ) / sizeof( TCHAR ), szDirectory ) ;
	if ( cchPath == 0 )
	{
		return -1 ;
	}

	GetFullPathName( pFileName, sizeof( szDirectory ) / sizeof( TCHAR ), szDirectory, NULL ) ;

	if ( OpenMode == O_LOAD )
	{
		if ( !FileExists( szDirectory ) ) 
		{
			return -1;
		}
	}

	m_hFileHandle = open( szDirectory, OpenMode ) ;	

	return m_hFileHandle ;
}

INT CGMStream::ReadBuffer( VOID* pbuffer, INT size )
{
	if ( m_hFileHandle == 0 )
		return 0 ;

	INT n = read( m_hFileHandle, pbuffer, size ) ;
	m_iFilePostion = m_iFilePostion + n ;
	
	return n ;
}

INT CGMStream::WriteBuffer( VOID* pbuffer, INT size )
{
	if ( m_hFileHandle == 0 )
	{
		return 0 ;
	}
	
	INT n = write( m_hFileHandle, pbuffer, size ) ;
	m_iFilePostion = m_iFilePostion + n ;
	
	return n ;
}

INT CGMStream::SeekBuffer( INT Offset, INT Origin )
{
	//<====================================
	// SEEK_SET = 0
	// 	Beginning of file. 
	// SEEK_CUR = 1
	// 	Current position of file poINTer. 
	// SEEK_END = 1
	// 	End of file. 
	//<====================================
	m_iFilePostion = lseek( m_hFileHandle, Offset, Origin ) ;
	return m_iFilePostion ;
}

INT CGMStream::GetSize()
{
	INT pos, posend ;

	pos		= SeekBuffer( 0, SEEK_CUR ) ;
	posend	= SeekBuffer( 0, SEEK_END ) ;
	pos		= SeekBuffer( pos, SEEK_SET ) ;

	return posend ; 
}


CGMFileStream::CGMFileStream () : CGMStream ()
{
}

CGMFileStream::~CGMFileStream ()
{
}

LPSTR CGMFileStream::GetFileName ()
{
	return m_FileName ;
}

LPSTR CGMFileStream::GetFileFullName ()
{
	TCHAR szDirectory[ 256 ] ;
	UINT cchPath ;
	cchPath = ::GetCurrentDirectory( sizeof( szDirectory ) / sizeof( TCHAR ), szDirectory ) ;
	if ( cchPath == 0 )
	{
		return NULL ;
	}

	GetFullPathName( m_FileName, sizeof( szDirectory ) / sizeof( TCHAR ), szDirectory, NULL ) ;
	m_FileFullName = szDirectory ;
	return m_FileFullName ;
}

INT CGMFileStream::Create( LPSTR pFileName, INT OpenMode )
{
	INT i =  CGMStream::Create( pFileName, OpenMode ) ;
	if ( i != -1 )
		m_FileName = pFileName ;

	return i ;
}



CGMMemoryStream::CGMMemoryStream ()
{
	m_pMemory		= NULL ;
	m_hMem			= NULL ;
	m_dwPostion		= 0 ;
	m_iSize			= 0 ;
	m_pBeginning	= NULL ;
	m_pCurrent		= NULL ;
	m_pEnd			= NULL ;
	m_iCurrentPos	= 0 ;
}

CGMMemoryStream::~CGMMemoryStream ()
{
	if (m_hMem)		GlobalFree( m_hMem ) ;
	
	m_hMem			= NULL ;
	m_pMemory		= NULL ;
	m_dwPostion		= 0 ;
	m_iSize			= 0 ;
	m_pBeginning	= NULL ;
	m_pCurrent		= NULL ;
	m_pEnd			= NULL ;
}

VOID CGMMemoryStream::Create ()
{
	if (m_hMem)		GlobalFree( m_hMem ) ;

	m_hMem			= NULL ;
	m_pMemory		= NULL ;
	m_dwPostion		= 0 ;
	m_iSize			= 0 ;
	m_pBeginning	= NULL ;
	m_pCurrent		= NULL ;
	m_pEnd			= NULL ;
}

INT CGMMemoryStream::ReadBuffer( VOID* pbuffer, INT size )
{
	if (m_iCurrentPos + size > m_iSize)
    {
		GMASSERT( FALSE, "CGMMemoryStream::ReadBuffer Error!!" ) ;
    }

	memcpy( pbuffer, m_pCurrent, size ) ;
	m_iCurrentPos	= m_iCurrentPos + size ;
	m_pCurrent		= m_pCurrent + size ;
	return size ;
}

INT CGMMemoryStream::WriteBuffer( VOID* pbuffer, INT size )
{
	if ( m_iCurrentPos + size > m_iSize )
    {
		GMASSERT( FALSE, "CGMMemoryStream::WriteBuffer Error!!" ) ;
    }

	memcpy( m_pCurrent, pbuffer, size ) ;
	m_iCurrentPos	= m_iCurrentPos + size ;
	m_pCurrent		= m_pCurrent + size ;
	return size ;
}

INT CGMMemoryStream::SeekBuffer( INT Offset, INT Origin )
{
	switch (Origin)
	{
		case FP_BEGIN :
			m_iCurrentPos = Offset;
			m_pCurrent	  = PBYTE(m_pMemory) + Offset;
			break;

		case FP_CURRENT :
			if ( (m_iCurrentPos + Offset) > m_iSize)
			{
				Offset = m_iSize - m_iCurrentPos;
			}
			m_iCurrentPos = m_iCurrentPos + Offset;
			m_pCurrent = PBYTE(m_pCurrent) + Offset;
			break;

		case FP_END :
			if (m_iSize < Offset)
			{
				Offset = m_iSize;
			}
			m_pCurrent = PBYTE(m_pMemory) + (m_iSize - Offset);
			m_iCurrentPos = (m_iSize - Offset);
			break;
	}
	return m_iCurrentPos ;
}

INT CGMMemoryStream::GetPostion () 
{
	return m_dwPostion ;
}

BOOL CGMMemoryStream::SetSize (INT Size)
{
	if (Size == 0)			return FALSE;
	if (m_iSize == Size)	return TRUE;

	Create ();

	m_iSize = Size;

	m_hMem = GlobalAlloc (GHND, m_iSize);
	m_pMemory = GlobalLock (m_hMem);
	if (!m_pMemory)
	{
		Create ();
		return FALSE;
	}
	m_pBeginning = PBYTE(m_pMemory);
	m_pCurrent = m_pBeginning;
	m_pEnd = m_pBeginning + m_iSize;

	return TRUE ;
}


INT CGMMemoryStream::GetSize ()
{
	return m_iSize;
}

VOID* CGMMemoryStream::GetMemory (INT Origin)
{
	switch (Origin)
	{
		case FP_BEGIN :
			return m_pBeginning;
		case FP_CURRENT :
			return m_pCurrent;
		case FP_END :
			return m_pEnd;
	}
	return NULL;
}

VOID CGMMemoryStream::LoadFromFile (LPSTR pFileName)
{
	CGMFileStream Stream;
	Stream.Create (pFileName, O_LOAD);
	LoadFormStream (&Stream);
}

VOID CGMMemoryStream::LoadFormStream (CGMStream* pStream)
{
	SetSize (pStream->GetSize ());
	pStream->ReadBuffer (m_pMemory, m_iSize);
}

VOID CGMMemoryStream::SaveToFile (LPSTR pFileName)
{
	CGMFileStream Stream;
	Stream.Create (pFileName, O_SAVE);
	SaveToStream (&Stream);
}

VOID CGMMemoryStream::SaveToStream (CGMStream* pStream)
{
	pStream->WriteBuffer (m_pMemory, m_iSize);
}


