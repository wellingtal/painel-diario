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
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, attackType.m_wType);
		return false;
	}

	const unsigned short wLockCount = GetSkillLockCount(attackType.m_wType);
	if (wLockCount < 0 || wLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
	{
		ERRLOG3(g_Log, "CID:0x%08x 쓰려는 스킬의 락카운트가 이상합니다. SkillType : 0x%04x, LockCount : %d", 
			m_dwCID, attackType.m_wType, wLockCount);
		return false;
	}

	unsigned char cDefenderNum = attackNode.m_wDefenserNum > AtNode::MAX_DEFENDER_NUM ?
		AtNode::MAX_DEFENDER_NUM : attackNode.m_wDefenserNum; // 최대 방어자 수 제한

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
			// 캐스팅에 실패한 경우
			return Attack(attackType, cDefenderNum, lpAggresiveCreature, attackNode.m_cDefenserJudge, wDefenserMPHeal);
		}
	}
	else
	{
		if (0 == pThisSkill[wLockCount].m_fMaxRange && 0 == pThisSkill[attackType.m_cSkillLockCount].m_fEffectExtent) 
		{
			if (m_dwCID != attackNode.m_dwDefenser[0])
			{
				ERRLOG2(g_Log, "CID:0x%08x 자기자신에게만 쓸 수 있는 스킬입니다. SkillID:0x%04x", 
					m_dwCID, attackType.m_wType);
				return false;
			}
		}

		// 공성병기는 몬스터를 공격할수 없다.
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(attackNode.m_dwDefenser[0]);
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			wError = PktAtAck::FAIL_TO_MONSTER;
			return false;
		}

		// 스킬 거리 체크
		CAggresiveCreature* lpTargetCreature = NULL;

		// Target Creature 얻어오기
		if (0 != GetMapIndex())
		{
			lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
			if (lpTargetCreature && lpTargetCreature->GetMapIndex() != GetMapIndex()) lpTargetCreature = NULL;
		}
		else
		{
			lpTargetCreature = CCreatureManager::GetInstance().GetAggresiveCreature(attackNode.m_dwDefenser[0]);
		}

		// Target Creature 처리하기
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

		// 클라이언트가 넘겨준 타겟들을 체크한다. (범위 마법에 걸리는 타겟은 따로 체크)
		for (unsigned char cDefender = 0; cDefender < cDefenderNum; ++cDefender) 
		{
			// Target Creature 얻기
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
				// 긍정적인 공격(-_-)
				if (Skill::Target::FRIEND == cTargetType)
				{
					wError = PktAtAck::FAIL_FRIENDLY_ATTACK;
					return false;
				}
				// 부정적인 공격(진짜 공격)
				else
				{
					// 자기를 중심으로 하는 범위형 스킬의 경우 타겟을 자신으로 세팅합니다.
					// (이 경우 this는 MultiAttack() 함수가 타켓에서 제외시켜 줍니다.)
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

		// 범위 마법 체크
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
		ERRLOG1(g_Log, "CID:0x%08x 죽은 공성 오브젝트가 공격하려고 하였습니다.", m_dwCID);
		return false;
	}

	if (cDefenderNum > AtNode::MAX_DEFENDER_NUM) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 공성 오브젝트가 공격할 때, 방어자의 숫자가 최대 방어자 숫자를 넘었습니다. 방어자수 : %d", 
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

	if ( IsRidable() )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (!lpRider)
		{
			ERRLOG2(g_Log, "CID:0x%08x 공성 오브젝트에 타고있는 캐릭터가 없습니다. RiderCID : 0x%08x", m_dwCID, m_dwRiderCID);
			return false;
		}

		// 공격시 무적 상태가 풀린다.
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

		// 최대 방어자 수 제한 (몬스터는 캐릭터와는 별도 처리)
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(ppDefenders[cIndex]->GetCID());
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		// TODO : 공격 방향을 설정해줍시다.
		cDefenserJudges[cDefender] = ClientConstants::Judge_Front;
		wDefenserMPHeal[cDefender] = 0;

		const unsigned short nPrevHP = ppDefenders[cIndex]->GetStatus().m_nNowHP;
		const unsigned short nPrevMP = ppDefenders[cIndex]->GetStatus().m_nNowMP;

		const unsigned short wPrevAttackerHP = m_CreatureStatus.m_nNowHP;

		// 대미지 반영
		lpDefenserNode[cDefender].m_wDamage = ppDefenders[cIndex]->ApplyDamage(attackType, this, cOffencerJudge,
			cDefenserJudges[cDefender], wOffencerMPHeal, wDefenserMPHeal[cDefender], wError);

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

			// 공격 패킷 만들기
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

	// 듀얼 상태라면 듀얼 셀로 처리
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
		ERRLOG0(g_Log, "CID:0x%08x 공격 목표 지점의 셀이 존재하지 않습니다.");
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

		// 타겟이 없거나, 방어자 수가 최대가 되면, 루프를 빠져나갑니다.
		while (NULL != lpTempTarget && nDefenderNum < AtNode::MAX_DEFENDER_NUM)
		{
			// 공격에 대한 예외상황
			bool bException = false;

			// 병기에 탑승한 캐릭터는 공격하지 않는다. 대신 병기를 공격한다.
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

			// 겹치는 게 있으면 처리하지 않는다.
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
		SERLOG0(g_Log, "스택 오버런 : 방어자수가 최대치를 넘어셨습니다.");
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

	// 타고 있던 캐릭터는 죽는다.
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

	// 크리쳐 매니져에서 삭제 (해당 셀에서도 삭제한다.)
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	// 중계 서버로 패킷 전송
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

	// 스타트킷 아이템 생성
	Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemProtoTypeID);

	if (NULL == lpItem)
	{
		ERRLOG0(g_Log, "길드 요새 생성 스타트킷 아이템 생성에 실패했습니다.");
		return false;
	}

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter( m_dwOwnerID );

	if (NULL != lpCharacter)
	{
		if (false == lpCharacter->GiveItem(lpItem))
		{
			ERRLOG0(g_Log, "길드 요새 생성 스타트킷을 돌려주는데 실패하였습니다.");

			DELETE_ITEM(lpItem);
			return false;
		}

		// GievItem 으로 스택된 경우
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

	// 우선 임시로 주석 처리.

	if(bMaterial)
	{
		GiveBackSiegeMaterial();
	}	

	// 바닥에 아이템 떨어뜨리기
	CCell::ItemInfo itemInfo;
	GetCellPos().m_lpCell->SetItem(GetCurrentPos(), lpItem, 0, 0, CCell::NONE, itemInfo);

	return true;
}

bool	CSiegeArms::GiveBackSiegeMaterial()
{
	// 업그레이드에 따른 갯수 설정
	unsigned char cNum = 0;
	
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(m_wObjectType);
	
	if (lpProtoType)
	{
		cNum = Siege::SIEGE_ARMS_UPGRADE_MATERIAL_NUM / 2;		
	}

	if (cNum > 0)
	{
		// 자재 아이템 돌려주기
		Item::CItem* lpMaterial = Item::CItemFactory::GetInstance().CreateItem(Item::EtcItemID::SIEGE_MATERIAL_ID);

		if (NULL == lpMaterial)
		{
			ERRLOG0(g_Log, "공성 병기 제작용 자재 아이템 생성에 실패했습니다.");
			return false;
		}

		lpMaterial->SetNumOrDurability(cNum);

		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwOwnerID);

		if (NULL != lpCharacter)
		{
			if (false == lpCharacter->GiveItem(lpMaterial))
			{
				ERRLOG0(g_Log, "공성 병기 제작용 자재를 돌려주는데 실패하였습니다.");

				DELETE_ITEM(lpMaterial);
				return false;
			}

			// GievItem 으로 스택된 경우
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

	// 공성 병기가 있는 반경 5셀 이내에 전송
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_cUpgradeStep;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}

bool	CSiegeArms::Repair(unsigned short wRepairHP)
{
	m_cState = Siege::COMPLETE;
	UpdateObjectInfo(Siege::REPAIR_HP, wRepairHP);

	// 공성 병기가 있는 반경 5셀 이내에 전송
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_CreatureStatus.m_nNowHP;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
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

	// 공성 병기가 있는 반경 5셀 이내에 전송
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= m_cUpgradeStep;
	pktSAC.m_cSubCmd		= PktSiegeArmsCmd::SIEGE_DESTROY_ARMS;

	// CreatureManager 에서 삭제
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd);
	}

	return true;
}

