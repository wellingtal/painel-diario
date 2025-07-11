
#include "stdafx.h"

#include <Log/ServerLog.h>
#include <Log/GameLog.h>

#include <Utility/Filter/Filter.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

#include <Network/IOCP/IOCPNet.h>
#include <Network/Packet/PacketStatistics.h>

#include "MasterPassword.h"
#include "RylAuthServer.h"
#include "AuthorizeUser.h"


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

class CCMDStartServer : public CConsoleCMDSingleton<CCMDStartServer>
{
protected:
    virtual bool DoProcess()
    {
        CRylAuthServer::GetInstance().InitializeClientListener();
        CRylAuthServer::GetInstance().ConnectToDBAgent();
        return true;
    }
};

class CCMDFlushLog : public CConsoleCMDSingleton<CCMDFlushLog>
{
protected:
    virtual bool DoProcess()
    {
        CRylAuthServer::GetInstance().PrintStatistics();
        CRylAuthServer::GetInstance().PrintServerInfo();   

        GetFunctionTimingResult("ROWAuthServer");//was ROW hayzohar

        CGameLog::GetInstance().Flush();

        SERLOG0(g_Log, "Flush Log.");
        SERLOG0(g_SessionLog, "Flush Log.");

        return true;
    }
};

class CCMDReloadSetup : public CConsoleCMDSingleton<CCMDReloadSetup>
{
protected:
    virtual bool DoProcess()
    {
        CRylAuthServer::GetInstance().ReloadSetup();
        return true;
    }
};

class CCMDMasterPass : public CConsoleCMDSingleton<CCMDMasterPass>
{
protected:
    virtual bool DoProcess()
    {
        Filter::InitFilter();

        // 마스터 패스워드 초기화
        if(!CMasterPassword::GetInstance().LoadMasterPassword())
        {
            SERLOG1(g_Log, "this:0x%p/Master password reload failed", this);
            return false;
        }        
        return true;
    }
};

class CCMDNewLog : public CConsoleCMDSingleton<CCMDNewLog>
{
protected:
	virtual bool DoProcess()
	{
		if (!CGameLog::GetInstance().NewLog())
		{
			SERLOG1(g_Log, "this:0x%p/New gamelog make failed", this);
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

class CCMDDBConnect : public CConsoleCMDSingleton<CCMDDBConnect>
{
protected:
	virtual bool DoProcess()
	{
		CAuthorizeUser::GetInstance().ConnectAll();
		return true;
	}
};

bool CRylAuthServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;

	ADD_COMMAND("startserver",	CCMDStartServer,	szErrorMessage);
	ADD_COMMAND("reloadsetup",	CCMDReloadSetup,	szErrorMessage);
	ADD_COMMAND("flush",		CCMDFlushLog,		szErrorMessage);
	ADD_COMMAND("reloadpass",	CCMDMasterPass,		szErrorMessage);

	ADD_COMMAND("dbconnect",	CCMDDBConnect,		szErrorMessage);

    ADD_COMMAND("statclear",    CCMDStatClear,      szErrorMessage);
    ADD_COMMAND("statlog",      CCMDStatLog,        szErrorMessage);

	ADD_COMMAND("newlog",		CCMDNewLog,			szErrorMessage);

	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

    return true;
}
