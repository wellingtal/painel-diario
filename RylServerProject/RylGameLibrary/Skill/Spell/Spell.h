#ifndef _CSPELL_H_
#define _CSPELL_H_

#include <climits>

#include <Skill/SkillStructure.h>

#include "SpellKind.h"


//	�α� ���� : x <--> (void*)0
//	MAXLEVEL  : ���� �α�
//	ERRLEVEL  : ���� �α�
//	TESTLEVEL : �׽�Ʈ �α�

#define SPLOG_MAXLEVEL(x)	(void*)0
#define SPLOG_ERRLEVEL(x)	(void*)0
#define SPLOG_TESTLEVEL(x)	(void*)0

//	���� ����
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

	//	Caster����
	void SetCaster(CAggresiveCreature* pCaster);	//	Caster�� �ٲ� �� ����Ѵ�.
	inline CAggresiveCreature* GetCaster(void) { return m_pCaster; }

	//	Affected����. �ֹ��� Activate, Deactivate�� ��ȣ ���� �������� ������ �Ѵ�.
	bool AddAffected(CAggresiveCreature* lpAffected, unsigned short& wError);
	bool RemoveAffected(CAggresiveCreature* pRemoved);
	void ClearAffected(void);

	//	Caster, Affected������ ���� ����.
	void ClearAll(void);

	//	�Ÿ��� üũ�ؼ� �ֹ� ȿ���� ���� �� ����.
	void CheckRange(void);
	
	// ��Ÿ
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
	
	// �ֹ� ȿ���� ���� �Ѱ� ����. 
	void Enable(unsigned long dwOperateFlag);
	void Disable(unsigned long dwOperateFlag);

protected:

	// ��� �޾ƾ߸� ������ �� �ִ�.
    CSpell(Spell_Info& spell_Info, Skill::Type::SkillType eSpellType);

	bool Operate(void);         // �ֹ� �ð� üũ. �ֹ��� ����.
	void Destroy(void);     // ClearAll�� ���� ������ �ϳ�, �Ҹ��ڿ��� �ѹ��� ȣ���Ѵ�.
							// ���� Ŭ������ �Ҹ��ڵ� �ݵ�� ������ �־�� �Ѵ�.
	
	// --------------------------------------------------------------------------------------------
	// ��ų Opeartion ����. Override �ʿ�.

    inline virtual void Operate(CAggresiveCreature* lpAffected) { };

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);

	// Activate �÷��� ����
	inline bool IsActivate(unsigned char cAffectedIndex);
    inline void SetActivate(unsigned char cAffectedIndex);	
	inline void SetDeactivate(unsigned char cAffectedIndex);
	inline void RemoveIndex(unsigned char cAffectedIndex);
	unsigned char GetAffectedIndex(CAggresiveCreature* lpAffected);

	// Internal �÷��� ����
	inline bool IsEnabled(void);
	inline bool SetEnableStatus(bool bStatus);

	// --------------------------------------------------------------------------------------------
	// Send ����

	void SendSpellInfo(CAggresiveCreature* lpAffected, unsigned char cSpellType, 
        unsigned short nEnchantLevel, unsigned long dwEnchantTime, bool bOnOff);


	enum InternalFlags
	{
		DISABLED		=	(  1 <<  0),	// �ֹ��� Disabled�Ǿ����� ���θ� ��Ÿ��.
		DESTROYED		=	(  1 <<  1)		// �ֹ��� Destroy�Ǿ����� ���η� ��Ÿ��.		
	};	

	CSpell*		        		m_pNextSpell;			// �ֹ� ����Ʈ�� �����ϱ� ���� ������.
    const Skill::ProtoType&     m_ProtoType;

	CAggresiveCreature*		m_pAffected[MAX_AFFECTED];	// �ֹ��� ���� ������ �ް� �ִ� ĳ����.
	CAggresiveCreature*		m_pCaster;					// �ֹ� ������.
	
	Skill::Type::SkillType	m_eSkillType;				// See Namespace Type

	unsigned long	    m_dwActivateStatus;	// �ֹ��� Activate���¸� ������ �ִ� Flag.

	unsigned short	    m_wSpellID;			// �ֹ� ID
	unsigned short	    m_wSpellLevel;		// �ֹ��� Level

	unsigned long	    m_dwDurationSec;	// �ֹ� ���ӽð�
	unsigned char		m_cSpellType;		// �ֹ��� Ÿ�� (������/������)
	unsigned char	    m_cAffectedNum;		// �ֹ��� ���� ������ �޴� ����� ��
	unsigned char	    m_cInternalFlags;	// 8bit Internal�÷���

	unsigned char		m_cSkillLockCount;	// �ֹ��� �� ��ų�� ��ī��Ʈ
	unsigned char		m_cSkillLevel;		// �ֹ��� �� ��ų�� ����

	friend class        CSpellMgr;			// Caster�� Affected�� ��ȣ���� ������ �ܺη� �����Ű�� �ʱ� ����.
	friend class        CGlobalSpellMgr;	// CGlobalSpellMgr���� m_pNextSpell�� Protected�Լ��� ����ϱ� ����.
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
		(((ULONG_MAX << (cAffectedIndex + 1)) & m_dwActivateStatus) >> 1) |	// �ε��� ���� ��Ʈ��(31~nIndex)
		(((1 << cAffectedIndex) - 1) & m_dwActivateStatus);					// �ε��� ������ ��Ʈ��(nIndex~0)
}


#endif