#include "stdafx.h"
#include "SiegeObject.h"

#include <Utility/Registry/RegFunctions.h>
#include <Utility/Math/Math.h>

#include <Skill/Spell/Spell.h>
#include <Skill/SkillMgr.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>

#include <Creature/CreatureManager.h>

#include <Map/FieldMap/CellManager.h>

#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>



void CSiegeObject::NormalBehavior(unsigned long dwTick)
{
	if ( !IsEmblem() && !IsGuard() )
	{
		return;
	}

	if (false == Castle::CCastleMgr::GetInstance().IsSiegeTime())
	{
		return;
	}

	// 선공 몹 처리
	if (NULL == m_lpTarget && true == m_MonsterInfo.m_bFirstAttack) 
	{
		SearchPlayer();
	}
}

void CSiegeObject::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if ( !IsEmblem() && !IsGuard() )
	{
		return;
	}

	if (false == Castle::CCastleMgr::GetInstance().IsSiegeTime())
	{
		CancelTarget();
		return;
	}

	// 마법 캐스팅 중일때는.. 아무런 다른 행동을 해서는 안된다.
	if (true == m_bCasting)
	{
		CastingAttackAction();
		return;
	}

	m_lpTarget = m_Threat.GetTarget();
	if (NULL == m_lpTarget ||
		(m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide)))
	{
		CancelTarget();
		return;
	}

	const float fDY = fabs(m_lpTarget->GetCurrentPos().m_fPointY - GetCurrentPos().m_fPointY);
	const float fDX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
	const float fDZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
	const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

	if (IsEmblem() && fDY > Siege::EMBLEM_ATTACK_HEIGHT_ERROR)
	{
		CancelTarget();	
		return;
	}

	if (((fDistance > m_wSearchRange) && false == m_bLongRangeAttacked) || (0 == m_lpTarget->GetStatus().m_nNowHP))
	{
		CancelTarget();	
		return;
	}


	const float fAttackRange = m_MonsterInfo.m_wAttackRange / 100.0f;

	if (fDistance > fAttackRange && 0 >= m_lCurrentFrame)
	{
		// 공격 범위 밖이다.
		if (true == SkillAttack())		// 스킬 공격을 사용하는가 체크
		{
			SkillAttackAction();
		}
/*		else
		{
			m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_WALK;	// 일단 걷기로 세팅
			GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);	// 걷기 모션 정보를 얻어온다.

			// 몬스터가 이동 공격을 할만한 거리인가 체크
			if (fDistance < fAttackRange + m_MotionInfo.m_fVelocity) 
			{
				// 걸어가면서 공격한다.
				WalkAttackAction(fDistance - fAttackRange + 0.1f);
			}
			else
			{
				// 달려간다
				RunAction(fDistance, m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
			}
		}
*/
		if (false == m_bAttacking) 
		{
			m_lCurrentFrame = m_MotionInfo.m_dwFrame;
		}
	}
	else
	{
		// 공격 범위 안이다.
		if (false == m_bAttacking)
		{
			if (true == SkillAttack())	// 스킬 공격을 사용하는가 체크
			{
				SkillAttackAction();
			}
/*			else
			{
				AttackAction();
			}
*/
		}
	}
}



