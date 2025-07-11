
#include "stdafx.h"

#include <Skill/SkillMgr.h>
#include <Creature/AggresiveCreature.h>

#include "Spell.h"
#include "Affected.h"

#include "SpellKind.h"


bool CAffectedSpell::Add(CSpell* pSpell, unsigned short& wError)
{	
	if (true == RemoveOverlappedSpell(pSpell))
	{      
        switch (pSpell->GetSkillType())
        {
	        case Skill::Type::CHANT:
			{
				if (m_cChantNum < MAX_CHANT) 
				{
					m_pChant[m_cChantNum] = pSpell;
					++m_cChantNum;
					return true;
				}

			} break;

	        case Skill::Type::ENCHANT:
			{
				if (Skill::SpellTarget::ENEMY_TARGET_ENCHANT == pSpell->GetSpellTarget())
				{
					RemoveEnchantBySpellType(Skill::SpellID::Stealth);
				}
				if (Skill::SpellID::Stealth == pSpell->GetSpellID() && 
					true == IsSpellThisTargetType(Skill::SpellTarget::ENEMY_TARGET_ENCHANT))
				{
					ERRLOG1(g_Log, "CID:0x%08x 적대 스킬이 풀리지 않은 상태에서 스텔스를 걸려고 합니다.", m_pOwner->GetCID());
					
					wError = CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT;
					return false;
				}

				if (m_cEnchantNum < MAX_ENCHANT)
				{
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
					m_pEnchant[m_cEnchantNum] = pSpell;
					++m_cEnchantNum;
					return true;
				}

			} break;
		}
	}


	wError = CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED;
	return false;
}


bool CAffectedSpell::Remove(CSpell* pSpell)
{
	CSpell** ppBegin	= NULL;	
	CSpell** ppPastEnd	= NULL;	
	
	switch (pSpell->GetSkillType())
    {
	    case Skill::Type::CHANT:
		{
			for (ppBegin = m_pChant, ppPastEnd = m_pChant + m_cChantNum;
				ppBegin != ppPastEnd; ++ppBegin)
			{
				if (pSpell == *ppBegin)
				{				
					std::copy(ppBegin + 1, ppPastEnd, ppBegin);
					--m_cChantNum;
                    
                    m_pChant[m_cChantNum] = 0;
                    return true;
				}
			}

		} break;

	    case Skill::Type::ENCHANT:    
		{
			for (ppBegin = m_pEnchant, ppPastEnd = m_pEnchant + m_cEnchantNum;
				ppBegin != ppPastEnd; ++ppBegin)
			{
				if (pSpell == *ppBegin)
				{	
					// edith 2008.10.16 오브 삭제시 로그 남김.
					if(pSpell->GetSpellType() == Skill::SpellType::PAYBUFF_SPELL)
					{
						unsigned long time = pSpell->GetDurationSec();
						if(m_pOwner)
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
						else
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
					}
/*
					// edith 2008.10.16 오브 삭제시 로그 남김.
					if(pSpell->GetSpellID() == Skill::SpellID::ExpOrb)
					{
						unsigned long time = pSpell->GetDurationSec();
						if(m_pOwner)
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove 경험치의 오브 삭제 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
						else
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove 경험치의 오브 삭제 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
					}
					else if(pSpell->GetSpellID() == Skill::SpellID::LuckyOrb)
					{
						unsigned long time = pSpell->GetDurationSec();
						if(m_pOwner)
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove 행운의 오브 삭제 : Type:%d : %d Tick남음", m_pOwner->GetUID(), m_pOwner->GetCID(), pSpell->GetSpellType(), time);
						else
							SERLOG4(g_SkillLog, "UID:%d/CID:0x%08x CAffectedSpell::Remove 행운의 오브 삭제 : Type:%d : %d Tick남음", 0, 0, pSpell->GetSpellType(), time);
					}
*/
					std::copy(ppBegin + 1, ppPastEnd, ppBegin);
					--m_cEnchantNum;

                    m_pEnchant[m_cEnchantNum] = 0;
					return true;
				}
			}

		} break;
	}

	return false;
}


void CAffectedSpell::ClearChant()
{
	CSpell**	pprBegin	= m_pChant + m_cChantNum - 1;
	CSpell**	pprPastEnd	= m_pChant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->RemoveAffected(m_pOwner);
	}	
}

