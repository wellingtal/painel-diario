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
			// 파티 정보 생성
			PARTY_DB_INFO PartyDBInfo = {0,};			

            if (!DBComponent::GameDB::GetPartyInfo(DBComponent, dwPartyID, &PartyDBInfo)) 
			{
				ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티의 GetPartyInfo을 호출하는데 실패했습니다.", dwPartyID);
				return 0;
			}
			
			lpParty = CreatePartyDBInfo(dwPartyID);
			if (0 == lpParty) 
			{
				ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 정보 생성에 실패하였습니다.", dwPartyID);
				return 0;
			}

			if (!lpParty->SerializeIn(PartyDBInfo.PartyInfo, PartyDBInfo.PartyUserInfo)) 
			{
				ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 정보 Serialize에 실패하였습니다.", dwPartyID);
				return 0;
			}

			// 파티 맵에 추가
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
        ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 열기 실패", dwPartyID);	
		return false;
	}

	PARTY_DB_INFO PartyInfo = {0,};

	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
		return false;
	}

    if (!DBComponent::GameDB::UpdatePartyInfo(DBComponent, dwPartyID, &PartyInfo))
	{
		ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티의 UpdatePartyInfo를 호출하는데 실패했습니다.", dwPartyID);
		return false;
	}

	RULLOG1(g_Log, "파티 로그 : PID:%10u / 파티 닫기.", dwPartyID);
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

    // 얻어온 세션 무결성 검사.
	if (0 == lpLeaderSessionData || 0 == lpMemberSessionData) 
	{
        ERRLOG4(g_Log, "파티 로그 : LeaderCID:%10u(0x%p) / MemberCID:%10u(0x%p) / "
            "파티 생성 에러 : 세션이 없습니다.", 
            dwLeaderID, lpLeaderSessionData, dwMemberID, lpMemberSessionData);
	}
    else if(CSessionData::SE_CHAR_ENABLED != lpLeaderSessionData->GetSessionState() || 
        CSessionData::SE_CHAR_ENABLED != lpMemberSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "파티 로그 : LeaderCID:%10u(ST:%s) / MemberCID:%10u(ST:%s) / "
            "파티 생성 에러 : 세션 상태가 활성화되지 않았습니다.", 
            dwLeaderID, g_szSessionStateString[lpLeaderSessionData->GetSessionState()], 
            dwMemberID, g_szSessionStateString[lpMemberSessionData->GetSessionState()]);
    }
    else if(0 == (lpLeaderCharData = lpLeaderSessionData->GetCharacterData()) ||
        0 == (lpMemberCharData = lpMemberSessionData->GetCharacterData()))
    {
        ERRLOG4(g_Log, "파티 로그 : LeaderCID:%10u(CharData:0x%p) / MemberCID:%10u(CharData:0x%p) / "
            "파티 생성 에러 : 캐릭터 데이터가 없습니다.", 
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
		
	    // DB 파티 생성
	    unsigned long dwPartyID = 0;

        if (!DBComponent::GameDB::InsertParty(DBComponent, &PartyInfo, &dwPartyID)) 
	    {
            ERRLOG3(g_Log, "파티 로그 : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "파티 생성 에러 : 파티의 InsertParty을 호출하는데 실패했습니다.", 
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else if (0 == dwPartyID)
	    {
		    ERRLOG3(g_Log, "파티 로그 : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "파티 생성 에러 : DB에서 파티 생성에 실패했습니다.",
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else if (0 == (lpParty = CreatePartyDBInfo(dwPartyID)))
        {
            ERRLOG3(g_Log, "파티 로그 : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "파티 생성 에러 : CPartyDBInfo생성 실패",
                dwPartyID, dwLeaderID, dwMemberID);
        }
        else if (!lpParty->SerializeIn(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	    {
            DeletePartyDBInfo(lpParty);
            lpParty = 0;
		    
            ERRLOG3(g_Log, "파티 로그 : PID:%10u / LeaderCID:%10u / MemberCID:%10u / "
                "파티 생성 에러 : 파티 데이터 Serialize 실패",
                dwPartyID, dwLeaderID, dwMemberID);
	    }
        else
        {
	        lpParty->PartyMemLogin(dwLeaderID, lpLeaderSessionData->GetServerID());
            lpParty->PartyMemLogin(dwMemberID, lpMemberSessionData->GetServerID());

	        InsertPartyMap(dwPartyID, lpParty);

            RULLOG3(g_Log, "파티 로그 : PID:%10u / LeaderCID:%10u / MemberCID;%10u / "
                "파티 생성 성공 ", dwPartyID, dwLeaderID, dwMemberID);
        }
    }

    return lpParty;
}

bool CPartyDBMgr::DestoryParty(CDBComponent& DBComponent, unsigned long dwPartyID)
{
	CPartyDBInfo* lpParty = FindPartyMap(dwPartyID);
	if (0 == lpParty)
	{
        ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 소멸 실패 : 파티가 없습니다", dwPartyID);
		return false;
	}

	if (MIN_MEMBER_NUM != lpParty->GetPartyMemNum()) 
	{
        ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 소멸 실패 : 인원수가 최소인원이 아닙니다", dwPartyID);
		return false;
	}

	PARTY_DB_INFO PartyInfo = {0,};
	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 소멸 실패 : 데이터를 얻어올 수 없습니다", dwPartyID);
		return false;
	}

    if (!DBComponent::GameDB::DeleteParty(DBComponent, &PartyInfo, dwPartyID)) 
	{
        ERRLOG1(g_Log, "파티 로그 : PID:%10u / 파티 소멸 실패 : DB호출 DeleteParty을 실패했습니다.", dwPartyID);
		return false;
	}

	// 캐쉬 DB에 저장되어 있는 캐릭터는 PID 재설정 해주어야 한다.	
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
                    RULLOG2(g_Log, "파티 로그 : PID:%10u / CID:10u / 파티 소멸 정보 : 파티 소멸로,  캐쉬 캐릭터의 PID지움", 
                        dwPartyID, PartyInfo.PartyInfo.MemberCID[nCount]);

                    lpMember->SetPID(0);
				}
			}
		}
	}

    SendPacket::DeleteParty(lpParty);

    RULLOG1(g_Log, "파티 로그 : PID:%10u / 파티 소멸 성공", dwPartyID);
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
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 파티 멤버가 없습니다",
            dwPartyID, dwCharID);

		return false;
    }
    else if(0 == (lpParty = OpenParty(DBComponent, dwPartyID)))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 캐릭터의 파티 열기 실패",
            dwPartyID, dwCharID);

		return false;
    }
    else if(0 == (lpInsertMemberCharData = lpInsertMember->GetCharacterData()))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 세션에 캐릭터 데이터 없음",
            dwPartyID, dwCharID);

		return false;
    }
    else if(!lpParty->InsertPartyMem(lpInsertMember->GetServerID(), dwCharID, wClass, cLevel, dwGID, lpInsertMemberCharData->GetName()))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 파티에 캐릭터 추가 실패",
            dwPartyID, dwCharID);

		return false;
    }
    
	if(!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 파티 데이터 얻기 실패",
            dwPartyID, dwCharID);

		return false;
	}
    else if(!DBComponent::GameDB::InsertPartyMember(DBComponent, dwPartyID, &PartyInfo, dwCharID))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : DB에 InsertPartyMember 호출 실패",
            dwPartyID, dwCharID);

		return false;
    }
    else if(!SendPacket::PartyCmd(PktDD::SCmdInsertPartyMem, lpParty->GetPID(), dwCharID, dwGID,
				wClass, lpInsertMember->GetServerID(), cLevel, lpInsertMemberCharData->GetName()))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 실패 : 게임서버에 파티 변경 전송 실패",
            dwPartyID, dwCharID);

		return false;
    }
    else
    {
        DETLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 추가 성공", dwPartyID, dwCharID);
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
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 실패 : 파티 열기 실패",
            dwPartyID, dwCharID);

        return false;
	}
    else if (MIN_MEMBER_NUM == lpParty->GetPartyMemNum())
	{
        // 멤버가 작으면 파티를 깨버린다.
		return DestoryParty(DBComponent, dwPartyID);
	}

	unsigned long dwServerID = 0;
	unsigned long dwOldLeader = lpParty->GetLeader();
	if (!lpParty->DeletePartyMem(dwCharID, dwServerID))
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 실패 : 파티 멤버 삭제 실패",
            dwPartyID, dwCharID);

		return false;
	}

    DETLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 성공", dwPartyID, dwCharID);

	PARTY_DB_INFO PartyInfo = {0,};
	if (!lpParty->SerializeOut(PartyInfo.PartyInfo, PartyInfo.PartyUserInfo)) 
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 실패 : 파티 멤버 데이터 얻기 실패",
            dwPartyID, dwCharID);
        
        return false;
	}
    else if(!DBComponent::GameDB::DeletePartyMember(DBComponent, dwPartyID, &PartyInfo, dwCharID)) 
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 실패 : DB에 DeletePartyMember 호출 실패",
            dwPartyID, dwCharID);
		
		return false;
	}

	// 리더 체크
    unsigned long dwLeader = lpParty->GetLeader();
	if (dwOldLeader != dwLeader)
	{
        // 파티 리더가 바뀜
		dwReference = dwLeader;
	}	

    if(!SendPacket::PartyCmd(PktDD::SCmdDeletePartyMem, lpParty->GetPID(), dwCharID, dwReference, 0, 0, 0, "삭제"))
    {
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 삭제 실패 : 게임서버로 리더 바뀜 전송 실패",
            dwPartyID, dwCharID);
    }

	if (0 == dwLeader)
	{
        // 리더가 없으면 파티를 닫는다.
		if(CloseParty(DBComponent, dwPartyID))
        {
            DETLOG3(g_Log, "파티 로그 : PID:%10u / CID:%10u / Leader:%10u / 파티 멤버 삭제 : 파티 리더가 없어서 파티를 닫습니다.",
                dwPartyID, dwCharID, dwLeader);
        }
    }

    return true;
}

