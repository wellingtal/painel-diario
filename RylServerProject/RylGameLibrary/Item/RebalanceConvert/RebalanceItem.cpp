#include "stdafx.h"
#include "RebalanceItem.h"

#include <Item/ItemMgr.h>
#include <Item/ItemSerialMgr.h>
#include <Item/ItemStructure.h>

#include "OldItemMgr.h"
#include "OldItemStructure.h"
#include "OldItemConstants.h"

#include "ContainerChecker.h"

#include <Log/ServerLog.h>
#include <numeric>

namespace RebalanceLib
{

CServerLog g_InvalidItemLog("InvalidItem");
CServerLog g_ItemGradeLog("ItemGrade");

CItemGrade& CItemGrade::GetInstance()
{
    static CItemGrade itemGrade;
    return itemGrade;
}

enum SupressedResult
{
    SUPRESSED_DEFAULT       = 0,
    SUPRESSED_1To7          = 1,
    SUPRESSED_7ToMax        = 2,
    MAX_SUPRESSED_GRADE     = 3
};


// forward delc.
Item::Grade::GradeInfo GetItemGrade(const Item::ItemInfo& newItemInfo, 
                                    short wNewAttribute[Item::Attribute::MAX_ATTRIBUTE_NUM],
                                    Item::EquipType::Grade eSuppressGrade, bool& bIsSuppressed);

Item::Grade::GradeInfo RebalanceAttr(const Item::ItemInfo& newItemInfo, unsigned char cUpgradeLevel, 
                                     short wOldAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM],
                                     short wInvalidAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM],
                                     short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM],
                                     bool bSupressedResult[MAX_SUPRESSED_GRADE]);

namespace UpgradeFactor
{    
    const int MAX_UPGRADECOL_NUM = 12;

