#include "stdafx.h"
#include <Item/Item.h>
#include "ExchangeContainer.h"
#include <Log/LogCommands.h>
#include <Log/ItemLog.h>
#include <Creature/Character/Character.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>

using namespace Item;

Item::CExchangeContainer::CExchangeContainer()
:   m_dwGold(0), m_bLock(false), m_bAccept(false), m_lpOwner(NULL), m_lpExchangeCharacter(NULL)
{


}


Item::CExchangeContainer::~CExchangeContainer()
{

}


Item::CItem* Item::CExchangeContainer::GetItem(Item::ItemPos itemPos) 
{ 
	return Item::CArrayContainer::GetItem(itemPos); 
}

bool Item::CExchangeContainer::SetItem(Item::ItemPos itemPos, Item::CItem* lpItem) 
{ 
	if (false == CheckLock())
	{
		ERRLOG1(g_Log, "CID:0x%10u 락한 상태에서 교환창에 아이템을 올리려 합니다.", m_dwCID);		
	}
	else
	{
		if (Item::CArrayContainer::SetItem(itemPos, lpItem)) 
		{
			if (NULL != m_lpExchangeCharacter)
			{
				CGameClientDispatch* lpGameClientDispatch = m_lpExchangeCharacter->GetDispatcher();
				if (NULL != lpGameClientDispatch)
				{
					GameClientSendPacket::SendCharExchangeItem(lpGameClientDispatch->GetSendStream(), 
						m_lpOwner->GetCID(), 0, GetItem(itemPos), itemPos, false, false);
				}
			}

			return true;
		}
	}

	return false;
}

bool Item::CExchangeContainer::RemoveItem(Item::ItemPos itemPos) 
{ 
	if (false == CheckLock())
	{
		ERRLOG1(g_Log, "CID:0x%10u 락한 상태에서 교환창에 아이템을 내리려 합니다.", m_dwCID);		
		return false;
	}

    if (NULL != m_lpExchangeCharacter)
    {
        CGameClientDispatch* lpGameClientDispatch = m_lpExchangeCharacter->GetDispatcher();
        if (NULL != lpGameClientDispatch)
        {
            GameClientSendPacket::SendCharExchangeItem(lpGameClientDispatch->GetSendStream(), 
                m_dwCID, 0, GetItem(itemPos), itemPos, false, true);
        }
    }

	return Item::CArrayContainer::RemoveItem(itemPos); 
}


bool Item::CExchangeContainer::AddGold(unsigned long dwGold)
{
	if (false == CheckLock()) 
	{
		ERRLOG1(g_Log, "CID:0x%10u 락한 상태에서 교환창에 돈을 올리려 합니다.", m_dwCID);		
	}
    else if(m_dwGold <= ULONG_MAX - dwGold)
    {
	    m_dwGold += dwGold;

        if(NULL != m_lpExchangeCharacter)
        {
            CGameClientDispatch* lpExchangerDispatch = m_lpExchangeCharacter->GetDispatcher();
            if(NULL != lpExchangerDispatch)
            {
                GameClientSendPacket::SendCharExchangeItem(lpExchangerDispatch->GetSendStream(),
                    m_dwCID, m_dwGold, NULL, Item::ItemPos(), false, false);
            }
        }

        return true;
    }
    else
    {
        ERRLOG2(g_Log, "CID:0x%10u 교환 돈 오버플로우가 발생했습니다. : %dGold", m_dwCID, dwGold);
    }

	return false;
}


bool Item::CExchangeContainer::DeductGold(unsigned long dwGold)
{ 
	if (false == CheckLock()) 
	{
		ERRLOG1(g_Log, "CID:0x%10u 락한 상태에서 교환창의 돈을 내리려 합니다.", m_dwCID);		
	}
    else if (m_dwGold < dwGold)
    { 
        ERRLOG1(g_Log, "CID:0x%10u 내리려는 돈의 양이, 현재 보유 금액보다 큽니다.", m_dwCID);		
	}
    else
    {
        m_dwGold -= dwGold; 

        if(NULL != m_lpExchangeCharacter)
        {
            CGameClientDispatch* lpExchangerDispatch = m_lpExchangeCharacter->GetDispatcher();
            if(NULL != lpExchangerDispatch)
            {
                GameClientSendPacket::SendCharExchangeItem(lpExchangerDispatch->GetSendStream(),
                    m_dwCID, m_dwGold, NULL, Item::ItemPos(), false, false);
            }
        }

        return true;
    }

    return false;
}


