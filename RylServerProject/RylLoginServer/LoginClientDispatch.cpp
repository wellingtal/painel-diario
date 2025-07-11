#include "stdafx.h"
#include "LoginClientDispatch.h"
#include "LoginAgentServerTable.h"

#include <Log/ServerLog.h>
#include <Network/Session/Session.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Network/XORCrypt/XORCrypt.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/ClientToLoginServer.h>

#include <RylServerLibrary/Utility/Setup/ServerSetup.h>

#include <mmsystem.h>

enum LoginClientDispatchConstant
{
	DEFAULT_PROCESS_NUM = 5
};

CLoginClientDispatch::CLoginClientDispatch(CSession& Session)
:	CRylServerDispatch(Session, DEFAULT_PROCESS_NUM)
{

}

CLoginClientDispatch::~CLoginClientDispatch()
{
	
}


bool CLoginClientDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bPacketProcessResult = true;
	
	switch(lpPktBase->GetCmd())
	{
    case CmdSysPing:            
        m_CheckPing.SetLastPingRecvTime(timeGetTime());
        bPacketProcessResult = true;
		break;

	case CmdLoginServerList:
		bPacketProcessResult = ParseLoginServerList(static_cast<PktSvL*>(lpPktBase));
		break;

	case CmdSysPatchAddress:	// 구 런쳐 용
		bPacketProcessResult = ParseSysPatchAddress(static_cast<PktSPI*>(lpPktBase));	
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

bool CLoginClientDispatch::Dispatch()
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

        const int MAX_BUFFER = 128;
        char szBuffer[MAX_BUFFER];

        _snprintf(szBuffer, MAX_BUFFER - 1, 
            "CurrentTime:%u/LastRecvTime:%u/PingCount:%u/FirstCheckTime:%u/PingCheck failed. disconnect now.",
            dwCurrentTime, dwLastPingRecvTime, dwPingCount, dwFirstCheckTime);
        
		szBuffer[MAX_BUFFER - 1] = 0;

        LogErrorPacket(szBuffer, 0);
    }

    return false;
}

bool CLoginClientDispatch::ParseLoginServerList(PktSvL* lpPktSvLPt)
{	    
	unsigned long	dwClientVer		= lpPktSvLPt->m_dwClientVer;	
	unsigned short	usPatcherModel	= lpPktSvLPt->GetError();
	unsigned short	usError = 0;

	CServerSetup& ServerSetup = CServerSetup::GetInstance();

	CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(PktSvLAck));	
	if(0 == lpBuffer)
	{
		ERRLOG0(g_Log, "Cannot create buffer. insufficient memory");
	}
	else
	{
        PktSvLAck* lpSvLAckPt = reinterpret_cast<PktSvLAck*>(lpBuffer->wr_ptr());
    
		if(0 == usPatcherModel)
		{
			usError = 4;
		}
		else if(1 == usPatcherModel)
		{
			// 버젼, 패치
			lpSvLAckPt->m_dwClientVer = ServerSetup.GetClientVer();
			
            CLoginAgentServerTable& agentServerTable = CLoginAgentServerTable::GetInstance();

            if(CLoginAgentServerTable::SERVER_CLOSED == agentServerTable.GetOperateMode())
            {
                usError = 10;

                memset(lpSvLAckPt->m_PatchAddress, 0, sizeof(char) * PktSvLAck::PATCH_ADDRESS_LENGTH);
                memset(&lpSvLAckPt->m_ServerList, 0, sizeof(SERVER_LIST));
            }
            else
            {
                strncpy(lpSvLAckPt->m_PatchAddress, 
                    ServerSetup.GetPatchAddress(), 
                    PktSvLAck::PATCH_ADDRESS_LENGTH - 1);

                lpSvLAckPt->m_PatchAddress[PktSvLAck::PATCH_ADDRESS_LENGTH - 1] = '\0';	

                // 서버 리스트를 얻어 온다.		
                CLoginAgentServerTable::GetInstance().GetServerList(lpSvLAckPt->m_ServerList, 
                    m_Session.GetRemoteAddr().get_addr_in().sin_addr);
            }
		}
		else
		{
            usError = 4;
			ERRLOG1(g_Log, "IP:%15s/Can't recognize patch model.", m_Session.GetRemoteAddr().get_addr_string());			
		}

		if(!PacketWrap::WrapCryptOld(lpBuffer, sizeof(PktSvLAck), CmdLoginServerList, 0, usError))
		{	
			ERRLOG1(g_Log, "IP:%15s/Can't WrapCrypt packet.", m_Session.GetRemoteAddr().get_addr_string());
		}
		else if(m_Session.SendPending(lpBuffer))
		{
			return true;
		}

		SAFE_RELEASE_BUFFER(lpBuffer);
	}

	
	return false;
}

bool CLoginClientDispatch::ParseSysPatchAddress(PktSPI* lpPktSPIPt)
{
	CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(PktSPIAck));

	if(0 == lpBuffer)
	{
		ERRLOG0(g_Log, "Cannot create buffer. insufficient memory");
	}
	else
	{
        CServerSetup& ServerSetup = CServerSetup::GetInstance();

		PktSPIAck* lpSLAckPt = reinterpret_cast<PktSPIAck*>(lpBuffer->wr_ptr());
	    
		lpSLAckPt->m_dwClientVer = ServerSetup.GetClientVer();

        strncpy(lpSLAckPt->m_PatchAddress, 
			ServerSetup.GetPatchAddress(), PktSPIAck::PATCH_ADDRESS_LENGTH - 1);

        lpSLAckPt->m_PatchAddress[PktSPIAck::PATCH_ADDRESS_LENGTH - 1] = '\0';

        if(!PacketWrap::WrapHeader(lpBuffer, sizeof(PktSPIAck), CmdSysPatchAddress, 0, 0))
		{
			ERRLOG1(g_Log, "IP:%15s/Can't WrapCrypt packet.", m_Session.GetRemoteAddr().get_addr_string());
		}
		else if(m_Session.SendPending(lpBuffer))
		{
			return true;
		}

		SAFE_RELEASE_BUFFER(lpBuffer);
	}

	return false;
}
