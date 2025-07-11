
#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Utility/Time/Pulse/Pulse.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/FieldMap/VirtualArea/BGServer/BGServerMap.h>

#include <Skill/SkillTable.h>
#include <Community/Party/Party.h>
#include <Community/Party/PartyMgr.h>

#include <Creature/NPC/NPC.h>
#include <Creature/Monster/VirtualMonsterMgr.h>
#include <Creature/Character/CharRespawnMgr.h>

#include "PatternMonster.h"
#include "MonsterShout.h"

// ----------------------------------------------------------------------------------------------
// 소환수 (어드민 명령, 룬오프 스킬에 의함)

CSummonMonster::CSummonMonster()
:	CMonster(), m_lpMaster(NULL), m_bGuard(false)
{
	m_bAdminCmdSummon = true;
}

CSummonMonster::CSummonMonster(MonsterCreateInfo& MonsterCreateInfo, CCharacter* lpMaster)
:	CMonster(MonsterCreateInfo, true), m_lpMaster(lpMaster), m_bGuard(false)
{
	if (NULL != m_lpMaster)
	{
		m_CreatureStatus.m_nLevel = m_lpMaster->GetStatus().m_nLevel;
		m_lCurrentFrame = SUMMON_TIME;
	}
}

CSummonMonster::~CSummonMonster()
{
}


void CSummonMonster::GuardMe(CAggresiveCreature* lpTarget, unsigned short wThreat)
{
	if (true == m_bGuard)
	{
		AttackCmd(lpTarget, wThreat);
	}
}

void CSummonMonster::AttackCmd(CAggresiveCreature* lpTarget, unsigned short wThreat)
{
	m_bLongRangeAttacked = true;
	m_Threat.AddToThreatList(lpTarget, wThreat);
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_CMD_ATTACK);
}

void CSummonMonster::Attacked(void)
{
	if (true == m_bGuard)
	{
		CMonster::Attacked();
	}
}

bool CSummonMonster::Attack(AtType attackType, unsigned char cDefenderNum, 
							CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenserMPHeal)
{
	if (NULL == m_lpMaster)
	{
		return CMonster::Attack(attackType, cDefenderNum, ppDefenders, cDefenderJudges, wDefenserMPHeal);
	}

	unsigned char cOffencerJudge = 0;
	unsigned short wOffencerMPHeal = 0;
	unsigned short wError = PktAtAck::NO_SERVER_ERR;

	const int   MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
	char        szBuffer[MAX_BUFFER];

	PktAtAck*		lpPktAtAck		= reinterpret_cast<PktAtAck*>(szBuffer);
	DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtAck + 1);                

	unsigned char cDefender = 0;
	unsigned char cIndex = 0;

	for (; cIndex < cDefenderNum; ++cIndex) 
	{
		if (NULL == ppDefenders[cIndex]) { continue; }
		if (0 == ppDefenders[cIndex]->GetStatus().m_nNowHP) { continue; }

		CCharacter* lpCharacter = NULL;
		CMonster* lpSummonee = NULL;

		// 타켓이 캐릭터인 경우
		if (Creature::CT_PC == Creature::GetCreatureType(ppDefenders[cDefender]->GetCID()))
		{
			lpCharacter = reinterpret_cast<CCharacter *>(ppDefenders[cDefender]);
			lpSummonee = lpCharacter->GetSummonee();
		}
		else
		{
			// 타겟이 소환수인 경우
			if (Creature::IsSummonMonster(ppDefenders[cDefender]->GetCID()))
			{
				lpCharacter = reinterpret_cast<CSummonMonster *>(ppDefenders[cDefender])->GetMaster();
			}
		}

		// TODO : 공격 방향을 설정해줍시다.
		cDefenderJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenserMPHeal[cDefender] = 0;

		// 대미지 반영
		const unsigned short wDamage = 
			ppDefenders[cDefender]->ApplyDamage(attackType, this, cOffencerJudge, cDefenderJudges[cDefender], wOffencerMPHeal, wDefenserMPHeal[cDefender], wError);

		// 공격 패킷 만들기
		lpDefenserNode[cDefender].m_dwCharID = ppDefenders[cIndex]->GetCID();
		lpDefenserNode[cDefender].m_sCurrHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		lpDefenserNode[cDefender].m_sCurrMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;
		lpDefenserNode[cDefender].m_wMaxHP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxHP;
		lpDefenserNode[cDefender].m_wMaxMP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxMP;
		lpDefenserNode[cDefender].m_wMPHeal = wDefenserMPHeal[cDefender];

		lpDefenserNode[cDefender].m_wDamage = wDamage;
		lpDefenserNode[cDefender].m_cJudge = cDefenderJudges[cDefender];

		if (NULL != lpCharacter)
		{
			if (NULL != lpSummonee)
			{
				lpSummonee->GuardMe(this, wDamage);
			}

			lpCharacter->CalculateEquipDurability((ClientConstants::Judge_Guard == cDefenderJudges[cDefender]) ? 
				AtType::GUARD : AtType::DEFENCE);

			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, ppDefenders[cDefender], 
					attackType, m_MotionInfo.m_fDirection, wDamage, cDefenderJudges[cDefender], wDefenserMPHeal[cDefender], PktBase::NO_SERVER_ERR);
			}
		}

		++cDefender;
	}

	if (0 == cDefender) { return true; }

	lpPktAtAck->m_dwCharID = m_dwCID;
	lpPktAtAck->m_AtType = attackType;

	lpPktAtAck->m_wHP = m_CreatureStatus.m_nNowHP;
	lpPktAtAck->m_wMP = m_CreatureStatus.m_nNowMP;
	lpPktAtAck->m_wMPHeal = wOffencerMPHeal;

	lpPktAtAck->m_cJudge = cOffencerJudge;
	lpPktAtAck->m_cDefenserNum = cDefender;

	CGameClientDispatch* lpDispatch = m_lpMaster->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		if (true == SendStream.WrapCompress(
			szBuffer, sizeof(PktAtAck) + cDefender * sizeof(DefenserNode), CmdCharAttack, 0, wError) &&
			PktBase::NO_SERVER_ERR == wError)
		{
			CCell* lpCell = GetCellPos().m_lpCell;
			if (NULL != lpCell)
			{
				lpCell->SendAttackInfo(m_dwCID, attackType, cDefender, lpDefenserNode);
				return true;
			}
		}
	}

	return false;
}

bool CSummonMonster::Dead(CAggresiveCreature* pOffencer)
{
	if (NULL != m_lpMaster)
	{
		m_lpMaster->SetSummonee(NULL);
		GameClientSendPacket::SendCharSummonCmd(m_lpMaster, this, PktSummonCmd::SUC_DIE, m_dwCID);

		m_lpMaster = NULL;
	}

	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	if (NULL != m_CellPos.m_lpCell)
	{
		m_CellPos.m_lpCell->DeleteCreature(m_dwCID);
		m_CellPos.m_lpCell = NULL;
	}

	return CAggresiveCreature::Dead(pOffencer);
}

EnemyCheck::EnemyType CSummonMonster::IsEnemy(CCreature* lpTarget)
{
	if (NULL == m_lpMaster)
	{
		return CMonster::IsEnemy(lpTarget);
	}

	return m_lpMaster->IsEnemy(lpTarget);
}

bool CSummonMonster::GetMotion(unsigned long MotionID, MotionInfo &Motion)
{
	if (NULL == m_lpMaster)
	{
		return CMonster::GetMotion(MotionID, Motion);
	}
	
	if (false == CMonster::GetMotion(MotionID, Motion))
	{
		return false;
	}

	// 소환수의 이동 속도는 소환자와 맞춰준다. (수식은 적정량...)
	if (MonsterInfo::Z3D_CA_WALK == MotionID || MonsterInfo::Z3D_CA_RUN == MotionID)
	{
		Motion.m_fVelocity = ((4.9f * (1.0f + (m_lpMaster->GetCharStatus().m_nDEX - 20) * 0.03f)) * Motion.m_dwFrame) / FPS;
	}

	return true;
}


// ----------------------------------------------------------------------------------------------
// 명예의 석상

CStatue::CStatue()
:	CMonster(), m_dwDuration(0), m_wBonusTurn(0), m_cHPUpdateCount(0),
	m_dwLastAttackedTime(0), m_lpParent(NULL), m_lpLinkNPC(NULL)
{
}

CStatue::CStatue(MonsterCreateInfo& MonsterCreateInfo, CStatue* lpParent)
:	CMonster(MonsterCreateInfo), m_dwDuration(0), m_wBonusTurn(0),
	m_cHPUpdateCount(0), m_dwLastAttackedTime(0), m_lpParent(lpParent)
{
	m_nMovingPattern = FIXED;

	unsigned long dwNPCID = (MonsterCreateInfo.m_dwCID - Creature::MONSTER_BIT) | Creature::NPC_LINK_BIT;
	m_lpLinkNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
}										
										
CStatue::~CStatue()
{
}


void CStatue::GiveMileage(char cGiveType)
{
	switch (m_MonsterInfo.m_dwKID)
	{
		// 1레벨
		case MonsterInfo::STATUE_HUMAN_COMPLETE1:
		case MonsterInfo::STATUE_AKHAN_COMPLETE1:
		// 2레벨
		case MonsterInfo::STATUE_HUMAN_COMPLETE2:
		case MonsterInfo::STATUE_AKHAN_COMPLETE2:
			CCreatureManager::GetInstance().ProcessAllCharacter(CStatueInfo(this, true, cGiveType));
			break;
	}
}

