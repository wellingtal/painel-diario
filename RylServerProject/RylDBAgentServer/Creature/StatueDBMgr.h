#ifndef _RYL_DBAGENT_STATUE_DB_MANAGER_H_
#define _RYL_DBAGENT_STATUE_DB_MANAGER_H_

// 국가전 석상 DB 정보를 가지고 있는 객체

#pragma once

#include <map>
#include <Network/Packet/PacketStruct/CastlePacket.h>

// 전방 참조
class CDBComponent;
class CSendStream;

class CStatueDBMgr
{
public:
	static CStatueDBMgr& GetInstance();

	// <Key, StatueInfo*> : Key = (Index << 16) | (Channel)
	typedef std::map<unsigned long, StatueInfoDB*>	StatueMap;


	bool	Initialize(CDBComponent& DBComponent);
	
	unsigned long	CalcKeyValue(unsigned long dwIndex, unsigned char cChannel) const;

	bool	UpdateKID(unsigned long dwIndex, unsigned char cChannel, unsigned short wNewKID);
	bool	UpdateLostHP(unsigned long dwIndex, unsigned char cChannel, unsigned long dwLostHP);

	// Send to gameserver
	void	SendStatueInfo(CSendStream& SendStream, unsigned char cChannel);

private:
	
	CStatueDBMgr();
	~CStatueDBMgr();

    StatueInfoDB*	CreateStatue( const StatueInfoDB& statueInfo ) ;
	void			ClearStatueMap() ;

	CDBComponent*	m_lpDBComponent;
	StatueMap		m_StatueMap;
};

#endif	// _RYL_DBAGENT_STATUE_DB_MANAGER_H_
