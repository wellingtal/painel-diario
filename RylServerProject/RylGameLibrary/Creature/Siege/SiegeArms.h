#ifndef _BASE_SIEGE_ARMS_OBJECT_H_
#define _BASE_SIEGE_ARMS_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeConstants.h>

using namespace Siege;


class CSiegeArms : public CSiegeObject
{
public:
	virtual ~CSiegeArms();

	// CSkillMonster 의 기능
	virtual bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError);
	virtual bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	virtual bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType);

	virtual bool	Dead(CAggresiveCreature* pOffencer);

	// 공성 병기 관련 함수
	bool	Build(unsigned char cUpgradeStep = 0);
	bool	Repair(unsigned short wRepairHP);
	bool	Destroy(unsigned char cOffencerNation = 0, bool bTakeGold = false );
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);

	// 스타터킷 아이템으로 전환 (bMaterial 이 true 이면 자재의 50% 를 돌려준다.)
	
	bool	ToStarterKit(bool bMaterial = false);

	bool	GiveBackSiegeMaterial();							// 공성 병기 자재 돌려주기
	unsigned short	GetRepairMaterialNum();						// 수리시 필요한 자재수

	// Rider 관련 정보
	virtual bool	Ride(unsigned long dwCID);					// 병기 탑승
	virtual bool	GetOff(unsigned long dwCID);				// 병기 내림 (중계 서버로 패킷 전송 : 사용안할시 파괴를 위해)

protected:
	CSiegeArms(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep);

	friend	class	CSiegeObjectMgr;
};

#endif	_BASE_SIEGE_ARMS_OBJECT_H_