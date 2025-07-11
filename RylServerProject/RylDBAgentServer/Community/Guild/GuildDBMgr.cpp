#include "stdafx.h"

#include <vector>
#include <atltime.h>

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/SendPacket/SendGuild.h>
#include <Network/Dispatch/GameDispatch.h>

#include <DB/DBComponent.h>
#include <DB/GuildDBComponent.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <Creature/Siege/SiegeObjectDB.h>
#include <Creature/Siege/SiegeObjectDBMgr.h>

#include <Castle/CastleDBMgr.h>
#include <Community/Guild/GuildUtil.h>

#include "GuildDB.h"
#include "GuildDBMgr.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <GameTime/GameTimeConstants.h>

using namespace Guild;

CGuildDBMgr& CGuildDBMgr::GetInstance()
{
    static CGuildDBMgr guildDBMgr;
    return guildDBMgr;
}

CGuildDBMgr::CGuildDBMgr(void)
:	CGuildMgr(), m_lpDBComponent(0)
{
}

CGuildDBMgr::~CGuildDBMgr(void)
{
}

bool CGuildDBMgr::Initialize(CDBComponent& DBComponent)
{
    m_lpDBComponent = &DBComponent;

	if (false == ReadGuildInfoDB())			{ return false; } 
	if (false == ReadGuildMemberDB())		{ return false; }
	if (false == ReadGuildRelationDB())		{ return false; }

	return true;
}

CGuildDB* CGuildDBMgr::CreateGuild(unsigned long dwMasterID, unsigned char cNation, char* szGuildName, unsigned short* wError)
{
	unsigned long dwGID = 0;

	GuildRight guildRight;
	if (0 == m_lpDBComponent || !DBComponent::GuildDB::InsertGuildInfo(*m_lpDBComponent, szGuildName, cNation, 
		reinterpret_cast<char *>(&guildRight), sizeof(GuildRight), &dwGID, wError)) 
	{
		ERRLOG1(g_Log, "길드 로그 : CID:%10u DB에서 길드 생성에 실패하였습니다.", dwMasterID);
		return NULL;
	}

	if (0 == dwGID) 
	{ 
		ERRLOG0(g_Log, "길드 생성에 실패하였습니다. 길드 아이디를 생성하지 못하였습니다.");
		return NULL; 
	}

	CGuildDB* lpGuild = new CGuildDB(*m_lpDBComponent, dwGID, cNation, szGuildName);

	if (!m_GuildMap.insert(GuildMap::value_type(dwGID, lpGuild)).second)
	{
		delete lpGuild;
		return NULL;
	}

	return lpGuild;
}

