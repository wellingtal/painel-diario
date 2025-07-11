
#include "stdafx.h"

#include <Log/LogCommands.h>
#include <Log/CharacterLog.h>

#include <Utility/Math/Math.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/ServerLogPacket.h>

#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentPacketParse.h>
#include <Network/Dispatch/DBAgent/RegularAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharLoginOut.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameLog/SendLogPacket.h>

#include <Network/Dispatch/DBAgent/CastlePacketParse.h>

#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/FieldMap/CellManager.h>

#include <Item/ItemFactory.h>
#include <Item/Container/ExchangeContainer.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellUtil.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/SiegeObjectMgr.h>


#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Community/Party/PartyMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "Character.h"
#include "CharacterCreate.h"
#include "SphereTree/CharSphereTree.h"

// forward decl.
void CheckDuplicatedItem(CCharacter& character);

void SendItemDuplicatedLog(CSendStream& SendStream, unsigned __int64 dwItemSerial, 
                           Item::CItemOwnerInfo& itemOwnerInfo, unsigned long dwItemQty);

void CCharacter::PrepareLogin(void)
{
    // HP/MP세팅
	
    m_CreatureStatus.m_nNowHP = m_DBData.m_Info.HP;
    m_CreatureStatus.m_nNowMP = m_DBData.m_Info.MP;
	
    // 복사 아이템 검사
    CheckDuplicatedItem(*this);
	
	// edith 2009.02.07 석상전 위치 내의 지역에서 로그인하는거 방지
	//if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		// 좌표 검사
		if (!CCellManager::GetInstance().CheckPositionInZone(m_CurrentPos))
		{
			if (CServerSetup::GetInstance().IsBattleGameServer())
			{
				m_CurrentPos = CharCreate::BGServerStartPos[GetRace()][Math::Random::ComplexRandom(CharCreate::MAX_LOBBY_RESPAWN_POS)];//hz pos possible needs to be changed
				m_CurrentPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
				m_CurrentPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
			}
			else
			{
				m_CurrentPos = CCharRespawnMgr::GetInstance().GetDefaultRespawnPos(GetNation());
			}

			m_DBData.m_Pos.LastPoint.fPointX = m_CurrentPos.m_fPointX;
			m_DBData.m_Pos.LastPoint.fPointY = m_CurrentPos.m_fPointY;
			m_DBData.m_Pos.LastPoint.fPointZ = m_CurrentPos.m_fPointZ;
		}
	}
	
    // 브로드캐스팅 데이터 준비
    m_SerializeCharacterData.PrepareData(*this);
    m_SerializeCharacterData.ClearDeltaData();

    SetOperationFlag(CHAR_INFO_LOADED);
}

