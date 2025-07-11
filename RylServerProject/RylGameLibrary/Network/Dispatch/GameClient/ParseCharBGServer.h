#ifndef _PARSE_CHAR_BATTLEGROUND_SERVER_PACKET_H_
#define _PARSE_CHAR_BATTLEGROUND_SERVER_PACKET_H_

// ���� ����
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
	bool ParseCharBGServerMapList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��Ʋ �׶��� ������ ��(��) ���� ����Ʈ
	bool ParseCharBGServerResultList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��Ʋ �׶��� ������ ��(��) ��� ����Ʈ
	bool ParseCharBGServerMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��Ʋ �׶��� ������ �� �̵� (��(��) �̵�)
	bool ParseCharBGServerMileageChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ��Ʋ �׶��� ������ ȯ���� ���
	bool ParseCharBGServerCharSlot(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��Ʋ �׶��� ���� ���� ĳ���� ���� ��û
};


#endif