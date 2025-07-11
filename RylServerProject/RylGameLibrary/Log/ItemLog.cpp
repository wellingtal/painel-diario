
#include "stdafx.h"

#include <Creature/Character/Character.h>
#include <Item/Container/ExchangeContainer.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include "ItemLog.h"
#include "GameLog.h"
#include "LogStruct.h"
#include "LogCommands.h"


void GAMELOG::LogMoveItem(const CCharacter& character, const TakeType takeType, 
                          const sMinItemInfo& minItemInfo, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	MoveItem Log
	sMoveItemLog* lpMoveItemLog = reinterpret_cast<sMoveItemLog*>(gameLog.ReserveBuffer(
        static_cast<unsigned short>(sizeof(sMoveItemLog) + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpMoveItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sMoveItemLog));
		lpMoveItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
            gameLog.GetLogTime(), CMD::MOVE_ITEM, static_cast<ERRType>(eError));

		lpMoveItemLog->InitMoveItemLog(takeType, minItemInfo);
		
		if (0 != eError)
		{
			// edith 2009.08.31 아이템 덤프를 남기지 않게 수정.
			int nBufferSize = sizeof(GAMELOG::sItemDump);

			char* pBuffer = reinterpret_cast<char*>(lpMoveItemLog+1);
			GAMELOG::sItemDump*	lpItemDump = reinterpret_cast<GAMELOG::sItemDump*>(pBuffer);
			std::fill_n(lpItemDump->m_usDataSize, int(GAMELOG::sItemDump::MAX_DUMP), 0);

			nUsage += nBufferSize;

//    		int nBufferSize = MAX_ITEM_DUMP_SIZE;
//			character.ItemDump(reinterpret_cast<char*>(lpMoveItemLog+1), &nBufferSize);
//			nUsage += nBufferSize;
		}

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 이동 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

void GAMELOG::LogSwapItem(const CCharacter& character, const TakeType srcTake, const TakeType dstTake, 
                          Item::CItem* lpSrcItem, Item::CItem* lpDstItem, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	SwapItem Log
	sSwapItemLog* lpSwapItemLog = reinterpret_cast<sSwapItemLog*>(gameLog.ReserveBuffer(
        static_cast<unsigned short>(sizeof(sSwapItemLog) + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpSwapItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sSwapItemLog));		
		lpSwapItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), CMD::SWAP_ITEM, static_cast<ERRType>(eError));

		lpSwapItemLog->InitSwapItemLog(srcTake, dstTake, lpSrcItem, lpDstItem);

		if (0 != eError)
		{
			int nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpSwapItemLog+1), &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 스왑 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}


void GAMELOG::LogUseItem(const CCharacter& character, const Item::ItemPos usePos, 
                         Item::CItem* lpUseItem, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	// UseItem Log
	sUseItemLog* lpUseItemLog = reinterpret_cast<sUseItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sUseItemLog) + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpUseItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sUseItemLog));
		lpUseItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), CMD::USE_ITEM, static_cast<ERRType>(eError));

		lpUseItemLog->InitUseItemLog(usePos, lpUseItem);

		if (0 != eError)
		{
			int nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpUseItemLog+1), &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 사용 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

void GAMELOG::LogUseLottery(const CCharacter& character, Item::CItem* lpGetItem, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	// UseLottery Log
	sUseLotteryLog* lpUseLotteryLog = reinterpret_cast<sUseLotteryLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sUseLotteryLog) + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpUseLotteryLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sUseLotteryLog));
		lpUseLotteryLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), CMD::USE_LOTTERY, static_cast<ERRType>(eError));

		lpUseLotteryLog->InitUseLotteryLog(lpGetItem);

		if (0 != eError)
		{
			int nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpUseLotteryLog+1), &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 사용 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}