bool CCharacter::Login()
{
    if (NULL == m_lpGameClientDispatch)
    {
        return false;
    }

	CSendStream& SendStream = m_lpGameClientDispatch->GetSendStream();
/*
#ifndef NO_GAMEGUARD
	// 로그인 하기전에 게임가드 처리
	// edith 2009.08.11 게임가드 테스트
	if (NULL != m_lpGameClientDispatch)// && 0 == GetAdminLevel()) 
	{ 				
		// edith 2009.08.11 게임가드 2.5 업그레이드
//		if (false == m_lpGameClientDispatch->IsAuth())
//		{
//			ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드를 보내지않아 연결을 끊습니다.", GetCID());
//			m_lpGameClientDispatch->Disconnect();
//			return false;
//		}

		GG_AUTH_DATA* lpAuthData = NULL;				
		if (false == m_lpGameClientDispatch->GetAuthQuery(&lpAuthData))
		{
			ERRLOG1(g_Log, "CID:0x%08x 게임 가드 인증 코드(2) 체크에 실패하여 연결을 끊습니다.", GetCID());
			m_lpGameClientDispatch->Disconnect();
			return false;
		}

		GameClientSendPacket::SendCSAuth(SendStream, 
			GetCID(), m_lpGameClientDispatch->GetAuthCode(), lpAuthData, PktBase::NO_SERVER_ERR);
	}
#endif
*/   


// DELETE_ME : 더이상 클라이언트가 반전 모드로의 변경은 불가능하다. 개념만 남아있을 뿐. (2005-05-31 by 로딘)
/*
    // 전쟁 모드 제한
	if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone() ||
		SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
	{
        m_PeaceMode.m_bPeace = true;
	}
	else
	{
        m_PeaceMode.m_bPeace = false;
	}

	SetPeaceMode(m_PeaceMode, false);
    GameClientSendPacket::SendCharPeaceMode(SendStream, m_dwCID, 0, m_PeaceMode.m_bPeace, 0);
*/

    // 배틀 그라운드 서버군에서 레벨은 40으로 고정
    if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
    {
        // 레벨은 40으로 고정
        while (static_cast<unsigned long>(m_CreatureStatus.m_nLevel) < 40)
        {
            if (false == IncrementExp(static_cast<unsigned long>(
                EXP::ExpTable[m_CreatureStatus.m_nLevel - 1])))
            {
                break;
            }
        }
    }

	// Cell 일 설정되지 않은 상태이므로 MoveTo 함수를 한번 호출해준다.
	MoveTo(GetCurrentPos(), false);

	// edith 2009.06.13 16번 존 / 17번 존에서 무적시간 조정
	int InvincibleTime = 30;

	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		InvincibleTime = 15;

		char cZone = CServerSetup::GetInstance().GetServerZone();
		//  edith 2009.06.13 석상전에서 로그인할 경우 스타팅 포인트에서 리스폰되어서 시작
		// 위치를 이동시키려면 MovePos 즉 워프를 이용해서 해당좌표로 강제이동시킨다.
		//const Position targetPos = CCharRespawnMgr::GetInstance().GetTownRespawnPos(cZone, GetNation());//wasnt commented out autospawn fix
		//MovePos(targetPos, cZone, false);//wasnt commented out autospawn fix

		// 죽은 상태면 리스폰 시킨다.
        if (0 == m_CreatureStatus.m_nNowHP)
		{
			Respawn();
		}
	}


    // 로그인시 30초간 무적
    Skill::CAddSpell<CInvincibleSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
        Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, InvincibleTime))(this);

	// 월드 웨폰이 존재하면 월드 웨폰 인챈트 적용
	CCamp* lpWorldWeapon = CSiegeObjectMgr::GetInstance().GetWorldWeapon();
	if (lpWorldWeapon)
	{
		AddWorldWeaponEnchant(reinterpret_cast<CAggresiveCreature*>(lpWorldWeapon), lpWorldWeapon->GetNation()); 
	}

	// DB에서 스펠을 읽어와 다시 걸어준다.
	const SPELL spell = GetSpell();
	GetSpellMgr().SetSpell(spell);


    // 길드 정보를 준다.
    CGuild* lpGuild = 0;
    if (0 != GetGID() && 0 != (lpGuild = CGuildMgr::GetInstance().GetGuild(GetGID())))
    {
        unsigned short wError = PktBase::NO_SERVER_ERR;
        unsigned char cTitle = lpGuild->GetTitle(m_dwCID);

        if (Guild::NONE == cTitle)
        {
            wError = PktBase::SERVER_ERROR;
        }

        GameClientSendPacket::SendCharMyGuildInfo(SendStream, 
            lpGuild->GetGold(), lpGuild->GetRight(), cTitle, wError);
    }

    // 성 정보 전송
    GameClientSendPacket::SendCharCastleInfo(SendStream);
    
	// 게임 시간 정보 전송
	GameClientSendPacket::SendCharGameTimeInfo(SendStream);

    // 길드 요새 정보 전송
    GameClientSendPacket::SendCharCampInfo(SendStream);

// CASTLE_TODO : 성이 길드 소유가 아니므로 일단 막아둔다. 

    // 리스폰 속도 향상 적용
