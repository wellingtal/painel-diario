#ifndef _CASTLE_EMBLEM_OBJECT_H_
#define _CASTLE_EMBLEM_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeConstants.h>

using namespace Siege;


class CCastleEmblem : public CSiegeObject
{
public:
	virtual ~CCastleEmblem();

	// CSkillMonster 의 기능
	void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);
	void	SearchPlayer(void);

	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	bool	Dead(CAggresiveCreature* pOffencer);

	// 성 상징물 관련 함수
	bool	Upgrade(unsigned char cUpgradeStep);
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);	// 상징물 업데이트 (소환중, 소환완료, 업그레이드중)

	// Get / Set 함수
	unsigned char	GetEnemyNation() const		{ return m_cEnemyNation;	}

	// Send 함수
	void	SendAttackedMessage();

private:
	CCastleEmblem(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	unsigned char	m_cEnemyNation;			// 상징물을 부슨 적의 국적
	unsigned long	m_dwLastAttackedTick;	// 마지막으로 공격 받은 시간

	friend	class	CSiegeObjectMgr;
};

#endif	_CASTLE_EMBLEM_OBJECT_H_