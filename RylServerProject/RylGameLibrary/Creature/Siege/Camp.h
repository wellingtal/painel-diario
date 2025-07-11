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

	// CSkillMonster 의 기능
	void	NormalBehavior(unsigned long dwTick);
	void	AttackBehavior(unsigned long dwTick);

	void	SearchPlayer(void);
	bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);
	
	bool	Dead(CAggresiveCreature* pOffencer);

	// 길드 요새 관련 함수
	virtual bool	Build(unsigned char cUpgradeStep = 0);		// 길드 요새 구축 완료
			bool	Cancel();									// 길드 요새 구축 취소 완료
			bool	Upgrade(unsigned char cUpgradeStep);		// 길드 요새 업그레이드 완료
			bool	Repair(unsigned short wRepairHP);			// 길드 요새 수리 완료
	virtual bool	Destroy(unsigned long dwOffencerGID = 0);	// 길드 요새 파괴 완료 (자체 파괴, 파괴됨)
			bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwNoValue, unsigned char cSubCmd);	// 길드 요새 정보 업데이트
			bool	ChangeType(unsigned short wChangeType);		// 길드 요새 타입 변형 완료
    
	void	SetRight(CampRight campRight);
	bool	CheckRight(unsigned char cRightType, unsigned long dwCID, unsigned long dwGID);

	// 길드 요새 파괴시 길드 마스터에게 돈을 돌려준다. (로그인해있을때만)
	// (로그아웃한 경우는 중계에서 알아서 처리해준다)
	void	AddGoldToMaster(unsigned long dwGold);

	// 스타터킷 아이템으로 전환 (bFullMaterial 이 true 이면 자재의 90%를 돌려준다.)
	bool	ToStarterKit(bool bFullMaterial = false);

	
	// 패킷 전송 함수
	void	SendAttackedMessage();				// To DBAgentServer


	// Get / Set 함수
	const CampRight&	GetCampRight() const	{ return m_CampRight;	}
	unsigned long		GetRepairGold() const;

	bool				UpdateMaterialNum(unsigned char cMaterial);

protected:

	CCamp(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
		unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount,
		const CampRight& campRight, bool bFullHP);

	unsigned long	m_dwLastAttackedTick;	// 마지막으로 공격 받은 시간

	CampRight		m_CampRight;			// 길드 요새 관리 권한

	unsigned long	m_dwCmdSenderCID;		// 길드 요새 명령 패킷을 요청한 캐릭터의 CID (업그레이드와 타입 변형에만 사용)

	friend	class	CSiegeObjectMgr;
};

#endif	_CAMP_OBJECT_H_