bool CStatue::RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue)
{
	if (false == CAggresiveCreature::RegenHPAndMP(usAdditionalHP, usAdditionalMP, bAddDefaultRegenValue)) {
		return false;
	}

	// 다크 카나번 국가 전쟁 석상
	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		// 중계서버에 HP 업데이트 정보 보내기
		SendHPUpdate();
	}
/*
	// 신규존 생명축출기
	else if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE9)
	{
		// 중계서버에 HP 업데이트 정보 보내기
		SendHPUpdate();
	}
*/
	return true;
}

bool CStatue::Dead(CAggresiveCreature* pOffencer)//what happen when the statue is dead hayzohar
{
	if (STATE_ID_DIE == m_nCurrentState) { return false; }

	unsigned short wDamage = m_CreatureStatus.m_nNowHP;
	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	if (NULL == pOffencer)
	{
		// 석상 로딩 완료
		// AttackInfo 를 보내서 로딩중인 석상을 클라이언트들이 없애도록 해야한다.
		AtType attackType;
		DefenserNode Defenser[AtNode::MAX_DEFENDER_NUM] = {0, };

		attackType.m_wType = AtType::RIGHT_MELEE;

		Defenser[0].m_cJudge	= ClientConstants::Judge_Front;
		Defenser[0].m_dwCharID	= GetCID();
		Defenser[0].m_wMaxHP	= m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		Defenser[0].m_wMaxMP	= m_CreatureStatus.m_StatusInfo.m_nMaxMP;
		Defenser[0].m_sCurrHP	= m_CreatureStatus.m_nNowHP;
		Defenser[0].m_sCurrMP	= m_CreatureStatus.m_nNowMP;
		Defenser[0].m_wMPHeal	= 0;
		Defenser[0].m_wDamage	= wDamage;

		CCell* lpCell = m_CellPos.m_lpCell;
		if (lpCell)
		{
			lpCell->SendAttackInfo(0, attackType, 1, &Defenser[0]);
		}
	}

	if (NULL != m_CellPos.m_lpCell)
	{
		m_CellPos.m_lpCell->DeleteCreature(m_dwCID);
		m_CellPos.m_lpCell = NULL;
	}

	CStatue* lpNextStatue = NULL;

	if (NULL == pOffencer)
	{
		// 석상 로딩 완료
		switch (m_MonsterInfo.m_dwKID)
		{
			// 1레벨
			case MonsterInfo::STATUE_HUMAN_LOADING1:
			case MonsterInfo::STATUE_AKHAN_LOADING1:
			case MonsterInfo::BG_STATUE_HUMAN_LOADING1:
			case MonsterInfo::BG_STATUE_AKHAN_LOADING1:
			case MonsterInfo::POWER_STATUE_HUMAN_LOADING1:
			case MonsterInfo::POWER_STATUE_AKHAN_LOADING1:
			case MonsterInfo::INT_STATUE_HUMAN_LOADING1:
			case MonsterInfo::INT_STATUE_AKHAN_LOADING1:
			case MonsterInfo::EXP_STATUE_HUMAN_LOADING1:
			case MonsterInfo::EXP_STATUE_AKHAN_LOADING1:
			case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1:
			case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1:
			// 2레벨
			case MonsterInfo::STATUE_HUMAN_LOADING2:
			case MonsterInfo::STATUE_AKHAN_LOADING2:
			case MonsterInfo::BG_STATUE_HUMAN_LOADING2:
			case MonsterInfo::BG_STATUE_AKHAN_LOADING2:
			case MonsterInfo::POWER_STATUE_HUMAN_LOADING2:
			case MonsterInfo::POWER_STATUE_AKHAN_LOADING2:
			case MonsterInfo::INT_STATUE_HUMAN_LOADING2:
			case MonsterInfo::INT_STATUE_AKHAN_LOADING2:
			case MonsterInfo::EXP_STATUE_HUMAN_LOADING2:
			case MonsterInfo::EXP_STATUE_AKHAN_LOADING2:
			case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2:
			case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2:
			case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2:
			case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2:
				lpNextStatue = GetLinkStatue(MonsterInfo::GetCompleteStatueKID(static_cast<unsigned short>(m_MonsterInfo.m_dwKID)));
				break;

			default:
				// 이 루틴은 어드민 명령에 의해서만 수행되어야 한다. (초기화)
				CStatue* lpParent = (NULL == m_lpParent) ? this : m_lpParent;
				lpNextStatue = GetLinkStatue(static_cast<unsigned short>(lpParent->m_MonsterInfo.m_dwKID));
				break;
		}
	}
	else
	{
		// 적군에 의해 석상이 부서진 경우
		if (Creature::CT_PC == Creature::GetCreatureType(pOffencer->GetCID()))
		{
			CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(pOffencer);
			lpNextStatue = GetLinkStatue(MonsterInfo::GetLoadingStatueKID(lpCharacter->GetNation(), static_cast<unsigned short>(m_MonsterInfo.m_dwKID)));//hayzohar
		}
		else if (Creature::IsSummonMonster(pOffencer->GetCID()))
		{
			CSummonMonster* lpSummonMonster = reinterpret_cast<CSummonMonster*>(pOffencer);
			CCharacter* lpMaster = lpSummonMonster->GetMaster();
			if (lpMaster)
			{
				lpNextStatue = GetLinkStatue(MonsterInfo::GetLoadingStatueKID(lpMaster->GetNation(), static_cast<unsigned short>(m_MonsterInfo.m_dwKID)));//hayzohar
			}
		}
		else
		{
			ERRLOG2(g_Log, "CID:0x%08x 석상을 부순 자가 캐릭터가 아닙니다. 공격자CID:0x%08x", m_dwCID, pOffencer->GetCID());
			lpNextStatue = GetLinkStatue(MonsterInfo::GetDefaultStatueKID(static_cast<unsigned short>(m_MonsterInfo.m_dwKID)));
		}
	}

	if (NULL != lpNextStatue)
	{
		lpNextStatue->Respawn(m_dwLastTime);

		// 컨텐츠 : 다크 카나번 국가 전쟁
	//	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar
	//	{
			// 2008.07.17 석상전일땐 석상정보를 전달해야한다.
			// 다크 카나번 국가 전쟁시
			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				// 중계서버에 KID 및 HP 업데이트 정보 보내기
				lpNextStatue->SendKIDUpdate();
				lpNextStatue->SendHPUpdate(true);

				// 라지맵 표시 정보 보내기
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// 석상 리스폰 포인트 추가 혹은 삭제
				unsigned long dwStatueIndex = (lpNextStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
				if (lpNextStatue->EnableRespawn() && MonsterInfo::IsRaceCompleteStatueKID(lpNextStatue->GetKID()))
				{
					CCharRespawnMgr::GetInstance().AddRealmStatueRespawnPoint(dwStatueIndex, lpNextStatue->GetNation(), lpNextStatue->GetCurrentPos());
				}
				else
				{
					CCharRespawnMgr::GetInstance().DeleteRealmStatueRespawnPoint(dwStatueIndex);
				}
			}
	//	}
/*
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
		{
			// 신규존 생명 축출기
			if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE9)
			{
				// 중계서버에 KID 및 HP 업데이트 정보 보내기
				lpNextStatue->SendKIDUpdate();
				lpNextStatue->SendHPUpdate(true);

				// 라지맵 표시 정보 보내기
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// 신규존 생명 축출기 포인트 추가 혹은 삭제
				unsigned long dwStatueIndex = (lpNextStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
				if (lpNextStatue->EnableRespawn() && MonsterInfo::IsRaceCompleteStatueKID(lpNextStatue->GetKID()))
				{
					CCharRespawnMgr::GetInstance().AddRealmStatueRespawnPoint(dwStatueIndex, lpNextStatue->GetNation(), lpNextStatue->GetCurrentPos());
				}
				else
				{
					CCharRespawnMgr::GetInstance().DeleteRealmStatueRespawnPoint(dwStatueIndex);
				}
			}
		}
*/
	}

	// 방 정보 전송
	if (GetMapIndex() != 0)
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
		if (NULL != lpVirtualArea && lpVirtualArea->GetVirtualType() == VirtualArea::BGSERVERMAP)
		{
			VirtualArea::CBGServerMap* lpBGServerMap = reinterpret_cast<VirtualArea::CBGServerMap*>(lpVirtualArea);
			if (VirtualArea::STATUE == lpBGServerMap->GetMapType())
			{
				lpBGServerMap->CalculateScore();
				lpBGServerMap->SendMapInfo();

				// 로그 남기기
				CCharacter* lpOffencerChar = reinterpret_cast<CCharacter*>(pOffencer);
				if (lpOffencerChar)
				{
					char szOffencerNation[8];
					if (lpOffencerChar->GetRace() == CClass::HUMAN) strcpy(szOffencerNation, "HUMAN");
					else strcpy(szOffencerNation, "AKHAN");

					DETLOG7(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x 캐릭터(%s, %s)가 석상을 부셨습니다. (현재 Score - HM: %d  AK: %d)",
							(lpBGServerMap->GetMapIndex() & ~VirtualArea::BGSERVERMAP), lpBGServerMap->GetMapTypeName(),
							lpOffencerChar->GetCID(), lpOffencerChar->GetCharacterName(), szOffencerNation,
							lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN], lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN]);
				}
				else
				{
					DETLOG4(g_Log, "Battle Server Log :: (Channel : %d, %s) - 누군가가 석상을 부셨습니다. (현재 Score - HM: %d  AK: %d)",
							(lpBGServerMap->GetMapIndex() & ~VirtualArea::BGSERVERMAP), lpBGServerMap->GetMapTypeName(),
							lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN], lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN]);
				}
			}
		}
	}

	return CAggresiveCreature::Dead(pOffencer);
}


