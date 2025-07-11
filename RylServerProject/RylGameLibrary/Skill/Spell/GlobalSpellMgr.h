#ifndef _CGLOBAL_SPELL_MGR_
#define _CGLOBAL_SPELL_MGR_

#include "NullSpell.h"

class CGlobalSpellMgr
{
public:

	void Add(CSpell* pSpell);	//	Spell의 생성자에서 추가한다.
    void Process();				//	매초마다 실행된다. 주문 처리 및 필요 없어진 주문을 제거한다.
	void Clear();				//	Spell들을 전부 제거한다.

	size_t GetSpellNum() const { return m_nSpellNum; }	//	현재 등록된 Spell개수를 리턴한다.
	static CGlobalSpellMgr& GetInstance();

protected:

	CGlobalSpellMgr();
	~CGlobalSpellMgr();

	size_t		m_nSpellNum;
	CNullSpell	m_HeadSpell;
};

#endif