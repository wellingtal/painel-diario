#ifndef _ITEM_CONSTANTS_H_
#define _ITEM_CONSTANTS_H_

#include <vector>
#include <Utility/TypeArray.h>

namespace Item
{
    // ���� ����
    struct ItemInfo;
    struct ItemAttribute;

	class CItem;

	enum Const
	{
		MAX_GEM_KIND				= 15,		// ������ ����
		MAX_LEVEL_OPTION_UPGRADE	= 5,		// ���ý� �ɼ��� �����ϴ� �ִ� ���� (���� �������� ������ �����Ѵ�.)
		MAX_OPTION_BASE_NUM			= 6,		// �� �����ۿ� ���� �� �ִ� +�� �ִ� ����

		GEMETYPE_RUNE				= 5,		// 5������ ��Ÿ���� ���̴�
		GEMETYPE_RUNE_NUM			= 12,		// ��Ÿ���� ���� �� 12���̴� ���� 3�� ���

		MAX_TEMP_INVEN_ITEM_NUM		= 50,		// �ӽ� �κ��丮�� ���� �� �ִ� �ִ� ������ ����

		PRICE_AGGRAVATION			= 7,		// ���뷱�� �� �����ۿ� ���� ���� ����ġ

		GM_MODEL_ITEM				= 49999,	// GM �𵨷� ���� ������ ���̵�.

		MAX_UPGRADE_COUNT			= 3,		// ����� �� ���� ���׷��̵� ��.
	};

    namespace ItemType
    {
        // ������ Ÿ�� ( 8bit )
        enum Type
        {
			NONE					= 0, 

			// ���ɼ� ���
			SHIRT					= 1,	TUNIC					= 2,

			// �ΰ� ��
			CON_ARMOUR				= 3,	CON_HELM				= 4,
			CON_GLOVE				= 5,	CON_BOOTS				= 6,
			DEX_ARMOUR				= 7,	DEX_HELM				= 8, 
			DEX_GLOVE				= 9,	DEX_BOOTS				= 10, 

			// �ΰ� ����
			ONEHANDED_SWORD			= 11,	TWOHANDED_SWORD			= 12, 
			ONEHANDED_AXE			= 13,	TWOHANDED_AXE			= 14,
			ONEHANDED_BLUNT			= 15,	TWOHANDED_BLUNT			= 16,
			BOW						= 17,	CROSSBOW				= 18,
			STAFF					= 19,	DAGGER					= 20,
			SHIELD					= 21, 

			// ��ĭ ��
			CON_BODY				= 22,	CON_HEAD				= 23, 
			CON_PELVIS				= 24, 	CON_PROTECT_A			= 25,
			DEX_BODY				= 26,	DEX_HEAD				= 27, 
			DEX_PELVIS				= 28,	DEX_PROTECT_A			= 29, 

			// ��ĭ ����
			COM_BLUNT				= 30,	COM_SWORD				= 31,
			OPP_HAMMER				= 32, 	OPP_AXE					= 33, 
			OPP_SLUSHER				= 34,	OPP_TALON				= 35, 
			OPP_SYTHE				= 36,	

			// ��ų��
			SKILL_A_GUARD			= 37,	SKILL_A_ATTACK			= 38,
			SKILL_A_GUN				= 39,	SKILL_A_KNIFE			= 40, 

			// ź���
			ARROW					= 41,	BOLT					= 42, 
			AMMO					= 43, 

			// ��� ������
			POTION					= 44,	EXP_BOOK				= 45,
			CASH_BOOK				= 46,	EVENT_LOTTERY			= 47, 
			PORTAL					= 48,	FIRE_CRACKER			= 49, 
			SKILL_BOOK				= 50,	FIXED_PORTAL			= 51,	
			SKILL_ITEM				= 52,	CASH_ITEM				= 53, 

			// ��� �Ұ� ������
			GEM						= 54,	ORE						= 55,
			ETC						= 56, 	

			// ������ ��Ÿ��Ŷ
			CAMP_KIT				= 57,	SHORT_RANGE_ARMS_KIT	= 58, 
			LONG_RANGE_ARMS_KIT		= 59, 	AIRSHIP_KIT				= 60,
			WORLDWEAPON_K_KIT		= 61,	WORLDWEAPON_M_KIT		= 62,
			MINERAL_KIT				= 63,

			// ������ ����Ʈ�� ������
			QUEST_ITEM				= 64,

