#ifndef _SEND_CHAR_CASTLE_H_
#define _SEND_CHAR_CASTLE_H_

#include <Item/ItemStructure.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>

// 전방 참조
class CSendStream;

namespace GameClientSendPacket
{
	// 성 정보 전송 (성 관련 오브젝트 제외 : 브로드캐스트로 보내진다.)
	bool SendCharCastleInfo(CSendStream& SendStream);

	// 게임 시간 정보 전송
	bool SendCharGameTimeInfo(CSendStream& SendStream);

	// 스텔스 사용가능 여부 정보 전송
    bool SendStealthInfo(CSendStream& SendStream, bool bUseStealth);
	
    // 길드 요새 정보 전송
	bool SendCharCampInfo(CSendStream& SendStream);

	
	
	// 성 관련 명령 패킷 전송
	bool SendCharCastleCmd(CSendStream& SendStream, unsigned long dwCastleID, unsigned long dwCastleObjID,
							unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// 길드 요새 관련 명령 패킷 전송
	bool SendCharCampCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwCampID,
						 unsigned char cSubCmd, unsigned short wError);

	// 공성 병기 관련 명령 패킷 전송
	bool SendCharSiegeArmsCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwArmsID,
							  unsigned char cSubCmd, unsigned short wError);


	
	// 성 관리 권한 패킷 전송
	bool SendCharCastleRight(CSendStream& SendStream, unsigned long dwCasltID,
							 const CastleRight& castleRight, unsigned short wError);

	// 길드 요새 관리 권한 패킷 전송
	bool SendCharCampRight(CSendStream& SendStream, unsigned long dwCampID,
						   const CampRight& campRight, unsigned short wError);
    

		
	
	
	// 길드 요새 구축 패킷 전송		( GameServer  -->  DBAgentServer )
	bool SendCharCreateCamp(CSendStream& AgentSendStream, unsigned long dwActorCID, unsigned long dwGID,
							unsigned short wObjectType, const Position& pos);

	// 공성 병기 생성 패킷 전송	( GameServer  -->  DBAgentServer )
	bool SendCharCreateSiegeArms(CSendStream& AgentSendStream, unsigned long dwOwnerID, unsigned char cNation,
								 unsigned short wType, unsigned char cUpgradeStep, const Position& pos);

    	
	
	// 성 관련 명령 패킷을 DBAgent 서버로 보내기		( GameServer  -->  DBAgentServer )
	bool SendCharCastleCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCastleID, unsigned long dwCastleObjectID,
									unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// 길드 요새 관련 명령 패킷을 DBAgent 서버로 보내기		( GameServer  -->  DBAgentServer )
	bool SendCharCampCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCampID,
								  unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError);

	// 공성 병기 관련 명령 패킷 DBAgent 서버로 보내기	( GameServer  -->  DBAgentServer )
	bool SendCharSiegeArmsCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwArmsID,
										unsigned long dwValue, unsigned char cSubCmd, unsigned short wError);

	// 길드 요새 메세지 패킷 전송		( GameServer  -->  DBAgentServer )
	bool SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, unsigned short wError);
	bool SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, 
									  const char* szName, int nNum, int nTotal, unsigned short wError);

	// 전쟁 플래그 변경 패킷 전송
	bool SendWarOnOff(CSendStream& SendStream, unsigned long dwCID, unsigned char cType, unsigned char cFlag, unsigned short wError);

	// 국가 전쟁 공헌훈장 포인트 전송.
	bool SendRealmPoint(CSendStream& SendStream, unsigned long dwCID, unsigned char cRealmPoint, unsigned char cType, unsigned short wError);
};

#endif