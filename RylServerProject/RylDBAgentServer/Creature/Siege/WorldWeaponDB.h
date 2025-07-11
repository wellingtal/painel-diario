//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeaponDB.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: ���� ���� DB
//
// Date: 2004. 12. 14. (ȭ)
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

	// Process �Լ�
	Siege::eReturn	Process();

	// Cmd ó�� ���� �Լ�
	bool	Cancel();			// ���� ��� �Ϸ�
	bool	Destroy(unsigned long dwEnemyGID = 0, bool bDeductFame = false);
    
	bool	UpdateWeaponState(unsigned char cWeaponState, unsigned long dwValue1, unsigned long dwValue2);
	bool	Fire();
		
protected:

	// Client �� ��û���� ���ο� ���� ���� ����
	CWorldWeaponDB(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwGID, unsigned short wObjectType,
				   unsigned char cZone, unsigned char cChannel, Position Pos);

	// DB ���� �о�� ������ ���� ���� ��ü ����
	CWorldWeaponDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);

	unsigned char	m_cChargeTime;
	int				m_iFireX, m_iFireZ;

	friend	class	CSiegeObjectDBMgr;
};

#endif //_RYL_DBAGENT_WORLD_WEAPON_DB_H_