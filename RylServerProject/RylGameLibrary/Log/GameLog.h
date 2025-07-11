#ifndef _CRYLGAMELOG_H_
#define _CRYLGAMELOG_H_

#include <winsock2.h>
#include <windows.h>

#include <ctime>
#include <list>
#include <algorithm>
#include <zlib/zlib.h>
#include <Thread/Lock.h>
#include <Thread/Thread.h>
#include <Pattern/Singleton.h>


struct LogBuffer
{
	typedef std::list<LogBuffer*>	List;
	typedef CCSLock         		Lock;

	enum { MAX_LOG_BUFFER = 65000 };

	int		m_nUsage;
	char	m_Buffer[MAX_LOG_BUFFER];

	LogBuffer() : m_nUsage(0) { }
	void Initialize() { m_nUsage = 0; }
};



class CLogSaveThread : public CThread
{
public:

	CLogSaveThread();
	virtual ~CLogSaveThread();

	BOOL FlushSignal() { return PulseEvent(m_hFlush); }
	bool NewLog();

	
protected:

	virtual unsigned int Run();	// 실제 실행 되는 루프를 넣는다.
	virtual BOOL End();			// 루프가 끝날 수 있는 루틴을 넣는다.

    bool SetLogFileName();
	bool WriteLog();
	bool Compress();

	

	enum 
    { 
        MAX_FILE_SIZE = 25 * 1024 * 1024
    };

	volatile LONG   m_bEnd;

	HANDLE	        m_hFlush;
	HANDLE	        m_hFile;
	unsigned long	m_dwTotalWritten;
	
	LogBuffer::List	m_WriteBufferList;
    
    char m_szLogFileName[MAX_PATH];
};


class CGameLog
{
public:

    static CGameLog& GetInstance();

	CGameLog();
	~CGameLog();

	bool Initialize(const char* szLogFilePrefix = 0);
	bool Destroy();
	
	char*  ReserveBuffer(unsigned short usBufferSize);
    void   UseComplete(unsigned short usRealUse) { m_lpLogBuffer->m_nUsage += usRealUse; }

    time_t GetLogTime()    { return m_time; }
    void   UpdateLogTime() { time(&m_time); }

    BOOL   Flush();

	bool NewLog();

private:

	enum 
    { 
        DEFAULT_FREE_LOG_BUFFER_NUM = 100 
    };

	LogBuffer* GetBuffer();
	void FreeBuffer(LogBuffer** ppLogBuffer);
	void PushFullBuffer(LogBuffer** ppLogBuffer);
	void SpliceOutFullBuffer(LogBuffer::List& logBufferList);
    void SpliceInFreeBuffer(LogBuffer::List& logBufferList);
	
    const char* GetLogFilePrefix() const { return m_szLogFilePrefix; }

	LogBuffer::Lock	m_LogBufferLock;

	LogBuffer::List	m_FreeList;
	LogBuffer::List	m_FullList;

	CLogSaveThread	m_LogSaveThread;
    LogBuffer*		m_lpLogBuffer;
	time_t			m_time;

    char m_szLogFilePrefix[MAX_PATH];
    
	friend class CLogSaveThread;
};


#endif