
#ifndef _PATTERN_MONSTER_H_
#define _PATTERN_MONSTER_H_

#include "Monster.h"

// -------------------------------------------------------------------------------------
// 소환수

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
// 명예의 석상

class CStatue : public CMonster
{
public:

	enum
	{
		MAX_HP_UPDATE_COUNT		= 10,			// 10번에 한번 HP 업데이트

		STATUE_ATTACKED_TIME	= 3000,			// 공격 당했을때 3초당 1번씩 패킷 전송
		STATUE_LOADING_TIME		= 300000,		// 석상 소환 시간(5분)
		BG_STATUE_LOADING_TIME	= 120000,		// 배틀 그라운드 서버군 석상 소환 시간 (2분)
		UPDATE_INFO_CYCLE		= 150,			// 석상 정보 업데이트 주기(5초)

		BONUS_DISTANCE			= 100,			// 공헌메달 보너스를 받을 수 있는 거리
		BONUS_MILEAGE			= 100,			// 한번에 얻는 공헌메달 보너스의 양
		BONUS_TIME				= 12			// 공헌메달 보너스를 얻는 주기(1분)
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
	bool Rest(void);							// 배틀 그라운드에 석상이 하나도 나오지 않도록 쉬게하는 처리
												// Dead 와 동일하나 다음 석상을 소환하지 않는다.

	EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget);
	
	bool CreateLinkStatue(unsigned short wKind);
	CStatue* GetLinkStatue(unsigned short wKind);

	Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError);

	Position& GetOriginalPos() { return m_OriginalPosition;	}


	// 국가전 석상 인챈트 효과 종류
	unsigned char GetRealmStatueEnchantType() const		{ return m_MonsterInfo.m_cStatueEffectType;		}

	// 국가전 석상 인챈트 효과 퍼센트
	unsigned char GetRealmStatueEnchantPercent() const	{ return m_MonsterInfo.m_cStatueEffectPercent;	}

	// 국가전 석상 리스폰 가능 여부
	bool EnableRespawn() const	{ return m_MonsterInfo.m_bStatueRespawnEnable;	}

	// DBAgentServer 로 KID, HP 업데이트 패킷 보내는 함수
	bool SendKIDUpdate();
	bool SendHPUpdate(bool bForce=false);

	// Client 로 Attacked 정보 보내기
	void SendAttacked();

protected:

	unsigned long	m_dwDuration;			
	unsigned short	m_wBonusTurn;
	unsigned char	m_cHPUpdateCount;
	unsigned long	m_dwLastAttackedTime;

	CStatue*		m_lpParent;				// 석상의 초기 상태 (명예의 석상은 5가지 석상이 한 세트로 링크되어 있다.)
	CNPC*			m_lpLinkNPC;			// 메달 상점 역할을 위해 링크된 NPC
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
// GM의 축복
// edith 2010.01.16 GM의 축복의 메달 기능 구현 1분에 1~5개씩 메달 부여
// StatueInfo 와 같은 역활을 하기 때문에 이곳에 구현함.
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
// 수집 몬스터

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
// 오브젝트 몬스터 (꽃, 나무, 버섯류)

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
// 패턴 몬스터

class CSkillMonster : public CMonster
{
public:

	virtual ~CSkillMonster();

	char GetConsumeMPCount(void)			{ return m_cConsumeMPCount;				}

	bool Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal);
	
	virtual void NormalBehavior(unsigned long dwTick);
	virtual void AttackBehavior(unsigned long dwTick);

	// Action 관련 처리
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
		USE_SKILL_LEVEL		= 5		// 4레벨 스킬보다 낮은 스킬을 가진 몬스터가 사용하는 고정 스킬 레벨
									// (1단계 스킬을 사용하는 몬스터는 제외)
	};

public:
	virtual LONG SendMove(unsigned short nAniNum);

protected:

	CSkillMonster();
	CSkillMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);

	bool Dead(CAggresiveCreature* pOffencer);		// 죽었을 때 호출되는 함수

	// 자신의 주변에서 셀크기(32m) 반경내에 있는 아군 파티원중에 가장 레벨이 높은 몬스터를 리턴
	CAggresiveCreature*	FindHighestLVMember();

	// 해당 스킬의 범위 안에 있는 녀석들 중
	// 인챈트 부여 스킬이 걸리지 않은 가장 레벨이 높은 몬스터를 찾아서 리턴해주는 함수
	CAggresiveCreature*	FindHighestLVMember(const AtType& attackType);

	// 파티원 중에서 HP 가 fRemain 퍼센트 이하로 남아있는 파티원이 있는지 검색
	// bRegin 이 TRUE 이면 해당 공격의 범위 안에 있는 파티원 중에서 찾는다.
	CAggresiveCreature* FindLowestHPMember(const AtType& attackType, bool bRegin, float fRemainHP = 0.5f);


	// 패턴에 맞는 적을 찾는 함수
	// Type1 : LV, HP
	// Type2 : Highest, Lowest
	CAggresiveCreature* FindEnemy(const AtType& attackType, unsigned char cType1, unsigned char cType2);


	float	CalculateDistance(CAggresiveCreature* pTarget);
			
	virtual bool SkillAttack(void) { return false; }

	// 일반적인 스킬 사용 함수
	bool UseSkill(AtType attackType, CAggresiveCreature** ppDefenders, char cSkillPattern);	

	// 캐스팅 액션이 끝난후 나가는 스킬 사용 함수
	bool UseCastedSkill(void);																
    
	unsigned long	m_lastCastTime[MAX_SKILL_PATTERN];	// 마지막으로 스킬을 사용한 시간
	char			m_cConsumeMPCount;					// MP 소모까지 남은 카운트 수 저장

	// Casting
	bool			m_bCasting;							// 마법 캐스팅 중인가?
	int				m_nCastingCount;					// 캐스팅 동작 횟수
	AtType			m_attackType;						// 캐스팅이 끝난후 나갈 스킬
	unsigned long	m_dwTargetCID;						// 스킬의 대상 CID (죽거나 나가버릴수 있기때문에 CID로 체크)
	char			m_cSkillPattern;					// m_lastCastTime 을 갱신하기 위해 사용한 스킬의 type index (m_skillID 의 Index 이다.)
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
	
	Position		m_RespawnPosition;	// 리스폰 중심 좌표
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