#include "stdafx.h"
#include "ServerWindowFramework.h"

#include <Thread/Lock.h>
#include <Thread/ThreadMgr.h>
#include <Pattern/Command.h>

#include <Log/ServerLog.h>
#include <Network/IOCP/IOCPNet.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include "./SysTray/SysTray.h"
#include "./ConsoleWindow/ConsoleWindow.h"
#include "./ConsoleWIndow/ConsoleCMDFactory.h"

#include "./MsgProc/MsgProc.h"
#include "./MsgProc/TimerProc.h"

#include <Network/Dispatch/ServerRequest.h>

#include <Utility/Setup/ServerSetup.h>
#include <Network/Packet/ManagePacketCmd.h>

#include <mmsystem.h>

namespace ServerWindowFrameworkConst
{
	enum Const
	{
        INITIALIZE_SUCCESS		= (1 << 0),
		DESTROY_CALLED			= (1 << 1),
		DEFAULT_PROCESS_TICK	= 100			// 0.1초마다 한번씩 처리
	};
};

const char* CServerWindowFramework::ms_this = TEXT("CServerWindowFramework");
const UINT	CServerWindowFramework::ms_SendDataMsg = RegisterWindowMessage("Send WM_COPYDATAMessage");

/*
// 트레이 아이콘을 정기적으로 업데이트해 준다.
class CCheckSysTray : public CTimerProc
{
public:

	CCheckSysTray(CSysTray& sysTray) : m_sysTray(sysTray) { }
	virtual ~CCheckSysTray() { }
	virtual VOID operator () (HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) { m_sysTray.Update(); }

private:

	CSysTray&	m_sysTray;
};
*/

class CSendDataToManageClient : public CMsgProc
{
public:
	
	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CBuffer* lpBuffer = reinterpret_cast<CBuffer*>(lParam);

		if(0 != lpBuffer)
		{
            const char* szManageClientWindowName = CServerSetup::GetManageClientWindowName();

            HWND hManageWnd = FindWindow(szManageClientWindowName, szManageClientWindowName);
            if(0 != hManageWnd)
            {
                COPYDATASTRUCT copyStruct;

                copyStruct.dwData = 0;
                copyStruct.cbData = lpBuffer->length();
                copyStruct.lpData = lpBuffer->rd_ptr();

                SendMessage(hManageWnd, WM_COPYDATA, 0, (LPARAM)&copyStruct);
            }

			SAFE_RELEASE_BUFFER(lpBuffer);
		}

        return 0;
	}
};

class CRecvCommandFromManageClient : public CMsgProc
{
public:

    CRecvCommandFromManageClient(
        CServerWindowFramework& serverWindowFramework,
        CConsoleCMDFactory& commandFactory, 
        CCommandProcess& commandProcess) 
        :   m_ServerWindowFramework(serverWindowFramework),
            m_CommandFactory(commandFactory),
            m_CommandProcess(commandProcess)
    {
    
    }
    
    virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if(0 != lParam)
        {
            COPYDATASTRUCT& copyDataStruct = *reinterpret_cast<PCOPYDATASTRUCT>(lParam);
                        
            ServerManage::ManageCommand* lpManageCmd = 
                reinterpret_cast<ServerManage::ManageCommand*>(copyDataStruct.lpData);

            if(ServerManage::CMD::ExecuteCommand == lpManageCmd->GetCmd())
            {
                char szCmdBuffer[CConsoleCMDFactory::MAX_CMD_BUFFER];
                
                CCommand* lpCommand = 0;
                int nLength = _snprintf(szCmdBuffer, CConsoleCMDFactory::MAX_CMD_BUFFER - 1, 
                    "%s", reinterpret_cast<char*>(lpManageCmd + 1));

                szCmdBuffer[CConsoleCMDFactory::MAX_CMD_BUFFER - 1] = 0;

                if(0 < nLength)
                {
                    lpCommand = m_CommandFactory.Create(szCmdBuffer, nLength);
                }

                if(0 != lpCommand)
                {
                    m_CommandProcess.Add(lpCommand);
                    m_ServerWindowFramework.PrintOutput(szCmdBuffer);
                }
                else
                {
                    ERRLOG1(g_Log, "Command create failed from ManageTool. CMD:%s", szCmdBuffer);
                }
            }
        }