void CAffectedSpell::ClearEnchant()
{
	CSpell**	pprBegin	= m_pEnchant + m_cEnchantNum - 1;
	CSpell**	pprPastEnd	= m_pEnchant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->RemoveAffected(m_pOwner);
	}	
}

void CAffectedSpell::EnableChant(unsigned long dwOperateFlag)
{
	CSpell**	pprBegin	= m_pChant + m_cChantNum - 1;
	CSpell**	pprPastEnd	= m_pChant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->Enable(dwOperateFlag);
	}	
}

void CAffectedSpell::EnableEnchant(unsigned long dwOperateFlag)
{
	CSpell**	pprBegin	= m_pEnchant + m_cEnchantNum - 1;
	CSpell**	pprPastEnd	= m_pEnchant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->Enable(dwOperateFlag);
	}	
}

void CAffectedSpell::DisableChant(unsigned long dwOperateFlag)
{
	CSpell**	pprBegin	= m_pChant + m_cChantNum - 1;
	CSpell**	pprPastEnd	= m_pChant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->Disable(dwOperateFlag);
	}	
}

void CAffectedSpell::DisableEnchant(unsigned long dwOperateFlag)
{
	CSpell**	pprBegin	= m_pEnchant + m_cEnchantNum - 1;
	CSpell**	pprPastEnd	= m_pEnchant - 1;

	for (;pprBegin != pprPastEnd; --pprBegin)
	{
        (*pprBegin)->Disable(dwOperateFlag);
	}	
}

CSpell*	CAffectedSpell::GetEnchant(int Index)
{
	if(Index < 0)
		return NULL;

	if(Index >= MAX_ENCHANT)
		return NULL;

	return m_pEnchant[Index];
}

CSpell*	CAffectedSpell::GetSpell(unsigned short usSpellID)
{	
	CSpell** ppBegin	= NULL;
	CSpell** ppPastEnd	= NULL;

	// 챈트
	ppBegin		= m_pChant;
	ppPastEnd	= m_pChant + m_cChantNum;

	for (;ppBegin != ppPastEnd; ++ppBegin)
	{
		if ((*ppBegin)->GetSpellID() == usSpellID && true == (*ppBegin)->IsActivate(m_pOwner))
		{
			return *ppBegin;
		}
	}

	// 인챈트
	ppBegin		= m_pEnchant;
	ppPastEnd	= m_pEnchant + m_cEnchantNum;
	
	for (;ppBegin != ppPastEnd; ++ppBegin)
	{
		if ((*ppBegin)->GetSpellID() == usSpellID)
		{
			return *ppBegin;
		}
	}

	return NULL;
}

SPELL CAffectedSpell::GetSpellInfo(BOOL bDead)
{
	SPELL spell = SPELL::SPELL();
/*
	CSpell** ppBegin	= NULL;
	CSpell** ppPastEnd	= NULL;

	// 인챈트
	ppBegin		= m_pEnchant;
	ppPastEnd	= m_pEnchant + MAX_ENCHANT;

	unsigned char cIndex = 0;

	for (;ppBegin != ppPastEnd; ++ppBegin)
	{
		switch((*ppBegin)->GetSpellType())
		{
		case Skill::SpellType::BUFF_SPELL:
			if(bDead == TRUE)
				continue;

			// edith 2008.06.03 석상전 버프를 저장한다.
		case Skill::SpellType::STATUE_SPELL:
		case Skill::SpellType::PAYBUFF_SPELL:
		case Skill::SpellType::ETERNAL_SPELL:
			spell.Spells[cIndex] = SPELLINSTANCE((*ppBegin)->GetDurationSec(), (*ppBegin)->GetSpellID(), (*ppBegin)->GetSpellLevel(), (*ppBegin)->GetSpellType()); 
			++cIndex;
			break;
		}

		if(cIndex >= SPELL::MAX_SPELL_NUM)
			break;
	}
*/

	unsigned char cIndex = 0;

	for (int i = 0; i < MAX_ENCHANT; ++i)
	{
		if(!m_pEnchant[i])
			continue;

		switch(m_pEnchant[i]->GetSpellType())
		{
		case Skill::SpellType::BUFF_SPELL:
			if(bDead == TRUE)
				break;

			// edith 2008.06.03 석상전 버프를 저장한다.
		case Skill::SpellType::STATUE_SPELL:
		case Skill::SpellType::PAYBUFF_SPELL:
		case Skill::SpellType::ETERNAL_SPELL:
			spell.Spells[cIndex] = SPELLINSTANCE(m_pEnchant[i]->GetDurationSec(), m_pEnchant[i]->GetSpellID(), m_pEnchant[i]->GetSpellLevel(), m_pEnchant[i]->GetSpellType()); 
			++cIndex;
			break;
		}

		if(cIndex >= SPELL::MAX_SPELL_NUM)
			break;
	}


	return spell;
}