    // ���׷��̵尡 �� �����ۿ� ���ؼ� ������ ���ʽ��� �����ݴϴ�.
    Item::Grade::GradeInfo RebalanceUpgradeBonus(const Item::ItemInfo& newItemInfo,
        short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM], 
        const unsigned short usUpgradeFactor[][MAX_UPGRADECOL_NUM], 
        bool bSupressedResult[MAX_SUPRESSED_GRADE], 
        unsigned short wDropOption, unsigned char cUpgradeLevel);

    // ���׷��̵��, �� �ܰ谪�� ���� Additional Value ���̺��Դϴ�.

    // CON_ARMOUR, CON_BODY, DEX_BODY
    const unsigned short CON_Armour_CON_Body_DEX_Body[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::ARMOR,           270,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::EVADE,           270,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_HP,          270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::HP_REGEN,        270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAX_MP,          270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAGIC_RESIST,    270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // DEX_ARMOUR
    const unsigned short DEX_Armour[][MAX_UPGRADECOL_NUM] =
    {
        { Item::Attribute::ARMOR,           320,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::EVADE,           320,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_HP,          320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::HP_REGEN,        320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAX_MP,          320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAGIC_RESIST,    320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

	// edith 2008.01.14 �����׽Źߵ��� ������ ���׷��̵� ���Ѻκ�
    // ������ ���׷��̵� ����
    // CON_HELM, CON_HEAD, DEX_HEAD
    // DEX_HELM

    // DAGGER
    const unsigned short Dagger[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,      320,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_DAMAGE,      320,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::HIT_RATE,        320,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,        320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,	        320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // OPP_TALON
    const unsigned short OPP_Talon[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,		260,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_DAMAGE,		260,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::HIT_RATE,		260,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,		260,1,1,1,1,1,1,3,4,5,0 },        
        { Item::Attribute::BLOCK,			260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // SKILL_A_KNIFE
    const unsigned short Skill_A_Knife[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,		260,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_DAMAGE,		260,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::HIT_RATE,		260,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,		260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,			260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // ONEHANDED_SWORD, ONEHANDED_BLUNT, COM_SWORD, OPP_HAMMER
    const unsigned short ONEHANDED_Sword_Blunt_COM_Sword_OPP_Hammer[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,320,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,320,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,320,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,320,1,1,1,1,1,1,3,4,5,0 },        
        { Item::Attribute::BLOCK,320,1,1,1,1,1,1,3,4,5,0 },        
        { Item::Attribute::NONE, }
    };

    // ONEHANDED_AXE, OPP_SLUSHER, COM_BLUNT
    const unsigned short ONEHANDED_Axe_OPP_Slusher_COM_Blunt[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,320,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,320,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,320,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // OPP_AXE
    const unsigned short OPP_Axe[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,220,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,220,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,220,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,220,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,220,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // BOW, CROSSBOW
    const unsigned short Bow_CrossBow[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,260,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,260,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,260,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // SKILL_A_ATTACK
    const unsigned short Skill_A_Attack[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,310,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,310,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,310,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,310,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,310,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // SKILL_A_GUN
    const unsigned short Skill_A_Gun[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,270,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::MAX_DAMAGE,270,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::HIT_RATE,270,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,270,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // TWOHANDED_SWORD
    const unsigned short TWOHANDED_Sword[][MAX_UPGRADECOL_NUM] =
    {
        { Item::Attribute::MIN_DAMAGE,310,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::MAX_DAMAGE,310,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::HIT_RATE,310,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,310,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,310,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // TWOHANDED_AXE, TWOHANDED_BLUNT
    const unsigned short TWOHANDED_Axe_TWOHANDED_Blunt[][MAX_UPGRADECOL_NUM] =
    {
        { Item::Attribute::MIN_DAMAGE,260,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::MAX_DAMAGE,260,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::HIT_RATE,260,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::CRITICAL,260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,260,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // STAFF, OPP_SYTHE
    const unsigned short Staff_OPP_Sythe[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MAGIC_POWER,320,10,10,10,10,10,10,15,20,25,0 },
        { Item::Attribute::MAX_DAMAGE,320,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::HIT_RATE,320,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::MAX_MP,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MP_REGEN,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::CRITICAL,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,320,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

    // SHIELD, SKILL_A_GUARD
    const unsigned short Shield_Skill_A_Guard[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MAX_HP,245,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::HP_REGEN,245,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAX_MP,245,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MP_REGEN,245,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::BLOCK,245,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

	// Ʈ����
    const unsigned short Trophy[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::MIN_DAMAGE,10,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::MAX_DAMAGE,10,6,6,6,6,6,6,9,12,15,0 },
        { Item::Attribute::HIT_RATE,10,8,8,8,8,8,8,12,16,20,0 },
        { Item::Attribute::MAX_MP,10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MP_REGEN,10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::CRITICAL,10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

	// ��Ÿ��
    const unsigned short Santa_Cloth[][MAX_UPGRADECOL_NUM] = 
    {
        { Item::Attribute::ARMOR,10,4,4,4,4,4,4,6,8,10,0 },
        { Item::Attribute::EVADE, 10,2,2,2,2,2,2,3,4,5,0 },
        { Item::Attribute::MAX_HP, 10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::HP_REGEN, 10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::MAGIC_RESIST,10,1,1,1,1,1,1,3,4,5,0 },
        { Item::Attribute::NONE, }
    };

};

bool ConvertRebalanceItem(unsigned long dwUID, unsigned long dwCID, 
                          const Item::ItemData* lpItemData, 
                          const Item::ItemInfo& newItemInfo, 
                          const OldItem::ItemInfo& oldItemInfo, 
                          char* szData_Out, unsigned long& dwDataSize_InOut, 
                          Item::Grade::GradeInfo& convertedGrade)
{   
    convertedGrade = Item::Grade::GradeInfo();

    const OldItem::EquipmentInfo* lpEquipmentInfo = 
        reinterpret_cast<const OldItem::EquipmentInfo*>(lpItemData + 1);

    const unsigned char* lpSocketInfo = reinterpret_cast<const unsigned char*>(lpEquipmentInfo + 1);

    if(dwDataSize_InOut < sizeof(Item::ItemData) + sizeof(Item::EquipmentInfo) + 
        sizeof(Item::ItemAttribute) * Item::EquipmentInfo::MAX_ATTRIBUTE_NUM)
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / CurrentDatSize:%10u / Not enough datasize",
            dwUID, dwCID, dwDataSize_InOut);

        return false;
    }
    else if(0 != lpEquipmentInfo->m_cReserved)
    {
        ERRLOG5(g_InvalidItemLog, "UID:%10u / CID:%10u / ItemUID:0x%016I64X / PrototypeID:%5u / cSeasonRecord:%3d / Already Rebalanced Item",
            dwUID, dwCID, lpItemData->m_dwUID, lpItemData->m_usProtoTypeID, lpEquipmentInfo->m_cReserved);

        memcpy(szData_Out, lpItemData, lpItemData->m_cItemSize);
        dwDataSize_InOut = lpItemData->m_cItemSize;
    }
    else
    {
        char*           szOutDataPos    = szData_Out;
        unsigned char   cOldItemType    = oldItemInfo.m_DetailData.m_cItemType;
        unsigned short  wDropOption     = newItemInfo.m_DetailData.m_wDropOption;
        unsigned char   cMaxDurablility = lpEquipmentInfo->m_cDiffMaxDurability +
            oldItemInfo.m_DetailData.m_cDefaultDurabilityOrStack;

        Item::ItemData*         lpNewItemData      = reinterpret_cast<Item::ItemData*>(szOutDataPos);
        Item::EquipmentInfo*    lpNewEquipmentInfo = reinterpret_cast<Item::EquipmentInfo*>(lpNewItemData + 1);

        if(150 < cMaxDurablility)
        {
            DETLOG7(g_InvalidItemLog, "UID:%10u / CID:%10u / ItemUID:0x%016I64X / ItemSize:%3u / PrototypeID:%5u / "
                "ItemType:%20s / Durability:%3d / Item durability is invalid.", 
                dwUID, dwCID, lpItemData->m_dwUID, lpItemData->m_cItemSize, lpItemData->m_usProtoTypeID, 
                OldItem::CItemType::GetInstance().GetItemTypeName()[cOldItemType].GetTypeName(), cMaxDurablility);

            cMaxDurablility = 150;
        }
        
        *lpNewItemData = *lpItemData;

        // ������ ������ �̵�
        szOutDataPos += sizeof(Item::ItemData) + sizeof(Item::EquipmentInfo);

        // ��� ������ ä��
        lpNewEquipmentInfo->m_cUpgradeLevel     = 0; // ���׷��̵� ������ �ʱ�ȭ�Ѵ�

        lpNewEquipmentInfo->m_cDiffMaxSocket    = 0; // ���� ������ �ʱ�ȭ�Ѵ�
        lpNewEquipmentInfo->m_cSeasonRecord		= 0; // ������ ���� �ñ⵵ �ʱ�ȭ�Ѵ�

        lpNewEquipmentInfo->m_cSocketNum        = 0; // ���� ������ �ʱ�ȭ�Ѵ�
        lpNewEquipmentInfo->m_cAttributeNum     = 0; // �ϴ��� �Ӽ� ������ �ʱ�ȭ�Ѵ� (���߿� �ؿ��� �Ӽ� ��� ������ �ٽ� �־��ش�)

        // �ִ� ������ ����(150�� ���� �ʵ��� �Ѵ�)
        lpNewEquipmentInfo->m_cDiffMaxDurability = 
            cMaxDurablility - oldItemInfo.m_DetailData.m_cDefaultDurabilityOrStack;

		lpNewEquipmentInfo->m_cCoreLevel		= 0;
		lpNewEquipmentInfo->m_cReserved1		= 0; // ���� ������
        lpNewEquipmentInfo->m_cReserved2        = 0; // ���� ������

        // ��� ���Ѱ� ���
        unsigned long dwOldStatusLimitSum = std::accumulate(oldItemInfo.m_UseLimit.m_nStatusLimit, 
            oldItemInfo.m_UseLimit.m_nStatusLimit + OldItem::StatusLimit::MAX_STATUS_LIMIT, 0L);

        if(0 == dwOldStatusLimitSum) { ++dwOldStatusLimitSum; }

        const float fLimitValue = static_cast<float>(newItemInfo.m_UseLimit.m_wLimitValue) / dwOldStatusLimitSum;

        // ���� ������ �Ӽ��� ����� ������ �����ε�...
        // 1. ������ �Ӽ����� �迭�� ��´�.
        // 2. ������ Ÿ�Ժ���, ���������� �� �y���鼭 ������ �Ӽ����� �ɷ���, ���� ����� �� �����Ѵ�.
        
        short wOldAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM];		// ���� �Ӽ���
        short wInvalidAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM];	// �̻��� �Ӽ���        
        short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM];			// �� �Ӽ���

        std::fill_n(wOldAttr, unsigned short(OldItem::Attribute::MAX_ATTRIBUTE_NUM), 0);
        std::fill_n(wNewAttr, unsigned short(Item::Attribute::MAX_ATTRIBUTE_NUM), 0);

        const OldItem::ItemAttribute* lpAttribute = 
            reinterpret_cast<const OldItem::ItemAttribute*>(lpSocketInfo + lpEquipmentInfo->m_cSocketNum);

        // ������ �Ӽ��� �迭�� ���
        const OldItem::ItemAttribute* lpAttributePos = lpAttribute;
        const OldItem::ItemAttribute* lpAttributeEnd = lpAttribute + lpEquipmentInfo->m_cAttributeNum;

        for(; lpAttributePos != lpAttributeEnd; ++lpAttributePos)
        {
            if(0 != lpAttributePos->m_cType && 
                lpAttributePos->m_cType < OldItem::Attribute::MAX_ATTRIBUTE_NUM)
            {
                wOldAttr[lpAttributePos->m_cType] += lpAttributePos->m_usValue;
            }
        }

        std::copy(wOldAttr, wOldAttr + OldItem::Attribute::MAX_ATTRIBUTE_NUM, wInvalidAttr);
        
        bool bSupressedResult[MAX_SUPRESSED_GRADE];

        // ��� Ÿ�Կ� ���� �Ӽ����� �����Ѵ�.
        convertedGrade = RebalanceAttr(newItemInfo, lpEquipmentInfo->m_cDiffMaterialType, 
            wOldAttr, wInvalidAttr, wNewAttr, bSupressedResult);
        
        // wNewAttr�� 0�� �ƴ� �׸� ��� szOutDataPos�� �����Ѵ�.
        short* lpNewAttrPos =  wNewAttr;
        short* lpNewAttrEnd =  wNewAttr + Item::Attribute::MAX_ATTRIBUTE_NUM;

        int nAttrPos = 0;
        int nAddedAttrNum = 0; 

        for(;lpNewAttrPos != lpNewAttrEnd; 
            ++lpNewAttrPos, ++nAttrPos)
        {   
            if(*lpNewAttrPos < 0) 
            { 
                *lpNewAttrPos = 0; 
            }
            else if(0 < *lpNewAttrPos)
            {
                Item::ItemAttribute itemAttribute;
                itemAttribute.m_cType = nAttrPos;
                itemAttribute.m_usValue = *lpNewAttrPos;

                memcpy(szOutDataPos, &itemAttribute, sizeof(Item::ItemAttribute));
                szOutDataPos += sizeof(Item::ItemAttribute);

                ++nAddedAttrNum;
            }
        }

        // �Ӽ� ���� ����
        lpNewEquipmentInfo->m_cAttributeNum = nAddedAttrNum;

 
        if(bSupressedResult[SUPRESSED_DEFAULT])
        {
            // B�׷��̵带 �Ѵµ� C�׷��̵�� ���ѵǾ���ȴ�.

        }

        if(bSupressedResult[SUPRESSED_1To7])
        {
            // A�׷��̵带 �Ѵµ� B�׷��̵�� ���ѵǾ���ȴ�.

        }

        if(convertedGrade.m_eItemGrade <= Item::EquipType::A_GRADE)
        {
            // AA�׷��̵带 �Ѵµ� A�׷��̵�� ���ѵǾ���ȴ�.
            const int MAX_BUFFER = 1024;
            char szBuffer[MAX_BUFFER];

            int nTotalLength = _snprintf(szBuffer, MAX_BUFFER - 1, 
                "%s is %s.\tUID:%10u\tCID:%10u\tItemUID:0x%016I64X\tItemSize:%3u\tPrototypeID:%5u\t"
                "ItemName:%20s\t%d\tItemType:%20s\t",
                bSupressedResult[SUPRESSED_7ToMax] ? "SupressedToAGrade" : "ItemGrade",
                GetGradeString(convertedGrade.m_eItemGrade), dwUID, dwCID, 
                lpItemData->m_dwUID, lpItemData->m_cItemSize, lpItemData->m_usProtoTypeID,                                 
                newItemInfo.m_SpriteData.m_szName, newItemInfo.m_UseLimit.m_wLimitValue,
                newItemInfo.m_StringData.m_szTypeName);

            short* lpAttrPos = wNewAttr;
            short* lpAttrEnd = wNewAttr + Item::Attribute::MAX_ATTRIBUTE_NUM;

            Item::EquipType::Grade* lpGradePos = convertedGrade.m_aryAttributeGrade;
            Item::EquipType::Grade* lpGradeEnd = convertedGrade.m_aryAttributeGrade + 
                Item::Attribute::MAX_ATTRIBUTE_NUM;

            for(int nCount = 0; 
                lpAttrPos != lpAttrEnd; 
                ++lpAttrPos, ++lpGradePos, ++nCount)
            {
                if(*lpGradePos == convertedGrade.m_eItemGrade)
                {
                    int nLength = _snprintf(szBuffer + nTotalLength, MAX_BUFFER - nTotalLength -1, "%s:%3d(%s)\t", 
                        Item::Attribute::Attributes[nCount].GetTypeName(), (*lpAttrPos), GetGradeString(*lpGradePos));

                    if(0 < nLength)
                    {
                        nTotalLength += nLength;
                    }
                }
            }

            szBuffer[nTotalLength] = 0;
            INFLOG0(g_ItemGradeLog, szBuffer);
        }

		int nInvalidCount = 0;
        for(; nInvalidCount < OldItem::Attribute::MAX_ATTRIBUTE_NUM; ++nInvalidCount)
        {
            if(SHRT_MAX == wInvalidAttr[nInvalidCount])
            {
                break;
            }
        }

        if(nInvalidCount != OldItem::Attribute::MAX_ATTRIBUTE_NUM)
        {
            const int MAX_BUFFER = 1024;
            char szBuffer[MAX_BUFFER];

            int nTotalLength = _snprintf(szBuffer, MAX_BUFFER - 1, 
                "Item attribute is invalid.\tUID:%10u\tCID:%10u\tItemUID:0x%016I64X\t"
                "ItemSize:%3u\tPrototypeID:%5u\tItemType:%20s\tInvalidAttr:%20s(%3d)\t",
                dwUID, dwCID, lpItemData->m_dwUID, lpItemData->m_cItemSize, lpItemData->m_usProtoTypeID, 
                OldItem::CItemType::GetInstance().GetItemTypeName()[cOldItemType].GetTypeName(), 
                OldItem::Attribute::Attributes[nInvalidCount].GetTypeName(), 
                wOldAttr[nInvalidCount]);

            short* lpAttrPos = wOldAttr;
            short* lpAttrEnd = wOldAttr + OldItem::Attribute::MAX_ATTRIBUTE_NUM;

            for(; lpAttrPos != lpAttrEnd; ++lpAttrPos)
            {
                int nLength = _snprintf(szBuffer + nTotalLength, MAX_BUFFER - nTotalLength -1, "%3d\t", *lpAttrPos);
                if(0 < nLength)
                {
                    nTotalLength += nLength;
                }
            }

            INFLOG0(g_InvalidItemLog, szBuffer);
        }

        dwDataSize_InOut = static_cast<unsigned long>(szOutDataPos - szData_Out);
        lpNewItemData->m_cItemSize = static_cast<unsigned char>(dwDataSize_InOut);
    }

    return true;
}


Item::Grade::GradeInfo GetItemGrade(const Item::ItemInfo& newItemInfo, 
                                    short wNewAttribute[Item::Attribute::MAX_ATTRIBUTE_NUM],
                                    Item::EquipType::Grade eSuppressGrade, bool& bIsSuppressed)
{
    using namespace Item;

    bIsSuppressed = false;

    // �������� �׷��̵� ������ �ʱ�ȭ
    Grade::GradeInfo gradeInfo = Grade::GradeInfo();

    if (0 != newItemInfo.m_DetailData.m_wDropOption)
    {
        const CItemType::ArrayType eEquipType = 
            CItemType::GetEquipType(newItemInfo.m_DetailData.m_dwFlags);

        for(unsigned char cAttributeIndex = 0; 
            cAttributeIndex < Item::Attribute::MAX_ATTRIBUTE_NUM;
            ++cAttributeIndex)
        {
            short wMaxAttribute = static_cast<short>(Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, 
                eSuppressGrade, static_cast<Attribute::Type>(cAttributeIndex)) * 
                static_cast<float>(newItemInfo.m_DetailData.m_wDropOption));

            if(wMaxAttribute < wNewAttribute[cAttributeIndex])
            {
                wNewAttribute[cAttributeIndex] = wMaxAttribute;
                bIsSuppressed = true;
            }

            const float fGradeFactor = 
                (wNewAttribute[cAttributeIndex]) / static_cast<float>(newItemInfo.m_DetailData.m_wDropOption);

            // D �׷��̵带 �ʰ��ϸ� F �׷��̵尡 �ȴ�.
            for (unsigned char cGradeIndex = 0; cGradeIndex <= EquipType::D_GRADE; ++cGradeIndex)
            {
                const float fGradeValue = Grade::GetGradeValue(eEquipType, Grade::VALUE_GRADE, 
                    static_cast<EquipType::Grade>(cGradeIndex), static_cast<Attribute::Type>(cAttributeIndex));

                if (0 != fGradeValue && fGradeFactor > fGradeValue)
                {
                    gradeInfo.m_aryAttributeGrade[cAttributeIndex] = static_cast<EquipType::Grade>(cGradeIndex);

                    if (gradeInfo.m_eItemGrade > gradeInfo.m_aryAttributeGrade[cAttributeIndex])
                    {
                        gradeInfo.m_eItemGrade = gradeInfo.m_aryAttributeGrade[cAttributeIndex];
                        gradeInfo.m_cPlus = 0;
                    }
                    else
                    {
                        if (gradeInfo.m_eItemGrade == gradeInfo.m_aryAttributeGrade[cAttributeIndex])
                        {
                            ++gradeInfo.m_cPlus;
                        }
                    }
                    break;
                }
            }
        }
    }

    return gradeInfo;
}

void CItemGrade::WriteGradeNum()
{
    // ������ ��� ���
    for(int nCount = 0; nCount < Item::EquipType::MAX_GRADE; ++nCount)
    {
        Item::EquipType::Grade eGrade = static_cast<Item::EquipType::Grade>(nCount);

        INFLOG2(g_ItemGradeLog, "%5s Grade : %20I64u", 
            GetGradeString(eGrade), GetGradeItemNum(eGrade));
    }
}

const char* GetGradeString(Item::EquipType::Grade grade)
{
    switch(grade)
    {
		
		case Item::EquipType::S_GRADE:		return "S";
		case Item::EquipType::AAA_GRADE:    return "AAA";
		case Item::EquipType::AA_GRADE:     return "AA";
		case Item::EquipType::A_GRADE:      return "A";
		case Item::EquipType::B_GRADE:      return "B";
		case Item::EquipType::C_GRADE:      return "C";
		case Item::EquipType::D_GRADE:      return "D";
		case Item::EquipType::F_GRADE:      return "F";
		case Item::EquipType::X_GRADE:		return "X";
    }

    return "Unknown";
}


inline void CheckInvaildAttr(short wInvalidAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM], 
                             const short wOldAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM], 
                             unsigned short usIndex, float fValue)
{
    if(usIndex < OldItem::Attribute::MAX_ATTRIBUTE_NUM && fValue < wOldAttr[usIndex])
    {
        wInvalidAttr[usIndex] = SHRT_MAX;
    }
}



Item::Grade::GradeInfo RebalanceAttr(const Item::ItemInfo& newItemInfo, unsigned char cUpgradeLevel, 
                                     short wOldAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM],
                                     short wInvalidAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM],
                                     short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM], 
                                     bool bSupressedResult[MAX_SUPRESSED_GRADE])
{
    unsigned short          wDropOption = newItemInfo.m_DetailData.m_wDropOption;
    Item::Grade::GradeInfo  itemGrade;

    bSupressedResult[SUPRESSED_DEFAULT]    = false;
    bSupressedResult[SUPRESSED_1To7]       = false;
    bSupressedResult[SUPRESSED_7ToMax]     = false;

    unsigned char cMinUpgradeLevel = 0;
    unsigned char c7LevUpgradeValue = 0;
    unsigned char c8LevUpgradeValue = 0;
    unsigned char c9LevUpgradeValue = 0;

    if (2102 <= newItemInfo.m_usProtoTypeID && newItemInfo.m_usProtoTypeID <= 2149)
    {
        // Ʈ����
        memset(wInvalidAttr, 0, sizeof(unsigned short) * OldItem::Attribute::MAX_ATTRIBUTE_NUM);
        memset(wNewAttr, 0, sizeof(unsigned short) * Item::Attribute::MAX_ATTRIBUTE_NUM);

        itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
            UpgradeFactor::Trophy, bSupressedResult, 1, cUpgradeLevel);
    }
    else if(9911 == newItemInfo.m_usProtoTypeID ||
        9928 == newItemInfo.m_usProtoTypeID ||
        9929 == newItemInfo.m_usProtoTypeID)
    {
        // ��Ÿ��
        memset(wInvalidAttr, 0, sizeof(unsigned short) * OldItem::Attribute::MAX_ATTRIBUTE_NUM);
        memset(wNewAttr, 0, sizeof(unsigned short) * Item::Attribute::MAX_ATTRIBUTE_NUM);

        itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
            UpgradeFactor::Santa_Cloth, bSupressedResult, 1, cUpgradeLevel);
    }
    else 
    {
        switch(newItemInfo.m_DetailData.m_cItemType)
        {
        case Item::ItemType::CON_ARMOUR:
        case Item::ItemType::CON_BODY:
        case Item::ItemType::DEX_BODY:

            /*
            CON_ARMOUR, CON_BODY, DEX_BODY
            ����	    42.5	����	    243	    5.717647059		�� ���� * 5.72	
            ����	28.75	ȸ��	    60.75	2.113043478		�� ���� * 2.11	
            HP Max	    39.5	HP Max	    60.75	1.537974684		�� HP Max * 1.54	
            MP Max	    39.5	HP ȸ��	    121.5	3.075949367		�� HP ȸ�� * 3.08	
                                ��������	48.6	                DropOption * 0.12	
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE,            42.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE_REVISION,   28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_HP,             39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_MP,             39.5f);
                    
            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE] * 5.72f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE_REVISION] * 2.11f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_HP] * 1.54f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_MP] * 3.08f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wDropOption * 0.12f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::CON_Armour_CON_Body_DEX_Body, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::DEX_ARMOUR:

            /*
            DEX_ARMOUR
            ����	    42.5	����	    288	    6.776470588		�� ���� * 6.78	4	DropOption / 320 * 4 * ���ôܰ�	6	DropOption / 320 * 6 * (���ôܰ� - 6)
            ����	28.75	ȸ��	    72	    2.504347826		�� ���� * 2.50	2	DropOption / 320 * 2 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            HP Max	    39.5	HP Max	    72	    1.82278481		�� HP Max * 1.82	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            MP Max	    39.5	HP ȸ��	    144	    3.64556962		�� MP Max * 3.65	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
                                ��������	57.6	                DropOption * 0.12	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE,            42.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE_REVISION,   28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_HP,             39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_MP,             39.5f);        

            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE] * 6.78f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE_REVISION] * 2.50f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_HP] * 1.82f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_MP] * 3.65f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wDropOption * 0.12f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::DEX_Armour, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::CON_HELM:
        case Item::ItemType::CON_HEAD:
        case Item::ItemType::DEX_HEAD:

            /*
            CON_HELM, CON_HEAD, DEX_HEAD

            ���׷�	39.5	����	    234	    5.924050633		�� ���׷� * 5.92
            ���׷�	39.5	ȸ��	    58.5	1.481012658		�� ���׷� * 1.48
            HP ȸ��	39.5	HP Max	    58.5	1.481012658		�� HP ȸ�� * 1.48
            MP ȸ��	39.5	HP ȸ��	    117	    2.962025316		�� MP ȸ�� * 2.96
            ���׷�	22.75	��������	46.8	2.057142857		�� ���׷� * 2.06
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  22.75f);

            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 5.92f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 1.48f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.48f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 2.96f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 2.06f);

            // �⺻�� ����� ��������, ������ �׷��̵� ����� �ؼ� B��� �̻��� C������� �����.
            itemGrade = GetItemGrade(newItemInfo, wNewAttr, Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);
            break;

        case Item::ItemType::DEX_HELM:

            /*
            DEX_HELM
            ���׷�	39.5	����	    277.2	7.017721519		�� ���׷� * 7.02
            ���׷�	39.5	ȸ��	    69.3	1.75443038		�� ���׷� * 1.75
            HP ȸ��	39.5	HP Max	    69.3	1.75443038		�� HP ȸ�� * 1.75
            MP ȸ��	39.5	HP ȸ��	    138.6	3.508860759		�� MP ȸ�� * 3.51
            ���׷�	22.75	��������	55.44	2.436923077		�� ���׷� * 2.44
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  22.75f);        

            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 7.02f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 1.75f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.75f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 3.51f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 2.44f);

            // �⺻�� ����� ��������, ������ �׷��̵� ����� �ؼ� B��� �̻��� C������� �����.
            itemGrade = GetItemGrade(newItemInfo, wNewAttr, Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);
            break;

        case Item::ItemType::DAGGER:

            /*
            DAGGER
            mDmg	    13.375	mDmg	    57.6	4.306542056		�� mDmg * 4.31	    2	DropOption / 320 * 2 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    72	    2.10989011		�� MDmg * 2.11	    2	DropOption / 320 * 2 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    288	    10.0173913		�� ���ݺ��� * 10.02	8	DropOption / 320 * 8 * ���ôܰ�	12	DropOption / 320 * 12 * (���ôܰ� - 6)
                                MPMax	    72	                    DropOption * 0.15	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
                                MPȸ��	    144	                    DropOption * 0.3	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	57.6	6.630215827		�� ũ��Ƽ�� * 6.63	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            ���	    8.6875	���	    57.6	6.630215827		�� ��� * 6.63	    0	0	0	0
            */

            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 4.31f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 2.11f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 10.02f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 6.63f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 6.63f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Dagger, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::OPP_TALON:

            /*
            OPP_TALON
            mDmg	    13.375	mDmg	    46.8	3.499065421		�� mDmg * 3.50	    2	DropOption / 260 * 2 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    58.5	1.714285714		�� mDmg * 1.71	    2	DropOption / 260 * 2 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    234	    8.139130435		�� ���ݺ��� * 8.14	8	DropOption / 260 * 8 * ���ôܰ�	12	DropOption / 260 * 12 * (���ôܰ� - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
                                MPȸ��	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	46.8	5.38705036		�� ũ��Ƽ�� * 5.39	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ���	    8.6875	���	    46.8	5.38705036		�� ��� * 5.39	    0	0	0	0
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 3.50f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 1.71f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 8.14f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 5.39f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 5.39f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::OPP_Talon, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::SKILL_A_KNIFE:

            /*
            SKILL_A_KNIFE
            mDmg	    13.375	mDmg	    46.8	3.499065421		�� mDmg * 3.50	    2	DropOption / 260 * 2 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    58.5	1.714285714		�� MDmg * 1.71	    2	DropOption / 260 * 2 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ���ݺ���	10.375	����	    234	    22.55421687		�� ���ݺ��� * 22.55	8	DropOption / 260 * 8 * ���ôܰ�	12	DropOption / 260 * 12 * (���ôܰ� - 6)
            HP ȸ��	    39.5	MPMax	    58.5	1.481012658		�� HP ȸ�� * 1.48	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            MP ȸ��	    39.5	MPȸ��	    117	    2.962025316		�� MP ȸ�� * 2.96	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ����	10.375	ũ��Ƽ��	46.8	4.510843373		�� ���� * 4.51	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ���׷�	    8.6875	���	    46.8	5.38705036		�� ���׷� * 5.39	0	0	0	0
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,        13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,        34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 3.50f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 1.71f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 22.55f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.48f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 2.96f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE_REVISION] * 4.51f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 5.39f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Skill_A_Knife, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::ONEHANDED_SWORD:
        case Item::ItemType::ONEHANDED_BLUNT:
        case Item::ItemType::COM_SWORD:
        case Item::ItemType::OPP_HAMMER:

            /*
            ONEHANDED_SWORD, ONEHANDED_BLUNT, COM_SWORD, OPP_HAMMER

            mDmg	    13.375	mDmg	    86.4	6.459813084		�� mDmg * 6.46	    4	DropOption / 320 * 4 * ���ôܰ�	6	DropOption / 320 * 6 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    115.2	3.375824176		�� MDmg * 3.38	    4	DropOption / 320 * 4 * ���ôܰ�	6	DropOption / 320 * 6 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    288	    10.0173913		�� ���ݺ��� * 10.02	8	DropOption / 320 * 8 * ���ôܰ�	12	DropOption / 320 * 12 * (���ôܰ� - 6)
                                MPMax	    72	                    DropOption * 0.15	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
                                MPȸ��	    144	                    DropOption * 0.3	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	57.6	6.630215827		�� ũ��Ƽ�� * 6.63	1	DropOption / 320 * 1 * ���ôܰ�	3	DropOption / 320 * 3 * (���ôܰ� - 6)
            ���	    8.6875	���	    57.6	6.630215827		�� ��� * 6.63	    0	0	0	0
            
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 6.46f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 3.38f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 10.02f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 6.63f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 6.63f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::ONEHANDED_Sword_Blunt_COM_Sword_OPP_Hammer, 
                bSupressedResult, wDropOption, cUpgradeLevel);

            break;

        case Item::ItemType::ONEHANDED_AXE:
        case Item::ItemType::COM_BLUNT:
        case Item::ItemType::OPP_SLUSHER:

            /*
            ONEHANDED_AXE, COM_BLUNT, OPP_SLUSHER

            mDmg	    13.375	mDmg	    70.2	5.248598131		�� mDmg * 5.25	    4	
            MDmg	    34.125	MDmg	    93.6	2.742857143		�� mDmg * 2.74	    4	
            ���ݺ���	28.75	����	    234	    8.139130435		�� ���ݺ��� * 8.14	8	
                                MPMax	    58.5	                DropOption * 0.15	1	
                                MPȸ��	    117	                    DropOption * 0.3	1	
            ũ��Ƽ��	8.6875	ũ��Ƽ��	46.8	5.38705036		�� ũ��Ƽ�� * 5.39	1	
            ���	    8.6875	���	    46.8	5.38705036		�� ��� * 5.39	    0	

            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 5.25f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 2.74f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 8.14f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 5.39f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 5.39f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::ONEHANDED_Sword_Blunt_COM_Sword_OPP_Hammer, 
                bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::OPP_AXE:

            /*
            mDmg	    13.375	mDmg	    59.4	4.441121495		�� mDmg * 4.44
            MDmg	    34.125	MDmg	    79.2	2.320879121		�� MDmg * 2.32
            ���ݺ���	28.75	����	    198	    6.886956522		�� ���ݺ��� * 6.89
            MPMax	    49.5	                DropOption * 0.15
            MPȸ��	    99	                    DropOption * 0.3
            ũ��Ƽ��	8.6875	ũ��Ƽ��	39.6	4.558273381		�� ũ��Ƽ�� * 4.56
            ���	    8.6875	���	    39.6	4.558273381		�� ��� * 4.56
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 4.44f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 2.32f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 6.89f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 4.56f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 4.56f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::OPP_Axe, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::BOW:
        case Item::ItemType::CROSSBOW:

            /*
            BOW, CROSSBOW

            mDmg	    13.375	mDmg	    70.2	5.248598131		�� mDmg * 5.25	    4	DropOption / 260 * 4 * MIN(���ôܰ�, 6)	6	DropOption / 260 * 6 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    93.6	2.742857143		�� mDmg * 2.74	    4	DropOption / 260 * 4 * MIN(���ôܰ�, 6)	6	DropOption / 260 * 6 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    234	    8.139130435		�� ���ݺ��� * 8.14	8	DropOption / 260 * 8 * MIN(���ôܰ�, 6)	12	DropOption / 260 * 12 * (���ôܰ� - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * MIN(���ôܰ�, 6)	3	DropOption / 260 * 3 * (���ôܰ� - 6)
                                MPȸ��	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * MIN(���ôܰ�, 6)	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	46.8	5.38705036		�� ũ��Ƽ�� * 5.39	1	DropOption / 260 * 1 * MIN(���ôܰ�, 6)	3	DropOption / 260 * 3 * (���ôܰ� - 6)
                                ���	    46.8	                DropOption * 0.12	0	0	0	0
            */

            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 5.25f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 2.74f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 8.14f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 5.39f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wDropOption * 0.12f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Bow_CrossBow, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::SKILL_A_ATTACK:

            /*
            SKILL_A_ATTACK
            mDmg	    13.375	mDmg	    83.7	6.257943925		�� mDmg * 6.26	    4	DropOption / 310 * 4 * ���ôܰ�	6	DropOption / 310 * 6 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    111.6	3.27032967		�� MDmg * 3.27	    4	DropOption / 310 * 4 * ���ôܰ�	6	DropOption / 310 * 6 * (���ôܰ� - 6)
            ���ݺ���	10.375	����	    279	    26.89156627		�� ���ݺ��� * 26.89	8	DropOption / 310 * 8 * ���ôܰ�	12	DropOption / 310 * 12 * (���ôܰ� - 6)
            HP ȸ��	    39.5	MPMax	    69.75	1.765822785		�� HP ȸ�� * 1.77	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
            MP ȸ��	    39.5	MPȸ��	    139.5	3.53164557		�� MP ȸ�� * 3.53	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
            ����	10.375	ũ��Ƽ��	55.8	5.378313253		�� ���� * 5.38	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
            ���׷�	    8.6875	���	    55.8	6.423021583		�� ���׷� * 6.42	0	0	0	0
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,        13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,        34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 6.26f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 3.27f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 26.89f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.77f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 3.53f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE_REVISION] * 5.38f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 6.42f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Skill_A_Attack, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::SKILL_A_GUN:

            /*
            SKILL_A_GUN
            mDmg	    13.375	mDmg	    72.9	5.45046729		�� mDmg * 5.45	    4	DropOption / 270 * 4 * ���ôܰ�	6	DropOption / 270 * 6 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    97.2	2.848351648		�� MDmg * 2.85	    4	DropOption / 270 * 4 * ���ôܰ�	6	DropOption / 270 * 6 * (���ôܰ� - 6)
            ���ݺ���	10.375	����	    243	    23.42168675		�� ���ݺ��� * 23.42	8	DropOption / 270 * 8 * ���ôܰ�	12	DropOption / 270 * 12 * (���ôܰ� - 6)
            HP ȸ��	    39.5	MPMax	    60.75	1.537974684		�� HP ȸ�� * 1.54	1	DropOption / 270 * 1 * ���ôܰ�	3	DropOption / 270 * 3 * (���ôܰ� - 6)
            MP ȸ��	    39.5	MPȸ��	    121.5	3.075949367		�� MP ȸ�� * 3.08	1	DropOption / 270 * 1 * ���ôܰ�	3	DropOption / 270 * 3 * (���ôܰ� - 6)
            ����	10.375	ũ��Ƽ��	48.6	4.684337349		�� ���� * 4.68	1	DropOption / 270 * 1 * ���ôܰ�	3	DropOption / 270 * 3 * (���ôܰ� - 6)
            ���׷�	    8.6875	���	    48.6	5.594244604		�� ���׷� * 5.59	0	0	0	0
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,        13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,        34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::DEFENCE_REVISION,  10.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 5.45f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 2.85f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 23.42f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.54f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 3.08f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::DEFENCE_REVISION] * 4.68f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 5.59f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Skill_A_Gun, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::TWOHANDED_SWORD:

            /*
            TWOHANDED_SWORD
            mDmg	    13.375	mDmg	    125.55	9.386915888		�� mDmg * 9.39	    6	DropOption / 310 * 6 * ���ôܰ�	9	DropOption / 310 * 9 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    153.5	4.498168498		�� MDmg * 4.50	    6	DropOption / 310 * 6 * ���ôܰ�	9	DropOption / 310 * 9 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    279	    9.704347826		�� ���ݺ��� * 9.70	8	DropOption / 310 * 8 * ���ôܰ�	12	DropOption / 310 * 12 * (���ôܰ� - 6)
                                MPMax	    69.75	                DropOption * 0.15	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
                                MPȸ��	    139.5	                DropOption * 0.3	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	55.8	6.423021583		�� ũ��Ƽ�� * 6.42	1	DropOption / 310 * 1 * ���ôܰ�	3	DropOption / 310 * 3 * (���ôܰ� - 6)
            ���	    8.6875	���	    55.8	6.423021583		�� ��� * 6.42	    0	0	0	0
            */

            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 9.39f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 4.50f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 9.70f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 6.42f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 6.42f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::TWOHANDED_Sword, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::TWOHANDED_AXE:
        case Item::ItemType::TWOHANDED_BLUNT:

            /*
            TWOHANDED_AXE, TWOHANDED_BLUNT
            mDmg	    13.375	mDmg	    105.3	7.872897196		�� mDmg * 7.87	    6	DropOption / 260 * 6 * ���ôܰ�	9	DropOption / 260 * 9 * (���ôܰ� - 6)
            MDmg	    34.125	MDmg	    128.7	3.771428571		�� MDmg * 3.77	    6	DropOption / 260 * 6 * ���ôܰ�	9	DropOption / 260 * 9 * (���ôܰ� - 6)
            ���ݺ���	28.75	����	    234	    8.139130435		�� ���ݺ��� * 8.14	8	DropOption / 260 * 8 * ���ôܰ�	12	DropOption / 260 * 12 * (���ôܰ� - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
                                MPȸ��	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ũ��Ƽ��	8.6875	ũ��Ƽ��	46.8	5.38705036		�� ũ��Ƽ�� * 5.39	1	DropOption / 260 * 1 * ���ôܰ�	3	DropOption / 260 * 3 * (���ôܰ� - 6)
            ���	    8.6875	���	    46.8	5.38705036		�� ��� * 5.39	    0	0	0	0
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wNewAttr[Item::Attribute::MIN_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MIN_DAMAGE] * 7.87f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 3.77f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 8.14f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 5.39f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 5.39f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::TWOHANDED_Axe_TWOHANDED_Blunt, bSupressedResult, wDropOption, cUpgradeLevel);
            break;

        case Item::ItemType::STAFF:
        case Item::ItemType::OPP_SYTHE:

            /*
            STAFF, OPP_SYTHE

            "STAFF, OPP_SYTHE
            mDmg	    13.375	������	    288	    DropOption * 0.75
            MDmg	    34.125	MDmg	    158.4	�� MDmg * 4.64
            ���ݺ���	28.75	����	    288	    �� ���ݺ��� * 10.02
                                MPMax	    144	    DropOption * 0.15
                                MPȸ��	    288	    DropOption * 0.3
            ũ��Ƽ��	8.6875	ũ��Ƽ��	57.6	�� ũ��Ƽ�� * 6.63
            ���	    8.6875	���	    57.6	�� ��� * 6.63

            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MIN_DAMAGE,            13.375f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAX_DAMAGE,            34.125f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::OFFENCE_REVISION,      28.75f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::CRITICAL_PERCENTAGE,   8.6875f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   8.6875f);

            wInvalidAttr[OldItem::Attribute::MAX_MP] = 0;
            wInvalidAttr[OldItem::Attribute::MP_REGEN_AMOUNT] = 0;

            wNewAttr[Item::Attribute::MAGIC_POWER]  = static_cast<short>(wDropOption * 0.75f);
            wNewAttr[Item::Attribute::MAX_DAMAGE]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAX_DAMAGE] * 4.64f);
            wNewAttr[Item::Attribute::HIT_RATE]     = static_cast<short>(wOldAttr[OldItem::Attribute::OFFENCE_REVISION] * 10.02f);
            wNewAttr[Item::Attribute::MAX_MP]       = static_cast<short>(wDropOption * 0.15f);
            wNewAttr[Item::Attribute::MP_REGEN]     = static_cast<short>(wDropOption * 0.3f);
            wNewAttr[Item::Attribute::CRITICAL]     = static_cast<short>(wOldAttr[OldItem::Attribute::CRITICAL_PERCENTAGE] * 6.63f);
            wNewAttr[Item::Attribute::BLOCK]        = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 6.63f);

            itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                UpgradeFactor::Staff_OPP_Sythe, bSupressedResult, wDropOption, cUpgradeLevel);
			if (0 == cUpgradeLevel)
			{
				itemGrade = GetItemGrade(newItemInfo, wNewAttr, Item::EquipType::C_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);
			}
            break;

        case Item::ItemType::SHIELD:
        case Item::ItemType::SKILL_A_GUARD:

            /* SHIELD, SKILL_A_GUARD
            ���	11.375	HPMax	27.563	2.423120879		�� ��� * 2.42
            ���	11.375	HPȸ��	55.125	4.846153846		�� ��� * 4.85
            ���	11.375	MPMax	27.563	2.423120879		�� ��� * 2.42
            ���	11.375	MPȸ��	55.125	4.846153846		�� ��� * 4.85
            ���	11.375	���	44.1	3.876923077		�� ��� * 3.88
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::BLOCKING_PERCENTAGE,   11.375f);

            if(Item::ItemType::SKILL_A_GUARD == newItemInfo.m_DetailData.m_cItemType)
            {
                wInvalidAttr[Item::Attribute::MIN_DAMAGE] = 0;
                wInvalidAttr[Item::Attribute::MAX_DAMAGE] = 0;
            }

            wNewAttr[Item::Attribute::MAX_HP]   = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 2.42f);
            wNewAttr[Item::Attribute::HP_REGEN] = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 4.85f);
            wNewAttr[Item::Attribute::MAX_MP]   = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 2.42f);
            wNewAttr[Item::Attribute::MP_REGEN] = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 4.85f);
            wNewAttr[Item::Attribute::BLOCK]    = static_cast<short>(wOldAttr[OldItem::Attribute::BLOCKING_PERCENTAGE] * 3.88f);

            if(1701 != newItemInfo.m_usProtoTypeID)
            {
                itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
                    UpgradeFactor::Shield_Skill_A_Guard, bSupressedResult, wDropOption, cUpgradeLevel);
            }
            else
            {   
                // ��Ŭ���� ���׷��̵� �ܰ踦 �ٸ��� ó���Ѵ�.
                itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
                    Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);

                unsigned char cMinUpgradeLevel = min(cUpgradeLevel, 6);
                unsigned char c7To9UpgradeLevel = (6 < cUpgradeLevel) ? (cUpgradeLevel - 6) : 0;

                if(0 < cMinUpgradeLevel)
                {
                    wNewAttr[Item::Attribute::MAX_HP]   += static_cast<short>(wDropOption / 245 * 1 * cMinUpgradeLevel);
                    wNewAttr[Item::Attribute::HP_REGEN] += static_cast<short>(wDropOption / 245 * 1 * cMinUpgradeLevel);
                    wNewAttr[Item::Attribute::MAX_MP]   += static_cast<short>(wDropOption / 245 * 1 * cMinUpgradeLevel);
                    wNewAttr[Item::Attribute::MP_REGEN] += static_cast<short>(wDropOption / 245 * 1 * cMinUpgradeLevel);
                    wNewAttr[Item::Attribute::BLOCK]    += static_cast<short>(wDropOption / 245 * 1 * cMinUpgradeLevel);
                
                    itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
                        Item::EquipType::A_GRADE, bSupressedResult[SUPRESSED_1To7]);
                }

                if(0 < c7To9UpgradeLevel)
                {
                    wNewAttr[Item::Attribute::MAX_HP]   += c7To9UpgradeLevel;
                    wNewAttr[Item::Attribute::HP_REGEN] += c7To9UpgradeLevel;
                    wNewAttr[Item::Attribute::MAX_MP]   += c7To9UpgradeLevel;
                    wNewAttr[Item::Attribute::MP_REGEN] += c7To9UpgradeLevel;
                    wNewAttr[Item::Attribute::BLOCK]    += c7To9UpgradeLevel;

                    itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
                        Item::EquipType::AA_GRADE, bSupressedResult[SUPRESSED_7ToMax]);
                }
            }
            break;
        }
    }

    return itemGrade;
}


Item::Grade::GradeInfo UpgradeFactor::RebalanceUpgradeBonus(const Item::ItemInfo& newItemInfo,
                                                            short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM], 
                                                            const unsigned short usUpgradeFactor[][UpgradeFactor::MAX_UPGRADECOL_NUM], 
                                                            bool bSupressedResult[MAX_SUPRESSED_GRADE],        
                                                            unsigned short wDropOption, unsigned char cUpgradeLevel)
{
    Item::Grade::GradeInfo itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
        Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);

    // �ϴ� 6�ܰ������ �ѹ� �� ������.
    const int n1stLevelCount = 6;
    float fDropOption = static_cast<float>(wDropOption);

    for(int nCount = 0; Item::Attribute::NONE != usUpgradeFactor[nCount][0]; ++nCount)
    {
        // usUpgradeFactor[][0] : ������ �Ӽ�
        // usUpgradeFactor[][1] : Div��

        float fDiv = usUpgradeFactor[nCount][1];

        int nPos = 2;
        int nEnd = min(cUpgradeLevel, n1stLevelCount) + nPos;
        int nUpgradeBonus = 0;

        for(; nPos < nEnd; ++nPos)
        {
            nUpgradeBonus += usUpgradeFactor[nCount][nPos];
        }

        wNewAttr[usUpgradeFactor[nCount][0]] += 
            static_cast<short>(fDropOption / fDiv * nUpgradeBonus);
    }

    itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
        Item::EquipType::A_GRADE, bSupressedResult[SUPRESSED_1To7]);

    // 7~9�ܰ踦 ������ A������� �����.
    if(n1stLevelCount < cUpgradeLevel)
    {
        for(int nCount = 0; Item::Attribute::NONE != usUpgradeFactor[nCount][0]; ++nCount)
        {
            // usUpgradeFactor[][0] : ������ �Ӽ�
            // usUpgradeFactor[][1] : Div��

            float fDiv = usUpgradeFactor[nCount][1];

            int nPos = n1stLevelCount + 2;
            int nEnd = cUpgradeLevel + 2;
            int nUpgradeBonus = 0;

            for(; nPos < nEnd; ++nPos)
            {
                nUpgradeBonus += usUpgradeFactor[nCount][nPos];
            }

            wNewAttr[usUpgradeFactor[nCount][0]] += 
                static_cast<short>(fDropOption / fDiv * nUpgradeBonus);
        }

        itemGrade = GetItemGrade(newItemInfo, wNewAttr, 
            Item::EquipType::AA_GRADE, bSupressedResult[SUPRESSED_7ToMax]);
    }

    return itemGrade;
}


bool ConvertContainer(unsigned long dwUID, unsigned long dwCID, 
                      CItemChecker* lpItemChecker, SkillBookInfo* lpSkillBookInfo,
                      const char* szOriginalData, unsigned long dwOriginalSize,
                      char* szChangedData, unsigned long& dwChangedSize_InOut)
{
    const char* szPos = szOriginalData;
    const char* szEnd = szOriginalData + dwOriginalSize;

    char* szResultPos = szChangedData;
    char* szResultEnd = szChangedData + dwChangedSize_InOut;

    Item::Grade::GradeInfo convertedGrade;

    for(; szPos < szEnd && szResultPos < szResultEnd; )
    {
        const Item::ItemData*       lpItemData = reinterpret_cast<const Item::ItemData*>(szPos);
        Item::ItemData*             lpResultItemData = reinterpret_cast<Item::ItemData*>(szResultPos);

        const Item::ItemInfo*       lpNewItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(lpItemData->m_usProtoTypeID);
        const OldItem::ItemInfo*    lpOldItemInfo = OldItem::CItemMgr::GetInstance().GetItemInfo(lpItemData->m_usProtoTypeID);

        if(0 == lpNewItemInfo || 0 == lpOldItemInfo)
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / Serial:0x%016I64X / ProtoType:%6d / Script has not this item", 
                dwUID, dwCID, lpItemData->m_dwUID, lpItemData->m_usProtoTypeID);
        }
        else if(0 != (lpNewItemInfo->m_DetailData.m_dwFlags & Item::DetailData::EQUIP) && 
            sizeof(Item::ItemData) + sizeof(Item::EquipmentInfo) <= lpItemData->m_cItemSize)
        {
            // ����� ��� ������ �������� �Ѵ�.
            unsigned long dwDataSize_InOut = 
                static_cast<unsigned long>(szResultEnd - szResultPos);

            if(ConvertRebalanceItem(dwUID, dwCID, 
                lpItemData, *lpNewItemInfo, *lpOldItemInfo, 
                szResultPos, dwDataSize_InOut, convertedGrade))
            {
                // ������ ��ġ�� �����Ѵ�.
                if(0 != lpItemChecker)
                {
                    lpItemChecker->Set(reinterpret_cast<Item::ItemData*>(szResultPos)->m_ItemPos, 
                        lpNewItemInfo->m_DetailData.m_cXSize, lpNewItemInfo->m_DetailData.m_cYSize);
                }

                // ���� ��ġ ����
                szResultPos += dwDataSize_InOut;
            }
            else
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / Serial:0x%016I64X / ProtoType:%6d / Item recreation failed", 
                    dwUID, dwCID, lpItemData->m_dwUID, lpItemData->m_usProtoTypeID);
            }
        }
        else if(Item::ItemType::SKILL_BOOK == lpNewItemInfo->m_DetailData.m_cItemType &&
            0 != lpSkillBookInfo && 
            lpNewItemInfo->m_UseItemInfo.m_usSkill_LockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT)
        {
            // ������ Ÿ���� ��ų���̰�, ��ų�� �迭�� �޾Ҵٸ�, 
            // ��ȯ�� �迭�� ī�����ϰ�, �������� �������� �ʴ´�.
            lpSkillBookInfo->m_dwBookNum[lpNewItemInfo->m_UseItemInfo.m_usSkill_LockCount] += 
                lpItemData->m_cNumOrDurability;
        }
        else
        {
            // ������ ��ġ�� �����Ѵ�.
            if(0 != lpItemChecker)
            {
                lpItemChecker->Set(lpItemData->m_ItemPos, 
                    lpNewItemInfo->m_DetailData.m_cXSize, lpNewItemInfo->m_DetailData.m_cYSize);
            }

            // �׳� �������� �����Ѵ�.
            memcpy(szResultPos, lpItemData, lpItemData->m_cItemSize);
            szResultPos += lpItemData->m_cItemSize;
        }

        szPos += lpItemData->m_cItemSize;
    }        

    dwChangedSize_InOut = static_cast<unsigned long>(szResultPos - szChangedData);
    return true;
}

bool AddSkillBook(unsigned long dwUID, unsigned long dwCID, const char* szPosition, 
                  Item::CItemSerialMgr& itemSerialMgr, unsigned long& dwGold_InOut, 
                  char* szData, unsigned long& dwDataSize_InOut, unsigned long dwMaxDataSize,
                  CItemChecker& itemChecker, SkillBookInfo& skillBookInfo)
{
    // 1/2 ���� �����͸� ������ �ٲ㼭 ���� �ش�.
    const unsigned long dw1stSkillBookGold = 1000;
    const unsigned long dw2ndSkillBookGold = 10000;

    const unsigned short w3rdSkillBookPrototypeID = 3401;
    const unsigned short w4thSkillBookPrototypeID = 3402;

    const unsigned char  cBookXSize = 1;
    const unsigned char  cBookYSize = 1;
       
    unsigned long dwAddGold = 
        dw1stSkillBookGold * skillBookInfo.m_dwBookNum[0] + 
        dw2ndSkillBookGold * skillBookInfo.m_dwBookNum[1];

    if(ULONG_MAX - dwAddGold < dwGold_InOut)
    {
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / CurrentGold:%u / AddGold:%u / "
            "1,2�ܰ� ��ų�� ������ ���� �� �� �����ϴ�. ���� %u�� �����մϴ�",
            dwUID, dwCID, dwGold_InOut, dwAddGold, ULONG_MAX);

        dwGold_InOut = ULONG_MAX;
    }
    else
    {
        dwGold_InOut += dwAddGold;
    }

    skillBookInfo.m_dwBookNum[0] = 0;
    skillBookInfo.m_dwBookNum[1] = 0;

    // 3/4 ���� �����͸� ��ų������ �ٲ㼭 ���� �ش�. 
    // �̹� ItemChecker�� �������� ���� ���õǾ� �ִ� ��Ȳ�̴�.
    Item::ItemPos itemPos;

    // 3�ܰ� ������ ����
    char*       szDataPos = szData + dwDataSize_InOut;
    const char* szDataEnd = szData + dwMaxDataSize;

    while (0 < skillBookInfo.m_dwBookNum[2] && 
        szDataPos + sizeof(Item::ItemData) < szDataEnd &&
        itemChecker.FindEmptyPos(itemPos, cBookXSize, cBookYSize))
    {
        Item::ItemData skillBook3rd;
        memset(&skillBook3rd, 0, sizeof(Item::ItemData));

        unsigned char cAmount = 0;

        if(UCHAR_MAX < skillBookInfo.m_dwBookNum[2])
        {
            cAmount = UCHAR_MAX;
            skillBookInfo.m_dwBookNum[2] -= UCHAR_MAX;
        }
        else
        {
            cAmount = static_cast<unsigned char>(skillBookInfo.m_dwBookNum[2]);
            skillBookInfo.m_dwBookNum[2] = 0;
        }

        if(0 < cAmount)
        {
            skillBook3rd.m_dwUID                = itemSerialMgr.GetNewItemSerial();
            skillBook3rd.m_usProtoTypeID        = w3rdSkillBookPrototypeID;
            skillBook3rd.m_ItemPos              = itemPos;
            skillBook3rd.m_cNumOrDurability     = cAmount;
            skillBook3rd.m_cItemSize            = sizeof(Item::ItemData);

            memcpy(szDataPos, &skillBook3rd, sizeof(Item::ItemData));
            szDataPos += sizeof(Item::ItemData);

            itemChecker.Set(itemPos, cBookXSize, cBookYSize);

            INFLOG8(g_Log, "%s / UID:%10u / CID:%10u / ItemUID:0x%016I64X / PrototypeID:%6d / "
                "ItemPos:(%d:0x%03X) / Num:%3d / 3�ܰ� ��ų�� ���� ����",
                szPosition, dwUID, dwCID, skillBook3rd.m_dwUID, skillBook3rd.m_usProtoTypeID, 
                skillBook3rd.m_ItemPos.m_cPos, skillBook3rd.m_ItemPos.m_cIndex, 
                skillBook3rd.m_cNumOrDurability);
        }
    }

    // 4�ܰ� ��ų�� ����
    while (0 < skillBookInfo.m_dwBookNum[3] && 
        szDataPos + sizeof(Item::ItemData) < szDataEnd &&
        itemChecker.FindEmptyPos(itemPos, 1, 1))
    {
        Item::ItemData skillBook4th;
        memset(&skillBook4th, 0, sizeof(Item::ItemData));

        unsigned char cAmount = 0;

        if(UCHAR_MAX < skillBookInfo.m_dwBookNum[3])
        {
            cAmount = UCHAR_MAX;
            skillBookInfo.m_dwBookNum[3] -= UCHAR_MAX;
        }
        else
        {
            cAmount = static_cast<unsigned char>(skillBookInfo.m_dwBookNum[3]);
            skillBookInfo.m_dwBookNum[3] = 0;
        }

        if (0 < cAmount)
        {
            skillBook4th.m_dwUID                = itemSerialMgr.GetNewItemSerial();
            skillBook4th.m_usProtoTypeID        = w4thSkillBookPrototypeID;
            skillBook4th.m_ItemPos              = itemPos;
            skillBook4th.m_cNumOrDurability     = cAmount;
            skillBook4th.m_cItemSize            = sizeof(Item::ItemData);

            memcpy(szDataPos, &skillBook4th, sizeof(Item::ItemData));
            szDataPos += sizeof(Item::ItemData);

            itemChecker.Set(itemPos, cBookXSize, cBookYSize);

            INFLOG8(g_Log, "%s / UID:%10u / CID:%10u / ItemUID:0x%016I64X / PrototypeID:%6d / "
                "ItemPos:(%d:0x%03X) / Num:%3d / 4�ܰ� ��ų�� ���� ����",
                szPosition, dwUID, dwCID, skillBook4th.m_dwUID, skillBook4th.m_usProtoTypeID, 
                skillBook4th.m_ItemPos.m_cPos, skillBook4th.m_ItemPos.m_cIndex, 
                skillBook4th.m_cNumOrDurability);
        }
    }

    dwDataSize_InOut = static_cast<unsigned long>(szDataPos - szData);
    return true;
}


}