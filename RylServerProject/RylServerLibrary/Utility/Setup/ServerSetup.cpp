#include "stdafx.h"
#include "ServerSetup.h"

#include <Log/ServerLog.h>

#include <Utility/Math/Math.h>
#include <Utility/Registry/RegFunctions.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

const int MAX_INI_STRING = 260;
const char szINIFileName[] = "DemonSetup.ini";

static OleDB::ConnType GetConnTypeFromString(const TCHAR* szConnType)
{
    const int MAX_CONNTYPE_STRING = 64;
    TCHAR szBuffer[MAX_CONNTYPE_STRING];

    _sntprintf(szBuffer, MAX_CONNTYPE_STRING, "%s", szConnType);
    szBuffer[MAX_CONNTYPE_STRING - 1] = 0;

    _tcsupr(szBuffer);

    OleDB::ConnType eConnType = OleDB::ConnType_MSSQL;
    
    if (0 == _tcscmp(szBuffer, "MSSQL"))        {   eConnType = OleDB::ConnType_MSSQL;  }
    else if (0 == _tcscmp(szBuffer, "ODBC"))    {   eConnType = OleDB::ConnType_ODBC;   }
    else if (0 == _tcscmp(szBuffer, "ORACLE"))  {   eConnType = OleDB::ConnType_ORACLE; }

    return eConnType;
}

CServerSetup& CServerSetup::GetInstance()
{
	static CServerSetup serverSetup;
	return serverSetup;
}


CServerSetup::CServerSetup() 
:   m_dwLimitVer(0),
    m_dwClientVer(0), 
    m_dwBillingType(0),
    m_dwAuthType(0),
	m_eNationType(GameRYL::KOREA), 
	m_bHanCheck(false), 
    m_bHackCheck(false), 
    m_bAdminIPCheck(false), 
    m_bDuelModeCheck(false), 
    m_bLotteryEvent(false), 
	m_bLevelUpEvent(false), 
    m_bBattleAuth(false),
	m_bBattleGame(false),
	m_bBattleAgent(false),
    m_bSupressCharCreate(false),
    m_bSupressCharDelete(false),
    m_cRestrictedPart1ToPart2Level(0),
    m_cMaxTransferPart1ToPart2Count(0),
    m_dwUserLimit(0), 
    m_dwCheckSum(0), 
    m_dwExp(0), 
    m_dwDrop(0), 
	m_dwFame(0),
	m_dwRefine(0),
	m_dwMileage(0),
	m_dwEquipCorr(0),
    m_wBattleLimit(0),
	m_wBattleLimitPer(0),
	m_dwGameContentsFlag(0)
{
	memset(&m_ServerID, 0, sizeof(SERVER_ID));

    memset(&m_stPart1UnifiedDBInfo, 0, sizeof(DBInfo));
	memset(&m_stAdminToolDBInfo, 0, sizeof(DBInfo));
    memset(&m_stKeeperDBInfo, 0, sizeof(DBInfo));
    memset(&m_stAuthDBInfo, 0, sizeof(DBInfo));
    memset(&m_stGameDBInfo, 0, sizeof(DBInfo));
    memset(&m_stLogDBInfo, 0, sizeof(DBInfo));
}

CServerSetup::~CServerSetup()
{


}

unsigned long CServerSetup::GetServerID(void)
{
	return m_ServerID;
}

char CServerSetup::GetServerGroup(void)	
{
	SERVER_ID serverID;
	serverID.dwID = m_ServerID;

	return serverID.sID.Group;	
}

char CServerSetup::GetServerZone(void)
{
	SERVER_ID serverID;
	serverID.dwID = m_ServerID;

	return serverID.sID.ID;		
}

char CServerSetup::GetServerChannel(void)
{
	SERVER_ID serverID;
	serverID.dwID = m_ServerID;

	return serverID.sID.Channel;		
}


bool CServerSetup::Initialize(ServerType eServerType)
{
    bool bResult = false;

	switch (eServerType)
	{
		case LoginServer:	bResult = InitLoginServer();	break;
		case AuthServer:	bResult = InitAuthServer();	    break;
		case GameServer:	bResult = InitGameServer();	    break;
		case AgentServer:	bResult = InitAgentServer();	break;
		case UIDServer:		bResult = InitUIDServer();	    break;
        case LogServer:     bResult = InitLogServer();      break;
		case ChatServer:    bResult = InitChatServer();	    break;
	}

//	GameRYL::REBALANCE_OVERITEM |  
	switch (m_eNationType)
	{
		case GameRYL::KOREA:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		// ���翬 S�׷��̵� ����ǰ�
//		case GameRYL::GLOBAL:		m_dwGameContentsFlag = GameRYL::REBALANCE_OVERITEM | GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		// �Ʒ��� ����
		case GameRYL::GLOBAL:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::CHINA:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::THAILAND:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::JAPAN:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
	}

	return bResult;
}

