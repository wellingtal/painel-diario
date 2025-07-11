#include "stdafx.h"
#include "CastleEmblem.h"

#include <Castle/Castle.h>
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

CCastleEmblem::CCastleEmblem(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject)
: CSiegeObject(MonsterCreate, CastleObject), m_cEnemyNation(Creature::STATELESS), m_dwLastAttackedTick( 0 )
{
}


CCastleEmblem::~CCastleEmblem()
{
}


void	CCastleEmblem::NormalBehavior(unsigned long dwTick)
{
	if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		return;
	}

	// ���� �� ó��
	if (NULL == m_lpTarget && true == m_MonsterInfo.m_bFirstAttack) 
	{
		SearchPlayer();
	}
}

void	CCastleEmblem::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		CancelTarget();
		return;
	}

	// ��¡���� ��ȯ �Ϸ� ���°� �ƴ� ���� �ƹ��� �ٸ� �ൿ�� �ؼ��� �ȵȴ�.
	if (m_cState != Siege::COMPLETE)
	{
		return;
	}

	// ���� ĳ���� ���϶���.. �ƹ��� �ٸ� �ൿ�� �ؼ��� �ȵȴ�.
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

	if (fDY > Siege::EMBLEM_ATTACK_HEIGHT_ERROR)
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
		// ���� ���� ���̴�.
		if (true == SkillAttack())		// ��ų ������ ����ϴ°� üũ
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
		// ���� ���� ���̴�.
		if (false == m_bAttacking)
		{
			if (true == SkillAttack())	// ��ų ������ ����ϴ°� üũ
			{
				SkillAttackAction();
			}
		}
	}
}

void	CCastleEmblem::SearchPlayer(void)
{
	// TODO : �ػ� ������ ���� float ����� ���ֺ���.
	if (NULL == m_CellPos.m_lpCell)
	{
		ERRLOG1(g_Log, "CID:0X%08 ���� ������Ʈ�� �� ���� �ۿ� �ֽ��ϴ�.", m_dwCID);
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

bool	CCastleEmblem::Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)
{
	return (m_cState == Siege::COMPLETE) ? CSkillMonster::Attack(attackType, cDefenderNum, ppDefenders, cDefenserJudges, wDefenserMPHeal) : false;
}

bool	CCastleEmblem::Dead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// �߰� ������ ��Ŷ ����
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		m_cEnemyNation = reinterpret_cast<CCharacter*>(pOffencer)->GetNation();
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), pOffencer->GetCID(),
																GetCastleID(), m_dwCID, m_cEnemyNation, 0,
																PktCastleCmd::CASTLE_UPDATE_EMBLEM, PktBase::NO_SERVER_ERR);
	}

	return false;
}


bool	CCastleEmblem::Upgrade(unsigned char cUpgradeStep)
{
	// �� ������Ʈ�� ���׷��̵� ȿ���� �����ش�.
	Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastle(GetCastleID());
	if (lpCastle)
	{
		lpCastle->DegradeByEmblem();
	}

	// ���׷��̵� ó��
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = cUpgradeStep;

	UpdateObjectInfo(Siege::UPGRADE_HP);

	// ���׷���Ʈ Ÿ�Կ� ���� ó���� �ؾ��Ѵ�.
	if (lpCastle)
	{
		lpCastle->UpgradeByEmblem();
	}

	// ��� ĳ���Ϳ��� �����Ѵ�.!!
	// �� ��¡�� ���׷��̵� �ܰ�� ���� ������ �ʿ��ϹǷ� ��� ĳ���Ϳ��� ����
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cUpgradeStep;	// ���׷��̵� �ܰ�
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);

	return true;
}


// ��¡�� ������Ʈ (��ȯ��, ��ȯ�Ϸ�, ���׷��̵���)
bool	CCastleEmblem::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd)
{
	if ( !IsEmblem() ) return false;

	m_cState = cState;

	PktCastleCmd pktCC;

	if (PktCastleCmd::CASTLE_UPGRADE_EMBLEM == cSubCmd)
	{
		m_cUpgradeType = static_cast<unsigned char>(dwValue1);

		UpdateObjectInfo();
	}
	else if (PktCastleCmd::CASTLE_UPDATE_EMBLEM == cSubCmd)
	{
		// ���׷��̵� ȿ�� �����ֱ�
		Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastle(GetCastleID());
		if (lpCastle)
		{
			lpCastle->DegradeByEmblem();
		}

		switch (dwValue1)
		{
			case Siege::MINE:
			{
				m_cSubState = Siege::MINE;
				std::swap(m_cNation, m_cEnemyNation);
				m_cEnemyNation = Creature::STATELESS;
				dwValue1 = m_cNation;
			}
			break;

			case Siege::ENEMY:
			{
				m_cSubState = Siege::ENEMY;
				std::swap(m_cNation, m_cEnemyNation);
				dwValue1 = m_cNation;
			}
			break;
		}

		m_cUpgradeType = Siege::NO_JEWEL;
		m_cUpgradeStep = 0;

		UpdateObjectInfo(Siege::FULL_HP);

		// 30�ʰ� ����
		Skill::CAddSpell<CInvincibleSpell>(
			CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
			Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, 30))(this);

	}
	else if (PktCastleCmd::CASTLE_SUMMON_EMBLEM_COMPLETE == cSubCmd)
	{
		m_cSubState = Siege::MINE;
		m_cEnemyNation = Creature::STATELESS;

		UpdateObjectInfo(Siege::FULL_HP);
	}

	// �޼��� ó�� ������ ���� ��� �ο����� ������.
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= dwValue1;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= cSubCmd;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

void	CCastleEmblem::SendAttackedMessage()
{
	unsigned long dwNowTime = timeGetTime();
	if ( dwNowTime - m_dwLastAttackedTick >= Siege::EMBLEM_ATTACKED_INTERVAL )
	{
		m_dwLastAttackedTick = dwNowTime;

		// �޼��� ó�� ������ ���� ��� �ο����� ������.
		PktCastleCmd pktCC;
		pktCC.m_dwCastleID			= GetCastleID();
		pktCC.m_dwCastleObjectID	= m_dwCID;
		pktCC.m_cState				= m_cState;
		pktCC.m_dwValue1			= 0;
		pktCC.m_dwValue2			= 0;
		pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_EMBLEM_ATTACKED;

		char* szPacket = reinterpret_cast<char *>(&pktCC);
		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
		{
			CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		}
	}
}
