
#include "stdafx.h"

#include <Utility/Setup/ServerSetup.h>

#include <Log/LogStruct.h>
#include <Log/ItemLog.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/ServerRequest.h>
#include <Network/Dispatch/GameClient/SendCharLoginOut.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Dispatch/GameClient/SendCharAdmin.h>
#include <Network/Dispatch/GameClient/ParseCharAdmin.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/GameEventPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#include <Network/Packet/PacketStruct/CharStatusPacket.h>

#include <Network/Dispatch/DBAgent/DBAgentPacketParse.h>

#include <GameTime/GameTimeConstants.h>

#include <Creature/Monster/Monster.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Monster/AwardTable.h>

#include <Community/Party/PartyMgr.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/ItemStructure.h>
#include <Ranking/RankingMgr.h>
#include <Quest/QuestMgr.h>

#include "DBAgentRequest.h"
#include "DBAgentDispatch.h"
#include "DBAgentPacketParse.h"

#include <Creature/Threat.h>

struct Respawn
{
	bool operator() (CCharacter* lpCharacter)
	{
		if (NULL != lpCharacter)
		{
			lpCharacter->Respawn();
		}

		return true;
	}
};

bool DBAgentPacketParse::SendAbnormalLogout(unsigned long dwUID, unsigned long dwCID, unsigned long dwSessionID, 
                                            unsigned long dwRequestKey, CGameClientDispatch* lpGameClientDispatch)
{
    GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{                        
        CSendStream& agentSendStream = lpDBAgentDispatch->GetSendStream();
        
        PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(
            agentSendStream.GetBuffer(sizeof(PktDBUpdate)));
        
        if (0 != lpPktDBUpdate)
        {
            memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));

            lpPktDBUpdate->m_dlItemSerial	= Item::CItemFactory::GetInstance().GetItemUID();
            lpPktDBUpdate->m_dwSessionID    = dwSessionID;
            lpPktDBUpdate->m_dwUserID		= dwUID;
            lpPktDBUpdate->m_dwCharID		= dwCID;
            lpPktDBUpdate->m_TypeCode		= DBUpdateData::ABNORMAL_LOGOUT;
                        
            lpPktDBUpdate->m_dwRequestKey   = 0;
            lpPktDBUpdate->m_Address.S_un.S_addr = 0;
            lpPktDBUpdate->m_cAdminLevel    = 0;
			
            // WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현            
			lpPktDBUpdate->m_cAccountNation	= 0;

            return agentSendStream.WrapHeader(sizeof(PktDBUpdate), CmdDBUpdateData, 0, 0);
        }
    }

    unsigned long dwDispatchUID = 0;
    if (0 != lpGameClientDispatch)
    {
        dwDispatchUID = lpGameClientDispatch->GetUID();
    }

    ERRLOG5(g_Log, "UID:%d/CID:0x%08x/DP:0x%p/RequestKey:%d/DPUID:%d/DBRequest Failed - "
        "Login failed. But cannot send abnormal logout packet", 
        dwUID, dwCID, lpGameClientDispatch, dwRequestKey, dwDispatchUID);

    return false;
}

bool DBAgentPacketParse::ParseDepositUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    PktDepositUpdateDB* lpPktDepoistUpdateDB = static_cast<PktDepositUpdateDB*>(lpPktBase);

    // 패킷 길이 검사.
    CHECK_PACKET_SIZE(

        size_t nPacketLength = lpPktBase->GetLen();

        if (nPacketLength < sizeof(PktDepositUpdateDB))
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 최소 크기:%d, 현재 크기:%d", sizeof(PktDepositUpdateDB), nPacketLength);
            return false;
        }

        size_t nTotalLength = lpPktDepoistUpdateDB->m_usDataSize + sizeof(PktDepositUpdateDB);
        if (nTotalLength != nPacketLength)
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 패킷 크기:%d, 전체 크기:%d", nPacketLength, nTotalLength);
            return false;
        }
    );

    CServerRequest::Result result(lpPktDepoistUpdateDB->m_dwRequestKey, 
        lpPktDepoistUpdateDB->m_bUpdateComplete);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    unsigned long dwCID = lpPktDepoistUpdateDB->m_dwCID;

    if (0 != lpGameClientDispatch)
    {
        CCharacter* lpCharacter = lpGameClientDispatch->GetCharacter();

        if (0 != lpCharacter && dwCID == lpCharacter->GetCID())
        {
            Item::CDepositContainer& Deposit = lpCharacter->GetDeposit();

            // 반드시 먼저 넣어야 한다.
            Deposit.SetTabFlag(lpPktDepoistUpdateDB->m_dwTabFlag);

            if (!Deposit.SerializeIn(reinterpret_cast<char*>(lpPktDepoistUpdateDB + 1), lpPktDepoistUpdateDB->m_usDataSize))
            {
                ERRLOG1(g_Log, "CID:0x%08x 창고 데이터의 업데이트에 실패했습니다.", dwCID);
            }
            
            // 업데이트가 완료되는 경우, 클라이언트로 로그인을 보낸다.
            CGameClientDispatch* lpGameClientDispatch = lpCharacter->GetDispatcher();
            if (lpPktDepoistUpdateDB->m_bUpdateComplete && 0 != lpGameClientDispatch)
            {
                LOG_INOUT(DETLOG6(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchUID:%d/DispatchPointer:0x%p 로그인을 처리합니다.(%s) "
				    "DBAgent에서 로그인을 받아 캐릭터에 세팅했습니다.", 
                    lpCharacter->GetUID(), dwCID, lpCharacter, lpGameClientDispatch->GetUID(), 
                    lpGameClientDispatch, (lpCharacter->IsPeaceMode()) ? "평화모드" : "전쟁모드"));

                lpCharacter->PrepareLogin();

	            // 캐릭터 로그인 보내기
                return GameClientSendPacket::SendCharLogin(lpGameClientDispatch->GetSendStream(), 
					lpCharacter, &lpGameClientDispatch->GetRemoteAddr().get_addr_in(), PktBase::NO_SERVER_ERR);
            }
        }
        else
        {
            ERRLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/CharacterCID:0x%08x/ Invalid CID",
                lpPktDepoistUpdateDB->m_dwUID, lpPktDepoistUpdateDB->m_dwCID, lpCharacter,
                lpPktDepoistUpdateDB->m_dwRequestKey, (0 != lpCharacter) ? lpCharacter->GetCID() : 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%d/CID:0x%08x/RequestKey:%d/ DBRequest Failed - Requestkey is invalid",
            lpPktDepoistUpdateDB->m_dwUID, lpPktDepoistUpdateDB->m_dwCID, lpPktDepoistUpdateDB->m_dwRequestKey);
    }

    return true;
}


bool DBAgentPacketParse::ParseDepositCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktDeposit), DBAgentDispatch);

    PktDeposit* lpPktDeposit = static_cast<PktDeposit*>(lpPktBase);

    unsigned long dwCID = *reinterpret_cast<unsigned long*>(lpPktDeposit->m_szData);

    CServerRequest::Result result(lpPktDeposit->m_dwRequestKey, false);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    if (0 != lpGameClientDispatch)
    {
        CCharacter* lpCharacter = lpGameClientDispatch->GetCharacter();

        if (0 != lpCharacter && dwCID == lpCharacter->GetCID())
        {
            switch (lpPktDeposit->m_cCmd)
            {
            case PktDeposit::PASSWORD:

                lpCharacter->GetDeposit().SetPassword(lpPktDeposit->m_szData + 
                    sizeof(unsigned long), Deposit::PASSWORD_LENGTH);
                break;

            case PktDeposit::GOLD:

                lpCharacter->GetDeposit().AddGold(
                    *reinterpret_cast<unsigned long*>(lpPktDeposit->m_szData + sizeof(unsigned long)));
                break;
            };
        }
        else
        {
            ERRLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/CharacterCID:0x%08x/ Invalid CID",
                0, dwCID, lpCharacter, lpPktDeposit->m_dwRequestKey, 
                (0 != lpCharacter) ? lpCharacter->GetCID() : 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%d/CID:0x%08x/RequestKey:%d/ DBRequest Failed - Requestkey is invalid",
            0, dwCID, lpPktDeposit->m_dwRequestKey);
    }

    return true;
}

