#ifndef _PARSE_CHAR_ATTACK_H_
#define _PARSE_CHAR_ATTACK_H_

// 전방 참조
struct  PktBase;

class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharAttack(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// 공격 패킷
    bool ParseCharSwitchEQ(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 장비바꾸기
    bool ParseCharRespawn(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// 캐릭터 리스폰
    bool ParseCharRespawnWaitQueue(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 리스폰 큐 대기인 정보 (배틀그라운드용)
	bool ParseCharRespawnInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 리스폰 정보 (리스폰 위치들)
	bool ParseCharRespawnAreaInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 리스폰 지역 세부 정보
    bool ParseCharDuelCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// 듀얼 패킷 명령	
	bool ParseCharPeaceMode(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 반전 모드
	bool ParseCharSummonCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 소환수 명령

    // Tcp로 보는 캐릭터 이동.
    // bool ParseCharMoveUpdate(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 캐릭터 움직임 업데이트
    bool ParseCharMoveEx(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);              // TCP 캐릭터 이동
};


#endif