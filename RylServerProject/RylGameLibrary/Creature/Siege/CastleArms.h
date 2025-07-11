#ifndef _BASE_CASTLE_ARMS_OBJECT_H_
#define _BASE_CASTLE_ARMS_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeConstants.h>

using namespace Siege;


class CCastleArms : public CSiegeObject
{
public:
	virtual ~CCastleArms();

	// CSkillMonster 의 기능
	virtual bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError);
	virtual bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	virtual bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType);

	virtual bool	Dead(CAggresiveCreature* pOffencer);

	// 수성 병기 관련 함수
	bool	Upgrade(unsigned char cUpgradeStep);
	bool	Repair(unsigned short wRepairHP);
	bool	Destroy(unsigned char cOffencerNation = 0, bool bTakeGold = false );
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwOwnerID, unsigned char cSubCmd);

	bool	ChangeType(unsigned short wType);

	// Rider 관련 정보
	virtual bool		Ride(unsigned long dwCID);			// 병기 탑승
	virtual bool		GetOff(unsigned long dwCID);		// 병기 내림

protected:
	CCastleArms(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	friend	class	CSiegeObjectMgr;
};

#endif	_BASE_CASTLE_ARMS_OBJECT_H_