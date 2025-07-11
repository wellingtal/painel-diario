#include "GMFileUtil.h"
#include <shlwapi.h>
#include "GMMemory.h"
 
INT FileAge( LPCTSTR lpFileName )
{
	HANDLE			hFind ;
	WIN32_FIND_DATA FindFileData ;
	FILETIME		LocalFileTime ;
	WORD			h, l ;
	INT				n = 0 ;

	hFind = FindFirstFile( lpFileName, &FindFileData ) ;
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind ) ;
		if ( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
		{
			FileTimeToLocalFileTime( &FindFileData.ftLastWriteTime, &LocalFileTime ) ;
			if (FileTimeToDosDateTime( &LocalFileTime, &h, &l ) )
			{
				n = MAKELONG( h,l ) ;
				return n ;	
			}
		}
	}

	return -1 ;
}

//------------------------------------------------------------------------------
//		������ �ִ��� ����
//------------------------------------------------------------------------------
BOOL FileExists( LPCTSTR lpFileName )
{
	return ( ( FileAge( lpFileName ) ) != -1 ) ;
}

//------------------------------------------------------------------------------
//		��Ʈ������ ����Ȯ���ڸ� ����
//		[shlwapi.lib] �߰�
//------------------------------------------------------------------------------
LPTSTR ExtractFileExt (LPCTSTR pFileName)
{
	return PathFindExtension (pFileName);
}

//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
VOID ExtractFilePath (LPCTSTR pFileName, CGMString& StrDir)
{
	CHAR str[256];
	_splitpath (pFileName, NULL, str, NULL, NULL );
	StrDir = str;
}
