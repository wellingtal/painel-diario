#ifndef _SEND_CHAR_COMMUNITY_H_
#define _SEND_CHAR_COMMUNITY_H_


// 전방 참조
class CCharacter;
class CSendStream;

struct PktWh;
struct AddressInfo;
struct TakeType;
struct GuildRight;

namespace Guild
{
	struct MemberInfo;
};

namespace GameClientSendPacket
{
    // 캐릭터 교환 정보 보내기
    bool SendCharExchangeCmd(CSendStream& SendStream, unsigned long dwSenderID_In, 
        unsigned long dwRecverID_In, unsigned char cCmd_In, unsigned short usError_In);

	// ---------------------------------------------------------------------------------------------------------------------

	// 캐릭터 파티 커맨드 보내기.
    bool SendCharPartyCmd(CSendStream& SendStream, unsigned long dwPartyID_In, 
        const AddressInfo &AddressInfo_In, const char* szSenderName_In, unsigned long dwSenderID_In, 
        unsigned long dwReceiverID_In, unsigned short usCmd_In, unsigned short usError_In);

    // 캐릭터 파티 생성을 DBAgent로 보내기
	bool SendCharPartyCreateToDBAgent(CSendStream& AgentSendStream, unsigned long dwLeaderID, unsigned long dwMemberID, unsigned char cZone,
			unsigned short wLeaderClass, char cLeaderLevel, unsigned long dwLeaderGID,
			unsigned short wMemberClass, char cMemberLevel, unsigned long dwMemberGID);

    bool SendCharPartyCreateToDBAgent(CSendStream& AgentSendStream, unsigned long dwLeaderID, unsigned long dwMemberID,
			unsigned short wLeaderClass, char cLeaderLevel, unsigned short wMemberClass, char cMemberLevel);

    // 캐릭터 파티 파괴를 DBAgent로 보내기
    bool SendCharPartyDestroyToDBAgent(CSendStream& AgentSendStream, unsigned long dwPartyID);

    // 파티 멤버 데이터 보내기
    bool SendPartyMemberData(CSendStream& SendStream, unsigned long dwPartyID, unsigned long dwSenderID, unsigned long dwGID,
			unsigned short wClass, unsigned long dwServerID, char cLevel, const char* strSenderName, unsigned short usCmd);

	// ---------------------------------------------------------------------------------------------------------------------

	// 노점상 아이템 등록
	bool SendCharStallRegisterItem(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwShopID, 
        TakeType takeType, unsigned long dwPrice, unsigned char cCmd, unsigned short usError);

	// 노점상 입장
	bool SendCharStallEnter(CSendStream& SendStream, unsigned long dwCustomerID, unsigned long dwOwnerID, 
		unsigned short usError);

	// 노점상 아이템 정보
	bool SendCharStallItemInfo(CSendStream& SendStream, unsigned long dwOwnerID, unsigned long* arylCustomerID, 
		unsigned long dwTempSafe, unsigned char cTax, char* szBuffer, unsigned long dwItemSize, unsigned char cItemNum);


	// ---------------------------------------------------------------------------------------------------------------------

	// 친구 등록/삭제 ack, 거부 등록/삭제 Ack, 친구 로그인, 친구 로그아웃
    bool SendCharFriendAck(CSendStream& SendStream, unsigned char cAckCmd, unsigned long dwCID, const char* szFriendName, 
					unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, unsigned short usError);

    // 친구 등록당함
    bool SendCharFriendAdded(CSendStream& SendStream, unsigned long dwCID, const char* szName);

    // DBAgent에 친구 리스트 변경사항을 보냄 (친구 등록/삭제, 거부 등록/삭제, 그룹 세팅)
    bool SendFriendListChangeToDB(CSendStream& AgentSendStream, unsigned long dwUID, unsigned long dwCID, unsigned long dwReferenceUID, 
									unsigned long dwReferenceCID, unsigned long dwData, unsigned long dwGID, unsigned short wClass, 
									char cLevel, unsigned long dwServerID, unsigned char cChangeType);
    
	// ---------------------------------------------------------------------------------------------------------------------

	// 길드 생성
	bool SendCharCreateGuild(CSendStream& SendStream, unsigned long dwMasterID, unsigned long dwGuildID, 
		unsigned char cInclination, char* szGuildName, unsigned short wError);

	// 길드 멤버 관련 명령
    bool SendCharGuildCmd(CSendStream& SendStream, unsigned long dwGID, unsigned long dwSenderID, unsigned long dwReceiverID, 
		const char* szGuildName, const char* szSenderName, unsigned short wCmd, unsigned short wError);

	// 길드 마크 변경
	bool SendCharGuildMark(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
		char* szMark, unsigned long dwGold, unsigned short wError);

	// 길드 레벨 변경
	bool SendCharGuildLevel(CSendStream& SendStream, unsigned long dwUID, unsigned char cLevel, unsigned long dwGold, unsigned short wError);

	// 길드 관계 변경
	bool SendCharGuildRelation(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID,
		unsigned long dwTargetGID, unsigned long dwValue, unsigned char cSubCmd, unsigned short wError,
		const char* pszGuildName=NULL, const char* pszTempGuildName=NULL);

	// 길드 성향 변경
	bool SendCharGuildInclination(CSendStream& SendStream, unsigned long dwUID, unsigned char cInclination, unsigned short wError);

	// 길드 권한 설정
	bool SendCharGuildRight(CSendStream& SendStream, unsigned long dwGID, GuildRight guildRight, unsigned short wError);

	// 자기 길드 정보
	bool SendCharMyGuildInfo(CSendStream& SendStream, unsigned long dwGold, GuildRight guildRight, unsigned char cTitle, 
		unsigned short wError);

	// 길드 금고
	bool SendCharGuildSafe(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
		unsigned long dwSafeGold, unsigned long dwCharGold, unsigned char cCmd, const char* szCharName, unsigned short wError);

	// 길드 멤버 정보 업데이트
	bool SendCharUpdateGuildMemberInfo(CSendStream& SendStream, unsigned long dwGID, unsigned long dwCID, Guild::MemberInfo memberInfo, 
		unsigned short wError);

	// ---------------------------------------------------------------------------------------------------------------------

	// 채팅 금지
	bool SendCharChatBan(CSendStream& SendStream, unsigned long dwAdminCID, unsigned long dwTargetCID, unsigned long dwMinutes);
}

#endif