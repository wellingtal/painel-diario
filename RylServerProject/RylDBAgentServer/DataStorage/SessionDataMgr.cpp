#include "stdafx.h"
#include "SessionDataMgr.h"
#include "CharacterData.h"

#include <DB/DBComponent.h>

#include <Community/Party/PartyDBInfo.h>
#include <Community/Party/PartyDBMgr.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <Log/ServerLog.h>

#ifdef _RYL_GAME_CLIENT_
#include "RYLNetworkData.h"
#endif

#ifndef _RYL_GAME_CLIENT_
#include <Utility/Setup/ServerSetup.h>
#endif


namespace DBAgent
{
namespace DataStorage
{

CSessionDataMgr& CSessionDataMgr::GetInstance()
{
    static CSessionDataMgr sessionDataMgr;
    return sessionDataMgr;
}

CSessionData* CSessionDataMgr::GetOpenSession(unsigned long dwUID)
{
    SessionMap::iterator pos = m_uidMap.find(dwUID);
    return pos != m_uidMap.end() ? pos->second : 0;    
}


CSessionData* CSessionDataMgr::GetOpenSession(const char* szAccountName, 
                                              IN_ADDR peerAddress, bool bCheckAddress)
{
    SessionMap::iterator pos = m_uidMap.begin();
    SessionMap::iterator end = m_uidMap.end();

    CSessionData* lpSessionData = 0;
    CSessionData* lpResultSessionData = 0;

    for(; pos != end; ++pos)
    {
        lpSessionData = pos->second;

        if((bCheckAddress && lpSessionData->GetRemoteAddress().S_un.S_addr == peerAddress.S_un.S_addr) &&
            0 == strncmp(szAccountName, lpSessionData->GetAccountName(), CHAR_INFOST::MAX_ACCOUNT_LEN))
        {
            lpResultSessionData = lpSessionData;
            break;
        }
    }

    return lpResultSessionData;
}

CSessionData* CSessionDataMgr::GetCharLoadedSession(unsigned long dwCID)
{
    SessionMap::iterator pos = m_cidMap.find(dwCID);
    return pos != m_cidMap.end() ? pos->second : 0;    
}

bool CSessionDataMgr::IsLoadedChar(const char* szCharName)
{
	if (NULL != szCharName) 
    {
        SessionMap::iterator pos = m_cidMap.begin();
        SessionMap::iterator end = m_cidMap.end();

        CSessionData*   lpSessionData = 0;
        CCharacterData* lpCharData    = 0;

        for(; pos != end; ++pos)
	    {
			lpSessionData = pos->second;
            if(NULL != lpSessionData && NULL != (lpCharData = lpSessionData->GetCharacterData()))
            {
				if (NULL != lpCharData->GetName() && 
					0 == strncmp(lpCharData->GetName(), szCharName, CHAR_INFOST::MAX_NAME_LEN))
				{
					return true;
				}
		    }
	    }
    }

	return false;
}

CSessionData* CSessionDataMgr::SessionOpen(unsigned long dwUID)
{
    CSessionData* lpSessionData = 0;

    if(0 == dwUID)
    {
        SERLOG0(g_Log, "���� ���� ���� : �߸��� ���� ���̵��Դϴ�.");        
    }
    else
    {
        // ���� DB�� ���� ã��
        SessionMap::iterator pos = m_uidMap.find(dwUID);
        if(pos != m_uidMap.end())
        {
            lpSessionData = pos->second;

            CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

            if (CSessionData::SE_USER_ENABLED == eSessionState ||
                CSessionData::SE_CHAR_ENABLED == eSessionState)
            {
                ERRLOG1(g_Log, "UID:%10u / ���� ���� ���� : �̹� ������ �����մϴ�.", dwUID);
                lpSessionData = 0;
            }
            else
            {
                INFLOG1(g_Log, "UID:%10u / �̹� �����ϴ� ���� ���� ����", dwUID);
            }
        }
        else
        {
            lpSessionData = new CSessionData;

            if(0 != lpSessionData)
            {
                if(m_uidMap.insert(std::make_pair(dwUID, lpSessionData)).second)
                {
                    lpSessionData->SetUID(dwUID);
                    INFLOG1(g_Log, "UID:%10u / ���� ���� ����", dwUID);
                }
                else
                {
                    ERRLOG1(g_Log, "UID:%10u / ���� ���� ���� : UID�ʿ� ���� ����", dwUID);
						
					if(lpSessionData)
					{
						delete lpSessionData;
						lpSessionData = NULL;
					}						
                }
            }
        }
    }

    return lpSessionData;
}


void CSessionDataMgr::SessionClose(unsigned long dwUID, unsigned long dwSessionID)
{
    SessionMap::iterator pos = m_uidMap.find(dwUID);
    if(pos == m_uidMap.end())
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �ݱ� ���� : ������ �������� �ʽ��ϴ�", dwUID);        
    }
    else
    {
        CSessionData* lpSessionData = pos->second;
        m_uidMap.erase(pos);
        
        if(lpSessionData && lpSessionData->GetSessionID() != dwSessionID)
        {
            SERLOG3(g_Log, "UID:%10u / OldSessionID:%10u / CloseSessionID:%10u / ���� �ݱ� ���� : "
                "���� ID�� �ٸ��ϴ�", dwUID, lpSessionData->GetSessionID(), dwSessionID);
        }

		if(lpSessionData)
		{
			delete lpSessionData;
			lpSessionData = NULL;
		}
    }
}


void CSessionDataMgr::SessionCloseWithLogout(unsigned long dwUID, unsigned long dwServerID)
{
    SessionMap::iterator pos = m_uidMap.find(dwUID);
    if(pos == m_uidMap.end())
    {
        ERRLOG1(g_Log, "UID:%10u / ���� ������ �ݱ� ���� : ������ �������� �ʽ��ϴ�", dwUID);        
    }
    else
    {
        CSessionData* lpSessionData = pos->second;

        if(lpSessionData->GetServerID() != dwServerID)
        {
            SERLOG3(g_Log, "UID:%10u / ServerID:0x%08X / CloseServerID:0x%08X / ���� ������ �ݱ� ���� : "
                "�������� ���� ID�� �ٸ��ϴ�", dwUID, lpSessionData->GetServerID(), dwServerID);
        }
        else            
        {
            SessionCloseWithLogout(pos);
        }        
    }
}


void CSessionDataMgr::SessionCloseWithLogout(SessionMap::iterator logoutPos)
{
    CSessionData* lpSessionData = logoutPos->second;
    m_uidMap.erase(logoutPos);

    CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

    if(CSessionData::SE_USER_ENABLED == eSessionState)
    {
        // ���� Disable
        lpSessionData->UserDisable();
    }
    else if(CSessionData::SE_CHAR_ENABLED == eSessionState)
    {
        CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();
        if(0 != lpCharacterData)
        {
            unsigned long dwCID = lpCharacterData->GetCID();
            unsigned long dwPID = lpCharacterData->GetInfo().PID;
            unsigned long dwGID = lpCharacterData->GetInfo().GID;
            
            // ��Ƽ�� �α׾ƿ� �ϸ�
            CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
                CDBSingleObject::GetInstance(), dwPID);

            if (0 != lpParty)
            {
                CPartyDBMgr::GetInstance().LogoutPartyMember(
                    CDBSingleObject::GetInstance(), lpParty, dwCID);
            }

            // ��忡 �α׾ƿ� �˸�
            CGuildDB* lpGuild = static_cast<CGuildDB*>(
                CGuildDBMgr::GetInstance().GetGuild(dwGID));

            if (0 != lpGuild)
            {
                lpGuild->SendLoginOutMember(dwCID, 0);
            }
        }

        // ĳ���� Disable
        lpSessionData->CharDisable(
            lpSessionData->GetCID(), lpSessionData->GetServerID());
    }

