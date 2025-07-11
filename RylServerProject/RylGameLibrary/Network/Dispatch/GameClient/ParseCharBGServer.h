#ifndef _PARSE_CHAR_BATTLEGROUND_SERVER_PACKET_H_
#define _PARSE_CHAR_BATTLEGROUND_SERVER_PACKET_H_

// 전방 참조
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
	bool ParseCharBGServerMapList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 배틀 그라운드 서버군 맵(방) 정보 리스트
	bool ParseCharBGServerResultList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 배틀 그라운드 서버군 맵(방) 결과 리스트
	bool ParseCharBGServerMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 배틀 그라운드 서버군 존 이동 (맵(방) 이동)
	bool ParseCharBGServerMileageChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 배틀 그라운드 서버군 환전소 명령
	bool ParseCharBGServerCharSlot(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 배틀 그라운드 서버 정섭 캐릭터 정보 요청
};


#endif