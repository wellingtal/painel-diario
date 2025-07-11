
#include "stdafx.h"

#include <Item/Item.h>
#include <Creature/Character/Character.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>

#include "SendCharItem.h"


bool GameClientSendPacket::SendCharTakeItem(CSendStream& SendStream, unsigned long dwCharID, 
                                            TakeType takeType, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktTI));
    if (NULL != lpBuffer)
    {
        PktTI* lpPktTIAck = reinterpret_cast<PktTI*>(lpBuffer);

        if (0 == usError)
        {
            lpPktTIAck->m_dwCharID = dwCharID;
            lpPktTIAck->m_TakeType = takeType;
        }
        else
        {
            lpPktTIAck->m_dwCharID = 0;
            lpPktTIAck->m_TakeType = TakeType();
        }
                
        return SendStream.WrapCrypt(sizeof(PktTI), CmdCharTakeItem, 0, usError);
    };

    return false;
}


bool GameClientSendPacket::SendCharSwapItem(CSendStream& SendStream, unsigned long dwCharID, 
                                            TakeType takeSrc, TakeType takeDst, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSwI));
    if (NULL != lpBuffer)
    {
        PktSwI* lpPktSwlAck = reinterpret_cast<PktSwI*>(lpBuffer);

        if (0 == usError)
        {
            lpPktSwlAck->m_dwCharID = dwCharID;
            lpPktSwlAck->m_SwapSrc  = takeSrc;
		    lpPktSwlAck->m_SwapDst	= takeDst;
        }
        else
        {
            lpPktSwlAck->m_dwCharID = 0;
            lpPktSwlAck->m_SwapSrc	= TakeType();
		    lpPktSwlAck->m_SwapDst	= TakeType();
        }
        
        return SendStream.WrapCrypt(sizeof(PktSwI), CmdCharSwapItem, 0, usError);
    };

    return false;
}

bool GameClientSendPacket::SendCharRepairItem(CSendStream& SendStream, unsigned long dwCharID, 
                                              unsigned long dwRepairGold, Item::ItemPos itemPos, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktRpI));
    if (NULL != lpBuffer)
    {
        PktRpI* lpPktRpl = reinterpret_cast<PktRpI*>(lpBuffer);

        lpPktRpl->m_dwCharID    = dwCharID;
        lpPktRpl->m_dwGold      = dwRepairGold;
        lpPktRpl->m_itemPos     = itemPos;

        return SendStream.WrapCrypt(sizeof(PktRpI), CmdCharRepairItem, 0, usError);
    };
 
    return false;
}

// 아이템 수리 (장비하고 있는 아이템 모두 수리)
bool GameClientSendPacket::SendCharRepairAllItem(CSendStream& SendStream, unsigned long dwCharID,
												 unsigned long dwRepairGold, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktRpAI));
	if (NULL != lpBuffer)
	{
		PktRpAI* lpPktRpAI = reinterpret_cast<PktRpAI*>(lpBuffer);

		lpPktRpAI->m_dwCharID    = dwCharID;
		lpPktRpAI->m_dwGold      = dwRepairGold;

		return SendStream.WrapCrypt(sizeof(PktRpAI), CmdCharRepairAllItem, 0, usError);
	};

	return false;
}

// 캐쉬 아이템 사용
bool GameClientSendPacket::SendCharUseCashItem(CSendStream& SendStream, unsigned long dwSender, 
                                           unsigned long dwReceiver, Item::ItemPos itemPos, 
                                           unsigned char cRemainItemNum, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUI));
    if (NULL != lpBuffer)
    {
        PktUI* lpPktUI = reinterpret_cast<PktUI*>(lpBuffer);

        lpPktUI->m_dwSender         = dwSender;
        lpPktUI->m_dwRecver         = dwReceiver;
        lpPktUI->m_itemPos          = itemPos;
        lpPktUI->m_cRemainItemNum   = cRemainItemNum;

        return SendStream.WrapCrypt(sizeof(PktUI), CmdCharUseCashItem, 0, usError);
    }

    return false;
}

