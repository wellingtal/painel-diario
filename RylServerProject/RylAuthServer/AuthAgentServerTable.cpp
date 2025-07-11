#include "stdafx.h"
#include "AuthAgentDispatch.h"
#include "AuthAgentServerTable.h"

#include <Network/Session/Session.h>

#include <Log/ServerLog.h>

CAuthAgentServerTable& CAuthAgentServerTable::GetInstance()
{
	static CAuthAgentServerTable authAgentServerTable;
	return authAgentServerTable;
}


CAuthAgentServerTable::CAuthAgentServerTable()
{
	memset(&m_AgentServerInfo, 0, sizeof(RylServerInfo));
}


CAuthAgentServerTable::~CAuthAgentServerTable()
{

}

void CAuthAgentServerTable::GetPatchServerAddress(unsigned long& dwClientVer, 
                                                  char* szPatchAddress_Out, size_t nBufferLen)
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);

    dwClientVer = m_AgentServerInfo.m_dwClientVer;

	int nLength = _snprintf(szPatchAddress_Out, nBufferLen - 1, "%s", m_AgentServerInfo.m_szPatchAddress);
	if(0 < nLength)
	{
        szPatchAddress_Out[nLength] = '\0';
	}
}


void CAuthAgentServerTable::UpdateServerData(unsigned long dwChecksum, unsigned long dwClientVer, 
											 const char* szPatchAddress)
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);
    
    m_AgentServerInfo.m_dwChecksum  = dwChecksum;
    m_AgentServerInfo.m_dwClientVer = dwClientVer;

	int nLength = _snprintf(m_AgentServerInfo.m_szPatchAddress, RylServerInfo::PATCH_ADDRESS_LENGTH - 1,
		"%s", szPatchAddress);

	if(0 < nLength)
	{
        m_AgentServerInfo.m_szPatchAddress[nLength] = '\0';
		m_AgentServerInfo.m_nPatchAddressLen = nLength;
	}
}


void CAuthAgentServerTable::UpdateChannelUserNum(unsigned short* lpdwClientChannelNum, 
												 unsigned short usChannelNum)
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);

    m_AgentServerInfo.m_usChannelNum = usChannelNum;

    unsigned short* lpdwPastEnd = lpdwClientChannelNum + 
		std::min(unsigned short(usChannelNum), unsigned short(SERVER_GROUP::CHANNEL_NUM));

    std::copy(lpdwClientChannelNum, lpdwPastEnd, m_AgentServerInfo.m_usChannelClientNum);    
}


unsigned long CAuthAgentServerTable::GetServerVersion()
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);
    return m_AgentServerInfo.m_dwClientVer;
}

unsigned long CAuthAgentServerTable::GetServerChecksum()
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);
    return m_AgentServerInfo.m_dwChecksum;
}


void CAuthAgentServerTable::SetAgentServerInfo(RylServerInfo& AgentServerInfo)
{
	AuthAgentLock::Syncronize sync(m_AuthAgentLock);
	m_AgentServerInfo = AgentServerInfo;
}


void CAuthAgentServerTable::GetAgentServerInfo(RylServerInfo& AgentServerInfo)
{
    AuthAgentLock::Syncronize sync(m_AuthAgentLock);
    AgentServerInfo = m_AgentServerInfo;
}


/*
bool CAuthAgentServerTable::InsertUIDDB(const unsigned long dwUID, CAuthClientDispatch* lpClientDispatch)
{
	UIDDBLock::Syncronize sync(m_UIDDBLock);

	RemoveUIDDB(dwUID);
	return m_UIDDB.insert(std::make_pair(dwUID, lpClientDispatch)).second;
}

void CAuthAgentServerTable::RemoveUIDDB(const unsigned long dwUID)
{
	UIDDBLock::Syncronize sync(m_UIDDBLock);

	m_UIDDB.erase(dwUID);
}

CAuthClientDispatch* CAuthAgentServerTable::FindInUIDDB(const unsigned long dwUID)
{
	UIDDBLock::Syncronize sync(m_UIDDBLock);

	UIDDB::iterator find = m_UIDDB.find(dwUID);
    return (find != m_UIDDB.end()) ? (find->second) : 0;
}

*/