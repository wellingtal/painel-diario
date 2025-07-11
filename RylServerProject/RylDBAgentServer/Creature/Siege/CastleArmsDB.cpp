#include "stdafx.h"
#include "CastleArmsDB.h"
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


CCastleArmsDB::CCastleArmsDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject)
: CSiegeObjectDB(DBComponent, CastleObject)
{
}

CCastleArmsDB::~CCastleArmsDB()
{
}

bool	CCastleArmsDB::Build(unsigned short wType, unsigned char cDevelopTime, unsigned long dwDevelopGold, unsigned long dwOwnerID)
{
	if (Siege::CASTLE_ARMS_NPC != m_wObjectType) return false;

	if (Siege::COMPLETE == m_cState)
	{
		m_wObjectType = wType;
		m_cState = Siege::DEVELOPING;
		m_cUpgradeStep = 0;
		m_cDevelopTime = cDevelopTime;
		m_dwOwnerID = dwOwnerID;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCastleObjectType(m_DBComponent, m_dwCID, wType, m_dwOwnerID);
		DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
		DBComponent::CastleDB::UpdateCastleObjectTime(m_DBComponent, m_dwCID, sysTime);

		return true;
	}

	return false;
}

bool	CCastleArmsDB::Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold, unsigned char cUpgradeType)
{
	if ( !IsCastleArms() )
	{
		return false;
	}

	if (Siege::COMPLETE == m_cState)
	{
		m_cState = Siege::UPGRADING;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		m_cUpgradeTime = cUpgradeTime;

// CASTLE_TODO : 개인 금고에서 돈을 빼야한다.

//		// 업글 비용 길드 창고에서 빼기
//		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
//			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));
//
//		if (lpGuild)
//		{
//			lpGuild->DeductGold(dwUpgradeGold);
//		}

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

bool	CCastleArmsDB::Repair(unsigned long dwRepairHP, unsigned long dwRepairGold)
{
	if ( !IsCastleArms() )
	{
		return false;
	}

	if (Siege::COMPLETE == m_cState)
	{
		m_cState = Siege::REPARING;
		m_dwRepairHP = dwRepairHP;
		m_cRepairTime = Siege::CASTLE_ARMS_REPAIR_TIME;

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

bool	CCastleArmsDB::Destroy(unsigned long dwEnemyNation, bool bDeductFame)
{
	if ( !IsCastleArms() || Siege::CASTLE_ARMS_NPC == m_wObjectType )
	{
		return false;
	}

	m_wObjectType = Siege::CASTLE_ARMS_NPC;
	m_cState = Siege::COMPLETE;
	m_cUpgradeStep = 0;
	m_dwHP = 0;
	m_dwOwnerID = 0;

	DBComponent::CastleDB::UpdateCastleObjectType(m_DBComponent, m_dwCID, m_wObjectType, m_dwOwnerID);
	DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
	DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

	return true;
}

void	CCastleArmsDB::UpdateLastUseTime(bool bInit)
{
	CSiegeObjectDB::UpdateLastUseTime(bInit);

	if (!bInit)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		DBComponent::CastleDB::UpdateCastleObjectLastUseTime(m_DBComponent, m_dwCID, sysTime);
	}
}

void	CCastleArmsDB::UpdateHP(unsigned long dwNowHP)
{
	CSiegeObjectDB::UpdateHP(dwNowHP);

	DBComponent::CastleDB::UpdateCastleObjectHP(m_DBComponent, m_dwCID, m_dwHP);
}

Siege::eReturn	CCastleArmsDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

	switch (m_cState)
	{
		case Siege::COMPLETE:	// 병기 개발 완료된 정상 상태일때
		{
			if (0 != m_LastUseTime.Year && Siege::CASTLE_ARMS_NPC != m_wObjectType)
			{
				CTime lastUseTime(m_LastUseTime.Year, m_LastUseTime.Month, m_LastUseTime.Day,
						m_LastUseTime.Hour, m_LastUseTime.Minute, m_LastUseTime.Second);

				timeSpan = nowTime - lastUseTime;
				if (timeSpan.GetTotalMinutes() >= Siege::CASTLE_ARMS_LEAST_USE_TIME)
				{
					// 병기 파괴
					Destroy();

					// 게임 서버에게 결과 전송..
					SendCastleCmd(0, PktCastleCmd::CASTLE_DESTROY_ARMS);
				}
			}
		}
		break;

		case Siege::DEVELOPING:	// 병기 개발중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime developTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - developTime;
				if (timeSpan.GetTotalMinutes() >= m_cDevelopTime)
				{
					// 병기 개발 완료
					m_cState = Siege::COMPLETE;
					
					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);

					UpdateLastUseTime();

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_wObjectType, PktCastleCmd::CASTLE_CREATE_ARMS_COMPLETE);
				}
			}
		}
		break;

		case Siege::UPGRADING:	// 업그레이드중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime upgradeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - upgradeTime;
				if (timeSpan.GetTotalMinutes() >= m_cUpgradeTime)
				{
					// 수리 완료
					m_cState = Siege::COMPLETE;
					++m_cUpgradeStep;
                    
					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
					DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

					UpdateLastUseTime();

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_cUpgradeStep, PktCastleCmd::CASTLE_UPGRADE_ARMS_COMPLETE);
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

					UpdateLastUseTime();

					// 게임 서버에게 결과 전송..
					SendCastleCmd(m_dwRepairHP, PktCastleCmd::CASTLE_REPAIR_ARMS_COMPLETE);
					m_dwRepairHP = 0;
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}
