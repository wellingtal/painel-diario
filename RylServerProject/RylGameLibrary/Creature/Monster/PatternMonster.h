
#ifndef _PATTERN_MONSTER_H_
#define _PATTERN_MONSTER_H_

#include "Monster.h"

// -------------------------------------------------------------------------------------
// ��ȯ��

class CSummonMonster : public CMonster
{
public:

	CSummonMonster();
	CSummonMonster(MonsterCreateInfo& MonsterCreate, CCharacter* lpMaster);

	virtual ~CSummonMonster();

	void NormalBehavior(unsigned long dwTick);
	void AttackBehavior(unsigned long dwTick);
	void ReturnBehavior(unsigned long dwTick);
	void DeadBehavior(unsigned long dwTick);

	void GuardMe(CAggresiveCreature* lpTarget, unsigned short wThreat);
	void AttackCmd(CAggresiveCreature* lpTarget, unsigned short wThreat);
	void Attacked(void);
	bool Attack(AtType attackType, unsigned char cDefenderNum, 
		CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal);

	bool Dead(CAggresiveCreature* pOffencer);	
	EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget);
	bool GetMotion(unsigned long MotionID, MotionInfo &Motion);

	void SetGuard(bool bGuard)		{ m_bGuard = bGuard; }
	CCharacter* GetMaster(void)		{ return m_lpMaster; }

	unsigned char GetNation() const
	{ 
		if (NULL == m_lpMaster)
		{
			return Creature::STATELESS;
		}

		return m_lpMaster->GetNation();	
	}


protected:

	enum 
	{ 
		SUPPORT_DISTANCE	= 3,
		SUMMON_TIME			= 4 * FPS
	};

	CCharacter*		m_lpMaster;

	bool			m_bGuard;
	bool            m_bPadding[3];
};


// -------------------------------------------------------------------------------------
// ���� ����

class CStatue : public CMonster
{
public:

	enum
	{
		MAX_HP_UPDATE_COUNT		= 10,			// 10���� �ѹ� HP ������Ʈ

		STATUE_ATTACKED_TIME	= 3000,			// ���� �������� 3�ʴ� 1���� ��Ŷ ����
		STATUE_LOADING_TIME		= 300000,		// ���� ��ȯ �ð�(5��)
		BG_STATUE_LOADING_TIME	= 120000,		// ��Ʋ �׶��� ������ ���� ��ȯ �ð� (2��)
		UPDATE_INFO_CYCLE		= 150,			// ���� ���� ������Ʈ �ֱ�(5��)

		BONUS_DISTANCE			= 100,			// ����޴� ���ʽ��� ���� �� �ִ� �Ÿ�
		BONUS_MILEAGE			= 100,			// �ѹ��� ��� ����޴� ���ʽ��� ��
		BONUS_TIME				= 12			// ����޴� ���ʽ��� ��� �ֱ�(1��)
	};

	CStatue();
	CStatue(MonsterCreateInfo& MonsterCreate, CStatue* lpParent);

	virtual ~CStatue();

	void NormalBehavior(unsigned long dwTick);
	void AttackBehavior(unsigned long dwTick);
	void ReturnBehavior(unsigned long dwTick);
	void DeadBehavior(unsigned long dwTick);

	void GiveMileage(char cGiveType);

	virtual bool RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue);
	bool Dead(CAggresiveCreature* pOffencer);
	bool Rest(void);							// ��Ʋ �׶��忡 ������ �ϳ��� ������ �ʵ��� �����ϴ� ó��
												// Dead �� �����ϳ� ���� ������ ��ȯ���� �ʴ´�.

	EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget);
	
	bool CreateLinkStatue(unsigned short wKind);
	CStatue* GetLinkStatue(unsigned short wKind);

	Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError);

	Position& GetOriginalPos() { return m_OriginalPosition;	}


	// ������ ���� ��æƮ ȿ�� ����
	unsigned char GetRealmStatueEnchantType() const		{ return m_MonsterInfo.m_cStatueEffectType;		}

	// ������ ���� ��æƮ ȿ�� �ۼ�Ʈ
	unsigned char GetRealmStatueEnchantPercent() const	{ return m_MonsterInfo.m_cStatueEffectPercent;	}

	// ������ ���� ������ ���� ����
	bool EnableRespawn() const	{ return m_MonsterInfo.m_bStatueRespawnEnable;	}

	// DBAgentServer �� KID, HP ������Ʈ ��Ŷ ������ �Լ�
	bool SendKIDUpdate();
	bool SendHPUpdate(bool bForce=false);

	// Client �� Attacked ���� ������
	void SendAttacked();

