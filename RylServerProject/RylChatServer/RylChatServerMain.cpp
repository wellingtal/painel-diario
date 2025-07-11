// RylChatServer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Resource.h"

#include <Thread/Lock.h>                    // CNamedMutex
#include <Log/ServerLog.h>
#include <Utility/Debug/ExceptionReport.h>  // g_CExceptionReport

#include "RylChatServer.h"								 // CRylChatServerWindow

void __cdecl report_failure(int code, void * unused)
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

int WINAPI ExceptionUserFunc(TCHAR* szBuffer, const int nBufferSize)
{
	SERLOG0(g_Log, "Flush log");
	SERLOG0(g_SessionLog, "Flush log");

	return _snprintf(szBuffer, nBufferSize, "Userdata flush completed.");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CNamedMutex	Mutex("ROWChatServer", TRUE);//was ROW hayzohar

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ERRLOG0(g_Log, "Already chat server exist. shutdown chatserver and restart");
		return 0;
	}

    // Exception 관련 설정
    //_set_security_error_handler(report_failure);

	unsigned long dwExceptionFeatures = CExceptionReport::CATCH_EXCEPTION | 
		CExceptionReport::USE_MINIDUMP | CExceptionReport::USE_REPORT;

	CExceptionReport::GetInstance().Enable(dwExceptionFeatures);
	CExceptionReport::GetInstance().SetUserFunc(ExceptionUserFunc);

	// MiniDumpWithFullMemory, MiniDumpNormal
    CExceptionReport::GetInstance().SetDumpLevel(MiniDumpNormal);

	CRylChatServer& ChatServer = CRylChatServer::GetInstance();

	if(ChatServer.Initialize(hInstance, "Chat Server", lpCmdLine, IDI_MAIN, IDR_MENU))
	{
		ChatServer.ProcessMessage();
	}

	return 0;
}