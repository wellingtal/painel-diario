#include "stdafx.h"
#include "LogDispatch.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Dispatch/GameLog/SendLogPacket.h>

#include <Utility/Setup/ServerSetup.h>

CLogDispatch::CLogDispatch(CSession& Session)
:   CRylServerDispatch(Session, 0xFFFFFFFF)
{
    
}


CLogDispatch::~CLogDispatch()
{

}

void CLogDispatch::Connected()
{
    SendLogPacket::ServerLogin(m_SendStream, CServerSetup::GetInstance().GetServerID());
}

void CLogDispatch::Disconnected()
{
    ERRLOG0(g_Log, "로그서버와의 연결이 끊어졌습니다.");
    GetDispatchTable().RemoveDispatch(this);
}

bool CLogDispatch::DispatchPacket(PktBase* lpPktBase)
{    
	bool bResult = false;

    switch(lpPktBase->GetCmd())
    {
    case CmdSysServerLogin:     
		bResult = ProcessServerLoginAck(lpPktBase);
		break;

	default:
		bResult = true;
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Logpacket", 
			this, lpPktBase->GetCmd());

		break;
    }

	if(!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Logpacket Process failed", 
			this, lpPktBase->GetCmd());
	}

    return true;
}


bool CLogDispatch::ProcessServerLoginAck(PktBase* lpPktBase)
{
    // 받은 데이터는 별로 쓸모가 없음..

    if(0 == lpPktBase->GetError())
    {
        DETLOG0(g_Log, "로그서버와 연결되었습니다.");
        GetDispatchTable().SetDispatch(this);
        return true;
    }

    ERRLOG0(g_Log, "로그서버와 연결에 실패했습니다.");    
    return false;
}