protected:

	unsigned long	m_dwDuration;			
	unsigned short	m_wBonusTurn;
	unsigned char	m_cHPUpdateCount;
	unsigned long	m_dwLastAttackedTime;

	CStatue*		m_lpParent;				// ������ �ʱ� ���� (���� ������ 5���� ������ �� ��Ʈ�� ��ũ�Ǿ� �ִ�.)
	CNPC*			m_lpLinkNPC;			// �޴� ���� ������ ���� ��ũ�� NPC
};

class CStatueInfo
{
public:

    CStatueInfo(CStatue* lpStatue, bool bBonusTurn, char cGiveType);
    bool operator () (CCharacter* lpCharacter);

private:

    CStatue*	m_lpStatue;
	bool		m_bBonusTurn;
	char		m_cGiveType;
};

// -------------------------------------------------------------------------------------
// GM�� �ູ
// edith 2010.01.16 GM�� �ູ�� �޴� ��� ���� 1�п� 1~5���� �޴� �ο�
// StatueInfo �� ���� ��Ȱ�� �ϱ� ������ �̰��� ������.
class CBlessMileageInfo
{
public:

    CBlessMileageInfo(char cBlessType);
    bool operator () (CCharacter* lpCharacter);

private:
	char		m_cBlessType;
	char		m_cAddMileage;
};


// -------------------------------------------------------------------------------------
// ���� ����

class CGatherMonster : public CMonster
{
public:
	CGatherMonster();
	CGatherMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CGatherMonster();

	virtual void NormalBehavior(unsigned long dwTick);
	virtual void AttackBehavior(unsigned long dwTick);
	virtual void ReturnBehavior(unsigned long dwTick);
};

// -------------------------------------------------------------------------------------
// ������Ʈ ���� (��, ����, ������)

class CObjectMonster : public CMonster
{
public:
	CObjectMonster();
	CObjectMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CObjectMonster();

	virtual void NormalBehavior(unsigned long dwTick);
	virtual void AttackBehavior(unsigned long dwTick);
	virtual void ReturnBehavior(unsigned long dwTick);
};


// -------------------------------------------------------------------------------------
// ���� ����

class CSkillMonster : public CMonster
{
public:

	virtual ~CSkillMonster();

	char GetConsumeMPCount(void)			{ return m_cConsumeMPCount;				}

	bool Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal);
	
	virtual void NormalBehavior(unsigned long dwTick);
	virtual void AttackBehavior(unsigned long dwTick);

	// Action ���� ó��
	void CastingAttackAction();
	void SkillAttackAction();
	
	enum SkillPattern
	{
		A_SKILL		= 0,
		B_SKILL		= 1,
		C_SKILL		= 2,
		D_SKILL		= 3,
		BOSS_CHANT_SKILL	= 4,

		MAX_SKILL_PATTERN	= 5
	};

	enum EnemyFindType
	{
		TYPE_LV			= 0,
		TYPE_HP			= 1,

		TYPE_HIGHEST	= 0,
		TYPE_LOWEST		= 1
	};

	enum Const
	{
		USE_SKILL_MIN_LEVEL	= 4,
		USE_SKILL_LEVEL		= 5		// 4���� ��ų���� ���� ��ų�� ���� ���Ͱ� ����ϴ� ���� ��ų ����
									// (1�ܰ� ��ų�� ����ϴ� ���ʹ� ����)
	};

public:
	virtual LONG SendMove(unsigned short nAniNum);