			// �Ǽ��縮 ������
			NECKLACE				= 65,	RING					= 66,
			RUNE					= 67,

			// �Ǹ� ���� �߰� ������(����/�ǸŰ� ����. 2006-06-19 by hackermz)
			GEM_SELL				= 68,

			RIDE					= 69,	TRANSFORM				= 70,

			MAX_ITEM_TYPE			= 71,
        };
    };

	// ������ ���� ����
	namespace StatusLimit 
	{        
		enum Type
		{
			NONE	            = 0,	
			STR		            = 1,	
			DEX		            = 2,	
			CON		            = 3,	
			INT		            = 4,	
			WIS		            = 5, 
			LEVEL				= 6,

			MAX_STATUS_LIMIT    = 7
		};
	};

    namespace Attribute
    {
		enum Type
		{
			// ---------------------------------------------------------------------------
			// �� ���� �Ӽ��� DB�� ����Ǵ� ������ �����ÿ� DB �������� �ʿ��մϴ�.
			NONE					= 0,

			MIN_DAMAGE				= 1,
			MAX_DAMAGE				= 2,
			ARMOR					= 3,
			HIT_RATE				= 4,
			EVADE					= 5,

			MAX_HP					= 6,
			HP_REGEN				= 7,
			MAX_MP					= 8,
			MP_REGEN				= 9,

			CRITICAL				= 10,
			BLOCK					= 11,
			SPEED					= 12,

			MAGIC_POWER				= 13,
			MAGIC_RESIST			= 14,

			LUCKCHANCE				= 15,	
			MAX_DB_ATTRIBUTE_NUM	= 16, 
			// ---------------------------------------------------------------------------

			// ---------------------------------------------------------------------------
			// ���� �߰� �Ӽ��� DB�� ������� �ʽ��ϴ�. (���� �ɼ��� ���� �� �����ϴ�.)
			COOLDOWN				= 16,
			SKILL_POINT				= 17,

			FROST					= 18,
			FIRE					= 19,
			ELECTRO					= 20,
			DARKNESS				= 21,
		//	ADD_STR				    = 21,

			RUNTYPE					= 22,
			MAX_ATTRIBUTE_NUM		= 23	
			// ---------------------------------------------------------------------------
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
			ARMOR_TYPE				= 0,
			DAGGER_TYPE				= 1,
			ONEHANDED_WEAPON_TYPE	= 2,
			LONGRANGE_WEAPON_TYPE	= 3,
			TWOHANDED_WEAPON_TYPE	= 4,
			STAFF_TYPE				= 5,
			SHIELD_TYPE				= 6,

			MAX_TYPE				= 7
		};

		enum Grade
		{
			S_GRADE			= 0,
			AAA_GRADE		= 1,
			AA_GRADE		= 2,
			A_GRADE			= 3,
			B_GRADE			= 4,
			C_GRADE			= 5,
			D_GRADE			= 6,
			F_GRADE			= 7,
			X_GRADE			= 8,
			
			MAX_GRADE		= 9
		};

		enum OptionType
		{
			NONE					= 0,
			STANDARD_OPTION			= 1,
			OVER_OPTION				= 2,
			NO_OPTION				= 3,

			MAX_OPTION_TYPE			= 4
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
		    WEAPON_HAND2		= 12,		    AVATA				= 13,	

            // ��ĭ ��� ��ġ
		    HEAD				= 0,		    BODY				= 3,
		    PROTECT_ARM			= 4,		    PELVIS				= 5,
			SKILL_ARM			= 9,		    WEAPON				= 11,

		    MAX_EQUPMENT_POS	= 15,

		    TEMP_ITEM_INDEX		= 128
        };
	};

	namespace ExtraSpacePos
	{
        enum Type
        {
			// --------------------------------------------------------------------------
			// 2004-07-14 by �ε�
			//
			// Extra�� DB ũ��� ���� ��뷮�� �ִ� 494 / 508 bytes 
			// ( DB ũ�⸦ �ʰ����� �ʵ��� �����սô�. )
			//
			// ���� �ڷ� :	������ ũ��� 14 bytes, 
			//				����� �ִ� ũ��� 52 bytes 
			//				( Ǯ�ɼ��϶�... 14(�⺻) + 6(���) + 8(����) + 24(�ɼ�) )
			// --------------------------------------------------------------------------
			
		    UPGRADE_EQUIPMENT_POS	= 0,	// ���� ���
		    UPGRADE_MATERIAL_POS	= 1,	// ���� ����

