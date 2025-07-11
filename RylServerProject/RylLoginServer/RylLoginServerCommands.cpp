#include "stdafx.h"
#include "RylLoginServer.h"
#include "LoginAgentServerTable.h"

#include <Log/ServerLog.h>
#include <Network/Session/LimitUserByIP.h>
#include <Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

class CCMDPrintLog : public CConsoleCMDSingleton<CCMDPrintLog>
{
protected:
    virtual bool DoProcess()
    {
        CRylLoginServer::GetInstance().PrintStatistics();
        CRylLoginServer::GetInstance().PrintServerInfo();

        SERLOG0(g_Log, "Flush ServerLog.");
        SERLOG0(g_SessionLog, "Flush ServerLog.");
        return true;
    }
};

class CCMDReloadSetup : public CConsoleCMDSingleton<CCMDReloadSetup>
{
protected:
    virtual bool DoProcess()
    {
        CRylLoginServer::GetInstance().ReloadSetup();
        return true;
    }
};

class CCMDReloadScript : public CConsoleCMDSingleton<CCMDReloadScript>
{
protected:
    virtual bool DoProcess()
    {
        return CLoginAgentServerTable::GetInstance().LoadAllowedIPList(
            CLoginAgentServerTable::GetAllowedIPListFileName());		
    }
};

class CCmdServerClose : public CConsoleCMDSingleton<CCmdServerClose>
{
protected:
    virtual bool DoProcess()
    {
        CLoginAgentServerTable::GetInstance().SetOperateMode(
            CLoginAgentServerTable::SERVER_CLOSED);
        return true;
    }
};

class CCmdServerReadyPatch : public CConsoleCMDSingleton<CCmdServerReadyPatch>
{
protected:
    virtual bool DoProcess()
    {
        CLoginAgentServerTable::GetInstance().SetOperateMode(
            CLoginAgentServerTable::SERVER_PATCH_ONLY);
        return true;
    }
};

class CCmdServerOpen : public CConsoleCMDSingleton<CCmdServerOpen>
{
protected:
    virtual bool DoProcess()
    {
        CLoginAgentServerTable::GetInstance().SetOperateMode(
            CLoginAgentServerTable::SERVER_OPEN);
        return true;
    }
};

class CCmdIPOpenMode : public CConsoleCommand
{
protected:

    virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength)
    {
        CCmdIPOpenMode* lpOpenAll = new CCmdIPOpenMode;

        if(0 != lpOpenAll)
        {
            const int MAX_BUFFER = 512;
            char szCommandData[MAX_BUFFER];

            strncpy(szCommandData, szCommand, MAX_PATH - 1);
            szCommandData[MAX_PATH - 1] = 0;

            char* szDelimit = "\r\n\t ";
            char* szToken = strtok(szCommandData, szDelimit);

            lpOpenAll->m_eIPMode = CLoginAgentServerTable::MAX_IP_MODE;

            if(0 != szToken)
            {
                _strlwr(szToken);

                if(0 == strcmp(szToken, "ip_open_allowed"))
                {
                    lpOpenAll->m_eIPMode = CLoginAgentServerTable::ALLOW_SOME;
                }
                else if(0 == strcmp(szToken, "ip_open_all"))
                {
                    lpOpenAll->m_eIPMode = CLoginAgentServerTable::ALLOW_ALL;
                }
                else if(0 == strcmp(szToken, "ip_admin_only"))
                {
                    lpOpenAll->m_eIPMode = CLoginAgentServerTable::DENY_ALL_BUT_ALLOW_ADMIN;
                }
            }

            lpOpenAll->m_nServerGroup = -2;

            szToken = strtok(0, szDelimit);
            if(0 != szToken)
            {
                _strlwr(szToken);

                if(0 == strcmp(szToken, "all"))
                {
                    lpOpenAll->m_nServerGroup = -1;
                }
                else
                {
                    lpOpenAll->m_nServerGroup = atoi(szToken);

                    if(0 == lpOpenAll->m_nServerGroup && 0 != strcmp(szToken, "0"))
                    {
                        lpOpenAll->Destroy();
                        lpOpenAll = 0;
                    }
                }
            }
        }

        return lpOpenAll;
    }

    virtual bool Destroy() { delete this; return true; }

    virtual bool DoProcess()
    {
        if(-1 == m_nServerGroup)
        {
            CLoginAgentServerTable::GetInstance().SetIPMode(m_eIPMode);
        }
        else if(0 <= m_nServerGroup && m_nServerGroup < SERVER_ID::MAX_GROUP_NUM)
        {
            CLoginAgentServerTable::GetInstance().SetIPMode(m_nServerGroup, m_eIPMode);
        }

        return true;
    }

    int                             m_nServerGroup;
    CLoginAgentServerTable::IPMode  m_eIPMode;
};

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

		SERLOG0(g_Log, "New Log.");
		SERLOG0(g_SessionLog, "New Log.");

		return true;
	}
};


bool CRylLoginServer::InitializeCommand()
{
#define INIT_COMMAND_FAILED(detail) TEXT("Command create failed - "##detail)

#define ADD_COMMAND(cmdstring, cmdobject, errmsg_val) \
	if(0 == (errmsg_val) && !GetCommandFactory()->AddCommand(cmdstring, new cmdobject)) { \
		(errmsg_val) = INIT_COMMAND_FAILED(cmdstring); }
	
	const TCHAR* szErrorMessage = 0;

    ADD_COMMAND("server_close",         CCmdServerClose,    szErrorMessage);
    ADD_COMMAND("server_ready_patch",   CCmdServerReadyPatch, szErrorMessage);
    ADD_COMMAND("server_open",          CCmdServerOpen,     szErrorMessage);

    ADD_COMMAND("ip_open_allowed",          CCmdIPOpenMode, szErrorMessage);
    ADD_COMMAND("ip_open_all",              CCmdIPOpenMode, szErrorMessage);
    ADD_COMMAND("ip_admin_only",  CCmdIPOpenMode, szErrorMessage);

	ADD_COMMAND("flush",		        CCMDPrintLog,		szErrorMessage);
	ADD_COMMAND("reloadsetup",	        CCMDReloadSetup,	szErrorMessage);
	ADD_COMMAND("reloadscript",	        CCMDReloadScript,	szErrorMessage);

	ADD_COMMAND("newlog",				CCMDNewLog,			szErrorMessage);
    
	if(0 != szErrorMessage)
	{
		ERRLOG0(g_Log, szErrorMessage);
		return false;
	};

    return true;
}
