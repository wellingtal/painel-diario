#ifndef _OLD_ITEM_CONSTANTS_H_
#define _OLD_ITEM_CONSTANTS_H_

#include <vector>
#include <Pattern/Singleton.h>
#include <Utility/TypeArray.h>

namespace OldItem
{
    // ���� ����
    struct ItemInfo;
    struct ItemAttribute;

    namespace ItemType
    {
        // ������ Ÿ�� ( 8bit )
        enum Type
        {
            // �ΰ� ������.
            HELM				= 0,      	SHIRT					= 1,
            TUNIC				= 2,      	ARMOUR					= 3,
            GLOVE				= 4,      	BOOTS					= 5,
            SWORD				= 6,      	BLUNT       			= 7,
            AXE					= 8,      	TWOHANDED_BLUNT			= 9,
            TWOHANDED_AXE		= 10,     	TWOHANDED_SWORD			= 11,
            BOW					= 12,     	CROSSBOW				= 13,
            STAFF				= 14,     	DAGGER					= 15,
            SHIELD				= 16,     	CLOAK					= 17,
            RING				= 18,     	NECKLACE				= 19,

            // ���� ������
            POTION				= 20,     	POISON					= 21,
            TRAP				= 22,     	SKILLBOOK				= 23,

            GEM					= 24,     	RUNE					= 25,
            MATERIAL			= 26,     	AMMO					= 27,
            ETC					= 28,

            // ��ĭ ������
            HEAD				= 29,	  	BODY					= 30,
            PROTECT_A			= 31,     	PELVIS					= 32,
            COM_BLUNT			= 33,     	COM_SWORD				= 34,
            OPP_HAMMER			= 35,     	OPP_AXE					= 36,
            OPP_SLUSHER			= 37,	  	OPP_TALON				= 38,
            OPP_SYTHE			= 39,	  	SKILL_A_GUARD			= 40,
            SKILL_A_ATTACK		= 41,	  	SKILL_A_GUN				= 42,
            SKILL_A_KNIFE		= 43,	  	ACCESSORY				= 44,

            // ���� ������ (�Ŀ� �߰���)
            ARROW				= 45,		BOLT					= 46,
			PORTAL				= 47,		EVENT_DROP				= 48,
			EVENT_LOTTERY		= 49,		EXPBOOK					= 50,
			CASHBOOK			= 51,		FIRE_CRACKER			= 52, 
			CAMP_KIT			= 53,		SHORT_RANGE_ARMS_KIT	= 54,	// ����, �ٰŸ� �������� ��ŸƮŶ
			LONG_RANGE_ARMS_KIT	= 55,		AIRSHIP_KIT				= 56,	// ���Ÿ� ��������, ����� ��ŸƮŶ
            MAX_ITEM_TYPE		= 57
        };

		enum DropType
		{
			MAX_DROP_TYPE		= 35
		};
    };

    class CItemType
    {
    public:

        static CItemType& GetInstance();

        enum ArrayType
        {
            EQUIP_TYPE          = 0,    USE_ITEM_TYPE    = 1,
            WEAPON_TYPE         = 2,    ARMOUR_TYPE      = 3,
            SKILL_ARM_TYPE      = 4,    TWOHANDED_TYPE   = 5,
            UPGRADE_SOCKET_ONLY = 6,    MAX_ARRAY_TYPE   = 7
        };

        typedef std::vector<OldItem::ItemAttribute>    ItemAttributeArray;

        // ���� ��� �Լ�        
        static void GetInstallGemAttribute(const unsigned char* cSockets_In, 
            ItemAttribute* cAttributes_Out, ItemType::Type eItemType, 
            unsigned char cSocketNum, unsigned char cMaxAttributeNum);

        static bool IsUpgradeSocketOnly(ItemType::Type eItemType);
        static void GetUpgradeItemAttribute(ItemType::Type eItemType, ItemAttribute* cAttributes_Out,
            unsigned char cUpgradeLevel, unsigned char cMaxUpgradeAttributeNum);
        
        // ��� �Լ�
        bool IsCorrectItemType(ArrayType itemType, unsigned char cEquipType);
		unsigned char ConvertRandomOptionType(unsigned char cEquipType);

        void SetEquipTypeFlags(ItemInfo& itemInfo);
        void SetUseItemTypeFlags(ItemInfo& itemInfo);
        void SetEtcItemTypeFlags(ItemInfo& itemInfo);

        CTypeName* GetItemTypeName() { return m_ItemTypeNames; }

    private:

        CItemType();

