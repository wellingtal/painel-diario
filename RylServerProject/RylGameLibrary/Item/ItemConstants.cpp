#include "stdafx.h"
#include "ItemConstants.h"
#include "ItemStructure.h"

#include <algorithm>
#include <Item/Item.h>
#include "GMMemory.h"

using namespace Item;

CItemType& CItemType::GetInstance()
{
    static CItemType itemType;
    return itemType;
}

const CTypeName Attribute::Attributes[Attribute::MAX_ATTRIBUTE_NUM] = 
{
	CTypeName(Attribute::NONE,			"None"), 			
	CTypeName(Attribute::MIN_DAMAGE,	"MinDamage"), 		
	CTypeName(Attribute::MAX_DAMAGE,	"MaxDamage"), 
	CTypeName(Attribute::ARMOR,			"Armor"), 		
	CTypeName(Attribute::HIT_RATE,		"HitRate"), 
	CTypeName(Attribute::EVADE,			"Evade"), 
	CTypeName(Attribute::MAX_HP,		"MaxHP"), 
	CTypeName(Attribute::HP_REGEN,		"HPRegen"), 		
	CTypeName(Attribute::MAX_MP,		"MaxMP"), 
	CTypeName(Attribute::MP_REGEN,		"MPRegen"), 
    CTypeName(Attribute::CRITICAL,		"Critical"),
    CTypeName(Attribute::BLOCK,			"Block"), 		
    CTypeName(Attribute::SPEED,			"Speed"), 		
    CTypeName(Attribute::MAGIC_POWER,	"MagicPower"), 			
    CTypeName(Attribute::MAGIC_RESIST,	"MagicResist"),
    //--//	start..
	CTypeName(Attribute::LUCKCHANCE,	"LuckChance"),
//	CTypeName(Attribute::RUNE,			"Rune"),
	CTypeName(Attribute::COOLDOWN,		"CoolDown"),
	CTypeName(Attribute::SKILL_POINT,	"SkillPoint"),
	CTypeName(Attribute::FROST,			"Frost"),
	CTypeName(Attribute::FIRE,			"Fire"),
	CTypeName(Attribute::ELECTRO,		"Electro"),
//	CTypeName(Attribute::DARKNESS,		"Darkness")
	CTypeName(Attribute::ADD_STR,		"ADD_STR")
	//--//	end..
};


