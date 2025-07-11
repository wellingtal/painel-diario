#include "stdafx.h"
#include "PartyDBInfo.h"

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Log/ServerLog.h>


namespace DBAgent
{

CPartyDBInfo::CPartyDBInfo(unsigned long PartyID_In)		
{	
	memset(&m_Party, 0, sizeof(PARTY));
	m_Party.m_dwPartyID = PartyID_In;
	
	std::fill_n(&m_Party.ServerID[0], int(PARTY::MAX_MEM), 0);
	std::fill_n(&m_bAutoRouting[0], int(PARTY::MAX_MEM), 1);

	std::fill_n(&m_Party.m_cLevel[0], int(PARTY::MAX_MEM), 0);
	std::fill_n(&m_Party.m_wClass[0], int(PARTY::MAX_MEM), 0);
	std::fill_n(&m_Party.m_dwGID[0], int(PARTY::MAX_MEM), 0);
}

bool CPartyDBInfo::SerializeIn(PARTY_INFO& PartyInfo_In, PARTY_USER_INFO& PartyUserInfo_In)
{
	int Count = 0;
	for (; Count < PARTY::MAX_MEM; ++Count)
	{
		if (0 == PartyInfo_In.MemberCID[Count])
		{
			break;
		}

		strncpy((char *)m_Party.Name[Count], (char *)PartyInfo_In.Name[Count], CHAR_INFOST::MAX_NAME_LEN);
		m_Party.MemberCID[Count] = PartyInfo_In.MemberCID[Count];
		m_Party.ServerID[Count]	= 0;

		// 유저 정보 //
		m_Party.m_cLevel[Count] = PartyUserInfo_In.m_cLevel[Count];
		m_Party.m_wClass[Count] = PartyUserInfo_In.m_wClass[Count];
		m_Party.m_dwGID[Count]	= PartyUserInfo_In.m_dwGID[Count];
	}

	m_Party.m_cMemberNum	= Count;	
	return true;
}

bool CPartyDBInfo::SerializeOut(PARTY_INFO& PartyInfo_Out, PARTY_USER_INFO& PartyUserInfo_Out)
{
	if (m_Party.m_cMemberNum > PARTY::MAX_MEM)
	{
		return false;
	}

	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		PartyInfo_Out.MemberCID[Count] = m_Party.MemberCID[Count];
		strncpy((char *)PartyInfo_Out.Name[Count], (char *)m_Party.Name[Count], CHAR_INFOST::MAX_NAME_LEN);

		// 유저 정보 //
		PartyUserInfo_Out.m_cLevel[Count]	= m_Party.m_cLevel[Count];
		PartyUserInfo_Out.m_wClass[Count]	= m_Party.m_wClass[Count];
		PartyUserInfo_Out.m_dwGID[Count]	= m_Party.m_dwGID[Count];
	}	

	return true;
}

unsigned long CPartyDBInfo::GetFirstMem(void)
{
	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		if (0 != m_Party.ServerID[Count]) 
		{
			return m_Party.MemberCID[Count];
		}
	}

	return 0;
}

bool CPartyDBInfo::IsMember(unsigned long CharID_In)
{
	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		if (m_Party.MemberCID[Count] == CharID_In)
		{
			return true;
		}
	}

	return false;
}

bool CPartyDBInfo::PartyMemLogin(unsigned long dwCharID_In, unsigned long dwServerID_In)
{
	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		if (m_Party.MemberCID[Count] == dwCharID_In)
		{
			m_Party.ServerID[Count] = dwServerID_In;
			if (0 == m_Party.m_dwLeaderID)
			{
				SetLeader(dwCharID_In);
			}

			return true;
		}
	}

	return false;
}

bool CPartyDBInfo::PartyMemLogout(unsigned long CharID_In)
{
	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		if (m_Party.MemberCID[Count] == CharID_In)
		{
			m_Party.ServerID[Count] = 0;
			if (m_Party.m_dwLeaderID == CharID_In)
			{
				SetLeader(GetFirstMem());
			}

			return true;
		}
	}

	return false;
}

void CPartyDBInfo::PartyMemAllLogout(void)
{
	std::fill_n(&m_Party.ServerID[0], int(PARTY::MAX_MEM), 0);
}

