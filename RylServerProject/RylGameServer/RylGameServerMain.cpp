// RylLoginServer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Resource.h"

#include <Log/ServerLog.h>
#include <Log/GameLog.h>
#include <Thread/Lock.h>                    // CNamedMutex
#include <Utility/Debug/ExceptionReport.h>  // g_CExceptionReport
#include <Utility/Setup/ServerSetup.h>

#include "RylGameServer.h"                             // CRylLoginServerWindow

#include <db/dbdefine.h>
#include <DB/DBComponent.h>


void __cdecl stackoverrun_handler(int code, void * unused)
{
    SERLOG0(g_Log, "Stack buffer overrun occured. shutdown server.");
	SERLOG0(g_SessionLog, "Stack buffer overrun occured. shutdown server.");

	// 일부러 Exception을 내서 덤프를 남긴다.
	__try
	{
		int nZero = 0;
		int nError = 10 / nZero;
	}
	__except(UnhandledExceptionFilter(GetExceptionInformation()))
	{
		SERLOG0(g_Log, "Shit!");
	}
    
    exit(1);
}

void __cdecl purecall_handler()
{
    SERLOG0(g_Log, "pure virtual function call occured. shutdown server.");
	SERLOG0(g_SessionLog, "pure virtual function call occured. shutdown server.");

	// 일부러 Exception을 내서 덤프를 남긴다.
	__try
	{
		int nZero = 0;
		int nError = 10 / nZero;
	}
	__except(UnhandledExceptionFilter(GetExceptionInformation()))
	{
		SERLOG0(g_Log, "Shit!");
	}
    
    exit(1);
}


int WINAPI ExceptionUserFunc(TCHAR* szBuffer, const int nBufferSize)
{
	SERLOG0(g_Log, "Flush log");
	SERLOG0(g_SessionLog, "Flush log");

	CGameLog::GetInstance().Flush();

	return _snprintf(szBuffer, nBufferSize, "Userdata flush completed.");
}

CString GetLocalIP()
{
    CString ip = "";
    char szHostName[256];
    PHOSTENT pHostInfo;

    WSADATA wsadata;
    WSAStartup(WORD(2.0), &wsadata);

    if( gethostname(szHostName,sizeof(szHostName)) ==0)
    {
        if((pHostInfo = gethostbyname(szHostName)) != NULL)
        {
            ip = inet_ntoa(*(struct in_addr *)*pHostInfo->h_addr_list); 
        }
    }
 
    WSACleanup();

    return ip;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Mutex 설정
    char szProgramName[MAX_PATH];

    _snprintf(szProgramName, MAX_PATH - 1, "%s%02d%02d", 
        "ROWGameServer", //was ROW hayzohar
        CServerSetup::GetInstance().GetZoneFromCmdLine(), 
         CServerSetup::GetInstance().GetChannelFromCmdLine());

    szProgramName[MAX_PATH - 1] = 0;

	CNamedMutex	Mutex(szProgramName, TRUE);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
		ERRLOG0(g_Log, "GameServer already operating now. please shutdown and restart");
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 바리케이트 처리
	int	iReturn = 0;
/*	CDBComponent	ClassifiedDB;

	if(!ClassifiedDB.Connect(_T("10.21.45.2"), _T("NaveAUTH"), _T("edith"), _T("wkrdmstn0508"), OleDB::ConnType_MSSQL))
	{
//		ERRLOG0(g_Log, "GameServer not System. please shutdown and restart");
		return 0;
	}

	char strIP[32];
	sprintf(strIP, GetLocalIP().GetString());

	char Query[512];
	sprintf(Query, "dbo.AuthCode '%s', '%s'", "2809468e3cdcb9cd5c3a2b3a05d4b016", strIP);

	if (!ClassifiedDB.ExecuteQueryGetData(Query, (void *)&iReturn)) 
	{
//		ERRLOG0(g_Log, "GameServer not System. please shutdown and restart");
		return 0;
	}
	ClassifiedDB.DisconnectDataSource();
*/
	///////////////////////////////////////////////////////////////////////////////
	// 리턴값체크
	if(iReturn == 1)
	{
//		ERRLOG0(g_Log, "GameServer not System. please shutdown and restart");
		return 0;
	}
	else
	{
		// Exception 관련 설정
		//_set_security_error_handler(stackoverrun_handler);
		_set_purecall_handler(purecall_handler);

		unsigned long dwExceptionFeatures = CExceptionReport::CATCH_EXCEPTION | 
			CExceptionReport::USE_MINIDUMP | CExceptionReport::USE_REPORT;

		CExceptionReport::GetInstance().Enable(dwExceptionFeatures);
		CExceptionReport::GetInstance().SetUserFunc(ExceptionUserFunc);

		// MiniDumpWithFullMemory, MiniDumpNormal
		CExceptionReport::GetInstance().SetDumpLevel(MiniDumpNormal);

		// 게임서버 시작
		CRylGameServer& GameServer = CRylGameServer::GetInstance();

		char szWindowName[MAX_PATH];
		sprintf(szWindowName, "Game Server Zone #%d Channel #%d", 
			CServerSetup::GetInstance().GetZoneFromCmdLine(), CServerSetup::GetInstance().GetChannelFromCmdLine());
		if (GameServer.Initialize(hInstance, szWindowName, lpCmdLine, IDI_GAME_SERVER_ICON, IDR_GAME_SERVER_MENU))
		{
			GameServer.ProcessMessage();
		}
	}

    return 0;
}

