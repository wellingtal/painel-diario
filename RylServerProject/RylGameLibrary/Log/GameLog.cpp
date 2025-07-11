
#include "stdafx.h"

#include <Thread/ThreadMgr.h>
#include <Utility/Debug/DebugUtils.h>

#include "GameLog.h"


CGameLog& CGameLog::GetInstance()
{
    static CGameLog gameLog;
    return gameLog;
}


CLogSaveThread::CLogSaveThread()
:	m_hFlush(CreateEvent(0, TRUE, FALSE, 0)),
	m_hFile(INVALID_HANDLE_VALUE), m_dwTotalWritten(0)
{
	InterlockedExchange(&m_bEnd, FALSE);
} 

CLogSaveThread::~CLogSaveThread()
{
    if(INVALID_HANDLE_VALUE != m_hFile)
    { 
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }

    if(0 != m_hFlush) 
    {
        CloseHandle(m_hFlush); 
        m_hFlush = 0;
    }	
}


unsigned int CLogSaveThread::Run()
{
	for(;TRUE != InterlockedCompareExchange(&m_bEnd, TRUE, TRUE);)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hFlush, 1000))
		{
			continue;
		}
		
		WriteLog();
	};
	
	WriteLog();
	return 0;
}


BOOL CLogSaveThread::End()
{
	InterlockedExchange(&m_bEnd, TRUE);
	SetEvent(m_hFlush);
	return TRUE;
}


bool CLogSaveThread::WriteLog()
{
	if(INVALID_HANDLE_VALUE == m_hFile)
	{
		SetLogFileName();
		m_hFile = CreateFile(m_szLogFileName, GENERIC_WRITE,
			FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if(INVALID_HANDLE_VALUE == m_hFile)
		{
			return false;
		}
	}

	CGameLog::GetInstance().SpliceOutFullBuffer(m_WriteBufferList);

	if(m_WriteBufferList.empty())
	{
		return true;
	}

	unsigned long dwWritten = 0;

	for(LogBuffer::List::iterator itr = m_WriteBufferList.begin();
		itr != m_WriteBufferList.end(); ++itr)
	{
		LogBuffer* pLogBuffer = *itr;
		if(FALSE == WriteFile(m_hFile, pLogBuffer->m_Buffer, pLogBuffer->m_nUsage, &dwWritten, 0))
		{
            ERRLOG3(g_Log, "로그 파일 기록에 실패했습니다. ErrorNum : %d, FileHandle:0x%p, 버퍼 크기:%d", 
                GetLastError(), m_hFile, pLogBuffer->m_nUsage);
		}

		m_dwTotalWritten += dwWritten;
	}

	CGameLog::GetInstance().SpliceInFreeBuffer(m_WriteBufferList);
	

	if(m_dwTotalWritten > MAX_FILE_SIZE)
	{
        if(INVALID_HANDLE_VALUE != m_hFile) { CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE; }	// 파일 닫고...
		Compress();
		m_dwTotalWritten = 0;
	}

	return true;
}

bool CLogSaveThread::Compress()
{
	HANDLE hFile = CreateFile(m_szLogFileName, GENERIC_READ, 
		FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		ERRLOG0(g_Log, "게임 로그를 압축할 수 없습니다.");
		return false;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	unsigned long dwSpinCount = 0;
	char szCompressedLogFileName[MAX_PATH*2];
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_tsplitpath(m_szLogFileName, drive, dir, fname, ext);	

	while (TRUE) 
	{		
		_snprintf(szCompressedLogFileName, MAX_PATH*2, "%s\\%s-%04d%02d%02d-%02d%02d%02d-%04d.zip",
			dir, fname, sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond, dwSpinCount);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(szCompressedLogFileName))
		{
			break;
		}

		++dwSpinCount;
	}

	gzFile file = gzopen(szCompressedLogFileName, "wb");
	if(0 == file)
	{
		ERRLOG0(g_Log, "게임 로그를 압축해 넣을 파일을 만들 수 없습니다.");
		CloseHandle(hFile);
		return false;
	}

	unsigned long	dwRead = 0;
	char	szBuffer[4096];

	while(FALSE != ReadFile(hFile, szBuffer, 4096, &dwRead, 0))
	{
		if(0 == dwRead)
		{
			break;
		}

		if(0 == gzwrite(file, szBuffer, dwRead))
		{
			ERRLOG0(g_Log, "에러로 인해 압축 파일에 기록할 수 없습니다.");
			break;
		}
	}

	gzclose(file);
	CloseHandle(hFile);

	int nError = GetLastError();
	if(0 != nError && ERROR_HANDLE_EOF != nError)
	{
		ERRLOG1(g_Log, "%d 에러로 인해 로그 파일 압축이 실패했습니다.", nError);
		return false;
	}
	
	if(FALSE == DeleteFile(m_szLogFileName))
	{
		ERRLOG1(g_Log, "%d 에러로 인해 압축하고 난 로그를 삭제하지 못했습니다.", nError);
		return false;
	}		

	return true;
}



bool CLogSaveThread::SetLogFileName()
{
    char szProgramName[MAX_PATH];
	char szLogFilePrefix[MAX_PATH];

    DbgUtils::SetProgramName(szProgramName, MAX_PATH);
    szProgramName[MAX_PATH - 1] = 0;

    _snprintf(szLogFilePrefix, MAX_PATH - 1, "%s", 
        CGameLog::GetInstance().GetLogFilePrefix());

    szLogFilePrefix[MAX_PATH - 1] = 0;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	// create the directory if it doesn't exist
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(szProgramName))
	{
		if (!CreateDirectory(szProgramName, 0))
		{			
			return false;
		}
	}

	unsigned long dwSpinCount = 0;

	while (TRUE) 
	{
		int LogLen = _snprintf(m_szLogFileName, MAX_PATH, 
			"%s\\%sGameLog-%04d%02d%02d-%02d%02d%02d-%04d.log", 
			szProgramName, szLogFilePrefix, sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond, dwSpinCount);

		if(LogLen <= 0)
		{
			ERRLOG0(g_Log, "게임 로그를 위한 파일 이름을 생성할 수 없습니다.");
			return false;
		}

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_szLogFileName))
		{
			break;
		}

		++dwSpinCount;
	}

	return true;
}

