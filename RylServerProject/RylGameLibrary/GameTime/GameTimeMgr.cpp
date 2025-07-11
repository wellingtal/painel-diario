#include "stdafx.h"
#include "GameTimeConstants.h"
#include "GameTimeMgr.h"

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <Castle/CastleMgr.h>

#include <Creature/CreatureManager.h>


CGameTimeMgr::CGameTimeMgr()
:	m_cNowGuildTime(GameTime::GT_NORMAL),
	m_cNowRealmTime(GameTime::RT_NORMAL),
	m_cNowSiegeTime(GameTime::ST_NORMAL),
	m_cNowEnterTime(GameTime::ET_LIMIT),
	m_cNowEventTime(GameTime::EVENT_NORMAL),
	m_dwOldGuildRemainSec(0),
	m_dwOldRealmRemainSec(0),
	m_dwOldSiegeRemainSec(0),
	m_dwOldEnterRemainSec(0),
	m_dwOldEventRemainSec(0)
{
}

CGameTimeMgr::~CGameTimeMgr()
{
}

CGameTimeMgr&	CGameTimeMgr::GetInstance()
{
	static CGameTimeMgr	ms_this;
	return ms_this;
}

void	CGameTimeMgr::SetTimeValue(unsigned char cTimeType, unsigned char cTimeValue, unsigned char cMonth, unsigned char cDay,
								   unsigned char cHour, unsigned char cMinute, unsigned long dwDuration)
{
	switch (cTimeType)
	{
		case GameTime::GUILD:
		{
			m_cNowGuildTime = cTimeValue;
			m_GuildDetailTimeInfo = sDetailTimeInfo(cMonth, cDay, cHour, cMinute, dwDuration);
			m_dwOldGuildRemainSec = GetDetailRemainSec(GameTime::GUILD);
		}
		break;

		case GameTime::REALM:
		{
			m_cNowRealmTime = cTimeValue;
			m_RealmDetailTimeInfo = sDetailTimeInfo(cMonth, cDay, cHour, cMinute, dwDuration);
			m_dwOldRealmRemainSec = GetDetailRemainSec(GameTime::REALM);
		}
		break;

		case GameTime::SIEGE:
		{
			m_cNowSiegeTime = cTimeValue;
			m_SiegeDetailTimeInfo = sDetailTimeInfo(cMonth, cDay, cHour, cMinute, dwDuration);
			m_dwOldSiegeRemainSec = GetDetailRemainSec(GameTime::SIEGE);
		}
		break;

		case GameTime::NEWZONE:
		{
			m_cNowEnterTime = cTimeValue;
			m_EnterDetailTimeInfo = sDetailTimeInfo(cMonth, cDay, cHour, cMinute, dwDuration);
			m_dwOldEnterRemainSec = GetDetailRemainSec(GameTime::NEWZONE);
		}
		break;

		case GameTime::EVENT:
		{
			m_cNowEventTime = cTimeValue;
			m_EventDetailTimeInfo = sDetailTimeInfo(cMonth, cDay, cHour, cMinute, dwDuration);
			m_dwOldEventRemainSec = GetDetailRemainSec(GameTime::EVENT);
		}
		break;

		default:	ERRLOG1(g_Log, "게임타입의 시간 값을 설정하는데 실패했습니다. Type : %d", cTimeType);	break;
	}
}

unsigned char	CGameTimeMgr::GetCurrentGameTime(unsigned char cTimeType) const
{
	switch (cTimeType)
	{
		case GameTime::GUILD:		return m_cNowGuildTime;		break;
		case GameTime::REALM:		return m_cNowRealmTime;		break;
		case GameTime::SIEGE:		return m_cNowSiegeTime;		break;
		case GameTime::NEWZONE:		return m_cNowEnterTime;		break;
		case GameTime::EVENT:		return m_cNowEventTime;		break;
	}

	ERRLOG1(g_Log, "현재 게임타입의 시간 상태를 얻는데 실패했습니다. Type : %d", cTimeType);
	return 0;
}

