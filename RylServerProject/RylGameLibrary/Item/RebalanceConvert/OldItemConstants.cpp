#include "stdafx.h"
#include "OldItemConstants.h"
#include "OldItemStructure.h"

#include <algorithm>

using namespace OldItem;

OldItem::CItemType& OldItem::CItemType::GetInstance()
{
    static CItemType itemType;
    return itemType;
}

const CTypeName OldItem::Attribute::Attributes[OldItem::Attribute::MAX_ATTRIBUTE_NUM] = 
{
    CTypeName(Attribute::NONE,                   "NONE"),
    CTypeName(Attribute::CRITICAL_TYPE,          "CRITICAL_TYPE"),
    CTypeName(Attribute::CRITICAL_PERCENTAGE,    "CRITICAL_PERCENTAGE"),
    CTypeName(Attribute::MIN_DAMAGE,             "MIN_DAMAGE"),
    CTypeName(Attribute::MAX_DAMAGE,             "MAX_DAMAGE"),
    CTypeName(Attribute::DRC,                    "DRC"),
    CTypeName(Attribute::OFFENCE_REVISION,       "OFFENCE_REVISION"),
    CTypeName(Attribute::DEFENCE,                "DEFENCE"),
    CTypeName(Attribute::DEFENCE_REVISION,       "DEFENCE_REVISION"),
    CTypeName(Attribute::MAGIC_RESISTANCE,       "MAGIC_RESISTANCE"),
    CTypeName(Attribute::BLOCKING_PERCENTAGE,    "BLOCKING_PERCENTAGE"),
    CTypeName(Attribute::ATTACK_SPEED,           "ATTACK_SPEED"),
    CTypeName(Attribute::MOVE_SPEED,             "MOVE_SPEED"),
    CTypeName(Attribute::ATTACK_RANGE,           "ATTACK_RANGE"),
    CTypeName(Attribute::RANGE_ATTACK_DISTANCE,  "RANGE_ATTACK_DISTANCE"),
    CTypeName(Attribute::MAX_HP,                 "MAX_HP"),
    CTypeName(Attribute::MAX_MP,                 "MAX_MP"),
    CTypeName(Attribute::HP_REGEN_AMOUNT,        "HP_REGEN_AMOUNT"),
    CTypeName(Attribute::MP_REGEN_AMOUNT,        "MP_REGEN_AMOUNT"),
    CTypeName(Attribute::FIRE_ATTACK,            "FIRE_ATTACK"),
    CTypeName(Attribute::LIGHTNING_ATTACK,       "LIGHTNING_ATTACK"),
    CTypeName(Attribute::COLD_ATTACK,            "COLD_ATTACK"),
    CTypeName(Attribute::DRAIN_ATTACK,           "DRAIN_ATTACK"),
    CTypeName(Attribute::POISON_ATTACK,          "POISON_ATTACK"),
    CTypeName(Attribute::FIRE_RESISTANCE,        "FIRE_RESISTANCE"),
    CTypeName(Attribute::LIGHTNING_RESISTANCE,   "LIGHTNING_RESISTANCE"),
    CTypeName(Attribute::COLD_RESISTANCE,        "COLD_RESISTANCE"),
    CTypeName(Attribute::DRAIN_RESISTANCE,       "DRAIN_RESISTANCE"),
    CTypeName(Attribute::POISON_RESISTANCE,      "POISON_RESISTANCE"),
    CTypeName(Attribute::ADD_STR,                "ADD_STR"),
    CTypeName(Attribute::ADD_DEX,                "ADD_DEX"),
    CTypeName(Attribute::ADD_CON,                "ADD_CON"),
    CTypeName(Attribute::ADD_INT,                "ADD_INT"),
    CTypeName(Attribute::ADD_WIS,                "ADD_WIS"),
    CTypeName(Attribute::MAGIC_POWER,            "MAGIC_POWER")
};



const CTypeName OldItem::Grade::Grades[Grade::MAX_ITEM_GRADE] =
{
	CTypeName(Grade::ETC, "ETC"),
	CTypeName(Grade::NORMAL, "NORMAL"),
	CTypeName(Grade::ENHANCED, "ENHANCED"),
	CTypeName(Grade::RARE, "RARE")
};