bool DBAgentPacketParse::ParseConfigInfoDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    PktConfigInfo* lpConfigDB = static_cast<PktConfigInfo*>(lpPktBase);

	unsigned long dwUserID = lpConfigDB->m_dwUserID;
	unsigned long dwCharID = lpConfigDB->m_dwCharID;
    unsigned short wSize = lpConfigDB->m_wSize;

	char* lpDataPointer = reinterpret_cast<char *>(lpConfigDB) + sizeof(PktConfigInfo);

    CServerRequest::Result result(lpConfigDB->m_dwRequestKey, false);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    // 클라이언트를 찾아 업데이트한다.    
    if (0 != lpGameClientDispatch)
    {
        CCharacter* lpCharacter = lpGameClientDispatch->GetCharacter();
        
        if (0 != lpCharacter && dwCharID == lpCharacter->GetCID())
        {
            if (0 != wSize)
            {
				// 반전 모드는 개념만이 남아있지만, DB 구조 변경이 불편하므로 껍데기만 남겨둔다. (2005-05-31 by 로딘)
				PeaceModeInfo PeaceMode;
		        PeaceModeInfo* lpPeaceModeInfo = reinterpret_cast<PeaceModeInfo *>(lpDataPointer);
		        PeaceMode = *lpPeaceModeInfo;
//		        lpCharacter->SetPeaceMode(PeaceMode, true);
		        lpDataPointer += sizeof(PeaceModeInfo);

		        RejectOption Reject;
		        RejectOption* lpRejectOption = reinterpret_cast<RejectOption *>(lpDataPointer);
		        Reject = *lpRejectOption;
		        lpCharacter->ControlOption(Reject, true);
		        lpDataPointer += sizeof(RejectOption);
            }
        }
        else
        {
            ERRLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/CharacterCID:0x%08x/ Invalid CID",
                dwUserID, dwCharID, lpCharacter, lpConfigDB->m_dwRequestKey, 
                (0 != lpCharacter) ? lpCharacter->GetCID() : 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%d/CID:0x%08x/RequestKey:%d/ DBRequest Failed - Requestkey is invalid",
            dwUserID, dwCharID, lpConfigDB->m_dwRequestKey);
    }

	return true;
}


bool DBAgentPacketParse::ParseUpdateDBData(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    PktDBUpdate* lpPktDBUpdate = static_cast<PktDBUpdate*>(lpPktBase);

    // 패킷 길이 검사.
    CHECK_PACKET_SIZE(

        size_t nPacketLength = lpPktBase->GetLen();

        if (nPacketLength < sizeof(PktDBUpdate))
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 최소 크기:%d, 현재 크기:%d", sizeof(PktDBUpdate), nPacketLength);
            return false;
        }

        unsigned short* lpUpdateBegin = lpPktDBUpdate->m_usUpdate;
        unsigned short* lpUpdatePastEnd = lpUpdateBegin + DBUpdateData::MAX_UPDATE_DB;
        size_t nTotalLength = std::accumulate(lpUpdateBegin, lpUpdatePastEnd, 0) + sizeof(PktDBUpdate);

        if (nTotalLength != nPacketLength)
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 패킷 크기:%d, 전체 크기:%d", nPacketLength, nTotalLength);
            return false;
        }
    );

    unsigned long   dwSessionID     = lpPktDBUpdate->m_dwSessionID;
    unsigned long   dwUID           = lpPktDBUpdate->m_dwUserID;
    unsigned long   dwCID           = lpPktDBUpdate->m_dwCharID;
    unsigned long   dwRequestKey    = lpPktDBUpdate->m_dwRequestKey;
	// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
	unsigned char	cAccountNation		= lpPktDBUpdate->m_cAccountNation;
    unsigned char   cNameChangeCount	= lpPktDBUpdate->m_cNameChangeCount;
	unsigned char	cGuildWarFlag		= lpPktDBUpdate->m_cGuildWarFlag;
	unsigned char	cRealmWarFlag		= lpPktDBUpdate->m_cRealmWarFlag;
	unsigned char	cRealmPoint			= lpPktDBUpdate->m_cRealmPoint;
	unsigned char	cTacticsFlag		= lpPktDBUpdate->m_cTacticsFlag;

    unsigned short  usTypeCode      = lpPktDBUpdate->m_TypeCode;
	unsigned int	dwPlayTime		= lpPktDBUpdate->m_PlayTime;
	long			lPremiumTime	= lpPktDBUpdate->m_PremiumTime;
	int				iPremiumType	= lpPktDBUpdate->m_PremiumType;

    unsigned short  wError          = lpPktDBUpdate->GetError();

    unsigned long dwDispatchUID = 0;

	CServerRequest::Result result(dwRequestKey, false);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    bool bSuccessGetDispatch = true;

	if (0 == lpGameClientDispatch)
	{
        ERRLOG4(g_Log, "UID:%d/CID:0x%08x/DP:0x%p/RequestKey:%d/ "
            "DBRequest Failed - Invalid GameClientDispatch",
            dwUID, dwCID, lpGameClientDispatch, dwRequestKey);

        wError = 1;
	}
    else
    {
        unsigned long dwDispatchRequestKey = lpGameClientDispatch->PopRequestKey();

        if (dwRequestKey != dwDispatchRequestKey)
        {
            ERRLOG6(g_Log, "UID:%d/CID:0x%08x(0x%p)/DP:0x%p/RequestKey:%d/DPRequestKey:%d/ "
                "DBRequest Failed - Requestkey is invalid",
                dwUID, dwCID, lpGameClientDispatch->GetCharacter(),
                lpGameClientDispatch, dwRequestKey, dwDispatchRequestKey);

            wError = 1;
        }
        else
        {
            DETLOG6(g_Log, "UID:%d/CID:0x%08x(0x%p)/DP:0x%p/RequestKey:%d/DPRequestKey:%d/ "
                "DBRequest Success - Requestkey is same",
                dwUID, dwCID, lpGameClientDispatch->GetCharacter(),
                lpGameClientDispatch, dwRequestKey, dwDispatchRequestKey);
        }

        dwDispatchUID = lpGameClientDispatch->GetUID();
        if (dwUID != dwDispatchUID)
        {
            ERRLOG7(g_Log, "UID:%d/CID:0x%08x(0x%p)/DP:0x%p/RequestKey:%d/DPRequestKey:%d/DPUID:%d "
                "DBRequest Failed - UID is Invalid", 
                dwUID, dwCID, lpGameClientDispatch->GetCharacter(), lpGameClientDispatch, 
                dwRequestKey, dwDispatchRequestKey, dwDispatchUID);

            wError = 1;
        }
    }

	if (0 == wError && (DBUpdateData::LOGIN != usTypeCode && DBUpdateData::ADMIN_LOGIN != usTypeCode)) 
    { 
        ERRLOG3(g_Log, "CID:0x%08x DBAgent로부터 이상한 데이터가 들어왔습니다. "
			"타입 코드는 %d, 유저 세션 포인터는 0x%p", dwCID, usTypeCode, lpGameClientDispatch);		

        wError = 1;
    }

    if (0 != wError)
	{
		ERRLOG3(g_Log, "UID:%d/CID:0x%08x 로그인 에러. %d", dwUID, dwCID, wError);
	}
    else
    {
	    // 캐릭터 생성
        CCharacter* lpCharacter = CCreatureManager::GetInstance().CreateCharacter(dwCID, dwSessionID);
	    if (0 == lpCharacter)
	    {
            wError = 1;
		    ERRLOG2(g_Log, "UID:%d/CID:0x%08x/Character create failed.", dwUID, dwCID);
	    }
        else if (!lpCharacter->Initialize(lpGameClientDispatch))
        {
            wError = 1;
		    ERRLOG2(g_Log, "UID:%d/CID:0x%08x/Character initialize failed.", dwUID, dwCID);
        }
        else
        {
	        // Admin권한 세팅
	        if (usTypeCode == DBUpdateData::ADMIN_LOGIN) 
	        {
                DETLOG3(g_Log, "UID:%d/CID:0x%08x Admin 권한을 가진 캐릭터가 로그인했습니다. 운영자 권한 : %d",
                    dwUID, dwCID, lpPktDBUpdate->m_cAdminLevel);

                lpCharacter->SetAdminLevel(lpPktDBUpdate->m_cAdminLevel);	
				lpCharacter->SetGMModelFlag(lpPktDBUpdate->m_cAdminFlag);
	        }

			// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
			// 캐릭터 계정 국적 세팅
			lpCharacter->SetNation(cAccountNation);
            lpCharacter->SetNameChangeCount(cNameChangeCount);
			lpCharacter->SetGuildWarFlag(cGuildWarFlag);
			lpCharacter->SetRealmWarFlag(cRealmWarFlag);
			lpCharacter->SetTacticsFlag(cTacticsFlag);
            lpCharacter->SetAccountName(lpPktDBUpdate->m_szAccountName);
			lpCharacter->SetRealmPoint(cRealmPoint);
			lpCharacter->SetPlayTime(dwPlayTime);
			lpCharacter->SetPremiumService(lPremiumTime, iPremiumType);
			

	        // 캐릭터 정보 세팅
	        if (false == lpCharacter->SetCharacterInfo(reinterpret_cast<char*>(lpPktDBUpdate + 1), lpPktDBUpdate->m_usUpdate))
            {
                wError = 1;
		        ERRLOG2(g_Log, "UID:%d/CID:0x%08x/CharacterInfo setting failed.", dwUID, dwCID);                
            }

			// 스킬의 무결성 체크
			lpCharacter->CheckSkillVaild();

	        // 캐릭터 메니저 등록 - 캐릭터 이름 Map때문에, 캐릭터 정보 세팅 후 등록해야 함. - by Sparrowhawk.
	        if (0 == wError && !CCreatureManager::GetInstance().AddCreature(lpCharacter))	
	        {
                wError = 1;
		        ERRLOG2(g_Log, "UID:%d/CID:0x%08x/CCreatureManager register failed.", dwUID, dwCID);		        
	        }

            if (0 != wError)
            {
                // 에러 발생했으면 전부 삭제~~
                lpGameClientDispatch->SetCharacter(0);
                CCreatureManager::GetInstance().DeleteCharacter(lpCharacter);
                lpCharacter = 0;
            }

            // 창고 데이터가 전부 오면 클라이언트로 로그인 보냄.

			// 채팅 금지 정보를 채팅서버에서 받아온다.
			GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, CChatDispatch::GetDispatchTable());

			if(lpChatDispatch)
			{
				char* lpBuffer = lpChatDispatch->GetSendStream().GetBuffer(sizeof(PktChatBanInfo));

				if(lpBuffer)
				{
					PktChatBanInfo* lpChaBanInfo = reinterpret_cast<PktChatBanInfo*>(lpBuffer);
					lpChaBanInfo->m_dwCID = dwCID;
					lpChatDispatch->GetSendStream().WrapCrypt(sizeof(PktChatBanInfo), CmdChatBanInfo, 0, 0);
				}
			}			
        }
    }

    if (0 != wError)
    {
        // 캐릭터 데이터 세팅에 실패했으니 비정상 로그아웃을 보냄.

        if (0 == lpPktDBUpdate->GetError())
        {
            // 로그인은 정상적으로 처리되었었으면, 로그인 취소를 보냄.
            SendAbnormalLogout(dwUID, dwCID, dwSessionID, dwRequestKey, lpGameClientDispatch);
        }

        if (0 != wError && 0 != lpGameClientDispatch)
        {
            // 에러가 들어왔으면 로그인 불가.
            return GameClientSendPacket::SendCharLogin(lpGameClientDispatch->GetSendStream(), 0, 0, wError);
        }
    }

    return true;
}