bool CLogSaveThread::NewLog()
{
	if(INVALID_HANDLE_VALUE != m_hFile)
		CloseHandle(m_hFile);

	SetLogFileName();
	m_hFile = CreateFile(m_szLogFileName, GENERIC_WRITE,
		FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if(INVALID_HANDLE_VALUE == m_hFile)
		return false;
	else
		return true;
}

CGameLog::CGameLog()
:	m_lpLogBuffer(0)
{
    memset(m_szLogFilePrefix, 0, sizeof(char) * MAX_PATH);
}


CGameLog::~CGameLog()
{
	Destroy();
}


bool CGameLog::Initialize(const char* szLogFilePrefix)
{
    if(0 == szLogFilePrefix)
    {
        DbgUtils::SetProgramName(m_szLogFilePrefix, MAX_PATH);
    }
    else
    {
        _snprintf(m_szLogFilePrefix, MAX_PATH - 1, "%s", szLogFilePrefix);
        m_szLogFilePrefix[MAX_PATH - 1] = 0;
    }

	for(int nCount = 0; nCount < DEFAULT_FREE_LOG_BUFFER_NUM; ++nCount)
	{
		LogBuffer* pLogBuffer = new LogBuffer;
		if(0 == pLogBuffer)
		{
			ERRLOG0(g_Log, "게임 로그 버퍼를 할당하는 데 실패했습니다.");
			return false;
		}

		m_FreeList.push_back(pLogBuffer);
	};

    if(0 == m_lpLogBuffer)
    {
        // 버퍼가 할당되어 있지 않으면 할당한다.
    	m_lpLogBuffer = new LogBuffer;
    }

	if(0 == m_lpLogBuffer)
	{
		ERRLOG0(g_Log, "게임 로그 버퍼를 할당하는 데 실패했습니다.");
		return false;
	}
	
	if(INVALID_HANDLE_VALUE == CThreadMgr::Run(&m_LogSaveThread))
	{
		ERRLOG0(g_Log, "Flush스레드를 생성하는 데 실패했습니다.");
		return false;
	}

	return true;
}


bool CGameLog::Destroy()
{
	Flush();
	CThreadMgr::Stop(&m_LogSaveThread, INFINITE);

    LogBuffer::Lock::Syncronize sync(m_LogBufferLock);

    for(LogBuffer::List::iterator itr = m_FreeList.begin(); 
        itr != m_FreeList.end(); ++itr)
    {
        delete *itr;
    }
	m_FreeList.clear();

    for(LogBuffer::List::iterator itr = m_FullList.begin(); 
        itr != m_FullList.end(); ++itr)
    {
        delete *itr;
    }
	m_FullList.clear();
	return true;
}


char* CGameLog::ReserveBuffer(unsigned short usReserve)
{
	if(0 == m_lpLogBuffer)
	{
		m_lpLogBuffer = GetBuffer();
	}
	
	if(LogBuffer::MAX_LOG_BUFFER < m_lpLogBuffer->m_nUsage + usReserve)
	{
		Flush();
		m_lpLogBuffer = GetBuffer();		
	}

	if(0 == m_lpLogBuffer)
	{
		ERRLOG0(g_Log, "로그 버퍼가 0입니다. ");
		return 0;
	}
		
	return &m_lpLogBuffer->m_Buffer[m_lpLogBuffer->m_nUsage];
}


LogBuffer* CGameLog::GetBuffer()
{
	LogBuffer::Lock::Syncronize sync(m_LogBufferLock);

	LogBuffer*	pLogBuffer = 0;
	if(m_FreeList.empty())
	{
		pLogBuffer = new LogBuffer;
	}
	else
	{
		pLogBuffer = m_FreeList.front();
		m_FreeList.pop_front();
	}

	if(0 == pLogBuffer)
	{
		ERRLOG0(g_Log, "버퍼를 할당할 수 없습니다.");
		return 0;
	}

	pLogBuffer->Initialize();
	return pLogBuffer;
};


BOOL CGameLog::Flush()
{
	PushFullBuffer(&m_lpLogBuffer);	
	return m_LogSaveThread.FlushSignal();
}

void CGameLog::FreeBuffer(LogBuffer** ppLogBuffer)
{
	if(0 == *ppLogBuffer) { return; }

	LogBuffer::Lock::Syncronize sync(m_LogBufferLock);	
	m_FreeList.push_back(*ppLogBuffer);
	*ppLogBuffer = 0;
}

void CGameLog::PushFullBuffer(LogBuffer** ppLogBuffer)
{
	if(0 == *ppLogBuffer) { return; }

	LogBuffer::Lock::Syncronize sync(m_LogBufferLock);

	if(0 == (*ppLogBuffer)->m_nUsage)
	{
		m_FreeList.push_back(*ppLogBuffer);
	}
	else
	{
        m_FullList.push_back(*ppLogBuffer);
	}
	*ppLogBuffer = 0;
}

void CGameLog::SpliceOutFullBuffer(LogBuffer::List& logBufferList)
{
	LogBuffer::Lock::Syncronize sync(m_LogBufferLock);
	if(m_FullList.empty())
	{
		return;
	}

	logBufferList.splice(logBufferList.end(), m_FullList);
}

void CGameLog::SpliceInFreeBuffer(LogBuffer::List& logBufferList)
{
	LogBuffer::Lock::Syncronize sync(m_LogBufferLock);
	
	if(logBufferList.empty())
	{
		return;
	}

	m_FreeList.splice(m_FreeList.end(), logBufferList);
}

bool CGameLog::NewLog()
{
	if (m_LogSaveThread.NewLog())
		return true;
	else
		return false;
}