OldItem::CItemType::CItemType()
{
    using namespace OldItem::ItemType;

    int nCount = 0;

    for(nCount = 0; nCount < MAX_ARRAY_TYPE; ++nCount) 
    {
        m_ItemTypes[nCount].reserve(MAX_ITEM_TYPE);
    }

	m_ItemTypeNames[HELM]					= CTypeName(HELM, "HELM");
	m_ItemTypeNames[SHIRT]					= CTypeName(SHIRT, "SHIRT");
	m_ItemTypeNames[TUNIC]					= CTypeName(TUNIC,  "TUNIC");
	m_ItemTypeNames[ARMOUR]					= CTypeName(ARMOUR,  "ARMOUR");
	m_ItemTypeNames[GLOVE]					= CTypeName(GLOVE,  "GLOVE");
	m_ItemTypeNames[BOOTS]					= CTypeName(BOOTS,  "BOOTS");
	m_ItemTypeNames[SWORD]					= CTypeName(SWORD,  "ONEHANDED_SWORD");
	m_ItemTypeNames[BLUNT]					= CTypeName(BLUNT,  "ONEHANDED_BLUNT");
	m_ItemTypeNames[AXE]					= CTypeName(AXE,  "ONEHANDED_AXE");
	m_ItemTypeNames[TWOHANDED_BLUNT]		= CTypeName(TWOHANDED_BLUNT,  "TWOHANDED_BLUNT");
	m_ItemTypeNames[TWOHANDED_AXE]			= CTypeName(TWOHANDED_AXE,  "TWOHANDED_AXE");
	m_ItemTypeNames[TWOHANDED_SWORD]		= CTypeName(TWOHANDED_SWORD,  "TWOHANDED_SWORD");
	m_ItemTypeNames[BOW]					= CTypeName(BOW,  "BOW");
	m_ItemTypeNames[CROSSBOW]				= CTypeName(CROSSBOW,  "CROSSBOW");
	m_ItemTypeNames[STAFF]					= CTypeName(STAFF,  "STAFF");
	m_ItemTypeNames[DAGGER]					= CTypeName(DAGGER,  "DAGGER");
	m_ItemTypeNames[SHIELD]					= CTypeName(SHIELD,  "SHIELD");
	m_ItemTypeNames[CLOAK]					= CTypeName(CLOAK,  "CLOAK");
	m_ItemTypeNames[RING]					= CTypeName(RING,  "RING");
	m_ItemTypeNames[NECKLACE]				= CTypeName(NECKLACE,  "NECKLACE");

	m_ItemTypeNames[POTION]					= CTypeName(POTION,  "POTION");
	m_ItemTypeNames[POISON]					= CTypeName(POISON,  "POISON");
	m_ItemTypeNames[TRAP]					= CTypeName(TRAP,  "TRAP");
	m_ItemTypeNames[SKILLBOOK]				= CTypeName(SKILLBOOK,  "SKILLBOOK");
	m_ItemTypeNames[GEM]					= CTypeName(GEM,  "GEM");
	m_ItemTypeNames[RUNE]					= CTypeName(RUNE,  "RUNE");
	m_ItemTypeNames[MATERIAL]				= CTypeName(MATERIAL,  "ORE");	
	m_ItemTypeNames[AMMO]					= CTypeName(AMMO,  "AMMO");
	m_ItemTypeNames[ETC]					= CTypeName(ETC,  "ETC");

	m_ItemTypeNames[HEAD]					= CTypeName(HEAD,  "HEAD");
	m_ItemTypeNames[BODY]					= CTypeName(BODY,  "BODY");
	m_ItemTypeNames[PROTECT_A]				= CTypeName(PROTECT_A,  "PROTECT_A");
	m_ItemTypeNames[PELVIS]					= CTypeName(PELVIS,  "PELVIS");
	m_ItemTypeNames[COM_BLUNT]				= CTypeName(COM_BLUNT,  "COM_BLUNT");
	m_ItemTypeNames[COM_SWORD]				= CTypeName(COM_SWORD,  "COM_SWORD");
	m_ItemTypeNames[OPP_HAMMER]				= CTypeName(OPP_HAMMER,  "OPP_HAMMER");
	m_ItemTypeNames[OPP_AXE]				= CTypeName(OPP_AXE,  "OPP_AXE");
	m_ItemTypeNames[OPP_SLUSHER]			= CTypeName(OPP_SLUSHER,  "OPP_SLUSHER");
	m_ItemTypeNames[OPP_TALON]				= CTypeName(OPP_TALON,  "OPP_TALON");
	m_ItemTypeNames[OPP_SYTHE]				= CTypeName(OPP_SYTHE,  "OPP_SYTHE");
	m_ItemTypeNames[SKILL_A_GUARD]			= CTypeName(SKILL_A_GUARD,  "SKILL_A_GUARD");
	m_ItemTypeNames[SKILL_A_ATTACK]			= CTypeName(SKILL_A_ATTACK,  "SKILL_A_ATTACK");
	m_ItemTypeNames[SKILL_A_GUN]			= CTypeName(SKILL_A_GUN,  "SKILL_A_GUN");
	m_ItemTypeNames[SKILL_A_KNIFE]		    = CTypeName(SKILL_A_KNIFE,  "SKILL_A_KNIFE");
	m_ItemTypeNames[ACCESSORY]				= CTypeName(ACCESSORY,  "ACCESSORY");

	m_ItemTypeNames[ARROW]					= CTypeName(ARROW,  "ARROW");
	m_ItemTypeNames[BOLT]					= CTypeName(BOLT,  "BOLT");
	m_ItemTypeNames[PORTAL]					= CTypeName(PORTAL,	"PORTAL");
	m_ItemTypeNames[EVENT_DROP]				= CTypeName(EVENT_DROP,	"EVENT_DROP");
	m_ItemTypeNames[EVENT_LOTTERY]			= CTypeName(EVENT_LOTTERY,	"EVENT_LOTTERY");
	m_ItemTypeNames[EXPBOOK]				= CTypeName(EXPBOOK,	"EXPBOOK");
	m_ItemTypeNames[CASHBOOK]				= CTypeName(CASHBOOK,	"CASHBOOK");
	m_ItemTypeNames[FIRE_CRACKER]			= CTypeName(FIRE_CRACKER,	"FIRE_CRACKER");

	m_ItemTypeNames[CAMP_KIT]				= CTypeName(CAMP_KIT,	"CAMP_KIT");
	m_ItemTypeNames[SHORT_RANGE_ARMS_KIT]	= CTypeName(SHORT_RANGE_ARMS_KIT,	"SHORT_RANGE_ARMS_KIT");
	m_ItemTypeNames[LONG_RANGE_ARMS_KIT]	= CTypeName(LONG_RANGE_ARMS_KIT,	"LONG_RANGE_ARMS_KIT");
	m_ItemTypeNames[AIRSHIP_KIT]			= CTypeName(AIRSHIP_KIT,	"AIRSHIP_KIT");

    // 장비
    m_ItemTypes[EQUIP_TYPE].push_back(ARROW);              	m_ItemTypes[EQUIP_TYPE].push_back(BOLT);
    m_ItemTypes[EQUIP_TYPE].push_back(HELM);               	m_ItemTypes[EQUIP_TYPE].push_back(SHIRT);
    m_ItemTypes[EQUIP_TYPE].push_back(TUNIC);              	m_ItemTypes[EQUIP_TYPE].push_back(ARMOUR);
    m_ItemTypes[EQUIP_TYPE].push_back(SHIELD);             	m_ItemTypes[EQUIP_TYPE].push_back(CLOAK);
    m_ItemTypes[EQUIP_TYPE].push_back(GLOVE);              	m_ItemTypes[EQUIP_TYPE].push_back(BOOTS);
    m_ItemTypes[EQUIP_TYPE].push_back(SWORD);              	m_ItemTypes[EQUIP_TYPE].push_back(BLUNT);
    m_ItemTypes[EQUIP_TYPE].push_back(AXE);                	m_ItemTypes[EQUIP_TYPE].push_back(DAGGER);
    m_ItemTypes[EQUIP_TYPE].push_back(STAFF);              	m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_BLUNT);
    m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_AXE);	   	m_ItemTypes[EQUIP_TYPE].push_back(TWOHANDED_SWORD);
    m_ItemTypes[EQUIP_TYPE].push_back(BOW);                	m_ItemTypes[EQUIP_TYPE].push_back(CROSSBOW);
    m_ItemTypes[EQUIP_TYPE].push_back(RING);               	m_ItemTypes[EQUIP_TYPE].push_back(NECKLACE);
    m_ItemTypes[EQUIP_TYPE].push_back(HEAD);               	m_ItemTypes[EQUIP_TYPE].push_back(BODY);
    m_ItemTypes[EQUIP_TYPE].push_back(PELVIS);             	m_ItemTypes[EQUIP_TYPE].push_back(PROTECT_A);
    m_ItemTypes[EQUIP_TYPE].push_back(COM_BLUNT);          	m_ItemTypes[EQUIP_TYPE].push_back(COM_SWORD);
    m_ItemTypes[EQUIP_TYPE].push_back(OPP_HAMMER);         	m_ItemTypes[EQUIP_TYPE].push_back(OPP_AXE);
    m_ItemTypes[EQUIP_TYPE].push_back(OPP_SLUSHER);	       	m_ItemTypes[EQUIP_TYPE].push_back(OPP_TALON);
    m_ItemTypes[EQUIP_TYPE].push_back(OPP_SYTHE);          	m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_GUARD);
    m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_ATTACK);     	m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_GUN);
    m_ItemTypes[EQUIP_TYPE].push_back(SKILL_A_KNIFE);      	m_ItemTypes[EQUIP_TYPE].push_back(ACCESSORY);

    // 사용 아이템
    m_ItemTypes[USE_ITEM_TYPE].push_back(POTION);			m_ItemTypes[USE_ITEM_TYPE].push_back(POISON);    
    m_ItemTypes[USE_ITEM_TYPE].push_back(TRAP);				m_ItemTypes[USE_ITEM_TYPE].push_back(SKILLBOOK);
	m_ItemTypes[USE_ITEM_TYPE].push_back(AMMO);				m_ItemTypes[USE_ITEM_TYPE].push_back(PORTAL);
	m_ItemTypes[USE_ITEM_TYPE].push_back(EVENT_LOTTERY);	m_ItemTypes[USE_ITEM_TYPE].push_back(EXPBOOK);
	m_ItemTypes[USE_ITEM_TYPE].push_back(CASHBOOK);			m_ItemTypes[USE_ITEM_TYPE].push_back(FIRE_CRACKER);

	m_ItemTypes[USE_ITEM_TYPE].push_back(CAMP_KIT);			
	m_ItemTypes[USE_ITEM_TYPE].push_back(SHORT_RANGE_ARMS_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(LONG_RANGE_ARMS_KIT);
	m_ItemTypes[USE_ITEM_TYPE].push_back(AIRSHIP_KIT);

    // 무기
    m_ItemTypes[WEAPON_TYPE].push_back(SWORD);             	m_ItemTypes[WEAPON_TYPE].push_back(BLUNT);
    m_ItemTypes[WEAPON_TYPE].push_back(AXE);               	m_ItemTypes[WEAPON_TYPE].push_back(DAGGER);
    m_ItemTypes[WEAPON_TYPE].push_back(STAFF);             	m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_BLUNT);
    m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_AXE);	   	m_ItemTypes[WEAPON_TYPE].push_back(TWOHANDED_SWORD);
    m_ItemTypes[WEAPON_TYPE].push_back(BOW);               	m_ItemTypes[WEAPON_TYPE].push_back(CROSSBOW);
    m_ItemTypes[WEAPON_TYPE].push_back(COM_BLUNT);         	m_ItemTypes[WEAPON_TYPE].push_back(COM_SWORD);
    m_ItemTypes[WEAPON_TYPE].push_back(OPP_HAMMER);        	m_ItemTypes[WEAPON_TYPE].push_back(OPP_AXE);
    m_ItemTypes[WEAPON_TYPE].push_back(OPP_SLUSHER);	   	m_ItemTypes[WEAPON_TYPE].push_back(OPP_TALON);
    m_ItemTypes[WEAPON_TYPE].push_back(OPP_SYTHE);         	

    // 방어구
    m_ItemTypes[ARMOUR_TYPE].push_back(HELM);              	m_ItemTypes[ARMOUR_TYPE].push_back(SHIRT);
    m_ItemTypes[ARMOUR_TYPE].push_back(TUNIC);             	m_ItemTypes[ARMOUR_TYPE].push_back(ARMOUR);
    m_ItemTypes[ARMOUR_TYPE].push_back(SHIELD);            	m_ItemTypes[ARMOUR_TYPE].push_back(CLOAK);
    m_ItemTypes[ARMOUR_TYPE].push_back(GLOVE);             	m_ItemTypes[ARMOUR_TYPE].push_back(BOOTS);
    m_ItemTypes[ARMOUR_TYPE].push_back(HEAD);              	m_ItemTypes[ARMOUR_TYPE].push_back(BODY);
    m_ItemTypes[ARMOUR_TYPE].push_back(PELVIS);            	m_ItemTypes[ARMOUR_TYPE].push_back(PROTECT_A);

    // 스킬암
    m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_GUARD);  	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_ATTACK);
    m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_GUN);    	m_ItemTypes[SKILL_ARM_TYPE].push_back(SKILL_A_KNIFE);      

    // 양손 무기
    m_ItemTypes[TWOHANDED_TYPE].push_back(STAFF);          	m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_BLUNT);
    m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_AXE);  	m_ItemTypes[TWOHANDED_TYPE].push_back(TWOHANDED_SWORD);
    m_ItemTypes[TWOHANDED_TYPE].push_back(BOW);            	m_ItemTypes[TWOHANDED_TYPE].push_back(CROSSBOW);

    // 소켓만 업그레이드하는지 여부
    m_ItemTypes[UPGRADE_SOCKET_ONLY].push_back(HELM);
    m_ItemTypes[UPGRADE_SOCKET_ONLY].push_back(RING);
    m_ItemTypes[UPGRADE_SOCKET_ONLY].push_back(NECKLACE);

    m_ItemTypes[UPGRADE_SOCKET_ONLY].push_back(HEAD);
    m_ItemTypes[UPGRADE_SOCKET_ONLY].push_back(ACCESSORY);

    for(nCount = 0; nCount < MAX_ARRAY_TYPE; ++nCount)
    {
        std::sort(m_ItemTypes[nCount].begin(), m_ItemTypes[nCount].end());
    }
}

