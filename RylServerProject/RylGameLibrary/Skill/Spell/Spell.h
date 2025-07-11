#ifndef _CSPELL_H_
#define _CSPELL_H_

#include <climits>

#include <Skill/SkillStructure.h>

#include "SpellKind.h"


//	로그 레벨 : x <--> (void*)0
//	MAXLEVEL  : 세부 로그
//	ERRLEVEL  : 에러 로그
//	TESTLEVEL : 테스트 로그

#define SPLOG_MAXLEVEL(x)	(void*)0
#define SPLOG_ERRLEVEL(x)	(void*)0
#define SPLOG_TESTLEVEL(x)	(void*)0

//	전방 참조
class CAggresiveCreature;

class CSpell
{
public:

	enum EnchantError
	{
		NO_ENCHANT_ERROR				= 0,
		ENCHNAT_FAIL_ALREADY_AFFECTED	= 1,
		ENCHANT_FAIL_BY_ENEMY_ENCHANT	= 2,
		ENCHANT_FAIL_BY_RESIST			= 3
	};

	enum Const
	{
		INFINITE_DURATION	= ULONG_MAX,
		REALM_TIME			= 7200,
		REDUCE_MANA_TURN	= 5,
		MAX_AFFECTED		= 10
	};

    enum OperateFlag
    {
        DO_NOT_SEND         =   1 << 0
    };

    struct Spell_Info
    {
        const Skill::ProtoType&     m_SkillProtoType;
        CAggresiveCreature*         m_lpCaster;

		unsigned long	            m_dwDurationSec;
		unsigned short              m_wSpellID;
        unsigned short				m_wSpellLevel;
		unsigned char				m_cSpellType;

		unsigned char				m_cPadding;
		unsigned char				m_cSkillLockCount;
		unsigned char				m_cSkillLevel;

        Spell_Info(const Skill::ProtoType& SkillProtoType, CAggresiveCreature* lpCaster, 
            unsigned char cSpellType, unsigned short wSpellID, unsigned short wSpellLevel, unsigned long dwDurationSec, 
			unsigned char cSkillLockCount = 0, unsigned char cSkillLevel = 0);
    };

	virtual ~CSpell() { Destroy(); }

	//	Caster관련
	void SetCaster(CAggresiveCreature* pCaster);	//	Caster가 바뀔 때 사용한다.
	inline CAggresiveCreature* GetCaster(void) { return m_pCaster; }

	//	Affected관련. 주문의 Activate, Deactivate와 상호 참조 해제등의 역할을 한다.
	bool AddAffected(CAggresiveCreature* lpAffected, unsigned short& wError);
	bool RemoveAffected(CAggresiveCreature* pRemoved);
	void ClearAffected(void);

	//	Caster, Affected정보를 전부 제거.
	void ClearAll(void);

	//	거리를 체크해서 주문 효과를 적용 및 해지.
	void CheckRange(void);
	
	// 기타
	inline bool IsActivate(CAggresiveCreature* lpAffected);
	inline bool IsValid(void) const { return (0 != m_cAffectedNum); }

	inline unsigned short GetSpellID(void) const		{ return m_wSpellID;			}
	inline unsigned short GetSpellLevel(void) const		{ return m_wSpellLevel;			}
	inline unsigned long GetDurationSec(void) const		{ return m_dwDurationSec;		}
	inline unsigned char GetSpellType(void) const		{ return m_cSpellType;			}
	inline unsigned char GetSkillLockCount(void) const	{ return m_cSkillLockCount;		}
	inline unsigned char GetSkillLevel(void) const		{ return m_cSkillLevel;			}
    
    const Skill::SpellTarget::Type GetSpellTarget(void);
    inline const Skill::Type::SkillType GetSkillType(void) const { return m_eSkillType; }
    inline const bool IsPartySkill(void) const { return m_ProtoType.m_eTargetType == Skill::Target::PARTY; }
    
	inline unsigned short DecreaseSkillLevel(unsigned short usDecreaseAmount);
	
	// 주문 효과를 전부 켜고 끈다. 
	void Enable(unsigned long dwOperateFlag);
	void Disable(unsigned long dwOperateFlag);

protected:

	// 상속 받아야만 생성할 수 있다.
    CSpell(Spell_Info& spell_Info, Skill::Type::SkillType eSpellType);

	bool Operate(void);         // 주문 시간 체크. 주문을 적용.
	void Destroy(void);     // ClearAll과 같은 역할을 하나, 소멸자에서 한번만 호출한다.
							// 유도 클래스의 소멸자도 반드시 실행해 주어야 한다.
	
	// --------------------------------------------------------------------------------------------
	// 스킬 Opeartion 관련. Override 필요.