bool DBAgentPacketParse::ParseAgentParty(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    int nPacketLength = lpPktBase->GetLen();
    if (nPacketLength < sizeof(PktDD))
    {
        ERRLOG0(g_Log, "패킷 길이가 너무 작습니다.");
        return false;
    }

	PktDD*  lpPktDD = static_cast<PktDD*>(lpPktBase);
	CCharacterParty* lpParty = NULL;

	// 존에 파티원이 없어도 다른 존에 파티원이 로그인 하면 파티를 얻어온다.

	switch (lpPktDD->m_wCmd)
	{
	    case PktDD::SCmdCreateParty:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCPDAck), DBAgentDispatch);

			PktCPDAck* lpCPDAck = static_cast<PktCPDAck*>(lpPktBase);		

			lpParty = new CCharacterParty(lpCPDAck->m_Party, true);

			if (NULL == lpParty)
			{
				ERRLOG1(g_Log, "PID:0x%08x 파티 생성 실패.", lpCPDAck->m_Party.m_dwPartyID);					
			}
			else
			{
				PARTY party = lpCPDAck->m_Party;
				PARTYLOG(DETLOG3(g_Log, "파티 로그(생성) - PID:0x%08x 리더:0x%08x 가입자:0x%08x", 
					party.m_dwPartyID, party.m_dwLeaderID, party.MemberCID[1]));

				lpParty->UpdatePartyMemberInfo(lpCPDAck->m_dwLeaderID, lpCPDAck->m_dwLeaderGID, lpCPDAck->m_wLeaderClass, lpCPDAck->m_cLeaderLevel, true);
				lpParty->UpdatePartyMemberInfo(lpCPDAck->m_dwMemberID, lpCPDAck->m_dwMemberGID, lpCPDAck->m_wMemberClass, lpCPDAck->m_cMemberLevel, true);				
			}
            break;
		} 

		case PktDD::SCmdDeleteParty:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktDPD), DBAgentDispatch);

			PktDPD*         lpDPD       = static_cast<PktDPD*>(lpPktBase);
			unsigned long   dwPartyID   = lpDPD->m_dwPartyID;

			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL != lpParty)
			{
				if (false == lpParty->Destory(0, 0))
				{
					ERRLOG1(g_Log, "PID:0x%08x 파티 소멸 실패.", dwPartyID);
					break;
				}
				else
				{
					PARTYLOG(DETLOG1(g_Log, "파티 로그(해체) - PID:0x%08x", dwPartyID));
				}

				CPartyMgr::GetInstance().DeleteParty(lpDPD->m_dwPartyID);
			}
			break;
		} 

		case PktDD::SCmdGetPartyInfo:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPIDAck), DBAgentDispatch);

			PktPIDAck* lpPIDAck = static_cast<PktPIDAck*>(lpPktBase);
			
            CCharacterParty* lpParty = static_cast<CCharacterParty*>(
                CPartyMgr::GetInstance().GetParty(lpPIDAck->m_Party.m_dwPartyID));

            if (0 == lpParty)
            {
                lpParty = new CCharacterParty(lpPIDAck->m_Party, false);

			    if (0 == lpParty)
			    {
				    ERRLOG1(g_Log, "PID:0x%08x 파티 생성 실패.", lpPIDAck->m_Party.m_dwPartyID);					    
			    }
            }

            if (0 != lpParty)			
			{
				PARTY party = lpPIDAck->m_Party;
				PARTYLOG(DETLOG2(g_Log, "파티 로그(객체생성) - PID:0x%08x 리더:0x%08x", party.m_dwPartyID, party.m_dwLeaderID));

				if(lpPIDAck->m_dwCharID)
				{
					lpParty->UpdatePartyMemberInfo(lpPIDAck->m_dwCharID, lpPIDAck->m_dwGID, lpPIDAck->m_wClass, lpPIDAck->m_cLevel, true);

					// 접속한 캐릭터에게 다른존에 있는 캐릭터 정보를 보낸다 //
					lpParty->SendMemberInfoAllNZone(lpPIDAck->m_dwCharID);
				}				
			}
			break;
		} 

		case PktDD::SCmdLoginPartyMem:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*         lpPMD       = static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID	= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID	= lpPMD->m_dwSenderID;		
			unsigned long   dwGID		= lpPMD->m_dwGID;
			unsigned long	dwServerID	= lpPMD->m_dwServerID;
			unsigned short	wClass		= lpPMD->m_wClass;
			char			cLevel		= lpPMD->m_cLevel;

			if (PktPMD::FAIL_WRONG_PARTY == lpPMD->GetError())
			{
				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
				if (NULL != lpCharacter)
				{
					lpCharacter->SetPID(0);
				}

				ERRLOG1(g_Log, "CID:0x%08x 잘못된 파티 정보를 가지고 있습니다.", dwSenderID);
				break;
			}

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));

			if (NULL != lpParty)
			{
				lpParty->Login(dwSenderID, dwServerID);
				PARTYLOG(DETLOG2(g_Log, "파티 로그(로그인) - PID:0x%08x 로그인한놈:0x%08x", dwPartyID, dwSenderID));		

				lpParty->UpdatePartyMemberInfo(dwSenderID, dwGID, wClass, cLevel, true);

				// 같은 존에 있는 캐릭터에게 보낸다 //
				char* szPacket = reinterpret_cast<char*>(lpPMD);
				
				if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPMD), CmdCharPartyMemData, 0, 0))
				{
					lpParty->SendAllLoggedMember(
						szPacket, sizeof(PktPMD), 0, CmdCharPartyMemData);					
				}
				
				// 접속한 캐릭터에게 다른존에 있는 캐릭터 정보를 보낸다 //
				lpParty->SendMemberInfoAllNZone(dwSenderID);

			}
			else
			{
				if (CServerSetup::GetInstance().GetServerID() == dwServerID)
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if (0 == lpDBAgentDispatch)                    
                    {
                        ERRLOG0(g_Log, "DBAgent Dispatch 얻기 실패");
                        break;
                    }
                    else
                    {
                        CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
                        char* lpBuffer = SendStream.GetBuffer(sizeof(PktPID));
                        if (NULL != lpBuffer)
                        {
                            PktPID* lpPktPID = reinterpret_cast<PktPID*>(lpBuffer);

                            lpPktPID->m_wCmd        = PktDD::SCmdGetPartyInfo;
                            lpPktPID->m_dwCharID    = dwSenderID;
                            lpPktPID->m_dwPartyID   = dwPartyID;
							lpPktPID->m_dwGID		= dwGID;
							lpPktPID->m_dwServerID	= dwServerID;
							lpPktPID->m_wClass		= wClass;
							lpPktPID->m_cLevel		= cLevel;
                            
							PARTYLOG(DETLOG2(g_Log, "파티 로그(객체생성 요청) - PID:0x%08x 리더:0x%08x", dwPartyID, dwSenderID));
                            SendStream.WrapHeader(sizeof(PktPID), CmdAgentParty, 0, 0);
                        }
                    }
				}
			}
			break;
		} 

		case PktDD::SCmdLogoutPartyMem:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*         lpPMD       	= static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID		= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID		= lpPMD->m_dwSenderID;
			unsigned long   dwReferenceID	= lpPMD->m_dwGID;

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL != lpParty)
			{
				unsigned long dwLeader = lpParty->GetLeader();
				int result = lpParty->Logout(dwSenderID, dwReferenceID);

				// 다른 존 캐릭터에 로그아웃 정보를 캐릭터들에게 보낸다. //
				lpParty->SendMemberInfoOutNZone(dwSenderID, PktDD::SCmdLogoutPartyMem);

				if (1 == result)
				{
					// 파티 소멸 -- Fix Minbobo(정보는 항상 유지가 되어야 함).
					//PARTYLOG(DETLOG2(g_Log, "파티 로그(로그아웃::객체소멸) - PID:0x%08x 로그아웃한놈:0x%08x", dwPartyID, dwSenderID));
					//CPartyMgr::GetInstance().DeleteParty(dwPartyID);
					lpParty->SetLeader(dwReferenceID);
					break;
				}
				else
				{
					// 리더가 로그아웃 한 경우였다면...
					if (dwLeader == dwSenderID)
					{
						lpParty->TransferLeader(dwReferenceID);
						dwLeader = lpParty->GetLeader();
					}
				}

				PARTYLOG(DETLOG3(g_Log, "파티 로그(로그아웃) - PID:0x%08x 로그아웃한놈:0x%08x 리더:0x%08x", dwPartyID, dwSenderID, dwLeader));
			}
			break;
		} 

		
		case PktDD::SCmdInsertPartyMem:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*			lpPMD		= static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID	= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID	= lpPMD->m_dwSenderID;
			unsigned long   dwServerID	= lpPMD->m_dwServerID;
			unsigned short	wClass		= lpPMD->m_wClass;
			unsigned long	dwGID		= lpPMD->m_dwGID;
			char			cLevel		= lpPMD->m_cLevel;

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL != lpParty)
			{
				lpParty->Join(dwSenderID, dwServerID, lpPMD->m_strSenderName);

				lpParty->UpdatePartyMemberInfo(dwSenderID, dwGID, wClass, cLevel, true);			

				char* szPacket = reinterpret_cast<char*>(lpPMD);

				if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPMD), CmdCharPartyMemData, 0, 0))
				{
					lpParty->SendAllLoggedMember(
						szPacket, sizeof(PktPMD), 0, CmdCharPartyMemData);					
				}

				// 접속한 캐릭터에게 다른존에 있는 캐릭터 정보를 보낸다 //
				lpParty->SendMemberInfoAllNZone(dwSenderID);

				PARTYLOG(DETLOG2(g_Log, "파티 로그(가입) - PID:0x%08x 가입한놈:0x%08x", dwPartyID, dwSenderID));
			}
			break;
		} 

		case PktDD::SCmdDeletePartyMem:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*         lpPMD	        = static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID		= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID		= lpPMD->m_dwSenderID;
			unsigned long   dwReferenceID	= lpPMD->m_dwGID;			

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL != lpParty)
			{
				unsigned long dwLeader = lpParty->GetLeader();

				// 다른 존 캐릭터에 삭제 정보를 캐릭터들에게 보낸다. //
				lpParty->SendMemberInfoOutNZone(dwSenderID, PktDD::SCmdDeletePartyMem);				

				if (1 == lpParty->Leave(dwSenderID, dwReferenceID))
				{
					// 비어 있는 파티 소멸
					PARTYLOG(DETLOG2(g_Log, "파티 로그(탈퇴::객체소멸) - PID:0x%08x 탈퇴한놈:0x%08x", dwPartyID, dwSenderID));
					CPartyMgr::GetInstance().DeleteParty(dwPartyID);
					break;
				}

				// 리더가 지워진 경우였다면...
				if (dwLeader == dwSenderID)
				{
					lpParty->TransferLeader(dwReferenceID);	
				}

				PARTYLOG(DETLOG3(g_Log, "파티 로그(탈퇴) - PID:0x%08x 탈퇴한놈:0x%08x 리더:0x%08x", 
					dwPartyID, dwSenderID, lpParty->GetLeader()));
			}
			break;
		} 

		case PktDD::SCmdTransferLeader:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);
    
			PktPMD*			lpPMD			= static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID		= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID		= lpPMD->m_dwSenderID;

            // 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL == lpParty)
			{
				ERRLOG1(g_Log, "PID:0x%08x 파티 얻기 실패.", dwPartyID);
				break;
			}

			lpParty->TransferLeader(dwSenderID);	

			PARTYLOG(DETLOG2(g_Log, "파티 로그(리더양도) - PID:0x%08x 새리더:0x%08x", dwPartyID, dwSenderID));
			break;
		} 

		case PktDD::SCmdAutoRoutingOn:
		{
			CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*			lpPMD			= static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID		= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID		= lpPMD->m_dwSenderID;

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL == lpParty)
			{
				ERRLOG1(g_Log, "PID:0x%08x 파티 얻기 실패.", dwPartyID);
				break;
			}

			if (false == lpParty->AdjustAutoRouting(dwSenderID, true))
			{
				ERRLOG1(g_Log, "CID:0x%08x 오토루팅 켜기 실패(CID가 0이면 ALL)", dwSenderID);
			}

			lpParty->UpdatePartyMemberAutoRouting(dwSenderID, true);

			char* szPacket = reinterpret_cast<char*>(lpPMD);

			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPMD), CmdCharPartyMemData, 0, 0))
			{
				lpParty->SendAllLoggedMember(
					szPacket, sizeof(PktPMD), 0, CmdCharPartyMemData);

				// 다른 존에서 캐릭터에 정보가 변경되었을때 //
				lpParty->SendMemberInfoNZone(szPacket, sizeof(PktPMD), dwSenderID, CmdCharPartyMemData);
			}

			break;
		} 

		case PktDD::SCmdAutoRoutingOff:
		{
			CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

			PktPMD*			lpPMD			= static_cast<PktPMD*>(lpPktBase);
			unsigned long   dwPartyID		= lpPMD->m_dwPartyID;
			unsigned long   dwSenderID		= lpPMD->m_dwSenderID;

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));
			if (NULL == lpParty)
			{
				ERRLOG1(g_Log, "PID:0x%08x 파티 얻기 실패.", dwPartyID);
				break;
			}

			if (false == lpParty->AdjustAutoRouting(dwSenderID, false))
			{
				ERRLOG1(g_Log, "CID:0x%08x 오토루팅 끄기 실패(CID가 0이면 ALL)", dwSenderID);
			}						
					
			lpParty->UpdatePartyMemberAutoRouting(dwSenderID, false);

			char* szPacket = reinterpret_cast<char*>(lpPMD);
			
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPMD), CmdCharPartyMemData, 0, 0))
			{
				lpParty->SendAllLoggedMember(
					szPacket, sizeof(PktPMD), 0, CmdCharPartyMemData);

				// 다른 존에서 캐릭터에 정보가 변경되었을때 //
				lpParty->SendMemberInfoNZone(szPacket, sizeof(PktPMD), dwSenderID, CmdCharPartyMemData);
			}

			break;
		} 

    	case PktDD::SCmdDeadPartyMem:
		{
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktDeadInfo), DBAgentDispatch);

			PktDeadInfo* lpPktDeadInfo = static_cast<PktDeadInfo*>(lpPktBase);

			// 파티 얻기
			lpParty = reinterpret_cast<CCharacterParty*>(
                CPartyMgr::GetInstance().GetParty(lpPktDeadInfo->m_dwDeadPID));

			if (NULL == lpParty)
			{
				ERRLOG1(g_Log, "PID:0x%08x 파티 얻기 실패.", lpPktDeadInfo->m_dwDeadPID);
			}
            else
            {
			    char* szPacket = reinterpret_cast<char*>(lpPktDeadInfo);
			    if (PacketWrap::WrapCrypt(szPacket, sizeof(PktDeadInfo), CmdCharDeadInfo, 0, 0))
			    {
				    lpParty->SendAllLoggedMember(szPacket, sizeof(PktDeadInfo), 0, CmdCharDeadInfo);
			    }
            }
			break;
		}

        case PktDD::SCmdMoveZonePartyMem:		// 파티원 존이동 변경.
		case PktDD::SCmdLevelUpPartyMem:		// 파티원 레벨업 변경. 
        case PktDD::SCmdChangeClassPartyMem:	// 파티원 클래스 변경.
		case PktDD::SCmdChangeGuildPartyMem:	// 파티원 길드정보 변경.
        {
            CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPMD), DBAgentDispatch);

            PktPMD*	        lpPMD       = static_cast<PktPMD*>(lpPktBase);
            unsigned long   dwPartyID   = lpPMD->m_dwPartyID;
			unsigned long	dwSenderID	= lpPMD->m_dwSenderID;			

            // 파티 얻기
            lpParty = reinterpret_cast<CCharacterParty*>(CPartyMgr::GetInstance().GetParty(dwPartyID));

            if (0 != lpParty)
            {
				if(lpPktDD->m_wCmd==PktDD::SCmdLevelUpPartyMem)
				{
					lpParty->UpdatePartyMemberLevel(lpPMD->m_dwSenderID, lpPMD->m_cLevel);
				}
				if(lpPktDD->m_wCmd==PktDD::SCmdMoveZonePartyMem)				
				{
					lpParty->UpdatePartyMemberServer(lpPMD->m_dwSenderID, lpPMD->m_dwServerID);
				}
				if(lpPktDD->m_wCmd==PktDD::SCmdChangeClassPartyMem)				
				{
					lpParty->UpdatePartyMemberClass(lpPMD->m_dwSenderID, lpPMD->m_wClass);			
				}
				if(lpPktDD->m_wCmd==PktDD::SCmdChangeGuildPartyMem)				
				{
					lpParty->UpdatePartyMemberGuild(lpPMD->m_dwSenderID, lpPMD->m_dwGID);								
				}

                char* szPacket = reinterpret_cast<char*>(lpPMD);
                if (PacketWrap::WrapCrypt(szPacket, sizeof(PktPMD), CmdCharPartyMemData, 0, 0))
                {
                    lpParty->SendAllLoggedMember(
                        szPacket, sizeof(PktPMD), 0, CmdCharPartyMemData);
					
					// 다른 존에서 캐릭터에 정보가 변경되었을때 //
					lpParty->SendMemberInfoNZone(szPacket, sizeof(PktPMD), dwSenderID, CmdCharPartyMemData);
                }
            }
            else
            {
                ERRLOG1(g_Log, "PID:0x%08x 파티 얻기 실패.", dwPartyID);
            }
            break;
        }
    }

    return true;
}


