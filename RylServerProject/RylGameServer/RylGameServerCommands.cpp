#include "stdafx.h"
#include "RylGameServer.h"

#include <Log/ServerLog.h>
#include <Network/IOCP/IOCPNet.h>
#include <Commands/Commands.h>

#include <Creature/CreatureManager.h>
#include <Map/FieldMap/CellManager.h>
#include <RylGameLibrary/Log/GameLog.h>

#include <Utility/Debug/PerformanceCheck.h>
#include <Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

#include <Network/Packet/PacketStatistics.h>

#include <Commands/DummyCharacters.h>

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
        CRylGameServer::GetInstance().StartServer();
        return true;
    }
};

class CCMDConnect : public CConsoleCMDSingleton<CCMDConnect>
{
protected:
    virtual bool DoProcess()
    {
        CRylGameServer::GetInstance().ConnectToAgent();
        CRylGameServer::GetInstance().ConnectToLogServer();
        CRylGameServer::GetInstance().ConnectToChatServer();
        return true;
    }
};

class CCMDShowStatistics : public CConsoleCMDSingleton<CCMDShowStatistics>
{
protected:
    virtual bool DoProcess()
    {
        CRylGameServer&     RylGameServer   = CRylGameServer::GetInstance();
        CCreatureManager&   CreatureManager = CCreatureManager::GetInstance();

        RylGameServer.PrintStatistics();
        RylGameServer.PrintServerInfo();
	
        return true;
    }
};


class CCMDPrintLog : public CConsoleCMDSingleton<CCMDPrintLog>
{
protected:
    virtual bool DoProcess()
    {
        GetFunctionTimingResult("ROWGameServer");//was ROW hayzohar
        CCellManager::GetInstance().CheckCellStatus();
        return true;
    }
};

class CCMDFlushLog : public CConsoleCMDSingleton<CCMDFlushLog>
{
protected:
    virtual bool DoProcess()
	{
		CGameLog::GetInstance().Flush();
        SERLOG0(g_Log, "Flush log.");
        SERLOG0(g_SessionLog, "Flush log.");
        return true;
    }
};


class CCMDReloadSetup : public CConsoleCMDSingleton<CCMDReloadSetup>
{
protected:
    virtual bool DoProcess()
    {
        CRylGameServer::GetInstance().ReloadSetup();
        return true;
    }
};


class CCMDDummyCharacters : public CConsoleCommand
{
protected:

    virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength)
    {
        const int MAX_BUFFER = 512;
        char szCommandBuf[MAX_BUFFER];
        const char* szDelimit = " \t\r\n";

        strncpy(szCommandBuf, szCommand, MAX_BUFFER - 1);
        szCommandBuf[MAX_BUFFER - 1] = 0;

        strtok(szCommandBuf, szDelimit);
        char* szCharNum = strtok(0, szDelimit);

        CCMDDummyCharacters* lpDummyCharacterCMD = 0;

        if(0 != szCharNum)
        {
            lpDummyCharacterCMD = new CCMDDummyCharacters;
            if(0 != lpDummyCharacterCMD)
            {
                lpDummyCharacterCMD->m_dwLoadCharNum = atol(szCharNum);
            }            
        }

        return lpDummyCharacterCMD;
    }

    virtual bool Destroy() { delete this; return true; }

    virtual bool DoProcess()
    {
        CRylGameServer::GetInstance().PrintOutput("DummyCharacter %u Initialize %s",
            m_dwLoadCharNum, CDummyCharacterList::GetInstance().Initialize(m_dwLoadCharNum) ? "succeeded" : "failed");
        
        return true;
    }

    unsigned long m_dwLoadCharNum;
};

class CCMDClearDummyCharacters : public CConsoleCMDSingleton<CCMDClearDummyCharacters>
{
protected:

    virtual bool DoProcess()
    {
        CDummyCharacterList::GetInstance().Destroy();
        CRylGameServer::GetInstance().PrintOutput("DummyCharacter Uninitialized");
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


bool CRylGameServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;

	ADD_COMMAND("startserver",	CCMDStartServer,	szErrorMessage);
	ADD_COMMAND("connect",		CCMDConnect,		szErrorMessage);
	ADD_COMMAND("pool",			CCMDShowStatistics,	szErrorMessage);
	ADD_COMMAND("log",			CCMDPrintLog,		szErrorMessage);
	ADD_COMMAND("reloadsetup",	CCMDReloadSetup,	szErrorMessage);
	ADD_COMMAND("flush",		CCMDFlushLog,		szErrorMessage);
	ADD_COMMAND("itemdrop",		CCMDDropItem,		szErrorMessage);
	ADD_COMMAND("itemdroplist",	CCMDDropItemList,	szErrorMessage);
	ADD_COMMAND("notify",		CCMDNotify,			szErrorMessage);
	ADD_COMMAND("autobalance",	CCMDAutoBalance,	szErrorMessage);
	ADD_COMMAND("lotteryevent",	CCMDLotteryEvent,	szErrorMessage);

    ADD_COMMAND("statclear",    CCMDStatClear,      szErrorMessage);
    ADD_COMMAND("statlog",      CCMDStatLog,        szErrorMessage);

	ADD_COMMAND("setdummies",   CCMDDummyCharacters,        szErrorMessage);
    ADD_COMMAND("resetdummies", CCMDClearDummyCharacters,   szErrorMessage);

	ADD_COMMAND("newlog",		CCMDNewLog,			szErrorMessage);

	



	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

	return true;
}
