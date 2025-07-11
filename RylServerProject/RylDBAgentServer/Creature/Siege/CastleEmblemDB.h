#ifndef _RYL_DBAGENT_CASTLE_EMBLEM_DB_H_
#define _RYL_DBAGENT_CASTLE_EMBLEM_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CCastleEmblemDB : public CSiegeObjectDB
{
public:
	virtual ~CCastleEmblemDB();

	// Process 함수
	Siege::eReturn	Process();

	// Cmd 처리 가상 함수
	bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL);
	bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);

	void	UpdateHP(unsigned long dwNowHP);

private:

	CCastleEmblemDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject);

	unsigned char	m_cEnemyNation;			// Nation	: 성 상징물을 공격해서 부순 적의 국가 ID

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_CASTLE_EMBLEM_DB_H_