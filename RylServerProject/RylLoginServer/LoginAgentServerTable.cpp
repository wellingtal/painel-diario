#include "stdafx.h"

#include "LoginAgentDispatch.h"
#include "LoginAgentServerTable.h"

#include <Network/Session/Session.h>
#include <Utility/Setup/ServerSetup.h>
#include <Log/ServerLog.h>

const char* g_szGroupFileName	    = "./Script/Server/ServerGroup.txt";
const char* g_szIPFileName		    = "./Script/Server/ServerIP.txt";

struct FindByIP
{
    bool operator () (const CLoginAgentServerTable::AllowedIP& lhs, 
        const CLoginAgentServerTable::AllowedIP& rhs)
    {
        return lhs.first < rhs.first;
    }
};

CLoginAgentServerTable& CLoginAgentServerTable::GetInstance()
{
	static CLoginAgentServerTable LoginAgentServerTable;
	return LoginAgentServerTable;
}

const char* CLoginAgentServerTable::GetAllowedIPListFileName()
{
    return "./Script/Server/PreConnectIPList.txt";
}

CLoginAgentServerTable::CLoginAgentServerTable()
:   m_nAgentNum(0), m_eOperateMode(SERVER_CLOSED)
{
    std::fill_n(m_ipMode, int(SERVER_ID::MAX_GROUP_NUM), DENY_ALL_BUT_ALLOW_ADMIN);

}

CLoginAgentServerTable::~CLoginAgentServerTable()
{

}

bool CLoginAgentServerTable::Initialize()
{
	// 파일 열기
    const int MAX_BUFFERSIZE = 256;
	char szBuffer[MAX_BUFFERSIZE];

	FILE *lpFile = fopen(g_szGroupFileName, "rt");
	if (0 != lpFile)
    {
	    int nIndex = 0;

	    while(true)
	    {
		    if (0 == fgets(szBuffer, MAX_BUFFERSIZE, lpFile))
            {
                break;
            }

		    strncpy(m_AgentServerInfo[nIndex].m_szGroupName, strtok(szBuffer, "\n"), 
                RylServerInfo::GROUP_NAME_LENGTH - 1);
            m_AgentServerInfo[nIndex].m_szGroupName[RylServerInfo::GROUP_NAME_LENGTH - 1] = '\0';
            m_AgentServerInfo[nIndex].m_nGroupNameLen = strlen(m_AgentServerInfo[nIndex].m_szGroupName);
            ++nIndex;
	    }

	    fclose(lpFile);
    }
    else
    {
        ERRLOG1(g_Log, "Cannot load server group file  : %s", g_szGroupFileName);
    }
    
	if (CServerSetup::GetInstance().GetAlone())
	{
		lpFile = fopen(g_szIPFileName, "rt");
		if (0 == lpFile)
        {
            return false;
        }

		int Group = 0;

		while(true)
		{
			if (0 == fgets(szBuffer, MAX_BUFFERSIZE, lpFile))
			{
				break;
			}

			Group = atoi(strtok(szBuffer, "\n:"));

			m_AgentServerInfo[Group].m_ServerUID.sID.Group = Group;
            m_AgentServerInfo[Group].m_ServerUID.sID.Type = CServerSetup::AgentServer;

			m_AgentServerInfo[Group].m_ServerAddress.S_un.S_addr = inet_addr(strtok(0, "\n:"));
			
			m_AgentServerInfo[Group].m_usChannelClientNum[0] = 1;
			m_AgentServerInfo[Group].m_usChannelClientNum[1] = 2;
			m_AgentServerInfo[Group].m_usChannelNum = 2;

            ++m_nAgentNum;
		}

		fclose(lpFile);
	}

    LoadAllowedIPList();
    return true;
}

