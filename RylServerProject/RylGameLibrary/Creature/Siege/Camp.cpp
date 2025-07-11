#include "stdafx.h"
#include "Camp.h"

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Skill/SkillTable.h>
#include <Skill/SkillMgr.h>

#include <Utility/Math/Math.h>

#include <Item/Item.h>
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

CCamp::CCamp(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID,
			 unsigned long dwHP, unsigned short wObjectType, unsigned char cState, 
			 unsigned char cSubState, unsigned char cUpgradeStep, unsigned char cMaterial,
			 unsigned char cSiegeCount, const CampRight& campRight, bool bFullHP)
: CSiegeObject(MonsterCreate, dwCampID, dwGID, dwHP, wObjectType, cState, cSubState, cUpgradeStep, cMaterial, cSiegeCount, bFullHP),
  m_CampRight(campRight), m_dwLastAttackedTick(0), m_dwCmdSenderCID(0)
{

}

CCamp::~CCamp()
{

}

void	CCamp::NormalBehavior(unsigned long dwTick)
{
	// ���� ó��
	if (NULL == m_lpTarget && true == m_MonsterInfo.m_bFirstAttack) 
	{
		SearchPlayer();
	}
}

void	CCamp::AttackBehavior(unsigned long dwTick)
{
	PERFORMANCE_CHECK(FunctionTimingCheck)

	// ������, ���� �����, �ı��Ǿ����� ������ �� ����.
	if (Siege::DEVELOPING == m_cState ||
		Siege::CANCELING == m_cState ||
		Siege::DESTROYED == m_cState)
	{
		CancelTarget();
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


	// �Ÿ� üũ
	if (fDistance > Siege::CAMP_ATTACK_RANGE || 0 == m_lpTarget->GetStatus().m_nNowHP)
	{
		CancelTarget();	
		return;
	}

	if (0 >= m_lCurrentFrame)
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

	// ������ �������ѵд�.
	m_MotionInfo.m_fDirection = 0;
}

void	CCamp::SearchPlayer(void)
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

	const float fSquareSearchRange = (float)(Siege::CAMP_ATTACK_RANGE * Siege::CAMP_ATTACK_RANGE);

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

			pTempTarget = pCell->GetNextCharacter();
		}
	}

	if (NULL != pCurrentTarget)
	{
		m_Threat.AddToThreatList(pCurrentTarget, 1);
		m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_SEEN_PLAYER);
	}
}

bool	CCamp::Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)
{
	return CSkillMonster::Attack(attackType, cDefenderNum, ppDefenders, cDefenserJudges, wDefenserMPHeal);
}

bool	CCamp::Dead(CAggresiveCreature* pOffencer)
{
	if (STATE_ID_DIE == m_nCurrentState) { return false; }

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ZERO_HP);

	// ũ���� �Ŵ������� ���� (�ش� �������� �����Ѵ�.)
	CCreatureManager::GetInstance().DeleteCreature(m_dwCID);

	// �߰� ������ ��Ŷ ����
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		if ( pOffencer )
		{
			DETLOG3(g_Log, "��� ��� �̺�Ʈ �α� : �ı��� ��� ���̵�(0x%08x), �ı��� ĳ���� ���̵�(0x%08x), �ı��� ��� ���̵�(0x%08x)", 
				GetCampID(), pOffencer->GetCID(), pOffencer->GetGID());

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


bool	CCamp::ToStarterKit(bool bFullMaterial)
{
	// ������ ����
	Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(Item::EtcItemID::CAMP_KIT_ID);
	if (NULL == lpItem)
	{
		ERRLOG0(g_Log, "��� ��� ���� ��ŸƮŶ ������ ������ �����߽��ϴ�.");
		return false;
	}

	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
	if (NULL != lpGuild)
	{
		Guild::MemberInfo MasterInfo = lpGuild->GetMaster();
		unsigned long dwMasterID = MasterInfo.m_dwCID;
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter( dwMasterID );

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

			return true;
		}
	}

	// �ٴڿ� ������ ����߸���
	CCell::ItemInfo itemInfo;
	GetCellPos().m_lpCell->SetItem(GetCurrentPos(), lpItem, 0, 0, CCell::NONE, itemInfo);

	return true;
}


