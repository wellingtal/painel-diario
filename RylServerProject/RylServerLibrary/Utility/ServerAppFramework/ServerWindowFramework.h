#ifndef _CSERVER_WINDOW_FRAMEWORK_
#define _CSERVER_WINDOW_FRAMEWORK_

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <climits>
#include <Thread/Lock.h>
#include <Thread/Thread.h>

#include <list>

// 전방 참조
class CSysTray;
class CConsoleWindow;

class CCommand;
class CCommandProcess;

class CConsoleCMDFactory;
class CIOCPNet;

class CMsgProcessMgr;
class CTimerProcMgr;

class CPulse;
class CBufferFactory;
class CServerWindowFramework;


// 프로세스 스레드
class CProcessThread : public CThread
{
public:

	CProcessThread(CServerWindowFramework& ServerWindowFramework);
	CProcessThread(CServerWindowFramework& ServerWindowFramework, long nProcessTick);
	virtual ~CProcessThread();

protected:
	
	virtual void InternalRun(CPulse& Pulse) = 0;
    virtual void Cleanup(CPulse& Pulse) = 0;
	
private:

	virtual unsigned int Run();	// 실제 실행 되는 루프를 넣는다.
	virtual BOOL End() { InterlockedExchange(&m_bExit, TRUE); return TRUE; }
    BOOL IsEndLoop() { return InterlockedCompareExchange(&m_bExit, TRUE, TRUE); }

	volatile LONG   m_bExit;
	long            m_nProcessTick;
	CServerWindowFramework&	m_ServerWindowFramework;
};

class CServerWindowFramework
{
public:

	bool Initialize(HINSTANCE hInstance, const TCHAR* szWndApplicationName, 
		const char* szCmdLine, int nICON_ID, int nMenu_ID);

	HWND GetWnd() { return m_hWnd; }
	void ProcessMessage();
    
	bool isValid();

	void PrintOutput(const char* lpFormat, ...);
	void PrintInfo(const char* lpFormat, ...);

	bool SendManageClientPacket(const void* lpData, unsigned short usLength);
    void SendManageClientPing();

    void SetStatusFlag(unsigned long dwServerStatus) { m_dwServerStatus = dwServerStatus; }
    unsigned long GetStatusFlag() { return m_dwServerStatus; }

	CMsgProcessMgr*		GetMsgProcessMgr()	{ return m_lpMsgProcessMgr;   }
	CTimerProcMgr*		GetTimerProcMgr()	{ return m_lpTimerProcessMgr; }
	CIOCPNet*			GetIOCPNet()		{ return m_lpIOCPNet;		  }
	CConsoleWindow*     GetConsoleWindow()	{ return m_lpConsoleWindow;   }
	CCommandProcess*    GetCommandProcess() { return m_lpCommandProcess;  }
	CConsoleCMDFactory*	GetCommandFactory() { return m_lpCommandFactory;  }	// 내부에서 생성
	
private:

    bool InitWindow(WNDCLASS& wndClass, const char* szWndApplicationName);
    void Destroy();

protected:

    // 상속을 받아서 세부 내용을 구현하도록 함.
    CServerWindowFramework();
    virtual ~CServerWindowFramework();

    // 반드시 초기화를 해 주어야 함.
    bool InitializeFramework(HINSTANCE hInstance, const TCHAR* szWndApplicationName, int nICON_ID, int nMenu_ID);

	// 메시지 프로세스 추가, 커맨드 추가, Listener추가 등의 처리를 한다.
	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine) = 0;

	// 처리 스레드 추가.
	bool AddProcessThread(CThread* lpProcessThread);	

    static CServerWindowFramework* GetThisPtr(HWND hWnd) { return reinterpret_cast<CServerWindowFramework*>(GetProp(hWnd, ms_this)); }
	static LRESULT CALLBACK ServerWindowFrameworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static VOID CALLBACK FrameworkTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:

	typedef std::list<CThread*>	ProcessThreadList;

	HINSTANCE			m_hInstance;
    HWND				m_hWnd;
	int					m_nMenuID;
	unsigned long		m_dwInternalFlags;
    unsigned long       m_dwServerStatus;

	CSysTray*			m_lpSysTray;            // 내부에서 생성
    CConsoleWindow*		m_lpConsoleWindow;      // 내부에서 생성
    CCommandProcess*	m_lpCommandProcess;		// 내부에서 생성
    CConsoleCMDFactory*	m_lpCommandFactory;		// 내부에서 생성
    
	CIOCPNet*			m_lpIOCPNet;			// 내부에서 생성

	CMsgProcessMgr*		m_lpMsgProcessMgr;		// 메시지 프로세스 매니저
	CTimerProcMgr*		m_lpTimerProcessMgr;	// 타이머 프로세스 매니저

	CBufferFactory*		m_lpBufferFactory;		// 버퍼 Factory

	ProcessThreadList	m_ProcessThreadList;

    char                m_szAppName[MAX_PATH];

    static const char*	ms_this;
	static const UINT	ms_SendDataMsg;
};

#endif