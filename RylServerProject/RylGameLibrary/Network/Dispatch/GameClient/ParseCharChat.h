#ifndef _PARSE_CHAR_CHAT_PACKET_H_
#define _PARSE_CHAR_CHAT_PACKET_H_

// ���� ����
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
    bool ParseCharWhisper(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase); // �ӻ���
    bool ParseCharChat(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ĳƽ�� ä��
};


#endif