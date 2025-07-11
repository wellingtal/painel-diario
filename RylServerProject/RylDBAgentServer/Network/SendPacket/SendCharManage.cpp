#include "stdafx.h"
#include "SendCharManage.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Utility/Setup/ServerSetup.h>

#include <DB/DBdefine.h>

namespace DBAgent
{
namespace SendPacket
{

// WORK_LIST 2.1 계정 국적 추가
// 캐릭터 뷰 얻어오기
bool UserLogin(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID, 
    const CHAR_VIEW* lpCharView, const sGuildData* lpGuildData, const STORE_INFO* pStoreInfo, unsigned long dwCharViewNum, 
	unsigned char cAccountNation, int nPlayTime, unsigned char cAdminLevel, unsigned char cBillingUser,
    unsigned char cBillingType, unsigned char cLoginType, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktULDAck));
    if(0 != lpBuffer)
    {
        PktULDAck* lpPktULDAck = reinterpret_cast<PktULDAck*>(lpBuffer);

        lpPktULDAck->m_dwRequestKey = dwRequestKey;
        lpPktULDAck->m_wCmd         = PktDD::SCmdUserLogin;

        lpPktULDAck->m_nPlayTime    = nPlayTime;
        lpPktULDAck->m_usAdminLevel = cAdminLevel;
        lpPktULDAck->m_cBillingUser = cBillingUser;
        lpPktULDAck->m_cBillingType = cBillingType;

        lpPktULDAck->m_cLoginType   = cLoginType;

        lpPktULDAck->m_dwUserID     = dwUID;

		// WORK_LIST 2.1 계정 국적 추가
		lpPktULDAck->m_cAccountNation = cAccountNation;

        memset(lpPktULDAck->m_CharView, 0, sizeof(CHAR_VIEW) * PktULDAck::MAX_CHAR_VIEW);
		memset(lpPktULDAck->m_GuildData, 0, sizeof(sGuildData) * PktULDAck::MAX_CHAR_VIEW);
		memset(&lpPktULDAck->m_sStoreInfo, 0, sizeof(STORE_INFO));

        if(0 != lpCharView && 0 != dwCharViewNum)
        {
            memcpy(lpPktULDAck->m_CharView, lpCharView, 
                sizeof(CHAR_VIEW) * std::min(unsigned long(PktULDAck::MAX_CHAR_VIEW), dwCharViewNum));

			memcpy(lpPktULDAck->m_GuildData, lpGuildData, sizeof(sGuildData) * PktULDAck::MAX_CHAR_VIEW);	

			memcpy(&lpPktULDAck->m_sStoreInfo, pStoreInfo, sizeof(STORE_INFO));
        }

        return SendStream.WrapHeader(sizeof(PktULDAck), CmdDBGetData, 0, usError);
    }

    return false;
}


// 캐릭터 선택.
bool CharSelect(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned char cZone, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCSDAck));

    if(0 != lpBuffer)
    {
        PktCSDAck* lpPktCSDAck = reinterpret_cast<PktCSDAck*>(lpBuffer);

        lpPktCSDAck->m_dwRequestKey = dwRequestKey;
        lpPktCSDAck->m_wCmd         = PktDD::SCmdCharSelect;
        lpPktCSDAck->m_cZone        = cZone;

        CUserPercentageInZone userPercentageInZone(
            lpPktCSDAck->m_wChannelNum, cZone, PktCSDAck::MAX_CHANNEL_NUM);

        CGameDispatch::GetDispatchTable().Process(userPercentageInZone);

        return SendStream.WrapHeader(sizeof(PktCSDAck), CmdDBGetData, 0, usError);
    }

    return false;
}

bool CharCreate(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned long dwCID, unsigned long dwSlot, 
                const CHAR_VIEW& CharView, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCCDAck));

    if(0 != lpBuffer)
    {
        PktCCDAck* lpPktCCDAck = reinterpret_cast<PktCCDAck*>(lpBuffer);

        lpPktCCDAck->m_dwRequestKey = dwRequestKey;
        lpPktCCDAck->m_wCmd         = PktDD::SCmdCharCreate;

        lpPktCCDAck->m_dwCharID     = dwCID;
        lpPktCCDAck->m_dwSlot       = dwSlot;
        lpPktCCDAck->m_CharView     = CharView;

        return SendStream.WrapHeader(sizeof(PktCCDAck), CmdDBGetData, 0, usError);
    }

    return false;
}