inline bool Item::CExchangeContainer::CheckLock(void)
{
	if (m_bLock)
    {
		return false;
	}

    if (NULL == m_lpOwner || NULL == m_lpExchangeCharacter) 
    {
		return true;
	}

	if (m_lpExchangeCharacter->GetExchange().GetLock())
	{
        CGameClientDispatch* lpOwnerDispatch        = m_lpOwner->GetDispatcher();
        CGameClientDispatch* lpExchangerDispatch    = m_lpExchangeCharacter->GetDispatcher();

		m_lpExchangeCharacter->GetExchange().SetLock(false);

        if(NULL != lpOwnerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpOwnerDispatch->GetSendStream(), m_lpExchangeCharacter->GetCID(), 
                m_dwCID, PktExC::EXC_UNLOCK, PktExC::NO_SERVER_ERR);
        }

        if(NULL != lpExchangerDispatch)
        {
		    GameClientSendPacket::SendCharExchangeCmd(lpExchangerDispatch->GetSendStream(), m_lpExchangeCharacter->GetCID(), 
                m_dwCID, PktExC::EXC_UNLOCK, PktExC::NO_SERVER_ERR);
        }
	}

	return true;
}



bool Item::CExchangeContainer::ExchangeOK(bool bOK)
{
    bool bResult = true;

	if (NULL == m_lpExchangeCharacter) 
	{
        bResult = false;
		ERRLOG1(g_Log, "CID:0x%10u 거래하는 캐릭터가 없는데 교환이 성립되었습니다.", m_dwCID);
	}
    else
    {
        Item::CExchangeContainer& ExchangerContainer = m_lpExchangeCharacter->GetExchange();

        if (bOK)
	    {
            unsigned long dwExchangerCID = m_lpExchangeCharacter->GetCID();

		    // 교환 전 로그.
		    GAMELOG::LogExchangeItem(*m_lpOwner, dwExchangerCID, *this, GAMELOG::CMD::BEFORE_EXCHANGE_ITEM);
		    GAMELOG::LogExchangeItem(*m_lpExchangeCharacter, m_dwCID, ExchangerContainer, GAMELOG::CMD::BEFORE_EXCHANGE_ITEM);

            // 교환
            std::swap(m_lppItems, ExchangerContainer.m_lppItems);		    
		    std::swap(m_dwGold, ExchangerContainer.m_dwGold);

		    // 교환 후 로그.
		    GAMELOG::LogExchangeItem(*m_lpOwner, dwExchangerCID, *this, GAMELOG::CMD::AFTER_EXCHANGE_ITEM);
		    GAMELOG::LogExchangeItem(*m_lpExchangeCharacter, m_dwCID, ExchangerContainer, GAMELOG::CMD::AFTER_EXCHANGE_ITEM);
        }

	    m_lpExchangeCharacter->AddGold(ExchangerContainer.GetGold(), false);
	    ExchangerContainer.Clear();

	    m_lpExchangeCharacter->DBUpdateForce(DBUpdateData::UPDATE);
    }

	m_lpOwner->AddGold(m_dwGold, false);
	Clear();

    m_lpOwner->DBUpdateForce(DBUpdateData::UPDATE);
	return bResult;
}


void Item::CExchangeContainer::ExchangeCancel(void)
{
	if (NULL == m_lpExchangeCharacter) 
    {
		return;
	}

	CGameClientDispatch*        lpOwnerDispatch             = m_lpOwner->GetDispatcher();
    CGameClientDispatch*        lpExchangerDispatch         = m_lpExchangeCharacter->GetDispatcher();

    unsigned long                       m_dwExchangerCID            = m_lpExchangeCharacter->GetCID();    
    Item::CExchangeContainer&   ExchangerExchageContainer   = m_lpExchangeCharacter->GetExchange();

	if (m_bAccept)
	{
		m_bAccept = false;

        if(NULL != lpOwnerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpOwnerDispatch->GetSendStream(), 
                m_dwCID, m_dwExchangerCID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);
        }

        if(NULL != lpExchangerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpExchangerDispatch->GetSendStream(), 
                m_dwCID, m_dwExchangerCID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);
        }
	}

	if (ExchangerExchageContainer.GetAccept())
	{
		ExchangerExchageContainer.SetAccept(false);

        if(NULL != lpOwnerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpOwnerDispatch->GetSendStream(),
                m_dwExchangerCID, m_dwCID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);
        }

        if(NULL != lpExchangerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpExchangerDispatch->GetSendStream(), 
                m_dwExchangerCID, m_dwCID, PktExC::EXC_CANCEL, PktExC::NO_SERVER_ERR);
        }
	}
}


bool Item::CExchangeContainer::Initialize(CCharacter* lpCharacter, 
                                          unsigned char cExchangeWidth, unsigned char cExchangeHeight)
{
    m_lpOwner = lpCharacter;
    return CArrayContainer::Initialize(lpCharacter->GetCID(), cExchangeWidth, cExchangeHeight, 1);
}
