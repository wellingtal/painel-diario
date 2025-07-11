
#include "stdafx.h"

#include <Item/Item.h>
#include <Item/Container/EquipmentsContainer.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/AddressPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#include <Map/FieldMap/Cell.h>
#include <Creature/Character/Character.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Ranking/RankingMgr.h>

#include "SendCharEtc.h"


bool GameClientSendPacket::SendCharBindPosition(CCharacter& character, unsigned long dwNPCID, 
												unsigned char cCmd, Position Pos, char cZone, unsigned short usError)
{
    CGameClientDispatch* lpDispatch = 0;

    if(0 != (lpDispatch = character.GetDispatcher()))
    {
        CSendStream& SendStream = lpDispatch->GetSendStream();

        char* lpBuffer = SendStream.GetBuffer(sizeof(PktBP));
	    if (NULL != lpBuffer) 
        {
	        PktBP* lpPktBPAck = reinterpret_cast<PktBP*>(lpBuffer);

	        lpPktBPAck->m_dwNPCID = dwNPCID;
	        lpPktBPAck->m_cCmd = cCmd;

	        lpPktBPAck->m_Pos.fPointX = Pos.m_fPointX;
	        lpPktBPAck->m_Pos.fPointY = Pos.m_fPointY;
	        lpPktBPAck->m_Pos.fPointZ = Pos.m_fPointZ;

	        lpPktBPAck->m_cZone = cZone;

            if(SendStream.WrapCrypt(sizeof(PktBP), CmdCharBindPosition, 0, usError))
            {
                CCell* lpCell = character.GetCellPos().m_lpCell;

                if(0 == usError && 0 != lpCell && PktBP::BP_RECALL == cCmd)
                {
                    // 리콜인 경우 주변에 이펙트 뿌림.
                    unsigned long   dwCID       = character.GetCID();
                    const Position& currentPos  = character.GetCurrentPos();

					// edith 2009.03.07 리콜 높이 조절
                    CastObject castObject;
                    castObject.m_wTypeID = 0;
                    castObject.m_cObjectType = 200;
                    castObject.m_cObjectLevel = 0;
                    castObject.m_DstPos.fPointX = currentPos.m_fPointX;
                    castObject.m_DstPos.fPointY = currentPos.m_fPointY+0.3f;
                    castObject.m_DstPos.fPointZ = currentPos.m_fPointZ;
                    
                    lpCell->SendCastObjectInfo(dwCID, dwCID, castObject);
                }

                return true;
            }
        }
    }

    return false;
}


bool GameClientSendPacket::SendCharHPRegen(CSendStream& SendStream, unsigned long dwCID, 
                                           unsigned short nNowHP, unsigned short nNowMP)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktHP));
	if (NULL == lpBuffer) { return false; }

	PktHP* lpPktHP = reinterpret_cast<PktHP*>(lpBuffer);

	lpPktHP->m_dwCharID = dwCID;
	lpPktHP->m_sHP      = nNowHP;
	lpPktHP->m_sMP      = nNowMP;

    return SendStream.WrapCrypt(sizeof(PktHP), CmdCharHPRegen, 0, 0);
}


bool GameClientSendPacket::SendCharQuickSlotMove(CSendStream& SendStream, 
												 TakeType& takeType, unsigned short wSkillID, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktQSM));
	if (NULL == lpBuffer) { return false; }

	PktQSM*     lpPktQSM    = reinterpret_cast<PktQSM*>(lpBuffer);

	lpPktQSM->m_TakeType	= takeType;
	lpPktQSM->m_usSkillID	= wSkillID;

    return SendStream.WrapCrypt(sizeof(PktQSM), CmdCharQuickSlotMove, 0, usError);
}


bool GameClientSendPacket::SendCharControlOption(CSendStream& SendStream, unsigned long dwCharID, RejectOption& Reject)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCOp));
	if (NULL == lpBuffer) { return false; }

	PktCOp* lpPktCOp = reinterpret_cast<PktCOp*>(lpBuffer);

	lpPktCOp->m_dwCharID = dwCharID;
	lpPktCOp->m_RejectOption = Reject;

    return SendStream.WrapCrypt(sizeof(PktCOp), CmdCharControlOption, 0, 0);
}

