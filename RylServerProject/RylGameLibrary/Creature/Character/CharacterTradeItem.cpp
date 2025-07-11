#include "stdafx.h"

#include <Item/Item.h>
#include <Item/ItemFactory.h>

#include <Creature/NPC/NPC.h>
#include <Creature/CreatureManager.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Log/ItemLog.h>

#include "Character.h"


unsigned long CCharacter::RepairItem(const unsigned long dwNPCID, Item::ItemPos itemPos, unsigned short& wError)
{
	CCreature* lpCreature = CCreatureManager::GetInstance().GetCreature(dwNPCID);
	if (NULL == lpCreature) 
	{ 
		ERRLOG2(g_Log, "CID:%10u 수리해줄 크리쳐가 존재하지 않습니다. 수리 실패. CID:%10u", m_dwCID, dwNPCID); 
		wError = PktRpI::FAIL_NOT_CREATURE;
		return 0; 
	}

	if (Creature::CT_NPC == Creature::GetCreatureType(dwNPCID))
	{
		int nZone = CServerSetup::GetInstance().GetServerZone();
		if (GetMapIndex() != 0)
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
			if (NULL != lpVirtualArea)
			{
				nZone = static_cast<int>(lpVirtualArea->GetVirtualZone());
			}
		}

		CNPC* lpNPC = reinterpret_cast<CNPC*>(lpCreature);
		if (lpNPC->GetZone() != nZone)
		{
			ERRLOG3(g_Log, "CID:%10u NPC의 존 번호가 다릅니다. Server Zone : %d, NPC Zone : %d", m_dwCID, nZone, lpNPC->GetZone()); 
			wError = PktRpI::FAIL_NOT_NPCZONE;
			return 0; 
		}
	}

	Item::CItem* lpItem = GetItem(itemPos);
	if (NULL == lpItem) 
	{ 
		ERRLOG3(g_Log, "CID:%10u 아이템이 %d Pos, %d Index에 없습니다", m_dwCID, itemPos.m_cPos, itemPos.m_cIndex); 
		wError = PktRpI::FAIL_NOT_POSITEM;
		return 0; 
	}

	if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
	{
		ERRLOG1(g_Log, "CID:%10u 스택 아이템이어서 수리할 수 없습니다", m_dwCID); 
		wError = PktRpI::FAIL_NOT_STACKABLE;
		return 0; 
	}

	if (false == lpItem->IsSet(Item::DetailData::EQUIP))
	{ 
		ERRLOG1(g_Log, "CID:%10u 장비 아이템이 아니어서 수리할 수 없습니다", m_dwCID); 
		wError = PktRpI::FAIL_NOT_EQUIP;
		return 0; 
	}

	if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability()) 
	{ 
//		ERRLOG4(g_Log, "CID:%10u 장비가 이미 최대 내구도에 도달해서, 수리할 수 없습니다."
//			" 종류ID:%d, 현재 내구도:%d, 최대 내구도:%d", m_dwCID, lpItem->GetPrototypeID(),
//			lpItem->GetNumOrDurability(), lpItem->GetMaxNumOrDurability());
		wError = PktRpI::FAIL_FULL_DRUA;
		return 0; 
	}

	Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem);
	unsigned long dwGold = lpCreature->RepairItem(lpEquipment, m_DBData.m_Info.Gold);
	if (0 != dwGold) 
	{ 
		CalculateStatusData(false);
	}
	else
	{
		wError = PktRpAI::FAIL_NOT_REPAIR;
	}

	return dwGold;
}