//    if (0 != GetGID())
//    {
//        Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByGID(GetGID());
//        if (lpCastle)
//        {
//            CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
//            if (lpEmblem && lpEmblem->GetUpgradeStep() > 0)
//            {
//                UpgradeRespawnSpeedByEmblem(lpEmblem->GetUpgradeType(), lpEmblem->GetUpgradeStep());
//            }
//        }
//    }

    // 엘리트 보너스 정보 전송
    GameClientSendPacket::SendCharEliteBonus(SendStream, GetEliteBonus());

	// 거부 옵션 정보 전송
	GameClientSendPacket::SendCharControlOption(SendStream, m_dwCID, m_RejectOption);

	// 어빌리티 포인트 업데이트
	UpdateUseAbilityPoint();

	// edith 2008.06.03 공헌훈장 포인트효과
	// 컨텐츠 : 다크 카나번 국가 전쟁
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		// 국가전쟁 공헌훈장 포인트 효과.
		if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
		{
			RealmSkill::RealmInchantAdd(this);
		}
	//}
	// Admin 캐릭터에는 예외처리 (QA 쪽 요청).
	if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))//was(true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE)) hayzohar statue war fix
	{
		if(GetAdminLevel())
		{
			goto lb_move;
		}
	}
/*
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
	{
		if(GetAdminLevel())
		{
			goto lb_move;
		}
	}
*/
	// 길드전, 국가전 참여 캐릭터는 해당 시간에 로그인시 존 이동 처리를 해준다.
	if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&
		(GetGuildWarFlag() == Creature::WAR_ON || GetGuildWarFlag() == Creature::WAR_INSTANCE))
	{
		MoveToGuildWarZone();
	}

	// 공성전 중 로그인할 경우 공성측은 리스폰 지역으로 이동시킨다.
	// edith 2008.07.17 캐피탈존에 로그인했는데 공성시간이면 리스폰지역으로 이동함.
	if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
	{
		if(CGameTimeMgr::GetInstance().IsSiegeWarTime())
		{
			using namespace DBAgentPacketParse;

			
			SiegeMovePos(this);
		}
	}

	// 컨텐츠 : 다크 카나번 국가 전쟁
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
			(GetRealmWarFlag() == Creature::WAR_ON || GetRealmWarFlag() == Creature::WAR_INSTANCE) &&

			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL < CServerSetup::GetInstance().GetServerZone() || CServerSetup::GetInstance().GetServerZone() > SERVER_ID::STONE_WAR3) )
		{
			// 랠름전 시간인데 전투참가가 되어있고.하지만 16번이 아니니 강제로 16번으로 이동.
			MoveToRealmWarZone();
		}

		if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			GetRealmWarFlag() == Creature::WAR_OFF &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
		{
			// edith 2008.07.08 쟁에 참여를 안했는데 16번 존이고.. 랠름전 시간이면 강제 참여
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
				CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

			if (0 != lpDBAgentDispatch)
			{
				GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), GetCID(), GameTime::REALM, Creature::WAR_INSTANCE, 0);
			}

			// 전쟁시간에 로그인했는데 얼래 내가 석상전 맵에서 로그인을 했어.. 이르면 리스폰위치로 강제이동해..
			// edith 2008.07.08 전쟁에 참가안한 사람이지만 16번존에 존재하면 리스폰위치로 강제 이동시킨다.
//			MoveToRealmWarZone();
		}	

lb_move:

		// edith 2008.06.03 석상 인첸트효과
		// 다크 카나번에 로그인시
		if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
		{
			// 석상 정보를 보내준다.
			CCreatureManager::GetInstance().SendRealmStatueDisplayInfo(SendStream);

			bool bAddRealmStatueEnchant = false;
			//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
			//{
				// 국가 전쟁 시간이 아니라면, 석상 인챈트 효과를 걸어준다.
				if (!CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					bAddRealmStatueEnchant = true;
				}
			//}
/*
			// 공선전용 맵과 석상전용 맵이 따로기 때문에 해당 로직 불필요
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
			{
				// 공성전 컨텐츠 포함시 공성전 시간도 아니라면, 석상 인챈트 효과를 걸어준다.
				if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
				{
					bAddRealmStatueEnchant = false;
				}
			}
*/
/*
			// 석상 인챈트 효과를 적용할 수 있는 상황이라면....
			if (bAddRealmStatueEnchant)
			{
				// edith 2008.06.03 석상전이 끝날때 11시간 버프를 걸어준다.
				CCreatureManager::GetInstance().AddRealmStatueEnchant(this);
			}
*/
		}
		// 신규존 로그인시
		else if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE5)
		{
			// 생명축출기 정보를 보내준다.
			CCreatureManager::GetInstance().SendRealmStatueDisplayInfo(SendStream);
		}
	//}

    LOG_INOUT(
        
        const int MAX_LOG_BUFFER = 1024;
        char szBuffer[MAX_LOG_BUFFER];        
        unsigned long dwDispatchUID = m_lpGameClientDispatch->GetUID();
        const SOCKADDR_IN& sockAddr = m_lpGameClientDispatch->GetRemoteAddr().get_addr_in();

        _snprintf(szBuffer, MAX_LOG_BUFFER - 1,
            "UID:%d/CID:0x%08x(0x%p)/DispatchUID:%d/DispatchPointer:0x%p 로그인에 성공하였습니다. "
            "몇가지 기본 정보를 찍습니다. %s(lev:%2d, exp:%016I64u) IP:%15s:%5u", 
            m_dwUID, m_dwCID, this, dwDispatchUID, m_lpGameClientDispatch, 
            m_DBData.m_Info.Name, m_DBData.m_Info.Level, m_DBData.m_Info.Exp, 
            inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));

        szBuffer[MAX_LOG_BUFFER - 1] = 0;
        DETLOG0(g_Log, szBuffer);

        );
    
	return true;
}