bool CSiegeObject::SkillAttack()
{
	unsigned char cSkillLockCount = m_cUpgradeStep + 1;
	unsigned char cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;

	// MON_TODO : 5단계 마법(LockCount : 4)은 아직 사용할수 없다.
	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}
    	

	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, MAX_PATTERN = 2 };
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
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: 공격 스킬열에 해당하는 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[SkillPattern::ATTACK_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = SkillPattern::ATTACK_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[SkillPattern::ATTACK_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// A		B		D	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: 범위 스킬열에 해당하는 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[SkillPattern::EXTENT_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = SkillPattern::EXTENT_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[SkillPattern::EXTENT_SKILL];
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

void	CSiegeObject::UpdateObjectInfo(unsigned char cHPType, unsigned short wRepairHP)
{
	unsigned short wPrevMaxHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	unsigned short wNowHP = m_CreatureStatus.m_nNowHP;
	int nKID = Siege::GetKID(m_wObjectType, m_cState, m_cUpgradeType, m_cUpgradeStep);

	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (lpProtoType)
	{
		m_CreatureStatus = lpProtoType->m_CreatureStatus;
		m_MonsterInfo = lpProtoType->m_MonsterInfo;

		m_CreatureStatus.m_StatusInfo.CalculateSubStatus();
	}

	switch (cHPType)
	{
	case Siege::NOW_HP:
		m_CreatureStatus.m_nNowHP = wNowHP;
		break;

	case Siege::UPGRADE_HP:
		m_CreatureStatus.m_nNowHP = wNowHP + m_CreatureStatus.m_StatusInfo.m_nMaxHP - wPrevMaxHP;
		SendHPUpdateToDBAgent();
		break;

	case Siege::REPAIR_HP:
		m_CreatureStatus.m_nNowHP = wNowHP + wRepairHP;
		SendHPUpdateToDBAgent(true);
		break;

	case Siege::FULL_HP:
		m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		SendHPUpdateToDBAgent(true);
		break;
	}
}

void	CSiegeObject::UpgradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	int nKID = Siege::GetKID(Siege::EMBLEM, Siege::COMPLETE, cUpgradeType, cUpgradeStep);

	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (lpProtoType)
	{
		// 업그레이드 속도 향상
		m_MonsterInfo.m_cUpgradeSpeed -= static_cast<unsigned char>(m_MonsterInfo.m_cUpgradeSpeed
													* lpProtoType->m_MonsterInfo.m_fUpgradeSpeedUp);

		// 개발 속도 향상
		m_MonsterInfo.m_cDevelopSpeed -= static_cast<unsigned char>(m_MonsterInfo.m_cDevelopSpeed
													* lpProtoType->m_MonsterInfo.m_fDevelopSpeedUp);

		// 방어력 향상
		m_CreatureStatus.m_StatusInfo.m_wArmor += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wArmor
																* lpProtoType->m_MonsterInfo.m_fDefenseUp);
		m_CreatureStatus.m_StatusInfo.m_wMagicResist += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMagicResist
																		* lpProtoType->m_MonsterInfo.m_fDefenseUp);

		// HP 향상
		unsigned short wAddHP = static_cast<unsigned short>(m_CreatureStatus.m_StatusInfo.m_nMaxHP
															* lpProtoType->m_MonsterInfo.m_fHPUp);
		m_CreatureStatus.m_StatusInfo.m_nMaxHP += wAddHP;
		m_CreatureStatus.m_nNowHP += wAddHP;

		// 공격력 향상
		m_CreatureStatus.m_StatusInfo.m_wMinDamage += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMinDamage
																	* lpProtoType->m_MonsterInfo.m_fOffenseUp);
		m_CreatureStatus.m_StatusInfo.m_wMaxDamage += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMaxDamage
																	* lpProtoType->m_MonsterInfo.m_fOffenseUp);
		m_CreatureStatus.m_StatusInfo.m_wMagicPower += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMagicPower
																	* lpProtoType->m_MonsterInfo.m_fOffenseUp);

		// 생성 비용 절감
		m_MonsterInfo.m_dwDevelopGold -= static_cast<unsigned long>(m_MonsterInfo.m_dwDevelopGold
													* lpProtoType->m_MonsterInfo.m_fDevelopGoldDown);

		// 업그레이드 비용 절감
		m_MonsterInfo.m_dwUpgradeGold -= static_cast<unsigned long>(m_MonsterInfo.m_dwUpgradeGold
													* lpProtoType->m_MonsterInfo.m_fUpgradeGoldDown);
	}
}

void	CSiegeObject::DegradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	int nKID = Siege::GetKID(Siege::EMBLEM, Siege::COMPLETE, cUpgradeType, cUpgradeStep);
	const CMonsterMgr::MonsterProtoType* lpProtoTypeEmblem = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

	nKID = GetKID();
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

	if (lpProtoType && lpProtoTypeEmblem)
	{
		unsigned short wNowHP = m_CreatureStatus.m_nNowHP -
			                    static_cast<unsigned short>(m_CreatureStatus.m_nNowHP * lpProtoTypeEmblem->m_MonsterInfo.m_fHPUp);

		m_CreatureStatus = lpProtoType->m_CreatureStatus;
		m_MonsterInfo = lpProtoType->m_MonsterInfo;

		m_CreatureStatus.m_nNowHP = wNowHP;
	}
}