        typedef std::vector<ItemType::Type> ItemTypeArray;
        ItemTypeArray                       m_ItemTypes[MAX_ARRAY_TYPE];
        CTypeName                           m_ItemTypeNames[ItemType::MAX_ITEM_TYPE];
    };
    

    namespace Attribute
    {
        enum Type
        {
            // Creature�� ���� �Ӽ�. �����ۿ��� �����.
		    NONE					= 0,		    CRITICAL_TYPE			= 1,
		    CRITICAL_PERCENTAGE		= 2,		    MIN_DAMAGE				= 3,
		    MAX_DAMAGE				= 4,		    DRC						= 5,
		    OFFENCE_REVISION		= 6,		    DEFENCE					= 7,
		    DEFENCE_REVISION		= 8,		    MAGIC_RESISTANCE		= 9,
		    BLOCKING_PERCENTAGE		= 10,		    ATTACK_SPEED			= 11,
		    MOVE_SPEED				= 12,		    ATTACK_RANGE			= 13,
		    RANGE_ATTACK_DISTANCE	= 14,		    MAX_HP					= 15,
		    MAX_MP					= 16,		    HP_REGEN_AMOUNT			= 17,
		    MP_REGEN_AMOUNT			= 18,		    FIRE_ATTACK				= 19,
		    LIGHTNING_ATTACK		= 20,		    COLD_ATTACK				= 21,
		    DRAIN_ATTACK			= 22,		    POISON_ATTACK			= 23,
		    FIRE_RESISTANCE			= 24,		    LIGHTNING_RESISTANCE	= 25,
		    COLD_RESISTANCE			= 26,		    DRAIN_RESISTANCE		= 27,
		    POISON_RESISTANCE		= 28,		    ADD_STR					= 29,
		    ADD_DEX					= 30,		    ADD_CON					= 31,
		    ADD_INT					= 32,		    ADD_WIS					= 33,
			MAGIC_POWER				= 34,

			MAX_ATTRIBUTE_NUM		= 35
        };

		enum Trend
		{
			Aggressive	= (1 << 0),
			Defensive	= (1 << 1)
		};

		enum CriticalType
		{
			NONE_TYPE	= 0,	
			
			SWORD_TYPE	= 1,	
			AXE_TYPE	= 2,	
			BLUNT_TYPE	= 3
		};

		extern const CTypeName   Attributes[MAX_ATTRIBUTE_NUM];
    };

	namespace EquipType 
	{ 
		enum Type
		{
            Attach = 0, 	
			Detach = 1,

			GuardArm = 2
		};

		enum DoubleSwordType
		{ 
			ErrorDoubleSword	= 0,
			SoloSword			= 1, 
			WarriorDoubleSword	= 2, 
			AssasinDoubleDagger	= 3
		};
	};

	namespace RandomOption
	{
		enum Type
		{
			WEAPON			= 0,	ARMOUR_BODY		= 1, 
			HELM_HEAD		= 2,	RING_ACCESSORY	= 3, 
			NECKLACE		= 4,	SKILL_A_GUARD	= 5, 
			SKILL_A_WEAPON	= 6,	
			
			MAX_RANDOM_OPTION_TYPE	= 7
		};

		enum Attribute 
		{ 
			MAX_DURABILITY			= 0,		NOW_DURABILITY		= 1, 
			MIN_DAMAGE				= 2,		MAX_DAMAGE			= 3, 
			OFFENCE_REVISION		= 4,		DEFENCE				= 5, 
			DEFENCE_REVISION		= 6,		MAGIC_RESISTANCE	= 7,
			CRITICAL_PERCENTAGE		= 8,		BLOCKING_PERCENTAGE	= 9, 
			MAX_HP					= 10,		MAX_MP				= 11, 
			HP_REGEN_AMOUNT			= 12,		MP_REGEN_AMOUNT		= 13,

			MAX_RANDOM_OPTION_ATTRIBUTE	= 14
		};

		enum ValueRange
		{
			MIN_MINUS_FACTOR	= 0,	MIN_MULTIPLY_FACTOR = 1, 
			MIN_DIVIDE_FACTOR	= 2,	MAX_MULTIPLY_FACTOR = 3, 
			MAX_DIVIDE_FACTOR	= 4,	MAX_PLUS_FACTOR		= 5,

			MAX_RANDOM_OPTION_VALUE_RANGE = 6
		};
	};

	namespace EquipmentPos 
    {
        // ��� ��ġ ( 8bit )