bool CCharacter::Logout(DBUpdateData::UpdateType eUpdateType)
{
	// 배틀그라운드의 경우 (죽은 상태라면) 리스폰 큐에서 제거
	if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
	{
		CCreatureManager::GetInstance().PopRespawnQueue(this);
	}

	// 죽었는데 리스폰 안하고 로그아웃하면 
	// 내구도가 안깍이는 버그가 있어서 로그아웃할때 부활한 상태가 아니면 내구도를 깍아버린다.
	if(IsDead())
	{
		switch(m_eLastDeadType)
		{
		case DEAD_BY_NONE:      // 올 수 없음
		case DEAD_BY_CHARACTER: // 캐릭터에 의해 사망            
			break;

		case DEAD_BY_SUICIDE:   // 자살

			// 자살시 내구도 감소
			CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_SUICIDE);
			break;

		case DEAD_BY_MONSTER:   // 몬스터에 의해 사망

			if (!CServerSetup::GetInstance().GetDeathPenaltyEvent())
			{
				unsigned char cDecreasePersent = 0;

				if(m_AbilityValue[Skill::Type::AB_ENDUR_SHILD] != 0)
					cDecreasePersent = DURABILITY_DECREASE_PERSENT_BY_MONSTER*m_AbilityValue[Skill::Type::AB_ENDUR_SHILD]/100;

				// 몬스터에게 죽었을 때 내구도 감소
                CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_MONSTER-cDecreasePersent);
			}
			break;
		}
	}

	// 월드 웨폰 인챈트를 제거한다.
	ClearWorldWeaponEnchant();

	// 성문을 막고 있던 중이었다면... 성문 막기를 취소한다.
	if (0 != m_dwProtectGateCID)
	{
		CCastleGate* lpGate = reinterpret_cast<CCastleGate*>( CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwProtectGateCID) );
		if (lpGate)
		{
			lpGate->DeleteProtectGate(this);

			// 클라이언트에게 전송
			CGameClientDispatch* lpDispatch = GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), lpGate->GetCastleID(),
					lpGate->GetCID(), 0, 0,
					PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL,
					PktCastleCmd::NO_SERVER_ERR);
			}
		}
	}

	// 셀 로그아웃
	if (NULL != m_CellPos.m_lpCell)
	{
		m_CellPos.m_lpCell->DeleteCreature(m_dwCID);
		m_CellPos.m_lpCell = NULL;
	}

    // 채팅 서버로 로그아웃을 던진다.
    m_SerializeCharacterData.SendChatLogout(*this);

	// BG_TODO : 듀얼이나, 던젼의 경우에는.. 다시 처리해야 할지도 모른다.
	// VirtualArea 에 있었다면, VirtualArea 의 캐릭터리스트에서 제거
	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualAreaMgr::GetInstance().LeaveVirtualArea(this);
	}

	// 소환수가 있다면 없앤다.
	if (NULL != m_lpSummonee)
	{
		m_lpSummonee->Dead(NULL);		
	}

	// 거래중이라면 거래 취소
    CCharacter* lpExchangeCharacter = m_Exchange.GetExchangeCharacter();
	if (NULL != lpExchangeCharacter)
	{
		m_Exchange.ExchangeOK(false);

        CGameClientDispatch* lpExchangerDispatch = lpExchangeCharacter->GetDispatcher();
        if (NULL != lpExchangerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpExchangerDispatch->GetSendStream(), 
                m_dwCID, lpExchangeCharacter->GetCID(), PktExC::EXC_QUIT, PktExC::NO_SERVER_ERR);
        }
	}

	// 듀얼 초기화
	DuelInit(PktDuC::DUC_LOGOUT);

    // 파티찾기 리스트에서 삭제
	CPartyMgr::GetInstance().DeleteFindPartyList(m_dwCID);

    // 파티 주문 제거.
    if (0 != GetPID())
    {
        // 파티 로그아웃
        if (NULL != m_pParty)
        {
			m_pParty->PrepareLogout(m_dwCID);
        }
        else
        {
            ERRLOG3(g_Log, "CID:0x%08x 파티에 속한 녀석의 파티 포인터가 NULL입니다. PID:0x%08x, PartyPointer:0x%08x", 
                m_dwCID, m_DBData.m_Info.PID, m_pParty);
        }
    }
	
	// 캐릭터에 현제 걸려있는 스펠들을 DB에 저장한다.
	// 죽은게 아니면 스펠을 다시 읽지 않는다. 
	// 죽은직후 스펠을 저장한후 스펠을 삭제하기 때문에 여기서 다시 스펠을 저장하면
	// 스펠이 사라진다.
	if(!IsDead())
	{
		const SPELL spell = GetSpellMgr().GetAffectedInfo().GetSpellInfo();
		SetSpell(spell);
	}

	// 환전소 기능을 사용해서 임시 객체가 남아있다면 삭제
	CRegularAgentDispatch::GetTempCharacterMgr().EraseChar(m_dwCID);
	
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
        CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();

        // 퀘스트 정보 저장
		GameClientSendPacket::SendCharQuestInfo(AgentSendStream, this);

		// 환경 설정 정보 저장
		GameClientSendPacket::SendConfigInfoDB(AgentSendStream, this);
	}

	// 병기에 탑승중이라면 내린것으로 간주
	if (IsRideArms())
	{
		CSiegeObject* lpArms = CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwRideArmsCID);
		if (lpArms)
		{
			// 수송선 조종사가 나간경우
			if (Siege::RIDER_FOR_OWNER == lpArms->IsRider(m_dwCID))
			{
				lpArms->AllGetOff();
			}
			else
			{
				m_dwRideArmsCID = 0;
				lpArms->GetOff(m_dwCID);
			}
		}
	}

	int     nTotalSize = sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE;
    char    szCharBuffer[sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE];
	char*   lpCharacterInfo = szCharBuffer + sizeof(PktDBUpdate);

    PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(szCharBuffer);
    memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));
	
    unsigned short usError = 0;
	unsigned char cAdmin = (true == IsAdmin()) ? 1 : 0;
    
	bool bCharacterUpdate = GetCharacterInfo(lpCharacterInfo, &nTotalSize, lpPktDBUpdate->m_usUpdate);
	if (!bCharacterUpdate) 
	{
		nTotalSize = 0;
        usError = 1;        
        std::fill_n(lpPktDBUpdate->m_usUpdate, unsigned short(DBUpdateData::MAX_UPDATE_DB), 0);		
		ERRLOG1(g_Log, "CID:0x%08x 데이터를 복사해 올 수 없습니다. DB에 업데이트 할 수 없습니다.", m_dwCID);
	}
    else if (0 != lpDBAgentDispatch)
    {
	    if (IsOperationFlagSet(CHAR_INFO_LOADED))
	    {
            CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();

            // 창고 데이터 업데이트(창고가 열려 있으면 업데이트한다.)
            // 순서 주의! DB에 Update할 때는 창고 업데이트 후, 캐릭터를 업데이트한다.
            if (!m_Deposit.DBUpdate(AgentSendStream))
            {
                ERRLOG1(g_Log, "CID:0x%08x 창고 업데이트 실패", m_dwCID);
            }

            // 로그아웃 정보 DBAgent에 보내기
            lpPktDBUpdate->m_dlItemSerial	= Item::CItemFactory::GetInstance().GetItemUID();
            lpPktDBUpdate->m_dwSessionID    = m_dwSessionID;
            lpPktDBUpdate->m_dwUserID		= m_dwUID;
            lpPktDBUpdate->m_dwCharID		= m_dwCID;
            lpPktDBUpdate->m_TypeCode		= eUpdateType;
            lpPktDBUpdate->m_dwRequestKey   = 0;
			lpPktDBUpdate->m_cAdminFlag		= GetGMModelFlag();

            lpPktDBUpdate->m_dwRequestKey   = 0;
            lpPktDBUpdate->m_Address.S_un.S_addr = 0;
            lpPktDBUpdate->m_cAdminLevel    = 0;
			// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
            //lpPktDBUpdate->m_cPadding			= 0;
			lpPktDBUpdate->m_cAccountNation		= 0;
            lpPktDBUpdate->m_cNameChangeCount	= 0;
			lpPktDBUpdate->m_cGuildWarFlag		= 0;
			lpPktDBUpdate->m_cRealmWarFlag		= 0;
			lpPktDBUpdate->m_cRealmPoint		= 0;
			lpPktDBUpdate->m_cTacticsFlag		= 0;
			lpPktDBUpdate->m_PlayTime			= 0;
			lpPktDBUpdate->m_PremiumTime		= 0;
			lpPktDBUpdate->m_PremiumType		= 0;
			

            if (AgentSendStream.WrapCompress(reinterpret_cast<char*>(lpPktDBUpdate), 
                static_cast<unsigned short>(sizeof(PktDBUpdate) + nTotalSize), CmdDBUpdateData, 0, 0))
            {
 			    LOG_INOUT(
				    char szExp[64];
				    Math::Convert::Hex64ToStr(szExp, m_DBData.m_Info.Exp);
                    DETLOG7(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p 의 캐릭터 정보를 DBAgent에 업데이트합니다. 유저 로그아웃을 처리합니다."
					    " 로그아웃에 성공하였습니다. 몇가지 기본 정보를 찍습니다. %s(lev:%2d, exp:%s)",
					    m_dwUID, m_dwCID, this, m_lpGameClientDispatch, m_DBData.m_Info.Name, m_DBData.m_Info.Level, szExp)
			    );
			}
			else
			{
				ERRLOG1(g_Log, "CID:0x%08x DB에 캐릭터 업데이트를 할 수 없습니다. 전송에 실패했습니다.", m_dwCID);
            }
	    }
        else
        {
            // 데이터가 세팅되지 않았으니, 정상 로그아웃은 아니다.
            DBAgentPacketParse::SendAbnormalLogout(
                m_dwUID, m_dwCID, m_dwSessionID, 0, m_lpGameClientDispatch);        
        }
    }
	else
	{
        ERRLOG2(g_Log, "CID:0x%08x/AgentSession:0x%p/ DBUpdate failed.",
            m_dwCID, lpDBAgentDispatch);
	}

    SOCKADDR_IN remoteAddr;
    if (0 != m_lpGameClientDispatch)
    {
        remoteAddr = m_lpGameClientDispatch->GetRemoteAddr().get_addr_in();
    }
    else
    {
        memset(&remoteAddr, 0, sizeof(SOCKADDR_IN));
    }

    // 게임로그에 캐릭터 로그아웃 남기기
	GAMELOG::LogCharLoginOut(m_dwUID, this, &remoteAddr, lpCharacterInfo, nTotalSize, 
        lpPktDBUpdate->m_usUpdate, GAMELOG::CMD::CHAR_LOGOUT, usError);

	// 로그 서버에 로그아웃 보내기.
	// SendLogPacket::CharLogout(*this);

	LOG_INOUT(DETLOG4(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p 다음 유저의 로그아웃을 처리합니다. 다음 유저를 제거합니다.", 
		m_dwUID, m_dwCID, this, m_lpGameClientDispatch));

	return CCreatureManager::GetInstance().DeleteCreature(m_dwCID);
}