CItemType::CItemType()
{
    using namespace ItemType;

    int nCount = 0;

    for (nCount = 0; nCount < MAX_ARRAY_TYPE; ++nCount) 
    {
        m_ItemTypes[nCount].reserve(MAX_ITEM_TYPE);
    }

	m_ItemTypeNames[SHIRT]					= CTypeName(SHIRT, "SHIRT");
	m_ItemTypeNames[TUNIC]					= CTypeName(TUNIC, "TUNIC");

	m_ItemTypeNames[CON_ARMOUR]				= CTypeName(CON_ARMOUR, "CON_ARMOUR");
	m_ItemTypeNames[CON_HELM]				= CTypeName(CON_HELM, "CON_HELM");
	m_ItemTypeNames[CON_GLOVE]				= CTypeName(CON_GLOVE, "CON_GLOVE");
	m_ItemTypeNames[CON_BOOTS]				= CTypeName(CON_BOOTS, "CON_BOOTS");
	m_ItemTypeNames[DEX_ARMOUR]				= CTypeName(DEX_ARMOUR, "DEX_ARMOUR");
	m_ItemTypeNames[DEX_HELM]				= CTypeName(DEX_HELM, "DEX_HELM");
	m_ItemTypeNames[DEX_GLOVE]				= CTypeName(DEX_GLOVE, "DEX_GLOVE");
	m_ItemTypeNames[DEX_BOOTS]				= CTypeName(DEX_BOOTS, "DEX_BOOTS");

	m_ItemTypeNames[ONEHANDED_SWORD]		= CTypeName(ONEHANDED_SWORD, "ONEHANDED_SWORD");
	m_ItemTypeNames[TWOHANDED_SWORD]		= CTypeName(TWOHANDED_SWORD, "TWOHANDED_SWORD");
	m_ItemTypeNames[ONEHANDED_AXE]			= CTypeName(ONEHANDED_AXE, "ONEHANDED_AXE");
	m_ItemTypeNames[TWOHANDED_AXE]			= CTypeName(TWOHANDED_AXE, "TWOHANDED_AXE");
	m_ItemTypeNames[ONEHANDED_BLUNT]		= CTypeName(ONEHANDED_BLUNT, "ONEHANDED_BLUNT");
	m_ItemTypeNames[TWOHANDED_BLUNT]		= CTypeName(TWOHANDED_BLUNT, "TWOHANDED_BLUNT");
	m_ItemTypeNames[BOW]					= CTypeName(BOW, "BOW");
	m_ItemTypeNames[CROSSBOW]				= CTypeName(CROSSBOW, "CROSSBOW");
	m_ItemTypeNames[STAFF]					= CTypeName(STAFF, "STAFF");
	m_ItemTypeNames[DAGGER]					= CTypeName(DAGGER, "DAGGER");
	m_ItemTypeNames[SHIELD]					= CTypeName(SHIELD, "SHIELD");

	m_ItemTypeNames[CON_BODY]				= CTypeName(CON_BODY, "CON_BODY");
	m_ItemTypeNames[CON_HEAD]				= CTypeName(CON_HEAD, "CON_HEAD");
	m_ItemTypeNames[CON_PELVIS]				= CTypeName(CON_PELVIS, "CON_PELVIS");
	m_ItemTypeNames[CON_PROTECT_A]			= CTypeName(CON_PROTECT_A, "CON_PROTECT_A");
	m_ItemTypeNames[DEX_BODY]				= CTypeName(DEX_BODY, "DEX_BODY");
	m_ItemTypeNames[DEX_HEAD]				= CTypeName(DEX_HEAD, "DEX_HEAD");
	m_ItemTypeNames[DEX_PELVIS]				= CTypeName(DEX_PELVIS, "DEX_PELVIS");
	m_ItemTypeNames[DEX_PROTECT_A]			= CTypeName(DEX_PROTECT_A, "DEX_PROTECT_A");

	m_ItemTypeNames[COM_BLUNT]				= CTypeName(COM_BLUNT, "COM_BLUNT");
	m_ItemTypeNames[COM_SWORD]				= CTypeName(COM_SWORD, "COM_SWORD");
	m_ItemTypeNames[OPP_HAMMER]				= CTypeName(OPP_HAMMER, "OPP_HAMMER");
	m_ItemTypeNames[OPP_AXE]				= CTypeName(OPP_AXE, "OPP_AXE");
	m_ItemTypeNames[OPP_SLUSHER]			= CTypeName(OPP_SLUSHER, "OPP_SLUSHER");
	m_ItemTypeNames[OPP_TALON]				= CTypeName(OPP_TALON, "OPP_TALON");
	m_ItemTypeNames[OPP_SYTHE]				= CTypeName(OPP_SYTHE, "OPP_SYTHE");

	m_ItemTypeNames[SKILL_A_GUARD]			= CTypeName(SKILL_A_GUARD, "SKILL_A_GUARD");
	m_ItemTypeNames[SKILL_A_ATTACK]			= CTypeName(SKILL_A_ATTACK, "SKILL_A_ATTACK");
	m_ItemTypeNames[SKILL_A_GUN]			= CTypeName(SKILL_A_GUN, "SKILL_A_GUN");
	m_ItemTypeNames[SKILL_A_KNIFE]		    = CTypeName(SKILL_A_KNIFE, "SKILL_A_KNIFE");

	m_ItemTypeNames[ARROW]					= CTypeName(ARROW, "ARROW");
	m_ItemTypeNames[BOLT]					= CTypeName(BOLT, "BOLT");
	m_ItemTypeNames[AMMO]					= CTypeName(AMMO, "AMMO");

	m_ItemTypeNames[POTION]					= CTypeName(POTION, "POTION");
	m_ItemTypeNames[EXP_BOOK]				= CTypeName(EXP_BOOK, "EXPBOOK");
	m_ItemTypeNames[CASH_BOOK]				= CTypeName(CASH_BOOK, "CASHBOOK");
//	m_ItemTypeNames[MEDALBOOK]				= CTypeName(MEDALBOOK, "MEDALBOOK");
	m_ItemTypeNames[EVENT_LOTTERY]			= CTypeName(EVENT_LOTTERY, "EVENT_LOTTERY");
	m_ItemTypeNames[PORTAL]					= CTypeName(PORTAL, "PORTAL");
	m_ItemTypeNames[FIRE_CRACKER]			= CTypeName(FIRE_CRACKER, "FIRE_CRACKER");
	m_ItemTypeNames[SKILL_BOOK]				= CTypeName(SKILL_BOOK, "SKILLBOOK");
	m_ItemTypeNames[FIXED_PORTAL]			= CTypeName(FIXED_PORTAL, "FIXED_PORTAL");
	m_ItemTypeNames[SKILL_ITEM]				= CTypeName(SKILL_ITEM, "SKILL_ITEM");
	m_ItemTypeNames[CASH_ITEM]				= CTypeName(CASH_ITEM,	"CASH_ITEM");

	m_ItemTypeNames[GEM]					= CTypeName(GEM, "GEM");
	m_ItemTypeNames[ORE]					= CTypeName(ORE, "ORE");	
	m_ItemTypeNames[ETC]					= CTypeName(ETC, "ETC");

	m_ItemTypeNames[CAMP_KIT]				= CTypeName(CAMP_KIT, "CAMP_KIT");
	m_ItemTypeNames[SHORT_RANGE_ARMS_KIT]	= CTypeName(SHORT_RANGE_ARMS_KIT, "SHORT_RANGE_ARMS_KIT");
	m_ItemTypeNames[LONG_RANGE_ARMS_KIT]	= CTypeName(LONG_RANGE_ARMS_KIT, "LONG_RANGE_ARMS_KIT");
	m_ItemTypeNames[AIRSHIP_KIT]			= CTypeName(AIRSHIP_KIT, "AIRSHIP_KIT");
	m_ItemTypeNames[WORLDWEAPON_K_KIT]		= CTypeName(WORLDWEAPON_K_KIT, "WORLDWEAPON_K_KIT");
	m_ItemTypeNames[WORLDWEAPON_M_KIT]		= CTypeName(WORLDWEAPON_M_KIT, "WORLDWEAPON_M_KIT");
	m_ItemTypeNames[MINERAL_KIT]			= CTypeName(MINERAL_KIT, "MINERAL_KIT");

	m_ItemTypeNames[QUEST_ITEM]				= CTypeName(QUEST_ITEM, "QUEST_ITEM");

	//--//	start..
	m_ItemTypeNames[NECKLACE]				= CTypeName(NECKLACE, "NECKLACE");
	m_ItemTypeNames[RING]					= CTypeName(RING, "RING");
	m_ItemTypeNames[RUNE]					= CTypeName(RUNE, "RUNE");

	m_ItemTypeNames[GEM_SELL]				= CTypeName(GEM_SELL, "GEM_SELL");

	m_ItemTypeNames[RIDE]					= CTypeName(RIDE, "RIDE");
	m_ItemTypeNames[TRANSFORM]				= CTypeName(TRANSFORM, "TRANSFORM");

	//--//	end..

	// --------------------------------------------------------------------------------------------------------
	// ���

	m_ItemTypes[EQUIP_TYPE].push_back(SHIRT);				m_ItemTypes[EQUIP_TYPE].push_back(TUNIC);					

	m_ItemTypes[EQUIP_TYPE].push_back(CON_ARMOUR);			m_ItemTypes[EQUIP_TYPE].push_back(CON_HELM);				
	m_ItemTypes[EQUIP_TYPE].push_back(CON_GLOVE);			m_ItemTypes[EQUIP_TYPE].push_back(CON_BOOTS);				
	m_ItemTypes[EQUIP_TYPE].push_back(DEX_ARMOUR);			m_ItemTypes[EQUIP_TYPE].push_back(DEX_HELM);				
	m_ItemTypes[EQUIP_TYPE].push_back(DEX_GLOVE);			m_ItemTypes[EQUIP_TYPE].push_back(DEX_BOOTS);				

	m_ItemTypes[EQUIP_TYPE].push_back(ONEHANDED_SWORD);		m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_SWORD);		
	m_ItemTypes[EQUIP_TYPE].push_back(ONEHANDED_AXE);		m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_AXE);			
	m_ItemTypes[EQUIP_TYPE].push_back(ONEHANDED_BLUNT);		m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_BLUNT);		
	m_ItemTypes[EQUIP_TYPE].push_back(BOW);					m_ItemTypes[EQUIP_TYPE].push_back(CROSSBOW);				
	m_ItemTypes[EQUIP_TYPE].push_back(STAFF);				m_ItemTypes[EQUIP_TYPE].push_back(DAGGER);				
	m_ItemTypes[EQUIP_TYPE].push_back(SHIELD);							  

	m_ItemTypes[EQUIP_TYPE].push_back(CON_BODY);			m_ItemTypes[EQUIP_TYPE].push_back(CON_HEAD);				
	m_ItemTypes[EQUIP_TYPE].push_back(CON_PELVIS);			m_ItemTypes[EQUIP_TYPE].push_back(CON_PROTECT_A);			
	m_ItemTypes[EQUIP_TYPE].push_back(DEX_BODY);			m_ItemTypes[EQUIP_TYPE].push_back(DEX_HEAD);				
	m_ItemTypes[EQUIP_TYPE].push_back(DEX_PELVIS);			m_ItemTypes[EQUIP_TYPE].push_back(DEX_PROTECT_A);			

	m_ItemTypes[EQUIP_TYPE].push_back(COM_BLUNT);			m_ItemTypes[EQUIP_TYPE].push_back(COM_SWORD);				
	m_ItemTypes[EQUIP_TYPE].push_back(OPP_HAMMER);			m_ItemTypes[EQUIP_TYPE].push_back(OPP_AXE);				
	m_ItemTypes[EQUIP_TYPE].push_back(OPP_SLUSHER);			m_ItemTypes[EQUIP_TYPE].push_back(OPP_TALON);				
	m_ItemTypes[EQUIP_TYPE].push_back(OPP_SYTHE);							

	m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_GUARD);		m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_ATTACK);		
	m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_GUN);			m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_KNIFE);			

	m_ItemTypes[EQUIP_TYPE].push_back(ARROW);				m_ItemTypes[EQUIP_TYPE].push_back(BOLT);					

	// --------------------------------------------------------------------------------------------------------
    // ��� ������

	// źȯ�� '���'�� �ƴ� '��� ������'���� ����մϴ�.
	m_ItemTypes[USE_ITEM_TYPE].push_back(AMMO);

    m_ItemTypes[USE_ITEM_TYPE].push_back(POTION);			m_ItemTypes[USE_ITEM_TYPE].push_back(EXP_BOOK);    
    m_ItemTypes[USE_ITEM_TYPE].push_back(CASH_BOOK);		//m_ItemTypes[USE_ITEM_TYPE].push_back(MEDALBOOK);
	m_ItemTypes[USE_ITEM_TYPE].push_back(EVENT_LOTTERY);
	m_ItemTypes[USE_ITEM_TYPE].push_back(PORTAL);			m_ItemTypes[USE_ITEM_TYPE].push_back(FIRE_CRACKER);
	m_ItemTypes[USE_ITEM_TYPE].push_back(SKILL_BOOK);		m_ItemTypes[USE_ITEM_TYPE].push_back(FIXED_PORTAL);			
	m_ItemTypes[USE_ITEM_TYPE].push_back(SKILL_ITEM);		m_ItemTypes[USE_ITEM_TYPE].push_back(CASH_ITEM);	

	m_ItemTypes[USE_ITEM_TYPE].push_back(CAMP_KIT);			
	m_ItemTypes[USE_ITEM_TYPE].push_back(SHORT_RANGE_ARMS_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(LONG_RANGE_ARMS_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(AIRSHIP_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(WORLDWEAPON_K_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(WORLDWEAPON_M_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(MINERAL_KIT);

	m_ItemTypes[USE_ITEM_TYPE].push_back(QUEST_ITEM);

	// --------------------------------------------------------------------------------------------------------
    // ����

	m_ItemTypes[WEAPON_TYPE].push_back(ONEHANDED_SWORD);	m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_SWORD);		
	m_ItemTypes[WEAPON_TYPE].push_back(ONEHANDED_AXE);		m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_AXE);			
	m_ItemTypes[WEAPON_TYPE].push_back(ONEHANDED_BLUNT);	m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_BLUNT);		
	m_ItemTypes[WEAPON_TYPE].push_back(BOW);				m_ItemTypes[WEAPON_TYPE].push_back(CROSSBOW);				
	m_ItemTypes[WEAPON_TYPE].push_back(STAFF);				m_ItemTypes[WEAPON_TYPE].push_back(DAGGER);				

	m_ItemTypes[WEAPON_TYPE].push_back(COM_BLUNT);			m_ItemTypes[WEAPON_TYPE].push_back(COM_SWORD);				
	m_ItemTypes[WEAPON_TYPE].push_back(OPP_HAMMER);			m_ItemTypes[WEAPON_TYPE].push_back(OPP_AXE);				
	m_ItemTypes[WEAPON_TYPE].push_back(OPP_SLUSHER);		m_ItemTypes[WEAPON_TYPE].push_back(OPP_TALON);				
	m_ItemTypes[WEAPON_TYPE].push_back(OPP_SYTHE);							

	// --------------------------------------------------------------------------------------------------------
	// ��ų��

	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_GUARD);  	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_ATTACK);
	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_GUN);    	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_KNIFE);      

	// --------------------------------------------------------------------------------------------------------
	// �Ʒ��� �з��� ���� �� �ִ� �ɼǿ� ���� ������.

	// ������
	m_ItemTypes[HELM_TYPE].push_back(CON_HELM);				m_ItemTypes[HELM_TYPE].push_back(DEX_HELM);		
	m_ItemTypes[HELM_TYPE].push_back(CON_HEAD);				m_ItemTypes[HELM_TYPE].push_back(DEX_HEAD);		

	// edith �������� �尩,�Ź�,�����,��ü�� �߰� (�ɼ��� ���ü��ְ��ϱ�����)
	m_ItemTypes[HELM_TYPE].push_back(CON_GLOVE);			m_ItemTypes[HELM_TYPE].push_back(CON_BOOTS);		
	m_ItemTypes[HELM_TYPE].push_back(DEX_GLOVE);			m_ItemTypes[HELM_TYPE].push_back(DEX_BOOTS);		
	m_ItemTypes[HELM_TYPE].push_back(CON_PELVIS);			m_ItemTypes[HELM_TYPE].push_back(CON_PROTECT_A);		
	m_ItemTypes[HELM_TYPE].push_back(DEX_PELVIS);			m_ItemTypes[HELM_TYPE].push_back(DEX_PROTECT_A);		


    // ���ʷ�
	m_ItemTypes[ARMOUR_TYPE].push_back(CON_ARMOUR);			m_ItemTypes[ARMOUR_TYPE].push_back(DEX_ARMOUR);		
	m_ItemTypes[ARMOUR_TYPE].push_back(CON_BODY);			m_ItemTypes[ARMOUR_TYPE].push_back(DEX_BODY);		

    // �ܰ˷�
	m_ItemTypes[DAGGER_TYPE].push_back(DAGGER);				m_ItemTypes[DAGGER_TYPE].push_back(OPP_TALON);		
	m_ItemTypes[DAGGER_TYPE].push_back(SKILL_A_KNIFE);		

	// �Ѽ� �����
	m_ItemTypes[ONEHANDED_TYPE].push_back(ONEHANDED_SWORD);	m_ItemTypes[ONEHANDED_TYPE].push_back(ONEHANDED_AXE);		
	m_ItemTypes[ONEHANDED_TYPE].push_back(ONEHANDED_BLUNT);	m_ItemTypes[ONEHANDED_TYPE].push_back(COM_BLUNT);		
	m_ItemTypes[ONEHANDED_TYPE].push_back(COM_SWORD);		m_ItemTypes[ONEHANDED_TYPE].push_back(OPP_HAMMER);		
	m_ItemTypes[ONEHANDED_TYPE].push_back(OPP_AXE);			m_ItemTypes[ONEHANDED_TYPE].push_back(OPP_SLUSHER);		
	m_ItemTypes[ONEHANDED_TYPE].push_back(SKILL_A_ATTACK);		

	// ��Ÿ� �����
	m_ItemTypes[ONEHANDED_TYPE].push_back(SKILL_A_GUN);		m_ItemTypes[LONGRANGE_TYPE].push_back(BOW);		
	m_ItemTypes[LONGRANGE_TYPE].push_back(CROSSBOW);		

	// ��� �����
	m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_SWORD);	m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_AXE);		
	m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_BLUNT);		

	// �����̷�
	m_ItemTypes[STAFF_TYPE].push_back(STAFF);				m_ItemTypes[STAFF_TYPE].push_back(OPP_SYTHE);		

	// ���з�
	m_ItemTypes[SHIELD_TYPE].push_back(SHIELD);				m_ItemTypes[SHIELD_TYPE].push_back(SKILL_A_GUARD);		

	// --------------------------------------------------------------------------------------------------------
	// Standard-Option Type

	m_ItemTypes[STANDARD_TYPE].push_back(CON_ARMOUR);		m_ItemTypes[STANDARD_TYPE].push_back(DEX_ARMOUR);		
	m_ItemTypes[STANDARD_TYPE].push_back(CON_BODY);			m_ItemTypes[STANDARD_TYPE].push_back(DEX_BODY);		
	m_ItemTypes[STANDARD_TYPE].push_back(SHIELD);			m_ItemTypes[STANDARD_TYPE].push_back(SKILL_A_GUARD);		
	m_ItemTypes[STANDARD_TYPE].push_back(ONEHANDED_SWORD);	m_ItemTypes[STANDARD_TYPE].push_back(TWOHANDED_SWORD);
	m_ItemTypes[STANDARD_TYPE].push_back(ONEHANDED_AXE);	m_ItemTypes[STANDARD_TYPE].push_back(TWOHANDED_AXE);			
	m_ItemTypes[STANDARD_TYPE].push_back(ONEHANDED_BLUNT);	m_ItemTypes[STANDARD_TYPE].push_back(TWOHANDED_BLUNT);		
	m_ItemTypes[STANDARD_TYPE].push_back(BOW);				m_ItemTypes[STANDARD_TYPE].push_back(CROSSBOW);		
	m_ItemTypes[STANDARD_TYPE].push_back(STAFF);			m_ItemTypes[STANDARD_TYPE].push_back(DAGGER);				
	m_ItemTypes[STANDARD_TYPE].push_back(COM_BLUNT);		m_ItemTypes[STANDARD_TYPE].push_back(COM_SWORD);		
	m_ItemTypes[STANDARD_TYPE].push_back(OPP_HAMMER);		m_ItemTypes[STANDARD_TYPE].push_back(OPP_AXE);			
	m_ItemTypes[STANDARD_TYPE].push_back(OPP_SLUSHER);		m_ItemTypes[STANDARD_TYPE].push_back(OPP_TALON);		
	m_ItemTypes[STANDARD_TYPE].push_back(OPP_SYTHE);		m_ItemTypes[STANDARD_TYPE].push_back(SKILL_A_ATTACK);		
	m_ItemTypes[STANDARD_TYPE].push_back(SKILL_A_GUN);		m_ItemTypes[STANDARD_TYPE].push_back(SKILL_A_KNIFE);		

	// edith 2008.01.14 ���Ĵٵ�ɼ����� ������ �����ϱ� ����� �ɷ���.
	m_ItemTypes[STANDARD_TYPE].push_back(CON_HELM);			m_ItemTypes[STANDARD_TYPE].push_back(DEX_HELM);		
	m_ItemTypes[STANDARD_TYPE].push_back(CON_HEAD);			m_ItemTypes[STANDARD_TYPE].push_back(DEX_HEAD);		
	m_ItemTypes[STANDARD_TYPE].push_back(CON_GLOVE);		m_ItemTypes[STANDARD_TYPE].push_back(CON_BOOTS);				
	m_ItemTypes[STANDARD_TYPE].push_back(DEX_GLOVE);		m_ItemTypes[STANDARD_TYPE].push_back(DEX_BOOTS);				
	m_ItemTypes[STANDARD_TYPE].push_back(CON_PELVIS);		m_ItemTypes[STANDARD_TYPE].push_back(CON_PROTECT_A);			
	m_ItemTypes[STANDARD_TYPE].push_back(DEX_PELVIS);		m_ItemTypes[STANDARD_TYPE].push_back(DEX_PROTECT_A);			

	// --------------------------------------------------------------------------------------------------------
	// Over-Option Type
