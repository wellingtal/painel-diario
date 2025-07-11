#ifndef _GAMA_CLIENT_SEND_CASTLE_PACKET_H_
#define _GAMA_CLIENT_SEND_CASTLE_PACKET_H_

#include <Network/Packet/PacketStruct/CastlePacket.h>

namespace ClientNet
{
	// forward decl.
	class CClientEventHandler;
}


namespace SendPacket
{
	// ���� ��� ��Ŷ
	bool CharCastleCmd(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCastleID, 
					   unsigned long dwCastleObjectID, unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd);

	bool CharCampCmd(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCampID,
					 unsigned long dwValue, unsigned long dwValue2, unsigned char cSubCmd);

	bool CharSiegeArmsCmd(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwArmsID, unsigned char cSubCmd);


	// ���� ���� ���� ��Ŷ
	bool CharCastleRight(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCastleID, CastleRight castleRight);

	bool CharCampRight(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCampID, CampRight campRight);

	

	// �� ��¡�� ���� ������ �ֱ�/����
	bool CharTakeCastleJewel(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCastleID, unsigned char cSubCmd,
						unsigned short wItemID, unsigned char cIndex, unsigned char cNum, Item::ItemPos pos);
	
	// ��� ��� ���� �ֱ�/����
	bool CharTakeMaterial(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned long dwCampID, unsigned char cSubCmd,
						unsigned char cNum, Item::ItemPos pos);

	// ���, ���� ���� ����.
	bool WarOnOff(ClientNet::CClientEventHandler* lpHandler, unsigned long dwCID, unsigned char cFlag, unsigned char cType);	
};

#endif