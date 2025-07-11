#ifndef _CCASTING_H_
#define _CCASTING_H_

//	전방 참조
class CSpell;
class CAggresiveCreature;

class CCastingSpell
{
public:

	CCastingSpell() : m_pOwner(NULL), m_usInternalFlag(0), m_cEnchantNum(0), m_cChantNum(0) { }
	~CCastingSpell() { ClearAll(); }

	inline void SetOwner(CAggresiveCreature* pOwner) { m_pOwner = pOwner; }

	bool Add(CSpell* pSpell);
	bool Remove(CSpell* pSpell);	

	void ClearChant();
	void ClearEnchant();

	inline void ClearAll();

	CSpell*	GetSpell(unsigned short usSkill_ID);

	void EnableChant(unsigned long dwOperateFlag = 0);
	void DisableChant(unsigned long dwOperateFlag = 0);

protected:

	enum 
	{
		MAX_ENCHANT_CASTING	= 20,
		MAX_CHANT_CASTING	=  1	
	};

    CSpell*	m_pEnchantCasting[MAX_ENCHANT_CASTING];	//	캐스팅 한 주문
	CSpell*	m_pChantCasting[MAX_CHANT_CASTING];		//	캐릭터가 사용하고 있는 Chant주문.

	CAggresiveCreature*	m_pOwner;

	unsigned short	m_usInternalFlag;
	unsigned char	m_cEnchantNum;
	unsigned char	m_cChantNum;
};

inline void CCastingSpell::ClearAll()
{
	ClearChant();
	ClearEnchant();
}

#endif