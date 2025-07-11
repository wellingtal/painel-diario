
#include "stdafx.h"

#include <Utility/Registry/RegFunctions.h>
#include <Utility/Math/Math.h>

#include <Skill/Spell/Spell.h>
#include <Creature/CreatureManager.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <network/packet/packetstruct/serverinfo.h>
#include <mmsystem.h>//hayzohar added this
#include "Monster.h"
#include "PatternMonster.h"

//---------------------------------------------------------------------------------------
// 기본 몬스터

///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::CheckPartyTarget
//
// Description : 몬스터 파티의 타겟을 체크해서 가장 가까운 타켓을 공격한다.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : Attack 행동으로 변경되면 true 리턴, 행동에 변화가 없으면 false 리턴
///////////////////////////////////////////////////////////////////////////////////
static unsigned long LastTime=0;

bool CMonster::CheckPartyTarget(void)
{
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(GetParty());
	if (NULL == lpParty) 
	{
		return false; 
	}

	CMonsterParty::PartyTargetSet& targetSet = lpParty->GetPartyTargetSet();
	if (true == targetSet.empty()) 
	{
		return false; 
	}

	typedef std::map<float, CAggresiveCreature*>	DistanceTargetMap;

	CAggresiveCreature* lpTarget = NULL;
	DistanceTargetMap attackTargetMap;
	
	CMonsterParty::PartyTargetSet::iterator itr = targetSet.begin();
	CMonsterParty::PartyTargetSet::iterator end = targetSet.end();

	for (; itr != end; )
	{
		unsigned long dwTargetCID = *itr;

		// 없는 크리쳐인지 체크
		if (0 != GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
			if (NULL != lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (NULL != lpVirtualMonsterMgr) 
				{
					lpTarget = lpVirtualMonsterMgr->GetAggresiveCreature(dwTargetCID);
				}
			}
		}
		else
		{
			lpTarget = CCreatureManager::GetInstance().GetAggresiveCreature(dwTargetCID);
		}

		if (NULL == lpTarget || true == lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
		{
			itr = targetSet.erase(itr);
		}
		else
		{
			const Position& targetPos = lpTarget->GetCurrentPos();

			// 거리 계산
			float fSquareTargetDistance = (m_CurrentPos.m_fPointX - targetPos.m_fPointX) * (m_CurrentPos.m_fPointX - targetPos.m_fPointX) + 
                (m_CurrentPos.m_fPointZ - targetPos.m_fPointZ) * (m_CurrentPos.m_fPointZ - targetPos.m_fPointZ);

			float fSquareHelpDistance = (lpParty->GetHelpRange() * lpParty->GetHelpRange());
			if (fSquareTargetDistance <= fSquareHelpDistance)
			{
				attackTargetMap.insert(make_pair(fSquareTargetDistance, lpTarget));
			}

			++itr;
		}
	}

	// 거리가 가장 가까운 타겟을 가져온다.
	if (false == attackTargetMap.empty())
	{
		const float fAttackRange = m_MonsterInfo.m_wAttackRange / 100.0f;

		DistanceTargetMap::iterator itr = attackTargetMap.begin();
		const float fDistance = sqrtf(itr->first);
		lpTarget = itr->second;

		m_Threat.AddToThreatList(lpTarget, 1);
		m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_SEEN_PLAYER);
		if (fAttackRange < fDistance || GetPattern() == MonsterInfo::PATTERN_MAGE || GetPattern() == MonsterInfo::PATTERN_ACOLYTE)
		{
			m_bLongRangeAttacked = true;
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Function : CMonster::SearchPlayer
//
// Description : 지역내 플레이어 찾기
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CMonster::SearchPlayer(void)
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
			if (pTempTarget->GetStatus().m_nNowHP > 0 && false == pTempTarget->IsRideArms()) 
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
						// edith 2008.02.13 선공몹 처리확율
						// 레벨차에 따른 선공 확률
						// 레벨 갭을 리턴한다.
//						if ((CalculateLevelGap(pTempTarget) + 10) * 2 > static_cast<int>(Math::Random::ComplexRandom(100)))
						int iGap = CalculateLevelGap(pTempTarget);

						if(pTempTarget->GetAbilityValue(Skill::Type::AB_NOFIRSTATK))
						{
							// 선공 금지 어빌리티
							if ( iGap > pTempTarget->GetAbilityValue(Skill::Type::AB_NOFIRSTATK))
							{	
								pCurrentTarget = pTempTarget;
								break;
							}
						}
						else
						{
							// 100퍼센트 선공
							if ( iGap >= 0)
							{	
								pCurrentTarget = pTempTarget;
								break;
							}
							else if( iGap >= -5 && static_cast<int>(Math::Random::ComplexRandom(2)) == 1)
							{
								// 50프로 선공
								pCurrentTarget = pTempTarget;
								break;
							}
						}
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



void CMonster::NormalBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (m_nLeaveMovingNum != 0)
	{
		m_nLeaveMovingNum--;
		if (F_CELL_CANNOTCHANGE == MoveTo(CalculateCoor(), false)) 
		{
			ms_NormalBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MIN);
			m_nLeaveMovingNum = 0;
		}
		m_lCurrentFrame = m_MotionInfo.m_dwFrame;
		return;
	}


	// 파티가 있고, 파티가 싸우고 있다면...
	// 싸우는 상대중 일정 범위 내에서.. 자신에게 가까운 녀석에게 달려간다.
	// 자신의 시야 안에 들어오면 Threat 리스트에 해당 몬스터를 추가하고,
	// Attack 상태로 만든다.
	if (GetParty())
	{
		if (CheckPartyTarget()) return;
	}
	
	
	// 선공 몹 처리
	if (NULL == m_lpTarget && true == m_MonsterInfo.m_bFirstAttack) 
	{
		SearchPlayer();
	}
	
	// TODO : 이동 패턴에 따른 처리 (루트형이 아직 미결)
	if (CCellManager::GetInstance().IsMoving() && m_nNormalMovingDelay <= 0 &&
		CCellManager::GetInstance().GetMovingNum() > Math::Random::ComplexRandom(100))
	{
		const float fOriginalDX = m_OriginalPosition.m_fPointX - GetCurrentPos().m_fPointX;
		const float fOriginalDZ = m_OriginalPosition.m_fPointZ - GetCurrentPos().m_fPointZ;

		const float fOriginalDistance = (fOriginalDX * fOriginalDX) + (fOriginalDZ * fOriginalDZ);

		if (fOriginalDistance > (MoveRange * MoveRange) && m_nMovingPattern == AREA)
		{
			m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_RUN;
			GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);
			m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
													m_OriginalPosition.m_fPointX, m_OriginalPosition.m_fPointZ);
		}
		else
		{
			m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_WALK;
			GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);
			m_MotionInfo.m_fDirection = (Math::Random::ComplexRandom(62832)) / 10000.0f;
		}

		if (m_nMovingPattern == FIXED)
		{
			m_MotionInfo.m_fDirection = 0;
			m_MotionInfo.m_fVelocity = 0;
		}

		m_nNormalMovingDelay = SendMove(RUN_ANI_LIMIT_MAX);
		ms_NormalBehaviorSendCount += m_nNormalMovingDelay;

		m_nLeaveMovingNum--;
		MoveTo(CalculateCoor(), false);
		m_lCurrentFrame = m_MotionInfo.m_dwFrame;
	} 
	else 
	{
		m_nNormalMovingDelay -= RATE_MOVING_AND_PLAYER;
		m_lCurrentFrame = FPS;
	}	
}

