
#include "stdafx.h"

#include <Log/LogCommands.h>
#include <Log/CharacterLog.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>
#include <Network/Dispatch/GameLog/SendLogPacket.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Creature/Character/Character.h>
#include <Creature/CreatureManager.h>

#include <Utility/Setup/ServerSetup.h>
#include <Item/ItemFactory.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>
#include "SendCharLoginOut.h"



bool GameClientSendPacket::SendCharLogin(CSendStream& SendStream, CCharacter* lpCharacter, 
										 const SOCKADDR_IN* lpSockAddr_In, unsigned short usError)
{
	int		nTotalSize = sizeof(PktCLiAck) + DBUpdateData::MAX_DBUPDATE_SIZE;
	char	szCharInfo[sizeof(PktCLiAck) + DBUpdateData::MAX_DBUPDATE_SIZE] = { 0, };
    char*	lpCharacterInfo = szCharInfo + sizeof(PktCLiAck);
	CHAR_INFOST* lpTempCharInfo = reinterpret_cast<CHAR_INFOST*>(lpCharacterInfo);

    unsigned long   dwUID = 0;
    unsigned long   dwCID = 0;    
	bool			bPeaceMode = false;
	bool			bCheckRelation = false;

    PktCLiAck* lpPktCLiAck = reinterpret_cast<PktCLiAck*>(szCharInfo);
    memset(lpPktCLiAck, 0, sizeof(PktCLiAck));

    if (NULL != lpCharacter && 0 == usError)
    {
        if (!lpCharacter->GetCharacterInfo(lpCharacterInfo, &nTotalSize, lpPktCLiAck->m_usUpdate))
        {
			ERRLOG0(g_Log, "데이터를 복사해 올 수 없습니다. 클라이언트에 데이터를 보낼 수 없습니다.");
			usError = 2;
        }

		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCharacter->GetGID() ) ;
		if ( lpGuild && Guild::JOIN_WAIT == lpGuild->GetTitle( lpCharacter->GetCID() ) )
		{
			lpTempCharInfo->GID |= Guild::MEMBER_WAIT_BIT;
		}
        
        CServerSetup& serverSetup = CServerSetup::GetInstance();
        
        memset(&lpPktCLiAck->m_GameServerUDPAddr, 0, sizeof(lpPktCLiAck->m_GameServerUDPAddr));
        memset(&lpPktCLiAck->m_ChatServerTCPAddr, 0, sizeof(lpPktCLiAck->m_ChatServerTCPAddr));
        
        lpPktCLiAck->m_dwUID			= dwUID = lpCharacter->GetUID();
        lpPktCLiAck->m_dwServerID		= CServerSetup::GetInstance().GetServerID();
		lpPktCLiAck->m_bPeaceMode		= bPeaceMode = lpCharacter->IsPeaceMode();
        lpPktCLiAck->m_cNameChangeCount = lpCharacter->GetNameChangeCount();
		lpPktCLiAck->m_cGuildWarFlag	= lpCharacter->GetGuildWarFlag();
		lpPktCLiAck->m_cRealmWarFlag	= lpCharacter->GetRealmWarFlag();
		lpPktCLiAck->m_cAdminFlag		= lpCharacter->GetGMModelFlag();
		lpPktCLiAck->m_cRealmPoint		= lpCharacter->GetRealmPoint();
		lpPktCLiAck->m_cTacticsFlag		= lpCharacter->GetTacticsFlag();
		lpPktCLiAck->m_PlayTime			= lpCharacter->GetPlayTime();
		lpPktCLiAck->m_PremiumTime		= lpCharacter->GetPremiumTime();
		lpPktCLiAck->m_PremiumType		= lpCharacter->GetPremiumType();

		// --------------------------------------------------------------------------------------
		// 서버 시간은 매일 자정을 기준으로 하여 몇 밀리초가 지났는지 설정한다.

		time_t timeNow, timeStandard;

		time(&timeNow);
		tm* tmNow = localtime(&timeNow);

		tmNow->tm_hour = tmNow->tm_min = tmNow->tm_sec = 0;
		timeStandard = mktime(tmNow);
		
        lpPktCLiAck->m_dwServerTime = static_cast<unsigned long>(timeNow - timeStandard) * 1000;

		// --------------------------------------------------------------------------------------
		lpPktCLiAck->m_bCheckRelation = false;
		
    }
    else
    {
        if(0 == usError)
        {
            usError = 1;
        }

        nTotalSize = 0;
        std::fill_n(lpPktCLiAck->m_usUpdate, int(DBUpdateData::MAX_UPDATE_DB), 0);
    }
	
    if(0 == usError)
    {
        // 로그인 로그 남기기.
        GAMELOG::LogCharLoginOut(dwUID, lpCharacter, lpSockAddr_In, lpCharacterInfo, nTotalSize, 
            lpPktCLiAck->m_usUpdate, GAMELOG::CMD::CHAR_LOGIN, usError);
    }

    // 로그 DB에 로그 쏘기
    // SendLogPacket::CharLogin(*lpCharacter);

    if (SendStream.WrapCompress(szCharInfo, static_cast<unsigned short>(sizeof(PktCLiAck) + nTotalSize),
        CmdCharLogin, 0, usError))
    {
        if (NULL != lpCharacter && 0 == usError)
        {
            CFriendList&    friendList  = lpCharacter->GetFriendList();
            CBanList&       banList     = lpCharacter->GetBanList();

            dwUID = lpCharacter->GetUID();
            dwCID = lpCharacter->GetCID();            

            GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, 
                CChatDispatch::GetDispatchTable());

            if(0 != lpChatDispatch)
            {
                // 채팅 서버로 로그인을 던진다.
                lpCharacter->GetSerializeData().SendChatLogin(*lpCharacter);

                CSendStream& chatSendStream = lpChatDispatch->GetSendStream();
				
                CCommunityListSend sendFriendList(friendList);
                CCommunityListSend sendBanList(banList);

				unsigned long	dwGID		= lpCharacter->GetGID();
				unsigned short	wClass		= lpCharacter->GetClass();
				char			cLevel		= lpCharacter->GetLevel();
				unsigned long	dwServerID	= CServerSetup::GetInstance().GetServerID();

                // 채팅 서버로 거부 리스트를 준다.
                if(!sendBanList.Send(dwUID, dwCID, dwGID, wClass, cLevel, dwServerID, chatSendStream))
                {
                    ERRLOG2(g_Log, "UID:%d/CID:%d/ 거부 리스트를 채팅 서버에 보내는 데 실패했습니다.", dwUID, dwCID);
                }

                // 채팅 서버로 친구 리스트를 준다.
                if(!sendFriendList.Send(dwUID, dwCID, dwGID, wClass, cLevel, dwServerID, chatSendStream))
                {
                    ERRLOG2(g_Log, "UID:%d/CID:%d/ 친구 리스트를 채팅 서버에 보내는 데 실패했습니다.", dwUID, dwCID);
                }
            }
			
            // 창고 정보를 업데이트한다.
            return lpCharacter->GetDeposit().ClientUpdate(SendStream);
        }

        return true;
    }

    return false;
}

