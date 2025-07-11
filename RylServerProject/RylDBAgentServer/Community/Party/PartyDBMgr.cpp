#include "stdafx.h"
#include "PartyDBMgr.h"
#include "PartyDBInfo.h"

#include <Network/SendPacket/SendParty.h>
#include <Network/Dispatch/GameDispatch.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Log/ServerLog.h>

#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketCommand.h>

#include <new>

namespace DBAgent
{

CPartyDBMgr& CPartyDBMgr::GetInstance()
{
    static CPartyDBMgr partyDBMgr;
    return partyDBMgr;
}

CPartyDBMgr::CPartyDBMgr()
:   m_PartyDBPool(sizeof(CPartyDBInfo))
{

}


CPartyDBInfo* CPartyDBMgr::CreatePartyDBInfo(unsigned long dwPID)
{
    return new CPartyDBInfo(dwPID);

    // return new (m_PartyDBPool.malloc()) CPartyDBInfo(dwPID);
}

void CPartyDBMgr::DeletePartyDBInfo(CPartyDBInfo* lpPartyDBInfo)
{
    delete lpPartyDBInfo;

    // lpPartyDBInfo->~CPartyDBInfo(); 
    // m_PartyDBPool.free(lpPartyDBInfo);
}


CPartyDBMgr::~CPartyDBMgr()
{
    PartyMap::iterator pos = m_PartyMap.begin();
    PartyMap::iterator end = m_PartyMap.end();

    for(; pos != end; ++pos)
    {
        DeletePartyDBInfo(pos->second);
    }

    m_PartyMap.clear();
}



bool CPartyDBMgr::InsertPartyMap(unsigned long dwPartyID, CPartyDBInfo* lpParty)
{
	m_PartyMap.insert(std::make_pair(dwPartyID, lpParty)).second;
	return true;
}

bool CPartyDBMgr::DeletePartyMap(unsigned long dwPartyID)
{
	PartyMap::iterator itr = m_PartyMap.find(dwPartyID);
	if (itr == m_PartyMap.end())
    {
        return false;
    }

    itr->second->~CPartyDBInfo();
	DeletePartyDBInfo(itr->second);

	m_PartyMap.erase(dwPartyID);
	return true;
}

CPartyDBInfo* CPartyDBMgr::FindPartyMap(unsigned long dwPartyID_In)
{
	PartyMap::iterator itr = m_PartyMap.find(dwPartyID_In);
	if (itr == m_PartyMap.end())	
    {	
        return 0;	
    }
	
	return itr->second;
}

//////////////////////////////////////////////////////////////////////////////////////

CPartyDBInfo* CPartyDBMgr::OpenParty(CDBComponent& DBComponent, unsigned long dwPartyID)
{
    CPartyDBInfo* lpParty = 0;

	if (0 != dwPartyID)
	{
		lpParty = FindPartyMap(dwPartyID);

		if (0 == lpParty)
		{
			// ��Ƽ ���� ����
			PARTY_DB_INFO PartyDBInfo = {0,};			

            if (!DBComponent::GameDB::GetPartyInfo(DBComponent, dwPartyID, &PartyDBInfo)) 
			{
				ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ�� GetPartyInfo�� ȣ���ϴµ� �����߽��ϴ�.", dwPartyID);
				return 0;
			}
			
			lpParty = CreatePartyDBInfo(dwPartyID);
			if (0 == lpParty) 
			{
				ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ ���� ������ �����Ͽ����ϴ�.", dwPartyID);
				return 0;
			}

			if (!lpParty->SerializeIn(PartyDBInfo.PartyInfo, PartyDBInfo.PartyUserInfo)) 
			{
				ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ ���� Serialize�� �����Ͽ����ϴ�.", dwPartyID);
				return 0;
			}

			// ��Ƽ �ʿ� �߰�
			InsertPartyMap(dwPartyID, lpParty);
		}
	}

	return lpParty;
}

bool CPartyDBMgr::CloseParty(CDBComponent& DBComponent, unsigned long dwPartyID)
{
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ ���� ����", dwPartyID);	
		return false;
	}

	PARTY_DB_INFO PartyInfo = {0,};

	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
		return false;
	}

    if (!DBComponent::GameDB::UpdatePartyInfo(DBComponent, dwPartyID, &PartyInfo))
	{
		ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ�� UpdatePartyInfo�� ȣ���ϴµ� �����߽��ϴ�.", dwPartyID);
		return false;
	}

	RULLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �ݱ�.", dwPartyID);
	return DeletePartyMap(dwPartyID);
}

