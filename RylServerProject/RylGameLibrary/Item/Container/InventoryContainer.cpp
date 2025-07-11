
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

		// ����Ʈ Ʈ���� �ߵ�
		short wCount = (true == lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
		lpCharacter->CheckTrigger(Quest::TRIGGER_PICK, lpItem->GetPrototypeID(), Position(), wCount);
	}

	if (NULL == lpItem)
	{
		ERRLOG1(g_Log, "CID:%10u �������� NULL�Դϴ�.", m_dwCID);
		return false;
	}

	unsigned char cX, cY, cTab;
	itemPos.GetPos(cX, cY, cTab);

	// ����Ʈ �� üũ
	unsigned char cItemType = lpItem->GetItemInfo().m_DetailData.m_cItemType;
/*
	// edith 2008.06.04 �κ��丮 ����Ʈ ������ �� ����
	if ((Item::ItemType::QUEST_ITEM == cItemType && ContainerConstant::QUEST_INVENTORY_TAB != cTab) ||
		(Item::ItemType::QUEST_ITEM != cItemType && ContainerConstant::QUEST_INVENTORY_TAB == cTab))
	{
		ERRLOG4(g_Log, "CID:%10u �����ۿ� �´� �ùٸ� ���� �ƴմϴ�. ItemType:%d, Tab:%d/%d", 
			m_dwCID, cItemType, cTab, m_nTabNum);
		return false;
	}
*/
	unsigned char cXSize = lpItem->GetItemInfo().m_DetailData.m_cXSize;
	unsigned char cYSize = lpItem->GetItemInfo().m_DetailData.m_cYSize;

	if (m_nXSize < static_cast<size_t>(cX + cXSize) || 
		m_nYSize < static_cast<size_t>(cY + cYSize) || m_nTabNum <= cTab)
	{
		ERRLOG9(g_Log, "CID:%10u ������ ��ġ �� ũ�Ⱑ �ùٸ��� �ʽ��ϴ�. "
			"X:%d/%d, Y:%d/%d, Tab:%d/%d, Xũ��:%d, Yũ��:%d", 
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
				ERRLOG4(g_Log, "CID:%10u (%d,%d,%d) �̹� �� ��ҿ� �������� �ֽ��ϴ�.",
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
		ERRLOG2(g_Log, "CID:%10u ������ ���� ID�� �����ϴ�. usProtoTypeID:%d",
			m_dwCID, usProtoTypeID);
		return false;
	}

	unsigned char cX, cY, cTab;
	itemPos.GetPos(cX, cY, cTab);

	// ����Ʈ �� üũ
	unsigned char cItemType = lpItemInfo->m_DetailData.m_cItemType;
/*
	// edith 2008.06.04 �κ��丮 ����Ʈ ������ �� ����
	if ((Item::ItemType::QUEST_ITEM == cItemType && ContainerConstant::QUEST_INVENTORY_TAB != cTab) ||
		(Item::ItemType::QUEST_ITEM != cItemType && ContainerConstant::QUEST_INVENTORY_TAB == cTab))
	{
		ERRLOG4(g_Log, "CID:%10u �����ۿ� �´� �ùٸ� ���� �ƴմϴ�. ItemType:%d, Tab:%d/%d", 
			m_dwCID, cItemType, cTab, m_nTabNum);
		return false;
	}
*/
	unsigned char cXSize = lpItemInfo->m_DetailData.m_cXSize;
	unsigned char cYSize = lpItemInfo->m_DetailData.m_cYSize;

	if (m_nXSize < static_cast<size_t>(cX + cXSize) || 
		m_nYSize < static_cast<size_t>(cY + cYSize) || m_nTabNum <= cTab)
	{
		ERRLOG9(g_Log, "CID:%10u ������ ��ġ �� ũ�Ⱑ �ùٸ��� �ʽ��ϴ�. "
			"X:%d/%d, Y:%d/%d, Tab:%d/%d, Xũ��:%d, Yũ��:%d", 
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
				// ������ ���
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

				ERRLOG4(g_Log, "CID:%10u (%d,%d,%d) �̹� �� ��ҿ� �������� �ְ�, ������ ���� �����ϴ�.",
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

		// ����Ʈ Ʈ���� �ߵ�
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

		// ����Ʈ �� üũ
		unsigned char cItemType = lpItemInfo->m_DetailData.m_cItemType;

		unsigned char cStartTab = 0;
		unsigned char cEndTab = m_nTabNum;
/*
		// edith 2008.06.04 �κ��丮 ����Ʈ ������ �� ����
		unsigned char cEndTab = m_nTabNum - 1;

		if (Item::ItemType::QUEST_ITEM == cItemType)
		{
			cStartTab = m_nTabNum - 1;
			cEndTab = m_nTabNum;
		}
*/
		if (DetailData::STACKABLE == (lpItemInfo->m_DetailData.m_dwFlags & DetailData::STACKABLE) && true == bStack)
		{
			// �����Ҽ� �ִ� �������� �ִ°� ã�´�.
			for (cTab = cStartTab; cTab < cEndTab; ++cTab)
			{
				for (cHeight = 0; cHeight < m_nYSize; ++cHeight)
				{
					if (cHeight + cYSize> m_nYSize) 
					{
						// YSize�� ��踦 ����� ���� ������...
						break;
					}

					for (cWidth = 0; cWidth < m_nXSize; ++cWidth)
					{
						if (cWidth + cXSize > m_nXSize) 
						{
							// XSize�� ��踦 ����� ���� ������...
							break;	
						}

						CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cHeight + cWidth];
						if (NULL != lpItem)
						{
							// ��� �������� ������ �� ����.
							if (false == lpItem->IsSet(DetailData::EQUIP))
							{
								// �̹� �������� ���� ��, ȹ���� �������� ���ð����� �������̰�, Max ���� �ƴ϶��,
								// �ش� �ڸ��� ���ý�Ų��.
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

					// �������� ���ų�, �ٸ��ų�, �����Ҽ� ������, ���� ������ ã�´�.
				}
			}
		}

		// ������ �� �ִ� �������� �����Ƿ�, �� ������ ã�´�.
		for (cTab = cStartTab; cTab < cEndTab; ++cTab)
		{
			for (cHeight = 0; cHeight < m_nYSize; ++cHeight)
			{
				if (cHeight + cYSize> m_nYSize) 
				{
					// YSize�� ��踦 ����� ���� ������...
					break;
				}

				for (cWidth = 0; cWidth < m_nXSize; ++cWidth)
				{
					if (cWidth + cXSize > m_nXSize) 
					{
						// XSize�� ��踦 ����� ���� ������...
						break;	
					}

					CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cHeight + cWidth];
					if (NULL == lpItem)
					{
						unsigned char cX, cY;
						lpItem = NULL;

						// �������� ���� ĭ�̸� ũ�⸦ ���Ͽ� ������ ����Ѱ� üũ
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

