#ifndef _GAME_TIME_MANAGER_H_
#define _GAME_TIME_MANAGER_H_

#pragma once
#include <Network/Stream/SendStream.h>

class CGameTimeMgr
{
protected:
	CGameTimeMgr();
	~CGameTimeMgr();

	// 다음번 길드전, 렐름전, 공성전의 날짜 및 시간과 전쟁시간 정보 구조체
	struct sDetailTimeInfo
	{
		unsigned char m_cMonth;
		unsigned char m_cDay;
		unsigned char m_cHour;
		unsigned char m_cMinute;
		unsigned long m_dwDuration;	// (분)

		sDetailTimeInfo()
			: m_cMonth(0), m_cDay(0), m_cHour(0), m_cMinute(0), m_dwDuration(0)
		{
		}

		sDetailTimeInfo(unsigned char cMonth, unsigned char cDay, unsigned char cHour, unsigned char cMinute, unsigned long dwDuration)
			: m_cMonth(cMonth), m_cDay(cDay), m_cHour(cHour), m_cMinute(cMinute), m_dwDuration(dwDuration)
		{
		}
	};

	bool	CheckNeedUpdate();

public:
	
	bool	SendGameTimeInfo(CSendStream& SendStream);
	void	UpdateGameTimeInfo();		// 모든 클라이언트에게 게임 시간 정보 업데이트(싱크를 맞추기 위해서)

	void	SetTimeValue(unsigned char cTimeType, unsigned char cTimeValue, unsigned char cMonth, unsigned char cDay,
						 unsigned char cHour, unsigned char cMinute, unsigned long dwDuration);

	unsigned char	GetCurrentGameTime(unsigned char cTimeType) const;
	unsigned char	GetCurrentGuildTime() const;
	unsigned char	GetCurrentRealmTime() const;
	unsigned char	GetCurrentSiegeTime() const;
	unsigned char	GetCurrentEnterTime() const;

	// edith 2009.07.21 게임타임 작업물
	unsigned char	GetCurrentEventTime() const;

	unsigned char	GetDetailMonth(unsigned char cType) const;
	unsigned char	GetDetailDay(unsigned char cType) const;
	unsigned char	GetDetailHour(unsigned char cType) const;
	unsigned char	GetDetailMinute(unsigned char cType) const;
	unsigned long	GetDetailRemainSec(unsigned char cType) const;

	bool	IsGuildWarTime() const;
	bool	IsRealmWarTime() const;
	bool	IsSiegeWarTime() const;
	bool	IsEnterTime() const;

	// edith 2009.07.21 게임타임 작업물
	bool	IsEventTime() const;

	bool	IsGuildWarTime(unsigned char cTime) const;
	bool	IsRealmWarTime(unsigned char cTime) const;
	bool	IsSiegeWarTime(unsigned char cTime) const;
	bool	IsEnterTime(unsigned char cTime) const;

	// edith 2009.07.21 게임타임 작업물
	bool	IsEventTime(unsigned char cTime) const;

	bool	IsRealmWarReadyTime() const;
	bool	IsRealmWarReadyTime(unsigned char cTime) const;
	
	static CGameTimeMgr& GetInstance();

private:

	unsigned char	m_cNowGuildTime;
	unsigned char	m_cNowRealmTime;
	unsigned char	m_cNowSiegeTime;
	unsigned char	m_cNowEnterTime;	// 테섭용 신규존 입장 제한 플래그

	// edith 2009.07.21 게임타임 작업물
	unsigned char	m_cNowEventTime;	

	sDetailTimeInfo	m_GuildDetailTimeInfo;
	sDetailTimeInfo	m_RealmDetailTimeInfo;
	sDetailTimeInfo	m_SiegeDetailTimeInfo;
	sDetailTimeInfo	m_EnterDetailTimeInfo;
	// edith 2009.07.21 게임타임 작업물
	sDetailTimeInfo	m_EventDetailTimeInfo;

	unsigned long	m_dwOldGuildRemainSec;
	unsigned long	m_dwOldRealmRemainSec;
	unsigned long	m_dwOldSiegeRemainSec;
	unsigned long	m_dwOldEnterRemainSec;
	// edith 2009.07.21 게임타임 작업물
	unsigned long	m_dwOldEventRemainSec;
};

#endif	// _GAME_TIME_MANAGER_H_