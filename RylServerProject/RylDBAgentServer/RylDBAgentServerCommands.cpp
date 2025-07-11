#include "stdafx.h"
#include "RylDBAgentServer.h"

#include <Log/ServerLog.h>
#include <Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

#include <Network/Dispatch/LoginDispatch.h>
#include <Network/Dispatch/AuthDispatch.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Network/SendPacket/SendServerInfo.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <DataStorage/StoreData.h>
#include <DataStorage/StoreDataMgr.h>

#include <DataStorage/AdminDataMgr.h>

#include <Utility/Debug/PerformanceCheck.h>
#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <GameTime/GameTimeDBMgr.h>

class CCMDPrintLog : public CConsoleCMDSingleton<CCMDPrintLog>
{
protected:
    virtual bool DoProcess()
    {
        CRylDBAgentServer::GetInstance().PrintStatistics();
        CRylDBAgentServer::GetInstance().PrintServerInfo();

        SERLOG0(g_Log, "Flush ServerLog.");
        SERLOG0(g_SessionLog, "Flush ServerLog.");

        GetFunctionTimingResult("DBAgentLog");
        return true;
    }
};


class CCMDAliveIP : public CConsoleCMDSingleton<CCMDAliveIP>
{
protected:
	virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength);
	virtual bool Destroy() { delete this; return true; }

	virtual bool DoProcess()
	{
#ifdef AUTH_MY
		if(!g_IPSec.IsAliveIP(szIP))
		{
			SERLOG1(g_Log, "AliveIP Faild (%s).", szIP);
			CRylDBAgentServer::GetInstance().PrintOutput("AliveIP Faild.");
			return false;
		}
#endif
		SERLOG1(g_Log, "AliveIP OK (%s).", szIP);
		CRylDBAgentServer::GetInstance().PrintOutput("AliveIP OK.");
		return true;
	}

public:
	TCHAR szIP[32];
};

CConsoleCommand* CCMDAliveIP::Clone(const TCHAR* szCommand, size_t nCommandLength)
{
	const int MAX_COMMAND_BUFFER = 260;
	char szCommandBuffer[MAX_COMMAND_BUFFER];

	strncpy(szCommandBuffer, szCommand, MAX_COMMAND_BUFFER - 1);
	szCommandBuffer[MAX_COMMAND_BUFFER - 1] = 0;

	CCMDAliveIP* lpCmd = 0;

	const char* szDelimiter = " \t\r\n";
	char* szToken = strtok(szCommandBuffer, szDelimiter);
	if(0 != szToken)	
	{	
		const char* szIP = strtok(0, szDelimiter);

		lpCmd = new CCMDAliveIP;
		if(0 != lpCmd)                
		{ 
			strcpy(lpCmd->szIP, szIP);
		}                                 
	}

	return lpCmd;	
}

class CCMDReloadSetup : public CConsoleCMDSingleton<CCMDReloadSetup>
{
protected:
    virtual bool DoProcess()
    {
        CRylDBAgentServer::GetInstance().ReloadSetup();
        DBAgent::SendPacket::UpdateChannelAllServer();
        return true;
    }
};

class CCMDReloadAdmin : public CConsoleCMDSingleton<CCMDReloadAdmin>
{
protected:
    virtual bool DoProcess()
    {
        CRylDBAgentServer& DBAgentServer = CRylDBAgentServer::GetInstance();

        const char* szResult = DBAgent::DataStorage::CAdminDataMgr::GetInstance().LoadAdminList(
            CDBSingleObject::GetInstance()) ? "Adminlist reload succeeded" : "Adminlist reload failed";

        DBAgentServer.PrintOutput(szResult);
        return true;
    }
};

class CCMDConnect : public CConsoleCMDSingleton<CCMDConnect>
{
protected:	
    virtual bool DoProcess()
    {
        CRylDBAgentServer::GetInstance().ConnectAll();
        return true;
    }
};

// 게임 시간 리로드.
class CCMDReloadGameTime : public CConsoleCMDSingleton<CCMDReloadGameTime>
{
protected:

	virtual bool DoProcess()
	{
		CRylDBAgentServer& rylDBAgentServer = CRylDBAgentServer::GetInstance();

		if(!CGameTimeDBMgr::GetInstance().LoadGameTimeScript("./Script/Game/GameTimeScript.txt"))
		{
			rylDBAgentServer.PrintOutput("Reload Game Time Script Fail");
			return false;
		}

		rylDBAgentServer.PrintOutput("Reload Game Time Script Success");
		
		return true;
	}
};

// 공성 시간 출력.
class CCMDGameTime : public CConsoleCMDSingleton<CCMDGameTime>
{
protected:

	virtual bool DoProcess()
	{
		CRylDBAgentServer& rylDBAgentServer = CRylDBAgentServer::GetInstance();

		const CGameTimeDBMgr& cGameTimeDBMgr = CGameTimeDBMgr::GetInstance();

		CGameTimeDBMgr::GameTimeTable::const_iterator pos = cGameTimeDBMgr.GetGameTimeTable().begin();
		CGameTimeDBMgr::GameTimeTable::const_iterator end = cGameTimeDBMgr.GetGameTimeTable().end();

		CTime nowTime = CTime::GetCurrentTime();

		for(; pos!=end; ++pos)
		{
			const CGameTimeDBMgr::sGameTime* lpGameTime = &pos->second;

			if (NULL != lpGameTime)
			{
				char szType[0xff] = {0, };

				switch (lpGameTime->m_eTimeType)
				{
					case GameTime::GUILD:	strcpy(szType, "Guild War");	break;
					case GameTime::REALM:	strcpy(szType, "Realm War");	break;
					case GameTime::SIEGE:	strcpy(szType, "Siege");		break;
				}

				switch (lpGameTime->m_eTimeCycle)
				{
					case GameTime::WEEKLY:
					{
						char szWeek[0xff] = {0, };

						switch (lpGameTime->m_eWeek)
						{
							case GameTime::SUNDAY:		strcpy(szWeek, "Sunday");		break;
							case GameTime::MONDAY:		strcpy(szWeek, "Monday");		break;
							case GameTime::TUESDAY:		strcpy(szWeek, "Tuesday");		break;
							case GameTime::WEDNESDAY:	strcpy(szWeek, "Wednesday");	break;
							case GameTime::THURSDAY:	strcpy(szWeek, "Thursday");		break;
							case GameTime::FRIDAY:		strcpy(szWeek, "Friday");		break;
							case GameTime::SATURDAY:	strcpy(szWeek, "Saturday");		break;
						}
						
						rylDBAgentServer.PrintOutput("Type : %s, Cycle : Weekly, Date : %s %.2d:%.2d, Time : %d Minutes", szType, szWeek,
							lpGameTime->m_cHour, lpGameTime->m_cMinute, lpGameTime->m_dwDuration);
					}
					break;

				case GameTime::DAILY:
					{
						rylDBAgentServer.PrintOutput("Type : %s, Cycle : Daily, Date : %.2d:%.2d, Time : %d Minutes", szType, 
							lpGameTime->m_cHour, lpGameTime->m_cMinute, lpGameTime->m_dwDuration);
					}
					break;

				case GameTime::MONTHLY:
					{
						rylDBAgentServer.PrintOutput("Type : %s, Cycle : Monthly, Date : %.2d %.2d:%.2d, Time : %d Minutes",
							lpGameTime->m_cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, lpGameTime->m_dwDuration);
					}
					break;

				case GameTime::DATE:
					{						
						rylDBAgentServer.PrintOutput("Type : %s, Cycle : Date, Date : %.4d-%.2d-%.2d %.2d:%.2d, Time : %d Minutes",
							lpGameTime->m_wYear, lpGameTime->m_cMonth, lpGameTime->m_cDay,
							lpGameTime->m_cHour, lpGameTime->m_cMinute, lpGameTime->m_dwDuration);
					}
					break;				
				}
			}
		}
		
		return true;
	}
};

class CCMDLoadChar : public CConsoleCommand
{
protected:
    virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength);
    virtual bool Destroy() { delete this; return true; }

    virtual bool DoProcess();

    unsigned long   m_dwUID;
    unsigned char   m_cOldServerGroupID;
};

