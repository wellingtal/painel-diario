#ifndef _CSPELL_MGR_H_
#define _CSPELL_MGR_H_

#include "Casting.h"
#include "Affected.h"

//	���� ����
class CSpell;
class CAggresiveCreature;


class CSpellMgr
{
public:

	CSpellMgr();
	~CSpellMgr();

	inline void SetOwner(CAggresiveCreature* pOwner);	//	������ �ݵ�� ȣ���� �� ��.
	inline CAggresiveCreature* GetOwner() { return m_pOwner; }

	inline CCastingSpell&	GetCastingInfo()		{ return m_CastingInfo;		}
	inline CAffectedSpell&	GetAffectedInfo()		{ return m_AffectedInfo;	}
	void				SetSpell(const SPELL spell);

protected:

    CAggresiveCreature*	m_pOwner;	    //	���� �Ŵ����� �����ϰ� �ִ� Creature.(�ٲ��� ����)

	CCastingSpell	    m_CastingInfo;
	CAffectedSpell	    m_AffectedInfo;
	
	friend class CSpell;			//	Caster�� Affected�� ��ȣ���� ������ �ܺη� �����Ű�� �ʱ� ����.
};


inline void CSpellMgr::SetOwner(CAggresiveCreature* pOwner)
{
	m_pOwner = pOwner;
	m_CastingInfo.SetOwner(pOwner);
	m_AffectedInfo.SetOwner(pOwner);
}

#endif