        return 0;
    }

private:

    CServerWindowFramework& m_ServerWindowFramework;
    CConsoleCMDFactory&     m_CommandFactory;
    CCommandProcess&        m_CommandProcess;
};


CServerWindowFramework::CServerWindowFramework()
:   m_hWnd(0),
	m_hInstance(0),
	m_nMenuID(0),
	m_dwInternalFlags(0),
    m_dwServerStatus(0),
    m_lpSysTray(0),
    m_lpConsoleWindow(0),
	m_lpCommandProcess(new (std::nothrow) CCommandProcess),
    m_lpCommandFactory(new (std::nothrow) CConsoleCMDFactory),    
	m_lpIOCPNet(new (std::nothrow) CIOCPNet),
	m_lpMsgProcessMgr(new (std::nothrow) CMsgProcessMgr),
	m_lpTimerProcessMgr(new (std::nothrow) CTimerProcMgr),
	m_lpBufferFactory(new (std::nothrow) CPoolBufferFactory)
{

}


CServerWindowFramework::~CServerWindowFramework()
{
    Destroy();
}


bool CServerWindowFramework::Initialize(HINSTANCE hInstance, const char* szWndApplicationName,
										const TCHAR* szCmdLine, int nICON_ID, int nMenu_ID)
{
	const char* szErrorLogText = 0;

#define INIT_ERR_TEXT(DetailError) TEXT("this:0x%p/ServerWindowFramework initialize failed - "##DetailError)

	TIMECAPS tc;
	memset(&tc, 0, sizeof(TIMECAPS));

	if(0 == m_lpCommandProcess || 0 == m_lpCommandFactory || 0 == m_lpIOCPNet ||
        0 == m_lpMsgProcessMgr || 0 == m_lpTimerProcessMgr || 0 == m_lpBufferFactory)
	{
		// 개체 생성 실패
		szErrorLogText = INIT_ERR_TEXT("Create defaultobject failed");
	}
	else if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
	{
		// 타이머 시간 설정 얻어오기 실패
		szErrorLogText = INIT_ERR_TEXT("Get timer resolution failed");
	}
	else if (TIMERR_NOERROR != timeBeginPeriod(min(max(tc.wPeriodMin, unsigned int(1)), tc.wPeriodMax)))
	{
		// 타이머 시간 최소값으로 세팅 실패
		szErrorLogText = INIT_ERR_TEXT("Set timer resolution failed");
	}
	else if(!InitializeFramework(hInstance, szWndApplicationName, nICON_ID, nMenu_ID))
	{
		// 프레임워크 설정 실패
		szErrorLogText = INIT_ERR_TEXT("InitialzeFramework failed");
	}
	else if(!ApplicationSpecificInit(szCmdLine))
	{
		// 메시지 루프 설정 실패
		szErrorLogText = INIT_ERR_TEXT("ApplicationSpecificInit failed");
	}

	if(0 != szErrorLogText)
	{
        ERRLOG1(g_Log, szErrorLogText, this);
		return false;
	}

#undef INIT_ERR_TEXT

    // 초기화 성공
    SendManageClientPing();
    return true;
}

bool CServerWindowFramework::isValid()
{
	return (m_dwInternalFlags & ServerWindowFrameworkConst::INITIALIZE_SUCCESS) && 
		!(m_dwInternalFlags & ServerWindowFrameworkConst::DESTROY_CALLED);
}