bool CStatue::Rest(void)
{
	if (STATE_ID_DIE == m_nCurrentState) 
	{ 
		return false; 
	}

	m_wBonusTurn = 0;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	if (NULL != m_CellPos.m_lpCell)
	{
		m_CellPos.m_lpCell->DeleteCreature(m_dwCID);
		m_CellPos.m_lpCell = NULL;
	}

	return CAggresiveCreature::Dead(NULL);
}

EnemyCheck::EnemyType CStatue::IsEnemy(CCreature* lpTarget)
{
	return CMonster::IsEnemy(lpTarget);
}

bool CStatue::CreateLinkStatue(unsigned short wKind)
{
	// 초기 상태가 중립이 아닌 석상들을 위한 처리
	// ex) 초기 상태가 휴먼 석상이라면 휴먼 석상을 만들 타이밍에 중립 석상을 만든다.
	if (m_MonsterInfo.m_dwKID == wKind)
	{
		if (0 != GetMapIndex())
		{
			wKind = MonsterInfo::BG_STATUE_NEUTRALITY1;
		}
		else
		{
			// 1레벨
			if (MonsterInfo::STATUE_NEUTRALITY1 <= wKind && MonsterInfo::STATUE_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::STATUE_NEUTRALITY1;
			}
			else if (MonsterInfo::POWER_STATUE_NEUTRALITY1 <= wKind && MonsterInfo::POWER_STATUE_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::POWER_STATUE_NEUTRALITY1;
			}
			else if (MonsterInfo::INT_STATUE_NEUTRALITY1 <= wKind && MonsterInfo::INT_STATUE_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::INT_STATUE_NEUTRALITY1;
			}
			else if (MonsterInfo::EXP_STATUE_NEUTRALITY1 <= wKind && MonsterInfo::EXP_STATUE_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::EXP_STATUE_NEUTRALITY1;
			}
			else if (MonsterInfo::WEALTH_STATUE_NEUTRALITY1 <= wKind && MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::WEALTH_STATUE_NEUTRALITY1;
			}
			else if (MonsterInfo::LIFE_EXTRACT_NEUTRALITY1 <= wKind && MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1 >= wKind)
			{
				wKind = MonsterInfo::LIFE_EXTRACT_NEUTRALITY1;
			}
			// 2레벨
			else if (MonsterInfo::STATUE_NEUTRALITY2 <= wKind && MonsterInfo::STATUE_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::STATUE_NEUTRALITY2;
			}
			else if (MonsterInfo::POWER_STATUE_NEUTRALITY2 <= wKind && MonsterInfo::POWER_STATUE_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::POWER_STATUE_NEUTRALITY2;
			}
			else if (MonsterInfo::INT_STATUE_NEUTRALITY2 <= wKind && MonsterInfo::INT_STATUE_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::INT_STATUE_NEUTRALITY2;
			}
			else if (MonsterInfo::EXP_STATUE_NEUTRALITY2 <= wKind && MonsterInfo::EXP_STATUE_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::EXP_STATUE_NEUTRALITY2;
			}
			else if (MonsterInfo::WEALTH_STATUE_NEUTRALITY2 <= wKind && MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::WEALTH_STATUE_NEUTRALITY2;
			}
			else if (MonsterInfo::LIFE_EXTRACT_NEUTRALITY2 <= wKind && MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2 >= wKind)
			{
				wKind = MonsterInfo::LIFE_EXTRACT_NEUTRALITY2;
			}
		}
	}

	CMonster::MonsterCreateInfo tempInfo;

	tempInfo.m_dwCID = (m_dwCID & ~Creature::MONSTER_KIND_BIT) | wKind;

	// 이미 존재하는지 체크
	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
		if (lpVirtualArea)
		{
			CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
			if (lpVirtualMonsterMgr && NULL != lpVirtualMonsterMgr->GetCreature(tempInfo.m_dwCID))
			{
				ERRLOG0(g_Log, "석상 생성에 실패하였습니다.");
				return false;
			}
		}
	}
	else
	{
        if (NULL != CCreatureManager::GetInstance().GetCreature(tempInfo.m_dwCID))
		{
			ERRLOG0(g_Log, "석상 생성에 실패하였습니다.");
			return false;
		}
	}

	tempInfo.m_nKID = wKind;
	tempInfo.m_Pos = m_OriginalPosition;

	CStatue* lpStatue = new CStatue(tempInfo, this);
	if (NULL == lpStatue) 
	{
		ERRLOG0(g_Log, "석상 생성에 실패하였습니다.");
		return false;
	}

	lpStatue->SetMapIndex(GetMapIndex());
	if (false == lpStatue->InitMonster(tempInfo.m_Pos, true))
	{
		ERRLOG0(g_Log, "석상 생성에 실패하였습니다.");
		return false;
	}

	// 몬스터 추가
	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
		if (lpVirtualArea)
		{
			CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
			if (lpVirtualMonsterMgr) lpVirtualMonsterMgr->AddMonster(lpStatue);
		}
	}
	else
	{
		CCreatureManager::GetInstance().AddCreature(lpStatue);
	}
	
	return true;
}

CStatue* CStatue::GetLinkStatue(unsigned short wKind)
{
	CStatue* lpParent = (NULL == m_lpParent) ? this : m_lpParent;

	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
		if (lpVirtualArea)
		{
			CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
			if (lpVirtualMonsterMgr)
			{
				return reinterpret_cast<CStatue *>(lpVirtualMonsterMgr->GetMonster(
													(lpParent->GetCID() & ~Creature::MONSTER_KIND_BIT) | wKind));
			}
		}
	}
	else
	{
		return reinterpret_cast<CStatue *>(CCreatureManager::GetInstance().GetMonster(
											(lpParent->GetCID() & ~Creature::MONSTER_KIND_BIT) | wKind));
	}

	return NULL;
}

Item::CItem* CStatue::SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
									  Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError)
{
	if (NULL != m_lpLinkNPC)
	{
		unsigned short wError = 0;
		return m_lpLinkNPC->SellToCharacter(lpCustomer, wKindItem, takeType, lpRequestItem, dwPrice, wCouponID, wError);
		usError = wError;
	}

	return NULL;
}

bool CStatue::SendKIDUpdate()
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
        char* lpBuffer = SendStream.GetBuffer(sizeof(PktStatueCmd) + sizeof(StatueInfo));
		if (NULL != lpBuffer)
		{
			PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd *>(lpBuffer);
			StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);

			lpPktStatueCmd->m_wSubCmd	= PktStatueCmd::CMD_UPDATE_KID;
			lpPktStatueCmd->m_dwValue	= CServerSetup::GetInstance().GetServerChannel();
			lpPktStatueCmd->m_wSize		= sizeof(StatueInfo);

			lpStatueInfo->m_dwIndex		= (GetCID() & ~Creature::MONSTER_BIT) >> 16;
			lpStatueInfo->m_wKID		= GetKID();
			lpStatueInfo->m_dwLostHP	= 0;
			
			return SendStream.WrapCrypt(sizeof(PktStatueCmd) + sizeof(StatueInfo), CmdStatueCmd, 0, 0);
		}
	}

	return false;
}

