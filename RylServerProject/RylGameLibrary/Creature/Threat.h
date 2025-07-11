#ifndef _CTHREAT_H_
#define _CTHREAT_H_

#pragma once
#pragma warning(disable:4800)

#include <list>


class CAggresiveCreature;
class CParty;

class CThreat
{
public:

	enum Const
	{	
		MAX_THREAT_TARGET	= 10,			// 쓰레트 리스트의 최대 개체 수
		MAX_THREAT_REWARD	= 3,			// 쓰레트에 의해 경험치 보상을 받는 최대 그룹 수

		FAME_CONSENT_GAP	= 5,			// 명성 분배 허용 레벨 폭
	//	EXP_CONSENT_GAP		= 14,			// 경험치 분배 허용 레벨 폭
	//	EXP_BALANCE_HANDLE	= 14,			// 파티 경험치 분배용 밸런스 핸들
		EXP_CONSENT_GAP		= 100,			// 경험치 분배 허용 레벨 폭
		EXP_BALANCE_HANDLE	= 50,

		MOVING_ATTACK_TAUNT_RATE	= 3,	// 이동 공격이 가능한 거리의 타겟에 대한 쓰레트 가중치
		ATTACK_TAUNT_RATE			= 6		// 일반 공격이 가능한 거리의 타겟에 대한 쓰레트 가중치
	};

	enum AffectThreatType
	{
		TAUNT		= 1,
		DETAUNT		= 2
	};

	struct ThreatInfo
	{
		CAggresiveCreature* m_pCreature;
		long				m_lThreatAmount;

		ThreatInfo() : m_pCreature(NULL), m_lThreatAmount(0) { }
		ThreatInfo(CAggresiveCreature* pCreature, long lThreatAmount)
			:	m_pCreature(pCreature), m_lThreatAmount(lThreatAmount) { }
	};

	struct AwardInfo
	{
		CAggresiveCreature* m_pCreature;
		CParty*             m_pParty;
		long				m_lAward;

		AwardInfo() : m_pCreature(NULL), m_pParty(NULL), m_lAward(0) { }
		AwardInfo(CAggresiveCreature* pCreature, CParty* pParty, long lAward)
			:	m_pCreature(pCreature), m_pParty(pParty), m_lAward(lAward) { }
	};

	struct SaveEnemyInfo
	{
		// 명성 어뷰징 금지 장치 (최근에 자신을 죽인 캐릭터를 기억해둔다.)
		enum 
		{	
			MAX_SAVING_ENEMY			= 5,		// 기억하는 최대 개체 수
			MAX_SAVING_TICKCOUNT		= 300000,	// 한 개체당 최대 기억 시간 (5분)
			MAX_SAVING_TICKCOUNT_FOR_US	= 900000	// 미국 토너먼트 서버군용 한 개체당 최대 기억 시간 (15분)
		};

		unsigned long	m_dwCID;
		unsigned long	m_dwTickCount;

		SaveEnemyInfo() : m_dwCID(0), m_dwTickCount(0) { }
	};


	CThreat(void);
	~CThreat(void);

	void ClearAll(void);
	void ClearThreatList(void);
	void ClearThreatenedList(void);

	void SetOwner(CAggresiveCreature* pCreature) { m_pOwner = pCreature; }

	void AddToThreatList(CAggresiveCreature* pAttackCreature, long lThreatAmount);
	void AddToThreatenedList(CAggresiveCreature* pDefendCreature);

	void HealThreat(CAggresiveCreature* pHealCreature, long lThreatAmount);
	void AffectThreat(CAggresiveCreature* pTauntCreature, long lDamage, AffectThreatType eType);

	bool DeleteThreat(CAggresiveCreature* pAttackCreature);
	bool DeleteThreatened(CAggresiveCreature* pDefendCreature);

	CAggresiveCreature* GetTarget(void);
	CAggresiveCreature* GetMaxThreatCreature(void)	{ return m_ThreatList.empty() ? NULL : m_ThreatList.back().m_pCreature;		}
    long GetMaxThreatAmount(void)					{ return m_ThreatList.empty() ? 0 : m_ThreatList.back().m_lThreatAmount;	}

	inline float GetAggravation(CAggresiveCreature *pCreature);
	unsigned char GetAward(unsigned long* aryItemID, Item::CItem** aryItem, unsigned long* dwOwnerID, bool farm=false);//hz added this for farm last hit get the farm=false

	void DivisionExp(void);					// 경험치 분배
	bool SaveEnemy(unsigned long dwCID);	// 최근 죽인 상대를 저장 (명성 밀어주기 방지용)

	SaveEnemyInfo* GetSaveEnemyInfo(void)	{ return m_LatestEnemy; }

protected:

	typedef std::list<ThreatInfo>			ThreatList;
	typedef std::list<CAggresiveCreature*>	ThreatenedList;

	ThreatList		    m_ThreatList;		// 쓰레트 리스트
	ThreatenedList	    m_ThreatenedList;	// 타 크리쳐의 쓰레트 리스트와의 상호참조용 리스트

	CAggresiveCreature* m_pOwner;			// 이 쓰레트를 가지고 있는 크리처

	unsigned char			m_cMaxLevel;								// 쓰레트 리스트에 들어왔던 크리쳐중 최고 레벨 (아이템 보상용)
	Creature::CreatureType	m_eMaxLevelCreatureType;					// 최고 레벨 크리쳐의 타입

	SaveEnemyInfo		m_LatestEnemy[SaveEnemyInfo::MAX_SAVING_ENEMY];	// 최근에 자신을 살해한 크리쳐 목록 (PvP시 어뷰즈 방지)
};


#endif