bool CServerSetup::InitLoginServer(void)
{
	char szServerID[MAX_INI_STRING] = "";
    char szPingCheck[MAX_INI_STRING] = "";
    char szAloneMode[MAX_INI_STRING] = "";
    char szLimitVer[MAX_INI_STRING] = "";

    const char* szErrorString = "Unknown Error";

	if (!Registry::ReadString(szINIFileName, "LoginServer", "ServerID", szServerID, MAX_INI_STRING))
	{
        szErrorString = "LoginServer : ���� ���̵� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "LoginServer", "PingCheck", szPingCheck, MAX_INI_STRING))
	{
		szErrorString = "LoginServer : ��üũ ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "LoginServer", "AloneMode", szAloneMode, MAX_INI_STRING))
	{
		szErrorString = "LoginServer : ȥ�� ������� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "LoginServer", "LeastPatchVer", szLimitVer, MAX_INI_STRING))
	{
		szErrorString = "LoginServer : Ŭ���̾�Ʈ ���� ���� ��� ����";
	}
	else if (!Registry::ReadString(szINIFileName, "LoginServer", "PatchAddr", m_chPatchAddr, MAX_PATH))
	{
		szErrorString = "LoginServer : ��ġ �ּ� ��� ����.";
	}
    else
    {
        m_ServerID = atol(szServerID);
	    m_bPingCheck = (1 == atol(szPingCheck));
	    m_bAlone = (1 == atol(szAloneMode));
	    m_dwClientVer = atol(szLimitVer);
        return true;
    }

    ERRLOG0(g_Log, szErrorString);
    return false;	
}

bool CServerSetup::InitAuthServer(void)
{
	char szServerID[MAX_INI_STRING] = "";
	char szAuthType[MAX_INI_STRING] = "";
    char szDBAgentAddr[MAX_INI_STRING] = "";

	char szPingCheck[MAX_INI_STRING] = "";
	char szHanCheck[MAX_INI_STRING] = "";

    char szBattleServer[MAX_INI_STRING] = "";
    char szBattleAgeLimit[MAX_INI_STRING] = "";
    char szSupressCharCreate[MAX_INI_STRING] = "";
    char szSupressCharDelete[MAX_INI_STRING] = "";
	char szTestGroupAuth[MAX_INI_STRING] = "";

    char szAuthDBType[MAX_INI_STRING] = "";
    char szAdminToolDBType[MAX_INI_STRING] = "";


    char szGammaniaCode[MAX_INI_STRING] = "";
    char szGammaniaRegin[MAX_INI_STRING] = "";
    char szGammaniaAddr[MAX_INI_STRING] = "";
    char szGammaniaPort1[MAX_INI_STRING] = "";
    char szGammaniaPort2[MAX_INI_STRING] = ""; //test

    const char* szErrorString = "Unknown Error"; 

	if (!Registry::ReadString(szINIFileName, "AuthServer", "ServerID", szServerID, MAX_INI_STRING))
	{
        szErrorString = "AuthServer : ���� ���� ���̵� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "AuthServer", "AuthType", szAuthType, MAX_INI_STRING))
	{
        szErrorString = "AuthServer : ���� Ÿ�� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "DBAgentAddr", szDBAgentAddr, MAX_INI_STRING))
	{	
        szErrorString = "AuthServer : �߰� ���� �ּ� �б� ����";
	}
	else if (!Registry::ReadString(szINIFileName, "AuthServer", "PingCheck", szPingCheck, MAX_INI_STRING))
	{
        szErrorString = "AuthServer : ��üũ ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "AuthServer", "KoreanLangCheck", szHanCheck, MAX_INI_STRING))
	{
        szErrorString = "AuthServer : �ѱ�üũ ����.";
	}	    
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "BattleServerAuth", szBattleServer, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : ��Ʋ�������� ���� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "TestGroupAuth", szTestGroupAuth, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �׽�Ʈ�׷����� �����׷����� ���� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "GammaniaServerCode", szGammaniaCode, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �����Ͼ� ���� �ڵ� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "GammaniaRegin", szGammaniaRegin, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �����Ͼ� Regin �ڵ� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "GammaniaAddr", szGammaniaAddr, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �����Ͼ� Addr �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "GammaniaPort1", szGammaniaPort1, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �����Ͼ� Port1 �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "AuthServer", "GammaniaPort2", szGammaniaPort2, MAX_INI_STRING))
    {
        szErrorString = "AuthServer : �����Ͼ� Port2 �б� ����";
    }
    else if (
        !Registry::ReadString(szINIFileName, "AuthServer", "AuthDBType", szAuthDBType, MAX_INI_STRING) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AuthDBAddr", m_stAuthDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AuthDBName", m_stAuthDBInfo.m_szDBName, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AuthDBUser", m_stAuthDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AuthDBPass", m_stAuthDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
    {
        szErrorString = "AuthServer : ���� DB���� ���� ��� ����";
    }
    else if (
        !Registry::ReadString(szINIFileName, "AuthServer", "AdminToolDBType", szAdminToolDBType, MAX_INI_STRING) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AdminToolDBAddr", m_stAdminToolDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AdminToolDBName", m_stAdminToolDBInfo.m_szDBName, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AdminToolDBUser", m_stAdminToolDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "AuthServer", "AdminToolDBPass", m_stAdminToolDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
    {
        szErrorString = "AuthServer : ��� DB���� ���� ��� ����";
    }
    else
    {
        m_stAuthDBInfo.m_ConnType = GetConnTypeFromString(szAuthDBType);
        m_stAdminToolDBInfo.m_ConnType = GetConnTypeFromString(szAdminToolDBType);

        Registry::ReadString(szINIFileName, "AuthServer", "SupressCharCreate", szSupressCharCreate, MAX_INI_STRING);
        m_bSupressCharCreate = (1 == atoi(szSupressCharCreate));
    
        Registry::ReadString(szINIFileName, "AuthServer", "SupressCharDelete", szSupressCharDelete, MAX_INI_STRING);
        m_bSupressCharDelete = (1 == atoi(szSupressCharDelete));

        m_ServerID = atoi(szServerID);
        m_bPingCheck = (1 == atoi(szPingCheck));
	    m_bHanCheck = (1 == atoi(szHanCheck));
	    m_dwAuthType = atoi(szAuthType);
        m_ServerAddress[AgentServer].set_addr(szDBAgentAddr, DBAgentServerAuthServerListen);
        m_bBattleAuth = (1 == atoi(szBattleServer));        
        m_bTestGroupAuth = (1 == atoi(szTestGroupAuth));

		////////////////////////////////////////////////////////////////////////
		// edith 2008.01.21 �����Ͼ� ���� �������� ���� ����..
		strcpy(m_szGammaniaCode, szGammaniaCode);
		strcpy(m_szGammaniaRegin, szGammaniaRegin);
		strcpy(m_szGammaniaAddr, szGammaniaAddr);

		m_GammaniaPort1 = atoi(szGammaniaPort1);
		m_GammaniaPort2 = atoi(szGammaniaPort2);

        return true;
    }
    
    ERRLOG0(g_Log, szErrorString);
    return false;
}

bool CServerSetup::InitAgentServer(void)
{
	char szServerID[MAX_INI_STRING] = "";
    char szBillingType[MAX_INI_STRING] = "";
    char szPingCheck[MAX_INI_STRING] = "";
	char szAdminIPCheck[MAX_INI_STRING] = "";
    char szServerVersion[MAX_INI_STRING] = "";

	char szCheckSum[MAX_INI_STRING] = "";
    char szLoginServerAddr[MAX_INI_STRING] = "";
    char szUIDServerAddr[MAX_INI_STRING];
	char szBattleServer[MAX_INI_STRING] = "";
	char szAgentServerType[MAX_INI_STRING];
	char szNationType[MAX_INI_STRING] = "";

	char szUserLimit[MAX_INI_STRING] = "";
	char szBattleLimit[MAX_INI_STRING] = "";
	char szBattleLimitPer[MAX_INI_STRING] = "";

    char szGameDBConnType[MAX_INI_STRING] = "";

    const char* szErrorString = "Unknown Error";

	if (!Registry::ReadString(szINIFileName, "DBAgentServer", "ServerID", szServerID, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ���� ���̵� ��� ����";
	}
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "ServerType", szAgentServerType, MAX_INI_STRING))
	{	
        szErrorString = "DBAgentServer : ���� Ÿ�� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "BillingType", szBillingType, MAX_INI_STRING))
    {
        szErrorString = "DBAgentServer : ���� ���� Ÿ�� ��� ����";
    }
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "NationType", szNationType, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ���� ���� ��� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "ClientVersion", szServerVersion, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : Ŭ���̾�Ʈ ���� ���� ����";
    }
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "PatchAddress", m_chPatchAddr, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ��ġ �ּ� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "LoginServerAddr", szLoginServerAddr, MAX_INI_STRING))
	{	        
        szErrorString = "DBAgentServer : �α��� ���� �ּ� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "UIDServerAddr", szUIDServerAddr, MAX_INI_STRING))
	{	
        szErrorString = "DBAgentServer : UID ���� �ּ� �б� ����";		
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "CheckSum", szCheckSum, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : üũ�� ��� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "PingCheck", szPingCheck, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ��üũ ����";		
	}
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "AdminIPCheck", szAdminIPCheck, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ���� IPüũ ����.";
	}
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "BattleServer", szBattleServer, MAX_INI_STRING))
    {
        szErrorString = "DBAgentServer : �߰� ������ ��Ʋ�������� ���� �б� ����";        
    }
    else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "LimitServerGroupUserNum", szUserLimit, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ���� ���� ����";
	}
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "LimitBattleUser", szBattleLimit, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ��Ʋ�׶��� ���� �ο� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "DBAgentServer", "LimitBattleUserPer", szBattleLimitPer, MAX_INI_STRING))
	{
        szErrorString = "DBAgentServer : ��Ʋ�׶��� ���� �ο� �ۼ�Ʈ ��� ����.";
	}
    else if (
        !Registry::ReadString(szINIFileName, "DBAgentServer", "GameDBType", szGameDBConnType, MAX_INI_STRING) ||
        !Registry::ReadString(szINIFileName, "DBAgentServer", "GameDBAddr", m_stGameDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "DBAgentServer", "GameDBName", m_stGameDBInfo.m_szDBName, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "DBAgentServer", "GameDBUser", m_stGameDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
        !Registry::ReadString(szINIFileName, "DBAgentServer", "GameDBPass", m_stGameDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
    {
        szErrorString = "DBAgentServer : ���� DB���� �б� ����";
    }
    else 
    {
        m_stGameDBInfo.m_ConnType = GetConnTypeFromString(szGameDBConnType);

        m_ServerID = atol(szServerID);
        m_dwBillingType = atoi(szBillingType);
	    m_bPingCheck = (1 == atoi(szPingCheck));
	    m_bAdminIPCheck = (1 == atoi(szAdminIPCheck));
	    m_dwClientVer = atol(szServerVersion);

	    m_dwUserLimit = atol(szUserLimit);
        m_wBattleLimit = static_cast<unsigned short>(atol(szBattleLimit));
		m_wBattleLimitPer = static_cast<unsigned short>(atol(szBattleLimitPer));
	    m_dwCheckSum = Math::Convert::Atoi(szCheckSum);
        m_ServerAddress[LoginServer].set_addr(szLoginServerAddr, LoginServerDBAgentServerListen);

        m_ServerAddress[UIDServer].set_addr(szUIDServerAddr, UIDServerDBAgentServerListen);
        m_bBattleAgent = (1 == atoi(szBattleServer));
        m_eAgentServerType = static_cast<UnifiedConst::AgentServerType>(atoi(szAgentServerType));
	    m_eNationType = static_cast<GameRYL::ServiceNation>(atol(szNationType));

        // ��Ʈ2 ���� �������϶��� �ʿ��� ����
	    if (UnifiedConst::Part2Unified == m_eAgentServerType)
	    {
		    char szPart1UnifiedAgentAddr[MAX_INI_STRING];
            char szRestrictedPart1ToPart2Level[MAX_INI_STRING];
            char szMaxTransferCount[MAX_INI_STRING];

		    if (!Registry::ReadString(szINIFileName, "DBAgentServer", 
                "Part1UnifiedAgentAddr", szPart1UnifiedAgentAddr, MAX_INI_STRING))
		    {	                
			    ERRLOG0(g_Log, "��Ʈ1 ���� �߰� ���� �ּ� �б� ����");
			    return false;
		    }            
            else if (!Registry::ReadString(szINIFileName, "DBAgentServer",
                "RestrictedPart1ToPart2Level", szRestrictedPart1ToPart2Level, MAX_INI_STRING))
            {
                ERRLOG0(g_Log, "��Ʈ1���� ��Ʈ2�� ���������� �ּҷ��� �б� ����");
                return false;
            }
            else if (!Registry::ReadString(szINIFileName, "DBAgentServer", 
                "MaxTransferCharNum", szMaxTransferCount, MAX_INI_STRING))
            {
                ERRLOG0(g_Log, "��Ʈ1���� ��Ʈ2�� ���������� �ִ� ĳ���� �� �б� ����");
                return false;
            }
            else
            {
                m_Part1UnifiedAgentAddr.set_addr(szPart1UnifiedAgentAddr, DBAgentAdminToolServerListen);
                m_cRestrictedPart1ToPart2Level = static_cast<unsigned char>(atoi(szRestrictedPart1ToPart2Level));
                m_cMaxTransferPart1ToPart2Count = static_cast<unsigned char>(atoi(szMaxTransferCount));        
            }
	    }
        else if (UnifiedConst::Part2Selectable == m_eAgentServerType)
        {
            char szSelectableUnifiedServerNum[MAX_INI_STRING];

            if (!Registry::ReadString(szINIFileName, "DBAgentServer", 
                "Part2SelectableUnifiedServerNum", szSelectableUnifiedServerNum, MAX_INI_STRING))
            {	
                ERRLOG0(g_Log, "������ ���� ��� �б� ����");
                return false;
            }

            m_dwSelectableUnifiedServerNum = atol(szSelectableUnifiedServerNum);
        }

    	return true;
    }

    ERRLOG0(g_Log, szErrorString);
    return false;
}

bool CServerSetup::InitUIDServer(void)
{
	char szFreeCheck[MAX_INI_STRING] = "";
    char szIgnoreFlag[MAX_INI_STRING] = "";
    char szBillingType[MAX_INI_STRING] = "";
    char szHanUnitedBillingAddr[MAX_INI_STRING] = "";
    char szBillingDBConnType[MAX_INI_STRING] = "";

    char szGammaniaCode[MAX_INI_STRING] = "";
    char szGammaniaRegin[MAX_INI_STRING] = "";
    char szGammaniaAddr[MAX_INI_STRING] = "";
    char szGammaniaPort1[MAX_INI_STRING] = "";
    char szGammaniaPort2[MAX_INI_STRING] = "";


    const char* szErrorString = "Unknown Error";

	if (!Registry::ReadString(szINIFileName, "UIDServer", "FreeCheck", szFreeCheck, MAX_INI_STRING))
	{
        szErrorString = "UIDServer : ����üũ ����.";
	}
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "IgnoreFlag", szIgnoreFlag, MAX_INI_STRING))
	{
        szErrorString = "UIDServer : �÷��� ���� üũ ����.";
	}
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "BillingType", szBillingType, MAX_INI_STRING))
	{
        szErrorString = "UIDServer : ���� Ÿ�� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "GammaniaServerCode", szGammaniaCode, MAX_INI_STRING))
    {
        szErrorString = "UIDServer : �����Ͼ� ���� �ڵ� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "GammaniaRegin", szGammaniaRegin, MAX_INI_STRING))
    {
        szErrorString = "UIDServer : �����Ͼ� Regin �ڵ� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "GammaniaAddr", szGammaniaAddr, MAX_INI_STRING))
    {
        szErrorString = "UIDServer : �����Ͼ� Addr �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "GammaniaPort1", szGammaniaPort1, MAX_INI_STRING))
    {
        szErrorString = "UIDServer : �����Ͼ� Port1 �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, "UIDServer", "GammaniaPort2", szGammaniaPort2, MAX_INI_STRING))
    {
        szErrorString = "UIDServer : �����Ͼ� Port2 �б� ����";
    }
    else
    {
		////////////////////////////////////////////////////////////////////////
		// edith 2008.01.21 �����Ͼ� ���� �������� ���� ����..
		strcpy(m_szGammaniaCode, szGammaniaCode);
		strcpy(m_szGammaniaRegin, szGammaniaRegin);
		strcpy(m_szGammaniaAddr, szGammaniaAddr);

		m_GammaniaPort1 = atoi(szGammaniaPort1);
		m_GammaniaPort2 = atoi(szGammaniaPort2);
		////////////////////////////////////////////////////////////////////////

        m_bFreeCheck = (1 == atoi(szFreeCheck));
	    m_bIgnoreFlag = (1 == atoi(szIgnoreFlag));
        m_dwBillingType = atoi(szBillingType);

        if (m_dwBillingType == GamaUnitedBilling && 
            !Registry::ReadString(szINIFileName, "UIDServer", "HanUnitedBillingAddr", 
            szHanUnitedBillingAddr, MAX_INI_STRING))
        {
            szErrorString = "UIDServer : �Ѱ��� ���պ��� �ּ� �б� ����";
        }    
        else if (            
            !Registry::ReadString(szINIFileName, "UIDServer", "BillingDBType", szBillingDBConnType, MAX_INI_STRING) ||
            !Registry::ReadString(szINIFileName, "UIDServer", "BillingDBAddr", m_stKeeperDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
            !Registry::ReadString(szINIFileName, "UIDServer", "BillingDBName", m_stKeeperDBInfo.m_szDBName, DBInfo::MAX_BUFFER) ||
            !Registry::ReadString(szINIFileName, "UIDServer", "BillingDBUser", m_stKeeperDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
            !Registry::ReadString(szINIFileName, "UIDServer", "BillingDBPass", m_stKeeperDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
        {
            szErrorString = "UIDServer : ���� DB ���� ���� �б� ����";
        }
        else
        {
            m_stKeeperDBInfo.m_ConnType = 
                GetConnTypeFromString(szBillingDBConnType);

            unsigned short usPort = static_cast<unsigned short>(
                Registry::ReadInt(szINIFileName, "UIDServer", "HanUnitedBillingPort"));

            m_HanUnitedBillingAddr.set_addr(szHanUnitedBillingAddr, usPort);
            return true;
        }
    }

    ERRLOG0(g_Log, szErrorString);
	return false;
}

bool CServerSetup::InitChatServer(void)
{
    char szChatServerID[MAX_INI_STRING] = "";
    char szDBAgentServerAddr[MAX_INI_STRING] = "";
    char szChatPingCheck[MAX_INI_STRING] = "";
	char szChatToolIPCheck[MAX_INI_STRING] = "";
    char szAdminToolDBType[MAX_INI_STRING] = "";

    const char* szErrorString = "Unknown Error";

    if (!Registry::ReadString(szINIFileName, "ChatServer", "ServerID", szChatServerID, MAX_INI_STRING))
    {
        szErrorString = "ChatServer : ���� ID �б� ����";
    }    
	else if (!Registry::ReadString(szINIFileName, "ChatServer", "PingCheck", szChatPingCheck, MAX_INI_STRING))
	{
        szErrorString = "ChatServer : �� üũ �б� ����";
	}  
	else if (!Registry::ReadString(szINIFileName, "ChatServer", "UserIPCheck", szChatToolIPCheck, MAX_INI_STRING))
	{
		szErrorString = "ChatServer : ����͸� �� IP üũ �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "ChatServer", "DBAgentAddress", szDBAgentServerAddr, MAX_INI_STRING))
    {
        szErrorString = "ChatServer : ������ �߰� ���� �ּ� �б� ����";
    }
    else if (
        !Registry::ReadString(szINIFileName, "ChatServer", "AdminToolDBType", szAdminToolDBType, MAX_INI_STRING) ||
        !Registry::ReadString(szINIFileName, "ChatServer", "AdminToolDBAddr", m_stAdminToolDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
	    !Registry::ReadString(szINIFileName, "ChatServer", "AdminToolDBName", m_stAdminToolDBInfo.m_szDBName, DBInfo::MAX_BUFFER) || 
	    !Registry::ReadString(szINIFileName, "ChatServer", "AdminToolDBUser", m_stAdminToolDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
	    !Registry::ReadString(szINIFileName, "ChatServer", "AdminToolDBPass", m_stAdminToolDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
    {
        szErrorString = "ChatServer : � DB ���� ���� �б� ����(For chat ban)";
	}
    else
    {
        m_stAdminToolDBInfo.m_ConnType = 
            GetConnTypeFromString(szAdminToolDBType);

        m_ServerID = atoi(szChatServerID);
        m_bPingCheck = (1 == atoi(szChatPingCheck)) ? true : false;
		m_bChatToolIPCheck = (1 == atol(szChatToolIPCheck)) ? true : false;
        m_ServerAddress[AgentServer].set_addr(szDBAgentServerAddr, DBAgentServerChatServerListen);

        return true;
    }

    ERRLOG0(g_Log, szErrorString);
    return false;    
}


bool CServerSetup::InitLogServer()
{
    char szLogDBType[MAX_INI_STRING] = "";

    if (!Registry::ReadString(szINIFileName, "LogServer", "LogDBType", szLogDBType, MAX_INI_STRING) ||
        !Registry::ReadString(szINIFileName, "LogServer", "LogDBAddr", m_stLogDBInfo.m_szDBAddr, DBInfo::MAX_BUFFER) ||
	    !Registry::ReadString(szINIFileName, "LogServer", "LogDBName", m_stLogDBInfo.m_szDBName, DBInfo::MAX_BUFFER) || 
	    !Registry::ReadString(szINIFileName, "LogServer", "LogDBUser", m_stLogDBInfo.m_szDBUser, DBInfo::MAX_BUFFER) ||
	    !Registry::ReadString(szINIFileName, "LogServer", "LogDBPass", m_stLogDBInfo.m_szDBPass, DBInfo::MAX_BUFFER))
    {
        ERRLOG0(g_Log, "LogServer : �α� DB ���� ���� �б� ����");
	}
    else
    {
        m_stLogDBInfo.m_ConnType = GetConnTypeFromString(szLogDBType);
        return true;
    }

    return true;
}

const char* CServerSetup::GetServerWindowName(ServerType eServerType)
{
	const char* lpWindowName = 0;

	switch(eServerType)
	{
		case LoginServer:		lpWindowName = "Login Server";		break;
		case AuthServer:		lpWindowName = "Auth Server";		break;
		case GameServer:		lpWindowName = "Game Server";		break;
		case AgentServer:		lpWindowName = "Agent Server";		break;
		case UIDServer:			lpWindowName = "UID Server";		break;
		case AdminToolServer:	lpWindowName = "AdminToolServer";	break;
		case LogServer:			lpWindowName = "Log Server";		break;
		case ChatServer:		lpWindowName = "Chat Server";		break;
	}

	return lpWindowName;
}

const char* CServerSetup::GetManageClientWindowName()
{
    return "ManageClient";
}

bool CServerSetup::InitGameServer(void)
{
	//	���⼭ Ŀ�ǵ� ������ �Ľ��ؼ��� ���ǰ��� ����� �־��ָ� ��;
	//	���� ��� "-z1 -c1"�� Ŀ�ǵ� �����϶��� ���� [Zone_0101]

    char szSection[MAX_INI_STRING];
    char szServerID[MAX_INI_STRING] = "";
    char szPingCheck[MAX_INI_STRING] = "";
    char szSpeedHack[MAX_INI_STRING] = "";
    char szLotteryEvent[MAX_INI_STRING] = "";
    char szLevelUpEvent[MAX_INI_STRING] = "";
    char szDeathPenaltyEvent[MAX_INI_STRING] = "";
    char szExp[MAX_INI_STRING] = "";
    char szDrop[MAX_INI_STRING] = "";
    char szFame[MAX_INI_STRING] = "";
    char szRefine[MAX_INI_STRING] = "";
    char szMileage[MAX_INI_STRING] = "";
    char szEquipCorr[MAX_INI_STRING] = "";

    char szBattleLimit[MAX_INI_STRING] = "";
	char szBattleLimitPer[MAX_INI_STRING] = "";


    char szDBAgentServerAddr[MAX_INI_STRING] = "";
    char szLogServerAddr[MAX_INI_STRING] = "";
    char szChatServerAddr[MAX_INI_STRING] = "";
	char szNationType[MAX_INI_STRING] = "";
    char szBattleServer[MAX_INI_STRING] = "";

    const char* szErrorString = "(z:%02d, c:%02d) Unknown Error";

	int nZoneIndex = GetZoneFromCmdLine();
	int nChannelIndex = GetChannelFromCmdLine();

	int nLength = _snprintf( szSection, MAX_INI_STRING - 1, "Zone_%02d%02d", nZoneIndex, nChannelIndex);
	if (nLength <= 0)
	{		
		szErrorString = "(z:%02d, c:%02d) Ŀ�ǵ� ���� �Ľ� ����";
	}    
    else if (!Registry::ReadString(szINIFileName, szSection, "ServerID", szServerID, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ���� ���� ���̵� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "PingCheck", szPingCheck, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��üũ �б� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "SpeedHackCheck", szSpeedHack, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��üũ �б� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "LotteryEvent", szLotteryEvent, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ���� �̺�Ʈ üũ �б� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "LevelUpEvent", szLevelUpEvent, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ������ �̺�Ʈ üũ ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "DeathPenaltyEvent", szDeathPenaltyEvent, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ����� �г�Ƽ �̺�Ʈ üũ ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "ExpRate", szExp, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ����ġ ���� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "DropRate", szDrop, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ����� ���� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "Fame", szFame, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��ġ ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "Refine", szRefine, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ���ü����� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "Mileage", szMileage, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ���ϸ��� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "EquipCorrRate", szEquipCorr, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��� ���� ���� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "DBAgentServerAddr", szDBAgentServerAddr, MAX_INI_STRING))
	{	
		szErrorString = "(z:%02d, c:%02d) �߰� ���� �ּ� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, szSection, "LogServerAddr", szLogServerAddr, MAX_INI_STRING))
    {	
        szErrorString = "(z:%02d, c:%02d) �α� ���� �ּ� �б� ����";
    }
    else if (!Registry::ReadString(szINIFileName, szSection, "ChatServerAddr", szChatServerAddr, MAX_INI_STRING))
    {	
        szErrorString = "(z:%02d, c:%02d) ä�� ���� �ּ� �б� ����";
    }
	else if (!Registry::ReadString(szINIFileName, szSection, "BattleUserLimit", szBattleLimit, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��Ʋ�׶��� ���� �ο� ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, szSection, "BattleUserLimitPer", szBattleLimitPer, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ��Ʋ�׶��� ���� �ο� �ۼ�Ʈ ��� ����.";
	}
	else if (!Registry::ReadString(szINIFileName, "Zone_ETC", "GameServerNation", szNationType, MAX_INI_STRING))
	{
		szErrorString = "(z:%02d, c:%02d) ���� ���� ���� �б� ����";
	}
    else if (!Registry::ReadString(szINIFileName, "Zone_ETC", "BattleServerGroup", szBattleServer, MAX_INI_STRING))
    {	
        szErrorString = "(z:%02d, c:%02d) ��Ʋ�������� ���� �б� ����";
    }
    else
    {
        m_ServerID = atol(szServerID);
	    m_bPingCheck = (1 == atol(szPingCheck));
	    m_bHackCheck = (1 == atol(szSpeedHack));
        m_bLotteryEvent = (1 == atol(szLotteryEvent));
        m_bLevelUpEvent = (1 == atol(szLevelUpEvent));
	    m_bDeathPenaltyEvent = (1 == atol(szDeathPenaltyEvent));
        m_bBattleGame = (1 == atol(szBattleServer));

	    m_dwExp = atol(szExp);
	    m_dwDrop = atol(szDrop);
        m_dwFame = atol(szFame);
		m_dwRefine = atol(szRefine);
        m_dwMileage = atol(szMileage);
		m_dwEquipCorr = atoi(szEquipCorr);
        m_wBattleLimit = static_cast<unsigned short>(atol(szBattleLimit));
  		m_wBattleLimitPer = static_cast<unsigned short>(atol(szBattleLimitPer));

	    m_eNationType = static_cast<GameRYL::ServiceNation>(atoi(szNationType));

        m_ServerAddress[AgentServer].set_addr(szDBAgentServerAddr, DBAgentServerGameServerListen);
        m_ServerAddress[LogServer].set_addr(szLogServerAddr, LogServerOtherServerListen);
        m_ServerAddress[ChatServer].set_addr(szChatServerAddr, ChatServerGameServerListen);


        CTCPFactory tcpFactory;

        char szMyAddress[MAX_INI_STRING];
        tcpFactory.GetNetworkInfo(szMyAddress, MAX_INI_STRING);

        m_P1GameServerUDPAddr.set_addr(szMyAddress, GetP1GameServerUDPPort(m_ServerID));
        return true;
    }

    ERRLOG2(g_Log, szErrorString, nZoneIndex, nChannelIndex);
    return false;
}

unsigned short CServerSetup::GetGameServerTCPPort(unsigned long dwServerID)
{
    SERVER_ID serverID;
    serverID.dwID = dwServerID;

    return serverID.GetZone() + serverID.GetChannel() * MAX_ZONE + GameServerClientBaseListen;
}

unsigned short CServerSetup::GetP1GameServerUDPPort(unsigned long dwServerID)
{
    SERVER_ID serverID;
    serverID.dwID = dwServerID;

    return serverID.GetZone() + serverID.GetChannel() * MAX_ZONE + P1GameServerClientUDPListen;
}

char CServerSetup::GetZoneFromCmdLine()
{
	char szCommandLineBuffer[MAX_PATH];	
	const char* szDelimit = " \r\n\t";

    char cZone = 1;

	int nLength = _snprintf(szCommandLineBuffer, MAX_PATH - 1, "%s", GetCommandLine());
	if(nLength < 0)
	{
		// TODO : write error code
		ERRLOG0(g_Log, "CommandLine Parse failed - Zone");
	}
    else
    {
        char* szToken = strtok(szCommandLineBuffer, szDelimit);
	    while(0 != szToken)
	    {
		    if(0 == strcmp(szToken, "-z"))
		    {
			    // �� ��ȣ �Ľ�
			    szToken = strtok(0, szDelimit);
                if(0 != szToken)
			    {
				    cZone = static_cast<char>(atoi(szToken));
                    break;
			    }
		    }
    		
		    szToken = strtok(0, szDelimit);
	    }
    }

    return cZone;
}

char CServerSetup::GetChannelFromCmdLine()
{
	char szCommandLineBuffer[MAX_PATH];	
	const char* szDelimit = " \r\n\t";

    char cChannel = 0;

	int nLength = _snprintf(szCommandLineBuffer, MAX_PATH - 1, "%s", GetCommandLine());
	if(nLength < 0)
	{
		// TODO : write error code
		ERRLOG0(g_Log, "CommandLine Parse failed - Channel");
	}
    else
    {
        char* szToken = strtok(szCommandLineBuffer, szDelimit);
	    while(0 != szToken)
	    {
		    if(0 == strcmp(szToken, "-c"))
		    {
			    // Channel ��ȣ �Ľ�
			    szToken = strtok(0, szDelimit);
                if(0 != szToken)
			    {
				    cChannel = static_cast<char>(atoi(szToken));
                    break;
			    }
		    }
    		
		    szToken = strtok(0, szDelimit);
	    }
    }

    return cChannel;
}

