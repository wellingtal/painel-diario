#include <stdafx.h>
#include <zlib/zlib.h>
#include <BaseLibrary/Utility/Resource/EnsureCleanup.h>

#include "LogStruct.h"
#include "ParseLog.h"

CParseLog::CParseLog()
:   m_lpRawLog(NULL), m_dwRawLogSize(0)
{

}

CParseLog::~CParseLog()
{
    Destroy();

}

void CParseLog::Destroy()
{
    if(NULL != m_lpRawLog)
    {
        delete [] m_lpRawLog;
        m_lpRawLog = NULL;
        m_dwRawLogSize = 0;
    }

    m_LogPtrArray.clear();
}


bool CParseLog::LoadFile(const char* szFileName)
{
    Destroy();

	if(0 != strstr(szFileName, ".zip"))
	{
		HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        CEnsureCloseHandle  handle(hFile);

        // gzip�� �� �� 4byte�� UncompressedSize�� �ִ�. �α� ���������� 1���� �����Ƿ� �����ϴ�.
        unsigned long dwSizeHigh = 0;
        unsigned long dwOffset = GetFileSize(hFile, &dwSizeHigh);

        LONG  nSizeHigh = 0;
        SetFilePointer(hFile, dwOffset - sizeof(unsigned long), &nSizeHigh, FILE_BEGIN);

        unsigned long dwReadSize = 0;
        if(!ReadFile(hFile, &m_dwRawLogSize, sizeof(unsigned long), &dwReadSize, NULL))
        {
            return false;
        }

		gzFile gzfile = gzopen(szFileName, "rb");
		if(NULL == gzfile)
		{
			return false;
		}
        
        unsigned long dwTotalReadSize = 0;
        m_lpRawLog = new char[m_dwRawLogSize];

        while(dwTotalReadSize < m_dwRawLogSize)
        {
            int nReadSize = gzread(gzfile, m_lpRawLog + dwTotalReadSize, 
                m_dwRawLogSize - dwTotalReadSize);

            if(-1 == nReadSize)
            {
                gzclose(gzfile);
                return false;
            }

            dwTotalReadSize += nReadSize;
        }

        gzclose(gzfile);
	}
	else
	{
		HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, NULL);

        if(INVALID_HANDLE_VALUE == hFile)
        {
            return false;
        }

        CEnsureCloseHandle  ensureClose(hFile);

        unsigned long dwSizeHigh = 0, dwTotalReadSize = 0;
        m_dwRawLogSize = GetFileSize(hFile, &dwSizeHigh);
        m_lpRawLog = new char[m_dwRawLogSize];

        while(dwTotalReadSize < m_dwRawLogSize)
        {
            unsigned long dwReadSize = 0;
            if(!ReadFile(hFile, m_lpRawLog + dwTotalReadSize, 
                m_dwRawLogSize - dwTotalReadSize, &dwReadSize, NULL))
	        {
		        return false;
	        }

            dwTotalReadSize += dwReadSize;
        }
    }

    if(!Parse())
    {
        Destroy();
        return false;
    }

    return true;
}


bool CParseLog::Parse()
{
    char* lpBegin   = m_lpRawLog;
    char* lpEnd     = m_lpRawLog + m_dwRawLogSize;    

    GAMELOG::sLogBase*  lpCurrentPos    = NULL;
    GAMELOG::sLogBase*  lpLastPos       = NULL;

    int nSize = 0;

	for(;lpBegin < lpEnd;)
	{
		lpCurrentPos = reinterpret_cast<GAMELOG::sLogBase*>(lpBegin);
		nSize = lpCurrentPos->GetSize();

		if (0 == nSize)
		{
			unsigned char cLastCmd = NULL != lpLastPos ? lpLastPos->m_cCmd : 0;

			ERRLOG3(g_Log, "�˸��� Ŀ�ǵ尡 ���� ���̻� �˻��� �� �����ϴ�."
				" Byte��ġ %d, Ŀ�ǵ� 0x%x, ���� Ŀ�ǵ� 0x%x", 
				lpEnd - lpBegin, lpCurrentPos->m_cCmd, cLastCmd);

			return false;
		}
		else
			m_LogPtrArray.push_back(lpCurrentPos);

        lpLastPos = lpCurrentPos;
        lpBegin += nSize;
	};

    return true;
}

