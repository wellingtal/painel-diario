#include "stdafx.h"
#include "ChatDispatch.h"

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/SendPacket/SendServerInfo.h>

namespace DBAgent
{

CSingleDispatch& CChatDispatch::GetDispatchTable()
{
    static CSingleDispatch chatDispatch;
    return chatDispatch;
}

CChatDispatch::CChatDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{


}

CChatDispatch::~CChatDispatch()
{


}

void CChatDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());
}

void CChatDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ChatServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    if(0 != m_dwServerID)
    {
        GetDispatchTable().RemoveDispatch(this);
    }
}

bool CChatDispatch::DispatchPacket(PktBase* lpPktBase)
{    
    bool bResult = false;
    PktBase::CMDType cCmd = lpPktBase->GetCmd();

    switch(cCmd)
    {
    case CmdSysServerLogin:
        bResult = ParseServerLogin(static_cast<PktSL*>(lpPktBase));
        break;

    case CmdSysServerLogout:
        bResult = ParseServerLogout(lpPktBase);
        break;

    default:

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ ä�ü��� ��Ŷ ó�� ���� : ���� Ŀ�ǵ��Դϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ ä�ü��� ��Ŷ ó�� ���� : ó���� �����߽��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}


bool CChatDispatch::ParseServerLogin(PktSL* lpPktSL)
{
    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ ä�ü��� ���� �õ� : ��Ŷ ����",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

    GET_SINGLE_DISPATCH(lpChatDispatch, 
        CChatDispatch, GetDispatchTable());

    if(0 != lpChatDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ ä�ü��� ���� ���� : �̹� ������ �ֽ��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch ����.
        GetDispatchTable().SetDispatch(this);
        return SendPacket::ServerLoginAck(m_SendStream, m_dwServerID, 0LL);
    }

    return true;
}

bool CChatDispatch::ParseServerLogout(PktBase* lpPktBase)
{
    // ���� ��Ŷ �״�� ���� �ش�.

    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));
    if(0 != lpBuffer)
    {
        return m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return false;
}



}