bool CharDelete(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned long dwUID, unsigned long dwSlot, 
                unsigned long dwCID, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCDD));

    if(0 != lpBuffer)
    {
        PktCDD* lpPktCDDAck = reinterpret_cast<PktCDD*>(lpBuffer);

        lpPktCDDAck->m_dwRequestKey = dwRequestKey;
        lpPktCDDAck->m_wCmd         = PktDD::SCmdCharDelete;

        lpPktCDDAck->m_dwUserID     = dwUID;
        lpPktCDDAck->m_dwSlotNum    = dwSlot;
        lpPktCDDAck->m_dwCharID     = dwCID;

        return SendStream.WrapHeader(sizeof(PktCDD), CmdDBGetData, 0, usError);
    }

    return false;
}

// WORK_LIST 2.1 계정 국적 추가
bool SelectAccountNation(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
						 unsigned char cType, unsigned char cAccountNation, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktSAND));

	if (0 != lpBuffer)
	{
		PktSAND* lpPktSANDAck = reinterpret_cast<PktSAND*>(lpBuffer);

		lpPktSANDAck->m_dwRequestKey	= dwRequestKey;
		lpPktSANDAck->m_wCmd			= PktDD::SCmdSelectNation;

		lpPktSANDAck->m_dwUserID	    = dwUID;
		lpPktSANDAck->m_cType			= cType;
		lpPktSANDAck->m_cAccountNation  = cAccountNation;

		return SendStream.WrapHeader(sizeof(PktSAND), CmdDBGetData, 0, usError);
	}

	return false;
}

// WORK_LIST 2.3 계정 국적 변경 기능 구현
bool NationChangeResult(CSendStream& SendStream, unsigned long dwRequestKey, unsigned long dwUID,
						unsigned long* aryGID, unsigned long* aryFame)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktANCAck));

	if (0 != lpBuffer)
	{
		PktANCAck* lpPktANCAck = reinterpret_cast<PktANCAck*>(lpBuffer);

		lpPktANCAck->m_dwRequestKey	= dwRequestKey;
		lpPktANCAck->m_wCmd			= PktDD::SCmdNationChangeResult;

		lpPktANCAck->m_dwUserID	    = dwUID;
		::memcpy(lpPktANCAck->m_dwGID, aryGID, sizeof(unsigned long) * PktANCAck::MAX_CHAR_VIEW);
		::memcpy(lpPktANCAck->m_dwFame, aryFame, sizeof(unsigned long) * PktANCAck::MAX_CHAR_VIEW);

		return SendStream.WrapHeader(sizeof(PktANCAck), CmdDBGetData, 0, 0);
	}

	return false;
}

bool UnifiedCharInfo(CSendStream& SendStream, unsigned long dwUID, 
                     unsigned char cTransferedCharCount, const USER_INFO& userInfo, 
                     const UnifiedStoreInfo* lpUnifiedStoreInfo, unsigned long dwUnifiedStoreNum,
                     const UnifiedCharData* lpUnifiedCharData, unsigned long dwUnifiedCharNum)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUnifiedCharInfo));

    if (0 != lpBuffer)
    {
        PktUnifiedCharInfo* lpPktUnifiedCharInfo = 
            reinterpret_cast<PktUnifiedCharInfo*>(lpBuffer);

        lpPktUnifiedCharInfo->dwUID = dwUID;

        unsigned long dwMinUnifiedStoreNum = 
            std::min(unsigned long(PktUnifiedCharInfo::MAX_STORE_INFO), dwUnifiedStoreNum);

        unsigned long dwMinUnifiedCharNum = 
            std::min(unsigned long(PktUnifiedCharInfo::MAX_CHAR_DATA), dwUnifiedCharNum);

        lpPktUnifiedCharInfo->cRestrictedPart1ToPart2Level = 
            CServerSetup::GetInstance().GetRestrictedPart1ToPart2Level();

        unsigned char cMaxTransferCount = CServerSetup::GetInstance().GetMaxTransferPart1ToPart2Count();

        if (cTransferedCharCount <= cMaxTransferCount)
        {
            lpPktUnifiedCharInfo->cRemainCharTransferCount = cMaxTransferCount - cTransferedCharCount;
        }
        else
        {
            lpPktUnifiedCharInfo->cRemainCharTransferCount = 0;
        }

        lpPktUnifiedCharInfo->userInfo = userInfo;

        memcpy(lpPktUnifiedCharInfo->unifiedStoreInfo, lpUnifiedStoreInfo, 
            sizeof(UnifiedStoreInfo) * dwMinUnifiedStoreNum);

        memcpy(lpPktUnifiedCharInfo->unifiedCharData, lpUnifiedCharData,
            sizeof(UnifiedCharData) * dwMinUnifiedCharNum);

        lpPktUnifiedCharInfo->cStoreInfoNum = static_cast<unsigned char>(dwMinUnifiedStoreNum);
        lpPktUnifiedCharInfo->cCharDataNum = static_cast<unsigned char>(dwMinUnifiedCharNum);

        return SendStream.WrapHeader(sizeof(PktUnifiedCharInfo), CmdUnifiedCharInfo, 0, 0);
    }

    return false;
}