void CMonster::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	m_lpTarget = m_Threat.GetTarget();
	if (NULL == m_lpTarget || MonsterInfo::PATTERN_BG == m_MonsterInfo.m_cSkillPattern) 
	{
		CancelTarget();
		return;
	}
	else if (m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
	{
		CancelTarget();
		return;
	}

	// 병기에 탑승한 캐릭터는 때리지 않는다.
	if (Creature::CT_PC == Creature::GetCreatureType(m_lpTarget->GetCID()))
	{
		CCharacter* lpTargetChar = reinterpret_cast<CCharacter*>(m_lpTarget);
		if (lpTargetChar->IsRideArms())
		{
			CancelTarget();
			return;
		}
	}

	const float fDX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
	const float fDZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
	const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

	// 검색할수 있는 영역보다 넓거나 HP가 0이면 캔슬한다.
	if (((fDistance > m_wSearchRange) && false == m_bLongRangeAttacked) || (0 == m_lpTarget->GetStatus().m_nNowHP))
	{
		CancelTarget();		
		return;
	}
	
	// 파티의 타겟으로 집어 넣는다.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
	if (NULL != lpParty)
	{
		// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
		if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
		{
			lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
			// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
		}
		else if (GetPattern() == MonsterInfo::PATTERN_BOSS ||
			GetPattern() == MonsterInfo::PATTERN_NAMED ||
			GetPattern() == MonsterInfo::PATTERN_CHIEF)
		{
			lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
			// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
		}
	}

	const float fAttackRange = m_MonsterInfo.m_wAttackRange / 100.0f;

	if (fDistance > fAttackRange && 0 >= m_lCurrentFrame)
	{
		// 공격 범위 밖이다.
		m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_WALK;	// 일단 걷기로 세팅
		GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);	// 걷기 모션 정보를 얻어온다.

		// 몬스터가 이동 공격을 할만한 거리인가 체크
		if (fDistance < fAttackRange + m_MotionInfo.m_fVelocity)
		{
			// edith 2009.08.06 공격시 걸어서 공격을 없앤다.
			// 달려간다
			RunAction(fDistance, m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

			// 걸어가면서 공격
//			WalkAttackAction(fDistance - fAttackRange + 0.1f);

			// 내 활동구역을 벗어나따. 돌아가자~!
			if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
			{
				m_wSearchRange = OutsideSearchRange;
				CancelTarget();
			}

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
			AttackAction();
		}
	}
}