bool OldItem::CItemType::IsCorrectItemType(ArrayType itemType, unsigned char cEquipType)
{
    return std::binary_search(m_ItemTypes[itemType].begin(),
        m_ItemTypes[itemType].end(), cEquipType);
}

unsigned char OldItem::CItemType::ConvertRandomOptionType(unsigned char cEquipType)
{
    using namespace OldItem::ItemType;

	if (true == IsCorrectItemType(CItemType::WEAPON_TYPE, cEquipType))
	{
		return RandomOption::WEAPON;
	}
	else
	{
		switch (cEquipType)
		{
			case ARMOUR:	case BODY:				return RandomOption::ARMOUR_BODY;
			case HELM:		case HEAD:				return RandomOption::HELM_HEAD;
			case RING:		case ACCESSORY:			return RandomOption::RING_ACCESSORY;
			case NECKLACE:							return RandomOption::NECKLACE;

			case SHIELD:	case SKILL_A_GUARD:		return RandomOption::SKILL_A_GUARD;

			case SKILL_A_ATTACK:
			case SKILL_A_GUN:
			case SKILL_A_KNIFE:
				return RandomOption::SKILL_A_WEAPON;
		}
	}
	
	return RandomOption::MAX_RANDOM_OPTION_TYPE;
}

void OldItem::CItemType::SetEquipTypeFlags(OldItem::ItemInfo& itemInfo)
{
    using namespace OldItem::ItemType;

    switch (itemInfo.m_DetailData.m_cItemType)
    {
		case ARROW:
		case BOLT:
			itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::STACKABLE;
			break;
    }
   
    if (IsCorrectItemType(WEAPON_TYPE, itemInfo.m_DetailData.m_cItemType))
    {
        itemInfo.m_DetailData.m_dwFlags |= DetailData::WEAPON;

        if (IsCorrectItemType(TWOHANDED_TYPE, itemInfo.m_DetailData.m_cItemType))
        {
            itemInfo.m_DetailData.m_dwFlags |= DetailData::TWOHANDED;
        }

    }
	else if (IsCorrectItemType(SKILL_ARM_TYPE, itemInfo.m_DetailData.m_cItemType))
	{
		itemInfo.m_DetailData.m_dwFlags |= DetailData::SKILL_ARM;
	}
    else if (IsCorrectItemType(ARMOUR_TYPE, itemInfo.m_DetailData.m_cItemType))
    {
        itemInfo.m_DetailData.m_dwFlags |= DetailData::ARMOUR;
    }
    
    itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::EQUIP;
}