bool CStatue::SendHPUpdate(bool bForce)
{
	++m_cHPUpdateCount;
	if (m_cHPUpdateCount >= MAX_HP_UPDATE_COUNT || bForce)
	{
		m_cHPUpdateCount = 0;

		GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
			CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if (0 != lpDBAgentDispatch)
		{
			CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktStatueCmd) + sizeof(StatueInfo));
			if (NULL != lpBuffer)
			{
				PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd *>(lpBuffer);
				StatueInfo* lpStatueInfo = reinterpret_cast<StatueInfo*>(lpPktStatueCmd + 1);

				lpPktStatueCmd->m_wSubCmd	= PktStatueCmd::CMD_UPDATE_LOST_HP;
				lpPktStatueCmd->m_dwValue	= CServerSetup::GetInstance().GetServerChannel();
				lpPktStatueCmd->m_wSize		= sizeof(StatueInfo);

				lpStatueInfo->m_dwIndex		= (GetCID() & ~Creature::MONSTER_BIT) >> 16;
				lpStatueInfo->m_wKID		= 0;
				lpStatueInfo->m_dwLostHP	= GetStatus().m_StatusInfo.m_nMaxHP - GetStatus().m_nNowHP;

				return SendStream.WrapCrypt(sizeof(PktStatueCmd) + sizeof(StatueInfo), CmdStatueCmd, 0, 0);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

void CStatue::SendAttacked()
{
	unsigned long dwNowTime = timeGetTime();
	if (dwNowTime - m_dwLastAttackedTime >= STATUE_ATTACKED_TIME)
	{
		m_dwLastAttackedTime = dwNowTime;
		unsigned long dwIndex = (GetCID() & ~Creature::MONSTER_BIT) >> 16;

		// 라지맵 표시 정보 보내기
		CCreatureManager::GetInstance().SendRealmStatueDisplayInfo(dwIndex, true);
	}
}

CStatueInfo::CStatueInfo(CStatue* lpStatue, bool bBonusTurn, char cGiveType)
:	m_lpStatue(lpStatue), m_bBonusTurn(bBonusTurn), m_cGiveType(cGiveType)
{
}

bool CStatueInfo::operator () (CCharacter* lpCharacter)
{
    if (NULL != lpCharacter)
    {
		if (lpCharacter->GetMapIndex() != m_lpStatue->GetMapIndex())
		{
			return false;
		}

		char cAddMileage = 3;//was 1

//		if(SERVER_ID::STONE_WAR2 == CServerSetup::GetInstance().GetServerZone())
//			cAddMileage = 5;
//		else if(SERVER_ID::STONE_WAR3 == CServerSetup::GetInstance().GetServerZone())
//			cAddMileage = 10;

		PktStatueInfo pktSI;

		pktSI.m_dwMileage = 0;

		pktSI.m_wHumanNum = CCreatureManager::GetInstance().GetCharacterNum(CClass::HUMAN);
		pktSI.m_wAkhanNum = CCreatureManager::GetInstance().GetCharacterNum(CClass::AKHAN);

		pktSI.m_wNowHP = m_lpStatue->GetStatus().m_nNowHP;
		pktSI.m_wKID = static_cast<unsigned short>(m_lpStatue->GetCID() & Creature::MONSTER_KIND_BIT);
		pktSI.m_cState = m_lpStatue->GetCurrentState();
		pktSI.m_cIndex = static_cast<unsigned char>(m_lpStatue->GetSerialNumber());

		if (true == m_bBonusTurn)
		{
			if(m_cGiveType == 0)	// 기존방식 과거 ZONE3번에서 진행했던 배틀그라운드 형식
			{
				const unsigned char cBonusTurn = 
					CCreatureManager::GetInstance().GetBonusTurn(lpCharacter->GetMapIndex());

				if (0 != lpCharacter->GetStatus().m_nNowHP && 
					0 != m_lpStatue->GetStatus().m_nNowHP && 
					static_cast<unsigned char>(lpCharacter->GetRace()) == cBonusTurn && 
					m_lpStatue->GetCurrentPos().GetDistance(lpCharacter->GetCurrentPos()) < CStatue::BONUS_DISTANCE)
				{
					lpCharacter->SetMileage(lpCharacter->GetMileage() + CStatue::BONUS_MILEAGE);
					pktSI.m_dwMileage = lpCharacter->GetMileage();
				}
			}
			else
			{
				// 해당석상이 어느종족꺼냐..
				unsigned char cBonusTurn = CClass::MAX_RACE;
				switch (m_lpStatue->GetKID())
				{
					case MonsterInfo::STATUE_HUMAN_COMPLETE1:
						cAddMileage = 1;//was 1 on all row
						cBonusTurn = CClass::HUMAN;
						break;
					case MonsterInfo::STATUE_HUMAN_COMPLETE2:
						cAddMileage = 1;
						cBonusTurn = CClass::HUMAN;
						break;
					case MonsterInfo::STATUE_AKHAN_COMPLETE1:
						cAddMileage = 1;
						cBonusTurn = CClass::AKHAN;
						break;
					case MonsterInfo::STATUE_AKHAN_COMPLETE2:
						cAddMileage = 1;
						cBonusTurn = CClass::AKHAN;
						break;
				}

				if (0 != lpCharacter->GetStatus().m_nNowHP && 
					0 != m_lpStatue->GetStatus().m_nNowHP && 
					static_cast<unsigned char>(lpCharacter->GetRace()) == cBonusTurn)
				{
					lpCharacter->SetMileage(lpCharacter->GetMileage() + cAddMileage );
					pktSI.m_dwMileage = lpCharacter->GetMileage();
				}
			}
		}

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
        if (NULL != lpDispatch)
        {
			char* szPacket = reinterpret_cast<char *>(&pktSI);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktStatueInfo), CmdStatueInfo, 0, 0))
			{
	            return lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktStatueInfo), CmdStatueInfo);
			}
        }
    }

    return false;
}


CBlessMileageInfo::CBlessMileageInfo(char cBlessType)
:	m_cBlessType(cBlessType)
{
	m_cAddMileage = 1;

	if(SERVER_ID::STONE_WAR2 == CServerSetup::GetInstance().GetServerZone())
		m_cAddMileage = 5;
	else if(SERVER_ID::STONE_WAR3 == CServerSetup::GetInstance().GetServerZone())
		m_cAddMileage = 10;
}

bool CBlessMileageInfo::operator () (CCharacter* lpCharacter)
{
    if (NULL != lpCharacter)
    {
		PktStatueInfo pktSI;

		pktSI.m_dwMileage = 0;

		pktSI.m_wHumanNum = CCreatureManager::GetInstance().GetCharacterNum(CClass::HUMAN);
		pktSI.m_wAkhanNum = CCreatureManager::GetInstance().GetCharacterNum(CClass::AKHAN);

		pktSI.m_wNowHP = 0;
		pktSI.m_wKID = 0;
		pktSI.m_cState = 0;
		pktSI.m_cIndex = 0;

		if (0 != lpCharacter->GetStatus().m_nNowHP) 
		{
			lpCharacter->SetMileage(lpCharacter->GetMileage() + m_cAddMileage);
			pktSI.m_dwMileage = lpCharacter->GetMileage();
		}

		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
        if (NULL != lpDispatch)
        {
			char* szPacket = reinterpret_cast<char *>(&pktSI);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktStatueInfo), CmdStatueInfo, 0, 0))
			{
	            return lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktStatueInfo), CmdStatueInfo);
			}
        }
    }

    return false;
}


// -------------------------------------------------------------------------------------
// 오브젝트 몬스터 (꽃, 나무, 버섯류)

CGatherMonster::CGatherMonster()
:	CMonster()
{
}

CGatherMonster::CGatherMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
: CMonster(MonsterCreate, bAdminCmdSummon)
{
}

CGatherMonster::~CGatherMonster()
{
}

// -------------------------------------------------------------------------------------
// 오브젝트 몬스터 (꽃, 나무, 버섯류)

CObjectMonster::CObjectMonster()
:	CMonster()
{
}

CObjectMonster::CObjectMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
: CMonster(MonsterCreate, bAdminCmdSummon)
{
}

CObjectMonster::~CObjectMonster()
{
}


// -------------------------------------------------------------------------------------
// 스킬 사용 몬스터

CSkillMonster::CSkillMonster()
:	CMonster(), m_cConsumeMPCount(0), m_bCasting(false), m_dwTargetCID(0), m_cSkillPattern(0), m_nCastingCount(0)
{
	std::fill_n(m_lastCastTime, int(MAX_SKILL_PATTERN), 0);
	ZeroMemory(&m_attackType, sizeof(AtType));
}

CSkillMonster::CSkillMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CMonster(MonsterCreate, bAdminCmdSummon), m_cConsumeMPCount(0), m_bCasting(false),
	m_dwTargetCID(0), m_cSkillPattern(0), m_nCastingCount(0)
{
	std::fill_n(m_lastCastTime, int(MAX_SKILL_PATTERN), 0);
	ZeroMemory(&m_attackType, sizeof(AtType));
}

CSkillMonster::~CSkillMonster()
{
}

bool CSkillMonster::Dead(CAggresiveCreature* pOffencer)
{
	m_bCasting = false;

    const char* szCharacterName = 0;    
    if (0 != pOffencer && Creature::CT_PC == Creature::GetCreatureType(pOffencer->GetCID()))
    {
        szCharacterName = static_cast<CCharacter*>(pOffencer)->GetCharacterName();
    }    

    CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
        static_cast<unsigned short>(m_CurrentPos.m_fPointX),
        static_cast<unsigned short>(m_CurrentPos.m_fPointZ), 
        CMonsterShout::DEAD, szCharacterName, 0);

	return CMonster::Dead(pOffencer);
}

LONG CSkillMonster::SendMove(unsigned short nAniNum)
{
	// edith 2009.07.16 몬스터는 스킬 캐스팅 중에는 이동하지 마라.
//	if(m_bCasting)
//		return 0;

	return CMonster::SendMove(nAniNum);
}


