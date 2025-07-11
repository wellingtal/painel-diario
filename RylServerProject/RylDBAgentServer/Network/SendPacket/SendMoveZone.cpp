#include "stdafx.h"
#include "SendMoveZone.h"

#include <Network/Stream/SendStream.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Utility/Setup/ServerSetup.h>

namespace DBAgent
{
namespace SendPacket
{

bool CharMoveZone(CSendStream& SendStream, unsigned long dwRequestKey, 
                  unsigned char cZone, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSZMvDAck));
    if(0 != lpBuffer)
    {
        PktSZMvDAck* lpPktSZMvDAck = reinterpret_cast<PktSZMvDAck*>(lpBuffer);

        lpPktSZMvDAck->m_dwRequestKey   = dwRequestKey;
        lpPktSZMvDAck->m_wCmd           = PktDD::SCmdCharMoveZone;

        lpPktSZMvDAck->m_cZone          = cZone;

        CUserPercentageInZone getUserPercentage(
            lpPktSZMvDAck->m_wChannelNum, cZone, PktSZMvDAck::MAX_CHANNEL_NUM);

        CGameDispatch::GetDispatchTable().Process(getUserPercentage);

        return SendStream.WrapHeader(sizeof(PktSZMvDAck), CmdAgentZone, 0, usError);
    }
    
    return false;
}

bool ServerZone(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned char cZone, unsigned char cChannel, unsigned short usError)
{   
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSAAck));
    if(0 != lpBuffer)
    {
        unsigned short usError = 0;

        PktSAAck* lpPktSAAck = reinterpret_cast<PktSAAck*>(lpBuffer);
    
        lpPktSAAck->m_dwRequestKey  = dwRequestKey;
        lpPktSAAck->m_wCmd          = PktDD::SCmdServerZone;

        lpPktSAAck->m_dwServerID	            = 0;
        lpPktSAAck->m_GameAddress.S_un.S_addr   = 0;
        
        if(0 == usError)
        {
            SERVER_ID serverID;
            serverID.sID.Type       = CServerSetup::GameServer;
            serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
            serverID.sID.Channel    = cChannel;
            serverID.sID.ID         = cZone;

            GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            int nUserLimit = static_cast<int>(CServerSetup::GetInstance().GetUserLimit());

            if(0 == lpGameDispatch)
            {
                usError = PktSAAck::SERVER_ERROR;
            }
            else if(nUserLimit < lpGameDispatch->GetCharNum())
            {
                usError = PktSAAck::FAIL_USER_LIMIT;
            }
            else
            {
                lpPktSAAck->m_dwServerID	= lpGameDispatch->GetServerID();
                lpPktSAAck->m_GameAddress	= lpGameDispatch->GetRemoteAddr().get_addr_in().sin_addr;
            }
        }

        return SendStream.WrapHeader(sizeof(PktSAAck), CmdAgentZone, 0, usError);
    }

    return false;
}

bool ServerZoneEnd(CSendStream& SendStream, unsigned long dwRequestKey)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDD));

    if(0 != lpBuffer)
    {
        PktDD* lpPktDD = reinterpret_cast<PktDD*>(lpBuffer);

        lpPktDD->m_dwRequestKey = dwRequestKey;
        lpPktDD->m_wCmd = PktDD::SCmdServerZoneEnd;

        return SendStream.WrapHeader(sizeof(PktDD), CmdAgentZone, 0, 0);
    }

    return 0;
}


}
}