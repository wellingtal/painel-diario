#ifndef _EXCHANGE_CONTAINER_H_
#define _EXCHANGE_CONTAINER_H_

#include "ItemContainer.h"

// 전방 참조
class CCharacter;

namespace Item
{
	class CExchangeContainer : public CArrayContainer
    {
    public:

        CExchangeContainer();
        virtual ~CExchangeContainer();

        bool Initialize(CCharacter* lpCharacter, unsigned char cExchangeWidth, unsigned char cExchangeHeight);

        virtual CItem* GetItem(ItemPos itemPos);
        virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
        virtual bool RemoveItem(ItemPos itemPos);        
        		
		void SetExchangeCharacter(CCharacter *pCharacter) { m_lpExchangeCharacter = pCharacter; }
		CCharacter* GetExchangeCharacter(void) { return m_lpExchangeCharacter; }

		bool ExchangeOK(bool bOK);
		void ExchangeCancel(void);

		bool AddGold(unsigned long dwGold);        
		bool DeductGold(unsigned long dwGold);
        unsigned long GetGold() const { return m_dwGold; }

		void SetLock(bool bLock) { m_bLock = bLock; }
		bool GetLock(void) { return m_bLock; }
		void SetAccept(bool bAccept) { m_bAccept = bAccept; }
		bool GetAccept(void) { return m_bAccept; }

	protected:

		inline bool CheckLock();
        inline void Clear();

        CCharacter*     m_lpOwner;
        CCharacter*		m_lpExchangeCharacter;
		unsigned long	m_dwGold;

		bool	        m_bLock;
		bool	        m_bAccept;
        unsigned char   m_cPadding[2];  // 4 byte alignment...
    };
};


void Item::CExchangeContainer::Clear()
{
	m_dwGold = 0;
	m_lpExchangeCharacter = NULL;
	m_bLock = false;
	m_bAccept = false;
}


#endif