bool CSkillMonster::Attack(AtType attackType, unsigned char cDefenderNum, 
						   CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal)
{
	if (m_CreatureStatus.m_nNowHP == 0) 
	{
		ERRLOG1(g_Log, "CID:0x%08x 죽은 몬스터가 공격하려고 하였습니다.", m_dwCID);
		return false;
	}

	if (cDefenderNum > AtNode::MAX_DEFENDER_NUM) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 몬스터가 공격할 때, 방어자의 숫자가 최대 방어자 숫자를 넘었습니다. 방어자수 : %d", 
			m_dwCID, cDefenderNum);
		cDefenderNum = AtNode::MAX_DEFENDER_NUM;
	}

	if (0 == (attackType.m_wType & AtType::SKILL_BIT) && 0 == cDefenderNum) 
	{
		ERRLOG0(g_Log, "스킬이 아닌 일반 공격은, 반드시 타겟이 있을 경우에만 서버로 보내야 합니다.");
		return false;
	}


	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL == pSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
			return false;
		}
	}

	unsigned char cOffencerJudge = 0;
	unsigned short wOffencerMPHeal = 0;
	unsigned short wError = PktAtAck::NO_SERVER_ERR;

	m_cConsumeMPCount = std::min(cDefenderNum, unsigned char(AtNode::MAX_MONSTER_DEFENDER_NUM));

	unsigned char cDefender = 0;
	unsigned char cIndex = 0;

	// MON_TODO : by Vincent - 2004 : 2 : 25
	DefenserNode Defenser[AtNode::MAX_DEFENDER_NUM];
	// ------------

    // 몬스터 외치기 때문에 넣음.
    unsigned int nCriticalCount = 0;
    CCharacter*  lpDefendShoutCharacter = 0;



	for (; cIndex < cDefenderNum; ++cIndex) 
	{
		// MP 소모 타이밍까지의 카운트 (범위 마법은 한 번만 MP 소모)
		--m_cConsumeMPCount;

		if (NULL == ppDefenders[cIndex])
		{ 
			continue; 
		}
		if (0 == ppDefenders[cIndex]->GetStatus().m_nNowHP)
		{ 
			continue; 
		}

		// TODO : 공격 방향을 설정해줍시다.
		cDefenderJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenderMPHeal[cDefender] = 0;

		const unsigned short nPrevHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nPrevMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;

		const unsigned short wPrevAttackerHP = m_CreatureStatus.m_nNowHP;

		// 대미지 반영
		const unsigned short wDamage = ppDefenders[cIndex]->ApplyDamage(attackType, this, 
            cOffencerJudge, cDefenderJudges[cDefender], wOffencerMPHeal, wDefenderMPHeal[cDefender], wError);

        if (cDefenderJudges[cDefender] == ClientConstants::Judge_Critical)
        {
            ++nCriticalCount;
        }

		const unsigned short nNowHP	= ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nNowMP	= ppDefenders[cIndex]->GetStatus().m_nNowMP;

		// 스킬에 의한 자살 방지
		if (0 == m_CreatureStatus.m_nNowHP)
		{
			m_CreatureStatus.m_nNowHP = wPrevAttackerHP;
			wError = PktAtAck::FAIL_SUICIDE;
			break;
		}
		else
		{
			if (Creature::CT_PC == Creature::GetCreatureType(ppDefenders[cIndex]->GetCID()))
			{
                // 방어자가 캐릭터인 경우
				CCharacter* lpDefendCharacter = (CCharacter *)ppDefenders[cIndex];

                if (0 == lpDefendShoutCharacter || 0 == (rand() % 3))
                {
                    lpDefendShoutCharacter = lpDefendCharacter;
                }

				CMonster* lpSummonee = lpDefendCharacter->GetSummonee();
				if (NULL != lpSummonee)
				{
					lpSummonee->GuardMe(this, wDamage);
				}

				lpDefendCharacter->CalculateEquipDurability((ClientConstants::Judge_Guard == cDefenderJudges[cDefender]) ? 
					AtType::GUARD : AtType::DEFENCE);

				CGameClientDispatch* lpDispatch = lpDefendCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, lpDefendCharacter, 
						attackType, m_MotionInfo.m_fDirection, wDamage, cDefenderJudges[cDefender],
						wDefenderMPHeal[cDefender], PktBase::NO_SERVER_ERR);
				}
			}
		}

		Defenser[cDefender].m_cJudge	= cDefenderJudges[cDefender];
		Defenser[cDefender].m_dwCharID	= ppDefenders[cDefender]->GetCID();
		Defenser[cDefender].m_wMaxHP	= ppDefenders[cDefender]->GetStatus().m_StatusInfo.m_nMaxHP;
		Defenser[cDefender].m_wMaxMP	= ppDefenders[cDefender]->GetStatus().m_StatusInfo.m_nMaxMP;
		Defenser[cDefender].m_sCurrHP	= ppDefenders[cDefender]->GetStatus().m_nNowHP;
		Defenser[cDefender].m_sCurrMP	= ppDefenders[cDefender]->GetStatus().m_nNowMP;
		Defenser[cDefender].m_wMPHeal	= wDefenderMPHeal[cDefender];
		Defenser[cDefender].m_wDamage	= wDamage;

		++cDefender;
	}
    
    // 외치기 검사
    CMonsterShout::Behavior eBehavior = CMonsterShout::NORMAL_ATTACK;
    unsigned short usShoutSkill_ID = 0;

	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		if (0 == cDefender) 
		{ 
			Skill::CFunctions::ConsumeMP(attackType, this, 0);
		}

        eBehavior = CMonsterShout::SKILL_ATTACK;
        usShoutSkill_ID = attackType.m_wType;
	}
    else if (0 < nCriticalCount)
    {
        eBehavior = CMonsterShout::CRITICAL_ATTACK;
    }

    const char* szName = 0;
    if (0 != lpDefendShoutCharacter)
    {
        szName = lpDefendShoutCharacter->GetCharacterName();
    }

    // 외치기 검색해서 외침. 이름은, 공격하는 경우이므로 맞는 넘 아무넘이나 한넘 잡아서 그넘 이름을 준다.
    CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
        static_cast<unsigned short>(m_CurrentPos.m_fPointX),
        static_cast<unsigned short>(m_CurrentPos.m_fPointZ), eBehavior, 
        szName, usShoutSkill_ID);

	// MON_TODO : by Vincent - 2004 : 2 : 25
	CCell* lpCell = m_CellPos.m_lpCell;
	if (lpCell)
	{
		lpCell->SendAttackInfo(GetCID(), attackType, cDefender, &Defenser[0]);
	}
	// ------------

	return true;
}

bool CSkillMonster::UseSkill(AtType attackType,	CAggresiveCreature** ppDefenders, char cSkillPattern)
{
	if (NULL == ppDefenders[0])
	{
		return false;
	}

	// 스킬 얻기
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return false;
	}

	char cTargetType = Skill::Target::ENEMY;
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	unsigned short useMP = pThisSkill[cSkillLockCount].m_StartMP + pThisSkill[cSkillLockCount].m_LockMP + 
		(pThisSkill[cSkillLockCount].m_LevelMP + pThisSkill[cSkillLockCount].m_LockMP) * cSkillLevel ;
	unsigned long dwCurrentTime = timeGetTime();
	unsigned long dwCoolDownTime = pThisSkill[cSkillLockCount].m_dwCoolDownTime;
	if (pThisSkill->m_bIsClassSkill) dwCoolDownTime = pThisSkill[0].m_dwCoolDownTime;

	// 스킬을 사용할수 없는 상황인지 체크
	if (m_CreatureStatus.m_nNowMP < useMP || 
		dwCurrentTime - m_lastCastTime[cSkillPattern] < dwCoolDownTime ||
		Math::Random::ComplexRandom(100) > m_MonsterInfo.m_wSkillUseRate)
	{
		return false;
	}


	// 스킬 거리 체크
	unsigned char cDefenderNum = 1;
	unsigned char nDefenserJudges[AtNode::MAX_DEFENDER_NUM];
	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM];
    CAggresiveCreature* pTarget = ppDefenders[0];

	if (NULL != pTarget)
	{
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) * 
										(m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) + 
										(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ) * 
										(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ);

		float fSquareEffectDistance = (pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
										(pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

		if (fSquareTargetDistance > fSquareEffectDistance)
		{
			return false;
		}

		// 스킬 대상 설정
		if (pThisSkill->m_eTargetType == Skill::Target::FRIEND || 
			pThisSkill->m_eTargetType == Skill::Target::DEAD_FRIEND ||
			pThisSkill->m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF ||
			pThisSkill->m_eTargetType == Skill::Target::FRIEND_OBJECT ||
			pThisSkill->m_eTargetType == Skill::Target::PARTY || 
			pThisSkill->m_eTargetType == Skill::Target::SUMMON)
		{
			cTargetType = Skill::Target::FRIEND;
		}
	}


	// TODO : 공격하는 방향을 설정합시다. (현재는 무조건 정면)
	short nMaxDefenderNum = AtNode::MAX_DEFENDER_NUM;
	std::fill_n(&nDefenserJudges[0], nMaxDefenderNum, ClientConstants::Judge_Front);
	std::fill_n(&wDefenserMPHeal[0], nMaxDefenderNum, 0);

	float fDir = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
							pTarget->GetCurrentPos().m_fPointX, pTarget->GetCurrentPos().m_fPointZ);

	// 캐스팅 타입이면, 캐스트 모드로 변환하고, 나머지는 바로 사용한다.
	if (pThisSkill->m_eSkillType == Skill::Type::CAST)
	{
		m_bCasting = true;

		m_attackType = attackType;
		m_dwTargetCID = ppDefenders[0]->GetCID();
		m_cSkillPattern = cSkillPattern;
		// edith 2009.08.06 스킬 공격시 이동하지 않게 하기
		SendMove(1);
		return true;
	}

	// 스킬 사용한 시간을 저장
	m_lastCastTime[cSkillPattern] = dwCurrentTime;

	// MON_TODO : 스킬 사용 정보를 보내준다.
	CCell* lpCell = m_CellPos.m_lpCell;
	if (lpCell)
	{
		CastObject castObject;
		castObject.m_wTypeID = attackType.m_wType;
		castObject.m_DstPos.fPointX = ppDefenders[0]->GetCurrentPos().m_fPointX * 100.f;
		castObject.m_DstPos.fPointY = ppDefenders[0]->GetCurrentPos().m_fPointY * 100.f;
		castObject.m_DstPos.fPointZ = ppDefenders[0]->GetCurrentPos().m_fPointZ * 100.f;
		castObject.m_cObjectLevel = attackType.m_cSkillLockCount;
		if (pThisSkill->m_bIsClassSkill) castObject.m_cObjectLevel = 0;
		castObject.m_dwTargetID = ppDefenders[0]->GetCID();
		castObject.m_cObjectType = 6;

		const Skill::ProtoType* lpSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL == lpSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", 
				castObject.m_dwTargetID, attackType.m_wType);
			return 0;
		}
		else
		{
            switch (lpSkillProtoType->m_usSkill_ID)
			{
			case 0x8104 : castObject.m_cObjectType = 11; break;
			case 0x9104 : castObject.m_cObjectType = 31; break;
			case 0x8603 : castObject.m_cObjectType = 12; break;
			case 0x8702 : castObject.m_cObjectType = 20; break;
			case 0x9802 : castObject.m_cObjectType = 34; break;
			}
		}

		// edith 2009.08.06 몬스터가 스킬쓸때 스킬의 이팩트를 클라이언트에서 띄워주기 위한 부분.
		lpCell->SendCastObjectInfo(GetCID(), ppDefenders[0]->GetCID(), castObject);
	}
	
	// 범위 마법이라면...
	if (0 != pThisSkill[cSkillLockCount].m_fEffectExtent)
	{
		if (Skill::Target::PARTY == pThisSkill[cSkillLockCount].m_eTargetType)
		{
			// 파티 마법이라면
			CParty* lpParty = ppDefenders[0]->GetParty();
			if (NULL != lpParty)
			{
				return lpParty->Attack(attackType, ppDefenders, nDefenserJudges, this,
							pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent, cTargetType);
			}
		}
		else
		{
			// 일반 범위 마법
			return CAggresiveCreature::MultiAttack(attackType, cDefenderNum, ppDefenders, nDefenserJudges, 
						GetCurrentPos(), fDir, pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE, m_MonsterInfo.m_fAttackAngle, cTargetType);
		}
	}

	// 사용하는 스킬의 종류에 따라서 Attack 호출 함수를 달리한다.
	// Enchant 부여 마법의 경우는 한명에게만 사용한다.
	return Attack(attackType, cDefenderNum, ppDefenders, nDefenserJudges, wDefenserMPHeal);
}

