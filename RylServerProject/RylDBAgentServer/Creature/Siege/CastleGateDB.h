#ifndef _RYL_DBAGENT_CASTLE_GATE_DB_H_
#define _RYL_DBAGENT_CASTLE_GATE_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CCastleGateDB : public CSiegeObjectDB
{
public:
	virtual ~CCastleGateDB();

	// Process �Լ�
	Siege::eReturn	Process();

	// Cmd ó�� ���� �Լ�
	bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL);
	bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0);
	bool	Restore(unsigned char cRestoreTime, unsigned long dwRestoreGold = 0);
	bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);

	void	UpdateHP(unsigned long dwNowHP);

	// �߰� �������� ������ Open / Close ���·� ����� ��� (���´� Complete�� �ȴ�.)
	void	ForceOpen();
	void	ForceClose();

private:

	CCastleGateDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject);

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_CASTLE_GATE_DB_H_