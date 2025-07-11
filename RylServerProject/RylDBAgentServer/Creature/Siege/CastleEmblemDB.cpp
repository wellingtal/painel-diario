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

	// 상징물이 부셔지면 성문도 부서지도록 만든다.
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
		case Siege::COMPLETE:	// 소환 완료 상태
		{
		}
		break;

		case Siege::DEVELOPING:	// 소환중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime summonTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - summonTime;
				if (timeSpan.GetTotalMinutes() >= Siege::EMBLEM_SUMMON_TIME)
				{
					// 성 상징물 소환 완료
					m_cState = Siege::COMPLETE;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, Siege::MINE);

					if (m_cSubState == Siege::MINE)
					{
						// 아군이 소환 완료

						// 게임 서버에게 결과 전송..
						// [Out] dwValue1 : SubState (아군이 소환했는지, 적이 소환했는지 정보)
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
						// 적군이 소환 완료.. 성주 변경
						Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle( GetCastleID() );
						
						if (lpCastle)
						{
							if(CGameTimeDBMgr::GetInstance().IsSiegeWarTime())
							{
								// CASTLE_TODO : 공성전은 길드와 무관하다.
								//							CSiegeObjectDBMgr::GetInstance().DestroyCamp(m_dwGID);
								lpCastle->ChangeCastleMaster(m_cEnemyNation);
								m_cSubState = Siege::MINE;								

								// 게임 서버에게 결과 전송..
								// [Out] dwValue1 : 새로운 성의 국가
								SendCastleCmd(m_cEnemyNation, PktCastleCmd::CASTLE_CHANGE_MASTER);
							}
							else	// 공성시간이 이후 소환이 완료 되었을 경우 성에 주인을 바꾸지 않는다.
							{				
								unsigned char cNation = Creature::STATELESS;
								
								if(m_cEnemyNation!=Creature::STATELESS)
								{
									cNation = (m_cEnemyNation==Creature::KARTERANT) ? Creature::MERKADIA : Creature::KARTERANT;
								}
								
								lpCastle->ChangeCastleMaster(cNation);
								m_cSubState = Siege::MINE;

								// 게임 서버에게 결과 전송..
								// [Out] dwValue1 : 새로운 성의 국가
								SendCastleCmd(cNation, PktCastleCmd::CASTLE_CHANGE_MASTER);
							}
						}						
					}						
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
					// 길드 요새 업그레이드 완료
					m_cState = Siege::COMPLETE;
					++m_cUpgradeStep;

					DBComponent::CastleDB::UpdateCastleObjectState(m_DBComponent, m_dwCID, m_cState, m_cSubState);
					DBComponent::CastleDB::UpdateCastleObjectUpgrade(m_DBComponent, m_dwCID, m_cUpgradeStep, m_cUpgradeType);

					// 게임 서버에게 결과 전송..
					// [In/Out] dwValue1 : 업그레이드 단계
					SendCastleCmd(m_cUpgradeStep, PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE);
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}
