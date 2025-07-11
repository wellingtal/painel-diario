//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeaponDB.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 월드 웨폰 DB
//
// Date: 2004. 12. 14. (화)
//--------------------------------------------------------------------------------------------------------------------------
#ifndef _RYL_DBAGENT_WORLD_WEAPON_DB_H_
#define _RYL_DBAGENT_WORLD_WEAPON_DB_H_

#pragma once
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "CampDB.h"

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CWorldWeaponDB
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CWorldWeaponDB : public CCampDB
{
public:
	virtual ~CWorldWeaponDB();

	// Process 함수
	Siege::eReturn	Process();

	// Cmd 처리 가상 함수
	bool	Cancel();			// 구축 취소 완료
	bool	Destroy(unsigned long dwEnemyGID = 0, bool bDeductFame = false);
    
	bool	UpdateWeaponState(unsigned char cWeaponState, unsigned long dwValue1, unsigned long dwValue2);
	bool	Fire();
		
protected:

	// Client 의 요청으로 새로운 월드 웨폰 생성
	CWorldWeaponDB(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwGID, unsigned short wObjectType,
				   unsigned char cZone, unsigned char cChannel, Position Pos);

	// DB 에서 읽어온 값으로 월드 웨폰 객체 생성
	CWorldWeaponDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);

	unsigned char	m_cChargeTime;
	int				m_iFireX, m_iFireZ;

	friend	class	CSiegeObjectDBMgr;
};

#endif //_RYL_DBAGENT_WORLD_WEAPON_DB_H_