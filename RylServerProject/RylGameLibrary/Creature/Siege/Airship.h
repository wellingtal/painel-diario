#ifndef _AIRSHIP_OBJECT_H_
#define _AIRSHIP_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeArms.h>

using namespace Siege;


class CAirship : public CSiegeArms
{
public:
	virtual ~CAirship();

	// CSkillMonster 의 기능
	void	NormalBehavior(unsigned long dwTick)				{			}
	void	AttackBehavior(unsigned long dwTick)				{			}
	void	SearchPlayer(void)									{			}

	bool	Dead(CAggresiveCreature* pOffencer);
	void	MoveTo(const Position& NewPosition);

	// 드랍쉽은 아래 함수의 구현이 없다.
	bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
					unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// Rider 관련 정보
	unsigned char	IsRider(unsigned long dwCID) const;			// 해당 캐릭터가 병기에 탑승해 있는가?
	bool			Ride(unsigned long dwCID);					// 병기 탑승
	bool			GetOff(unsigned long dwCID);				// 병기 내림 (중계 서버로 패킷 전송)
	void			AllGetOff();								// 병기에서 모두 내림 (패킷 전송)

	void			GetRiders( unsigned long* pRiders ) const;
	unsigned char	GetRiderNum() const;

private:
	CAirship(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep);

	
	unsigned long	m_dwPassengerCID[Siege::AIRSHIP_PASSENGER_NUM];	// 승객 CID

	friend	class	CSiegeObjectMgr;
};

#endif	_AIRSHIP_OBJECT_H_