#ifndef _SEND_GAME_SERVER_PACKET_H_
#define _SEND_GAME_SERVER_PACKET_H_

#include <winsock2.h>
#include <windows.h>

// ���� ����
class  CSendStream;

namespace GameServerSendPacket
{
    
    bool SendUpdateAddressAck(CSendStream& SendStream, unsigned long dwCID, 
        const SOCKADDR_IN& PublicAddr, const SOCKADDR_IN& PrivateAddr);


};

#endif