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

		// ���� ��� ��� â���� ����
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
	// �� ����.
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

/*	// �������� ������� ���� ��� ��� ���� ��� �ּ� ó��
	if (0 != m_LastUseTime.Year && Siege::DEVELOPING != m_cState)
	{
		CTime lastUseTime(m_LastUseTime.Year, m_LastUseTime.Month, m_LastUseTime.Day,
						  m_LastUseTime.Hour, m_LastUseTime.Minute, m_LastUseTime.Second);

		timeSpan = nowTime - lastUseTime;
		if (timeSpan.GetDays() > Siege::CAMP_LEAST_USE_TIME)
		{
			DestroyCamp();
			SendCampCmd(PktCampCmd::CAMP_DESTROY);

			// �� ó��
			Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
			Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

			if (lpGuild)
			{
				lpGuild->AddAllMemberFame( Siege::FAME_FOR_DESTROYED_CAMP );
			}

			// ��� ��� ��ü ����
			return Siege::RET_DESTROY_CAMP;
		}
	}
*/	

	switch (m_cState)
	{
		case Siege::COMPLETE:	// ���� �Ϸ� ����
		{
		}
		break;

		case Siege::DEVELOPING:	// ������
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

					// ��� ��� ���� �Ϸ�
					m_cState = Siege::COMPLETE;
					UpdateLastUseTime();

					// DB�� ���� ������Ʈ
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));					

					if (IsWorldWeapon())
					{
						// ���� ���� ���� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� ��ȣ, ����
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
						// ��� ��� ���� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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

						// ��� ��� ���� �Ϸ�
						// �޼��� ����, �� ��ȣ, ��� �̸�, ����
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

					// ���� �������� ��� ����..
					SendCampCmd(PktCampCmd::CAMP_BUILD_COMPLETE);
				}
				else
				{
					// ������ �϶� 10�� ������ �޼����� �����ش�.
					if (0 < timeSpan.GetTotalMinutes() && m_dwOldTotalMinutes != timeSpan.GetTotalMinutes() &&
						0 == (timeSpan.GetTotalMinutes() % Siege::CAMP_ENCAMPING_INTERVAL))
					{
						m_dwOldTotalMinutes = static_cast<unsigned long>( timeSpan.GetTotalMinutes() );

						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						// �޼��� ����
						PktCampMessage pktCampMessage;
						memset(&pktCampMessage, 0, sizeof(PktCampMessage));
						
						if (IsWorldWeapon())
						{
							// ���� ���� ���� ��
							// �޼��� ����, �� ��ȣ, ���� �Ϸ���� ���� �ð�, ����
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
							// ��� ��� ���� ��
							// �޼��� ����, �� ��ȣ, ���� �Ϸ���� ���� �ð�, ���� �ƴ�
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

		case Siege::CANCELING:	// ���� ������ (��ŸƮŶ���� ������)
		{
			if (0 != m_TimeValue.Year)
			{
				CTime cancelTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
					m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - cancelTime;
				if (timeSpan.GetTotalMinutes() >= Siege::CAMP_CANCELING_TIME)
				{
					// ��� ��� ���� ���� �Ϸ�
					Cancel();

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// ���� ���� ���� ��� �Ϸ�
						// �޼��� ����, �� ��ȣ, ����
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
						// ��� ��� ���� ��� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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

					// ���� �������� ��� ����..
					SendCampCmd(PktCampCmd::CAMP_CANCEL_COMPLETE);

					// ������ �����ִ� ó��
					Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
						Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

					if (lpGuild)
					{
						lpGuild->AddGoldToMaster( Siege::GOLD_FOR_CANCEL_CAMP );
					}

					// ��� ��� ��ü ����
					return Siege::RET_DESTROY_CAMP;
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

					// DB�� ���� ������Ʈ
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampUpgrade(m_DBComponent, GetCampID(), m_cUpgradeStep);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// ���� ���� ���׷��̵� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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
						// ��� ��� ���׷��̵� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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

					// ���� �������� ��� ����..
					SendCampCmd(PktCampCmd::CAMP_UPGRADE_COMPLETE, m_cUpgradeStep);
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
				if (timeSpan.GetTotalSeconds() >= m_cRepairTime)
				{
					// ��� ��� ���� �Ϸ�
					m_cState = Siege::COMPLETE;

					// DB�� ���� ������Ʈ
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					if (IsWorldWeapon())
					{
						// ���� ���� ���� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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
						// ��� ��� ���� �Ϸ�
						// �޼��� ����, �� ��ȣ, ���� �ƴ�
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

					// ���� �������� ��� ����..
					SendCampCmd(PktCampCmd::CAMP_REPAIR_COMPLETE, m_dwRepairHP);
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
				if (timeSpan.GetTotalMinutes() >= Siege::CAMP_TO_STARTKIT_TIME)
				{
					// ��� ��� �ı� �Ϸ�
					Destroy();

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
						Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

					short sFameForSelfDestroyCamp = Siege::FAME_FOR_SELF_DESTROY_CAMP;

					if (IsWorldWeapon())
					{
						// ���� ���� ������ �ı� �Ϸ� ����
						// �޼��� ����, �� ��ȣ, ��� �̸�, ����, ����
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_WEAPON_SELF_DESTROY_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());	// ��� �̸�
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
						// ��� ��� ������ �ı� �Ϸ�
						// �޼��� ����, �� ��ȣ, ��, �����ִ� ���, ���� �ƴ�
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

						// ��� ��� ������ �ı� �Ϸ� ����
						// �޼��� ����, �� ��ȣ, ��� �̸�, ����
						pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_SELF_DESTROY_END;
						pktCampMessage.m_dwCampID	= GetCampID();
						pktCampMessage.m_dwGID		= m_dwGID;
						pktCampMessage.m_cZone		= m_cZone;
						pktCampMessage.m_bNotify	= true;

						if (NULL != lpGuild)
						{
							strcpy(pktCampMessage.m_szGuildName, lpGuild->GetName());	// ��� �̸�
						}

						if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
						{
							DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
								reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
						}
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// ���� �������� ��� ����..
					SendCampCmd(PktCampCmd::CAMP_TO_STARTKIT_COMPLETE);

					// �� ó��, ������ �����ִ� ó��
					if (lpGuild)
					{
						lpGuild->AddAllMemberFame( Guild::TYPE_VALUE, sFameForSelfDestroyCamp );
						lpGuild->AddGoldToMaster( Siege::GOLD_FOR_SELF_DESTROY_CAMP );
					}

					// ��� ��� ��ü ����
					return Siege::RET_DESTROY_CAMP;
				}
			}
		}
		break;


		case Siege::CHANGING:	// ����� Ÿ�� ������
		{
			if (0 != m_TimeValue.Year)
			{
				CTime changeTime(m_TimeValue.Year, m_TimeValue.Month, m_TimeValue.Day,
						m_TimeValue.Hour, m_TimeValue.Minute, m_TimeValue.Second);

				timeSpan = nowTime - changeTime;
				if (timeSpan.GetTotalMinutes() >= m_cChangeTime)
				{
					// ��� ��� Ÿ�� ���� �Ϸ�
					m_cState = Siege::COMPLETE;

					// ä���⿴�ٸ�, ���� ���� �� �ӽ� ������ ��� ������.
					if (Siege::MINING_CAMP == m_wObjectType)
					{
						DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::ACCUMULATED_MINERAL);
						DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::TEMPORARY_MINERAL);
					}
					
					// �������� �ٸ� Ÿ���� �Ǵ� ��� DB ����
					if (Siege::CAMP_SHOP == m_wObjectType)
					{
						DBComponent::CastleDB::DeleteCampShopInfo(m_DBComponent, GetCampID());
					}
					
					// �������� �ٲ�� ��� DB ����
					if (Siege::CAMP_SHOP == m_wChangeType)
					{
						DBComponent::CastleDB::InsertCampShopInfo(m_DBComponent, GetCampID());
					}

					// ���� ������Ʈ
					SYSTEMTIME sysTime;
					::memset(&sysTime, 0, sizeof(SYSTEMTIME));
					::memset(&m_TimeValue, 0, sizeof(TIME));

					DBComponent::CastleDB::UpdateCampState(m_DBComponent, GetCampID(), m_cState);
					DBComponent::CastleDB::UpdateCampType(m_DBComponent, GetCampID(), m_wChangeType);
					DBComponent::CastleDB::UpdateCampTime(m_DBComponent, GetCampID(), Siege::TYPE_REMAIN_TIME, sysTime);


					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// �޼��� ����
					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));

					// ��� ��� Ÿ�� ���� �Ϸ�
					// �޼��� ����, �� ��ȣ, ������ Ÿ��, ���� �ƴ�
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

					// ���� �������� ��� ����..
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

	// ��� ����
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