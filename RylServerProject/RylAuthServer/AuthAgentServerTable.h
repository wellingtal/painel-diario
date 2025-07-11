#ifndef _AUTH_AGENT_SERVER_TABLE_H_
#define _AUTH_AGENT_SERVER_TABLE_H_

#include <map>
#include <Thread/Lock.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "AuthAgentDispatch.h"

// 전방 참조
class CSession;
class CAuthAgentDispatch;
class CAuthClientDispatch;

// MultiThread 가능.
class CAuthAgentServerTable
{
public:
   
	static CAuthAgentServerTable& GetInstance();

    void Destroy();

    // Get Data
    void GetPatchServerAddress(unsigned long& dwClientVer, 
		char* szPatchAddress_Out, size_t nBufferLen);

	void SetAgentServerInfo(RylServerInfo& ServerInfo);
	void GetAgentServerInfo(RylServerInfo& ServerInfo);
    unsigned long GetServerVersion();
    unsigned long GetServerChecksum();    

    // Update Data
    void UpdateServerData(unsigned long dwChecksum, 
		unsigned long dwClientVer, const char* szPatchAddress);

    void UpdateChannelUserNum(unsigned short* lpdwClientChannelNum, 
		unsigned short usChannelNum);

private:

    CAuthAgentServerTable();
	~CAuthAgentServerTable();

    typedef CCSLock         AuthAgentLock;
    AuthAgentLock           m_AuthAgentLock;
    CACHE_PAD(PaddingAgent, sizeof(AuthAgentLock));
    RylServerInfo           m_AgentServerInfo;
};

#endif