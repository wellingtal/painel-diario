#include "stdafx.h"
#include "SendLogPacket.h"
#include "LogDispatch.h"


#include <RylGameLibrary/Item/Item.h>
#include <RylGameLibrary/Log/GameLog.h>
#include <RylGameLibrary/Creature/Character/Character.h>
#include <RylServerLibrary/Network/Stream/SendStream.h>
#include <RylServerLibrary/Network/Packet/PacketCommand.h>
#include <RylServerLibrary/Network/Packet/PacketStruct/ServerPacket.h>
#include <RylServerLibrary/Network/Packet/PacketStruct/GameLogPacket.h>


bool SendLogPacket::ServerLogin(CSendStream& LogSendStream, unsigned long dwServerID)
{
    char* lpBuffer = LogSendStream.GetBuffer(sizeof(PktSL));
    if(NULL != lpBuffer)
    {
        PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

        memset(lpPktSL, 0, sizeof(PktSL));
        lpPktSL->m_dwServerID = dwServerID;

        return LogSendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }

    return false;
}

bool SendLogPacket::DefaultInfo(CSendStream& LogSendStream, CCharacter& Character, 
                                unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
                                unsigned char cCommand, unsigned char cFlags)
{
    char* lpBuffer = LogSendStream.GetBuffer(sizeof(GAMELOG::PACKET::DefaultInfo));
    if(NULL != lpBuffer)
    {
        GAMELOG::PACKET::DefaultInfo* lpDefaultInfo = 
            reinterpret_cast<GAMELOG::PACKET::DefaultInfo*>(lpBuffer);

        lpDefaultInfo->m_dwTime         = time(NULL);

        lpDefaultInfo->m_dwFirstUID     = Character.GetUID();
        lpDefaultInfo->m_dwFirstCID     = Character.GetCID();
        lpDefaultInfo->m_dwGold         = dwGold;
        lpDefaultInfo->m_dwSecondUID    = dwSecondUID;
        lpDefaultInfo->m_dwSecondCID    = dwSecondCID;
        
        const Position& Pos = Character.GetCurrentPos();

        lpDefaultInfo->m_usPosX         = static_cast<unsigned short>(Pos.m_fPointX);
        lpDefaultInfo->m_usPosY         = static_cast<unsigned short>(Pos.m_fPointY);
        lpDefaultInfo->m_usPosZ         = static_cast<unsigned short>(Pos.m_fPointZ);
        lpDefaultInfo->m_cCommand       = cCommand;
        lpDefaultInfo->m_cFlags         = cFlags;

        return LogSendStream.WrapHeader(sizeof(GAMELOG::PACKET::DefaultInfo), 
            GAMELOG::CMD::DefaultInfo, 0, 0);
    }

    return false;
}

bool SendLogPacket::ItemInfo(CSendStream& LogSendStream, CCharacter& Character, Item::CItem& Item,
                             unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
                             unsigned char cCommand, unsigned char cFlags)
{
    char* lpBuffer = LogSendStream.GetBuffer(sizeof(GAMELOG::PACKET::ItemInfo));
    if(NULL != lpBuffer)
    {
        GAMELOG::PACKET::ItemInfo* lpItemInfo =
            reinterpret_cast<GAMELOG::PACKET::ItemInfo*>(lpBuffer);

        lpItemInfo->m_dwTime            = time(NULL);

        lpItemInfo->m_dwFirstUID        = Character.GetUID();
        lpItemInfo->m_dwFirstCID        = Character.GetCID();
        lpItemInfo->m_dwGold            = dwGold;
        lpItemInfo->m_dwSecondUID       = dwSecondUID;
        lpItemInfo->m_dwSecondCID       = dwSecondCID;
        
        const Position& Pos = Character.GetCurrentPos();

        lpItemInfo->m_usPosX            = static_cast<unsigned short>(Pos.m_fPointX);
        lpItemInfo->m_usPosY            = static_cast<unsigned short>(Pos.m_fPointY);
        lpItemInfo->m_usPosZ            = static_cast<unsigned short>(Pos.m_fPointZ);
        lpItemInfo->m_cCommand          = cCommand;
        lpItemInfo->m_cFlags            = cFlags;

        lpItemInfo->m_dwItemUID         = Item.GetUID();
        lpItemInfo->m_usItemProtoTypeID = Item.GetPrototypeID();
        lpItemInfo->m_cItemAmount       = Item.GetNumOrDurability();

        Item::ItemPos itemPos = Item.GetPos();

        lpItemInfo->m_cTakeType         = itemPos.m_cPos;

        switch(itemPos.m_cPos)
        {
			case TakeType::TS_INVEN:
			case TakeType::TS_EXCHANGE:
			case TakeType::TS_DEPOSIT:
			case TakeType::TS_STALL:

				lpItemInfo->m_cItemPosX     = itemPos.GetXIndex();
				lpItemInfo->m_cItemPosY     = itemPos.GetYIndex();
				lpItemInfo->m_cItemPosZ     = itemPos.GetZIndex();
				break;

			case TakeType::TS_EQUIP:
			case TakeType::TS_TEMP:
			case TakeType::TS_EXTRA:

				lpItemInfo->m_cItemPosX     = itemPos.m_cIndex;
				lpItemInfo->m_cItemPosY     = 0;
				lpItemInfo->m_cItemPosZ     = 0;
				break;
        }
            
        lpItemInfo->m_cItemExtra        = 0;

        return LogSendStream.WrapHeader(sizeof(GAMELOG::PACKET::ItemInfo),
            GAMELOG::CMD::ItemInfo, 0, 0);
    }

    return false;
}

