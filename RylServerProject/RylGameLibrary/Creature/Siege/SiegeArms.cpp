#include "stdafx.h"
#include "SiegeArms.h"

#include <Skill/SkillTable.h>
#include <Skill/SkillMgr.h>

#include <Utility/Math/Math.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/Duelmap/DuelCellManager.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>

#include <Network/ClientSocket/ClientConstants.h>




CSiegeArms::CSiegeArms(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
					   unsigned long dwHP, unsigned short wObjectType, unsigned char cState,
					   unsigned char cUpgradeStep)
: CSiegeObject(MonsterCreate, dwOwnerID, cNation, dwHP, wObjectType, cState, cUpgradeStep)
{
}

CSiegeArms::~CSiegeArms()
{
}

bool	CSiegeArms::AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (NULL == lpRideChar || m_CreatureStatus.m_nNowHP == 0)
	{
		wError = PktAtAck::FAIL_ALREADY_DEAD;
		return true;
	}

	if (0 == (attackType.m_wType & AtType::SKILL_BIT))
	{
		wError = PktAtAck::FAIL_NOT_SIEGE_ATTACK;
		return false;
	}

	const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
	if (NULL == pThisSkill) 
	{ 
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return false;
	}

	const unsigned short wLockCount = GetSkillLockCount(attackType.m_wType);
	if (wLockCount < 0 || wLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ ��ų�� ��ī��Ʈ�� �̻��մϴ�. SkillType : 0x%04x, LockCount : %d", 
			m_dwCID, attackType.m_wType, wLockCount);
		return false;
	}

	unsigned char cDefenderNum = attackNode.m_wDefenserNum > AtNode::MAX_DEFENDER_NUM ?
		AtNode::MAX_DEFENDER_NUM : attackNode.m_wDefenserNum; // �ִ� ����� �� ����

	CAggresiveCreature*	lpAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned short wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	char cTargetType = Skill::Target::ENEMY;

	if (0 == cDefenderNum)
	{
		if (0 != attackType.m_cMissileAttack)
		{
			return MissileAttack(attackType, 0, pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent, 
				Math::Const::PI * 2, cTargetType);
		}
		else
		{
			// ĳ���ÿ� ������ ���
			return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenserMPHeal);
		}
	}
	else
	{
		if (0 == pThisSkill[wLockCount].m_fMaxRange && 0 == pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent) 
		{
			if (m_dwCID != attackNode.m_dwDefenser[0])
			{
				ERRLOG2(g_Log, "CID:0x%08x �ڱ��ڽſ��Ը� �� �� �ִ� ��ų�Դϴ�. SkillID:0x%04x", 
					m_dwCID, attackType.m_wType);
				return false;
			}
		}

		// ��������� ���͸� �����Ҽ� ����.
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(attackNode.m_dwDefenser[0]);
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			wError = PktAtAck::FAIL_TO_MONSTER;
			return false;
		}

		// ��ų �Ÿ� üũ
		CAggresiveCreature* lpTargetCreature = NULL;

		// Target Creature ������
		if (0 != GetMapIndex())
		{
			lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
			if (lpTargetCreature && lpTargetCreature->GetMapIndex() != GetMapIndex()) lpTargetCreature = NULL;
		}
		else
		{
			lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
		}

		// Target Creature ó���ϱ�
		if (NULL != lpTargetCreature)
		{
			float fSquareTargetDistance = (m_CurrentPos.m_fPointX - lpTargetCreature->GetCurrentPos().m_fPointX) * 
				(m_CurrentPos.m_fPointX - lpTargetCreature->GetCurrentPos().m_fPointX) + 
				(m_CurrentPos.m_fPointZ - lpTargetCreature->GetCurrentPos().m_fPointZ) * 
				(m_CurrentPos.m_fPointZ - lpTargetCreature->GetCurrentPos().m_fPointZ);

			float fSquareEffectDistance = (pThisSkill[wLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE) * 
				(pThisSkill[wLockCount].m_fMaxRange + Skill::ERROR_OF_DISTANCE);

			if (fSquareTargetDistance > fSquareEffectDistance)
			{
				wError = PktAtAck::FAIL_TOO_FAR;
				return false;
			}
		}

		if (pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND || 
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::DEAD_FRIEND ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::FRIEND_OBJECT ||
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::PARTY || 
			pThisSkill[attackType.m_cSkillLockCount].m_eTargetType == Skill::Target::SUMMON)
		{
			//cTargetType = Skill::Target::FRIEND;
			wError = PktAtAck::FAIL_FRIENDLY_ATTACK;
			return false;
		}

		// Ŭ���̾�Ʈ�� �Ѱ��� Ÿ�ٵ��� üũ�Ѵ�. (���� ������ �ɸ��� Ÿ���� ���� üũ)
		for (unsigned char cDefender = 0; cDefender < cDefenderNum; ++cDefender) 
		{
			// Target Creature ���
			CAggresiveCreature* lpCreature = NULL;

			Creature::CreatureType eCreatureType = Creature::GetCreatureType(attackNode.m_dwDefenser[cDefender]);
			if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
			{
				continue;
			}

			lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[cDefender]);
			if (lpCreature && lpCreature->GetMapIndex() != GetMapIndex())	
			{
				lpCreature = NULL;
			}

			if (NULL != lpCreature)
			{
				// �������� ����(-_-)
				if (Skill::Target::FRIEND == cTargetType)
				{
					wError = PktAtAck::FAIL_FRIENDLY_ATTACK;
					return false;
				}
				// �������� ����(��¥ ����)
				else
				{
					// �ڱ⸦ �߽����� �ϴ� ������ ��ų�� ��� Ÿ���� �ڽ����� �����մϴ�.
					// (�� ��� this�� MultiAttack() �Լ��� Ÿ�Ͽ��� ���ܽ��� �ݴϴ�.)
					if (EnemyCheck::EC_ENEMY == IsEnemy(lpCreature) || lpCreature == this)
					{
						lpAggresiveCreature[cDefender] = lpCreature;
					}
				}
			}
		}

		if (0 == cDefenderNum || NULL == lpAggresiveCreature[0]) 
		{
			return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenserMPHeal);
		}

		// ���� ���� üũ
		if (0 != pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent)
		{
			if (Skill::Target::PARTY == pThisSkill[attackType.m_cSkillLockCount].m_eTargetType)
			{
				wError = PktAtAck::FAIL_FRIENDLY_ATTACK;
				return false;
			}
			else
			{
				return CAggresiveCreature::MultiAttack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, 
					lpAggresiveCreature[0]->GetCurrentPos(), 0, pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent, 
					Math::Const::PI * 2, cTargetType);
			}
		}
	}

	return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenserMPHeal);
}

