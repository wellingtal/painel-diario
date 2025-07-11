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

	// CSkillMonster �� ���
	virtual bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError);
	virtual bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	virtual bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType);

	virtual bool	Dead(CAggresiveCreature* pOffencer);

	// ���� ���� ���� �Լ�
	bool	Build(unsigned char cUpgradeStep = 0);
	bool	Repair(unsigned short wRepairHP);
	bool	Destroy(unsigned char cOffencerNation = 0, bool bTakeGold = false );
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);

	// ��Ÿ��Ŷ ���������� ��ȯ (bMaterial �� true �̸� ������ 50% �� �����ش�.)
	
	bool	ToStarterKit(bool bMaterial = false);

	bool	GiveBackSiegeMaterial();							// ���� ���� ���� �����ֱ�
	unsigned short	GetRepairMaterialNum();						// ������ �ʿ��� �����

	// Rider ���� ����
	virtual bool	Ride(unsigned long dwCID);					// ���� ž��
	virtual bool	GetOff(unsigned long dwCID);				// ���� ���� (�߰� ������ ��Ŷ ���� : �����ҽ� �ı��� ����)

protected:
	CSiegeArms(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep);

	friend	class	CSiegeObjectMgr;
};

#endif	_BASE_SIEGE_ARMS_OBJECT_H_