//////////////////////////////////////////////////////////////////////////////////////

CPartyDBInfo* CPartyDBMgr::CreateParty(CDBComponent& DBComponent, unsigned long dwLeaderID, char cLeaderLevel, unsigned short wLeaderClass, unsigned long dwLeaderGID,
                                       unsigned long dwMemberID, char cMemberLevel, unsigned short wMemberClass, unsigned long dwMemberGID)
{
    using namespace DataStorage;

    CSessionData* lpLeaderSessionData = 
        CSessionDataMgr::GetInstance().GetCharLoadedSession(dwLeaderID);

    CSessionData* lpMemberSessionData = 
        CSessionDataMgr::GetInstance().GetCharLoadedSession(dwMemberID);

    CCharacterData* lpLeaderCharData = 0;
    CCharacterData* lpMemberCharData = 0;

    CPartyDBInfo* lpParty = 0;

    // ���� ���� ���Ἲ �˻�.
	if (0 == lpLeaderSessionData || 0 == lpMemberSessionData) 
	{
        ERRLOG4(g_Log, "��Ƽ �α� : LeaderCID:%10u(0x%p) / MemberCID:%10u(0x%p) / "
            "��Ƽ ���� ���� : ������ �����ϴ�.", 
            dwLeaderID, lpLeaderSessionData, dwMemberID, lpMemberSessionData);
	}
    else if(CSessionData::SE_CHAR_ENABLED != lpLeaderSessionData->GetSessionState() || 
        CSessionData::SE_CHAR_ENABLED != lpMemberSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "��Ƽ �α� : LeaderCID:%10u(ST:%s) / MemberCID:%10u(ST:%s) / "
            "��Ƽ ���� ���� : ���� ���°� Ȱ��ȭ���� �ʾҽ��ϴ�.", 
            dwLeaderID, g_szSessionStateString[lpLeaderSessionData->GetSessionState()], 
            dwMemberID, g_szSessionStateString[lpMemberSessionData->GetSessionState()]);
    }
    else if(0 == (lpLeaderCharData = lpLeaderSessionData->GetCharacterData()) ||
        0 == (lpMemberCharData = lpMemberSessionData->GetCharacterData()))
    {
        ERRLOG4(g_Log, "��Ƽ �α� : LeaderCID:%10u(CharData:0x%p) / MemberCID:%10u(CharData:0x%p) / "
            "��Ƽ ���� ���� : ĳ���� �����Ͱ� �����ϴ�.", 
            dwLeaderID, lpLeaderCharData, dwMemberID, lpMemberCharData);;
    }
    else
    {
	    PARTY_DB_INFO PartyInfo = {0,};

	    PartyInfo.PartyInfo.MemberCID[0]	= lpLeaderCharData->GetCID();
		PartyInfo.PartyUserInfo.m_cLevel[0]	= cLeaderLevel;
		PartyInfo.PartyUserInfo.m_wClass[0]	= wLeaderClass;
		PartyInfo.PartyUserInfo.m_dwGID[0]	= dwLeaderGID;
		strncpy(PartyInfo.PartyInfo.Name[0], lpLeaderCharData->GetName(), CHAR_INFOST::MAX_NAME_LEN);

	
	    PartyInfo.PartyInfo.MemberCID[1] = lpMemberCharData->GetCID();    
		PartyInfo.PartyUserInfo.m_cLevel[1]	= cMemberLevel;
		PartyInfo.PartyUserInfo.m_wClass[1]	= wMemberClass;
		PartyInfo.PartyUserInfo.m_dwGID[1]	= dwMemberGID;		
	    strncpy(PartyInfo.PartyInfo.Name[1], lpMemberCharData->GetName(), CHAR_INFOST::MAX_NAME_LEN);		
		
	    // DB ��Ƽ ����
	    unsigned long dwPartyID = 0;

        if (!DBComponent::GameDB::InsertParty(DBComponent, &PartyInfo, &dwPartyID)) 
	    {
            ERRLOG3(g_Log, "��Ƽ �α� : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "��Ƽ ���� ���� : ��Ƽ�� InsertParty�� ȣ���ϴµ� �����߽��ϴ�.", 
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else if (0 == dwPartyID)
	    {
		    ERRLOG3(g_Log, "��Ƽ �α� : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "��Ƽ ���� ���� : DB���� ��Ƽ ������ �����߽��ϴ�.",
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else if (0 == (lpParty = CreatePartyDBInfo(dwPartyID)))
        {
            ERRLOG3(g_Log, "��Ƽ �α� : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "��Ƽ ���� ���� : CPartyDBInfo���� ����",
                dwPartyID, dwLeaderID, dwMemberID);
        }
        else if (!lpParty->SerializeIn(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	    {
            DeletePartyDBInfo(lpParty);
            lpParty = 0;
		    
            ERRLOG3(g_Log, "��Ƽ �α� : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "��Ƽ ���� ���� : ��Ƽ ������ Serialize ����",
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else
        {
	        lpParty->PartyMemLogin(dwLeaderID, lpLeaderSessionData->GetServerID());
            lpParty->PartyMemLogin(dwMemberID, lpMemberSessionData->GetServerID());

	        InsertPartyMap(dwPartyID, lpParty);

            RULLOG3(g_Log, "��Ƽ �α� : PID:%10u / LeaderCID:%10u / MemberCID;%10u / "
                "��Ƽ ���� ���� ", dwPartyID, dwLeaderID, dwMemberID);
        }
    }

    return lpParty;
}

bool CPartyDBMgr::DestoryParty(CDBComponent& DBComponent, unsigned long dwPartyID)
{
	CPartyDBInfo* lpParty = FindPartyMap(dwPartyID);
	if (0 == lpParty)
	{
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �Ҹ� ���� : ��Ƽ�� �����ϴ�", dwPartyID);
		return false;
	}

	if (MIN_MEMBER_NUM != lpParty->GetPartyMemNum()) 
	{
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �Ҹ� ���� : �ο����� �ּ��ο��� �ƴմϴ�", dwPartyID);
		return false;
	}

	PARTY_DB_INFO PartyInfo = {0,};
	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �Ҹ� ���� : �����͸� ���� �� �����ϴ�", dwPartyID);
		return false;
	}

    if (!DBComponent::GameDB::DeleteParty(DBComponent, &PartyInfo, dwPartyID)) 
	{
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �Ҹ� ���� : DBȣ�� DeleteParty�� �����߽��ϴ�.", dwPartyID);
		return false;
	}

	// ĳ�� DB�� ����Ǿ� �ִ� ĳ���ʹ� PID �缳�� ���־�� �Ѵ�.	
    using namespace DataStorage;

    CCharacterData* lpMember = 0;
	for (unsigned long nCount = 0; nCount < lpParty->GetPartyMemNum(); ++nCount)
	{
		if (0 == lpParty->IsLoginedByIndex(nCount))
		{
            lpMember = static_cast<CCharacterData*>(CCharacterDataMgr::GetInstance().GetLogoutData(PartyInfo.PartyInfo.MemberCID[nCount]));

			if (0 != lpMember)
			{
				if (dwPartyID == lpMember->GetPID())
				{					
                    RULLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:10u / ��Ƽ �Ҹ� ���� : ��Ƽ �Ҹ��,  ĳ�� ĳ������ PID����", 
                        dwPartyID, PartyInfo.PartyInfo.MemberCID[nCount]);

                    lpMember->SetPID(0);
				}
			}
		}
	}

    SendPacket::DeleteParty(lpParty);

    RULLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ �Ҹ� ����", dwPartyID);
	return DeletePartyMap(dwPartyID);
}

//////////////////////////////////////////////////////////////////////////////////////

bool CPartyDBMgr::InsertPartyMember(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID, unsigned long dwGID,
										unsigned short wClass, char cLevel)
{
    using namespace DataStorage;

    CSessionData*   lpInsertMember = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCharID);
    CCharacterData* lpInsertMemberCharData = 0;
    CPartyDBInfo*   lpParty = 0;

    PARTY_DB_INFO PartyInfo = {0,};	

    if(0 == lpInsertMember)
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ��Ƽ ����� �����ϴ�",
            dwPartyID, dwCharID);

		return false;
    }
    else if(0 == (lpParty = OpenParty(DBComponent, dwPartyID)))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ĳ������ ��Ƽ ���� ����",
            dwPartyID, dwCharID);

		return false;
    }
    else if(0 == (lpInsertMemberCharData = lpInsertMember->GetCharacterData()))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ���ǿ� ĳ���� ������ ����",
            dwPartyID, dwCharID);

		return false;
    }
    else if(!lpParty->InsertPartyMem(lpInsertMember->GetServerID(), dwCharID, wClass, cLevel, dwGID, lpInsertMemberCharData->GetName()))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ��Ƽ�� ĳ���� �߰� ����",
            dwPartyID, dwCharID);

		return false;
    }
    
	if(!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ��Ƽ ������ ��� ����",
            dwPartyID, dwCharID);

		return false;
	}
    else if(!DBComponent::GameDB::InsertPartyMember(DBComponent, dwPartyID, &PartyInfo, dwCharID))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : DB�� InsertPartyMember ȣ�� ����",
            dwPartyID, dwCharID);

		return false;
    }
    else if(!SendPacket::PartyCmd(PktDD::SCmdInsertPartyMem, lpParty->GetPID(), dwCharID, dwGID,
				wClass, lpInsertMember->GetServerID(), cLevel, lpInsertMemberCharData->GetName()))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ���� : ���Ӽ����� ��Ƽ ���� ���� ����",
            dwPartyID, dwCharID);

		return false;
    }
    else
    {
        DETLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ����", dwPartyID, dwCharID);
        return true;
    }

	return false;
}

