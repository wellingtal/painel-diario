#include "stdafx.h"
#include "WorldWeaponDB.h"
#include "SiegeObjectDBMgr.h"

#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Log/ServerLog.h>

CWorldWeaponDB::CWorldWeaponDB(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwGID, unsigned short wObjectType,
							   unsigned char cZone, unsigned char cChannel, Position Pos)
: CCampDB(DBComponent, dwCampID, dwGID, cZone, cChannel, Pos), m_iFireX(0), m_iFireZ(0)
{
	m_wObjectType = wObjectType;
}

CWorldWeaponDB::CWorldWeaponDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo)
: CCampDB(DBComponent, CampInfo), m_iFireX(0), m_iFireZ(0)
{
}

CWorldWeaponDB::~CWorldWeaponDB()
{
}

bool	CWorldWeaponDB::UpdateWeaponState(unsigned char cWeaponState, unsigned long dwValue1, unsigned long dwValue2)
{
	if (Siege::WEAPON_CHARGE != cWeaponState && Siege::WEAPON_FIRE != cWeaponState)
	{
		return false;
	}

	switch (cWeaponState)
	{
		case Siege::WEAPON_CHARGE:	m_cChargeTime = static_cast<unsigned char>(dwValue1);							break;
		case Siege::WEAPON_FIRE:	m_iFireX = static_cast<int>(dwValue1); m_iFireZ = static_cast<int>(dwValue2);	break;
	}

	m_cSubState = cWeaponState;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_TimeValue.Year	= sysTime.wYear;
	m_TimeValue.Month	= sysTime.wMonth;
	m_TimeValue.Day		= sysTime.wDay;
	m_TimeValue.Hour	= sysTime.wHour;
	m_TimeValue.Minute	= sysTime.wMinute;
	m_TimeValue.Second	= sysTime.wSecond;

	DBComponent::CastleDB::UpdateCampSubState(m_DBComponent, GetCampID(), m_cSubState);
	DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

	return true;
}

bool	CWorldWeaponDB::Fire()
{
	// 월드 웨폰 파괴 정보 DB 에 추가한다.
	if (Siege::KARTERANT_WEAPON == m_wObjectType)
	{
		DBComponent::CastleDB::InsertWorldWeaponInfo(m_DBComponent, GetZone(), GetChannel(), Creature::KARTERANT);

		// 월드 웨폰 정보 갱신
		CSiegeObjectDBMgr::GetInstance().UpdateWorldWeaponInfo(GetZone(), GetChannel(), Creature::STATELESS,
			Creature::KARTERANT, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}
	else if (Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		DBComponent::CastleDB::InsertWorldWeaponInfo(m_DBComponent, GetZone(), GetChannel(), Creature::MERKADIA);

		// 월드 웨폰 정보 갱신
		CSiegeObjectDBMgr::GetInstance().UpdateWorldWeaponInfo(GetZone(), GetChannel(), Creature::STATELESS,
			Creature::MERKADIA, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}

	return true;
}

Siege::eReturn	CWorldWeaponDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

	// 구축 완료 상태에서 (장전)
	if (Siege::COMPLETE == m_cState && Siege::WEAPON_CHARGE == m_cSubState)
	{
		if (0 != m_TimeValue.Year)
		{
			CTime chargeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
							m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

			timeSpan = nowTime - chargeTime;
			if (timeSpan.GetTotalMinutes() >= m_cChargeTime)
			{
				// 월드 웨폰 장전 완료
				m_cSubState = Siege::WEAPON_READY;

				// DB에 상태 업데이트
				SYSTEMTIME sysTime;
				::memset(&sysTime, 0, sizeof(SYSTEMTIME));
				::memset(&m_TimeValue, 0, sizeof(TIME));

				DBComponent::CastleDB::UpdateCampSubState(m_DBComponent, GetCampID(), m_cSubState);
				DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

				// 게임 서버에게 결과 전송..
				SendCampCmd(PktCampCmd::WORLDWEAPON_CHARGE_COMPLETE);

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 메세지 전송
				PktCampMessage pktCampMessage;
				memset(&pktCampMessage, 0, sizeof(PktCampMessage));

				// 월드 웨폰 무기 장전 완료
				// 메세지 종류, 존 번호, 월드 웨폰 국가, 공지
				pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_CHARGE_END;
				pktCampMessage.m_dwCampID	= GetCampID();
				pktCampMessage.m_dwGID		= m_dwGID;
                pktCampMessage.m_cZone		= m_cZone;
				pktCampMessage.m_bNotify	= true;

				Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB *>(
					Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

				if (lpGuild) 
				{
					pktCampMessage.m_cNation = lpGuild->GetNation();
				}

				if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
				{
					DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
						reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
				return RET_OK;
			}
		}
	}

	return CCampDB::Process();
}

bool	CWorldWeaponDB::Cancel()
{
	// 월드 웨폰이 없어졌으므로, 월드 웨폰 소유국가 정보 갱신
	CSiegeObjectDBMgr::GetInstance().DeleteWorldWeaponNation(GetZone(), GetChannel());

	return CCampDB::Cancel();
}

bool	CWorldWeaponDB::Destroy(unsigned long dwEnemyGID, bool bDeductFame)
{
	// 월드 웨폰 파괴 정보 DB 에 추가한다.
	if (Siege::KARTERANT_WEAPON == m_wObjectType)
	{
		DBComponent::CastleDB::InsertWorldWeaponInfo(m_DBComponent, GetZone(), GetChannel(), Creature::KARTERANT);

		// 월드 웨폰 정보 갱신
		CSiegeObjectDBMgr::GetInstance().UpdateWorldWeaponInfo(GetZone(), GetChannel(), Creature::STATELESS,
			Creature::KARTERANT, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}
	else if (Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		DBComponent::CastleDB::InsertWorldWeaponInfo(m_DBComponent, GetZone(), GetChannel(), Creature::MERKADIA);

		// 월드 웨폰 정보 갱신
		CSiegeObjectDBMgr::GetInstance().UpdateWorldWeaponInfo(GetZone(), GetChannel(), Creature::STATELESS,
			Creature::MERKADIA, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
	}

	return CCampDB::Destroy(dwEnemyGID, bDeductFame);
}