bool SendLogPacket::EquipInfo(CSendStream& LogSendStream, CCharacter& Character, Item::CEquipment& Equipment,
                              unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
                              unsigned char cCommand, unsigned char cFlags)
{
    char* lpBuffer = LogSendStream.GetBuffer(sizeof(GAMELOG::PACKET::EquipInfo));
    if(NULL != lpBuffer)
    {
        GAMELOG::PACKET::EquipInfo* lpEquipInfo =
            reinterpret_cast<GAMELOG::PACKET::EquipInfo*>(lpBuffer);

        lpEquipInfo->m_dwTime           = time(NULL);
        
        lpEquipInfo->m_dwFirstUID       = Character.GetUID();
        lpEquipInfo->m_dwFirstCID       = Character.GetCID();
        lpEquipInfo->m_dwGold           = dwGold;
        lpEquipInfo->m_dwSecondUID      = dwSecondUID;
        lpEquipInfo->m_dwSecondCID      = dwSecondCID;
        
        const Position& Pos = Character.GetCurrentPos();

        lpEquipInfo->m_usPosX           = static_cast<unsigned short>(Pos.m_fPointX);
        lpEquipInfo->m_usPosY           = static_cast<unsigned short>(Pos.m_fPointY);
        lpEquipInfo->m_usPosZ           = static_cast<unsigned short>(Pos.m_fPointZ);
        lpEquipInfo->m_cCommand         = cCommand;
        lpEquipInfo->m_cFlags           = cFlags;

        lpEquipInfo->m_dwItemUID         = Equipment.GetUID();
        lpEquipInfo->m_usItemProtoTypeID = Equipment.GetPrototypeID();
        lpEquipInfo->m_cItemAmount       = Equipment.GetNumOrDurability();

        Item::ItemPos itemPos = Equipment.GetPos();

        lpEquipInfo->m_cTakeType        = itemPos.m_cPos;

        switch(itemPos.m_cPos)
        {
			case TakeType::TS_INVEN:
			case TakeType::TS_EXCHANGE:
			case TakeType::TS_DEPOSIT:
			case TakeType::TS_STALL:

				lpEquipInfo->m_cItemPosX    = itemPos.GetXIndex();
				lpEquipInfo->m_cItemPosY    = itemPos.GetYIndex();
				lpEquipInfo->m_cItemPosZ    = itemPos.GetZIndex();
				break;

			case TakeType::TS_EQUIP:
			case TakeType::TS_TEMP:
			case TakeType::TS_EXTRA:

				lpEquipInfo->m_cItemPosX    = itemPos.m_cIndex;
				lpEquipInfo->m_cItemPosY    = 0;
				lpEquipInfo->m_cItemPosZ    = 0;
				break;
        }

        lpEquipInfo->m_cItemExtra       = 0;

        // lpEquipInfo->m_usRuneSocket     = Equipment.GetRuneSocket();     
        lpEquipInfo->m_cUpgradeStep     = Equipment.GetUpgradeLevel();
        lpEquipInfo->m_cMaxSocket       = Equipment.GetMaxSocketNum();
        lpEquipInfo->m_cMaxDurability   = Equipment.GetMaxNumOrDurability();

        Equipment.GetSocket(lpEquipInfo->m_cSocket, GAMELOG::PACKET::MAX_SOCKET_NUM);
        Equipment.GetAttribute(lpEquipInfo->m_usAttribute, GAMELOG::PACKET::MAX_ATTRIBUTE_NUM);

        return LogSendStream.WrapHeader(sizeof(GAMELOG::PACKET::EquipInfo),
            GAMELOG::CMD::EquipInfo, 0, 0);
    }

    return false;
}