void OldItem::CItemType::SetUseItemTypeFlags(OldItem::ItemInfo& itemInfo)
{
    using namespace OldItem::ItemType;

    switch (itemInfo.m_DetailData.m_cItemType)
    {
		case POTION:
		case POISON:
		case TRAP:
		case PORTAL:
		case FIRE_CRACKER:
			itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::QUICKSLOT_IN;
			break;
    }

    if (itemInfo.m_DetailData.m_cMaxDurabilityOrStack > 1)
    {
        itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::STACKABLE;
    }

    itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::USE_ITEM;
}


void OldItem::CItemType::SetEtcItemTypeFlags(OldItem::ItemInfo& itemInfo)
{
    if (itemInfo.m_DetailData.m_cMaxDurabilityOrStack > 1)
    {
        itemInfo.m_DetailData.m_dwFlags |= OldItem::DetailData::STACKABLE;
    }
}


using namespace OldItem::Attribute;
typedef OldItem::ItemAttribute Gem;
typedef OldItem::ItemAttribute Upg;

// 동일 보석 1개 능력치 테이블
const Gem GemAttribute1[5][4] = 
{
	//	무기 / 스킬암			아머							방패 / 가드암					투구
	Gem(FIRE_ATTACK, 1),		Gem(FIRE_RESISTANCE, 1),		Gem(FIRE_RESISTANCE, 1),		Gem(ADD_STR, 3),	// 루비	조각
	Gem(POISON_ATTACK, 1),		Gem(POISON_RESISTANCE, 1),		Gem(POISON_RESISTANCE, 1),		Gem(ADD_CON, 3),	// 에메랄드 조각
	Gem(COLD_ATTACK, 1),		Gem(COLD_RESISTANCE, 1),		Gem(COLD_RESISTANCE, 1),		Gem(ADD_INT, 3),	// 사파이어 조각
	Gem(LIGHTNING_ATTACK, 1),	Gem(LIGHTNING_RESISTANCE, 1),	Gem(LIGHTNING_RESISTANCE, 1),	Gem(ADD_WIS, 3),	// 다이아몬드 조각	
	Gem(DRAIN_ATTACK, 1),		Gem(DRAIN_RESISTANCE, 1),		Gem(DRAIN_RESISTANCE, 1),		Gem(ADD_DEX, 3)		// 블랙문 조각
};

