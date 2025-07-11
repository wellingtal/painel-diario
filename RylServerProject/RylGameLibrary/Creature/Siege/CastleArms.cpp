#include "stdafx.h"
#include "CastleArms.h"

#include <Skill/SkillTable.h>
#include <Skill/SkillMgr.h>

#include <Utility/Math/Math.h>

#include <Item/ItemFactory.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/DuelMap/DuelCellManager.h>

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


CCastleArms::CCastleArms(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject)
: CSiegeObject(MonsterCreate, CastleObject)
{
}

CCastleArms::~CCastleArms()
{
}


bool	CCastleArms::AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	if (Siege::CASTLE_ARMS_NPC == m_wObjectType)
	{
		wError = PktAtAck::SERVER_ERROR;
		return true;
	}

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

			Creature::CreatureType eCreatureType =  Creature::GetCreatureType(attackNode.m_dwDefenser[cDefender]);
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

bool	CCastleArms::Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)
{
	if (Siege::CASTLE_ARMS_NPC == m_wObjectType)
	{
		return false;
	}

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


bool	CCastleArms::MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)
{
	if (Siege::LONG_RANGE_CASTLE_ARMS != m_wObjectType)
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




bool	CCastleArms::Dead(CAggresiveCreature* pOffencer)
{
	if (Siege::CASTLE_ARMS_NPC == m_wObjectType)
	{
		return true;
	}

	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	//m_wObjectType = Siege::CASTLE_ARMS_NPC;
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
			//lpRider->GetOff();
			lpRider->Kill(pOffencer);
		}

		//m_dwRiderCID = 0;
		m_dwOwnerID = 0; 
	}

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), pOffencer->GetCID(), 
																GetCastleID(), m_dwCID, PktCastleCmd::DESTROY, 0,
																PktCastleCmd::CASTLE_DESTROY_ARMS, PktBase::NO_SERVER_ERR);
	}

	return false;
}