void CMonster::ReturnBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (m_nLeaveMovingNum <= 0)
	{
		m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_RUN;
		GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);

		const float fOriginalDX = m_OriginalPosition.m_fPointX - GetCurrentPos().m_fPointX;
		const float fOriginalDZ = m_OriginalPosition.m_fPointZ - GetCurrentPos().m_fPointZ;
		const float fOriginalDistance = (fOriginalDX * fOriginalDX) + (fOriginalDZ * fOriginalDZ);	

		m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
											m_OriginalPosition.m_fPointX, m_OriginalPosition.m_fPointZ);

		unsigned short nAniNum = (unsigned short)(sqrtf(fOriginalDistance) / m_MotionInfo.m_fVelocity);
		nAniNum = (nAniNum > RUN_ANI_LIMIT_MAX) ? RUN_ANI_LIMIT_MAX : nAniNum;
		if (nAniNum == 0)
		{
			m_wSearchRange = m_wDefaultSearchRange;
			m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ARRIVAL_SITE);
			m_Threat.ClearThreatList();
			CancelTarget();

			return;
		}

		ms_ReturnBehaviorSendCount += SendMove(nAniNum);
	}

	m_nLeaveMovingNum--;
	if (F_CELL_CANNOTCHANGE == MoveTo(CalculateCoor(), false)) 
	{
		ms_ReturnBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MIN);
		m_nLeaveMovingNum = 0;
	}
	m_lCurrentFrame = m_MotionInfo.m_dwFrame;	
}

void CMonster::EscapeBehavior(void)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (NULL == m_lpTarget) {
		m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_LEAVE_PLAYER);
		return;
	}

	m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_RUN;
	GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);

	m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
		m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

	m_MotionInfo.m_fDirection += Math::Const::PI;
	if (m_MotionInfo.m_fDirection >= (Math::Const::PI * 2.0f)) {
		m_MotionInfo.m_fDirection -= (Math::Const::PI * 2.0f);
	}
	
	ms_EscapeBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MIN);
	MoveTo(CalculateCoor(), false);
	m_nLeaveMovingNum = 0;

	m_lCurrentFrame = m_MotionInfo.m_dwFrame;	
}

void CMonster::DeadBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (dwTick < m_dwLastTime + m_MonsterInfo.m_dwRespawnTime)
	{
		m_lCurrentFrame = FPS;
		return;
	}

//	m_Threat.ClearThreatList();
//	CancelTarget();

	m_nLeaveMovingNum = 0;
	Respawn(dwTick);
}


//---------------------------------------------------------------------------------------
// 소환 몬스터

void CSummonMonster::NormalBehavior(unsigned long dwTick)
{
	if (NULL == m_lpMaster)
	{
		CMonster::NormalBehavior(dwTick);
		return;
	}

	--m_nLeaveMovingNum;
	if (0 >= m_nLeaveMovingNum)
	{
		const float fDX = m_lpMaster->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
		const float fDZ = m_lpMaster->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
		const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

		if (fDistance <= SUPPORT_DISTANCE) { return; }

		m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_RUN;
		GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);
		m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
			m_lpMaster->GetCurrentPos().m_fPointX, m_lpMaster->GetCurrentPos().m_fPointZ);

		unsigned short nAniNum = static_cast<unsigned short>(fDistance / m_MotionInfo.m_fVelocity);
		nAniNum = (nAniNum > RUN_ANI_LIMIT_MAX) ? RUN_ANI_LIMIT_MAX : nAniNum;
		if (0 == nAniNum) { return; }

		ms_NormalBehaviorSendCount += SendMove(nAniNum);
	}

	if (F_CELL_CANNOTCHANGE == MoveTo(CalculateCoor(), false)) 
	{
		m_nLeaveMovingNum = 0;
		ms_ReturnBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MIN);
	}
	m_lCurrentFrame = m_MotionInfo.m_dwFrame;	
}

void CSummonMonster::AttackBehavior(unsigned long dwTick)
{
	if (NULL != m_lpMaster)
	{
		m_OriginalPosition = m_lpMaster->GetCurrentPos();
	}

	CMonster::AttackBehavior(dwTick);
}

void CSummonMonster::ReturnBehavior(unsigned long dwTick)
{
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ARRIVAL_SITE);
	m_Threat.ClearThreatList();
	CancelTarget();
}

