#ifndef _PARSE_CHAR_COMMUNITY_PACKET_H_
#define _PARSE_CHAR_COMMUNITY_PACKET_H_

// 전방 참조
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
    // ----------------------------------------------------------------------------------------------
    // 캐릭터 커뮤니티 관련 패킷 - ParseCharCommunity.cpp -------------------------------------------

    bool ParseCharExchangeCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 교환 명령	
    bool ParseCharPartyCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 파티 명령
    bool ParseCharPartyFind(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 파티 찾기

    bool ParseCharStallOpen(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 노점상 개설
    bool ParseCharStallRegisterItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 노점상 아이템 등록
    bool ParseCharStallEnter(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 노점상 입장

    bool ParseCharFriendAdd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);           // 친구(거부)리스트 추가
    bool ParseCharFriendRemove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);        // 친구(거부)리스트 삭제
    bool ParseCharFriendEtc(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);           // 기타 처리(그룹 설정 등)

	bool ParseCharCreateGuild(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 생성
	bool ParseCharGuildCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 멤버 관련 명령
	bool ParseCharGuildMark(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 마크 변경
	bool ParseCharGuildLevel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 레벨 변경
	bool ParseCharGuildRelation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 길드 관계 변경
	bool ParseCharGuildInclination(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 길드 성향 변경
	bool ParseCharGuildList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 리스트
	bool ParseCharGuildRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 권한 설정
	bool ParseCharGuildMemberList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 길드 멤버 리스트
	bool ParseCharGuildSafe(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 길드 금고
	bool ParseCharGuildHostilityList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 적대 길드 형황 리스트
	bool ParseCharGuildRelationInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 길드의 관계 리스트 요청
};


#endif