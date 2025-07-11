#include "stdafx.h"
#include "CastleGate.h"

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


CCastleGate::CCastleGate(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject)
: CSiegeObject(MonsterCreate, CastleObject)
{
}

CCastleGate::~CCastleGate()
{
}

bool	CCastleGate::Dead(CAggresiveCreature* pOffencer)
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
		return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), pOffencer->GetCID(),
																GetCastleID(), m_dwCID, 0, 0, PktCastleCmd::CASTLE_DESTROY_GATE,
																PktBase::NO_SERVER_ERR);
	}

	return false;
}


void	CCastleGate::AddProtectGate(CCharacter* lpProtectChar)
{
	if (lpProtectChar && lpProtectChar->GetStatus().m_nNowHP > 0)
	{
		ProtectCIDList::iterator itr = std::find(m_ProtectCharList.begin(), m_ProtectCharList.end(), lpProtectChar->GetCID());
		if (itr == m_ProtectCharList.end())
		{
			m_ProtectCharList.push_back(lpProtectChar->GetCID());
			lpProtectChar->SetProtectGateCID(m_dwCID);
			++m_CreatureStatus.m_StatusInfo.m_wBlock;

			SendProtectGateInfo();
		}
	}
}

void	CCastleGate::DeleteProtectGate(CCharacter* lpProtectChar)
{
	if (lpProtectChar)
	{
		ProtectCIDList::iterator itr = std::find(m_ProtectCharList.begin(), m_ProtectCharList.end(), lpProtectChar->GetCID());
		if (itr != m_ProtectCharList.end())
		{
			m_ProtectCharList.erase(itr);
			lpProtectChar->SetProtectGateCID(0);
			if (0 < m_CreatureStatus.m_StatusInfo.m_wBlock)
			{
				--m_CreatureStatus.m_StatusInfo.m_wBlock;
			}

			SendProtectGateInfo();
		}
	}
}

void	CCastleGate::DivideDamage(CAggresiveCreature* pOffencer, unsigned short wDamage)
{
	if (NULL == pOffencer)
	{
		return;
	}

	int nNum = static_cast<int>(m_ProtectCharList.size());
	wDamage = static_cast<unsigned short>(wDamage * 100 / 1.33f);
	unsigned short wCharDamage = (nNum == 0) ? 0 : wDamage / nNum;

	ProtectCIDList::iterator itr = m_ProtectCharList.begin();
	while (itr != m_ProtectCharList.end())
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter((*itr));
		if (lpCharacter)
		{
			unsigned short wNowHP = lpCharacter->GetStatus().m_nNowHP;
			const unsigned short wThreatAmount = (wNowHP < wCharDamage) ? wNowHP : wCharDamage;
			lpCharacter->GetThreat().AddToThreatList(pOffencer, wThreatAmount);
			lpCharacter->GetStatus().m_nNowHP = (wNowHP > wCharDamage) ? wNowHP - wCharDamage : 0;

			if (0 == lpCharacter->GetStatus().m_nNowHP)
			{
				lpCharacter->Dead(pOffencer);
				lpCharacter->GetThreat().ClearAll();
			}

			AtType attackType;
			attackType.m_wType = AtType::RIGHT_MELEE;

			CGameClientDispatch* pCharacterDispatcher = lpCharacter->GetDispatcher();
			if (NULL != pCharacterDispatcher)
			{
				GameClientSendPacket::SendCharAttacked(pCharacterDispatcher->GetSendStream(), pOffencer, lpCharacter, 
					attackType, 0, wCharDamage, ClientConstants::Judge_Front, 0, PktBase::NO_SERVER_ERR);
			}
		}
	}
}

void	CCastleGate::SendProtectGateInfo()
{
	int nNum = static_cast<int>(m_ProtectCharList.size());
	// 블럭률 계산식 : INT((99 * 블럭수치 / 2) / (블럭수치 / 2 + 50) * 100) / 100
	int nBlockRate = static_cast<int>((99 * m_CreatureStatus.m_StatusInfo.m_wBlock / 2) / (m_CreatureStatus.m_StatusInfo.m_wBlock / 2 + 50) * 100) / 100;

	if (0 == nNum) return;

	ProtectCIDList::iterator itr = m_ProtectCharList.begin();
	while (itr != m_ProtectCharList.end())
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter((*itr));
		if (lpCharacter)
		{
			CGameClientDispatch* pCharacterDispatcher = lpCharacter->GetDispatcher();
			if (NULL != pCharacterDispatcher)
			{
				GameClientSendPacket::SendCharCastleCmd(pCharacterDispatcher->GetSendStream(), GetCastleID(), m_dwCID,
					nBlockRate, m_CreatureStatus.m_nNowHP,
					PktCastleCmd::CASTLE_GATE_PROTECT_INFO, PktBase::NO_SERVER_ERR);
			}
		}

		++itr;
	}
}

