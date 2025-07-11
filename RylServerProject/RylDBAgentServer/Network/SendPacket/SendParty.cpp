#include "stdafx.h"
#include "SendParty.h"
#include <Community/Party/PartyDBInfo.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/DataPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Dispatch/GameDispatch.h>

namespace DBAgent
{
namespace SendPacket
{        

bool DeleteParty(CPartyDBInfo* lpParty)
{
    if(0 != lpParty)
    {
        PktDPD pktDPD;
        memset(&pktDPD, 0, sizeof(PktDPD));

        pktDPD.m_wCmd       = PktDD::SCmdDeleteParty;
        pktDPD.m_dwPartyID  = lpParty->GetPID();

        if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktDPD),
            sizeof(PktDPD), CmdAgentParty, 0, 0))
        {
            CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktDPD), sizeof(PktDPD), CmdAgentParty));

            return true;
        }
    }

    return false;
}

bool PartyCmd(CSendStream& SendStream, unsigned short cSubCmd, unsigned long dwPID, unsigned long dwSenderID, unsigned long dwGID, 
				unsigned short wClass, unsigned long dwServerID, char cLevel, const char* szSenderName, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktPMD));

    if(0 != lpBuffer)
    {
        PktPMD* lpPktPMD = reinterpret_cast<PktPMD*>(lpBuffer);

        lpPktPMD->m_wCmd            = cSubCmd;
        lpPktPMD->m_dwPartyID       = dwPID;
        lpPktPMD->m_dwSenderID      = dwSenderID;
		lpPktPMD->m_dwGID			= dwGID;
		lpPktPMD->m_wClass			= wClass;
		lpPktPMD->m_dwServerID		= dwServerID;
		lpPktPMD->m_cLevel			= cLevel;

        if(0 != szSenderName)
        {
            strncpy(lpPktPMD->m_strSenderName, szSenderName, CHAR_INFOST::MAX_NAME_LEN);
            lpPktPMD->m_strSenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
        }
        else
        {
            memset(lpPktPMD->m_strSenderName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
        }

        return SendStream.WrapHeader(sizeof(PktPMD), CmdAgentParty, 0, usError);
    }

    return false;
}

bool PartyCmd(unsigned short cSubCmd, unsigned long dwPID, unsigned long dwSenderID, unsigned long dwGID, 
			  unsigned short wClass, unsigned long dwServerID, char cLevel, const char* szSenderName)
{
    PktPMD pktPMD;
    memset(&pktPMD, 0, sizeof(PktPMD));
    
    pktPMD.m_wCmd			= cSubCmd;
    pktPMD.m_dwPartyID		= dwPID;
    pktPMD.m_dwSenderID		= dwSenderID;
    pktPMD.m_dwGID			= dwGID;
	pktPMD.m_wClass			= wClass;
	pktPMD.m_dwServerID		= dwServerID;
	pktPMD.m_cLevel			= cLevel;	

    if (0 != szSenderName)
    {
        strncpy(pktPMD.m_strSenderName, szSenderName, CHAR_INFOST::MAX_NAME_LEN);
        pktPMD.m_strSenderName[CHAR_INFOST::MAX_NAME_LEN - 1];
    }
    else
    {
        memset(pktPMD.m_strSenderName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    }

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktPMD), 
        sizeof(PktPMD), CmdAgentParty, 0, 0))
    {
		// 게임 서버로 브로드캐스팅 //
        CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
            reinterpret_cast<char*>(&pktPMD), sizeof(PktPMD), CmdAgentParty));

        return true;
    }

    return false;
}



}
}
