#include "stdafx.h"
#include "Airship.h"

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


CAirship::CAirship(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
				   unsigned long dwHP, unsigned short wObjectType, unsigned char cState,
				   unsigned char cUpgradeStep)
: CSiegeArms(MonsterCreate, dwOwnerID, cNation, dwHP, wObjectType, cState, cUpgradeStep)
{
	std::fill_n(&m_dwPassengerCID[0], int(Siege::AIRSHIP_PASSENGER_NUM), 0);
}

CAirship::~CAirship()
{
}

bool	CAirship::Dead(CAggresiveCreature* pOffencer)
{
	if (NULL == pOffencer) return false;
	if (STATE_ID_DIE == m_nCurrentState) return false;

	m_CreatureStatus.m_nNowHP = 0;
	m_dwLastBehaviorTick = m_dwLastTime = CPulse::GetInstance().GetLastTick();
	m_lCurrentFrame = FPS;
	m_bAttacking = false;
	m_bCasting = false;

	// 타고 있던 캐릭터는 죽는다.
	CCharacter* lpRider = NULL;
	if (m_dwRiderCID)
	{
		lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if ( lpRider )
		{
			lpRider->GetOff();
			lpRider->Kill(pOffencer);
		}

		m_dwRiderCID = 0;
	}

	for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
	{
		lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwPassengerCID[i]);
		if ( lpRider )
		{
			lpRider->GetOff();
			lpRider->Kill(pOffencer);
		}

		m_dwPassengerCID[i] = 0;
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


void	CAirship::MoveTo(const Position& NewPosition)
{
	CSiegeObject::MoveTo(NewPosition);

	for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
	{
		if (0 != m_dwPassengerCID[i])
		{
			CCharacter* lpPassenger = CCreatureManager::GetInstance().GetCharacter(m_dwPassengerCID[i]);
			if (lpPassenger)
			{
				lpPassenger->MoveTo(NewPosition, false);
			}
		}
	}
}

unsigned char	CAirship::IsRider(unsigned long dwCID) const
{
	if (m_dwRiderCID == dwCID)
	{
		return Siege::RIDER_FOR_OWNER;
	}
	else
	{
		for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
		{
			if (dwCID == m_dwPassengerCID[i])
			{
				return Siege::RIDER_FOR_PASSENGER;
			}
		}
	}

	return Siege::NOT_RIDER;
}

bool	CAirship::Ride(unsigned long dwCID)
{
	if (dwCID == m_dwOwnerID)
	{
		return CSiegeArms::Ride(dwCID);
	}
	else if (Siege::NOT_RIDER == IsRider(dwCID))
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider)
		{
			for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
			{
				if (0 == m_dwPassengerCID[i] && 0 != m_cNation && m_cNation == lpRider->GetNation())
				{
					m_dwPassengerCID[i] = dwCID;
					lpRider->Ride(m_dwCID);
					lpRider->SkillClear();
					
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
		}
	}

	return false;
}

bool	CAirship::GetOff(unsigned long dwCID)
{
	if (dwCID == m_dwRiderCID)
	{
		return CSiegeArms::GetOff(dwCID);
	}
	else if (Siege::NOT_RIDER != IsRider(dwCID))
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwCID);
		if (lpRider)
		{
			for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
			{
				if (dwCID == m_dwPassengerCID[i])
				{
					m_dwPassengerCID[i] = 0;
					lpRider->GetOff();					

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
		}
	}

	return false;
}

void	CAirship::AllGetOff()
{
	// 주인이 타고 있으면, 주인부터 내린다.
	if (0 != m_dwRiderCID)
	{
		GetOff(m_dwRiderCID);
	}

	// 손님 내리기
	for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
	{
		if (0 != m_dwPassengerCID[i])
		{
			GetOff(m_dwPassengerCID[i]);
		}
	}
}

void	CAirship::GetRiders(unsigned long* pRiders) const
{
	CSiegeObject::GetRiders(pRiders);

	std::copy( &m_dwPassengerCID[0], &m_dwPassengerCID[ Siege::AIRSHIP_PASSENGER_NUM ], &pRiders[1] );
}

unsigned char	CAirship::GetRiderNum() const
{
	unsigned char cRiderNum = CSiegeObject::GetRiderNum();

	for (int i=0; i<Siege::AIRSHIP_PASSENGER_NUM; ++i)
	{
		if (0 != m_dwPassengerCID[i])
		{
			++cRiderNum;
		}
	}

	return cRiderNum;
}