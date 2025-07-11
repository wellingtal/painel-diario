#include "stdafx.h"
#include "CastleGateDB.h"
#include "SiegeObjectDBMgr.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>

#include <Utility/Setup/ServerSetup.h>

#include <atltime.h>


CCastleGateDB::CCastleGateDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject)
: CSiegeObjectDB(DBComponent, CastleObject)
{
}

CCastleGateDB::~CCastleGateDB()
{
}

bool	CCastleGateDB::Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold, unsigned char cUpgradeType)
{
	if (Siege::COMPLETE == m_cState)
	{
		if (Siege::MAX_UPGRADE_NUM == m_cUpgradeStep) return false;

		m_cState = Siege::UPGRADING;
		m_cUpgradeTime = cUpgradeTime;

// CASTLE_TODO : 개인 금고에서 빼내야한다.

//		// 업글 비용 길드 창고에서 빼기
//		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
//			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));
//
//		if (lpGuild)
//		{
//			lpGuild->DeductGold(dwUpgradeGold);
//		}

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
		DBComponent::CastleDB::UpdateCastleObjectTime(m_DBComponent, m_dwCID, sysTime);

		return true;
	}

	return false;
}

bool	CCastleGateDB::Repair(unsigned long dwRepairHP, unsigned long dwRepairGold)
{
	if (Siege::COMPLETE == m_cState)
	{
		m_cState = Siege::REPARING;
		m_dwRepairHP = dwRepairHP;
		m_cRepairTime = static_cast<unsigned char>(dwRepairHP * Siege::GATE_REPAIR_MIN_PER_HP);

// CASTLE_TODO : 개인 금고에서 빼내야한다.

//		// 수리 비용 길드 창고에서 빼기
//		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
//			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));
//
//		if (lpGuild)
//		{
//			lpGuild->DeductGold(dwRepairGold);
//		}

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
		DBComponent::CastleDB::UpdateCastleObjectTime(m_DBComponent, m_dwCID, sysTime);

		return true;
	}

	return false;
}

bool	CCastleGateDB::Restore(unsigned char cRestoreTime, unsigned long dwRestoreGold)
{
	if (Siege::DESTROYED == m_cState)
	{
		m_cState = Siege::RESTORING;
		m_cDevelopTime = cRestoreTime;

// CASTLE_TODO : 개인 금고에서 빼내야한다.

		// 복구 비용을 길드 창고에서 빼기
//		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
//			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));
//
//		if (lpGuild)
//		{
//			lpGuild->DeductGold(dwRestoreGold);
//		}

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
		DBComponent::CastleDB::UpdateCastleObjectTime(m_DBComponent, m_dwCID, sysTime);

		return true;
	}

	return false;
}

bool	CCastleGateDB::Destroy(unsigned long dwEnemyNation, bool bDeductFame)
{
	m_cState = Siege::DESTROYED;
	m_cSubState = Siege::OPENED;
	m_cUpgradeStep = 0;
	m_dwHP = 0;

	DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
	DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

	return true;
}

void	CCastleGateDB::UpdateHP(unsigned long dwNowHP)
{
	CSiegeObjectDB::UpdateHP(dwNowHP);

	DBComponent::CastleDB::UpdateCastleObjectHP(m_DBComponent, m_dwCID, m_dwHP);
}

void	CCastleGateDB::ForceOpen()
{
	if (Siege::COMPLETE == m_cState && Siege::OPENED == m_cSubState)
	{
		return ;
	}

	m_cState = Siege::COMPLETE;
	m_cSubState = Siege::OPENED;

	DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);

	// 게임 서버에게 결과 전송..
	SendCastleCmd(0, PktCastleCmd::CASTLE_GATE_FORCE_OPEN);
}

void	CCastleGateDB::ForceClose()
{
	if (Siege::COMPLETE == m_cSubState && Siege::CLOSED == m_cSubState)
	{
		return ;
	}

	m_cState = Siege::COMPLETE;
	m_cSubState = Siege::CLOSED;

	DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);

	// 게임 서버에게 결과 전송..
	SendCastleCmd(0, PktCastleCmd::CASTLE_GATE_FORCE_CLOSE);
}

Siege::eReturn	CCastleGateDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

	switch (m_cState)
	{
		case Siege::UPGRADING:	// 업그레이드중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime upgradeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - upgradeTime;
				if (timeSpan.GetTotalMinutes() >= m_cUpgradeTime)
				{
					// 성문 업그레이드 완료
					m_cState = Siege::COMPLETE;
					++m_cUpgradeStep;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
					DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_cUpgradeStep, PktCastleCmd::CASTLE_UPGRADE_GATE_COMPLETE);
				}
			}
		}
		break;

		case Siege::REPARING:	// 수리중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime repairTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - repairTime;
				if (timeSpan.GetTotalMinutes() >= m_cRepairTime)
				{
					// 수리 완료
					m_cState = Siege::COMPLETE;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_dwRepairHP, PktCastleCmd::CASTLE_REPAIR_GATE_COMPLETE);
					m_dwRepairHP = 0;
				}
			}
		}
		break;

		case Siege::RESTORING:	// 복구중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime restoreTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - restoreTime;
				if (timeSpan.GetTotalMinutes() >= m_cDevelopTime)
				{
					// 복구 완료
					m_cState = Siege::COMPLETE;
					m_cUpgradeStep = 0;
					m_dwHP = 0;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_dwHP, PktCastleCmd::CASTLE_RESTORE_GATE_COMPLETE);
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}
