#include "stdafx.h"

#include <Pattern/Singleton.h>
#include <Utility/Math/Math.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "ItemContainer.h"

namespace Item
{
    class CNullItem : public CItem, public CSingleton<CNullItem>
    {
    public:
        virtual ~CNullItem() { }
    private:

        CNullItem(const ItemInfo& itemInfo) : CItem(itemInfo) { }
        
        static ItemInfo  ms_thisiteminfo;
        static CNullItem ms_this;
    };
};

Item::ItemInfo      Item::CNullItem::ms_thisiteminfo(0xFFFF);        
Item::CNullItem     Item::CNullItem::ms_this(Item::CNullItem::ms_thisiteminfo);


size_t LogErrorItem(unsigned long dwCID, const char* szDetail, const char* szBuffer)
{
    const int   MIN_BUFFER = 64;
    char        szItemUID[MIN_BUFFER];

    const Item::ItemData* lpItemData = reinterpret_cast<const Item::ItemData*>(szBuffer);    
    Math::Convert::Hex64ToStr(szItemUID, lpItemData->m_dwUID);

    ERRLOG4(g_Log, "CID:%10u %s. UID:%s, ProtoTypeID:%d", 
        dwCID, szDetail, szItemUID, lpItemData->m_usProtoTypeID);

    return lpItemData->m_cItemSize;
}



// ----------------------------------------------------------------------------------------
// CItemContainer

Item::CItemContainer::CItemContainer()
:   m_dwCID(0), m_nMaxSize(0), m_lpNullItem(&CNullItem::GetInstance()), 
    m_lppItems(NULL), m_usFlags(0)
{

}

Item::CItemContainer::~CItemContainer()
{
    Destroy();
}


bool Item::CItemContainer::Initialize(unsigned long dwCID, unsigned short nMaxSize)
{    
    Destroy();

    m_dwCID     = dwCID;
    m_nMaxSize  = nMaxSize;

    m_lppItems  = new CItem*[m_nMaxSize];
    if(NULL != m_lppItems)
    {
        std::fill_n(m_lppItems, m_nMaxSize, reinterpret_cast<CItem*>(NULL));
        return true;
    }

    return false;
}

void Item::CItemContainer::ClearItems()
{
    if(NULL != m_lppItems)
    {
        CItem** lppItem         = m_lppItems;
        CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;

		// edith 2010.01.23 ������ �����̳� ���� ���� ����. 
//      for (; lppItem != lppItemPastEnd; ++lppItem)
        for (int i = 0; i < m_nMaxSize; ++i, ++lppItem)
        {
            CItem* lpItem = *lppItem;
            if(NULL != lpItem && m_lpNullItem != lpItem)
            {
                DELETE_ITEM(lpItem);                
            }            
        }

        std::fill_n(m_lppItems, m_nMaxSize, reinterpret_cast<CItem*>(NULL));
    }
}


void Item::CItemContainer::Destroy()
{
    if(NULL != m_lppItems)
    {
        ClearItems();
 
        delete [] m_lppItems;
        m_lppItems = NULL;
    }
}


void Item::CItemContainer::DumpItemInfo()
{
    const int MIN_BUFFER = 32;
    const int MAX_BUFFER = 256;
    char szUID[MIN_BUFFER];
    char szBuffer[MAX_BUFFER];

    CItem** lppItem         = m_lppItems;
    CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;
    
	// edith 2010.01.23 ������ �����̳� ���� ���� ����. 
//  for (; lppItem != lppItemPastEnd; ++lppItem)
    for (int i = 0; i < m_nMaxSize; ++i, ++lppItem)
    {
        const CItem* lpItem = *lppItem;

        if(NULL != lpItem && m_lpNullItem != lpItem)
        {
            Math::Convert::Hex64ToStr(szUID, lpItem->GetUID());
                        
            const ItemPos itemPos = lpItem->GetPos();
            const DetailData& detailData = lpItem->GetItemInfo().m_DetailData;

            unsigned char cX, cY, cTab;
            itemPos.GetPos(cX, cY, cTab);

            _snprintf(szBuffer, MAX_BUFFER, "CID:%10u �� �������Դϴ�. UID: %s ����ID : %5d,"
                " ��ǥ:(%d - %2d,%2d,%2d), ũ��:(%2d, %2d), ���� ����(������):%2d",
		        m_dwCID, szUID, lpItem->GetPrototypeID(), itemPos.m_cIndex, cX, cY, cTab,
                detailData.m_cXSize, detailData.m_cXSize, lpItem->GetNumOrDurability());

            ERRLOG1(g_Log, "%s", szBuffer);
        }
    }
}

