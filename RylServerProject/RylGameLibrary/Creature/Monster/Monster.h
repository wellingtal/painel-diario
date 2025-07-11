
#ifndef _CMONSTER_H_
#define _CMONSTER_H_

#include <Creature/Character/Character.h>
#include <Creature/Monster/FSM/FSM.h>

#include <Map/FieldMap/Cell.h>
#include <Skill/Spell/SpellKind.h>
#include <GameEvent/GameEventMgr.h>
#include <Community/Party/Party.h>

#include "MonsterMgr.h"


const unsigned long LongRange = 45;				// 장거리 공격에 의해 쫓아가는 거리.
const unsigned long OutsideSearchRange = 10;	// 영역 밖에서의 시야
const unsigned long ReturnRange = 20;			// 영역
const unsigned long MoveRange = 15;				// 영역 안에서의 최대 이동거리.

// 전방 참조
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

	struct MonsterCreateInfo;	// 몬스터 정보 구조체
	typedef MonsterCreateInfo *LPMonsterCreateInfo;

	CMonster();
	CMonster(MonsterCreateInfo& MonsterCreate, bool bAdminCmdSummon = false);
	virtual ~CMonster();

	// --------------------------------------------------------------------------------------------
	// 초기 설정

	bool InitMonster(Position& Pos, bool bDead = false);

	virtual bool GetMotion(unsigned long MotionID, MotionInfo& Motion);		// 몬스터 모션 정보를 얻음.

	// --------------------------------------------------------------------------------------------
	// 몬스터 행동 관련

	bool CheckPartyTarget();			// 몬스터 파티의 타겟을 체크해서 패턴을 변경하는가?
	virtual void SearchPlayer(void);	// 플레이어 검색

	bool IsOverlap(void);		// 다른 몬스터와 겹치는가 검색

	bool Process(void);			// 몬스터 행동 처리 - 내부에서 UpdateBehavior호출

	void UpdateBehavior(unsigned long dwTick);			// 몬스터 행동 업데이트

	virtual void NormalBehavior(unsigned long dwTick);	// 보통 상태
	virtual void AttackBehavior(unsigned long dwTick);	// 공격 상태
	virtual void ReturnBehavior(unsigned long dwTick);	// 귀환 상태
	virtual void EscapeBehavior(void);					// 도망 상태
	virtual void DeadBehavior(unsigned long dwTick);	// 죽은 상태
	
	// Action 관련 처리
	void AttackAction();
	void WalkAttackAction(float fVelocity=0.1f);
	void RunAction(float fDistance, float fDstX, float fDstZ);
	bool IsReturn();
	
	inline Position CalculateCoor(void);	// 이동 후의 좌표 계산

	virtual const int CalculateFixLevelGap(CAggresiveCreature *pDefender);

	virtual bool Attack(AtType attackType, unsigned char cDefenderNum, 
		CAggresiveCreature** ppDefenders, unsigned char* cDefenderJudges, unsigned short* wDefenderMPHeal);

	inline virtual void Attacked(void);				// 공격을 받음
	bool MultiAttack(void);							// 타겟을 찾아 공격 (내부에서 Attack함수를 호출)
	void CancelTarget(bool bSaveThreat = false);	// 타겟을 취소

	virtual bool Dead(CAggresiveCreature* pOffencer);	// 죽었을 때 호출되는 함수
	virtual EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL);

	virtual void Respawn(unsigned long dwTick);

	// --------------------------------------------------------------------------------------------
	// 파티 관련

	void			SetPID(unsigned long dwPID)		{ m_dwPID = dwPID;	}
	unsigned long	GetPID(void)					{ return m_dwPID;	}

	// --------------------------------------------------------------------------------------------
	// 패킷을 만들어서 보내는 함수들

	virtual LONG SendMove(unsigned short nAniNum);
	
	// --------------------------------------------------------------------------------------------
	// 기타 함수

	virtual Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError) { return NULL; }
	
	bool HasSkill(void);		// 스킬을 사용하는 몬스터인가?
	int GetUseSkillNum(void);	// 몇가지의 스킬을 가지고 있는가?

	inline unsigned char GetOptionBaseNum(Item::EquipType::OptionType eType);

	unsigned long GetDropRate(unsigned char nIndex);				
	unsigned long GetAwardItem(int nIndex)	{ return m_MonsterInfo.m_aryAwardItem[nIndex];	}

	inline CStatue* DowncastToStatue(void);

	//	몬스터 일련번호 얻기
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

	// 각종 상수들
	enum _Timing 
	{ 
		FPS = 30
	};

	MonsterInfo		m_MonsterInfo;
	Position		m_OriginalPosition;			// 몬스터의 초기 위치
	unsigned long	m_dwLastBehaviorTick;		// 마지막으로 행동한 시점의 Tick

    Broadcast2nd::CSerializeMonsterData     m_SerializeMonsterData;

	CAggresiveCreature*	m_lpTarget;				// 공격할 타겟

	long	m_lCurrentFrame;					// 현재 진행중인 Frame을 나타냄. 동작이 끝났는지를 확인하기 위해 삽입

	int		m_nNormalMovingDelay;				// 노말 상태의 이동 타이밍 간격 (주위 플레이어 수에 따라 유동적)
	int		m_nLeaveMovingNum;					// 남은 이동 수
	int		m_nMovingPattern;					// 이동 패턴 (영역, 붙박이, 루트, 비영역)
	int		m_nCurrentState;					// 현재 State

	unsigned short	m_wSearchRange;				// 몬스터의 시야
	unsigned short	m_wDefaultSearchRange;		// 몬스터의 기본 시야

	bool	m_bLongRangeAttacked;				// 원거리 공격? (원거리 공격에 맞으면 시야를 넓혀 타겟을 찾음)
	bool	m_bScout;							// 스카우터인가?
	bool	m_bAttacking;						// 공격 중인가?

    unsigned short	m_wRespawnArea;				// 리스폰 지점에서의 랜덤영역값

	// 옵션 요소.
	bool	m_bAvoid;							// 피할 턴인가? (피하기 옵션이 켜진 경우)
	bool	m_bAdminCmdSummon;					// Admin 명령으로 소환한 몬스터인가?

	// 파티 관련 PID
	unsigned long m_dwPID;						// 몬스터 파티 아이디

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