unsigned char	CGameTimeMgr::GetCurrentGuildTime() const
{
	return m_cNowGuildTime;
}

unsigned char	CGameTimeMgr::GetCurrentRealmTime() const
{
	return m_cNowRealmTime;
}

unsigned char	CGameTimeMgr::GetCurrentSiegeTime() const
{
	return m_cNowSiegeTime;
}

unsigned char	CGameTimeMgr::GetCurrentEnterTime() const
{
	return m_cNowEnterTime;
}

unsigned char	CGameTimeMgr::GetCurrentEventTime() const
{
	return m_cNowEventTime;
}

bool	CGameTimeMgr::SendGameTimeInfo(CSendStream& SendStream)
{
	unsigned short wBufferSize = static_cast<unsigned short>(sizeof(PktGameTimeInfo)
		+ sizeof(CastleSiegeInfo) * Castle::CCastleMgr::GetInstance().GetCastleNum());

	char* lpBuffer = SendStream.GetBuffer(wBufferSize);
	if (lpBuffer)
	{
		PktGameTimeInfo* lpPktGTI = reinterpret_cast<PktGameTimeInfo*>(lpBuffer);
		CastleSiegeInfo* lpCastleSiegeInfo = reinterpret_cast<CastleSiegeInfo*>(lpPktGTI + 1);

		unsigned char cNum = 0;
		unsigned short wSize = sizeof(PktGameTimeInfo);
		ZeroMemory(lpPktGTI, sizeof(PktGameTimeInfo));

		Castle::CCastleMgr::GetInstance().GetCastleSiegeInfo(lpCastleSiegeInfo, cNum, wSize);

		lpPktGTI->m_cType = GameTime::TYPE_CHARACTER_LOGIN;
		lpPktGTI->m_wSize = wSize;
		lpPktGTI->m_cCastleNum = cNum;
		lpPktGTI->m_GuildTimeInfo.m_bUpdated = true;
		lpPktGTI->m_RealmTimeInfo.m_bUpdated = true;
		lpPktGTI->m_SiegeTimeInfo.m_bUpdated = true;
		lpPktGTI->m_EventTimeInfo.m_bUpdated = true;
		lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime = m_cNowGuildTime;
		lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime = m_cNowRealmTime;
		lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime = m_cNowSiegeTime;
		lpPktGTI->m_EventTimeInfo.m_cNowEventTime = m_cNowEventTime;

		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::GUILD);

		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::REALM);

		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::SIEGE);

		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::NEWZONE);

		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::EVENT);

		return SendStream.WrapCrypt(wBufferSize, CmdGameTimeInfo, 0, PktBase::NO_SERVER_ERR);
	}

	return false;
}

