#ifndef _SEND_CHAR_LOGIN_OUT_H_
#define _SEND_CHAR_LOGIN_OUT_H_

#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>

#include <winsock2.h>
#include <windows.h>
#include <DB/DBDefine.h>

// 전방 참조
class CCharacter;
class CSendStream;
class CGameClientDispatch;

struct PktDBUpdate;
struct _GG_AUTH_DATA;

namespace GameClientSendPacket
{
    // Client로 전송
    bool SendCharLogin(CSendStream& SendStream, CCharacter* lpCharacter, 
		const SOCKADDR_IN* lpSockAddr_In, unsigned short usError);
    
    bool SendCharMoveZone(CSendStream& SendStream, unsigned short* lpChannelNums, 
        unsigned char cChannelNum, unsigned char cZone, unsigned short usError);

    bool SendServerZone(CSendStream& SendStream, unsigned long dwServerID, 
        const sockaddr_in& gameServerTCPAddress, unsigned short usError);

	bool SendCSAuth(CSendStream& SendStream, unsigned long dwCID, 
		unsigned long dwAuthCode, const _GG_AUTH_DATA* lpAuthCode2, unsigned short usError);


    // DBAgent로 전송
    bool SendLoginToDBAgent(CSendStream& AgentSendStream, unsigned long dwRequestKey, 
        unsigned long dwSessionID, unsigned long dwUserID, 
        unsigned long dwCharID, const IN_ADDR remoteAddress);

    bool SendMoveZoneToDBAgent(CSendStream& AgentSendStream, POS& newPos, 
        unsigned long dwRequestKey, unsigned long dwUserID, unsigned char cZone, char cChannel);

    bool SendServerZoneToDBAgent(CSendStream& AgentSendStream, 
        unsigned long dwRequestKey, unsigned long dwUserID, 
        unsigned char cZone, unsigned char cChannel);

	bool SendKeyInfo(CSendStream& SendStream, unsigned long dwCID, unsigned long* pKeyInfo);
};

#endif