bool GameClientSendPacket::SendCharUseItem(CSendStream& SendStream, unsigned long dwSender, 
                                           unsigned long dwReceiver, Item::ItemPos itemPos, 
                                           unsigned char cRemainItemNum, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUI));
    if (NULL != lpBuffer)
    {
        PktUI* lpPktUI = reinterpret_cast<PktUI*>(lpBuffer);

        lpPktUI->m_dwSender         = dwSender;
        lpPktUI->m_dwRecver         = dwReceiver;
        lpPktUI->m_itemPos          = itemPos;
        lpPktUI->m_cRemainItemNum   = cRemainItemNum;

        return SendStream.WrapCrypt(sizeof(PktUI), CmdCharUseItem, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharTradeItem(CSendStream& SendStream, CCharacter* lpCharacter, unsigned long dwNPCID, Item::ItemPos CouponPos, 
                                             Item::CItem* lpItem, Item::ItemPos itemPos, unsigned char cNum, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktTrAck) + Item::MAX_ITEM_SIZE);
    if (NULL != lpBuffer)
    {
        PktTrAck* lpPktTrAck = reinterpret_cast<PktTrAck*>(lpBuffer);
        
        size_t nItemSize = 0;
		if (PktBase::NO_SERVER_ERR == usError)
        {
            lpPktTrAck->m_dwCharID  = lpCharacter->GetCID();
            lpPktTrAck->m_dwNPCID   = dwNPCID;

			lpPktTrAck->m_dwGold    = lpCharacter->GetGold();
			lpPktTrAck->m_dwMileage	= lpCharacter->GetMileage();

			lpPktTrAck->m_CouponPos	= CouponPos;

            if (NULL != lpItem)
            {
                nItemSize = Item::MAX_ITEM_SIZE;
                if (false == lpItem->SerializeOut(lpBuffer + sizeof(PktTrAck), nItemSize))
                {
                    ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", lpCharacter->GetCID());
                }
            }
            else
            {
                nItemSize = 0;
            }

            lpPktTrAck->m_wSize     = static_cast<unsigned short>(nItemSize);            
            lpPktTrAck->m_itemPos   = itemPos;
			lpPktTrAck->m_cNum		= cNum;
        }
        else
        {
            lpPktTrAck->m_dwCharID  = 0;
            lpPktTrAck->m_dwNPCID   = 0;
            lpPktTrAck->m_dwGold    = 0;
			lpPktTrAck->m_CouponPos	= Item::ItemPos();

            lpPktTrAck->m_wSize     = 0;
            lpPktTrAck->m_itemPos   = Item::ItemPos();
			lpPktTrAck->m_cNum		= 0;
        }

        return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktTrAck) + nItemSize), 
            CmdCharTradeItem, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharEquipShopInfo(CSendStream& SendStream, unsigned long dwCID, unsigned long dwNPCID, unsigned long dwTime, 
												 unsigned char cRace, unsigned char cTabPage, unsigned char cNum, 
												 unsigned short* aryItemID, unsigned char* aryItemGrade, unsigned short usError)
{
	const int nBufferSize = sizeof(PktEquipShopInfo) + (sizeof(unsigned short) + sizeof(unsigned char)) * cNum;
	char* lpBuffer = SendStream.GetBuffer(nBufferSize);

	if (NULL != lpBuffer)
	{
		PktEquipShopInfo* lpPktEquipShopInfo = reinterpret_cast<PktEquipShopInfo *>(lpBuffer);

		lpPktEquipShopInfo->m_dwCharID	= dwCID;
		lpPktEquipShopInfo->m_dwNPCID	= dwNPCID;

		lpPktEquipShopInfo->m_dwTime	= dwTime;

		lpPktEquipShopInfo->m_cRace		= cRace;
		lpPktEquipShopInfo->m_cTabPage	= cTabPage;

		lpPktEquipShopInfo->m_cNum		= cNum;

		unsigned short* lpItemID = reinterpret_cast<unsigned short *>(lpPktEquipShopInfo + 1);
		for (unsigned char cIDIndex = 0; cIDIndex < cNum; ++cIDIndex)
		{
			*lpItemID = aryItemID[cIDIndex];
			++lpItemID;
		}

		unsigned char* lpItemGrade = reinterpret_cast<unsigned char *>(lpItemID);
		for (unsigned char cGradeIndex = 0; cGradeIndex < cNum; ++cGradeIndex)
		{
			*lpItemGrade = aryItemGrade[cGradeIndex];
			++lpItemGrade;
		}

		return SendStream.WrapCrypt(nBufferSize, CmdCharEquipShopInfo, 0, usError);
	}

	return false;
}