bool CServerWindowFramework::InitializeFramework(HINSTANCE hInstance,
                                                 const char* szWndApplicationName,
												 int nICON_ID, int nMenu_ID)
{
	if(!(m_dwInternalFlags & ServerWindowFrameworkConst::INITIALIZE_SUCCESS))
	{
        _sntprintf(m_szAppName, MAX_PATH - 1, _T("%s"), szWndApplicationName);
        m_szAppName[MAX_PATH - 1] = 0;

		m_hInstance = hInstance;
		m_nMenuID = nMenu_ID;

        WNDCLASS    wndClass;

		wndClass.hInstance		= hInstance;
		wndClass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(nICON_ID));
		wndClass.lpszClassName	= m_szAppName;

		wndClass.style			= 0;
		wndClass.lpfnWndProc	= (WNDPROC)CServerWindowFramework::ServerWindowFrameworkProc;
		wndClass.cbClsExtra		= 0;
		wndClass.cbWndExtra		= 0;
		wndClass.hCursor		= 0;
		wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wndClass.lpszMenuName	= 0;

		if (!RegisterClass(&wndClass))
		{
			ERRLOG1(g_Log, "RegisterClass failed. ErrorCode:%d.", GetLastError());
			return false;
		}
	     
		m_hWnd = CreateWindow(wndClass.lpszClassName, m_szAppName, WS_OVERLAPPEDWINDOW | WS_MINIMIZE, 
			0, 0, 0, 0, 0, 0, wndClass.hInstance, 0);

		if (0 == m_hWnd)
		{
			ERRLOG1(g_Log, "CreateWindow failed. ErrorCode:%d", GetLastError());
			return false;
		}

		if(!SetProp(m_hWnd, ms_this, this))
		{
			return false;
		}

		m_lpSysTray = new CSysTray(m_hWnd, wndClass.hInstance);
		m_lpConsoleWindow = new CConsoleWindow(wndClass.hInstance, m_hWnd,
            *m_lpCommandFactory, *m_lpCommandProcess);

		if(0 == m_lpSysTray || 0 == m_lpConsoleWindow)
		{
			return false;
		}

		if(!m_lpSysTray->AddIcon(szWndApplicationName, 
			LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(nICON_ID)), nICON_ID))
		{        
			return false;
		}
		
        if(!m_lpMsgProcessMgr->Register(ms_SendDataMsg, new CSendDataToManageClient))
        {
            return false;
        }
        else if(!m_lpMsgProcessMgr->Register(WM_COPYDATA, 
            new CRecvCommandFromManageClient(*this, *m_lpCommandFactory, *m_lpCommandProcess)))
        {
            return false;
        }

		if(!m_lpIOCPNet->Initialize())
		{
			return false;
		}

		m_dwInternalFlags |= ServerWindowFrameworkConst::INITIALIZE_SUCCESS;
	}

    return true;
}


void CServerWindowFramework::Destroy()
{
	if(!(m_dwInternalFlags & ServerWindowFrameworkConst::DESTROY_CALLED))
	{
        m_dwInternalFlags |= ServerWindowFrameworkConst::DESTROY_CALLED;

		delete m_lpIOCPNet;
		m_lpIOCPNet = 0;
	    
		delete m_lpMsgProcessMgr;
		m_lpMsgProcessMgr = 0;

		delete m_lpTimerProcessMgr;
		m_lpTimerProcessMgr = 0;

		delete m_lpSysTray;
		m_lpSysTray = 0;

		if(0 != m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = 0;
		}

		delete m_lpConsoleWindow;
		m_lpConsoleWindow = 0;

		delete m_lpCommandProcess;
		m_lpCommandProcess = 0;

		delete m_lpCommandFactory;
		m_lpCommandFactory = 0;	

		delete m_lpBufferFactory;
		m_lpBufferFactory = 0;
	}
}


