//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeapon.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 월드 웨폰
//
// Date: 2004. 12. 14. (화)
//--------------------------------------------------------------------------------------------------------------------------

#ifndef _WORLD_WEAPON_OBJECT_H_
#define _WORLD_WEAPON_OBJECT_H_

#pragma once
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include <Creature/Siege/Camp.h>

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CWorldWeapon
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CWorldWeapon : public CCamp
{
public:
	virtual ~CWorldWeapon();

	bool	Build(unsigned char cUpgradeStep = 0);		// 월드 웨폰 구축 완료
	bool	Destroy(unsigned long dwOffencerGID = 0);	// 월드 웨폰 파괴 완료 (자체 파괴, 파괴됨)

	bool	UpdateWeaponState(unsigned char cWeaponState, int iX = 0, int iZ = 0);
	bool	FireWarning();
	bool	Fire();

	int		GetFireCount() const	{ return m_iWorldWeaponCount;	}

private:
	CWorldWeapon(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
				unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
				unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount,
				const CampRight& campRight, bool bFullHP);

	int		m_iFireX, m_iFireZ;				// 발사 좌표
	int		m_iWorldWeaponCount;			// 월드 웨폰 발사 카운트

	friend	class	CSiegeObjectMgr;
};

#endif //_WORLD_WEAPON_OBJECT_H_