void	CCastleGate::Open()
{
	if ( !IsGate() ) return;

	m_cSubState = Siege::OPENED;

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cSubState;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_GATE_OPEN;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}
}

void	CCastleGate::Close()
{
	if ( !IsGate() ) return;

	m_cSubState = Siege::CLOSED;

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cSubState;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_GATE_CLOSE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}
}

void	CCastleGate::ForceOpen()
{
	if ( !IsGate() ) return;
	if ( Siege::COMPLETE == m_cState && Siege::OPENED == m_cSubState )
	{
		return;
	}

	m_cState	= Siege::COMPLETE;
	m_cSubState = Siege::OPENED;
	m_nCurrentState = STATE_ID_NORMAL;
	
	UpdateObjectInfo(Siege::FULL_HP, 0);

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cSubState;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_GATE_OPEN;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}
}

void	CCastleGate::ForceClose()
{
	if ( !IsGate() ) return;
	if ( Siege::COMPLETE == m_cState && Siege::CLOSED == m_cSubState )
	{
		return;
	}

	m_cState	= Siege::COMPLETE;
	m_cSubState = Siege::CLOSED;
	m_nCurrentState = STATE_ID_NORMAL;

	UpdateObjectInfo(Siege::FULL_HP, 0);

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cSubState;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_GATE_CLOSE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}
}

bool	CCastleGate::Upgrade(unsigned char cUpgradeStep)
{
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = cUpgradeStep;

	UpdateObjectInfo(Siege::UPGRADE_HP);

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_cUpgradeStep;	// 업그레이드 단계
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_UPGRADE_GATE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleGate::Repair(unsigned short wRepairHP)
{
	m_cState = Siege::COMPLETE;

	UpdateObjectInfo(Siege::REPAIR_HP, wRepairHP);

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_CreatureStatus.m_nNowHP;	// 현재 HP
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_REPAIR_GATE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleGate::Restore()
{
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = 0;
	m_nCurrentState = STATE_ID_NORMAL;

	UpdateObjectInfo();

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= m_CreatureStatus.m_nNowHP;	// 현재 HP
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_RESTORE_GATE_COMPLETE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		// Vincent - 브로드 캐스트 테스트 코드
		//SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleGate::Destroy(unsigned char cOffencerNation, bool bTakeGold)
{
	m_cState = Siege::DESTROYED;
	m_cSubState = Siege::OPENED;
	m_cUpgradeStep = 0;

	m_CreatureStatus.m_nNowHP = 0;

	UpdateObjectInfo();

	// 메세지 처리 때문에 존의 모든 인원에게 보낸다.
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= 0;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_DESTROY_GATE;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

bool	CCastleGate::Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd)
{
	m_cState = cState;

	if (PktCastleCmd::CASTLE_RESTORE_GATE == cSubCmd)
	{
		UpdateObjectInfo(Siege::FULL_HP);
	}
	else
	{
		UpdateObjectInfo();
	}

	// 성문이 있는 반경 5셀 이내에 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= GetCastleID();
	pktCC.m_dwCastleObjectID	= m_dwCID;
	pktCC.m_cState				= m_cState;
	pktCC.m_dwValue1			= dwValue1;
	pktCC.m_dwValue2			= 0;
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




unsigned long	CCastleGate::GetRepairGold() const
{
//	int nDiffHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP - m_CreatureStatus.m_nNowHP;
//	int nShare = nDiffHP / Siege::CASTLE_ARMS_REPAIR_HP_UNIT;
//	int nLeftOver = nDiffHP % Siege::CASTLE_ARMS_REPAIR_HP_UNIT;
//	if (nLeftOver > 0) ++nShare;
//
//	return nShare * Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT;
	return 0;
}