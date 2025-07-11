
#include "stdafx.h"

#include <Item/Item.h>
#include <Item/Container/ItemContainer.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>

#include <Creature/Character/Character.h>
#include <Log/ItemLog.h>

#include "TempInvenContainer.h"

Item::CTempInvenContainer::CTempInvenContainer()
:	m_lpOwner(NULL), m_cItemNum(0)
{
}

Item::CTempInvenContainer::~CTempInvenContainer()
{
}

bool Item::CTempInvenContainer::Initialize(CCharacter* lpCharacter, unsigned short nMaxSize)
{
	m_lpOwner = lpCharacter;
	return Item::CListContainer::Initialize(lpCharacter->GetCID(), nMaxSize);
}

Item::CItem* Item::CTempInvenContainer::GetItem(Item::ItemPos itemPos) 
{ 
	CItem* lpItem = m_lppItems[0];
	return (lpItem != m_lpNullItem) ? lpItem : NULL;
}

bool Item::CTempInvenContainer::SetItem(Item::ItemPos itemPos, Item::CItem* lpItem) 
{ 
	CGameClientDispatch* lpDispatch = m_lpOwner->GetDispatcher();

	if (true == IsFull())
	{
		CCell::ItemInfo itemInfo;

		CCell* lpCell = m_lpOwner->GetCellPos().m_lpCell;
		if (NULL == lpCell) 
		{
			ERRLOG1(g_Log, "CID:%10u / 셀 얻기 실패", m_lpOwner->GetCID());
			return false;
		}

		// 맨 위의 아이템
		Item::CItem* lpItem = m_lppItems[0];
		ItemPos itemPos(TakeType::TS_TEMPINVEN, 0);

		// 임시 인벤토리에서 제거
		if (false == RemoveItem(itemPos))
		{
			ERRLOG1(g_Log, "CID:%10u / 임시 인벤토리가 꽉 차서 맨 위의 아이템을 떨구는데 실패하였습니다. (임시 인벤토리에서 제거)",
				m_lpOwner->GetCID());		    
			return false;
		}

		// 필드에 아이템 떨구기
		lpCell->SetItem(m_lpOwner->GetCurrentPos(), lpItem, 0, m_lpOwner->GetCID(), CCell::NONE, itemInfo);

		// 아이템 떨구기 보냄
		GAMELOG::LogDropItem(*m_lpOwner, itemPos, itemInfo.m_lpItem, 0, 0);

		if (0 != lpDispatch)
		{
			GameClientSendPacket::SendCharPullDown(lpDispatch->GetSendStream(),
				m_lpOwner->GetCID(), itemPos, itemInfo, PktBase::NO_SERVER_ERR);        
		}
	}

	itemPos.m_cIndex = m_cItemNum;

	if (!Item::CListContainer::SetItem(itemPos, lpItem)) 
	{ 
		ERRLOG1(g_Log, "CID:%10u / 임시 인벤토리에 아이템 추가를 실패하였습니다.", m_lpOwner->GetCID()); 
		return false; 
	}

    // 성공시 아이템 개수 증가
	++m_cItemNum;

	if (0 != lpDispatch)
	{
		unsigned char cNum = (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
		GameClientSendPacket::SendCharPickUp(lpDispatch->GetSendStream(), m_lpOwner->GetCID(), 
			0, 0, lpItem, itemPos, cNum, PktBase::NO_SERVER_ERR);
	}

	return true;
}

bool Item::CTempInvenContainer::RemoveItem(Item::ItemPos itemPos) 
{
	if (NULL == m_lppItems[0])
	{
        ERRLOG1(g_Log, "CID:%10u / 임시 인벤토리에서 아이템을 제거하는데 실패하였습니다.", m_dwCID);
		return false;
	}

	for (unsigned char cIndex = 0; cIndex < m_nMaxSize - 1; ++cIndex)
	{
		m_lppItems[cIndex] = m_lppItems[cIndex + 1];
		if (NULL != m_lppItems[cIndex])
		{
			m_lppItems[cIndex]->MoveItem(ItemPos(TakeType::TS_TEMPINVEN, cIndex));
		}
	}

	m_lppItems[m_nMaxSize - 1] = NULL;
	--m_cItemNum;

	return true;
}

bool Item::CTempInvenContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
	if (false == CListContainer::SerializeIn(szItemBuffer_In, dwBufferSize_In))
	{
		return false;
	}

	for (unsigned char cIndex = 0; cIndex < m_nMaxSize; ++cIndex)
	{
		if (NULL == m_lppItems[cIndex])
		{
			break;
		}

		++m_cItemNum;
	}

	return true;
}
