#include "stdafx.h"
#include "Resource.h"
#include "RylDBAgentServer.h"

#include <Log/ServerLog.h>
#include <Utility/ServerAppFramework/MsgProc/MsgProc.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleWindow.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleCMDFactory.h>

class CProcessCOMMAND : public CMsgProc
{
public:

	CProcessCOMMAND(CConsoleWindow& ConsoleWindow) : m_ConsoleWindow(ConsoleWindow) { }
	virtual ~CProcessCOMMAND() { }

	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		char* szCommand = 0;	

		switch (LOWORD(wParam)) 
		{
		case IDM_START_CONSOLE:     m_ConsoleWindow.Initialize("ROWDBAgentServer"); break;//was ROW hayzohar
		case IDM_STOP_CONSOLE:      m_ConsoleWindow.Destroy();		                break;

		case IDM_STATUS:            szCommand = "flush";            break;
		case IDM_RELOAD_CONFIG:     szCommand = "reloadsetup";      break;

		case IDM_CONNECT_ALL:       szCommand = "connect";          break;
        case IDM_DBCONNECT_ALL:       szCommand = "dbconnect";          break;

		}

		if(0 != szCommand)
		{
			m_ConsoleWindow.GetCMDProcess().Add(
				m_ConsoleWindow.GetConsoleCMDFactory().Create(szCommand, strlen(szCommand)));
		}

		if(LOWORD(wParam) == IDM_SHUTDOWN)
		{
			DETLOG0(g_Log, "Terminate AgentServer System Tray.");
			PostMessage(hWnd, WM_QUIT, 0, 0);
		}		

		return 0;		
	}

private:

	CConsoleWindow&	m_ConsoleWindow;
};


bool CRylDBAgentServer::InitializeMsgProc()
{
	int nErrorCount = 0;

	CMsgProcessMgr* lpMsgProcessMgr = GetMsgProcessMgr();
	if(0 != lpMsgProcessMgr)
	{
		if(GetConsoleWindow())
		{
            nErrorCount += lpMsgProcessMgr->Register(WM_COMMAND, 
				new CProcessCOMMAND(*GetConsoleWindow())) ? 0 : 1;
		}

        /*
		nErrorCount += lpMsgProcessMgr->Register(WM_RYLLOGIN_AUTOSTART, new CProcessRYLLOGIN_AUTOSTART) ? 0 : 1;
		nErrorCount += lpMsgProcessMgr->Register(WM_LOGIN_CONNECTPERMIT, new CProcessLOGIN_CONNECTPERMIT) ? 0 : 1;
		nErrorCount += lpMsgProcessMgr->Register(WM_RYLLOGIN_QUIT, new CProcessRYLLOGIN_QUIT) ? 0 : 1;
        */
	}
   
    return (0 == nErrorCount);
}

