#include "stdafx.h"
#include "Resource.h"
#include "RylGameServer.h"

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
		case ID_START_CONSOLE:      m_ConsoleWindow.Initialize("RylGameServer");    break;//was /*m_ConsoleWindow.Initialize("ROWGameServer");*/  hayzohar
		case ID_STOP_CONSOLE:       m_ConsoleWindow.Destroy();                      break;

		case ID_SHOW_STATUS:        szCommand = "pool";             break;
		case ID_LOAD_SETTING:       szCommand = "reloadsetup";      break;
		case ID_START_SERVER:       szCommand = "startall";         break;
        case ID_CONNECT_ALL:        szCommand = "connect";          break;		}

		if(0 != szCommand)
		{
			m_ConsoleWindow.GetCMDProcess().Add(
				m_ConsoleWindow.GetConsoleCMDFactory().Create(szCommand, strlen(szCommand)));
		}

		if(LOWORD(wParam) == ID_SERVER_STOP)
		{
			DETLOG0(g_Log, "Terminate GameServer System Tray.");
			PostMessage(hWnd, WM_QUIT, 0, 0);
		}		

		return 0;		
	}

private:

	CConsoleWindow&	m_ConsoleWindow;
};

/*
class CProcessRYLGAME_AUTOSTART : public CMsgProc
{
public:
	
	CProcessRYLGAME_AUTOSTART(CConsoleWindow& consoleWindow)
		:	m_ConsoleWindow(consoleWindow)
	{
	
	}
	virtual ~CProcessRYLGAME_AUTOSTART() { }
	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		DETLOG0(g_Log, "AutoStart Started");

        m_ConsoleWindow.GetCMDProcess().Add(
			m_ConsoleWindow.GetConsoleCMDFactory().Create("startall", strlen("startall")));
		return 0;
	}

private:

	CConsoleWindow& m_ConsoleWindow;
};

class CProcessGAME_CONNECTTOAGENT : public CMsgProc
{
public:

	CProcessGAME_CONNECTTOAGENT(CConsoleWindow& consoleWindow)
		:	m_ConsoleWindow(consoleWindow)
	{
	
	}

	virtual ~CProcessGAME_CONNECTTOAGENT() { }
	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		DETLOG0(g_Log, "관리툴의 요청으로 서버로 접속.");

		m_ConsoleWindow.GetCMDProcess().Add(
			m_ConsoleWindow.GetConsoleCMDFactory().Create("connect", strlen("connect")));
		return 0;
	}

private:

	CConsoleWindow& m_ConsoleWindow;
};


class CProcessRYLGAME_QUIT : public CMsgProc
{
public:
	
	virtual ~CProcessRYLGAME_QUIT() { }
	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		DETLOG0(g_Log, "관리툴에 의해서 게임 서버의 시스템 트레이를 종료합니다.");
		SendMessage(hWnd, WM_COMMAND, ID_SERVER_STOP, 0);
		return 0;
	}
};


class CProcessCOPYDATA : public CMsgProc
{
public:

	CProcessCOPYDATA(CConsoleWindow& consoleWindow)
		:	m_ConsoleWindow(consoleWindow)
	{
	
	}

	virtual ~CProcessCOPYDATA() { }

	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		PCOPYDATASTRUCT lpCDStruct = reinterpret_cast<PCOPYDATASTRUCT>(lParam);

		if(NULL != lpCDStruct)
		{
			char*	szMessage	= reinterpret_cast<MNGMSG_Notify*>(lpCDStruct->lpData)->m_Message;
			size_t	nMsgLen		= strlen(szMessage);

			szMessage[MNGMSG_Notify::MAX_NOTIFY_NUM - 1] = '\0';
			m_ConsoleWindow.PrintOutput(szMessage, nMsgLen);

			if (MNGMSGTYPE_NOTIFY == lpCDStruct->dwData)
			{
				m_ConsoleWindow.GetCMDProcess().Add(
					m_ConsoleWindow.GetConsoleCMDFactory().Create(szMessage, nMsgLen));
			}
		}

		return 0;
	}

private:

	CConsoleWindow& m_ConsoleWindow;
};
*/


bool CRylGameServer::InitializeMsgProc()
{
	int nErrorCount = 0;

	CMsgProcessMgr* lpMsgProcessMgr = GetMsgProcessMgr();
	if(0 != lpMsgProcessMgr)
	{
        
		if(GetConsoleWindow())
		{
            
            nErrorCount += lpMsgProcessMgr->Register(WM_COMMAND, 
				new CProcessCOMMAND(*GetConsoleWindow())) ? 0 : 1;
            /*
			nErrorCount += lpMsgProcessMgr->Register(WM_RYLGAME_AUTOSTART, 
				new CProcessRYLGAME_AUTOSTART(*GetConsoleWindow())) ? 0 : 1;

			nErrorCount += lpMsgProcessMgr->Register(WM_GAME_CONNECTTOAGENT, 
				new CProcessGAME_CONNECTTOAGENT(*GetConsoleWindow())) ? 0 : 1;

            
			nErrorCount += lpMsgProcessMgr->Register(WM_COPYDATA, 
				new CProcessCOPYDATA(*GetConsoleWindow())) ? 0 : 1;
            */
		}
		
        /*
		nErrorCount += lpMsgProcessMgr->Register(WM_RYLGAME_QUIT, new CProcessRYLGAME_QUIT) ? 0 : 1;
        */
	}
   
    return (0 == nErrorCount);
}