void GAMELOG::LogSplitItem(const CCharacter& character, const TakeType splitTake, 
                           Item::CItem* lpPrevItem, Item::CItem* lpSplitItem, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    //	SplitItem Log
	sSplitItemLog* lpSplitItemLog = reinterpret_cast<sSplitItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sSplitItemLog) + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpSplitItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sSplitItemLog));		
		lpSplitItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
            gameLog.GetLogTime(), CMD::SPLIT_ITEM, static_cast<GAMELOG::ERRType>(eError));

		lpSplitItemLog->InitSplitItemLog(splitTake, lpPrevItem, lpSplitItem);

		if (0 != eError)
		{
			int	nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpSplitItemLog+1), &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 스플릿 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}


void GAMELOG::LogPickupItem(const CCharacter& character, const Item::ItemPos pickupPos, 
        Item::CItem* lpItem, const unsigned long dwGold, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	PickupItem Log
	sPickupItemLog* lpPickupItemLog = reinterpret_cast<sPickupItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sPickupItemLog) + Item::MAX_ITEM_SIZE + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpPickupItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sPickupItemLog));
		lpPickupItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), CMD::PICKUP_ITEM, static_cast<GAMELOG::ERRType>(eError));

		size_t nItemSize = Item::MAX_ITEM_SIZE;		
		if (0 == eError && 0 == dwGold && NULL != lpItem)
		{
            if (!lpItem->SerializeOut(reinterpret_cast<char*>(lpPickupItemLog+1), nItemSize))
            {
                ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
            }

			nUsage += static_cast<unsigned short>(nItemSize);
		}
        else
        {
            nItemSize = 0;
        }

		lpPickupItemLog->InitPickupItemLog(dwGold, pickupPos);

		if (0 != eError)
		{
			int	nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpPickupItemLog+1) + nItemSize, &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 집기 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}


// DROP_ITEM
void GAMELOG::LogDropItem(const CCharacter& character, const Item::ItemPos dropFrom, 
                 Item::CItem* lpItem, const unsigned long dwGold, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	DropItem Log
	sDropItemLog* lpDropItemLog = reinterpret_cast<sDropItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sDropItemLog) + Item::MAX_ITEM_SIZE + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpDropItemLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sDropItemLog));		
		lpDropItemLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), CMD::DROP_ITEM, static_cast<GAMELOG::ERRType>(eError));

        size_t nItemSize = Item::MAX_ITEM_SIZE;		
		if (0 == eError && 0 == dwGold && NULL != lpItem) 
		{
            if (!lpItem->SerializeOut(reinterpret_cast<char*>(lpDropItemLog+1), nItemSize))
            {
                ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
            }

			nUsage += static_cast<unsigned short>(nItemSize);
		}
        else
        {
            nItemSize = 0;
        }

		// 나중에 돈 Drop할 경우, 네번째 인자에 돈 값을 넣을 것.
		lpDropItemLog->InitDropItemLog(dwGold, dropFrom);
		
		if (0 != eError)
		{
			int	nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(reinterpret_cast<char*>(lpDropItemLog+1) + nItemSize, &nBufferSize);
            nUsage += nBufferSize;
		}
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 버리기 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}