    // ���� ��������.
	
	if(lpSessionData)
	{
		delete lpSessionData;
		lpSessionData = NULL;
	}		
}



void CSessionDataMgr::SessionCloseCurrentServer(unsigned long dwServerID)
{
    INFLOG1(g_Log, "ServerID:0x%08X / ���� �α� �ƿ����� ���� �α� �ƿ� ó�� ����", dwServerID);

    SessionMap::iterator pos = m_uidMap.begin();
    SessionMap::iterator end = m_uidMap.end();
    
    unsigned int nLogoutCount = 0;

    for(; pos != end; )
    {
        CSessionData* lpSessionData = pos->second;

        if(lpSessionData->GetServerID() == dwServerID)
        {
            SessionCloseWithLogout(pos++);
            ++nLogoutCount;
        }
        else
        {
            ++pos;
        }
    }

    INFLOG2(g_Log, "ServerID:0x%08X / ���� �α� �ƿ����� ���� �α� �ƿ� ó�� �Ϸ� : %u��", 
        dwServerID, nLogoutCount);    
}


bool CSessionDataMgr::AddCharLoadedSession(unsigned long dwCID, CSessionData& SessionData)
{
    if(m_cidMap.insert(std::make_pair(dwCID, &SessionData)).second)
    {
        INFLOG1(g_Log, "CID:%10u / Ȱ��ȭ�� ���� �߰� ���� ", dwCID);
        return true;
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u / Ȱ��ȭ�� ���� �߰� ���� : �̹� ������ �ֽ��ϴ�.", dwCID);
    }
    
    return false;
}