bool CSkillMonster::UseCastedSkill(void)
{
	CAggresiveCreature* pTarget = reinterpret_cast<CAggresiveCreature*>(CCreatureManager::GetInstance().GetCreature(m_dwTargetCID));

	if (NULL == pTarget) return false;
	if (pTarget->GetMapIndex() != GetMapIndex()) return false;

	// 스킬 얻기
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(m_attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, m_attackType.m_wType);
		return false;
	}

	char cTargetType = Skill::Target::ENEMY;
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / 6;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % 6;

	// 100% 확률로 스킬을 사용한 것이기 때문에.. 캐스트 타임을 셋팅하고, 마법 사용 확률은 검사하지 않는다.
	m_lastCastTime[m_cSkillPattern] = timeGetTime();

	unsigned char cDefenderNum = 1;
	unsigned char nDefenserJudges[AtNode::MAX_DEFENDER_NUM];
	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM];
	CAggresiveCreature* ppDefenders[AtNode::MAX_DEFENDER_NUM] = {0, };
	ppDefenders[0] = pTarget;
	
	// 스킬 거리 체크
	if (NULL != pTarget)
	{
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) * 
										(m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) + 
										(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ) * 
										(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ);

		float fSquareEffectDistance = (pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
										(pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

		if (fSquareTargetDistance > fSquareEffectDistance)
		{
			return false;
		}

		// 스킬 대상 설정
		if (pThisSkill->m_eTargetType == Skill::Target::FRIEND || 
			pThisSkill->m_eTargetType == Skill::Target::DEAD_FRIEND ||
			pThisSkill->m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF ||
			pThisSkill->m_eTargetType == Skill::Target::FRIEND_OBJECT ||
			pThisSkill->m_eTargetType == Skill::Target::PARTY || 
			pThisSkill->m_eTargetType == Skill::Target::SUMMON)
		{
			cTargetType = Skill::Target::FRIEND;
		}
	}

    
	// TODO : 공격하는 방향을 설정합시다. (현재는 무조건 정면)
	short nMaxDefenderNum = AtNode::MAX_DEFENDER_NUM;
	std::fill_n(&nDefenserJudges[0], nMaxDefenderNum, ClientConstants::Judge_Front);
	std::fill_n(&wDefenserMPHeal[0], nMaxDefenderNum, 0);

	float fDir = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
							pTarget->GetCurrentPos().m_fPointX, pTarget->GetCurrentPos().m_fPointZ);

	// MON_TODO : 스킬 사용 정보를 보내준다.
	CCell* lpCell = m_CellPos.m_lpCell;
	if (lpCell)
	{
		CastObject castObject;
		castObject.m_wTypeID = m_attackType.m_wType;
		castObject.m_DstPos.fPointX = pTarget->GetCurrentPos().m_fPointX * 100.f;
		castObject.m_DstPos.fPointY = pTarget->GetCurrentPos().m_fPointY * 100.f;
		castObject.m_DstPos.fPointZ = pTarget->GetCurrentPos().m_fPointZ * 100.f;
//		castObject.m_cObjectType = 0;
//		castObject.m_cObjectLevel = cSkillLockCount;
//		castObject.m_dwTargetID = pTarget->GetCID();
//		castObject.m_cObjectType = 7;
		castObject.m_cObjectType = 6;
		castObject.m_cObjectLevel = m_attackType.m_cSkillLockCount;
		if (pThisSkill->m_bIsClassSkill) castObject.m_cObjectLevel = 0;
		castObject.m_dwTargetID = pTarget->GetCID();

		lpCell->SendCastObjectInfo(GetCID(), pTarget->GetCID(), castObject);
	}

	// 범위 마법이라면...
	if (0 != pThisSkill[cSkillLockCount].m_fEffectExtent)
	{
		if (Skill::Target::PARTY == pThisSkill[cSkillLockCount].m_eTargetType)
		{
			// 파티 마법이라면
			CParty* lpParty = ppDefenders[0]->GetParty();
			if (NULL != lpParty)
			{
				return lpParty->Attack(m_attackType, ppDefenders, nDefenserJudges, this,
							pThisSkill[m_attackType.m_cSkillLockCount].m_fEffectExtent, cTargetType);
			}
		}
		else
		{
			// 일반 범위 마법
			return CAggresiveCreature::MultiAttack(m_attackType, cDefenderNum, ppDefenders, nDefenserJudges, 
						GetCurrentPos(), fDir, pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE, m_MonsterInfo.m_fAttackAngle, cTargetType);
		}
	}

	// 사용하는 스킬의 종류에 따라서 Attack 호출 함수를 달리한다.
	// Enchant 부여 마법의 경우는 한명에게만 사용한다.
	return Attack(m_attackType, cDefenderNum, ppDefenders, nDefenserJudges, wDefenserMPHeal);
}

float CSkillMonster::CalculateDistance(CAggresiveCreature* pTarget)
{
	if (NULL == pTarget) return 0;

	float fSquareTargetDistance = (m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) * 
									(m_CurrentPos.m_fPointX - pTarget->GetCurrentPos().m_fPointX) + 
									(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ) * 
									(m_CurrentPos.m_fPointZ - pTarget->GetCurrentPos().m_fPointZ);

	return sqrtf(fSquareTargetDistance);
}




struct CompareLevel
{
	typedef std::vector<CAggresiveCreature*>	TargetVector;
	bool operator () (const TargetVector::value_type& lhs, const TargetVector::value_type& rhs) const
	{
		return lhs->GetStatus().m_nLevel > rhs->GetStatus().m_nLevel;
	}
};

