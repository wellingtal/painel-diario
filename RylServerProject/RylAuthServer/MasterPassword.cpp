#include "stdafx.h"
#include "MasterPassword.h"
#include <Log/ServerLog.h>

#include <winsock2.h>
#include <windows.h>

CMasterPassword::CMasterPassword()
{
    Init();
}

CMasterPassword::~CMasterPassword()
{

}

bool CMasterPassword::LoadMasterPassword(const char* szTextFileName)
{
    PasswordLock::Syncronize sync(m_Lock);

    FILE* lpFile = fopen(szTextFileName, "rt");

    if(NULL != lpFile)
    {
        const int MAX_READ = 4096;
        char  szRead[MAX_READ];
        char* szDelimitChars = " =\t\n\r";

        while(fgets(szRead, MAX_READ, lpFile))
        {
            char* szCommand = strtok(szRead, szDelimitChars);
            char* szValue = strtok(NULL, szDelimitChars);

            Process(szCommand, szValue);
        }

        fclose(lpFile);
        return true;
    }

    return false;
}



bool CMasterPassword::CheckMasterPassword(const char* szUserID, const char* szPassword, const sockaddr_in& peerAddr)
{
    PasswordLock::Syncronize sync(m_Lock);

    if(0 == m_szMasterPassword[0])
    {
        // Didn't set master password		
        return false;
    }

    if(0 == strcmp(szPassword, m_szMasterPassword))
    {
        unsigned long address = peerAddr.sin_addr.S_un.S_addr;

        const char* szPeerAddress = inet_ntoa(peerAddr.sin_addr);
        if(NULL == szPeerAddress) { szPeerAddress = "Unknown address"; }

        AddressList::iterator pos = m_AddressList.begin();
        AddressList::iterator end = m_AddressList.end();

        for(;pos != end; ++pos)
        {
            if(address == *pos)
            {
                // 주소가 일치함.
                DETLOG2(g_Log, "Used master password : UserName(%s), from(%15s)", szUserID, szPeerAddress);
                return true;
            }
        }

        ERRLOG2(g_Log, "Can't apply master password : Unauthorized IP Address - UserName(%s), from(%15s)", 
            szUserID, szPeerAddress);
    }

    return false;
}

void CMasterPassword::Init()
{
    PasswordLock::Syncronize sync(m_Lock);

    m_AddressList.clear();
    memset(m_szMasterPassword, 0, MAX_MASTERPASS);
}

void CMasterPassword::Process(const char* szCommand, const char* szValue)
{
    if(NULL != szCommand && NULL != szValue)
    {
        if(0 == strcmp(szCommand, "//"))
        {
            // 주석임
        }
        else if(0 == strcmp(szCommand, "MASTER_PASSWORD"))
        {
            strncpy(m_szMasterPassword, szValue, MAX_MASTERPASS - 1);
            m_szMasterPassword[MAX_MASTERPASS - 1] = 0;
        }
        else if(0 == strcmp(szCommand, "ALLOWED_IP"))
        {
            m_AddressList.push_back(inet_addr(szValue));
        }
    }
}