/*
	m_ItemTypes[OVER_TYPE].push_back(CON_HELM);				m_ItemTypes[OVER_TYPE].push_back(DEX_HELM);		
	m_ItemTypes[OVER_TYPE].push_back(CON_HEAD);				m_ItemTypes[OVER_TYPE].push_back(DEX_HEAD);		

	// --------------------------------------------------------------------------------------------------------
	// No-Option Type

	m_ItemTypes[NO_TYPE].push_back(CON_GLOVE);				m_ItemTypes[NO_TYPE].push_back(CON_BOOTS);				
	m_ItemTypes[NO_TYPE].push_back(DEX_GLOVE);				m_ItemTypes[NO_TYPE].push_back(DEX_BOOTS);				
	m_ItemTypes[NO_TYPE].push_back(CON_PELVIS);				m_ItemTypes[NO_TYPE].push_back(CON_PROTECT_A);			
	m_ItemTypes[NO_TYPE].push_back(DEX_PELVIS);				m_ItemTypes[NO_TYPE].push_back(DEX_PROTECT_A);			
*/
	// No-Option Type
//	m_ItemTypes[NO_TYPE].push_back(RIDE);					m_ItemTypes[NO_TYPE].push_back(TRANSFORM);			

	// --------------------------------------------------------------------------------------------------------

	//--//	start..
	// --------------------------------------------------------------------------------------------------------
	// Accessory Type
	m_ItemTypes[ACCESSORY_TYPE].push_back(NECKLACE);		m_ItemTypes[ACCESSORY_TYPE].push_back(RING);
	m_ItemTypes[ACCESSORY_TYPE].push_back(RUNE);
	// --------------------------------------------------------------------------------------------------------
	//--//	end..

	// �ƹ�Ÿ���� ������
	m_ItemTypes[AVATA_TYPE].push_back(RIDE);				m_ItemTypes[AVATA_TYPE].push_back(TRANSFORM);		


	for (nCount = 0; nCount < MAX_ARRAY_TYPE; ++nCount)
    {
        std::sort(m_ItemTypes[nCount].begin(), m_ItemTypes[nCount].end());
    }

	// --------------------------------------------------------------------------------------------------------
	// ���Ѱ� �̸�
	m_ItemLimitNames[StatusLimit::NONE]	= CTypeName(StatusLimit::NONE, "NONE");
	m_ItemLimitNames[StatusLimit::STR]	= CTypeName(StatusLimit::STR, "STR");
	m_ItemLimitNames[StatusLimit::DEX]	= CTypeName(StatusLimit::DEX, "DEX");
	m_ItemLimitNames[StatusLimit::CON]	= CTypeName(StatusLimit::CON, "CON");
	m_ItemLimitNames[StatusLimit::INT]	= CTypeName(StatusLimit::INT, "INT");
	m_ItemLimitNames[StatusLimit::WIS]	= CTypeName(StatusLimit::WIS, "WIS");
	m_ItemLimitNames[StatusLimit::LEVEL] = CTypeName(StatusLimit::LEVEL, "LEVEL");	//--//
	// --------------------------------------------------------------------------------------------------------

	//--//	start..
	// --------------------------------------------------------------------------------------------------------
	// �׷��̵� �̸�
	m_ItemGradeNames[EquipType::S_GRADE]	= CTypeName(EquipType::S_GRADE,	"S");
	m_ItemGradeNames[EquipType::AAA_GRADE]	= CTypeName(EquipType::AAA_GRADE,	"AAA");
	m_ItemGradeNames[EquipType::AA_GRADE]	= CTypeName(EquipType::AA_GRADE,	"AA");
	m_ItemGradeNames[EquipType::A_GRADE]	= CTypeName(EquipType::A_GRADE,		"A");
	m_ItemGradeNames[EquipType::B_GRADE]	= CTypeName(EquipType::B_GRADE,		"B");
	m_ItemGradeNames[EquipType::C_GRADE]	= CTypeName(EquipType::C_GRADE,		"C");
	m_ItemGradeNames[EquipType::D_GRADE]	= CTypeName(EquipType::D_GRADE,		"D");
	m_ItemGradeNames[EquipType::F_GRADE]	= CTypeName(EquipType::F_GRADE,		"F");
	m_ItemGradeNames[EquipType::X_GRADE]	= CTypeName(EquipType::X_GRADE,		"X");
	// --------------------------------------------------------------------------------------------------------
	//--//	end..
}

