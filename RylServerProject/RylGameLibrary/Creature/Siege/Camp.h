#ifndef _CAMP_OBJECT_H_
#define _CAMP_OBJECT_H_

#pragma once

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeConstants.h>

using namespace Siege;


class CCamp : public CSiegeObject
{
public:

	virtual ~CCamp();

	// CSkillMonster �� ���
	void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);

	void	SearchPlayer(void);
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	
	bool	Dead(CAggresiveCreature* pOffencer);

	// ��� ��� ���� �Լ�
	virtual bool	Build(unsigned char cUpgradeStep = 0);		// ��� ��� ���� �Ϸ�
			bool	Cancel();									// ��� ��� ���� ��� �Ϸ�
			bool	Upgrade(unsigned char cUpgradeStep);		// ��� ��� ���׷��̵� �Ϸ�
			bool	Repair(unsigned short wRepairHP);			// ��� ��� ���� �Ϸ�
	virtual bool	Destroy(unsigned long dwOffencerGID = 0);	// ��� ��� �ı� �Ϸ� (��ü �ı�, �ı���)
			bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);	// ��� ��� ���� ������Ʈ
			bool	ChangeType(unsigned short wChangeType);		// ��� ��� Ÿ�� ���� �Ϸ�
    
	void	SetRight(CampRight campRight);
	bool	CheckRight(unsigned char cRightType, unsigned long dwCID, unsigned long dwGID);

	// ��� ��� �ı��� ��� �����Ϳ��� ���� �����ش�. (�α�������������)
	// (�α׾ƿ��� ���� �߰迡�� �˾Ƽ� ó�����ش�)
	void	AddGoldToMaster(unsigned long dwGold);

	// ��Ÿ��Ŷ ���������� ��ȯ (bFullMaterial �� true �̸� ������ 90%�� �����ش�.)
	bool	ToStarterKit(bool bFullMaterial = false);

	
	// ��Ŷ ���� �Լ�
	void	SendAttackedMessage();				// To DBAgentServer


	// Get / Set �Լ�
	const CampRight&	GetCampRight() const	{ return m_CampRight;	}
	unsigned long		GetRepairGold() const;

	bool				UpdateMaterialNum(unsigned char cMaterial);

protected:

	CCamp(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
		unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount,
		const CampRight& campRight, bool bFullHP);

	unsigned long	m_dwLastAttackedTick;	// ���������� ���� ���� �ð�

	CampRight		m_CampRight;			// ��� ��� ���� ����

	unsigned long	m_dwCmdSenderCID;		// ��� ��� ��� ��Ŷ�� ��û�� ĳ������ CID (���׷��̵�� Ÿ�� �������� ���)

	friend	class	CSiegeObjectMgr;
};

#endif	_CAMP_OBJECT_H_