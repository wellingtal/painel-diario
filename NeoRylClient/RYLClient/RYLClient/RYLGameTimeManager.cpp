#include <winsock2.h>

#include "RYLGameTimeManager.h"
#include "RYLStringTable.h"
#include "GMMemory.h"

CRYLGameTimeManager::CRYLGameTimeManager() 
:	m_cNowGuildTime(0), m_cNowRealmTime(0), m_cNowSiegeTime(0), m_cNowEventTime(0)
{
}

CRYLGameTimeManager* CRYLGameTimeManager::Instance()
{
	static CRYLGameTimeManager ms_this;
	return &ms_this;
}

CRYLGameTimeManager::~CRYLGameTimeManager() 
{
}

void	CRYLGameTimeManager::SetTimeValue( unsigned char cType, const GuildTimeInfo& guildTimeInfo,
										   const RealmTimeInfo& realmTimeInfo, const SiegeTimeInfo& siegeTimeInfo, const EnterTimeInfo& enterTimeInfo ,
										   const EventTimeInfo& eventTimeInfo )
{
	if ( GameTime::TYPE_CHARACTER_LOGIN == cType || GameTime::TYPE_UPDATE_TIME == cType || GameTime::TYPE_CHANGE_GAME_TIME == cType )
	{
		if ( guildTimeInfo.m_bUpdated )
		{
			m_cNowGuildTime = guildTimeInfo.m_cNowGuildTime ;

			m_GuildDetailTime = sDetailTimeInfo( guildTimeInfo.m_TimeInfo.m_cMonth, guildTimeInfo.m_TimeInfo.m_cDay,
												 guildTimeInfo.m_TimeInfo.m_cHour, guildTimeInfo.m_TimeInfo.m_cMinute,
												 guildTimeInfo.m_TimeInfo.m_dwTime * 1000) ;
		}

		if ( realmTimeInfo.m_bUpdated )
		{
			m_cNowRealmTime = realmTimeInfo.m_cNowRealmTime ;

			m_RealmDetailTime = sDetailTimeInfo( realmTimeInfo.m_TimeInfo.m_cMonth, realmTimeInfo.m_TimeInfo.m_cDay,
												 realmTimeInfo.m_TimeInfo.m_cHour, realmTimeInfo.m_TimeInfo.m_cMinute,
												 realmTimeInfo.m_TimeInfo.m_dwTime * 1000) ;
		}

		if ( siegeTimeInfo.m_bUpdated )
		{
			m_cNowSiegeTime = siegeTimeInfo.m_cNowSiegeTime ;

			m_SiegeDetailTime = sDetailTimeInfo( siegeTimeInfo.m_TimeInfo.m_cMonth, siegeTimeInfo.m_TimeInfo.m_cDay,
												 siegeTimeInfo.m_TimeInfo.m_cHour, siegeTimeInfo.m_TimeInfo.m_cMinute,
												 siegeTimeInfo.m_TimeInfo.m_dwTime ) ;
		}

		if ( enterTimeInfo.m_bUpdated )
		{
			m_cNowEnterTime = enterTimeInfo.m_cNowEnterTime ;

			m_EnterDetailTime = sDetailTimeInfo( enterTimeInfo.m_TimeInfo.m_cMonth, enterTimeInfo.m_TimeInfo.m_cDay,
				enterTimeInfo.m_TimeInfo.m_cHour, enterTimeInfo.m_TimeInfo.m_cMinute,
				enterTimeInfo.m_TimeInfo.m_dwTime ) ;
		}

		if ( eventTimeInfo.m_bUpdated )
		{
			m_cNowEventTime = eventTimeInfo.m_cNowEventTime ;

			m_EventDetailTime = sDetailTimeInfo( eventTimeInfo.m_TimeInfo.m_cMonth, eventTimeInfo.m_TimeInfo.m_cDay,
				eventTimeInfo.m_TimeInfo.m_cHour, eventTimeInfo.m_TimeInfo.m_cMinute,
				eventTimeInfo.m_TimeInfo.m_dwTime ) ;
		}
		
	}
}

VOID	CRYLGameTimeManager::UpdateGuildTime( FLOAT fUpdate )
{		
	if (m_GuildDetailTime.m_dwRemainSec - fUpdate > 0 )
	{
		m_GuildDetailTime.m_dwRemainSec -= fUpdate ;
		return ;
	}	

	m_GuildDetailTime.m_dwRemainSec = 0 ;
}

VOID	CRYLGameTimeManager::UpdateRealmTime( FLOAT fUpdate )
{
	if ( m_RealmDetailTime.m_dwRemainSec - fUpdate > 0 )
	{
		m_RealmDetailTime.m_dwRemainSec -= fUpdate ;
		return ;
	}	

	m_RealmDetailTime.m_dwRemainSec = 0 ;
}

bool	CRYLGameTimeManager::IsGuildWarTime() const
{
	switch ( m_cNowGuildTime )
	{
		case GameTime::GT_GUILD_WAR :
		case GameTime::GT_PREPARE_END_30 :
			return true ;

		default :
			return false ;
	}

	return false ;
}

bool	CRYLGameTimeManager::IsRealmWarTime() const
{
	if ( GameTime::RT_REALM_WAR == m_cNowRealmTime || GameTime::RT_REALM_WAR_END_5 == m_cNowRealmTime)
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsSiegeWarTime() const
{
	if ( GameTime::ST_SIEGE_WAR == m_cNowSiegeTime || GameTime::ST_PREPARE_END_5 == m_cNowSiegeTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsEnterTime() const
{
	if ( GameTime::ET_ALLOWENTER == m_cNowSiegeTime || GameTime::ET_PREPARE_END_5 == m_cNowSiegeTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsEventTime() const
{
	if ( GameTime::EVENT_NORMAL != m_cNowEventTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsGuildWarTime( unsigned long dwTime ) const
{
	switch ( dwTime )
	{
		case GameTime::GT_GUILD_WAR :
		case GameTime::GT_PREPARE_END_30 :
			return true ;

		default :
			return false ;
	}

	return false ;
}

bool	CRYLGameTimeManager::IsRealmWarTime( unsigned long dwTime ) const
{
	if ( GameTime::RT_REALM_WAR == dwTime || GameTime::RT_REALM_WAR_END_5 == dwTime)
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsSiegeWarTime( unsigned long dwTime ) const
{
	if ( GameTime::ST_SIEGE_WAR == dwTime || GameTime::ST_PREPARE_END_5 == dwTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsEnterTime( unsigned long dwTime ) const
{
	if ( GameTime::ET_ALLOWENTER == dwTime || GameTime::ET_PREPARE_END_5 == dwTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsEventTime( unsigned long dwTime ) const
{
	if ( GameTime::EVENT_NORMAL != dwTime )
		return true ;

	return false ;
}

bool	CRYLGameTimeManager::IsRealmWarReadyTime() const
{
	switch ( m_cNowRealmTime )
	{
		case GameTime::RT_PREPARE_START_10 :
		case GameTime::RT_PREPARE_START_5 :
			return true ;

		default :
			return false ;
	}

	return false ;
}

bool	CRYLGameTimeManager::IsRealmWarReadyTime( unsigned long dwTime ) const
{
	switch ( dwTime )
	{
		case GameTime::RT_PREPARE_START_10 :
		case GameTime::RT_PREPARE_START_5 :
			return true ;

		default :
			return false ;
	}

	return false ;
}