bool DBAgentPacketParse::ParseAgentZone(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    size_t nPacketLength = lpPktBase->GetLen();

    if (nPacketLength < sizeof(PktDD))
    {
        ERRLOG2(g_Log, "패킷 길이가 이상합니다. packetLength:%d, sizeof(PktDD):%d", nPacketLength, sizeof(PktDD));
        return false;
    }

    PktDD*          lpPktDD			= static_cast<PktDD*>(lpPktBase);
    unsigned long   dwRequestKey	= lpPktDD->m_dwRequestKey;
    unsigned short  wError			= lpPktBase->GetError();

	CServerRequest::Result result(dwRequestKey);

    CGameClientDispatch* lpDispatch = static_cast<CGameClientDispatch*>(result.GetDispatch());
	
	CCharacter* lpCharacter = lpDispatch->GetCharacter();

	unsigned long dwUID = (NULL != lpCharacter) ? lpCharacter->GetUID() : 0;
	unsigned long dwCID = (NULL != lpCharacter) ? lpCharacter->GetCID() : 0;

    unsigned long dwDispatchRequestKey = lpDispatch->PopRequestKey();
    if (dwRequestKey != dwDispatchRequestKey)
    {
        ERRLOG6(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p/DBRequest Failed - RequestKey가 다릅니다 : 받은 키:%d, Dispatcher의 키:%d",
            dwUID, dwCID, lpCharacter, lpDispatch, dwRequestKey, dwDispatchRequestKey);
    }
    else
    {
        DETLOG6(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p/DBRequest Success - 받은 키:%d, Dispatcher의 키:%d",
            dwUID, dwCID, lpCharacter, lpDispatch, dwRequestKey, dwDispatchRequestKey);
    }

    if (NULL == lpDispatch)
    {
        ERRLOG0(g_Log, "GameClientDispatch를 얻어올 수 없습니다.");        
    }
    else
    {
        CSendStream& SendStream = lpDispatch->GetSendStream();
    
        bool bSendResult = true;

	    switch (lpPktDD->m_wCmd)
	    {
	        case PktDD::SCmdCharMoveZone:
		    { 
				// 에러가 나도 해동 존 프로시져를 초기화 해야한다.
				// 안그럼 이후에 존이동 메시지는 전부 무시하게 된다. 그러면 해당존에서
				// 로그아웃을 하기전에는 이동을 할수가 없다.
//				if (PktBase::NO_SERVER_ERR != wError && NULL != lpCharacter)
				if (NULL != lpCharacter)
				{
					lpCharacter->ResetOperationFlag(CCharacter::MOVEZONE_PROCESSED);
				}

			    PktSZMvDAck* lpSZMvDAck = static_cast<PktSZMvDAck*>(lpPktDD);
                bSendResult = GameClientSendPacket::SendCharMoveZone(SendStream, 
                    lpSZMvDAck->m_wChannelNum, PktSZMvDAck::MAX_CHANNEL_NUM, lpSZMvDAck->m_cZone, wError);

				char cZone = lpSZMvDAck->m_cZone;

				// edith 2009.06.27 서버이동에 실패하면 전쟁을 강제로 꺼준다.
				// edith 2009.07.19 이곳에서는 lpCharacter가 NULL 일수가 있다. 6월에 추가한 전장오프기능에서 다운이 일어날 확율이 높다.
				if (0 != wError && NULL != lpCharacter)
				{
					if (SERVER_ID::CAPITAL == cZone)
					{
						lpCharacter->SetGuildWarFlag(Creature::WAR_OFF);
						// Client 로 패킷전달
						GameClientSendPacket::SendWarOnOff(lpDispatch->GetSendStream(), lpCharacter->GetCID(), GameTime::GUILD, Creature::WAR_OFF, 0);
						// DBAgent로 패킷전달
						GameClientSendPacket::SendWarOnOff(DBAgentDispatch.GetSendStream(), lpCharacter->GetCID(), GameTime::GUILD, Creature::WAR_OFF, 0);
					}
					else if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= cZone && cZone <= SERVER_ID::STONE_WAR3)
					{
						lpCharacter->SetRealmWarFlag(Creature::WAR_OFF);
						// Client 로 패킷전달
						GameClientSendPacket::SendWarOnOff(lpDispatch->GetSendStream(), lpCharacter->GetCID(), GameTime::REALM, Creature::WAR_OFF, 0);
						// DBAgent로 패킷전달
						GameClientSendPacket::SendWarOnOff(DBAgentDispatch.GetSendStream(), lpCharacter->GetCID(), GameTime::REALM, Creature::WAR_OFF, 0);
					}
				}
				break;
		    } 

		    case PktDD::SCmdServerZone:
		    { 
			    PktSAAck* lpSAAck = static_cast<PktSAAck*>(lpPktDD);

			    if (0 == wError)
			    {
				    if (NULL != lpCharacter)
				    {
					    lpCharacter->MoveZoneProcess(lpSAAck->m_dwServerID);
				    }
			    }

                lpDispatch->SetMoveAddress(lpSAAck->m_dwServerID, 
                    INET_Addr(lpSAAck->m_GameAddress, CServerSetup::GetGameServerTCPPort(lpSAAck->m_dwServerID)));

                if (0 == wError)
                {
                    CSendStream& DBASendStream = DBAgentDispatch.GetSendStream();

	                char* lpBuffer = DBASendStream.GetBuffer(sizeof(PktDD));
		            if (0 != lpBuffer)
                    {
	                    CDBRequest DBRequest(*lpDispatch, 600);

                        if (DBRequest.IsValid())
                        {
                            PktDD* lpPktDD = reinterpret_cast<PktDD*>(lpBuffer);
                            lpPktDD->InitPtSubCmd(DBRequest.GetRequestKey(), PktDD::SCmdServerZoneEnd);

				            if (DBASendStream.WrapHeader(sizeof(PktDD), CmdAgentZone, 0, 0))
				            {
					            DETLOG4(g_Log, "SS:0x%p/DP:0x%p/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						            &lpDispatch->GetSession(), lpDispatch, PktSA::SCmdServerZoneEnd, DBRequest.GetRequestKey());

                                lpDispatch->PushRequestKey(DBRequest.GetRequestKey());
                                break;
				            }
                            else
                            {
                                DBRequest.CancelRequest();
                            }
                        }
                    }
                }

                bSendResult = GameClientSendPacket::SendServerZone(SendStream, 
                    lpDispatch->GetMoveServerID(), lpDispatch->GetMoveAddress().get_addr_in(), wError);

                break;
		    } 

            case PktDD::SCmdServerZoneEnd:
            {
                bSendResult = GameClientSendPacket::SendServerZone(SendStream, 
                    lpDispatch->GetMoveServerID(), lpDispatch->GetMoveAddress().get_addr_in(), 0);
                break;
            }

			default:
			{
				ERRLOG2(g_Log, "this:0x%p DB로부터 이상한 패킷이 들어왔습니다. 커맨드:%x ", 
					lpDispatch, lpPktDD->m_wCmd);
				break;
			}
	    }

        if (false == bSendResult)
        {
            lpDispatch->Disconnect();
        }
    }

    return true;
}


