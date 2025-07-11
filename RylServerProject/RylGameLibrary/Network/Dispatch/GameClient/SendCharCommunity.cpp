
#include "stdafx.h"
#include "SendCharCommunity.h"

#include <Creature/Character/Character.h>
#include <Creature/CreatureManager.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/Packet/PacketStruct/FriendPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>


bool GameClientSendPacket::SendCharExchangeCmd(CSendStream& SendStream, unsigned long dwSenderID_In,
                                               unsigned long dwRecverID_In, unsigned char cCmd_In, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktExC));
    if (NULL != lpBuffer)
    {
        PktExC* lpPktExC = reinterpret_cast<PktExC*>(lpBuffer);

        lpPktExC->m_dwSenderID  = dwSenderID_In;
        lpPktExC->m_dwRecverID  = dwRecverID_In;
        lpPktExC->m_cCmd        = cCmd_In;

        return SendStream.WrapCrypt(sizeof(PktExC), CmdCharExchangeCmd, 0, usError_In);
    }

    return false;
}

bool GameClientSendPacket::SendCharPartyCmd(CSendStream& SendStream, unsigned long dwPartyID_In, 
                                            const AddressInfo &AddressInfo_In, const char* szSenderName_In, 
                                            unsigned long dwSenderID_In, unsigned long dwReceiverID_In, 
                                            unsigned short usCmd_In, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktPCAck));
    if (NULL != lpBuffer)
    {
        PktPCAck* lpPktPCAck = reinterpret_cast<PktPCAck*>(lpBuffer);

	    lpPktPCAck->m_SenderAddressInfo = AddressInfo_In;                                       // Sender의 주소
	    if (NULL != szSenderName_In)
        {
            strncpy(lpPktPCAck->m_strSenderName, szSenderName_In, CHAR_INFOST::MAX_NAME_LEN);   // Sender의 이름
        }
        else
        {
            memset(lpPktPCAck->m_strSenderName, 0, CHAR_INFOST::MAX_NAME_LEN);
        }

	    lpPktPCAck->m_dwPartyID         = dwPartyID_In;                         // 파티 아이디
	    lpPktPCAck->m_dwSenderID        = dwSenderID_In;                        // 캐릭터 아이디(주체)
	    lpPktPCAck->m_dwReferenceID     = dwReceiverID_In;	                    // 참조 아이디(대상 캐릭터/서버 아이디)
        lpPktPCAck->m_cCmd              = static_cast<unsigned char>(usCmd_In); // 명령

        return SendStream.WrapCrypt(sizeof(PktPCAck), CmdCharPartyCmd, 0, usError_In);
    }

    return false;
}