void CAffectedSpell::ApplyPartyChant(CAggresiveCreature* pAffected)
{
	unsigned short wError = CSpell::NO_ENCHANT_ERROR;

	for (CSpell** ppBegin = m_pChant, **ppPastEnd = m_pChant + m_cChantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
		if ((*ppBegin)->IsPartySkill())
		{
            (*ppBegin)->AddAffected(pAffected, wError);
		}
	}
}


void CAffectedSpell::ApplyEnchant(CAggresiveCreature* pAffected)
{
	unsigned short wError = CSpell::NO_ENCHANT_ERROR;

	for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
        (*ppBegin)->AddAffected(pAffected, wError);
	}	
}


bool CAffectedSpell::RemoveOverlappedSpell(CSpell* pSpell)
{
	CSkillMgr& SkillMgr = CSkillMgr::GetInstance();
	unsigned short	usSkill_ID	= pSpell->GetSpellID();

	CSpell** ppBegin	= NULL;
	CSpell** ppPastEnd	= NULL;

	if (Skill::Type::ENCHANT == pSpell->GetSkillType())
	{
		ppBegin		= m_pEnchant;
		ppPastEnd	= m_pEnchant + m_cEnchantNum;
	}
	else
	{
		return true;
	}

	for (; ppBegin != ppPastEnd; ++ppBegin)
	{
		CSpell* pOverlappedSpell = (*ppBegin);

		if (NULL != pOverlappedSpell)
		{
			if (usSkill_ID == pOverlappedSpell->GetSpellID())
			{
				bool bRemoveAffected = false;

				// 여기서 스킬을 비교한다. 스킬레벨이 더 높은걸로 돌게 비교 
				if (pSpell->GetSpellLevel() > pOverlappedSpell->GetSpellLevel())
				{
					bRemoveAffected = true;				
				}
				else if (pSpell->GetSpellLevel() == pOverlappedSpell->GetSpellLevel())
				{
					if (pSpell->GetDurationSec() >= pOverlappedSpell->GetDurationSec())
					{
						bRemoveAffected = true;
					}
				}

				if (true == bRemoveAffected)
				{
					pOverlappedSpell->ClearAll();
				}
				else
				{
					return false;
				}			
			}
		}
	}

	return true;
}



void CAffectedSpell::RemoveChantByCaster(CAggresiveCreature* pCaster)
{
	for (CSpell** ppBegin = m_pChant, **ppPastEnd = m_pChant + m_cChantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
        CSpell* lpSpell = (*ppBegin);

		if (0 != lpSpell && pCaster == lpSpell->GetCaster())
		{
            lpSpell->RemoveAffected(m_pOwner);
		}
	}	
}


void CAffectedSpell::RemoveEnchantByCaster(CAggresiveCreature* pCaster)
{
	for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
        CSpell* lpSpell = (*ppBegin);

		if (0 != lpSpell && pCaster == lpSpell->GetCaster())
		{
			lpSpell->RemoveAffected(m_pOwner);
		}
	}
}



void CAffectedSpell::RemoveChantBySpellType(unsigned char cSpellType)
{
	for (CSpell** ppBegin = m_pChant, **ppPastEnd = m_pChant + m_cChantNum;
        ppBegin != ppPastEnd; ++ppBegin)
	{
        CSpell* lpSpell = (*ppBegin);

        if (0 != lpSpell && lpSpell->GetSpellID() == cSpellType)
        {
            lpSpell->RemoveAffected(m_pOwner);
        }
	}	
}


bool CAffectedSpell::RemoveEnchantBySpellType(unsigned char cSpellType)
{
	for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
        ppBegin != ppPastEnd; ++ppBegin)
	{
        CSpell* lpSpell = (*ppBegin);

        if (0 != lpSpell && lpSpell->GetSpellID() == cSpellType)
        {
			lpSpell->RemoveAffected(m_pOwner);
			return true;
		}
	}

	return false;
}

bool CAffectedSpell::IsSpellOfEnemyCharacter(void)
{
	for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
		CAggresiveCreature* lpCaster = (*ppBegin)->GetCaster();

		Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpCaster->GetCID());
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		if (EnemyCheck::EC_ENEMY == m_pOwner->IsEnemy(lpCaster))
		{
			return true;
		}
	}

	return false;
}