bool CPartyDBMgr::DeletePartyMember(CDBComponent& DBComponent, unsigned long dwPartyID, 
                                    unsigned long dwCharID, unsigned long dwReference)
{
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ���� : ��Ƽ ���� ����",
            dwPartyID, dwCharID);

        return false;
	}
    else if (MIN_MEMBER_NUM == lpParty->GetPartyMemNum())
	{
        // ����� ������ ��Ƽ�� ��������.
		return DestoryParty(DBComponent, dwPartyID);
	}

	unsigned long dwServerID = 0;
	unsigned long dwOldLeader = lpParty->GetLeader();
	if (!lpParty->DeletePartyMem(dwCharID, dwServerID))
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ���� : ��Ƽ ��� ���� ����",
            dwPartyID, dwCharID);

		return false;
	}

    DETLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ����", dwPartyID, dwCharID);

	PARTY_DB_INFO PartyInfo = {0,};
	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ���� : ��Ƽ ��� ������ ��� ����",
            dwPartyID, dwCharID);
        
        return false;
	}
    else if(!DBComponent::GameDB::DeletePartyMember(DBComponent, dwPartyID, &PartyInfo, dwCharID)) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ���� : DB�� DeletePartyMember ȣ�� ����",
            dwPartyID, dwCharID);
		
		return false;
	}

	// ���� üũ
    unsigned long dwLeader = lpParty->GetLeader();
	if (dwOldLeader != dwLeader)
	{
        // ��Ƽ ������ �ٲ�
		dwReference = dwLeader;
	}	

    if(!SendPacket::PartyCmd(PktDD::SCmdDeletePartyMem, lpParty->GetPID(), dwCharID, dwReference, 0, 0, 0, "����"))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ���� : ���Ӽ����� ���� �ٲ� ���� ����",
            dwPartyID, dwCharID);
    }

	if (0 == dwLeader)
	{
        // ������ ������ ��Ƽ�� �ݴ´�.
		if(CloseParty(DBComponent, dwPartyID))
        {
            DETLOG3(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / Leader:%10u / ��Ƽ ��� ���� : ��Ƽ ������ ��� ��Ƽ�� �ݽ��ϴ�.",
                dwPartyID, dwCharID, dwLeader);
        }
    }

    return true;
}