GuildMap::iterator CGuildDBMgr::DissolveGuild(unsigned long dwGID, unsigned short usDissolveReason)
{
	GuildMap::iterator it = m_GuildMap.find(dwGID);
	if (it == m_GuildMap.end()) 
	{ 
        ERRLOG1(g_Log, "GID:%10u / 길드 해체 실패 : 존재하지않는 길드를 해체하려합니다.", dwGID);
		return it; 
	}

	CGuildDB* lpGuild = static_cast<CGuildDB*>(it->second);
	for (MemberList::iterator MemberIt = lpGuild->m_MemberList.begin(); MemberIt != lpGuild->m_MemberList.end(); ++MemberIt)
	{
		MemberInfo memberInfo = *MemberIt;

        using namespace DBAgent::DataStorage;

        CSessionData* lpSessionData = 
            CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

        CCharacterData* lpCharacterData = 0;

		if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
		{
            // 세션이 없거나, 세션에 캐릭터가 연결되어 있지 않은 경우, 로그아웃 셋에서 찾아본다.
            lpCharacterData = 
                static_cast<CCharacterData*>(CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));
		}

		if (0 != lpCharacterData)
		{			
            lpCharacterData->SetGID(0);
		}

		// WORK_LIST 2.3 계정 국적 변경 기능 구현
		// 길드 파괴시 길드 멤버 탈퇴로 인한 명성 감소
//		lpGuild->AddMemberFame(Guild::TYPE_HALF, memberInfo.m_dwCID, 0);
	}

	// 삭제되는 길드와 관계가 있는 것을 모두 삭제한다.
	lpGuild->DeleteRelation();

    PktCreateGuild pktCreateGuild;
    memset(&pktCreateGuild, 0, sizeof(PktCreateGuild));
    
    pktCreateGuild.m_dwCID = 0;
    pktCreateGuild.m_dwGID = lpGuild->m_dwGID;
    pktCreateGuild.m_cInclination = lpGuild->m_cInclination;

    strncpy(pktCreateGuild.m_szGuildName, lpGuild->GetName(), Guild::MAX_GUILD_NAME_LEN);

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCreateGuild), sizeof(PktCreateGuild), 
        CmdCreateGuild, usDissolveReason, PktCreateGuild::FAIL_DISSOLVE_GUILD))
    {
        DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
            reinterpret_cast<char*>(&pktCreateGuild), sizeof(PktCreateGuild), CmdCreateGuild));
    }

    // 길드 정보 삭제
	DBComponent::GuildDB::DeleteGuildInfo(*m_lpDBComponent, lpGuild->GetGID());

	// 길드 관계 삭제
	DBComponent::GuildDB::DeleteGuildRelation(*m_lpDBComponent, lpGuild->GetGID());

	delete it->second;
	it = m_GuildMap.erase(it);

	// 해당 길드의 진지를 모두 파괴합니다.
	CSiegeObjectDBMgr::GetInstance().DestroyCamp(dwGID);

	return it;
}


bool CGuildDBMgr::SetRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType)
{
	CGuildDB* lpGuild = static_cast<CGuildDB*>(GetGuild(dwGID));
	CGuildDB* lpTargetGuild = static_cast<CGuildDB*>(GetGuild(dwTargetGID));

	if (!lpGuild || !lpTargetGuild) { return false ; }

	if(Guild::TARGET_NEUTRALITY == cRelationType)
	{
		if (!lpGuild->SetRelation(dwTargetGID, cRelationType, Guild::BY_TARGET))
		{
			return false;
		}

		return true;
	}

	// 적대길드 선택을 했는데 이미 상대방이 평화허락을 안해준 상태면 
	// 디비 상태만 업데이트 시킨다.
	unsigned char tnRelation = lpTargetGuild->GetRelation(dwGID, Guild::BY_TARGET);

	if( (Guild::HOSTILITY == cRelationType ||
		Guild::COUNTER_HOSTILITY == cRelationType || 
		Guild::ALERT_HOSTILITY == cRelationType) &&
		Guild::NEUTRALITY != tnRelation) 
	{
		// 디비에 쓰지 말고 상태만 업데이트한다.
		if (!lpGuild->SetRelation(dwTargetGID, cRelationType, Guild::BY_MINE, 0))
		{
			return false;
		}

		// 다시 수정해줘야하기 때문에 이건 맞다.
		if (!lpTargetGuild->SetRelation(dwGID, cRelationType, Guild::BY_TARGET))
		{
			return false;
		}

		return true;
	}

	if (!lpGuild->SetRelation(dwTargetGID, cRelationType, Guild::BY_MINE))
	{
		return false;
	}

	if (!lpTargetGuild->SetRelation(dwGID, cRelationType, Guild::BY_TARGET))
	{
		return false;
	}

	return true;
}

void CGuildDBMgr::CancelAlertAndCounterHostility()
{
	for (GuildMap::iterator GuildIt = m_GuildMap.begin(); GuildIt != m_GuildMap.end(); ++GuildIt)
	{
		CGuildDB* lpGuild = static_cast<CGuildDB*>(GuildIt->second);

		if (lpGuild->GetNumOfRelation(Guild::ALERT_HOSTILITY) > 0 ||
			lpGuild->GetNumOfRelation(Guild::COUNTER_HOSTILITY) > 0)
		{
			lpGuild->CancelAlertAndCounterHostility();
		}
	}
}

