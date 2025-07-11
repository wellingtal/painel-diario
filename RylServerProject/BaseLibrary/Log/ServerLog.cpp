#include "stdafx.h"
#include "ServerLog.h"
#include "../Utility/Debug/DebugUtils.h"

CServerLog g_Log;
CServerLog g_PacketLog("Packet", 0);
CServerLog g_SessionLog("SessionLog", 0);
CServerLog g_SkillLog("SkillLog", 0);


enum LOG_CONSTANT
{
    MAX_LOG_PER_SIZE        = 16384,
    MAX_LOG_FILE_SIZE       = 50 * 1024 * 1024,    // 100Mega
    MAX_LOG_BUFFER          = 256 * 1024,
    MAX_LOG_BUFFER_BOUND    = 248 * 1024
};


static const char* s_LogType[MAX_LOG_TYPE] = 
{
    "NOR",  // Normal
	"RUL",	// Game manager things
    "INF",	// Informations about current status

	"DET",	// Detail
	"SER",	// System call error
	"ERR",	// Fatal error (must be fixed!)
	"DBG",	// Debug related informations
	"WRN",	// Warning (must be fixed!)
};


CServerLog::CServerLog(const char* szLogName, const char* szPathName)
:   m_hFile(INVALID_HANDLE_VALUE), 
    m_dwTotalFileWriteSize(0LL),
    m_dwTotalLoggedSize(0LL),
    m_dwCurrentFileWriteSize(0), m_dwBufferUsed(0),
	m_dwLogEnabled(0xFFFFFFFF)
{
    if(0 != szLogName)
    {
        _snprintf(m_szProgramName, MAX_PATH - 1, "%s", szLogName);
        m_szProgramName[MAX_PATH - 1] = 0;
    }
    else
    {
        DbgUtils::SetProgramName(m_szProgramName, MAX_PATH);
    }

    if(0 != szPathName)
    {
        _snprintf(m_szLogPath, MAX_PATH - 1, "%s", szPathName);
        m_szLogPath[MAX_PATH - 1] = 0;
    }
    else
    {
        DbgUtils::SetProgramName(m_szLogPath, MAX_PATH);
    }

    m_lpBuffer = new (std::nothrow) char[MAX_LOG_BUFFER];
}


CServerLog::~CServerLog()
{
    ServerLogLock::Syncronize sync(m_ServerLogLock);

    SimpleLog(LOG_NORMAL, "로그를 끝냅니다.");

    if(0 != m_dwBufferUsed)
    {
        InternalFlush();
    }

	if(INVALID_HANDLE_VALUE != m_hFile)
	{
		CloseHandle(m_hFile);
	}

    delete [] m_lpBuffer;
    m_lpBuffer = 0;
}

void _cdecl CServerLog::Log(const char* pFormat, ...)
{
    // Lock and Push
    ServerLogLock::Syncronize sync(m_ServerLogLock);

	va_list	arguments;
	va_start_ext(arguments, pFormat);

    int nLength = _vsntprintf(m_lpBuffer + m_dwBufferUsed, 
        MAX_LOG_BUFFER - m_dwBufferUsed, pFormat, arguments);
    
    va_end(arguments);

    if(0 < nLength)
    {
        m_dwBufferUsed += nLength;
        m_dwTotalLoggedSize += nLength;
    }

    if(MAX_LOG_BUFFER_BOUND <= m_dwBufferUsed)
    {
        InternalFlush();
    }
}