bool CPartyDBMgr::AutoRoutingOn(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID)
{
	// CharID �� 0 �̸� ALL
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� ���� ON ���� : ��Ƽ�� �����ϴ�",
            dwPartyID, dwCharID);	
		return false;
	}
	
	if (!lpParty->AutoRouting(dwCharID, true, (0 == dwCharID) ? true : false))
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� ���� ON ���� : ���� ���� �ѱ� ���� (CID�� 0�̸� ���)",
            dwPartyID, dwCharID);	

        return false;
	}

    return SendPacket::PartyCmd(PktDD::SCmdAutoRoutingOn, lpParty->GetPID(), dwCharID, 0, 0, 0, 0, 0);	
}

bool CPartyDBMgr::AutoRoutingOff(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID)
{
	// CharID �� 0 �̸� ALL
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� ���� OFF ���� : ��Ƽ�� �����ϴ�",
            dwPartyID, dwCharID);	
		return false;
	}
	
	if (false == lpParty->AutoRouting(dwCharID, false, (0 == dwCharID) ? true : false))
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� ���� OFF ���� : ���� ���� �ѱ� ���� (CID�� 0�̸� ���)",
            dwPartyID, dwCharID);	

        return false;
	}

    return SendPacket::PartyCmd(PktDD::SCmdAutoRoutingOff, lpParty->GetPID(), dwCharID, 0, 0, 0, 0, 0);	
}

