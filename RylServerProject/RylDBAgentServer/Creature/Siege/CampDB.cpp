#include "stdafx.h"
#include "CampDB.h"
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


CCampDB::CCampDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo)
: CSiegeObjectDB(DBComponent, CampInfo), m_wChangeType(0),
  m_dwOldTotalMinutes(Siege::CAMP_ENCAMPING_TIME)
{
	::memcpy(&m_CampRight, CampInfo.m_szRight, sizeof(CampRight));
}

CCampDB::CCampDB(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwGID,
		unsigned char cZone, unsigned char cChannel, Position Pos)
: CSiegeObjectDB(DBComponent, dwCampID, dwGID, cZone, cChannel, Pos),
  m_wChangeType(0), m_dwOldTotalMinutes(Siege::CAMP_ENCAMPING_TIME)
{
}

CCampDB::~CCampDB()
{
}


bool	CCampDB::CancelBuild()
{
	if (m_cState == Siege::DEVELOPING)
	{
		m_cState = Siege::CANCELING;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
		DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

		return true;
	}

	return false;
}

bool	CCampDB::Cancel()
{
	m_cState = Siege::DESTROYED;
	m_dwHP = 0;
	m_cUpgradeStep = 0;

	return true;
}

bool	CCampDB::Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold, unsigned char cUpgradeType)
{
	if (Siege::COMPLETE == m_cState)
	{
		if (Siege::MAX_UPGRADE_NUM == m_cUpgradeStep) return false;

		m_cState = Siege::UPGRADING;
		m_cUpgradeTime = cUpgradeTime;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
		DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

		return true;
	}

	return false;
}

bool	CCampDB::Repair(unsigned long dwRepairHP, unsigned long dwRepairGold)
{
	if (Siege::COMPLETE == m_cState)
	{
        m_cState = Siege::REPARING;
		m_dwRepairHP = dwRepairHP;
		m_cRepairTime = Siege::CAMP_REPAIR_TIME;

		// 수리 비용 길드 창고에서 빼기
		if (dwRepairGold > 0)
		{
			Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
				Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

			if (lpGuild)
			{
				lpGuild->DeductGold(dwRepairGold);
			}
		}

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
		DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

		return true;
	}

	return false;
}

bool	CCampDB::Destroy(unsigned long dwEnemyNation, bool bDeductFame)
{
	m_cState = Siege::DESTROYED;
	m_dwHP = 0;
	m_cUpgradeStep = 0;

	return true;
}

bool	CCampDB::ChangeType(unsigned short wType)
{
	if (Siege::COMPLETE == m_cState)
	{
		m_cState = Siege::CHANGING;
		m_cChangeTime = Siege::CAMP_CHANGING_TIME;
		m_wChangeType = wType;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_TimeValue.Year	= sysTime.wYear;
		m_TimeValue.Month	= sysTime.wMonth;
		m_TimeValue.Day		= sysTime.wDay;
		m_TimeValue.Hour	= sysTime.wHour;
		m_TimeValue.Minute	= sysTime.wMinute;
		m_TimeValue.Second	= sysTime.wSecond;

		DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
		DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

		return true;
	}

	return true;
}

void	CCampDB::UpdateLastUseTime(bool bInit)
{
	CSiegeObjectDB::UpdateLastUseTime(bInit);

	if (!bInit)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_LAST_USE_TIME, sysTime);
	}
}

void	CCampDB::UpdateHP(unsigned long dwNowHP)
{
	CSiegeObjectDB::UpdateHP(dwNowHP);

	DBComponent::CastleDB::UpdateCampHP(m_DBComponent, GetCampID(), m_dwHP);
}

bool	CCampDB::ToStartKit()
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

	DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
	DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

	return true;
}

void	CCampDB::SetRight(CampRight campRight)
{
	m_CampRight = campRight;
	DBComponent::CastleDB::UpdateCampRight(m_DBComponent, GetCampID(), reinterpret_cast<char*>(&m_CampRight), sizeof(CampRight));
}

