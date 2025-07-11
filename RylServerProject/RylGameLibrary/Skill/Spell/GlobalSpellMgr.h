#ifndef _CGLOBAL_SPELL_MGR_
#define _CGLOBAL_SPELL_MGR_

#include "NullSpell.h"

class CGlobalSpellMgr
{
public:

	void Add(CSpell* pSpell);	//	Spell�� �����ڿ��� �߰��Ѵ�.
    void Process();				//	���ʸ��� ����ȴ�. �ֹ� ó�� �� �ʿ� ������ �ֹ��� �����Ѵ�.
	void Clear();				//	Spell���� ���� �����Ѵ�.

	size_t GetSpellNum() const { return m_nSpellNum; }	//	���� ��ϵ� Spell������ �����Ѵ�.
	static CGlobalSpellMgr& GetInstance();

protected:

	CGlobalSpellMgr();
	~CGlobalSpellMgr();

	size_t		m_nSpellNum;
	CNullSpell	m_HeadSpell;
};

#endif