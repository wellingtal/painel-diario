#ifndef _GUILD_PACKET_PARSE_H_
#define _GUILD_PACKET_PARSE_H_

// 전방 참조
struct PktBase;
class CDBAgentDispatch;

namespace DBAgentPacketParse
{
	// 길드 업데이트
	bool ParseCreateGuild(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 길드 생성
	bool ParseGuildCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 멤버 관련 명령
	bool ParseGuildMark(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 마크 변경
	bool ParseGuildLevel(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 길드 레벨 변경
	bool ParseGuildRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 길드 권한 변경
	bool ParseGuildRelation(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 길드 관계 변경
	bool ParseGuildInclination(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 길드 성향 변경
	bool ParseGuildDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 데이터 업데이트
	bool ParseGuildSafe(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 금고
	bool ParseGuildMemberInfoUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// 길드 멤버 정보 업데이트
	bool ParseGuildMemberFameUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// 길드 멤버 명성 업데이트
	bool ParseGuildMemberGoldUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// 길드 멤버 돈 업데이트
};

#endif