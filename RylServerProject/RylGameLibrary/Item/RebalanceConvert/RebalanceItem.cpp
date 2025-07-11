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

    // 업그레이드가 된 아이템에 대해서 컨버팅 보너스를 더해줍니다.
    Item::Grade::GradeInfo RebalanceUpgradeBonus(const Item::ItemInfo& newItemInfo,
        short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM], 
        const unsigned short usUpgradeFactor[][MAX_UPGRADECOL_NUM], 
        bool bSupressedResult[MAX_SUPRESSED_GRADE], 
        unsigned short wDropOption, unsigned char cUpgradeLevel);

    // 업그레이드시, 각 단계값에 대한 Additional Value 테이블입니다.

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

	// edith 2008.01.14 투구및신발등의 아이템 업그레이드 제한부분
    // 투구는 업그레이드 못함
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

	// 트로피
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

	// 산타옷
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

        // 데이터 포지션 이동
        szOutDataPos += sizeof(Item::ItemData) + sizeof(Item::EquipmentInfo);

        // 장비 데이터 채움
        lpNewEquipmentInfo->m_cUpgradeLevel     = 0; // 업그레이드 레벨을 초기화한다

        lpNewEquipmentInfo->m_cDiffMaxSocket    = 0; // 소켓 개수도 초기화한다
        lpNewEquipmentInfo->m_cSeasonRecord		= 0; // 아이템 생성 시기도 초기화한다

        lpNewEquipmentInfo->m_cSocketNum        = 0; // 소켓 개수도 초기화한다
        lpNewEquipmentInfo->m_cAttributeNum     = 0; // 일단은 속성 개수도 초기화한다 (나중에 밑에서 속성 계산 끝나고 다시 넣어준다)

        // 최대 내구도 수정(150을 넘지 않도록 한다)
        lpNewEquipmentInfo->m_cDiffMaxDurability = 
            cMaxDurablility - oldItemInfo.m_DetailData.m_cDefaultDurabilityOrStack;

		lpNewEquipmentInfo->m_cCoreLevel		= 0;
		lpNewEquipmentInfo->m_cReserved1		= 0; // 예약 데이터
        lpNewEquipmentInfo->m_cReserved2        = 0; // 예약 데이터

        // 장비 제한값 계산
        unsigned long dwOldStatusLimitSum = std::accumulate(oldItemInfo.m_UseLimit.m_nStatusLimit, 
            oldItemInfo.m_UseLimit.m_nStatusLimit + OldItem::StatusLimit::MAX_STATUS_LIMIT, 0L);

        if(0 == dwOldStatusLimitSum) { ++dwOldStatusLimitSum; }

        const float fLimitValue = static_cast<float>(newItemInfo.m_UseLimit.m_wLimitValue) / dwOldStatusLimitSum;

        // 이제 아이템 속성을 지대로 복사할 차례인데...
        // 1. 아이템 속성값을 배열에 담는다.
        // 2. 아이템 타입별로, 위에서부터 죽 흝으면서 적용할 속성값만 걸러서, 값을 계산한 후 복사한다.
        
        short wOldAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM];		// 예전 속성값
        short wInvalidAttr[OldItem::Attribute::MAX_ATTRIBUTE_NUM];	// 이상한 속성값        
        short wNewAttr[Item::Attribute::MAX_ATTRIBUTE_NUM];			// 새 속성값

        std::fill_n(wOldAttr, unsigned short(OldItem::Attribute::MAX_ATTRIBUTE_NUM), 0);
        std::fill_n(wNewAttr, unsigned short(Item::Attribute::MAX_ATTRIBUTE_NUM), 0);

        const OldItem::ItemAttribute* lpAttribute = 
            reinterpret_cast<const OldItem::ItemAttribute*>(lpSocketInfo + lpEquipmentInfo->m_cSocketNum);

        // 아이템 속성값 배열에 담기
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

        // 장비 타입에 따라서 속성값을 재계산한다.
        convertedGrade = RebalanceAttr(newItemInfo, lpEquipmentInfo->m_cDiffMaterialType, 
            wOldAttr, wInvalidAttr, wNewAttr, bSupressedResult);
        
        // wNewAttr를 0이 아닌 항목만 골라서 szOutDataPos에 저장한다.
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

        // 속성 개수 조절
        lpNewEquipmentInfo->m_cAttributeNum = nAddedAttrNum;

 
        if(bSupressedResult[SUPRESSED_DEFAULT])
        {
            // B그레이드를 넘는데 C그레이드로 제한되어버렸다.

        }

        if(bSupressedResult[SUPRESSED_1To7])
        {
            // A그레이드를 넘는데 B그레이드로 제한되어버렸다.

        }

        if(convertedGrade.m_eItemGrade <= Item::EquipType::A_GRADE)
        {
            // AA그레이드를 넘는데 A그레이드로 제한되어버렸다.
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

    // 아이템의 그레이드 정보를 초기화
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

            // D 그레이드를 초과하면 F 그레이드가 된다.
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
    // 아이템 등급 출력
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
        // 트로피
        memset(wInvalidAttr, 0, sizeof(unsigned short) * OldItem::Attribute::MAX_ATTRIBUTE_NUM);
        memset(wNewAttr, 0, sizeof(unsigned short) * Item::Attribute::MAX_ATTRIBUTE_NUM);

        itemGrade = UpgradeFactor::RebalanceUpgradeBonus(newItemInfo, wNewAttr, 
            UpgradeFactor::Trophy, bSupressedResult, 1, cUpgradeLevel);
    }
    else if(9911 == newItemInfo.m_usProtoTypeID ||
        9928 == newItemInfo.m_usProtoTypeID ||
        9929 == newItemInfo.m_usProtoTypeID)
    {
        // 산타옷
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
            방어력	    42.5	방어력	    243	    5.717647059		구 방어력 * 5.72	
            방어보정	28.75	회피	    60.75	2.113043478		구 방어보정 * 2.11	
            HP Max	    39.5	HP Max	    60.75	1.537974684		구 HP Max * 1.54	
            MP Max	    39.5	HP 회복	    121.5	3.075949367		구 HP 회복 * 3.08	
                                마법저항	48.6	                DropOption * 0.12	
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
            방어력	    42.5	방어력	    288	    6.776470588		구 방어력 * 6.78	4	DropOption / 320 * 4 * 제련단계	6	DropOption / 320 * 6 * (제련단계 - 6)
            방어보정	28.75	회피	    72	    2.504347826		구 방어보정 * 2.50	2	DropOption / 320 * 2 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            HP Max	    39.5	HP Max	    72	    1.82278481		구 HP Max * 1.82	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            MP Max	    39.5	HP 회복	    144	    3.64556962		구 MP Max * 3.65	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
                                마법저항	57.6	                DropOption * 0.12	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
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

            저항력	39.5	방어력	    234	    5.924050633		구 저항력 * 5.92
            저항력	39.5	회피	    58.5	1.481012658		구 저항력 * 1.48
            HP 회복	39.5	HP Max	    58.5	1.481012658		구 HP 회복 * 1.48
            MP 회복	39.5	HP 회복	    117	    2.962025316		구 MP 회복 * 2.96
            저항력	22.75	마법저항	46.8	2.057142857		구 저항력 * 2.06
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  22.75f);

            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 5.92f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 1.48f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.48f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 2.96f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 2.06f);

            // 기본값 계산이 끝났으니, 아이템 그레이드 계산을 해서 B등급 이상은 C등급으로 낮춘다.
            itemGrade = GetItemGrade(newItemInfo, wNewAttr, Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);
            break;

        case Item::ItemType::DEX_HELM:

            /*
            DEX_HELM
            저항력	39.5	방어력	    277.2	7.017721519		구 저항력 * 7.02
            저항력	39.5	회피	    69.3	1.75443038		구 저항력 * 1.75
            HP 회복	39.5	HP Max	    69.3	1.75443038		구 HP 회복 * 1.75
            MP 회복	39.5	HP 회복	    138.6	3.508860759		구 MP 회복 * 3.51
            저항력	22.75	마법저항	55.44	2.436923077		구 저항력 * 2.44
            */
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::HP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MP_REGEN_AMOUNT,   39.5f);
            CheckInvaildAttr(wInvalidAttr, wOldAttr, OldItem::Attribute::MAGIC_RESISTANCE,  22.75f);        

            wNewAttr[Item::Attribute::ARMOR]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 7.02f);
            wNewAttr[Item::Attribute::EVADE]          = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 1.75f);
            wNewAttr[Item::Attribute::MAX_HP]         = static_cast<short>(wOldAttr[OldItem::Attribute::HP_REGEN_AMOUNT] * 1.75f);
            wNewAttr[Item::Attribute::HP_REGEN]       = static_cast<short>(wOldAttr[OldItem::Attribute::MP_REGEN_AMOUNT] * 3.51f);
            wNewAttr[Item::Attribute::MAGIC_RESIST]   = static_cast<short>(wOldAttr[OldItem::Attribute::MAGIC_RESISTANCE] * 2.44f);

            // 기본값 계산이 끝났으니, 아이템 그레이드 계산을 해서 B등급 이상은 C등급으로 낮춘다.
            itemGrade = GetItemGrade(newItemInfo, wNewAttr, Item::EquipType::B_GRADE, bSupressedResult[SUPRESSED_DEFAULT]);
            break;

        case Item::ItemType::DAGGER:

            /*
            DAGGER
            mDmg	    13.375	mDmg	    57.6	4.306542056		구 mDmg * 4.31	    2	DropOption / 320 * 2 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    72	    2.10989011		구 MDmg * 2.11	    2	DropOption / 320 * 2 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            공격보정	28.75	명중	    288	    10.0173913		구 공격보정 * 10.02	8	DropOption / 320 * 8 * 제련단계	12	DropOption / 320 * 12 * (제련단계 - 6)
                                MPMax	    72	                    DropOption * 0.15	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
                                MP회복	    144	                    DropOption * 0.3	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	57.6	6.630215827		구 크리티컬 * 6.63	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            블록	    8.6875	블록	    57.6	6.630215827		구 블록 * 6.63	    0	0	0	0
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
            mDmg	    13.375	mDmg	    46.8	3.499065421		구 mDmg * 3.50	    2	DropOption / 260 * 2 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    58.5	1.714285714		구 mDmg * 1.71	    2	DropOption / 260 * 2 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            공격보정	28.75	명중	    234	    8.139130435		구 공격보정 * 8.14	8	DropOption / 260 * 8 * 제련단계	12	DropOption / 260 * 12 * (제련단계 - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
                                MP회복	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	46.8	5.38705036		구 크리티컬 * 5.39	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            블록	    8.6875	블록	    46.8	5.38705036		구 블록 * 5.39	    0	0	0	0
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
            mDmg	    13.375	mDmg	    46.8	3.499065421		구 mDmg * 3.50	    2	DropOption / 260 * 2 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    58.5	1.714285714		구 MDmg * 1.71	    2	DropOption / 260 * 2 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            공격보정	10.375	명중	    234	    22.55421687		구 공격보정 * 22.55	8	DropOption / 260 * 8 * 제련단계	12	DropOption / 260 * 12 * (제련단계 - 6)
            HP 회복	    39.5	MPMax	    58.5	1.481012658		구 HP 회복 * 1.48	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            MP 회복	    39.5	MP회복	    117	    2.962025316		구 MP 회복 * 2.96	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            방어보정	10.375	크리티컬	46.8	4.510843373		구 방어보정 * 4.51	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            저항력	    8.6875	블록	    46.8	5.38705036		구 저항력 * 5.39	0	0	0	0
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

            mDmg	    13.375	mDmg	    86.4	6.459813084		구 mDmg * 6.46	    4	DropOption / 320 * 4 * 제련단계	6	DropOption / 320 * 6 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    115.2	3.375824176		구 MDmg * 3.38	    4	DropOption / 320 * 4 * 제련단계	6	DropOption / 320 * 6 * (제련단계 - 6)
            공격보정	28.75	명중	    288	    10.0173913		구 공격보정 * 10.02	8	DropOption / 320 * 8 * 제련단계	12	DropOption / 320 * 12 * (제련단계 - 6)
                                MPMax	    72	                    DropOption * 0.15	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
                                MP회복	    144	                    DropOption * 0.3	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	57.6	6.630215827		구 크리티컬 * 6.63	1	DropOption / 320 * 1 * 제련단계	3	DropOption / 320 * 3 * (제련단계 - 6)
            블록	    8.6875	블록	    57.6	6.630215827		구 블록 * 6.63	    0	0	0	0
            
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

            mDmg	    13.375	mDmg	    70.2	5.248598131		구 mDmg * 5.25	    4	
            MDmg	    34.125	MDmg	    93.6	2.742857143		구 mDmg * 2.74	    4	
            공격보정	28.75	명중	    234	    8.139130435		구 공격보정 * 8.14	8	
                                MPMax	    58.5	                DropOption * 0.15	1	
                                MP회복	    117	                    DropOption * 0.3	1	
            크리티컬	8.6875	크리티컬	46.8	5.38705036		구 크리티컬 * 5.39	1	
            블록	    8.6875	블록	    46.8	5.38705036		구 블록 * 5.39	    0	

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
            mDmg	    13.375	mDmg	    59.4	4.441121495		구 mDmg * 4.44
            MDmg	    34.125	MDmg	    79.2	2.320879121		구 MDmg * 2.32
            공격보정	28.75	명중	    198	    6.886956522		구 공격보정 * 6.89
            MPMax	    49.5	                DropOption * 0.15
            MP회복	    99	                    DropOption * 0.3
            크리티컬	8.6875	크리티컬	39.6	4.558273381		구 크리티컬 * 4.56
            블록	    8.6875	블록	    39.6	4.558273381		구 블록 * 4.56
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

            mDmg	    13.375	mDmg	    70.2	5.248598131		구 mDmg * 5.25	    4	DropOption / 260 * 4 * MIN(제련단계, 6)	6	DropOption / 260 * 6 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    93.6	2.742857143		구 mDmg * 2.74	    4	DropOption / 260 * 4 * MIN(제련단계, 6)	6	DropOption / 260 * 6 * (제련단계 - 6)
            공격보정	28.75	명중	    234	    8.139130435		구 공격보정 * 8.14	8	DropOption / 260 * 8 * MIN(제련단계, 6)	12	DropOption / 260 * 12 * (제련단계 - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * MIN(제련단계, 6)	3	DropOption / 260 * 3 * (제련단계 - 6)
                                MP회복	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * MIN(제련단계, 6)	3	DropOption / 260 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	46.8	5.38705036		구 크리티컬 * 5.39	1	DropOption / 260 * 1 * MIN(제련단계, 6)	3	DropOption / 260 * 3 * (제련단계 - 6)
                                블록	    46.8	                DropOption * 0.12	0	0	0	0
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
            mDmg	    13.375	mDmg	    83.7	6.257943925		구 mDmg * 6.26	    4	DropOption / 310 * 4 * 제련단계	6	DropOption / 310 * 6 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    111.6	3.27032967		구 MDmg * 3.27	    4	DropOption / 310 * 4 * 제련단계	6	DropOption / 310 * 6 * (제련단계 - 6)
            공격보정	10.375	명중	    279	    26.89156627		구 공격보정 * 26.89	8	DropOption / 310 * 8 * 제련단계	12	DropOption / 310 * 12 * (제련단계 - 6)
            HP 회복	    39.5	MPMax	    69.75	1.765822785		구 HP 회복 * 1.77	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
            MP 회복	    39.5	MP회복	    139.5	3.53164557		구 MP 회복 * 3.53	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
            방어보정	10.375	크리티컬	55.8	5.378313253		구 방어보정 * 5.38	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
            저항력	    8.6875	블록	    55.8	6.423021583		구 저항력 * 6.42	0	0	0	0
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
            mDmg	    13.375	mDmg	    72.9	5.45046729		구 mDmg * 5.45	    4	DropOption / 270 * 4 * 제련단계	6	DropOption / 270 * 6 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    97.2	2.848351648		구 MDmg * 2.85	    4	DropOption / 270 * 4 * 제련단계	6	DropOption / 270 * 6 * (제련단계 - 6)
            공격보정	10.375	명중	    243	    23.42168675		구 공격보정 * 23.42	8	DropOption / 270 * 8 * 제련단계	12	DropOption / 270 * 12 * (제련단계 - 6)
            HP 회복	    39.5	MPMax	    60.75	1.537974684		구 HP 회복 * 1.54	1	DropOption / 270 * 1 * 제련단계	3	DropOption / 270 * 3 * (제련단계 - 6)
            MP 회복	    39.5	MP회복	    121.5	3.075949367		구 MP 회복 * 3.08	1	DropOption / 270 * 1 * 제련단계	3	DropOption / 270 * 3 * (제련단계 - 6)
            방어보정	10.375	크리티컬	48.6	4.684337349		구 방어보정 * 4.68	1	DropOption / 270 * 1 * 제련단계	3	DropOption / 270 * 3 * (제련단계 - 6)
            저항력	    8.6875	블록	    48.6	5.594244604		구 저항력 * 5.59	0	0	0	0
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
            mDmg	    13.375	mDmg	    125.55	9.386915888		구 mDmg * 9.39	    6	DropOption / 310 * 6 * 제련단계	9	DropOption / 310 * 9 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    153.5	4.498168498		구 MDmg * 4.50	    6	DropOption / 310 * 6 * 제련단계	9	DropOption / 310 * 9 * (제련단계 - 6)
            공격보정	28.75	명중	    279	    9.704347826		구 공격보정 * 9.70	8	DropOption / 310 * 8 * 제련단계	12	DropOption / 310 * 12 * (제련단계 - 6)
                                MPMax	    69.75	                DropOption * 0.15	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
                                MP회복	    139.5	                DropOption * 0.3	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	55.8	6.423021583		구 크리티컬 * 6.42	1	DropOption / 310 * 1 * 제련단계	3	DropOption / 310 * 3 * (제련단계 - 6)
            블록	    8.6875	블록	    55.8	6.423021583		구 블록 * 6.42	    0	0	0	0
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
            mDmg	    13.375	mDmg	    105.3	7.872897196		구 mDmg * 7.87	    6	DropOption / 260 * 6 * 제련단계	9	DropOption / 260 * 9 * (제련단계 - 6)
            MDmg	    34.125	MDmg	    128.7	3.771428571		구 MDmg * 3.77	    6	DropOption / 260 * 6 * 제련단계	9	DropOption / 260 * 9 * (제련단계 - 6)
            공격보정	28.75	명중	    234	    8.139130435		구 공격보정 * 8.14	8	DropOption / 260 * 8 * 제련단계	12	DropOption / 260 * 12 * (제련단계 - 6)
                                MPMax	    58.5	                DropOption * 0.15	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
                                MP회복	    117	                    DropOption * 0.3	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            크리티컬	8.6875	크리티컬	46.8	5.38705036		구 크리티컬 * 5.39	1	DropOption / 260 * 1 * 제련단계	3	DropOption / 260 * 3 * (제련단계 - 6)
            블록	    8.6875	블록	    46.8	5.38705036		구 블록 * 5.39	    0	0	0	0
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
            mDmg	    13.375	마법력	    288	    DropOption * 0.75
            MDmg	    34.125	MDmg	    158.4	구 MDmg * 4.64
            공격보정	28.75	명중	    288	    구 공격보정 * 10.02
                                MPMax	    144	    DropOption * 0.15
                                MP회복	    288	    DropOption * 0.3
            크리티컬	8.6875	크리티컬	57.6	구 크리티컬 * 6.63
            블록	    8.6875	블록	    57.6	구 블록 * 6.63

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
            블록	11.375	HPMax	27.563	2.423120879		구 블록 * 2.42
            블록	11.375	HP회복	55.125	4.846153846		구 블록 * 4.85
            블록	11.375	MPMax	27.563	2.423120879		구 블록 * 2.42
            블록	11.375	MP회복	55.125	4.846153846		구 블록 * 4.85
            블록	11.375	블록	44.1	3.876923077		구 블록 * 3.88
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
                // 버클러는 업그레이드 단계를 다르게 처리한다.
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

    // 일단 6단계까지만 한번 쫙 돌린다.
    const int n1stLevelCount = 6;
    float fDropOption = static_cast<float>(wDropOption);

    for(int nCount = 0; Item::Attribute::NONE != usUpgradeFactor[nCount][0]; ++nCount)
    {
        // usUpgradeFactor[][0] : 아이템 속성
        // usUpgradeFactor[][1] : Div값

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

    // 7~9단계를 돌리고 A등급으로 낮춘다.
    if(n1stLevelCount < cUpgradeLevel)
    {
        for(int nCount = 0; Item::Attribute::NONE != usUpgradeFactor[nCount][0]; ++nCount)
        {
            // usUpgradeFactor[][0] : 아이템 속성
            // usUpgradeFactor[][1] : Div값

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
            // 장비인 경우 아이템 컨버팅을 한다.
            unsigned long dwDataSize_InOut = 
                static_cast<unsigned long>(szResultEnd - szResultPos);

            if(ConvertRebalanceItem(dwUID, dwCID, 
                lpItemData, *lpNewItemInfo, *lpOldItemInfo, 
                szResultPos, dwDataSize_InOut, convertedGrade))
            {
                // 아이템 위치를 세팅한다.
                if(0 != lpItemChecker)
                {
                    lpItemChecker->Set(reinterpret_cast<Item::ItemData*>(szResultPos)->m_ItemPos, 
                        lpNewItemInfo->m_DetailData.m_cXSize, lpNewItemInfo->m_DetailData.m_cYSize);
                }

                // 복사 위치 증가
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
            // 아이템 타입이 스킬북이고, 스킬북 배열을 받았다면, 
            // 교환권 배열에 카운팅하고, 아이템을 복사하지 않는다.
            lpSkillBookInfo->m_dwBookNum[lpNewItemInfo->m_UseItemInfo.m_usSkill_LockCount] += 
                lpItemData->m_cNumOrDurability;
        }
        else
        {
            // 아이템 위치를 세팅한다.
            if(0 != lpItemChecker)
            {
                lpItemChecker->Set(lpItemData->m_ItemPos, 
                    lpNewItemInfo->m_DetailData.m_cXSize, lpNewItemInfo->m_DetailData.m_cYSize);
            }

            // 그냥 아이템을 복사한다.
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
    // 1/2 레벨 데이터를 돈으로 바꿔서 더해 준다.
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
            "1,2단계 스킬북 보상을 전부 할 수 없습니다. 돈을 %u로 세팅합니다",
            dwUID, dwCID, dwGold_InOut, dwAddGold, ULONG_MAX);

        dwGold_InOut = ULONG_MAX;
    }
    else
    {
        dwGold_InOut += dwAddGold;
    }

    skillBookInfo.m_dwBookNum[0] = 0;
    skillBookInfo.m_dwBookNum[1] = 0;

    // 3/4 레벨 데이터를 스킬북으로 바꿔서 더해 준다. 
    // 이미 ItemChecker에 아이템이 전부 세팅되어 있는 상황이다.
    Item::ItemPos itemPos;

    // 3단계 아이템 삽입
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
                "ItemPos:(%d:0x%03X) / Num:%3d / 3단계 스킬북 삽입 성공",
                szPosition, dwUID, dwCID, skillBook3rd.m_dwUID, skillBook3rd.m_usProtoTypeID, 
                skillBook3rd.m_ItemPos.m_cPos, skillBook3rd.m_ItemPos.m_cIndex, 
                skillBook3rd.m_cNumOrDurability);
        }
    }

    // 4단계 스킬북 삽입
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
                "ItemPos:(%d:0x%03X) / Num:%3d / 4단계 스킬북 삽입 성공",
                szPosition, dwUID, dwCID, skillBook4th.m_dwUID, skillBook4th.m_usProtoTypeID, 
                skillBook4th.m_ItemPos.m_cPos, skillBook4th.m_ItemPos.m_cIndex, 
                skillBook4th.m_cNumOrDurability);
        }
    }

    dwDataSize_InOut = static_cast<unsigned long>(szDataPos - szData);
    return true;
}


}