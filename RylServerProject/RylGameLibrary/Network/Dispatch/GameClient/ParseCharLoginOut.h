#ifndef _PARSE_CHAR_LOGIN_OUT_H_
#define _PARSE_CHAR_LOGIN_OUT_H_

struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    // 일반적인 게임서버의 로그인 과정.
    bool ParseCharLogin(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);       // 캐릭터 로그인 - DBAgent로 캐릭터 정보 요청
    bool ParseCharLogout(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);      // 캐릭터 로그아웃
    bool ParseCharMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);    // 캐릭터 존 이동
    bool ParseServerZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 서버 이동
	bool ParseCSAuth(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 인증 코드 (게임 가드)

	bool ParseKeyInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 조이스틱 키 정보 관련.

    // 던전 서버 로그인 과정은? --;;
};

#endif