bool CPartyDBMgr::AutoRoutingOn(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID)
{
	// CharID 가 0 이면 ALL
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 오토 루팅 ON 실패 : 파티가 없습니다",
            dwPartyID, dwCharID);	
		return false;
	}
	
	if (!lpParty->AutoRouting(dwCharID, true, (0 == dwCharID) ? true : false))
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 오토 루팅 ON 실패 : 오토 루팅 켜기 실패 (CID가 0이면 모두)",
            dwPartyID, dwCharID);	

        return false;
	}

    return SendPacket::PartyCmd(PktDD::SCmdAutoRoutingOn, lpParty->GetPID(), dwCharID, 0, 0, 0, 0, 0);	
}

bool CPartyDBMgr::AutoRoutingOff(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID)
{
	// CharID 가 0 이면 ALL
	CPartyDBInfo* lpParty = OpenParty(DBComponent, dwPartyID);
	if (0 == lpParty) 
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 오토 루팅 OFF 실패 : 파티가 없습니다",
            dwPartyID, dwCharID);	
		return false;
	}
	
	if (false == lpParty->AutoRouting(dwCharID, false, (0 == dwCharID) ? true : false))
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 오토 루팅 OFF 실패 : 오토 루팅 켜기 실패 (CID가 0이면 모두)",
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
        ERRLOG3(g_Log, "파티 로그 : UID:%10u / CID:%10u / ServerID:0x%08X / "
            "파티 멤버 로그인 실패 : 파티가 없습니다", dwUserID, dwCharID, dwServerID);		
	}
    else if (!lpParty->IsMember(dwCharID))
	{
        ERRLOG4(g_Log, "파티 로그 : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
            "파티 멤버 로그인 실패 : 파티 멤버가 아닙니다", 
            lpParty->GetPID(), dwUserID, dwCharID, dwServerID);		
	}
    else
    {
        unsigned long dwPartyID	= lpParty->GetPID();

        // 접속 DB에서 세션 확인

        using namespace DataStorage;

        CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCharID);
        CCharacterData* lpCharacterData = 0;

        if (0 == dwPartyID || 0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
        {
            ERRLOG6(g_Log, "파티 로그 : PID:%10u / UID:%10u / CID:%10u(Session:0x%p/Character:0x%p) / ServerID:0x%08X / "
                "파티 멤버 로그인 실패 : 세션 혹은 캐릭터 데이터가 없습니다", 
                dwPartyID, dwUserID, dwCharID, lpSessionData, lpCharacterData, dwServerID);		
        }
        else if (lpCharacterData->GetPID() != dwPartyID)
        {
            ERRLOG5(g_Log, "파티 로그 : PID:%10u / CharacterDataPID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                "파티 멤버 로그인 실패 : 요청 PID와 캐릭터 PID가 다릅니다",
                dwPartyID, lpCharacterData->GetPID(), dwUserID, dwCharID, dwServerID);
        }
        else if (!lpParty->PartyMemLogin(dwCharID, dwServerID))
        {
            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID,
                DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                ERRLOG4(g_Log, "파티 로그 : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                    "파티 멤버 로그인 실패 : 파티 로그인 실패", 
                    dwPartyID, dwUserID, dwCharID, dwServerID);		                				

                SendPacket::PartyCmd(lpGameDispatch->GetSendStream(), PktDD::SCmdLoginPartyMem, 
                    dwPartyID, dwCharID, dwGID, wClass, dwServerID, cLevel, lpCharacterData->GetName(), 2);
            }
        }
        else
        {
            DETLOG4(g_Log, "파티 로그 : PID:%10u / UID:%10u / CID:%10u / ServerID:0x%08X / "
                "파티 멤버 로그인 성공", dwPartyID, dwUserID, dwCharID, dwServerID);		

			CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(CDBSingleObject::GetInstance(), dwPartyID);

			if(lpParty)
			{
				// 캐릭터 정보 업데이트 //
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
        ERRLOG1(g_Log, "파티 로그 : CID:%10u / 파티 멤버 로그아웃 실패 : 파티 열기 실패", dwCharID);		
	}
    else if (!lpParty->IsMember(dwCharID))
	{
        ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 로그아웃 실패 : 파티원이 아닙니다", 
            lpParty->GetPID(), dwCharID);

    }
    else
    {
	    unsigned long dwPartyID     = lpParty->GetPID();
	    unsigned long dwOldLeader   = lpParty->GetLeader();

	    if (!lpParty->PartyMemLogout(dwCharID))
	    {
            ERRLOG2(g_Log, "파티 로그 : PID:%10u / CID:%10u / 파티 멤버 로그아웃 실패 : 멤버 로그아웃 실패",
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

            DETLOG3(g_Log, "파티 로그 : PID:%10u / CID:%10u / NewLeader:%10u / 파티 멤버 로그아웃 성공",
                dwPartyID, dwCharID, dwReference);

            SendPacket::PartyCmd(PktDD::SCmdLogoutPartyMem, lpParty->GetPID(), dwCharID, dwReference, 0, 0, 0, 0);

	        if (0 == dwLeader)
	        {
                // 파티 리더가 없어서 파티를 깨버립니다.
                if(CloseParty(DBComponent, dwPartyID))
                {
                    DETLOG3(g_Log, "파티 로그 : PID:%10u / CID:%10u / Leader:%10u / 파티 멤버 로그아웃 : 파티 리더가 없어서 파티를 닫습니다.",
                        dwPartyID, dwCharID, dwLeader);
                }
            }
        }
    }
}

