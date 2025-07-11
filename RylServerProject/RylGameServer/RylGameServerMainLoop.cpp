
#include "stdafx.h"

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/Character/SphereTree/CharSphereTree.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Castle/CastleMgr.h>

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/DuelMap/DuelCellManager.h>

#include <Network/Session/Session.h>
#include <Network/Session/LimitUserByIP.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharLoginOut.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentPacketParse.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/IOCP/IOCPNet.h>

#include <Quest/QuestMgr.h>

#include <GameTime/GameTimeMgr.h>

#include <Pattern/Command.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Log/GameLog.h>
#include <Skill/Spell/GlobalSpellMgr.h>
#include <Community/Party/PartyMgr.h>

#include <algorithm>
#include "mmsystem.h"

#include "RylGameServer.h"
#include "./Commands/DummyCharacters.h"


struct FnDisconnectCharacter
{
    bool operator() (CCharacter* lpCharacter)
    {
        if (NULL != lpCharacter)
        {
            CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
            if (NULL != lpDispatch) 
			{ 
				lpDispatch->Disconnect();
			}
        }

        return true;
    }
};

struct FnRegenHPAndMP
{
    bool operator() (CAggresiveCreature* lpAggresiveCreature)
    {
        if (NULL != lpAggresiveCreature) 
		{
            lpAggresiveCreature->RegenHPAndMP(0, 0, true);
        }

        return true;
    }
};

struct FnCSAuth
{
	bool operator() (CCharacter* lpCharacter)
	{
		if (NULL != lpCharacter) 
		{
			// edith 2009.08.11 게임가드 테스트
			if (0 == lpCharacter->GetAdminLevel())
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch) 
				{ 				
/*
					// edith 2009.08.11 게임가드 2.5 업그레이드
					if (false == lpDispatch->IsAuth())
					{
						ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드를 보내지않아 연결을 끊습니다.", lpCharacter->GetCID());
						lpDispatch->Disconnect();
						return true;
					}
*/
					GG_AUTH_DATA* lpAuthData = NULL;				
					if (false == lpDispatch->GetAuthQuery(&lpAuthData))
					{
						ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드(2) 체크에 실패하여 연결을 끊습니다.", lpCharacter->GetCID());
						lpDispatch->Disconnect();
						return true;
					}

					// edith 2009.08.11 게임가드 2.5 업그레이드
//					MessageBox(NULL, "게임가드 인증 시간 체크 결과", "GG", MB_OK);
					return GameClientSendPacket::SendCSAuth(lpDispatch->GetSendStream(), 
						lpCharacter->GetCID(), lpDispatch->GetAuthCode(), lpAuthData, PktBase::NO_SERVER_ERR);
				}
			}
		}
		return true;
	}
};


struct InstrumentInfo
{
	InstrumentInfo(unsigned long dwIndex, char* szName) : m_dwIndex(dwIndex), m_szName(szName) { }
	unsigned long m_dwIndex;
	char*		  m_szName;
};


struct PrepareBroadcastData
{
    void operator() (CCharacter* lpCharacter)
    { if(0 != lpCharacter) { lpCharacter->GetSerializeData().PrepareData(*lpCharacter); } }

    void operator() (CMonster* lpMonster)
    { if(0 != lpMonster) { lpMonster->GetSerializeData().PrepareData(*lpMonster); } }
};


enum InstrumentsType
{
	INSTRUMENT_TOTAL_LOOP,
	INSTRUMENT_CHECK_DELETE_ITEM,
	INSTRUMENT_REGEN_HP_MP,
	INSTRUMENT_DBUPDATE,
	INSTRUMENT_CELLBROADCASTING,
	INSTRUMENT_GLOBALSPELLMGR,
	INSTRUMENT_PROCESS_ALL_MONSTER,	
	INSTRUMENT_CHARACTER_LOGOUT,	
	INSTRUMENT_PROCESS_RESPAWN_QUEUE,
	INSTRUMENT_AUTO_RESPAWN,
	INSTRUMENT_VIRTUALAREA,
	INSTRUMENT_RESPAWN,
	INSTRUMENT_CHAR_SPHERE_TREE,
	INSTRUMENT_WORLDWEAPON_FIRE,
	MAX_PERFORMANCE_INSTRUMENTS
};

