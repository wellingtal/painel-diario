#ifndef _PARSE_CHAR_ETC_H_
#define _PARSE_CHAR_ETC_H_


// 전방 참조
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharSuicide(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// 캐릭터 자살	
    bool ParseCharBindPosition(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 캐릭터 바인드 포지션

    bool ParseCharQuickSlotMove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 캐릭터 퀵 슬롯 이동
    bool ParseCharControlOption(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 캐릭터 옵션 조정
	bool ParseCharAuthorizePanel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 인증 판넬에 대한 승인

	bool ParseCharFameInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 캐릭터 명성(공헌도) 정보 요청
	bool ParseCharNameCheck(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// hz added for new packet
	bool ParseGetCharInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
	bool ParseCharRankingInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 랭킹 정보

    bool ParseCastObjectInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);          // 오브젝트 던지기 패킷 정보
};


#endif