bool DBAgentPacketParse::ParseSysServerLogin(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSLAck), DBAgentDispatch);

	PktSLAck* lpPktSLAck = static_cast<PktSLAck*>(lpPktBase);
	
	// 아이템 시리얼 초기화
    Item::CItemFactory& ItemFactory = Item::CItemFactory::GetInstance();
    if (ItemFactory.GetItemUID() < lpPktSLAck->m_dlItemUID)
    {
        ItemFactory.SetItemUID(lpPktSLAck->m_dlItemUID);
    }

	CServerSetup::GetInstance().SetClientVer(lpPktSLAck->m_dwClientVer);
	CServerSetup::GetInstance().SetCheckSum(lpPktSLAck->m_dwCheckSum);

	CDBAgentDispatch::GetDispatchTable().SetDispatch(&DBAgentDispatch);
    return true;
}


bool DBAgentPacketParse::ParseSysServerVerUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSVU), DBAgentDispatch);

	PktSVU* lpPktSVU	= static_cast<PktSVU*>(lpPktBase);

    CServerSetup::GetInstance().SetClientVer(lpPktSVU->m_dwClientVer);
	CServerSetup::GetInstance().SetCheckSum(lpPktSVU->m_dwCheckSum);
    return true;
}

bool DBAgentPacketParse::ParseSysChannelUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSCInfo), DBAgentDispatch);

	PktSCInfo* lpPktSCInfo = static_cast<PktSCInfo*>(lpPktBase);

	CCreatureManager::GetInstance().CalculateEliteBonus(lpPktSCInfo->m_usChannelClientNum);

    return true;
}

