
#include "stdafx.h"

#include <Utility/Math/Math.h>

#include <Map/FieldMap/Cell.h>
#include <Skill/SkillMgr.h>

#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>

#include "SpellTable.h"
#include "SpellMgr.h"
#include "GlobalSpellMgr.h"


//-------------------------------------------------------------------------------------------------
// Chant Spell

void CChantSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (lpAffected == m_pCaster)
	{
		m_cOperateTurn++;
		if (m_cOperateTurn == UCHAR_MAX)
		{
			m_cOperateTurn = 0;
		}

		if (0 == (m_cOperateTurn % CSpell::REDUCE_MANA_TURN)) 
		{
			CreatureStatus& affectedStatus      = lpAffected->GetStatus();
			CCastingSpell&  affectedCastingInfo = lpAffected->GetSpellMgr().GetCastingInfo();

			if (m_nConsumeMPAmount > affectedStatus.m_nNowMP)
			{
				affectedStatus.m_nNowMP = 0;
				affectedCastingInfo.DisableChant();
			}
			else
			{
				affectedStatus.m_nNowMP -= m_nConsumeMPAmount;
				affectedCastingInfo.EnableChant();
			}
		}	
	}
}

//-------------------------------------------------------------------------------------------------
// BattleSong Spell

bool CBattleSongSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CBattleSongSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}


//-------------------------------------------------------------------------------------------------
// CStealHandSpell Spell

bool CStealHandSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CStealHandSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}


//-------------------------------------------------------------------------------------------------
// MaintenanceChant Spell

void CMaintenanceChantSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_cOperateTurn % 2)
	{
        lpAffected->RegenHPAndMP(m_wSpellLevel, 0, false);
	}

	CChantSpell::Operate(lpAffected);
}

bool CMaintenanceChantSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CMaintenanceChantSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

void CClairvoyanceChantSpell::Operate(CAggresiveCreature* lpAffected)
{
	/*if (0 == m_cOperateTurn % 2)
	{
        lpAffected->RegenHPAndMP(m_wSpellLevel, 0, false);
	}*/

	CChantSpell::Operate(lpAffected);
}

bool CClairvoyanceChantSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CClairvoyanceChantSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// AccelerationChant Spell

void CAccelerationChantSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_cOperateTurn % 2)
	{
        lpAffected->RegenHPAndMP(0, m_wSpellLevel, false);
	}

	CChantSpell::Operate(lpAffected);
}

bool CAccelerationChantSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CAccelerationChantSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// LifeAura Spell

void CLifeAuraSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_cOperateTurn % 2)
	{
        lpAffected->RegenHPAndMP(m_wSpellLevel * 2, m_wSpellLevel * 2, false);
	}

	CChantSpell::Operate(lpAffected);
}

//-------------------------------------------------------------------------------------------------

// GuardAura Spell

void CGuardAuraSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_cOperateTurn % 2)
	{
		lpAffected->GuardAura(m_wSpellLevel * 2,m_wSpellLevel * 2,false);
	}

	CChantSpell::Operate(lpAffected);
}

//-------------------------------------------------------------------------------------------------
// SpeedBuff Spell

bool CSpeedBuffChantSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CSpeedBuffChantSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Regeneration Spell

void CRegenerationSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_dwDurationSec % 2)
	{
		lpAffected->RegenHPAndMP(m_wSpellLevel * 2, 0, false);
	}
}

//-------------------------------------------------------------------------------------------------
// CBuffPotionSpell Spell

bool CBuffPotionSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CBuffPotionSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}


//-------------------------------------------------------------------------------------------------
// Blaze Spell

bool CBlazeSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CBlazeSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Charging Spell

bool CChargingSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CChargingSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Encourage Spell

bool CEncourageSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CEncourageSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// EnchantWeapon Spell

bool CEnchantWeaponSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CEnchantWeaponSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// BrightArmor Spell

bool CBrightArmorSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CBrightArmorSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// HardenSkin Spell

bool CHardenSkinSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CHardenSkinSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Guard Spell

bool CGuardSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CGuardSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// CMichaelBlessSpell Spell

bool CMichaelBlessSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CMichaelBlessSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// CExpOrbSpell Spell

bool CExpOrbSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CExpOrbSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// CLuckyOrbSpell Spell

bool CLuckyOrbSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CLuckyOrbSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}


