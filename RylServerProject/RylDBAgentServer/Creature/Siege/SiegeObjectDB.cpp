#include "stdafx.h"
#include "SiegeObjectDB.h"
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


CSiegeObjectDB::CSiegeObjectDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject)
:   m_dwCID(Creature::SIEGE_OBJECT_BIT | CastleObject.m_dwCID), 
	m_dwCampOrCastleID(CastleObject.m_dwCastleID), 
    m_dwOwnerID(CastleObject.m_dwOwnerID), 
    m_dwGID(0),
	m_cNation(0),
    m_dwHP(CastleObject.m_dwHP), 
    m_wObjectType(CastleObject.m_wObjectType), 
    m_cState(CastleObject.m_cState),
    m_cSubState(CastleObject.m_cSubState),
    m_cUpgradeStep(CastleObject.m_cUpgradeStep), 
    m_cUpgradeType(CastleObject.m_cUpgradeType),
    m_cZone(0), 
    m_cChannel(0), 
    m_TimeValue(CastleObject.m_RemainUpTime), 
	m_LastUseTime(CastleObject.m_LastUseTime),
    m_Pos(CastleObject.m_Pos), 
	m_cMaterial(0),
	m_cSiegeCount(0),
    m_dwRepairHP(0),
    m_fDefaultDir(CastleObject.m_fDefaultDir), 
    m_cDevelopTime(Siege::DEFAULT_TIME_VALUE), 
    m_cUpgradeTime(Siege::DEFAULT_TIME_VALUE),
    m_cRepairTime(Siege::DEFAULT_REPAIR_TIME),
	m_cChangeTime(Siege::DEFAULT_TIME_VALUE),
    m_DBComponent(DBComponent)
{
}


CSiegeObjectDB::CSiegeObjectDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo)
:   m_dwCID(0), 
	m_dwCampOrCastleID(Castle::CAMP_BIT | CampInfo.m_dwCampID),
    m_dwOwnerID(0), 
    m_dwGID(CampInfo.m_dwGID), 
	m_cNation(0),
    m_dwHP(CampInfo.m_dwHP), 
    m_wObjectType(CampInfo.m_wObjectType),
    m_cState(CampInfo.m_cState), 
    m_cSubState(CampInfo.m_cSubState), 
    m_cUpgradeStep(CampInfo.m_cUpgradeStep), 
    m_cUpgradeType(0), 
    m_cZone(CampInfo.m_cZone),
    m_cChannel(CampInfo.m_cChannel), 
    m_TimeValue(CampInfo.m_TimeValue), 
    m_LastUseTime(CampInfo.m_LastUseTime), 
    m_Pos(CampInfo.m_Pos),
	m_cMaterial(CampInfo.m_cMaterial),
	m_cSiegeCount(CampInfo.m_cSiegeCount),
    m_dwRepairHP(0), 
    m_fDefaultDir(0), 
    m_cDevelopTime(Siege::DEFAULT_TIME_VALUE), 
    m_cUpgradeTime(Siege::DEFAULT_TIME_VALUE),
    m_cRepairTime(Siege::DEFAULT_REPAIR_TIME),
	m_cChangeTime(Siege::DEFAULT_TIME_VALUE),
    m_DBComponent(DBComponent)
{
}

CSiegeObjectDB::CSiegeObjectDB(CDBComponent& DBComponent, unsigned long dwCampID, 
                               unsigned long dwGID, unsigned char cZone, 
                               unsigned char cChannel, Position Pos)
:   m_dwCID(0),
	m_dwCampOrCastleID(dwCampID),
    m_dwOwnerID(0), 
    m_dwGID(dwGID), 
	m_cNation(0),
    m_dwHP(0),
    m_wObjectType(Siege::CAMP), 
    m_cSubState(0),
    m_cState(DEVELOPING), 
    m_cUpgradeStep(0), 
    m_cUpgradeType(0), 
    m_cZone(cZone), 
    m_cChannel(cChannel), 
    m_Pos(Pos),
	m_cMaterial(0),
	m_cSiegeCount(0),
    m_dwRepairHP(0), 
    m_fDefaultDir(0), 
    m_cDevelopTime(Siege::DEFAULT_TIME_VALUE), 
    m_cUpgradeTime(Siege::DEFAULT_TIME_VALUE),
    m_cRepairTime(Siege::DEFAULT_REPAIR_TIME), 
	m_cChangeTime(Siege::DEFAULT_TIME_VALUE),
    m_DBComponent(DBComponent)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

    m_TimeValue.Year	= sysTime.wYear;
	m_TimeValue.Month	= sysTime.wMonth;	
	m_TimeValue.Day		= sysTime.wDay;
	m_TimeValue.Hour	= sysTime.wHour;
	m_TimeValue.Minute	= sysTime.wMinute;
	m_TimeValue.Second	= sysTime.wSecond;

	m_LastUseTime = m_TimeValue;
}

