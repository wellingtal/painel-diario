#ifndef _SEND_CHAR_COMMUNITY_H_
#define _SEND_CHAR_COMMUNITY_H_


// ���� ����
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
    // ĳ���� ��ȯ ���� ������
    bool SendCharExchangeCmd(CSendStream& SendStream, unsigned long dwSenderID_In, 
        unsigned long dwRecverID_In, unsigned char cCmd_In, unsigned short usError_In);

	// ---------------------------------------------------------------------------------------------------------------------

	// ĳ���� ��Ƽ Ŀ�ǵ� ������.
    bool SendCharPartyCmd(CSendStream& SendStream, unsigned long dwPartyID_In, 
        const AddressInfo &AddressInfo_In, const char* szSenderName_In, unsigned long dwSenderID_In, 
        unsigned long dwReceiverID_In, unsigned short usCmd_In, unsigned short usError_In);

    // ĳ���� ��Ƽ ������ DBAgent�� ������
	bool SendCharPartyCreateToDBAgent(CSendStream& AgentSendStream, unsigned long dwLeaderID, unsigned long dwMemberID, unsigned char cZone,
			unsigned short wLeaderClass, char cLeaderLevel, unsigned long dwLeaderGID,
			unsigned short wMemberClass, char cMemberLevel, unsigned long dwMemberGID);

    bool SendCharPartyCreateToDBAgent(CSendStream& AgentSendStream, unsigned long dwLeaderID, unsigned long dwMemberID,
			unsigned short wLeaderClass, char cLeaderLevel, unsigned short wMemberClass, char cMemberLevel);

    // ĳ���� ��Ƽ �ı��� DBAgent�� ������
    bool SendCharPartyDestroyToDBAgent(CSendStream& AgentSendStream, unsigned long dwPartyID);

    // ��Ƽ ��� ������ ������
    bool SendPartyMemberData(CSendStream& SendStream, unsigned long dwPartyID, unsigned long dwSenderID, unsigned long dwGID,
			unsigned short wClass, unsigned long dwServerID, char cLevel, const char* strSenderName, unsigned short usCmd);

	// ---------------------------------------------------------------------------------------------------------------------

	// ������ ������ ���
	bool SendCharStallRegisterItem(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwShopID, 
        TakeType takeType, unsigned long dwPrice, unsigned char cCmd, unsigned short usError);

	// ������ ����
	bool SendCharStallEnter(CSendStream& SendStream, unsigned long dwCustomerID, unsigned long dwOwnerID, 
		unsigned short usError);

	// ������ ������ ����
	bool SendCharStallItemInfo(CSendStream& SendStream, unsigned long dwOwnerID, unsigned long* arylCustomerID, 
		unsigned long dwTempSafe, unsigned char cTax, char* szBuffer, unsigned long dwItemSize, unsigned char cItemNum);


	// ---------------------------------------------------------------------------------------------------------------------

	// ģ�� ���/���� ack, �ź� ���/���� Ack, ģ�� �α���, ģ�� �α׾ƿ�
    bool SendCharFriendAck(CSendStream& SendStream, unsigned char cAckCmd, unsigned long dwCID, const char* szFriendName, 
					unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, unsigned short usError);

    // ģ�� ��ϴ���
    bool SendCharFriendAdded(CSendStream& SendStream, unsigned long dwCID, const char* szName);

    // DBAgent�� ģ�� ����Ʈ ��������� ���� (ģ�� ���/����, �ź� ���/����, �׷� ����)
    bool SendFriendListChangeToDB(CSendStream& AgentSendStream, unsigned long dwUID, unsigned long dwCID, unsigned long dwReferenceUID, 
									unsigned long dwReferenceCID, unsigned long dwData, unsigned long dwGID, unsigned short wClass, 
									char cLevel, unsigned long dwServerID, unsigned char cChangeType);
    
	// ---------------------------------------------------------------------------------------------------------------------

	// ��� ����
	bool SendCharCreateGuild(CSendStream& SendStream, unsigned long dwMasterID, unsigned long dwGuildID, 
		unsigned char cInclination, char* szGuildName, unsigned short wError);

	// ��� ��� ���� ���
    bool SendCharGuildCmd(CSendStream& SendStream, unsigned long dwGID, unsigned long dwSenderID, unsigned long dwReceiverID, 
		const char* szGuildName, const char* szSenderName, unsigned short wCmd, unsigned short wError);

	// ��� ��ũ ����
	bool SendCharGuildMark(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
		char* szMark, unsigned long dwGold, unsigned short wError);

	// ��� ���� ����
	bool SendCharGuildLevel(CSendStream& SendStream, unsigned long dwUID, unsigned char cLevel, unsigned long dwGold, unsigned short wError);

	// ��� ���� ����
	bool SendCharGuildRelation(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID,
		unsigned long dwTargetGID, unsigned long dwValue, unsigned char cSubCmd, unsigned short wError,
		const char* pszGuildName=NULL, const char* pszTempGuildName=NULL);

	// ��� ���� ����
	bool SendCharGuildInclination(CSendStream& SendStream, unsigned long dwUID, unsigned char cInclination, unsigned short wError);

	// ��� ���� ����
	bool SendCharGuildRight(CSendStream& SendStream, unsigned long dwGID, GuildRight guildRight, unsigned short wError);

	// �ڱ� ��� ����
	bool SendCharMyGuildInfo(CSendStream& SendStream, unsigned long dwGold, GuildRight guildRight, unsigned char cTitle, 
		unsigned short wError);

	// ��� �ݰ�
	bool SendCharGuildSafe(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGID, 
		unsigned long dwSafeGold, unsigned long dwCharGold, unsigned char cCmd, const char* szCharName, unsigned short wError);

	// ��� ��� ���� ������Ʈ
	bool SendCharUpdateGuildMemberInfo(CSendStream& SendStream, unsigned long dwGID, unsigned long dwCID, Guild::MemberInfo memberInfo, 
		unsigned short wError);

	// ---------------------------------------------------------------------------------------------------------------------

	// ä�� ����
	bool SendCharChatBan(CSendStream& SendStream, unsigned long dwAdminCID, unsigned long dwTargetCID, unsigned long dwMinutes);
}

#endif