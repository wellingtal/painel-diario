#ifndef _AIRSHIP_OBJECT_H_
#define _AIRSHIP_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeArms.h>

using namespace Siege;


class CAirship : public CSiegeArms
{
public:
	virtual ~CAirship();

	// CSkillMonster �� ���
	void	NormalBehavior(unsigned long dwTick)				{			}
	void	AttackBehavior(unsigned long dwTick)				{			}
	void	SearchPlayer(void)									{			}

	bool	Dead(CAggresiveCreature* pOffencer);
	void	MoveTo(const Position& NewPosition);

	// ������� �Ʒ� �Լ��� ������ ����.
	bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
					unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// Rider ���� ����
	unsigned char	IsRider(unsigned long dwCID) const;			// �ش� ĳ���Ͱ� ���⿡ ž���� �ִ°�?
	bool			Ride(unsigned long dwCID);					// ���� ž��
	bool			GetOff(unsigned long dwCID);				// ���� ���� (�߰� ������ ��Ŷ ����)
	void			AllGetOff();								// ���⿡�� ��� ���� (��Ŷ ����)

	void			GetRiders( unsigned long* pRiders ) const;
	unsigned char	GetRiderNum() const;

private:
	CAirship(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep);

	
	unsigned long	m_dwPassengerCID[Siege::AIRSHIP_PASSENGER_NUM];	// �°� CID

	friend	class	CSiegeObjectMgr;
};

#endif	_AIRSHIP_OBJECT_H_