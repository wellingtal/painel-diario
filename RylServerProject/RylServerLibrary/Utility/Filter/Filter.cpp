
#include "stdafx.h"

#include <cstdio>

#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>
#include <Utility/Resource/EnsureCleanup.h>

#include <Network/XORCrypt/XORCrypt.h>

// 클라이언트에서도 쓰이므로 include를 명시한다.
#include <Log/ServerLog.h>

#include <DB/DBDefine.h>

#include "Filter.h"

#define DECODE_HEADER(Start_In, Length_In, PageVer_In, PageNum_In)  CXORCrypt::GetInstance().DecodeHeader((Start_In),(Length_In),(PageVer_In),(PageNum_In))
#define ENCODE_HEADER(Start_In, Length_In, PageVer_In, PageNum_In)  CXORCrypt::GetInstance().EncodeHeader((Start_In),(Length_In),(PageVer_In),(PageNum_In))
#define COMPRESS(In, In_len, Out, Out_len)      CMiniLZO::Compress((In), (In_len), (Out), (Out_len))
#define DECOMPRESS(In, In_len, Out, Out_len)    CMiniLZO::Decompress((In), (In_len), (Out), (Out_len))

static Filter::FilterName g_FilterName;
static Filter::FilterName g_AccountName;


//Interface////////////////////////////////////////////////////////////////////////////////////
//
// InitFilter [ public ]     
// - 필터 체커 초기화
//
// Parameter :
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool Filter::InitFilter(const char* szFilterFileName)
{
	FILE *pFile;
	char Buffer[256];

///////////////////////////////////////////////////////////////////////////////////////////////
//
// 욕 단어
//
///////////////////////////////////////////////////////////////////////////////////////////////
	pFile = fopen((NULL != szFilterFileName) ? szFilterFileName : FilterFileName, "r");
	if (NULL == pFile) { return false; }

	char Temp[256];

	while(true)
	{
		if(fgets(Buffer, 256, pFile) == NULL)
			break;

		if(strcmp(Buffer, "\n") == 0)
			continue;

		strcpy(Temp, strtok(Buffer, "\n"));
		char *Upper = _strlwr(Temp);

		strcpy(g_FilterName.FilterStrings[g_FilterName.Total], Upper);
		g_FilterName.Total = g_FilterName.Total + 1;
	}

	fclose(pFile);

#ifndef _RYL_GAME_CLIENT_
///////////////////////////////////////////////////////////////////////////////////////////////
//
// 제한 단어
//
///////////////////////////////////////////////////////////////////////////////////////////////
	pFile = fopen(LimitFileName, "r");
	if(pFile != NULL)
	{
		g_FilterName.Total = 0;
		while(true)
		{
			if(fgets(Buffer, 256, pFile) == NULL)
				break;

			if(strcmp(Buffer, "\n") == 0)
				continue;

			strcpy(Temp, strtok(Buffer, "\n"));
			char *Upper = _strlwr(Temp);

			strcpy(g_FilterName.FilterStrings[g_FilterName.Total], Upper);
			g_FilterName.Total = g_FilterName.Total + 1;
		}

		fclose(pFile);
	}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// 계정
//
///////////////////////////////////////////////////////////////////////////////////////////////
	pFile = fopen(AccountFileName, "r");
	if(pFile != NULL)
	{
		while(true)
		{
			if(fgets(Buffer, 256, pFile) == NULL)
				break;

			if(strcmp(Buffer, "\n") == 0)
				continue;

			strcpy(Temp, strtok(Buffer, "\n"));
			char *Upper = _strlwr(Temp);

			strcpy(g_AccountName.FilterStrings[g_AccountName.Total], Upper);
			g_AccountName.Total = g_AccountName.Total + 1;
		}

		fclose(pFile);
	}
#endif

	return true;
}

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// NameCheck [ public ]     
// - 이름 검사. 특정 이름이 검출되면 false 리턴
//
// Parameter :
//	1st : 이름
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool Filter::NameCheck(const char *CheckName_In)
{
    char CheckName[CHAR_INFOST::MAX_NAME_LEN] = "";

	strncpy(CheckName, CheckName_In, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    CheckName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

	char *Upper = _strlwr(CheckName);
	for(unsigned short ACount = 0; ACount < g_FilterName.Total; ACount++)
	{
		if(strstr(Upper, g_FilterName.FilterStrings[ACount]) != NULL)
			return false;
	}
	
	return true;
}

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// AccountCheck [ public ]     
// - 계정 체크
//
// Parameter :
//	1st : 계정
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool Filter::AccountCheck(const char *Account_In)
{
	for(unsigned short ACount = 0; ACount < g_FilterName.Total; ACount++)
	{
		if(!strcmp(Account_In, g_AccountName.FilterStrings[ACount]) != NULL)
			return false;
	}
	
	return true;
}

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// ClearFilterDB [ public ]     
// - 필터 체커의 내용을 모두 삭제
//
// Parameter :
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
void Filter::ClearFilterDB(void)
{
	for (int nIndex=0; nIndex<g_FilterName.Total; nIndex++)
	{
		strcpy(g_FilterName.FilterStrings[nIndex], "\0");
	}

	g_FilterName.Total = 0;
}

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// SaveFilterDBToBinary [ public ]     
// - 필터 체커의 내용을 바이너리 파일로 저장
//
// Parameter :
//	1st : 저장할 파일 이름
//	2nd : 더미 파일 이름
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool Filter::SaveFilterDBToBinary(const char* szFileNameBinary, const char* szTrashFile)
{
	HANDLE hFile = CreateFile(szFileNameBinary,GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return false; }

	CEnsureCloseHandle file(hFile);

	unsigned long   dwFilterSize	= static_cast<unsigned long>(MAX_FILTER_LEN * g_FilterName.Total);
	unsigned long   dwCompressSize  = dwFilterSize;

	char*   lpFilterInfo        = new char[dwFilterSize];
	char*   lpCompressedInfo    = new char[dwFilterSize];

	if (NULL == lpFilterInfo || NULL == lpCompressedInfo)
	{
		ERRLOG0(g_Log, "메모리가 부족합니다.");
		return false;
	}

	CEnsureDeleteArray<char> filterInfo(lpFilterInfo);
	CEnsureDeleteArray<char> compressInfo(lpCompressedInfo);

	memcpy(lpFilterInfo, g_FilterName.FilterStrings, dwFilterSize);
	ENCODE_HEADER(lpFilterInfo, dwFilterSize, 0, 1);
	COMPRESS(lpFilterInfo, dwFilterSize, lpCompressedInfo, &dwCompressSize);

	unsigned long dwWritten = 0;

	// 쓰레기(더미) 자료
	HANDLE hTrashFile = CreateFile(szTrashFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hTrashFile == INVALID_HANDLE_VALUE) 
	{ 
		ERRLOG1(g_Log, "%s 파일을 열 수 없습니다.", szTrashFile);
		return false; 
	} 

	CEnsureCloseHandle trashFile(hTrashFile);

	unsigned long dwRead            = 0;
	unsigned long dwFileHighSize    = 0;
	unsigned long dwFileSize        = GetFileSize(hTrashFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];
	CEnsureDeleteArray<char> allocated(lpAllocated);

	if (false == ReadFile(hTrashFile, lpAllocated, dwFileSize, &dwRead, NULL))
	{
		ERRLOG0(g_Log, "쓰레기 파일을 읽을 수 없습니다.");
		return false;
	}

	WriteFile(hFile, &dwFileSize, sizeof(unsigned long), &dwWritten, 0);
	WriteFile(hFile, lpAllocated, dwFileSize, &dwWritten, 0);

	// 올바른 자료
	WriteFile(hFile, &dwFilterSize, sizeof(unsigned long), &dwWritten, 0);	// 원본 데이터 크기
	WriteFile(hFile, lpCompressedInfo, dwCompressSize, &dwWritten, 0);		// 압축된 데이터 크기
	return true;
}

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// LoadFilterDBFromBinary [ public ]     
// - 바이너리 파일에서 필터 체커의 내용을 추출
//
// Parameter :
//	1st : 추출할 파일 이름
//
// Return:
//	
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool Filter::LoadFilterDBFromBinary(const char* szFileNameBinary)
{
	HANDLE hFile = CreateFile(szFileNameBinary,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return false; } 

	CEnsureCloseHandle hfile(hFile);

	unsigned long dwRead            = 0;
	unsigned long dwFileHighSize    = 0;
	unsigned long dwFileSize        = GetFileSize(hFile, &dwFileHighSize);

	char* lpAllocated = new char[dwFileSize];
	CEnsureDeleteArray<char> allocated(lpAllocated);

	if (NULL == lpAllocated)
	{
		ERRLOG0(g_Log, "메모리가 부족합니다.");
		return false;
	}

	if (false == ReadFile(hFile, lpAllocated, dwFileSize, &dwRead, NULL)) 
	{
		ERRLOG1(g_Log, "읽기 실패 : %d에러", GetLastError());
		return false;
	}

	unsigned long dwHeaderSize          = sizeof(unsigned long) + *reinterpret_cast<unsigned long*>(lpAllocated) + sizeof(unsigned long);
	unsigned long dwDecompressedSize    = *reinterpret_cast<unsigned long*>(lpAllocated + dwHeaderSize - sizeof(unsigned long));

	ClearFilterDB();

	g_FilterName.Total = static_cast<unsigned short>(dwDecompressedSize / MAX_FILTER_LEN);

	DECOMPRESS(lpAllocated + dwHeaderSize, dwFileSize - dwHeaderSize, 
		reinterpret_cast<char *>(&g_FilterName.FilterStrings), &dwDecompressedSize);
	DECODE_HEADER(reinterpret_cast<char *>(&g_FilterName.FilterStrings), dwDecompressedSize, 0, 1);	
	return true;
}