bool GameClientSendPacket::SendCharPartyCreateToDBAgent(CSendStream& AgentSendStream, unsigned long dwLeaderID, unsigned long dwMemberID, unsigned char cZone,
														unsigned short wLeaderClass, char cLeaderLevel, unsigned long dwLeaderGID,
														unsigned short wMemberClass, char cMemberLevel, unsigned long dwMemberGID)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCPD));
    if (NULL != lpBuffer)
    {
        PktCPD* lpPktCPD			= reinterpret_cast<PktCPD*>(lpBuffer);

        lpPktCPD->m_wCmd			= PktDD::SCmdCreateParty;
        
		lpPktCPD->m_dwLeaderID		= dwLeaderID;        		
		lpPktCPD->m_wLeaderClass	= wLeaderClass;
		lpPktCPD->m_cLeaderLevel	= cLeaderLevel;
		lpPktCPD->m_dwLeaderGID		= dwLeaderGID;
		
		lpPktCPD->m_dwMemberID		= dwMemberID;
		lpPktCPD->m_wMemberClass	= wMemberClass;
		lpPktCPD->m_cMemberLevel	= cMemberLevel;
		lpPktCPD->m_dwMemberGID		= dwMemberGID;
		
        return AgentSendStream.WrapHeader(sizeof(PktCPD), CmdAgentParty, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendCharPartyDestroyToDBAgent(CSendStream& AgentSendStream, unsigned long dwPartyID)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktDPD));
    if (NULL != lpBuffer)
    {
        PktDPD* lpPktDPD = reinterpret_cast<PktDPD*>(lpBuffer);

        lpPktDPD->m_wCmd			= PktDD::SCmdDeleteParty;
		lpPktDPD->m_dwPartyID		= dwPartyID;

        return AgentSendStream.WrapHeader(sizeof(PktDPD), CmdAgentParty, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendPartyMemberData(CSendStream& SendStream, unsigned long dwPartyID, unsigned long dwSenderID, unsigned long dwGID, 
											   unsigned short wClass, unsigned long dwServerID, char cLevel, const char* strSenderName, unsigned short usCmd)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktPMD));
    if (NULL != lpBuffer)
    {
        PktPMD* lpPktPMD = reinterpret_cast<PktPMD*>(lpBuffer);

		lpPktPMD->m_wCmd		    = usCmd;
		lpPktPMD->m_dwPartyID		= dwPartyID;
		lpPktPMD->m_dwSenderID		= dwSenderID;
		lpPktPMD->m_dwGID			= dwGID;
		lpPktPMD->m_dwServerID		= dwServerID;
		lpPktPMD->m_wClass			= wClass;
		lpPktPMD->m_cLevel			= cLevel;

		strncpy(lpPktPMD->m_strSenderName, strSenderName, CHAR_INFOST::MAX_NAME_LEN);

        return SendStream.WrapHeader(sizeof(PktPMD), CmdAgentParty, 0, 0);
    }

    return false;
}

bool GameClientSendPacket::SendCharStallRegisterItem(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwShopID, 
													 TakeType takeType, unsigned long dwPrice, unsigned char cCmd, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktStRI));
    if (NULL != lpBuffer)
    {
        PktStRI* lpPktStRI = reinterpret_cast<PktStRI*>(lpBuffer);

		lpPktStRI->m_dwCharID		= dwCharID;
		lpPktStRI->m_dwShopID		= dwShopID;
		lpPktStRI->m_TakeType		= takeType;
		lpPktStRI->m_dwPrice		= dwPrice;
		lpPktStRI->m_cCmd			= cCmd;

        return SendStream.WrapCrypt(sizeof(PktStRI), CmdCharStallRegisterItem, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharStallEnter(CSendStream& SendStream, unsigned long dwCustomerID, unsigned long dwOwnerID, 
											  unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktStE));
    if (NULL != lpBuffer)
    {
        PktStE* lpPktStE = reinterpret_cast<PktStE*>(lpBuffer);

		lpPktStE->m_dwCustomerID	= dwCustomerID;
		lpPktStE->m_dwOwnerID		= dwOwnerID;

        return SendStream.WrapCrypt(sizeof(PktStE), CmdCharStallEnter, 0, usError);
    }

    return false;
}


bool GameClientSendPacket::SendCharStallItemInfo(CSendStream& SendStream, 
												 unsigned long dwOwnerID, unsigned long* arylCustomerID, 
												 unsigned long dwTempSafe, unsigned char cTax, 
												 char* szBuffer, unsigned long dwItemSize, unsigned char cItemNum)
{
	PktStIInfo* lpPktStIInfo = reinterpret_cast<PktStIInfo*>(szBuffer);

	lpPktStIInfo->m_dwOwnerID = dwOwnerID;

	std::copy(arylCustomerID, arylCustomerID + PktStIInfo::MAX_CUSTOMER_NUM, lpPktStIInfo->m_dwCustomerID);

	lpPktStIInfo->m_dwTempSafe = dwTempSafe;
	lpPktStIInfo->m_cTax = cTax;

	lpPktStIInfo->m_dwItemSize = dwItemSize;
	lpPktStIInfo->m_cItemNum = cItemNum;

	return SendStream.WrapCompress(szBuffer, 
		static_cast<unsigned short>(sizeof(PktStIInfo) + dwItemSize + (sizeof(unsigned long) * cItemNum)), 
		CmdCharStallItemInfo, 0, 0);
}