bool GameClientSendPacket::SendCharPickUp(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID,
                                          unsigned long dwGold, Item::CItem* lpItem, Item::ItemPos dstPos, 
                                          unsigned char cNum, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktPUAck) + Item::MAX_ITEM_SIZE);
    if (NULL != lpBuffer)
    {
        PktPUAck*   lpPktPUAck  = reinterpret_cast<PktPUAck*>(lpBuffer);
        size_t      nItemSize   = 0;

		lpPktPUAck->m_cType     = 0;
		lpPktPUAck->m_dwSize	= 0;

        if (0 == usError)
        {
		    if (0 != dwGold) 
		    {
			    lpPktPUAck->m_cType     = PktPUAck::Gold;
			    lpPktPUAck->m_dwSize	= dwGold;
		    }
		    else if (NULL != lpItem) 
		    {       
                nItemSize = Item::MAX_ITEM_SIZE;
			    if (!lpItem->SerializeOut(lpBuffer + sizeof(PktPUAck), nItemSize))
                {   
                    ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCID);
                }

			    lpPktPUAck->m_cType     = PktPUAck::Item;
			    lpPktPUAck->m_dwSize    = static_cast<unsigned long>(nItemSize);
		    }
        }

        lpPktPUAck->m_dwCharID      = dwCID;
	    lpPktPUAck->m_nObjectID		= nObjectID;
	    lpPktPUAck->m_itemPos		= dstPos;
        lpPktPUAck->m_cNum          = cNum;

        return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktPUAck) + nItemSize),
            CmdCharPickUp, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharAutoRouting(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID, 
											   unsigned short wItemID, unsigned char cNum, unsigned char cCmd)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktAutoRouting));
	if (NULL == lpBuffer) { return false; }

	PktAutoRouting* lpPktAR = reinterpret_cast<PktAutoRouting *>(lpBuffer);

	lpPktAR->m_dwCharID = dwCID;
	lpPktAR->m_nObjectID = nObjectID;

	lpPktAR->m_wItemID = wItemID;
	lpPktAR->m_cNum = cNum;

	lpPktAR->m_cCmd = cCmd;

	return SendStream.WrapCrypt(sizeof(PktAutoRouting), CmdCharAutoRouting, 0, 0);
}


bool GameClientSendPacket::SendCharPullDown(CSendStream& SendStream, unsigned long dwCID, Item::ItemPos itemPos, 
                                            CCell::ItemInfo& itemInfo, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktPDAck));
    if (NULL != lpBuffer)
    {
        PktPDAck* lpPktPDAck = reinterpret_cast<PktPDAck*>(lpBuffer);
        itemInfo.MakeFieldObject(lpPktPDAck->m_FieldObject);

	    if (0 == usError)
	    {
		    lpPktPDAck->m_dwCharID  = dwCID;
            lpPktPDAck->m_itemPos   = itemPos;
	    }
	    else
	    {
		    lpPktPDAck->m_dwCharID  = 0;
            lpPktPDAck->m_itemPos   = Item::ItemPos();
	    }

        return SendStream.WrapCrypt(sizeof(PktPDAck), CmdCharPullDown, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharDisappearItem(CSendStream& SendStream, unsigned long dwCID, 
												 Item::ItemPos itemPos, unsigned char cNum, 
												 unsigned char cCmd, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktDisappearItem));
	if (NULL != lpBuffer)
	{
		PktDisappearItem* lpPktDI = reinterpret_cast<PktDisappearItem*>(lpBuffer);

		lpPktDI->m_dwCharID		= dwCID;
		lpPktDI->m_itemPos		= itemPos;
		lpPktDI->m_cNum			= cNum;
		lpPktDI->m_cCmd			= cCmd;

		return SendStream.WrapCrypt(sizeof(PktDisappearItem), CmdCharDisappearItem, 0, usError);
	}

	return false;
}


