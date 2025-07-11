#ifndef _PARSE_CHAR_CASTLE_PACKET_H_
#define _PARSE_CHAR_CASTLE_PACKET_H_

// ���� ����
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
	// �� ���� ��� ��Ŷ ó��
	bool ParseCharCastleCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// ��� ��� ���� ��� ��Ŷ ó��
	bool ParseCharCampCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// ���� ���� ���� ��� ��Ŷ ó��
	bool ParseCharSiegeArmsCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);


	// �� ���� ���� ��Ŷ ó��
	bool ParseCharCastleRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// ��� ��� ���� ���� ��Ŷ ó��
	bool ParseCharCampRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);


	// �� ��¡���� ���� ������ �ֱ�/����
	bool ParseCharTakeCastleJewel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
	
	// ��� ��� ���� �ֱ�/����
	bool ParseCharTakeMaterial(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// ���, ���� ���� On, Off ����.
	bool ParseWarOnOff(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
};


#endif