// 친구 등록, 친구 삭제, 거부 등록, 거부 삭제 Ack, 친구 로그인, 친구 로그아웃
bool GameClientSendPacket::SendCharFriendAck(CSendStream& SendStream, unsigned char cAckCmd, unsigned long dwCID, const char* szFriendName, 
                                             unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktFriendAck));
    if (NULL != lpBuffer)
    {
        PktFriendAck* lpPktFriendAck = reinterpret_cast<PktFriendAck*>(lpBuffer);

        lpPktFriendAck->m_dwCID			= dwCID;
        lpPktFriendAck->m_cCmd			= cAckCmd;
		lpPktFriendAck->m_dwGID			= dwGID;
		lpPktFriendAck->m_wClass		= wClass;
		lpPktFriendAck->m_cLevel		= cLevel;
		lpPktFriendAck->m_dwServerID	= dwServerID;

        if (NULL == szFriendName)
        {
            memset(lpPktFriendAck->m_szName, 0, PktFriendAck::MAX_NAME);
        }
        else
        {
            strncpy(lpPktFriendAck->m_szName, szFriendName, PktFriendAck::MAX_NAME);
        }

        return SendStream.WrapCrypt(sizeof(PktFriendAck), CmdFriendAck, 0, usError);
    }

    return false;
}


// 친구 등록당함
bool GameClientSendPacket::SendCharFriendAdded(CSendStream& SendStream, unsigned long dwCID, const char* szName)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktFriendAddReq));
    if (NULL != lpBuffer)
    {
        PktFriendAddReq* lpPktFriendAddRequest = reinterpret_cast<PktFriendAddReq*>(lpBuffer);

        memcpy(lpPktFriendAddRequest->m_szName, szName, PktFriendAddReq::MAX_NAME);
        lpPktFriendAddRequest->m_cCmd   = PktFriendAddReq::ADDED_INFO;
        lpPktFriendAddRequest->m_dwCID  = dwCID;

        return SendStream.WrapCrypt(sizeof(PktFriendAddReq), CmdFriendAddRequest, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendFriendListChangeToDB(CSendStream& AgentSendStream, unsigned long dwUID, unsigned long dwCID,
                                                    unsigned long dwReferenceUID, unsigned long dwReferenceCID, unsigned long dwData, 
                                                    unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, unsigned char cChangeType)
{
    char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktFriendDB));
    if (NULL != lpBuffer)
    {
        PktFriendDB* lpPktFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

        lpPktFriendDB->m_dwOwnerUID     = dwUID;
        lpPktFriendDB->m_dwOwnerCID     = dwCID;
        lpPktFriendDB->m_dwReferenceUID = dwReferenceUID;
        lpPktFriendDB->m_dwReferenceCID = dwReferenceCID;
        lpPktFriendDB->m_dwData         = dwData;
        lpPktFriendDB->m_cCmd           = cChangeType;
		lpPktFriendDB->m_dwGID			= dwGID;
		lpPktFriendDB->m_wClass			= wClass;
		lpPktFriendDB->m_cLevel			= cLevel;
		lpPktFriendDB->m_dwServerID		= dwServerID;

        return AgentSendStream.WrapCrypt(sizeof(PktFriendDB), CmdFriendDB, 0, 0);
    }

    return false;
}


