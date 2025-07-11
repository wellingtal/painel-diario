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
		ERRLOG2(g_Log, "CID:%10u �������� ũ���İ� �������� �ʽ��ϴ�. ���� ����. CID:%10u", m_dwCID, dwNPCID); 
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
			ERRLOG3(g_Log, "CID:%10u NPC�� �� ��ȣ�� �ٸ��ϴ�. Server Zone : %d, NPC Zone : %d", m_dwCID, nZone, lpNPC->GetZone()); 
			wError = PktRpI::FAIL_NOT_NPCZONE;
			return 0; 
		}
	}

	Item::CItem* lpItem = GetItem(itemPos);
	if (NULL == lpItem) 
	{ 
		ERRLOG3(g_Log, "CID:%10u �������� %d Pos, %d Index�� �����ϴ�", m_dwCID, itemPos.m_cPos, itemPos.m_cIndex); 
		wError = PktRpI::FAIL_NOT_POSITEM;
		return 0; 
	}

	if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
	{
		ERRLOG1(g_Log, "CID:%10u ���� �������̾ ������ �� �����ϴ�", m_dwCID); 
		wError = PktRpI::FAIL_NOT_STACKABLE;
		return 0; 
	}

	if (false == lpItem->IsSet(Item::DetailData::EQUIP))
	{ 
		ERRLOG1(g_Log, "CID:%10u ��� �������� �ƴϾ ������ �� �����ϴ�", m_dwCID); 
		wError = PktRpI::FAIL_NOT_EQUIP;
		return 0; 
	}

	if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability()) 
	{ 
//		ERRLOG4(g_Log, "CID:%10u ��� �̹� �ִ� �������� �����ؼ�, ������ �� �����ϴ�."
//			" ����ID:%d, ���� ������:%d, �ִ� ������:%d", m_dwCID, lpItem->GetPrototypeID(),
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
		ERRLOG2(g_Log, "CID:%10u �������� ũ���İ� �������� �ʽ��ϴ�. ���� ����. CID:%10u", m_dwCID, dwNPCID); 
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
			ERRLOG3(g_Log, "CID:%10u NPC�� �� ��ȣ�� �ٸ��ϴ�. Server Zone : %d, NPC Zone : %d", m_dwCID, nZone, lpNPC->GetZone()); 
			wError = PktRpAI::FAIL_NOT_NPCZONE;
			return 0; 
		}
	}

	unsigned long dwRepairGold = 0;
	Item::ItemPos itemPos;
	itemPos.m_cPos = TakeType::TS_EQUIP;	
	// 1�ܰ� �˻�
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
			// ȭ��/��Ʈ�� ��� ���� �Ұ�
			continue;
		}

		if (false == lpItem->IsSet(Item::DetailData::EQUIP))
		{ 
			ERRLOG3(g_Log, "CID:%10u �����ϴ� �������� ��� �������� �ƴմϴ�. (%d Pos, %d Index)",
					m_dwCID, itemPos.m_cPos, itemPos.m_cIndex);
			continue;
		}

		if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability())
		{
			// Ǯ������ ���� ��ŵ
			continue;
		}

		// ��带 ����Ѵ�.
		Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem);
		if(lpEquipment)
			dwRepairGold += lpEquipment->GetRepairPrice();
	}

	// ���� �������� ���̵�� ������ �����Ѵ�.
	if (dwRepairGold > GetGold())
	{
		wError = PktRpAI::NOT_ENOUGH_GOLD;
		return 0;
	}

	dwRepairGold = 0;
	// 2�ܰ� ���� ����
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
			// ȭ��/��Ʈ�� ��� ���� �Ұ�
			continue;
		}

		if (false == lpItem->IsSet(Item::DetailData::EQUIP))
		{ 
			ERRLOG3(g_Log, "CID:%10u �����ϴ� �������� ��� �������� �ƴմϴ�. (%d Pos, %d Index)",
					m_dwCID, itemPos.m_cPos, itemPos.m_cIndex);
			continue;
		}

		if (lpItem->GetNumOrDurability() >= lpItem->GetMaxNumOrDurability())
		{
			// Ǯ������ ���� ��ŵ
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
		ERRLOG2(g_Log, "������ ���� : ���� �����մϴ�. ����:%d, ������:%d", dwPrice, dwCurrentGold);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
	}

	// ������ �ִ� �ִ� ������ �ʰ�
	if (GetGold() > ULONG_MAX - dwPrice)
	{
		ERRLOG2(g_Log, "������ ���� : �ŷ��� Gold�� �ִ� �������� �ʰ��մϴ�. ����:%d, ������:%d", dwPrice, GetGold());
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
			ERRLOG1(g_Log, "CID:%10u ������ ������ ����� �� �����ϴ�.", dwCustomerCID);
		}

		ERRLOG4(g_Log, "CID:%10u ������ ����:%d�� (%2d:%2d)�� ������ �ֱ� ����.", 
			dwCustomerCID, lpItem->GetPrototypeID(), takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);

		return NULL;
	}

	bool bStackable = lpItem->IsSet(Item::DetailData::STACKABLE);
    unsigned char cNumOrDurability = lpItem->GetNumOrDurability();
	Item::ItemPos ItemPos = lpItem->GetPos();

    // ���� ������ �������� ���, �������� Ȯ��.
    if (!(bStackable && (cNumOrDurability < takeType.m_cNum)))
    {
        // ������ �Ұ����ϰų�, ���� �Ĵ� ��쿡�� ������ ����.
        if (!bStackable || (bStackable && (takeType.m_cNum == lpItem->GetNumOrDurability())))
        {
			if (false == m_Stall.RemoveItem(takeType.m_srcPos))
			{
				ERRLOG3(g_Log, "CID:%10u �������� (%2d, %4x)��ġ�κ��� ����� �� �����߽��ϴ�.", 
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
				ERRLOG1(g_Log, "������ ���� : ������ ���� ����. ProtoTypeID : %d", usProtoTypeID);
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
		ERRLOG4(g_Log, "������ ���� : (%2d, %4x)�� ������ ���� : %d�� �ȷ��� ������ ���� : %d��", 
            takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, cNumOrDurability, takeType.m_cNum);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
    }

	GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::NO_SERVER_ERR);
	return lpItem;
}