void GAMELOG::LogTradeItem(const CAggresiveCreature& creature, const unsigned long dwTraderCID, 
						   const unsigned long dwGold, Item::CItem* lpItem, const Item::ItemPos itemPos, 
						   const unsigned char cCmd, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	TradeItem Log
	sTradeItemLog* lpTradeItemLog = reinterpret_cast<sTradeItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sTradeItemLog) + Item::MAX_ITEM_SIZE + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpTradeItemLog)
	{
		size_t nItemSize = Item::MAX_ITEM_SIZE;
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sTradeItemLog));

		CMDType LogCmd;
		switch (cCmd)
		{
			case PktTr::TRC_BUY_WITH_GOLD:		    LogCmd = CMD::BUY_ITEM;			    break;
			case PktTr::TRC_BUY_WITH_MILEAGE:	    LogCmd = CMD::MEDAL_BUY_ITEM;	    break;
			case PktTr::TRC_SELL:				    LogCmd = CMD::SELL_ITEM;		    break;
            case PktTr::TRC_BUY_WITH_SKILL_COUPON:  LogCmd = CMD::TICKET_BUY_SKILLBOOK; break;

            default:
                // 로그 기록하지 않는다.
                gameLog.UseComplete(0);
                return;
		}

		lpTradeItemLog->InitLogBase(creature.GetUID(), creature.GetCID(), 
            creature.GetCurrentPos(), gameLog.GetLogTime(), LogCmd, static_cast<GAMELOG::ERRType>(eError));

		if (0 == eError && 0 != lpItem)
		{
			if (!lpItem->SerializeOut(reinterpret_cast<char*>(lpTradeItemLog+1), nItemSize))
            {
                ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", creature.GetCID());
            }

			nUsage += static_cast<unsigned short>(nItemSize);
		}
		else
		{
			nItemSize = 0;
		}

		lpTradeItemLog->InitTradeItemLog(dwTraderCID, dwGold, itemPos);

		if (0 != eError)
		{
			int	nBufferSize = MAX_ITEM_DUMP_SIZE;
			creature.ItemDump(reinterpret_cast<char*>(lpTradeItemLog+1) + nItemSize, &nBufferSize);
            nUsage += nBufferSize;
		}

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 트레이트 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			creature.GetCID());
	}
}


void GAMELOG::LogExchangeItem(const CCharacter& character, const unsigned long dwDstCID, 
                              const Item::CExchangeContainer& Exchange, const GAMELOG::CMDType cLogCMD)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	unsigned short usReserveSize = sizeof(sExchangeItemLog) + EXCHANGE::MAX_EXCHANGE_SIZE;

	sExchangeItemLog* pExchangeItemLog = reinterpret_cast<sExchangeItemLog*>(
		gameLog.ReserveBuffer(usReserveSize));

	if (NULL != pExchangeItemLog)
	{
		ERRType	cErr = 0;
		unsigned long dwItemSize = EXCHANGE::MAX_EXCHANGE_SIZE;
        if (!Exchange.SerializeOut(reinterpret_cast<char*>(pExchangeItemLog+1), dwItemSize))
		{
			cErr = 1;
			dwItemSize = 0;
		}

		pExchangeItemLog->InitLogBase(character.GetUID(), character.GetCID(), 
			character.GetCurrentPos(), gameLog.GetLogTime(), cLogCMD, cErr);

		pExchangeItemLog->InitExchangeItemLog(Exchange.GetGold(), dwDstCID, 
			static_cast<unsigned short>(dwItemSize));

		gameLog.UseComplete(sizeof(sExchangeItemLog) + static_cast<unsigned short>(dwItemSize));
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 교환 로그를 남길 수 없습니다. 버퍼를 얻어올 수 없습니다.", 
			character.GetCID());
	}
}


