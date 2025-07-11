#ifndef _RYL_DBAGENT_CASTLE_ARMS_DB_H_
#define _RYL_DBAGENT_CASTLE_ARMS_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CCastleArmsDB : public CSiegeObjectDB
{
public:
	virtual ~CCastleArmsDB();

	// Process 함수
	Siege::eReturn	Process();

	// Cmd 처리 가상 함수
	bool	Build(unsigned short wType, unsigned char cDevelopTime, unsigned long dwDevelopGold = 0, unsigned long dwOwnerID = 0);
	bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL);
	bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0);
	bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);

	void	UpdateLastUseTime(bool bInit=false);
	void	UpdateHP(unsigned long dwNowHP);
	
private:

	CCastleArmsDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject);

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_CASTLE_ARMS_DB_H_