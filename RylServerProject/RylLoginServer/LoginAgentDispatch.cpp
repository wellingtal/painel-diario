#include "stdafx.h"
#include "LoginAgentDispatch.h"
#include "LoginAgentServerTable.h"

#include <Log/ServerLog.h>
#include <Network/Session/Session.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

enum LoginAgentDispatchConstant
{
	DEFAULT_PROCESS_NUM = 5
};


CLoginAgentDispatch::CLoginAgentDispatch(CSession& Session)
:   CRylServerDispatch(Session, DEFAULT_PROCESS_NUM), m_dwServerID(0)
{

}

CLoginAgentDispatch::~CLoginAgentDispatch()
{


}

bool CLoginAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bPacketProcessResult = true;
	
	switch(lpPktBase->GetCmd())
	{
    case CmdSysServerLogin:
		bPacketProcessResult = ParseSysServerLogin(static_cast<PktSL*>(lpPktBase));
		break;

    case CmdSysServerVerUpdate:
		bPacketProcessResult = ParseSysServerVerUpdate(static_cast<PktSVU*>(lpPktBase));
		break;

    case CmdSysChannelUpdate:
		bPacketProcessResult = ParseSysChannelUpdate(static_cast<PktSCInfo*>(lpPktBase)); 
		break;
	
	default:
		LogErrorPacket("Invalid packet command", lpPktBase->GetCmd());
		break;
	}

	if(!bPacketProcessResult)
	{		
		LogErrorPacket("Packet process failed", lpPktBase->GetCmd());
	}

	return true;
}

void CLoginAgentDispatch::Disconnected()
{
	//  관리툴 관련.
    CLoginAgentServerTable::GetInstance().RemoveConnection(m_dwServerID, this);
}


bool CLoginAgentDispatch::ParseSysServerLogin(PktSL* lpPktSLPt)
{
	// 서버군 등록
    m_dwServerID = lpPktSLPt->m_dwServerID;

    if(!CLoginAgentServerTable::GetInstance().AddConnection(
        lpPktSLPt->m_dwServerID, lpPktSLPt->m_Address, this))
    {
		ERRLOG1(g_Log, "ServerID:0x%08x/Can't register server group. Current registered now.", m_dwServerID);
		return false;
	}

    DETLOG1(g_Log, "ServerID:0x%08x/ServerGroup Login", m_dwServerID);
    return true;
}

bool CLoginAgentDispatch::ParseSysServerVerUpdate(PktSVU* lpSVUPt)
{
    if(CLoginAgentServerTable::GetInstance().UpdateServerData(m_dwServerID, 
		lpSVUPt->m_ServerAddr, lpSVUPt->m_dwClientVer, lpSVUPt->m_PatchAddress))
    {        
		DETLOG3(g_Log, "ServerID:0x%08x/ClientVersion:%u/PatchAddress:%s/Agent Info Updated", 
            lpSVUPt->m_dwServerID, lpSVUPt->m_dwClientVer, lpSVUPt->m_PatchAddress);
    }
    else
    {
		ERRLOG4(g_Log, "ServerID:0x%08x/NewServerID:0x%08x/ClientVersion:%u/PatchAddress:%s/"
			"Server version update failed. ServerGroup is invalid",            
            m_dwServerID, lpSVUPt->m_dwServerID, lpSVUPt->m_dwClientVer, lpSVUPt->m_PatchAddress);
    }

    return true;
}


bool CLoginAgentDispatch::ParseSysChannelUpdate(PktSCInfo* lpPktSCInfoPt)
{
    if(!CLoginAgentServerTable::GetInstance().UpdateChannelUserNum(
		lpPktSCInfoPt->m_dwServerID, 
		lpPktSCInfoPt->m_usChannelClientNum, 
		lpPktSCInfoPt->m_cChannelNum))
    {
		ERRLOG2(g_Log, "ServerID:0x%08x/ChannelNum%u/Invalid channel info update", 
            lpPktSCInfoPt->m_dwServerID, lpPktSCInfoPt->m_cChannelNum);
    }

    return true;
}