void CharLoginout(CCharacter& Character, unsigned char cCommand)
{
    unsigned char cInventoryCommand = 0;
    unsigned char cDepositCommand   = 0;

    switch(cCommand)
    {
    case GAMELOG::CMD::CHAR_LOGIN:

        cInventoryCommand   = GAMELOG::CMD::CHAR_LOGIN_INVENTORY_GOLD;
        cDepositCommand     = GAMELOG::CMD::CHAR_LOGIN_DEPOSIT_GOLD;
        break;

    case GAMELOG::CMD::CHAR_LOGOUT:

        cInventoryCommand   = GAMELOG::CMD::CHAR_LOGOUT_INVENTORY_GOLD;
        cDepositCommand     = GAMELOG::CMD::CHAR_LOGOUT_DEPOSIT_GOLD;
        break;

    default:

        return;
    }

	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        // 캐릭터 소지 아이템 정보를 전부 남긴다.
        // 캐릭터 인벤토리, 창고 소지금 정보를 전부 남긴다.
        
        // 소지 아이템 : 1. Equipment, 2. Inventory, 3. Extra, 4.Exchange, 5. Deposit

        Item::CItem*                    lpItem      = NULL;
        Item::CEquipment*               lpEquipment = NULL;
        Item::CItemContainer::iterator  begin, end;

        Item::CDepositContainer&        Deposit = Character.GetDeposit();

        const int MAX_CONTAINER = 8;
        Item::CItemContainer* lpItemContainers[MAX_CONTAINER] =
        {
            &Character.GetEquipments(),
            &Character.GetInventory(),
            &Character.GetExtra(),
            &Character.GetExchange(),
            Deposit.GetTab(0),
            Deposit.GetTab(1),
            Deposit.GetTab(2),
            Deposit.GetTab(3)
        };
        
        Item::CItemContainer** first = lpItemContainers;
        Item::CItemContainer** last  = lpItemContainers + MAX_CONTAINER;
        
        for(; first != last; ++first)
        {
            Item::CItemContainer* lpItemContainer = *first;

            if(NULL != lpItemContainer)
            {
                begin   = lpItemContainer->begin();
                end     = lpItemContainer->end();

                for(;begin != end; ++begin)
                {
                    Item::CItem* lpItem = *begin;

                    if(NULL != lpItem)
                    {
                        if(lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem))
                        {
                            SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 0, 0, 0, cCommand, 0);
                        }
                        else
                        {
                            SendLogPacket::ItemInfo(SendStream, Character, *lpItem, 0, 0, 0, cCommand, 0);
                        }
                    }
                }
            }
        }

        SendLogPacket::DefaultInfo(SendStream, Character, Character.GetGold(), 0, 0, cInventoryCommand, 0);    // 인벤토리 돈
        SendLogPacket::DefaultInfo(SendStream, Character, Deposit.GetGold(), 0, 0, cDepositCommand, 0);        // 창고 돈
    }
}


void SendLogPacket::CharLogin(CCharacter& Character)    { CharLoginout(Character, GAMELOG::CMD::CHAR_LOGIN);   }
void SendLogPacket::CharLogout(CCharacter& Character)   { CharLoginout(Character, GAMELOG::CMD::CHAR_LOGOUT);  }


void PickupOrDrop(CCharacter& Character, Item::CItem* lpItem, unsigned long dwGold, unsigned char cCommand)
{
    switch(cCommand)
    {
    case GAMELOG::CMD::PICKUP:
    case GAMELOG::CMD::DROP:
        break;

    default:
        return;
    }

	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpItem)
        {
            Item::CEquipment* lpEquipment = NULL; 

            if(lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem))
            {
                SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 0, 0, 0, cCommand, 0);
            }
            else
            {
                SendLogPacket::ItemInfo(SendStream, Character, *lpItem, 0, 0, 0, cCommand, 0);
            }
        }
        else if(0 != dwGold)
        {
            SendLogPacket::DefaultInfo(SendStream, Character, dwGold, 0, 0, cCommand, 0);
        }
    }
}


void SendLogPacket::Pickup(CCharacter& Character, Item::CItem* lpPickupItem, unsigned long dwPickupGold)
{
    PickupOrDrop(Character, lpPickupItem, dwPickupGold, GAMELOG::CMD::PICKUP);
}