bool CLoginAgentServerTable::LoadAllowedIPList(const char* szFileName)
{
    m_ipList.clear();
    m_ipList.reserve(10000);

    FILE* lpFile = fopen(szFileName, "rt");
    if(0 != lpFile)
    {
        const int MAX_BUFFER = 1024;
        char szBuffer[MAX_BUFFER];
        char szAddressBuffer[MAX_PATH];

        char*  szCommentHeader =  "//";
        size_t nCommentHeaderLen = strlen(szCommentHeader);

        char* szDelimiter = ":\r\n\t ";

        while(fgets(szBuffer, MAX_BUFFER - 1, lpFile))
        {
            szBuffer[MAX_BUFFER - 1] = 0;
            unsigned long dwAddress = 0;

            char* szToken = strtok(szBuffer, szDelimiter);
            if(0 != szToken && 0 != strncmp(szCommentHeader, szToken, nCommentHeaderLen))
            {
                char* szServerGroup  = szToken;
                char* szAddress      = strtok(0, szDelimiter);
                char* szAddressStart = strtok(0, szDelimiter);
                char* szAddressEnd   = strtok(0, szDelimiter);

                if(0 != szAddress)
                {
                    int nServerGroup = atoi(szServerGroup);

                    if(0 != szAddressStart && 0 != szAddressEnd)
                    {
                        int nAddressStart = atoi(szAddressStart);
                        int nAddressEnd   = atoi(szAddressEnd);

                        for(; nAddressStart <= nAddressEnd; ++nAddressStart)
                        {
                            if(0 < _snprintf(szAddressBuffer, MAX_PATH - 1, 
                                "%s%d", szAddress, nAddressStart))
                            {
                                szAddressBuffer[MAX_PATH - 1] = 0;
                                dwAddress = inet_addr(szAddressBuffer);

                                if(0 != dwAddress)
                                {
                                    m_ipList.push_back(AllowedIP(dwAddress, nServerGroup));
                                }
                            }
                        }
                    }
                    else
                    {
                        // 주소가 하나밖에 없다.
                        dwAddress = inet_addr(szAddress);

                        if(0 != dwAddress)
                        {
                            m_ipList.push_back(AllowedIP(dwAddress, nServerGroup));
                        }
                    }
                }

            }
        }

        std::sort(m_ipList.begin(), m_ipList.end(), FindByIP());

        fclose(lpFile);
        return true;
    }

    return false;
}



bool CLoginAgentServerTable::AddConnection(unsigned long dwServerID, IN_ADDR ServerAddress, 
                                           CLoginAgentDispatch* lpLoginAgentDispatch)
{
    SERVER_ID   serverID;
    serverID.dwID = dwServerID;

	if (0 <= serverID.sID.Group && serverID.sID.Group < SERVER_ID::MAX_GROUP_NUM && 0 != lpLoginAgentDispatch)
    {
        RylServerInfo& ServerInfo = m_AgentServerInfo[serverID.sID.Group];
	    if (0 == ServerInfo.m_ServerUID.dwID && 0 == m_lpLoginAgentDispatch[serverID.sID.Group])
	    {
            ServerInfo.m_ServerUID      = serverID;
            ServerInfo.m_ServerAddress  = ServerAddress;
            m_lpLoginAgentDispatch[serverID.sID.Group] = lpLoginAgentDispatch;
	        ++m_nAgentNum;	
            return true;
	    }
			
		CLoginAgentDispatch* lpLoginAgentDispatch = m_lpLoginAgentDispatch[serverID.sID.Group];

        if (0 != lpLoginAgentDispatch && 
            ServerAddress.S_un.S_addr == ServerInfo.m_ServerAddress.S_un.S_addr)
        {
            // 동일 머신으로 로그인 시도. 둘 다 접속 해제함.
            lpLoginAgentDispatch->Shutdown();
        }
    }

    return false;
}


void CLoginAgentServerTable::RemoveConnection(unsigned long dwServerID, CLoginAgentDispatch* lpLoginAgentDispatch)
{
    SERVER_ID ServerID;
    ServerID.dwID = dwServerID;

    if (0 <= ServerID.sID.Group && ServerID.sID.Group < SERVER_ID::MAX_GROUP_NUM)
    {
        if (dwServerID == m_AgentServerInfo[ServerID.sID.Group].m_ServerUID.dwID &&
            lpLoginAgentDispatch == m_lpLoginAgentDispatch[ServerID.sID.Group])
        {
            RylServerInfo& ServerInfo = m_AgentServerInfo[ServerID.sID.Group];

            ServerInfo.m_ServerUID.dwID             = 0;
            ServerInfo.m_ServerAddress.S_un.S_addr  = 0;
            
            ServerInfo.m_dwClientVer    = 0;
            ServerInfo.m_dwChecksum     = 0;
            
            ServerInfo.m_nPatchAddressLen = 0;            
            memset(ServerInfo.m_szPatchAddress, 0, sizeof(char) * RylServerInfo::PATCH_ADDRESS_LENGTH);

            memset(ServerInfo.m_usChannelClientNum, 0, 
                sizeof(unsigned short) * SERVER_GROUP::CHANNEL_NUM);
            ServerInfo.m_usChannelNum = 0;

            m_lpLoginAgentDispatch[ServerID.sID.Group] = 0;
            --m_nAgentNum;
        }
    }
}

