#ifndef _CSPELL_MGR_H_
#define _CSPELL_MGR_H_

#include "Casting.h"
#include "Affected.h"

//	전방 참조
class CSpell;
class CAggresiveCreature;


class CSpellMgr
{
public:

	CSpellMgr();
	~CSpellMgr();

	inline void SetOwner(CAggresiveCreature* pOwner);	//	생성시 반드시 호출해 줄 것.
	inline CAggresiveCreature* GetOwner() { return m_pOwner; }

	inline CCastingSpell&	GetCastingInfo()		{ return m_CastingInfo;		}
	inline CAffectedSpell&	GetAffectedInfo()		{ return m_AffectedInfo;	}
	void				SetSpell(const SPELL spell);

protected:

    CAggresiveCreature*	m_pOwner;	    //	현재 매니저를 소유하고 있는 Creature.(바뀌지 않음)

	CCastingSpell	    m_CastingInfo;
	CAffectedSpell	    m_AffectedInfo;
	
	friend class CSpell;			//	Caster와 Affected의 상호참조 관리를 외부로 노출시키지 않기 위함.
};


inline void CSpellMgr::SetOwner(CAggresiveCreature* pOwner)
{
	m_pOwner = pOwner;
	m_CastingInfo.SetOwner(pOwner);
	m_AffectedInfo.SetOwner(pOwner);
}

#endif