unsigned short CSiegeObject::GetDefaultNowHP()
{
	Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastle( GetCastleID() );
	CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();

	if (lpCastle && lpEmblem)
	{
		int nKID = lpEmblem->GetKID();
		const CMonsterMgr::MonsterProtoType* lpProtoTypeEmblem = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

		if (lpProtoTypeEmblem)
		{
			return m_CreatureStatus.m_nNowHP -	static_cast<unsigned short>(m_CreatureStatus.m_nNowHP * lpProtoTypeEmblem->m_MonsterInfo.m_fHPUp);
		}
	}

	return m_CreatureStatus.m_nNowHP;
}

void	CSiegeObject::SetACTByObjectType()
{
	switch (m_wObjectType)
	{
	case Siege::EMBLEM:						m_cAttackableCreatureType = Creature::ACT_EMBLEM;					break;
	case Siege::GATE:						m_cAttackableCreatureType = Creature::ACT_GATE;						break;
	case Siege::BACKDOOR:					m_cAttackableCreatureType = Creature::ACT_NONE;						break;
	case Siege::CAMP:						m_cAttackableCreatureType = Creature::ACT_CAMP;						break;
	case Siege::CASTLE_ARMS_NPC:			m_cAttackableCreatureType = Creature::ACT_NONE;						break;
	case Siege::GUARD:						m_cAttackableCreatureType = Creature::ACT_GUARD;					break;
	case Siege::SHORT_RANGE_CASTLE_ARMS:	m_cAttackableCreatureType = Creature::ACT_SHORT_RANGE_CASTLE_ARMS;	break;
	case Siege::LONG_RANGE_CASTLE_ARMS:		m_cAttackableCreatureType = Creature::ACT_LONG_RANGE_CASTLE_ARMS;	break;
	case Siege::SHORT_RANGE_SIEGE_ARMS:		m_cAttackableCreatureType = Creature::ACT_SHORT_RANGE_SIEGE_ARMS;	break;
	case Siege::LONG_RANGE_SIEGE_ARMS:		m_cAttackableCreatureType = Creature::ACT_LONG_RANGE_SIEGE_ARMS;	break;
	case Siege::AIRSHIP:					m_cAttackableCreatureType = Creature::ACT_AIRSHIP;					break;
	}
}

void	CSiegeObject::SearchPlayer()
{
	switch (m_wObjectType)
	{
	case Siege::GUARD:
		SearchAirShip();
		break;

	case Siege::EMBLEM:
		SearchEnemyPlayer();
		break;
	}
}

void	CSiegeObject::SearchAirShip()
{
	// TODO : 해상도 조절을 통해 float 계산을 없애보자.
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG1(g_Log, "CID:0X%08 공성 오브젝트가 셀 범위 밖에 있습니다.", m_dwCID);
		return;
	}

	CCell*			pCell = NULL;
	CSiegeObject*	pTempTarget = NULL;
	CSiegeObject*	pCurrentTarget = NULL;

//	const float	fSquareSearchRange = (float)(m_wSearchRange * m_wSearchRange);
	const float fSquareSearchRange = (float)(45 * 45);

	for (int nCellCount = 0; nCellCount < CCell::CONNECT_NUM; ++nCellCount)
	{
		pCell = m_CellPos.m_lpCell->GetConnectCell(nCellCount);
		if (NULL == pCell || false == pCell->IsSiegeObject())
		{
			continue;
		}

		pTempTarget = pCell->GetFirstAirShip();

		while (NULL != pTempTarget)
		{
			if (pTempTarget->GetStatus().m_nNowHP > 0 && CCreature::ENEMY == IsEnemy(pTempTarget))
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

			pTempTarget = pCell->GetNextAirShip();
		}
	}

	if (NULL != pCurrentTarget)
	{
		m_Threat.AddToThreatList(pCurrentTarget, 1);
		m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_SEEN_PLAYER);
	}
}

void	CSiegeObject::SearchEnemyPlayer()
{
	// TODO : 해상도 조절을 통해 float 계산을 없애보자.
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG1(g_Log, "CID:0X%08 공성 오브젝트가 셀 범위 밖에 있습니다.", m_dwCID);
		return;
	}

	CCell*			pCell = NULL;
	CCharacter*		pTempTarget = NULL;
	CCharacter*		pCurrentTarget = NULL;

	//	const float	fSquareSearchRange = (float)(m_wSearchRange * m_wSearchRange);
	const float fSquareSearchRange = (float)(32 * 32);

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
			const float fDistY = fabs(pTempTarget->GetCurrentPos().m_fPointY - GetCurrentPos().m_fPointY);

			if (fDistY <= Siege::EMBLEM_ATTACK_HEIGHT_ERROR &&
				pTempTarget->GetStatus().m_nNowHP > 0 && 
                CCreature::ENEMY == IsEnemy(pTempTarget))
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