bool GameClientSendPacket::SendCharSplitItem(CSendStream& SendStream, unsigned long dwCID, 
                                             Item::CItem* lpSplitItem, TakeType takeType, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSpItAck) + Item::MAX_ITEM_SIZE);
    if (NULL != lpBuffer)
    {
        PktSpItAck* lpPktSpltAck = reinterpret_cast<PktSpItAck*>(lpBuffer);

        size_t nItemSize = 0;
        if (NULL != lpSplitItem)
        {
            nItemSize = Item::MAX_ITEM_SIZE;
            if (!lpSplitItem->SerializeOut(lpBuffer + sizeof(PktSpItAck), nItemSize))
            {
                ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCID);
            }
        }
        
        if (0 == usError)
        {
            lpPktSpltAck->m_dwCharID = dwCID;
            lpPktSpltAck->m_TakeType = takeType;
        }
        else
        {
            lpPktSpltAck->m_dwCharID = 0;
            lpPktSpltAck->m_TakeType = TakeType();
        }

        lpPktSpltAck->m_cSize = static_cast<unsigned char>(nItemSize);
        return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktSpItAck) + nItemSize), 
            CmdCharSplitItem, 0, usError);
    }

    return false;
}

bool GameClientSendPacket::SendCharTakeGold(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGold, 
                                            unsigned char cSrcPos, unsigned char cDstPos, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktTG));
    if (NULL != lpBuffer)
    {
        PktTG* lpPktTG = reinterpret_cast<PktTG*>(lpBuffer);

	    if (0 == usError)
	    {
		    lpPktTG->m_dwCharID	= dwCID;
            lpPktTG->m_dwGold	= dwGold;
		    lpPktTG->m_cSrcPos	= cSrcPos;
		    lpPktTG->m_cDstPos	= cDstPos;
		    
	    }
	    else
	    {
		    lpPktTG->m_dwCharID	= 0;
            lpPktTG->m_dwGold	= 0;
		    lpPktTG->m_cSrcPos	= 0;
		    lpPktTG->m_cDstPos	= 0;		    
	    }

        return SendStream.WrapCrypt(sizeof(PktTG), CmdCharTakeGold, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharExchangeItem(CSendStream& SendStream, unsigned long dwOwnerCID, unsigned long dwGold, 
                                                Item::CItem* lpItem, Item::ItemPos itemPos, bool bStack, bool bRemove)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktExI) + Item::MAX_ITEM_SIZE);
    if (NULL != lpBuffer)
    {
        PktExI* lpPktExI    = reinterpret_cast<PktExI*>(lpBuffer);
	    size_t  nItemSize   = 0;

        unsigned short  usError = 0;

		if (NULL == lpItem)
		{
			lpPktExI->m_cType	= PktExI::Gold;
			lpPktExI->m_dwSize	= dwGold;
	    }
	    else 
	    {
            if (false == bStack)
            {
                nItemSize = Item::MAX_ITEM_SIZE;
            
                char* lpItemBuffer = lpBuffer + sizeof(PktExI);
                if (!lpItem->SerializeOut(lpItemBuffer, nItemSize))
                {
                    usError = 1;
                    ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwOwnerCID);
                }

                lpPktExI->m_dwSize  = static_cast<unsigned long>(nItemSize);
            }
            else
            {
                lpPktExI->m_dwSize  = 0;
				lpPktExI->m_cNum	= lpItem->GetNumOrDurability();
            }

            lpPktExI->m_cType = bRemove ? PktExI::Remove : PktExI::Item;		    
	    }

        lpPktExI->m_dwCharID    = dwOwnerCID;
	    lpPktExI->m_itemPos     = itemPos;

        return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktExI) + nItemSize),
            CmdCharExchangeItem, 0, usError);
    }

    return false;
}