void	CGameTimeMgr::UpdateGameTimeInfo()
{
	if (CheckNeedUpdate())
	{
		const unsigned short MAX_BUFFER_SIZE = static_cast<unsigned short>(
			sizeof(PktGameTimeInfo)	+ sizeof(CastleSiegeInfo) * Castle::MAX_CASTLE_NUM);

		char szPacket[MAX_BUFFER_SIZE] = {0, };

		PktGameTimeInfo* lpPktGTI = reinterpret_cast<PktGameTimeInfo*>(szPacket);
		CastleSiegeInfo* lpCastleSiegeInfo = reinterpret_cast<CastleSiegeInfo*>(lpPktGTI + 1);

		unsigned char cNum = 0;
		unsigned short wSize = sizeof(PktGameTimeInfo);
		ZeroMemory(lpPktGTI, sizeof(PktGameTimeInfo));

		Castle::CCastleMgr::GetInstance().GetCastleSiegeInfo(lpCastleSiegeInfo, cNum, wSize);

		lpPktGTI->m_cType = GameTime::TYPE_UPDATE_TIME;
		lpPktGTI->m_wSize = wSize;
		lpPktGTI->m_cCastleNum = cNum;
		lpPktGTI->m_GuildTimeInfo.m_bUpdated = true;
		lpPktGTI->m_RealmTimeInfo.m_bUpdated = true;
		lpPktGTI->m_SiegeTimeInfo.m_bUpdated = true;
		lpPktGTI->m_EventTimeInfo.m_bUpdated = true;
		lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime = m_cNowGuildTime;
		lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime = m_cNowRealmTime;
		lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime = m_cNowSiegeTime;
		lpPktGTI->m_EventTimeInfo.m_cNowEventTime = m_cNowEventTime;

		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::GUILD);
		lpPktGTI->m_GuildTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::GUILD);

		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::REALM);
		lpPktGTI->m_RealmTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::REALM);

		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::SIEGE);
		lpPktGTI->m_SiegeTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::SIEGE);

		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::NEWZONE);
		lpPktGTI->m_EnterTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::NEWZONE);

		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMonth	= GetDetailMonth(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cDay		= GetDetailDay(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cHour	= GetDetailHour(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_cMinute	= GetDetailMinute(GameTime::EVENT);
		lpPktGTI->m_EventTimeInfo.m_TimeInfo.m_dwTime	= GetDetailRemainSec(GameTime::EVENT);

		if (PacketWrap::WrapCrypt(szPacket, wSize, CmdGameTimeInfo, 0, 0))
		{
			CCreatureManager::GetInstance().SendAllCharacter(szPacket, wSize, CmdGameTimeInfo);
		}
	}
}

bool	CGameTimeMgr::CheckNeedUpdate()
{
	unsigned long dwGuildRemainSec = GetDetailRemainSec(GameTime::GUILD);
	unsigned long dwRealmRemainSec = GetDetailRemainSec(GameTime::REALM);
	unsigned long dwSiegeRemainSec = GetDetailRemainSec(GameTime::SIEGE);
	unsigned long dwEnterRemainSec = GetDetailRemainSec(GameTime::NEWZONE);
	unsigned long dwEventRemainSec = GetDetailRemainSec(GameTime::EVENT);

	// 길드전 일반 상태와 길드전 상태일때 5분에 한번씩 업데이트
	if ((GameTime::GT_NORMAL == m_cNowGuildTime || GameTime::GT_GUILD_WAR == m_cNowGuildTime) &&
		m_dwOldGuildRemainSec - dwGuildRemainSec >= 300)
	{
		m_dwOldGuildRemainSec = dwGuildRemainSec;
		return true;
	}
	// 길드전 시작 30분전, 종료 30분전은 1분에 한번씩 업데이트
	else if ((GameTime::GT_PREPARE_START_30 == m_cNowGuildTime || GameTime::GT_PREPARE_END_30 == m_cNowGuildTime) &&
			m_dwOldGuildRemainSec - dwGuildRemainSec >= 60)
	{
		m_dwOldGuildRemainSec = dwGuildRemainSec;
		return true;
	}
	// 길드전 시작 5분전에는 10초에 한번씩 업데이트
	else if (GameTime::GT_PREPARE_START_5 == m_cNowGuildTime && m_dwOldGuildRemainSec - dwGuildRemainSec >= 10)
	{
		m_dwOldGuildRemainSec = dwGuildRemainSec;
		return true;
	}

	// 국가전 일반 상태와 국가전 상태일때 5분에 한번씩 업데이트
	if ((GameTime::RT_NORMAL == m_cNowRealmTime || GameTime::RT_REALM_WAR == m_cNowRealmTime) &&
		m_dwOldRealmRemainSec - dwRealmRemainSec >= 300)
	{
		m_dwOldRealmRemainSec = dwRealmRemainSec;
		return true;
	}
	// 국가전 시작 30분, 15분전은 1분에 한번씩 업데이트, 종료30분전 포함
/*	else if ((GameTime::RT_PREPARE_START_30 == m_cNowRealmTime || GameTime::RT_PREPARE_START_15 == m_cNowRealmTime) &&
		m_dwOldRealmRemainSec - dwRealmRemainSec >= 60)
	{
		m_dwOldRealmRemainSec = dwRealmRemainSec;
		return true;
	}
*/	else if ((GameTime::RT_PREPARE_START_30 == m_cNowRealmTime || GameTime::RT_PREPARE_START_15 == m_cNowRealmTime || GameTime::RT_REALM_WAR_END_5 == m_cNowRealmTime) &&
		m_dwOldRealmRemainSec - dwRealmRemainSec >= 60)
	{
		m_dwOldRealmRemainSec = dwRealmRemainSec;
		return true;
	}
	// 국가전 시작 10분은 30초에 한번씩 업데이트
	else if (GameTime::RT_PREPARE_START_10 == m_cNowRealmTime && m_dwOldRealmRemainSec - dwRealmRemainSec >= 30)
	{
		m_dwOldRealmRemainSec = dwRealmRemainSec;
		return true;
	}
	// 국가전 시작 5분은 10초에 한번씩 업데이트
	else if (GameTime::RT_PREPARE_START_5 == m_cNowRealmTime && m_dwOldRealmRemainSec - dwRealmRemainSec >= 10)
	{
		m_dwOldRealmRemainSec = dwRealmRemainSec;
		return true;
	}

	// 공성전 일반 상태와 공성전 상태일때는 5분에 한번씩 업데이트
	if ((GameTime::ST_NORMAL == m_cNowSiegeTime || GameTime::ST_SIEGE_WAR == m_cNowSiegeTime ) &&
		m_dwOldSiegeRemainSec - dwSiegeRemainSec >= 300)
	{
		m_dwOldSiegeRemainSec = dwSiegeRemainSec;
		return true;
	}
	// 공성전 시작 5분전, 종료 5분전에는 10초에 한번씩 업데이트
	else if ((GameTime::ST_PREPARE_START_5 == m_cNowSiegeTime || GameTime::ST_PREPARE_END_5 == m_cNowSiegeTime) &&
			 m_dwOldSiegeRemainSec - dwSiegeRemainSec >= 10)
	{
		m_dwOldSiegeRemainSec = dwSiegeRemainSec;
		return true;
	}

	// 존 진입제한 불가 상태와 가능 상태일때는 5분에 한번씩 업데이트
	if ((GameTime::ET_LIMIT == m_cNowEnterTime || GameTime::ET_ALLOWENTER == m_cNowEnterTime ) &&
		m_dwOldEnterRemainSec - dwEnterRemainSec >= 300)
	{
		m_dwOldEnterRemainSec = dwEnterRemainSec;
		return true;
	}
	// 존 진입제한 시작 5분전, 종료 5분전에는 10초에 한번씩 업데이트
	else if ((GameTime::ET_PREPARE_START_5 == m_cNowEnterTime || GameTime::ET_PREPARE_END_5 == m_cNowEnterTime) &&
		m_dwOldEnterRemainSec - dwEnterRemainSec >= 10)
	{
		m_dwOldEnterRemainSec = dwEnterRemainSec;
		return true;
	}

	// 게임 이벤트 불가 상태와 이벤트 상태일때는 10초에 한번씩 업데이트
	if ( (GameTime::EVENT_NORMAL != m_cNowEventTime) &&	m_dwOldEventRemainSec - dwEventRemainSec >= 10)
	{
		m_dwOldEventRemainSec = dwEventRemainSec;
		return true;
	}
	return false;
}


unsigned char	CGameTimeMgr::GetDetailMonth(unsigned char cType) const
{
	switch (cType)
	{
		case GameTime::GUILD:	return m_GuildDetailTimeInfo.m_cMonth;
		case GameTime::REALM:	return m_RealmDetailTimeInfo.m_cMonth;
		case GameTime::SIEGE:	return m_SiegeDetailTimeInfo.m_cMonth;
		case GameTime::NEWZONE:	return m_EnterDetailTimeInfo.m_cMonth;
		case GameTime::EVENT:	return m_EventDetailTimeInfo.m_cMonth;
	}

	return 0;
}

unsigned char	CGameTimeMgr::GetDetailDay(unsigned char cType) const
{
	switch (cType)
	{
		case GameTime::GUILD:	return m_GuildDetailTimeInfo.m_cDay;
		case GameTime::REALM:	return m_RealmDetailTimeInfo.m_cDay;
		case GameTime::SIEGE:	return m_SiegeDetailTimeInfo.m_cDay;
		case GameTime::NEWZONE:	return m_EnterDetailTimeInfo.m_cDay;
		case GameTime::EVENT:	return m_EventDetailTimeInfo.m_cDay;
	}

	return 0;
}

unsigned char	CGameTimeMgr::GetDetailHour(unsigned char cType) const
{
	switch (cType)
	{
		case GameTime::GUILD:	return m_GuildDetailTimeInfo.m_cHour;
		case GameTime::REALM:	return m_RealmDetailTimeInfo.m_cHour;
		case GameTime::SIEGE:	return m_SiegeDetailTimeInfo.m_cHour;
		case GameTime::NEWZONE:	return m_EnterDetailTimeInfo.m_cHour;
		case GameTime::EVENT:	return m_EventDetailTimeInfo.m_cHour;
	}

	return 0;
}

unsigned char	CGameTimeMgr::GetDetailMinute(unsigned char cType) const
{
	switch (cType)
	{
		case GameTime::GUILD:	return m_GuildDetailTimeInfo.m_cMinute;
		case GameTime::REALM:	return m_RealmDetailTimeInfo.m_cMinute;
		case GameTime::SIEGE:	return m_SiegeDetailTimeInfo.m_cMinute;
		case GameTime::NEWZONE:	return m_EnterDetailTimeInfo.m_cMinute;
		case GameTime::EVENT:	return m_EventDetailTimeInfo.m_cMinute;
	}

	return 0;
}

unsigned long	CGameTimeMgr::GetDetailRemainSec(unsigned char cType) const
{
	// 끝날때까지의 남은 시간을 구하는 것인지 체크
	bool bRequestEndTime = false;
	CTime nowTime = CTime::GetCurrentTime();
	CTime tempTime;
	unsigned long dwDuration = 0;

	switch (cType)
	{
		case GameTime::GUILD:
		{
			if (0 != m_GuildDetailTimeInfo.m_cDay)
			{
				tempTime = CTime(nowTime.GetYear(), m_GuildDetailTimeInfo.m_cMonth, m_GuildDetailTimeInfo.m_cDay,
								m_GuildDetailTimeInfo.m_cHour, m_GuildDetailTimeInfo.m_cMinute, 0);
				dwDuration = m_GuildDetailTimeInfo.m_dwDuration;

				if (IsGuildWarTime())
					bRequestEndTime = true;
			}
			else
			{
				return 0;
			}
		}
		break;

		case GameTime::REALM:
		{
			if (0 != m_RealmDetailTimeInfo.m_cDay)
			{
				tempTime = CTime(nowTime.GetYear(), m_RealmDetailTimeInfo.m_cMonth, m_RealmDetailTimeInfo.m_cDay,
								m_RealmDetailTimeInfo.m_cHour, m_RealmDetailTimeInfo.m_cMinute, 0);
				dwDuration = m_RealmDetailTimeInfo.m_dwDuration;

				if (IsRealmWarTime())
					bRequestEndTime = true;
			}
			else
			{
				return 0;
			}
		}
		break;

		case GameTime::SIEGE:
		{
			if (0 != m_SiegeDetailTimeInfo.m_cDay)
			{
				tempTime = CTime(nowTime.GetYear(), m_SiegeDetailTimeInfo.m_cMonth, m_SiegeDetailTimeInfo.m_cDay,
								m_SiegeDetailTimeInfo.m_cHour, m_SiegeDetailTimeInfo.m_cMinute, 0);
				dwDuration = m_SiegeDetailTimeInfo.m_dwDuration;

				if (IsSiegeWarTime())
					bRequestEndTime = true;
			}
			else
			{
				return 0;
			}
		}
		break;

		case GameTime::NEWZONE:
		{
			if (0 != m_EnterDetailTimeInfo.m_cDay)
			{
				tempTime = CTime(nowTime.GetYear(), m_EnterDetailTimeInfo.m_cMonth, m_EnterDetailTimeInfo.m_cDay,
					m_EnterDetailTimeInfo.m_cHour, m_EnterDetailTimeInfo.m_cMinute, 0);
				dwDuration = m_EnterDetailTimeInfo.m_dwDuration;

				if (IsEnterTime())
					bRequestEndTime = true;
			}
			else
			{
				return 0;
			}
		}
		break;

		case GameTime::EVENT:
		{
			if (0 != m_EventDetailTimeInfo.m_cDay)
			{
				tempTime = CTime(nowTime.GetYear(), m_EventDetailTimeInfo.m_cMonth, m_EventDetailTimeInfo.m_cDay,
					m_EventDetailTimeInfo.m_cHour, m_EventDetailTimeInfo.m_cMinute, 0);
				dwDuration = m_EventDetailTimeInfo.m_dwDuration;

				if (IsEventTime())
					bRequestEndTime = true;
			}
			else
			{
				return 0;
			}
		}
		break;
	}

	CTimeSpan tempSpan(tempTime - nowTime);
	if (bRequestEndTime)
	{
		tempSpan += dwDuration * 60;
	}

	if (tempSpan.GetTimeSpan() > 0)
	{
		return static_cast<unsigned long>(tempSpan.GetTimeSpan());
	}

	return 0;
}

bool	CGameTimeMgr::IsGuildWarTime() const
{
	switch (m_cNowGuildTime)
	{
		case GameTime::GT_GUILD_WAR:
		case GameTime::GT_PREPARE_END_30:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeMgr::IsRealmWarTime() const
{
	if (GameTime::RT_REALM_WAR == m_cNowRealmTime || GameTime::RT_REALM_WAR_END_5 == m_cNowRealmTime)
		return true;

	return false;
}

bool	CGameTimeMgr::IsSiegeWarTime() const
{
	if (GameTime::ST_SIEGE_WAR == m_cNowSiegeTime || GameTime::ST_PREPARE_END_5 == m_cNowSiegeTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsEnterTime() const
{
	if (GameTime::ET_ALLOWENTER == m_cNowEnterTime || GameTime::ET_PREPARE_END_5 == m_cNowEnterTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsEventTime() const
{
	if (GameTime::EVENT_NORMAL != m_cNowEventTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsGuildWarTime(unsigned char cTime) const
{
	switch (cTime)
	{
		case GameTime::GT_GUILD_WAR:
		case GameTime::GT_PREPARE_END_30:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeMgr::IsRealmWarTime(unsigned char cTime) const
{
	if (GameTime::RT_REALM_WAR == cTime || GameTime::RT_REALM_WAR_END_5 == cTime)
		return true;

	return false;
}

bool	CGameTimeMgr::IsSiegeWarTime(unsigned char cTime) const
{
	if (GameTime::ST_SIEGE_WAR == cTime || GameTime::ST_PREPARE_END_5 == cTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsEnterTime(unsigned char cTime) const
{
	if (GameTime::ET_ALLOWENTER== cTime || GameTime::ET_PREPARE_END_5 == cTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsEventTime(unsigned char cTime) const
{
	if (GameTime::EVENT_NORMAL != cTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeMgr::IsRealmWarReadyTime() const
{
	switch (m_cNowRealmTime)
	{
		case GameTime::RT_PREPARE_START_10:
		case GameTime::RT_PREPARE_START_5:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeMgr::IsRealmWarReadyTime(unsigned char cTime) const
{
	switch (cTime)
	{
		case GameTime::RT_PREPARE_START_10:
		case GameTime::RT_PREPARE_START_5:
			return true;

		default:
			return false;
	}

	return false;
}
