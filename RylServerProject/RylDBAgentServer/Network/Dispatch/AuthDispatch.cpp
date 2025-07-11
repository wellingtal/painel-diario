#include "stdafx.h"
#include "AuthDispatch.h"

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>
#include <Network/Address/INET_Addr.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendLoginout.h>

#include <Network/ParsePacket/ParseCharManage.h>
#include <Network/ParsePacket/ParseGuild.h>
#include <Network/ParsePacket/ParseParty.h>
#include <Network/ParsePacket/ParseMoveZone.h>

#include <Network/Packet/PacketCommand.h>

#include <Network/Dispatch/LoginDispatch.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>


namespace DBAgent
{

CSingleDispatch& CAuthDispatch::GetDispatchTable()
{
    static CSingleDispatch authDispatch;
    return authDispatch;
}


CAuthDispatch::CAuthDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
        m_dwServerID(0), 
        m_nUserNum(1)
{


}

CAuthDispatch::~CAuthDispatch()
{


}

void CAuthDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/AuthServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());
}

void CAuthDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/AuthServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    if(0 != m_dwServerID)
    {
        GetDispatchTable().RemoveDispatch(this);

        if(0LL != m_AuthItemSerialMgr.GetItemSerial())
        {
            // ������ �ø��� DB�� ����
            m_AuthItemSerialMgr.SaveItemSerial(
                CDBSingleObject::GetInstance(), m_dwServerID);
        }

        GET_SINGLE_DISPATCH(lpLoginDispatch, CLoginDispatch, 
            CLoginDispatch::GetDispatchTable());

        if(0 != lpLoginDispatch)
        {
            // �α��μ����� ���� ���� ������Ʈ
            SendPacket::UpdateServerVersion(lpLoginDispatch->GetSendStream());
        }

        // ���� ������ �α��ε� ����� ���� ����������.
        DataStorage::CSessionDataMgr::GetInstance().SessionCloseCurrentServer(m_dwServerID);
    }    
}

bool CAuthDispatch::DispatchPacket(PktBase* lpPktBase)
{
	// Auth ���� ���� ��Ŷ�� ó���ϴ� �κ��̴�.

    bool bResult = false;
    PktBase::CMDType cCmd = lpPktBase->GetCmd();

    switch(cCmd)
    {
    case CmdSysServerLogin:     bResult = ParseServerLogin(static_cast<PktSL*>(lpPktBase));         break;
    case CmdSysServerLogout:    bResult = ParseServerLogout(lpPktBase);                             break;
    case CmdUserKill:           bResult = ParseUserKill(static_cast<PktUK*>(lpPktBase));            break;

    case CmdDBGetData:
		{
			PktDD* pkDD = static_cast<PktDD*>(lpPktBase);

//			DETLOG4(g_Log, "SS:0x%08x/DP:0x%08x/Len:0x%08x/CMD:0x%04x : CmdDBGetData", &GetSession(), this, pkDD->GetLen(), pkDD->m_wCmd);

			bResult = ParseCharManage::Parse(GetSendStream(), m_dwServerID, 
				m_AuthItemSerialMgr, pkDD);
		}
        break;

    case CmdAgentZone:  bResult = ParseMoveZone::Parse(GetSendStream(), static_cast<PktDD*>(lpPktBase));    break;
    case CmdAgentParty: bResult = ParseParty::Parse(GetSendStream(), lpPktBase);                            break;
    case CmdGuildCmd:   bResult = ParseGuild::GuildCmd(GetSendStream(), lpPktBase);                         break;


    case CmdUnifiedCharSelect:
        bResult = ParseCharManage::UnifiedCharSelect(GetSendStream(), lpPktBase);
        break;

    default:

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ �������� ��Ŷ ó�� ���� : ���� Ŀ�ǵ��Դϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ �������� ��Ŷ ó�� ���� : ó���� �����߽��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}


bool CAuthDispatch::ParseServerLogin(PktSL* lpPktSL)
{
    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ �������� ���� �õ� : ��Ŷ ����",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);
    
    GET_SINGLE_DISPATCH(lpAuthDispatch, 
        CAuthDispatch, GetDispatchTable());

    if(0 != lpAuthDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ �������� ���� ���� : �̹� ������ �ֽ��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch ����.
        GetDispatchTable().SetDispatch(this);

        // ������ �ø��� �ε�.
        if(!m_AuthItemSerialMgr.LoadItemSerial(CDBSingleObject::GetInstance(), m_dwServerID))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/�������� ���� ���� : ������ �ø��� �ε� ����. ������ �����ϴ�.",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

            CloseSession();
            return false;
        }

        GET_SINGLE_DISPATCH(lpLoginDispatch, CLoginDispatch, 
            CLoginDispatch::GetDispatchTable());

        if(0 != lpLoginDispatch)
        {
            // �α��μ����� ���� ���� ������Ʈ
            SendPacket::UpdateServerVersion(lpLoginDispatch->GetSendStream());
        }
        
        return SendPacket::ServerLoginAck(m_SendStream, 
            m_dwServerID, m_AuthItemSerialMgr.GetItemSerial());
    }

    return true;
}

bool CAuthDispatch::ParseServerLogout(PktBase* lpPktBase)
{
    // ���� ��Ŷ �״�� ���� �ش�.

    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));
    if(0 != lpBuffer)
    {
        return m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return false;
}

bool CAuthDispatch::ParseUserKill(PktUK* lpPktUK)
{
    // ���� ���̱⸦ �����ؼ�, Ack�� ���� ����̴�.

    if(2 == lpPktUK->GetError())
    {
        DataStorage::CSessionData* lpSessionData = 
            DataStorage::CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

        if(0 != lpSessionData && 
            DataStorage::CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState())
        {
            if(lpSessionData->GetServerID() == lpPktUK->m_dwServerID)
            {
                DataStorage::CSessionDataMgr::GetInstance().SessionCloseWithLogout(
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID);

                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ���Ӽ��� ���� ���� ���� : ĳ���� ���̱� ����", 
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID);
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u / FromServerID:0x%08X / TargetServerID:0x%08X / "
                    "���Ӽ��� ���� ���� ���� ���� : ���� ������ ���̱� ��û ������ �ٸ�",
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID, lpSessionData->GetServerID());
            }
        }
    }

    return true;
}

}