bool	CSiegeArms::Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)
{
	if (m_CreatureStatus.m_nNowHP == 0) 
	{
		ERRLOG1(g_Log, "CID:0x%08x ���� ���� ������Ʈ�� �����Ϸ��� �Ͽ����ϴ�.", m_dwCID);
		return false;
	}

	if (cDefenderNum > AtNode::MAX_DEFENDER_NUM) 
	{
		ERRLOG2(g_Log, "CID:0x%08x ���� ������Ʈ�� ������ ��, ������� ���ڰ� �ִ� ����� ���ڸ� �Ѿ����ϴ�. ����ڼ� : %d", 
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

	if ( IsRidable() )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (!lpRider)
		{
			ERRLOG2(g_Log, "CID:0x%08x ���� ������Ʈ�� Ÿ���ִ� ĳ���Ͱ� �����ϴ�. RiderCID : 0x%08x", m_dwCID, m_dwRiderCID);
			return false;
		}

		// ���ݽ� ���� ���°� Ǯ����.
		lpRider->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::Invincible);
	}

	unsigned char cOffencerJudge = 0;
	unsigned short wOffencerMPHeal = 0;
	unsigned short wError = PktAtAck::NO_SERVER_ERR;

	const int   MAX_BUFFER = sizeof(PktAtAck) + AtNode::MAX_DEFENDER_NUM * sizeof(DefenserNode);
	char        szBuffer[MAX_BUFFER];

	PktAtAck*		lpPktAtAck		= reinterpret_cast<PktAtAck*>(szBuffer);
	DefenserNode*	lpDefenserNode	= reinterpret_cast<DefenserNode*>(lpPktAtAck + 1);                

	m_cConsumeMPCount = std::min(cDefenderNum, unsigned char(AtNode::MAX_MONSTER_DEFENDER_NUM));

	unsigned char cDefender = 0;
	unsigned char cIndex = 0;

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

		// �ִ� ����� �� ���� (���ʹ� ĳ���Ϳʹ� ���� ó��)
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(ppDefenders[cIndex]->GetCID());
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		// TODO : ���� ������ �������ݽô�.
		cDefenserJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenserMPHeal[cDefender] = 0;

		const unsigned short nPrevHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nPrevMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;

		const unsigned short wPrevAttackerHP = m_CreatureStatus.m_nNowHP;

		// ����� �ݿ�
		lpDefenserNode[cDefender].m_wDamage = ppDefenders[cIndex]->ApplyDamage(attackType, this, cOffencerJudge,
			cDefenserJudges[cDefender], wOffencerMPHeal, wDefenserMPHeal[cDefender], wError);

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
				CCharacter* lpDefendCharacter = (CCharacter *)ppDefenders[cIndex];

				CMonster* lpSummonee = lpDefendCharacter->GetSummonee();
				if (NULL != lpSummonee)
				{
					lpSummonee->GuardMe(this, lpDefenserNode[cDefender].m_wDamage);
				}

				lpDefendCharacter->CalculateEquipDurability((ClientConstants::Judge_Guard == cDefenserJudges[cDefender]) ? 
					AtType::GUARD : AtType::DEFENCE);

				CGameClientDispatch* lpDispatch = lpDefendCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), this, lpDefendCharacter, 
						attackType, m_MotionInfo.m_fDirection, lpDefenserNode[cDefender].m_wDamage,
						cDefenserJudges[cDefender], wDefenserMPHeal[cDefender], PktBase::NO_SERVER_ERR);
				}
			}

			// ���� ��Ŷ �����
			lpDefenserNode[cDefender].m_dwCharID = ppDefenders[cIndex]->GetCID();
			lpDefenserNode[cDefender].m_sCurrHP = nNowHP;
			lpDefenserNode[cDefender].m_sCurrMP = nNowMP;
			lpDefenserNode[cDefender].m_wMaxHP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxHP;
			lpDefenserNode[cDefender].m_wMaxMP = ppDefenders[cIndex]->GetStatus().m_StatusInfo.m_nMaxMP;
			lpDefenserNode[cDefender].m_wMPHeal = wDefenserMPHeal[cDefender];

			lpDefenserNode[cDefender].m_cJudge = cDefenserJudges[cDefender];
		}

		++cDefender;
	}

	if (0 != (attackType.m_wType & AtType::SKILL_BIT))
	{
		if (0 == cDefender) 
		{ 
			Skill::CFunctions::ConsumeMP(attackType, this, 0);
		}
	}

	lpPktAtAck->m_dwCharID = m_dwCID;
	lpPktAtAck->m_AtType = attackType;

	lpPktAtAck->m_wHP = m_CreatureStatus.m_nNowHP;
	lpPktAtAck->m_wMP = m_CreatureStatus.m_nNowMP;
	lpPktAtAck->m_wMPHeal = wOffencerMPHeal;

	lpPktAtAck->m_cJudge = cOffencerJudge;
	lpPktAtAck->m_cDefenserNum = cDefender;

	if ( IsRidable() )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider && lpRider->GetDispatcher())
		{
			CSendStream& SendStream = (lpRider->GetDispatcher())->GetSendStream();
			if (true == SendStream.WrapCompress(
				szBuffer, sizeof(PktAtAck) + cDefender * sizeof(DefenserNode), CmdCharAttack, 0, wError) && 
				PktBase::NO_SERVER_ERR == wError)
			{
				CCell* lpCell = GetCellPos().m_lpCell;
				if (lpCell)
				{
					lpCell->SendAttackInfo(m_dwCID, attackType, cDefender, lpDefenserNode);
					return true;
				}
			}
		}
	}
	else
	{
		CCell* lpCell = GetCellPos().m_lpCell;
		if (lpCell)
		{
			lpCell->SendAttackInfo(m_dwCID, attackType, cDefender, lpDefenserNode);
			return true;
		}
	}

	return false;
}


