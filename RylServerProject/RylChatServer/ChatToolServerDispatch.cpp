#include "stdafx.h"
#include "ChatToolServerDispatch.h"
#include "ChatServerConfig.h"
#include "ChatGameServerDispatch.h"
#include "ChatClientDispatch.h"

#include "ChatLog.h"

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Address/INET_Addr.h>
#include <Log/ServerLog.h>

#include <mmsystem.h>

#include <Network/Broadcast/ChatCharData.h>


CMultiDispatch& CChatToolServerDispatch::GetDispatchTable()
{
    static CMultiDispatch multiDispatch;
    return multiDispatch;
}

CChatAdminNames& CChatAdminNames::GetInstance()
{
    static CChatAdminNames chatAdminNames;
    return chatAdminNames;
}

bool CChatAdminNames::RegisterName(const char* szAdminName)
{
    std::string adminName(szAdminName);

    NameSet::iterator pos = m_NameSet.lower_bound(adminName);
    NameSet::iterator end = m_NameSet.end();

    return (pos == end || *pos != adminName) ? m_NameSet.insert(szAdminName).second : true;
}

bool CChatAdminNames::RemoveName(const char* szAdminName)
{
    return 0 != m_NameSet.erase(szAdminName);
}

bool CChatAdminNames::HasName(const char* szAdminName)
{
    return m_NameSet.end() != m_NameSet.find(szAdminName);
}



CChatToolServerDispatch::CChatToolServerDispatch(CSession& Session)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_bAuthorized(false), m_dwLastPingRecvTime(timeGetTime())
{
    memset(m_szUserID, 0, sizeof(char) * ChatToolPkt::MAX_USER_ID);
    memset(m_szPassword, 0, sizeof(char) * ChatToolPkt::MAX_PASSWORD);
    memset(m_cChatOptions, 0, sizeof(char) * ChatToolPkt::ChatOption::MAX_OPTION);
}

void CChatToolServerDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/ä�� ����͸� ���� ����Ǿ����ϴ�. ",
        &GetSession(), this, GetRemoteAddr().get_addr_string());
}

void CChatToolServerDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/ä�� ����͸� ���� ������ ������ϴ�.",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    if (m_bAuthorized)
    {
        // ���� �¿��� �����Ѵ�.
        GetDispatchTable().RemoveDispatch(reinterpret_cast<unsigned long>(this));

        CChatAdminNames::GetInstance().RemoveName(m_szUserID);
        CChatServerConfig::GetInstance().Logout(m_szUserID);        
    }
}

bool CChatToolServerDispatch::Dispatch()
{
    unsigned long dwCurrentTime = timeGetTime();
    if (2 * 1000 * 60 < dwCurrentTime - m_dwLastPingRecvTime)
    {
        // 2�е��� Ping�� ������ ������ ���� ����
        DETLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/"
            "�� ��Ŷ�� ������ �ʾƼ� �����ϴ�. : dwCurrentTime:%u / dwLastPingRecvTime:%u",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), 
            dwCurrentTime, m_dwLastPingRecvTime);

        return false;
    }

    return CRylServerDispatch::Dispatch();
}
    
bool CChatToolServerDispatch::DispatchPacket(PktBase* lpPktBase)
{
    if (lpPktBase->GetCmd() == ChatToolCMD::ChatPing)
    {
        CheckPing(lpPktBase);
    }
    else if (!m_bAuthorized)
    {
        if (lpPktBase->GetCmd() == ChatToolCMD::Authorize)
        {
            Authorize(lpPktBase);
        }
        else
        {
            DETLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/�������� ���� ������ ��Ŷ 0x%02d�� �����Ͽ����ϴ�.",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), lpPktBase->GetCmd());
        }
    }
    else
    {
        switch(lpPktBase->GetCmd())
        {
        case ChatToolCMD::ChangeOption:
            ChangeOption(lpPktBase);
            break;

        case ChatToolCMD::ChatRequest:
            ChatRequest(lpPktBase);
            break;

        case ChatToolCMD::ChatAdminStatus:
            ChatAdminStatus(lpPktBase);
            break;
        }
    }

    return true;
}