bool CItemType::IsCorrectItemType(ArrayType itemType, unsigned char cEquipType)
{
    return std::binary_search(m_ItemTypes[itemType].begin(),
        m_ItemTypes[itemType].end(), cEquipType);
}

bool CItemType::IsCorrectOptionType(EquipType::OptionType eOptionType, ItemType::Type eItemType)
{
	switch (eOptionType)
	{
		case EquipType::STANDARD_OPTION:	return IsCorrectItemType(STANDARD_TYPE, eItemType);
		case EquipType::OVER_OPTION:		return IsCorrectItemType(OVER_TYPE, eItemType);
		case EquipType::NO_OPTION:			return IsCorrectItemType(NO_TYPE, eItemType);
	}

	return false;
}

EquipType::OptionType CItemType::GetOptionType(ItemType::Type eItemType)
{
	if (true == IsCorrectOptionType(EquipType::STANDARD_OPTION, eItemType))
	{
		return EquipType::STANDARD_OPTION;
	}

	if (true == IsCorrectOptionType(EquipType::OVER_OPTION, eItemType))
	{
		return EquipType::OVER_OPTION;
	}

	if (true == IsCorrectOptionType(EquipType::NO_OPTION, eItemType))
	{
		return EquipType::NO_OPTION;
	}

	return EquipType::NONE;	
}