unsigned long CCharacter::RepairAllItem(const unsigned long dwNPCID, unsigned short& wError)
{
	CCreature* lpCreature = CCreatureManager::GetInstance().GetCreature(dwNPCID);
	if (NULL == lpCreature) 
	{ 
		ERRLOG2(g_Log, "CID:%10u 수리해줄 크리쳐가 존재하지 않습니다. 수리 실패. CID:%10u", m_dwCID, dwNPCID); 
		wError = PktRpAI::FAIL_NOT_CREATURE;
		return 0; 
	}

	if (Creature::CT_NPC == Creature::GetCreatureType(dwNPCID))
	{
		int nZone = CServerSetup::GetInstance().GetServerZone();
		if (GetMapIndex() != 0)
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
			if (NULL != lpVirtualArea)
			{
				nZone = static_cast<int>(lpVirtualArea->GetVirtualZone());
			}
		}

		CNPC* lpNPC = reinterpret_cast<CNPC*>(lpCreature);
		if (lpNPC->GetZone() != nZone)
		{
			ERRLOG3(g_Log, "CID:%10u NPC의 존 번호가 다릅니다. Server Zone : %d, NPC Zone : %d", m_dwCID, nZone, lpNPC->GetZone()); 
			wError = PktRpAI::FAIL_NOT_NPCZONE;
			return 0; 
		}
	}

	unsigned long dwRepairGold = 0;
	Item::ItemPos itemPos;
	itemPos.m_cPos = TakeType::TS_EQUIP;	
	// 1단계 검사
	for (unsigned char cIndex = 0; cIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++cIndex)
	{
		if (GetRace() == CClass::AKHAN && 
			(Item::EquipmentPos::SHIRT == cIndex || 
			Item::EquipmentPos::TUNIC == cIndex || 
			Item::EquipmentPos::SHIELD_HAND2 == cIndex || 
			Item::EquipmentPos::WEAPON_HAND2 < cIndex)) 
		{
			continue;
		}

		itemPos.m_cIndex = cIndex;
		Item::CItem* lpItem = GetItem(itemPos);
		if (NULL == lpItem) { continue; }

		if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
		{
			// 화살/볼트의 경우 수리 불가
			continue;
		}

		if (false == lpItem->IsSet(Item::DetailData::EQUIP))
		{ 
			ERRLOG3(g_Log, "CID:%10u 수리하는 아이템이 장비 아이템이 아닙니다. (%d Pos, %d Index)",
					m_dwCID, itemPos.m_cPos, itemPos.m_cIndex);
			continue;
		}

		if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability())
		{
			// 풀내구의 장비는 스킵
			continue;
		}

		// 골드를 계산한다.
		Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem);
		if(lpEquipment)
			dwRepairGold += lpEquipment->GetRepairPrice();
	}

	// 돈이 실제보다 많이들면 에러를 리턴한다.
	if (dwRepairGold > GetGold())
	{
		wError = PktRpAI::NOT_ENOUGH_GOLD;
		return 0;
	}

	dwRepairGold = 0;
	// 2단계 실제 수리
	for (unsigned char cIndex = 0; cIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++cIndex)
	{
		if (GetRace() == CClass::AKHAN && 
			(Item::EquipmentPos::SHIRT == cIndex || 
			Item::EquipmentPos::TUNIC == cIndex || 
			Item::EquipmentPos::SHIELD_HAND2 == cIndex || 
			Item::EquipmentPos::WEAPON_HAND2 < cIndex)) 
		{
			continue;
		}

		itemPos.m_cIndex = cIndex;
		Item::CItem* lpItem = GetItem(itemPos);
		if (NULL == lpItem) { continue; }

		if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
		{
			// 화살/볼트의 경우 수리 불가
			continue;
		}

		if (false == lpItem->IsSet(Item::DetailData::EQUIP))
		{ 
			ERRLOG3(g_Log, "CID:%10u 수리하는 아이템이 장비 아이템이 아닙니다. (%d Pos, %d Index)",
					m_dwCID, itemPos.m_cPos, itemPos.m_cIndex);
			continue;
		}

		if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability())
		{
			// 풀내구의 장비는 스킵
			continue;
		}

		Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem);

		if(lpEquipment)
			dwRepairGold += lpCreature->RepairItem(lpEquipment, m_DBData.m_Info.Gold);
	}

	if (0 != dwRepairGold) 
	{ 
		CalculateStatusData(false);
	}
	else
	{
		wError = PktRpAI::FAIL_NOT_REPAIR;
	}

	return dwRepairGold;
}