bool	CSiegeArms::MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)
{
	if (Siege::LONG_RANGE_SIEGE_ARMS != m_wObjectType)
	{
		return false;
	}

	CCell* lpCell = NULL;

	// ��� ���¶�� ��� ���� ó��
	if(CServerSetup::GetInstance().GetDuelModeCheck() && NULL != GetDuelOpponent())
	{
		lpCell = CDuelCellManager::GetInstance().GetCell(GetCID());
	}
	else
	{
		lpCell = CCellManager::GetInstance().GetCell(m_CellPos.m_wMapIndex,
			static_cast<unsigned long>(attackType.m_DstPos.fPointX),
			static_cast<unsigned long>(attackType.m_DstPos.fPointY),
			static_cast<unsigned long>(attackType.m_DstPos.fPointZ));
	}

	if (NULL == lpCell)
	{
		ERRLOG0(g_Log, "CID:0x%08x ���� ��ǥ ������ ���� �������� �ʽ��ϴ�.");
		return false; 
	}

	CAggresiveCreature*	ppDefenders[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned char		cDefenserJudges[AtNode::MAX_DEFENDER_NUM] = {0, };
	unsigned short		wDefenserMPHeal[AtNode::MAX_DEFENDER_NUM] = {0, };
	int nDefenderNum = 0;

	for (int nDirection = 0; nDirection < CCell::CONNECT_NUM && nDefenderNum < AtNode::MAX_DEFENDER_NUM; ++nDirection)
	{
		CCell* lpConnectCell = lpCell->GetConnectCell(nDirection);
		if (NULL == lpConnectCell) 
		{
			continue;
		}

		CAggresiveCreature* lpTempTarget = lpConnectCell->GetFirstAggresiveCreature();

		// Ÿ���� ���ų�, ����� ���� �ִ밡 �Ǹ�, ������ ���������ϴ�.
		while (NULL != lpTempTarget && nDefenderNum < AtNode::MAX_DEFENDER_NUM)
		{
			// ���ݿ� ���� ���ܻ�Ȳ
			bool bException = false;

			// ���⿡ ž���� ĳ���ʹ� �������� �ʴ´�. ��� ���⸦ �����Ѵ�.
			if (Creature::CT_PC == Creature::GetCreatureType(lpTempTarget->GetCID()))
			{
				CCharacter* lpRideChar = reinterpret_cast<CCharacter*>(lpTempTarget);
				if (true == lpRideChar->IsRideArms())
				{
					bException = true;
				}
			}

			EnemyCheck::EnemyType eTargetType = IsEnemy(lpTempTarget);
			if ((EnemyCheck::EC_NEUTRAL == eTargetType) || 
				(Skill::Target::FRIEND == cTargetType && EnemyCheck::EC_ENEMY == eTargetType) || 
				(Skill::Target::ENEMY == cTargetType && EnemyCheck::EC_FRIEND == eTargetType))
			{
				bException = true;
			}

			// ��ġ�� �� ������ ó������ �ʴ´�.
			for (int nIndex = 0; nIndex < nDefenderNum; nIndex++)
			{
				if (ppDefenders[nIndex] == lpTempTarget)
				{
					bException = true;
					break;
				}
			}

			if (false == bException)
			{
				const float fDX = lpTempTarget->GetCurrentPos().m_fPointX - attackType.m_DstPos.fPointX;
				const float fDZ = lpTempTarget->GetCurrentPos().m_fPointZ - attackType.m_DstPos.fPointZ;
				const float fDistance = (fDX * fDX) + (fDZ * fDZ);

				const float fSquareAttackRange = nRange * nRange;

				if (fDistance <= fSquareAttackRange)
				{
					const float fTempfDir = CalcDir2D(attackType.m_DstPos.fPointX, attackType.m_DstPos.fPointZ, 
						lpTempTarget->GetCurrentPos().m_fPointX, lpTempTarget->GetCurrentPos().m_fPointZ);
					const float fDifference = (fTempfDir >= fDir) ? (fTempfDir-fDir) : (fDir-fTempfDir);		

					if (fDifference <= fAngle && 0 < lpTempTarget->GetStatus().m_nNowHP)
					{
						ppDefenders[nDefenderNum]       = lpTempTarget;	
						cDefenserJudges[nDefenderNum]   = ClientConstants::Judge_Front;
						wDefenserMPHeal[nDefenderNum]	= 0;
						++nDefenderNum;
					}
				}
			}

			lpTempTarget = lpConnectCell->GetNextAggresiveCreature();
		}
	}

	if (AtNode::MAX_DEFENDER_NUM < nDefenderNum) 
	{
		SERLOG0(g_Log, "���� ������ : ����ڼ��� �ִ�ġ�� �Ѿ�̽��ϴ�.");
	}

	return Attack(attackType, nDefenderNum, ppDefenders, cDefenserJudges, wDefenserMPHeal);
}


bool	CSiegeArms::Dead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	// Ÿ�� �ִ� ĳ���ʹ� �״´�.
	if (0 != m_dwRiderCID)
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider)
		{
			lpRider->GetOff();
			lpRider->Kill(pOffencer);
		}

		m_dwRiderCID = 0;
	}

	// ũ���� �Ŵ������� ���� (�ش� �������� �����Ѵ�.)
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	// �߰� ������ ��Ŷ ����
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, m_dwCID, 
			0, PktSiegeArmsCmd::SIEGE_DESTROY_ARMS,
			PktSiegeArmsCmd::NO_SERVER_ERR);
	}

	return false;
}