// 동일 보석 2개 이상 능력치 테이블
const Gem GemAttribute2[5][4] = 
{
	//	무기 / 스킬암				아머						방패 / 가드암					투구
	Gem(MAX_DAMAGE, 1),				Gem(DEFENCE_REVISION, 1),	Gem(DEFENCE_REVISION, 1),		Gem(ADD_STR, 3),	// 루비 조각
	Gem(HP_REGEN_AMOUNT, 2),		Gem(MAX_HP, 1),				Gem(MAX_HP, 1),			    	Gem(ADD_CON, 3),	// 에메랄드 조각
	Gem(MP_REGEN_AMOUNT, 2),		Gem(MAX_MP, 1),				Gem(MAGIC_RESISTANCE, 2),		Gem(ADD_INT, 3),	// 사파이어 조각
	Gem(CRITICAL_PERCENTAGE, 1),	Gem(MAGIC_RESISTANCE, 2),	Gem(BLOCKING_PERCENTAGE, 1),	Gem(ADD_DEX, 3),	// 다이아몬드 조각	
	Gem(OFFENCE_REVISION, 1),		Gem(DEFENCE, 1),			Gem(DEFENCE, 1),				Gem(ADD_DEX, 3)		// 블랙문 조각
};

// 동일 보석 2개 이상 능력치 테이블
const Upg UpgradeAttribute[15][4] = 
{
	//	무기 / 스킬암				아머						방패 / 가드암					투구
	Upg(MAX_DAMAGE, 3),				Upg(DEFENCE, 3),			Upg(BLOCKING_PERCENTAGE, 2),	Upg(0, 0),			// 1단계
	Upg(MAX_DAMAGE, 3),				Upg(DEFENCE, 3),			Upg(BLOCKING_PERCENTAGE, 2),	Upg(0, 0),			// 2단계
	Upg(MAX_DAMAGE, 6),				Upg(DEFENCE, 6),			Upg(BLOCKING_PERCENTAGE, 4),	Upg(0, 0),			// 3단계
	Upg(MAX_DAMAGE, 6),				Upg(DEFENCE, 6),			Upg(BLOCKING_PERCENTAGE, 4),	Upg(0, 0),			// 4단계
	Upg(MAX_DAMAGE, 9),				Upg(DEFENCE, 9),			Upg(BLOCKING_PERCENTAGE, 6),	Upg(0, 0),			// 5단계
	Upg(MAX_DAMAGE, 9),				Upg(DEFENCE, 9),			Upg(BLOCKING_PERCENTAGE, 6),	Upg(0, 0),			// 6단계
	Upg(MAX_DAMAGE, 12),			Upg(DEFENCE, 12),			Upg(BLOCKING_PERCENTAGE, 8),	Upg(0, 0),			// 7단계
	Upg(MAX_DAMAGE, 12),			Upg(DEFENCE, 12),			Upg(BLOCKING_PERCENTAGE, 8),	Upg(0, 0),			// 8단계
	Upg(MAX_DAMAGE, 15),			Upg(DEFENCE, 15),			Upg(BLOCKING_PERCENTAGE, 10),	Upg(0, 0),			// 9단계
	Upg(MAX_DAMAGE, 15),			Upg(DEFENCE, 15),			Upg(BLOCKING_PERCENTAGE, 10),	Upg(0, 0),			// 10단계
	Upg(MAX_DAMAGE, 19),			Upg(DEFENCE, 19),			Upg(BLOCKING_PERCENTAGE, 13),	Upg(0, 0),			// 11단계
	Upg(MAX_DAMAGE, 19),			Upg(DEFENCE, 19),			Upg(BLOCKING_PERCENTAGE, 13),	Upg(0, 0),			// 12단계
	Upg(MAX_DAMAGE, 23),			Upg(DEFENCE, 23),			Upg(BLOCKING_PERCENTAGE, 16),	Upg(0, 0),			// 13단계
	Upg(MAX_DAMAGE, 23),			Upg(DEFENCE, 23),			Upg(BLOCKING_PERCENTAGE, 16),	Upg(0, 0),			// 14단계
	Upg(MAX_DAMAGE, 27),			Upg(DEFENCE, 27),			Upg(BLOCKING_PERCENTAGE, 19),	Upg(0, 0),			// 15단계
};