void SendLogPacket::Drop(CCharacter& Character, Item::CItem* lpDropItem, unsigned long dwDropGold)
{
    PickupOrDrop(Character, lpDropItem, dwDropGold, GAMELOG::CMD::DROP);
}


void BuyOrSell(CCharacter& Character, unsigned long dwOtherwiseUID, unsigned dwOtherwiseCID, 
               Item::CItem* lpItem, unsigned long dwPrice, unsigned char cCommand)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpItem)
        {
            Item::CEquipment* lpEquipment = NULL; 

            if(lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem))
            {
                SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, dwPrice, 
                    dwOtherwiseUID, dwOtherwiseCID, cCommand, 0);
            }
            else
            {
                SendLogPacket::ItemInfo(SendStream, Character, *lpItem, dwPrice,
                    dwOtherwiseUID, dwOtherwiseCID, cCommand, 0);
            }
        }
    }
}


void SendLogPacket::BuyItem(CCharacter& Buyer, unsigned long dwSellerUID, unsigned dwSellerCID, 
                            Item::CItem* lpBuyItem, unsigned long dwPrice)
{
    BuyOrSell(Buyer, dwSellerUID, dwSellerCID, lpBuyItem, dwPrice, GAMELOG::CMD::BUY_ITEM);
}

void SendLogPacket::SellItem(CCharacter& Seller, unsigned long dwBuyerUID, unsigned dwBuyerCID,
                             Item::CItem* lpSell_Item, unsigned long dwPrice)
{
    BuyOrSell(Seller, dwBuyerUID, dwBuyerCID, lpSell_Item, dwPrice, GAMELOG::CMD::SELL_ITEM);
}


void Exchange(CCharacter& Character, unsigned long dwExchangerUID,
              unsigned long dwExchangerCID, unsigned long dwExchangeGold, unsigned char cCommand)
{
    switch(cCommand)
    {
    case GAMELOG::CMD::BEFORE_EXCHANGE:
    case GAMELOG::CMD::AFTER_EXCHANGE:
        break;

    default:
        return;
    }

	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream&                SendStream  = lpLogDispatch->GetSendStream();
        Item::CExchangeContainer&   Exchange    = Character.GetExchange();
    
        Item::CItemContainer::iterator begin    = Exchange.begin();
        Item::CItemContainer::iterator end      = Exchange.end();
    
        Item::CItem* lpItem = NULL;

        for(; begin != end; ++begin)
        {
            lpItem = *begin;

            if(NULL != lpItem)
            {
                Item::CEquipment* lpEquipment = NULL; 

                if(lpEquipment = Item::CEquipment::DowncastToEquipment(lpItem))
                {
                    SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 0, 
                        dwExchangerUID, dwExchangerCID, cCommand, 0);
                }
                else
                {
                    SendLogPacket::ItemInfo(SendStream, Character, *lpItem, 0,
                        dwExchangerUID, dwExchangerCID, cCommand, 0);
                }
            }
        }

        SendLogPacket::DefaultInfo(SendStream, Character, Exchange.GetGold(), 
            dwExchangerUID, dwExchangerCID, cCommand, 0);
    }
}

void SendLogPacket::BeforeExchange(CCharacter& Character, unsigned long dwExchangerUID,
                                       unsigned long dwExchangerCID, unsigned long dwExchangeGold)
{
    Exchange(Character, dwExchangerUID, dwExchangerCID,
        dwExchangeGold, GAMELOG::CMD::BEFORE_EXCHANGE);
}

void SendLogPacket::AfterExchange(CCharacter& Character, unsigned long dwExchangerUID, 
                                      unsigned long dwExchangerCID, unsigned long dwExchangeGold)
{
    Exchange(Character, dwExchangerUID, dwExchangerCID, 
        dwExchangeGold, GAMELOG::CMD::AFTER_EXCHANGE);
}

void SendLogPacket::ItemUse(CCharacter& Character, Item::CItem* lpUseItem, unsigned long dwUseGold)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpUseItem)
        {
            Item::CEquipment* lpEquipment = NULL;

            if(lpEquipment = Item::CEquipment::DowncastToEquipment(lpUseItem))
            {
                SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 
                    dwUseGold, 0, 0, GAMELOG::CMD::USE_ITEM, 0);
            }
            else
            {
                SendLogPacket::ItemInfo(SendStream, Character, *lpUseItem, 
                    dwUseGold, 0, 0, GAMELOG::CMD::USE_ITEM, 0);
            }
        }
    }    
}


