#ifndef _CASTLE_GUARD_OBJECT_H_
#define _CASTLE_GUARD_OBJECT_H_

#pragma once

#include <Creature/Siege/CastleArms.h>

using namespace Siege;


class CCastleGuard : public CCastleArms
{
public:
	virtual ~CCastleGuard();

	// CSkillMonster 의 기능
    void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);
	void	SearchPlayer(void);

	bool	Dead(CAggresiveCreature* pOffencer);

	// 가드는 자동 공격이므로 아래 함수의 구현이 없다.
	bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
					unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// Rider 관련 정보
	bool	Ride(unsigned long dwCID)		{ return false;	}	// 병기 탑승
	bool	GetOff(unsigned long dwCID)		{ return false;	}	// 병기 내림 (중계 서버로 패킷 전송)

private:
	CCastleGuard(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	friend	class	CSiegeObjectMgr;
};

#endif	_CASTLE_GUARD_OBJECT_H_