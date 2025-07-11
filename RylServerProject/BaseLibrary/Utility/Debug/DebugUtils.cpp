
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <lmerr.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#include "DebugUtils.h"

//
// Set application-name for prefix of log filename
//
void DbgUtils::SetProgramName(TCHAR* pszOutBuffer, int nBufferSize, TCHAR* pszProgramName)
{
	if(0 == pszProgramName)
	{
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFilename[MAX_PATH], szExt[MAX_PATH];

		// Figure out what the report file will be named, and store it away
	    GetModuleFileName(0, pszOutBuffer, nBufferSize);

		PTSTR pszDot = pszOutBuffer;
	    
		// Look for the '.' before the "EXE" extension. Replace '.' to '\0'
		if((pszDot = _tcsrchr( pszDot, _T('.')))) 
		{
			*pszDot = 0;
		}

		_tsplitpath(pszOutBuffer, szDrive, szDir, szFilename, szExt);
		_tcsncpy(pszOutBuffer, szFilename, nBufferSize);
	}
	else
	{
		_tcsncpy(pszOutBuffer, pszProgramName, nBufferSize);
	}
}


DbgUtils::CConvertErrorToText& DbgUtils::CConvertErrorToText::GetInstance()
{ 
	static CConvertErrorToText convertErrorToText; 
	return convertErrorToText;
}


DbgUtils::CConvertErrorToText::CConvertErrorToText()
:	m_hNetMsg(LoadLibraryEx(TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE))
{
	
}


DbgUtils::CConvertErrorToText::~CConvertErrorToText()
{
	if(0 != m_hNetMsg)
	{
		FreeLibrary(m_hNetMsg);
	}
}


unsigned long DbgUtils::CConvertErrorToText::GetErrorTextBuffer(TCHAR* lpMessage, 
																unsigned long dwBufferLen, 
																unsigned long dwLastError)
{
	
	unsigned long dwFormatFlags = FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
	LPCVOID	lpModule = 0;

	if (dwLastError >= NERR_BASE && dwLastError <= MAX_NERR && 0 != m_hNetMsg)
	{
		lpModule = m_hNetMsg;
		dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	}
	
	return FormatMessage(dwFormatFlags, lpModule, dwLastError, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpMessage, dwBufferLen, 0);	
}
