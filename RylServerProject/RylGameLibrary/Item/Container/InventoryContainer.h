#ifndef _INVENTORY_CONTAINER_H_
#define _INVENTORY_CONTAINER_H_

#include "ItemContainer.h"

namespace Item
{    
    class CItem;

	class CInventoryContainer : public CArrayContainer
    {
    public:

        CInventoryContainer();
        virtual ~CInventoryContainer();

		ItemPos GetBlankPos(unsigned short wProtoTypeID, unsigned char cNum, bool bStack, CItem** ppPrevItem);
		bool TestItem(ItemPos itemPos, unsigned short usProtoTypeID, unsigned char cNum); 

		virtual bool SetItem(ItemPos itemPos, CItem* lpItem);
		virtual bool RemoveItem(ItemPos itemPos);

		void SetAdminTool(void)		{ m_bAdminToolFlag = true;	}


	private:

		bool	m_bAdminToolFlag;
	};
};

#endif