CConsoleCommand* CCMDLoadChar::Clone(const TCHAR* szCommand, size_t nCommandLength)
{
    const int MAX_COMMAND_BUFFER = 260;
    char szCommandBuffer[MAX_COMMAND_BUFFER];

    strncpy(szCommandBuffer, szCommand, MAX_COMMAND_BUFFER - 1);
    szCommandBuffer[MAX_COMMAND_BUFFER - 1] = 0;

    CCMDLoadChar* lpCmdLoadChar = 0;

    const char* szDelimiter = " \t\r\n";
    char* szToken = strtok(szCommandBuffer, szDelimiter);
    if(0 != szToken)	
    {	
        const char* szUID = strtok(0, szDelimiter);
        const char* szGroupID = strtok(0, szDelimiter);
        if(0 != szUID && 0 != szGroupID)	
        {
            unsigned long dwUID = atoi(szUID);
            unsigned char cOldServerGroupID = static_cast<unsigned char>(atoi(szGroupID));

            if(0 != dwUID && 0 != cOldServerGroupID)	
            {	
                lpCmdLoadChar = new CCMDLoadChar;
                if(0 != lpCmdLoadChar)                
                { 
                    lpCmdLoadChar->m_dwUID = dwUID;
                    lpCmdLoadChar->m_cOldServerGroupID = cOldServerGroupID;
                }                                 
            }
        }
    }

    return lpCmdLoadChar;	
}


bool CCMDLoadChar::DoProcess()
{
    // 먼저 소유 캐릭을 찾는다.
    USER_INFO userInfo;
    memset(&userInfo, 0, sizeof(USER_INFO));

    CRylDBAgentServer& rylDBAgentServer = CRylDBAgentServer::GetInstance();

    if (DBComponent::GameDB::GetUserInfo(CDBSingleObject::GetInstance(), m_dwUID, 
        userInfo, m_cOldServerGroupID))
    {
        using namespace DBAgent::DataStorage;

        CStoreDataMgr&      storeDataMgr    = CStoreDataMgr::GetInstance();
        CCharacterDataMgr&  charDataMgr     = CCharacterDataMgr::GetInstance();

        rylDBAgentServer.PrintOutput("UID:%10u / Store reload %s", m_dwUID, 
            DATA_SUCCEEDED == storeDataMgr.ReloadData(UnifiedStoreKey(m_dwUID, m_cOldServerGroupID)) 
            ? "succeeded" : "failed");

        for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
        {
            if(0 != userInfo.CharID[nCount])
            {
                // 캐릭터 로드
                rylDBAgentServer.PrintOutput("UID:%10u / CID:%10u / Character%d reload %s", 
                    m_dwUID, userInfo.CharID[nCount], nCount,
                    DATA_SUCCEEDED == charDataMgr.ReloadData(userInfo.CharID[nCount]) 
                    ? "succeeded" : "failed");
            }
        }
    }
    else
    {
        rylDBAgentServer.PrintOutput("UID:%10u / User reload failed :%s", m_dwUID, 
            CDBSingleObject::GetInstance().GetErrorString());
    }

    return true;
}

class CCMDNewLog : public CConsoleCMDSingleton<CCMDNewLog>
{
protected:
	virtual bool DoProcess()
	{
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
		CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_GameDB);
		return true;
	}
};

bool CRylDBAgentServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;

	ADD_COMMAND("flush",		        CCMDPrintLog,			szErrorMessage);
	ADD_COMMAND("reloadsetup",	        CCMDReloadSetup,		szErrorMessage);
	
    ADD_COMMAND("connect",              CCMDConnect,			szErrorMessage);
    ADD_COMMAND("loadchar",             CCMDLoadChar,			szErrorMessage);
    ADD_COMMAND("reloadadmin",          CCMDReloadAdmin,		szErrorMessage);

	ADD_COMMAND("dbconnect",			CCMDDBConnect,		szErrorMessage);

	// 공성 시간 관련 명령어.

	ADD_COMMAND("reloadgametime",		CCMDReloadGameTime,		szErrorMessage);
	ADD_COMMAND("gametime",				CCMDGameTime,			szErrorMessage);

	ADD_COMMAND("newlog",				CCMDNewLog,				szErrorMessage);

	ADD_COMMAND("aliveip",				CCMDAliveIP,		szErrorMessage);

	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

    return true;
}