bool DBAgentPacketParse::ParseSysRankingUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktRankingInfo* lpRankingInfo = static_cast<PktRankingInfo *>(lpPktBase);

	unsigned short wNum = lpRankingInfo->m_wNum;
	SERLOG1(g_Log, "ParseSysRankingUpdate : %d", wNum);

	RankingNode* lpRankingNode = reinterpret_cast<RankingNode *>(lpRankingInfo + 1);
	for (int nIndex = 0; nIndex < wNum; ++nIndex)
	{
//		SERLOG4(g_Log, "Ranking %d - Name : %s, Class : %d, Level : %d", nIndex, lpRankingNode[nIndex].m_szCharName, lpRankingNode[nIndex].m_cClass, lpRankingNode[nIndex].m_cLevel);

		CRankingMgr::GetInstance().UpdateRanking(lpRankingNode[nIndex]);
	}


	return true;
}

bool DBAgentPacketParse::ParseUserKill(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktUK), DBAgentDispatch)

	PktUK* lpPktUK = static_cast<PktUK*>(lpPktBase);
	unsigned long dwUID = lpPktUK->m_dwUserID;
	unsigned long dwCID = lpPktUK->m_dwCharID;

	// 캐릭터 얻기
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (0 == lpCharacter)
	{
        ERRLOG2(g_Log, "UID:%u/CID:0x%08x/캐릭터 얻기 실패.", dwUID, dwCID);
		SendUserKill(DBAgentDispatch, dwUID, 2);
    }
    else
    {
	    // 유저 세션 얻기
	    CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	    
        if (0 == lpDispatch)
        {
            // 이미 접속이 끊어졌기 때문에, 잠시 후에 로그아웃이 갈 것이다.
            // 유저 죽이기 실패를 보낼 필요가 없다.
            // SendUserKill(DBAgentDispatch, dwUID, 2);

            if (lpCharacter->IsLogined() && !lpCharacter->IsLogout())
            {
                SERLOG2(g_Log, "UID:%u/CID:0x%08x/CGameClientDispatch가 NULL이지만, 로그아웃이 세팅되지 않았습니다.", dwUID, dwCID);
            }
        }
        else if (dwUID != lpDispatch->GetUID())
        {
            ERRLOG4(g_Log, "UID:%d/CID:0x%08x/UID다름. 받은 UID:%d 캐릭터 UID:%d", 
                lpDispatch->GetUID(), dwCID, dwUID, lpCharacter->GetUID());

            SendUserKill(DBAgentDispatch, dwUID, 2);
        }
        else
        {             
            DETLOG2(g_Log, "UID:%d/CID:0x%08x/유저 죽이기 왔음.", dwUID, dwCID);

            if (0 != lpDispatch)
            {
                // 소켓 클로즈
                lpDispatch->Disconnect();
                lpDispatch->Disconnected();
            }
        }
    }

    return true;
}

bool DBAgentPacketParse::SendUserKill(CDBAgentDispatch& DBAgentDispatch, unsigned long dwUserID, unsigned short wError)
{
	CSendStream& SendStream = DBAgentDispatch.GetSendStream();

	char* szBuffer = SendStream.GetBuffer(sizeof(PktUK));

	if (0 != szBuffer)
	{	    
		PktUK* lpPktUK = reinterpret_cast<PktUK*>(szBuffer);

        lpPktUK->m_dwServerID	= CServerSetup::GetInstance().GetServerID();
		lpPktUK->m_dwUserID		= dwUserID;
		lpPktUK->m_dwCharID		= 0;

		return SendStream.WrapHeader(sizeof(PktUK), CmdUserKill, 0, wError);
	}

	return false;
}

bool DBAgentPacketParse::SendServerLogout(CDBAgentDispatch& DBAgentDispatch)
{
	CSendStream& SendStream = DBAgentDispatch.GetSendStream();

	char* szBuffer = SendStream.GetBuffer(sizeof(PktBase));

	if (0 != szBuffer)
	{	    
		return SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
	}

	return false;
}


// 친구 리스트 업데이트
bool DBAgentPacketParse::ParseFriendList(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    PktFriendDB* lpPktFriendDB = static_cast<PktFriendDB*>(lpPktBase);

    CHECK_PACKET_SIZE(
    {
        size_t nPacketLength = lpPktBase->GetLen();

        if (nPacketLength < sizeof(PktFriendDB))
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 최소 크기:%d, 현재 크기:%d", sizeof(PktFriendDB), nPacketLength);
            return false;
        }

        size_t nTotalLength = lpPktFriendDB->m_dwData + sizeof(PktFriendDB);
        if (nTotalLength != nPacketLength)
        {
            ERRLOG2(g_Log, "패킷 길이가 맞지 않습니다. 패킷 크기:%d, 전체 크기:%d", nPacketLength, nTotalLength);
            return false;
        }
    }
    )
    
	CServerRequest::Result result(lpPktFriendDB->m_dwRequestKey, false);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    // 클라이언트를 찾아 업데이트한다.    
    if (0 != lpGameClientDispatch)
    {
        CCharacter* lpCharacter = lpGameClientDispatch->GetCharacter();
        
        if (0 != lpCharacter && lpPktFriendDB->m_dwOwnerCID == lpCharacter->GetCID())
        {
            switch (lpPktFriendDB->m_cCmd)
            {
            case PktFriendDB::FRIEND_LIST:
                lpCharacter->GetFriendList().SerializeIn(
                    reinterpret_cast<char*>(lpPktFriendDB + 1), lpPktFriendDB->m_dwData);
                break;

            case PktFriendDB::BAN_LIST:

                lpCharacter->GetBanList().SerializeIn(
                    reinterpret_cast<char*>(lpPktFriendDB + 1), lpPktFriendDB->m_dwData);
                break;

            default:

                SERLOG2(g_Log, "CID:0x%08x DBAgent로부터 잘못된 커맨드의 친구 리스트 업데이트가 왔습니다. %d",
                    lpPktFriendDB->m_dwOwnerCID, lpPktFriendDB->m_cCmd);
                break;
            };
        }
        else
        {
            ERRLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/CharacterCID:0x%08x/ Invalid CID",
                lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID, 
                lpCharacter, lpPktFriendDB->m_dwRequestKey, 
                (0 != lpCharacter) ? lpCharacter->GetCID() : 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%d/CID:0x%08x/RequestKey:%d/ DBRequest Failed - Requestkey is invalid",
            lpPktFriendDB->m_dwOwnerUID, lpPktFriendDB->m_dwOwnerCID, lpPktFriendDB->m_dwRequestKey);
    }

    return true;
}

// 퀘스트 업데이트
bool DBAgentPacketParse::ParseQuestDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktQuestDB* lpPktQuestDB = static_cast<PktQuestDB *>(lpPktBase);

	CServerRequest::Result result(lpPktQuestDB->m_dwRequestKey, false);

    CGameClientDispatch* lpGameClientDispatch = 
        static_cast<CGameClientDispatch*>(result.GetDispatch());

    if (NULL != lpGameClientDispatch)
    {
		CSendStream& SendStream = lpGameClientDispatch->GetSendStream();
        CCharacter* lpCharacter = lpGameClientDispatch->GetCharacter();

	    if (NULL != lpCharacter && lpPktQuestDB->m_dwCID == lpCharacter->GetCID())
	    {
		    int nSize = 0;
		    int nIndex = 0;

		    Quest::ExecutingQuest* aryExecutingQuest = lpCharacter->GetExecutingQuest();
			unsigned short* aryHistoryQuest = lpCharacter->GetHistoryQuest();

			// 수행중인 퀘스트 목록
			while (nSize < lpPktQuestDB->m_wExecuteQuestSize)
		    {
			    PktQuestDB::ExecutingQuest* lpExecuteQuest = 
				    reinterpret_cast<PktQuestDB::ExecutingQuest *>(reinterpret_cast<char *>(lpPktBase) + sizeof(PktQuestDB) + nSize);

				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(lpExecuteQuest->m_wQuestID);
				if (NULL != lpQuest && false == lpQuest->CheckNationDependent(lpCharacter->GetNation()))
				{
					aryExecutingQuest[nIndex] = 
						Quest::ExecutingQuest(lpExecuteQuest->m_wQuestID, lpExecuteQuest->m_cPhase, lpExecuteQuest->m_cTriggerCount);
					++nIndex;
				}

			    nSize += sizeof(PktQuestDB::ExecutingQuest);
		    }

		    nIndex = 0;

			bool bDeleteQuest = false;

			// 완료 퀘스트 목록
		    while (nSize < lpPktQuestDB->m_wExecuteQuestSize + lpPktQuestDB->m_wHistoryQuestSize)
		    {
			    unsigned short* lpHistoryQuest = 
				    reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(lpPktBase) + sizeof(PktQuestDB) + nSize);

				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(*lpHistoryQuest);
				if (NULL == lpQuest || lpQuest->CheckNationDependent( lpCharacter->GetNation() ))
				{
					bDeleteQuest = true;
				}
				else
				{
					aryHistoryQuest[nIndex] = *lpHistoryQuest;
					++nIndex;
				}

			    nSize += sizeof(unsigned short);
		    }

            GameClientSendPacket::SendCharQuestInfo(SendStream, lpCharacter);

			// 퀘스트에 의해 받는 영향을 계산
			lpCharacter->CalculateStatusData(false);
			if (true == bDeleteQuest)
			{
				if (true == lpCharacter->CalculateMaxSkillSlot())
				{
					GameClientSendPacket::SendCharStateRedistribution(SendStream, 
						lpCharacter->GetCID(), lpCharacter->GetState(), lpCharacter->GetSkill(), PktBase::NO_SERVER_ERR);
				}
			}
		}
        else 
        {
            ERRLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/RequestKey:%d/CharacterCID:0x%08x/ Invalid CID",
                lpPktQuestDB->m_dwUID, lpPktQuestDB->m_dwCID, 
                lpCharacter, lpPktQuestDB->m_dwRequestKey, 
                (NULL != lpCharacter) ? lpCharacter->GetCID() : 0);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%d/CID:0x%08x/RequestKey:%d/ DBRequest Failed - Requestkey is invalid",
            lpPktQuestDB->m_dwUID, lpPktQuestDB->m_dwCID, lpPktQuestDB->m_dwRequestKey);
    }

	return true;
}


