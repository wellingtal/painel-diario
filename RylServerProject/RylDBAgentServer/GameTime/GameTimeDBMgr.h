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

	// ���� �ð��� ����ü
	struct sGameTime
	{
		unsigned long	m_dwID;

		TimeType		m_eTimeType;		// ���� �ð� Ÿ�� (�����, ������, ������)
        Cycle			m_eTimeCycle;		// ���� �ð� �ֱ� (����, ����, �ſ�, Ư����)
		Week			m_eWeek;			// Cycle �� Weekly �� ����� ����

		unsigned short	m_wYear;			// �⵵
		unsigned char	m_cMonth;			// ��
		unsigned char	m_cDay;				// ��
		unsigned char	m_cHour;			// �ð�
		unsigned char	m_cMinute;			// ��
		unsigned long	m_dwDuration;		// �÷��� Ÿ�� (��)

		// edith 2009.07.21 ����Ÿ�� �۾���
		unsigned long	m_dwValue1;			// ��Ÿ�۾���
		unsigned long	m_dwValue2;			// ��Ÿ�۾���

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



	// �Ľ� ����ü
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
	void	SendGameTimeInfo(unsigned long dwCastleID);		// Ư�� ���� ���� ���ɿ��� �����

	unsigned char	GetCurrentGameTime(unsigned char cTimeType) const;
	unsigned char	GetCurrentGuildTime() const;
	unsigned char	GetCurrentRealmTime() const;
	unsigned char	GetCurrentSiegeTime() const;
	unsigned char	GetCurrentEnterTime() const;
	unsigned char	GetCurrentMiningTime() const;

	// edith 2009.07.21 ����Ÿ�� �۾���
	unsigned char	GetCurrentEventTime() const;


	bool	IsGuildWarTime() const;
	bool	IsRealmWarTime() const;
	bool	IsSiegeWarTime() const;
	bool	IsEnterTime() const;
	bool	IsMiningTime() const;

	// edith 2009.07.21 ����Ÿ�� �۾���
	bool	IsEventTime() const;

	bool	IsGuildWarTime(unsigned char cTime) const;
	bool	IsRealmWarTime(unsigned char cTime) const;
	bool	IsSiegeWarTime(unsigned char cTime) const;
	bool	IsEnterTime(unsigned char cTime) const;
	bool	IsMiningTime(unsigned char cTime) const;

	// edith 2009.07.21 ����Ÿ�� �۾���
	bool	IsEventTime(unsigned char cTime) const;

	bool	IsRealmWarReadyTime() const;
	bool	IsRealmWarReadyTime(unsigned char cTime) const;

	const GameTimeTable&	GetGameTimeTable() const	{ return m_GameTimeTable;	}

	static CGameTimeDBMgr& GetInstance();

	void CheckRealmCount(CTime& startTime, unsigned long* dwRealmCount_Out);		// �������� Ƚ���� üũ.

	unsigned long	GetRealmDuration(void) { return m_dwRealmTime;	}

protected:

	CGameTimeDBMgr();
	~CGameTimeDBMgr();

private:

	bool	SetGameTimeParseData(ParseDataArray& parseDataArray);

	bool	IntegrityCheck();				// ���Ἲ �˻�

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
	unsigned char		m_cNowEnterTime;	// enum EnterTime. �׼��� �ű��� �������� �÷���
	unsigned char		m_cNowMiningTime;	// enum MiningTime

	// edith 2009.07.21 ����Ÿ�� �۾���
	unsigned char		m_cNowEventTime;	// enum EventTime

	GameTimeTable		m_GameTimeTable;

	CDBComponent*		m_lpDBComponent;

	unsigned long		m_dwRealmTime;		// ���翡 �������� �÷��� Ÿ���� ������ �ִ´�.
};

namespace RealmPoint
{
	void AddRealmPoint(DBAgent::DataStorage::CSessionData* sessionData, unsigned char cRealmType);
	void InitRealmTime(DBAgent::DataStorage::CSessionData* sessionData, TIME* pTime);
};

#endif	// _RYL_DBAGENT_GAME_TIME_DB_MANAGER_H_