void CGuildDBMgr::CancelAlertHostility(unsigned long dwTargetGID)
{
	for (GuildMap::iterator GuildIt = m_GuildMap.begin(); GuildIt != m_GuildMap.end(); ++GuildIt)
	{
		CGuildDB* lpGuild = static_cast<CGuildDB*>(GuildIt->second);

		if (lpGuild->GetRelationGID(Guild::ALERT_HOSTILITY) == dwTargetGID)
		{
			lpGuild->CancelAlertHostility(dwTargetGID);
		}
	}
}

void CGuildDBMgr::ProcessGuild(void)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CTime NowTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	CTimeSpan timeSpan(0);

	for (GuildMap::iterator GuildIt = m_GuildMap.begin(); GuildIt != m_GuildMap.end(); )
	{
		CGuildDB* lpGuild = static_cast<CGuildDB*>(GuildIt->second);

/*
		// 오랫동안 로그인하지 않은 길드 해체
		if (0 != lpGuild->m_tmLastLogout.Year)
		{
			CTime GuildTime(lpGuild->m_tmLastLogout.Year, lpGuild->m_tmLastLogout.Month, lpGuild->m_tmLastLogout.Day, 
				lpGuild->m_tmLastLogout.Hour, lpGuild->m_tmLastLogout.Minute, lpGuild->m_tmLastLogout.Second);
			timeSpan = NowTime - GuildTime;
			if (timeSpan.GetTotalMinutes() > DELETE_GUILD_LOGOUT_TIME)
			{
				DETLOG1(g_Log, "GID:%10u / 길드가 해체되었습니다. - 길드원이 오랫동안 아무도 로그인하지 않은 길드", 
					lpGuild->GetGID());
                GuildIt = DissolveGuild(lpGuild->GetGID(), PktCreateGuild::NONE_LOGIN);
				continue;
			}
		}

		// 길드원이 최소 인원 미만일 때 길드 해체
		if (0 != lpGuild->m_tmCheckMember.Year && DELETE_GUILD_MEMBER_NUM > lpGuild->GetCurrentMemberNum())
		{
			CTime GuildTime(lpGuild->m_tmCheckMember.Year, lpGuild->m_tmCheckMember.Month, lpGuild->m_tmCheckMember.Day, 
				lpGuild->m_tmCheckMember.Hour, lpGuild->m_tmCheckMember.Minute, lpGuild->m_tmCheckMember.Second);
			timeSpan = NowTime - GuildTime;
			if (timeSpan.GetTotalMinutes() > MEMBER_NUM_DELETE_TIME)
			{
				DETLOG1(g_Log, "GID:%10u / 길드가 해체되었습니다. - 길드원이 최소 인원 미만인 채 일정 시간 경과", 
					lpGuild->GetGID());
                GuildIt = DissolveGuild(lpGuild->GetGID(), PktCreateGuild::MIN_MEMBER_TIMEOUT);
				continue;
			}
		}

		// 오랫동안 로그인하지 않은 길드 마스터의 권한 양도
		MemberInfo memberInfo = lpGuild->GetMaster();
		if (0 != lpGuild->m_tmGMLastLogout.Year && 0 == memberInfo.m_dwServerID)
		{
			CTime GuildTime(lpGuild->m_tmGMLastLogout.Year, lpGuild->m_tmGMLastLogout.Month, lpGuild->m_tmGMLastLogout.Day, 
				lpGuild->m_tmGMLastLogout.Hour, lpGuild->m_tmGMLastLogout.Minute, lpGuild->m_tmGMLastLogout.Second);
			timeSpan = NowTime - GuildTime;
			if (timeSpan.GetTotalMinutes() > TRANFER_MASTER_TIME)
			{
				// 길드가 삭제되는 경우를 대비한다.
				GuildMap::iterator TempGuildIt = GuildIt;
				++TempGuildIt;

				unsigned long dwNewMaster = lpGuild->SetNewMaster();
				if (0 == dwNewMaster)
				{
					DETLOG1(g_Log, "GID:%10u / 길드가 해체되었습니다. - 일정 기간동안 로그인하지 않은 길드마스터의 권한 양도", lpGuild->GetGID());
                    CGuildDBMgr::GetInstance().DissolveGuild(lpGuild->GetGID(), PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_NONE_LOGIN);
					GuildIt = TempGuildIt;
					continue;
				}

				DBAgent::SendPacket::GuildCmd(NULL, lpGuild->GetGID(), dwNewMaster, MASTER, 
					PktGuildCmd::GC_SETUP_TITLE, PktBase::NO_SERVER_ERR);

				lpGuild->SetTitle(memberInfo.m_dwCID, COMMON);

				DBAgent::SendPacket::GuildCmd(NULL, lpGuild->GetGID(), memberInfo.m_dwCID, COMMON, 
					PktGuildCmd::GC_SETUP_TITLE, PktBase::NO_SERVER_ERR);

				DETLOG3(g_Log, "GID:%10u / 길드마스터(이전CID:%10u, 현재CID:%10u)가 변경되었습니다. "
					"- 일정 기간동안 로그인하지 않은 길드마스터의 권한 양도", 
					lpGuild->GetGID(), memberInfo.m_dwCID, dwNewMaster);
			}
		}
*/
		// 길드 재정시 시간 체크.
		if(lpGuild->GetGuildPointFlag())
		{
			// 시간 검사 //
			TIME* pTime = lpGuild->GetGuildPointTime();

			if(pTime)
			{
				CTime nowTime = CTime::GetCurrentTime();
				CTimeSpan timeSpan(0);

				CTime GuildPointTime(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second);

				timeSpan = nowTime - GuildPointTime;

				if(timeSpan.GetTotalMinutes() >= Guild::TIME_GUILD_POINT)
				{
					// 메세지 보냄 //
					lpGuild->SetGuildPointFlag(0);

					PktCampMessage pktCampMessage;
					memset(&pktCampMessage, 0, sizeof(PktCampMessage));			
					pktCampMessage.m_cMsgCmd	= PktCampMessage::MSGCMD_GUILD_TIME_COMPLETE;					
					pktCampMessage.m_bNotify	= false;
					pktCampMessage.m_dwGID		= lpGuild->GetGID();

					if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage, 0, 0))
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktCampMessage), sizeof(PktCampMessage), CmdCampMessage));
					}
				}
			}
		}

		// 탈퇴 대기자들의 완전 탈퇴
		std::list<unsigned long> deleteList;

		for (MemberList::iterator MemberIt = lpGuild->m_MemberList.begin(); MemberIt != lpGuild->m_MemberList.end(); ++MemberIt)
		{
			MemberInfo memberInfo = *MemberIt;

			if (Guild::LEAVE_WAIT != memberInfo.m_MemberListInfo.m_cTitle)
			{
				continue;
			}

			if (0 != memberInfo.m_LeaveGuildTime.Year)
			{
				CTime GuildTime(memberInfo.m_LeaveGuildTime.Year, memberInfo.m_LeaveGuildTime.Month, memberInfo.m_LeaveGuildTime.Day, 
					memberInfo.m_LeaveGuildTime.Hour, memberInfo.m_LeaveGuildTime.Minute, memberInfo.m_LeaveGuildTime.Second);
				timeSpan = NowTime - GuildTime;

				unsigned long dwLeaveWaitTime = LEAVE_WAIT_TIME;

				if (timeSpan.GetTotalMinutes() > dwLeaveWaitTime)
				{
					deleteList.push_back(memberInfo.m_dwCID);
				}
			}
		}

		for (std::list<unsigned long>::iterator DeleteIt = deleteList.begin(); DeleteIt != deleteList.end(); ++DeleteIt)
		{
			using namespace DBAgent::DataStorage;
			
			CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(*DeleteIt);

			if(lpSessionData)
			{
				CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();

				if(lpCharacterData)
				{
					unsigned short wError = PktBase::NO_SERVER_ERR;

					lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);

					PktWarOnOff pktWarOnOff;
					memset(&pktWarOnOff, 0, sizeof(PktWarOnOff));

					pktWarOnOff.m_dwCID	= *DeleteIt;
					pktWarOnOff.m_cType	= GameTime::GUILD;
					pktWarOnOff.m_cFlag	= Creature::WAR_OFF;			

					if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktWarOnOff), sizeof(PktWarOnOff), CmdWarOnOff, 0, wError))						
					{
						DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
							reinterpret_cast<char*>(&pktWarOnOff), sizeof(PktWarOnOff), CmdWarOnOff));						
					}
				}
			}
			else
			{
				DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), *DeleteIt, Creature::WAR_OFF);
			}

			lpGuild->LeaveMember(*DeleteIt);

            DBAgent::SendPacket::GuildCmd(NULL, lpGuild->GetGID(), 
                *DeleteIt, 0, PktGuildCmd::GC_KICK, NULL, NULL, PktBase::NO_SERVER_ERR);
		}

		deleteList.clear();

		++GuildIt;
	}
}