void CPartyDBMgr::LoginPartyMember(CPartyDBInfo* lpParty, unsigned long dwUserID, unsigned long dwCharID, unsigned long dwGID, 
                                   unsigned short wClass, unsigned long dwServerID, char cLevel)
{
	if (0 == lpParty) 
	{
        ERRLOG3(g_Log, "��Ƽ �α� : UID:%10u / CID:%10u / ServerID:0x%08X / "
            "��Ƽ ��� �α��� ���� : ��Ƽ�� �����ϴ�", dwUserID, dwCharID, dwServerID);		
	}
    else if (!lpParty->IsMember(dwCharID))
	{
        ERRLOG4(g_Log, "��Ƽ �α� : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
            "��Ƽ ��� �α��� ���� : ��Ƽ ����� �ƴմϴ�", 
            lpParty->GetPID(), dwUserID, dwCharID, dwServerID);		
	}
    else
    {
        unsigned long dwPartyID	= lpParty->GetPID();

        // ���� DB���� ���� Ȯ��

        using namespace DataStorage;

        CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCharID);
        CCharacterData* lpCharacterData = 0;

        if (0 == dwPartyID || 0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
        {
            ERRLOG6(g_Log, "��Ƽ �α� : PID:%10u / UID:%10u / CID:%10u(Session:0x%p/Character:0x%p) / ServerID:0x%08X / "
                "��Ƽ ��� �α��� ���� : ���� Ȥ�� ĳ���� �����Ͱ� �����ϴ�", 
                dwPartyID, dwUserID, dwCharID, lpSessionData, lpCharacterData, dwServerID);		
        }
        else if (lpCharacterData->GetPID() != dwPartyID)
        {
            ERRLOG5(g_Log, "��Ƽ �α� : PID:%10u / CharacterDataPID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                "��Ƽ ��� �α��� ���� : ��û PID�� ĳ���� PID�� �ٸ��ϴ�",
                dwPartyID, lpCharacterData->GetPID(), dwUserID, dwCharID, dwServerID);
        }
        else if (!lpParty->PartyMemLogin(dwCharID, dwServerID))
        {
            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID,
                DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                ERRLOG4(g_Log, "��Ƽ �α� : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                    "��Ƽ ��� �α��� ���� : ��Ƽ �α��� ����", 
                    dwPartyID, dwUserID, dwCharID, dwServerID);		                				

                SendPacket::PartyCmd(lpGameDispatch->GetSendStream(), PktDD::SCmdLoginPartyMem, 
                    dwPartyID, dwCharID, dwGID, wClass, dwServerID, cLevel, lpCharacterData->GetName(), 2);
            }
        }
        else
        {
            DETLOG4(g_Log, "��Ƽ �α� : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                "��Ƽ ��� �α��� ����", dwPartyID, dwUserID, dwCharID, dwServerID);		

			CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(CDBSingleObject::GetInstance(), dwPartyID);

			if(lpParty)
			{
				// ĳ���� ���� ������Ʈ //
				lpParty->SetUserInfoLevel(dwCharID, cLevel);
				lpParty->SetUserInfoClass(dwCharID, wClass);
				lpParty->SetUserInfoGID(dwCharID, dwGID);
			}

            SendPacket::PartyCmd(PktDD::SCmdLoginPartyMem, lpParty->GetPID(), 
                dwCharID, dwGID, wClass, dwServerID, cLevel, lpCharacterData->GetName());
        }
    }
}