enum EquipType {	None = 0,	Weapon = 1, Armour = 2, Shield = 3, Helm = 4	};
short GetEquipType(OldItem::ItemType::Type eItemType)
{
	using namespace OldItem::ItemType;

	// 장비 타입 얻기
	switch (eItemType)
	{
	case SWORD:			case BLUNT:				case AXE:			case TWOHANDED_BLUNT:	// 인간	
	case TWOHANDED_AXE:	case TWOHANDED_SWORD:	case BOW:			case CROSSBOW:
	case STAFF:			case DAGGER:
	case COM_BLUNT:		case COM_SWORD:			case OPP_HAMMER:	case OPP_AXE:			// 아칸
	case OPP_SLUSHER:	case OPP_TALON:			case OPP_SYTHE:		case SKILL_A_ATTACK:
	case SKILL_A_GUN:	case SKILL_A_KNIFE:
		return Weapon;

	case ARMOUR:				// 인간
	case BODY:					// 아칸	
		return Armour;

	case SHIELD:				// 인간
	case SKILL_A_GUARD:			// 아칸
		return Shield;

	case HELM:					// 인간
	case HEAD:					// 아칸
		return Helm;
	}

	return None;
}

void OldItem::CItemType::GetInstallGemAttribute(const unsigned char* cSockets_In, 
                                                OldItem::ItemAttribute* cAttributes_Out, 
                                                OldItem::ItemType::Type eItemType, 
                                                unsigned char cSocketNum,
                                                unsigned char cMaxAttributeNum)
{
	enum GemType
	{	
		Ruby		= 0,
		Emerald		= 1,
		Sapphire	= 2,
		Diamond		= 3,
		BlackMoon	= 4,
		MaxType		= 5
	};

	short EquipType = GetEquipType(eItemType);	// 장비 타입
	short GemType	= 0;	// 잼 타입
	short GemLevel	= 0;	// 잼 레벨

	if (0 >= EquipType)
		return;

	Gem GemAttr;
	unsigned short AttributeIndex = 0;
	for(unsigned char SocketIndex = 0;
        SocketIndex < cSocketNum && AttributeIndex < cMaxAttributeNum;
        ++SocketIndex)
	{
		GemType = (cSockets_In[SocketIndex] - 1) % MaxType;
		GemLevel = ((cSockets_In[SocketIndex] - 1) / MaxType) + 1;
		
		if (0 == SocketIndex)
		{
			GemAttr = GemAttribute1[GemType][EquipType - 1];	// 소켓 첫번째
		}
		else
		{
			GemAttr = GemAttribute2[GemType][EquipType - 1];	// 그 이후의 소켓
		}

		GemAttr.m_usValue *= GemLevel;
		cAttributes_Out[AttributeIndex] = GemAttr;
		++AttributeIndex;

		if (MAX_DAMAGE == GemAttr.m_cType)
		{
			// 최고, 최소 대미지 같이 올린다.
			GemAttr.m_cType = MIN_DAMAGE;		
			cAttributes_Out[AttributeIndex] = GemAttr;
			++AttributeIndex;
		}
	}
}