void Item::CItemContainer::DumpMoneyInfo(unsigned long dwDepositMoney)
{
}


bool Item::CItemContainer::SerializeOut(char* szItemBuffer_Out, unsigned long& dwBufferSize_InOut) const
{
    CItem** lppItem         = m_lppItems;
    CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;
    
    size_t nRemainBufferSize    = static_cast<size_t>(dwBufferSize_InOut);
    size_t nWritten             = 0;

	// edith 2010.01.23 ������ �����̳� ���� ���� ����. 
//  for (; lppItem != lppItemPastEnd; ++lppItem)
    for (int i = 0; i < m_nMaxSize; ++i, ++lppItem)
    {
        CItem* lpItem = *lppItem;
        if(NULL != lpItem && m_lpNullItem != lpItem)
        {
            size_t nItemSize = nRemainBufferSize;

			// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
			lpItem->RevisionLimit();

            if(!lpItem->SerializeOut(szItemBuffer_Out + nWritten, nItemSize))
            {
                ERRLOG3(g_Log, "CID:%10u ������ SerializeOut�� �����߽��ϴ�. ���۰� �����մϴ�. �� ũ��:%d, ���� ���� ũ��:%d",
                    m_dwCID, nWritten, nRemainBufferSize);
                return false;
            }

            nWritten += nItemSize;
            nRemainBufferSize -= nItemSize;
        }
    }
    
    dwBufferSize_InOut = static_cast<unsigned long>(nWritten);
    return true;
}


// ----------------------------------------------------------------------------------------
// CArrayContainer

Item::CArrayContainer::CArrayContainer()
{
        
}

Item::CArrayContainer::~CArrayContainer()
{

}

bool Item::CArrayContainer::Initialize(unsigned long dwCID, unsigned char nXSize, unsigned char nYSize, unsigned char nTabNum)
{
    m_dwCID     = dwCID;
    m_nXSize    = nXSize;
    m_nYSize    = nYSize;
    m_nTabNum   = nTabNum;
    
    m_nSizePerTab   = nXSize * nYSize;
    return CItemContainer::Initialize(dwCID, nXSize * nYSize * nTabNum);
}

Item::CItem* Item::CArrayContainer::GetItem(Item::ItemPos itemPos)
{
    unsigned char cX, cY, cTab;
    itemPos.GetPos(cX, cY, cTab);

    if(m_nXSize <= cX || m_nYSize <= cY || m_nTabNum <= cTab)
    {
        ERRLOG7(g_Log, "CID:%10u ������ ��ġ�� �ùٸ��� �ʽ��ϴ�. X:%d/%d, Y:%d/%d, Tab:%d/%d", 
            m_dwCID, cX, m_nXSize, cY, m_nYSize, cTab, m_nTabNum);
        return NULL;
    }

    CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cY + cX];
    return  (lpItem != m_lpNullItem) ? lpItem : NULL;
}

