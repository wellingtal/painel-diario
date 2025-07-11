#include "stdafx.h"
#include "SendGuild.h"

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>

#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameDispatch.h>

namespace DBAgent
{
namespace SendPacket
{        


bool GuildCmd(CSendStream* lpSendStream, 
                  unsigned long dwGID, unsigned long dwSenderID, unsigned long dwReferenceID, 
                  unsigned short wCmd, const char* szSenderName, const char* szGuildName, unsigned short wError)
{
    PktGuildCmd pktGuildCmd;
    memset(&pktGuildCmd, 0, sizeof(PktGuildCmd));

    pktGuildCmd.m_dwGID = dwGID;
    pktGuildCmd.m_dwSenderID = dwSenderID;
    pktGuildCmd.m_dwReferenceID = dwReferenceID;
    pktGuildCmd.m_wCmd = wCmd;

	if(szSenderName)
		strncpy(pktGuildCmd.m_szSenderName, szSenderName, MAX_MEMBER_NAME_LEN);

	if(szGuildName)
		strncpy(pktGuildCmd.m_szGuildName, szGuildName, MAX_GUILD_NAME_LEN);
	

    if (PktBase::NO_SERVER_ERR == wError)
    {
        // 서버 에러가 아닌 경우 전 서버에 보낸다.
        if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildCmd), 
            sizeof(PktGuildCmd), CmdGuildCmd, 0, wError))
        {
            DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
                reinterpret_cast<char*>(&pktGuildCmd), sizeof(PktGuildCmd), CmdGuildCmd));

            return true;
        }
    }
    else if(0 != lpSendStream)
    {
        // 서버 에러인경우, 해당 서버에만 보낸다.
        return lpSendStream->WrapCompress(reinterpret_cast<char*>(&pktGuildCmd), 
            sizeof(PktGuildCmd), CmdGuildCmd, 0, wError);
    }

    return false;
}

bool GuildMemberFameUpdate(unsigned long dwCID, unsigned long dwOurGID, unsigned char cType, int nAddFame, unsigned char cZone, unsigned long dwEnemyGID)
{
    PktGuildMemberFameUpdate pktGuildMemberFameUpdate;
    memset(&pktGuildMemberFameUpdate, 0, sizeof(PktGuildMemberFameUpdate));

    pktGuildMemberFameUpdate.m_dwCID = dwCID;
    pktGuildMemberFameUpdate.m_dwOurGID = dwOurGID;
	pktGuildMemberFameUpdate.m_dwEnemyGID = dwEnemyGID;
	pktGuildMemberFameUpdate.m_cType = cType;
    pktGuildMemberFameUpdate.m_nAddFame = nAddFame;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildMemberFameUpdate),
        sizeof(PktGuildMemberFameUpdate), CmdGuildMemberFameUpdate, 0, 0))
    {
        DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketToZone(
            reinterpret_cast<char*>(&pktGuildMemberFameUpdate),
            sizeof(PktGuildMemberFameUpdate), CmdGuildMemberFameUpdate, cZone));

        return true;
    }

    return false;
}

bool GuildMemberGoldUpdate(unsigned long dwCID, unsigned long dwGID, unsigned long dwGold, unsigned char cZone)
{
	PktGuildMemberGoldUpdate pktGuildMemberGoldUpdate;
	memset(&pktGuildMemberGoldUpdate, 0, sizeof(PktGuildMemberGoldUpdate));

	pktGuildMemberGoldUpdate.m_dwCID	= dwCID;
	pktGuildMemberGoldUpdate.m_dwGID	= dwGID;
	pktGuildMemberGoldUpdate.m_dwGold	= dwGold;

	if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildMemberGoldUpdate),
		sizeof(PktGuildMemberGoldUpdate), CmdGuildMemberGoldUpdate, 0, 0))
	{
		DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketToZone(
			reinterpret_cast<char*>(&pktGuildMemberGoldUpdate),
			sizeof(PktGuildMemberGoldUpdate), CmdGuildMemberGoldUpdate, cZone));

		return true;
	}

	return false;
}

/*bool KeyInfo(unsigned long dwCID)
{
	PktGuildMemberGoldUpdate pktGuildMemberGoldUpdate;
	memset(&pktGuildMemberGoldUpdate, 0, sizeof(PktGuildMemberGoldUpdate));

	return false;
}*/


}
}