void OldItem::CItemType::GetUpgradeItemAttribute(OldItem::ItemType::Type eItemType, OldItem::ItemAttribute* cAttributes_Out,
                                                 unsigned char cUpgradeLevel, unsigned char cMaxUpgradeAttributeNum)
{
    using namespace OldItem::ItemType;

	short EquipType = GetEquipType(eItemType);	// 장비 타입

	if (0 == cUpgradeLevel || 0 >= EquipType)
		return;

	Upg UpgAttr = UpgradeAttribute[cUpgradeLevel - 1][EquipType - 1];
	if (0 == UpgAttr.m_usValue)
		return;

	short AttributeIndex = 0;
	cAttributes_Out[AttributeIndex] = UpgAttr;
	++AttributeIndex;

	if (MAX_DAMAGE == UpgAttr.m_cType)
	{
		// 최고, 최소 대미지 같이 올린다.
		UpgAttr.m_cType = MIN_DAMAGE;		
		cAttributes_Out[AttributeIndex] = UpgAttr;
		++AttributeIndex;
	}
      
/* FIXME :
	if (IsWeapon(EquipType))
	{
		Attribute[0].m_nAttributeType	= Attribute::MIN_DAMAGE;		// 최소 데미지 강화
		Attribute[0].m_nValue			= 3;

		Attribute[1].m_nAttributeType	= Attribute::MAX_DAMAGE;		// 최대 데미지 강화
		Attribute[1].m_nValue			= 3;

		return 2;
	}
	else if (IsArmour(EquipType))
	{
		Attribute[0].m_nAttributeType	= Attribute::DEFENCE;			// 방어력 강화
		Attribute[0].m_nValue			= 3;
	
		return 1;
	}
*/
}