bool CGuildDBMgr::ReadGuildInfoDB(void)
{	
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

    char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1,
        "SELECT nGuildID, strGuildName, tnGuildLevel, nGuildFame, nGuildGold, "
        "tLastLogout, tCheckMember, tGMLastLogout, bGuildMark, bRight, tnNationType, tChangeState "
        "FROM TblGuildInfo WHERE OldServerGroupID = %d", CServerSetup::GetInstance().GetAgentServerType());

    szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || 
        !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
        ERRLOG0(g_Log, "ReadGuildInfoDB 실패 : 길드 DB 얻어오기 실패");
		return false;
	}

	GuildInfoDB* guildInfo = new GuildInfoDB[MAX_ROWS];
	memset(guildInfo, 0, sizeof(GuildInfoDB) * MAX_ROWS);

    int nTotalGuildNum = 0;

	while (m_lpDBComponent->GetData((void**)guildInfo, sizeof(GuildInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

        nTotalGuildNum += nGetRows;

		for (GuildInfoDB* lpGuildInfo = guildInfo; nGetRows > 0; --nGetRows, ++lpGuildInfo)
		{
			CGuildDB* lpGuild = new CGuildDB(*m_lpDBComponent, *lpGuildInfo);
			if (false == m_GuildMap.insert(GuildMap::value_type(lpGuild->GetGID(), lpGuild)).second)
			{
				delete lpGuild;
				continue;
			}
		}
        
		memset(guildInfo, 0, sizeof(GuildInfoDB) * MAX_ROWS);
	}

    INFLOG1(g_Log, "길드 : %d 개를 읽어왔습니다", nTotalGuildNum);

	delete [] guildInfo;
	return true;	
}

bool CGuildDBMgr::ReadGuildMemberDB(void)
{
    char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1,
        "SELECT TblGuildMember.nGuildID, TblGuildMember.nCID, "
        "TblGuildMember.nPositionInGuild, CharInfo.Name, "
        "TblGuildMember.nPosition, CharInfo.Level, CharInfo.Class, "
        "CharInfo.Fame, CharInfo.Gold, CharInfoEx.GuildWarFlag, TblGuildMember.tLeaveGuildTime, TblGuildMember.Tactics "
        "FROM TblGuildInfo, TblGuildMember, CharInfo, CharInfoEx WHERE "
        "TblGuildInfo.nGuildID = TblGuildMember.nGuildID AND "
        "TblGuildMember.nCID = CharInfo.CID AND "
		"TblGuildMember.nCID = CharInfoEx.CID AND "
        "TblGuildInfo.OldServerGroupID = %d", CServerSetup::GetInstance().GetAgentServerType());

    szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
        ERRLOG0(g_Log, "ReadGuildMemberDB 실패 : 길드 멤버 DB 얻어오기 실패");
		return false;
	}

    typedef std::pair<GuildMemberDB*, int>  GuildMemberDBRows;
    typedef std::vector<GuildMemberDBRows>  VecGuildMemberDB;

    VecGuildMemberDB    vecGuildMemberDB;

    const int MAX_ROWS = 4096;
    int nGetRows = 0;
    int nTotalGuildMemberNum = 0;

	GuildMemberDB* guildMember = new GuildMemberDB[MAX_ROWS];
	memset(guildMember, 0, sizeof(GuildMemberDB) * MAX_ROWS);
    
	while (m_lpDBComponent->GetData((void**)guildMember, sizeof(GuildMemberDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

        nTotalGuildMemberNum += nGetRows;

        vecGuildMemberDB.push_back(GuildMemberDBRows(guildMember, nGetRows));
        
        guildMember = new GuildMemberDB[MAX_ROWS];
        memset(guildMember, 0, sizeof(GuildMemberDB) * MAX_ROWS);
	}

    if (0 != guildMember)
    {
        delete [] guildMember;
        guildMember = 0;
    }

    VecGuildMemberDB::iterator pos = vecGuildMemberDB.begin();
    VecGuildMemberDB::iterator end = vecGuildMemberDB.end();

    CGuild* lpGuild = 0;

    for(; pos != end; ++pos)
    {
        GuildMemberDB* lpGuildPos = pos->first;
        GuildMemberDB* lpGuildEnd = lpGuildPos + pos->second;

        for(; lpGuildPos != lpGuildEnd; ++lpGuildPos)
        {
            if (NULL == lpGuild || lpGuild->GetGID() != lpGuildPos->m_dwGID)
            {
                lpGuild = static_cast<CGuildDB*>(GetGuild(lpGuildPos->m_dwGID));
                if (NULL == lpGuild)
                {
                    ERRLOG2(g_Log, "GID:%10u / CID:%10u / 길드 멤버 DB읽기 실패 : 존재하지않는 길드에 속한 멤버입니다", 
                        lpGuildPos->m_dwGID, lpGuildPos->m_dwCID);
                    continue;
                }
            }

            if(!lpGuild->JoinMemberDB(*lpGuildPos))
            {
                ERRLOG3(g_Log, "GID:%10u / CID:%10u / Name:%s / DB에서 읽어온 길드 멤버를 Join하는 데 실패했씁니다",
                    lpGuildPos->m_dwGID, lpGuildPos->m_dwCID, lpGuildPos->m_strName);
            }
        }

        delete [] pos->first;
        *pos = GuildMemberDBRows((GuildMemberDB*)0, 0);
    }

    INFLOG1(g_Log, "길드 멤버 : %d 명을 읽어왔습니다", nTotalGuildMemberNum);

	return true;	
}

bool CGuildDBMgr::ReadGuildRelationDB(void)
{
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1,
		"SELECT TblGuildRelation.nGuildID, TblGuildRelation.nTargetGuildID, TblGuildRelation.tnRelation "
		"FROM TblGuildInfo, TblGuildRelation WHERE "
		"TblGuildInfo.nGuildID = TblGuildRelation.nGuildID AND TblGuildInfo.OldServerGroupID = %d", 
		CServerSetup::GetInstance().GetAgentServerType());

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "ReadGuildRelationDB 실패 : 길드 관계 DB 얻어오기 실패");
		return false;
	}

	GuildRelationDB* guildRelation = new GuildRelationDB[MAX_ROWS];
	memset(guildRelation, 0, sizeof(GuildRelationDB) * MAX_ROWS);

	int nTotalGuildRelationNum = 0;
	CGuildDB* lpGuild = NULL;
	CGuildDB* lpTargetGuild = NULL;

	while (m_lpDBComponent->GetData((void**)guildRelation, sizeof(GuildRelationDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		nTotalGuildRelationNum += nGetRows;

		for (GuildRelationDB* lpGuildRelation = guildRelation; nGetRows > 0; --nGetRows, ++lpGuildRelation)
		{
			lpGuild = static_cast<CGuildDB*>(GetGuild(lpGuildRelation->m_dwGID));
			if (NULL == lpGuild)
			{
				ERRLOG2(g_Log, "GID:%10u / TargetGID:%10u / 존재하지않는 길드가 길드간 관계를 맺으려 합니다. ", 
						lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID);

				continue;
			}

			lpTargetGuild = static_cast<CGuildDB*>(GetGuild(lpGuildRelation->m_dwTargetGID));
			if (NULL == lpTargetGuild)
			{
				ERRLOG2(g_Log, "GID:%10u / TargetGID:%10u / 존재하지않는 길드와 길드간 관계를 맺으려 합니다. ", 
					lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID);

				continue;
			}

			INFLOG3(g_Log, "GID:%10u / TargetGID:%10u / Relation:%d 길드 관계 로드",
				lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID, lpGuildRelation->m_cRelation);

			// TARGET_NEUTRALITY 타입이면 TargetGuild에서 평화선언을 해야 사라지는것이기 때문에
			// 가해자한테는 넣어주지 않는다.
			if(lpGuildRelation->m_cRelation != Guild::TARGET_NEUTRALITY)
				lpGuild->InsertRelation(lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID, lpGuildRelation->m_cRelation);

			lpTargetGuild->InsertRelation(lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID, lpGuildRelation->m_cRelation);
		}

		memset(guildRelation, 0, sizeof(GuildRelationDB) * MAX_ROWS);
	}

	INFLOG1(g_Log, "길드 관계 : %d 개를 읽어왔습니다", nTotalGuildRelationNum);

	delete [] guildRelation;	
	return true;
}


void CGuildDBMgr::SendGuildDBList(CSendStream& SendStream)
{
	// 길드 DB 패킷 + 길드 정보 + 멤버 정보 + 길드관계정보(자신이 맺은) + 길드관계정보(다른길드로부터)
    const unsigned short MAX_GUILD_BUFFER_SIZE = sizeof(PktGuildDB) + sizeof(GuildInfoDB) + 
												 sizeof(GuildMemberDB) * MAX_MEMBER_NUM +
												 sizeof(GuildRelationDB) * MAX_RELATION_NUM * 2;

    char szBuffer[MAX_GUILD_BUFFER_SIZE];
    unsigned short wBufferSize = 0;

    unsigned char cTotalMemberNum = 0;
	unsigned char cRelationByMineNum = 0;
	unsigned char cRelationByTargetNum = 0;

    for (GuildMap::iterator it = m_GuildMap.begin(); it != m_GuildMap.end(); ++it)
    {
        CGuild* lpGuild = it->second;
        if (NULL == lpGuild)
        {
            ERRLOG0(g_Log, "NULL 길드가 포함되어 있습니다.");
            return;
        }
        
        SerializeOut(lpGuild, szBuffer + sizeof(PktGuildDB), 
            wBufferSize, cTotalMemberNum, cRelationByMineNum, cRelationByTargetNum);

        PktGuildDB* lpGuildDB = reinterpret_cast<PktGuildDB*>(szBuffer);

        lpGuildDB->m_bStartFlag = (it == m_GuildMap.begin()) ? true : false;

        lpGuildDB->m_dwGID = lpGuild->GetGID();

        lpGuildDB->m_cTotalMemberNum = cTotalMemberNum;
		lpGuildDB->m_cRelationByMineNum = cRelationByMineNum;
		lpGuildDB->m_cRelationByTargetNum = cRelationByTargetNum;

        lpGuildDB->m_wSize = wBufferSize;

        SendStream.WrapCompress(szBuffer, sizeof(PktGuildDB) + wBufferSize, 
            CmdGuildDB, 0, PktBase::NO_SERVER_ERR);
    }    
}

void CGuildDBMgr::KickAllWaitMember()
{
	GuildMap::iterator itr = m_GuildMap.begin() ;
	GuildMap::iterator end = m_GuildMap.end() ;
	CGuildDB* lpGuild = NULL ;

	while ( itr != end )
	{
		lpGuild = static_cast<CGuildDB*>( itr->second ) ;
		if ( lpGuild )
		{
			lpGuild->KickAllWaitMember() ;
		}

		++itr ;
	}
}
