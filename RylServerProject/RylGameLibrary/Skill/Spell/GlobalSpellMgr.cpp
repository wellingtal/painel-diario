
#include "stdafx.h"

#include <Community/Party/Party.h>

#include "Spell.h"
#include "GlobalSpellMgr.h"


CGlobalSpellMgr& CGlobalSpellMgr::GetInstance() 
{ 
	static CGlobalSpellMgr globalSpellMgr; 
	return globalSpellMgr; 
}


CGlobalSpellMgr::CGlobalSpellMgr()
:	m_nSpellNum(0)
{


}


CGlobalSpellMgr::~CGlobalSpellMgr()
{
	Clear();
}


void CGlobalSpellMgr::Process()
{
	CSpell* pCurrentSpell	= m_HeadSpell.m_pNextSpell;
	CSpell* pPrevSpell		= &m_HeadSpell;
	
	for (;NULL != pCurrentSpell;)
	{
        // 주문에 의해 영향을 받는 이가 없거나, operate에 실패하면 delete
		// (영향을 받는 이가 없으면 operate할 필요 없음)
		if (false == pCurrentSpell->IsValid() || false == pCurrentSpell->Operate())
		{
			pPrevSpell->m_pNextSpell = pCurrentSpell->m_pNextSpell;
			delete pCurrentSpell;
			pCurrentSpell = pPrevSpell->m_pNextSpell;
		
			--m_nSpellNum;
		}
		else
		{
	        pPrevSpell = pCurrentSpell;
			pCurrentSpell = pCurrentSpell->m_pNextSpell;
		}
	}	
}


void CGlobalSpellMgr::Add(CSpell* pSpell)
{
	if(NULL != pSpell)
	{
		pSpell->m_pNextSpell = m_HeadSpell.m_pNextSpell;
		m_HeadSpell.m_pNextSpell = pSpell;

		++m_nSpellNum;
	}
}


void CGlobalSpellMgr::Clear()
{
	CSpell* pDelSpell = NULL;
	CSpell* pPosSpell = m_HeadSpell.m_pNextSpell;

	for (;NULL != pPosSpell;)
	{
		pDelSpell = pPosSpell;
        pPosSpell = pPosSpell->m_pNextSpell;

		delete pDelSpell;
		pDelSpell = NULL;
	}

	m_nSpellNum = 0;
}
