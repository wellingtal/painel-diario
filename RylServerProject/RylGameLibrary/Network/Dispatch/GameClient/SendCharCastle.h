#ifndef _SEND_CHAR_CASTLE_H_
#define _SEND_CHAR_CASTLE_H_

#include <Item/ItemStructure.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>

// ���� ����
class CSendStream;

namespace GameClientSendPacket
{
	// �� ���� ���� (�� ���� ������Ʈ ���� : ��ε�ĳ��Ʈ�� ��������.)
	bool SendCharCastleInfo(CSendStream& SendStream);

	// ���� �ð� ���� ����
	bool SendCharGameTimeInfo(CSendStream& SendStream);

	// ���ڽ� ��밡�� ���� ���� ����
    bool SendStealthInfo(CSendStream& SendStream, bool bUseStealth);
	
    // ��� ��� ���� ����
	bool SendCharCampInfo(CSendStream& SendStream);

	
	
	// �� ���� ��� ��Ŷ ����
	bool SendCharCastleCmd(CSendStream& SendStream, unsigned long dwCastleID, unsigned long dwCastleObjID,
							unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// ��� ��� ���� ��� ��Ŷ ����
	bool SendCharCampCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwCampID,
						 unsigned char cSubCmd, unsigned short wError);

	// ���� ���� ���� ��� ��Ŷ ����
	bool SendCharSiegeArmsCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwArmsID,
							  unsigned char cSubCmd, unsigned short wError);


	
	// �� ���� ���� ��Ŷ ����
	bool SendCharCastleRight(CSendStream& SendStream, unsigned long dwCasltID,
							 const CastleRight& castleRight, unsigned short wError);

	// ��� ��� ���� ���� ��Ŷ ����
	bool SendCharCampRight(CSendStream& SendStream, unsigned long dwCampID,
						   const CampRight& campRight, unsigned short wError);
    

		
	
	
	// ��� ��� ���� ��Ŷ ����		( GameServer  -->  DBAgentServer )
	bool SendCharCreateCamp(CSendStream& AgentSendStream, unsigned long dwActorCID, unsigned long dwGID,
							unsigned short wObjectType, const Position& pos);

	// ���� ���� ���� ��Ŷ ����	( GameServer  -->  DBAgentServer )
	bool SendCharCreateSiegeArms(CSendStream& AgentSendStream, unsigned long dwOwnerID, unsigned char cNation,
								 unsigned short wType, unsigned char cUpgradeStep, const Position& pos);

    	
	
	// �� ���� ��� ��Ŷ�� DBAgent ������ ������		( GameServer  -->  DBAgentServer )
	bool SendCharCastleCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCastleID, unsigned long dwCastleObjectID,
									unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// ��� ��� ���� ��� ��Ŷ�� DBAgent ������ ������		( GameServer  -->  DBAgentServer )
	bool SendCharCampCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCampID,
								  unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// ���� ���� ���� ��� ��Ŷ DBAgent ������ ������	( GameServer  -->  DBAgentServer )
	bool SendCharSiegeArmsCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwArmsID,
										unsigned long dwValue, unsigned char cSubCmd, unsigned short wError);

	// ��� ��� �޼��� ��Ŷ ����		( GameServer  -->  DBAgentServer )
	bool SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, unsigned short wError);
	bool SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, 
									  const char* szName, int nNum, int nTotal, unsigned short wError);

	// ���� �÷��� ���� ��Ŷ ����
	bool SendWarOnOff(CSendStream& SendStream, unsigned long dwCID, unsigned char cType, unsigned char cFlag, unsigned short wError);

	// ���� ���� �������� ����Ʈ ����.
	bool SendRealmPoint(CSendStream& SendStream, unsigned long dwCID, unsigned char cRealmPoint, unsigned char cType, unsigned short wError);
};

#endif