void CServerWindowFramework::ProcessMessage()
{
    MSG message;

	while(GetMessage(&message, 0, 0, 0))
	{
		if(!IsDialogMessage(m_hWnd, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message); 
		}
	}

	SERLOG1(g_Log, "this:0x%p/finish server", this);
	SERLOG1(g_SessionLog, "this:0x%p/finish server", this);

	CServerRequest::GetInstance().RequestOff();
	CServerRequest::GetInstance().RemoveAllRequest();

	// 서버 종료. 스레드를 끝낸다.
	ProcessThreadList::iterator pos = m_ProcessThreadList.begin();
	ProcessThreadList::iterator end = m_ProcessThreadList.end();

	for(;pos != end; ++pos)
	{
        // TODO : 2초간 대기.. 여기서 종료시 문제 생기면, 덤프 생길 수 있음.

		CThread* lpProcessThread = *pos;
		CThreadMgr::Stop(lpProcessThread, 2000);
	};

    SERLOG1(g_Log, "this:0x%p/finish process threads", this);

	pos = m_ProcessThreadList.begin();
	end = m_ProcessThreadList.end();
	for(;pos != end; ++pos)
	{
        delete *pos;
	}

	m_ProcessThreadList.clear();

	Destroy();
}

LRESULT CALLBACK CServerWindowFramework::ServerWindowFrameworkProc(HWND hWnd, UINT uMsg, 
																   WPARAM wParam, LPARAM lParam)
{
    CServerWindowFramework* lpServerWindowFramework = GetThisPtr(hWnd);

	// Preprocess Messages
	if(WM_DESTROY == uMsg)
	{
		PostQuitMessage(0);
	}
	else if(0 != lpServerWindowFramework)
    {
		CMsgProc* lpMsgProc = 0;

		if(0 != lpServerWindowFramework->m_lpMsgProcessMgr)
		{
            lpMsgProc = lpServerWindowFramework->m_lpMsgProcessMgr->FindMsgProc(uMsg);
		}

		if(0 != lpMsgProc)
		{
			return (*lpMsgProc)(hWnd, uMsg, wParam, lParam);
		}
		else if(uMsg == CSysTray::GetSysTrayNotifyMsg())
		{
			switch(lParam) 
			{
			case WM_RBUTTONUP:
				{					
					CSysTray* lpSysTray = lpServerWindowFramework->m_lpSysTray;
					if(0 != lpSysTray)
					{
						HMENU hMenu = LoadMenu(lpServerWindowFramework->m_hInstance, 
							MAKEINTRESOURCE(lpServerWindowFramework->m_nMenuID));

						if(0 != hMenu)
						{
							HMENU hSubMenu = GetSubMenu(hMenu, 0);

							if(0 != hSubMenu)
							{
                                lpSysTray->ShowPopupMenu(hWnd, hSubMenu);
								DestroyMenu(hSubMenu);
							}

                            DestroyMenu(hMenu);
						}						
					}
				}
				break;
			}

			return 0;
		}
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CServerWindowFramework::PrintOutput(const char* lpFormat, ... )
{
	if(0 != m_lpConsoleWindow)
	{
        char szText[CConsoleWindow::MAX_MESSAGE_SIZE];

        va_list	arguments;
        va_start(arguments, lpFormat);
        
        int nLength = _vsntprintf(szText, 
            CConsoleWindow::MAX_MESSAGE_SIZE - 1, lpFormat, arguments);

        va_end(arguments);

        if(0 < nLength)
        {
            szText[nLength] = 0;
            m_lpConsoleWindow->PrintOutput(szText);
        }
	}
}

void CServerWindowFramework::PrintInfo(const char* lpFormat, ... )
{
	if(0 != m_lpConsoleWindow)
	{
        char szText[CConsoleWindow::MAX_MESSAGE_SIZE];

        va_list	arguments;
        va_start(arguments, lpFormat);

        int nLength = _vsntprintf(szText, 
            CConsoleWindow::MAX_MESSAGE_SIZE - 1, lpFormat, arguments);

        va_end(arguments);

        if(0 < nLength)
        {
            szText[nLength] = 0;
            m_lpConsoleWindow->PrintInfo(szText);
        }
	}
}


bool CServerWindowFramework::AddProcessThread(CThread* lpProcessThread)
{
	const char* szErrorString = 0;

	if(0 == lpProcessThread)
	{
		szErrorString = TEXT("this:0x%p/Process thread pointer is 0");
	}
	else if(INVALID_HANDLE_VALUE == CThreadMgr::Run(lpProcessThread))
    {
		szErrorString = TEXT("this:0x%p/Process thread start failed");
    }
	else
	{
        m_ProcessThreadList.push_back(lpProcessThread);
		return true;
	}

	ERRLOG1(g_Log, szErrorString, lpProcessThread);
	return false;
}


bool CServerWindowFramework::SendManageClientPacket(const void* lpData, unsigned short usLength)
{
    if(0 != m_lpBufferFactory)
    {
        CBuffer* lpBuffer = CREATE_BUFFER(*m_lpBufferFactory, usLength);

	    if(0 != lpBuffer)
	    {		    
    	    memcpy(lpBuffer->wr_ptr(), lpData, usLength);
		    lpBuffer->wr_ptr(usLength);

		    if(0 != PostMessage(m_hWnd, ms_SendDataMsg, 0, 
                reinterpret_cast<LPARAM>(lpBuffer)))
            {
                return true;
            }

            SAFE_RELEASE_BUFFER(lpBuffer);
	    }
    }
	return false;
}


void CServerWindowFramework::SendManageClientPing()
{
    ServerManage::PktManagePing managePing;

    GetModuleFileName(0, managePing.m_szAppFullPathName, MAX_PATH - 1);
    managePing.m_szAppFullPathName[MAX_PATH - 1] = 0;

    _snprintf(managePing.m_szWindowName, MAX_PATH - 1, "%s", m_szAppName);
    managePing.m_szWindowName[MAX_PATH - 1] = 0;

    _snprintf(managePing.m_szCommandLine, MAX_PATH * 2 - 1, "%s", GetCommandLine());
    managePing.m_szCommandLine[MAX_PATH * 2 - 1] = 0;

    managePing.m_dwStatusFlag = m_dwServerStatus;
    managePing.InitPtHead(sizeof(ServerManage::PktManagePing), ServerManage::CMD::IPC_ManagePing, 0, 0); 

    SendManageClientPacket(&managePing, managePing.GetLen());
}


// ---------------------------------------------------------------------------------------------
// 처리 스레드
//

CProcessThread::CProcessThread(CServerWindowFramework& ServerWindowFramework)
:	m_bExit(FALSE), m_nProcessTick(ServerWindowFrameworkConst::DEFAULT_PROCESS_TICK),
	m_ServerWindowFramework(ServerWindowFramework)
{
	
}

CProcessThread::CProcessThread(CServerWindowFramework& ServerWindowFramework, long nProcessTick)
:	m_bExit(FALSE), m_nProcessTick(nProcessTick),
	m_ServerWindowFramework(ServerWindowFramework)
{
	
}


CProcessThread::~CProcessThread()
{
	CThreadMgr::Stop(this, INFINITE);
}

unsigned int CProcessThread::Run()
{
	if(!m_ServerWindowFramework.isValid())
	{
		// 에러 로그 남김
		return -1;
	}

    CCommandProcess&    CommandProcess  = *m_ServerWindowFramework.GetCommandProcess();
	CIOCPNet&           IOCPNetwork     = *m_ServerWindowFramework.GetIOCPNet();
	CServerRequest&     ServerRequest   = CServerRequest::GetInstance();

	CPulse& Pulse = CPulse::GetInstance();
	Pulse.SetTicksPerPulse(m_nProcessTick);

	while(!IsEndLoop())
	{
		unsigned long dwCurrentPulse = Pulse.CheckSleep();
		
		CommandProcess.ProcessAll();    // 커맨드 큐 처리
		IOCPNetwork.Process();          // IOCP Network처리

		// 내부 동작 처리.
		InternalRun(Pulse);
		
		if(Pulse.ProcessBySecond(1))
        {
            // ServerRequest 처리. 1초마다 timeout request를 처리한다.
            ServerRequest.RemoveTimeoutRequest();	
        }
        
        if(Pulse.ProcessBySecond(5)) 
        {
            // ManageClient에 5초마다 Ping정보 및, Application Name을 전송한다.
            m_ServerWindowFramework.SendManageClientPing();
        }
	}

    // 클린업 코드를 넣는다.
    Cleanup(Pulse);
	return 0;
}

