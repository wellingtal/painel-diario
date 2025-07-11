#ifndef _CNULLSPELL_H_
#define _CNULLSPELL_H_

#include "Spell.h"

class CNullSpell : public CSpell
{
public:

	CNullSpell() : CSpell(Spell_Info(Skill::ProtoType(), NULL, 
		Skill::SpellType::NONE, Skill::SpellID::None, 0, 0), Skill::Type::NONE) { }
	virtual ~CNullSpell() { }

protected:
	
	inline virtual bool Activate(CAggresiveCreature* pAffected, unsigned long dwOperateFlag) { return true; }
	inline virtual bool Deactivate(CAggresiveCreature* pAffected, unsigned long dwOperateFlag) { return true; }
};


#endif