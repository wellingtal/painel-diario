#ifndef _RYL_GAMETIME_DB_COMPONENT_H_
#define _RYL_GAMETIME_DB_COMPONENT_H_

// forward decl.

class CDBComponent;

namespace DBComponent
{
	namespace GameTimeDB
	{
		// 게임 시간 업데이트
		bool UpdateGameTimeInfo(CDBComponent& DBComponent, unsigned char cGuildTime, unsigned char cRealmTime,
								unsigned char cSiegeTime, unsigned char cMiningTime);
		bool UpdateGuildTime(CDBComponent& DBComponent, unsigned char cGuildTime);
		bool UpdateRealmTime(CDBComponent& DBComponent, unsigned char cRealmTime);
		bool UpdateSiegeTime(CDBComponent& DBComponent, unsigned char cSiegeTime);
		bool UpdateEnterTime(CDBComponent& DBComponent, unsigned char cEnterTime);
		bool UpdateMiningTime(CDBComponent& DBComponent, unsigned char cMiningTime);
		bool UpdateEventTime(CDBComponent& DBComponent, unsigned char cEventTime);
	}
}

#endif