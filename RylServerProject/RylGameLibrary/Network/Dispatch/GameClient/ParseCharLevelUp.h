#ifndef _PARSE_CHAR_LEVELUP_H_
#define _PARSE_CHAR_LEVELUP_H_


// 전방 참조
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharClassUpgrade(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 클래스 업그레이드
    bool ParseCharIncreasePoint(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// IP 증가 패킷
	bool ParseCharStateRedistribution(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 스탯 재분배(클래스 처음 상태로)

	bool ParseCharStatusRetrain(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 스테이터스 재훈련
};


#endif