void CheckDuplicatedItem(CCharacter& character)
{
    using namespace Item;

    CItemOwnerInfo itemOwnerInfo(
        character.GetCharacterName(), character.GetUID(), character.GetCID());

    CItemOwnerInfo* lpDuplicateOwner = 0;

    CItemFactory& itemFactory = Item::CItemFactory::GetInstance();
    
    // first : 소유자 정보 / second : 복사 아이템 Qty
    typedef std::pair<CItemOwnerInfo*, unsigned long>   DuplicateInfo;

    typedef std::multimap<unsigned __int64, DuplicateInfo, std::less<unsigned __int64>,
        boost::fast_pool_allocator<std::pair<unsigned __int64, DuplicateInfo> > > DuplicatedMap;

    DuplicatedMap duplicatedItemMap;

    // 복사 데이터가 있으면 DB중계로 송신하기 위해서..
    GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, 
        CDBAgentDispatch::GetDispatchTable());

    // 아이템 컨테이너들을 배열에 넣는다.
    const int MAX_CONTAINER_SET = 7;
    CItemContainer* containerSet[MAX_CONTAINER_SET] = 
    {
        &character.GetInventory(),
        &character.GetEquipments(),
        &character.GetExtra(),
        &character.GetExchange(),
        &character.GetDeposit(),
        &character.GetStall(),
        &character.GetTempInven(),
    };

    CItemContainer** lppContainerPos = containerSet;
    CItemContainer** lppContainerEnd = containerSet + MAX_CONTAINER_SET;

    for(; lppContainerPos != lppContainerEnd; ++lppContainerPos)
    {
        CItemContainer& itemContainer = **lppContainerPos;

        CItemContainer::iterator pos = itemContainer.begin();
        CItemContainer::iterator end = itemContainer.end();
        
        for(; pos != end; ++pos)
        {
            CItem* lpItem = *pos;

            if (0 != lpItem)
            {
                unsigned __int64 dwItemSerial = lpItem->GetUID();

                lpDuplicateOwner = itemFactory.AddItemMap(dwItemSerial, itemOwnerInfo);
                if (0 != lpDuplicateOwner)
                {
                    // 복사 발견. 일단 나와 이녀석의 시리얼 및 정보를 리스트 등으로 기억해두었다가
                    // 개수를 세어서 나중에 한번에 보낸다.
                    
                    // 내 아이템 정보   : dwItemSerial, itemOwnerInfo
                    // 상대 아이템 정보 : dwItemSerial, lpDuplicateOwner

                    std::pair<DuplicatedMap::iterator, DuplicatedMap::iterator> result = 
                        duplicatedItemMap.equal_range(dwItemSerial);

                    bool bAddedQty = false;
                    bool bAddedDuplicateOwner = false;

                    for(;result.first != result.second; ++result.first)
                    {
                        unsigned __int64 dwItemUID = result.first->first;

                        // first : 소유자 정보 / second : 복사 아이템 Qty
                        DuplicateInfo&  duplicateInfo = result.first->second;                        
                        CItemOwnerInfo& duplicateOwnerInfo = *duplicateInfo.first;

                        if (duplicateOwnerInfo.GetCID() == itemOwnerInfo.GetCID())
                        {
                            // 소유자가 같은 아이템이다. Qty를 증가시켜준다.
                            // Qty는 복제된 아이템을 '내가' 몇개나 가지고 있는지를 나타낸다.
                            ++duplicateInfo.second;
                            bAddedQty = true;
                        }                       
                        else if (duplicateOwnerInfo.GetCID() == lpDuplicateOwner->GetCID())
                        {
                            // 이전에 가지고 있던 사람이 내가 아니라 다른 사람이다.                            
                            bAddedDuplicateOwner = true;
                        }
                    }
                    
                    if (!bAddedQty)
                    {
                        // 숫자가 추가되지 않았으면, 항목에 새로 추가.
                        duplicatedItemMap.insert(result.second, DuplicatedMap::value_type(dwItemSerial, 
                            DuplicateInfo(&itemOwnerInfo, (itemOwnerInfo.GetCID() == lpDuplicateOwner->GetCID()) ? 2 : 1)));
                    }

                    if (!bAddedDuplicateOwner && lpDuplicateOwner->GetCID() != itemOwnerInfo.GetCID())
                    {
                        duplicatedItemMap.insert(result.second, 
                            DuplicatedMap::value_type(dwItemSerial, DuplicateInfo(lpDuplicateOwner, 1)));
                    }
                }
            }
        }
    }

    DuplicatedMap::iterator pos = duplicatedItemMap.begin();
    DuplicatedMap::iterator end = duplicatedItemMap.end();

    if (0 != lpDBAgentDispatch)
    {
        // 전송
        for(; pos != end; ++pos)
        {
            SendItemDuplicatedLog(lpDBAgentDispatch->GetSendStream(), 
                pos->first, *pos->second.first, pos->second.second);
        }
    }
    else
    {
        // 로그를 남긴다.
        for(; pos != end; ++pos)
        {
            DuplicateInfo& dupInfo = pos->second;

            ERRLOG5(g_Log, "UID:%10u / CID:%10u / CharName:%s / ItemSerial:0x%I64X / Qty:%u / 아이템 복사가 발견되었습니다",
                dupInfo.first->GetUID(), dupInfo.first->GetCID(), dupInfo.first->GetName(), pos->first, dupInfo.second);
        }
    }
}