void CSummonMonster::DeadBehavior(unsigned long dwTick)
{
	// 소환 몬스터는 사망하면 삭제될 때까지 대기한다. 삭제는 메인 루프에서 일괄 처리한다.
}


//---------------------------------------------------------------------------------------
// 명예의 석상

void CStatue::NormalBehavior(unsigned long dwTick)
{

	unsigned char cServerZone = CServerSetup::GetInstance().GetServerZone();
	if (SERVER_ID::ZONE3 == cServerZone || SERVER_ID::STATUE_ZONE == cServerZone || SERVER_ID::CAPITAL == cServerZone)
	{
        CCreatureManager::GetInstance().ProcessAllCharacter(CStatueInfo(this, false, 0));
	}

	// 1레벨
	if (MonsterInfo::STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::POWER_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::POWER_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::INT_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::INT_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::EXP_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::EXP_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||

		//2레벨
	    MonsterInfo::STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::POWER_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::POWER_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::INT_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::INT_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::EXP_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::EXP_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
		MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID)
	{
		if (dwTick > m_dwLastTime + STATUE_LOADING_TIME)
		{
			Dead(NULL);
			return;
		}
	}
	else if (MonsterInfo::BG_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID || 
			 MonsterInfo::BG_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
			 // 2레벨
			 MonsterInfo::BG_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID || 
			 MonsterInfo::BG_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID)

	{
		if (dwTick > m_dwLastTime + BG_STATUE_LOADING_TIME)
		{
			Dead(NULL);
			return;
		}
	}

	m_lCurrentFrame = UPDATE_INFO_CYCLE;
	//unsigned long now=0;
	//CCreatureManager::GetInstance().ProcessAllCharacter(CStatueInfo(this, false, 0));
	//if (MonsterInfo::STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::POWER_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::POWER_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::INT_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::INT_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::EXP_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::EXP_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||

	//	//2레벨
	//    MonsterInfo::STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::POWER_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::POWER_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::INT_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::INT_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::EXP_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::EXP_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID ||
	//	MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID)
	//{
	//	now = timeGetTime();
	//	if(LastTime == 0)
	//		LastTime=timeGetTime();

	//	if (dwTick > m_dwLastTime + STATUE_LOADING_TIME)//((now- LastTime > STATUE_LOADING_TIME) && (LastTime != 0))//was(dwTick > m_dwLastTime + STATUE_LOADING_TIME)
	//	{
	//		Dead(NULL);
	//		LastTime=0;
	//		return;
	//	}

	//}
	//else if (MonsterInfo::BG_STATUE_HUMAN_LOADING1 == m_MonsterInfo.m_dwKID || 
	//		 MonsterInfo::BG_STATUE_AKHAN_LOADING1 == m_MonsterInfo.m_dwKID ||
	//		 // 2레벨
	//		 MonsterInfo::BG_STATUE_HUMAN_LOADING2 == m_MonsterInfo.m_dwKID || 
	//		 MonsterInfo::BG_STATUE_AKHAN_LOADING2 == m_MonsterInfo.m_dwKID)

	//{
	//	if ((now- LastTime > BG_STATUE_LOADING_TIME) && (LastTime != 0))
	//	{
	//		Dead(NULL);
	//		LastTime=0;
	//		return;
	//	}
	//}
	////unsigned char cServerZone = CServerSetup::GetInstance().GetServerZone();
	////if (SERVER_ID::ZONE3 == cServerZone || SERVER_ID::STATUE_ZONE == cServerZone)//hayzohar wasnt commented out
	////{
 //       CCreatureManager::GetInstance().ProcessAllCharacter(CStatueInfo(this, false, 0));
	////}
	//// 1레벨

}

void CStatue::AttackBehavior(unsigned long dwTick)
{
	m_lpTarget = m_Threat.GetTarget();
	if (NULL == m_lpTarget || m_wDefaultSearchRange < m_CurrentPos.GetDistance(m_lpTarget->GetCurrentPos()))
	{
		CancelTarget();
	}
	else if (m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
	{
		CancelTarget();
	}

	NormalBehavior(dwTick);
}

void CStatue::ReturnBehavior(unsigned long dwTick)
{
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ARRIVAL_SITE);
	NormalBehavior(dwTick);
	
}

void CStatue::DeadBehavior(unsigned long dwTick)
{
	// 구조물은 죽은 후 자동으로 리스폰되지 않는다. 링크된 다른 구조물에 의해 리스폰.
}

// -------------------------------------------------------------------------------------
// 수집 몬스터

void CGatherMonster::NormalBehavior(unsigned long dwTick)
{
	return;
}

void CGatherMonster::AttackBehavior(unsigned long dwTick)
{
	m_lpTarget = m_Threat.GetTarget();
	if (NULL != m_lpTarget)
	{
		// 파티의 타겟으로 집어 넣는다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
		if (NULL != lpParty)
		{
			// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
			if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
			{
				// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
				lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
				return;
			}
		}
	}

	CancelTarget(true);
}