void GAMELOG::LogInstallSocket(const CCharacter& character, const TakeType GemAndEquip, 
                               Item::CItem* lpGemItem, Item::CItem* lpEquipItem, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	InstallSocket Log
	sInstallSocketLog* lpInstallSocketLog = 
		reinterpret_cast<sInstallSocketLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sInstallSocketLog) + Item::MAX_ITEM_SIZE * 2 + MAX_ITEM_DUMP_SIZE)));

	if (NULL != lpInstallSocketLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sInstallSocketLog));
		lpInstallSocketLog->InitLogBase(character.GetUID(), character.GetCID(),
			character.GetCurrentPos(), gameLog.GetLogTime(), CMD::INSTALL_SOCKET_ITEM, static_cast<GAMELOG::ERRType>(eError));
		
		size_t nGemItemSize = 0;
		size_t nEquipItemSize = 0;	

		char* pItemBuffer = reinterpret_cast<char*>(lpInstallSocketLog+1);

        if (0 == eError)
        {
		    if (0 != lpGemItem)
		    {
                nGemItemSize = Item::MAX_ITEM_SIZE;
			    if (!lpGemItem->SerializeOut(pItemBuffer, nGemItemSize))
                {
                    ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
                }

			    nUsage += static_cast<unsigned short>(nGemItemSize);
			    pItemBuffer += nGemItemSize;
		    }

		    if (0 != lpEquipItem)
		    {
                nEquipItemSize = Item::MAX_ITEM_SIZE;
			    if (!lpEquipItem->SerializeOut(pItemBuffer, nEquipItemSize))
                {
                    ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
                }

			    nUsage += static_cast<unsigned short>(nEquipItemSize);
			    pItemBuffer += nEquipItemSize;
		    }
        }
        else
		{
			int nBufferSize = MAX_ITEM_DUMP_SIZE;
			character.ItemDump(pItemBuffer, &nBufferSize);
            nUsage += nBufferSize;
		}

        lpInstallSocketLog->InitInstallSocketLog(GemAndEquip, 
            static_cast<unsigned char>(nGemItemSize), static_cast<unsigned char>(nEquipItemSize));

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 아이템 소켓 박기 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

void GAMELOG::LogUpgradeItem(const CCharacter& character, unsigned long dwCurrentGold, 
                             unsigned long dwUsedGold, const unsigned char cLastUpgradeLevel, 
                             Item::CItem* lpResult, Item::CItem* lpMineral, const unsigned char cValue, unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sUpgradeItemLogV2* lpUpgradeItemLogV2 = reinterpret_cast<sUpgradeItemLogV2*>(
        gameLog.ReserveBuffer(sizeof(sUpgradeItemLogV2) + Item::MAX_ITEM_SIZE * 2));

    if (NULL != lpUpgradeItemLogV2)
    {        
        unsigned short nUsage = static_cast<unsigned short>(sizeof(sUpgradeItemLogV2));

        lpUpgradeItemLogV2->InitLogBase(character.GetUID(), character.GetCID(),
            character.GetCurrentPos(), gameLog.GetLogTime(), 
            CMD::UPGRADE_ITEM_V2, static_cast<GAMELOG::ERRType>(eError));

        size_t nResultSize = 0;
        size_t nMineralSize = 0;

        char* lpItemBuffer = reinterpret_cast<char*>(lpUpgradeItemLogV2 + 1);

        if (0 == eError)
        {
            if (NULL != lpMineral)
            {
                nMineralSize = Item::MAX_ITEM_SIZE;
                if (!lpMineral->SerializeOut(lpItemBuffer, nMineralSize))
                {
                    ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
                }

                nUsage += static_cast<unsigned short>(nMineralSize);
                lpItemBuffer += nMineralSize;
            }

            if (NULL != lpResult)
            {
                nResultSize = Item::MAX_ITEM_SIZE;
                if (!lpResult->SerializeOut(lpItemBuffer, nResultSize))
                {
                    ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
                }

                nUsage += static_cast<unsigned short>(nResultSize);
                lpItemBuffer += nResultSize;
            }
        }
        else
        {
            int nBufferSize = MAX_ITEM_DUMP_SIZE;
            character.ItemDump(lpItemBuffer, &nBufferSize);
            nUsage += nBufferSize;
        }

        lpUpgradeItemLogV2->InitUpgradeItemLog(dwCurrentGold, dwUsedGold, cLastUpgradeLevel, 
            static_cast<unsigned char>(nMineralSize), static_cast<unsigned char>(nResultSize));

        gameLog.UseComplete(nUsage);
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 아이템 업그레이드 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없스니다.", 
            character.GetCID());
    }
}