CSiegeObjectDB::CSiegeObjectDB(CDBComponent& DBComponent, unsigned long dwOwnerID, 
                               unsigned char cNation, unsigned short wObjectType,
                               unsigned char cUpgradeStep, unsigned char cZone, unsigned char cChannel, 
                               Position Pos)
:   m_dwCID(0),
	m_dwCampOrCastleID(0),
    m_dwOwnerID(dwOwnerID), 
    m_dwGID(0), 
	m_cNation(cNation),
    m_dwHP(0), 
    m_wObjectType(wObjectType), 
    m_cSubState(0),
    m_cState(DEVELOPING), 
    m_cUpgradeStep(cUpgradeStep), 
    m_cUpgradeType(0),
    m_cZone(cZone), 
    m_cChannel(cChannel), 
    m_Pos(Pos), 
	m_cMaterial(0),
	m_cSiegeCount(0),
    m_dwRepairHP(0), 
    m_fDefaultDir(0), 
    m_cDevelopTime(Siege::DEFAULT_TIME_VALUE),
    m_cUpgradeTime(Siege::DEFAULT_TIME_VALUE), 
    m_cRepairTime(Siege::DEFAULT_REPAIR_TIME), 
	m_cChangeTime(Siege::DEFAULT_TIME_VALUE),
    m_DBComponent(DBComponent)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_TimeValue.Year	= sysTime.wYear;
	m_TimeValue.Month	= sysTime.wMonth;	
	m_TimeValue.Day		= sysTime.wDay;
	m_TimeValue.Hour	= sysTime.wHour;
	m_TimeValue.Minute	= sysTime.wMinute;
	m_TimeValue.Second	= sysTime.wSecond;

	m_LastUseTime = m_TimeValue;
}

CSiegeObjectDB::~CSiegeObjectDB()
{
}


void	CSiegeObjectDB::SendCampCmd(unsigned char cSubCmd, unsigned long dwValue1, unsigned long dwValue2)
{
	if ( IsCamp() )
	{
		SERVER_ID serverID;
		serverID.sID.Type       = CServerSetup::GameServer;
		serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
		serverID.sID.Channel    = m_cChannel;
		serverID.sID.ID         = m_cZone;

		GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
			DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

		if(0 != lpGameDispatch)
		{
			CSendStream& SendStream = lpGameDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktCampCmd));
			if(0 != lpBuffer)
			{
				PktCampCmd* lpPktCampCmd = reinterpret_cast<PktCampCmd*>(lpBuffer);

				lpPktCampCmd->m_dwCID			= m_dwCID;
				lpPktCampCmd->m_dwCampID		= GetCampID();
				lpPktCampCmd->m_cState			= m_cState;
				lpPktCampCmd->m_dwValue1		= dwValue1;
				lpPktCampCmd->m_dwValue2		= dwValue2;
				lpPktCampCmd->m_cSubCmd			= cSubCmd;

				SendStream.WrapHeader(sizeof(PktCampCmd), CmdCampCmd, 0, 0);
			}
		}
	}
}


void	CSiegeObjectDB::SendCastleCmd(unsigned long dwValue, unsigned char cSubCmd)
{
	if ( IsCastleObject() )
	{
		SERVER_ID serverID;
		serverID.sID.Type       = CServerSetup::GameServer;
		serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
		serverID.sID.Channel    = m_cChannel;
		serverID.sID.ID         = m_cZone;

		GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
			DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

		if (0 != lpGameDispatch)
		{
			CSendStream& SendStream = lpGameDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktCampCmd));
			if (0 != lpBuffer)
			{
				PktCastleCmd* lpPktCastleCmd = reinterpret_cast<PktCastleCmd*>(lpBuffer);

				lpPktCastleCmd->m_dwCastleID		= GetCastleID();
				lpPktCastleCmd->m_dwCastleObjectID	= m_dwCID;
				lpPktCastleCmd->m_cState			= m_cState;
				lpPktCastleCmd->m_dwValue1			= dwValue;
				lpPktCastleCmd->m_dwValue2			= 0;
				lpPktCastleCmd->m_cSubCmd			= cSubCmd;

				SendStream.WrapHeader(sizeof(PktCastleCmd), CmdCastleCmd, 0, 0);
			}
		}
	}
}