bool UnifiedCharSelectReq(CSendStream& SendStream, unsigned long dwUID,                           
                          unsigned long dwRequestKey, unsigned long* lpdwCID, unsigned long dwCIDNum,  
                          unsigned char cSelectedStoreServerGroup, unsigned char cSelectedNation)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUnifiedCharSelectReq));

    if (0 != lpBuffer)
    {
        PktUnifiedCharSelectReq* lpPktUnifiedCharSelectReq = 
            reinterpret_cast<PktUnifiedCharSelectReq*>(lpBuffer);

        memset(lpPktUnifiedCharSelectReq->szPassword, 
            0, sizeof(char) * PktUnifiedCharSelectReq::MAX_PASSWORD_LEN);

        lpPktUnifiedCharSelectReq->dwUID= dwUID;        
        lpPktUnifiedCharSelectReq->dwRequestKey = dwRequestKey;

        memcpy(lpPktUnifiedCharSelectReq->dwCID, lpdwCID, 
            sizeof(unsigned long) * min(long(USER_INFO::MAX_CHAR_NUM), long(dwCIDNum)));

        lpPktUnifiedCharSelectReq->cSelectedServerGroupID = cSelectedStoreServerGroup;
        lpPktUnifiedCharSelectReq->cSelectedNation = cSelectedNation;
        
        return SendStream.WrapHeader(sizeof(PktUnifiedCharSelectReq), CmdUnifiedCharSelect, 0, 0);
    }

    return false;
}


bool UnifiedCharSelectAck(CSendStream& SendStream, unsigned long dwUID, 
                          unsigned long dwRequestKey, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUnifiedCharSelectAck));

    if (0 != lpBuffer)
    {
        PktUnifiedCharSelectAck* lpPktUnifiedCharSelectAck = 
            reinterpret_cast<PktUnifiedCharSelectAck*>(lpBuffer);

        lpPktUnifiedCharSelectAck->dwUID = dwUID;
        lpPktUnifiedCharSelectAck->dwRequestKey = dwRequestKey;

        return SendStream.WrapHeader(sizeof(PktUnifiedCharSelectAck), 
            CmdUnifiedCharSelect, 0, usError);
    }

    return false;
}

bool CharUseCashItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
						Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError)
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

bool CharExtraEvent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwCID,
					 unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError)
{
	// edith 2009.09.05 신규 이벤트 제작부분

	char* lpBuffer = SendStream.GetBuffer(sizeof(PktExtraEvent));

	if (NULL != lpBuffer)
	{
		PktExtraEvent* lpPktEE = reinterpret_cast<PktExtraEvent*>(lpBuffer);

		lpPktEE->m_dwUserID         = dwUID;
		lpPktEE->m_dwCharID         = dwCID;
		lpPktEE->m_dwEventID        = dwEventID;
		lpPktEE->m_dwValue1			= dwValue1;
		lpPktEE->m_dwValue2			= dwValue2;

		return SendStream.WrapCrypt(sizeof(PktExtraEvent), CmdExtraEvent, 0, usError);
	}

	return false;
}

}
}