// REPAIR_ITEM
void GAMELOG::LogRepairItem(const CCharacter& character, Item::CItem* lpRepairedItem, 
                            const unsigned long dwRepairPrice, const unsigned char cPreRepairDurability, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sRepairItemLog* lpRepairItemLog = reinterpret_cast<sRepairItemLog*>(
        gameLog.ReserveBuffer(sizeof(sRepairItemLog)));

    if (NULL != lpRepairItemLog)
    {
		lpRepairItemLog->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::REPAIR_ITEM, static_cast<GAMELOG::ERRType>(eError));
        
        lpRepairItemLog->InitRepairItemLog(dwRepairPrice, lpRepairedItem, cPreRepairDurability);

		gameLog.UseComplete(sizeof(sRepairItemLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 아이템 수리 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            character.GetCID());
    }

}

void GAMELOG::LogChangeWeapon(const CCharacter& character, const unsigned char cCurrentHand)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sChangeWeaponLog* lpChangeWeaponLog = reinterpret_cast<sChangeWeaponLog*>(
        gameLog.ReserveBuffer(sizeof(sChangeWeaponLog)));

    if (NULL != lpChangeWeaponLog)
    {
		lpChangeWeaponLog->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::CHANGE_WEAPON, 0);
        
        lpChangeWeaponLog->InitCurrentHand(cCurrentHand);

		gameLog.UseComplete(sizeof(sChangeWeaponLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 무기 바꿈 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            character.GetCID());
    }
}

void GAMELOG::LogChangeRide(const CCharacter& character, const unsigned char cCurrentRide)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sChangeRideLog* lpChangeRideLog = reinterpret_cast<sChangeRideLog*>(
        gameLog.ReserveBuffer(sizeof(sChangeRideLog)));

    if (NULL != lpChangeRideLog)
    {
		lpChangeRideLog->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::CHANGE_RIDE, 0);
        
        lpChangeRideLog->InitCurrentRide(cCurrentRide);

		gameLog.UseComplete(sizeof(sChangeRideLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 말 타기 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            character.GetCID());
    }
}


// TAKE_GOLD_V2
void GAMELOG::LogTakeGold(const CCharacter& character, unsigned long dwSrcGold, unsigned long dwDstGold,
                          unsigned long dwMoveGold, unsigned char cSrcPos, unsigned char cDstPos, 
                          unsigned char cPurpose, unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sTakeGoldLogV2* lpTakeGoldLogV2 = reinterpret_cast<sTakeGoldLogV2*>(gameLog.ReserveBuffer(sizeof(sTakeGoldLogV2)));

    if (NULL != lpTakeGoldLogV2)
    {
		lpTakeGoldLogV2->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::TAKE_GOLD_V2, static_cast<GAMELOG::ERRType>(eError));
        
        lpTakeGoldLogV2->m_SrcGold  = dwSrcGold;
        lpTakeGoldLogV2->m_DstGold  = dwDstGold;
        lpTakeGoldLogV2->m_MoveGold = dwMoveGold;
        lpTakeGoldLogV2->m_cSrcPos  = cSrcPos;
        lpTakeGoldLogV2->m_cDstPos  = cDstPos;
        lpTakeGoldLogV2->m_cPurpose = cPurpose;
        
		gameLog.UseComplete(sizeof(sTakeGoldLogV2));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 돈 이동 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            character.GetCID());
    }
}


// STALL_OPEN_CLOSE
void GAMELOG::LogStallOpenClose(const CAggresiveCreature& creature, const char* szStallName, const bool bOpen)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sStallOpenCloseLog* lpStallOpenCloseLog = 
        reinterpret_cast<sStallOpenCloseLog*>(gameLog.ReserveBuffer(sizeof(sStallOpenCloseLog)));

    if (NULL != lpStallOpenCloseLog)
    {
		lpStallOpenCloseLog->InitLogBase(creature.GetUID(), creature.GetCID(), 
            creature.GetCurrentPos(), gameLog.GetLogTime(), CMD::STALL_OPEN_CLOSE, 0);

        lpStallOpenCloseLog->InitStallOpenCloseLog(bOpen ? sStallOpenCloseLog::STALL_OPEN : sStallOpenCloseLog::STALL_CLOSE,
            szStallName);

        gameLog.UseComplete(sizeof(sStallOpenCloseLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 노점상 열고 닫기 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            creature.GetCID());
    }
}

