#ifndef _OLD_ITEM_STRUCTURE_H_
#define _OLD_ITEM_STRUCTURE_H_

#include <Creature/CreatureStructure.h>
#include "OldItemConstants.h"


namespace OldItem
{
    class CItem;

    #pragma pack(1)

    // ------------------------------------------------------------------------------------------
    // UseLimit, ItemInfo 는 스크립트에만 넣는다.

    struct ItemPos
    {
        unsigned short  m_cPos      : 4;    //  아이템 포지션 (Equip, Inven...) (0~15)
        unsigned short  m_cIndex    : 12;   //  아이템 인덱스 (0~4095)

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
        unsigned __int64    m_dwUID;                //  아이템 UID
        unsigned short      m_usProtoTypeID;        //  아이템 종류 ID
        ItemPos             m_ItemPos;              //  아이템 위치
        unsigned char       m_cItemSize;            //  아이템 구조체 크기.
        unsigned char       m_cNumOrDurability;     //  개수 혹은 내구도

        ItemData() { m_dwUID = m_usProtoTypeID = m_cNumOrDurability = 0; m_cItemSize = sizeof(ItemData); }
        void DumpInfo(unsigned long dwCID, const char* szExtraString) const;
    };

    struct ItemAttribute
    {
        unsigned short  m_cType     : 6;    //  속성 개수 6bit (64가지)
        short           m_usValue   : 10;   //  속성 값  10bit (-512~511까지)

        ItemAttribute() : m_cType(0), m_usValue(0) { }
        ItemAttribute(unsigned char cType, unsigned short usValue) : m_cType(cType), m_usValue(usValue) { }
    };

    // 뒤에 Socket내용물이 m_cSocketNum만큼, Attribute가 m_cDiffAttributeNum만큼 붙는다.    
    struct EquipmentInfo
    {
        enum
        {
            MAX_UPGRADE_LEVEL           = 15,   // 최대 업그레이드 레벨 - 15
            MAX_UPGRADE_SOCKET_ONLY     = 5,    // 소켓만 업그레이드할때 최대 레벨

            MAX_UPGRADE_ATTRIBUTE       = 2,    // 업그레이드로 인해서 얻을 수 있는 최대 속성 수

            MAX_MINSIZE_SOCKET_NUM      = 6,    // 크기가 1by1인 아이템의 최대 소켓 수
            MAX_MINSIZE_ATTRIBUTE_NUM   = 6,    // 크기가 1by1인 아이템의 최대 속성 수

            MAX_SOCKET_NUM              = 8,    // 일반 아이템의 최대 소켓 수
            MAX_SOCKET_ATTRIBUTE_NUM    = 16,   // 소켓 하나당 최대 속성이 두개까지 달릴 수 있음.
            MAX_ATTRIBUTE_NUM           = 12,   // 일반 아이템의 최대 속성 수

            MAX_SOCKET_AND_ATTRIBUTE_MINISIZE	= (MAX_MINSIZE_SOCKET_NUM * sizeof(char) + MAX_MINSIZE_ATTRIBUTE_NUM * sizeof(ItemAttribute)),
            MAX_SOCKET_AND_ATTRIBUTE_SIZE		= (MAX_SOCKET_NUM * sizeof(char) + MAX_ATTRIBUTE_NUM * sizeof(ItemAttribute))
        };
        
        unsigned short  m_cRuneSocket           :   11; //  룬 소켓에 들어있는 아이템 종류 ID
        short           m_cDiffMaterialType     :   5;  //  상점 아이템 재질 + m_cDiffMaterialType = 이 아이템의 재질 값(-16~15)

        char            m_cDiffMaxSocket        :   5;  //  상점 아이템 소켓 수 + m_cDiffMaxSocket = 이 아이템의 최대 소켓 수 (-16~15)
        unsigned char   m_cReserved             :   3;  //  예약 데이터...

        unsigned char   m_cSocketNum            :   4;  //  이 아이템의 현재 소켓 수 (0~15)
        unsigned char   m_cAttributeNum         :   4;  //  상점 아이템에 없거나, 수치가 다른 속성 수(0~15개)

        char            m_cDiffMaxDurability;           //  상점 아이템과의 최대 내구도 차이.
        unsigned char   m_cReserved2;                   //  예약 데이터...

        EquipmentInfo()
        {
            m_cRuneSocket = m_cDiffMaterialType = m_cDiffMaxSocket = m_cDiffMaxDurability = 
                m_cSocketNum = m_cAttributeNum = m_cReserved = m_cReserved2 = 0;
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
            EQUIP           = (  1 <<  0),  // 장비할수 있는지 여부
            USE_ITEM        = (  1 <<  1),  // 사용할 수 있는지 여부            
            QUICKSLOT_IN    = (  1 <<  2),  // 퀵슬롯에 들어갈 수 있는지 여부
            STACKABLE       = (  1 <<  3),  // 스택할 수 있는지 여부
            TWOHANDED       = (  1 <<  4),  // 양손으로 집어야 하는 아이템인지 여부
            WEAPON          = (  1 <<  5),  // 무기인지 여부
            ARMOUR          = (  1 <<  6),  // 갑옷류인지 여부
            SKILL_ARM       = (  1 <<  7),  // 스킬암인지 여부
        };

		unsigned char	m_cItemType;	// 아이템 종류( namespace Type 참고 )
		unsigned char	m_cXSize;       // 아이템 X크기
		unsigned char	m_cYSize;       // 아이템 Y크기
		unsigned char	m_cOptionLimit;
                
        unsigned char   m_cDefaultDurabilityOrStack;
        unsigned char   m_cMaxDurabilityOrStack;
        unsigned char   m_cMaterialType;
        unsigned char   m_cMaxSocketNum;
		unsigned long	m_dwCraftExp;

        Grade::Type     m_ItemGrade;
        unsigned long   m_dwPrice;
		unsigned long	m_dwBlackPrice;
		unsigned long	m_dwMedalPrice;
        unsigned long   m_dwFlags;

        DetailData();
        void Initialize();
    };
	
    struct EquipAttribute
    {           
        unsigned short m_usAttributeValue[OldItem::Attribute::MAX_ATTRIBUTE_NUM];        
        unsigned short m_nAttibuteNum;

        EquipAttribute();
        void Initialize();
    };

    struct UseItemInfo
    {
        unsigned short	m_usSkill_ID;
        unsigned short	m_usSkill_LockCount;

		unsigned char	m_cZone;
		Position		m_Pos;

		unsigned long	m_dwAmount;

        UseItemInfo();
        void Initialize();
    };

    struct UseLimit
    {        
        enum DeactivatedType
		{
			STATUS_LIMIT    = (  1 <<  0),
			CLASS_LIMIT     = (  1 <<  1),
			BROKEN_ITEM     = (  1 <<  2)
		};
                
        unsigned short  m_nStatusLimit[StatusLimit::MAX_STATUS_LIMIT];        
		unsigned short	m_nSkillType;	// 스킬 제한 타입
        unsigned char	m_cSkillLevel;	// 스킬 제한 레벨        
        unsigned char   m_cLevelLimit;
        unsigned long   m_dwClassLimit;
        
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

		// const char* GetItemDescribe(void) const;
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

		ChemicalInfo();
		ChemicalInfo(unsigned short wPickkingItemID, unsigned char cPickkingItemNum, 
			unsigned short wTargetItemID, unsigned char cTargetItemNum);

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
}

#endif