bool Item::CArrayContainer::SetItem(Item::ItemPos itemPos, Item::CItem* lpItem)
{
    if(NULL == lpItem)
    {
        ERRLOG1(g_Log, "CID:%10u �������� NULL�Դϴ�.", m_dwCID);
        return false;
    }

    unsigned char cX, cY, cTab;
    itemPos.GetPos(cX, cY, cTab);

    unsigned char cXSize = lpItem->GetItemInfo().m_DetailData.m_cXSize;
    unsigned char cYSize = lpItem->GetItemInfo().m_DetailData.m_cYSize;

    if(m_nXSize < static_cast<size_t>(cX + cXSize) || 
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
            if(0 != *lppItemXIndex)
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

bool Item::CArrayContainer::RemoveItem(Item::ItemPos itemPos)
{
    unsigned char cX, cY, cTab;
    itemPos.GetPos(cX, cY, cTab);

    if(m_nXSize <= cX || m_nYSize <= cY || m_nTabNum <= cTab)
    {
        ERRLOG7(g_Log, "CID:%10u ������ ��ġ �� ũ�Ⱑ �ùٸ��� �ʽ��ϴ�. X:%d/%d, Y:%d/%d, Tab:%d/%d",
            m_dwCID, cX, m_nXSize, cY, m_nYSize, cTab, m_nTabNum);
        return false;
    }

    CItem* lpItem = m_lppItems[m_nSizePerTab * cTab + m_nXSize * cY + cX];
    if(NULL == lpItem || m_lpNullItem == lpItem)
    {
        ERRLOG4(g_Log, "CID:%10u �� ��ġ�� �������� �����ϴ�. X:%d, Y:%d, Tab:%d", m_dwCID, cX, cY, cTab);
        return false;
    }

    unsigned char cXSize = lpItem->GetItemInfo().m_DetailData.m_cXSize;
    unsigned char cYSize = lpItem->GetItemInfo().m_DetailData.m_cYSize;
 
    if(m_nXSize < static_cast<size_t>(cX + cXSize) || 
        m_nYSize < static_cast<size_t>(cY + cYSize))
    {        
        ERRLOG9(g_Log, "CID:%10u ������ ��ġ �� ũ�Ⱑ �ùٸ��� �ʽ��ϴ�. "
            "X:%d/%d, Y:%d/%d, Tab:%d/%d, Xũ��:%d, Yũ��:%d", 
            m_dwCID, cX, m_nXSize, cY, m_nYSize, cTab, m_nTabNum, cXSize, cYSize);
        return false;        
    }

    CItem** lppItemYIndexPastEnd  = m_lppItems + m_nSizePerTab * cTab + m_nXSize * (cY + cYSize);    
    CItem **lppItemYIndex, **lppItemXIndex, **lppItemXIndexPastEnd;

    for (lppItemYIndex = m_lppItems + m_nSizePerTab * cTab + cY * m_nXSize;
        lppItemYIndex != lppItemYIndexPastEnd; lppItemYIndex += m_nXSize)
    {
        lppItemXIndexPastEnd = lppItemYIndex + cX + cXSize;

        for (lppItemXIndex = lppItemYIndex + cX;
            lppItemXIndex != lppItemXIndexPastEnd; ++lppItemXIndex)
        {
            *lppItemXIndex = NULL;
        }
    }

    return true;
}

unsigned short Item::CArrayContainer::GetItemNum(unsigned short usProtoTypeID)
{
	unsigned short wCount = 0;

	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
				const CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];
				if (NULL == lpItem) { continue; }

				if (usProtoTypeID == lpItem->GetPrototypeID())
				{
					wCount += (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
				}
			}
		}
	}

	return wCount;
}

bool Item::CArrayContainer::DisappearItem(unsigned short wItemID, unsigned short wItemNum, 
										  vector<Item::ItemGarbage>& vecItemGarbage)
{
	unsigned short tempItemNum = GetItemNum(wItemID);
	if (wItemNum > tempItemNum) 
	{ 
		DETLOG3(g_Log, "DisappearItem = �ش� �������� �������� �ʽ��ϴ�. ItemID:%d, ItemNum:%d/%d", wItemID, tempItemNum, wItemNum);
		return false; 
	}

	// ������ �ɼ� �����Ƿ� short ������ ��ȯ
	short itemNum = static_cast<short>(wItemNum);
	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
				CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];
				if (NULL == lpItem) { continue; }

				if (wItemID == lpItem->GetPrototypeID())
				{
                    itemNum -= (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;

					if (0 < itemNum)
					{
						vecItemGarbage.push_back(ItemGarbage(lpItem, 0));
					}
					else if (0 == itemNum)
					{
						vecItemGarbage.push_back(ItemGarbage(lpItem, 0));
						return true;
					}
					else if (0 > itemNum)
					{
						unsigned char cRemainNum = static_cast<unsigned char>( -itemNum );
						vecItemGarbage.push_back(ItemGarbage(lpItem, cRemainNum));
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Item::CArrayContainer::DisappearItem(unsigned short wItemID, unsigned short wItemNum)
{
	if (wItemNum > GetItemNum(wItemID)) 
	{ 
		return false; 
	}

	// ������ �ɼ� �����Ƿ� short ������ ��ȯ
	short itemNum = static_cast<short>(wItemNum);
	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
				CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];
				if (NULL == lpItem) { continue; }

				if (wItemID == lpItem->GetPrototypeID())
				{
					itemNum -= (lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;

					CCharacter* lpOwner = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
					if (NULL == lpOwner) { return false; }
					CGameClientDispatch* lpDispatch = lpOwner->GetDispatcher();

					if (0 < itemNum)
					{
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharDisappearItem(lpDispatch->GetSendStream(), 
								m_dwCID, lpItem->GetPos(), 0, PktDisappearItem::DIC_LINKED_QUEST, PktBase::NO_SERVER_ERR);
						}

						RemoveItem(lpItem->GetPos());
						DELETE_ITEM(lpItem);
					}
					else if (0 == itemNum)
					{
						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharDisappearItem(lpDispatch->GetSendStream(), 
								m_dwCID, lpItem->GetPos(), 0, PktDisappearItem::DIC_LINKED_QUEST, PktBase::NO_SERVER_ERR);
						}

						RemoveItem(lpItem->GetPos());
						DELETE_ITEM(lpItem);

						return true;
					}
					else if (0 > itemNum)
					{
						unsigned char cRemainNum = static_cast<unsigned char>( -itemNum );

						if (NULL != lpDispatch)
						{
							GameClientSendPacket::SendCharDisappearItem(lpDispatch->GetSendStream(), 
								m_dwCID, lpItem->GetPos(), cRemainNum, PktDisappearItem::DIC_LINKED_QUEST, PktBase::NO_SERVER_ERR);
						}

						lpItem->SetNumOrDurability(cRemainNum);

						return true;
					}
				}
			}
		}
	}

	return false;
}

