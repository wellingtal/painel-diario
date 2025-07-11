
#include "stdafx.h"

#include "SendCharUpgradeItem.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>


bool GameClientSendPacket::SendCharInstallSocket(CSendStream& SendStream, unsigned long dwCharID, 
												 Item::ItemPos EquipPos, Item::ItemPos GemPos,
                                                 Item::CEquipment* lpEquipment, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktISAck) + Item::MAX_ITEM_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktISAck* lpPktISAck = reinterpret_cast<PktISAck *>(lpBuffer);

    size_t nItemSize = 0;
    if (NULL != lpEquipment)
    { 
        nItemSize = Item::MAX_ITEM_SIZE;
		if (false == lpEquipment->SerializeOut(lpBuffer + sizeof(PktISAck), nItemSize))
        {
            ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCharID);
        }
    }

	lpPktISAck->m_dwCharID		= dwCharID;
	lpPktISAck->m_equipPos		= EquipPos;
	lpPktISAck->m_gemPos		= GemPos;

	lpPktISAck->m_cSize			= static_cast<unsigned char>(nItemSize);

    return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktISAck) + nItemSize), 
		CmdCharInstallSocket, 0, wError);
}

bool GameClientSendPacket::SendCharItemChemical(CSendStream& SendStream, unsigned long dwCharID, 
												Item::ItemPos PickkingPos, Item::ItemPos TargetPos, 
												unsigned short wPickkingID, unsigned char cPickkingNum, 
												Item::CItem* lpResultItem, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktItemChemicalAck) + Item::MAX_ITEM_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktItemChemicalAck* lpPktICAck = reinterpret_cast<PktItemChemicalAck *>(lpBuffer);

	size_t nItemSize = 0;
	if (NULL != lpResultItem)
	{
		nItemSize = Item::MAX_ITEM_SIZE;
		if (false == lpResultItem->SerializeOut(lpBuffer + sizeof(PktItemChemicalAck), nItemSize))
		{
			ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCharID);
		}
	}

	lpPktICAck->m_dwCharID		= dwCharID;

	lpPktICAck->m_pickkingPos	= PickkingPos;
	lpPktICAck->m_targetPos		= TargetPos;

	lpPktICAck->m_wPickkingID	= wPickkingID;
	lpPktICAck->m_cPickkingNum	= cPickkingNum;

	lpPktICAck->m_cResultSize	= static_cast<unsigned char>(nItemSize);

	return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktItemChemicalAck) + nItemSize), 
		CmdCharItemChemical, 0, wError);
}

bool GameClientSendPacket::SendCharUpgradeItem(CSendStream& SendStream, unsigned long dwCharID, 
                                               unsigned long dwCurrentGold, Item::CItem* lpItem, 
                                               unsigned char cCurrentMineralNum, unsigned char cValue, unsigned short wError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUgIAck) + Item::MAX_ITEM_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktUgIAck* lpPktUgIAck = reinterpret_cast<PktUgIAck *>(lpBuffer);

    size_t nItemSize = 0;
    if (NULL != lpItem)
    {
        nItemSize = Item::MAX_ITEM_SIZE;
        if (false == lpItem->SerializeOut(lpBuffer + sizeof(PktUgIAck), nItemSize))
        {
            ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCharID);
        }
    }

    lpPktUgIAck->m_dwCharID             = dwCharID;
    lpPktUgIAck->m_dwCurrentGold        = dwCurrentGold;
    lpPktUgIAck->m_cCurrentMineralNum   = cCurrentMineralNum;
	lpPktUgIAck->m_cValue				= cValue;
    lpPktUgIAck->m_cSize                = static_cast<unsigned char>(nItemSize);

    return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktUgIAck) + nItemSize),
        CmdCharUpgradeItem, 0, wError);
}

bool GameClientSendPacket::SendCharItemOptionGraft(CSendStream& SendStream, unsigned long dwCharID, 
												   unsigned long dwCurrentGold, Item::CItem* lpResultItem, 
												   unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktItemOptionGraftAck) + Item::MAX_ITEM_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktItemOptionGraftAck* lpPktIOGAck = reinterpret_cast<PktItemOptionGraftAck *>(lpBuffer);

	size_t nItemSize = 0;
	if (NULL != lpResultItem)
	{
		nItemSize = Item::MAX_ITEM_SIZE;
		if (false == lpResultItem->SerializeOut(lpBuffer + sizeof(PktItemOptionGraftAck), nItemSize))
		{
			ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCharID);
		}
	}

	lpPktIOGAck->m_dwCharID     = dwCharID;
	lpPktIOGAck->m_dwGold		= dwCurrentGold;
	lpPktIOGAck->m_cSize		= static_cast<unsigned char>(nItemSize);

	return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktItemOptionGraftAck) + nItemSize),
		CmdCharItemOptionGraft, 0, wError);
}

bool GameClientSendPacket::SendCharItemCompensation(CSendStream& SendStream, unsigned long dwCharID, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktItemCompensation));
	if (NULL == lpBuffer) { return false; }

	PktItemCompensation* lpPICAck = reinterpret_cast<PktItemCompensation *>(lpBuffer);

	lpPICAck->m_dwCharID		= dwCharID;

	return SendStream.WrapCrypt(sizeof(PktItemCompensation), CmdCharItemCompensation, 0, wError);
}

bool GameClientSendPacket::SendCharRuneInstallSocket(CSendStream& SendStream, unsigned long dwCharID, Item::ItemPos EquipPos, 
												 Item::ItemPos RunePos, Item::CEquipment* lpEquipment, unsigned char cType, unsigned short wError)
												 
{
	// 룬 아이템 설치/삭제 패킷을 클라이언트로 보낸다.

	char* lpBuffer = SendStream.GetBuffer(sizeof(PktIRSAck) + Item::MAX_ITEM_SIZE);

	if(!lpBuffer)
	{ 
		return false;
	}

	PktIRSAck* lpPktIRSAck = reinterpret_cast<PktIRSAck*>(lpBuffer);

	size_t nItemSize = 0;

	if(lpEquipment)
	{ 
		nItemSize = Item::MAX_ITEM_SIZE;

		if(!lpEquipment->SerializeOut(lpBuffer + sizeof(PktIRSAck), nItemSize))
		{
			ERRLOG1(g_Log, "CID:0x%08x 아이템 SerializeOut에 실패했습니다.", dwCharID);
		}
	}

	lpPktIRSAck->m_dwCharID			= dwCharID;
	lpPktIRSAck->m_equipPos			= EquipPos;
	lpPktIRSAck->m_runePos			= RunePos;	
	lpPktIRSAck->m_cSize			= static_cast<unsigned char>(nItemSize);
	lpPktIRSAck->m_cType			= cType;

	return SendStream.WrapCrypt(static_cast<unsigned short>(sizeof(PktIRSAck) + nItemSize), 
		CmdCharInstallRuneSocket, 0, wError);	
}