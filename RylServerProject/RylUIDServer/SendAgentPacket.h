#ifndef _SEND_AGENT_PACKET_H_
#define _SEND_AGENT_PACKET_H_

#include <winsock2.h>
#include <windows.h>

namespace AgentSendPacket
{
    // 특정 서버군에만 보낸다.
    bool SendUserKill(char Group, unsigned long UserID);
	bool SendUpdateUIDTable(char AgentGroup, char Cmd, unsigned long UserID, unsigned long Flag, 
        int PlayTime, unsigned long dwCRMIndex1, char BillingType, 
        unsigned short usState, unsigned short usErrorCode);

    // 빌링 타임아웃 경고를 보낸다.
    bool SendHanBTN(char cAgentGroup, unsigned long dwUID, 
        unsigned char cRemainMinute, unsigned char cBillingType);

    // 모든 서버군에 보낸다.
    bool SendHanUnitedBTN(const char* szID, const char* szIP, const char* szMsg);
    bool SendHanUnitedUserKill(const char* szID, const char* szIP);
    bool SendHanUnitedUserKill(unsigned long dwAgentGroup, const char* szID, const char* szIP);
};

#endif