CAggresiveCreature*	CSkillMonster::FindHighestLVMember()
{
	if (NULL == m_pParty) return NULL;

	// 파티 멤버를 되져본다.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// 해당 몬스터를 찾아서 저장해둘 벡터
	std::vector<CMonster* > targetVector;
	targetVector.clear();
	targetVector.reserve(10);
	
	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// 멤버 몬스터 얻어온기
		CMonster* lpMonster = lpParty->GetMember(i);

		// 거리 체크
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
									  (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
									  (m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
									  (m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

		if (fSquareTargetDistance <= CCell::CELL_DISTANCE)
		{
			targetVector.push_back(lpMonster);
		}
	}

	// 한명도 없다면...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// 해당 패턴에 선택된 몬스터 중에서 최고 레벨의 몬스터를 리턴한다.
	std::sort(targetVector.begin(), targetVector.end(), CompareLevel());
	return targetVector[0];
}

CAggresiveCreature*	CSkillMonster::FindHighestLVMember(const AtType& attackType)
{
	if (NULL == m_pParty)
	{
		if (true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType)) 
		{
			return NULL;
		}

		return this;
	}

	// 스킬 얻기
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// 파티 멤버를 되져본다.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// 스킬 유효 거리
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
									(pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	// 해당 몬스터를 찾아서 저장해둘 벡터
	std::vector<CMonster* > targetVector;
	targetVector.clear();

	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// 멤버 몬스터 얻어온기
		CMonster* lpMonster = lpParty->GetMember(i);

		// NULL 이거나 이미 해당 스킬이 부여된 상태라면 다음으로..
		if (NULL == lpMonster || true == lpMonster->GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
		{
			continue;
		}

		// 거리 체크
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
										(m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
										(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
										(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

		if (fSquareTargetDistance <= fSquareEffectDistance)
		{
			targetVector.push_back(lpMonster);
		}
	}

	// 한명도 없다면...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// 해당 패턴에 선택된 몬스터 중에서 최고 레벨의 몬스터를 리턴한다.
	std::sort(targetVector.begin(), targetVector.end(), CompareLevel());
	return targetVector[0];
}


struct CompareHP
{
	typedef std::vector<CAggresiveCreature*>	TargetVector;
	bool operator () (const TargetVector::value_type& lhs, const TargetVector::value_type& rhs) const
	{
		return lhs->GetStatus().m_nNowHP < rhs->GetStatus().m_nNowHP;
	}
};

CAggresiveCreature* CSkillMonster::FindLowestHPMember(const AtType& attackType, bool bRegin, float fRemainHP)
{
	if (NULL == m_pParty)
	{
		if (GetStatus().m_StatusInfo.m_nMaxHP * fRemainHP >= GetStatus().m_nNowHP) return this;
		return NULL;
	}

	// 스킬 얻기
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// 파티 멤버를 되져본다.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// 스킬 유효 거리
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
									(pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	// 해당 몬스터를 찾아서 저장해둘 벡터
	std::vector<CMonster* > targetVector;
	targetVector.clear();

	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// 멤버 몬스터 얻어온기
		CMonster* lpMonster = lpParty->GetMember(i);

		// NULL 이면 다음으로..
		if (NULL == lpMonster)
		{
			continue;
		}

		// HP가 체크
		if (lpMonster->GetStatus().m_StatusInfo.m_nMaxHP * fRemainHP >= lpMonster->GetStatus().m_nNowHP)
		{
			if (bRegin)
			{
				// 거리 체크
				float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
												(m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
												(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
												(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

				if (fSquareTargetDistance > fSquareEffectDistance) continue;
			}

			targetVector.push_back(lpMonster);
		}
	}

	// 한명도 없다면...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// 해당 패턴에 선택된 몬스터 중에서 최저 HP 별로 sort 한다.
	std::sort(targetVector.begin(), targetVector.end(), CompareHP());
	return targetVector[0];
}

CAggresiveCreature* CSkillMonster::FindEnemy(const AtType& attackType, unsigned char cType1, unsigned char cType2)
{
	// 스킬 얻기
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// 적의 CID Set 을 만든다.
	CMonsterParty::PartyTargetSet targetSet;
	targetSet.clear();

	if (m_pParty)
	{
		// 파티 멤버의 적을 Set 에 넣는다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);
		targetSet = lpParty->GetPartyTargetSet();
	}

	// 원래 타겟 또한 적의 Set 에 넣는다.
	if (m_lpTarget) targetSet.insert(m_lpTarget->GetCID());

	// 스킬 유효 거리
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
								  (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	CMonsterParty::PartyTargetSet::iterator pos = targetSet.begin();
	CMonsterParty::PartyTargetSet::iterator end = targetSet.end();
	CAggresiveCreature* lpTarget = NULL;

	while (pos != end)
	{
		// 스킬 유효 범위 내의 적을 찾는다.
		CAggresiveCreature* lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(*pos);
		if (NULL == lpCreature || EnemyCheck::EC_ENEMY != IsEnemy(lpCreature))
		{
			++pos;
			continue;
		}

		if (NULL == lpTarget)
		{
			lpTarget = lpCreature;
			++pos;
			continue;
		}

		// 패턴 타입에 맞는 타겟을 찾는다.
		switch (cType1)
		{
			case TYPE_LV:
			{
				if (TYPE_HIGHEST == cType2 && lpTarget->GetStatus().m_nLevel < lpCreature->GetStatus().m_nLevel)		lpTarget = lpCreature;
				else if (TYPE_LOWEST == cType2 && lpTarget->GetStatus().m_nLevel > lpCreature->GetStatus().m_nLevel)	lpTarget = lpCreature;
			}
			break;

			case TYPE_HP:
			{
				if (TYPE_HIGHEST == cType2 && lpTarget->GetStatus().m_nNowHP < lpCreature->GetStatus().m_nNowHP)		lpTarget = lpCreature;
				else if (TYPE_LOWEST == cType2 && lpTarget->GetStatus().m_nNowHP > lpCreature->GetStatus().m_nNowHP)	lpTarget = lpCreature;
			}
			break;
		}

		++pos;
	}

	return lpTarget;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (디펜더)

CDefenderMonster::CDefenderMonster()
:	CSkillMonster()
{
}

CDefenderMonster::CDefenderMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
}

CDefenderMonster::~CDefenderMonster()
{
}

bool CDefenderMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}

	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, MAX_PATTERN = 4 };
	int nSelectPattern = FIRST_PATTERN;
	
	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };
	
	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;
	
	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
            {
				// 조건		대상	행동
				// B		B		D	: 자신에게 가장 위협적인 적이 자신으로부터 5m 이상 멀어질 때
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				if (CalculateDistance(m_lpTarget) < 5.f)
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// C		C		B	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (워리어)

CWarriorMonster::CWarriorMonster()
:	CSkillMonster()
{
}

CWarriorMonster::CWarriorMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
}

CWarriorMonster::~CWarriorMonster()
{
}

bool CWarriorMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}

	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, MAX_PATTERN = 4 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 조건		대상	행동
				// B		B		D	: 자신에게 가장 위협적인 적이 자신으로부터 5m 이상 멀어질 때
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				if (CalculateDistance(m_lpTarget) < 5.f)
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// C		C		B	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// C		B		A	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때
				//						: 자신에게 가장 위협적인 적
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (어콜라이트)

CAcolyteMonster::CAcolyteMonster()
:	CSkillMonster()
{
	m_wDefaultSearchRange = ACOLYTE_SEARCH_RANGE;
}

CAcolyteMonster::CAcolyteMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
	m_wDefaultSearchRange = ACOLYTE_SEARCH_RANGE;
}

CAcolyteMonster::~CAcolyteMonster()
{
}

bool CAcolyteMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}


	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, MAX_PATTERN = 4 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 조건		대상	행동
				// C		C		B	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// D		C		D	: 그룹에 속해 있는 아군중 하나라도 HP가 70% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true, 0.7f);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// A		J		C	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 부여 스킬이 걸려있지 않은 아군 중 가장 높은 LV
				//						: C 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];

				// 해당 기술의 거리 안에 있는 최고 레벨의 해당 몬스터를 리턴해준다.
				// 해당 부여 스킬의 스펠이 이미 걸려있는 녀석을 제외하고 찾아서 넘겨준다.
				ppAggresiveCreature[0]	= FindHighestLVMember(attackType);

				// 한명도 없다면.. 다음 패턴으로
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (메이지)
CMageMonster::CMageMonster()
:	CSkillMonster()
{
	m_wDefaultSearchRange = MAGE_SEARCH_RANGE;
}

CMageMonster::CMageMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
	m_wDefaultSearchRange = MAGE_SEARCH_RANGE;
}

CMageMonster::~CMageMonster()
{
}

bool CMageMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}
	
	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}


	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, MAX_PATTERN = 4 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 조건		대상	행동
				// F		A		B	: 자신의 MP가 30% 이하가 되었을 때			
				//						: 자신에게
				//						: B 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL] ||
					m_CreatureStatus.m_nNowMP > m_CreatureStatus.m_StatusInfo.m_nMaxMP * 0.3f)
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
            break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// A		F		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 가장 낮은 HP를 가지고 있는 적 
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];

				ppAggresiveCreature[0] = FindEnemy(attackType, TYPE_HP, TYPE_LOWEST);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// A		F		D	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 가장 낮은 HP를 가지고 있는 적 
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				ppAggresiveCreature[0] = FindEnemy(attackType, TYPE_HP, TYPE_LOWEST);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (보스)

CBossMonster::CBossMonster()
:	CSkillMonster()
{
	m_wDefaultSearchRange = BOSS_SEARCH_RANGE;
}

CBossMonster::CBossMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
	m_wDefaultSearchRange = BOSS_SEARCH_RANGE;
}

CBossMonster::~CBossMonster()
{
}

