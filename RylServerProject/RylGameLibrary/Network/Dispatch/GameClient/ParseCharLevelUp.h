#ifndef _PARSE_CHAR_LEVELUP_H_
#define _PARSE_CHAR_LEVELUP_H_


// ���� ����
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharClassUpgrade(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// Ŭ���� ���׷��̵�
    bool ParseCharIncreasePoint(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// IP ���� ��Ŷ
	bool ParseCharStateRedistribution(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ���� ��й�(Ŭ���� ó�� ���·�)

	bool ParseCharStatusRetrain(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// �������ͽ� ���Ʒ�
};


#endif