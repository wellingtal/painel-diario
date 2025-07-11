#include "stdafx.h"
#include "RylChatServer.h"
#include <BaseLibrary/Log/ServerLog.h>
#include <BaseLibrary/Network/IOCP/IOCPNet.h>
#include <RylServerLibrary/Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

#include <Network/Packet/PacketStatistics.h>

#include "ChatLog.h"

class CCMDStatClear : public CConsoleCMDSingleton<CCMDStatClear>
{
protected:
    virtual bool DoProcess()
    {
        CPacketStatistics::GetInstance().Clear();
        return true;
    }
};

class CCMDStatLog : public CConsoleCMDSingleton<CCMDStatLog>
{
protected:
    virtual bool DoProcess()
    {
        CPacketStatistics::GetInstance().Log();
        return true;
    }
};

class CCMDShowStatistics : public CConsoleCMDSingleton<CCMDShowStatistics>
{
protected:
	virtual bool DoProcess()
	{
		CRylChatServer::GetInstance().PrintStatistics();
		CRylChatServer::GetInstance().PrintServerInfo();
		return true;
	}
};


class CCMDPrintLog : public CConsoleCMDSingleton<CCMDPrintLog>
{
protected:
	virtual bool DoProcess()
	{
        const int MAX_BUFFER = 4096;
        char szBuffer[MAX_BUFFER];

        if(CRylChatServer::GetInstance().MakeChatLog(szBuffer, MAX_BUFFER))
        {
            SERLOG1(g_Log, "채팅 통계를 남깁니다 : \r\n%s", szBuffer);
        }

		return true;
	}
};

class CCMDFlush : public CConsoleCMDSingleton<CCMDFlush>
{
protected:
    virtual bool DoProcess()
    {
        SERLOG0(g_Log, "로그를 Flush합니다");
        SERLOG0(g_SessionLog, "로그를 Flush합니다.");
        CChatLog::GetInstance().Flush();
        return true;
    }
};

class CCMDConnect : public CConsoleCMDSingleton<CCMDConnect>
{
protected:
    virtual bool DoProcess()
    {
        CRylChatServer::GetInstance().ConnectToAgentServer();
        return true;
    }
};

class CCMDReloadSetup : public CConsoleCMDSingleton<CCMDReloadSetup>
{
protected:
    virtual bool DoProcess()
    {
		CRylChatServer::GetInstance().ReloadSetup();
        return true;
    }
};

class CCMDNewLog : public CConsoleCMDSingleton<CCMDNewLog>
{
protected:
	virtual bool DoProcess()
	{
		if (!g_ChattingLog.NewLog())
		{
			SERLOG1(g_Log, "this:0x%p/New chatlog make failed", this);
			return false;
		}

		if (!g_Log.NewLog())
		{
			SERLOG1(g_Log, "this:0x%p/New serverlog make failed", this);
			return false;
		}        

		return true;
	}
};

bool CRylChatServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;

	ADD_COMMAND("pool",			CCMDShowStatistics,	szErrorMessage);
	ADD_COMMAND("log",			CCMDPrintLog,		szErrorMessage);
	ADD_COMMAND("flush",		CCMDFlush,			szErrorMessage);
	ADD_COMMAND("connect",		CCMDConnect,		szErrorMessage);
	ADD_COMMAND("reloadsetup",	CCMDReloadSetup,	szErrorMessage);
    
    ADD_COMMAND("statclear",    CCMDStatClear,      szErrorMessage);
    ADD_COMMAND("statlog",      CCMDStatLog,        szErrorMessage);

	ADD_COMMAND("newlog",	    CCMDNewLog,			szErrorMessage);

	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

    return true;
}