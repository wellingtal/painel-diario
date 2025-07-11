#ifndef _PARSE_CHAR_CASTLE_PACKET_H_
#define _PARSE_CHAR_CASTLE_PACKET_H_

// 전방 참조
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
	// 성 관련 명령 패킷 처리
	bool ParseCharCastleCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// 길드 요새 관련 명령 패킷 처리
	bool ParseCharCampCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// 공성 병기 관련 명령 패킷 처리
	bool ParseCharSiegeArmsCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);


	// 성 관리 권한 패킷 처리
	bool ParseCharCastleRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// 길드 요새 관리 권한 패킷 처리
	bool ParseCharCampRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);


	// 성 상징물에 보석 아이템 넣기/빼기
	bool ParseCharTakeCastleJewel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
	
	// 길드 요새 자재 넣기/빼기
	bool ParseCharTakeMaterial(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);

	// 길드, 국가 전쟁 On, Off 정보.
	bool ParseWarOnOff(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
};


#endif