void Item::CArrayContainer::DumpItemInfo()
{
	const int MAX_LINE = 1024;
	char szLine[MAX_LINE];

	int	nBufferPos = 0;

	ERRLOG1(g_Log, "CID:%10u �� ������ �α׸� �����մϴ�.", m_dwCID);

	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		ERRLOG2(g_Log, "CID:%10u �� �� %d�� �������Դϴ�.", m_dwCID, nTab);

		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			nBufferPos = _snprintf(szLine, MAX_LINE, "CID:%10u �� �������Դϴ�. ", m_dwCID);

			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
                const CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];

                nBufferPos += _snprintf(szLine + nBufferPos, MAX_LINE, " %5d ", 
                    ((0 != lpItem) ? lpItem->GetPrototypeID() : 0));
			}
			
			ERRLOG0(g_Log, szLine);
		}
	}

    CItemContainer::DumpItemInfo();
}

void Item::CArrayContainer::DumpMoneyInfo(unsigned long dwDepositMoney)
{
	CItemContainer::DumpMoneyInfo(dwDepositMoney);
}


bool Item::CArrayContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
    size_t nBufferSize = static_cast<size_t>(dwBufferSize_In);
    size_t nUsed = 0;

    Item::CItemFactory& ItemFactory = CItemFactory::GetInstance();

    while(nBufferSize > 0)
    {
        size_t nItemSize = nBufferSize;
        Item::CItem* lpItem = ItemFactory.CreateItem(szItemBuffer_In + nUsed, nItemSize);

		// edith 2008.08.26 ������ ��ũ��Ʈ���ش� �������� ������ �������� ������ ������.
        if(NULL == lpItem)
        {
            nItemSize = LogErrorItem(m_dwCID, "������ ������ �����߽��ϴ�", szItemBuffer_In + nUsed);
			break;	// �ּ�..
        }
        else if(!CArrayContainer::SetItem(lpItem->GetPos(), lpItem))
        {
            nItemSize = LogErrorItem(m_dwCID, "������ ���⸦ �����߽��ϴ�", szItemBuffer_In + nUsed);
            DELETE_ITEM(lpItem);
			break;
        }

        nUsed += nItemSize;
        nBufferSize -= nItemSize;        
    }
    
    return true;
}


// ----------------------------------------------------------------------------------------
// CListContainer

Item::CListContainer::CListContainer()
{

}


Item::CListContainer::~CListContainer()
{

}


Item::CItem* Item::CListContainer::GetItem(Item::ItemPos itemPos)
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {
        CItem* lpItem = m_lppItems[itemPos.m_cIndex];
        return (lpItem != m_lpNullItem) ? lpItem : NULL;
    }

    return NULL;
}


bool Item::CListContainer::SetItem(Item::ItemPos itemPos, Item::CItem* lpItem)
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {        
        if(NULL == m_lppItems[itemPos.m_cIndex])
        {
            m_lppItems[itemPos.m_cIndex] = lpItem;
            lpItem->MoveItem(itemPos);
            return true;
        }
        else
        {
            ERRLOG1(g_Log, "CID:%10u �������� �̹� �����մϴ�.", m_dwCID);
        }
    }

    return false;
}


