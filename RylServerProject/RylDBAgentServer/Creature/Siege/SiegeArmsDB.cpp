#include "stdafx.h"
#include "SiegeArmsDB.h"
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


CSiegeArmsDB::CSiegeArmsDB(CDBComponent& DBComponent, unsigned long dwOwnerID, 
			 unsigned char cNation, unsigned short wObjectType, unsigned char cUpgradeStep, 
			 unsigned char cZone, unsigned char cChannel, Position Pos)
: CSiegeObjectDB(DBComponent, dwOwnerID, cNation, wObjectType, cUpgradeStep, cZone, cChannel, Pos)
{
}

CSiegeArmsDB::~CSiegeArmsDB()
{
}

bool	CSiegeArmsDB::Repair(unsigned long dwRepairHP, unsigned long dwRepairGold)
{
	if (Siege::COMPLETE == m_cState)
	{
		m_cState		= Siege::REPARING;
		m_dwRepairHP	= dwRepairHP;
		m_cRepairTime	= Siege::SIEGE_ARMS_REPAIR_TIME;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		return true;
	}

	return false;
}

bool	CSiegeArmsDB::Destroy(unsigned long dwEnemyNation, bool bDeductFame)
{
	m_cState = Siege::DESTROYED;
	m_dwHP = 0;
	m_cUpgradeStep = 0;

	return true;
}

bool	CSiegeArmsDB::ToStartKit()
{
	m_cState = Siege::DESTROYING;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_TimeValue.Year	= sysTime.wYear;
	m_TimeValue.Month	= sysTime.wMonth;
	m_TimeValue.Day		= sysTime.wDay;
	m_TimeValue.Hour	= sysTime.wHour;
	m_TimeValue.Minute	= sysTime.wMinute;
	m_TimeValue.Second	= sysTime.wSecond;

	return true;
}


Siege::eReturn	CSiegeArmsDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

	switch (m_cState)
	{
		case Siege::COMPLETE:	// �Ϸ� ����
		{
			if (0 != m_LastUseTime.Year)
			{
				CTime lastUseTime(m_LastUseTime.Year, m_LastUseTime.Month, m_LastUseTime.Day,
						m_LastUseTime.Hour, m_LastUseTime.Minute, m_LastUseTime.Second);

				timeSpan = nowTime - lastUseTime;
				if (timeSpan.GetTotalMinutes() > Siege::SIEGE_ARMS_LEAST_USE_TIME)
				{
					Destroy();
					SendSiegeArmsCmd(0, PktSiegeArmsCmd::SIEGE_DESTROY_ARMS);

					// ���� ���� ��ü ����
					return Siege::RET_DESTROY_SIEGE_ARMS;
				}
			}
		}
		break;

		case Siege::DEVELOPING:	// ������
		{
			if (0 != m_TimeValue.Year)
			{
				CTime developTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - developTime;
				if (timeSpan.GetTotalMinutes() >= Siege::SIEGE_ARMS_BUILDING_TIME)
				{
					// ���� ���� ���� �Ϸ�
					m_cState = Siege::COMPLETE;
					UpdateLastUseTime();

					// ���� �������� ��� ����..
					SendSiegeArmsCmd(m_cUpgradeStep, PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE);
				}
			}
		}
		break;

		case Siege::REPARING:	// ������
		{
			if (0 != m_TimeValue.Year)
			{
				CTime repairTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - repairTime;
				if (timeSpan.GetTotalMinutes() >= m_cRepairTime)
				{
					// ���� ���� ���� �Ϸ�
					m_cState = Siege::COMPLETE;
					UpdateLastUseTime();

					// ���� �������� ��� ����..
					SendSiegeArmsCmd(m_dwRepairHP, PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE);
					m_dwRepairHP = 0;
				}
			}
		}
		break;

		case Siege::DESTROYING:	// ��ŸƮŶ���� ������ ��
		{
			if (0 != m_TimeValue.Year)
			{
				CTime destroyTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - destroyTime;
				if (timeSpan.GetTotalMinutes() >= Siege::SIEGE_ARMS_TO_STARTKIT_TIME)
				{
					// ���� ���� �ı�
					Destroy();
					UpdateLastUseTime();

					// ���� �������� ��� ����..
					SendSiegeArmsCmd(1, PktSiegeArmsCmd::SIEGE_TO_STARTKIT_COMPLETE);

					// ���� ���� ��ü ����
					return Siege::RET_DESTROY_SIEGE_ARMS;
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}