bool CLoginAgentServerTable::GetServerList(SERVER_LIST &ServerList, in_addr peerAddr)
{
    memset(&ServerList, 0, sizeof(SERVER_LIST));

    if(SERVER_OPEN == GetOperateMode())
    {
        unsigned int nGroup = 0;
        unsigned int nGroupProcessed = 0;

	    for (; nGroup < SERVER_ID::MAX_GROUP_NUM && nGroupProcessed < m_nAgentNum; ++nGroup)
	    {
		    RylServerInfo& ServerInfo = m_AgentServerInfo[nGroup];        

		    if (0 != ServerInfo.m_usChannelNum)
		    {
                bool bAllowed = false;

                switch(m_ipMode[nGroup])
                {
                case ALLOW_SOME:
                    {
                        IPList::iterator end = m_ipList.end();
                        std::pair<IPList::iterator, IPList::iterator> result 
                            = std::equal_range(m_ipList.begin(), end, 
                            AllowedIP(peerAddr.S_un.S_addr, 0), FindByIP());

                        for(; result.first != result.second; ++result.first )
                        {
                            int nResultGroup = result.first->second;

                            if(-1 == nResultGroup || -2 == nResultGroup ||
                                nGroup == nResultGroup)
                            {
                                // 전서버군 / 운영자 / 서버그룹 맞을때만 들어올 수 있다.
                                bAllowed = true;                            
                                break;
                            }
                        }
                    }
                    break;

                case ALLOW_ALL:

                    bAllowed = true;
                    break;


                case DENY_ALL_BUT_ALLOW_ADMIN:

                    IPList::iterator end = m_ipList.end();
                    std::pair<IPList::iterator, IPList::iterator> result 
                        = std::equal_range(m_ipList.begin(), end, 
                        AllowedIP(peerAddr.S_un.S_addr, 0), FindByIP());

                    for(; result.first != result.second; ++result.first )
                    {
                        int nResultGroup = result.first->second;

                        if(-2 == nResultGroup)
                        {
                            // 전부 막혀 있을 경우에는 운영자만 들어올 수 있다.
                            bAllowed = true;                            
                            break;
                        }
                    }
                }

                if(bAllowed)
                {
			        strncpy(ServerList.m_ServerGroup[nGroupProcessed].m_Name, 
				        ServerInfo.m_szGroupName, SERVER_GROUP::SERVER_NAME);
        	        
			        ServerList.m_ServerGroup[nGroupProcessed].m_Group	= ServerInfo.m_ServerUID.sID.Group;
			        ServerList.m_ServerGroup[nGroupProcessed].m_Address	= ServerInfo.m_ServerAddress;

			        unsigned short* lpdwChannelPastEnd = ServerInfo.m_usChannelClientNum + SERVER_GROUP::CHANNEL_NUM;
			        std::copy(ServerInfo.m_usChannelClientNum, lpdwChannelPastEnd, 
				        ServerList.m_ServerGroup[nGroupProcessed].m_ClientNum);

			        ++nGroupProcessed;
                }
            }
	    }

	    ServerList.m_ServerNum = static_cast<unsigned short>(nGroupProcessed);
    }
    else if(SERVER_PATCH_ONLY == GetOperateMode())
    {
        unsigned int nGroup = 0;
        unsigned int nGroupProcessed = 0;

        for (; nGroup < SERVER_ID::MAX_GROUP_NUM && nGroupProcessed < m_nAgentNum; ++nGroup)
        {
            RylServerInfo& ServerInfo = m_AgentServerInfo[nGroup];        

            if (0 != ServerInfo.m_usChannelNum)
            {
                bool bAllowed = false;

                // 패치만인 경우는, 서버 그룹이 -2인 경우만 들어올 수 있다.
                IPList::iterator end = m_ipList.end();
                std::pair<IPList::iterator, IPList::iterator> result 
                    = std::equal_range(m_ipList.begin(), end, 
                    AllowedIP(peerAddr.S_un.S_addr, 0), FindByIP());

                for(; result.first != result.second; ++result.first )
                {
                    int nResultGroup = result.first->second;

                    if(-2 == nResultGroup)
                    {
                        bAllowed = true;                            
                        break;
                    }
                }

                if(bAllowed)
                {
                    strncpy(ServerList.m_ServerGroup[nGroupProcessed].m_Name, 
                        ServerInfo.m_szGroupName, SERVER_GROUP::SERVER_NAME);

                    ServerList.m_ServerGroup[nGroupProcessed].m_Group	= ServerInfo.m_ServerUID.sID.Group;
                    ServerList.m_ServerGroup[nGroupProcessed].m_Address	= ServerInfo.m_ServerAddress;

                    unsigned short* lpdwChannelPastEnd = ServerInfo.m_usChannelClientNum + SERVER_GROUP::CHANNEL_NUM;
                    std::copy(ServerInfo.m_usChannelClientNum, lpdwChannelPastEnd, 
                        ServerList.m_ServerGroup[nGroupProcessed].m_ClientNum);

                    ++nGroupProcessed;
                }
            }
        }

        ServerList.m_ServerNum = static_cast<unsigned short>(nGroupProcessed);
    }

    return true;
}


