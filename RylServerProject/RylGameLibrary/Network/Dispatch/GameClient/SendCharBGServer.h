#ifndef _SEND_CHAR_BATTLEGROUND_SERVER_PACKET_H_
#define _SEND_CHAR_BATTLEGROUND_SERVER_PACKET_H_


// ���� ����
class CSendStream;

namespace VirtualArea
{
    struct RoomInfo;
	struct ResultInfo;
}


namespace GameClientSendPacket
{
	// ��Ʋ �׶��� ������ �濡 ����
	bool SendCharBGServerMoveZone(CSendStream& SendStream, unsigned char cZone, unsigned char cMoveType, unsigned short wError);

	// ��Ʋ �׶��� ������ ȯ���� ���
	bool SendCharBGServerMileageChange(CSendStream& SendStream, unsigned long dwCID, unsigned char cGroup, 
		unsigned char cCmd, unsigned long dwGold, unsigned long dwMileage, unsigned short wError);
}

#endif