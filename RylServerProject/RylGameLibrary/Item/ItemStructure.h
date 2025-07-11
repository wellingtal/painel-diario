#ifndef _ITEM_STRUCTURE_H_
#define _ITEM_STRUCTURE_H_

#include <Creature/CreatureStructure.h>
#include "ItemConstants.h"


namespace Item
{
	class CItem;

    #pragma pack(1)

    // ------------------------------------------------------------------------------------------
    // UseLimit, ItemInfo �� ��ũ��Ʈ���� �ִ´�.

    struct ItemPos
    {
        unsigned short  m_cPos      : 4;    //  ������ ������ (Equip, Inven...) (0~15)
        unsigned short  m_cIndex    : 12;   //  ������ �ε��� (0~4095)

        ItemPos() : m_cPos(0), m_cIndex(0) { }
        ItemPos(unsigned char cPos, unsigned short usIndex) : m_cPos(cPos), m_cIndex(usIndex) { }

        inline void SetPos(unsigned short wIndex) { m_cIndex = wIndex; }
        inline void SetPos(unsigned char cXIndex, unsigned char cYIndex, unsigned char cZIndex);        
        inline void GetPos(unsigned char& cXIndex, unsigned char& cYIndex, unsigned char& cZIndex) const;

        inline unsigned char GetXIndex(void) const { return (m_cIndex & 0x00F); }
        inline unsigned char GetYIndex(void) const { return (m_cIndex & 0x0F0) >> 4; }
        inline unsigned char GetZIndex(void) const { return (m_cIndex & 0xF00) >> 8; }

        inline void SetXIndex(unsigned char cXIndex) { m_cIndex = (m_cIndex & 0xFF0) + cXIndex; }
        inline void SetYIndex(unsigned char cYIndex) { m_cIndex = (m_cIndex & 0xF0F) + (cYIndex << 4); }
        inline void SetZIndex(unsigned char cZIndex) { m_cIndex = (m_cIndex & 0x0FF) + (cZIndex << 8); }

        inline bool operator == (const ItemPos& rhs) const { return (m_cPos == rhs.m_cPos && m_cIndex == rhs.m_cIndex); }
    };

    inline void ItemPos::SetPos(unsigned char cXIndex, unsigned char cYIndex, unsigned char cZIndex)
    {
        m_cIndex = (cZIndex << 8) + (cYIndex << 4) + cXIndex;
    }

    inline void ItemPos::GetPos(unsigned char& cXIndex, unsigned char& cYIndex, unsigned char& cZIndex) const
    {
        cXIndex = (m_cIndex & 0x00F);
        cYIndex = (m_cIndex & 0x0F0) >> 4;
        cZIndex = (m_cIndex & 0xF00) >> 8;
    }


    struct ItemData
    {
        unsigned __int64    m_dwUID;                //  ������ UID
        unsigned short      m_usProtoTypeID;        //  ������ ���� ID
        ItemPos             m_ItemPos;              //  ������ ��ġ
        unsigned char       m_cItemSize;            //  ������ ����ü ũ��
        unsigned char       m_cNumOrDurability;     //  ���� Ȥ�� ������

        ItemData() { m_dwUID = m_usProtoTypeID = m_cNumOrDurability = 0; m_cItemSize = sizeof(ItemData); }
        void DumpInfo(unsigned long dwCID, const char* szExtraString) const;
    };

    struct ItemAttribute
    {
        unsigned short  m_cType     : 4;    //  �Ӽ� ���� 4bit (16����)
        unsigned short  m_usValue   : 12;   //  �Ӽ� ��  12bit (0 ~ 4096)

        ItemAttribute() : m_cType(0), m_usValue(0) { }
        ItemAttribute(unsigned char cType, unsigned short usValue) : m_cType(cType), m_usValue(usValue) { }
    };

    // �ڿ� Socket���빰�� m_cSocketNum��ŭ, Attribute�� m_cDiffAttributeNum��ŭ �ٴ´�.    
    struct EquipmentInfo
    {
        enum
        {
            MAX_UPGRADE_LEVEL				= 14,   // blackdog supermod
			MAX_CORE_LEVEL					= 5,	// �ִ� �ھ� ���� (�ھ�� �ھ�� �߶� Ư���� �ɷ��� �ο��ϴ� �ý���)