void SendItemDuplicatedLog(CSendStream& SendStream, unsigned __int64 dwItemSerial,
                           Item::CItemOwnerInfo& itemOwnerInfo, unsigned long dwItemQty)
{
    PktItemDuplicated* lpPktItemDuplicated = 
        reinterpret_cast<PktItemDuplicated*>(SendStream.GetBuffer(sizeof(PktItemDuplicated)));

    if (0 != lpPktItemDuplicated)
    {
        lpPktItemDuplicated->m_cLogCmd = PktServerLog::ITEM_DUPLICATED_LOG;

        strncpy(lpPktItemDuplicated->m_szName, itemOwnerInfo.GetName(), PktItemDuplicated::MAX_NAME - 1);
        lpPktItemDuplicated->m_szName[PktItemDuplicated::MAX_NAME - 1] = 0;

        lpPktItemDuplicated->m_dwItemSerial = dwItemSerial;
        lpPktItemDuplicated->m_dwUID        = itemOwnerInfo.GetUID();
        lpPktItemDuplicated->m_dwCID        = itemOwnerInfo.GetCID();
        lpPktItemDuplicated->m_dwQty        = dwItemQty;

        SendStream.WrapHeader(sizeof(PktItemDuplicated), CmdServerLog, 0, 0);
    }
}

