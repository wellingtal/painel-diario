#ifndef _SEND_CHAR_BATTLEGROUND_SERVER_PACKET_H_
#define _SEND_CHAR_BATTLEGROUND_SERVER_PACKET_H_


// 전방 참조
class CSendStream;

namespace VirtualArea
{
    struct RoomInfo;
	struct ResultInfo;
}


namespace GameClientSendPacket
{
	// 배틀 그라운드 서버군 방에 입장
	bool SendCharBGServerMoveZone(CSendStream& SendStream, unsigned char cZone, unsigned char cMoveType, unsigned short wError);

	// 배틀 그라운드 서버군 환전소 명령
	bool SendCharBGServerMileageChange(CSendStream& SendStream, unsigned long dwCID, unsigned char cGroup, 
		unsigned char cCmd, unsigned long dwGold, unsigned long dwMileage, unsigned short wError);
}

#endif