// RylAuthServer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Resource.h"

#include <Thread/Lock.h>                    // CNamedMutex
#include <Log/ServerLog.h>
#include <Log/GameLog.h>
#include <Utility/Debug/ExceptionReport.h>  // g_CExceptionReport

#include "RylAuthServer.h"                  // CRylAuthServerWindow

int WINAPI ExceptionUserFunc(TCHAR* szBuffer, const int nBufferSize)
{
	SERLOG0(g_Log, "Flush log");
	SERLOG0(g_SessionLog, "Flush log");

	CGameLog::GetInstance().Flush();

	return _snprintf(szBuffer, nBufferSize, "Userdata flush completed.");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CNamedMutex	Mutex("ROWAuthServer", TRUE);//was ROW hayzohar

	if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
		ERRLOG0(g_Log, "AuthServer already operating now. please shutdown and restart");
		return 0;
	}

	unsigned long dwExceptionFeatures = CExceptionReport::CATCH_EXCEPTION | 
		CExceptionReport::USE_MINIDUMP | CExceptionReport::USE_REPORT;

	CExceptionReport::GetInstance().Enable(dwExceptionFeatures);
	CExceptionReport::GetInstance().SetUserFunc(ExceptionUserFunc);

	// MiniDumpWithFullMemory, MiniDumpNormal
    CExceptionReport::GetInstance().SetDumpLevel(MiniDumpNormal);

    CRylAuthServer& AuthServer = CRylAuthServer::GetInstance();

    if(AuthServer.Initialize(hInstance, "Auth Server", lpCmdLine,
		IDI_MAIN, IDR_MENU))
    {
        AuthServer.ProcessMessage();
    }

    return 0;
}

