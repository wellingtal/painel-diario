
#ifndef _CMONSTER_H_
#define _CMONSTER_H_

#include <Creature/Character/Character.h>
#include <Creature/Monster/FSM/FSM.h>

#include <Map/FieldMap/Cell.h>
#include <Skill/Spell/SpellKind.h>
#include <GameEvent/GameEventMgr.h>
#include <Community/Party/Party.h>

#include "MonsterMgr.h"


const unsigned long LongRange = 45;				// ��Ÿ� ���ݿ� ���� �Ѿư��� �Ÿ�.
const unsigned long OutsideSearchRange = 10;	// ���� �ۿ����� �þ�
const unsigned long ReturnRange = 20;			// ����
const unsigned long MoveRange = 15;				// ���� �ȿ����� �ִ� �̵��Ÿ�.

// ���� ����
class CStatue;

class CMonster : public CAggresiveCreature
{
public:

	enum MovingPattern { AREA = 0, FIXED, ROUTE, NONAREA };

	enum _Const 
	{
		MONSTER_SEARCH_RANGE	= 8,
		BOSS_SEARCH_RANGE		= 15,
		NAMED_SEARCH_RANGE		= 15,
		CHIEF_SEARCH_RANGE		= 15,
		MAGE_SEARCH_RANGE		= 18,
		ACOLYTE_SEARCH_RANGE	= 18,
		GUARD_SEARCH_RANGE		= 15,

		GOLD_INC_LEVEL_LIMIT	= 20,

		RUN_ANI_LIMIT_MAX = 3,
		RUN_ANI_LIMIT_MIN = 1,

		RATE_MOVING_AND_PLAYER = 10,

		MAX_MONSTER_UID = 0xFFFF - 0x8000 - 0x4000,

		MONSTER_PARTY_BIT = 0x80000000
	};

	struct MonsterCreateInfo;	// ���� ���� ����ü
	typedef MonsterCreateInfo *LPMonsterCreateInfo;

	CMonster();
	CMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);
	virtual ~CMonster();

	// --------------------------------------------------------------------------------------------
	// �ʱ� ����

	bool InitMonster(Position& Pos, bool bDead = false);

	virtual bool GetMotion(unsigned long MotionID, MotionInfo& Motion);		// ���� ��� ������ ����.

	// --------------------------------------------------------------------------------------------
	// ���� �ൿ ����

	bool CheckPartyTarget();			// ���� ��Ƽ�� Ÿ���� üũ�ؼ� ������ �����ϴ°�?
	virtual void SearchPlayer(void);	// �÷��̾� �˻�

	bool IsOverlap(void);		// �ٸ� ���Ϳ� ��ġ�°� �˻�

	bool Process(void);			// ���� �ൿ ó�� - ���ο��� UpdateBehaviorȣ��

	void UpdateBehavior(unsigned long dwTick);			// ���� �ൿ ������Ʈ

	virtual void NormalBehavior(unsigned long dwTick);	// ���� ����
	virtual void AttackBehavior(unsigned long dwTick);	// ���� ����
	virtual void ReturnBehavior(unsigned long dwTick);	// ��ȯ ����
	virtual void EscapeBehavior(void);					// ���� ����
	virtual void DeadBehavior(unsigned long dwTick);	// ���� ����
	
	// Action ���� ó��
	void AttackAction();
	void WalkAttackAction(float fVelocity=0.1f);
	void RunAction(float fDistance, float fDstX, float fDstZ);
	bool IsReturn();
	
	inline Position CalculateCoor(void);	// �̵� ���� ��ǥ ���

	virtual const int CalculateFixLevelGap(CAggresiveCreature *pDefender);

	virtual bool Attack(AtType attackType, unsigned char cDefenderNum, 
		CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal);

	inline virtual void Attacked(void);				// ������ ����
	bool MultiAttack(void);							// Ÿ���� ã�� ���� (���ο��� Attack�Լ��� ȣ��)
	void CancelTarget(bool bSaveThreat = false);	// Ÿ���� ���

	virtual bool Dead(CAggresiveCreature* pOffencer);	// �׾��� �� ȣ��Ǵ� �Լ�
	virtual EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL);

	virtual void Respawn(unsigned long dwTick);

	// --------------------------------------------------------------------------------------------
	// ��Ƽ ����

	void			SetPID(unsigned long dwPID)		{ m_dwPID = dwPID;	}
	unsigned long	GetPID(void)					{ return m_dwPID;	}

	// --------------------------------------------------------------------------------------------
	// ��Ŷ�� ���� ������ �Լ���

	virtual LONG SendMove(unsigned short nAniNum);
	
	// --------------------------------------------------------------------------------------------
	// ��Ÿ �Լ�

	virtual Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError) { return NULL; }
	
	bool HasSkill(void);		// ��ų�� ����ϴ� �����ΰ�?
	int GetUseSkillNum(void);	// ����� ��ų�� ������ �ִ°�?

	inline unsigned char GetOptionBaseNum(Item::EquipType::OptionType eType);

	unsigned long GetDropRate(unsigned char nIndex);				
	unsigned long GetAwardItem(int nIndex)	{ return m_MonsterInfo.m_aryAwardItem[nIndex];	}

	inline CStatue* DowncastToStatue(void);

	//	���� �Ϸù�ȣ ���
	int GetSerialNumber(void)				{ return ((m_dwCID - Creature::MONSTER_BIT) >> 16);				}	
	unsigned short GetKID(void)				{ return static_cast<unsigned short>(m_MonsterInfo.m_dwKID);	}

	unsigned char GetPattern(void)			{ return m_MonsterInfo.m_cSkillPattern;	}
	int GetCurrentState(void)				{ return m_nCurrentState;				}	
	short GetAttackRange(void)				{ return m_MonsterInfo.m_wAttackRange;	}

	bool IsDeadSummonMonster(void);
	virtual void SetGuard(bool bGuard)		{ return;		}

    virtual unsigned short ApplyDamage(AtType attackType, CAggresiveCreature* pOffencer, unsigned char &cOffencerJudge,
		unsigned char &cDefenserJudge, unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, unsigned short &wError);

	virtual void GuardMe(CAggresiveCreature* lpTarget, unsigned short wThreat)		{ return;	}
	virtual void AttackCmd(CAggresiveCreature* lpTarget, unsigned short wThreat)	{ return;	}

	virtual bool IsPeaceMode(void)			{ return false;	}
	virtual bool IsRideArms(void) const		{ return false; }
	virtual unsigned char GetNation() const	{ return m_MonsterInfo.m_cNation;		}

	virtual unsigned char GetRealmPoint() 	{ return 0;								}

	static void LogMonsterMoveCount(void);

    Broadcast2nd::CSerializeMonsterData& GetSerializeData() { return m_SerializeMonsterData; }

