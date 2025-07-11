
#include "stdafx.h"

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/ItemStructure.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Utility/Setup/ServerSetup.h>

#include "ItemCreator.h"


CItemCreator& CItemCreator::GetInstance() 
{
    static CItemCreator itemCreator;
    return itemCreator;
}

CItemCreator::CItemCreator()
{
}


// Desc : 아이템 스크립트 로딩 및 초기화 등등.
bool CItemCreator::Initialize()
{
    ItemCreationLock::Syncronize sync(m_ItemCreationLock);

	char szItemScriptName[MAX_PATH];
	if (SERVER_ID::GROUP_BATTLE_SERVER == CServerSetup::GetInstance().GetServerGroup())
	{
		strncpy(szItemScriptName, "./Script/Game/BGItemScript.txt", MAX_PATH);
	}
	else
	{
		strncpy(szItemScriptName, "./Script/Game/ItemScript.txt", MAX_PATH);
	}

	return Item::CItemMgr::GetInstance().LoadItemProtoType(szItemScriptName);
}


// Desc : 아이템 UID 세팅
void CItemCreator::SetUID(DWORD64 dwUID)
{
    ItemCreationLock::Syncronize sync(m_ItemCreationLock);

    Item::CItemFactory& ItemFactory = Item::CItemFactory::GetInstance();

    if(ItemFactory.GetItemUID() < dwUID)
    {
        ItemFactory.SetItemUID(dwUID);
    }
}

DWORD64 CItemCreator::GetUID() const
{
    return Item::CItemFactory::GetInstance().GetItemUID();
}


// Desc : 아이템 생성
Item::CItem* CItemCreator::Create(unsigned short usID)
{
    ItemCreationLock::Syncronize sync(m_ItemCreationLock);
	return Item::CItemFactory::GetInstance().CreateItem(usID);
}
