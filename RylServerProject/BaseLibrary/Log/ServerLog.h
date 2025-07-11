#ifndef _SERVER_LOG_H_
#define _SERVER_LOG_H_


#include "ServerLogDefine.h"
#include "../Thread/Lock.h"

class CNullLog
{
public:

    // constructor and destructor
	CNullLog(const char* szLogName = 0, const char* szPathName = 0) { }
	virtual ~CNullLog() { }

	void Enable(unsigned long dwEnableLogType) { }
	void Disable(unsigned long dwDisableLogType) { }

    void _cdecl Log(const char* pFormat, ...) { }
	void _cdecl SimpleLog(LOG_TYPE eLogType, const char* pFormat, ...) { }
    void _cdecl DetailLog(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine, const char* pFormat, ...) { }
	
	void Dump(const char* szSignature, char* lpDumpPos, size_t nLength) { }
    void Flush() {  }

    void SetLogFileName(const char* szLogName, const char* szPathName) { }
};


class CServerLog
{
public:

    // constructor and destructor
    CServerLog(const char* szLogName = 0, const char* szPathName = 0);
    virtual ~CServerLog();

	void Enable(unsigned long dwEnableLogType) { m_dwLogEnabled |= dwEnableLogType; }
	void Disable(unsigned long dwDisableLogType) { m_dwLogEnabled &= ~dwDisableLogType; }

    void _cdecl Log(const char* pFormat, ...);
    void _cdecl SimpleLog(LOG_TYPE eLogType, const char* pFormat, ...);
    void _cdecl DetailLog(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine, const char* pFormat, ...);

    void Dump(const char* szSignature, char* lpDumpPos, size_t nLength);

    void Flush() { ServerLogLock::Syncronize sync(m_ServerLogLock); InternalFlush(); }

    void SetLogFileName(const char* szLogName, const char* szPathName);

    unsigned __int64 GetTotalLoggedSize() const { return m_dwTotalLoggedSize; }
    unsigned __int64 GetTotalWriteSize() const { return m_dwTotalFileWriteSize; }
    unsigned long    GetCurrentFileWriteSize() const { return m_dwCurrentFileWriteSize; }

	bool NewLog();


protected:

	typedef CCSLock ServerLogLock;
	ServerLogLock	m_ServerLogLock;

    HANDLE  CreateLogFile();
    void    InternalFlush();

    unsigned __int64    m_dwTotalFileWriteSize;
    unsigned __int64    m_dwTotalLoggedSize;
    unsigned long       m_dwCurrentFileWriteSize;

    HANDLE              m_hFile;    
    unsigned long       m_dwBufferUsed;
	unsigned long	    m_dwLogEnabled;

    char    m_szProgramName[MAX_PATH];
    char    m_szLogPath[MAX_PATH];
    char*   m_lpBuffer;	
};


extern CServerLog g_Log;
extern CServerLog g_PacketLog;
extern CServerLog g_SessionLog;
extern CServerLog g_SkillLog;

#endif