#define DECLARE_INSTRUMENT_INFO(name) InstrumentInfo(name, #name)

const InstrumentInfo g_InstrumentInfo[MAX_PERFORMANCE_INSTRUMENTS] = 
{
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_TOTAL_LOOP),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_CHECK_DELETE_ITEM),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_REGEN_HP_MP),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_DBUPDATE),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_CELLBROADCASTING),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_GLOBALSPELLMGR),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_PROCESS_ALL_MONSTER),	
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_CHARACTER_LOGOUT),	
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_PROCESS_RESPAWN_QUEUE),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_AUTO_RESPAWN),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_VIRTUALAREA),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_RESPAWN),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_CHAR_SPHERE_TREE),
	DECLARE_INSTRUMENT_INFO(INSTRUMENT_WORLDWEAPON_FIRE)
};

#define INSTRUMENT_GAMESERVER(object, name) PERFORMANCE_CHECK(CAutoInstrument (name)((object)[(name)]));

class CGameServerProcessThread : public CProcessThread
{
public:

	enum
	{
		GAME_PROCESS_TICK	= 100,
		TICKS_PER_SECOND	= 1000 / GAME_PROCESS_TICK
	};
	
	CGameServerProcessThread(CRylGameServer& RylGameServer);
	
	virtual void InternalRun(CPulse& Pulse);
    virtual void Cleanup(CPulse& Pulse);

private:

	CRylGameServer&		m_RylGameServer;

    CCreatureManager&   m_CreatureManager;
    CCellManager&       m_CellManager;
	CDuelCellManager&   m_DuelCellManager;
    CGlobalSpellMgr&    m_GlobalSpellMgr;
    CGameLog&			m_GameLog;

	Castle::CCastleMgr&	m_CastleMgr;
	CSiegeObjectMgr&	m_SiegeObjectMgr;
	CCharRespawnMgr&	m_CharRespawnMgr;
	CCharSphereTree&	m_CharSphereTree;

    VirtualArea::CVirtualAreaMgr&	m_VirtualAreaMgr;

    CPerformanceInstrument	        m_Instruments[MAX_PERFORMANCE_INSTRUMENTS];

	std::mem_fun_t<bool, CMonster>	                m_processMonster;
    std::mem_fun_ref_t<void, CCell>                 m_processPrepareBroadCast;
	std::mem_fun1_ref_t<void, CCell, unsigned long> m_processBroadCast;	
};

bool CRylGameServer::AddGameProcessThread()
{
	return AddProcessThread(new CGameServerProcessThread(*this));
}


CGameServerProcessThread::CGameServerProcessThread(CRylGameServer& RylGameServer)
:	CProcessThread(RylGameServer, GAME_PROCESS_TICK), 
	m_RylGameServer(RylGameServer),
	m_CreatureManager(CCreatureManager::GetInstance()),
	m_CellManager(CCellManager::GetInstance()),
	m_VirtualAreaMgr(VirtualArea::CVirtualAreaMgr::GetInstance()),
	m_DuelCellManager(CDuelCellManager::GetInstance()),
	m_GlobalSpellMgr(CGlobalSpellMgr::GetInstance()),
	m_GameLog(CGameLog::GetInstance()),
	m_CastleMgr(Castle::CCastleMgr::GetInstance()),
	m_SiegeObjectMgr(CSiegeObjectMgr::GetInstance()),
	m_processMonster(&CMonster::Process),
	m_processPrepareBroadCast(&CCell::PrepareBroadCast),
	m_processBroadCast(&CCell::BroadCast),
	m_CharRespawnMgr(CCharRespawnMgr::GetInstance()),
	m_CharSphereTree(CCharSphereTree::GetInstance())
{
	for(unsigned int nCount = 0; nCount < MAX_PERFORMANCE_INSTRUMENTS; ++nCount)
	{
		m_Instruments[nCount].SetName(g_InstrumentInfo[nCount].m_szName);
	}    
}