bool CSessionDataMgr::RemoveCharLoadedSession(unsigned long dwCID)
{
    if(0 != m_cidMap.erase(dwCID))
    {
        INFLOG1(g_Log, "CID:%10u / Ȱ��ȭ�� ���� ���� ���� ", dwCID);
        return true;
    }
    else
    {
        ERRLOG1(g_Log, "CID:%10u / Ȱ��ȭ�� ���� ���� ���� : ������ �����ϴ�", dwCID);
    }

    return false;
}

// Load �� ĳ������ �ν��Ͻ� ���� �÷��� ������ OFF �� ������ش�.
void CSessionDataMgr::ClearGuildWarInstanceFlag()
{
	if (m_cidMap.empty())
		return;

	SessionMap::iterator pos = m_cidMap.begin();
	SessionMap::iterator end = m_cidMap.end();

	CSessionData*   lpSessionData = 0;
	CCharacterData* lpCharData    = 0;

	for(; pos != end; ++pos)
	{
		lpSessionData = pos->second;

#ifdef _RYL_GAME_CLIENT_
		if(GameRYL::GLOBAL == CRYLNetworkData::Instance()->m_eInternationalCode)
#endif

#ifndef _RYL_GAME_CLIENT_
			if(GameRYL::GLOBAL == CServerSetup::GetInstance().GetNationType())
#endif
			{
				if(NULL != lpSessionData)
				{
					if(NULL != (lpCharData = lpSessionData->GetCharacterData()))
					{
						if (Creature::WAR_INSTANCE == lpCharData->GetGuildWarFlag())
						{
							lpCharData->SetGuildWarFlag(Creature::WAR_OFF);
						}
					}
				}
			}
			else
			{
				if(NULL != lpSessionData && NULL != (lpCharData = lpSessionData->GetCharacterData()))
				{
					if (Creature::WAR_INSTANCE == lpCharData->GetGuildWarFlag())
					{
						lpCharData->SetGuildWarFlag(Creature::WAR_OFF);
					}
				}
			}
	}
}

void CSessionDataMgr::ClearRealmWarInstanceFlag()
{
	if (m_cidMap.empty())
		return;

	SessionMap::iterator pos = m_cidMap.begin();
	SessionMap::iterator end = m_cidMap.end();

	CSessionData*   lpSessionData = 0;
	CCharacterData* lpCharData    = 0;

	for(; pos != end; ++pos)
	{
		lpSessionData = pos->second;

#ifdef _RYL_GAME_CLIENT_
		if(GameRYL::GLOBAL == CRYLNetworkData::Instance()->m_eInternationalCode)
#endif

#ifndef _RYL_GAME_CLIENT_
			if(GameRYL::GLOBAL == CServerSetup::GetInstance().GetNationType())
#endif
			{
				if(NULL != lpSessionData)
				{
					if(NULL != (lpCharData = lpSessionData->GetCharacterData()))
					{
						if (Creature::WAR_INSTANCE == lpCharData->GetRealmWarFlag())
						{
							lpCharData->SetRealmWarFlag(Creature::WAR_OFF);
						}
					}
				}
			}
			else
			{
				if(NULL != lpSessionData && NULL != (lpCharData = lpSessionData->GetCharacterData()))
				{
					if (Creature::WAR_INSTANCE == lpCharData->GetRealmWarFlag())
					{
						lpCharData->SetRealmWarFlag(Creature::WAR_OFF);
					}
				}
			}
	}
}




}
}

