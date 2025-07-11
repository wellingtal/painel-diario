
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

// edith 2009.08.11 게임가드 2.5 업그레이드
void Log(const char *szMsg, ... ) 
{ 
	va_list marker; 
	char szTmp[256] = {0,}; 

	va_start( marker ,szMsg );   
	vsprintf(szTmp,szMsg, marker); 
	va_end( marker );              

	// DebugView 를 통해서 로그를 확인 할 수 있습니다.
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
    // 로그 이름 초기화.
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

    // 패킷 통계 로그 헤더 초기화
    CPacketStatistics::GetInstance().SetUserMessageFunc(PrePerformanceMsg, 0);

    // 퍼포먼스 로그 헤더 초기화
    CPerformanceCheck::GetInstance().SetUserMessageFunc(PrePerformanceMsg, 0);

    // 게임 로그 초기화
    if (false == CGameLog::GetInstance().Initialize(szLogFilePrefixName))
    {
        ERRLOG0(g_Log, "게임 로그를 초기화하는데 실패했습니다.");
        return false;
    }

    // 아이템 스크립트 로드
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
        ERRLOG0(g_Log, "아이템 스크립트를 읽을 수 없습니다.");
        return false;
    }

	// 아이템 합성 스크립트 로드
	if (false == Item::CItemMgr::GetInstance().LoadItemChemical())
	{
		ERRLOG0(g_Log, "아이템 합성 스크립트를 읽을 수 없습니다.");
		return false;
	}

	// 아이템 보상 스크립트 로드
	if (false == Item::CItemMgr::GetInstance().LoadItemSpeacialCompensation())
	{
		ERRLOG0(g_Log, "아이템 보상 스크립트를 읽을 수 없습니다.");
		return false;
	}

    // 스킬 스크립트 로드
    if (false == CSkillMgr::GetInstance().LoadSkillsFromFile())
    {
		ERRLOG0(g_Log, "스킬 스크립트를 읽을 수 없습니다.");
        return false;
    }

    // 스킬 테이블 초기화
    if (false == Skill::CProcessTable::GetInstance().Initialize())
    {
        ERRLOG0(g_Log, "스킬 처리 테이블을 초기화 할 수 없습니다.");
        return false;
    }
    
	// 게임 이벤트 초기화
	if (false == CGameEventMgr::GetInstance().Initialize())
	{
		ERRLOG0(g_Log, "게임 이벤트 초기화에 실패하였습니다.");
		return false;
	}

	// 리스폰 위치 스크립트 로드
	if (false == CCharRespawnMgr::GetInstance().LoadRespawnFromFile())
	{
		ERRLOG0(g_Log, "리스폰 위치 스크립트를 읽을 수 없습니다.");
		return false;
	}

    if (false == Filter::InitFilter())
    {
        ERRLOG0(g_Log, "이름 필터를 읽을 수 없습니다.");
        return false;
    }

	// 셀 로드
	CCellManager& CellManager = CCellManager::GetInstance();

	if (false == CellManager.LoadComplete())
	{
		CellManager.Load();
		CellManager.SetMoving(true);
	}

	// 광물 배치 스크립트 로드
	switch ( CServerSetup::GetInstance().GetServerZone() )
	{
		case SERVER_ID::CAPITAL:
		{
			char szMineralVeinScriptName[MAX_PATH];
			sprintf(szMineralVeinScriptName, "./Script/Game/MineralVein/MineralVein%d.gsf", CServerSetup::GetInstance().GetServerZone());
			CMineralVeinMgr& MineralVeinMgr = CMineralVeinMgr::GetInstance();
			if (false == MineralVeinMgr.LoadMineralVeinsFromBinary(szMineralVeinScriptName))
			{
				ERRLOG0(g_Log, "광물 배치 스크립트 로드에 실패했습니다.");
				return false;
			}
		}
		break;
	}

    // 몬스터 채팅 스크립트 초기화 - 셀 로드해서 몬스터 로드한 후 로드
    if (false == CMonsterShout::GetInstance().LoadScript())
    {
        ERRLOG0(g_Log, "몬스터 채팅 스크립트 로드에 실패했습니다.");
        return false;
    }

	// 피아식별 룰 초기화
	if (false == EnemyCheck::CCheckTable::GetInstance().Initialize())
	{
		ERRLOG0(g_Log, "피아식별 룰을 초기화하는데 실패하였습니다.");
		return false;
	}

	// ------------------------------------------------------------------------------------------
	// 게임 가드 관련 초기화
	// edith 2009.08.11 게임가드 2.5 업그레이드
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
	// edith 2009.08.11 게임가드 2.5 업그레이드
	// 1. 서버인증 초기화 함수 : InitGameguardAuth
	unsigned long dwGGErrCode = InitGameguardAuth("./Script/Server", 50);
	if (ERROR_SUCCESS != dwGGErrCode)
	{	
		ERRLOG1(g_Log, "RowGameServer initialize failed : GameGuard InitGameguardAuth failed(%d)", dwGGErrCode);
		return false;
	}
*/
	// 1. 서버인증 초기화 함수 : InitGameguardAuth
	unsigned long dwGGErrCode = InitGameguardAuth("./Script/Server", 50, true, NPLOG_DEBUG|NPLOG_ERROR);
	if (ERROR_SUCCESS != dwGGErrCode)
	{	
		ERRLOG1(g_Log, "ROWGameServer initialize failed : GameGuard InitGameguardAuth failed(%d)", dwGGErrCode);//was ROW hayzohar
		return false;
	}

	// 2. 서버인증 상태 저장함수 : SetUpdateCondition
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
