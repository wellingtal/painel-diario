#include "stdafx.h"

#include <Skill/SkillMgr.h>
#include <Creature/AggresiveCreature.h>

#include "Spell.h"
#include "Casting.h"


bool CCastingSpell::Add(CSpell* pSpell)
{	
	CSpell*	 pRemoveSpell   = 0;
	CSpell** ppPastEnd      = NULL;
   
    switch(pSpell->GetSkillType())
    {
    case Skill::Type::CHANT:

		if(m_cChantNum >= MAX_CHANT_CASTING)
		{			
			pRemoveSpell = m_pChantCasting[0];

			//	맨 처음에 캐스팅 한 주문을 지움.
			ppPastEnd = m_pChantCasting + m_cChantNum;
			std::copy(m_pChantCasting + 1, ppPastEnd, m_pChantCasting);
			--m_cChantNum;

            m_pChantCasting[m_cChantNum] = 0;

			pRemoveSpell->ClearAll();
		}

        m_pChantCasting[m_cChantNum] = pSpell;
		++m_cChantNum;
		return true;

    case Skill::Type::ENCHANT:

		if(m_cEnchantNum >= MAX_ENCHANT_CASTING)
		{
			pRemoveSpell = m_pEnchantCasting[0];
            


			//	맨 처음에 캐스팅 한 주문을 지움.
			ppPastEnd = m_pEnchantCasting + m_cEnchantNum;
			std::copy(m_pEnchantCasting + 1, ppPastEnd, m_pEnchantCasting);
			--m_cEnchantNum;

            m_pEnchantCasting[m_cEnchantNum] = 0;

			pRemoveSpell->ClearAll();
		}
/*
		// edith 2008.10.16 오브 삭제시 로그 남김.
		if(pSpell->GetSpellID() == Skill::SpellID::ExpOrb)
		{
			unsigned long time = pSpell->GetDurationSec();
			if(m_pOwner)
				SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Add 경험치의 오브 생성 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
			else
				SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Add 경험치의 오브 생성 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
		}
		else if(pSpell->GetSpellID() == Skill::SpellID::LuckyOrb)
		{
			unsigned long time = pSpell->GetDurationSec();
			if(m_pOwner)
				SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Add 행운의 오브 생성 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
			else
				SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Add 행운의 오브 생성 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
		}
*/
        m_pEnchantCasting[m_cEnchantNum] = pSpell;
		++m_cEnchantNum;
		return true;
	};
    
	return false;
}


bool CCastingSpell::Remove(CSpell* pSpell)
{
	CSpell** ppBegin	= NULL;
	CSpell** ppPastEnd	= NULL;

    switch(pSpell->GetSkillType())
    {
    case Skill::Type::CHANT:

        for(ppBegin	= m_pChantCasting, ppPastEnd = m_pChantCasting + m_cChantNum;
			ppBegin != ppPastEnd; ++ppBegin)
		{
            if((*ppBegin) == pSpell)
			{
				std::copy(ppBegin + 1, ppPastEnd, ppBegin);
				--m_cChantNum;

                m_pChantCasting[m_cChantNum] = 0;
				return true;
			}
		}
        break;

    case Skill::Type::ENCHANT:

		for(ppBegin	= m_pEnchantCasting, ppPastEnd = m_pEnchantCasting + m_cEnchantNum;
			ppBegin != ppPastEnd; ++ppBegin)
		{
            if((*ppBegin) == pSpell)
			{
/*
				// edith 2008.10.16 오브 삭제시 로그 남김.
				if(pSpell->GetSpellID() == Skill::SpellID::ExpOrb)
				{
					unsigned long time = pSpell->GetDurationSec();
					if(m_pOwner)
						SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CCastingSpell::Remove 경험치의 오브 삭제 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
					else
						SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CCastingSpell::Remove 경험치의 오브 삭제 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
				}
				else if(pSpell->GetSpellID() == Skill::SpellID::LuckyOrb)
				{
					unsigned long time = pSpell->GetDurationSec();
					if(m_pOwner)
						SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CCastingSpell::Remove 행운의 오브 삭제 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
					else
						SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CCastingSpell::Remove 행운의 오브 삭제 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
				}
*/
				std::copy(ppBegin + 1, ppPastEnd, ppBegin);
				--m_cEnchantNum;

                m_pEnchantCasting[m_cEnchantNum] = 0;
				return true;
			}
		}
        break;
	};

	return false;
}


void CCastingSpell::ClearChant()
{
	CSpell**	pprBegin	= m_pChantCasting + m_cChantNum - 1;
	CSpell**	pprPastEnd	= m_pChantCasting - 1;

	for(;pprBegin != pprPastEnd; --pprBegin)
	{
		(*pprBegin)->ClearAll();
	}	
}


void CCastingSpell::ClearEnchant()
{
	CSpell**	pprBegin	= m_pEnchantCasting + m_cEnchantNum - 1;
	CSpell**	pprPastEnd	= m_pEnchantCasting - 1;

	for(;pprBegin != pprPastEnd; --pprBegin)
	{
		(*pprBegin)->ClearAll();
	}	
}


CSpell* CCastingSpell::GetSpell(unsigned short usSpellID)
{
	CSpell** ppBegin	= NULL;
	CSpell** ppPastEnd	= NULL;

    // 챈트
	ppBegin		= m_pChantCasting;
    ppPastEnd	= m_pChantCasting + m_cChantNum;
	for(;ppBegin != ppPastEnd; ++ppBegin)
	{
		if((*ppBegin)->GetSpellID() == usSpellID)
		{
			return *ppBegin;
		}
	}

	// 인챈트
	ppBegin		= m_pEnchantCasting;
	ppPastEnd	= m_pEnchantCasting + m_cEnchantNum;		
	for(;ppBegin != ppPastEnd; ++ppBegin)
	{
		if((*ppBegin)->GetSpellID() == usSpellID)
		{
			return *ppBegin;
		}
	}

	return NULL;
}


void CCastingSpell::EnableChant(unsigned long dwOperateFlag)
{
	CSpell** ppBegin	= m_pChantCasting;
	CSpell** ppPastEnd	= m_pChantCasting + m_cChantNum;

	for(;ppBegin != ppPastEnd; ++ppBegin)
	{
		(*ppBegin)->Enable(dwOperateFlag);
	}
}

void CCastingSpell::DisableChant(unsigned long dwOperateFlag)
{
	CSpell** ppBegin	= m_pChantCasting;
	CSpell** ppPastEnd	= m_pChantCasting + m_cChantNum;

	for(;ppBegin != ppPastEnd; ++ppBegin)
	{
		(*ppBegin)->Disable(dwOperateFlag);
	}
}

