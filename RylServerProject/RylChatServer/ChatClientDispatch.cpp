#include "stdafx.h"

/*
#include "ChatClientDispatch.h"
#include "ChatSend.h"

#include <BaseLibrary/Log/ServerLog.h>
#include <Network/Packet/ChatPacket.h>
#include <Network/Session/Session.h>
#include <RylServerLibrary/Network/Packet/PacketCommand.h>

#include <RylGameLibrary/Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <RylGameLibrary/Network/Packet/PacketStruct/CharMovePacket.h>

#include <mmsystem.h>


CChatClientDispatch::ChatClientDispatchMap& CChatClientDispatch::GetDispatchMap()
{
    static CChatClientDispatch::ChatClientDispatchMap chatClientDispatchMap;
    return chatClientDispatchMap;
}


CChatClientDispatch::CChatClientDispatch(CSession& Session) 
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_dwUID(0), m_dwCID(0), m_dwServerID(0), m_usFlag(0)
{
    
}

bool CChatClientDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bResult = false;

    switch(lpPktBase->GetCmd())
	{
    case CmdSysPing:            
        m_CheckPing.SetLastPingRecvTime(timeGetTime());
        return true;
    
    case CmdChatClientLogin:    bResult = ParseCharChatLogin(static_cast<PktChatClientLogin*>(lpPktBase));	break;
    case CmdCharMoveUpdate:     bResult = ParseCharMoveUpdate(static_cast<PktMU*>(lpPktBase));				break;
	case CmdCharChat:		    bResult = ParseCharChat(static_cast<PktChat*>(lpPktBase));					break;
	case CmdCharWhisper:	    bResult = ParseCharWhisper(static_cast<PktWh*>(lpPktBase));					break;

	default:

		bResult = true;
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Command packet", this, lpPktBase->GetCmd());
		break;
	}

	if(!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Packet process failed", this, lpPktBase->GetCmd());
	}

	return true;
}

bool CChatClientDispatch::Dispatch()
{
	unsigned long dwCurrentTime = timeGetTime();

    if(m_CheckPing.CheckPing(dwCurrentTime))
    {
		return CRylServerDispatch::Dispatch();
    }
    else
    {                
        unsigned long dwPingCount           = 0;
        unsigned long dwLastPingRecvTime    = 0;
        unsigned long dwFirstCheckTime      = 0;

        m_CheckPing.GetPingData(dwPingCount, 
            dwLastPingRecvTime, dwFirstCheckTime);

        const int MAX_BUFFER = 256;
        char szBuffer[MAX_BUFFER];

        _snprintf(szBuffer, MAX_BUFFER - 1, 
            "UID:%u/CID:%u/ServerID:0x%08x/CurrentTime:%u/LastPingTime:%u/PingCount:%u/FirstCheckTime:%u/"
            "PingCheck failed. disconnect now.",
            m_dwUID, m_dwCID, m_dwServerID, dwCurrentTime, 
            dwLastPingRecvTime, dwPingCount, dwFirstCheckTime);

        szBuffer[MAX_BUFFER - 1] = 0;
        LogErrorPacket(szBuffer, 0);
    }

    return false;
}


void CChatClientDispatch::Disconnected()
{
    SetFlag(CHAR_DISCONNECTED);

    DETLOG3(g_Log, "UID:%10d/CID:%10d/%15s/Ŭ���̾�Ʈ���� ������ ����Ǿ����ϴ�.", m_dwUID, m_dwCID,
        m_Session.GetRemoteAddr().get_addr_string());

    CChatClientDispatch::GetDispatchMap().erase(m_dwUID);
}


bool CChatClientDispatch::ParseCharChatLogin(PktChatClientLogin* lpPktChatClientLogin)
{
    m_dwUID = lpPktChatClientLogin->m_dwUID;
    m_dwCID = lpPktChatClientLogin->m_dwCID;
    m_dwServerID = lpPktChatClientLogin->m_dwServerID;

    CChatClientDispatch::GetDispatchMap().insert(std::make_pair(m_dwUID, this));

    SetFlag(CHAR_CONNECTED);
    DETLOG3(g_Log, "UID:%10d/CID:%10d/%15s/Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.", 
		m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string());

    CChatSend::Rebind* lpRebind = CChatSend::GetInstance().GetRebind(m_dwUID, m_dwCID);
    if(NULL != lpRebind)
    {
        CChatCharInfo& ChatInfo = lpRebind->m_ChatInfo;

        // ���� �α��� ������ �� friend, ban �����Ͱ� ���� ������ Ŭ���̾�Ʈ�� ������.
        if(ChatInfo.IsSetFlags(CChatCharInfo::SendLoginComplete) && 
            !ChatInfo.IsSetFlags(CChatCharInfo::SendFriendInfo) &&            
            ChatInfo.IsSetFlags(CChatCharInfo::FriendListReceived | CChatCharInfo::BanListReceived))
        {
            ChatInfo.SetFlags(CChatCharInfo::SendFriendInfo);
            CChatSend::SendFriendInfo(ChatInfo, m_SendStream);

            DETLOG2(g_Log, "UID:%10d/CID:%10d/ ChatClientDispatch���� ģ�� ������ Ŭ���̾�Ʈ�� ���½��ϴ�.", 
                ChatInfo.GetUID(), ChatInfo.GetCID());
        }
    }

    return true;

    ///*
    // Ŭ���̾�Ʈ�� Ack�� ������.
    char* szBuffer = m_SendStream.GetBuffer(sizeof(PktChatClientLogin));
    if(NULL != szBuffer)
    {
        PktChatClientLogin* lpPktChatClientLoginAck = 
            reinterpret_cast<PktChatClientLogin*>(szBuffer);

        lpPktChatClientLoginAck->m_dwUID        = m_dwUID;
        lpPktChatClientLoginAck->m_dwCID        = m_dwCID;
        lpPktChatClientLoginAck->m_dwServerID   = m_dwServerID;

        if(m_SendStream.WrapCrypt(sizeof(PktChatClientLogin), CmdChatClientLogin, 0, 0))
        {
            return true;
        }
    }//

    ERRLOG2(g_Log, "UID:%10d/CID:%10d/ �α��ο� �����߽��ϴ�. ��Ŷ�� ���� �� �����ϴ�.", m_dwUID, m_dwCID);
    return false;
}

bool CChatClientDispatch::ParseCharMoveUpdate(PktMU* lpPktMU)
{
    m_ChatPos.SetPos(
        static_cast<CChatPosition::position_t>(lpPktMU->m_Position.fPointX),
        static_cast<CChatPosition::position_t>(lpPktMU->m_Position.fPointY),
        static_cast<CChatPosition::position_t>(lpPktMU->m_Position.fPointZ));

    return true;
}


bool CChatClientDispatch::ParseCharChat(PktChat* lpPktChat)
{
    CChatPosition::position_t MAX_SQUARED_RANGE = 32 * 32;
    const int nDefaultMaxPacketLen = sizeof(PktChat) + PktChat::PktChatMaxSize;
    
	int nPacketLength = lpPktChat->GetLen();    
    if(nPacketLength < sizeof(PktChat))
	{
        ERRLOG4(g_Log, "UID:%10d/CID:%10d/%15s/��Ŷ ���̰� �̻��մϴ�/����:%d/", 
            m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), nPacketLength);
		return false;
	}

    if(0 == m_dwUID || 0 == m_dwCID || !IsSetFlag(CHAR_CONNECTED))
    {
        ERRLOG4(g_Log, "UID:%10d/CID:%10d/%15s/������Ŷ:%s/UID, CID�� 0�̰ų� ���� ��Ŷ�� ���� ���� ���Դϴ�/",
            m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), IsSetFlag(CHAR_CONNECTED) ? "�޾���" : "������");
    }

    switch(lpPktChat->m_cCmd)
    {
    case PktChat::NORMAL:    
    case PktChat::PARTY:
    case PktChat::GUILD:
    case PktChat::SHOUT:
    case PktChat::CLIENT_LOG:    // �α�

    case PktChat::ADMIN_NORMAL_CHAT:
    case PktChat::ADMIN_SHOUT:
    
        // �Ÿ� üũ�ؼ� �ֺ� �ι��鿡�� ���� ����.
        if(0 != lpPktChat->m_cNum || nDefaultMaxPacketLen < nPacketLength)
        {
            ERRLOG4(g_Log, "UID:%10d/CID:%10d/%15s/��Ŷ ���̰� �̻��մϴ�/����:%d/", 
                m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), nPacketLength);
            return false;
        }

        {
            char*   lpChat  = reinterpret_cast<char*>(lpPktChat + 1);
            int     nMsgLen = nPacketLength - sizeof(PktChat);

            if(0 < nMsgLen)
            {
                // ������ ���ڸ� NULL�� �ٲ۴�.
                *(lpChat + nMsgLen - 1) = NULL;

                switch(lpPktChat->m_cCmd)
                {
                case PktChat::NORMAL:
                case PktChat::ADMIN_NORMAL_CHAT:                    
                    CChatSend::GetInstance().SendNearCharacter(m_dwUID, m_dwCID, 
                        m_ChatPos, MAX_SQUARED_RANGE, lpChat, nMsgLen, lpPktChat->m_cCmd);
                    break;

                case PktChat::SHOUT:
                case PktChat::ADMIN_SHOUT:
                    CChatSend::GetInstance().SendShout(m_dwUID, m_dwCID, 
                        m_ChatPos, lpChat, nMsgLen, lpPktChat->m_cCmd);
                    break;

                case PktChat::PARTY:     
                    CChatSend::GetInstance().SendPartyChat(m_dwUID, m_dwCID, 
                        m_ChatPos, lpChat, nMsgLen);
                    break;

                case PktChat::GUILD:
                    CChatSend::GetInstance().SendGuildChat(m_dwUID, m_dwCID, 
                        m_ChatPos, lpChat, nMsgLen);
                    break;

                case PktChat::CLIENT_LOG:    // �α�
                    DETLOG4(g_Log, "UID:%10d/CID:%10d/%15s/CLIENT_LOG: %s",
                        m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), lpChat);
                    break;
                }
            }
        }

        break;

    case PktChat::FRIEND:
    case PktChat::STALL:

        // �̸� ����� �޾� �ֺ� �ι��鿡�� ���� ����.
        {
            if(0 == lpPktChat->m_cNum ||
                nPacketLength < int(sizeof(PktChat) + (CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum)) ||
                nDefaultMaxPacketLen + CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum < nPacketLength)
            {
                ERRLOG4(g_Log, "UID:%10d/CID:%10d/%15s/��Ŷ ���̰� �̻��մϴ�/����:%d/", 
                    m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), nPacketLength);
                return false;
            }

            const char* lpNames = reinterpret_cast<char*>(lpPktChat + 1);
            const char* lpChat  = reinterpret_cast<char*>(lpPktChat + 1) + CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum;

            int nMsgLen = nPacketLength - sizeof(PktChat) - (CHAR_INFOST::MAX_NAME_LEN * lpPktChat->m_cNum);

            if(0 < nMsgLen)
            {
                CChatSend::GetInstance().SendTargetChat(m_dwUID, m_dwCID, m_ChatPos, 
                    lpNames, lpChat, nMsgLen, lpPktChat->m_cNum, lpPktChat->m_cCmd);
            }
        }
        break;

    default:

        return false;
    }

	return true;
}

bool CChatClientDispatch::ParseCharWhisper(PktWh* lpPktWh)
{
	int nPacketLength = lpPktWh->GetLen();
    if (nPacketLength < sizeof(PktWh) || sizeof(PktWh) + PktWh::PktWhMaxSize < nPacketLength)
	{
        ERRLOG4(g_Log, "UID:%10d/CID:%10d/%15s/��Ŷ ���̰� �̻��մϴ�/����:%d/", 
            m_dwUID, m_dwCID, m_Session.GetRemoteAddr().get_addr_string(), nPacketLength);
		return false;
	}    

    CChatSend::GetInstance().SendWhisper(m_dwUID, m_dwCID, m_ChatPos, m_SendStream, lpPktWh);
    return true;
}

*/