void CGatherMonster::ReturnBehavior(unsigned long dwTick)
{
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ARRIVAL_SITE);
}


// -------------------------------------------------------------------------------------
// 오브젝트 몬스터 (꽃, 나무, 버섯류)

void CObjectMonster::NormalBehavior(unsigned long dwTick)
{
	return;
}

void CObjectMonster::AttackBehavior(unsigned long dwTick)
{
	m_lpTarget = m_Threat.GetTarget();
	if (NULL != m_lpTarget)
	{
		// 파티의 타겟으로 집어 넣는다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
		if (NULL != lpParty)
		{
			// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
			if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
			{
				// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
				lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
				return;
			}
		}
	}

	CancelTarget(true);
}

void CObjectMonster::ReturnBehavior(unsigned long dwTick)
{
	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ARRIVAL_SITE);
}

//---------------------------------------------------------------------------------------
// 패턴 몬스터

void CSkillMonster::NormalBehavior(unsigned long dwTick)
{
	CMonster::NormalBehavior(dwTick);

	// Chant 스킬 사용을 끈다.
	if (GetPattern() == MonsterInfo::PATTERN_BOSS ||
		GetPattern() == MonsterInfo::PATTERN_NAMED ||
		GetPattern() == MonsterInfo::PATTERN_CHIEF)
	{
		m_SpellMgr.GetCastingInfo().DisableChant();
	}
}

