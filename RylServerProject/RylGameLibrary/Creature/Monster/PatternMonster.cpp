
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
// ��ȯ�� (���� ���, ����� ��ų�� ����)

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

		// Ÿ���� ĳ������ ���
		if (Creature::CT_PC == Creature::GetCreatureType(ppDefenders[cDefender]->GetCID()))
		{
			lpCharacter = reinterpret_cast<CCharacter *>(ppDefenders[cDefender]);
			lpSummonee = lpCharacter->GetSummonee();
		}
		else
		{
			// Ÿ���� ��ȯ���� ���
			if (Creature::IsSummonMonster(ppDefenders[cDefender]->GetCID()))
			{
				lpCharacter = reinterpret_cast<CSummonMonster *>(ppDefenders[cDefender])->GetMaster();
			}
		}

		// TODO : ���� ������ �������ݽô�.
		cDefenderJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenserMPHeal[cDefender] = 0;

		// ����� �ݿ�
		const unsigned short wDamage = 
			ppDefenders[cDefender]->ApplyDamage(attackType, this, cOffencerJudge, cDefenderJudges[cDefender], wOffencerMPHeal, wDefenserMPHeal[cDefender], wError);

		// ���� ��Ŷ �����
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

	// ��ȯ���� �̵� �ӵ��� ��ȯ�ڿ� �����ش�. (������ ������...)
	if (MonsterInfo::Z3D_CA_WALK == MotionID || MonsterInfo::Z3D_CA_RUN == MotionID)
	{
		Motion.m_fVelocity = ((4.9f * (1.0f + (m_lpMaster->GetCharStatus().m_nDEX - 20) * 0.03f)) * Motion.m_dwFrame) / FPS;
	}

	return true;
}


// ----------------------------------------------------------------------------------------------
// ���� ����

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
		// 1����
		case MonsterInfo::STATUE_HUMAN_COMPLETE1:
		case MonsterInfo::STATUE_AKHAN_COMPLETE1:
		// 2����
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

	// ��ũ ī���� ���� ���� ����
	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		// �߰輭���� HP ������Ʈ ���� ������
		SendHPUpdate();
	}
