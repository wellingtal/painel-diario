#include "stdafx.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ItemFactory.h"
#include "ItemConstants.h"	//--//

namespace Item
{


CItemOwnerInfo::CItemOwnerInfo(const char* szName, unsigned long dwUID, unsigned long dwCID)
:   m_dwUID(dwUID), m_dwCID(dwCID)
{
    strncpy(m_szName, szName, MAX_NAME);
    m_szName[MAX_NAME - 1] = 0;
}


CItemFactory& CItemFactory::GetInstance()
{
    static CItemFactory itemFactory;
    return itemFactory;
}

CItemFactory::CItemFactory()
:   m_nCurrentUID(0LL)
{


}

CItemFactory::~CItemFactory()
{


}

CItem* CItemFactory::CreateItem(const ItemInfo& itemInfo)
{
    CItem* lpItem = CreateTempItem(itemInfo);

    if(0 != lpItem)
    {
        lpItem->SetUID(GetNewItemUID());
    }

    return lpItem;
}


// Desc     : 아이템 시리얼이 0인 임시 아이템을 생성한다.
CItem* CItemFactory::CreateTempItem(const ItemInfo& itemInfo)
{
    unsigned long dwFlags = itemInfo.m_DetailData.m_dwFlags;

    CItem* lpItem = 0;

    if(DetailData::EQUIP == (dwFlags & DetailData::EQUIP))
    {
        lpItem = new CEquipment(itemInfo);
		
		//--//	start..
		if (0 == itemInfo.m_DetailData.m_wDropOption)
		{
			Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
			if (NULL != lpEquip)
			{
				lpEquip->m_GradeInfo.m_eItemGrade = static_cast<EquipType::Grade>(itemInfo.m_DetailData.m_cGrade);
				// edith 2008.02.10 임시아이템도 밸런스패치된걸로 처리한다. 원래는 없었다.
				// lpEquip->SetNewEquip();
			}
		}
		//--//	end..
    }
    else if(DetailData::USE_ITEM == (dwFlags & DetailData::USE_ITEM))
    {
        lpItem = new CUseItem(itemInfo);            
    }
    else
    {
        lpItem = new CItem(itemInfo);            
    }

    return lpItem;
}



CItem* CItemFactory::CreateItem(unsigned short usProtoTypeID)
{
    const ItemInfo* lpItemInfo = CItemMgr::GetInstance().GetItemInfo(usProtoTypeID);
   
    if(0 != lpItemInfo)
    {
        return CreateItem(*lpItemInfo);
    }

    ERRLOG1(g_Log, "ProtoTypeID:%u / 아이템 생성 실패 : 스크립트에 없는 종류ID입니다", usProtoTypeID);
    return 0;
}


CItem* CItemFactory::CreateItem(const char* lpSerializedItem_In, size_t& nParseLength_InOut)
{
    const ItemData* lpItemData = reinterpret_cast<const ItemData*>(lpSerializedItem_In);
    const ItemInfo* lpItemInfo = 0;
    
    if (0 != lpItemData && 
        0 != (lpItemInfo = CItemMgr::GetInstance().GetItemInfo(lpItemData->m_usProtoTypeID)))
    {
        // serialize시 itemUID가 들어가므로, itemUID가 생성된 아이템을 만들 필요는 없다.
        CItem* lpItem = CreateTempItem(*lpItemInfo);

        if (0 != lpItem && 
            !lpItem->SerializeIn(lpSerializedItem_In, nParseLength_InOut))
        {
            DELETE_ITEM(lpItem);
        }

        return lpItem;
    }

    return 0;
}


void CItemFactory::DeleteItem(Item::CItem* lpDeleteItem)
{
    // 여기서 전역 맵에서 데이터 삭제;;
    RemoveItemMap(lpDeleteItem->GetUID());
    delete lpDeleteItem;
}


CItemOwnerInfo* CItemFactory::AddItemMap(unsigned __int64 dwItemUID, const CItemOwnerInfo& itemOwnerInfo)
{
    CItemOwnerInfo* lpOwnerInfo = 0; 

    ItemSerialMap::iterator pos = m_ItemSerialMap.lower_bound(dwItemUID);

    if(pos == m_ItemSerialMap.end() || m_ItemSerialMap.comp(dwItemUID, pos->first))
    {
        m_ItemSerialMap.insert(pos, ItemSerialMap::value_type(dwItemUID, itemOwnerInfo));
    }
    else
    {
        // 이미 맵에 데이터가 있다. 소유자 정보를 리턴한다.
        lpOwnerInfo = &pos->second;
    }

    return lpOwnerInfo;
}

void CItemFactory::RemoveItemMap(unsigned __int64 dwItemUID)
{
    m_ItemSerialMap.erase(dwItemUID);
}

void CItemFactory::ClearItemMap()
{
    m_ItemSerialMap.clear();
}


Item::CItem* CreateItemFromGiveInfo(CItemMgr& itemMgr, CItemFactory& itemFactory, 
                                    GiveItemInfo& giveItemInfo, Item::ItemPos itemPos)
{
    char szItemData[Item::MAX_ITEM_SIZE];
    unsigned long dwDataSize = Item::MAX_ITEM_SIZE;

    unsigned __int64 dwItemUID = itemFactory.GetNewItemUID();

    if (CreateItemDataFromGiveInfo(itemMgr, giveItemInfo, 
        szItemData, dwDataSize, dwItemUID, itemPos))
    {
        size_t data_size = dwDataSize;
        return itemFactory.CreateItem(szItemData, data_size);        
    }

    return NULL;
}


inline void AddAttrFromGiveInfo(short wNewAttribute[Item::Attribute::MAX_ATTRIBUTE_NUM],
                                GiveItemInfo& giveItemInfo,
                                Item::CItemType::ArrayType eArrayType,
                                unsigned short wDropOption, 
                                Item::Attribute::Type eAttrType,
                                GiveItemInfo::GiveItemType eGiveItemType)
{
    if(giveItemInfo.m_cAttribute[eGiveItemType] < Item::EquipType::MAX_GRADE)
    {   
        unsigned short usMinValue = static_cast<short>(wDropOption * 
            Item::Grade::GetGradeValue(eArrayType, Item::Grade::VALUE_GRADE, 
            static_cast<Item::EquipType::Grade>(giveItemInfo.m_cAttribute[eGiveItemType]), eAttrType));

        if(Item::EquipType::AAA_GRADE == giveItemInfo.m_cAttribute[eGiveItemType])
        {
            wNewAttribute[eAttrType] += usMinValue + 2;
        }
        else
        {
            unsigned short usMaxValue = static_cast<short>(wDropOption * 
                Item::Grade::GetGradeValue(eArrayType, Item::Grade::VALUE_GRADE, 
                static_cast<Item::EquipType::Grade>(giveItemInfo.m_cAttribute[eGiveItemType - 1]), eAttrType));

            wNewAttribute[eAttrType] += static_cast<short>((usMaxValue + usMinValue) / 2.0f + 0.5f);
        }
    }
}

bool CreateItemDataFromGiveInfo(Item::CItemMgr& itemMgr, GiveItemInfo& giveItemInfo, 
                                char* szData, unsigned long& dwDataSize_InOut, 
                                unsigned __int64 dwItemUID, Item::ItemPos itemPos)//what is given to you when you upgrade? hayzohar
{
    // 일단 기본 아이템 데이터를 넣는다.
    const Item::ItemInfo* lpItemInfo = 0;

    if (Item::MAX_ITEM_SIZE <= dwDataSize_InOut &&
        0 != (lpItemInfo = itemMgr.GetItemInfo(giveItemInfo.m_usPrototypeID)))
    {
        Item::ItemData* lpItemData = reinterpret_cast<Item::ItemData*>(szData);

        lpItemData->m_dwUID             = dwItemUID;                        //  아이템 UID
        lpItemData->m_usProtoTypeID     = giveItemInfo.m_usPrototypeID;     //  아이템 종류 ID
        lpItemData->m_ItemPos           = itemPos;                          //  아이템 위치
        lpItemData->m_cItemSize         = sizeof(Item::ItemData);           //  아이템 구조체 크기
        lpItemData->m_cNumOrDurability  = giveItemInfo.m_cAmount;           //  개수 혹은 내구도

        if(lpItemInfo->m_DetailData.m_dwFlags & Item::DetailData::EQUIP)
        {
            // 장비다. 관련 데이터를 좀 더 넣는다.
            Item::EquipmentInfo* lpEquipmentInfo = 
                reinterpret_cast<Item::EquipmentInfo*>(lpItemData + 1);

            Item::ItemAttribute* lpItemAttribute = 
                reinterpret_cast<Item::ItemAttribute*>(lpEquipmentInfo + 1);

            const unsigned char cDefaultDurability = 
                lpItemInfo->m_DetailData.m_cDefaultDurabilityOrStack;

			lpEquipmentInfo->m_cUpgradeCnt		= 0;
            lpEquipmentInfo->m_cReserved1		= 0;
            lpEquipmentInfo->m_cUpgradeLevel    = 0;

            lpEquipmentInfo->m_cDiffMaxSocket   = 0;
            lpEquipmentInfo->m_cSeasonRecord    = 1;

            lpEquipmentInfo->m_cSocketNum       = 0;
            lpEquipmentInfo->m_cAttributeNum    = 0;

            lpEquipmentInfo->m_cDiffMaxDurability   = 
                (cDefaultDurability <= giveItemInfo.m_cAmount) 
                ? giveItemInfo.m_cAmount - cDefaultDurability : 0;

            lpEquipmentInfo->m_cReserved2		= 0;
			lpEquipmentInfo->m_cCoreLevel		= 0;

            lpItemData->m_cItemSize += sizeof(Item::EquipmentInfo);

            // 속성을 계산한다. 아이템 타입별로 가질 수 있는 속성이 다르다.
            
            Item::CItemType::ArrayType eArrayType = 
                Item::CItemType::GetEquipType(lpItemInfo->m_DetailData.m_dwFlags);

            short wNewAttribute[Item::Attribute::MAX_ATTRIBUTE_NUM];
            std::fill_n(wNewAttribute, int(Item::Attribute::MAX_ATTRIBUTE_NUM), 0);

            unsigned short wDropOption = lpItemInfo->m_DetailData.m_wDropOption;

            switch(lpItemInfo->m_DetailData.m_cItemType)
            {
            case Item::ItemType::CON_ARMOUR:
            case Item::ItemType::CON_BODY:
            case Item::ItemType::DEX_BODY:
            case Item::ItemType::DEX_ARMOUR:
            case Item::ItemType::CON_HELM:
            case Item::ItemType::CON_HEAD:
            case Item::ItemType::DEX_HEAD:
            case Item::ItemType::DEX_HELM:
///
				// edith 2008.01.14 아이템 옵션추가작업
            case Item::ItemType::CON_GLOVE:
            case Item::ItemType::DEX_GLOVE:
            case Item::ItemType::CON_BOOTS:
            case Item::ItemType::DEX_BOOTS:
            case Item::ItemType::CON_PELVIS:
            case Item::ItemType::DEX_PELVIS:
            case Item::ItemType::CON_PROTECT_A:
            case Item::ItemType::DEX_PROTECT_A:
///
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::ARMOR, GiveItemInfo::ARMOR);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::EVADE, GiveItemInfo::EVADE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_HP, GiveItemInfo::MAX_HP);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::HP_REGEN, GiveItemInfo::HP_REGEN);
            //    AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_MP, GiveItemInfo::MAX_MP);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAGIC_RESIST, GiveItemInfo::MAGIC_RESIST);
                break;

            case Item::ItemType::DAGGER:
            case Item::ItemType::OPP_TALON:
            case Item::ItemType::SKILL_A_KNIFE:
            case Item::ItemType::ONEHANDED_SWORD:
            case Item::ItemType::ONEHANDED_BLUNT:
            case Item::ItemType::COM_SWORD:
            case Item::ItemType::OPP_HAMMER:
            case Item::ItemType::ONEHANDED_AXE:
            case Item::ItemType::OPP_SLUSHER:
            case Item::ItemType::COM_BLUNT:
            case Item::ItemType::OPP_AXE:
            case Item::ItemType::BOW:
            case Item::ItemType::CROSSBOW:
            case Item::ItemType::SKILL_A_ATTACK:
            case Item::ItemType::SKILL_A_GUN:
            case Item::ItemType::TWOHANDED_SWORD:
            case Item::ItemType::TWOHANDED_AXE:
            case Item::ItemType::TWOHANDED_BLUNT:

                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MIN_DAMAGE, GiveItemInfo::MIN_DAMAGE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_DAMAGE, GiveItemInfo::MAX_DAMAGE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::HIT_RATE, GiveItemInfo::HIT_RATE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_MP, GiveItemInfo::MAX_MP);   // add by shadow
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MP_REGEN, GiveItemInfo::MP_REGEN);  // add by shadow
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::CRITICAL, GiveItemInfo::CRITICAL);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::BLOCK, GiveItemInfo::BLOCK);
                break;
                
            case Item::ItemType::STAFF:
            case Item::ItemType::OPP_SYTHE:

                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_DAMAGE, GiveItemInfo::MAX_DAMAGE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::HIT_RATE, GiveItemInfo::HIT_RATE);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_MP, GiveItemInfo::MAX_MP);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MP_REGEN, GiveItemInfo::MP_REGEN);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::CRITICAL, GiveItemInfo::CRITICAL);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAGIC_POWER, GiveItemInfo::MAGIC_POWER);
                break;
                
            case Item::ItemType::SHIELD:
            case Item::ItemType::SKILL_A_GUARD:

                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_HP, GiveItemInfo::MAX_HP);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::HP_REGEN, GiveItemInfo::HP_REGEN);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MAX_MP, GiveItemInfo::MAX_MP);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::MP_REGEN, GiveItemInfo::MP_REGEN);
                AddAttrFromGiveInfo(wNewAttribute, giveItemInfo, eArrayType, wDropOption, Item::Attribute::BLOCK, GiveItemInfo::BLOCK);
                break;
            }

            // 데이터를 다 넣었으니, 속성을 추가한다.
            for(int nCount = 0; nCount < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nCount)
            {
                if(0 < wNewAttribute[nCount])
                {
                    lpItemAttribute->m_cType    = nCount;
                    lpItemAttribute->m_usValue  = wNewAttribute[nCount];
                    
                    ++lpItemAttribute;
                    ++lpEquipmentInfo->m_cAttributeNum;
                }
            }

            lpItemData->m_cItemSize += 
                lpEquipmentInfo->m_cAttributeNum * sizeof(Item::ItemAttribute);
        }

        dwDataSize_InOut = lpItemData->m_cItemSize;
        return true;
    }    

    return false;
}


}