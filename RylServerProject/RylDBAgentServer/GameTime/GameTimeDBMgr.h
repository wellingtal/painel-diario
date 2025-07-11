#ifndef _RYL_DBAGENT_GAME_TIME_DB_MANAGER_H_
#define _RYL_DBAGENT_GAME_TIME_DB_MANAGER_H_

#pragma once

#include <map>
#include <vector>
#include <GameTime/GameTimeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <DataStorage/SessionData.h>

using namespace GameTime;
using namespace DBAgent;

class CDBComponent;

class CDBComponent;

class CSessionData;

class CGameTimeDBMgr
{
public:

	// 게임 시간별 구조체
	struct sGameTime
	{
		unsigned long	m_dwID;

		TimeType		m_eTimeType;		// 게임 시간 타입 (길드전, 렐름전, 공성전)
        Cycle			m_eTimeCycle;		// 게임 시간 주기 (매주, 매일, 매월, 특정일)
		Week			m_eWeek;			// Cycle 이 Weekly 일 경우의 요일

		unsigned short	m_wYear;			// 년도
		unsigned char	m_cMonth;			// 월
		unsigned char	m_cDay;				// 일
		unsigned char	m_cHour;			// 시간
		unsigned char	m_cMinute;			// 분
		unsigned long	m_dwDuration;		// 플레이 타임 (분)

		// edith 2009.07.21 게임타임 작업물
		unsigned long	m_dwValue1;			// 기타작업물
		unsigned long	m_dwValue2;			// 기타작업물

		sGameTime::sGameTime()
		{
			m_dwID			= 0;
			m_eTimeType		= TYPE_NONE;
			m_eTimeCycle	= CYCLE_NONE;
			m_eWeek			= WEEK_NONE;
			m_wYear			= 0;
			m_cMonth		= 0;
			m_cDay			= 0;
			m_cHour			= 0;
			m_cMinute		= 0;
			m_dwDuration	= 0;

			m_dwValue1 = 0;
			m_dwValue2 = 0;
		}
	};

	typedef std::map<unsigned long, sGameTime>	GameTimeTable;



	// 파싱 구조체
	struct ParseData
	{
		typedef bool(*ParseFunc) (sGameTime& gametime, const char* szValue);

		const char* m_szColumnName;
		ParseFunc   m_fnParseFunc;

		ParseData(const char* szColumnName, ParseFunc fnParseFunc)
			:   m_szColumnName(szColumnName), m_fnParseFunc(fnParseFunc) { }
	};

	typedef std::vector<ParseData>  ParseDataArray;

public:

	bool	LoadGameTimeScript(const char* szFileName);
	bool	Initialize(CDBComponent& DBComponent);
	void	InitGameTimeValue(unsigned char cGuildTime, unsigned char cRealmTime,
							  unsigned char cSiegeTime, unsigned char cMiningTime, 
							  unsigned char cEnterTime, unsigned char cEventTime);

	void	Process();
	void	CheckGameTime();
	void	SendGameTimeInfo(unsigned char cType, bool bUpdateGuildTime, bool bUpdateRealmTime, bool bUpdateSiegeTime, bool bUpdateEnterTime, bool bUpdateEventTime);
	void	SendGameTimeInfo(unsigned long dwCastleID);		// 특정 성의 공성 가능여부 변경시

	unsigned char	GetCurrentGameTime(unsigned char cTimeType) const;
	unsigned char	GetCurrentGuildTime() const;
	unsigned char	GetCurrentRealmTime() const;
	unsigned char	GetCurrentSiegeTime() const;
	unsigned char	GetCurrentEnterTime() const;
	unsigned char	GetCurrentMiningTime() const;

	// edith 2009.07.21 게임타임 작업물
	unsigned char	GetCurrentEventTime() const;


	bool	IsGuildWarTime() const;
	bool	IsRealmWarTime() const;
	bool	IsSiegeWarTime() const;
	bool	IsEnterTime() const;
	bool	IsMiningTime() const;

	// edith 2009.07.21 게임타임 작업물
	bool	IsEventTime() const;

	bool	IsGuildWarTime(unsigned char cTime) const;
	bool	IsRealmWarTime(unsigned char cTime) const;
	bool	IsSiegeWarTime(unsigned char cTime) const;
	bool	IsEnterTime(unsigned char cTime) const;
	bool	IsMiningTime(unsigned char cTime) const;

	// edith 2009.07.21 게임타임 작업물
	bool	IsEventTime(unsigned char cTime) const;

	bool	IsRealmWarReadyTime() const;
	bool	IsRealmWarReadyTime(unsigned char cTime) const;

	const GameTimeTable&	GetGameTimeTable() const	{ return m_GameTimeTable;	}

	static CGameTimeDBMgr& GetInstance();

	void CheckRealmCount(CTime& startTime, unsigned long* dwRealmCount_Out);		// 국가전쟁 횟수는 체크.

	unsigned long	GetRealmDuration(void) { return m_dwRealmTime;	}

protected:

	CGameTimeDBMgr();
	~CGameTimeDBMgr();

private:

	bool	SetGameTimeParseData(ParseDataArray& parseDataArray);

	bool	IntegrityCheck();				// 무결성 검사

	CTime	GetTimeByCycle(sGameTime* lpGameTime, CTime nowTime = 0);
	CTime	GetWeeklyTime(sGameTime* lpGameTime, CTime nowTime = 0);
	CTime	GetDailyTime(sGameTime* lpGameTime, CTime nowTime = 0);
	CTime	GetMonthlyTime(sGameTime* lpGameTime, CTime nowTime = 0);
	CTime	GetDateTime(sGameTime* lpGameTime, CTime nowTime = 0);

	void	GetTimeInfo(unsigned char cType, TimeInfo& timeInfo);

	void	GetNextMonth(CTime* pTime);

private:

	unsigned char		m_cNowGuildTime;	// enum GuildTime
	unsigned char		m_cNowRealmTime;	// enum RealmTime
	unsigned char		m_cNowSiegeTime;	// enum SiegeTime
	unsigned char		m_cNowEnterTime;	// enum EnterTime. 테섭용 신규존 진입제한 플래그
	unsigned char		m_cNowMiningTime;	// enum MiningTime

	// edith 2009.07.21 게임타임 작업물
	unsigned char		m_cNowEventTime;	// enum EventTime

	GameTimeTable		m_GameTimeTable;

	CDBComponent*		m_lpDBComponent;

	unsigned long		m_dwRealmTime;		// 현재에 국가전쟁 플레이 타임을 가지고 있는다.
};

namespace RealmPoint
{
	void AddRealmPoint(DBAgent::DataStorage::CSessionData* sessionData, unsigned char cRealmType);
	void InitRealmTime(DBAgent::DataStorage::CSessionData* sessionData, TIME* pTime);
};

#endif	// _RYL_DBAGENT_GAME_TIME_DB_MANAGER_H_
