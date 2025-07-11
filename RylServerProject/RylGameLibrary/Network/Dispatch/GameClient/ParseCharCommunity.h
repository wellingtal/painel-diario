#ifndef _PARSE_CHAR_COMMUNITY_PACKET_H_
#define _PARSE_CHAR_COMMUNITY_PACKET_H_

// ���� ����
class   CGameClientDispatch;
struct  PktBase;

namespace GameClientParsePacket
{
    // ----------------------------------------------------------------------------------------------
    // ĳ���� Ŀ�´�Ƽ ���� ��Ŷ - ParseCharCommunity.cpp -------------------------------------------

    bool ParseCharExchangeCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��ȯ ���	
    bool ParseCharPartyCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��Ƽ ���
    bool ParseCharPartyFind(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��Ƽ ã��

    bool ParseCharStallOpen(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ����
    bool ParseCharStallRegisterItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ������ ������ ���
    bool ParseCharStallEnter(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ����

    bool ParseCharFriendAdd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);           // ģ��(�ź�)����Ʈ �߰�
    bool ParseCharFriendRemove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);        // ģ��(�ź�)����Ʈ ����
    bool ParseCharFriendEtc(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);           // ��Ÿ ó��(�׷� ���� ��)

	bool ParseCharCreateGuild(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ����
	bool ParseCharGuildCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ��� ���� ���
	bool ParseCharGuildMark(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ��ũ ����
	bool ParseCharGuildLevel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ���� ����
	bool ParseCharGuildRelation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��� ���� ����
	bool ParseCharGuildInclination(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ��� ���� ����
	bool ParseCharGuildList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ����Ʈ
	bool ParseCharGuildRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� ���� ����
	bool ParseCharGuildMemberList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��� ��� ����Ʈ
	bool ParseCharGuildSafe(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��� �ݰ�
	bool ParseCharGuildHostilityList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ���� ��� ��Ȳ ����Ʈ
	bool ParseCharGuildRelationInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ����� ���� ����Ʈ ��û
};


#endif