bool CChatToolServerDispatch::Authorize(PktBase* lpPktBase)
{
    m_bAuthorized = false;
    ChatToolPkt::Authorize* lpAuthorize = static_cast<ChatToolPkt::Authorize*>(lpPktBase);

    if (sizeof(ChatToolPkt::Authorize) != lpPktBase->GetLen())
    {
        DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/���� ��Ŷ ���̰� ���� �ʽ��ϴ�.",
            &GetSession(), this, GetRemoteAddr().get_addr_string());
    }
    else
    {
        // for prevent buffer overflow.
        lpAuthorize->m_szUserID[ChatToolPkt::MAX_USER_ID - 1] = 0;
        lpAuthorize->m_szPassword[ChatToolPkt::MAX_PASSWORD - 1] = 0;

        if (CChatServerConfig::GetInstance().Authorize(
            lpAuthorize->m_szUserID, lpAuthorize->m_szPassword))
        {
            // ���� ����. ���� �¿� �߰��Ѵ�. ������ ����� �ȴ�.
            m_bAuthorized = true;

            memcpy(m_szUserID, lpAuthorize->m_szUserID, sizeof(char) * ChatToolPkt::MAX_USER_ID);
            memcpy(m_szPassword, lpAuthorize->m_szPassword, sizeof(char) * ChatToolPkt::MAX_PASSWORD);
            
            GetDispatchTable().SetDispatch(reinterpret_cast<unsigned long>(this), this);
            CChatAdminNames::GetInstance().RegisterName(m_szUserID);

            DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/������ �����߽��ϴ�.",
                &GetSession(), this, GetRemoteAddr().get_addr_string());            
        }
        else
        {
            DETLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/������ �����߽��ϴ�. ID:%s/Password:%s",
                &GetSession(), this, GetRemoteAddr().get_addr_string(),
                lpAuthorize->m_szUserID, lpAuthorize->m_szPassword);
        }
    }

    char* szBuffer = m_SendStream.GetBuffer(sizeof(ChatToolPkt::Authorize));
    if (0 != szBuffer)
    {
        ChatToolPkt::Authorize* lpAuthResult = reinterpret_cast<ChatToolPkt::Authorize*>(szBuffer);
        *lpAuthResult = *lpAuthorize;

        lpAuthResult->m_cResult = m_bAuthorized ? 1 : 0;

        if (!m_SendStream.WrapCrypt(sizeof(ChatToolPkt::Authorize), ChatToolCMD::Authorize, 0, 0))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/ID:%16s/IP:%15s/ChatMonitor/���� ��� ���ۿ� �����߽��ϴ�. /",
                &GetSession(), this, lpAuthorize->m_szUserID,  GetRemoteAddr().get_addr_string());
        }
    }

    return m_bAuthorized;
}


bool CChatToolServerDispatch::ChangeOption(PktBase* lpPktBase)
{
    if (sizeof(ChatToolPkt::ChatOption) != lpPktBase->GetLen())
    {
        DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/�ɼ� ���� ��Ŷ ���̰� ���� �ʽ��ϴ�.",
            &GetSession(), this, GetRemoteAddr().get_addr_string());

        return false;
    }

    ChatToolPkt::ChatOption* lpChatOption = static_cast<ChatToolPkt::ChatOption*>(lpPktBase);
    memcpy(m_cChatOptions, lpChatOption->m_cChatOption, 
        sizeof(char) * ChatToolPkt::ChatOption::MAX_OPTION);

    return true;
}