        enum Type
        {
            // �ΰ� ��� ��ġ
		    HELM				= 0,		    SHIRT				= 1,
		    TUNIC				= 2,		    ARMOUR				= 3,
		    GLOVE				= 4,		    BOOTS				= 5,
		    NECKLACE			= 6,		    RINGR				= 7,
		    RINGL				= 8,		    SHIELD_HAND1		= 9,
		    SHIELD_HAND2		= 10,		    WEAPON_HAND1		= 11,
		    WEAPON_HAND2		= 12,		    CLOAK				= 13,	
		    RIDE				= 14,	

            // ��ĭ ��� ��ġ
		    HEAD				= 0,		    BODY				= 3,
		    PROTECT_ARM			= 4,		    PELVIS				= 5,
		    ACCESSORY1			= 6,		    ACCESSORY2			= 7,
		    ACCESSORY3			= 8,		    SKILL_ARM			= 9,
		    WEAPON				= 11,

		    MAX_EQUPMENT_POS	= 16,

		    TEMP_ITEM_INDEX		= 128
        };
	};


	// ������ ���� ����
	namespace StatusLimit 
    {        
        enum Type
        {
		    NONE    = 0,		    LEVEL   = 1,
		    STR     = 2,		    DEX     = 3,
		    CON     = 4,		    INT     = 5,
		    WIS     = 6,

    		MAX_STATUS_LIMIT = 7
        };
	};

    // ������ ���
    namespace Grade 
    {
        enum Type
        {
            ETC 			=	0,
		    NORMAL			=	1,
		    ENHANCED		=	2,
		    RARE			=	3,
		    MAX_ITEM_GRADE	=	4
        };

        extern const CTypeName   Grades[MAX_ITEM_GRADE];
	};


    namespace ExtraSpacePos
	{
        enum Type
        {
		    UPGRADE_EQUIPMENT_POS	=	0,
		    UPGRADE_MATERIAL_POS	=	1,
		    HOLDITEM_POS			=	2,
			KIT_MATERIAL_POS		=	3,
			CAMP_MATERIAL_POS		=	4,	// ��� ����
			EMBLEM_JEWEL_POS1		=	5,	// ��� ����
			EMBLEM_JEWEL_POS2		=	6,	// ��� ����
			EMBLEM_JEWEL_POS3		=	7,	// ��� ����
			EMBLEM_JEWEL_POS4		=	8,	// ��� ����
			EMBLEM_JEWEL_POS5		=	9,	// ��� ����
			EMBLEM_JEWEL_POS6		=	10,	// ��� ����
			EMBLEM_JEWEL_POS7		=	11,	// ��� ����
			EMBLEM_JEWEL_POS8		=	12,	// ��� ����
			EMBLEM_JEWEL_POS9		=	13,	// ��� ����
		    
			MAX_EXTRA_SPACE_NUM		=	14,

			EMBLEM_JEWEL_POS_NUM	=	9
        };
	};


    namespace EtcItemID
    {
        enum Type
        {
            // ���� ������ ID
            GEM_START_ID            = 1901,
            GEM_END_ID              = 1915,

            // ���� ������ ID
            MINERAL_START_ID        = 2001,
            MINERAL_END_ID          = 2003,

			SYLVINE					= 2001,
			MYTHRIL					= 2002,
			ETERNIUM				= 2003,

			// ȸ���� ������ ID
			POTION_START_ID			= 3020,
			POTION_END_ID			= 3124,

			// �����ǵ� ID
			OBLIVION_STONE			= 9914,
			// ��Ȱ�ǵ� ID
			REBIRTH_STONE			= 9201,
			// ����㰡�� ID
			RENAME_WARRANT			= 9200,

			// ���� ���� ��ŸƮŶ ID
			CAMP_KIT_ID				= 7100,
			SHORT_RANGE_ARMS_KIT_ID	= 7101,
			LONG_RANGE_ARMS_KIT_ID	= 7102,
			AIRSHIP_KIT_ID			= 7103,
			
			// ���� ���� ���� ID
			SIEGE_MATERIAL_ID		= 9914,

            // ��ö ID
            SCRAPS                  = 2101
        };
    };

	namespace Chemical
	{
		// ������ �ռ� ���
		enum Result
		{
			S_SUCCESS			= 0,	// ����

			F_WRONG_RECIPE		= 2,	// �ùٸ� �ռ����� �ƴմϴ�.
			F_NOT_ENOUGH_ITEM	= 3,	// �ռ� �������� ���� �ּҷ��� ��ġ�� ���մϴ�.
			F_WRONG_RATE		= 4		// ��� �������� ���ڿ� ������ ���� �߻��մϴ�. �ռ� ������ �����ֽñ� �ٶ��ϴ�.
		};
	};
};


#endif