void CSkillMonster::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	// 마법 캐스팅 중일때는.. 아무런 다른 행동을 해서는 안된다.
	if (true == m_bCasting)
	{
		CastingAttackAction();
		return;
	}

	m_lpTarget = m_Threat.GetTarget();
	if (NULL == m_lpTarget || MonsterInfo::PATTERN_BG == m_MonsterInfo.m_cSkillPattern)
	{
		CancelTarget();
		return;
	}
	else if (m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
	{
		CancelTarget();
		return;
	}

	// 병기에 탑승한 캐릭터는 때리지 않는다.
	if (Creature::CT_PC == Creature::GetCreatureType(m_lpTarget->GetCID()))
	{
		CCharacter* lpTargetChar = reinterpret_cast<CCharacter*>(m_lpTarget);
		if (lpTargetChar->IsRideArms())
		{
			CancelTarget();
			return;
		}
	}

	const float fDX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
	const float fDZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
	const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

	if (((fDistance > m_wSearchRange) && false == m_bLongRangeAttacked) || (0 == m_lpTarget->GetStatus().m_nNowHP))
	{
		CancelTarget();	
		return;
	}


	// 파티의 타겟으로 집어 넣는다.
	CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
	if (NULL != lpParty)
	{
		// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
		if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
		{
			lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
			// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
		}
		else if (GetPattern() == MonsterInfo::PATTERN_BOSS ||
			GetPattern() == MonsterInfo::PATTERN_NAMED ||
			GetPattern() == MonsterInfo::PATTERN_CHIEF)
		{
			lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
			// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
		}
	}

	const float fAttackRange = m_MonsterInfo.m_wAttackRange / 100.0f;

	if (fDistance > fAttackRange && 0 >= m_lCurrentFrame)
	{
		// 공격 범위 밖이다.
		if (true == SkillAttack())		// 스킬 공격을 사용하는가 체크
		{
			SkillAttackAction();
		}
		else
		{
			m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_WALK;	// 일단 걷기로 세팅
			GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);	// 걷기 모션 정보를 얻어온다.

			// 몬스터가 이동 공격을 할만한 거리인가 체크
			if (fDistance < fAttackRange + m_MotionInfo.m_fVelocity) 
			{
				// edith 2009.08.06 공격시 걸어서 공격을 없앤다.
				// 달려간다
				RunAction(fDistance, m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

				// 걸어가면서 공격한다.
//				WalkAttackAction(fDistance - fAttackRange + 0.1f);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
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
			else
			{
				AttackAction();
			}
		}
	}
}


void CMageMonster::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	// 일정 거리를 유지하면서 마법 공격을 사용
	if (GetParty() == NULL) CSkillMonster::AttackBehavior(dwTick);
	else
	{
		// 마법 캐스팅 중일때는.. 아무런 다른 행동을 해서는 안된다.
		if (true == m_bCasting)
		{
			CastingAttackAction();
			return;
		}

		m_lpTarget = m_Threat.GetTarget();
		if (NULL == m_lpTarget)
		{
			CancelTarget();
			return;
		}
		else if (m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
		{
			CancelTarget();
			return;
		}

		// 병기에 탑승한 캐릭터는 때리지 않는다.
		if (Creature::CT_PC == Creature::GetCreatureType(m_lpTarget->GetCID()))
		{
			CCharacter* lpTargetChar = reinterpret_cast<CCharacter*>(m_lpTarget);
			if (lpTargetChar->IsRideArms())
			{
				CancelTarget();
				return;
			}
		}

		const float fDX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
		const float fDZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
		const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

		if (((fDistance > m_wSearchRange) && false == m_bLongRangeAttacked) || (0 == m_lpTarget->GetStatus().m_nNowHP))
		{
			CancelTarget();	
			return;
		}

		// 파티의 타겟으로 집어 넣는다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
		if (NULL != lpParty)
		{
			// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
			if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
			{
				lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
				// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
			}
		}

		// 거리 체크
		float fToMemberX, fToMemberZ, fToTargetX, fToTargetZ;
		float fDstX, fDstZ;
		fToTargetX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
		fToTargetZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;

		CAggresiveCreature* lpHiLVMember = FindHighestLVMember();
		if (lpHiLVMember)
		{
			fToMemberX = lpHiLVMember->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
			fToMemberZ = lpHiLVMember->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
		}
		else
		{
			fToMemberX = GetCurrentPos().m_fPointX;
			fToMemberZ = GetCurrentPos().m_fPointZ;
		}


		if (0 >= m_lCurrentFrame)
		{
			if (fDistance < NEAR_DIST_TO_TARGET)
			{
				// 너무 가까우면 도망간다.
				fDstX = GetCurrentPos().m_fPointX - (fToMemberX + fToTargetX) / 2;
				fDstZ = GetCurrentPos().m_fPointZ - (fToMemberZ + fToTargetZ) / 2;

				// 달려간다
				RunAction(fDistance, fDstX, fDstZ);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
			}
			else if (fDistance > FAR_DIST_TO_TARGET)
			{
				fDstX = GetCurrentPos().m_fPointX + (fToMemberX + fToTargetX) / 2;
				fDstZ = GetCurrentPos().m_fPointZ + (fToMemberZ + fToTargetZ) / 2;

				// 달려간다
				RunAction(fDistance, fDstX, fDstZ);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
			}
			else
			{
				// 스킬 공격을 사용하는가 체크
				if (false == m_bAttacking && true == SkillAttack())
				{
					SkillAttackAction();
				}
			}

			if (false == m_bAttacking) 
			{
				m_lCurrentFrame = m_MotionInfo.m_dwFrame;
			}
		}
	}
}

void CAcolyteMonster::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	// 일정 거리를 유지하면서 마법 공격을 사용
	if (GetParty() == NULL) CSkillMonster::AttackBehavior(dwTick);
	else
	{
		// 마법 캐스팅 중일때는.. 아무런 다른 행동을 해서는 안된다.
		if (true == m_bCasting)
		{
			CastingAttackAction();
			return;
		}

		m_lpTarget = m_Threat.GetTarget();
		if (NULL == m_lpTarget)
		{
			CancelTarget();
			return;
		}
		else if (m_lpTarget && true == m_lpTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
		{
			CancelTarget();
			return;
		}

		// 병기에 탑승한 캐릭터는 때리지 않는다.
		if (Creature::CT_PC == Creature::GetCreatureType(m_lpTarget->GetCID()))
		{
			CCharacter* lpTargetChar = reinterpret_cast<CCharacter*>(m_lpTarget);
			if (lpTargetChar->IsRideArms())
			{
				CancelTarget();
				return;
			}
		}

		const float fDX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
		const float fDZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
		const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

		if (((fDistance > m_wSearchRange) && false == m_bLongRangeAttacked) || (0 == m_lpTarget->GetStatus().m_nNowHP))
		{
			CancelTarget();	
			return;
		}

		// 파티의 타겟으로 집어 넣는다.
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty*>(GetParty());
		if (NULL != lpParty)
		{
			// 레벨차이가 너무 나면 파티의 타겟으로 집어넣지 않는다.
			if (lpParty->GetHighestLevel() + CMonsterParty::ERROR_OF_LEVEL_GAP > m_lpTarget->GetStatus().m_nLevel)
			{
				lpParty->GetPartyTargetSet().insert(m_lpTarget->GetCID());
				// set 에서 없어지는 것은 CancelTarget 에서 처리한다.
			}
		}

		// 거리 체크
		float fToMemberX, fToMemberZ, fToTargetX, fToTargetZ;
		float fDstX, fDstZ;
		fToTargetX = m_lpTarget->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
		fToTargetZ = m_lpTarget->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;

		CAggresiveCreature* lpHiLVMember = FindHighestLVMember();
		if (lpHiLVMember)
		{
			fToMemberX = lpHiLVMember->GetCurrentPos().m_fPointX - GetCurrentPos().m_fPointX;
			fToMemberZ = lpHiLVMember->GetCurrentPos().m_fPointZ - GetCurrentPos().m_fPointZ;
		}
		else
		{
			fToMemberX = GetCurrentPos().m_fPointX;
			fToMemberZ = GetCurrentPos().m_fPointZ;
		}


		if (0 >= m_lCurrentFrame)
		{
			if (fDistance < NEAR_DIST_TO_TARGET)
			{
				// 너무 가까우면 도망간다.
				fDstX = GetCurrentPos().m_fPointX - (fToMemberX + fToTargetX) / 2;
				fDstZ = GetCurrentPos().m_fPointZ - (fToMemberZ + fToTargetZ) / 2;

				// 달려간다
				RunAction(fDistance, fDstX, fDstZ);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
			}
			else if (fDistance > FAR_DIST_TO_TARGET)
			{
				fDstX = GetCurrentPos().m_fPointX + (fToMemberX + fToTargetX) / 2;
				fDstZ = GetCurrentPos().m_fPointZ + (fToMemberZ + fToTargetZ) / 2;

				// 달려간다
				RunAction(fDistance, fDstX, fDstZ);

				// 내 활동구역을 벗어나따. 돌아가자~!
				if (true == m_MonsterInfo.m_bReturnPosition && true == IsReturn())
				{
					m_wSearchRange = OutsideSearchRange;
					CancelTarget();
				}
			}
			else
			{
				// 스킬 공격을 사용하는가 체크
				if (false == m_bAttacking && true == SkillAttack())
				{
					SkillAttackAction();
				}
			}

			if (false == m_bAttacking) 
			{
				m_lCurrentFrame = m_MotionInfo.m_dwFrame;
			}
		}
	}
}


void CMonster::AttackAction()
{
	m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
											m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

	if (false == MultiAttack()) 
	{
		ERRLOG1(g_Log, "CID:0x%08x 몬스터가 공격을 실패하였습니다.", m_dwCID);
	}

	m_bAttacking = true;
	m_bLongRangeAttacked = false;

	GetMotion(MonsterInfo::Z3D_CA_ATTACK, m_MotionInfo);
	m_lCurrentFrame = m_MotionInfo.m_dwFrame;

	m_MotionInfo.m_fVelocity = 0;
	// edith 2009.08.06 스킬 공격시 이동하지 않게 하기
	ms_AttackBehaviorSendCount += SendMove(1);
	m_nLeaveMovingNum = 0;
}

void CMonster::WalkAttackAction(float fVelocity)
{
	// 공격 범위 밖이다.
	m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_WALK;	// 일단 걷기로 세팅
	GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);	// 걷기 모션 정보를 얻어온다.

	// 걸어간다.
	m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
											m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

	// 적당히 걸어가서...
//	m_MotionInfo.m_fVelocity = fDistance - fAttackRange + 0.1f;
	m_MotionInfo.m_fVelocity = fVelocity;

	// 공격~!!
	if (false == MultiAttack()) 
	{
		ERRLOG1(g_Log, "CID:0x%08x 몬스터가 공격을 실패하였습니다.", m_dwCID);
	}

	m_bAttacking = true;
	m_bLongRangeAttacked = false;

	ms_AttackBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MAX);
	MoveTo(CalculateCoor(), false);
	m_nLeaveMovingNum = 0;

	GetMotion(MonsterInfo::Z3D_CA_ATTACK, m_MotionInfo);
	m_lCurrentFrame = m_MotionInfo.m_dwFrame;
}