void CPartyDBMgr::SendToGameServerPartyData(CSendStream& SendStream)
{
	// 파티정보를 보내준다. //]		
	PartyMap::iterator start	= m_PartyMap.begin();
	PartyMap::iterator end		= m_PartyMap.end();

	for(; start!=end; ++start)
	{
		// 파티 정보가 있으면 보내준다 //
		CPartyDBInfo* lpPartyDBInfo = start->second;

		if(lpPartyDBInfo)
		{
			// 파티 정보를 보낸다 //

			PARTY sParty = lpPartyDBInfo->GetParty();
			
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktPIDAck));

			PktPIDAck* lppktPIDAck = reinterpret_cast<PktPIDAck*>(lpBuffer);

			memset(lppktPIDAck, 0, sizeof(PktPIDAck));

			lppktPIDAck->m_wCmd		= PktDD::SCmdGetPartyInfo;
			lppktPIDAck->m_Party	= lpPartyDBInfo->GetParty();

			SendStream.WrapHeader(sizeof(PktPIDAck), CmdAgentParty, 0, PktPMD::NO_SERVER_ERR);

			// 파티원 정보를 보낸다 //
			PARTY_USERINFO sPartyUserInfo = lpPartyDBInfo->GetPartyUserInfo();

			for(unsigned char cIndex = 0; cIndex < PARTY::MAX_MEM; cIndex++)
			{
				// 다른 게임 서버에 접속되어있는 캐릭터 //
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