// STALL_ENTER_LEAVE
void GAMELOG::LogStallEnterLeave(const CAggresiveCreature& creature, const unsigned long dwCustomerCID, const bool bEnter)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sStallEnterLeaveLog* lpStallEnterLeaveLog = 
        reinterpret_cast<sStallEnterLeaveLog*>(gameLog.ReserveBuffer(sizeof(sStallEnterLeaveLog)));

    if (NULL != lpStallEnterLeaveLog)
    {
		lpStallEnterLeaveLog->InitLogBase(creature.GetUID(), creature.GetCID(), 
            creature.GetCurrentPos(), gameLog.GetLogTime(), CMD::STALL_ENTER_LEAVE, 0);

        lpStallEnterLeaveLog->InitStallEnterLeaveLog(dwCustomerCID, 
            bEnter ? sStallEnterLeaveLog::STALL_ENTER : sStallEnterLeaveLog::STALL_LEAVE);

        gameLog.UseComplete(sizeof(sStallEnterLeaveLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 노점상 입장 퇴장 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            creature.GetCID());
    }
}

// STALL_ITEM_REGISTER_REMOVE
void GAMELOG::LogStallRegisterRemoveItem(const CCharacter& character, const Item::CItem* lpItem, const TakeType takeType,
                                         const unsigned char cPktStRICMD, const unsigned short usError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sStallRegisterRemoveItemLog* lpStallRegisterRemoveItemLog =
        reinterpret_cast<sStallRegisterRemoveItemLog*>(gameLog.ReserveBuffer(sizeof(sStallRegisterRemoveItemLog)));

    if (NULL != lpStallRegisterRemoveItemLog)
    {
        lpStallRegisterRemoveItemLog->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::STALL_ITEM_REGISTER_REMOVE, 
            static_cast<const GAMELOG::ERRType>(usError));

        sStallRegisterRemoveItemLog::StallMode eStallMode = sStallRegisterRemoveItemLog::STALL_UNKNOWN;

        switch(cPktStRICMD)
        {
			case PktStRI::SC_REGISTER:       	eStallMode = sStallRegisterRemoveItemLog::STALL_REGISTERITEM;    	break;
			case PktStRI::SC_CANCEL:         	eStallMode = sStallRegisterRemoveItemLog::STALL_REMOVEITEM;      	break;
			case PktStRI::SC_CAMP_REGISTER:		eStallMode = sStallRegisterRemoveItemLog::STALL_CAMP_REGISTER;		break;
			case PktStRI::SC_CAMP_CANCEL:		eStallMode = sStallRegisterRemoveItemLog::STALL_CAMP_REMOVE;		break;
        }
          
        lpStallRegisterRemoveItemLog->InitStallRegisterRemoveItemLog(eStallMode, lpItem, takeType);

        gameLog.UseComplete(sizeof(sStallRegisterRemoveItemLog));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 의 노점상 아이템 등록 제거 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.",
            character.GetCID());
    }
}

