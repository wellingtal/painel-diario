#include "stdafx.h"

#include "SpellUtil.h"
#include "Spell.h"
#include "SpellTable.h"
#include "SpellMgr.h"
#include <Skill/SkillTable.h>

using namespace Skill;

#define ADD_SPELL(SpellName) \
	CAddSpell<##SpellName>(CSpell::Spell_Info(CProcessTable::ProcessInfo::m_NullProtoType, NULL, spell.Spells[cIndex].cSpellType, \
	spell.Spells[cIndex].wSpellID, spell.Spells[cIndex].wEnchantLevel, spell.Spells[cIndex].dwDurationSec))(m_pOwner);


#define ADD_SPELLNEW(SpellName, SpellType) \
	CAddSpell<##SpellName>(CSpell::Spell_Info(CProcessTable::ProcessInfo::m_NullProtoType, NULL, SpellType, \
	spell.Spells[cIndex].wSpellID, spell.Spells[cIndex].wEnchantLevel, spell.Spells[cIndex].dwDurationSec))(m_pOwner);

CSpellMgr::CSpellMgr()
:	m_pOwner(NULL)	
{

}

CSpellMgr::~CSpellMgr()
{


}

void CSpellMgr::SetSpell(const SPELL spell)
{
	int iCount = 0;
	for (unsigned char cIndex = 0; cIndex < SPELL::MAX_SPELL_NUM; ++cIndex)
	{
		switch(spell.Spells[cIndex].wSpellID)
		{
		case Skill::SpellID::CD_8D12:
		case Skill::SpellID::CD_8D14:
		case Skill::SpellID::CD_8D16:
		case Skill::SpellID::CD_99A1:
		case Skill::SpellID::CD_99A2:
		case Skill::SpellID::CD_99A3:

		case Skill::SpellID::DemagePotion:
		case Skill::SpellID::ArmorPotion:
		case Skill::SpellID::HitRatePotion:
		case Skill::SpellID::EvadePotion:
		case Skill::SpellID::MaxHPPotion:
		case Skill::SpellID::MaxMPPotion:
		case Skill::SpellID::HPRegenPotion:
		case Skill::SpellID::MPRegenPotion:
		case Skill::SpellID::CriticalPotion:
		case Skill::SpellID::BlockPotion:
		case Skill::SpellID::SpeedPotion:
		case Skill::SpellID::DeCoolDownPotion:
		case Skill::SpellID::MagicPowerPotion:
		case Skill::SpellID::MagicResistPotion:
			ADD_SPELL(CBuffPotionSpell);
			iCount++;
			break;

		case Skill::SpellID::ExpOrb :
			ADD_SPELL(CExpOrbSpell);
//			ADD_SPELLNEW(CExpOrbSpell, Skill::SpellType::PAYBUFF_SPELL);
//			ADD_SPELL(CBuffPotionSpell);
			iCount++;
			break;

		case Skill::SpellID::LuckyOrb :
			ADD_SPELL(CLuckyOrbSpell);
//			ADD_SPELLNEW(CLuckyOrbSpell, Skill::SpellType::PAYBUFF_SPELL);
//			ADD_SPELL(CBuffPotionSpell);
			iCount++;
			break;

		case Skill::SpellID::MichaelBless :
			ADD_SPELL(CMichaelBlessSpell);
//			ADD_SPELLNEW(CLuckyOrbSpell, Skill::SpellType::PAYBUFF_SPELL);
//			ADD_SPELL(CBuffPotionSpell);
			iCount++;
			break;

		case Skill::SpellID::PowerStatue :
			ADD_SPELLNEW(CPowerStatueSpell, Skill::SpellType::STATUE_SPELL);
			iCount++;
			break;
		case Skill::SpellID::IntStatue :
			ADD_SPELLNEW(CIntStatueSpell, Skill::SpellType::STATUE_SPELL);
			iCount++;
			break;
		case Skill::SpellID::ExpStatue :
			ADD_SPELLNEW(CExpStatueSpell, Skill::SpellType::STATUE_SPELL);
			iCount++;
			break;
		case Skill::SpellID::WealthStatue :
			ADD_SPELLNEW(CWealthStatueSpell, Skill::SpellType::STATUE_SPELL);
			iCount++;
			break;
		}
	}


	return;

	// edith 100·¾ ´Þ¼ºÀÌÆÑÆ®
/*
	for (unsigned char cIndex = 0; cIndex < SPELL::MAX_SPELL_NUM; ++cIndex)
	{
		switch(spell.Spells[cIndex].wSpellID)
		{
			case Skill::SpellID::HundredLevel:		ADD_SPELL(CHundredLevelSpell);		break;
		}
	}
*/
}
