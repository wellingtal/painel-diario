
#include "stdafx.h"

#include <Creature/Character/Character.h>
#include <Item/Container/ContainerConstant.h>

#include "InventoryContainer.h"

using namespace Item;

CInventoryContainer::CInventoryContainer()
:	m_bAdminToolFlag(false)
{
}

CInventoryContainer::~CInventoryContainer()
{
}

bool CInventoryContainer::SetItem(ItemPos itemPos, CItem* lpItem)
{
	if (false == m_bAdminToolFlag)
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
		if (NULL == lpCharacter)
		{
			return false;
		}

		// 퀘스트 트리거 발동
		short wCount = (true == lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
		lpCharacter->CheckTrigger(Quest::TRIGGER_PICK, lpItem->GetPrototypeID(), Position(), wCount);
	}

	if (NULL == lpItem)
	{
		ERRLOG1(g_Log, "CID:%10u 아이템이 NULL입니다.", m_dwCID);
		return false;
	}

	unsigned char cX, cY, cTab;
	itemPos.GetPos(cX, cY, cTab);

	// 퀘스트 탭 체크
	unsigned char cItemType = lpItem->GetItemInfo().m_DetailData.m_cItemType;
/*
	// edith 2008.06.04 인벤토리 퀘스트 아이템 탭 제거
	if ((Item::ItemType::QUEST_ITEM == cItemType && ContainerConstant::QUEST_INVENTORY_TAB != cTab) ||
		(Item::ItemType::QUEST_ITEM != cItemType && ContainerConstant::QUEST_INVENTORY_TAB == cTab))
	{
		ERRLOG4(g_Log, "CID:%10u 아이템에 맞는 올바른 탭이 아닙니다. ItemType:%d, Tab:%d/%d", 
			m_dwCID, cItemType, cTab, m_nTabNum);
		return false;
	}
*/
	unsigned char cXSize = lpItem->GetItemInfo().m_DetailData.m_cXSize;
	unsigned char cYSize = lpItem->GetItemInfo().m_DetailData.m_cYSize;

	if (m_nXSize < static_cast<size_t>(cX + cXSize) || 
		m_nYSize < static_cast<size_t>(cY + cYSize) || m_nTabNum <= cTab)
	{
		ERRLOG9(g_Log, "CID:%10u 아이템 위치 및 크기가 올바르지 않습니다. "
			"X:%d/%d, Y:%d/%d, Tab:%d/%d, X크기:%d, Y크기:%d", 
			m_dwCID, cX, m_nXSize, cY, m_nYSize, cTab, m_nTabNum, cXSize, cYSize);
		return false;
	}

	CItem** lppItemYIndexPastEnd = m_lppItems + m_nSizePerTab * cTab + m_nXSize * (cY + cYSize);
	CItem **lppItemYIndex, **lppItemXIndex, **lppItemXIndexPastEnd;

	for (lppItemYIndex = m_lppItems + m_nSizePerTab * cTab + cY * m_nXSize;
		lppItemYIndex != lppItemYIndexPastEnd; lppItemYIndex += m_nXSize)
	{
		lppItemXIndexPastEnd = lppItemYIndex + cX + cXSize;

		for (lppItemXIndex = lppItemYIndex + cX;
			lppItemXIndex != lppItemXIndexPastEnd; ++lppItemXIndex)
		{
			if (0 != *lppItemXIndex)
			{
				ERRLOG4(g_Log, "CID:%10u (%d,%d,%d) 이미 그 장소에 아이템이 있습니다.",
					m_dwCID, cX, cY, cTab);
				return false;
			}            
		}        
	}

	for (lppItemYIndex = m_lppItems + m_nSizePerTab * cTab + cY * m_nXSize;
		lppItemYIndex != lppItemYIndexPastEnd; lppItemYIndex += m_nXSize)
	{
		lppItemXIndexPastEnd = lppItemYIndex + cX + cXSize;

		for (lppItemXIndex = lppItemYIndex + cX;
			lppItemXIndex != lppItemXIndexPastEnd; ++lppItemXIndex)
		{
			*lppItemXIndex = m_lpNullItem;
		}
	}

	m_lppItems[m_nSizePerTab * cTab + m_nXSize * cY + cX] = lpItem;
	lpItem->MoveItem(itemPos);
	return true;
}

