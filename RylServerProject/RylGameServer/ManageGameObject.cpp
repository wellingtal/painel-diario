
#include "stdafx.h"
#include "RylGameServer.h"

#include <Network/Dispatch/CheckPing.h>
#include <Network/Dispatch/GameClient/CheckSpeedHack.h>
#include <Network/Dispatch/GameClient/FieldServerClientDispatch.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStatistics.h>

#include <Utility/Filter/Filter.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/Debug/DebugUtils.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Creature/EnemyCheck.h>
#include <Creature/CreatureManager.h>
#include <Creature/Monster/MonsterShout.h>
#include <Creature/Character/CharRespawnMgr.h>

#include <Skill/SkillMgr.h>
#include <Skill/SkillTable.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/MineralVeinMgr.h>

#include <Log/GameLog.h>
#include <GameEvent/GameEventMgr.h>
#include <Item/ItemMgr.h>
#include <GameGuardLib/ggsrv.h>

// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
void Log(const char *szMsg, ... ) 
{ 
	va_list marker; 
	char szTmp[256] = {0,}; 

	va_start( marker ,szMsg );   
	vsprintf(szTmp,szMsg, marker); 
	va_end( marker );              

	// DebugView �� ���ؼ� �α׸� Ȯ�� �� �� �ֽ��ϴ�.
//	OutputDebugString( szTmp );
	DETLOG0(g_Log, szTmp);
} 

GGAUTHS_API void NpLog(int mode, char* msg)
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR))
	{
		Log(msg);
	}
}

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	Log( "GGAuth version update [%s] : [%ld] -> [%ld] \n", 
		report->nType==1?"GameGuard Ver":"Protocol Num", 
		report->dwBefore, report->dwNext );
}


void PrePerformanceMsg(FILE* lpFile)
{
    if(0 != lpFile)
    {
        SYSTEMTIME currentTime;
        GetLocalTime(&currentTime);

        fprintf(lpFile, "\n------------------------------------------------------------------------\n"
            "\tPID:0x%08x/ServerID:0x%08x/ServerZone:%02d/ServerChannel:%02d/\n"
            "\tGameServer PerformanceCheck. %04d-%02d-%02d %02d:%02d:%02d:%04d\n\n",
            GetCurrentProcessId(), CServerSetup::GetInstance().GetServerID(), 
            CServerSetup::GetZoneFromCmdLine(),            
            CServerSetup::GetChannelFromCmdLine(),
            
            currentTime.wYear,
            currentTime.wMonth,
            currentTime.wDay,
            currentTime.wHour,
            currentTime.wMinute,
            currentTime.wSecond,
            currentTime.wMilliseconds);
    }
}

