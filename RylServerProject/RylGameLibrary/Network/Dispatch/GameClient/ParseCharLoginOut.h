#ifndef _PARSE_CHAR_LOGIN_OUT_H_
#define _PARSE_CHAR_LOGIN_OUT_H_

struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    // �Ϲ����� ���Ӽ����� �α��� ����.
    bool ParseCharLogin(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);       // ĳ���� �α��� - DBAgent�� ĳ���� ���� ��û
    bool ParseCharLogout(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);      // ĳ���� �α׾ƿ�
    bool ParseCharMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);    // ĳ���� �� �̵�
    bool ParseServerZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ���� �̵�
	bool ParseCSAuth(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ���� �ڵ� (���� ����)

	bool ParseKeyInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ���̽�ƽ Ű ���� ����.

    // ���� ���� �α��� ������? --;;
};

#endif