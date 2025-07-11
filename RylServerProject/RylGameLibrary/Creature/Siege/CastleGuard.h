#ifndef _CASTLE_GUARD_OBJECT_H_
#define _CASTLE_GUARD_OBJECT_H_

#pragma once

#include <Creature/Siege/CastleArms.h>

using namespace Siege;


class CCastleGuard : public CCastleArms
{
public:
	virtual ~CCastleGuard();

	// CSkillMonster �� ���
    void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);
	void	SearchPlayer(void);

	bool	Dead(CAggresiveCreature* pOffencer);

	// ����� �ڵ� �����̹Ƿ� �Ʒ� �Լ��� ������ ����.
	bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
					unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// Rider ���� ����
	bool	Ride(unsigned long dwCID)		{ return false;	}	// ���� ž��
	bool	GetOff(unsigned long dwCID)		{ return false;	}	// ���� ���� (�߰� ������ ��Ŷ ����)

private:
	CCastleGuard(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	friend	class	CSiegeObjectMgr;
};

#endif	_CASTLE_GUARD_OBJECT_H_