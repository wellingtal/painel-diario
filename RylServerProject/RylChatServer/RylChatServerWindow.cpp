#include "stdafx.h"
#include "Resource.h"
#include "RylChatServer.h"

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
		case ID_START_CONSOLE:      /*m_ConsoleWindow.Initialize("RylChatServer"); */   break;//was  /*m_ConsoleWindow.Initialize("GPChatServer");*/ hayzohar
		case ID_STOP_CONSOLE:       m_ConsoleWindow.Destroy();                      break;
        case ID_CONNECT_TO_AGENT:   szCommand = "connect";         break;
		case ID_STATUS:             szCommand = "pool";            break;
		//case ID_RELOADSETUP:      szCommand = "reloadsetup";     break;
		//case ID_LOG:		        szCommand = "log";  		   break;
		}

		if(0 != szCommand)
		{
			m_ConsoleWindow.GetCMDProcess().Add(
				m_ConsoleWindow.GetConsoleCMDFactory().Create(szCommand, strlen(szCommand)));
		}

		if(LOWORD(wParam) == ID_QUIT)
		{
			DETLOG0(g_Log, "Terminate AuthServer System Tray.");
			PostMessage(hWnd, WM_QUIT, 0, 0);
		}		

		return 0;		
	}

private:

	CConsoleWindow&	m_ConsoleWindow;
};

bool CRylChatServer::InitializeMsgProc()
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
	}
   
    return (0 == nErrorCount);
}

