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
    ERRLOG0(g_Log, "�α׼������� ������ ���������ϴ�.");
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
    // ���� �����ʹ� ���� ���� ����..

    if(0 == lpPktBase->GetError())
    {
        DETLOG0(g_Log, "�α׼����� ����Ǿ����ϴ�.");
        GetDispatchTable().SetDispatch(this);
        return true;
    }

    ERRLOG0(g_Log, "�α׼����� ���ῡ �����߽��ϴ�.");    
    return false;
}