bool CChatToolServerDispatch::ChatRequest(PktBase* lpPktBase)
{
    ChatToolPkt::ChatRequest* lpChatRequest = static_cast<ChatToolPkt::ChatRequest*>(lpPktBase);

    unsigned short usError = 0;

    if (lpPktBase->GetLen() < sizeof(ChatToolPkt::ChatRequest) ||
        lpChatRequest->m_cChatMsgLen + sizeof(ChatToolPkt::ChatRequest) != lpPktBase->GetLen())
    {
        usError = ChatToolPkt::ChatRequest::PACKET_ERROR;

        DETLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatMonitor/Len:%d/ä�� ��û ��Ŷ ���̰� ���� �ʽ��ϴ�.",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), lpPktBase->GetLen());
    }
    else
    {
        // ���� �����÷ο� ����.
        lpChatRequest->m_szAdminName[ChatToolPkt::MAX_CHAR_NAME - 1] = 0;
        lpChatRequest->m_szTargetName[ChatToolPkt::MAX_CHAR_NAME - 1] = 0;
        *(reinterpret_cast<char*>(lpChatRequest + 1) + lpChatRequest->m_cChatMsgLen - 1) = 0;

        BattleInclination::CharData charData;
        memset(&charData, 0, sizeof(BattleInclination::CharData));

        charData.m_dwCID        = 0;                    // ĳ���� CID
        charData.m_dwGID        = 0;                    // ĳ���� GID        
        charData.m_dwCastleID   = 0;                    // ���� ������ ���, ���� ���� ID�� ����.
        charData.m_cSiegeState  = 0;                    // ���� ������ ���, ���� ���� ���¸� ����.
        charData.m_cNation      = Creature::KARTERANT;  // ĳ���� ����
        charData.m_cAdminLevel  = 1;
        charData.m_cFlags       = 0;                    // ��Ÿ ����

        ChatData::CCharInfoMap& charInfoMap = ChatData::CCharInfoMap::GetInstance();
        ChatData::CCharInfo* lpChatData = 0;

        switch(lpChatRequest->m_cChatType)
        {
        case PktChat::ADMIN_SHOUT:
        case PktChat::SHOUT:

            {
                char szMessage[PktChat::PktChatMaxSize];
                int nMsgLen = _snprintf(szMessage, PktChat::PktChatMaxSize - 1, 
                    "%s : %s", lpChatRequest->m_szAdminName, reinterpret_cast<char*>(lpChatRequest + 1));

                szMessage[PktChat::PktChatMaxSize - 1] = 0;

                if (0 < nMsgLen)
                {
                    CServerChatPacket serverChatPacket(szMessage, 
                        static_cast<PktChat::PktChatCmd>(lpChatRequest->m_cChatType), 0,
                        charData, lpChatRequest->m_szAdminName);

                    if (serverChatPacket.IsValid())
                    {
                        // �� ���Ӽ����� ��� �����Ѵ�.
                        CChatGameServerDispatch::GetDispatchTable().Process(
                            CSendChatAllGameServer(serverChatPacket));

                        CChatLog::GetInstance().AdminLog(0, 0, m_szUserID, 
							lpChatRequest->m_szAdminName, lpChatRequest->m_szTargetName,
							szMessage, lpChatRequest->m_cChatType, 0);
                    }
                    else
                    {
                        usError = ChatToolPkt::ChatRequest::PACKET_ERROR;
                    }
                }
                else
                {
                    usError = ChatToolPkt::ChatRequest::PACKET_ERROR;
                }
            }
            break;

        case PktChat::WHISPER:
			
            switch(lpChatRequest->m_cTargetType)
            {
            case ChatToolPkt::TARGET_UID:
                lpChatData = charInfoMap.GetCharInfoByUID(lpChatRequest->m_dwUID);
                break;

            case ChatToolPkt::TARGET_CHARNAME:
                lpChatData = charInfoMap.GetCharInfoByName(lpChatRequest->m_szTargetName);
                break;

            case ChatToolPkt::TARGET_ACCOUNTNAME:
                lpChatData = charInfoMap.GetCharInfoByAccountName(lpChatRequest->m_szTargetName);
                break;
            }

			if (0 != lpChatData)
            {
                const char*     szMessage   = reinterpret_cast<char*>(lpChatRequest + 1);
                unsigned long   dwTargetCID = lpChatData->GetCID();

                CServerChatPacket serverChatPacket(szMessage, 
                    static_cast<PktChat::PktChatCmd>(lpChatRequest->m_cChatType), 0,
                    charData, lpChatRequest->m_szAdminName, &dwTargetCID, 1);

				GET_MULTI_DISPATCH(lpDispatch, lpChatData->GetServerID(), 
					CChatGameServerDispatch, CChatGameServerDispatch::GetDispatchTable());

				if (0 != lpDispatch && serverChatPacket.IsValid())
				{
					// ���Ӽ����� �����Ѵ�.
					lpDispatch->GetSendStream().PutBuffer(
                        serverChatPacket.GetCompressedPacket(), 
						serverChatPacket.GetCompressedSize(), CmdCharChat);

					// ���� ������ UID�� �������ش�.
					lpChatRequest->m_dwUID = lpChatData->GetUID();
					// ���� ������ CCID�� �������ش�.
					lpChatRequest->m_dwCID = lpChatData->GetCID();
					
					// �α׸� �����.
					CChatLog::GetInstance().AdminLog(lpChatData->GetUID(), dwTargetCID, 
						m_szUserID, lpChatRequest->m_szAdminName, lpChatRequest->m_szTargetName, 
						szMessage, lpChatRequest->m_cChatType, 0);
				}
                else
                {
                    usError = ChatToolPkt::ChatRequest::PACKET_ERROR;
                }
            }
            else
            {
                usError = ChatToolPkt::ChatRequest::NONE_CHARACTER;
            }
            break;

        default:

            usError = ChatToolPkt::ChatRequest::PACKET_ERROR;
            break;
        }    
    }
    
    GetSendStream().WrapCompress(reinterpret_cast<char*>(lpChatRequest),
        sizeof(ChatToolPkt::ChatRequest), ChatToolCMD::ChatRequest, 0, usError);

    return (0 == usError);
}