    inline virtual void Operate(CAggresiveCreature* lpAffected) { };

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);

	// Activate 플래그 관련
	inline bool IsActivate(unsigned char cAffectedIndex);
    inline void SetActivate(unsigned char cAffectedIndex);	
	inline void SetDeactivate(unsigned char cAffectedIndex);
	inline void RemoveIndex(unsigned char cAffectedIndex);
	unsigned char GetAffectedIndex(CAggresiveCreature* lpAffected);

	// Internal 플래그 관련
	inline bool IsEnabled(void);
	inline bool SetEnableStatus(bool bStatus);

	// --------------------------------------------------------------------------------------------
	// Send 관련

	void SendSpellInfo(CAggresiveCreature* lpAffected, unsigned char cSpellType, 
        unsigned short nEnchantLevel, unsigned long dwEnchantTime, bool bOnOff);


	enum InternalFlags
	{
		DISABLED		=	(  1 <<  0),	// 주문이 Disabled되었는지 여부를 나타냄.
		DESTROYED		=	(  1 <<  1)		// 주문이 Destroy되었는지 여부룰 나타냄.		
	};	

	CSpell*		        		m_pNextSpell;			// 주문 리스트를 유지하기 위한 포인터.
    const Skill::ProtoType&     m_ProtoType;

	CAggresiveCreature*		m_pAffected[MAX_AFFECTED];	// 주문에 의해 영향을 받고 있는 캐릭터.
	CAggresiveCreature*		m_pCaster;					// 주문 시전자.
	
	Skill::Type::SkillType	m_eSkillType;				// See Namespace Type

	unsigned long	    m_dwActivateStatus;	// 주문의 Activate상태를 가지고 있는 Flag.

	unsigned short	    m_wSpellID;			// 주문 ID
	unsigned short	    m_wSpellLevel;		// 주문의 Level

	unsigned long	    m_dwDurationSec;	// 주문 지속시간
	unsigned char		m_cSpellType;		// 주문의 타입 (마법형/물리형)
	unsigned char	    m_cAffectedNum;		// 주문에 의해 영향을 받는 사람의 수
	unsigned char	    m_cInternalFlags;	// 8bit Internal플래그

	unsigned char		m_cSkillLockCount;	// 주문을 건 스킬의 락카운트
	unsigned char		m_cSkillLevel;		// 주문을 건 스킬의 레벨

	friend class        CSpellMgr;			// Caster와 Affected의 상호참조 관리를 외부로 노출시키지 않기 위함.
	friend class        CGlobalSpellMgr;	// CGlobalSpellMgr에서 m_pNextSpell및 Protected함수를 사용하기 위함.
};


inline CSpell::Spell_Info::Spell_Info(const Skill::ProtoType& SkillProtoType, CAggresiveCreature* lpCaster, 
									  unsigned char cSpellType, unsigned short wSpellID, unsigned short wSpellLevel, unsigned long dwDurationSec,
									  unsigned char cSkillLockCount, unsigned char cSkillLevel)
									  :	m_SkillProtoType(SkillProtoType), m_lpCaster(lpCaster), 
										m_cSpellType(cSpellType), m_wSpellID(wSpellID), m_wSpellLevel(wSpellLevel), m_dwDurationSec(dwDurationSec),
										m_cSkillLockCount(cSkillLockCount), m_cSkillLevel(cSkillLevel)
{
}

inline unsigned short CSpell::DecreaseSkillLevel(unsigned short usDecreaseAmount)
{
	if (m_wSpellLevel < usDecreaseAmount) 
	{ 
		m_wSpellLevel = 0; 
		return m_wSpellLevel; 
	}

	m_wSpellLevel -= usDecreaseAmount;
	return m_wSpellLevel;
}

inline bool CSpell::IsEnabled(void)
{
	return (DISABLED != (DISABLED & m_cInternalFlags));
}

inline bool CSpell::IsActivate(CAggresiveCreature* lpAffected)
{
	CAggresiveCreature** ppBegin	= m_pAffected;
	CAggresiveCreature** ppPastEnd	= m_pAffected + m_cAffectedNum;
	unsigned char cIndex = 0;

	for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
	{
		if (*ppBegin == lpAffected)
		{
			return IsActivate(cIndex);
		}
	}

	return false;
}

inline bool CSpell::IsActivate(unsigned char cAffectedIndex)
{
	const unsigned long dwIndexValue = (1 << cAffectedIndex);
	return (dwIndexValue == (m_dwActivateStatus & dwIndexValue));
}

inline void CSpell::SetActivate(unsigned char cAffectedIndex)
{
	m_dwActivateStatus |= (1 << cAffectedIndex);
}

inline void CSpell::SetDeactivate(unsigned char cAffectedIndex)
{
	m_dwActivateStatus &= ~(1 << cAffectedIndex);
}

inline void CSpell::RemoveIndex(unsigned char cAffectedIndex)
{
	m_dwActivateStatus = 
		(((ULONG_MAX << (cAffectedIndex + 1)) & m_dwActivateStatus) >> 1) |	// 인덱스 왼쪽 비트열(31~nIndex)
		(((1 << cAffectedIndex) - 1) & m_dwActivateStatus);					// 인덱스 오른쪽 비트열(nIndex~0)
}


#endif