void RealmSkill::RealmInchantAdd(CCharacter* lpCharacter)
{
	// edith 2010.01.02 공헌 훈장의 개념이 바뀌면서 공헌훈장 버프를 제외한다.
	return;

	// edith 2008.05.28 공헌 훈장별 버프 설정
	// 공헌 훈장 포인트 세팅 //
	unsigned char cLevel[2][6] =
	{
		0, 1, 1, 2, 2, 3,
		0, 0, 1, 1, 2, 2
	};

	if(!lpCharacter)
	{
		return;
	}

	unsigned char cPoint = lpCharacter->GetRealmPoint();

	if(!cPoint)
	{
		return;
	}	

	// HP 효과.
	if(cLevel[0][cPoint])
	{
		Skill::CAddSpell<CRealmHPSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter, 
			Skill::SpellType::REALM_SPELL, Skill::SpellID::RealmHP, cLevel[0][cPoint], CSpell::INFINITE_DURATION))(lpCharacter);
	}

	// MP 효과.
	if(cLevel[1][cPoint])
	{
		Skill::CAddSpell<CRealmMPSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter, 
			Skill::SpellType::REALM_SPELL, Skill::SpellID::RealmMP, cLevel[1][cPoint], CSpell::INFINITE_DURATION))(lpCharacter);
	}
}

void RealmSkill::RealmInchantRemove(CCharacter* lpCharacter)
{
	if(!lpCharacter)
		return;

	// HP 효과.
	if(lpCharacter->GetSpellMgr().GetAffectedInfo().GetSpell(Skill::SpellID::RealmHP))
	{
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::RealmHP);
	}

	// MP 효과.
	if(lpCharacter->GetSpellMgr().GetAffectedInfo().GetSpell(Skill::SpellID::RealmMP))
	{
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::RealmMP);
	}
}