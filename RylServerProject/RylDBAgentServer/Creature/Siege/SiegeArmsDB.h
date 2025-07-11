#ifndef _RYL_DBAGENT_SIEGE_ARMS_DB_H_
#define _RYL_DBAGENT_SIEGE_ARMS_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CSiegeArmsDB : public CSiegeObjectDB
{
public:
	virtual ~CSiegeArmsDB();

	// Process �Լ�
	Siege::eReturn	Process();

	// Cmd ó�� ���� �Լ�
	bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0);
	bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);

	bool	ToStartKit();

private:

	CSiegeArmsDB(CDBComponent& DBComponent, unsigned long dwOwnerID, 
				unsigned char cNation, unsigned short wObjectType, unsigned char cUpgradeStep, 
				unsigned char cZone, unsigned char cChannel, Position Pos);

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_SIEGE_ARMS_DB_H_