void CGameServerProcessThread::Cleanup(CPulse& Pulse)
{
    CLimitUserByIP* lpLimitByIP = m_RylGameServer.GetClientLimit();

    if(0 != lpLimitByIP)
    {
        // 이제부터 접속 금지!
        lpLimitByIP->OperateMode(CLimitUserByIP::DENY_ALL);
    }

	// 길드 요새 상점 정보 업데이트
	m_SiegeObjectMgr.ProcessCampShopUpdate(true);

	// 클라이언트 접속을 전부 끊고, DBAgent로 Logout을 보낸다.
	m_CreatureManager.ProcessAllCharacter(FnDisconnectCharacter());
	m_CreatureManager.ProcessAllCharacter(std::bind2nd(std::mem_fun1(&CCharacter::DBUpdateForce), DBUpdateData::LOGOUT));

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if(0 != lpDBAgentDispatch)
	{
		DBAgentPacketParse::SendServerLogout(*lpDBAgentDispatch);
       
        SERLOG1(g_Log, "this:0x%p/try client logout process", this);

		CIOCPNet* lpIOCPNet = m_RylGameServer.GetIOCPNet();
		if(0 != lpIOCPNet)
		{			
    		// 10초간 대기.
			unsigned long dwTotalWaitPulse = 10 * Pulse.GetTicksPerSec();
            unsigned long dwStartPulse = Pulse.GetCurrentPulse();
			
            // 서버 로그아웃을 받으면 중계 접속을 끊기 때문에, 중계 접속이 끊길때까지 루프를 돈다.
			while(!CDBAgentDispatch::GetDispatchTable().IsEmpty() &&
                 dwTotalWaitPulse < Pulse.GetCurrentPulse() - dwStartPulse)
			{
                Pulse.CheckSleep();
				lpIOCPNet->Process();				
			}
		}

        SERLOG2(g_Log, "this:0x%p/client logout process finish (%s)", this, 
            CDBAgentDispatch::GetDispatchTable().IsEmpty() ? "Finish complete" : "Timeout");
	}

    // 모든 크리쳐를 제거한다.
    CCreatureManager::GetInstance().DestroyAll();
    CDummyCharacterList::GetInstance().Destroy();

    // 셀을 제거한다.
    CCellManager::GetInstance().Destroy();
    
	GetFunctionTimingResult("ROWGameServer");//was ROW hayzohar
}

