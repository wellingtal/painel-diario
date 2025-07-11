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
		MAX_THREAT_TARGET	= 10,			// ����Ʈ ����Ʈ�� �ִ� ��ü ��
		MAX_THREAT_REWARD	= 3,			// ����Ʈ�� ���� ����ġ ������ �޴� �ִ� �׷� ��

		FAME_CONSENT_GAP	= 5,			// �� �й� ��� ���� ��
	//	EXP_CONSENT_GAP		= 14,			// ����ġ �й� ��� ���� ��
	//	EXP_BALANCE_HANDLE	= 14,			// ��Ƽ ����ġ �й�� �뷱�� �ڵ�
		EXP_CONSENT_GAP		= 100,			// ����ġ �й� ��� ���� ��
		EXP_BALANCE_HANDLE	= 50,

		MOVING_ATTACK_TAUNT_RATE	= 3,	// �̵� ������ ������ �Ÿ��� Ÿ�ٿ� ���� ����Ʈ ����ġ
		ATTACK_TAUNT_RATE			= 6		// �Ϲ� ������ ������ �Ÿ��� Ÿ�ٿ� ���� ����Ʈ ����ġ
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
		// �� ���¡ ���� ��ġ (�ֱٿ� �ڽ��� ���� ĳ���͸� ����صд�.)
		enum 
		{	
			MAX_SAVING_ENEMY			= 5,		// ����ϴ� �ִ� ��ü ��
			MAX_SAVING_TICKCOUNT		= 300000,	// �� ��ü�� �ִ� ��� �ð� (5��)
			MAX_SAVING_TICKCOUNT_FOR_US	= 900000	// �̱� ��ʸ�Ʈ �������� �� ��ü�� �ִ� ��� �ð� (15��)
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

	void DivisionExp(void);					// ����ġ �й�
	bool SaveEnemy(unsigned long dwCID);	// �ֱ� ���� ��븦 ���� (�� �о��ֱ� ������)

	SaveEnemyInfo* GetSaveEnemyInfo(void)	{ return m_LatestEnemy; }

protected:

	typedef std::list<ThreatInfo>			ThreatList;
	typedef std::list<CAggresiveCreature*>	ThreatenedList;

	ThreatList		    m_ThreatList;		// ����Ʈ ����Ʈ
	ThreatenedList	    m_ThreatenedList;	// Ÿ ũ������ ����Ʈ ����Ʈ���� ��ȣ������ ����Ʈ

	CAggresiveCreature* m_pOwner;			// �� ����Ʈ�� ������ �ִ� ũ��ó

	unsigned char			m_cMaxLevel;								// ����Ʈ ����Ʈ�� ���Դ� ũ������ �ְ� ���� (������ �����)
	Creature::CreatureType	m_eMaxLevelCreatureType;					// �ְ� ���� ũ������ Ÿ��

	SaveEnemyInfo		m_LatestEnemy[SaveEnemyInfo::MAX_SAVING_ENEMY];	// �ֱٿ� �ڽ��� ������ ũ���� ��� (PvP�� ����� ����)
};


#endif