void	CSiegeObjectDB::SendSiegeArmsCmd(unsigned long dwValue, unsigned char cSubCmd)
{
	if ( IsSiegeArms() )
	{
		SERVER_ID serverID;

		serverID.sID.Type       = CServerSetup::GameServer;
		serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
		serverID.sID.Channel    = m_cChannel;
		serverID.sID.ID         = m_cZone;

		GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
			DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

		if(0 != lpGameDispatch)
		{
			CSendStream& SendStream = lpGameDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktSiegeArmsCmd));
			if(0 != lpBuffer)
			{
				PktSiegeArmsCmd* lpPktSiegeArmsCmd = reinterpret_cast<PktSiegeArmsCmd*>(lpBuffer);

				lpPktSiegeArmsCmd->m_dwCID		= m_dwOwnerID;
				lpPktSiegeArmsCmd->m_dwArmsID	= m_dwCID;
				lpPktSiegeArmsCmd->m_cState		= m_cState;
				lpPktSiegeArmsCmd->m_dwValue	= dwValue;
				lpPktSiegeArmsCmd->m_cSubCmd	= cSubCmd;

				SendStream.WrapHeader(sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, 0);
			}
		}
	}
}


void	CSiegeObjectDB::UpdateLastUseTime(bool bInit)
{
	if (bInit)
	{
		m_LastUseTime.Year		= 0;
		m_LastUseTime.Month		= 0;
		m_LastUseTime.Day		= 0;
		m_LastUseTime.Hour		= 0;
		m_LastUseTime.Minute	= 0;
		m_LastUseTime.Second	= 0;
	}
	else
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		m_LastUseTime.Year		= sysTime.wYear;
		m_LastUseTime.Month		= sysTime.wMonth;
		m_LastUseTime.Day		= sysTime.wDay;
		m_LastUseTime.Hour		= sysTime.wHour;
		m_LastUseTime.Minute	= sysTime.wMinute;
		m_LastUseTime.Second	= sysTime.wSecond;
	}
}

bool	CSiegeObjectDB::IsCamp() const
{
	switch ( m_wObjectType )
	{
		case Siege::CAMP:
		case Siege::CAMP_SHOP:
		case Siege::MINING_CAMP:
		case Siege::KARTERANT_WEAPON:
		case Siege::MERKADIA_WEAPON:
			return true;
	}

	return false;
}

bool	CSiegeObjectDB::IsCastleObject() const
{
	switch ( m_wObjectType )
	{
		case Siege::EMBLEM :
		case Siege::GATE :
		case Siege::BACKDOOR :
		case Siege::CASTLE_ARMS_NPC :
		case Siege::GUARD :
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		{
			return true;
		}
	}

	return false;
}

bool	CSiegeObjectDB::IsCastleArms() const
{
	switch ( m_wObjectType )
	{
		case Siege::GUARD :	
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		case Siege::CASTLE_ARMS_NPC :
		{
			return true;
		}
	}

	return false;
}

bool	CSiegeObjectDB::IsSiegeArms() const
{
	switch ( m_wObjectType )
	{
		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
		case Siege::AIRSHIP :
		{
			return true;
		}
	}

	return false;
}

bool	CSiegeObjectDB::IsRidable() const
{
	switch ( m_wObjectType )
	{
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
		case Siege::AIRSHIP :
		{
			return true;
		}
	}

	return false;
}

bool CSiegeObjectDB::IsWorldWeapon() const
{
	if (Siege::KARTERANT_WEAPON == m_wObjectType || Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		return true;
	}

	return false;
}

void	CSiegeObjectDB::SetGID(unsigned long dwGID)
{
	m_dwGID = dwGID;

//	if (IsEmblem() && 0 == dwGID)
//	{
//		m_cState = Siege::COMPLETE;
//		m_cSubState = Siege::NONE;
//	}
}

void	CSiegeObjectDB::SetNation(unsigned char cNation)
{
	m_cNation = cNation ;

	if (IsEmblem() && Creature::STATELESS == cNation)
	{
		m_cState = Siege::COMPLETE;
		m_cSubState = Siege::NONE;
	}
}

void	CSiegeObjectDB::SetZoneChannel(unsigned char cZone, unsigned char cChannel)
{
	m_cZone = cZone;
	m_cChannel = cChannel;
}

void	CSiegeObjectDB::SetMaterialNum(unsigned char cMaterial)
{
	m_cMaterial = cMaterial;
}
