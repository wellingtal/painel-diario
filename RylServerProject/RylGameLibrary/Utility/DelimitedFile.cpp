
#include "stdafx.h"

#include "DelimitedFile.h"


CDelimitedFile::CDelimitedFile(const char* pszDelimiter)
:	m_fpFile(0), m_dwColumn(0), m_dwColumnCount(0)
{
	m_ColumnNames.reserve(DEFAULT_COLUMN_NUM);
	strncpy(m_szDelimiter, pszDelimiter, MAX_DELIMITER_NUM);
}

CDelimitedFile::~CDelimitedFile()
{
	Close();	
}


CDelimitedFile::SectionInfo::SectionInfo(std::string& szSectionName, unsigned long dwLine)
:	m_szSectionName(szSectionName), m_dwLine(dwLine)
{

}


bool CDelimitedFile::Open( LPCSTR szFilename, int nHeadLine, UINT nOpenFlags )
{
	if ( m_fpFile )
	{
		::fclose( m_fpFile );
		m_fpFile = 0;
	}

	m_fpFile = ::fopen( szFilename, (nOpenFlags & modeCreate) ? "wt" : "rt" );
	if (NULL == m_fpFile)
	{
        #ifdef ERRLOG2
		unsigned long dwError = ::GetLastError();
		ERRLOG2(g_Log, "파일 %s를 열 수 없습니다. Error Code : %d", szFilename, dwError);
		#endif

		return false;
	}

	if (nHeadLine > -1) 
	{
		for (int i = 0; i < nHeadLine; )
		{
			if (true == ReadLine())
			{
				++i;
			}
		}

		for (m_dwColumnCount = 0; true; ++m_dwColumnCount)
		{
			char* szColumnName = new char[MAX_COLUMN_LENGHT];
			
			if (0 == szColumnName) 
			{ 
				return false; 
			}

			if (false == ReadString(szColumnName, MAX_COLUMN_LENGHT - 1)) 
			{
				delete szColumnName;
				break;
			}

			size_t nLength = strlen(szColumnName);
			while (nLength-- > 0)
			{
				if (szColumnName[nLength] == '\r' || szColumnName[nLength] == '\n')
				{
					szColumnName[nLength] = 0;
				}
			}

			szColumnName[MAX_COLUMN_LENGHT - 1] = 0;
			m_ColumnNames.push_back(szColumnName);
		}
	}
	return true;
}

void CDelimitedFile::Close( void )
{
	if ( m_fpFile )
	{
		::fclose( m_fpFile );
		m_fpFile = 0;
	}

	ColumnArray::size_type nTotalSize = m_ColumnNames.size();
	for (ColumnArray::size_type nCount = 0; nCount < nTotalSize; ++nCount) 
	{ 
		delete [] m_ColumnNames[nCount];
	}

	m_ColumnNames.clear();
}

unsigned long CDelimitedFile::GetSection(std::string& szSectionName)
{
	for (SectionArray::iterator it = m_SectionInfo.begin(); it != m_SectionInfo.end(); ++it)
	{
		if (szSectionName == it->m_szSectionName)
		{
			return it->m_dwLine;
		}
	}

	return 0;
}

bool CDelimitedFile::ReadSection(void)
{
	char szBuffer[MAX_LINE_LENGTH];
	unsigned long dwLine = 1;

	m_SectionInfo.clear();

	while (NULL != fgets(szBuffer, MAX_LINE_LENGTH, m_fpFile))
	{   
		std::string strSectionIn = std::string(szBuffer);

		if (';' == *(strSectionIn.begin()) || 0 == strcmp("\n", strSectionIn.c_str())) 
		{
			continue;
		}

		if ('[' == *(strSectionIn.begin()))
		{
			std::string::iterator end = std::find(strSectionIn.begin(), strSectionIn.end(), ']');
			if (end != strSectionIn.end())
			{
				std::string strSectionOut;
				strSectionOut.assign(strSectionIn.begin() + 1, end);
				m_SectionInfo.push_back(SectionInfo(strSectionOut, dwLine));
			}
		}

		++dwLine;
	} 

	if (true == m_SectionInfo.empty())
	{
		return false;
	}

	rewind(m_fpFile);
	return true;   
}

bool CDelimitedFile::ReadLine(void)
{
	while (NULL != ::fgets(m_szLine, MAX_LINE_LENGTH, m_fpFile))
	{
		if (0 == strcmp("\n", m_szLine))
		{
			break;
		}

		if (';' != m_szLine[0]) 
		{
			m_dwColumn = 0;
			memcpy(m_szBackupLine, m_szLine, MAX_LINE_LENGTH);
			return true;
		}
	}

	return false;
}