bool CBossMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}


	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, FIFTH_PATTERN=4, MAX_PATTERN = 5 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 전투가 시작되면 Boss 의 챈트 스킬을 발동
				
				// 스킬이 없거나, 이미 챈트 스킬을 발동한 상태면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cChantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = BOSS_CHANT_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// A		F		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 가장 낮은 HP를 가지고 있는 적 
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];

				ppAggresiveCreature[0] = FindEnemy(attackType, TYPE_HP, TYPE_LOWEST);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// D		A		B	: 그룹에 속해 있는 아군중 하나라도 HP가 70% 이하가 되었을 때			
				//						: 자신에게
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true, 0.7f);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = this;
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// B		B		C	: 자신에게 가장 위협적인 적이 자신으로부터 5m 이상 멀어질 때
				//						: 자신에게 가장 위협적인 적에게
				//						: C 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];

				if (CalculateDistance(m_lpTarget) < 5.f)
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case FIFTH_PATTERN:
			{
				// 조건		대상	행동
				// C		B		D	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 자신에게 가장 위협적인 적
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}

// -------------------------------------------------------------------------------------
// 몬스터 타입 (치프)

CChiefMonster::CChiefMonster()
: CSkillMonster()
{
	m_wDefaultSearchRange = CHIEF_SEARCH_RANGE;
}

CChiefMonster::CChiefMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
: CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
	m_wDefaultSearchRange = CHIEF_SEARCH_RANGE;
}

CChiefMonster::~CChiefMonster()
{
}

bool CChiefMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}


	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, FIFTH_PATTERN=4, MAX_PATTERN = 5 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 전투가 시작되면 Boss 의 챈트 스킬을 발동

				// 스킬이 없거나, 이미 챈트 스킬을 발동한 상태면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cChantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = BOSS_CHANT_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// C		C		B	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case FIFTH_PATTERN:
			{
				// 조건		대상	행동
				// A		B		D	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}

// -------------------------------------------------------------------------------------
// 몬스터 타입 (네임드)

CNamedMonster::CNamedMonster()
:	CSkillMonster()
{
	m_wDefaultSearchRange = NAMED_SEARCH_RANGE;
}

CNamedMonster::CNamedMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
:	CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
	m_RespawnPosition = MonsterCreate.m_Pos;
	
	if (!bAdminCmdSummon && m_wRespawnArea > 0)
	{
		MonsterCreate.m_Pos.m_fPointX += static_cast<float>(Math::Random::SimpleRandom(GetTickCount(), m_wRespawnArea*2) - m_wRespawnArea);
		MonsterCreate.m_Pos.m_fPointZ += static_cast<float>(Math::Random::SimpleRandom(GetTickCount(), m_wRespawnArea*2) - m_wRespawnArea);
	}
	m_OriginalPosition = MonsterCreate.m_Pos;

	m_wDefaultSearchRange = NAMED_SEARCH_RANGE;
}

CNamedMonster::~CNamedMonster()
{
}

void CNamedMonster::Respawn(unsigned long dwTick)
{
	m_OriginalPosition = m_RespawnPosition;
	if(m_wRespawnArea > 0)
	{
		float randomValue = static_cast<float>(Math::Random::ComplexRandom(m_wRespawnArea*2));
		m_OriginalPosition.m_fPointX += randomValue - m_wRespawnArea;
		randomValue = static_cast<float>(Math::Random::ComplexRandom(m_wRespawnArea*2));
		m_OriginalPosition.m_fPointZ += randomValue - m_wRespawnArea;
	}

	InitMonster(m_OriginalPosition);
	m_nCurrentState = STATE_ID_NORMAL;

    // 리스폰시 외치기.
    CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
        static_cast<unsigned short>(m_CurrentPos.m_fPointX),
        static_cast<unsigned short>(m_CurrentPos.m_fPointZ), 
        CMonsterShout::RESPAWN, 0, 0);

	// 파티에 추가
	CParty* lpParty = CPartyMgr::GetInstance().GetParty(GetPID());
	if (NULL != lpParty)
	{
		SetParty(lpParty);
		lpParty->Join(GetCID(), 0, NULL, GetMapIndex());
	}
}

bool CNamedMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel) return false;

	
	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, FIFTH_PATTERN=4, MAX_PATTERN = 5 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 전투가 시작되면 Boss 의 챈트 스킬을 발동

				// 스킬이 없거나, 이미 챈트 스킬을 발동한 상태면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cChantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = BOSS_CHANT_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[BOSS_CHANT_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// C		B		A	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 자신에게 가장 위협적인 적
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// D		A		B	: 그룹에 속해 있는 아군중 하나라도 HP가 70% 이하가 되었을 때			
				//						: 자신에게
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true, 0.7f);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = this;
			}
			break;

		case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

		case FIFTH_PATTERN:
			{
				// 조건		대상	행동
				// A		B		D	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
            const char* szTargetName = 0;

            if (0 != ppAggresiveCreature[0] &&
                Creature::CT_PC == Creature::GetCreatureType(ppAggresiveCreature[0]->GetCID()))
            {
                szTargetName = static_cast<CCharacter*>(ppAggresiveCreature[0])->GetCharacterName();
            }

            // 스킬 사용시 외치기
            CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
                static_cast<unsigned short>(m_CurrentPos.m_fPointX),
                static_cast<unsigned short>(m_CurrentPos.m_fPointZ), 
                CMonsterShout::SKILL_ATTACK, szTargetName, attackType.m_wType);

			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}


// -------------------------------------------------------------------------------------
// 몬스터 타입 (경비병)

CGuardMonster::CGuardMonster()
: CSkillMonster()
{
	m_wDefaultSearchRange = GUARD_SEARCH_RANGE;
}

CGuardMonster::CGuardMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon)
: CSkillMonster(MonsterCreate, bAdminCmdSummon)
{
}

CGuardMonster::~CGuardMonster()
{
}

void CGuardMonster::SearchPlayer()
{
	// TODO : 해상도 조절을 통해 float 계산을 없애보자.
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG1(g_Log, "CID:0X%08 몬스터가 셀 범위 밖에 있습니다.", m_dwCID);
		return;
	}

	CCell*		pCell = NULL;
	CCharacter*	pTempTarget = NULL;
	CCharacter*	pCurrentTarget = NULL;

	const float	fSquareSearchRange = (float)(m_wSearchRange * m_wSearchRange);

	for (int nCellCount = 0; nCellCount < CCell::CONNECT_NUM; ++nCellCount)
	{
		pCell = m_CellPos.m_lpCell->GetConnectCell(nCellCount);
		if (NULL == pCell || false == pCell->IsCharacter())
		{
			continue;
		}

		pTempTarget = pCell->GetFirstCharacter();

		while (NULL != pTempTarget)
		{
			if (pTempTarget->GetStatus().m_nNowHP > 0 &&
				false == pTempTarget->IsRideArms() &&
				EnemyCheck::EC_ENEMY == IsEnemy(pTempTarget))
			{
				if (false == pTempTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth) && 
					false == pTempTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Invincible) && 
					false == pTempTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
				{				
					const float fDX = pTempTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
					const float fDZ = pTempTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;

					const float fDistance = (fDX * fDX) + (fDZ * fDZ);

					if (fDistance < fSquareSearchRange)
					{
						pCurrentTarget = pTempTarget;
						break;
					}
				}
			}

			pTempTarget = pCell->GetNextCharacter();
		}
	}

	if (NULL != pCurrentTarget)
	{
		m_Threat.AddToThreatList(pCurrentTarget, 1);
		m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_SEEN_PLAYER);
	}
}

bool CGuardMonster::SkillAttack(void)
{
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / CSkillMgr::MAX_SKILL_LEVEL;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % CSkillMgr::MAX_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}

	if (cSkillLockCount > 0 && cSkillLevel < CSkillMonster::USE_SKILL_MIN_LEVEL)
	{
		--cSkillLockCount;
		cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;
	}

	// 마법을 쓸수 없는.. m_nSkillLevel 이 0 인 녀석이다.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x 마법을 쓸수 없는 녀석입니다. 몬스터의 스킬레벨이 0 으로 설정되어 있습니다.", m_dwCID);
		return false;
	}

	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, THIRD_PATTERN=2, FOURTH_PATTERN=3, MAX_PATTERN = 4 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
			case FIRST_PATTERN:
			{
				// 조건		대상	행동
				// B		B		D	: 자신에게 가장 위협적인 적이 자신으로부터 5m 이상 멀어질 때
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];

				if (CalculateDistance(m_lpTarget) < 5.f)
				{
					++nSelectPattern;
					continue;
				}

				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

			case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// E		A		C	: 자신에게 부여 마법이 걸려져 있지 않을 때			
				//						: 자신에게
				//						: C 스킬 사용

				// 스킬이 없거나, 이미 부여 마법이 걸려 있는 상태라면.. 다음 패턴으로 넘긴다.
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL] || 
					true == GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];
				ppAggresiveCreature[0] = this;
			}
			break;

			case THIRD_PATTERN:
			{
				// 조건		대상	행동
				// C		C		B	: 그룹에 속해있는 아군중 하나라도 HP가 50% 이하가 되었을 때			
				//						: 가장 낮은 HP를 가지고 있는 아군
				//						: B 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[B_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = B_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[B_SKILL];

				ppAggresiveCreature[0] = FindLowestHPMember(attackType, true);
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

			case FOURTH_PATTERN:
			{
				// 조건		대상	행동
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

			default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}
