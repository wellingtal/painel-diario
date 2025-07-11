#ifndef _CAUTH_SERVER_TABLE_H_
#define _CAUTH_SERVER_TABLE_H_

#include <vector>
#include <algorithm>
#include <Thread/Lock.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

// 전방 참조
class CSession;
class CLoginAgentDispatch;

class CLoginAgentServerTable
{
public:
    
    enum OperateMode
    {
        SERVER_CLOSED       = 0,
        SERVER_PATCH_ONLY   = 1,
        SERVER_OPEN         = 2    
    };

    enum IPMode
    {
        ALLOW_SOME                  = 0,
        ALLOW_ALL                   = 1,
        DENY_ALL_BUT_ALLOW_ADMIN    = 2,
        MAX_IP_MODE                 = 3
    };

    // 첫번째 키가 IP, 두번째 키가 서버군. (-1이면 모든 서버군)
    typedef std::pair<unsigned long, int> AllowedIP;
    typedef std::vector<AllowedIP> IPList;


	static CLoginAgentServerTable& GetInstance();
    static const char* GetAllowedIPListFileName();

    bool Initialize(void);
    bool LoadAllowedIPList(const char* szFileName = GetAllowedIPListFileName());
    
    OperateMode GetOperateMode() { return m_eOperateMode; }
    void SetOperateMode(OperateMode eOperateMode) { m_eOperateMode = eOperateMode; }

    IPMode GetIPMode(unsigned long dwServerGroup)
    {
        if(dwServerGroup < SERVER_ID::MAX_GROUP_NUM) 
        {
            return m_ipMode[dwServerGroup];
        }
        
        return ALLOW_ALL; 
    }

    void SetIPMode(unsigned long dwServerGroup, IPMode eIPMode) { if(dwServerGroup < SERVER_ID::MAX_GROUP_NUM) { m_ipMode[dwServerGroup] = eIPMode; } }
    void SetIPMode(IPMode eIPMode) { std::fill_n(m_ipMode, int(SERVER_ID::MAX_GROUP_NUM), eIPMode); }

	bool AddConnection(unsigned long dwServerID, IN_ADDR ServerAddress, CLoginAgentDispatch* lpAgentDispatch);
	void RemoveConnection(unsigned long dwServerID, CLoginAgentDispatch* lpAgentDispatch);

	bool GetServerList(SERVER_LIST &ServerList, in_addr peerAddr);
	bool GetServerName(SERVER_ID &ServerID, char* szOutBuffer, size_t* nBufferSize_InOut);

	bool UpdateServerData(unsigned long dwServerID, IN_ADDR AuthAddr, unsigned long dwClientVer, const char* szPatchAddress);		
    bool UpdateChannelUserNum(unsigned long dwServerID, unsigned short* lpdwClientChannelNum, unsigned short usChannelNum);

    void GetAgentServerInfo(RylServerInfo* lpAgentServerInfo_Out, unsigned long* nAgentNum_InOut);

private:

	CLoginAgentServerTable();
	~CLoginAgentServerTable();

    RylServerInfo           m_AgentServerInfo[SERVER_ID::MAX_GROUP_NUM];
    CLoginAgentDispatch*    m_lpLoginAgentDispatch[SERVER_ID::MAX_GROUP_NUM];

    IPMode                  m_ipMode[SERVER_ID::MAX_GROUP_NUM];

    unsigned long           m_nAgentNum;
    OperateMode             m_eOperateMode;

    
    IPList                  m_ipList;
};

#endif