// 창고 업데이트 커맨드(클라이언트)
bool GameClientSendPacket::SendCharDepositCmd(CSendStream& SendStream, unsigned char cCmd, 
                                              char* szData, size_t nDataLength, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDeposit));
    if (NULL != lpBuffer)
    {
        PktDeposit* lpPktDeposit = reinterpret_cast<PktDeposit*>(lpBuffer);

        lpPktDeposit->m_cCmd = cCmd;
        memcpy(lpPktDeposit->m_szData, szData, 
            (PktDeposit::MIN_DATA_SIZE < nDataLength) ? PktDeposit::MIN_DATA_SIZE : nDataLength);

        return SendStream.WrapCrypt(sizeof(PktDeposit), CmdDeposit, 0, usError);
    }
    
	return false;
}

// 창고 업데이트(패스워드)
bool GameClientSendPacket::SendCharDepositPasswordToDBAgent(CSendStream& SendStream, unsigned long dwUID, 
                                                   const char* szPassword, size_t nPasswordLen)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDeposit));
    if (NULL != lpBuffer)
    {
        PktDeposit* lpPktDeposit = reinterpret_cast<PktDeposit*>(lpBuffer);

        lpPktDeposit->m_cCmd    = PktDeposit::PASSWORD;

        memcpy(lpPktDeposit->m_szData, &dwUID, sizeof(dwUID));
        memcpy(lpPktDeposit->m_szData + sizeof(dwUID), szPassword, 
            (Deposit::PASSWORD_LENGTH < nPasswordLen) ? Deposit::PASSWORD_LENGTH : nPasswordLen);

        return SendStream.WrapHeader(sizeof(PktDeposit), CmdDeposit, 0, 0);
    }

    return false;
}

// 창고 업데이트(Gold)
bool GameClientSendPacket::SendCharDepositGoldToDBAgent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwGold)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDeposit));
    if (NULL != lpBuffer)
    {
        PktDeposit* lpPktDeposit = reinterpret_cast<PktDeposit*>(lpBuffer);

        lpPktDeposit->m_cCmd    = PktDeposit::GOLD;

        memcpy(lpPktDeposit->m_szData, &dwUID, sizeof(dwUID));
        memcpy(lpPktDeposit->m_szData + sizeof(dwUID), &dwGold, sizeof(dwGold));

        return SendStream.WrapHeader(sizeof(PktDeposit), CmdDeposit, 0, 0);
    }

    return false;
}

// 복권 결과
bool GameClientSendPacket::SendCharLotteryResult(CSendStream& SendStream, unsigned long dwCID, 
												 Item::ItemPos itemPos, Item::CItem* lpItem, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktLotteryResult) + Item::MAX_ITEM_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktLotteryResult* lpPktLR = reinterpret_cast<PktLotteryResult *>(lpBuffer);

	lpPktLR->m_dwCharID = dwCID;
	lpPktLR->m_itemPos = itemPos;

	size_t nItemSize = 0;

	if (NULL != lpItem)
	{
		nItemSize = Item::MAX_ITEM_SIZE;

		char* lpItemBuffer = lpBuffer + sizeof(PktLotteryResult);
		if (false == lpItem->SerializeOut(lpItemBuffer, nItemSize))
		{
			usError = PktLotteryResult::SERVER_ERROR;
			ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCID);
		}
		lpPktLR->m_dwSize = static_cast<unsigned long>(nItemSize);
	}

	return SendStream.WrapHeader(static_cast<unsigned short>(sizeof(PktLotteryResult) + nItemSize), 
		CmdCharLotteryResult, 0, usError);
}

