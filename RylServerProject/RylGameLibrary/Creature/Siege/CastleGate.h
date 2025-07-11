#ifndef _CASTLE_GATE_OBJECT_H_
#define _CASTLE_GATE_OBJECT_H_

#pragma once

#include <list>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeConstants.h>

using namespace Siege;


class CCastleGate : public CSiegeObject
{
public:
	typedef std::list<unsigned long>		ProtectCIDList;

	virtual ~CCastleGate();

	// CSkillMonster 의 기능
	bool	Dead(CAggresiveCreature* pOffencer);

	// 성문 관련 함수
	void	AddProtectGate(CCharacter* lpProtectChar);
	void	DeleteProtectGate(CCharacter* lpProtectChar);
	void	DivideDamage(CAggresiveCreature* pOffencer, unsigned short wDamage);
	void	SendProtectGateInfo();

	void	Open();
	void	Close();
	void	ForceOpen();
	void	ForceClose();

	bool	Upgrade(unsigned char cUpgradeStep);
	bool	Repair(unsigned short wRepairHP);
	bool	Restore();
	bool	Destroy(unsigned char cOffencerNation = 0, bool bTakeGold = false );
	bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);

	// Get / Set 함수
	unsigned long		GetRepairGold() const;

private:
	CCastleGate(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);

	ProtectCIDList	m_ProtectCharList;		// 성문 막기중인 캐릭터 CID 리스트

	friend	class	CSiegeObjectMgr;
};

#endif	_CASTLE_GATE_OBJECT_H_