void CMonster::RunAction(float fDistance, float fDstX, float fDstZ)
{
	// 달려간다
	m_MotionInfo.m_wAction = MonsterInfo::Z3D_CA_RUN;
	GetMotion(m_MotionInfo.m_wAction, m_MotionInfo);

	float fNewDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, fDstX, fDstZ);

	if (m_nLeaveMovingNum <= 0 || (m_MotionInfo.m_fDirection - fNewDirection) > 1.0f || (fNewDirection - m_MotionInfo.m_fDirection) > 1.0f) 
	{
		m_MotionInfo.m_fDirection = fNewDirection;
		short nAniNum = static_cast<short>(fDistance / m_MotionInfo.m_fVelocity);

		if(nAniNum <= 0)
			nAniNum = RUN_ANI_LIMIT_MIN;

		// edith 2008.01.23 몬스터가 공격을 시작했을대 이동할수있는 루프
		nAniNum = (nAniNum > RUN_ANI_LIMIT_MAX) ? RUN_ANI_LIMIT_MAX : nAniNum;//RUN_ANI_LIMIT_MIN;

		ms_AttackBehaviorSendCount += SendMove(nAniNum);
	}

	--m_nLeaveMovingNum;
	if (F_CELL_CANNOTCHANGE == MoveTo(CalculateCoor(), false)) 
	{
		ms_AttackBehaviorSendCount += SendMove(RUN_ANI_LIMIT_MIN);
		m_nLeaveMovingNum = 0;
	}
}