void	CCampDB::SetMaterialNum(unsigned char cMaterial)
{
	CSiegeObjectDB::SetMaterialNum(cMaterial);
	DBComponent::CastleDB::UpdateCampMaterial(m_DBComponent, GetCampID(), m_cMaterial);
}

void CCampDB::UpdateRewardFame(int Fame)
{
	// 명성 보상.
	Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
		Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

	if (lpGuild)
	{
		lpGuild->AddLoginedMemberFame( Guild::TYPE_VALUE, Fame );
	}
}

Siege::eReturn	CCampDB::Process()
{
	CTime nowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(0);

/*	// 오랬동안 사용하지 않은 길드 요새 폭파 기능 주석 처리
	if (0 != m_LastUseTime.Year && Siege::DEVELOPING != m_cState)
	{
		CTime lastUseTime(m_LastUseTime.Year, m_LastUseTime.Month, m_LastUseTime.Day,
						  m_LastUseTime.Hour, m_LastUseTime.Minute, m_LastUseTime.Second);

		timeSpan = nowTime - lastUseTime;
		if (timeSpan.GetDays() > Siege::CAMP_LEAST_USE_TIME)
		{
			DestroyCamp();
			SendCampCmd(PktCampCmd::CAMP_DESTROY);

			// 명성 처리
			Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

			if (lpGuild)
			{
				lpGuild->AddAllMemberFame( Siege::FAME_FOR_DESTROYED_CAMP );
			}

			// 길드 요새 객체 삭제
			return Siege::RET_DESTROY_CAMP;
		}
	}
*/	

	switch (m_cState)
	{
		case Siege::COMPLETE:	// 구축 완료 상태
		{
		}
		break;

		case Siege::DEVELOPING:	// 구축중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime developTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - developTime;
				if (timeSpan.GetTotalMinutes() >= Siege::CAMP_ENCAMPING_TIME)
				{
					Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
						Guild::CGuildDBMgr::GetInstance().GetGuild( m_dwGID ));					

					// 길드 요새 구축 완료
					m_cState = Siege::COMPLETE;
					UpdateLastUseTime();

					// DB에 상태 업데이트
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));					

					if (IsWorldWeapon())
					{
						// 월드 웨폰 구축 완료
						// 메세지 종류, 존 번호, 국가 번호, 공지
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_ENCAMP_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							pktCampMessage.m_cNation = lpGuild->GetNation();
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 구축 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_ENCAMP_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}

						memset(&pktCampMessage, 0, sizeof(PktCampMessage));

						// 길드 요새 구축 완료
						// 메세지 종류, 존 번호, 길드 이름, 공지
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_ENCAMP_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_BUILD_COMPLETE);
				}
				else
				{
					// 구축중 일때 10분 단위로 메세지를 보내준다.
					if (0 < timeSpan.GetTotalMinutes() && m_dwOldTotalMinutes != timeSpan.GetTotalMinutes() &&
						0 == (timeSpan.GetTotalMinutes() % Siege::CAMP_ENCAMPING_INTERVAL))
					{
						m_dwOldTotalMinutes = static_cast<unsigned long>( timeSpan.GetTotalMinutes() );

						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						// 메세지 전송
						PktCampMessage pktCampMessage;
						memset(&pktCampMessage, 0, sizeof(PktCampMessage));
						
						if (IsWorldWeapon())
						{
							// 월드 웨폰 구축 중
							// 메세지 종류, 존 번호, 구축 완료까지 남은 시간, 공지
							pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_WEAPON_ENCAMPING;
							pktCampMessage.m_dwCampID		= GetCampID();
							pktCampMessage.m_dwGID			= m_dwGID;
							pktCampMessage.m_cZone			= m_cZone;
							pktCampMessage.m_cRemainTime	= static_cast<unsigned char>(Siege::CAMP_ENCAMPING_TIME - timeSpan.GetTotalMinutes());
							pktCampMessage.m_bNotify		= true;

							if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
							{
								DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
									reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
							}
						}
						else
						{
							// 길드 요새 구축 중
							// 메세지 종류, 존 번호, 구축 완료까지 남은 시간, 공지 아님
							pktCampMessage.m_cMsgCmd		= PktCampMessage::MSGCMD_ENCAMPING;
							pktCampMessage.m_dwCampID		= GetCampID();
							pktCampMessage.m_dwGID			= m_dwGID;
							pktCampMessage.m_cZone			= m_cZone;
							pktCampMessage.m_cRemainTime	= static_cast<unsigned char>(Siege::CAMP_ENCAMPING_TIME - timeSpan.GetTotalMinutes());
							pktCampMessage.m_bNotify		= false;

							if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
							{
								DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
									reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
							}
						}
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}
		}
		break;

		case Siege::CANCELING:	// 구축 해제중 (스타트킷으로 돌리기)
		{
			if (0 != m_TimeValue.Year)
			{
				CTime cancelTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - cancelTime;
				if (timeSpan.GetTotalMinutes() >= Siege::CAMP_CANCELING_TIME)
				{
					// 길드 요새 구축 해제 완료
					Cancel();

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// 월드 웨폰 구축 취소 완료
						// 메세지 종류, 존 번호, 공지
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_CANCEL_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 구축 취소 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_CANCEL_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_CANCEL_COMPLETE);

					// 돈으로 돌려주는 처리
					Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
						Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

					if (lpGuild)
					{
						lpGuild->AddGoldToMaster( Siege::GOLD_FOR_CANCEL_CAMP );
					}

					// 길드 요새 객체 삭제
					return Siege::RET_DESTROY_CAMP;
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

					// DB에 상태 업데이트
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampUpgrade(m_DBComponent, GetCampID(), m_cUpgradeStep);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// 월드 웨폰 업그레이드 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_UPGRADE_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 업그레이드 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_UPGRADE_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_UPGRADE_COMPLETE, m_cUpgradeStep);
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
				if (timeSpan.GetTotalSeconds() >= m_cRepairTime)
				{
					// 길드 요새 수리 완료
					m_cState = Siege::COMPLETE;

					// DB에 상태 업데이트
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// 월드 웨폰 수리 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_REPAIR_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 수리 완료
						// 메세지 종류, 존 번호, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_REPAIR_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_REPAIR_COMPLETE, m_dwRepairHP);
					m_dwRepairHP = 0;
				}
			}
		}
		break;

		case Siege::DESTROYING:	// 스타트킷으로 돌리기 중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime destroyTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - destroyTime;
				if (timeSpan.GetTotalMinutes() >= Siege::CAMP_TO_STARTKIT_TIME)
				{
					// 길드 요새 파괴 완료
					Destroy();

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
						Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

					short sFameForSelfDestroyCamp = Siege::FAME_FOR_SELF_DESTROY_CAMP;

					if (IsWorldWeapon())
					{
						// 월드 웨폰 스스로 파괴 완료 공지
						// 메세지 종류, 존 번호, 길드 이름, 국가, 공지
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_SELF_DESTROY_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());	// 길드 이름
							pktCampMessage.m_cNation = lpGuild->GetNation();
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					else
					{
						// 길드 요새 스스로 파괴 완료
						// 메세지 종류, 존 번호, 명성, 돌려주는 골드, 공지 아님
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_SELF_DESTROY_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_nValue1	= -(sFameForSelfDestroyCamp);
						pktCampMessage.m_nValue2	= Siege::GOLD_FOR_SELF_DESTROY_CAMP;
						pktCampMessage.m_bNotify	= false;

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}

						memset(&pktCampMessage, 0, sizeof(PktCampMessage));

						// 길드 요새 스스로 파괴 완료 공지
						// 메세지 종류, 존 번호, 길드 이름, 공지
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_SELF_DESTROY_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());	// 길드 이름
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_TO_STARTKIT_COMPLETE);

					// 명성 처리, 돈으로 돌려주는 처리
					if (lpGuild)
					{
						lpGuild->AddAllMemberFame( Guild::TYPE_VALUE, sFameForSelfDestroyCamp );
						lpGuild->AddGoldToMaster( Siege::GOLD_FOR_SELF_DESTROY_CAMP );
					}

					// 길드 요새 객체 삭제
					return Siege::RET_DESTROY_CAMP;
				}
			}
		}
		break;


		case Siege::CHANGING:	// 길드요새 타입 변형중
		{
			if (0 != m_TimeValue.Year)
			{
				CTime changeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
						m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - changeTime;
				if (timeSpan.GetTotalMinutes() >= m_cChangeTime)
				{
					// 길드 요새 타입 변형 완료
					m_cState = Siege::COMPLETE;

					// 채굴기였다면, 현재 광물 및 임시 광물을 모두 날린다.
					if (Siege::MINING_CAMP == m_wObjectType)
					{
						DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::ACCUMULATED_MINERAL);
						DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::TEMPORARY_MINERAL);
					}
					
					// 상점에서 다른 타입이 되는 경우 DB 삭제
					if (Siege::CAMP_SHOP == m_wObjectType)
					{
						DBComponent::CastleDB::DeleteCampShopInfo(m_DBComponent, GetCampID());
					}
					
					// 상점으로 바뀌는 경우 DB 생성
					if (Siege::CAMP_SHOP == m_wChangeType)
					{
						DBComponent::CastleDB::InsertCampShopInfo(m_DBComponent, GetCampID());
					}

					// 상태 업데이트
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampType(m_DBComponent, GetCampID(), m_wChangeType);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);


					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 메세지 전송
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					// 길드 요새 타입 변형 완료
					// 메세지 종류, 존 번호, 변형된 타입, 공지 아님
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_CHANGE_TYPE_END;
					pktCampMessage.m_dwCampID	= GetCampID();
					pktCampMessage.m_dwGID		= m_dwGID;
					pktCampMessage.m_cZone		= m_cZone;
					pktCampMessage.m_nValue1	= m_wChangeType;
					pktCampMessage.m_bNotify	= false;

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// 게임 서버에게 결과 전송..
					SendCampCmd(PktCampCmd::CAMP_CHANGE_COMPLETE, m_wChangeType);

					return Siege::RET_CHANGE_TYPE;
				}
			}
		}
		break;
	}

	return Siege::RET_OK;
}


