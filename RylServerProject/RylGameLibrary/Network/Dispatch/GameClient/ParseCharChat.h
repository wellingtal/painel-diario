#ifndef _PARSE_CHAR_CHAT_PACKET_H_
#define _PARSE_CHAR_CHAT_PACKET_H_

// 전방 참조
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
    bool ParseCharWhisper(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase); // 속삭임
    bool ParseCharChat(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 캐틱터 채팅
};


#endif