bool CChatToolServerDispatch::SendChatMessage(ChatData::CCharInfo& charInfo, 
                                              const char* szChatMsg, const char* szTargetName,
                                              unsigned short usXPos, unsigned short usZPos, 
                                              unsigned char cChatMsgType, unsigned short usLang, bool bTargetIsToolAdmin)
{
    // Dispatch �ɼ� ������ Culling �ʿ�.
    if (0 == m_cChatOptions[ChatToolPkt::ChatOption::DO_LISTEN])
    {
        // �ȵ�� ������ ���� �ʴ´�.
        return true;
    }

    unsigned char cOptionIndex = 0;

    switch(cChatMsgType)
    {
		case PktChat::NORMAL:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_NORMAL_CHAT;        break;
		case PktChat::PARTY:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_PARTY_CHAT;         break;
		case PktChat::DICE:		          cOptionIndex = ChatToolPkt::ChatOption::LISTEN_DICE_CHAT;			 break;
		case PktChat::FRIEND:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_FRIEND_CHAT;        break;
		case PktChat::GUILD:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_GUILD_CHAT;         break;

		case PktChat::STALL:              
		case PktChat::CAMP_SHOP:
			cOptionIndex = ChatToolPkt::ChatOption::LISTEN_STALL_CHAT;         
			break;

		case PktChat::TRADE:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_TRADE_CHAT;         break;
		case PktChat::SHOUT:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_SHOUT_CHAT;         break;
		case PktChat::ADMIN_NORMAL_CHAT:  cOptionIndex = ChatToolPkt::ChatOption::LISTEN_ADMIN_NORMAL_CHAT;  break;
		case PktChat::ADMIN_SHOUT:        cOptionIndex = ChatToolPkt::ChatOption::LISTEN_ADMIN_SHOUT;        break;
		case PktChat::WHISPER:            cOptionIndex = ChatToolPkt::ChatOption::LISTEN_WHISPER_CHAT;       break;
		case PktChat::NOTICE:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_NOTICE;             break;
    }

    if (!bTargetIsToolAdmin && 0 == m_cChatOptions[cOptionIndex])
    {
        // ��� ���� ���� ä�� Ÿ���� ���� �ʴ´�.
        return true;
    }

    char szBuffer[sizeof(ChatToolPkt::ChatDataSend) + PktChat::PktChatMaxSize + 1];

    ChatToolPkt::ChatDataSend* lpChatDataSend = reinterpret_cast<ChatToolPkt::ChatDataSend*>(szBuffer);
    char* szWriteChatMsg = reinterpret_cast<char*>(lpChatDataSend + 1);
    
    lpChatDataSend->m_dwUID = charInfo.GetUID();
    lpChatDataSend->m_dwCID = charInfo.GetCID();
    lpChatDataSend->m_dwServerID = charInfo.GetServerID();
	lpChatDataSend->m_usLang = usLang;
	lpChatDataSend->m_cRace = charInfo.GetRace();

    strncpy(lpChatDataSend->m_szSenderName, 
        charInfo.GetName(), CHAR_INFOST::MAX_NAME_LEN);

    lpChatDataSend->m_szSenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

    if (0 != szTargetName)
    {
        strncpy(lpChatDataSend->m_szTargetName, 
            szTargetName, CHAR_INFOST::MAX_NAME_LEN);

        lpChatDataSend->m_szTargetName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
    }
    else
    {
        memset(lpChatDataSend->m_szTargetName, 0, 
            sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    }

    lpChatDataSend->m_usXPos = usXPos;
    lpChatDataSend->m_usZPos = usZPos;

    lpChatDataSend->m_cChatType = cChatMsgType;

    size_t nChatMsgLen = strlen(szChatMsg);
    if (PktChat::PktChatMaxSize <= nChatMsgLen)
    {
        nChatMsgLen = PktChat::PktChatMaxSize - 1;
    }

    lpChatDataSend->m_cChatMsgLen = static_cast<unsigned char>(nChatMsgLen);
    memcpy(szWriteChatMsg, szChatMsg, lpChatDataSend->m_cChatMsgLen);
    szWriteChatMsg[PktChat::PktChatMaxSize - 1] = 0;
    
    return GetSendStream().WrapCompress(szBuffer, 
        sizeof(ChatToolPkt::ChatDataSend) + lpChatDataSend->m_cChatMsgLen, ChatToolCMD::ChatSend, 0, 0);
}


bool CChatToolServerDispatch::SendChatMessage(unsigned long dwTargetUID, unsigned long dwTargetCID,
											  const char* szSenderName, const char* szTargetName,
											  const char* szChatMsg, unsigned char cChatMsgType, unsigned short usLang)
{
    // Dispatch �ɼ� ������ Culling �ʿ�.
    if (0 == m_cChatOptions[ChatToolPkt::ChatOption::DO_LISTEN])
    {
        // �ȵ�� ������ ���� �ʴ´�.
        return true;
    }

    unsigned char cOptionIndex = 0;

    switch(cChatMsgType)
    {
		case PktChat::NORMAL:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_NORMAL_CHAT;        break;
		case PktChat::PARTY:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_PARTY_CHAT;         break;
		case PktChat::DICE:		          cOptionIndex = ChatToolPkt::ChatOption::LISTEN_DICE_CHAT;			 break;
		case PktChat::FRIEND:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_FRIEND_CHAT;        break;
		case PktChat::GUILD:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_GUILD_CHAT;         break;
		
		case PktChat::STALL:              
		case PktChat::CAMP_SHOP:
			cOptionIndex = ChatToolPkt::ChatOption::LISTEN_STALL_CHAT;         
			break;
		
		case PktChat::TRADE:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_TRADE_CHAT;         break;
		case PktChat::SHOUT:              cOptionIndex = ChatToolPkt::ChatOption::LISTEN_SHOUT_CHAT;         break;
		case PktChat::ADMIN_NORMAL_CHAT:  cOptionIndex = ChatToolPkt::ChatOption::LISTEN_ADMIN_NORMAL_CHAT;  break;
		case PktChat::ADMIN_SHOUT:        cOptionIndex = ChatToolPkt::ChatOption::LISTEN_ADMIN_SHOUT;        break;
		case PktChat::WHISPER:            cOptionIndex = ChatToolPkt::ChatOption::LISTEN_WHISPER_CHAT;       break;
		case PktChat::NOTICE:             cOptionIndex = ChatToolPkt::ChatOption::LISTEN_NOTICE;             break;
    }

    if (0 == m_cChatOptions[cOptionIndex])
    {
        // ��� ���� ���� ä�� Ÿ���� ���� �ʴ´�.
        return true;
    }

    char szBuffer[sizeof(ChatToolPkt::ChatDataSend) + PktChat::PktChatMaxSize + 1];

    ChatToolPkt::ChatDataSend* lpChatDataSend = reinterpret_cast<ChatToolPkt::ChatDataSend*>(szBuffer);
    char* szWriteChatMsg = reinterpret_cast<char*>(lpChatDataSend + 1);
    
    lpChatDataSend->m_dwUID = dwTargetUID;
    lpChatDataSend->m_dwCID = dwTargetCID;
	lpChatDataSend->m_usLang = usLang;

	ChatData::CCharInfoMap& charInfoMap = ChatData::CCharInfoMap::GetInstance();
	ChatData::CCharInfo* lpChatData = charInfoMap.GetCharInfoByUID(dwTargetUID);

	// edith 2009.07.15 ����͸������� �������� �Ӹ������� ���׼���.
	// �� �κ��� ê�������� ���� ������ GM�� ä���� ������ �����ٸ� Send�� GM�̸��� �ƴ� �ٸ�
	// �̸����� �����Ѵ�.
	if(lpChatData)
	{
		lpChatDataSend->m_dwServerID = lpChatData->GetServerID();
		lpChatDataSend->m_cRace = lpChatData->GetRace();

		strncpy(lpChatDataSend->m_szTargetName, lpChatData->GetName(), CHAR_INFOST::MAX_NAME_LEN);
		lpChatDataSend->m_szTargetName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
	}
	else
	{
		lpChatDataSend->m_dwServerID = 0;
		lpChatDataSend->m_cRace = 0;

		if (0 != szTargetName)
		{
			strncpy(lpChatDataSend->m_szTargetName, szTargetName, CHAR_INFOST::MAX_NAME_LEN);
			lpChatDataSend->m_szTargetName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
		}
		else
		{
			memset(lpChatDataSend->m_szTargetName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
		}
	}

	strncpy(lpChatDataSend->m_szSenderName, szSenderName, CHAR_INFOST::MAX_NAME_LEN);
    lpChatDataSend->m_szSenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
	    
    lpChatDataSend->m_usXPos = 0;
    lpChatDataSend->m_usZPos = 0;

    lpChatDataSend->m_cChatType = cChatMsgType;

    size_t nChatMsgLen = strlen(szChatMsg);
    if (PktChat::PktChatMaxSize <= nChatMsgLen)
    {
        nChatMsgLen = PktChat::PktChatMaxSize - 1;
    }

    lpChatDataSend->m_cChatMsgLen = static_cast<unsigned char>(nChatMsgLen);
    memcpy(szWriteChatMsg, szChatMsg, lpChatDataSend->m_cChatMsgLen);
    szWriteChatMsg[PktChat::PktChatMaxSize - 1] = 0;
    
    return GetSendStream().WrapCompress(szBuffer, 
        sizeof(ChatToolPkt::ChatDataSend) + lpChatDataSend->m_cChatMsgLen, ChatToolCMD::ChatSend, 0, 0);
}


bool CChatToolServerDispatch::CheckPing(PktBase* lpPktBase)
{
    m_dwLastPingRecvTime = timeGetTime();

    char* szBuffer = GetSendStream().GetBuffer(sizeof(PktSyP));
    if (0 != szBuffer)
    {
        PktSyP* lpPktSyP = reinterpret_cast<PktSyP*>(szBuffer);
        lpPktSyP->m_dwTickTime = timeGetTime();

        return GetSendStream().WrapCrypt(
            sizeof(PktSyP), ChatToolCMD::ChatPing, 0, 0);
    }

    return false;
}

bool CChatToolServerDispatch::ChatAdminStatus(PktBase* lpPktBase)
{
    if (lpPktBase->GetLen() != sizeof(ChatToolPkt::ChatAdminStatus))
    {        
        return false;
    }
    
    ChatToolPkt::ChatAdminStatus* lpChatAdminStatus = 
        reinterpret_cast<ChatToolPkt::ChatAdminStatus*>(lpPktBase);

    switch(lpChatAdminStatus->m_cChangeStatus)
    {
    case ChatToolPkt::ChatAdminStatus::LOGIN:        
        CChatAdminNames::GetInstance().RegisterName(m_szUserID);
        break;

    case ChatToolPkt::ChatAdminStatus::LOGOUT:        
        CChatAdminNames::GetInstance().RemoveName(m_szUserID);
        break;
    }

    return true;
}