void	CSiegeObject::MoveTo(const Position& NewPosition)
{
	CAggresiveCreature::MoveTo(NewPosition, false);

	for (int i=1; i<Siege::AIRSHIP_LIMIT_NUM; ++i)
	{
		if (0 != m_dwRideCID[i])
		{
			CCharacter* lpRideCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwRideCID[i]);
			if (lpRideCharacter)
			{
				lpRideCharacter->MoveTo(NewPosition, false);
			}
		}
	}
}


bool	CSiegeObject::Dead(CAggresiveCreature* pOffencer)
{
	switch (m_wObjectType)
	{
	case Siege::CAMP:				return CampDead(pOffencer);
	case Siege::EMBLEM:				return EmblemDead(pOffencer);

	case Siege::GATE:				return GateDead(pOffencer);
	case Siege::GUARD:				return GuardDead(pOffencer);

	case Siege::SHORT_RANGE_CASTLE_ARMS:
	case Siege::LONG_RANGE_CASTLE_ARMS:
		return CastleArmsDead(pOffencer);

	case Siege::AIRSHIP:
	case Siege::SHORT_RANGE_SIEGE_ARMS:
	case Siege::LONG_RANGE_SIEGE_ARMS:
		return SiegeArmsDead(pOffencer);

	default:						break;
	}

	return false;
}

bool	CSiegeObject::CampDead(CAggresiveCreature* pOffencer)
{
	if (STATE_ID_DIE == m_nCurrentState) { return false; }

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// 크리쳐 매니져에서 삭제 (해당 셀에서도 삭제한다.)
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		if ( pOffencer )
		{
			return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), pOffencer->GetCID(), GetCampID(), 
				pOffencer->GetGID(), 0, PktCampCmd::CAMP_DESTROY, PktBase::NO_SERVER_ERR);
		}
		else
		{
			return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, GetCampID(), 
				0, 0, PktCampCmd::CAMP_DESTROY, PktBase::NO_SERVER_ERR);
		}
	}

	return false;
}

bool	CSiegeObject::EmblemDead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		m_dwEnemyGID = reinterpret_cast<CCharacter*>(pOffencer)->GetGID();
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCastleID(), m_dwCID, 
																m_dwEnemyGID, 0, PktCastleCmd::CASTLE_UPDATE_EMBLEM, PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool	CSiegeObject::GateDead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCastleID(), m_dwCID, 
																0, 0, PktCastleCmd::CASTLE_DESTROY_GATE, PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool	CSiegeObject::GuardDead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_wObjectType = Siege::CASTLE_ARMS_NPC;
	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	SetACTByObjectType();

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCastleID(), m_dwCID, 
																PktCastleCmd::DESTROY, 0, PktCastleCmd::CASTLE_DESTROY_ARMS,
																PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool	CSiegeObject::CastleArmsDead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;
	if ( !IsCastleArms() || IsGuard() ) return false;

	m_wObjectType = Siege::CASTLE_ARMS_NPC;
	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	SetACTByObjectType();

	// 타고 있던 캐릭터는 죽는다.
	for (int i=0; i<Siege::AIRSHIP_LIMIT_NUM; ++i)
	{
		CCharacter* lpRideChar = CCreatureManager::GetInstance().GetCharacter(m_dwRideCID[i]);
		if (lpRideChar)
		{
			m_dwRideCID[i] = 0;
			lpRideChar->GetOff();
			lpRideChar->Kill(pOffencer);
		}
	}

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCastleID(), m_dwCID, 
																PktCastleCmd::DESTROY, 0, PktCastleCmd::CASTLE_DESTROY_ARMS,
																PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool	CSiegeObject::SiegeArmsDead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;
	if ( !IsSiegeArms() ) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	// 타고 있던 캐릭터는 죽는다.
	for (int i=0; i<Siege::AIRSHIP_LIMIT_NUM; ++i)
	{
		CCharacter* lpRideChar = CCreatureManager::GetInstance().GetCharacter(m_dwRideCID[i]);
		if (lpRideChar)
		{
			lpRideChar->GetOff();
			lpRideChar->Kill(pOffencer);
		}

		m_dwRideCID[i] = 0;
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