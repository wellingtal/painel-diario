
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
			ERRLOG1(g_Log, "CID:%10u / �� ��� ����", m_lpOwner->GetCID());
			return false;
		}

		// �� ���� ������
		Item::CItem* lpItem = m_lppItems[0];
		ItemPos itemPos(TakeType::TS_TEMPINVEN, 0);

		// �ӽ� �κ��丮���� ����
		if (false == RemoveItem(itemPos))
		{
			ERRLOG1(g_Log, "CID:%10u / �ӽ� �κ��丮�� �� ���� �� ���� �������� �����µ� �����Ͽ����ϴ�. (�ӽ� �κ��丮���� ����)",
				m_lpOwner->GetCID());		    
			return false;
		}

		// �ʵ忡 ������ ������
		lpCell->SetItem(m_lpOwner->GetCurrentPos(), lpItem, 0, m_lpOwner->GetCID(), CCell::NONE, itemInfo);

		// ������ ������ ����
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
		ERRLOG1(g_Log, "CID:%10u / �ӽ� �κ��丮�� ������ �߰��� �����Ͽ����ϴ�.", m_lpOwner->GetCID()); 
		return false; 
	}

    // ������ ������ ���� ����
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
        ERRLOG1(g_Log, "CID:%10u / �ӽ� �κ��丮���� �������� �����ϴµ� �����Ͽ����ϴ�.", m_dwCID);
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