// ITEM_ATTACH_OPTION   아이템 옵션 이식
void GAMELOG::LogItemAttachOption(const CCharacter& character, 
                                  unsigned long dwOldInvenGold, unsigned char cSelectedOptionIndex,
                                  const char* szSerializedOldItem, size_t nOldItemSize, 
                                  const char* szUseItemData, size_t nUseItemSize,
                                  Item::CItem* lpNewItem, unsigned short usError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sItemAttachOption* lpItemAttachOption = reinterpret_cast<sItemAttachOption*>(
        gameLog.ReserveBuffer(sizeof(sItemAttachOption) + Item::MAX_ITEM_SIZE * 3));

    if (NULL != lpItemAttachOption)
    {
        lpItemAttachOption->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::ITEM_ATTACH_OPTION, 
            static_cast<const GAMELOG::ERRType>(usError));

        lpItemAttachOption->m_dwOldInvenGold = dwOldInvenGold;
        lpItemAttachOption->m_dwNewInvenGold = character.GetGold();

        lpItemAttachOption->m_cSelectedOptionIndex = cSelectedOptionIndex;
        lpItemAttachOption->m_cOldItemLen = 0;
        lpItemAttachOption->m_cUseItemLen = 0;
        lpItemAttachOption->m_cNewItemLen = 0;

        char*  szItemData = reinterpret_cast<char*>(lpItemAttachOption + 1);
        size_t nItemLength = 0;

        if(0 != szSerializedOldItem && 0 != nOldItemSize)
        {
            memcpy(szItemData, szSerializedOldItem, nOldItemSize);

            lpItemAttachOption->m_cOldItemLen = 
                static_cast<unsigned char>(nOldItemSize);

            szItemData += nOldItemSize;
        }

        if(0 != szUseItemData && 0 != nUseItemSize)
        {
            memcpy(szItemData, szUseItemData, nUseItemSize);

            lpItemAttachOption->m_cUseItemLen = 
                static_cast<unsigned char>(nUseItemSize);

            szItemData += nUseItemSize;
        }

        if(0 != lpNewItem)
        {
            nItemLength = Item::MAX_ITEM_SIZE;

            if(lpNewItem->SerializeOut(szItemData, nItemLength))
            {
                lpItemAttachOption->m_cNewItemLen = 
                    static_cast<unsigned char>(nItemLength);

                szItemData += nItemLength;
            }
            else
            {
                ERRLOG1(g_Log, "CID:%10u 새 아이템 SerializeOut에 실패했습니다.", character.GetCID());
            }
        }

        gameLog.UseComplete(static_cast<unsigned short>(
            szItemData - reinterpret_cast<char*>(lpItemAttachOption)));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u / 아이템 옵션 이식 로그 기록 실패 : 버퍼 할당 실패", character.GetCID());
    }
}


// ITEM_COMPENSATION    아이템 보상
void GAMELOG::LogItemCompensation(const CCharacter& character, unsigned long dwOldInvenGold, 
                                  Item::CItem* lpOldItem, Item::CItem* lpNewItem,
                                  unsigned short usError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sItemCompensation* lpItemCompensation = reinterpret_cast<sItemCompensation*>(
        gameLog.ReserveBuffer(sizeof(sItemCompensation) + Item::MAX_ITEM_SIZE * 2));

    if (NULL != lpItemCompensation)
    {
        lpItemCompensation->InitLogBase(character.GetUID(), character.GetCID(), 
            character.GetCurrentPos(), gameLog.GetLogTime(), CMD::ITEM_COMPENSATION, 
            static_cast<const GAMELOG::ERRType>(usError));

        lpItemCompensation->m_dwOldInvenGold = dwOldInvenGold;
        lpItemCompensation->m_dwNewInvenGold = character.GetGold();

        lpItemCompensation->m_cOldItemLen = 0;
        lpItemCompensation->m_cNewItemLen = 0;
    
        char*  szItemData = reinterpret_cast<char*>(lpItemCompensation + 1);
        size_t nItemLength = 0;

        if(0 != lpOldItem)
        {
            nItemLength = Item::MAX_ITEM_SIZE;

            if(lpOldItem->SerializeOut(szItemData, nItemLength))
            {
                lpItemCompensation->m_cOldItemLen = 
                    static_cast<unsigned char>(nItemLength);

                szItemData += nItemLength;
            }
            else
            {
                ERRLOG1(g_Log, "CID:%10u 예전 아이템 SerializeOut에 실패했습니다.", character.GetCID());
            }
        }

        if(0 != lpNewItem)
        {
            nItemLength = Item::MAX_ITEM_SIZE;

            if(lpNewItem->SerializeOut(szItemData, nItemLength))
            {
                lpItemCompensation->m_cNewItemLen = 
                    static_cast<unsigned char>(nItemLength);

                szItemData += nItemLength;
            }
            else
            {
                ERRLOG1(g_Log, "CID:%10u 보상 아이템 SerializeOut에 실패했습니다.", character.GetCID());
            }
        }

        gameLog.UseComplete(static_cast<unsigned short>(
            szItemData - reinterpret_cast<char*>(lpItemCompensation)));
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u 아이템 보상 로그 기록 실패 : 버퍼 할당 실패", character.GetCID());
    }
}