bool	CSiegeArms::ToStarterKit(bool bMaterial)
{
	unsigned short wItemProtoTypeID;

	switch (m_wObjectType)
	{
		case Siege::SHORT_RANGE_SIEGE_ARMS:	wItemProtoTypeID = Item::EtcItemID::SHORT_RANGE_ARMS_KIT_ID;	break;
		case Siege::LONG_RANGE_SIEGE_ARMS:	wItemProtoTypeID = Item::EtcItemID::LONG_RANGE_ARMS_KIT_ID;		break;
		case Siege::AIRSHIP:				wItemProtoTypeID = Item::EtcItemID::AIRSHIP_KIT_ID;				break;
	}

	// ��ŸƮŶ ������ ����
	Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemProtoTypeID);

	if (NULL == lpItem)
	{
		ERRLOG0(g_Log, "��� ��� ���� ��ŸƮŶ ������ ������ �����߽��ϴ�.");
		return false;
	}

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter( m_dwOwnerID );

	if (NULL != lpCharacter)
	{
		if (false == lpCharacter->GiveItem(lpItem))
		{
			ERRLOG0(g_Log, "��� ��� ���� ��ŸƮŶ�� �����ִµ� �����Ͽ����ϴ�.");

			DELETE_ITEM(lpItem);
			return false;
		}

		// GievItem ���� ���õ� ���
		if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
		{
			DELETE_ITEM(lpItem);
		}

		if(bMaterial)
		{
			GiveBackSiegeMaterial();
		}
		
		return true;
	}

	// �켱 �ӽ÷� �ּ� ó��.

	if(bMaterial)
	{
		GiveBackSiegeMaterial();
	}	

	// �ٴڿ� ������ ����߸���
	CCell::ItemInfo itemInfo;
	GetCellPos().m_lpCell->SetItem(GetCurrentPos(), lpItem, 0, 0, CCell::NONE, itemInfo);

	return true;
}

