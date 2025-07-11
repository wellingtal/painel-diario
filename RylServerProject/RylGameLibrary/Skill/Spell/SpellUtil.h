#ifndef _SPELL_UTIL_H_
#define _SPELL_UTIL_H_

#include <Skill/Spell/Spell.h>
#include <Creature/Monster/MonsterMgr.h>
#include <Creature/Siege/SiegeConstants.h>

namespace Skill
{
	template<typename Spell_t>
	class CAddSpell
	{
	public:

		CAddSpell(const CSpell::Spell_Info& Spell_Info)
			:	m_Spell_Info(Spell_Info)
		{
		}

		unsigned short operator() (CAggresiveCreature* lpCharacter)
		{
			unsigned short wError = CSpell::NO_ENCHANT_ERROR;

			if (NULL != lpCharacter)
			{
				// 몬스터중 챈트/인챈트 무시 플래그가 있는 녀석은 스펠 무시
				Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpCharacter->GetCID());
				if (Creature::CT_MONSTER == eCreatureType || Creature::CT_SUMMON == eCreatureType)
				{
					const CMonsterMgr::MonsterProtoType* lpProtoType =
						CMonsterMgr::GetInstance().GetMonsterProtoType(lpCharacter->GetCID() & Creature::MONSTER_KIND_BIT);

					if (lpProtoType && lpProtoType->m_MonsterInfo.m_bIgnoreEnchant)
					{
						return wError;
					}
				}
				// 석상은 모든 챈트, 인챈트에 걸리지 않는다.
				else if (Creature::CT_STRUCT == eCreatureType)
				{
					return CSpell::ENCHANT_FAIL_BY_RESIST;
				}
				// 공성 오브젝트는 모든 챈트, 인챈트에 걸리지 않는다.
				else if (Creature::CT_SIEGE_OBJECT == eCreatureType)
				{
					// 성 상징물일 경우 무적 상태의 챈트만 허용한다. 나머지 오브젝트나 챈트, 인챈트는 허용하지 않는다.
					if (!(Siege::EMBLEM == lpCharacter->GetObjectType() && Skill::SpellID::Invincible == m_Spell_Info.m_wSpellID))
					{
						return CSpell::ENCHANT_FAIL_BY_RESIST;
					}
				}

				CSpell* lpSpell = new Spell_t(m_Spell_Info);

				if (NULL != lpSpell)
				{
					if (true == lpSpell->AddAffected(lpCharacter, wError))
					{
						CGlobalSpellMgr::GetInstance().Add(lpSpell);
						return wError;
					}

					delete lpSpell;
				}
			}

			return wError;
		}


	private:

		CSpell::Spell_Info	m_Spell_Info;	
	};
}


#endif