
#ifndef _SPELLTABLE_H_
#define _SPELLTABLE_H_

#include "Spell.h"


//-------------------------------------------------------------------------------------------------
// Chant Table

class CChantSpell : public CSpell
{
protected:

    CChantSpell(Spell_Info& spell_Info, short nConsumeMPAmount)
        :   CSpell(spell_Info, Skill::Type::CHANT), m_cOperateTurn(0), m_nConsumeMPAmount(nConsumeMPAmount) { }

	virtual void Operate(CAggresiveCreature* lpAffected);

	char    m_cOperateTurn;
	short   m_nConsumeMPAmount;
};


class CBattleSongSpell : public CChantSpell
{
public:

	CBattleSongSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
		:   CChantSpell(spell_Info, nConsumeMPAmount) { }

    virtual ~CBattleSongSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CStealHandSpell : public CChantSpell
{
public:

	CStealHandSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
		:   CChantSpell(spell_Info, nConsumeMPAmount) { }

    virtual ~CStealHandSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CMaintenanceChantSpell : public CChantSpell
{
public:

	CMaintenanceChantSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
        :   CChantSpell(spell_Info, nConsumeMPAmount) { }

	virtual ~CMaintenanceChantSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CClairvoyanceChantSpell : public CChantSpell
{
public:

	CClairvoyanceChantSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
        :   CChantSpell(spell_Info, nConsumeMPAmount) { }

	virtual ~CClairvoyanceChantSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CAccelerationChantSpell : public CChantSpell
{
public:

	CAccelerationChantSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
        :   CChantSpell(spell_Info, nConsumeMPAmount) { }

	virtual ~CAccelerationChantSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CLifeAuraSpell : public CChantSpell
{
public:

	CLifeAuraSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
        :   CChantSpell(spell_Info, nConsumeMPAmount) { }

	virtual ~CLifeAuraSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
};

class CGuardAuraSpell : public CChantSpell
{
public:

	CGuardAuraSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
        :   CChantSpell(spell_Info, nConsumeMPAmount) { }

	virtual ~CGuardAuraSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
};

class CSpeedBuffChantSpell : public CChantSpell
{
public:

	CSpeedBuffChantSpell(Spell_Info& spell_Info, short nConsumeMPAmount) 
		:   CChantSpell(spell_Info, nConsumeMPAmount) { }

		virtual ~CSpeedBuffChantSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};


//-------------------------------------------------------------------------------------------------
// Friend Target Enchant Table
class CMichaelBlessSpell : public CSpell
{
public:

	CMichaelBlessSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CMichaelBlessSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};


class CExpOrbSpell : public CSpell
{
public:

	CExpOrbSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CExpOrbSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CRegenerationSpell : public CSpell
{
public:

	CRegenerationSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CRegenerationSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
};

class CBuffPotionSpell : public CSpell
{
public:

	CBuffPotionSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CBuffPotionSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};


class CBlazeSpell : public CSpell
{
public:

	CBlazeSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CBlazeSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CChargingSpell : public CSpell
{
public:

	CChargingSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CChargingSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CStealthSpell : public CSpell
{
public:
	
	CStealthSpell(Spell_Info& spell_Info)
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CStealthSpell() { Destroy(); }
};

class CManaShellSpell : public CSpell
{
public:
	
	CManaShellSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CManaShellSpell() { Destroy(); }
};

class CEncourageSpell : public CSpell
{
public:
	
	CEncourageSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CEncourageSpell() { Destroy(); }

protected:

    virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CEnchantWeaponSpell : public CSpell
{
public:
	
	CEnchantWeaponSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CEnchantWeaponSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CBrightArmorSpell : public CSpell
{
public:
	
	CBrightArmorSpell(Spell_Info& spell_Info) 
        : CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CBrightArmorSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CHardenSkinSpell : public CSpell
{
public:
	
	CHardenSkinSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CHardenSkinSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CFlexibilitySpell : public CSpell
{
public:

	CFlexibilitySpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CFlexibilitySpell() { Destroy(); }
};

class CGuardSpell : public CSpell
{
public:

	CGuardSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT), m_nOriginalBlocking(0) { }
	virtual ~CGuardSpell() { Destroy(); }

protected:
	
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);

	short m_nOriginalBlocking;
};

class CLuckyOrbSpell : public CSpell
{
public:

	CLuckyOrbSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CLuckyOrbSpell() { Destroy(); }

protected:
	
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CChocolateSpell : public CSpell
{
public:

	CChocolateSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CChocolateSpell() { Destroy(); }

protected:
	
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CTasteWineSpell : public CSpell
{
public:

	CTasteWineSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CTasteWineSpell() { Destroy(); }

protected:
	
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CWifeCrackerSpell : public CSpell
{
public:

	CWifeCrackerSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CWifeCrackerSpell() { Destroy(); }
};


//-------------------------------------------------------------------------------------------------
// Enemy Target Enchant Table

class CSlowSpell : public CSpell
{
public:

	CSlowSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CSlowSpell() { Destroy(); }
};

class CArmorBrokenSpell : public CSpell
{
public:

	CArmorBrokenSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CArmorBrokenSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CHoldSpell : public CSpell
{
public:
	
	CHoldSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CHoldSpell() { Destroy(); }
};

class CStunSpell : public CSpell
{
public:
	
	CStunSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CStunSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CFrozenSpell : public CSpell
{
public:
	
	CFrozenSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }

    virtual ~CFrozenSpell() { Destroy(); }
};

class CPoisonedSpell : public CSpell
{
public:

	CPoisonedSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CPoisonedSpell() { Destroy(); }

protected:

    virtual void Operate(CAggresiveCreature* lpAffected);
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

//DeEnchant spell
class CDeEnchantSpell : public CSpell
{
public:

	CDeEnchantSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CDeEnchantSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CLowerStrengthSpell : public CSpell
{
public:
	
	CLowerStrengthSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CLowerStrengthSpell() { Destroy(); }

protected:
    
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CStoneFormSpell : public CSpell
{
public:
	
	CStoneFormSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CStoneFormSpell() { Destroy(); }

protected:
    
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CCounterAttackSpell : public CSpell
{
public:
	
	CCounterAttackSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CCounterAttackSpell() { Destroy(); }
};

class CEnvenomSpell : public CSpell
{
public:
	
	CEnvenomSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CEnvenomSpell() { Destroy(); }
};

class CBombSetSpell : public CSpell
{
public:
	
	CBombSetSpell(Spell_Info& spell_Info) 
        :   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CBombSetSpell() { Destroy(); }

protected:
    
    virtual void Operate(CAggresiveCreature* lpAffected);
};

class CFiredSpell : public CSpell
{
public:

	CFiredSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
		virtual ~CFiredSpell() { Destroy(); }

protected:

	virtual void Operate(CAggresiveCreature* lpAffected);
	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CCurseOfBlindSpell : public CSpell
{
public:

	CCurseOfBlindSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
		virtual ~CCurseOfBlindSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

//-------------------------------------------------------------------------------------------------
// Speacial Enchant Table
//hz added for kill streak Buff
class CKillStreakBuffSpell : public CSpell
{
public:
	CKillStreakBuffSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CKillStreakBuffSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

//hz end
//////////////////////////////////////////////////////////////////////
class CGreenPotionSpell : public CSpell
{
public:

	CGreenPotionSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CGreenPotionSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
class CGreenPotion1Spell : public CSpell
{
public:

	CGreenPotion1Spell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CGreenPotion1Spell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
class CGreenPotion2Spell : public CSpell
{
public:

	CGreenPotion2Spell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CGreenPotion2Spell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
class CGreenPotion3Spell : public CSpell
{
public:

	CGreenPotion3Spell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

	virtual ~CGreenPotion3Spell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};
///////////////////////////////////////////////////////////////

class CInvincibleSpell : public CSpell
{
public:
	
	CInvincibleSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CInvincibleSpell() { Destroy(); }
};

class CKarterantWorldBuffSpell : public CSpell
{
public:
	CKarterantWorldBuffSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CKarterantWorldBuffSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CMerkadiaWorldBuffSpell : public CSpell
{
public:
	CMerkadiaWorldBuffSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CMerkadiaWorldBuffSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CKarterantWorldDeBuffSpell : public CSpell
{
public:
	CKarterantWorldDeBuffSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CKarterantWorldDeBuffSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CMerkadiaWorldDeBuffSpell : public CSpell
{
public:
	CMerkadiaWorldDeBuffSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
	virtual ~CMerkadiaWorldDeBuffSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

//-------------------------------------------------------------------------------------------------
// Statue

class CPowerStatueSpell : public CSpell				// »˚¿« ºÆªÛ.
{
public:
	CPowerStatueSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }
		
		virtual ~CPowerStatueSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CIntStatueSpell : public CSpell				// ¡ˆ¥…¿« ºÆªÛ.
{
public:
	CIntStatueSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CIntStatueSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CExpStatueSpell : public CSpell				// ∞Ê«Ë¿« ºÆªÛ.
{
public:

	CExpStatueSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CExpStatueSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CWealthStatueSpell : public CSpell				// ∫Œ¿« ºÆªÛ.
{
public:

	CWealthStatueSpell(Spell_Info& spell_Info)
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CWealthStatueSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

//-------------------------------------------------------------------------------------------------
// Realm

class CRealmHPSpell : public CSpell				// ±π∞°¿¸¿Ô ∞¯«Â»∆¿Â ∆˜¿Œ∆Æ.
{
public:
	CRealmHPSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CRealmHPSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CRealmMPSpell : public CSpell				// ±π∞°¿¸¿Ô ∞¯«Â»∆¿Â ∆˜¿Œ∆Æ.
{
public:
	CRealmMPSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CRealmMPSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CHundredLevelSpell : public CSpell				// 100∑¶ ¿Ã∆Â∆Æ
{
public:
	CHundredLevelSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CHundredLevelSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};

class CDeCoolDownSpell : public CSpell				// ƒ≈∏¿” ∞®º“
{
public:
	CDeCoolDownSpell(Spell_Info& spell_Info) 
		:   CSpell(spell_Info, Skill::Type::ENCHANT) { }

		virtual ~CDeCoolDownSpell() { Destroy(); }

protected:

	virtual bool Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
	virtual bool Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag);
};


#endif