#include "stdafx.h"
#include "SendGameServerPacket.h"

#include <Network/Stream/SendStream.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/AddressPacket.h>

/*
bool SendSysServerLogin(CSendStream& SendStream, unsigned long dwServerID_In);

bool GameServerSendPacket::SendSysServerLogin(CSendStream& SendStream, unsigned long dwServerID_In)
{

    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSL));
    if(NULL != lpBuffer)
    {
        PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

        const int MAX_ADDRESS = 128;
        char szAddress[MAX_ADDRESS];

        CTCPFactory tcpFactory;		
		tcpFactory.GetNetworkInfo(szAddress, MAX_ADDRESS);

        lpPktSL->m_Address.S_un.S_addr = inet_addr(szAddress);
        lpPktSL->m_dwServerID = dwServerID_In;

        return SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }

    return false;
}
*/


bool GameServerSendPacket::SendUpdateAddressAck(CSendStream& SendStream, unsigned long dwCID, 
                                                const SOCKADDR_IN& PublicAddr, const SOCKADDR_IN& PrivateAddr)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUAAck));
    if(NULL != lpBuffer)
    {
        PktUAAck* lpPktUAAck = reinterpret_cast<PktUAAck*>(lpBuffer);
        
        lpPktUAAck->m_dwCharID			= dwCID;
	    lpPktUAAck->m_PublicAddress		= PublicAddr;
	    lpPktUAAck->m_PrivateAddress	= PrivateAddr;

        return SendStream.WrapCrypt(sizeof(PktUAAck), CmdCharUpdateAddress, 0, 0);
    }

    return false;
}