Item::CItem* CCharacter::SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
										 Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError)
{
	Item::CItem* lpItem = m_Stall.GetItem(takeType.m_srcPos);
	if (NULL == lpItem) { return NULL; }

    unsigned long dwCustomerCID = lpCustomer->GetCID();
	unsigned long dwCurrentGold = lpCustomer->GetGold();
	dwPrice = lpItem->GetBuyPrice() * takeType.m_cNum;

    if (dwPrice > dwCurrentGold)
	{
		ERRLOG2(g_Log, "노점상 오류 : 돈이 부족합니다. 가격:%d, 소지금:%d", dwPrice, dwCurrentGold);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
	}

	// 가질수 있는 최대 소지금 초과
	if (GetGold() > ULONG_MAX - dwPrice)
	{
		ERRLOG2(g_Log, "노점상 오류 : 거래후 Gold가 최대 소지금을 초과합니다. 가격:%d, 소지금:%d", dwPrice, GetGold());
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_GOLD_OVERFLOW);
		return NULL;
	}
    
	if (false == lpCustomer->GetInventory().TestItem(takeType.m_dstPos, lpItem->GetPrototypeID(), takeType.m_cNum))
	{
		Item::CItemContainer* lpItemContainer = lpCustomer->GetItemContainer(takeType.m_dstPos.m_cPos);
		if (NULL != lpItemContainer)
		{
			lpItemContainer->DumpItemInfo();
		}
		else
		{
			ERRLOG1(g_Log, "CID:%10u 아이템 덤프를 출력할 수 없습니다.", dwCustomerCID);
		}

		ERRLOG4(g_Log, "CID:%10u 아이템 종류:%d를 (%2d:%2d)에 아이템 넣기 실패.", 
			dwCustomerCID, lpItem->GetPrototypeID(), takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);

		return NULL;
	}

	bool bStackable = lpItem->IsSet(Item::DetailData::STACKABLE);
    unsigned char cNumOrDurability = lpItem->GetNumOrDurability();
	Item::ItemPos ItemPos = lpItem->GetPos();

    // 스택 가능한 아이템인 경우, 개수제한 확인.
    if (!(bStackable && (cNumOrDurability < takeType.m_cNum)))
    {
        // 스택이 불가능하거나, 전부 파는 경우에는 아이템 제거.
        if (!bStackable || (bStackable && (takeType.m_cNum == lpItem->GetNumOrDurability())))
        {
			if (false == m_Stall.RemoveItem(takeType.m_srcPos))
			{
				ERRLOG3(g_Log, "CID:%10u 아이템을 (%2d, %4x)위치로부터 지우는 데 실패했습니다.", 
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
				GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
				return NULL;
			}
			else
			{
				RemoveItem(ItemPos);

				m_Stall.SendRemoveItem(takeType, PktStRI::SC_CANCEL, lpCustomer->GetCharacterName());
			}
        }
		else
		{
	        lpItem->SetNumOrDurability(cNumOrDurability - takeType.m_cNum);

			m_Stall.SendRemoveItem(takeType, PktStRI::SC_CANCEL, lpCustomer->GetCharacterName());

			unsigned short usProtoTypeID = lpItem->GetPrototypeID();
			lpItem = Item::CItemFactory::GetInstance().CreateItem(usProtoTypeID);
			if (NULL == lpItem)
			{
				ERRLOG1(g_Log, "노점상 오류 : 아이템 생성 실패. ProtoTypeID : %d", usProtoTypeID);
				GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
				return NULL;
			}

			lpItem->SetNumOrDurability(takeType.m_cNum);
		}

        AddGold(dwPrice, false);

		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharTradeItem(m_lpGameClientDispatch->GetSendStream(), this, lpCustomer->GetCID(), 
				Item::ItemPos(), NULL, ItemPos, takeType.m_cNum, 0);
		}
    }
    else
    {
		ERRLOG4(g_Log, "노점상 오류 : (%2d, %4x)의 아이템 개수 : %d개 팔려는 아이템 개수 : %d개", 
            takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, cNumOrDurability, takeType.m_cNum);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
    }

	GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::NO_SERVER_ERR);
	return lpItem;
}