			MAX_ATTRIBUTE_PER_GEM			= 8,	// ���� �ϳ��� ��ȭ�Ǵ� �ִ� �Ӽ� ��
			MAX_ATTRIBUTE_PER_UPGRADE_LEVEL	= 2,	// �� �ܰ迡 ��ȭ�Ǵ� �ִ� �Ӽ� ��
            MAX_UPGRADE_ATTRIBUTE       	= 6,    // �������� ���ؼ� ���� �� �ִ� �ִ� �Ӽ� ��

            MAX_MINSIZE_SOCKET_NUM      	= 6,    // ũ�Ⱑ 1by1�� �������� �ִ� ���� ��
            MAX_MINSIZE_ATTRIBUTE_NUM   	= 6,    // ũ�Ⱑ 1by1�� �������� �ִ� �Ӽ� ��

            MAX_SOCKET_NUM              	= 12,    //blackdog  was 8 // blackdog supermod
            MAX_SOCKET_ATTRIBUTE_NUM    	= 96,   // blackdog was 16
			// edith 2009.09.16 ���� ���� ����
//			MAX_ATTRIBUTE_NUM           	= 22,   // �Ϲ� �������� �ִ� �Ӽ� ��
            MAX_ATTRIBUTE_NUM           	= 96,   // �Ϲ� �������� �ִ� �Ӽ� ��

			MAX_RUNE_SOCKET_NUM				= 2,	// �����Ҽ� �ִ� ���� �ִ� ��

            MAX_SOCKET_AND_ATTRIBUTE_MINISIZE	= (MAX_MINSIZE_SOCKET_NUM * sizeof(char) + MAX_MINSIZE_ATTRIBUTE_NUM * sizeof(ItemAttribute)),
            MAX_SOCKET_AND_ATTRIBUTE_SIZE		= (MAX_SOCKET_NUM * sizeof(char) + MAX_ATTRIBUTE_NUM * sizeof(ItemAttribute))
        };
        
		unsigned short	m_cUpgradeCnt			:	2;	// ��ý� ����� ���� ������ ����� ī��Ʈ.
        unsigned short  m_cReserved1			:   9;	// ���� ������... (0~511)
		short           m_cUpgradeLevel			:   5;  // ���׷��̵� �ܰ� (-16 ~ 15)

        char            m_cDiffMaxSocket        :   5;  // ���� ������ ���� �� + m_cDiffMaxSocket = �� �������� �ִ� ���� �� (-16 ~ 15)
        unsigned char   m_cSeasonRecord			:	3;  // ������ ���� �ñ� (0~7) (���뷱�� �� : 0, �� : 1, �������� �ɷ����� : 2)

        unsigned char   m_cSocketNum            :   4;  // �� �������� ���� ���� �� (0 ~ 15)
        unsigned char   m_cAttributeNum         :   4;  // ���� �����ۿ� ���ų�, ��ġ�� �ٸ� �Ӽ� �� (0 ~ 15��)

        char            m_cDiffMaxDurability;           // ���� �����۰��� �ִ� ������ ����.
		unsigned char   m_cCoreLevel			:	4;  // �ھ��� ����	(0 ~ 15)
		unsigned char   m_cReserved2			:	4;	// ���� ������...(0~124)

        EquipmentInfo()
        {
            m_cUpgradeCnt = m_cReserved1 = m_cUpgradeLevel = m_cDiffMaxSocket = m_cDiffMaxDurability = 
                m_cSocketNum = m_cAttributeNum = m_cSeasonRecord = m_cCoreLevel = m_cReserved2 = 0;
        }
    };

    #pragma pack()

    const unsigned char MAX_ITEM_SIZE = sizeof(ItemData) + 
        sizeof(EquipmentInfo) + EquipmentInfo::MAX_SOCKET_AND_ATTRIBUTE_SIZE;


    struct SpriteData
    {
	    enum { MAX_NAME = 64, MAX_SPRITE_NAME = 32 };

	    char    m_szName[MAX_NAME];
	    char    m_szSpriteName[MAX_SPRITE_NAME];

	    unsigned short	m_nSpriteMinX;
	    unsigned short	m_nSpriteMinY;
	    unsigned short	m_nSpriteMaxX;
	    unsigned short	m_nSpriteMaxY;