// 빈줄 및 C++ 스타일 주석 지원
bool CDelimitedFile::ReadLine2nd(void)
{
	while (NULL != ::fgets(m_szLine, MAX_LINE_LENGTH, m_fpFile))
	{
		// 문자열 처음부분에 있는 Tab 없애주기
		int iEnd = 0;
		while (true)
		{
			if ('\t' == m_szLine[iEnd])
			{
				++iEnd;
			}
			else
			{
				if (0 != iEnd)
				{
					strcpy(m_szLine, m_szLine + iEnd);
				}

				break;
			}
		}

		if (0 == strcmp("\n", m_szLine))
		{
			continue;
		}

		if ('/' != m_szLine[0] && '/' != m_szLine[1])
		{
			m_dwColumn = 0;
			memcpy(m_szBackupLine, m_szLine, MAX_LINE_LENGTH);
			return true;
		}
	}

	return false;
}

bool CDelimitedFile::ReadData( bool &bNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
		
	bNumber = false;
	if (atoi(pszData) == 1)
		bNumber = true;

	char szString[2];
	strncpy(szString, pszData, 1);
	if (NULL == ::strcmp(szString, "O"))
		bNumber = true;

	return true;
}

bool CDelimitedFile::ReadData( double &dNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	dNumber = atof( pszData );
	return true;
}

bool CDelimitedFile::ReadData( float &fNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	fNumber = (float) atof( pszData );
	return true;
}

bool CDelimitedFile::ReadData( long &lNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	lNumber = (long) atol( pszData );
	return true;
}

bool CDelimitedFile::ReadData( unsigned long &fNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	fNumber = (unsigned long) atol( pszData );
	return true;
}

bool CDelimitedFile::ReadData( int &iNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	iNumber = atoi( pszData );
	return true;
}

bool CDelimitedFile::ReadData( unsigned short &iNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	iNumber = (unsigned short)atoi( pszData );
	return true;
}

bool CDelimitedFile::ReadData( short &iNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	iNumber = (short)atoi( pszData );
	return true;
}

bool CDelimitedFile::ReadData( unsigned char &iNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	iNumber = (unsigned char)atoi( pszData );
	return true;
}

bool CDelimitedFile::ReadData( char &iNumber )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	iNumber = (char)atoi( pszData );
	return true;
}

bool CDelimitedFile::ReadData( __int64 &i64Number )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	i64Number = _atoi64( pszData );
	return true;
}

bool CDelimitedFile::ReadData( DWORD64 &i64Number )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	i64Number = (unsigned long)_atoi64( pszData );
	return true;
}

bool CDelimitedFile::ReadString( char *szString, unsigned long dwSize )
{
	CHAR *pszData = ::strtok( (m_dwColumn++) ? NULL : m_szLine, m_szDelimiter );

	if ( !pszData || !(*pszData) )
		return false;
	strncpy(szString, pszData, dwSize);
	return true;
}

bool CDelimitedFile::ReadData(const  char *szField, double &dNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( dNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, float &fNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( fNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, long &lNumber )
{
	int nSearch;

	if( ((nSearch = FindColumn( szField )) == -1) )
		return FALSE;
	GotoColumn( nSearch );
	return ReadData( lNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, unsigned long &fNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( fNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, int &iNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( iNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, unsigned short &iNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( iNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, short &iNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( iNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, unsigned char &iNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( iNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, char &iNumber )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( iNumber );
}

bool CDelimitedFile::ReadData(const  char *szField, __int64 &i64Number )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( i64Number );
}

bool CDelimitedFile::ReadData(const  char *szField, DWORD64 &i64Number )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadData( i64Number );
}

bool CDelimitedFile::ReadString(const  char *szField, char *szString, unsigned long dwSize )
{
	int nSearch;

	if ( ((nSearch = FindColumn( szField )) == -1) )
		return false;
	GotoColumn( nSearch );
	return ReadString( szString, dwSize );
}

bool CDelimitedFile::GotoColumn( int nColumn )
{
	CHAR* pszPos = 0;

	if (nColumn < 0 || (unsigned long)nColumn > m_dwColumnCount) {
		return false;
	}

	if ((m_dwColumn == (unsigned long) nColumn)) {
		return true;
	}

	if ( nColumn == 0 ) {
		m_dwColumn = 0;
		strtok(m_szLine, m_szDelimiter);
	} 
	else {
		for (;m_dwColumn < m_dwColumnCount && nColumn != m_dwColumn; ++m_dwColumn )
			pszPos = strtok(0, m_szDelimiter);

		if (m_dwColumn == m_dwColumnCount) {
			memcpy(m_szLine, m_szBackupLine, MAX_LINE_LENGTH);
			pszPos = strtok(m_szLine, m_szDelimiter);
			for (m_dwColumn = 1; m_dwColumn < (unsigned long)nColumn && nColumn != m_dwColumn; ++m_dwColumn ) {
				pszPos = strtok(NULL, m_szDelimiter );
			}
		}
	}

	return true;
}

int CDelimitedFile::FindColumn(const char *szField)
{	
	for (ColumnArray::size_type nCount = 0; nCount < m_ColumnNames.size(); ++nCount)
	{
		const char* szColumnName = m_ColumnNames[nCount];
		if (0 == strcmp(szField, szColumnName))
		{
            return static_cast<int>(nCount);
		}
	}

	return -1;
}