bool CInventoryContainer::TestItem(ItemPos itemPos, unsigned short usProtoTypeID, unsigned char cNum)
{
	const Item::ItemInfo* lpItemInfo = CItemMgr::GetInstance().GetItemInfo(usProtoTypeID);

	if (NULL == lpItemInfo)
	{
		ERRLOG2(g_Log, "CID:%10u 아이템 종류 ID가 없습니다. usProtoTypeID:%d",
			m_dwCID, usProtoTypeID);
		return false;
	}

	unsigned char cX, cY, cTab;
	itemPos.GetPos(cX, cY, cTab);

	// 퀘스트 탭 체크
	unsigned char cItemType = lpItemInfo->m_DetailData.m_cItemType;
/*
	// edith 2008.06.04 인벤토리 퀘스트 아이템 탭 제거
	if ((Item::ItemType::QUEST_ITEM == cItemType && ContainerConstant::QUEST_INVENTORY_TAB != cTab) ||
		(Item::ItemType::QUEST_ITEM != cItemType && ContainerConstant::QUEST_INVENTORY_TAB == cTab))
	{
		ERRLOG4(g_Log, "CID:%10u 아이템에 맞는 올바른 탭이 아닙니다. ItemType:%d, Tab:%d/%d", 
			m_dwCID, cItemType, cTab, m_nTabNum);
		return false;
	}
*/
	unsigned char cXSize = lpItemInfo->m_DetailData.m_cXSize;
	unsigned char cYSize = lpItemInfo->m_DetailData.m_cYSize;

	if (m_nXSize < static_cast<size_t>(cX + cXSize) || 
		m_nYSize < static_cast<size_t>(cY + cYSize) || m_nTabNum <= cTab)
	{
		ERRLOG9(g_Log, "CID:%10u 아이템 위치 및 크기가 올바르지 않습니다. "
			"X:%d/%d, Y:%d/%d, Tab:%d/%d, X크기:%d, Y크기:%d", 
			m_dwCID, cX, m_nXSize, cY, m_nYSize, cTab, m_nTabNum, cXSize, cYSize);
		return false;
	}

	CItem** lppItemYIndexPastEnd = m_lppItems + m_nSizePerTab * cTab + m_nXSize * (cY + cYSize);
	CItem **lppItemYIndex, **lppItemXIndex, **lppItemXIndexPastEnd;

	for (lppItemYIndex = m_lppItems + m_nSizePerTab * cTab + cY * m_nXSize;
		lppItemYIndex != lppItemYIndexPastEnd; lppItemYIndex += m_nXSize)
	{
		lppItemXIndexPastEnd = lppItemYIndex + cX + cXSize;

		for (lppItemXIndex = lppItemYIndex + cX;
			lppItemXIndex != lppItemXIndexPastEnd; ++lppItemXIndex)
		{
			CItem* lpPreItem = *lppItemXIndex;
			if (NULL != lpPreItem)
			{
				// 스택인 경우
				if (true == lpPreItem->IsSet(DetailData::STACKABLE))
				{
					if (lpPreItem->GetItemInfo().m_usProtoTypeID == usProtoTypeID)
					{
						if (lpPreItem->GetNumOrDurability() + cNum <= lpPreItem->GetMaxNumOrDurability())
						{
							return true;
						}
					}
				}

				ERRLOG4(g_Log, "CID:%10u (%d,%d,%d) 이미 그 장소에 아이템이 있고, 스택할 수도 없습니다.",
					m_dwCID, cX, cY, cTab);
				return false;
			}
		}
	}

	return true;
}

bool CInventoryContainer::RemoveItem(ItemPos itemPos)
{
	CItem* lpItem = GetItem(itemPos);
	if (NULL == lpItem)
	{
		return false;
	}

	if (false == m_bAdminToolFlag)
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
		if (NULL == lpCharacter)
		{
			return false;
		}

		// 퀘스트 트리거 발동
		short wCount = (true == lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() * (-1) : -1;
		lpCharacter->CheckTrigger(Quest::TRIGGER_PICK, lpItem->GetPrototypeID(), Position(), wCount);
	}

	return CArrayContainer::RemoveItem(itemPos);		
}