        SpriteData();
        void Initialize();
    };

    
    struct StringData
    {
	    enum
        {
            MAX_FIELD_MODEL_NAME = 32,  MAX_EQUIP_MODEL_NAME = 32,
            MAX_EFFECT_SOUND_NAME = 32, MAX_TYPE_NAME = 32, 
			MAX_ITEM_DESCRIBE = 256
	    };

	    char	m_szFieldModelName[MAX_FIELD_MODEL_NAME];
	    char	m_szEquipModelName[MAX_EQUIP_MODEL_NAME];
	    char	m_szEffectSoundName[MAX_EFFECT_SOUND_NAME];
		char	m_szTypeName[MAX_TYPE_NAME];
	    char	m_szItemDescribe[MAX_ITEM_DESCRIBE];

        StringData();
        void Initialize();
    };

	
    struct DetailData
    {
        enum Flags
        {
            EQUIP           = (1 << 0),  	// ����Ҽ� �ִ��� ����
            USE_ITEM        = (1 << 1),  	// ����� �� �ִ��� ����            
            QUICKSLOT_IN    = (1 << 2),  	// �����Կ� �� �� �ִ��� ����
            STACKABLE       = (1 << 3),  	// ������ �� �ִ��� ����

			HELM			= (1 << 4),		// ������
			ARMOR			= (1 << 5),  	// ���ʷ�
            DAGGER          = (1 << 6),  	// �ܰ˷�
            ONEHANDED       = (1 << 7),  	// �Ѽ� �����
			LONGRANGE		= (1 << 8),		// ��Ÿ� ����� 
            TWOHANDED       = (1 << 9),  	// ��� �����
			STAFF			= (1 << 10),	// �����̷�
			SHIELD			= (1 << 11),	// ���з�

			//--//	start..
			NECKLACE		= (1 << 12),	// �����
			RING			= (1 << 13),	// ����
			RUNE			= (1 << 14),	// ��
			//--//	end..
			AVATA			= (1 << 15)		// �ƹ�Ÿ����
        };

		unsigned long   m_dwPrice;			// �⺻ ����
		unsigned long	m_dwBlackPrice;		// �Ͻ���� �Ǹ� ����
		unsigned long	m_dwMedalPrice;		// �޴޻����� �Ǹ� ����

		unsigned short	m_wDropOption;		// ��ġ å���� ���� ��
		unsigned char	m_cGrade;			// ��ġ å������ 0 �϶� ����� ���	//--//
		unsigned char	m_cCriticalType;	// ũ��Ƽ�� Ÿ��
		unsigned char	m_cAttackRange;		// ���� �Ÿ�
                
		unsigned char	m_cItemType;		// ������ ����( namespace Type ���� )
		unsigned char	m_cXSize;       	// ������ Xũ��
		unsigned char	m_cYSize;       	// ������ Yũ��

		unsigned char   m_cMaxSocketNum;				// �ִ� ���ϼ�
		unsigned char   m_cDefaultDurabilityOrStack;	// �⺻ ����/������
        unsigned char   m_cMaxDurabilityOrStack;		// �ִ� ����/������

		unsigned char	m_cMinDropLevel;	// ����ϴ� ������ �ּ� ����
		unsigned char	m_cMaxDropLevel;	// ����ϴ� ������ �ִ� ����

		unsigned short	m_wLowLimitLevel;	// ���� ���Ѽ�.
		unsigned short	m_wHighLimitLevel;	// ���� ���Ѽ�.

		bool			m_bExchangeAndDrop;	// ��ȯ�� ����� �����Ѱ�?
		bool			m_bSell;			// �� �� �ִ� �������ΰ�?

		bool			m_bOptionPrice;		// �ɼ��� �����ؼ� ���� å���� �ٽ� �� ���ΰ�?

        unsigned long   m_dwFlags;

		DetailData();
        void Initialize();
    };
	
    struct EquipAttribute
    {           
        unsigned short m_usAttributeValue[Item::Attribute::MAX_ATTRIBUTE_NUM];        
        unsigned short m_nAttibuteNum;

        EquipAttribute();
        void Initialize();
    };

    struct UseItemInfo
    {
		Position		m_Pos;					// �̵� ��ǥ (�����̵���)
		unsigned char	m_cZone;				// �̵� �� (�����̵���)

		unsigned long	m_dwAmount;				// �� (����ġ����, ĳ����)