			HOLDITEM_POS			= 2,	// ��� �ִ� ������ 

			KIT_MATERIAL_POS		= 3,	// �������� ���� ����
			CAMP_MATERIAL_POS		= 4,	// ��� ��� ���� ���� (��� ����)

			EMBLEM_JEWEL_POS1		= 5,	// ��¡�� ���׷��̵� ���� (��� ����)
			EMBLEM_JEWEL_POS2		= 6,	// (��� ����)	
			EMBLEM_JEWEL_POS3		= 7,	// (��� ����)	
			EMBLEM_JEWEL_POS4		= 8,	// (��� ����)
			EMBLEM_JEWEL_POS5		= 9,	// (��� ����)	
			EMBLEM_JEWEL_POS6		= 10,	// (��� ����)	
			EMBLEM_JEWEL_POS7		= 11,	// (��� ����)	
			EMBLEM_JEWEL_POS8		= 12,	// (��� ����)	
			EMBLEM_JEWEL_POS9		= 13,	// (��� ����)	

			GRAFT_ORIGINAL_POS		= 14,	// �ɼ� �̽� ���� ���
			GRAFT_SACRIFICE_POS		= 15,	// �ɼ� �̽� ���� ���
			GRAFT_RESULT_POS		= 16,	// �ɼ� �̽� ��� ���

			COMPENSATION_POS		= 17,	// ���� �Ǹ� ���
		    
			MAX_EXTRA_SPACE_NUM		= 18,

			EMBLEM_JEWEL_POS_NUM	= 9
        };
	};

	namespace GiveItem 
	{
		enum ExceptionUID 
		{
			// �������� ������ ���޽� �ӽ� �κ��丮�� ���� ���¶�� ������ ����Ѵ�. �� ��, ���Ǵ� ���� UID
			PENDING_ITEM_UID		= 0xFFFFFFFF
		};
	};

    namespace EtcItemID
    {
        enum Type
        {
            // ���� ������ ID
            GEM_START_ID            = 1901, // was 1901
            GEM_END_ID              = 1915, // was 1942

            GEM_PERFECTEND_ID       = 1960,
			GEM_RUNE_START_ID		= 1961,		// �����Ӽ��ε� �����÷��ִ� ������(�麸���̶�� �Ҹ�)
			GEM_RUNE_END_ID			= 1975,
			GEM_UNSOCKET_ID			= 1990,
			GEM_MAXSOCKET_ID		= 1991,

			// �� ������ ID
			RUNE_START_ID			= 40000,	// ���� ���̵�� �����ۿ� �߰��� �� ���� (��� �Ҹ��� �� ID�� ���� ���� ����Ʈ!)
			RUNE_END_ID				= 44096,

			// �Ҹ��� �� ID
			DESTRUCTION_RUNE		= 40000,

            // ���� ������ ID
            MINERAL_START_ID        = 2001,
            MINERAL_END_ID          = 2003,

			SILVIN					= 2001,
			MITHRIL					= 2002,
			ITERNIUM				= 2003,
			SOULMETAL				= 2004,
			ANTONIUM				= 2005,
			ANCIENTMETAL			= 2006,
			ENDURANCESTONE			= 2007,	// ������ ���� ������
			AWAKENCORE				= 2008,	// ������ �ھ� (�����ڵ�3���� ����ǰ� �������� ���뷹��-5 ȿ��)

			// ��ö ID
			SCRAPS                  = 2101,

			// ȸ���� ������ ID
			BREAD					= 3020,
			RARE_STEAK				= 3021, 
			WELLDONE_STEAK			= 3022, 
			BACON					= 3023, 
			MANNA1					= 3024,
			MANNA2					= 3025,
			MANNA3					= 3026,
			MANNA4					= 3027,

			// ��ų�� ���� ID
			SKILL_COUPON_3			= 3001,
			SKILL_COUPON_4			= 3002,

			// ���� ���� ��ŸƮŶ ID
			CAMP_KIT_ID				= 7100,
			SHORT_RANGE_ARMS_KIT_ID	= 7101,
			LONG_RANGE_ARMS_KIT_ID	= 7102,
			AIRSHIP_KIT_ID			= 7103,
			WORLDWEAPON_K_KIT		= 7105,
			WORLDWEAPON_M_KIT		= 7106,
			MINERAL_KIT				= 7107,

			// ���� ���� ���� ID
			SIEGE_MATERIAL_ID		= 7104,

