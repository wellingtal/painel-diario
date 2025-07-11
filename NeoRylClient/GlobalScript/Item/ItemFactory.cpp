#include "stdafx.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ItemFactory.h"

#include <Log/ServerLog.h>
#include "GMMemory.h"

namespace Item
{



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
        lpItem->SetUID(m_nCurrentUID++);
    }

    return lpItem;
}


// Desc     : ������ �ø����� 0�� �ӽ� �������� �����Ѵ�.
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

    ERRLOG1(g_Log, "ProtoTypeID:%u / ������ ���� ���� : ��ũ��Ʈ�� ���� ����ID�Դϴ�", usProtoTypeID);
    return 0;
}


CItem* CItemFactory::CreateItem(const char* lpSerializedItem_In, size_t& nParseLength_InOut)
{
    const ItemData* lpItemData = reinterpret_cast<const ItemData*>(lpSerializedItem_In);
    const ItemInfo* lpItemInfo = 0;

    if(0 != lpItemData && 
        0 != (lpItemInfo = CItemMgr::GetInstance().GetItemInfo(lpItemData->m_usProtoTypeID)))
    {
        // serialize�� itemUID�� ���Ƿ�, itemUID�� ������ �������� ���� �ʿ�� ����.
        CItem* lpItem = CreateTempItem(*lpItemInfo);

        if(0 != lpItem)
        {
            if(!lpItem->SerializeIn(lpSerializedItem_In, nParseLength_InOut))
            {
                DELETE_ITEM(lpItem);
            }
        }

        return lpItem;
    }

    return 0;
}


void CItemFactory::DeleteItem(Item::CItem* lpDeleteItem)
{
    delete lpDeleteItem;
}



}