bool CAffectedSpell::IsSpellThisTargetType(Skill::SpellTarget::Type eTargetType)
{
	for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
		ppBegin != ppPastEnd; ++ppBegin)
	{
		if ((*ppBegin)->GetSpellTarget() == eTargetType)
		{
			return true;
		}
	}

	return false;
}

// --------------------------------------------------------------------------------------------------
// CAffectedSpell::Disenchant
//
// 인자 : 주문 종류(마법형/물리형/월드웨폰/월드웨폰제외/모두), 타겟(긍정적/부정적/모두), 파괴 기준(레벨/시간/모두), 주문 레벨, 갯수
// 리턴 : 파괴한 인챈트 갯수
// --------------------------------------------------------------------------------------------------
unsigned char CAffectedSpell::Disenchant(Skill::SpellType::Type eSpellType, Skill::SpellTarget::Type eTargetType, 
										 Skill::Disenchant::Type eDisenchantType, unsigned short usSkillLevel, unsigned char cNum)
{
	CSpell *pTargetSpell = NULL;
	unsigned short usMaxLevel = 0;
	unsigned long dwMaxDuration = 0;
	unsigned char cResult = 0;

	if (Skill::Disenchant::INFINITE_NUM == cNum)
	{
		cNum = m_cEnchantNum;
	}

	for (unsigned char nIndex = 0; nIndex < cNum; nIndex++)
	{
		for (CSpell** ppBegin = m_pEnchant, **ppPastEnd = m_pEnchant + m_cEnchantNum;
            ppBegin != ppPastEnd; ++ppBegin)
		{
			if (Skill::SpellTarget::ALL_ENCHANT != eTargetType && (*ppBegin)->GetSpellTarget() != eTargetType) 
            {
				// 긍정적? 부정적?
				continue;
			}

			// 죽었을때 없애는 인챈트이면
			if (Skill::Disenchant::DEAD == eDisenchantType)
			{
				// 버프스펠보다 낮은거면 다 지워라
				if(Skill::SpellType::BUFF_SPELL >= (*ppBegin)->GetSpellType())
				{
					pTargetSpell = *ppBegin;
					break;
				}
			}

			// edith 경험치의 오브이면 해제하면 안된다.
			if((*ppBegin)->GetSpellID() == Skill::SpellID::ExpOrb)
				continue;

			if((*ppBegin)->GetSpellID() == Skill::SpellID::LuckyOrb)
				continue;

			// 유료면 디스펠에서 디스펠 되면 안된다.
			if(Skill::SpellType::PAYBUFF_SPELL == (*ppBegin)->GetSpellType())
				continue;

			if (Skill::SpellType::NONE != eSpellType && (*ppBegin)->GetSpellType() != eSpellType)
			{
				// 마법형? 물리형?
				continue;
			}
			else if (Skill::SpellType::ETERNAL_SPELL != eSpellType &&
					Skill::SpellType::ETERNAL_SPELL == (*ppBegin)->GetSpellType())
			{
				// 영구적으로 유지되야 하는 스펠이면 제외
				continue;
			}

			// 모두 파괴
			if (Skill::Disenchant::NONE == eDisenchantType)
			{
				pTargetSpell = *ppBegin;
				break;
			}

			switch (eDisenchantType)
			{
				// 일정 레벨보다 낮은 것들 중 가장 높은 레벨의 것을 파괴
				case Skill::Disenchant::LEVEL:
				{
					if ((*ppBegin)->GetSpellLevel() < usSkillLevel && (*ppBegin)->GetSpellLevel() > usMaxLevel)
					{
						pTargetSpell = *ppBegin;
						usMaxLevel = (*ppBegin)->GetSpellLevel();
					}

				} break;

				// 가장 긴 시간이 남은 것을 파괴
				case Skill::Disenchant::DURATION:
				{
					if (CSpell::INFINITE_DURATION != (*ppBegin)->GetDurationSec() && (*ppBegin)->GetDurationSec() > dwMaxDuration)
					{
						pTargetSpell = *ppBegin;
						dwMaxDuration = (*ppBegin)->GetDurationSec();
					}					

				} break;
			}
		}

		if (NULL == pTargetSpell) 
        {
			return cResult;
		}

		pTargetSpell->RemoveAffected(m_pOwner);
		pTargetSpell = NULL;
		++cResult;
	}

	return cResult;
}