/*
	// �ű��� ���������
	else if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE9)
	{
		// �߰輭���� HP ������Ʈ ���� ������
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
		// ���� �ε� �Ϸ�
		// AttackInfo �� ������ �ε����� ������ Ŭ���̾�Ʈ���� ���ֵ��� �ؾ��Ѵ�.
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
		// ���� �ε� �Ϸ�
		switch (m_MonsterInfo.m_dwKID)
		{
			// 1����
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
			// 2����
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
				// �� ��ƾ�� ���� ��ɿ� ���ؼ��� ����Ǿ�� �Ѵ�. (�ʱ�ȭ)
				CStatue* lpParent = (NULL == m_lpParent) ? this : m_lpParent;
				lpNextStatue = GetLinkStatue(static_cast<unsigned short>(lpParent->m_MonsterInfo.m_dwKID));
				break;
		}
	}
	else
	{
		// ������ ���� ������ �μ��� ���
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
			ERRLOG2(g_Log, "CID:0x%08x ������ �μ� �ڰ� ĳ���Ͱ� �ƴմϴ�. ������CID:0x%08x", m_dwCID, pOffencer->GetCID());
			lpNextStatue = GetLinkStatue(MonsterInfo::GetDefaultStatueKID(static_cast<unsigned short>(m_MonsterInfo.m_dwKID)));
		}
	}

	if (NULL != lpNextStatue)
	{
		lpNextStatue->Respawn(m_dwLastTime);

		// ������ : ��ũ ī���� ���� ����
	//	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar
	//	{
			// 2008.07.17 �������϶� ���������� �����ؾ��Ѵ�.
			// ��ũ ī���� ���� �����
			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				// �߰輭���� KID �� HP ������Ʈ ���� ������
				lpNextStatue->SendKIDUpdate();
				lpNextStatue->SendHPUpdate(true);

				// ������ ǥ�� ���� ������
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// ���� ������ ����Ʈ �߰� Ȥ�� ����
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
			// �ű��� ���� �����
			if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE9)
			{
				// �߰輭���� KID �� HP ������Ʈ ���� ������
				lpNextStatue->SendKIDUpdate();
				lpNextStatue->SendHPUpdate(true);

				// ������ ǥ�� ���� ������
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();

				// �ű��� ���� ����� ����Ʈ �߰� Ȥ�� ����
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

	// �� ���� ����
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

				// �α� �����
				CCharacter* lpOffencerChar = reinterpret_cast<CCharacter*>(pOffencer);
				if (lpOffencerChar)
				{
					char szOffencerNation[8];
					if (lpOffencerChar->GetRace() == CClass::HUMAN) strcpy(szOffencerNation, "HUMAN");
					else strcpy(szOffencerNation, "AKHAN");

					DETLOG7(g_Log, "Battle Server Log :: (Channel : %d, %s) - CID : 0x%08x ĳ����(%s, %s)�� ������ �μ̽��ϴ�. (���� Score - HM: %d  AK: %d)",
							(lpBGServerMap->GetMapIndex() & ~VirtualArea::BGSERVERMAP), lpBGServerMap->GetMapTypeName(),
							lpOffencerChar->GetCID(), lpOffencerChar->GetCharacterName(), szOffencerNation,
							lpBGServerMap->GetMapInfo().m_wScore[CClass::HUMAN], lpBGServerMap->GetMapInfo().m_wScore[CClass::AKHAN]);
				}
				else
				{
					DETLOG4(g_Log, "Battle Server Log :: (Channel : %d, %s) - �������� ������ �μ̽��ϴ�. (���� Score - HM: %d  AK: %d)",
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
	// �ʱ� ���°� �߸��� �ƴ� ������� ���� ó��
	// ex) �ʱ� ���°� �޸� �����̶�� �޸� ������ ���� Ÿ�ֿ̹� �߸� ������ �����.
	if (m_MonsterInfo.m_dwKID == wKind)
	{
		if (0 != GetMapIndex())
		{
			wKind = MonsterInfo::BG_STATUE_NEUTRALITY1;
		}
		else
		{
			// 1����
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
			// 2����
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

	// �̹� �����ϴ��� üũ
	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
		if (lpVirtualArea)
		{
			CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
			if (lpVirtualMonsterMgr && NULL != lpVirtualMonsterMgr->GetCreature(tempInfo.m_dwCID))
			{
				ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
				return false;
			}
		}
	}
	else
	{
        if (NULL != CCreatureManager::GetInstance().GetCreature(tempInfo.m_dwCID))
		{
			ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
			return false;
		}
	}

	tempInfo.m_nKID = wKind;
	tempInfo.m_Pos = m_OriginalPosition;

	CStatue* lpStatue = new CStatue(tempInfo, this);
	if (NULL == lpStatue) 
	{
		ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
		return false;
	}

	lpStatue->SetMapIndex(GetMapIndex());
	if (false == lpStatue->InitMonster(tempInfo.m_Pos, true))
	{
		ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
		return false;
	}

	// ���� �߰�
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

		// ������ ǥ�� ���� ������
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
			if(m_cGiveType == 0)	// ������� ���� ZONE3������ �����ߴ� ��Ʋ�׶��� ����
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
				// �ش缮���� �����������..
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
// ������Ʈ ���� (��, ����, ������)

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
// ������Ʈ ���� (��, ����, ������)

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
// ��ų ��� ����

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
	// edith 2009.07.16 ���ʹ� ��ų ĳ���� �߿��� �̵����� ����.
//	if(m_bCasting)
//		return 0;

	return CMonster::SendMove(nAniNum);
}


bool CSkillMonster::Attack(AtType attackType, unsigned char cDefenderNum, 
						   CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal)
{
	if (m_CreatureStatus.m_nNowHP == 0) 
	{
		ERRLOG1(g_Log, "CID:0x%08x ���� ���Ͱ� �����Ϸ��� �Ͽ����ϴ�.", m_dwCID);
		return false;
	}

	if (cDefenderNum > AtNode::MAX_DEFENDER_NUM) 
	{
		ERRLOG2(g_Log, "CID:0x%08x ���Ͱ� ������ ��, ������� ���ڰ� �ִ� ����� ���ڸ� �Ѿ����ϴ�. ����ڼ� : %d", 
			m_dwCID, cDefenderNum);
		cDefenderNum = AtNode::MAX_DEFENDER_NUM;
	}

	if (0 == (attackType.m_wType & AtType::SKILL_BIT) && 0 == cDefenderNum) 
	{
		ERRLOG0(g_Log, "��ų�� �ƴ� �Ϲ� ������, �ݵ�� Ÿ���� ���� ��쿡�� ������ ������ �մϴ�.");
		return false;
	}


	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL == pSkillProtoType) 
		{
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
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

    // ���� ��ġ�� ������ ����.
    unsigned int nCriticalCount = 0;
    CCharacter*  lpDefendShoutCharacter = 0;



	for (; cIndex < cDefenderNum; ++cIndex) 
	{
		// MP �Ҹ� Ÿ�ֱ̹����� ī��Ʈ (���� ������ �� ���� MP �Ҹ�)
		--m_cConsumeMPCount;

		if (NULL == ppDefenders[cIndex])
		{ 
			continue; 
		}
		if (0 == ppDefenders[cIndex]->GetStatus().m_nNowHP)
		{ 
			continue; 
		}

		// TODO : ���� ������ �������ݽô�.
		cDefenderJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenderMPHeal[cDefender] = 0;

		const unsigned short nPrevHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nPrevMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;

		const unsigned short wPrevAttackerHP = m_CreatureStatus.m_nNowHP;

		// ����� �ݿ�
		const unsigned short wDamage = ppDefenders[cIndex]->ApplyDamage(attackType, this, 
            cOffencerJudge, cDefenderJudges[cDefender], wOffencerMPHeal, wDefenderMPHeal[cDefender], wError);

        if (cDefenderJudges[cDefender] == ClientConstants::Judge_Critical)
        {
            ++nCriticalCount;
        }

		const unsigned short nNowHP	= ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nNowMP	= ppDefenders[cIndex]->GetStatus().m_nNowMP;

		// ��ų�� ���� �ڻ� ����
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
                // ����ڰ� ĳ������ ���
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
    
    // ��ġ�� �˻�
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

    // ��ġ�� �˻��ؼ� ��ħ. �̸���, �����ϴ� ����̹Ƿ� �´� �� �ƹ����̳� �ѳ� ��Ƽ� �׳� �̸��� �ش�.
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

	// ��ų ���
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
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

	// ��ų�� ����Ҽ� ���� ��Ȳ���� üũ
	if (m_CreatureStatus.m_nNowMP < useMP || 
		dwCurrentTime - m_lastCastTime[cSkillPattern] < dwCoolDownTime ||
		Math::Random::ComplexRandom(100) > m_MonsterInfo.m_wSkillUseRate)
	{
		return false;
	}


	// ��ų �Ÿ� üũ
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

		// ��ų ��� ����
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


	// TODO : �����ϴ� ������ �����սô�. (����� ������ ����)
	short nMaxDefenderNum = AtNode::MAX_DEFENDER_NUM;
	std::fill_n(&nDefenserJudges[0], nMaxDefenderNum, ClientConstants::Judge_Front);
	std::fill_n(&wDefenserMPHeal[0], nMaxDefenderNum, 0);

	float fDir = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
							pTarget->GetCurrentPos().m_fPointX, pTarget->GetCurrentPos().m_fPointZ);

	// ĳ���� Ÿ���̸�, ĳ��Ʈ ���� ��ȯ�ϰ�, �������� �ٷ� ����Ѵ�.
	if (pThisSkill->m_eSkillType == Skill::Type::CAST)
	{
		m_bCasting = true;

		m_attackType = attackType;
		m_dwTargetCID = ppDefenders[0]->GetCID();
		m_cSkillPattern = cSkillPattern;
		// edith 2009.08.06 ��ų ���ݽ� �̵����� �ʰ� �ϱ�
		SendMove(1);
		return true;
	}

	// ��ų ����� �ð��� ����
	m_lastCastTime[cSkillPattern] = dwCurrentTime;

	// MON_TODO : ��ų ��� ������ �����ش�.
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
			ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", 
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

		// edith 2009.08.06 ���Ͱ� ��ų���� ��ų�� ����Ʈ�� Ŭ���̾�Ʈ���� ����ֱ� ���� �κ�.
		lpCell->SendCastObjectInfo(GetCID(), ppDefenders[0]->GetCID(), castObject);
	}
	
	// ���� �����̶��...
	if (0 != pThisSkill[cSkillLockCount].m_fEffectExtent)
	{
		if (Skill::Target::PARTY == pThisSkill[cSkillLockCount].m_eTargetType)
		{
			// ��Ƽ �����̶��
			CParty* lpParty = ppDefenders[0]->GetParty();
			if (NULL != lpParty)
			{
				return lpParty->Attack(attackType, ppDefenders, nDefenserJudges, this,
							pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent, cTargetType);
			}
		}
		else
		{
			// �Ϲ� ���� ����
			return CAggresiveCreature::MultiAttack(attackType, cDefenderNum, ppDefenders, nDefenserJudges, 
						GetCurrentPos(), fDir, pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE, m_MonsterInfo.m_fAttackAngle, cTargetType);
		}
	}

	// ����ϴ� ��ų�� ������ ���� Attack ȣ�� �Լ��� �޸��Ѵ�.
	// Enchant �ο� ������ ���� �Ѹ��Ը� ����Ѵ�.
	return Attack(attackType, cDefenderNum, ppDefenders, nDefenserJudges, wDefenserMPHeal);
}

bool CSkillMonster::UseCastedSkill(void)
{
	CAggresiveCreature* pTarget = reinterpret_cast<CAggresiveCreature*>(CCreatureManager::GetInstance().GetCreature(m_dwTargetCID));

	if (NULL == pTarget) return false;
	if (pTarget->GetMapIndex() != GetMapIndex()) return false;

	// ��ų ���
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(m_attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, m_attackType.m_wType);
		return false;
	}

	char cTargetType = Skill::Target::ENEMY;
	unsigned char cSkillLockCount = m_MonsterInfo.m_cSkillLevel / 6;
	unsigned char cSkillLevel = m_MonsterInfo.m_cSkillLevel % 6;

	// 100% Ȯ���� ��ų�� ����� ���̱� ������.. ĳ��Ʈ Ÿ���� �����ϰ�, ���� ��� Ȯ���� �˻����� �ʴ´�.
	m_lastCastTime[m_cSkillPattern] = timeGetTime();

	unsigned char cDefenderNum = 1;
	unsigned char nDefenserJudges[AtNode::MAX_DEFENDER_NUM];
	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM];
	CAggresiveCreature* ppDefenders[AtNode::MAX_DEFENDER_NUM] = {0, };
	ppDefenders[0] = pTarget;
	
	// ��ų �Ÿ� üũ
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

		// ��ų ��� ����
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

    
	// TODO : �����ϴ� ������ �����սô�. (����� ������ ����)
	short nMaxDefenderNum = AtNode::MAX_DEFENDER_NUM;
	std::fill_n(&nDefenserJudges[0], nMaxDefenderNum, ClientConstants::Judge_Front);
	std::fill_n(&wDefenserMPHeal[0], nMaxDefenderNum, 0);

	float fDir = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
							pTarget->GetCurrentPos().m_fPointX, pTarget->GetCurrentPos().m_fPointZ);

	// MON_TODO : ��ų ��� ������ �����ش�.
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

	// ���� �����̶��...
	if (0 != pThisSkill[cSkillLockCount].m_fEffectExtent)
	{
		if (Skill::Target::PARTY == pThisSkill[cSkillLockCount].m_eTargetType)
		{
			// ��Ƽ �����̶��
			CParty* lpParty = ppDefenders[0]->GetParty();
			if (NULL != lpParty)
			{
				return lpParty->Attack(m_attackType, ppDefenders, nDefenserJudges, this,
							pThisSkill[m_attackType.m_cSkillLockCount].m_fEffectExtent, cTargetType);
			}
		}
		else
		{
			// �Ϲ� ���� ����
			return CAggresiveCreature::MultiAttack(m_attackType, cDefenderNum, ppDefenders, nDefenserJudges, 
						GetCurrentPos(), fDir, pThisSkill[cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE, m_MonsterInfo.m_fAttackAngle, cTargetType);
		}
	}

	// ����ϴ� ��ų�� ������ ���� Attack ȣ�� �Լ��� �޸��Ѵ�.
	// Enchant �ο� ������ ���� �Ѹ��Ը� ����Ѵ�.
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

	// ��Ƽ ����� ��������.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// �ش� ���͸� ã�Ƽ� �����ص� ����
	std::vector<CMonster* > targetVector;
	targetVector.clear();
	targetVector.reserve(10);
	
	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// ��� ���� ���±�
		CMonster* lpMonster = lpParty->GetMember(i);

		// �Ÿ� üũ
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
									  (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
									  (m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
									  (m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

		if (fSquareTargetDistance <= CCell::CELL_DISTANCE)
		{
			targetVector.push_back(lpMonster);
		}
	}

	// �Ѹ� ���ٸ�...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// �ش� ���Ͽ� ���õ� ���� �߿��� �ְ� ������ ���͸� �����Ѵ�.
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

	// ��ų ���
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// ��Ƽ ����� ��������.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// ��ų ��ȿ �Ÿ�
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
									(pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	// �ش� ���͸� ã�Ƽ� �����ص� ����
	std::vector<CMonster* > targetVector;
	targetVector.clear();

	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// ��� ���� ���±�
		CMonster* lpMonster = lpParty->GetMember(i);

		// NULL �̰ų� �̹� �ش� ��ų�� �ο��� ���¶�� ��������..
		if (NULL == lpMonster || true == lpMonster->GetEnchantInfo().GetFlag(m_MonsterInfo.m_cEnchantSpellType))
		{
			continue;
		}

		// �Ÿ� üũ
		float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
										(m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
										(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
										(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

		if (fSquareTargetDistance <= fSquareEffectDistance)
		{
			targetVector.push_back(lpMonster);
		}
	}

	// �Ѹ� ���ٸ�...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// �ش� ���Ͽ� ���õ� ���� �߿��� �ְ� ������ ���͸� �����Ѵ�.
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

	// ��ų ���
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// ��Ƽ ����� ��������.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);

	// ��ų ��ȿ �Ÿ�
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
									(pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	// �ش� ���͸� ã�Ƽ� �����ص� ����
	std::vector<CMonster* > targetVector;
	targetVector.clear();

	for (int i=0; i<lpParty->GetMemberNum(); ++ i)
	{
		// ��� ���� ���±�
		CMonster* lpMonster = lpParty->GetMember(i);

		// NULL �̸� ��������..
		if (NULL == lpMonster)
		{
			continue;
		}

		// HP�� üũ
		if (lpMonster->GetStatus().m_StatusInfo.m_nMaxHP * fRemainHP >= lpMonster->GetStatus().m_nNowHP)
		{
			if (bRegin)
			{
				// �Ÿ� üũ
				float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) * 
												(m_CurrentPos.m_fPointX - lpMonster->GetCurrentPos().m_fPointX) + 
												(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ) * 
												(m_CurrentPos.m_fPointZ - lpMonster->GetCurrentPos().m_fPointZ);

				if (fSquareTargetDistance > fSquareEffectDistance) continue;
			}

			targetVector.push_back(lpMonster);
		}
	}

	// �Ѹ� ���ٸ�...
	if (targetVector.size() == 0)
	{
		return NULL;
	}

	// �ش� ���Ͽ� ���õ� ���� �߿��� ���� HP ���� sort �Ѵ�.
	std::sort(targetVector.begin(), targetVector.end(), CompareHP());
	return targetVector[0];
}

CAggresiveCreature* CSkillMonster::FindEnemy(const AtType& attackType, unsigned char cType1, unsigned char cType2)
{
	// ��ų ���
	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return NULL;
	}

	// ���� CID Set �� �����.
	CMonsterParty::PartyTargetSet targetSet;
	targetSet.clear();

	if (m_pParty)
	{
		// ��Ƽ ����� ���� Set �� �ִ´�.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(m_pParty);
		targetSet = lpParty->GetPartyTargetSet();
	}

	// ���� Ÿ�� ���� ���� Set �� �ִ´�.
	if (m_lpTarget) targetSet.insert(m_lpTarget->GetCID());

	// ��ų ��ȿ �Ÿ�
	float fSquareEffectDistance = (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
								  (pThisSkill[attackType.m_cSkillLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

	CMonsterParty::PartyTargetSet::iterator pos = targetSet.begin();
	CMonsterParty::PartyTargetSet::iterator end = targetSet.end();
	CAggresiveCreature* lpTarget = NULL;

	while (pos != end)
	{
		// ��ų ��ȿ ���� ���� ���� ã�´�.
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

		// ���� Ÿ�Կ� �´� Ÿ���� ã�´�.
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
// ���� Ÿ�� (�����)

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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ����		���	�ൿ
				// B		B		D	: �ڽſ��� ���� �������� ���� �ڽ����κ��� 5m �̻� �־��� ��
				//						: �ڽſ��� ���� �������� ������
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		C		B	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		B		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (������)

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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ����		���	�ൿ
				// B		B		D	: �ڽſ��� ���� �������� ���� �ڽ����κ��� 5m �̻� �־��� ��
				//						: �ڽſ��� ���� �������� ������
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		C		B	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// C		B		A	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��
				//						: �ڽſ��� ���� �������� ��
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (���ݶ���Ʈ)

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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ����		���	�ൿ
				// C		C		B	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// D		C		D	: �׷쿡 ���� �ִ� �Ʊ��� �ϳ��� HP�� 70% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		B		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		J		C	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ο� ��ų�� �ɷ����� ���� �Ʊ� �� ���� ���� LV
				//						: C ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
				if (0 == m_MonsterInfo.m_wSkillID[C_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = C_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[C_SKILL];

				// �ش� ����� �Ÿ� �ȿ� �ִ� �ְ� ������ �ش� ���͸� �������ش�.
				// �ش� �ο� ��ų�� ������ �̹� �ɷ��ִ� �༮�� �����ϰ� ã�Ƽ� �Ѱ��ش�.
				ppAggresiveCreature[0]	= FindHighestLVMember(attackType);

				// �Ѹ� ���ٸ�.. ���� ��������
				if (NULL == ppAggresiveCreature[0])
				{
					++nSelectPattern;
					continue;
				}
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (������)
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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ����		���	�ൿ
				// F		A		B	: �ڽ��� MP�� 30% ���ϰ� �Ǿ��� ��			
				//						: �ڽſ���
				//						: B ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// A		F		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: ���� ���� HP�� ������ �ִ� �� 
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		F		D	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: ���� ���� HP�� ������ �ִ� �� 
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (����)

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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ������ ���۵Ǹ� Boss �� æƮ ��ų�� �ߵ�
				
				// ��ų�� ���ų�, �̹� æƮ ��ų�� �ߵ��� ���¸�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// A		F		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: ���� ���� HP�� ������ �ִ� �� 
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// D		A		B	: �׷쿡 ���� �ִ� �Ʊ��� �ϳ��� HP�� 70% ���ϰ� �Ǿ��� ��			
				//						: �ڽſ���
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// B		B		C	: �ڽſ��� ���� �������� ���� �ڽ����κ��� 5m �̻� �־��� ��
				//						: �ڽſ��� ���� �������� ������
				//						: C ��ų ���

				// ��ų�� ���ٸ�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		B		D	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: �ڽſ��� ���� �������� ��
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (ġ��)

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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ������ ���۵Ǹ� Boss �� æƮ ��ų�� �ߵ�

				// ��ų�� ���ų�, �̹� æƮ ��ų�� �ߵ��� ���¸�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		C		B	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		B		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		B		D	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
// ���� Ÿ�� (���ӵ�)

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

    // �������� ��ġ��.
    CMonsterShout::GetInstance().Shout(m_dwCID, m_MonsterInfo.m_dwKID, 
        static_cast<unsigned short>(m_CurrentPos.m_fPointX),
        static_cast<unsigned short>(m_CurrentPos.m_fPointZ), 
        CMonsterShout::RESPAWN, 0, 0);

	// ��Ƽ�� �߰�
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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
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
				// ������ ���۵Ǹ� Boss �� æƮ ��ų�� �ߵ�

				// ��ų�� ���ų�, �̹� æƮ ��ų�� �ߵ��� ���¸�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		B		A	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: �ڽſ��� ���� �������� ��
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// D		A		B	: �׷쿡 ���� �ִ� �Ʊ��� �ϳ��� HP�� 70% ���ϰ� �Ǿ��� ��			
				//						: �ڽſ���
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// A		B		D	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
            const char* szTargetName = 0;

            if (0 != ppAggresiveCreature[0] &&
                Creature::CT_PC == Creature::GetCreatureType(ppAggresiveCreature[0]->GetCID()))
            {
                szTargetName = static_cast<CCharacter*>(ppAggresiveCreature[0])->GetCharacterName();
            }

            // ��ų ���� ��ġ��
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
// ���� Ÿ�� (���)

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
	// TODO : �ػ� ������ ���� float ����� ���ֺ���.
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG1(g_Log, "CID:0X%08 ���Ͱ� �� ���� �ۿ� �ֽ��ϴ�.", m_dwCID);
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

	// ������ ���� ����.. m_nSkillLevel �� 0 �� �༮�̴�.
	if (0 == cSkillLockCount && 0 == cSkillLevel)
	{
		//ERRLOG1(g_Log, "CID:0x%08x ������ ���� ���� �༮�Դϴ�. ������ ��ų������ 0 ���� �����Ǿ� �ֽ��ϴ�.", m_dwCID);
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
				// ����		���	�ൿ
				// B		B		D	: �ڽſ��� ���� �������� ���� �ڽ����κ��� 5m �̻� �־��� ��
				//						: �ڽſ��� ���� �������� ������
				//						: D ��ų ���

				// ��ų�� ���ٸ�.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// E		A		C	: �ڽſ��� �ο� ������ �ɷ��� ���� ���� ��			
				//						: �ڽſ���
				//						: C ��ų ���

				// ��ų�� ���ų�, �̹� �ο� ������ �ɷ� �ִ� ���¶��.. ���� �������� �ѱ��.
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
				// ����		���	�ൿ
				// C		C		B	: �׷쿡 �����ִ� �Ʊ��� �ϳ��� HP�� 50% ���ϰ� �Ǿ��� ��			
				//						: ���� ���� HP�� ������ �ִ� �Ʊ�
				//						: B ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				// ����		���	�ൿ
				// A		B		A	: ��ų ��� ���� MP �ܿ��� ���� / ����� ��ų ��� ���� ������ ���� ��			
				//						: �ڽſ��� ���� �������� ������
				//						: A ��ų ���

				// ��ų�� ���ٸ�.. ���� ��������..
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
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ �Ѿ�Խ��ϴ�.", m_dwCID);
				return false;
			}
		}

		// ��ų ��� (ĳ���� Ÿ���� ��쿡�� ����Ҽ� �ִ����� üũ�ϰ� �ȴ�.)
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