void CGameServerProcessThread::InternalRun(CPulse& Pulse)
{
    unsigned long dwCurrentPulse = Pulse.GetCurrentPulse();
    
    // 게임 로그 시간을 업데이트
    m_GameLog.UpdateLogTime();

	INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_TOTAL_LOOP);

	// 스펠(챈트&인챈트) 처리 (1초단위로 처리)
	if (0 == (dwCurrentPulse % (1 * TICKS_PER_SECOND)))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_GLOBALSPELLMGR);
		m_GlobalSpellMgr.Process();            
	}
    
    // 일정 시간 단위로 맵에서 아이템을 지우는 처리		
    assert(1 < CCell::CHECK_TIME && "펄스 세팅에 문제가 있습니다.");
    if (1 == (dwCurrentPulse % CCell::CHECK_TIME)) 
    {
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_CHECK_DELETE_ITEM);
        m_CellManager.ProcessAllCell(std::mem_fun_ref(&CCell::CheckDeleteItem));

		m_VirtualAreaMgr.ProcessDeleteItem();
    }

	// 몬스터 처리 (0.2 초에 한번 처리)		
	if (1 == (dwCurrentPulse % 2))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_PROCESS_ALL_MONSTER);						
		m_CreatureManager.ProcessAllMonster(m_processMonster);
		m_SiegeObjectMgr.ProcessAllSiegeObject();

		m_VirtualAreaMgr.ProcessAllMonster();
	}

	// 몬스터와 플레이어의 Regen 처리		
	assert(2 < CAggresiveCreature::REGEN_TIME && "펄스 세팅에 문제가 있습니다.");
	if (2 == (dwCurrentPulse % CAggresiveCreature::REGEN_TIME))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_REGEN_HP_MP);
        m_CreatureManager.ProcessAllMonster(FnRegenHPAndMP());
        m_CreatureManager.ProcessAllCharacter(FnRegenHPAndMP());
				
		m_VirtualAreaMgr.ProcessMonsterRegenHPAndMP();
		m_CastleMgr.ProcessEmblemRegenHPAndMP();
		m_SiegeObjectMgr.ProcessCampRegenHPAndMP();
	}

	// 가상맵 처리
	assert(2 < VirtualArea::CVirtualAreaMgr::VIRTUALAREA_PULSE && "펄스 세팅에 문제가 있습니다.");
	if (2 == (dwCurrentPulse % VirtualArea::CVirtualAreaMgr::VIRTUALAREA_PULSE))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_VIRTUALAREA);
		m_VirtualAreaMgr.ProcessAllVirtualArea();
	}

	// DBUpdate 처리		
    assert(3 < CCharacter::DBUPDATE_PULSE && "펄스 세팅에 문제가 있습니다.");
	if (3 == (dwCurrentPulse % (CCharacter::DBUPDATE_PULSE)))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_DBUPDATE);			

		// 캐릭터 정보
		m_CreatureManager.ProcessAllCharacter(
            std::bind2nd(std::mem_fun1(&CCharacter::DBUpdate), DBUpdateData::UPDATE)); 
		
		// 길드 요새 상점 정보
		m_SiegeObjectMgr.ProcessCampShopUpdate(false);
	}

    // 캐릭터 로그아웃 처리, 소환 몬스터의 사망 처리 
    assert(3 < CCharacter::LOGOUT_PULSE && "펄스 세팅에 문제가 있습니다.");
	if (3 == (dwCurrentPulse % CCharacter::LOGOUT_PULSE))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_CHARACTER_LOGOUT);
			    	
        m_CreatureManager.ProcessCharacterLogout();
		m_CreatureManager.ProcessSummonMonsterDead();

		m_VirtualAreaMgr.ProcessSummonMonsterDead();
	}

	// 공헌 메달 나눠주기
	// 배틀 그라운드, 배틀 서버에서 잠자는 시체 자동 리스폰 처리
	// 배틀 그라운드 석상 상태 처리 (전투 상태, 휴식 상태)
	assert(3 < CCharacter::BATTLE_GROUND_PULSE && "펄스 세팅에 문제가 있습니다.");
	if (3 == (dwCurrentPulse % CCharacter::BATTLE_GROUND_PULSE))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_AUTO_RESPAWN);
		m_CreatureManager.ProcessBattleGround();
	}

	// 클라이언트 브로드 캐스팅			
    assert(4 < CCell::BROADCASTING_TIME && "펄스 세팅에 문제가 있습니다.");
	if (4 == (dwCurrentPulse % CCell::BROADCASTING_TIME)) 
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_CELLBROADCASTING);

        CCreatureManager::GetInstance().ProcessAllCharacter(PrepareBroadcastData());
        CCreatureManager::GetInstance().ProcessAllMonster(PrepareBroadcastData());

        m_CellManager.ProcessAllCell(m_processPrepareBroadCast);
        m_CellManager.ProcessAllCell(m_processBroadCast, dwCurrentPulse);

        m_DuelCellManager.ProcessAllCell(m_processPrepareBroadCast);
        m_DuelCellManager.ProcessAllCell(m_processBroadCast, dwCurrentPulse);

		m_VirtualAreaMgr.ProcessAllCellPrepareBroadCast();
		m_VirtualAreaMgr.ProcessAllCellBroadCast(dwCurrentPulse);

		m_SiegeObjectMgr.PrepareBroadCast();
		m_SiegeObjectMgr.BroadCast();
	}

    // 리스폰 큐 프로세스 (1 초단위로 배틀그라운드만 적용)
    assert(7 < (1 * TICKS_PER_SECOND) && "펄스 세팅에 문제가 있습니다.");
	if (7 == (dwCurrentPulse % (1 * TICKS_PER_SECOND)))
	{						
		if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
		{
			INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_PROCESS_RESPAWN_QUEUE);
			m_CreatureManager.ProcessRespawnQueue();
		}			
	}

	// 리스폰 프로세스 (1 초단위로)
	assert(7 < (1 * TICKS_PER_SECOND) && "펄스 세팅에 문제가 있습니다.");
	if (7 == (dwCurrentPulse % (1 * TICKS_PER_SECOND)))
	{						
		if (SERVER_ID::ZONE3 != CServerSetup::GetInstance().GetServerZone())
		{
			INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_RESPAWN);
			m_CharRespawnMgr.ProcessRespawn();
		}			
	}

	// 캐릭터 스피어 트리 갱신
	assert(8 < (1 * TICKS_PER_SECOND) && "펄스 세팅에 문제가 있습니다.");
	if (8 == (dwCurrentPulse % (1 * TICKS_PER_SECOND)))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_CHAR_SPHERE_TREE);
		m_CharSphereTree.Process();
	}

	// 1초 단위로 월드 웨폰 발사 처리
	assert(9 < (1 * TICKS_PER_SECOND) && "펄스 세팅에 문제가 있습니다.");
	if (9 == (dwCurrentPulse % (1 * TICKS_PER_SECOND)))
	{
		INSTRUMENT_GAMESERVER(m_Instruments, INSTRUMENT_WORLDWEAPON_FIRE);
		m_SiegeObjectMgr.ProcessWorldWeaponFire();
	}

    // 3초단위로 파티 전부 돌면서 파티 정보 업데이트
    if (0 == (dwCurrentPulse % (3 * TICKS_PER_SECOND)))
    {
        CPartyMgr::GetInstance().UpdatePartyData();
    }
    
    // 3초단위로 콘솔 정보 업데이트
    if (0 == (dwCurrentPulse % (3 * TICKS_PER_SECOND)))
    {
        m_RylGameServer.PrintServerInfo();
        m_RylGameServer.PrintStatistics();            
    }

	// 2초 단위로 게임 시간 정보 업데이트
	if (0 == (dwCurrentPulse % (2 *TICKS_PER_SECOND)))
	{
		CGameTimeMgr::GetInstance().UpdateGameTimeInfo();
	}
	
	// 퍼포먼스 로그 출력 (1시간에 한 번씩)
	if (0 == (dwCurrentPulse % (60 * 60 * TICKS_PER_SECOND)))
	{
		GetFunctionTimingResult("ROWGameServer");//was ROW hayzohar
	}

    // 연결 정보 세팅 (DB중계, 채팅)
    if (0 == (dwCurrentPulse % (5 * TICKS_PER_SECOND)))
    {
        unsigned long dwStatusFlag = 0;

        if(!CDBAgentDispatch::GetDispatchTable().IsEmpty())
        {
            dwStatusFlag |= (1 << CServerSetup::AgentServer);
        }
            
        if(!CChatDispatch::GetDispatchTable().IsEmpty())
        {
            dwStatusFlag |= (1 << CServerSetup::ChatServer);
        }
        
        m_RylGameServer.SetStatusFlag(dwStatusFlag);
    }

#ifdef AUTH_MY
	m_RylGameServer.Update( dwCurrentPulse );
#endif

#ifndef NO_GAMEGUARD
	// 게임가드 인증 (3분에 한번씩)
	if (0 == (dwCurrentPulse % (3 * 60 * TICKS_PER_SECOND)))
	{
		// edith 2009.08.11 게임가드 2.5 업그레이드
		if (true == CServerSetup::GetInstance().GetHackCheck())
		{
			m_CreatureManager.ProcessAllCharacter(FnCSAuth());
		}
	}	
#endif

	// 퀘스트 처리.
	CQuestMgr::GetInstance().ProcessPendingQuest();

    /*
    // 반응성을 높이기 위해서, 시간이 남으면 패킷 처리좀 더 해 준다.
    CIOCPNet* lpIOCPNet = m_RylGameServer.GetIOCPNet();
    if(0 != lpIOCPNet)
    {
        const unsigned long dwMinSleepTime = 10;
        while(dwMinSleepTime < Pulse.GetRemainTime())
        {
            lpIOCPNet->Process();   
        }
    }
    */
}