		unsigned short	m_usSkill_ID;			// ���� ��ų ���̵� (����, ��ų��)
        unsigned short	m_usSkill_LockCount;	// ���� ��ų ��ī��Ʈ (����, ��ų��)

		unsigned short	m_wQuestID;				// ���� ����Ʈ ���̵� (����Ʈ ���� ������)

		unsigned char	m_cLimitRealmPoint;		// ���� ���� ����Ʈ�� ��� ����
		bool			m_bForStatueWar;		// ������(������) �ð����� ��� ����

        UseItemInfo();
        void Initialize();
    };

    struct UseLimit
    {        
        enum DeactivatedType
		{
			STATUS_LIMIT    = ( 1 << 0 ),
			CLASS_LIMIT     = ( 1 << 1 ),
			BROKEN_ITEM     = ( 1 << 2 )
		};
                
        unsigned long			m_dwClassLimit;

		unsigned char			m_cLimitStatus;
		unsigned short			m_wLimitValue;
         
        UseLimit();
        void Initialize();
	};

    #pragma pack(8)

    struct ItemInfo
    {
        unsigned short	m_usProtoTypeID;

        DetailData      m_DetailData;
        UseLimit        m_UseLimit;
        SpriteData      m_SpriteData;
        StringData      m_StringData;

        EquipAttribute  m_EquipAttribute;
        UseItemInfo     m_UseItemInfo;

        ItemInfo();
        ItemInfo(unsigned short usPrototypeID);
        void Initialize();

        inline bool operator < (ItemInfo& rhs)
        { 
			return (m_usProtoTypeID < rhs.m_usProtoTypeID); 
		}

		const char* GetItemDescribe(void) const;
    };

    #pragma pack()


	struct ItemGarbage
	{
		CItem*			m_lpItem;
		unsigned long	m_dwRemainNum;

		ItemGarbage(CItem* lpItem, unsigned long dwRemainNum);
	};

	struct ChemicalInfo
	{
		unsigned short	m_wPickkingItemID;
		unsigned short	m_wTargetItemID;
		unsigned short	m_wResultItemID;

		unsigned char	m_cPickkingItemNum;
		unsigned char	m_cTargetItemNum;
		unsigned char	m_cResultItemNum;

		unsigned long	m_dwItemID;		// ������ ���̵�
		unsigned short	m_usAbilityID;		// ����̵�
		unsigned char	m_cAbilityLevel;
		unsigned short	m_usQuestID;		// �����̵�		
		unsigned char	m_cPercent;		// ����Ȯ��


		ChemicalInfo();
		ChemicalInfo(unsigned short wPickkingItemID, unsigned char cPickkingItemNum, 
			unsigned short wTargetItemID, unsigned char cTargetItemNum, unsigned int dwItemID, unsigned short usAbilityID, unsigned char cAbilityLevel, unsigned short	usQuestID, unsigned char cPercent);

		bool operator < (const ChemicalInfo& rhs)
		{ 
			if (m_wPickkingItemID == rhs.m_wPickkingItemID)
			{
				return m_wTargetItemID < rhs.m_wTargetItemID; 
			}

			return m_wPickkingItemID < rhs.m_wPickkingItemID; 
		}

		bool operator == (const ChemicalInfo& rhs)
		{
			return (m_wPickkingItemID == rhs.m_wPickkingItemID && m_wTargetItemID == rhs.m_wTargetItemID && 
				m_cPickkingItemNum == rhs.m_cPickkingItemNum && m_cTargetItemNum == rhs.m_cTargetItemNum);
		}
	};

	namespace Grade 
	{
		struct GradeInfo
		{
			EquipType::Grade	m_eItemGrade;
			unsigned char		m_cPlus;

			EquipType::Grade	m_aryAttributeGrade[Attribute::MAX_ATTRIBUTE_NUM];

			GradeInfo();
		};

		enum TableType
		{
			DROP_MIN_VALUE		= 0,	// ��� �׷��̵� �ּҰ�
			DROP_MAX_VALUE		= 1,	// ��� �׷��̵� �ִ밪
			VALUE_GRADE			= 2,	// ��ġ �׷��̵�

			MAX_TABLE_TYPE		= 3
		};

		const float GetGradeValue(Item::CItemType::ArrayType eEquipType, Grade::TableType eTableType, 
			EquipType::Grade eGrade, Attribute::Type eType);
	};
};

#endif