bool DBAgentPacketParse::ParseBillingTimeoutNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktBTN), DBAgentDispatch)

	PktBTN* lpPktBTN    = static_cast<PktBTN*>(lpPktBase);
	unsigned long   dwUID       = lpPktBTN->m_dwUserID;
	unsigned long   dwCID 	    = lpPktBTN->m_dwCharID;

	// 캐릭터 얻기
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (0 == lpCharacter)
	{
        ERRLOG1(g_Log, "CID:0x%08x 캐릭터 얻기 실패.", dwCID);
    }
    else
    {
	    // 유저 세션 얻기
	    CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	    
        if (NULL == lpDispatch)
	    {	
            ERRLOG1(g_Log, "CID:0x%08x CGameClientDispatch얻기 실패.", dwCID);
        }
        else
        {
            if (dwUID != lpDispatch->GetUID())
            {
                ERRLOG4(g_Log, "UID:%d/CID:0x%08x UID다름. 받은 UID:%d 캐릭터 UID:%d", 
                    lpDispatch->GetUID(), dwCID, dwUID, lpCharacter->GetUID());

            }
            else
            {             
                // 공지 보내도록 세팅.
                CSendStream& SendStream = lpDispatch->GetSendStream();
                char* szBuffer = SendStream.GetBuffer(sizeof(PktBTN));
                if (NULL != szBuffer)                
                {
                    memcpy(szBuffer, lpPktBTN, sizeof(PktBTN));
                    SendStream.WrapCrypt(sizeof(PktBTN), CmdBillingTimeoutNotify, 0, 0);
                }
            }
        }
    }

    return true;
}

bool DBAgentPacketParse::ParseBillingTimeCheckNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktBTN), DBAgentDispatch)

	PktBTN* lpPktBTN    = static_cast<PktBTN*>(lpPktBase);
	unsigned long   dwUID       = lpPktBTN->m_dwUserID;
	unsigned long   dwCID 	    = lpPktBTN->m_dwCharID;

	ERRLOG1(g_Log, "CID:0x%08x 체킹1.", dwCID);			// 체킹 1

	// 캐릭터 얻기
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (0 == lpCharacter)
	{
        ERRLOG1(g_Log, "CID:0x%08x 캐릭터 얻기 실패.", dwCID);
    }
    else
    {
	    // 유저 세션 얻기
	    CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	    
        if (NULL == lpDispatch)
	    {	
            ERRLOG1(g_Log, "CID:0x%08x CGameClientDispatch얻기 실패.", dwCID);
        }
        else
        {
			ERRLOG1(g_Log, "CID:0x%08x 체킹2.", dwCID);		// 체킹 2

			if (dwUID != lpDispatch->GetUID())
            {
                ERRLOG4(g_Log, "UID:%d/CID:0x%08x UID다름. 받은 UID:%d 캐릭터 UID:%d", 
                    lpDispatch->GetUID(), dwCID, dwUID, lpCharacter->GetUID());

            }
            else
            {         
				ERRLOG1(g_Log, "CID:0x%08x 체킹3.", dwCID);	// 체킹 3
	

                // 공지 보내도록 세팅
                CSendStream& SendStream = lpDispatch->GetSendStream();
                char* szBuffer = SendStream.GetBuffer(sizeof(PktBTN));
                if (NULL != szBuffer)                
                {
                    memcpy(szBuffer, lpPktBTN, sizeof(PktBTN));
                    SendStream.WrapCrypt(sizeof(PktBTN), CmdBillingTimeCheckNotify, 0, 0);
                }
            }
        }
    }

    return true;
}

bool DBAgentPacketParse::ParseCharUseCashItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// DBAgent에서 아이템 사용에 대한 결과를 받는곳
	// 여기서 아이템의 개수를 1개 줄이고 해당 캐릭터한테 아이템이 사용됐다구 알린다.
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktUI), DBAgentDispatch)

	PktUI*      lpPktUI     = static_cast<PktUI*>(lpPktBase);

	Item::ItemPos   itemPos = lpPktUI->m_itemPos;
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(lpPktUI->m_dwSender);

	CGameClientDispatch* pDispatch = lpCharacter->GetDispatcher();

	CHECK_CHARACTER_PTR(lpCharacter, *pDispatch, lpPktBase->GetCmd());

	Item::CUseItem* lpUseItem = Item::CUseItem::DowncastToUseItem(lpCharacter->GetItem(itemPos));
	
	unsigned short usError = lpPktBase->GetError();

	// 아이템 사용 로그를 남긴다.
	GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

	// 아이템 갯수가 0개이면 아이템을 삭제한다.
	// 내구도 1감소
	unsigned char cRemainItemNum = 0;
	if(usError == 0)
	{
		cRemainItemNum = lpUseItem->GetNumOrDurability();

		if (0 < cRemainItemNum)
		{ 
			--cRemainItemNum; 
			lpUseItem->SetNumOrDurability(cRemainItemNum);
		}

		// 골드박스 ?일을 추가해준다.
		long dwTime = (lpCharacter->GetPremiumTime()+1440*lpUseItem->GetItemInfo().m_UseItemInfo.m_dwAmount);
		lpCharacter->SetPremiumService(dwTime, 1);

		if (0 == cRemainItemNum)
		{
			if (lpCharacter->RemoveItem(itemPos))
			{
				if (TakeType::TS_STALL == lpUseItem->GetRealPos().m_cPos)
				{
					lpCharacter->GetStall().RemoveItem(lpUseItem->GetRealPos());
				}
			}

			DELITEMLOG(DETLOG1(g_Log, "this:0X%08x 아이템을 삭제합니다.", lpUseItem));	
			DELETE_ITEM(lpUseItem);
		}
	}

	return GameClientSendPacket::SendCharUseCashItem(pDispatch->GetSendStream(), 
		lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
}


bool DBAgentPacketParse::ParseExtraEvent(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	// edith 2009.09.05 신규 이벤트 제작부분

	// DBAgent에서 아이템 사용에 대한 결과를 받는곳
	// 여기서 아이템의 개수를 1개 줄이고 해당 캐릭터한테 아이템이 사용됐다구 알린다.
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktExtraEvent), DBAgentDispatch)

	PktExtraEvent*      lpPktEE     = static_cast<PktExtraEvent*>(lpPktBase);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(lpPktEE->m_dwCharID);

	CGameClientDispatch* pDispatch = lpCharacter->GetDispatcher();

	unsigned short usError = lpPktBase->GetError();

	if(usError == 0)
	{
		// 에러가 없고 프리미엄 타임이 적용됐으면 
		if(lpPktEE->m_dwEventID == 0x001e0001 )
		{
			// 골드박스 7일을 추가해준다.
			long dwTime = (lpCharacter->GetPremiumTime()+1440*7);
			lpCharacter->SetPremiumService(dwTime, 1);
		}
		else if(lpPktEE->m_dwEventID == 0x003c0001 )
		{
			// 골드박스 7일을 추가해준다.
			long dwTime = (lpCharacter->GetPremiumTime()+1440*15);
			lpCharacter->SetPremiumService(dwTime, 1);
		}
		else if(lpPktEE->m_dwEventID == 0x005a0001 )
		{
			// 골드박스 7일을 추가해준다.
			long dwTime = (lpCharacter->GetPremiumTime()+1440*30);
			lpCharacter->SetPremiumService(dwTime, 1);
		}
	}

	return GameClientSendPacket::SendExtraEvent(pDispatch->GetSendStream(), 
		lpPktEE->m_dwUserID, lpPktEE->m_dwCharID, lpPktEE->m_dwEventID, lpPktEE->m_dwValue1, lpPktEE->m_dwValue2, usError);
}


bool DBAgentPacketParse::ParseEventDropItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktEventDropItem), DBAgentDispatch)

	PktEventDropItem* lpPktEDI = static_cast<PktEventDropItem *>(lpPktBase);
	unsigned short usItemID = lpPktEDI->m_usItemID;

	CGameEventMgr::GetInstance().PushDropEventItem(usItemID);
	return true;
}