ItemPos CInventoryContainer::GetBlankPos(unsigned short wProtoTypeID, unsigned char cNum, bool bStack, CItem** ppPrevItem)
{
	Item::ItemPos DstPos;
	DstPos.m_cPos = TakeType::TS_NONE;

	const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(wProtoTypeID);
	if (NULL != lpItemInfo)
	{
		unsigned char cHeight, cWidth, cTab;
		unsigned char cXSize = lpItemInfo->m_DetailData.m_cXSize;
		unsigned char cYSize = lpItemInfo->m_DetailData.m_cYSize;

		// 퀘스트 탭 체크
		unsigned char cItemType = lpItemInfo->m_DetailData.m_cItemType;

		unsigned char cStartTab = 0;
		unsigned char cEndTab = m_nTabNum;
/*
		// edith 2008.06.04 인벤토리 퀘스트 아이템 탭 제거
		unsigned char cEndTab = m_nTabNum - 1;

		if (Item::ItemType::QUEST_ITEM == cItemType)
		{
			cStartTab = m_nTabNum - 1;
			cEndTab = m_nTabNum;
		}
*/
		if (DetailData::STACKABLE == (lpItemInfo->m_DetailData.m_dwFlags & DetailData::STACKABLE) && true == bStack)
		{
			// 스택할수 있는 아이템이 있는가 찾는다.
			for (cTab = cStartTab; cTab < cEndTab; ++cTab)
			{
				for (cHeight = 0; cHeight < m_nYSize; ++cHeight)
				{
					if (cHeight + cYSize> m_nYSize) 
					{
						// YSize가 경계를 벗어나면 다음 탭으로...
						break;
					}

					for (cWidth = 0; cWidth < m_nXSize; ++cWidth)
					{
						if (cWidth + cXSize > m_nXSize) 
						{
							// XSize가 경계를 벗어나면 다음 행으로...
							break;	
						}

						CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cHeight + cWidth];
						if (NULL != lpItem)
						{
							// 장비 아이템은 스택할 수 없다.
							if (false == lpItem->IsSet(DetailData::EQUIP))
							{
								// 이미 아이템이 있을 때, 획득한 아이템이 스택가능한 아이템이고, Max 개가 아니라면,
								// 해당 자리에 스택시킨다.
								if (lpItem->GetItemInfo().m_usProtoTypeID == wProtoTypeID)
								{
									if (lpItem->GetNumOrDurability() + cNum <= lpItem->GetMaxNumOrDurability())
									{
										*ppPrevItem = lpItem;

										DstPos.m_cPos = TakeType::TS_INVEN;
										DstPos.SetPos(cWidth, cHeight, cTab);
										return DstPos;
									}
								}
							}
						}
					}

					// 아이템이 없거나, 다르거나, 스택할수 없으면, 다음 공간을 찾는다.
				}
			}
		}

		// 스택할 수 있는 아이템이 없으므로, 빈 공간을 찾는다.
		for (cTab = cStartTab; cTab < cEndTab; ++cTab)
		{
			for (cHeight = 0; cHeight < m_nYSize; ++cHeight)
			{
				if (cHeight + cYSize> m_nYSize) 
				{
					// YSize가 경계를 벗어나면 다음 탭으로...
					break;
				}

				for (cWidth = 0; cWidth < m_nXSize; ++cWidth)
				{
					if (cWidth + cXSize > m_nXSize) 
					{
						// XSize가 경계를 벗어나면 다음 행으로...
						break;	
					}

					CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cHeight + cWidth];
					if (NULL == lpItem)
					{
						unsigned char cX, cY;
						lpItem = NULL;

						// 아이템이 없는 칸이면 크기를 비교하여 공간이 충분한가 체크
						for (cX = cWidth; cX < cWidth + cXSize; ++cX)
						{
							for (cY = cHeight; cY < cHeight + cYSize; ++cY)
							{
								lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cY + cX];
								if (NULL != lpItem) 
								{
									break;
								}
							}

							if (NULL != lpItem) 
							{
								break;
							}
						}

						if (NULL == lpItem)
						{
							DstPos.m_cPos = TakeType::TS_INVEN;
							DstPos.SetPos(cWidth, cHeight, cTab);
							return DstPos;
						}
					}
				}
			}
		}
	}

	return DstPos;
}

