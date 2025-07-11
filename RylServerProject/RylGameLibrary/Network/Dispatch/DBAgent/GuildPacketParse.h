#ifndef _GUILD_PACKET_PARSE_H_
#define _GUILD_PACKET_PARSE_H_

// ���� ����
struct PktBase;
class CDBAgentDispatch;

namespace DBAgentPacketParse
{
	// ��� ������Ʈ
	bool ParseCreateGuild(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��� ����
	bool ParseGuildCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ��� ���� ���
	bool ParseGuildMark(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ��ũ ����
	bool ParseGuildLevel(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��� ���� ����
	bool ParseGuildRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��� ���� ����
	bool ParseGuildRelation(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��� ���� ����
	bool ParseGuildInclination(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ��� ���� ����
	bool ParseGuildDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ������ ������Ʈ
	bool ParseGuildSafe(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� �ݰ�
	bool ParseGuildMemberInfoUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// ��� ��� ���� ������Ʈ
	bool ParseGuildMemberFameUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// ��� ��� �� ������Ʈ
	bool ParseGuildMemberGoldUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// ��� ��� �� ������Ʈ
};

#endif