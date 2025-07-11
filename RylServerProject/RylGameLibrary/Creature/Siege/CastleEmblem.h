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

	// CSkillMonster �� ���
	void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);
	void	SearchPlayer(void);

	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	bool	Dead(CAggresiveCreature* pOffencer);

	// �� ��¡�� ���� �Լ�
	bool	Upgrade(unsigned char cUpgradeStep);
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);	// ��¡�� ������Ʈ (��ȯ��, ��ȯ�Ϸ�, ���׷��̵���)

	// Get / Set �Լ�
	unsigned char	GetEnemyNation() const		{ return m_cEnemyNation;	}

	// Send �Լ�
	void	SendAttackedMessage();

private:
	CCastleEmblem(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	unsigned char	m_cEnemyNation;			// ��¡���� �ν� ���� ����
	unsigned long	m_dwLastAttackedTick;	// ���������� ���� ���� �ð�

	friend	class	CSiegeObjectMgr;
};

#endif	_CASTLE_EMBLEM_OBJECT_H_