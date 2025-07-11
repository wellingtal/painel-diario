//--------------------------------------------------------------------------------------------------------------------------
// File Name: WorldWeapon.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: ���� ����
//
// Date: 2004. 12. 14. (ȭ)
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

	bool	Build(unsigned char cUpgradeStep = 0);		// ���� ���� ���� �Ϸ�
	bool	Destroy(unsigned long dwOffencerGID = 0);	// ���� ���� �ı� �Ϸ� (��ü �ı�, �ı���)

	bool	UpdateWeaponState(unsigned char cWeaponState, int iX = 0, int iZ = 0);
	bool	FireWarning();
	bool	Fire();

	int		GetFireCount() const	{ return m_iWorldWeaponCount;	}

private:
	CWorldWeapon(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
				unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
				unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount,
				const CampRight& campRight, bool bFullHP);

	int		m_iFireX, m_iFireZ;				// �߻� ��ǥ
	int		m_iWorldWeaponCount;			// ���� ���� �߻� ī��Ʈ

	friend	class	CSiegeObjectMgr;
};

#endif //_WORLD_WEAPON_OBJECT_H_