protected:

	CSkillMonster();
	CSkillMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	bool Dead(CAggresiveCreature* pOffencer);		// �׾��� �� ȣ��Ǵ� �Լ�

	// �ڽ��� �ֺ����� ��ũ��(32m) �ݰ泻�� �ִ� �Ʊ� ��Ƽ���߿� ���� ������ ���� ���͸� ����
	CAggresiveCreature*	FindHighestLVMember();

	// �ش� ��ų�� ���� �ȿ� �ִ� �༮�� ��
	// ��æƮ �ο� ��ų�� �ɸ��� ���� ���� ������ ���� ���͸� ã�Ƽ� �������ִ� �Լ�
	CAggresiveCreature*	FindHighestLVMember(const AtType& attackType);

	// ��Ƽ�� �߿��� HP �� fRemain �ۼ�Ʈ ���Ϸ� �����ִ� ��Ƽ���� �ִ��� �˻�
	// bRegin �� TRUE �̸� �ش� ������ ���� �ȿ� �ִ� ��Ƽ�� �߿��� ã�´�.
	CAggresiveCreature* FindLowestHPMember(const AtType& attackType, bool bRegin, float fRemainHP = 0.5f);


	// ���Ͽ� �´� ���� ã�� �Լ�
	// Type1 : LV, HP
	// Type2 : Highest, Lowest
	CAggresiveCreature* FindEnemy(const AtType& attackType, unsigned char cType1, unsigned char cType2);


	float	CalculateDistance(CAggresiveCreature* pTarget);
			
	virtual bool SkillAttack(void) { return false; }

	// �Ϲ����� ��ų ��� �Լ�
	bool UseSkill(AtType attackType, CAggresiveCreature** ppDefenders, char cSkillPattern);	

	// ĳ���� �׼��� ������ ������ ��ų ��� �Լ�
	bool UseCastedSkill(void);																
    
	unsigned long	m_lastCastTime[MAX_SKILL_PATTERN];	// ���������� ��ų�� ����� �ð�
	char			m_cConsumeMPCount;					// MP �Ҹ���� ���� ī��Ʈ �� ����

	// Casting
	bool			m_bCasting;							// ���� ĳ���� ���ΰ�?
	int				m_nCastingCount;					// ĳ���� ���� Ƚ��
	AtType			m_attackType;						// ĳ������ ������ ���� ��ų
	unsigned long	m_dwTargetCID;						// ��ų�� ��� CID (�װų� ���������� �ֱ⶧���� CID�� üũ)
	char			m_cSkillPattern;					// m_lastCastTime �� �����ϱ� ���� ����� ��ų�� type index (m_skillID �� Index �̴�.)
};


class CDefenderMonster : public CSkillMonster
{
public:

	CDefenderMonster();
	CDefenderMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CDefenderMonster();

protected:

	bool SkillAttack(void);
};


class CWarriorMonster : public CSkillMonster
{
public:

	CWarriorMonster();
	CWarriorMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CWarriorMonster();

protected:

	bool SkillAttack(void);
};


class CAcolyteMonster : public CSkillMonster
{
public:

	enum Distance
	{
		NEAR_DIST_TO_TARGET = 10,
		FAR_DIST_TO_TARGET = 15
	};

	CAcolyteMonster();
	CAcolyteMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CAcolyteMonster();

	void AttackBehavior(unsigned long dwTick);

protected:

	bool SkillAttack(void);
};


class CMageMonster : public CSkillMonster
{
public:

	enum Distance
	{
		NEAR_DIST_TO_TARGET = 10,
		FAR_DIST_TO_TARGET = 15
	};

	CMageMonster();
	CMageMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CMageMonster();

	void AttackBehavior(unsigned long dwTick);
	
protected:

	bool SkillAttack(void);
};

class CBossMonster : public CSkillMonster
{
public:

	CBossMonster();
	CBossMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CBossMonster();

protected:

	bool SkillAttack(void);
};

class CChiefMonster : public CSkillMonster
{
public:

	CChiefMonster();
	CChiefMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CChiefMonster();

protected:

	bool SkillAttack(void);
};

class CNamedMonster : public CSkillMonster
{
public:

	CNamedMonster();
	CNamedMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CNamedMonster();

	void Respawn(unsigned long dwTick);

protected:

	bool SkillAttack(void);
	
	Position		m_RespawnPosition;	// ������ �߽� ��ǥ
};

class CGuardMonster : public CSkillMonster
{
public:

	CGuardMonster();
	CGuardMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	virtual ~CGuardMonster();

	void SearchPlayer();

protected:

	bool SkillAttack(void);
};


#endif