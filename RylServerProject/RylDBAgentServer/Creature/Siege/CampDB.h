#ifndef _RYL_DBAGENT_CAMP_DB_H_
#define _RYL_DBAGENT_CAMP_DB_H_

#pragma once

#include "SiegeObjectDB.h"

using namespace Siege;

class CCampDB : public CSiegeObjectDB
{
public:
	virtual ~CCampDB();

	// Process 함수
	virtual Siege::eReturn	Process();

	// Cmd 처리 가상 함수
	virtual bool	CancelBuild();		// 구축 취소
	virtual bool	Cancel();			// 구축 취소 완료
	virtual bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL);
	virtual bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0);
	virtual bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false);	// 파괴 완료 (스스로 파괴, 적에게 파괴)
	virtual bool	ChangeType(unsigned short wType);

	void	UpdateRewardFame(int Fame);

	void	UpdateLastUseTime(bool bInit=false);
	void	UpdateHP(unsigned long dwNowHP);
	bool	ToStartKit();

	unsigned short		GetChangeType() const			{ return m_wChangeType;	}

	const CampRight&	GetRight() const				{ return m_CampRight;	}
	void				SetRight(CampRight campRight);	// 관리 권한 변경

	void				SetMaterialNum(unsigned char cMaterial);
	
	virtual bool	SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out);

protected:

	CCampDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);
	CCampDB(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwGID,
			unsigned char cZone, unsigned char cChannel, Position Pos);

    CampRight		m_CampRight;			// 길드 요새 관리 권한
	unsigned short	m_wChangeType;

	unsigned long	m_dwOldTotalMinutes;

	friend	class	CSiegeObjectDBMgr;
};

#endif	_RYL_DBAGENT_CAMP_DB_H_