// ��� ��� �ı��� ��� �����Ϳ��� ���� �����ش�. (�α�������������)
// (�α׾ƿ��� ���� ���� �߰� �������� ���� �����ִ� ó���� �Ѵ�.)
void	CCamp::AddGoldToMaster(unsigned long dwGold)
{
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);

	if (lpGuild)
	{
		Guild::MemberInfo& MasterInfo = lpGuild->GetMaster();
		CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter( MasterInfo.m_dwCID );

		if (lpMaster)
		{
			lpMaster->AddGold(dwGold, true);
		}
	}
}

// ��� ��� ���� �Ϸ�
bool	CCamp::Build(unsigned char cUpgradeStep)
{
	m_cState = Siege::COMPLETE;

	UpdateObjectInfo();

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= 0;
	pktCC.m_dwValue2		= 0;
	pktCC.m_cSubCmd			= PktCampCmd::CAMP_BUILD_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		//SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
		// Vincent - ��� ĳ���Ϳ��� �����ؾ� ������ ������ ����� ���ŵȴ�.
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		return true;
	}

	return false;
}

bool	CCamp::Cancel()
{
	const int MAX_BUFFER = sizeof(PktCampCmd);
	char szBuffer[MAX_BUFFER];

	PktCampCmd* lpPktCC = reinterpret_cast<PktCampCmd*>(szBuffer);

	lpPktCC->m_dwCID		= m_dwCID;
	lpPktCC->m_dwCampID		= GetCampID();
	lpPktCC->m_cState		= Siege::DESTROYED;
	lpPktCC->m_dwValue1		= 0;
	lpPktCC->m_dwValue2		= 0;
	lpPktCC->m_cSubCmd		= PktCampCmd::CAMP_DESTROY;

	if (true == PacketWrap::WrapCrypt(szBuffer, MAX_BUFFER, CmdCampCmd, 0 ,0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szBuffer, MAX_BUFFER, CmdCampCmd);
		return true;
	}

	return false;
}

