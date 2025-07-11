// StringFilter.cpp: implementation of the CStringFilter class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>
#include <Network/XORCrypt/XORCrypt.h>
#include <Utility/Resource/EnsureCleanup.h>
//#include "MemUtils.h"
//#include "..\\NeoRylClient\\Network\\Compress\\Compress.h"
//#include "..\\NeoRylClient\\Network\\Crypto\\DemonCryptoFunctions.h"

#include "StringFilter.h"
#include "windows.h"
#include "ScriptEngine.h"
#include <string>
#include "GMMemory.h"

CStringFilter g_StringFilter;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringFilter::CStringFilter()
{
	m_dwStringCount = 0;
	m_strSearched[0] = 0;
}

CStringFilter::~CStringFilter()
{

}

bool CStringFilter::ConvertFilter(char *strTarget)
{
	WCHAR strWideOri[300], strWideTarget[300], strWideString[300];
	int nBufferSize = 300;
	int nWideTargetLen, nWideStringLen;
	unsigned long i;
	int j;

	MultiByteToWideChar(CP_ACP, 0, strTarget, strlen(strTarget) + 1, strWideTarget, nBufferSize);
	nWideTargetLen = wcslen(strWideTarget);

	wcscpy(strWideOri, strWideTarget);
	bool bResult = false;

	for(i = 0; i < m_dwStringCount; i++)
	{
		wcscpy(strWideString, m_strString[i]);
		nWideStringLen = wcslen(strWideString);

		j = 0;

		if(nWideTargetLen >= nWideStringLen)
		{
			do
			{
				// 필터가 걸렸다.
				if(!wcsncmp(&strWideTarget[j], strWideString, nWideStringLen))
				{
					if(!bResult)
						WideCharToMultiByte(CP_ACP, 0, m_strString[i], -1, m_strSearched, strlen(m_strSearched), NULL, NULL);

					for(int x = 0; x < nWideStringLen; ++x)
					{
						strWideOri[j+x] = strWideTarget[j+x] = L'*';
					}
					bResult = true;
				}
				j++;
			} while(j <= (nWideTargetLen - nWideStringLen));
		}
	}

	WideCharToMultiByte(CP_ACP, 0, strWideOri, -1, strTarget, strlen(strTarget) + 1, NULL, NULL);
	return bResult;
}


bool CStringFilter::Filter(char *strTarget)
{
	WCHAR strWideTarget[300], strWideString[300];
	int nBufferSize = 300;
	int nWideTargetLen, nWideStringLen;
	unsigned long i;
	int j;

	MultiByteToWideChar(CP_ACP, 0, strTarget, strlen(strTarget) + 1, strWideTarget, nBufferSize);
	nWideTargetLen = wcslen(strWideTarget);

	for(i = 0; i < m_dwStringCount; i++)
	{
		wcscpy(strWideString, m_strString[i]);
		nWideStringLen = wcslen(strWideString);

		j = 0;

		if(nWideTargetLen >= nWideStringLen)
		{
			do
			{
				if(!wcsncmp(&strWideTarget[j], strWideString, nWideStringLen))
				{
					WideCharToMultiByte(CP_ACP, 0, m_strString[i], -1, m_strSearched, strlen(m_strSearched), NULL, NULL);
					return false;
				}
				j++;
			} while(j <= (nWideTargetLen - nWideStringLen));
		}
	}

	m_strSearched[0] = 0;
	return true;
}

bool CStringFilter::Load(const char *strScriptFile)
{
	char strString[15];
	int nBufferSize = 15;

	HANDLE hFile = CreateFile(strScriptFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 
	if (hFile == INVALID_HANDLE_VALUE) { 
		return false; 
	}

    CEnsureCloseHandle readFile(hFile);

	DWORD dwRead = 0;
	DWORD dwFileHighSize = 0;
	DWORD dwFileSize = GetFileSize(hFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];

    CEnsureDeleteArray<char> allocated(lpAllocated);

	if(FALSE == ReadFile(hFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
	{
		return false;
	}

	char *lpProtoType = NULL;

	DWORD dwHeaderSize = sizeof(DWORD) + *reinterpret_cast<DWORD*>(lpAllocated) + sizeof(DWORD);
	char* lpBuffer = new char[*reinterpret_cast<DWORD*>(lpAllocated + dwHeaderSize - sizeof(DWORD))];
	if (NULL == lpBuffer) 
	{ 
		return false; 
	}

    CEnsureDeleteArray<char> buffer(lpBuffer);

	char* lpBufferStartPointer = lpBuffer;
	DWORD dwBufferSize = 1000000;

	CMiniLZO::Decompress(lpAllocated + dwHeaderSize, dwFileSize - dwHeaderSize, lpBuffer, &dwBufferSize);
	CXORCrypt::GetInstance().DecodeHeader(lpBuffer, dwBufferSize, 0, 1);

	while (dwBufferSize > 0) 
	{
		lpProtoType = reinterpret_cast<char *>(lpBuffer);

		strncpy(strString, lpProtoType, sizeof(char[15]));

		MultiByteToWideChar(CP_ACP, 0, strString, sizeof(char[15]), m_strString[m_dwStringCount], nBufferSize);
		m_dwStringCount++;

		dwBufferSize -= sizeof(char[15]);
		lpBuffer += sizeof(char[15]);
	}

	return true;
}
