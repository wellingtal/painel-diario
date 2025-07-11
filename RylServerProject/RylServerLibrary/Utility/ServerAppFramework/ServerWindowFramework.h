#ifndef _CSERVER_WINDOW_FRAMEWORK_
#define _CSERVER_WINDOW_FRAMEWORK_

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <climits>
#include <Thread/Lock.h>
#include <Thread/Thread.h>

#include <list>

// ���� ����
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


// ���μ��� ������
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

	virtual unsigned int Run();	// ���� ���� �Ǵ� ������ �ִ´�.
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
	CConsoleCMDFactory*	GetCommandFactory() { return m_lpCommandFactory;  }	// ���ο��� ����
	
private:

    bool InitWindow(WNDCLASS& wndClass, const char* szWndApplicationName);
    void Destroy();

protected:

    // ����� �޾Ƽ� ���� ������ �����ϵ��� ��.
    CServerWindowFramework();
    virtual ~CServerWindowFramework();

    // �ݵ�� �ʱ�ȭ�� �� �־�� ��.
    bool InitializeFramework(HINSTANCE hInstance, const TCHAR* szWndApplicationName, int nICON_ID, int nMenu_ID);

	// �޽��� ���μ��� �߰�, Ŀ�ǵ� �߰�, Listener�߰� ���� ó���� �Ѵ�.
	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine) = 0;

	// ó�� ������ �߰�.
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

	CSysTray*			m_lpSysTray;            // ���ο��� ����
    CConsoleWindow*		m_lpConsoleWindow;      // ���ο��� ����
    CCommandProcess*	m_lpCommandProcess;		// ���ο��� ����
    CConsoleCMDFactory*	m_lpCommandFactory;		// ���ο��� ����
    
	CIOCPNet*			m_lpIOCPNet;			// ���ο��� ����

	CMsgProcessMgr*		m_lpMsgProcessMgr;		// �޽��� ���μ��� �Ŵ���
	CTimerProcMgr*		m_lpTimerProcessMgr;	// Ÿ�̸� ���μ��� �Ŵ���

	CBufferFactory*		m_lpBufferFactory;		// ���� Factory

	ProcessThreadList	m_ProcessThreadList;

    char                m_szAppName[MAX_PATH];

    static const char*	ms_this;
	static const UINT	ms_SendDataMsg;
};

#endif