CItemType::ArrayType CItemType::GetEquipType(unsigned long dwItemFlag)
{
	// ���� �ɼ� �ٴ� �ֵ�
	if (dwItemFlag & DetailData::HELM)			{ return HELM_TYPE;			}
	if (dwItemFlag & DetailData::ARMOR)			{ return ARMOUR_TYPE;		}
	if (dwItemFlag & DetailData::DAGGER)		{ return DAGGER_TYPE;		}
	if (dwItemFlag & DetailData::ONEHANDED)		{ return ONEHANDED_TYPE;	}
	if (dwItemFlag & DetailData::LONGRANGE)		{ return LONGRANGE_TYPE;	}
	if (dwItemFlag & DetailData::TWOHANDED)		{ return TWOHANDED_TYPE;	}
	if (dwItemFlag & DetailData::STAFF)			{ return STAFF_TYPE;		}
	if (dwItemFlag & DetailData::SHIELD)		{ return SHIELD_TYPE;		}

	return MAX_ARRAY_TYPE;
}

void CItemType::SetEquipTypeFlags(ItemInfo& itemInfo)
{
    using namespace ItemType;

    switch (itemInfo.m_DetailData.m_cItemType)
    {
		case ARROW:
		case BOLT:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::STACKABLE;
			break;
    }
   
	if (IsCorrectItemType(HELM_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::HELM;
	}  
	else if (IsCorrectItemType(ARMOUR_TYPE, itemInfo.m_DetailData.m_cItemType))
    {
        itemInfo.m_DetailData.m_dwFlags |= DetailData::ARMOR;
    }  
	else if (IsCorrectItemType(DAGGER_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::DAGGER;
	}
	else if (IsCorrectItemType(ONEHANDED_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::ONEHANDED;
	}
	else if (IsCorrectItemType(LONGRANGE_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::LONGRANGE;
	}
	else if (IsCorrectItemType(TWOHANDED_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::TWOHANDED;
	}
	else if (IsCorrectItemType(STAFF_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::STAFF;
	}
	else if (IsCorrectItemType(SHIELD_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::SHIELD;
	}

    itemInfo.m_DetailData.m_dwFlags |= DetailData::EQUIP;
}

//--//	start..
void CItemType::SetAccessoryTypeFlags(ItemInfo& itemInfo)
{
	using namespace ItemType;

	switch (itemInfo.m_DetailData.m_cItemType)
	{
		case NECKLACE:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::NECKLACE;
			itemInfo.m_DetailData.m_dwFlags |= DetailData::EQUIP;
			break;

		case RING:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::RING;
			itemInfo.m_DetailData.m_dwFlags |= DetailData::EQUIP;
			break;

		case RUNE:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::RUNE;
			itemInfo.m_DetailData.m_dwFlags |= DetailData::STACKABLE;
			break;
	}
}
//--//	end.

void CItemType::SetAvataTypeFlags(ItemInfo& itemInfo)
{
	using namespace ItemType;

	switch (itemInfo.m_DetailData.m_cItemType)
	{
		case RIDE:
		case TRANSFORM:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::AVATA;
			itemInfo.m_DetailData.m_dwFlags |= DetailData::EQUIP;
			break;
	}
}

void CItemType::SetUseItemTypeFlags(ItemInfo& itemInfo)
{
    using namespace ItemType;

    switch (itemInfo.m_DetailData.m_cItemType)
    {
		case POTION:
		case PORTAL:
		case FIRE_CRACKER:
		case FIXED_PORTAL:
		case SKILL_ITEM:
			itemInfo.m_DetailData.m_dwFlags |= DetailData::QUICKSLOT_IN;
			break;
    }

    if (itemInfo.m_DetailData.m_cMaxDurabilityOrStack > 1)
    {
        itemInfo.m_DetailData.m_dwFlags |= DetailData::STACKABLE;
    }

    itemInfo.m_DetailData.m_dwFlags |= DetailData::USE_ITEM;
}


void CItemType::SetEtcItemTypeFlags(ItemInfo& itemInfo)
{
    if (itemInfo.m_DetailData.m_cMaxDurabilityOrStack > 1)
    {
        itemInfo.m_DetailData.m_dwFlags |= DetailData::STACKABLE;
    }
}

using namespace Attribute;
typedef ItemAttribute Gem;
typedef ItemAttribute Upg;

	//***************************************************************************************************************************************//
	//																																		
	//												Gem Attributes    -      BlackDog	
	//
	//
void CItemType::SetInstallGemAttribute(CItem* lpItem) 
{   /** armor   block	def	evd	speed	hp	hprec	magicres  **/
	ItemAttribute aryArmorGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {		
		//	Topaz	I 				Blackmoon				Pearl				Amber					Ruby				Sapphire				GARNET			//	//	ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber			ALMIGHTY Ruby			ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(BLOCK, 5),			Gem(ARMOR, 5),			Gem(SPEED, 3),		Gem(EVADE, 8),			Gem(MAX_HP, 4),		Gem(HP_REGEN, 5),		Gem(MAGIC_RESIST, 8),	Gem(BLOCK, 30),			Gem(ARMOR, 30),			Gem(SPEED, 18),			Gem(EVADE, 48),			Gem(MAX_HP, 24),		Gem(HP_REGEN, 30),		Gem(MAGIC_RESIST, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(ARMOR, 3),			Gem(BLOCK, 3),			Gem(ARMOR, 3),			Gem(ARMOR, 3),			Gem(ARMOR, 3),			Gem(ARMOR, 3),			Gem(ARMOR, 3),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(SPEED, 1),			Gem(SPEED, 1),			Gem(BLOCK, 3),			Gem(BLOCK, 3),			Gem(BLOCK, 3),			Gem(BLOCK, 3),			Gem(BLOCK, 3),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(EVADE, 3),			Gem(EVADE, 3),			Gem(EVADE, 3),			Gem(SPEED, 1),			Gem(SPEED, 1),			Gem(SPEED, 1),			Gem(SPEED, 1),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAX_HP, 3),			Gem(MAX_HP, 3),			Gem(MAX_HP, 3),			Gem(MAX_HP, 3),			Gem(HP_REGEN, 3),		Gem(MAX_HP, 3),			Gem(MAX_HP, 3),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(HP_REGEN, 3),		Gem(HP_REGEN, 3),		Gem(HP_REGEN, 3),		Gem(HP_REGEN, 3),		Gem(MAGIC_RESIST, 3),	Gem(MAGIC_RESIST, 3),	Gem(HP_REGEN, 3),		Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAGIC_RESIST, 3),	Gem(MAGIC_RESIST, 3),	Gem(MAGIC_RESIST, 3),	Gem(MAGIC_RESIST, 3),	Gem(EVADE, 3),			Gem(EVADE, 3),			Gem(EVADE, 3),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};

	ItemAttribute aryDaggerGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {							
			//	Topaz					Blackmoon				Pearl				Amber					Ruby				Sapphire				GARNET			////ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber		ALMIGHTY Ruby				ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(MIN_DAMAGE, 3),		Gem(MAX_DAMAGE, 5),		Gem(CRITICAL, 3),	Gem(HIT_RATE, 8),		Gem(MAX_MP, 4),		Gem(MP_REGEN, 5),		Gem(MAGIC_POWER, 8),	Gem(MIN_DAMAGE, 18),	Gem(MAX_DAMAGE, 28),	Gem(CRITICAL, 18),		Gem(HIT_RATE, 48),	Gem(MAX_MP, 24),			Gem(MP_REGEN, 30),		Gem(MAGIC_POWER, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(MAX_DAMAGE, 3),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAX_DAMAGE, 22),	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};


	ItemAttribute aryOneHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {					
		//	Topaz					Blackmoon				Pearl				Amber					Ruby				Sapphire				GARNET			//	//	ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber		ALMIGHTY Ruby				ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(MIN_DAMAGE, 5),		Gem(MAX_DAMAGE, 8),		Gem(CRITICAL, 3),	Gem(HIT_RATE, 8),		Gem(MAX_MP, 4),		Gem(MP_REGEN, 5),		Gem(MAGIC_POWER, 8),	Gem(MIN_DAMAGE, 30),	Gem(MAX_DAMAGE, 38),	Gem(CRITICAL, 18),		Gem(HIT_RATE, 48),	Gem(MAX_MP, 24),			Gem(MP_REGEN, 30),		Gem(MAGIC_POWER, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(MAX_DAMAGE, 5),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAX_DAMAGE, 30),	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};

	ItemAttribute aryTwoHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {							
		//	Topaz					Blackmoon			 	Pearl				Amber					Ruby				Sapphire				GARNET			//	//	ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber		ALMIGHTY Ruby				ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(MIN_DAMAGE, 6),		Gem(MAX_DAMAGE, 9),		Gem(CRITICAL, 3),	Gem(HIT_RATE, 8),		Gem(MAX_MP, 4),		Gem(MP_REGEN, 5),		Gem(MAGIC_POWER, 8),	Gem(MIN_DAMAGE, 36),	Gem(MAX_DAMAGE, 44),	Gem(CRITICAL, 18),		Gem(HIT_RATE, 48),	Gem(MAX_MP, 24),			Gem(MP_REGEN, 30),		Gem(MAGIC_POWER, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(MAX_DAMAGE, 6),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAX_DAMAGE, 36),	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};
		
	ItemAttribute aryStaffGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {								
		//	Topaz					Blackmoon				Pearl				Amber					Ruby				Sapphire				GARNET			//	//	ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber		ALMIGHTY Ruby				ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(MIN_DAMAGE, 5),		Gem(MAX_DAMAGE, 7),		Gem(CRITICAL, 3),	Gem(HIT_RATE, 8),		Gem(MAX_MP, 4),		Gem(MP_REGEN, 5),		Gem(MAGIC_POWER, 8),	Gem(MIN_DAMAGE, 30),	Gem(MAX_DAMAGE, 36),	Gem(CRITICAL, 18),		Gem(HIT_RATE, 48),	Gem(MAX_MP, 24),			Gem(MP_REGEN, 30),		Gem(MAGIC_POWER, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(MAX_DAMAGE, 5),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(MAX_DAMAGE, 30),	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};

	ItemAttribute aryShieldGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {			
		//	Topaz	I 				Blackmoon				Pearl				Amber					Ruby				Sapphire				GARNET			//	//	ALMIGHTY Topaz			ALMIGHTY Blackmoon		ALMIGHTY Pearl			ALMIGHTY Amber		ALMIGHTY Ruby				ALMIGHTY Sapphire		ALMIGHTY GARNET					Fehu       //Highway Plague edit			
		{	Gem(BLOCK, 5),			Gem(ARMOR, 5),			Gem(SPEED, 3),		Gem(EVADE, 8),			Gem(MAX_HP, 4),		Gem(HP_REGEN, 5),		Gem(MAGIC_RESIST, 8),	Gem(BLOCK, 30),			Gem(ARMOR, 30),			Gem(SPEED, 18),			Gem(EVADE, 48),		Gem(MAX_HP, 24),			Gem(HP_REGEN, 30),		Gem(MAGIC_RESIST, 48),	Gem(LUCKCHANCE, 1)	},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),			Gem(NONE, 0)				},
	};

	//***************************************************************************************************************************************//
	//
	//
	//
	//
	//


// ryl2 avalansa

// edith 2008.03.02 ��������
/*void CItemType::SetInstallGemAttribute(CItem* lpItem)
{
	ItemAttribute aryArmorGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon				
		{	Gem(ARMOR, 2),				Gem(MAX_HP, 5),				Gem(HP_REGEN, 1),		Gem(MAGIC_RESIST, 1),		Gem(EVADE, 1)},			
		{	Gem(NONE, 0),				Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}		
	};
		//stats added for each Gem added in Sockets  - Dagger
	ItemAttribute aryDaggerGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon								
		{	Gem(MIN_DAMAGE, 1),			Gem(MAX_MP, 5),				Gem(MP_REGEN, 1),		Gem(CRITICAL, 1),			Gem(HIT_RATE, 4)}, 		
		{	Gem(MAX_DAMAGE, 1),			Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}			
	};
		//stats added for each Gem added in Sockets   - OneHandedWeapon
	ItemAttribute aryOneHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon					
		{	Gem(MIN_DAMAGE, 2),			Gem(MAX_MP, 5),				Gem(MP_REGEN, 1),		Gem(CRITICAL, 1),			Gem(HIT_RATE,4)},		
		{	Gem(MAX_DAMAGE, 2),			Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}			
	};
		//stats added for each Gem added in Sockets  - TwoHandedWeapon
	ItemAttribute aryTwoHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon							
		{	Gem(MIN_DAMAGE, 3),			Gem(MAX_MP, 5),				Gem(MP_REGEN, 1),		Gem(CRITICAL, 1),			Gem(HIT_RATE, 4)}, 		
		{	Gem(MAX_DAMAGE, 3),			Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}	
	};
		//stats added for each Gem added in Sockets  - Staff
	ItemAttribute aryStaffGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon									
		{	Gem(MAX_DAMAGE, 2),			Gem(MAX_MP, 10),			Gem(MP_REGEN, 2),		Gem(CRITICAL, 1),			Gem(HIT_RATE, 4)},		
		{	Gem(MAGIC_POWER, 4),		Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}		
	};
		//stats added for each Gem added in Sockets  - Shield
	ItemAttribute aryShieldGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Ruby						Beryl						Sapphire				Diamond						Black-moon				
		{	Gem(MAX_HP, 5),				Gem(MAX_MP, 5),				Gem(MP_REGEN, 1),		Gem(BLOCK, 1),				Gem(HP_REGEN, 1)}, 		
		{	Gem(NONE, 0),				Gem(NONE, 0),				Gem(NONE, 0),			Gem(NONE, 0),				Gem(NONE, 0)}   			
	};

	ItemAttribute aryRuneGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Fehu						Othila						Jera					dummy~
		{	Gem(SKILL_POINT, 1),			Gem(SKILL_POINT, 2),					Gem(SKILL_POINT, 3),			Gem(SKILL_POINT, 4),				Gem(SKILL_POINT, 5),	}, 
	};
	*/
/*
void CItemType::SetInstallGemAttribute(CItem* lpItem)
{
	ItemAttribute aryArmorGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	������					����					����				ȣ��					���				�����̾�			����				����Ƹ���			���̾Ƹ��			����				����				�ڼ���					���޶���				�丮��Ʈ
		//	������					����					����				ȸ��					�ִ�HP				�ִ�MP				HPȸ��				MPȸ��				ũ��Ƽ��			���				���ǵ�				����ٿ�				������					��������			
		{	Gem(ARMOR, 1),			Gem(ARMOR, 2),			Gem(MAX_HP, 1),		Gem(EVADE, 2),			Gem(MAX_HP, 2),		Gem(MAX_MP, 2),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 2),	Gem(MAX_MP, 1),		Gem(BLOCK, 2),		Gem(SPEED, 2),		Gem(MAGIC_POWER, 1),	Gem(MAGIC_POWER, 2),	Gem(MAGIC_RESIST, 2)		},
		{	Gem(EVADE, 1),			Gem(NONE, 0),			Gem(HP_REGEN, 1),	Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(MP_REGEN, 1),	Gem(NONE, 0),		Gem(NONE, 0),		Gem(MAGIC_RESIST, 1),	Gem(NONE, 0),			Gem(NONE, 0)				}
	};

	ItemAttribute aryDaggerGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	������					����					����				ȣ��					���				�����̾�			����				����Ƹ���			���̾Ƹ��			����				����				�ڼ���					���޶���				�丮��Ʈ
		//	������					����					����				ȸ��					�ִ�HP				�ִ�MP				HPȸ��				MPȸ��				ũ��Ƽ��			���				���ǵ�				����ٿ�				������					��������			
		{	Gem(MIN_DAMAGE, 1),		Gem(MAX_DAMAGE, 2),		Gem(HIT_RATE, 4),	Gem(HIT_RATE, 2),		Gem(MAX_HP, 2),		Gem(MAX_MP, 2),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 2),	Gem(CRITICAL, 2),	Gem(CRITICAL, 1),	Gem(MAX_HP, 1),		Gem(MAX_MP, 1),			Gem(MAGIC_POWER, 1),	Gem(MAGIC_RESIST, 1)	},
		{	Gem(MAX_DAMAGE, 1),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(MAX_DAMAGE, 1),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(HIT_RATE, 2),	Gem(HP_REGEN, 1),	Gem(MP_REGEN, 1),		Gem(NONE, 0),			Gem(NONE, 0)	}
	};

	ItemAttribute aryOneHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		{	Gem(MIN_DAMAGE, 2),		Gem(MIN_DAMAGE, 1),		Gem(HIT_RATE, 4),	Gem(HIT_RATE, 2),		Gem(MAX_HP, 2),		Gem(MAX_MP, 2),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 2),	Gem(CRITICAL, 2),	Gem(CRITICAL, 1),	Gem(MAX_HP, 1),		Gem(MAX_MP, 1),			Gem(MAGIC_POWER, 1),	Gem(MAGIC_RESIST, 1)	},
		{	Gem(MAX_DAMAGE, 2),		Gem(MAX_DAMAGE, 3),		Gem(NONE, 0),		Gem(MAX_DAMAGE, 2),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(HIT_RATE, 2),	Gem(HP_REGEN, 1),	Gem(MP_REGEN, 1),		Gem(NONE, 0),			Gem(NONE, 0)	}
	};

	ItemAttribute aryTwoHandedWeaponGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		{	Gem(MIN_DAMAGE, 3),		Gem(MIN_DAMAGE, 2),		Gem(HIT_RATE, 4),	Gem(HIT_RATE, 2),		Gem(MAX_HP, 2),		Gem(MAX_MP, 2),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 2),	Gem(CRITICAL, 2),	Gem(CRITICAL, 1),	Gem(MAX_HP, 1),		Gem(MAX_MP, 1),			Gem(MAGIC_POWER, 1),	Gem(MAGIC_RESIST, 1)	},
		{	Gem(MAX_DAMAGE, 3),		Gem(MAX_DAMAGE, 4),		Gem(NONE, 0),		Gem(MAX_DAMAGE, 3),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(HIT_RATE, 2),	Gem(HP_REGEN, 1),	Gem(MP_REGEN, 1),		Gem(NONE, 0),			Gem(NONE, 0)	}
	};

	ItemAttribute aryStaffGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	������					����					����				ȣ��					���				�����̾�			����				����Ƹ���			���̾Ƹ��			����				����				�ڼ���					���޶���				�丮��Ʈ
		//	������					����					����				ȸ��					�ִ�HP				�ִ�MP				HPȸ��				MPȸ��				ũ��Ƽ��			���				���ǵ�				����ٿ�				������					��������			
		{	Gem(MAGIC_POWER, 3),	Gem(MAGIC_RESIST, 3),	Gem(HIT_RATE, 4),	Gem(MAGIC_POWER, 3),	Gem(MAX_HP, 2),		Gem(MAX_MP, 4),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 4),	Gem(CRITICAL, 2),	Gem(CRITICAL, 1),	Gem(MAX_HP, 1),		Gem(MAX_MP, 2),			Gem(MAGIC_POWER, 6),	Gem(MAGIC_RESIST, 6)	},
		{	Gem(MAX_DAMAGE, 3),		Gem(MAX_DAMAGE, 3),		Gem(NONE, 0),		Gem(MAGIC_RESIST, 3),	Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(HIT_RATE, 2),	Gem(HP_REGEN, 1),	Gem(MP_REGEN, 2),		Gem(NONE, 0),			Gem(NONE, 0)	}
	};

	ItemAttribute aryShieldGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	������					����					����				ȣ��					���				�����̾�			����				����Ƹ���			���̾Ƹ��			����				����				�ڼ���					���޶���				�丮��Ʈ
		//	������					����					����				ȸ��					�ִ�HP				�ִ�MP				HPȸ��				MPȸ��				ũ��Ƽ��			���				���ǵ�				����ٿ�				������					��������			
		{	Gem(ARMOR, 1),			Gem(ARMOR, 2),			Gem(MAX_HP, 1),		Gem(EVADE, 2),			Gem(MAX_HP, 2),		Gem(MAX_MP, 2),		Gem(HP_REGEN, 2),	Gem(MP_REGEN, 2),	Gem(MAX_MP, 1),		Gem(BLOCK, 2),		Gem(SPEED, 2),		Gem(MAGIC_POWER, 1),	Gem(MAGIC_POWER, 2),	Gem(MAGIC_RESIST, 2)		},
		{	Gem(EVADE, 1),			Gem(NONE, 0),			Gem(HP_REGEN, 1),	Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(MP_REGEN, 1),	Gem(NONE, 0),		Gem(NONE, 0),		Gem(MAGIC_RESIST, 1),	Gem(NONE, 0),			Gem(NONE, 0)				}
	};

	// edith 2009.06.23 �������� ���� �ڱ� ����
	ItemAttribute aryRuneGemAttribute[EquipmentInfo::MAX_ATTRIBUTE_PER_GEM][MAX_GEM_KIND] = {
		//	Fehu					Othila					Jera				dummy~
		{	Gem(LUCKCHANCE, 1),		Gem(LUCKCHANCE, 2),		Gem(LUCKCHANCE, 3),	Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(MP_REGEN, 1),	Gem(NONE, 0),		Gem(NONE, 0),		Gem(MAGIC_RESIST, 1),	Gem(NONE, 0),			Gem(NONE, 0)				},
		{	Gem(NONE, 1),			Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),			Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(NONE, 0),		Gem(MP_REGEN, 1),	Gem(NONE, 0),		Gem(NONE, 0),		Gem(MAGIC_RESIST, 1),	Gem(NONE, 0),			Gem(NONE, 0)				}
	};
*/
	using namespace ItemType;

	CEquipment* lpEquip = CEquipment::DowncastToEquipment(lpItem);
	if (NULL != lpEquip)
	{
		short GemType	= 0;	// ���� ����
		short GemLevel	= 0;	// ���� ũ��

		unsigned short AttributeIndex = 0;
		for (unsigned char SocketIndex = 0;
			SocketIndex < lpEquip->GetCurrentSocketNum() && AttributeIndex < EquipmentInfo::MAX_SOCKET_ATTRIBUTE_NUM;
			++SocketIndex)
		{
			GemType = (lpEquip->GetSocket(SocketIndex) - 1) % MAX_GEM_KIND;
			GemLevel = ((lpEquip->GetSocket(SocketIndex) - 1) / MAX_GEM_KIND) + 1;
			
			for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_ATTRIBUTE_PER_GEM; ++cIndex)
			{
				Gem GemAttr;
				// edith 2009.06.23 ������ ���� ��ڱ�
				if(GemLevel == GEMETYPE_RUNE)	/// �� �����̸�
				{
					if(GemType >= GEMETYPE_RUNE_NUM)
						break;

					// ���� ���� �ƸӸ� �Ѵ�.
					switch (Item::CItemType::GetEquipType(lpEquip->GetItemInfo().m_DetailData.m_dwFlags))
					{
						//case HELM_TYPE:			GemAttr = aryRuneGemAttribute[cIndex][GemType];			break;
						//case ARMOUR_TYPE:		GemAttr = aryRuneGemAttribute[cIndex][GemType];			break;
					}

					if (0 == GemAttr.m_usValue)
					{
						break;
					}
				}
				else
				{
					// �Ϲ� ��Ÿ���̸�
					switch (Item::CItemType::GetEquipType(lpEquip->GetItemInfo().m_DetailData.m_dwFlags))
					{
						case HELM_TYPE:			GemAttr = aryArmorGemAttribute[cIndex][GemType];			break;
						case ARMOUR_TYPE:		GemAttr = aryArmorGemAttribute[cIndex][GemType];			break;
						case DAGGER_TYPE:		GemAttr = aryDaggerGemAttribute[cIndex][GemType];			break;

						case ONEHANDED_TYPE:	
						case LONGRANGE_TYPE:
							GemAttr = aryOneHandedWeaponGemAttribute[cIndex][GemType];	
							break;

						case TWOHANDED_TYPE:	GemAttr = aryTwoHandedWeaponGemAttribute[cIndex][GemType];	break;
						case STAFF_TYPE:		GemAttr = aryStaffGemAttribute[cIndex][GemType];			break;
						case SHIELD_TYPE:		GemAttr = aryShieldGemAttribute[cIndex][GemType];			break;
					}

					if (0 == GemAttr.m_usValue)
					{
						break;
					}

					GemAttr.m_usValue *= GemLevel;
				}

				lpEquip->m_SocketAttribute[AttributeIndex] = GemAttr;
				++AttributeIndex;
			}
		}
	}
}
//ryl2 avalansa upgrade system
void CItemType::SetUpgradeItemAttribute(CItem* lpItem)
{
	ItemAttribute aryArmorUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(ARMOR, 40),			Upg(EVADE, 20),		Upg(MAX_HP, 50),	Upg(HP_REGEN, 10),	Upg(MAGIC_RESIST, 10)	},
		{	Upg(NONE, 0),			Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryDaggerUpgrad[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 20),	Upg(HIT_RATE, 80),	Upg(MAX_MP, 50),	Upg(MP_REGEN, 10),	Upg(CRITICAL, 10)		},
		{	Upg(MAX_DAMAGE, 20),	Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryOneHandedWeaponUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 40),	Upg(HIT_RATE, 80),	Upg(MAX_MP, 50),	Upg(MP_REGEN, 10),	Upg(CRITICAL, 10)		},
		{	Upg(MAX_DAMAGE, 40),	Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryTwoHandedWeaponUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 60),	Upg(HIT_RATE, 80),	Upg(MAX_MP, 50),	Upg(MP_REGEN, 10),	Upg(CRITICAL, 10)		},
		{	Upg(MAX_DAMAGE, 60),	Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryStaffUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MAGIC_POWER, 100),	Upg(HIT_RATE, 80),	Upg(MAX_MP, 50),	Upg(MP_REGEN, 10),	Upg(CRITICAL, 10)		},
		{	Upg(MIN_DAMAGE, 60),	Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryShieldUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MAX_HP, 50),		Upg(HP_REGEN, 10),	Upg(MAX_MP, 50),	Upg(MP_REGEN, 10),	Upg(BLOCK, 10)			},
		{	Upg(NONE, 0),			Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryHelmUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(ARMOR, 40),			Upg(EVADE, 20),		Upg(MAX_HP, 50),	Upg(HP_REGEN, 20),	Upg(MAGIC_RESIST, 20)	},
		{	Upg(NONE, 0),			Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

/*
void CItemType::SetUpgradeItemAttribute(CItem* lpItem)
{
	ItemAttribute aryArmorUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(ARMOR, 4),			Upg(EVADE, 2),		Upg(MAX_HP, 1),		Upg(HP_REGEN, 1),	Upg(MAGIC_RESIST, 1)	},
		{	Upg(NONE, 0),			Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryDaggerUpgrad[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 2),		Upg(HIT_RATE, 8),	Upg(MAX_MP, 1),		Upg(MP_REGEN, 1),	Upg(CRITICAL, 1)		},
		{	Upg(MAX_DAMAGE, 2),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryOneHandedWeaponUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 4),		Upg(HIT_RATE, 8),	Upg(MAX_MP, 1),		Upg(MP_REGEN, 1),	Upg(CRITICAL, 1)		},
		{	Upg(MAX_DAMAGE, 4),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryTwoHandedWeaponUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MIN_DAMAGE, 6),		Upg(HIT_RATE, 8),	Upg(MAX_MP, 1),		Upg(MP_REGEN, 1),	Upg(CRITICAL, 1)		},
		{	Upg(MAX_DAMAGE, 6),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryStaffUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MAGIC_POWER, 10),	Upg(HIT_RATE, 8),	Upg(MAX_MP, 2),		Upg(MP_REGEN, 2),	Upg(CRITICAL, 1)		},
		{	Upg(MAX_DAMAGE, 6),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};

	ItemAttribute aryShieldUpgrade[EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL][MAX_LEVEL_OPTION_UPGRADE] = {
		{	Upg(MAX_HP, 1),			Upg(HP_REGEN, 1),	Upg(MAX_MP, 1),		Upg(MP_REGEN, 1),	Upg(BLOCK, 1)			},
		{	Upg(NONE, 0),			Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0),		Upg(NONE, 0)			}
	};
*/
	using namespace ItemType;

	CEquipment* lpEquip = CEquipment::DowncastToEquipment(lpItem);
	if (NULL != lpEquip)
	{
		short AttributeIndex = 0;
		unsigned char cUpgradeLevel = min(lpEquip->GetUpgradeLevel(), MAX_LEVEL_OPTION_UPGRADE);

		for (unsigned char cLevelIndex = 0; 
			cLevelIndex < cUpgradeLevel && AttributeIndex < EquipmentInfo::MAX_UPGRADE_ATTRIBUTE; 
			++cLevelIndex)
		{
			for (unsigned char cAttributeIndex = 0; cAttributeIndex < EquipmentInfo::MAX_ATTRIBUTE_PER_UPGRADE_LEVEL; ++cAttributeIndex)
			{
				Upg UpgAttr;
				switch (lpEquip->GetItemInfo().m_DetailData.m_cItemType)
				{
					case CON_ARMOUR:		case CON_BODY:
					case DEX_ARMOUR:		case DEX_BODY:

					// edith 2008.01.14 ���׷��̵� �����ϰ� ��������
					case CON_HELM:
					case DEX_HELM:
					case CON_HEAD:
					case DEX_HEAD:

					case CON_GLOVE:
					case DEX_GLOVE:
					case CON_BOOTS:
					case DEX_BOOTS:
					case CON_PELVIS:
					case DEX_PELVIS:
					case CON_PROTECT_A:
					case DEX_PROTECT_A:

						UpgAttr = aryArmorUpgrade[cAttributeIndex][cLevelIndex];
						break;
					
					case DAGGER:			case OPP_TALON:			case SKILL_A_KNIFE:
						UpgAttr = aryDaggerUpgrad[cAttributeIndex][cLevelIndex];					
						break;

					case ONEHANDED_SWORD:	case ONEHANDED_AXE:		case ONEHANDED_BLUNT:	
					case OPP_HAMMER:		case OPP_AXE:			case OPP_SLUSHER:
					case SKILL_A_ATTACK:	case SKILL_A_GUN:
					case BOW:				case CROSSBOW:
					case COM_BLUNT:			case COM_SWORD:
						UpgAttr = aryOneHandedWeaponUpgrade[cAttributeIndex][cLevelIndex];					
						break;

					case TWOHANDED_SWORD:	case TWOHANDED_AXE:		case TWOHANDED_BLUNT:
						UpgAttr = aryTwoHandedWeaponUpgrade[cAttributeIndex][cLevelIndex];					
						break;

					case STAFF:				case OPP_SYTHE:
						UpgAttr = aryStaffUpgrade[cAttributeIndex][cLevelIndex];					
						break;

					case SHIELD:			case SKILL_A_GUARD:
						UpgAttr = aryShieldUpgrade[cAttributeIndex][cLevelIndex];					
						break;
				}

				if (0 == UpgAttr.m_usValue)
				{
					break;
				}

				lpEquip->m_UpgradeAttribute[AttributeIndex] = UpgAttr;
				++AttributeIndex;
			}
		}
	}
}