//-------------------------------------------------------------------------------------------------
// Chocolate Spell

bool CChocolateSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CChocolateSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// CTasteWineSpell Spell

bool CTasteWineSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CTasteWineSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// ArmorBroken Spell

bool CArmorBrokenSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CArmorBrokenSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Stun Spell

bool CStunSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->GetSpellMgr().GetCastingInfo().ClearChant();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Poisoned Spell

void CPoisonedSpell::Operate(CAggresiveCreature* lpAffected)
{
	const short wDamage = static_cast<short>( (float)(m_wSpellLevel) * 1.0f );

	const short wThreatAmount = (lpAffected->GetStatus().m_nNowHP < wDamage) ? 
		lpAffected->GetStatus().m_nNowHP : wDamage;
	lpAffected->GetThreat().AddToThreatList(m_pCaster, wThreatAmount);		

	lpAffected->GetStatus().m_nNowHP = (lpAffected->GetStatus().m_nNowHP > wDamage) ? 
		lpAffected->GetStatus().m_nNowHP - wDamage : 0;

	if (0 == lpAffected->GetStatus().m_nNowHP)
	{
		lpAffected->Dead(m_pCaster);
		lpAffected->GetThreat().ClearAll();		
	} 

	DefenserNode node;
	
	node.m_wDamage = wDamage;
	node.m_dwCharID = lpAffected->GetCID();
	node.m_sCurrHP = lpAffected->GetStatus().m_nNowHP;
	node.m_sCurrMP = lpAffected->GetStatus().m_nNowMP;
	node.m_wMaxHP = lpAffected->GetStatus().m_StatusInfo.m_nMaxHP;
	node.m_wMaxMP = lpAffected->GetStatus().m_StatusInfo.m_nMaxMP;
	node.m_wMPHeal = 0;
	node.m_cJudge = ClientConstants::Judge_Poisoned;

	AtType attackType;
	attackType.m_wType = AtType::RIGHT_MELEE;

	CCell* lpCell = lpAffected->GetCellPos().m_lpCell;
	if (NULL != lpCell)
	{
		lpCell->SendAttackInfo(m_pCaster->GetCID(), attackType, 1, &node);
	}
	
	if (Creature::CT_PC == Creature::GetCreatureType(lpAffected->GetCID()))
	{
		AtType attackType;
		attackType.m_wType = AtType::RIGHT_MELEE;

		CCharacter* pAffectedCharacter = (CCharacter *)lpAffected;
		CGameClientDispatch* pCharacterDispatcher = pAffectedCharacter->GetDispatcher();
		if (NULL != pCharacterDispatcher)
		{
			GameClientSendPacket::SendCharAttacked(pCharacterDispatcher->GetSendStream(), m_pCaster, pAffectedCharacter, 
				attackType, 0, wDamage, ClientConstants::Judge_Poisoned, 0,  PktBase::NO_SERVER_ERR);
		}
	} 
}
//DeEnchant Spell

bool CDeEnchantSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CDeEnchantSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//end
bool CPoisonedSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CPoisonedSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// LowerStrength Spell

bool CLowerStrengthSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CLowerStrengthSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// StoneForm Spell

bool CStoneFormSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->GetSpellMgr().GetCastingInfo().ClearChant();

	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CStoneFormSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// BombSet Spell

void CBombSetSpell::Operate(CAggresiveCreature* lpAffected)
{
	if (0 == m_dwDurationSec)
	{
		AtType attackType;
		attackType.m_wType = 0x8708;	// Æø¹ß ½ºÅ³
		attackType.m_cSkillLockCount = m_wSpellLevel / 6;
		attackType.m_cSkillLevel = m_wSpellLevel % 6;

		unsigned char cDefenceJudge[AtNode::MAX_DEFENDER_NUM] = { 0, };

		m_pCaster->MultiAttack(attackType, 1, &lpAffected, cDefenceJudge, 
			lpAffected->GetCurrentPos(), 0, 12, Math::Const::PI * 2, Skill::Target::ENEMY);
	}
}

//-------------------------------------------------------------------------------------------------
// Fired Spell

