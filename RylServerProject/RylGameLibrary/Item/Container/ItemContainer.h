#ifndef _ITEM_CONTAINER_H_
#define _ITEM_CONTAINER_H_

#include <vector>
#include <Item/ItemStructure.h>

// 전방 참조
namespace Item
{    
    class CItem;

    class CItemContainer
    {
    public:

        CItemContainer();
        virtual ~CItemContainer();

		bool Initialize(unsigned long dwCID, unsigned short nMaxSize);
        void SetCID(unsigned long dwCID) { m_dwCID = dwCID; }
        void ClearItems();

        virtual CItem* GetItem(ItemPos itemPos) = 0;
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem) = 0;

        virtual bool RemoveItem(ItemPos itemPos) = 0;
        
        virtual bool SerializeOut(char* szItemBuffer_Out, unsigned long& dwBufferSize_InOut) const; 
        virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_InOut) = 0;

        virtual void DumpItemInfo();
		virtual void DumpMoneyInfo(unsigned long dwDepoistMoney);
		
        class iterator
        {
        public:

            iterator(CItem** lppItem, CItem* lpNull_Item, unsigned short usOffset) : m_lppItems(lppItem), m_lpNullItem(lpNull_Item), m_nCurrentPos(usOffset) { }
            iterator() : m_lppItems(NULL), m_lpNullItem(NULL), m_nCurrentPos(0) { }

            CItem* operator*() { return (m_lpNullItem != *(m_lppItems + m_nCurrentPos)) ? *(m_lppItems + m_nCurrentPos) : NULL; }
            iterator& operator++() { ++m_nCurrentPos; return (*this); }

            bool operator==(const iterator& _Right) const
            {	// test for iterator equality
                return (m_nCurrentPos == _Right.m_nCurrentPos && m_lppItems == _Right.m_lppItems
                    && m_lpNullItem == _Right.m_lpNullItem);
            }

            bool operator!=(const iterator& _Right) const
            {	// test for iterator inequality
                return (!(*this == _Right));
            }

        private:

            CItem**         m_lppItems;
            CItem*          m_lpNullItem;
            unsigned short  m_nCurrentPos;
        };

        iterator begin() const { return iterator(m_lppItems, m_lpNullItem, 0); }
        iterator end()   const { return iterator(m_lppItems, m_lpNullItem, m_nMaxSize); }
        
    protected:
        
        virtual void Destroy();

        CItem**         m_lppItems;
        CItem*          m_lpNullItem;

        unsigned long   m_dwCID;
        unsigned short  m_nMaxSize;
        unsigned short  m_usFlags;

        friend class iterator;
    };
    
    class CArrayContainer : public CItemContainer
    {
    public:

        CArrayContainer();
        virtual ~CArrayContainer();

        bool Initialize(unsigned long dwCID, unsigned char nXSize, unsigned char nYSize, unsigned char nTabNum);

        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);
		virtual unsigned short GetItemNum(unsigned short usProtoTypeID);

        virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);

        virtual void DumpItemInfo();
		virtual void DumpMoneyInfo(unsigned long dwDepoistMoney);

		bool DisappearItem(unsigned short wItemID, unsigned short wItemNum, std::vector<Item::ItemGarbage>& vecItemGarbage);
		bool DisappearItem(unsigned short wItemID, unsigned short wItemNum);

	protected:        
        
        unsigned char  m_nXSize;
        unsigned char  m_nYSize;
        unsigned char  m_nTabNum;
        unsigned char  m_nSizePerTab;
    };


	class CListContainer : public CItemContainer
    {
    public:

        CListContainer();
        virtual ~CListContainer();

        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);        

        virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);
    };

	class CSEQListContainer : public CItemContainer
    {
    public:

        CSEQListContainer();
        virtual ~CSEQListContainer();

        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);        

        virtual bool SerializeIn(const char* szItemBuffer_In, unsigned long dwBufferSize_In);

	private:
		unsigned short m_ListCount;

	public:
		unsigned short GetListCount(void)	{	return m_ListCount;		}
	};
};

#endif