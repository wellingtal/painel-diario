#ifndef _DBAGENT_PACKET_PARSE_H_
#define _DBAGENT_PACKET_PARSE_H_

// ���� ����
struct PktBase;
class CDBAgentDispatch;
class CGameClientDispatch;

namespace DBAgentPacketParse
{
	bool SendUserKill(CDBAgentDispatch& DBAgentDispatch, unsigned long dwUserID, unsigned short usError);
	bool SendServerLogout(CDBAgentDispatch& DBAgentDispatch);
    bool SendAbnormalLogout(unsigned long dwUID, unsigned long dwCID, 
        unsigned long dwSessionID, unsigned long dwRequestKey, 
        CGameClientDispatch* lpGameClientDispatch);

	bool SendRankDelete(const char* szName,	unsigned short usError);							// �̸� ����� ��ŷ������ ���� ��� ����.
					

	bool ParseUserKill(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);

    // �α��ν� DB�κ��� ������� ������Ʈ��. (DepositUpdate�� ������ ����.)
    bool ParseUpdateDBData(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);          	// DB������ ������Ʈ
    bool ParseFriendList(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);            	// ģ�� ����Ʈ ������Ʈ
	bool ParseQuestDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ����Ʈ ������Ʈ
	bool ParseDepositCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);            	// DB������ ������Ʈ

	bool ParseConfigInfoDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� ������Ʈ

    bool ParseAgentParty(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��Ƽ ���
    bool ParseDepositUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);         	// â�� ������Ʈ
    bool ParseAgentZone(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);

   	bool ParseSysServerLogin(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� �α���
	bool ParseSysServerVerUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// ���� ���� ������Ʈ
	bool ParseSysChannelUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� ���� ���� ������Ʈ    
	bool ParseSysRankingUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� ��ŷ ���� ������Ʈ

    bool ParseBillingTimeoutNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);  	// ���� Ÿ�Ӿƿ� ����
	bool ParseBillingTimeCheckNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);	// ���� üũ

	bool ParseCharUseCashItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ĳ�������� ���
	bool ParseExtraEvent(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ����Ʈ�� �̺�Ʈ �ߵ�


	bool ParseEventDropItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ������ ������ �̺�Ʈ
	bool ParseCharAdminCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� ��� ó��
    bool ParseGiveItemToTempInven(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);       // �ӽ��κ����� ������ ����
    bool ParseCharNameChangeAck(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);         // �̸� ���� ��û������ Ack

	bool ParseSaveEnemy(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ���� ���� ���� ����.

	bool ParseKeyInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ���̽�ƽ Ű ���� ����..
};

#endif