bool GameClientSendPacket::SendCharMoveZone(CSendStream& SendStream, unsigned short* lpChannelNums, 
                                            unsigned char cChannelNum, unsigned char cZone, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSZMvAck));
    if (NULL != lpBuffer)
    {
        PktSZMvAck* lpPktSZMvAck = reinterpret_cast<PktSZMvAck*>(lpBuffer);
    
        lpPktSZMvAck->m_cZone = cZone;

	    if (NULL != lpChannelNums)
	    {
			unsigned short* lpSrcPastEnd = lpChannelNums + 
				std::min(cChannelNum, (unsigned char)PktSZMvAck::MAX_CHANNEL_NUM);
            std::copy(lpChannelNums, lpSrcPastEnd, lpPktSZMvAck->m_wChannelNum);		    
	    }

        return SendStream.WrapCrypt(sizeof(PktSZMvAck), CmdCharMoveZone, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendServerZone(CSendStream& SendStream, unsigned long dwServerID, 
                                          const sockaddr_in& gameServerTCPAddress, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSZAck));
    if (NULL != lpBuffer)
    {
        PktSZAck* lpPktSZAck = reinterpret_cast<PktSZAck*>(lpBuffer);
	
	    lpPktSZAck->m_dwServerID	    = dwServerID;
	    lpPktSZAck->m_GameServerTCPAddr = gameServerTCPAddress;

        return SendStream.WrapCrypt(sizeof(PktSZAck), CmdServerZone, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCSAuth(CSendStream& SendStream, unsigned long dwCID, 
									  unsigned long dwAuthCode, const _GG_AUTH_DATA* lpAuthCode2, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCSAuth));
	if (NULL != lpBuffer)
	{
		PktCSAuth* lpPktCSAuth = reinterpret_cast<PktCSAuth *>(lpBuffer);

		lpPktCSAuth->m_dwCharID		= dwCID;
		lpPktCSAuth->m_dwAuthCode	= dwAuthCode;
		lpPktCSAuth->m_AuthCode2	= *lpAuthCode2;

		return SendStream.WrapCrypt(sizeof(PktCSAuth), CmdCSAuth, 0, usError);
	}

	return false;
}


// DBAgent로 전송
bool GameClientSendPacket::SendLoginToDBAgent(CSendStream& AgentSendStream, 
                                              unsigned long dwRequestKey, unsigned long dwSessionID, 
                                              unsigned long dwUserID, unsigned long dwCharID, 
                                              const IN_ADDR remoteAddress)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktDBUpdate));
    if (NULL != lpBuffer)
    {
        PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(lpBuffer);
        memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));

        lpPktDBUpdate->m_dlItemSerial   = Item::CItemFactory::GetInstance().GetItemUID();
        lpPktDBUpdate->m_dwSessionID    = dwSessionID;
        lpPktDBUpdate->m_dwUserID       = dwUserID;
        lpPktDBUpdate->m_dwCharID       = dwCharID;
        lpPktDBUpdate->m_dwRequestKey   = dwRequestKey;
        lpPktDBUpdate->m_Address		= remoteAddress;
        lpPktDBUpdate->m_TypeCode       = DBUpdateData::LOGIN;

        return AgentSendStream.WrapHeader(sizeof(PktDBUpdate), CmdDBUpdateData, 0, 0);
    }

    return false;
}   

