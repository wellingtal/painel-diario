
#include "stdafx.h"

#include "ChatSend.h"

#include "ChatAgentServerDispatch.h"
#include "ChatGameServerDispatch.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/FriendPacket.h>
#include <Network/Broadcast/ChatCharData.h>
#include <Network/Winsock/SocketFactory.h>

#include <Creature/Character/CharacterStructure.h>
#include <Log/ServerLog.h>

#include <Utility/Setup/ServerSetup.h>

CSingleDispatch& CChatAgentServerDispatch::GetDispatchTable()
{
	static CSingleDispatch singleDispatch;
	return singleDispatch;
}


CChatAgentServerDispatch::CChatAgentServerDispatch(CSession& Session)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{
    
}

void CChatAgentServerDispatch::Connected()
{
	DETLOG1(g_Log, "this:0x%p/�߰輭���� ����Ǿ����ϴ�.", this);

    if(!SendServerLoginToAgentServer(CServerSetup::GetInstance().GetServerID()))
    {
        INET_Addr& agentAddress = 
            CServerSetup::GetInstance().GetServerAddress(CServerSetup::AgentServer);

        ERRLOG1(g_Log, "�߰� ����(%16s)�� ä�� ���� �α��� �޽����� ���� �� �����ϴ�.",
            agentAddress.get_addr_string());
    }
}    

void CChatAgentServerDispatch::Disconnected()
{
	GetDispatchTable().RemoveDispatch(this);
    DETLOG1(g_Log, "this:0x%p/�߰輭������ ������ ������ϴ�.", this);
}


bool CChatAgentServerDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bResult = false;

    switch(lpPktBase->GetCmd())
    {
    case CmdSysServerLogin:     bResult = ProcessServerLoginAck(static_cast<PktSLAck*>(lpPktBase));		break;
    case CmdHanBTNWarning:      bResult = ProcessHanBTNWarning(static_cast<PktHanBTN*>(lpPktBase));     break;

	default:

		bResult = true;
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Command packet", this, lpPktBase->GetCmd());
		break;
	}

	if(!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Packet process failed", this, lpPktBase->GetCmd());
	}

	// �߸��� Ŀ�ǵ� ��Ŷ�� �͵� ������ �ʴ´�.
    return true;
}

// �߰輭���� �α��� Ack
bool CChatAgentServerDispatch::ProcessServerLoginAck(PktSLAck* lpPktSLAck) 
{
    GetDispatchTable().SetDispatch(this);
    return true;
}


bool CChatAgentServerDispatch::SendServerLoginToAgentServer(unsigned long dwServerID)
{
    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktSL));
    if (NULL != lpBuffer)
    {
        PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

        lpPktSL->m_dwServerID = dwServerID;

        const int MAX_ADDRESS = 128;
        char szAddress[MAX_ADDRESS];

        CTCPFactory tcpFactory;		
		tcpFactory.GetNetworkInfo(szAddress, MAX_ADDRESS);

        lpPktSL->m_Address.S_un.S_addr = inet_addr(szAddress);
   
        return m_SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }

    return false;
}


bool CChatAgentServerDispatch::ProcessHanBTNWarning(PktHanBTN* lpPktHanBTN)
{
    ChatData::CCharInfo* lpCharInfo = 
        ChatData::CCharInfoMap::GetInstance().GetCharInfoByCID(lpPktHanBTN->m_dwCID);

    if(0 != lpCharInfo && lpCharInfo->GetUID() == lpPktHanBTN->m_dwUID)
    {
        GET_MULTI_DISPATCH(lpDispatch, lpCharInfo->GetServerID(), 
            CChatGameServerDispatch, CChatGameServerDispatch::GetDispatchTable());

        if(0 != lpDispatch)
        {
            lpPktHanBTN->m_szMsg[PktHanBTN::MAX_HAN_BTN - 1] = 0;

            CServerChatPacket serverChatPacket(lpPktHanBTN->m_szMsg, 
                PktChat::NOTICE, 0, lpCharInfo->GetInclinationData(), 
                lpCharInfo->GetName(), &lpPktHanBTN->m_dwCID, 1);

            if(serverChatPacket.IsValid())
            {
                DETLOG3(g_Log, "UID:%u/CID:%u/�Ѱ��� �������� ���� ���� �� ��� �޽��� ��� ����(%s)",
                    lpPktHanBTN->m_dwUID, lpPktHanBTN->m_dwCID, lpPktHanBTN->m_szMsg);

                return lpDispatch->GetSendStream().PutBuffer(
                    serverChatPacket.GetCompressedPacket(),
                    serverChatPacket.GetCompressedSize(), CmdCharChat);                
            }
        }
    }

    ERRLOG3(g_Log, "UID:%u/CID:%u/�Ѱ��� �������� ���� ���� �� ��� �޽��� ��� ����(%s)",
        lpPktHanBTN->m_dwUID, lpPktHanBTN->m_dwCID, lpPktHanBTN->m_szMsg);

    return true;
}