bool Item::CListContainer::RemoveItem(Item::ItemPos itemPos)//hzhz
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {        
        if(NULL != m_lppItems[itemPos.m_cIndex])
        {            
            m_lppItems[itemPos.m_cIndex] = NULL;
            return true;
        }
        else
        {
            ERRLOG1(g_Log, "CID:%10u �������� �������� �ʽ��ϴ�.", m_dwCID);
        }
    }

    return false;
}


bool Item::CListContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
    size_t nBufferSize = static_cast<size_t>(dwBufferSize_In);
    size_t nUsed = 0;

    Item::CItemFactory& ItemFactory = CItemFactory::GetInstance();

    while(nBufferSize > 0)
    {
        size_t nItemSize = nBufferSize;
        Item::CItem* lpItem = ItemFactory.CreateItem(szItemBuffer_In + nUsed, nItemSize);

        if(NULL == lpItem)
        {
            nItemSize = LogErrorItem(m_dwCID, "������ ������ �����߽��ϴ�", szItemBuffer_In + nUsed);
        }
        else if(!CListContainer::SetItem(lpItem->GetPos(), lpItem))
        {
            nItemSize = LogErrorItem(m_dwCID, "������ ���⸦ �����߽��ϴ�", szItemBuffer_In + nUsed);
            DELETE_ITEM(lpItem);
        }

        nUsed += nItemSize;
        nBufferSize -= nItemSize;        
    }
    
    return true;
}


// ----------------------------------------------------------------------------------------
// CSEQListContainer

Item::CSEQListContainer::CSEQListContainer() : m_ListCount(0)
{

}


Item::CSEQListContainer::~CSEQListContainer()
{

}

Item::CItem* Item::CSEQListContainer::GetItem(ItemPos itemPos)
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {
        CItem* lpItem = m_lppItems[itemPos.m_cIndex];
        return (lpItem != m_lpNullItem) ? lpItem : NULL;
    }

    return NULL;
}


bool Item::CSEQListContainer::SetItem(ItemPos itemPos, Item::CItem* lpItem)
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {        
        if(NULL == m_lppItems[itemPos.m_cIndex])
        {
            m_lppItems[itemPos.m_cIndex] = lpItem;
            return true;
        }
        else
        {
            ERRLOG1(g_Log, "CID:%10u �������� �̹� �����մϴ�.", m_dwCID);
        }
    }

    return false;
}


bool Item::CSEQListContainer::RemoveItem(Item::ItemPos itemPos)
{
    if(itemPos.m_cIndex < m_nMaxSize)
    {        
        if(NULL != m_lppItems[itemPos.m_cIndex])
        {            
            m_lppItems[itemPos.m_cIndex] = NULL;
            return true;
        }
        else
        {
            ERRLOG1(g_Log, "CID:%10u �������� �������� �ʽ��ϴ�.", m_dwCID);
        }
    }

    return false;
}

bool Item::CSEQListContainer::SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In)
{
    size_t nBufferSize = static_cast<size_t>(dwBufferSize_In);
    size_t nUsed = 0;

    Item::CItemFactory& ItemFactory = CItemFactory::GetInstance();

	m_ListCount = 0;
    while(nBufferSize > 0)
    {
        size_t nItemSize = nBufferSize;

		// ȭ��/��Ʈ
		unsigned short ProtoTypeID = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_usProtoTypeID;
		if(ProtoTypeID >= 4001 && ProtoTypeID <= 4033)
		{
			nItemSize = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_cItemSize;
			if(20 != nItemSize)
			{
				ERRLOG1(g_Log, "CID:%10u �̻��� ȭ��. ������ ������ �����߽��ϴ�.", m_dwCID);
	 			
				nUsed += nItemSize;
				nBufferSize -= nItemSize;        
				continue;
			}
		}
		
		Item::CItem* lpItem = ItemFactory.CreateItem(szItemBuffer_In + nUsed, nItemSize);

        if(NULL == lpItem)
        {
            nItemSize = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_cItemSize;
            ERRLOG1(g_Log, "CID:%10u ������ ������ �����߽��ϴ�.", m_dwCID);
        }
        else if(!CSEQListContainer::SetItem(ItemPos(0, m_ListCount), lpItem))
        {
            nItemSize = reinterpret_cast<const Item::ItemData*>(szItemBuffer_In + nUsed)->m_cItemSize;
            ERRLOG1(g_Log, "CID:%10u ������ ���⸦ �����߽��ϴ�.", m_dwCID);
            DELETE_ITEM(lpItem);
        }

        nUsed += nItemSize;
        nBufferSize -= nItemSize;        

		++m_ListCount;
	}
    
    return true;
}
