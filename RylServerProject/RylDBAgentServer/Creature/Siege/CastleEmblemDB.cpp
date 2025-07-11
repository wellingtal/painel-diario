#include "stdafx.h"
#include "CastleEmblemDB.h"
#include "SiegeObjectDBMgr.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <Castle/CastleDB.h>
#include <Castle/CastleDBMgr.h>

#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>

#include <Utility/Setup/ServerSetup.h>

#include <atltime.h>

#include <GameTime/GameTimeDBMgr.h>



CCastleEmblemDB::CCastleEmblemDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject)
: CSiegeObjectDB(DBComponent, CastleObject), m_cEnemyNation(0)
{
	m_cSubState = Siege::MINE;
}

CCastleEmblemDB::~CCastleEmblemDB()
{
}

bool	CCastleEmblemDB::Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold, unsigned char cUpgradeType)
{
	if (Siege::COMPLETE == m_cState)
	{
		if (Siege::MAX_EMBLEM_UPGRADE == m_cUpgradeStep) return false;

		m_cState = Siege::UPGRADING;
		if (0 == m_cUpgradeStep) m_cUpgradeType = cUpgradeType;
		m_cUpgradeTime = cUpgradeTime;

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

bool	CCastleEmblemDB::Destroy(unsigned long dwEnemyNation, bool bDeductFame)
{
	m_cState = Siege::DEVELOPING;
	m_cUpgradeStep = 0;
	m_cUpgradeType = Siege::NO_JEWEL;

	if (m_cSubState == Siege::ENEMY)
	{
		m_cSubState = Siege::MINE;
		m_cEnemyNation = 0;
	}
	else
	{
		m_cSubState = Siege::ENEMY;
		m_cEnemyNation = (unsigned char)dwEnemyNation;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_TimeValue.Year	= sysTime.wYear;
	m_TimeValue.Month	= sysTime.wMonth;
	m_TimeValue.Day		= sysTime.wDay;
	m_TimeValue.Hour	= sysTime.wHour;
	m_TimeValue.Minute	= sysTime.wMinute;
	m_TimeValue.Second	= sysTime.wSecond;

	DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, Siege::MINE);
	DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);
	DBComponent::CastleDB::UpdateCastleObjectTime(m_DBComponent, m_dwCID, sysTime);

	// ��¡���� �μ����� ������ �μ������� �����.
	Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle(m_dwCampOrCastleID);
	if (0 != lpCastle)
	{
		CSiegeObjectDB* lpGate = lpCastle->GetCastleGate();
		if (0 != lpGate && Siege::DESTROYED != lpGate->GetState())
		{
			lpGate->Destroy();
			lpGate->SendCastleCmd(0, PktCastleCmd::CASTLE_DESTROY_GATE);
		}
	}

	return true;
}

void	CCastleEmblemDB::UpdateHP(unsigned long dwNowHP)
{
	CSiegeObjectDB::UpdateHP(dwNowHP);

	DBComponent::CastleDB::UpdateCastleObjectHP(m_DBComponent, m_dwCID, m_dwHP);
}


Siege::eReturn	CCastleEmblemDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

	switch (m_cState)
	{
		case Siege::COMPLETE:	// ��ȯ �Ϸ� ����
		{
		}
		break;

		case Siege::DEVELOPING:	// ��ȯ��
		{
			if (0 != m_TimeValue.Year)
			{
				CTime summonTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - summonTime;
				if (timeSpan.GetTotalMinutes() >= Siege::EMBLEM_SUMMON_TIME)
				{
					// �� ��¡�� ��ȯ �Ϸ�
					m_cState = Siege::COMPLETE;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, Siege::MINE);

					if (m_cSubState == Siege::MINE)
					{
						// �Ʊ��� ��ȯ �Ϸ�

						// ���� �������� ��� ����..
						// [Out] dwValue1 : SubState (�Ʊ��� ��ȯ�ߴ���, ���� ��ȯ�ߴ��� ����)
						SendCastleCmd(m_cSubState, PktCastleCmd::CASTLE_SUMMON_EMBLEM_COMPLETE);

						Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle( GetCastleID() );

						if(lpCastle)
						{
							lpCastle->ChangeCastleMaster(static_cast<unsigned char>(lpCastle->GetNation()));

							SendCastleCmd(lpCastle->GetNation(), PktCastleCmd::CASTLE_CHANGE_MASTER);
						}
					}
					else
					{
						// ������ ��ȯ �Ϸ�.. ���� ����
						Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle( GetCastleID() );
						
						if (lpCastle)
						{
							if(CGameTimeDBMgr::GetInstance().IsSiegeWarTime())
							{
								// CASTLE_TODO : �������� ���� �����ϴ�.
								//							CSiegeObjectDBMgr::GetInstance().DestroyCamp(m_dwGID);
								lpCastle->ChangeCastleMaster(m_cEnemyNation);
								m_cSubState = Siege::MINE;								

								// ���� �������� ��� ����..
								// [Out] dwValue1 : ���ο� ���� ����
								SendCastleCmd(m_cEnemyNation, PktCastleCmd::CASTLE_CHANGE_MASTER);
							}
							else	// �����ð��� ���� ��ȯ�� �Ϸ� �Ǿ��� ��� ���� ������ �ٲ��� �ʴ´�.
							{				
								unsigned char cNation = Creature::STATELESS;
								
								if(m_cEnemyNation!=Creature::STATELESS)
								{
									cNation = (m_cEnemyNation==Creature::KARTERANT) ? Creature::MERKADIA : Creature::KARTERANT;
								}
								
								lpCastle->ChangeCastleMaster(cNation);
								m_cSubState = Siege::MINE;

								// ���� �������� ��� ����..
								// [Out] dwValue1 : ���ο� ���� ����
								SendCastleCmd(cNation, PktCastleCmd::CASTLE_CHANGE_MASTER);
							}
						}						
					}						
				}
			}
		}
		break;

		case Siege::UPGRADING:	// ���׷��̵���
		{
			if (0 != m_TimeValue.Year)
			{
				CTime upgradeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - upgradeTime;
				if (timeSpan.GetTotalMinutes() >= m_cUpgradeTime)
				{
					// ��� ��� ���׷��̵� �Ϸ�
					m_cState = Siege::COMPLETE;
					++m_cUpgradeStep;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
					DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

					// ���� �������� ��� ����..
					// [In/Out] dwValue1 : ���׷��̵� �ܰ�
					SendCastleCmd(m_cUpgradeStep, PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE);
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}