bool GameClientSendPacket::SendCharCreateGuild(CSendStream& SendStream, unsigned long dwMasterID, unsigned long dwGuildID, 
											   unsigned char cInclination, char* szGuildName, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCreateGuild));
	if (NULL != lpBuffer)
	{
		PktCreateGuild* lpPktCreateGuild = reinterpret_cast<PktCreateGuild *>(lpBuffer);

		lpPktCreateGuild->m_dwCID = dwMasterID;                      
		lpPktCreateGuild->m_dwGID = dwGuildID;
		lpPktCreateGuild->m_cInclination = cInclination;
		strncpy(lpPktCreateGuild->m_szGuildName, szGuildName, Guild::MAX_GUILD_NAME_LEN);

		return SendStream.WrapCrypt(sizeof(PktCreateGuild), CmdCreateGuild, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildCmd(CSendStream& SendStream, unsigned long dwGID, unsigned long dwSenderID, unsigned long dwReceiverID, 
											const char* szGuildName, const char* szSenderName, unsigned short wCmd, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildCmd));
	if (NULL != lpBuffer)
	{
		PktGuildCmd* lpPktGuildCmd = reinterpret_cast<PktGuildCmd *>(lpBuffer);

		lpPktGuildCmd->m_dwGID		    = dwGID;                      
		
		lpPktGuildCmd->m_dwSenderID		= dwSenderID;                 
		lpPktGuildCmd->m_dwReferenceID	= dwReceiverID;	             

		strncpy(lpPktGuildCmd->m_szGuildName, szGuildName, MAX_GUILD_NAME_LEN);
		strncpy(lpPktGuildCmd->m_szSenderName, szSenderName, MAX_MEMBER_NAME_LEN);
		
		lpPktGuildCmd->m_wCmd           = wCmd;

		return SendStream.WrapCrypt(sizeof(PktGuildCmd), CmdGuildCmd, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildMark(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
											 char* szMark, unsigned long dwGold, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildMark));
	if (NULL != lpBuffer)
	{
		PktGuildMark* lpPktGuildMark = reinterpret_cast<PktGuildMark *>(lpBuffer);

		lpPktGuildMark->m_dwCID = dwCID;                      
		lpPktGuildMark->m_dwGID = dwGID;                      

		::memcpy(lpPktGuildMark->m_szMark, szMark, Guild::MAX_MARK_SIZE);

		return SendStream.WrapCrypt(sizeof(PktGuildMark), CmdGuildMark, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildLevel(CSendStream& SendStream, unsigned long dwUID, unsigned char cLevel, 
											  unsigned long dwGold, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildLevel));
	if (NULL != lpBuffer)
	{
		PktGuildLevel* lpPktGuildLevel = reinterpret_cast<PktGuildLevel *>(lpBuffer);

		lpPktGuildLevel->m_dwUID = dwUID;                      
		lpPktGuildLevel->m_cLevel = cLevel;

		return SendStream.WrapCrypt(sizeof(PktGuildLevel), CmdGuildLevel, 0, wError);
	}

	return false;
}


bool GameClientSendPacket::SendCharGuildRelation(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, unsigned long dwTargetGID,
												 unsigned long dwValue, unsigned char cSubCmd, unsigned short wError,
												 const char* pszGuildName, const char* pszTempGuildName)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildRelation));
	if (NULL != lpBuffer)
	{
		PktGuildRelation* lpPktGuildRelation = reinterpret_cast<PktGuildRelation *>(lpBuffer);
		ZeroMemory(lpPktGuildRelation, sizeof(PktGuildRelation));

		lpPktGuildRelation->m_dwCID = dwCID;      
		lpPktGuildRelation->m_dwGID = dwGID;
		lpPktGuildRelation->m_dwTargetGID = dwTargetGID;
		lpPktGuildRelation->m_dwValue = dwValue;
		lpPktGuildRelation->m_cSubCmd = cSubCmd;

		if (0 != pszGuildName)
		{
			strncpy(lpPktGuildRelation->m_szGuildName, pszGuildName, Guild::MAX_GUILD_NAME_LEN);
		}

		if (0 != pszTempGuildName)
		{
			strncpy(lpPktGuildRelation->m_szTempGuildName, pszTempGuildName, Guild::MAX_GUILD_NAME_LEN);
		}

		return SendStream.WrapCrypt(sizeof(PktGuildRelation), CmdGuildRelation, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildInclination(CSendStream& SendStream, unsigned long dwUID, unsigned char cInclination, 
													unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildInclination));
	if (NULL != lpBuffer)
	{
		PktGuildInclination* lpPktGuildInclination = reinterpret_cast<PktGuildInclination *>(lpBuffer);

		lpPktGuildInclination->m_dwUID = dwUID;      
		lpPktGuildInclination->m_cInclination = cInclination;

		return SendStream.WrapCrypt(sizeof(PktGuildInclination), CmdGuildInclination, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildRight(CSendStream& SendStream, unsigned long dwGID, GuildRight guildRight, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildRight));
	if (NULL != lpBuffer)
	{
		PktGuildRight* lpPktGuildRight = reinterpret_cast<PktGuildRight *>(lpBuffer);

		lpPktGuildRight->m_dwUID = dwGID;                      
		lpPktGuildRight->m_GuildRight = guildRight;

		return SendStream.WrapCrypt(sizeof(PktGuildRight), CmdGuildRight, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharMyGuildInfo(CSendStream& SendStream, unsigned long dwGold, 
											   GuildRight guildRight, unsigned char cTitle, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktMyGuildInfo));
	if (NULL != lpBuffer)
	{
		PktMyGuildInfo* lpPktMyGuildInfo = reinterpret_cast<PktMyGuildInfo *>(lpBuffer);

		lpPktMyGuildInfo->m_dwGold = dwGold;                      
		lpPktMyGuildInfo->m_GuildRight = guildRight;
		lpPktMyGuildInfo->m_cTitle = cTitle;

		return SendStream.WrapCrypt(sizeof(PktMyGuildInfo), CmdMyGuildInfo, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharGuildSafe(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
											 unsigned long dwSafeGold, unsigned long dwCharGold, unsigned char cCmd, 
											 const char* szCharName, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildSafeAck));
	if (NULL != lpBuffer)
	{
		PktGuildSafeAck* lpPktGSAck = reinterpret_cast<PktGuildSafeAck *>(lpBuffer);

		lpPktGSAck->m_dwCID = dwCID;    
		lpPktGSAck->m_dwGID = dwGID;
		lpPktGSAck->m_dwSafeGold = dwSafeGold;
		lpPktGSAck->m_dwCharGold = dwCharGold;
		lpPktGSAck->m_cCmd = cCmd;

		strncpy(lpPktGSAck->m_szCharName, szCharName, Guild::MAX_MEMBER_NAME_LEN);

		return SendStream.WrapCrypt(sizeof(PktGuildSafeAck), CmdGuildSafe, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharUpdateGuildMemberInfo(CSendStream& SendStream, unsigned long dwGID, unsigned long dwCID, 
														 MemberInfo memberInfo, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildMemberInfoUpdate));
	if (NULL != lpBuffer)
	{
		PktGuildMemberInfoUpdate* lpPktGMIU = reinterpret_cast<PktGuildMemberInfoUpdate *>(lpBuffer);

		lpPktGMIU->m_dwGID = dwGID;
		lpPktGMIU->m_dwCID = dwCID;    

		lpPktGMIU->m_MemberListInfo = memberInfo.m_MemberListInfo;
		lpPktGMIU->m_MemberDetailInfo = memberInfo.m_MemberDetailInfo;

		return SendStream.WrapCrypt(sizeof(PktGuildMemberInfoUpdate), CmdGuildMemberInfoUpdate, 0, wError);
	}

	return false;
}


bool GameClientSendPacket::SendCharChatBan(CSendStream& SendStream, unsigned long dwAdminCID, 
										   unsigned long dwTargetCID, unsigned long dwMinutes)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktChatBan));
	if (NULL != lpBuffer)
	{
		PktChatBan* lpPktChatBan = reinterpret_cast<PktChatBan *>(lpBuffer);

		lpPktChatBan->m_dwAdminCID = dwAdminCID;
		lpPktChatBan->m_dwTargetCID = dwTargetCID;
		lpPktChatBan->m_dwMinutes = dwMinutes;    

		return SendStream.WrapCrypt(sizeof(PktChatBan), CmdChatBan, 0, PktBase::NO_SERVER_ERR);
	}

	return false;
}