bool	CCampDB::SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out)
{
	if (!szBuffer_Out)
	{
		return false;
	}

	usBufferSize_Out = 0;
	dwCID_Out = m_dwCID;

	// 요새 정보
	CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(szBuffer_Out);

	lpCampInfoDB->m_dwCampID		= GetCampID();
	lpCampInfoDB->m_dwGID			= m_dwGID;
	lpCampInfoDB->m_dwHP			= m_dwHP;
	lpCampInfoDB->m_cZone			= m_cZone;
	lpCampInfoDB->m_cChannel		= 0;
	lpCampInfoDB->m_wObjectType		= m_wObjectType;
	lpCampInfoDB->m_cState			= m_cState;
	lpCampInfoDB->m_cSubState		= m_cSubState;
	lpCampInfoDB->m_cUpgradeStep	= m_cUpgradeStep;
	lpCampInfoDB->m_TimeValue		= m_TimeValue;
	lpCampInfoDB->m_LastUseTime		= m_LastUseTime;
	::memcpy(lpCampInfoDB->m_szRight, m_CampRight.m_aryCampRight, CampRight::MAX_RIGHT_NUM);
    lpCampInfoDB->m_Pos.fPointX		= m_Pos.m_fPointX;
	lpCampInfoDB->m_Pos.fPointY		= m_Pos.m_fPointY;
	lpCampInfoDB->m_Pos.fPointZ		= m_Pos.m_fPointZ;
	lpCampInfoDB->m_cMaterial		= m_cMaterial;
	lpCampInfoDB->m_cSiegeCount		= m_cSiegeCount;

	usBufferSize_Out += sizeof(CampInfoDB);

	return true;
}