// ILLEGAL_ITEM
void GAMELOG::LogIllegalItem(const unsigned long dwCID, const Item::ItemPos usePos, const Item::CItem* lpItem)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	//	Illegal Log
	sIllegalItemLog* lpIllegalItemLog = reinterpret_cast<sIllegalItemLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sIllegalItemLog))));

	if (NULL != lpIllegalItemLog)
	{
		Position pos = Position(0,0,0);
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sIllegalItemLog));
		lpIllegalItemLog->InitLogBase(0, dwCID, pos, gameLog.GetLogTime(), CMD::ILLEGAL_ITEM, static_cast<ERRType>(0));

		lpIllegalItemLog->InitIllegalItemLog(usePos, lpItem);

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 비인증 아이템 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.", dwCID);
	}
}

// ILLEGAL_WARPPOS
void GAMELOG::LogIllegalWarpPos(const CCharacter& character, const Position &oldPos, const Position &Pos)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	//	CharWarpPos Log
	sIllegalWarpPosLog* lpCharWarpPosLog = reinterpret_cast<sIllegalWarpPosLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sIllegalWarpPosLog))));

	if (NULL != lpCharWarpPosLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sIllegalWarpPosLog));
		lpCharWarpPosLog->InitLogBase(character.GetUID(), character.GetCID(), oldPos, gameLog.GetLogTime(), CMD::ILLEGAL_WARPPOS, static_cast<ERRType>(0));

		unsigned long dis = Pos.GetDistance(oldPos);
		lpCharWarpPosLog->InitCharWarpPosLog(Pos, dis);

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 비인증 워프 사용 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.", character.GetCID());
	}
}

// HACK_DOUBT
void GAMELOG::LogHackBoubt(const CCharacter& character, unsigned short wHackID, unsigned short wKindItem, Item::CItem* lpItem, unsigned short usError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	//	sHockDoubt Log
	sHockDoubtLog* lpHockDoubtLog = reinterpret_cast<sHockDoubtLog*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sHockDoubtLog) + Item::MAX_ITEM_SIZE)));

	if (NULL != lpHockDoubtLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(sHockDoubtLog));
		lpHockDoubtLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(), gameLog.GetLogTime(), CMD::HACK_DOUBT, static_cast<ERRType>(usError));

		size_t nItemSize = Item::MAX_ITEM_SIZE;		
		if (NULL != lpItem)
		{
			if (!lpItem->SerializeOut(reinterpret_cast<char*>(lpHockDoubtLog+1), nItemSize))
			{
				ERRLOG1(g_Log, "CID:%10u 아이템 SerializeOut에 실패했습니다.", character.GetCID());
			}

			nUsage += static_cast<unsigned short>(nItemSize);
		}
		else
			nItemSize = 0;

		lpHockDoubtLog->InitHackDoubt(wHackID, wKindItem, nItemSize);

		gameLog.UseComplete(nUsage);
	}
	else
	{
		ERRLOG1(g_Log, "CID:%10u 의 핵의심 메시지 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.", character.GetCID());
	}
}