bool CPartyDBInfo::IsLogined(unsigned long CharID_In)
{
	for (int Count = 0; Count < m_Party.m_cMemberNum; ++Count)
	{
		if (m_Party.MemberCID[Count] == CharID_In)
		{
			if (m_Party.ServerID[Count] != 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool CPartyDBInfo::TransferLeader(unsigned long OldLeaderID, unsigned long NewLeaderID)
{
	if (OldLeaderID != GetLeader())
	{
		return false;
	}

	if (false == IsLogined(OldLeaderID) || false == IsLogined(NewLeaderID))
	{
		return false;
	}

	SetLeader(NewLeaderID);
	return true;	
}

bool CPartyDBInfo::InsertPartyMem(unsigned long ServerID_In, unsigned long CharID_In, unsigned short Class_In, char Level_In, unsigned long GID_In, const char* CharName_In)
{
	if (PARTY::MAX_MEM <= m_Party.m_cMemberNum)	// 파티인원이 10명 이상
	{
		return false;
	}

	m_Party.ServerID[m_Party.m_cMemberNum]	= ServerID_In;
	m_Party.MemberCID[m_Party.m_cMemberNum] = CharID_In;
	m_Party.m_cLevel[m_Party.m_cMemberNum]	= Level_In;
	m_Party.m_wClass[m_Party.m_cMemberNum]	= Class_In;
	m_Party.m_dwGID[m_Party.m_cMemberNum]	= GID_In;	

	strncpy(m_Party.Name[m_Party.m_cMemberNum], CharName_In, CHAR_INFOST::MAX_NAME_LEN);

	++m_Party.m_cMemberNum;
	return true;
}

bool CPartyDBInfo::DeletePartyMem(unsigned long CharID_In, unsigned long& ServerID_Out)
{
    using namespace DBAgent::DataStorage;

    int nCount = 0;

	for (; nCount < m_Party.m_cMemberNum; ++nCount)
	{
		if (m_Party.MemberCID[nCount] == CharID_In)
		{
            CSessionData*   lpSessionData   = 0;
            CCharacterData* lpCharacterData = 0;

            if(0 == m_Party.ServerID[nCount])
            {
                lpCharacterData = static_cast<CCharacterData*>(
                    CCharacterDataMgr::GetInstance().GetLogoutData(CharID_In));
            }
            else
            {
                lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(CharID_In);
                if(0 != lpSessionData)
                {
                    lpCharacterData = lpSessionData->GetCharacterData();
                }
            }

			if (0 != lpCharacterData && m_Party.m_dwPartyID == lpCharacterData->GetPID())
			{
                RULLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 : 캐릭터의 PID를 0으로 셋", 
                    m_Party.m_dwPartyID, CharID_In);

                lpCharacterData->SetPID(0);                
			}

			ServerID_Out = m_Party.ServerID[nCount];
			break;
		}
	}

	if (nCount == m_Party.m_cMemberNum)
	{
		return false;
	}

	for (; nCount < m_Party.m_cMemberNum; ++nCount)
	{
		if ((nCount + 1) >= PARTY::MAX_MEM)
		{
			m_Party.ServerID[nCount]	= 0;
			m_Party.MemberCID[nCount]	= 0;
			m_Party.m_cLevel[nCount]	= 0;
			m_Party.m_dwGID[nCount]		= 0;
			m_Party.m_wClass[nCount]	= 0;

			std::fill_n(&m_Party.Name[nCount][0], int(CHAR_INFOST::MAX_NAME_LEN), 0);

			break;
		}

		m_Party.ServerID[nCount]	= m_Party.ServerID[nCount + 1];
		m_Party.MemberCID[nCount]	= m_Party.MemberCID[nCount + 1];
		m_Party.m_cLevel[nCount]	= m_Party.m_cLevel[nCount + 1];
		m_Party.m_dwGID[nCount]		= m_Party.m_dwGID[nCount + 1];
		m_Party.m_wClass[nCount]	= m_Party.m_wClass[nCount + 1];
		strncpy((char *)m_Party.Name[nCount], (char *)m_Party.Name[nCount + 1], CHAR_INFOST::MAX_NAME_LEN);
	}

	if (m_Party.m_dwLeaderID == CharID_In)
	{
		SetLeader(GetFirstMem());
	}

	--m_Party.m_cMemberNum;
	return true;
}

bool CPartyDBInfo::SetUserInfoLevel(unsigned long dwCID, char cLevel)
{
	for(unsigned char cIndex = 0; cIndex < m_Party.m_cMemberNum; ++cIndex)
	{
		if(m_Party.MemberCID[cIndex]==dwCID)
		{
			m_Party.m_cLevel[cIndex] = cLevel;
			return true;
		}
	}
	return false;
}

bool CPartyDBInfo::SetUserInfoClass(unsigned long dwCID, unsigned short wClass)
{
	for(unsigned char cIndex = 0; cIndex < m_Party.m_cMemberNum; ++cIndex)
	{
		if(m_Party.MemberCID[cIndex]==dwCID)
		{
			m_Party.m_wClass[cIndex] = wClass;
			return true;
		}
	}
	return false;
}

bool CPartyDBInfo::SetUserInfoGID(unsigned long dwCID, unsigned long dwGID)
{	
	for(unsigned char cIndex = 0; cIndex < m_Party.m_cMemberNum; ++cIndex)
	{
		if(m_Party.MemberCID[cIndex]==dwCID)
		{
			m_Party.m_dwGID[cIndex] = dwGID;
			return true;
		}
	}
	return false;
}

bool CPartyDBInfo::AutoRouting(unsigned long CharID_In, bool bOn, bool bAll)
{
	if (bAll == true)
	{
		int Count = 0;
		for (; Count < m_Party.m_cMemberNum; ++Count)
		{
			m_bAutoRouting[Count] = bOn;
		}
	}
	else
	{
		int Count = 0;
		for (; Count < m_Party.m_cMemberNum; ++Count)
		{
			if (m_Party.MemberCID[Count] == CharID_In)
			{
				m_bAutoRouting[Count] = bOn;
				return true;
			}
		}
	}

	return true;
}

}