bool	CSiegeArms::GiveBackSiegeMaterial()
{
	// ���׷��̵忡 ���� ���� ����
	unsigned char cNum = 0;
	
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(m_wObjectType);
	
	if (lpProtoType)
	{
		cNum = Siege::SIEGE_ARMS_UPGRADE_MATERIAL_NUM / 2;		
	}

	if (cNum > 0)
	{
		// ���� ������ �����ֱ�
		Item::CItem* lpMaterial = Item::CItemFactory::GetInstance().CreateItem(Item::EtcItemID::SIEGE_MATERIAL_ID);

		if (NULL == lpMaterial)
		{
			ERRLOG0(g_Log, "���� ���� ���ۿ� ���� ������ ������ �����߽��ϴ�.");
			return false;
		}

		lpMaterial->SetNumOrDurability(cNum);

		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwOwnerID);

		if (NULL != lpCharacter)
		{
			if (false == lpCharacter->GiveItem(lpMaterial))
			{
				ERRLOG0(g_Log, "���� ���� ���ۿ� ���縦 �����ִµ� �����Ͽ����ϴ�.");

				DELETE_ITEM(lpMaterial);
				return false;
			}

			// GievItem ���� ���õ� ���
			if (lpMaterial->IsSet(Item::DetailData::STACKABLE) && 0 == lpMaterial->GetNumOrDurability())
			{
				DELETE_ITEM(lpMaterial);
			}

			return true;
		}

		CCell::ItemInfo itemInfo;
		GetCellPos().m_lpCell->SetItem(GetCurrentPos(), lpMaterial, 0, 0, CCell::NONE, itemInfo);
	}

	return true;
}

