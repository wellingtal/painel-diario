#include "stdafx.h"
#include "CastleGuard.h"

#include <Castle/CastleMgr.h>

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

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

CCastleGuard::CCastleGuard(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject)
: CCastleArms(MonsterCreate, CastleObject)
{
}

CCastleGuard::~CCastleGuard()
{
}


void	CCastleGuard::NormalBehavior(unsigned long dwTick)
{
	if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		return;
	}

	// 선공 몹 처리
	if (NULL == m_lpTarget && true == m_MonsterInfo.m_bFirstAttack) 
	{
		SearchPlayer();
	}
}


void	CCastleGuard::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
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
		}
	}
}

void	CCastleGuard::SearchPlayer(void)
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
			if (pTempTarget->GetStatus().m_nNowHP > 0 && EnemyCheck::EC_ENEMY == IsEnemy(pTempTarget))
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

bool	CCastleGuard::Dead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_wObjectType = Siege::CASTLE_ARMS_NPC;
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
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), pOffencer->GetCID(),
																GetCastleID(), m_dwCID, PktCastleCmd::DESTROY, 0,
																PktCastleCmd::CASTLE_DESTROY_ARMS, PktBase::NO_SERVER_ERR);
	}

	return false;
}