bool CMonster::IsReturn()
{
	const float fOriginalDX = m_OriginalPosition.m_fPointX - GetCurrentPos().m_fPointX;
	const float fOriginalDZ = m_OriginalPosition.m_fPointZ - GetCurrentPos().m_fPointZ;

	const float fOriginalDistance = (fOriginalDX * fOriginalDX) + (fOriginalDZ * fOriginalDZ);
	const int nTempReturnRange = (true == m_bLongRangeAttacked) ? (LongRange + Math::Random::ComplexRandom(6) - 2) : (ReturnRange + Math::Random::ComplexRandom(6) - 2);

	if (fOriginalDistance > (nTempReturnRange * nTempReturnRange)) return true;

	return false;
}

void CSkillMonster::CastingAttackAction()
{
	// 캐스팅 애니메이션 중이라면...
	if (m_lCurrentFrame > 0)
	{
		CAggresiveCreature* pTarget = CCreatureManager::GetInstance().GetAggresiveCreature(m_dwTargetCID);
		if (NULL == pTarget || 
			pTarget->GetMapIndex() != GetMapIndex() || 
			true == pTarget->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))
		{
			m_dwTargetCID = 0;
			m_lpTarget = NULL;
			m_bCasting = false;
			CancelTarget();
			return;
		}

		// 병기에 탑승한 캐릭터는 때리지 않는다.
		if (Creature::CT_PC == Creature::GetCreatureType(pTarget->GetCID()))
		{
			CCharacter* lpTargetChar = reinterpret_cast<CCharacter*>(pTarget);
			if (lpTargetChar->IsRideArms())
			{
				m_dwTargetCID = 0;
				m_lpTarget = NULL;
				m_bCasting = false;
				CancelTarget();
				return;
			}
		}

		return;
	}

	// 치프, 메이지, 어콜라이트는 캐스팅 동작을 오래 취한다.
	if (GetPattern() == MonsterInfo::PATTERN_CHIEF ||
		GetPattern() == MonsterInfo::PATTERN_MAGE ||
		GetPattern() == MonsterInfo::PATTERN_ACOLYTE)
	{
		if (m_nCastingCount > 0)
		{
			if (m_lpTarget)
			{
				m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
													m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

				GetMotion(MonsterInfo::Z3D_CA_CASTING, m_MotionInfo);
				m_lCurrentFrame = m_MotionInfo.m_dwFrame;

				m_bCasting = true;
				m_MotionInfo.m_fVelocity = 0.0f;
				// edith 2009.08.06 스킬 공격시 이동하지 않게 하기
				ms_AttackBehaviorSendCount += SendMove(1);
				m_nLeaveMovingNum = 0;

				--m_nCastingCount;
			}
			else
			{
				m_dwTargetCID = 0;
				m_bCasting = false;
				m_nCastingCount = 0;
				CancelTarget();
			}

			return;
		}
	}

	// 캐스팅이 끝난후 스킬 사용
	UseCastedSkill();

	m_bCasting = false;
	m_bAttacking = true;
	m_bLongRangeAttacked = false;
	m_nCastingCount = 0;

	GetMotion(MonsterInfo::Z3D_CA_ATTACK, m_MotionInfo);
	m_lCurrentFrame = m_MotionInfo.m_dwFrame;
}

void CSkillMonster::SkillAttackAction()
{
	m_bAttacking = true;
	m_MotionInfo.m_fDirection = CalcDir2D(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ, 
										  m_lpTarget->GetCurrentPos().m_fPointX, m_lpTarget->GetCurrentPos().m_fPointZ);

	// 캐스팅 타입의 스킬이라면 캐스팅 액션으로 변경
	if (true == m_bCasting)
	{
		GetMotion(MonsterInfo::Z3D_CA_CASTING, m_MotionInfo);
		m_lCurrentFrame = m_MotionInfo.m_dwFrame;

		m_MotionInfo.m_fVelocity = 0.0f;

		// edith 2009.08.06 스킬 공격시 이동하지 않게 하기
		ms_AttackBehaviorSendCount += SendMove(1);
		m_nLeaveMovingNum = 0;

		m_nCastingCount = m_attackType.m_cSkillLockCount;
	}
	else
	{
		GetMotion(MonsterInfo::Z3D_CA_ATTACK, m_MotionInfo);
		m_lCurrentFrame = m_MotionInfo.m_dwFrame;

		m_MotionInfo.m_fVelocity = 0.0f;

		// edith 2009.08.06 스킬 공격시 이동하지 않게 하기
		ms_AttackBehaviorSendCount += SendMove(1);

		m_nLeaveMovingNum = 0;

		m_nCastingCount = 0;
	}
}