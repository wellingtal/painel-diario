#include "stdafx.h"
#include "RylUIDServer.h"
#include <Log/ServerLog.h>
#include <Network/IOCP/IOCPNet.h>
#include <Network/Packet/PacketStatistics.h>
#include <Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>
#include <Utility/Setup/ServerSetup.h>

#include <Utility/Debug/PerformanceCheck.h>
#include <DB/DBComponent.h>


class CCMDReloadConfig : public CConsoleCMDSingleton<CCMDReloadConfig>
{
protected:
    virtual bool DoProcess()
    {
        CServerSetup::GetInstance().Initialize(CServerSetup::UIDServer);
        return true;
    }
};


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

class CCMDPrintLog : public CConsoleCMDSingleton<CCMDPrintLog>
{
protected:
    virtual bool DoProcess()
    {
        CRylUIDServer::GetInstance().PrintStatistics();
        CRylUIDServer::GetInstance().PrintServerInfo();  

        SERLOG0(g_Log, "Flush log.");
        SERLOG0(g_SessionLog, "Flush log");
        return true;
    }
};

class CCMDPerfCheck : public CConsoleCMDSingleton<CCMDPerfCheck>
{
protected:

    virtual bool DoProcess()
    {
        GetFunctionTimingResult("UIDPerfCheck");
        return true;
    }
};

class CCMDNewLog : public CConsoleCMDSingleton<CCMDNewLog>
{
protected:
	virtual bool DoProcess()
	{
		if (!g_Log.NewLog())
		{
			SERLOG1(g_Log, "this:0x%p/New serverlog", this);
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
		CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_KeeperDB);
		return true;
	}
};


bool CRylUIDServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;
	
    ADD_COMMAND("reload",		CCMDReloadConfig,   szErrorMessage);
	ADD_COMMAND("flush",		CCMDPrintLog,		szErrorMessage);

	ADD_COMMAND("dbconnect",	CCMDDBConnect,		szErrorMessage);

    ADD_COMMAND("statclear",    CCMDStatClear,      szErrorMessage);
    ADD_COMMAND("statlog",      CCMDStatLog,        szErrorMessage);

    ADD_COMMAND("perflog",		CCMDPerfCheck,		szErrorMessage);

	ADD_COMMAND("newlog",		CCMDNewLog,		szErrorMessage);

	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

	return true;
}