bool GameClientSendPacket::SendMoveZoneToDBAgent(CSendStream& AgentSendStream, POS& newPos, 
                                                 unsigned long dwRequestKey, unsigned long dwUserID, 
                                                 unsigned char cZone, char cChannel)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktSZMvD));
    if (NULL != lpBuffer)
    {
        PktSZMvD* lpPktSZMvD = reinterpret_cast<PktSZMvD*>(lpBuffer);

        lpPktSZMvD->m_wCmd          = PktDD::SCmdCharMoveZone;
        lpPktSZMvD->m_dwRequestKey  = dwRequestKey;
        lpPktSZMvD->m_dwUserID	    = dwUserID; 
	    
	    lpPktSZMvD->m_NewPos	    = newPos;
        lpPktSZMvD->m_cZone		    = cZone;
		lpPktSZMvD->m_cChannel	    = cChannel;

        return AgentSendStream.WrapHeader(sizeof(PktSZMvD), CmdAgentZone, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendServerZoneToDBAgent(CSendStream& AgentSendStream, 
                                                   unsigned long dwRequestKey, unsigned long dwUserID, 
                                                   unsigned char cZone, unsigned char cChannel)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktSA));
    if (NULL != lpBuffer)
    {
        PktSA* lpPktSA = reinterpret_cast<PktSA*>(lpBuffer);

        lpPktSA->m_wCmd         = PktDD::SCmdServerZone;
        lpPktSA->m_dwRequestKey = dwRequestKey;
	    lpPktSA->m_dwUserID	    = dwUserID;
	    lpPktSA->m_cZone	    = cZone;
	    lpPktSA->m_cChannel	    = cChannel;

        return AgentSendStream.WrapHeader(sizeof(PktSA), CmdAgentZone, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendKeyInfo(CSendStream& SendStream, unsigned long dwCID, unsigned long* pKeyInfo)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktKeyInfo));

	if (NULL != lpBuffer)
	{
		PktKeyInfo* lpPktKeyInfo = reinterpret_cast<PktKeyInfo*>(lpBuffer);

		lpPktKeyInfo->m_dwCID = dwCID;
		memcpy(lpPktKeyInfo->m_dwKeyInfo, pKeyInfo, sizeof(unsigned long)*PktKeyInfo::MAX_KEY_INFO);
		
		return SendStream.WrapCrypt(sizeof(PktKeyInfo), CmdKeyInfo, 0, 0);
	}

	return false;
}