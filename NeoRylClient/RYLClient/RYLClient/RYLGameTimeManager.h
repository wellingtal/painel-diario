//=================================================================
// Name : RYLGameTimeManager.h
// Data : 2005. 07. 07
// Desc : Game Time Manager
//=================================================================
#ifndef __RYL_GAME_TIME_MANAGER_H__
#define __RYL_GAME_TIME_MANAGER_H__

#pragma once

#include <GameTime/GameTimeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

class CRYLGameTimeManager
{
private:
	CRYLGameTimeManager() ;
	virtual ~CRYLGameTimeManager() ;

public:

	struct sDetailTimeInfo
	{
		unsigned char	m_cMonth ;
		unsigned char	m_cDay ;
		unsigned char	m_cHour ;
		unsigned char	m_cMinute ;
		unsigned long	m_dwRemainSec ;

		sDetailTimeInfo()
			: m_cMonth(0), m_cDay(0), m_cHour(0), m_cMinute(0), m_dwRemainSec(0)
		{
		}

		sDetailTimeInfo( unsigned char cMonth, unsigned char cDay, unsigned char cHour, unsigned char cMinute, unsigned long dwRemainSec )
			: m_cMonth(cMonth), m_cDay(cDay), m_cHour(cHour), m_cMinute(cMinute), m_dwRemainSec(dwRemainSec)
		{
		}
	} ;

	void	SetTimeValue( unsigned char cType, const GuildTimeInfo& guildTimeInfo,
						  const RealmTimeInfo& realmTimeInfo, const SiegeTimeInfo& siegeTimeInfo, const EnterTimeInfo& enterTimeInfo,
						  const EventTimeInfo& eventTimeInfo) ;
	
	VOID	UpdateGuildTime( FLOAT fUpdate ) ;
	VOID	UpdateRealmTime( FLOAT fUpdate ) ;

	sDetailTimeInfo*	GetGuildDetailTimeInfo()	{ return &m_GuildDetailTime ;				}
	sDetailTimeInfo*	GetRealmDetailTimeInfo()	{ return &m_RealmDetailTime ;				}

	unsigned long		GetGuildRemainSec()			{ return m_GuildDetailTime.m_dwRemainSec ;	}
	unsigned long		GetRealmRemainSec()			{ return m_RealmDetailTime.m_dwRemainSec ;	}
	
	unsigned long		GetNowGuildTime() const		{ return m_cNowGuildTime ;					}
	unsigned long		GetNowRealmTime() const		{ return m_cNowRealmTime ;					}
    unsigned long		GetNowSiegeTime() const		{ return m_cNowSiegeTime ;					}
	unsigned long		GetNowEnterTime() const		{ return m_cNowEnterTime ;					}

	unsigned long		GetNowEventTime() const		{ return m_cNowEnterTime ;					}

	bool	IsGuildWarTime() const ;
	bool	IsRealmWarTime() const ;
	bool	IsSiegeWarTime() const ;
	bool	IsEnterTime() const ;
	bool	IsEventTime() const ;

	bool	IsGuildWarTime( unsigned long dwTime ) const ;
	bool	IsRealmWarTime( unsigned long dwTime ) const ;
	bool	IsSiegeWarTime( unsigned long dwTime ) const ;
	bool	IsEnterTime( unsigned long dwTime ) const ;
	bool	IsEventTime( unsigned long dwTime ) const ;
	
	bool	IsRealmWarReadyTime() const ;
	bool	IsRealmWarReadyTime( unsigned long dwTime ) const ;

	// Signton
	static CRYLGameTimeManager* Instance();

private:

	unsigned char	m_cNowGuildTime ;
	unsigned char	m_cNowRealmTime ;
	unsigned char	m_cNowSiegeTime ;
	unsigned char	m_cNowEnterTime ;
	unsigned char	m_cNowEventTime ;

	sDetailTimeInfo	m_GuildDetailTime ;
	sDetailTimeInfo	m_RealmDetailTime ;
	sDetailTimeInfo	m_SiegeDetailTime ;
	sDetailTimeInfo	m_EnterDetailTime ;
	sDetailTimeInfo	m_EventDetailTime ;
} ;

#endif	//__RYL_GAME_TIME_MANAGER_H__