unsigned short	CSiegeArms::GetRepairMaterialNum()
{
	unsigned long	dwHp	= m_CreatureStatus.m_StatusInfo.m_nMaxHP - m_CreatureStatus.m_nNowHP;
	unsigned long	dwPerHp	= m_CreatureStatus.m_StatusInfo.m_nMaxHP/Siege::SIEGE_ARMS_REPAIR_HP_PER_MATERIAL;						
	unsigned short	wNum	= static_cast<unsigned short>(dwHp / dwPerHp);

	if((dwHp%dwPerHp)!=0)
	{
		wNum++;
	}

	return wNum;
}

bool	CSiegeArms::Build(unsigned char cUpgradeStep)
{
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = cUpgradeStep;

	UpdateObjectInfo();

	// ���� ���Ⱑ �ִ� �ݰ� 5�� �̳��� ����
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_cUpgradeStep;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}

bool	CSiegeArms::Repair(unsigned short wRepairHP)
{
	m_cState = Siege::COMPLETE;
	UpdateObjectInfo(Siege::REPAIR_HP, wRepairHP);

	// ���� ���Ⱑ �ִ� �ݰ� 5�� �̳��� ����
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_CreatureStatus.m_nNowHP;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}

bool	CSiegeArms::Destroy(unsigned char cOffencerNation, bool bTakeGold)
{
	m_cState = Siege::DESTROYED;
	m_cUpgradeStep = 0;
	m_CreatureStatus.m_nNowHP = 0;

	// ���� ���Ⱑ �ִ� �ݰ� 5�� �̳��� ����
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_cUpgradeStep;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_DESTROY_ARMS;

	// CreatureManager ���� ����
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}

bool	CSiegeArms::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd)
{
	m_cState = cState;

	UpdateObjectInfo();

	// ���� ���Ⱑ �ִ� �ݰ� 5�� �̳��� ����
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= dwValue1;
	pktSAC.m_cSubCmd		= cSubCmd;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}


bool	CSiegeArms::Ride(unsigned long dwCID)
{
	if (0 == m_dwRiderCID)
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider && dwCID == m_dwOwnerID)
		{
			m_dwRiderCID = dwCID;
			lpRider->Ride(m_dwCID);
			lpRider->SkillClear();

			// ���ϰ� ũ�� ���Ӽ������� �ð� üũ ��ƾ�� �Űܿ����� ��������!!
			// �߰� ������ ��Ŷ ���� (�ð� üũ������...)
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (lpDBAgentDispatch)
			{
				GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, m_dwCID, 
															0, PktSiegeArmsCmd::SIEGE_RIDE_ARMS, PktBase::NO_SERVER_ERR);
			}

			// Ride �Լ� ��ü���� Ŭ���̾�Ʈ���� ��Ŷ�� �����ش�.
			PktSiegeArmsCmd pktSAC;
			pktSAC.m_dwCID			= GetOwnerID();
			pktSAC.m_dwArmsID		= m_dwCID;
			pktSAC.m_cState			= m_cState;
			pktSAC.m_dwValue		= dwCID;
			pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_RIDE_ARMS;

			char* szPacket = reinterpret_cast<char *>(&pktSAC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
			{
				SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
			}

			return true;
		}
	}

	return false;
}

bool	CSiegeArms::GetOff(unsigned long dwCID)
{
	if (Siege::NOT_RIDER != IsRider(dwCID))
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider)
		{
			m_dwRiderCID = 0;
			lpRider->GetOff();			

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
				CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

			// �������⿡�� ���ȴٴ� ������ DBAgent ������ �˷��ش�.
			if (0 != lpDBAgentDispatch)
			{
				CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();
				GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(AgentSendStream, dwCID, m_dwCID,
					0, PktSiegeArmsCmd::SIEGE_GETOFF_ARMS, PktBase::NO_SERVER_ERR);
			}

			// GetOff �Լ� ��ü���� Ŭ���̾�Ʈ���� ��Ŷ�� �����ش�.
			PktSiegeArmsCmd pktSAC;
			pktSAC.m_dwCID			= GetOwnerID();
			pktSAC.m_dwArmsID		= m_dwCID;
			pktSAC.m_cState			= m_cState;
			pktSAC.m_dwValue		= dwCID;
			pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_GETOFF_ARMS;

			char* szPacket = reinterpret_cast<char *>(&pktSAC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
			{
				SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
			}

			return true;
		}
	}

	return false;
}