bool	CSiegeArms::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd)
{
	m_cState = cState;

	UpdateObjectInfo();

	// 공성 병기가 있는 반경 5셀 이내에 전송
	PktSiegeArmsCmd pktSAC;
	pktSAC.m_dwCID			= GetOwnerID();
	pktSAC.m_dwArmsID		= m_dwCID;
	pktSAC.m_cState			= m_cState;
	pktSAC.m_dwValue		= dwValue1;
	pktSAC.m_cSubCmd		= cSubCmd;

	char* szPacket = reinterpret_cast<char *>(&pktSAC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
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

			// 부하가 크면 게임서버에로 시간 체크 루틴을 옮겨오도록 수정하자!!
			// 중계 서버로 패킷 전송 (시간 체크때문에...)
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (lpDBAgentDispatch)
			{
				GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, m_dwCID, 
															0, PktSiegeArmsCmd::SIEGE_RIDE_ARMS, PktBase::NO_SERVER_ERR);
			}

			// Ride 함수 자체에서 클라이언트에게 패킷을 보내준다.
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

			// 공성병기에서 내렸다는 정보를 DBAgent 서버에 알려준다.
			if (0 != lpDBAgentDispatch)
			{
				CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();
				GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(AgentSendStream, dwCID, m_dwCID,
					0, PktSiegeArmsCmd::SIEGE_GETOFF_ARMS, PktBase::NO_SERVER_ERR);
			}

			// GetOff 함수 자체에서 클라이언트에게 패킷을 보내준다.
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