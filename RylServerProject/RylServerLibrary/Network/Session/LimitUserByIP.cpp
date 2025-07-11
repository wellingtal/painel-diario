#include "stdafx.h"
#include "LimitUserByIP.h"



CLimitUserByIP::CLimitUserByIP(const char* szFileName)
:	m_eAllowMode(ALLOW_ALL)
{
	if(0 != szFileName && LoadAllowIPList(szFileName))
	{
		m_eAllowMode = ALLOW_SOME;
	}
}

CLimitUserByIP::~CLimitUserByIP()
{

}

bool CLimitUserByIP::operator () (INET_Addr& localAddr, INET_Addr& remoteAddr)
{
	LimitLock::Syncronize sync(m_LimitLock);

	switch(m_eAllowMode)
	{
	case ALLOW_ALL:		return true;		 
	case DENY_ALL:		return false;
	case ALLOW_SOME:	

		return std::binary_search(m_AllowIPList.begin(), m_AllowIPList.end(), 
			remoteAddr.get_addr_in().sin_addr.s_addr);
	}

	return false;	
}

bool CLimitUserByIP::LoadAllowIPList(const char* szFileName)
{
	FILE* lpFile = fopen(szFileName, "rt");

    if(0 != lpFile)
    {
		LimitLock::Syncronize sync(m_LimitLock);

		m_AllowIPList.clear();
        m_AllowIPList.reserve(256);

        const int MAX_BUFFER = 512;
        char szBuffer[MAX_BUFFER];

        while(fgets(szBuffer, MAX_BUFFER - 1, lpFile))
		{
            szBuffer[MAX_BUFFER - 1] = 0;
		
			unsigned long dwAddr = inet_addr(szBuffer);
            if(0 != dwAddr)
            {
				m_AllowIPList.push_back(dwAddr);
            }
        }

        std::sort(m_AllowIPList.begin(), m_AllowIPList.end());

        fclose(lpFile);
        return true;
    }

    return false;
}

void CLimitUserByIP::OperateMode(AllowMode_t eAllowMode)
{
	LimitLock::Syncronize sync(m_LimitLock);
	m_eAllowMode = eAllowMode;
}

CLimitUserByIP::AllowMode_t CLimitUserByIP::OperateMode()
{
	LimitLock::Syncronize sync(m_LimitLock);
	return m_eAllowMode;
}

void CLimitUserByIP::ClearIPList()
{
	LimitLock::Syncronize sync(m_LimitLock);
	m_AllowIPList.clear();
}


bool CLimitUserByIP::AddAllowIP(unsigned long dwIP)
{
    LimitLock::Syncronize sync(m_LimitLock);

    IPList::iterator end = m_AllowIPList.end();
    IPList::iterator pos = std::lower_bound(m_AllowIPList.begin(), end, dwIP);

    if(pos == end || *pos != dwIP)
    {
        m_AllowIPList.insert(pos, dwIP);
    }

    return true;
}

bool CLimitUserByIP::AddAllowIP(INET_Addr& address)
{
    return AddAllowIP(address.get_addr_in().sin_addr.S_un.S_addr);
}

bool CLimitUserByIP::AddAllowIP(in_addr addr)
{
    return AddAllowIP(addr.S_un.S_addr);
}



bool CLimitUserByIP::RemoveAllowIP(unsigned long dwIP)
{
    LimitLock::Syncronize sync(m_LimitLock);

    IPList::iterator end = m_AllowIPList.end();
    IPList::iterator pos = std::lower_bound(m_AllowIPList.begin(), end, dwIP);

    if(pos != end && *pos == dwIP)
    {
        m_AllowIPList.erase(pos);
    }

    return true;
}

bool CLimitUserByIP::RemoveAllowIP(INET_Addr& address)
{
    return RemoveAllowIP(address.get_addr_in().sin_addr.S_un.S_addr);
}

bool CLimitUserByIP::RemoveAddAllowIP(in_addr addr)
{
    return RemoveAllowIP(addr.S_un.S_addr);
}