protected:

	// ���� �����
	enum _Timing 
	{ 
		FPS = 30
	};

	MonsterInfo		m_MonsterInfo;
	Position		m_OriginalPosition;			// ������ �ʱ� ��ġ
	unsigned long	m_dwLastBehaviorTick;		// ���������� �ൿ�� ������ Tick

    Broadcast2nd::CSerializeMonsterData     m_SerializeMonsterData;

	CAggresiveCreature*	m_lpTarget;				// ������ Ÿ��

	long	m_lCurrentFrame;					// ���� �������� Frame�� ��Ÿ��. ������ ���������� Ȯ���ϱ� ���� ����

	int		m_nNormalMovingDelay;				// �븻 ������ �̵� Ÿ�̹� ���� (���� �÷��̾� ���� ���� ������)
	int		m_nLeaveMovingNum;					// ���� �̵� ��
	int		m_nMovingPattern;					// �̵� ���� (����, �ٹ���, ��Ʈ, �񿵿�)
	int		m_nCurrentState;					// ���� State

	unsigned short	m_wSearchRange;				// ������ �þ�
	unsigned short	m_wDefaultSearchRange;		// ������ �⺻ �þ�

	bool	m_bLongRangeAttacked;				// ���Ÿ� ����? (���Ÿ� ���ݿ� ������ �þ߸� ���� Ÿ���� ã��)
	bool	m_bScout;							// ��ī�����ΰ�?
	bool	m_bAttacking;						// ���� ���ΰ�?

    unsigned short	m_wRespawnArea;				// ������ ���������� ����������

	// �ɼ� ���.
	bool	m_bAvoid;							// ���� ���ΰ�? (���ϱ� �ɼ��� ���� ���)
	bool	m_bAdminCmdSummon;					// Admin ������� ��ȯ�� �����ΰ�?

	// ��Ƽ ���� PID
	unsigned long m_dwPID;						// ���� ��Ƽ ���̵�

	static LONG ms_NormalBehaviorSendCount;
	static LONG ms_AttackBehaviorSendCount;
	static LONG ms_ReturnBehaviorSendCount;
	static LONG ms_EscapeBehaviorSendCount;
	static LONG ms_DeadBehaviorSendCount;
};


struct CMonster::MonsterCreateInfo
{
	Position		m_Pos;

	unsigned long	m_dwCID;
	unsigned long	m_dwPID;
	
	int				m_nKID;
	int				m_nMovingPattern;

	bool			m_bScout;
	unsigned short	m_wRespawnArea;

	MonsterCreateInfo()
		:	m_dwCID(0), m_nKID(0), m_dwPID(0), m_bScout(false), m_nMovingPattern(0), m_wRespawnArea(0)
	{
	}
};


Position CMonster::CalculateCoor(void)
{
	float fVel = (true == GetEnchantInfo().GetFlag(Skill::SpellID::Slow)) ? 
		m_MotionInfo.m_fVelocity / 4 : m_MotionInfo.m_fVelocity;

	return Position(m_CurrentPos.m_fPointX + fVel * cosf(m_MotionInfo.m_fDirection),
		m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ + fVel * sinf(m_MotionInfo.m_fDirection));
}

void CMonster::Attacked(void)
{
	m_bLongRangeAttacked = true;
	if (m_nCurrentState == STATE_ID_NORMAL || m_nCurrentState == STATE_ID_RETURN) 
	{
		m_lCurrentFrame = 0;
	}

	m_nCurrentState = CFSM::GetInstance().StateTransition(m_nCurrentState, INPUT_ID_ATTACKED_PLAYER);
}

unsigned char CMonster::GetOptionBaseNum(Item::EquipType::OptionType eType)
{
	switch (eType)
	{
		case Item::EquipType::STANDARD_OPTION:		return m_MonsterInfo.m_cStandardBaseNum;
		case Item::EquipType::OVER_OPTION:			return m_MonsterInfo.m_cOverBaseNum;
	}

	return 0;
}

CStatue* CMonster::DowncastToStatue(void)
{
	if (0 != this &&
		MonsterInfo::MIN_STATUE_KID <= m_MonsterInfo.m_dwKID &&
		MonsterInfo::MAX_STATUE_KID >= m_MonsterInfo.m_dwKID)
	{
		return reinterpret_cast<CStatue *>(this);
	}

	return NULL;
}


#endif