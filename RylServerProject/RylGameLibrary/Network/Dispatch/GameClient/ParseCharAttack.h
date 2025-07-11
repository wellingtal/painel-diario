#ifndef _PARSE_CHAR_ATTACK_H_
#define _PARSE_CHAR_ATTACK_H_

// ���� ����
struct  PktBase;

class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharAttack(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// ���� ��Ŷ
    bool ParseCharSwitchEQ(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ���ٲٱ�
    bool ParseCharRespawn(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// ĳ���� ������
    bool ParseCharRespawnWaitQueue(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ������ ť ����� ���� (��Ʋ�׶����)
	bool ParseCharRespawnInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ���� (������ ��ġ��)
	bool ParseCharRespawnAreaInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ ���� ���� ����
    bool ParseCharDuelCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// ��� ��Ŷ ���	
	bool ParseCharPeaceMode(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ���� ���
	bool ParseCharSummonCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��ȯ�� ���

    // Tcp�� ���� ĳ���� �̵�.
    // bool ParseCharMoveUpdate(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ĳ���� ������ ������Ʈ
    bool ParseCharMoveEx(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);              // TCP ĳ���� �̵�
};


#endif