bool	CCastleArms::Upgrade(unsigned char cUpgradeStep)
{
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = cUpgradeStep;

	UpdateObjectInfo(Siege::UPGRADE_HP);

	// 수성 병기가 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cUpgradeStep;	// 업그레이드 단계
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_UPGRADE_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleArms::Repair(unsigned short wRepairHP)
{
	m_cState = Siege::COMPLETE;
	UpdateObjectInfo(Siege::REPAIR_HP, wRepairHP);

	// 수성 병기가 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= wRepairHP;
	pktCC.m_dwValue2			= m_CreatureStatus.m_nNowHP;	// 현재 HP
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_REPAIR_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleArms::Destroy(unsigned char cOffencerNation, bool bTakeGold)
{
	if (!IsCastleArms() || Siege::CASTLE_ARMS_NPC == m_wObjectType)
	{
		m_dwOwnerID = 0;
		return false;
	}

	// 생산 비용의 절반을 돌려준다.
	if (true == bTakeGold)
	{
        CCharacter* lpOwner = CCreatureManager::GetInstance().GetCharacter(m_dwOwnerID);
		if (0 != lpOwner)
		{
			lpOwner->AddGold(m_MonsterInfo.m_dwDevelopGold / 2, true);
		}
	}

	m_wObjectType = Siege::CASTLE_ARMS_NPC;
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = 0;
	m_dwOwnerID = 0;

	UpdateObjectInfo();

	m_CreatureStatus.m_nNowHP = 0;

	// 수성 병기가 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCID				= 0;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_wObjectType;	// 병기 관리 NPC Type ID
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_DESTROY_ARMS;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleArms::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwOnwerID, unsigned char cSubCmd)
{
	if (!IsCastleArms())
	{
		return false;
	}

	m_cState = cState;

	if (cSubCmd == PktCastleCmd::CASTLE_CREATE_ARMS)
	{
		dwValue2 = (dwValue2 & 0x0000FFFF);
		m_wObjectType = static_cast<unsigned short>(dwValue2);
		m_dwOwnerID = dwOnwerID;

		// 개인 금고에서 dwValue1 만큼의 돈을 깍는다.
		CCharacter* lpOwner = CCreatureManager::GetInstance().GetCharacter(m_dwOwnerID);
		if (0 != lpOwner)
		{
			lpOwner->DeductGold(dwValue1, true);
		}

		UpdateObjectInfo(Siege::FULL_HP);
	}
	else if (cSubCmd == PktCastleCmd::CASTLE_REPAIR_ARMS)
	{
		// 개인 금고에서 dwValue1 만큼의 돈을 깍는다.
		CCharacter* lpOwner = CCreatureManager::GetInstance().GetCharacter(m_dwOwnerID);
		if (0 != lpOwner)
		{
			lpOwner->DeductGold(dwValue1, true);
		}

		UpdateObjectInfo();
	}
	else
	{
		UpdateObjectInfo();
	}

	// 수성 병기가 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCID				= m_dwOwnerID;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= dwValue1;
	pktCC.m_dwValue2			= dwValue2;
	pktCC.m_cSubCmd				= cSubCmd;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleArms::ChangeType(unsigned short wType)
{
	m_wObjectType = wType;
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = 0;

	UpdateObjectInfo();

	// 수성 병기가 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= wType;		// 병기 타입
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_CREATE_ARMS_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}


bool	CCastleArms::Ride(unsigned long dwCID)
{
	if (0 == m_dwRiderCID)
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider && dwCID == m_dwOwnerID)
		{
			m_dwRiderCID = dwCID;
			lpRider->Ride(m_dwCID);
			lpRider->SkillClear();

			// 중계 서버에 보내줘야 사용하고 있음을 체크할 수 있다.
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (NULL == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "에이전트 얻기 실패.");	
				return false;
			}
			else
			{
				GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(),
					dwCID, m_dwCampOrCastleID, m_dwCID, 0, 0, PktCastleCmd::CASTLE_RIDE_ARMS, PktBase::NO_SERVER_ERR);
			}

			// Ride 함수 자체에서 패킷을 보내준다.
			PktCastleCmd pktCC;
			pktCC.m_dwCastleID			= GetCastleID();
			pktCC.m_dwCastleObjectID	= m_dwCID;
			pktCC.m_cState				= m_cState;
			pktCC.m_dwValue1			= dwCID;		// 탑승자 CID
			pktCC.m_dwValue2			= 0;
			pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_RIDE_ARMS;

			char* szPacket = reinterpret_cast<char *>(&pktCC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
			{
				SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
			}

			return true;
		}
	}

	return false;
}

bool	CCastleArms::GetOff(unsigned long dwCID)
{
	if (Siege::NOT_RIDER != IsRider(dwCID))
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider)
		{
			m_dwRiderCID = 0;
			lpRider->GetOff();
			
			// 중계 서버에 보내줘야 사용하고 있지 않음을 체크할 수 있다.
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (NULL == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "에이전트 얻기 실패.");	
				return false;
			}
			else
			{
				GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(),
					dwCID, m_dwCampOrCastleID, m_dwCID, 0, 0, PktCastleCmd::CASTLE_GETOFF_ARMS, PktBase::NO_SERVER_ERR);
			}

			// GetOff 함수 자체에서 패킷을 보내준다.
			PktCastleCmd pktCC;
			pktCC.m_dwCastleID			= GetCastleID();
			pktCC.m_dwCastleObjectID	= m_dwCID;
			pktCC.m_cState				= m_cState;
			pktCC.m_dwValue1			= dwCID;		// 내린 사람 CID
			pktCC.m_dwValue2			= 0;
			pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_GETOFF_ARMS;

			char* szPacket = reinterpret_cast<char *>(&pktCC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
			{
				SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
			}

			return true;
		}
	}

	return false;
}