bool CRylGameServer::InitializeGameObject(void)
{   
    // �α� �̸� �ʱ�ȭ.
    char szProgramName[MAX_PATH];
    char szLogFilePrefixName[MAX_PATH];
	char szSessionLogFilePrefixName[MAX_PATH];
    char szSkillLogFilePrefixName[MAX_PATH];

    DbgUtils::SetProgramName(szProgramName, MAX_PATH);
    szProgramName[MAX_PATH - 1] = 0;

    _snprintf(szLogFilePrefixName, MAX_PATH - 1, "%sZ%02dC%02d-", szProgramName, 
        CServerSetup::GetInstance().GetServerZone(),
        CServerSetup::GetInstance().GetServerChannel());        

    _snprintf(szSessionLogFilePrefixName, MAX_PATH - 1, "SessionLogZ%02dC%02d-",
        CServerSetup::GetInstance().GetServerZone(),
        CServerSetup::GetInstance().GetServerChannel());        

	_snprintf(szSkillLogFilePrefixName, MAX_PATH - 1, "SkillLogZ%02dC%02d-",
		CServerSetup::GetInstance().GetServerZone(),
		CServerSetup::GetInstance().GetServerChannel());        

    g_Log.SetLogFileName(szLogFilePrefixName, szProgramName);
    g_SessionLog.SetLogFileName(szSessionLogFilePrefixName, szProgramName);
	g_SkillLog.SetLogFileName(szSkillLogFilePrefixName, szProgramName);

    // ��Ŷ ��� �α� ��� �ʱ�ȭ
    CPacketStatistics::GetInstance().SetUserMessageFunc(PrePerformanceMsg, 0);

    // �����ս� �α� ��� �ʱ�ȭ
    CPerformanceCheck::GetInstance().SetUserMessageFunc(PrePerformanceMsg, 0);

    // ���� �α� �ʱ�ȭ
    if (false == CGameLog::GetInstance().Initialize(szLogFilePrefixName))
    {
        ERRLOG0(g_Log, "���� �α׸� �ʱ�ȭ�ϴµ� �����߽��ϴ�.");
        return false;
    }

    // ������ ��ũ��Ʈ �ε�
	char szItemScriptName[MAX_PATH];
	if (SERVER_ID::GROUP_BATTLE_SERVER == CServerSetup::GetInstance().GetServerGroup())
	{
		strncpy(szItemScriptName, "./Script/Game/BGItemScript.txt", MAX_PATH);
	}
	else
	{
		strncpy(szItemScriptName, "./Script/Game/ItemScript.txt", MAX_PATH);
	}

    if (false == Item::CItemMgr::GetInstance().LoadItemProtoType(szItemScriptName))
    {
        ERRLOG0(g_Log, "������ ��ũ��Ʈ�� ���� �� �����ϴ�.");
        return false;
    }

	// ������ �ռ� ��ũ��Ʈ �ε�
	if (false == Item::CItemMgr::GetInstance().LoadItemChemical())
	{
		ERRLOG0(g_Log, "������ �ռ� ��ũ��Ʈ�� ���� �� �����ϴ�.");
		return false;
	}

	// ������ ���� ��ũ��Ʈ �ε�
	if (false == Item::CItemMgr::GetInstance().LoadItemSpeacialCompensation())
	{
		ERRLOG0(g_Log, "������ ���� ��ũ��Ʈ�� ���� �� �����ϴ�.");
		return false;
	}

    // ��ų ��ũ��Ʈ �ε�
    if (false == CSkillMgr::GetInstance().LoadSkillsFromFile())
    {
		ERRLOG0(g_Log, "��ų ��ũ��Ʈ�� ���� �� �����ϴ�.");
        return false;
    }

    // ��ų ���̺� �ʱ�ȭ
    if (false == Skill::CProcessTable::GetInstance().Initialize())
    {
        ERRLOG0(g_Log, "��ų ó�� ���̺��� �ʱ�ȭ �� �� �����ϴ�.");
        return false;
    }
    
	// ���� �̺�Ʈ �ʱ�ȭ
	if (false == CGameEventMgr::GetInstance().Initialize())
	{
		ERRLOG0(g_Log, "���� �̺�Ʈ �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	// ������ ��ġ ��ũ��Ʈ �ε�
	if (false == CCharRespawnMgr::GetInstance().LoadRespawnFromFile())
	{
		ERRLOG0(g_Log, "������ ��ġ ��ũ��Ʈ�� ���� �� �����ϴ�.");
		return false;
	}

    if (false == Filter::InitFilter())
    {
        ERRLOG0(g_Log, "�̸� ���͸� ���� �� �����ϴ�.");
        return false;
    }

	// �� �ε�
	CCellManager& CellManager = CCellManager::GetInstance();

	if (false == CellManager.LoadComplete())
	{
		CellManager.Load();
		CellManager.SetMoving(true);
	}

	// ���� ��ġ ��ũ��Ʈ �ε�
	switch ( CServerSetup::GetInstance().GetServerZone() )
	{
		case SERVER_ID::CAPITAL:
		{
			char szMineralVeinScriptName[MAX_PATH];
			sprintf(szMineralVeinScriptName, "./Script/Game/MineralVein/MineralVein%d.gsf", CServerSetup::GetInstance().GetServerZone());
			CMineralVeinMgr& MineralVeinMgr = CMineralVeinMgr::GetInstance();
			if (false == MineralVeinMgr.LoadMineralVeinsFromBinary(szMineralVeinScriptName))
			{
				ERRLOG0(g_Log, "���� ��ġ ��ũ��Ʈ �ε忡 �����߽��ϴ�.");
				return false;
			}
		}
		break;
	}

    // ���� ä�� ��ũ��Ʈ �ʱ�ȭ - �� �ε��ؼ� ���� �ε��� �� �ε�
    if (false == CMonsterShout::GetInstance().LoadScript())
    {
        ERRLOG0(g_Log, "���� ä�� ��ũ��Ʈ �ε忡 �����߽��ϴ�.");
        return false;
    }

	// �Ǿƽĺ� �� �ʱ�ȭ
	if (false == EnemyCheck::CCheckTable::GetInstance().Initialize())
	{
		ERRLOG0(g_Log, "�Ǿƽĺ� ���� �ʱ�ȭ�ϴµ� �����Ͽ����ϴ�.");
		return false;
	}

	// ------------------------------------------------------------------------------------------
	// ���� ���� ���� �ʱ�ȭ
	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
/*
	if (0 == LoadAuthTable("./Script/Server/CSAuth.tab"))
	{
		ERRLOG0(g_Log, "RowGameServer initialize failed : GameGuard LoadAuthTable failed");
		return false;
	}
	
	if (0 == LoadAuthIndex("./Script/Server/CSAuth.idx"))
	{
		ERRLOG0(g_Log, "RowGameServer initialize failed : GameGuard LoadAuthIndex failed");
		return false;
	}

	if (0 == InitPacketProtect("RYLPART2", 1))
	{
		ERRLOG0(g_Log, "RowGameServer initialize failed : GameGuard InitPacketProtect failed");
		return false;
	}
*/

#ifndef NO_GAMEGUARD

/*
	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
	// 1. �������� �ʱ�ȭ �Լ� : InitGameguardAuth
	unsigned long dwGGErrCode = InitGameguardAuth("./Script/Server", 50);
	if (ERROR_SUCCESS != dwGGErrCode)
	{	
		ERRLOG1(g_Log, "RowGameServer initialize failed : GameGuard InitGameguardAuth failed(%d)", dwGGErrCode);
		return false;
	}
*/
	// 1. �������� �ʱ�ȭ �Լ� : InitGameguardAuth
	unsigned long dwGGErrCode = InitGameguardAuth("./Script/Server", 50, true, NPLOG_DEBUG|NPLOG_ERROR);
	if (ERROR_SUCCESS != dwGGErrCode)
	{	
		ERRLOG1(g_Log, "ROWGameServer initialize failed : GameGuard InitGameguardAuth failed(%d)", dwGGErrCode);//was ROW hayzohar
		return false;
	}

	// 2. �������� ���� �����Լ� : SetUpdateCondition
	// 2. Storage function of server certification condition : SetUpdateCondition
	SetUpdateCondition(30, 50);
	// ------------------------------------------------------------------------------------------

#endif

	m_bInitialized = true;
    return true;
}

void CRylGameServer::DestoryGameObject(void)
{       
#ifndef NO_GAMEGUARD
	CleanupGameguardAuth();
#endif
}
