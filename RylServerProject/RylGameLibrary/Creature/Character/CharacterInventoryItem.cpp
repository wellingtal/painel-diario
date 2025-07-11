
#include "stdafx.h"

#include <Item/Container/ItemContainer.h>
#include <Item/Container/EquipmentsContainer.h>
#include <Item/Container/ExchangeContainer.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include "Character.h"


Item::CItem* CCharacter::GetItem(Item::ItemPos SrcPos)
{
	switch (SrcPos.m_cPos) 
	{
		case TakeType::TS_EQUIP:		return m_Equipments.GetItem(SrcPos);
		case TakeType::TS_INVEN:		return m_Inventory.GetItem(SrcPos);		
		case TakeType::TS_TEMP:			SrcPos.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
		case TakeType::TS_EXTRA:		return m_ExtraSpace.GetItem(SrcPos);
		case TakeType::TS_EXCHANGE:		return m_Exchange.GetItem(SrcPos);
        case TakeType::TS_DEPOSIT:		return m_Deposit.GetItem(SrcPos);
		case TakeType::TS_STALL:		return m_Stall.GetItem(SrcPos);
		case TakeType::TS_TEMPINVEN:	return m_TempInven.GetItem(SrcPos);
	}

	return NULL;
}

bool CCharacter::RemoveItem(Item::ItemPos SrcPos)//hzhz
{
	Item::CItem* lpItem = GetItem(SrcPos);
	if (NULL == lpItem)
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ �������� �������� �ʽ��ϴ�. SrcPos:(%d/%d)", 
			m_dwCID, SrcPos.m_cPos, SrcPos.m_cIndex);
		return false;
	}

	// �����󿡼��� ������ �ƴѵ� ������ ������ ���� ���� ��... (�߸��ϸ� ������ ���� �� �ֽ��ϴ�.)
	if (TakeType::TS_STALL != SrcPos.m_cPos && TakeType::TS_STALL == lpItem->GetRealPos().m_cPos)
	{
		// RemoveItem() ���� RealPos �� �ٲ�Ƿ� �̸� �����صΰ�, �� ������ SendRemoveItem �� �����ش�. by Vincent
		Item::ItemPos realPos = lpItem->GetRealPos();
		
		m_Stall.RemoveItem(lpItem->GetRealPos());
		unsigned char cNum = (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
		m_Stall.SendRemoveItem(TakeType::TakeType(realPos, realPos, cNum), PktStRI::SC_CANCEL, "");
	}

	switch (SrcPos.m_cPos) 
	{
		case TakeType::TS_EQUIP:		return m_Equipments.RemoveItem(SrcPos);
		case TakeType::TS_INVEN:		return m_Inventory.RemoveItem(SrcPos);		
		case TakeType::TS_TEMP:			SrcPos.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
		case TakeType::TS_EXTRA:		return m_ExtraSpace.RemoveItem(SrcPos);
		case TakeType::TS_EXCHANGE:		return m_Exchange.RemoveItem(SrcPos);
		case TakeType::TS_DEPOSIT:		return m_Deposit.RemoveItem(SrcPos);

		case TakeType::TS_STALL:		
		{
			bool bResult = m_Stall.RemoveItem(SrcPos);
			if (true == bResult)
			{
				unsigned char cNum = (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
				m_Stall.SendRemoveItem(TakeType::TakeType(SrcPos, SrcPos, cNum), PktStRI::SC_CANCEL, "");
			}
			return bResult;
		}

		case TakeType::TS_TEMPINVEN:	return m_TempInven.RemoveItem(SrcPos);
	}

	return true;
}


bool CCharacter::SetItem(Item::ItemPos SrcPos, Item::CItem* lpItem)
{
	switch (SrcPos.m_cPos) 
	{
		case TakeType::TS_EQUIP:		return m_Equipments.SetItem(SrcPos, lpItem);	
		case TakeType::TS_INVEN:		return m_Inventory.SetItem(SrcPos, lpItem);
		case TakeType::TS_TEMP:			SrcPos.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
		case TakeType::TS_EXTRA:		return m_ExtraSpace.SetItem(SrcPos, lpItem);
		case TakeType::TS_EXCHANGE:		return m_Exchange.SetItem(SrcPos, lpItem);
        case TakeType::TS_DEPOSIT:		return m_Deposit.SetItem(SrcPos, lpItem);
		case TakeType::TS_STALL:		return m_Stall.SetItem(SrcPos, lpItem);
		case TakeType::TS_TEMPINVEN:	return m_TempInven.SetItem(SrcPos, lpItem);
	}

	return false;
}