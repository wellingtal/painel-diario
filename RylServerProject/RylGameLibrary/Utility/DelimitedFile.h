#ifndef _DELIMITED_FILE_H_
#define _DELIMITED_FILE_H_

#pragma once

#include <winsock2.h>
#include <windows.h>
#include <vector>


class CDelimitedFile
{
public:
	
	// MAX Delimiter num is 32;
	CDelimitedFile(const char* pszDelimiter = "\t");
	~CDelimitedFile();
	
	enum 
	{ 
		MAX_LINE_LENGTH		= 16384,
		MAX_DELIMITER_NUM	= 32,
		MAX_COLUMN_LENGHT	= 64,
		DEFAULT_COLUMN_NUM	= 32
	};

	enum OpenFlags 
	{
		modeRead	= (int) 0x00000,
		modeCreate	= (int) 0x01000
	};

private:
	
	struct SectionInfo
	{
		unsigned long	m_dwLine;
		std::string		m_szSectionName;

		SectionInfo() : m_dwLine(0) { m_szSectionName.clear(); }
		SectionInfo(std::string& szSectionName, unsigned long dwLine);
	};


	FILE*	m_fpFile;

	unsigned long	m_dwColumn;
	unsigned long	m_dwColumnCount;

	char	m_szLine[MAX_LINE_LENGTH];
	char	m_szBackupLine[MAX_LINE_LENGTH];
	char	m_szDelimiter[MAX_DELIMITER_NUM];

	typedef std::vector<char*>			ColumnArray;
	typedef std::vector<SectionInfo>	SectionArray;

	ColumnArray		m_ColumnNames;
	SectionArray	m_SectionInfo;


	int  FindColumn(const char *szField );
	bool GotoColumn( int nColumn );

public:

	bool Open(LPCSTR szFilename, int nHeadLine = -1, UINT nOpenFlags = modeRead );
	void Close();

	unsigned long GetSection(std::string& szSectionName);
	bool ReadSection(void);
	bool ReadLine2nd(void);	// 빈줄 및 C++ 스타일 주석 지원
	bool ReadLine();		// ; 주석지원, 빈줄 지원하지 않음

	bool ReadData(bool &bNumber);
	bool ReadData(double &dNumber);
	bool ReadData(float &fNumber);
	bool ReadData(long &lNumber);
	bool ReadData(unsigned long &fNumber);
	bool ReadData(int &iNumber);	
	bool ReadData(unsigned short &iNumber);
	bool ReadData(short &iNumber);
	bool ReadData(unsigned char &iNumber);
	bool ReadData(char &iNumber);
	bool ReadData(__int64 &i64Number);
	bool ReadData(DWORD64 &i64Number);
	bool ReadString(char *szString, unsigned long dwSize);

	bool ReadData(const char *szField, double &dNumber);
	bool ReadData(const char *szField, float &fNumber);
	bool ReadData(const char *szField, long &lNumber );
	bool ReadData(const char *szField, unsigned long &fNumber);
	bool ReadData(const char *szField, int &iNumber);	
	bool ReadData(const char *szField, unsigned short &iNumber);
	bool ReadData(const char *szField, short &iNumber);
	bool ReadData(const char *szField, unsigned char &iNumber);
	bool ReadData(const char *szField, char &iNumber);
	bool ReadData(const char *szField, __int64 &i64Number);
	bool ReadData(const char *szField, DWORD64 &i64Number);
	bool ReadString(const char *szField, char *szString, unsigned long dwSize);
};


#endif