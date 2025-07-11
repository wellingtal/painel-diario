
#ifndef _CAFFECTED_H_
#define _CAFFECTED_H_

#include "Spell.h"


//	전방 참조
class CAggresiveCreature;

class CAffectedSpell
{
public:

	CAffectedSpell() : m_cInternalFlag(0), m_cChantNum(0), m_cEnchantNum(0) 
	{ 
		int i;
		for(i = 0; i < MAX_CHANT; ++i)
			m_pChant[i] = NULL;

		for(i = 0; i < MAX_ENCHANT; ++i)
			m_pEnchant[i] = NULL;	//	영향을 받고 있는 Enchant주문
	}
	~CAffectedSpell() { ClearAll(); }

	void SetOwner(CAggresiveCreature* pOwner) { m_pOwner = pOwner; }	

	bool Add(CSpell* pSpell, unsigned short& wError);
	bool Remove(CSpell* pSpell);

	void ApplyPartyChant(CAggresiveCreature* pAffected);
	void ApplyEnchant(CAggresiveCreature* pAffected);

	void RemoveChantByCaster(CAggresiveCreature* pCaster);
	void RemoveEnchantByCaster(CAggresiveCreature* pCaster);

	void RemoveChantBySpellType(unsigned char cSpellType);
	bool RemoveEnchantBySpellType(unsigned char cSpellType);

	bool IsSpellOfEnemyCharacter(void);
	bool IsSpellThisTargetType(Skill::SpellTarget::Type eTargetType);
	unsigned char Disenchant(Skill::SpellType::Type eSpellType, Skill::SpellTarget::Type eTargetType, 
		Skill::Disenchant::Type eDisenchantType, unsigned short usSkillLevel, unsigned char cNum);

	void ClearChant();	
	void ClearEnchant();

	void EnableChant(unsigned long dwOperateFlag = 0);	
	void EnableEnchant(unsigned long dwOperateFlag = 0);

	void DisableChant(unsigned long dwOperateFlag = 0);
	void DisableEnchant(unsigned long dwOperateFlag = 0);

	inline void ClearAll();
	inline void EnableAll(unsigned long dwOperateFlag = 0);
	inline void DisableAll(unsigned long dwOperateFlag = 0);

	CSpell*	GetSpell(unsigned short usSpellID);


	CSpell*	GetEnchant(int Index);
	int		GetEnchantNum() { return m_cEnchantNum; }


	SPELL GetSpellInfo(BOOL bDead = FALSE);

	enum
	{
		MAX_CHANT			= 10,
		// edith 2008.05.06 인첸트 최대 개수
		MAX_ENCHANT			= 30
	};

protected:

	bool RemoveOverlappedSpell(CSpell* pSpell);

	CSpell*		m_pChant[MAX_CHANT];
	CSpell*		m_pEnchant[MAX_ENCHANT];	//	영향을 받고 있는 Enchant주문

	CAggresiveCreature*	m_pOwner;			//	영향을 받고 있는 캐릭터. 이 개체의 주인.

	unsigned char	m_cInternalFlag;
	unsigned char	m_cChantNum;
	unsigned char	m_cEnchantNum;
};

inline void CAffectedSpell::ClearAll()
{
	ClearChant();
	ClearEnchant();
}

inline void CAffectedSpell::EnableAll(unsigned long dwOperateFlag)
{
	EnableChant(dwOperateFlag);
	EnableEnchant(dwOperateFlag);
}

inline void CAffectedSpell::DisableAll(unsigned long dwOperateFlag)
{
	DisableChant(dwOperateFlag);
	DisableEnchant(dwOperateFlag);
}


#endif