void _cdecl CServerLog::SimpleLog(LOG_TYPE eLogType, const char* pFormat, ...)
{
/*
	LOG_NORMAL,		// 일반 로그 (클래스 생성, 종료)
	LOG_RULE,		// 운영 관련 로그		
	LOG_INFO,		// 정보 관련 로그
*/
	if(!(m_dwLogEnabled & (1 << eLogType)))
	{
		// Enabled된 로그가 아니다. 찍지 않는다.
		return;
	}

    if(0 != m_lpBuffer)
    {
        int  nLogSize = 0;
        char szBuffer[MAX_LOG_PER_SIZE];

        switch(eLogType)
        {
        case LOG_NORMAL:

            nLogSize = _snprintf(szBuffer, MAX_LOG_PER_SIZE, "[Ty-%s][", s_LogType[eLogType]);
            break;

        case LOG_RULE:
        case LOG_INFO:
            {            
                SYSTEMTIME  sysTime;
                GetLocalTime(&sysTime);

                nLogSize = _snprintf(szBuffer, MAX_LOG_PER_SIZE, 
                    "[Ty-%s][Tm-%04d-%02d-%02d %02d:%02d:%02d][Ex-", 
                    s_LogType[eLogType], sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
                    sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
            }
            break;
        };

	    va_list	arguments;
	    va_start_ext(arguments, pFormat);

        nLogSize += _vsntprintf(szBuffer + nLogSize, 
            MAX_LOG_PER_SIZE - nLogSize, pFormat, arguments);

        va_end(arguments);

        // Lock and Push
        ServerLogLock::Syncronize sync(m_ServerLogLock);

        int nLength = _snprintf(m_lpBuffer + m_dwBufferUsed,
            MAX_LOG_BUFFER - m_dwBufferUsed, "%s]\r\n", szBuffer);

        if(0 < nLength)
        {
            m_dwBufferUsed += nLength;
            m_dwTotalLoggedSize += nLength;
        }

        if(MAX_LOG_BUFFER_BOUND <= m_dwBufferUsed)
        {
            InternalFlush();
        }
    }
}


void _cdecl CServerLog::DetailLog(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, 
                                  int nLine, const char* pFormat, ...)
{
/*
	LOG_DETAIL,		// 하이 디테일 관련 로그
	LOG_SYSERR,		// 시스템 함수 에러 관련 로그
	LOG_ERROR,		// 에러 관련 로그
	LOG_DEBUG,		// 디버그 관련 로그
	LOG_WARN,		// 경고 관련 로그	
*/
	if(!(m_dwLogEnabled & (1 << eLogType)))
	{
		// Enabled된 로그가 아니다. 찍지 않는다.
		return;
	}

    if(0 != m_lpBuffer)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
     
        int  nLogSize = 0;
        char szBuffer[MAX_LOG_PER_SIZE];
        
        nLogSize = _snprintf(szBuffer, MAX_LOG_PER_SIZE, 
            "[Ty-%s][Tm-%04d-%02d-%02d %02d:%02d:%02d][Ex-", s_LogType[eLogType],
            sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
        
	    va_list	arguments;
	    va_start_ext(arguments, pFormat);

        nLogSize += _vsntprintf(szBuffer + nLogSize, MAX_LOG_PER_SIZE - nLogSize,
            pFormat, arguments);

        va_end(arguments);

        ServerLogLock::Syncronize sync(m_ServerLogLock);

	    int nLength = _snprintf(m_lpBuffer + m_dwBufferUsed, 
            MAX_LOG_BUFFER - m_dwBufferUsed,
		    "%s][Rt-%s][FN-%s][LN-%d]\r\n", 
            szBuffer, pRtn, pFileName, nLine);

        if(0 < nLength)
        {
            m_dwBufferUsed += nLength;
            m_dwTotalLoggedSize += nLength;
        }

        if(m_dwBufferUsed >= MAX_LOG_BUFFER_BOUND ||  eLogType == LOG_SYSERR)
        {
            InternalFlush();
        }
    }
}

void CServerLog::SetLogFileName(const char* szLogName, const char* szPathName)
{
    ServerLogLock::Syncronize sync(m_ServerLogLock);

    if(0 < m_dwBufferUsed)
    {
        InternalFlush();
    }

    if(INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    
    if(0 != szLogName)
    {
        _snprintf(m_szProgramName, MAX_PATH - 1, "%s", szLogName);
        m_szProgramName[MAX_PATH - 1] = 0;
    }
    else
    {
        DbgUtils::SetProgramName(m_szProgramName, MAX_PATH);
    }

    if(0 != szPathName)
    {
        _snprintf(m_szLogPath, MAX_PATH - 1, "%s", szPathName);
        m_szLogPath[MAX_PATH - 1] = 0;
    }
    else
    {
        DbgUtils::SetProgramName(m_szLogPath, MAX_PATH);
    }
}

HANDLE CServerLog::CreateLogFile()
{
    // 1. 로그 디렉토리를 생성.
    // 2. 로그 파일을 생성.
    // 3. 파일을 열어서 핸들 리턴.
        
    char szTime[MAX_PATH];
    char szFileName[MAX_PATH];

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_szLogPath))
    {
		if (!CreateDirectory(m_szLogPath, 0))
        {
			return INVALID_HANDLE_VALUE;
		}
	}

    SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);
	
	_snprintf(szTime, MAX_PATH, TEXT("%s%04d-%02d-%02d %02d,%02d,%02d-"), 
		m_szProgramName, sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    // create log file name in good order
	for(unsigned long dwSpinCount = 0; TRUE; ++dwSpinCount)
	{
		int nLength = _snprintf(szFileName, MAX_PATH, "%s/%s%04d.log", 
            m_szLogPath, szTime, dwSpinCount);

        if(nLength < 0)
        {
            return INVALID_HANDLE_VALUE;
        }

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(szFileName))
        {
			break;
		}
	}
    
    m_dwCurrentFileWriteSize = 0;

    return CreateFile(szFileName, GENERIC_WRITE, 
        FILE_SHARE_READ, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
}

void CServerLog::InternalFlush()
{
    unsigned long dwWritten = 0;

    if(INVALID_HANDLE_VALUE == m_hFile)
    {
        m_hFile = CreateLogFile();
    }

    WriteFile(m_hFile, m_lpBuffer, m_dwBufferUsed, &dwWritten, 0);

    m_dwCurrentFileWriteSize += dwWritten;
    m_dwBufferUsed = 0;

    if(MAX_LOG_FILE_SIZE < m_dwCurrentFileWriteSize)
    {
        CloseHandle(m_hFile);
        m_hFile = CreateLogFile();
    };
}


void CServerLog::Dump(const char* szSignature, char* lpDumpPos, size_t nLength)
{
    FILE* fFile = fopen("DumpMemory.txt", "ab");
    if(0 != fFile)
    {
        SYSTEMTIME  sysTime;
	    GetLocalTime(&sysTime);
	
        __try
        {
            fprintf(fFile, "\n %s : Dump Memory [%04d-%02d-%02d %02d,%02d,%02d]: %s  \r\n<", 
                m_szProgramName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond, szSignature);

            fwrite(lpDumpPos, nLength, 1, fFile);
        }
        __finally
        {
            fprintf(fFile, ">\r\n");
            fclose(fFile);
        }
    }
}

bool CServerLog::NewLog()
{
	if(INVALID_HANDLE_VALUE != m_hFile)
		CloseHandle(m_hFile);

	m_hFile = CreateLogFile();

	if(INVALID_HANDLE_VALUE == m_hFile)
		return false;
	else
		return true;
}