bool CLoginAgentServerTable::GetServerName(SERVER_ID &ServerID, char* szBuffer_Out, size_t* nBufferSize_InOut)
{
	if (0 <= ServerID.sID.Group && ServerID.sID.Group < SERVER_ID::MAX_GROUP_NUM)
    {
        RylServerInfo& ServerInfo = m_AgentServerInfo[ServerID.sID.Group];
        if (ServerInfo.m_nGroupNameLen <= *nBufferSize_InOut)
        {
            strncpy(szBuffer_Out, ServerInfo.m_szGroupName, ServerInfo.m_nGroupNameLen);
            szBuffer_Out[ServerInfo.m_nGroupNameLen] = '\0';
            *nBufferSize_InOut = ServerInfo.m_nGroupNameLen;
            return true;
        }
    }

    return false;
}

bool CLoginAgentServerTable::UpdateServerData(unsigned long dwServerID, IN_ADDR AuthAddr, unsigned long dwClientVer, 
                                                const char* szPatchAddress)
{
    SERVER_ID ServerID;
    ServerID.dwID = dwServerID;

	if (0 <= ServerID.sID.Group && ServerID.sID.Group < SERVER_ID::MAX_GROUP_NUM)
    {
        RylServerInfo& ServerInfo = m_AgentServerInfo[ServerID.sID.Group];
        if (dwServerID == ServerInfo.m_ServerUID.dwID && 0 != m_lpLoginAgentDispatch[ServerID.sID.Group])
        {
            ServerInfo.m_dwClientVer	= dwClientVer;     
			ServerInfo.m_ServerAddress	= AuthAddr;
            strncpy(ServerInfo.m_szPatchAddress, szPatchAddress, RylServerInfo::PATCH_ADDRESS_LENGTH - 1);
            ServerInfo.m_szPatchAddress[RylServerInfo::PATCH_ADDRESS_LENGTH - 1] = '\0';
            ServerInfo.m_nPatchAddressLen = strlen(ServerInfo.m_szPatchAddress);
            return true;
        }
    }

    return false;
}


bool CLoginAgentServerTable::UpdateChannelUserNum(unsigned long dwServerID, unsigned short* lpdwClientChannelNum, 
                                            unsigned short usChannelNum)
{
    SERVER_ID ServerID;
    ServerID.dwID = dwServerID;

	if (0 <= ServerID.sID.Group && ServerID.sID.Group < SERVER_ID::MAX_GROUP_NUM)
    {
        RylServerInfo& ServerInfo = m_AgentServerInfo[ServerID.sID.Group];
        if (dwServerID == ServerInfo.m_ServerUID.dwID && 0 != m_lpLoginAgentDispatch[ServerID.sID.Group])
        {
 			if (0 == ServerInfo.m_ServerAddress.S_un.S_addr)
			{
				ServerInfo.m_usChannelNum = 0;
			}
			else
			{
	      	    ServerInfo.m_usChannelNum = usChannelNum;
			}

            unsigned short* lpdwPastEnd = lpdwClientChannelNum + 
                std::min(unsigned short(usChannelNum), unsigned short(SERVER_GROUP::CHANNEL_NUM));

            std::copy(lpdwClientChannelNum, lpdwPastEnd, ServerInfo.m_usChannelClientNum);
            return true;
        }
    }

    return false;
}

void CLoginAgentServerTable::GetAgentServerInfo(RylServerInfo* lpAgentServerInfo_Out,
                                                  unsigned long* nAgentNum_InOut)
{
    RylServerInfo* lpAgentPastEnd = m_AgentServerInfo 
		+ std::min(unsigned long(SERVER_ID::MAX_GROUP_NUM), *nAgentNum_InOut);

    std::copy(m_AgentServerInfo, lpAgentPastEnd, lpAgentServerInfo_Out);
}