void CPartyDBMgr::LogoutPartyMember(CDBComponent& DBComponent, CPartyDBInfo* lpParty, unsigned long dwCharID)
{
	if (0 == lpParty)
	{
        ERRLOG1(g_Log, "��Ƽ �α� : CID:%10u / ��Ƽ ��� �α׾ƿ� ���� : ��Ƽ ���� ����", dwCharID);		
	}
    else if (!lpParty->IsMember(dwCharID))
	{
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �α׾ƿ� ���� : ��Ƽ���� �ƴմϴ�", 
            lpParty->GetPID(), dwCharID);

    }
    else
    {
	    unsigned long dwPartyID     = lpParty->GetPID();
	    unsigned long dwOldLeader   = lpParty->GetLeader();

	    if (!lpParty->PartyMemLogout(dwCharID))
	    {
            ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �α׾ƿ� ���� : ��� �α׾ƿ� ����",
                dwPartyID, dwCharID);		    
	    }
        else
        {
	        unsigned long dwLeader = lpParty->GetLeader();
	        unsigned long dwReference = 0;
	        if (dwOldLeader != dwLeader)
	        {
		        dwReference = dwLeader;
	        }

            DETLOG3(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / NewLeader:%10u / ��Ƽ ��� �α׾ƿ� ����",
                dwPartyID, dwCharID, dwReference);

            SendPacket::PartyCmd(PktDD::SCmdLogoutPartyMem, lpParty->GetPID(), dwCharID, dwReference, 0, 0, 0, 0);

	        if (0 == dwLeader)
	        {
                // ��Ƽ ������ ��� ��Ƽ�� �������ϴ�.
                if(CloseParty(DBComponent, dwPartyID))
                {
                    DETLOG3(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / Leader:%10u / ��Ƽ ��� �α׾ƿ� : ��Ƽ ������ ��� ��Ƽ�� �ݽ��ϴ�.",
                        dwPartyID, dwCharID, dwLeader);
                }
            }
        }
    }
}

void CPartyDBMgr::SendToGameServerPartyData(CSendStream& SendStream)
{
	// ��Ƽ������ �����ش�. //]		
	PartyMap::iterator start	= m_PartyMap.begin();
	PartyMap::iterator end		= m_PartyMap.end();

	for(; start!=end; ++start)
	{
		// ��Ƽ ������ ������ �����ش� //
		CPartyDBInfo* lpPartyDBInfo = start->second;

		if(lpPartyDBInfo)
		{
			// ��Ƽ ������ ������ //

			PARTY sParty = lpPartyDBInfo->GetParty();
			
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktPIDAck));

			PktPIDAck* lppktPIDAck = reinterpret_cast<PktPIDAck*>(lpBuffer);

			memset(lppktPIDAck, 0, sizeof(PktPIDAck));

			lppktPIDAck->m_wCmd		= PktDD::SCmdGetPartyInfo;
			lppktPIDAck->m_Party	= lpPartyDBInfo->GetParty();

			SendStream.WrapHeader(sizeof(PktPIDAck), CmdAgentParty, 0, PktPMD::NO_SERVER_ERR);

			// ��Ƽ�� ������ ������ //
			PARTY_USERINFO sPartyUserInfo = lpPartyDBInfo->GetPartyUserInfo();

			for(unsigned char cIndex = 0; cIndex < PARTY::MAX_MEM; cIndex++)
			{
				// �ٸ� ���� ������ ���ӵǾ��ִ� ĳ���� //
				if(sParty.MemberCID[cIndex] && sParty.ServerID[cIndex])
				{
					char* lpBuffer = SendStream.GetBuffer(sizeof(PktPMD));

					if(0 != lpBuffer)
					{
						PktPMD* lpPktPMD = reinterpret_cast<PktPMD*>(lpBuffer);

						lpPktPMD->m_wCmd            = PktDD::SCmdLoginPartyMem;
						lpPktPMD->m_dwPartyID       = sParty.m_dwPartyID;
						lpPktPMD->m_dwSenderID      = sParty.MemberCID[cIndex];
						lpPktPMD->m_dwGID			= sPartyUserInfo.m_dwGID[cIndex];
						lpPktPMD->m_wClass			= sParty.m_wClass[cIndex];
						lpPktPMD->m_dwServerID		= sParty.ServerID[cIndex];
						lpPktPMD->m_cLevel			= sParty.m_cLevel[cIndex];

						if(sParty.Name[cIndex])
						{
							strncpy(lpPktPMD->m_strSenderName, sParty.Name[cIndex], CHAR_INFOST::MAX_NAME_LEN);
							lpPktPMD->m_strSenderName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
						}
						else
						{
							memset(lpPktPMD->m_strSenderName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
						}

						SendStream.WrapHeader(sizeof(PktPMD), CmdAgentParty, 0, PktPMD::NO_SERVER_ERR);
					}					
				}
			}
		}
	}
}
}

