#ifndef _RYL_DBAGENT_CASTLE_GATE_DB_H_
#define _RYL_DBAGENT_CASTLE_GATE_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CCastleGateDB : public CSiegeObjectDB
{
public:
	virtual ~CCastleGateDB();

	// Process 함수
	Siege::eReturn	Process();

	// Cmd 처리 가상 함수
	bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL);
	bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0);
	bool	Restore(unsigned char cRestoreTime, unsigned long dwRestoreGold = 0);
	bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);

	void	UpdateHP(unsigned long dwNowHP);

	// 중계 서버에서 강제로 Open / Close 상태로 만드는 기능 (상태는 Complete로 된다.)
	void	ForceOpen();
	void	ForceClose();

private:

	CCastleGateDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject);

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_CASTLE_GATE_DB_H_