// 운영자 명령 처리
bool DBAgentPacketParse::ParseCharAdminCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)				// 어드민 명령 처리
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktAdmin), DBAgentDispatch);

	PktAdmin*   lpPktAdmin  = static_cast<PktAdmin*>(lpPktBase);

	// 해당 캐릭터가 로그 아웃한 경우
	if (lpPktAdmin->GetError() == PktAdmin::E_LOGOUT_CHAR)
	{
		// Admin 캐릭터를 목표 캐릭터가 있는 존과 위치로 이동 시킨다.
		CCharacter* lpAdmin = CCreatureManager::GetInstance().GetCharacter(lpPktAdmin->m_dwAdminCID);
		if (NULL == lpAdmin)
		{
			switch (lpPktAdmin->m_usCmd)
			{
			case PktAdmin::TRACE_CHAR:
				ERRLOG1(g_Log, "어드민 명령 오류 : %s 캐릭터 추적 명령 실패. 어드민 캐릭터가 NULL 입니다.", lpPktAdmin->m_stName);
				break;

			case PktAdmin::INFO_CHAR:
				ERRLOG1(g_Log, "어드민 명령 오류 : %s 캐릭터 정보 보기 명령 실패. 어드민 캐릭터가 NULL 입니다.", lpPktAdmin->m_stName);
				break;
			}

			return true;
		}

		CGameClientDispatch* lpDispatch = lpAdmin->GetDispatcher();
		if (NULL != lpDispatch)
		{
			GameClientSendPacket::SendCharAdmin(lpDispatch->GetSendStream(), lpPktAdmin, PktAdmin::E_LOGOUT_CHAR);
		}

		return true;
	}

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(lpPktAdmin->m_stName);
	if (NULL == lpCharacter && lpPktAdmin->m_usCmd != PktAdmin::TRACE_CHAR)
	{
		// 다른 게임 서버에서 추적할 캐릭터의 위치를 찾아서, 운영자가 있는 서버로 다시 왔을 경우에는 리턴하지 않음
		return true;
	}

	return GameClientParsePacket::ProcessAdminCmd(NULL, lpCharacter, lpPktAdmin);
}


// 임시 인벤으로 아이템 지급
bool DBAgentPacketParse::ParseGiveItemToTempInven(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    PktGiveItemToTempInven* lpGiveItemToTempInven = 
        static_cast<PktGiveItemToTempInven*>(lpPktBase);

    GiveItemInfo* lpPos = lpGiveItemToTempInven->m_GiveItemInfo;
    GiveItemInfo* lpEnd = lpPos + lpGiveItemToTempInven->m_cGiveItemNum;

    CCreatureManager&   creatureManager = CCreatureManager::GetInstance();

    for(; lpPos != lpEnd; ++lpPos)
    {
        lpPos->m_dwItemUID = 0LL;

        CCharacter* lpCharacter = creatureManager.GetCharacter(lpPos->m_dwCID);
        if (0 != lpCharacter && 
            lpCharacter->GetUID() == lpPos->m_dwUID &&
            lpCharacter->GetCID() == lpPos->m_dwCID)
        {
            // 임시 인벤에 공간이 있으면 아이템을 지급한다.
			if (false == lpCharacter->GetTempInven().IsFull())
			{
				lpCharacter->GiveItemByDBAgentRequest(*lpPos);
			}
			else
			{
				// 지급 못한 아이템은 펜딩시켜둔다.
				lpPos->m_dwItemUID = Item::GiveItem::PENDING_ITEM_UID;
			}
        }
    }

    // 받은 패킷 그대로에다. 지급한 아이템 UID추가해서 DB중계서버에 보낸다.
    char* lpSendBuffer = reinterpret_cast<char*>(lpGiveItemToTempInven);

    if(PacketWrap::WrapHeader(lpSendBuffer, sizeof(PktGiveItemToTempInven), CmdGiveItemToTempInven, 0, 0))
    {
        return DBAgentDispatch.GetSendStream().PutBuffer(
            lpSendBuffer, sizeof(PktGiveItemToTempInven), CmdGiveItemToTempInven);
    }

    return false;
}

// 이름 변경 요청에대한 Ack
bool DBAgentPacketParse::ParseCharNameChangeAck(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
    unsigned short usError = lpPktBase->GetError();

    PktCharNameChange* lpPktCharNameChange = 
        reinterpret_cast<PktCharNameChange*>(lpPktBase);

    const char*     szChangedName       = lpPktCharNameChange->m_szCharName;

    unsigned long   dwUID               = lpPktCharNameChange->m_dwUID;
    unsigned long   dwCID               = lpPktCharNameChange->m_dwCID;
    unsigned char   cNameChangeCount    = lpPktCharNameChange->m_cNameChangeCount;
	Item::ItemPos   ItemPos				= lpPktCharNameChange->m_ItemPos;         // 계명허가서 위치

    CCharacter*             lpCharacter     = CCreatureManager::GetInstance().GetCharacter(dwCID);
    CGameClientDispatch*    lpGameDispatch  = 0;

    if (0 != lpCharacter && 0 != (lpGameDispatch = lpCharacter->GetDispatcher()))
    {
        if (0 == usError)
        {
			// 여기서 해당아이템 제거
			if(!lpCharacter->CheckRenameWarrant(ItemPos, true))
			{
				usError = PktCharNameChange::ERR_NAMECHANGE_ITEM;
			}

            char szOldName[CHAR_INFOST::MAX_NAME_LEN];
            strncpy(szOldName, lpCharacter->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
            szOldName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

            if (CCreatureManager::GetInstance().ChangeCharacterName(dwCID, szChangedName))
            {
                // 이넘이 파티에 가입해 있으면, 이름 갱신
                if (0 != lpCharacter->GetPID() && 0 != lpCharacter->GetParty())
                {
                    lpCharacter->GetParty()->ChangeName(dwCID, szChangedName);
                }

                // 이넘이 길드에 가입해 있으면, 이름 갱신
                Guild::CGuild* lpGuild = 0;

                if (0 != lpCharacter->GetGID() && 
                    0 != (lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID())))
                {
                    lpGuild->ChangeMemberName(dwCID, szChangedName);
                }

                INFLOG4(g_Log, "UID:%10u / CID:%10u / OldName:%s / ChangedName:%s / 캐릭터 이름 바꾸기 성공",
                    dwUID, dwCID, szOldName, szChangedName);

                lpCharacter->SetNameChangeCount(cNameChangeCount);
            }
        }

        GameClientSendPacket::SendCharNameChange(lpGameDispatch->GetSendStream(),
            dwUID, dwCID, szChangedName, cNameChangeCount, &ItemPos, usError);
    }

    return true;
}

bool DBAgentPacketParse::ParseSaveEnemy(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{		
	PktSaveEnemy*			lpPktSaveEnemy		= reinterpret_cast<PktSaveEnemy*>(lpPktBase);
	CThreat::SaveEnemyInfo*	lpPktSaveEnemyInfo	= reinterpret_cast<CThreat::SaveEnemyInfo*>((char*)lpPktBase+sizeof(PktSaveEnemy));
	unsigned long			dwCID				= lpPktSaveEnemy->m_dwCID;
	unsigned long			dwTick				= ::GetTickCount();
	unsigned long			dwSavingTicCount	= CThreat::SaveEnemyInfo::MAX_SAVING_TICKCOUNT;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);

	if (lpCharacter)
	{
		CThreat::SaveEnemyInfo* lpSaveEnemyInfo = lpCharacter->GetThreat().GetSaveEnemyInfo();

		if (lpSaveEnemyInfo)
		{
			if (lpPktSaveEnemy->m_dwAttackedCID && lpPktSaveEnemy->m_dwTick<dwSavingTicCount)
			{
				lpSaveEnemyInfo[0].m_dwCID			= lpPktSaveEnemy->m_dwAttackedCID;
				lpSaveEnemyInfo[0].m_dwTickCount	= dwTick-lpPktSaveEnemy->m_dwTick;
			}

			for (unsigned char cIndex = 1; cIndex < CThreat::SaveEnemyInfo::MAX_SAVING_ENEMY; cIndex++)
			{
				if (lpSaveEnemyInfo[cIndex].m_dwCID && lpSaveEnemyInfo[cIndex].m_dwTickCount<dwSavingTicCount)
				{
					lpSaveEnemyInfo[cIndex].m_dwCID			= lpPktSaveEnemyInfo[cIndex-1].m_dwCID;
					lpSaveEnemyInfo[cIndex].m_dwTickCount	= lpPktSaveEnemyInfo[cIndex-1].m_dwTickCount;
				}
			}
		}
	}

	return true;
}

bool DBAgentPacketParse::ParseKeyInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase)
{
	PktKeyInfo*		lpPktKeyInfo	= reinterpret_cast<PktKeyInfo*>(lpPktBase);	
	unsigned long	dwCID			= lpPktKeyInfo->m_dwCID;
	
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);

	if (lpCharacter)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

		if (lpDispatch)
		{
			GameClientSendPacket::SendKeyInfo(lpDispatch->GetSendStream(), lpCharacter->GetCID(), lpPktKeyInfo->m_dwKeyInfo);
		}		
	}

	return true;
}