bool GameClientSendPacket::SendCharFameInfo(CSendStream& SendStream, CCharacter* lpRequestCharacter, 
											const char* szWinCharName, const char* szLoseCharName, 
											unsigned char cOtherNation, unsigned char cOtherClass,
											unsigned char cCmd, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktFIAck));
	if (NULL == lpBuffer) { return false; }

	PktFIAck* lpPktFIAck = reinterpret_cast<PktFIAck*>(lpBuffer);

	lpPktFIAck->m_dwCharID = lpRequestCharacter->GetCID();

	lpPktFIAck->m_cOtherNation = cOtherNation;
	lpPktFIAck->m_cOtherClass = cOtherClass;

	lpPktFIAck->m_dwFame = lpRequestCharacter->GetFame();		
	lpPktFIAck->m_dwMileage = lpRequestCharacter->GetMileage();
	lpPktFIAck->m_cRankingByRace = 
		CRankingMgr::GetInstance().GetRank(lpRequestCharacter->GetCharacterName(), 0);
	lpPktFIAck->m_cRankingByClass = 
		CRankingMgr::GetInstance().GetRank(lpRequestCharacter->GetCharacterName(), 
		static_cast<unsigned char>(lpRequestCharacter->GetClass()));

	lpPktFIAck->m_cCmd = cCmd;
	lpPktFIAck->m_KillCount= lpRequestCharacter->m_dwKilingCount;
	lpPktFIAck->m_KillspreeLevel= lpRequestCharacter->getKillSpreeLevel();

	strncpy(lpPktFIAck->m_szWinCharName, szWinCharName, PktFIAck::MAX_NAME_LEN);
	strncpy(lpPktFIAck->m_szLoseCharName, szLoseCharName, PktFIAck::MAX_NAME_LEN);

    return SendStream.WrapCrypt(sizeof(PktFIAck), CmdCharFameInfo, 0, usError);
}

bool GameClientSendPacket::SendCharAuthorizePanel(CSendStream& SendStream, CCharacter* lpCaster, unsigned char cCmd)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktAPAck));
	if (NULL == lpBuffer) { return false; }

	PktAPAck* lpPktAPAck = reinterpret_cast<PktAPAck *>(lpBuffer);

	lpPktAPAck->m_dwCasterID = lpCaster->GetCID();
	strncpy(lpPktAPAck->m_szCasterName, lpCaster->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
	lpPktAPAck->m_cCmd = cCmd;

	return SendStream.WrapCrypt(sizeof(PktAPAck), CmdCharAuthorizePanel, 0, 0);
}

bool GameClientSendPacket::SendConfigInfoDB(CSendStream& SendStream, CCharacter* lpCharacter)
{
	const int MAX_BUFFER = sizeof(PktConfigInfo) + PktConfigInfo::MAX_BUFFER_SIZE;
	char szBuffer[MAX_BUFFER];

	PktConfigInfo* lpPktConfigInfo = reinterpret_cast<PktConfigInfo *>(szBuffer);
	unsigned short wSize = 0;

	lpPktConfigInfo->m_dwUserID = lpCharacter->GetUID();
	lpPktConfigInfo->m_dwCharID = lpCharacter->GetCID();

	// 반전/전투 모드 (반전 모드는 없어졌지만 DB 수정이 불편하므로 공간은 유지 (2005-05-31 by 로딘)
	PeaceModeInfo* lpPeaceModeInfo = reinterpret_cast<PeaceModeInfo *>(szBuffer + sizeof(PktConfigInfo));
//	*lpPeaceModeInfo = lpCharacter->GetPeaceMode();
	wSize += sizeof(PeaceModeInfo);

	// 거부 옵션
	RejectOption* lpRejectOption = reinterpret_cast<RejectOption *>(szBuffer + sizeof(PktConfigInfo) + wSize);
	*lpRejectOption = lpCharacter->GetRejectOption();
	wSize += sizeof(RejectOption);

	lpPktConfigInfo->m_wSize = wSize;

	return SendStream.WrapCompress(szBuffer, static_cast<unsigned short>(sizeof(PktConfigInfo) + lpPktConfigInfo->m_wSize), 
		CmdConfigInfoDB, 0, 0);
}

bool GameClientSendPacket::SendCharEliteBonus(CSendStream& SendStream, char cEliteBonus)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktEB));
	if (NULL == lpBuffer) { return false; }

	PktEB* lpPktEB = reinterpret_cast<PktEB*>(lpBuffer);

	lpPktEB->m_cEliteBonus = cEliteBonus;

	return SendStream.WrapCrypt(sizeof(PktEB), CmdEliteBonus, 0, 0);
}


bool GameClientSendPacket::SendCharAdmin(CSendStream& SendStream, PktAdmin* pPktAdmin, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktAdmin));
	if (NULL == lpBuffer) { return false; }

	PktAdmin* lpPktAdmin = reinterpret_cast<PktAdmin*>(lpBuffer);

	memcpy(lpPktAdmin, pPktAdmin, sizeof(PktAdmin));

	return SendStream.WrapCrypt(sizeof(PktAdmin), CmdCharAdminCmd, 0, wError);
}

// 캐릭터 특수 이벤트 관련
bool GameClientSendPacket::SendExtraEvent(CSendStream& SendStream, unsigned long dwUserID, unsigned long dwCharID, unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError)
{
	// edith 2009.09.05 신규 이벤트 제작부분
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktExtraEvent));
	if (NULL == lpBuffer)
	{
		return false;
	}

	PktExtraEvent* lpPktEE = reinterpret_cast<PktExtraEvent *>(lpBuffer);

	lpPktEE->m_dwUserID = dwUserID;
	lpPktEE->m_dwCharID = dwCharID;
	lpPktEE->m_dwEventID = dwEventID;
	lpPktEE->m_dwValue1 = dwValue1;
	lpPktEE->m_dwValue2 = dwValue2;

	return SendStream.WrapCrypt(sizeof(PktExtraEvent), CmdExtraEvent, 0, usError);

}