			// �����ǵ� ID
			OBLIVION_STONE			= 9914,
			// ��Ȱ�ǵ� ID
			REBIRTH_STONE			= 9201,
			// ����㰡�� ID
			RENAME_WARRANT			= 9200,

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
			F_WRONG_RATE		= 4,	// ��� �������� ���ڿ� ������ ���� �߻��մϴ�. �ռ� ������ �����ֽñ� �ٶ��ϴ�.
			F_NOT_QUEST			= 5,	// �ʿ�����Ʈ�� ���� ���ߴ�.
			F_NOT_ABILITY		= 6,	// �ش� �����Ƽ�� �������� �ʴ´�.
		};
	};

	namespace Compensation
	{
		// Ŭ������ Ư�� ���� �Ǹ�
		enum SpeacialCase
		{
			CASE1		= ItemType::MAX_ITEM_TYPE, 
			CASE2		= ItemType::MAX_ITEM_TYPE + 1, 
			CASE3		= ItemType::MAX_ITEM_TYPE + 2, 
			CASE4		= ItemType::MAX_ITEM_TYPE + 3, 
			CASE5		= ItemType::MAX_ITEM_TYPE + 4, 
			CASE6		= ItemType::MAX_ITEM_TYPE + 5, 
			CASE7		= ItemType::MAX_ITEM_TYPE + 6, 
			CASE8		= ItemType::MAX_ITEM_TYPE + 7, 
			CASE9		= ItemType::MAX_ITEM_TYPE + 8, 

			MAX_CASE	= ItemType::MAX_ITEM_TYPE + 9
		};
	};

	class CItemType
    {
    public:

        enum ArrayType
        {
            EQUIP_TYPE			= 0,
			USE_ITEM_TYPE		= 1,

			WEAPON_TYPE			= 2,    
			SKILL_ARM_TYPE		= 3,    

			HELM_TYPE			= 4,
			ARMOUR_TYPE			= 5,
			DAGGER_TYPE			= 6,
			ONEHANDED_TYPE		= 7,
			LONGRANGE_TYPE		= 8,
			TWOHANDED_TYPE		= 9,
			STAFF_TYPE			= 10,
			SHIELD_TYPE			= 11,

			STANDARD_TYPE		= 12,
			OVER_TYPE			= 13,
			NO_TYPE				= 14,

			ACCESSORY_TYPE		= 15,

			AVATA_TYPE			= 16,

			MAX_ARRAY_TYPE		= 17
        };

        typedef std::vector<ItemAttribute>    ItemAttributeArray;

        static CItemType& GetInstance();

        // ���� ��� �Լ�        
		static ArrayType GetEquipType(unsigned long dwItemFlag);

		// Rodin : �� �Լ����� CEquipment�� ��� �Լ��� �Ǵ°� ���� ��? (�׷��鼭 CEquipment�� friend�� ����!)
		static void SetInstallGemAttribute(CItem* lpItem);
		static void SetUpgradeItemAttribute(CItem* lpItem);       

		// ��� �Լ�
        bool IsCorrectItemType(ArrayType itemType, unsigned char cEquipType);
		bool IsCorrectOptionType(EquipType::OptionType eOptionType, ItemType::Type eItemType);

		EquipType::OptionType GetOptionType(ItemType::Type eItemType);

        void SetEquipTypeFlags(ItemInfo& itemInfo);
		void SetAccessoryTypeFlags(ItemInfo& itemInfo);
		void SetAvataTypeFlags(ItemInfo& itemInfo);
        void SetUseItemTypeFlags(ItemInfo& itemInfo);
        void SetEtcItemTypeFlags(ItemInfo& itemInfo);

        CTypeName* GetItemTypeName(void)	{ return m_ItemTypeNames;	}
		CTypeName* GetItemLimitName(void)	{ return m_ItemLimitNames;	}
		CTypeName* GetItemGradeName(void)	{ return m_ItemGradeNames;	}

    private:

        CItemType();

        typedef std::vector<ItemType::Type> ItemTypeArray;
        ItemTypeArray                       m_ItemTypes[MAX_ARRAY_TYPE];
        CTypeName                           m_ItemTypeNames[ItemType::MAX_ITEM_TYPE];
		CTypeName							m_ItemLimitNames[StatusLimit::MAX_STATUS_LIMIT];
		CTypeName							m_ItemGradeNames[EquipType::MAX_GRADE];
    };
};


#endif