void CFiredSpell::Operate(CAggresiveCreature* lpAffected)
{
	const short wDamage = static_cast<short>( (float)(m_wSpellLevel) * 1.5f );

	const short wThreatAmount = (lpAffected->GetStatus().m_nNowHP < wDamage) ? 
		lpAffected->GetStatus().m_nNowHP : wDamage;
	lpAffected->GetThreat().AddToThreatList(m_pCaster, wThreatAmount);		

	lpAffected->GetStatus().m_nNowHP = (lpAffected->GetStatus().m_nNowHP > wDamage) ? 
		lpAffected->GetStatus().m_nNowHP - wDamage : 0;

	if (0 == lpAffected->GetStatus().m_nNowHP)
	{
		lpAffected->Dead(m_pCaster);
		lpAffected->GetThreat().ClearAll();		
	} 

	DefenserNode node;

	node.m_wDamage = wDamage;
	node.m_dwCharID = lpAffected->GetCID();
	node.m_sCurrHP = lpAffected->GetStatus().m_nNowHP;
	node.m_sCurrMP = lpAffected->GetStatus().m_nNowMP;
	node.m_wMaxHP = lpAffected->GetStatus().m_StatusInfo.m_nMaxHP;
	node.m_wMaxMP = lpAffected->GetStatus().m_StatusInfo.m_nMaxMP;
	node.m_wMPHeal = 0;
	node.m_cJudge = ClientConstants::Judge_Fired;

	AtType attackType;
	attackType.m_wType = AtType::RIGHT_MELEE;

	CCell* lpCell = lpAffected->GetCellPos().m_lpCell;
	if (NULL != lpCell)
	{
		lpCell->SendAttackInfo(m_pCaster->GetCID(), attackType, 1, &node);
	}

	if (Creature::CT_PC == Creature::GetCreatureType(lpAffected->GetCID()))
	{
		AtType attackType;
		attackType.m_wType = AtType::RIGHT_MELEE;

		CCharacter* pAffectedCharacter = (CCharacter *)lpAffected;
		CGameClientDispatch* pCharacterDispatcher = pAffectedCharacter->GetDispatcher();
		if (NULL != pCharacterDispatcher)
		{
			GameClientSendPacket::SendCharAttacked(pCharacterDispatcher->GetSendStream(), m_pCaster, pAffectedCharacter, 
				attackType, 0, wDamage, ClientConstants::Judge_Fired, 0,  PktBase::NO_SERVER_ERR);
		}
	} 
}

bool CFiredSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CFiredSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// CurseOfBlind Spell

bool CCurseOfBlindSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CCurseOfBlindSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// KarterantWorldBuff Spell

bool CKarterantWorldBuffSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CKarterantWorldBuffSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//hz added this for killstreak buff
//-------------------------------------------------------------------------------------------------
// KarterantWorldBuff Spell

bool CKillStreakBuffSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CKillStreakBuffSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//hz end

//////////////////////////////////////////////////////////////////////
bool CGreenPotionSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CGreenPotionSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
bool CGreenPotion1Spell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CGreenPotion1Spell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
bool CGreenPotion2Spell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CGreenPotion2Spell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
bool CGreenPotion3Spell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CGreenPotion3Spell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// MerkadiaWorldBuff Spell

bool CMerkadiaWorldBuffSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CMerkadiaWorldBuffSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// KarterantWorldDeBuff Spell

bool CKarterantWorldDeBuffSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CKarterantWorldDeBuffSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// MerkadiaWorldDeBuff Spell

bool CMerkadiaWorldDeBuffSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CMerkadiaWorldDeBuffSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// PowerStatue Spell

bool CPowerStatueSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CPowerStatueSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// IntStatue Spell

bool CIntStatueSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CIntStatueSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// ExpStatue Spell

bool CExpStatueSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CExpStatueSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// WealthStatue Spell

bool CWealthStatueSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CWealthStatueSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}


//-------------------------------------------------------------------------------------------------
// RealmHP Spell

bool CRealmHPSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CRealmHPSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// RealmMP Spell

bool CRealmMPSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CRealmMPSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Hundred Spell

bool CHundredLevelSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CHundredLevelSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}

//-------------------------------------------------------------------------------------------------
// Hundred Spell

bool CDeCoolDownSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), m_wSpellLevel);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Activate(lpAffected, dwOperateFlag);
}

bool CDeCoolDownSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->SetEnchantLevel(static_cast<Skill::SpellID::Type>(m_wSpellID), 0);
	lpAffected->CalculateEnchantStatus();

	return CSpell::Deactivate(lpAffected, dwOperateFlag);
}