// ��� ��� ���׷��̵� �Ϸ�
bool	CCamp::Upgrade(unsigned char cUpgradeStep)
{
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = cUpgradeStep;

	UpdateObjectInfo(Siege::UPGRADE_HP);

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= m_cUpgradeStep;
	pktCC.m_dwValue2		= 0;
	pktCC.m_cSubCmd			= PktCampCmd::CAMP_UPGRADE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

// ��� ��� ���� �Ϸ�
bool	CCamp::Repair(unsigned short wRepairHP)
{
	m_cState = Siege::COMPLETE;
	UpdateObjectInfo(Siege::REPAIR_HP, wRepairHP);

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= m_CreatureStatus.m_nNowHP;
	pktCC.m_dwValue2		= 0;
	pktCC.m_cSubCmd			= PktCampCmd::CAMP_REPAIR_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}


bool	CCamp::Destroy(unsigned long dwOffencerGID)
{
	const int MAX_BUFFER = sizeof(PktCampCmd);
	char szBuffer[MAX_BUFFER];

	PktCampCmd* lpPktCC = reinterpret_cast<PktCampCmd*>(szBuffer);

	lpPktCC->m_dwCID		= m_dwCID;
	lpPktCC->m_dwCampID		= GetCampID();
	lpPktCC->m_cState		= Siege::DESTROYED;
	lpPktCC->m_dwValue1		= 0;
	lpPktCC->m_dwValue2		= 0;
	lpPktCC->m_cSubCmd		= PktCampCmd::CAMP_DESTROY;

	if (true == PacketWrap::WrapCrypt(szBuffer, MAX_BUFFER, CmdCampCmd, 0 ,0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szBuffer, MAX_BUFFER, CmdCampCmd);

		// ���� ���
		CCell* lpCell = CCellManager::GetInstance().GetCell(0,
								static_cast<unsigned long>(GetCurrentPos().m_fPointX), 
								static_cast<unsigned long>(GetCurrentPos().m_fPointY), 
								static_cast<unsigned long>(GetCurrentPos().m_fPointZ));

		if (NULL == lpCell)
		{
			ERRLOG4(g_Log, "CampID:0x%08x ���縦 ����� ��� ����� ��ġ�� �̻��մϴ�. X:%.1f, Y:%.1f, Z:%.1f", 
				GetCampID(), GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ);
			return false;
		}

		unsigned char cAmount = m_cMaterial + Siege::GetUpgradeMaterialNum(m_wObjectType) * m_cUpgradeStep / 2;
		while (cAmount > 0)
		{
			Item::CItem* lpMaterial = Item::CItemFactory::GetInstance().CreateItem(Item::EtcItemID::SIEGE_MATERIAL_ID);
			if (NULL == lpMaterial)
			{
				ERRLOG0(g_Log, "���� ���� ���ۿ� ���� ������ ������ �����߽��ϴ�.");
				return false;
			}

			unsigned char cMaxNum = lpMaterial->GetMaxNumOrDurability();
			if (cAmount < cMaxNum)
			{
				lpMaterial->SetNumOrDurability(cAmount);
				cMaxNum = cAmount;
				cAmount = 0;
			}
			else
			{
				lpMaterial->SetNumOrDurability(cMaxNum);
				cAmount -= cMaxNum;
			}

			CCell::ItemInfo itemInfo;

			lpCell->SetItem(GetCurrentPos(), lpMaterial, 0, dwOffencerGID, 
				(0 == dwOffencerGID) ? CCell::NONE : CCell::GUILD, itemInfo);
		}

		return true;
	}

	return false;
}


bool	CCamp::ChangeType(unsigned short wChangeType)
{
	m_cState = Siege::COMPLETE;

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= wChangeType;
	pktCC.m_dwValue2		= 0;
	pktCC.m_cSubCmd			= PktCampCmd::CAMP_CHANGE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}


// ��� ��� ���� ������Ʈ
bool	CCamp::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd)
{
	m_cState = cState;
	
	UpdateObjectInfo();

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= dwValue1;
	pktCC.m_dwValue2		= dwValue2;
	pktCC.m_cSubCmd			= cSubCmd;

	// ���� �Ҹ� �ʿ���� ����� ���� ������ ������ �����ش�.
	switch (cSubCmd)
	{
		case PktCampCmd::CAMP_UPGRADE:
		{
			m_cUpgradeStep = static_cast<unsigned char>(dwValue1);
			pktCC.m_dwValue2 = m_cMaterial;
		}
		break;

		case PktCampCmd::CAMP_REPAIR:
		case PktCampCmd::CAMP_CHANGE_TYPE:
		{
			pktCC.m_dwValue2 = m_cMaterial;
		}
		break;
	}

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

void	CCamp::SetRight(CampRight campRight)
{
	m_CampRight = campRight;

	// �ش� ����� �����鿡�� ����
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (lpGuild)
	{
		PktCampRight pktCR;
		pktCR.m_dwCID			= m_dwCID;
		pktCR.m_dwCampID		= GetCampID();
		pktCR.m_CampRight		= m_CampRight;

		char* szPacket = reinterpret_cast<char *>(&pktCR);
		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampRight), CmdCampRight, 0, 0))
		{
			lpGuild->SendAllMember(szPacket, sizeof(PktCampRight), CmdCampRight);
		}
	}
}

bool	CCamp::CheckRight(unsigned char cRightType, unsigned long dwCID, unsigned long dwGID)
{
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
	if (lpGuild)
	{
		if (m_CampRight.Check(cRightType, lpGuild->GetTitle(dwCID)))
		{
			return true;
		}
	}

	return false;
}


void	CCamp::SendAttackedMessage()
{
	unsigned long dwNowTime = timeGetTime();
	if ( dwNowTime - m_dwLastAttackedTick >= Siege::CAMP_ATTACKED_INTERVAL )
	{
		// �߰� ������ ��Ŷ ����
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
		if (lpDBAgentDispatch)
		{
			if (IsWorldWeapon())
			{
				GameClientSendPacket::SendCharCampMessageToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCampID(),
					PktCampMessage::MSGCMD_WEAPON_ATTACKED, PktBase::NO_SERVER_ERR);
			}
			else
			{
				GameClientSendPacket::SendCharCampMessageToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCampID(),
					PktCampMessage::MSGCMD_ATTACKED, PktBase::NO_SERVER_ERR);
			}
		}

		m_dwLastAttackedTick = dwNowTime;
	}
}


unsigned long	CCamp::GetRepairGold() const
{
	int nDiffHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP - m_CreatureStatus.m_nNowHP;
	return nDiffHP * Siege::CAMP_REPAIR_GOLD_PER_HP;
}