void SendLogPacket::ItemUpgrade(CCharacter& Character, Item::CEquipment* lpEquipment, 
                                Item::CItem* lpMineral, unsigned long dwUseGold)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpEquipment && NULL != lpMineral)
        {
            ItemUse(Character, lpMineral, 0);
            SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 
                dwUseGold, 0, 0, GAMELOG::CMD::UPGRADE_ITEM, 0);
        }
    }
}


void SendLogPacket::InstallSocket(CCharacter& Character, Item::CEquipment* lpEquipment, Item::CItem* lpGems)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpEquipment && NULL != lpGems)
        {
            ItemUse(Character, lpGems, 0);
            SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 
                0, 0, 0, GAMELOG::CMD::INSTALL_SOCKET, 0);
        }
    }
}


void QuestLog(CCharacter& Character, Item::CItem* lpQuestItem, unsigned long dwQuestNPC_CID, 
              unsigned long dwGold, unsigned char cCommand)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpQuestItem)
        {
            Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpQuestItem);

            if(NULL != lpEquipment)
            {
                SendLogPacket::EquipInfo(SendStream, Character, 
                    *lpEquipment, dwGold, 0, dwQuestNPC_CID, cCommand, 0);
            }
            else
            {
                SendLogPacket::ItemInfo(SendStream, Character, 
                    *lpQuestItem, dwGold, 0, dwQuestNPC_CID, cCommand, 0);
            }
        }
        else
        {
            SendLogPacket::DefaultInfo(SendStream, Character, 
                dwGold, 0, dwQuestNPC_CID, cCommand, 0);
        }
    }
}



void SendLogPacket::GetQuestItem(CCharacter& Character, Item::CItem* lpQuestItem, 
                                 unsigned long dwQuestNPC_CID, unsigned long dwGetGold)
{
    QuestLog(Character, lpQuestItem, dwQuestNPC_CID, dwGetGold, GAMELOG::CMD::GET_QUEST_ITEM);
}

void SendLogPacket::RemoveQuestItem(CCharacter& Character, Item::CItem* lpQuestItem, 
                                    unsigned long dwQuestNPC_CID, unsigned long dwRemoveGold)
{
    QuestLog(Character, lpQuestItem, dwQuestNPC_CID, dwRemoveGold, GAMELOG::CMD::REMOVE_QUEST_ITEM);
}

void SendLogPacket::GetQuestReward(CCharacter& Character, Item::CItem* lpQuestReward, 
                                   unsigned long dwQuestNPC_CID, unsigned long dwRewardGold)
{
    QuestLog(Character, lpQuestReward, dwQuestNPC_CID, dwRewardGold, GAMELOG::CMD::GET_QUEST_REWARD);
}



void SendLogPacket::RepairItem(CCharacter& Character, Item::CEquipment* lpEquipment, 
                               unsigned long dwRepairNPC_CID, unsigned long dwUsedGold)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpEquipment)
        {
            SendLogPacket::EquipInfo(SendStream, Character, *lpEquipment, 
                dwUsedGold, 0, dwRepairNPC_CID, GAMELOG::CMD::REPAIR_ITEM, 0);
        }
    }
}

void SendLogPacket::AdminDropItem(CCharacter& Character, unsigned long dwAdminUID, unsigned long dwAdminCID,
                                  Item::CItem* lpAdminItem, unsigned long dwGiveGold)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {
        CSendStream& SendStream = lpLogDispatch->GetSendStream();

        if(NULL != lpAdminItem)
        {
            Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpAdminItem);

            if(NULL != lpEquipment)
            {
                EquipInfo(SendStream, Character, *lpEquipment, dwGiveGold, 
                    dwAdminUID, dwAdminCID, GAMELOG::CMD::ADMIN_DROP, 0);
            }
            else
            {
                ItemInfo(SendStream, Character, *lpAdminItem, dwGiveGold, 
                    dwAdminUID, dwAdminCID, GAMELOG::CMD::ADMIN_DROP, 0);
            }
        }
        else if(0 != dwGiveGold)
        {
            DefaultInfo(SendStream, Character, dwGiveGold, 
                dwAdminUID, dwAdminCID, GAMELOG::CMD::ADMIN_DROP, 0);
        }
    }
}

void SendLogPacket::RentDepositGold(CCharacter& Character, unsigned long dwRentDepositGold)
{
	GET_SINGLE_DISPATCH(lpLogDispatch, CLogDispatch, 
		CLogDispatch::GetDispatchTable());

    if(NULL != lpLogDispatch)
    {        
        DefaultInfo(lpLogDispatch->GetSendStream(), Character, 
            dwRentDepositGold, 0, 0, GAMELOG::CMD::DEPOSIT_RENT_GOLD, 0);
    }
}
