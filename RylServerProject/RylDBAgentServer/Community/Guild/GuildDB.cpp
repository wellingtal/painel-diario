#include "stdafx.h"

#include <list>
#include <vector>
#include <algorithm>
#include <numeric>

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/SendPacket/SendGuild.h>
#include <Network/Dispatch/GameDispatch.h>

#include <DB/DBComponent.h>
#include <DB/GuildDBComponent.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Creature/Siege/SiegeObjectDBMgr.h>
#include <Community/Guild/GuildUtil.h>

#include "GuildDBMgr.h"
#include "GuildDB.h"

CGuildDB::CGuildDB(CDBComponent& DBComponent)
:	CGuild(), m_DBComponent(DBComponent)
{
}

CGuildDB::CGuildDB(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cNation, char* szName)
:	CGuild(dwGID, cNation, szName), m_DBComponent(DBComponent)
{
}

CGuildDB::CGuildDB(CDBComponent& DBComponent, GuildInfoDB& guildInfo)
:	CGuild(guildInfo), m_DBComponent(DBComponent)
{
}

CGuildDB::~CGuildDB(void)
{
}


bool CGuildDB::SetMark(char* szMark)
{
	::memcpy(m_szMark, szMark, MAX_MARK_SIZE);
	DeductGold(Guild::SET_MARK_GOLD);

	return DBComponent::GuildDB::UpdateGuildMark(m_DBComponent, m_dwGID, m_szMark, MAX_MARK_SIZE, m_dwGold);
}

bool CGuildDB::SetLevel(unsigned char cLevel)
{
	m_cLevel = cLevel;

	unsigned long dwSetLevelGold = ms_arySetLevelGold[cLevel];

	DeductGold(dwSetLevelGold);

	return DBComponent::GuildDB::UpdateGuildLevel(m_DBComponent, m_dwGID, m_cLevel, m_dwGold);
}

// 더이상 길드 국적은 변경할 수 없습니다. (2004-11-22 by 로딘)
/*
bool CGuildDB::SetInclination(unsigned char cInclination)
{
	m_cInclination = cInclination;

	SYSTEMTIME systemTime;
	::memset(&systemTime, 0, sizeof(SYSTEMTIME));

	GetLocalTime(&systemTime);

	m_tmChangeInclination.Year = systemTime.wYear;
	m_tmChangeInclination.Month = systemTime.wMonth;
	m_tmChangeInclination.Day = systemTime.wDay;
	m_tmChangeInclination.Hour = systemTime.wHour;
	m_tmChangeInclination.Minute = systemTime.wMinute;
	m_tmChangeInclination.Second = systemTime.wSecond;
	m_tmChangeInclination.MSecond = systemTime.wMilliseconds;

	// 해당 길드의 진지를 모두 파괴한다.
	int nDestroyCampNum = CSiegeObjectDBMgr::GetInstance().DestroyCamp( m_dwGID ) ;

	// 명성치 변경
	if (nDestroyCampNum > 0)
	{
		AddAllMemberFame( Guild::TYPE_VALUE, nDestroyCampNum * Siege::FAME_FOR_DESTROYED_CAMP ) ;
	}

	if (RACE_BOTH != (m_cInclination & RACE_BIT))
	{
		std::list<unsigned long> deleteList;

		for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
		{
			MemberInfo memberInfo = *it;
			CClass::RaceType eRace = CClass::GetRace(memberInfo.m_MemberListInfo.m_cClass);

			// 종족이 길드 성향과 다르면 강제 탈퇴 당한다.
			if ((RACE_HUMAN == (m_cInclination & RACE_BIT) && CClass::AKHAN == eRace) || 
				(RACE_AKHAN == (m_cInclination & RACE_BIT) && CClass::HUMAN == eRace))
			{
				deleteList.push_back(memberInfo.m_dwCID);
			}
		}

		for (std::list<unsigned long>::iterator DeleteIt = deleteList.begin(); DeleteIt != deleteList.end(); ++DeleteIt)
		{
			LeaveMember(*DeleteIt);
		}

		deleteList.clear();
	}

	// WORK_LIST 2.3 계정 국적 변경 기능 구현
	// 국적 변경으로 인한 명성치 감소
//	AddAllMemberFame( Guild::TYPE_HALF, 0 );

	return DBComponent::GuildDB::UpdateGuildInclination(m_DBComponent, m_dwGID, m_cInclination);
}
*/

bool CGuildDB::SetRight(GuildRight guildRight)
{
	m_GuildRight = guildRight;

	return DBComponent::GuildDB::UpdateGuildRight(m_DBComponent, m_dwGID, 
		reinterpret_cast<char *>(&guildRight), sizeof(GuildRight));
}

bool CGuildDB::SetTitle(unsigned long dwCID, unsigned char cTitle)
{
/*	// 테스트
	{
		int iTest = 0;
		for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
		{
			MemberInfo memberInfo = *it;
			if (JOIN_WAIT == memberInfo.m_MemberListInfo.m_cTitle && 0 == memberInfo.m_cTactics)
			{
				iTest++;
			}
		}
	}
*/

	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end()) 
	{ 
		return E_NOT_MEMBER; 
	}

	MemberInfo& memberInfo = *it;

	SYSTEMTIME systemTime;
	::memset(&systemTime, 0, sizeof(SYSTEMTIME));

	memberInfo.m_MemberListInfo.m_cTitle = cTitle;

	// 탈퇴 신청
	if (LEAVE_WAIT == cTitle)
	{
		GetLocalTime(&systemTime);

		memberInfo.m_LeaveGuildTime.Year = systemTime.wYear;
		memberInfo.m_LeaveGuildTime.Month = systemTime.wMonth;
		memberInfo.m_LeaveGuildTime.Day = systemTime.wDay;
		memberInfo.m_LeaveGuildTime.Hour = systemTime.wHour;
		memberInfo.m_LeaveGuildTime.Minute = systemTime.wMinute;
		memberInfo.m_LeaveGuildTime.Second = systemTime.wSecond;
		memberInfo.m_LeaveGuildTime.MSecond = systemTime.wMilliseconds;
	}
	else
	{
		::memset(&memberInfo.m_LeaveGuildTime, 0, sizeof(TIME));
	}

	return DBComponent::GuildDB::UpdateMemberTitle(m_DBComponent, dwCID, cTitle, systemTime);
}

bool CGuildDB::SetRelation(unsigned long dwTargetGID, unsigned char cRelationType, unsigned char cActorType, unsigned char cInsert)
{
	switch (cActorType)
	{
		// 내가 대상 길드에 선언
		case Guild::BY_MINE:
		{
			switch (cRelationType)
			{
				case Guild::HOSTILITY:
				case Guild::ALERT_HOSTILITY:
				case Guild::COUNTER_HOSTILITY:
				{
					unsigned char cOldRelation = GetRelation(dwTargetGID);
					if (Guild::NEUTRALITY == cOldRelation || Guild::MINE == cOldRelation)
					{
						if(cInsert == 0)
						{
							// 업데이트
							if (!DBComponent::GuildDB::UpdateGuildRelation(m_DBComponent, m_dwGID, dwTargetGID, cRelationType))
							{
								ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 업데이트 실패",
										m_dwGID, dwTargetGID, cRelationType);
								return false;
							}
						}
						else
						{
							// 추가
							if (!DBComponent::GuildDB::InsertGuildRelation(m_DBComponent, m_dwGID, dwTargetGID, cRelationType))
							{
								ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 추가 실패",
										m_dwGID, dwTargetGID, cRelationType);
								return false;
							}
						}

						RelationMap::iterator itr = m_Relation[Guild::BY_MINE].find(dwTargetGID);
						if (itr != m_Relation[Guild::BY_MINE].end())
						{
							itr->second = cRelationType;
						}
						else
						{
							if (!m_Relation[Guild::BY_MINE].insert(std::make_pair(dwTargetGID, cRelationType)).second)
							{
								ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 추가 실패",
										m_dwGID, dwTargetGID, cRelationType);
								return false;
							}
						}
					}
					else
					{
						// 업데이트
						if (!DBComponent::GuildDB::UpdateGuildRelation(m_DBComponent, m_dwGID, dwTargetGID, cRelationType))
						{
							ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 업데이트 실패",
									m_dwGID, dwTargetGID, cRelationType);
							return false;
						}

						RelationMap::iterator itr = m_Relation[Guild::BY_MINE].find(dwTargetGID);
						if (itr != m_Relation[Guild::BY_MINE].end())
						{
							itr->second = cRelationType;
						}
						else
						{
							ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 업데이트 실패 (길드 관계를 찾을수 없음)",
									m_dwGID, dwTargetGID, cRelationType);
							return false;
						}
					}

					break;
				}

				case Guild::NEUTRALITY:
				{
					// A->B에게 일방적으로 적대선언을 한다음
					// 일방적으로 A->B에게 평화선언을 했으면.. DB는 5로 지워준다.
					// 그리고 나자신한테는 제거해준다.

					// 정보 업데이트
					// 이제 정보상태가 5로 바꼈기때문에 TargetGuild가 평화선언을 해주지 않으면 안된다.
					if (!DBComponent::GuildDB::UpdateGuildRelation(m_DBComponent, m_dwGID, dwTargetGID, Guild::TARGET_NEUTRALITY))
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 업데이트 실패",
								m_dwGID, dwTargetGID, Guild::TARGET_NEUTRALITY);
						return false;
					}

/*
					// 삭제
					if (!DBComponent::GuildDB::DeleteGuildRelation(m_DBComponent, m_dwGID, dwTargetGID))
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 삭제 실패",
								m_dwGID, dwTargetGID, cRelationType);
						return false;
					}
*/
					RelationMap::iterator itr = m_Relation[Guild::BY_MINE].find(dwTargetGID);
					if (itr != m_Relation[Guild::BY_MINE].end())
					{
						m_Relation[Guild::BY_MINE].erase(itr);
					}
					else
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 삭제 실패 (길드 관계를 찾을수 없음)",
								m_dwGID, dwTargetGID, cRelationType);
						return false;
					}

					break;
				}

				default: return false;
			}

			break;
		}

		// 대상 길드가 나에게 선언
		case Guild::BY_TARGET:
		{
			switch (cRelationType)
			{
				case Guild::TARGET_NEUTRALITY:
				{
					// 삭제
					if (!DBComponent::GuildDB::DeleteGuildRelation(m_DBComponent, dwTargetGID, m_dwGID, Guild::TARGET_NEUTRALITY))
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 DB 에 삭제 실패",
								m_dwGID, dwTargetGID, cRelationType);
						return false;
					}

					// 나 자신이 나에게 정의된 타켓길드를 삭제하는 로직이다.
					RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].find(dwTargetGID);
					if (itr != m_Relation[Guild::BY_TARGET].end())
					{
						m_Relation[Guild::BY_TARGET].erase(itr);
					}
					else
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 삭제 실패 (길드 관계를 찾을수 없음)",
								dwTargetGID, m_dwGID, cRelationType);
						return false;
					}
					break;
				}

				case Guild::HOSTILITY:
				case Guild::ALERT_HOSTILITY:
				case Guild::COUNTER_HOSTILITY:
				{
					RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].find(dwTargetGID);
					if (itr != m_Relation[Guild::BY_TARGET].end())
					{
						itr->second = cRelationType;
					}
					else
					{
						m_Relation[Guild::BY_TARGET].insert(std::make_pair(dwTargetGID, cRelationType)).second;
					}

					break;
				}

				case Guild::NEUTRALITY:
				{
					// 적대를 Guild::TARGET_NEUTRALITY로 바꿔줘야한다.
					RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].find(dwTargetGID);
					if (itr != m_Relation[Guild::BY_TARGET].end())
					{
						itr->second = Guild::TARGET_NEUTRALITY;
					}
					else
					{
						m_Relation[Guild::BY_TARGET].insert(std::make_pair(dwTargetGID, Guild::TARGET_NEUTRALITY)).second;
					}

					break;
/*
					// 이 부분은 Guild::TARGET_NEUTRALITY 여기에서 빼주므로 여기서는 빼서는 안된다.
					RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].find(dwTargetGID);
					if (itr != m_Relation[Guild::BY_TARGET].end())
					{
						m_Relation[Guild::BY_TARGET].erase(itr);
					}
					else
					{
						ERRLOG3(g_Log, "GID:0x%08x  TargetGID:0x%08x  Relation:%d  길드 관계 삭제 실패 (길드 관계를 찾을수 없음)",
								dwTargetGID, m_dwGID, cRelationType);
						return false;
					}

					break;
*/
				}
				default: return false;
			}

			break;
		}
	}

	return true;
}

void CGuildDB::DeleteRelation()
{
	std::vector<unsigned long> vecTargetGID;
	vecTargetGID.reserve(m_Relation[Guild::BY_MINE].size());

	RelationMap::iterator itr = m_Relation[Guild::BY_MINE].begin();
	while (itr != m_Relation[Guild::BY_MINE].end())
	{
		vecTargetGID.push_back(itr->first);
		++itr;
	}

	std::vector<unsigned long>::iterator vecItr = vecTargetGID.begin();
	while (vecItr != vecTargetGID.end())
	{
		CGuildDBMgr::GetInstance().SetRelation(GetGID(), (*vecItr), Guild::NEUTRALITY);

		PktGuildRelation pktGuildRelation;
		memset(&pktGuildRelation, 0, sizeof(PktGuildRelation));

		pktGuildRelation.m_dwCID = 0;
		pktGuildRelation.m_dwGID = GetGID();
		pktGuildRelation.m_dwTargetGID = (*vecItr);
		pktGuildRelation.m_dwValue = 0;
		pktGuildRelation.m_cSubCmd = PktGuildRelation::GR_PEACE;

		if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildRelation), 
			sizeof(PktGuildRelation), CmdGuildRelation, 0, 0))
		{
			DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktGuildRelation), 
					sizeof(PktGuildRelation), CmdGuildRelation));
		}

		++vecItr;
	}
}

void CGuildDB::CancelAlertAndCounterHostility()
{
	std::vector<unsigned long> vecTargetGID;
	vecTargetGID.reserve(m_Relation[Guild::BY_MINE].size());

	RelationMap::iterator itr = m_Relation[Guild::BY_MINE].begin();
	while (itr != m_Relation[Guild::BY_MINE].end())
	{
		if (itr->second == Guild::ALERT_HOSTILITY ||
			itr->second == Guild::COUNTER_HOSTILITY)
		{
			vecTargetGID.push_back(itr->first);
		}

		++itr;
	}

	std::vector<unsigned long>::iterator vecItr = vecTargetGID.begin();
	while (vecItr != vecTargetGID.end())
	{
		CGuildDBMgr::GetInstance().SetRelation(GetGID(), (*vecItr), Guild::NEUTRALITY);

		PktGuildRelation pktGuildRelation;
		memset(&pktGuildRelation, 0, sizeof(PktGuildRelation));

		pktGuildRelation.m_dwCID = 0;
		pktGuildRelation.m_dwGID = GetGID();
		pktGuildRelation.m_dwTargetGID = (*vecItr);
		pktGuildRelation.m_dwValue = 0;
		pktGuildRelation.m_cSubCmd = PktGuildRelation::GR_PEACE;

		if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildRelation), 
			sizeof(PktGuildRelation), CmdGuildRelation, 0, 0))
		{
			DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
				reinterpret_cast<char*>(&pktGuildRelation), 
				sizeof(PktGuildRelation), CmdGuildRelation));
		}

		++vecItr;
	}
}

void CGuildDB::CancelAlertHostility(unsigned long dwTargetGID)
{
	CGuildDBMgr::GetInstance().SetRelation(GetGID(), dwTargetGID, Guild::NEUTRALITY);

	PktGuildRelation pktGuildRelation;
	memset(&pktGuildRelation, 0, sizeof(PktGuildRelation));

	pktGuildRelation.m_dwCID = 0;
	pktGuildRelation.m_dwGID = GetGID();
	pktGuildRelation.m_dwTargetGID = dwTargetGID;
	pktGuildRelation.m_dwValue = 0;
	pktGuildRelation.m_cSubCmd = PktGuildRelation::GR_PEACE;

	if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildRelation), 
		sizeof(PktGuildRelation), CmdGuildRelation, 0, 0))
	{
		DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
			reinterpret_cast<char*>(&pktGuildRelation), 
			sizeof(PktGuildRelation), CmdGuildRelation));
	}
}

// ------------------------------------------------------------------------------------------------
// 길드 금고 방출
//
// Input : dwGold - 방출하는 총 금액
// Return : unsigned long - 길드원 한 명당 얻게되는 금액
// ------------------------------------------------------------------------------------------------
unsigned long CGuildDB::ReleaseGold(unsigned long dwGold)
{
	// 가입 대기자와 탈퇴 대기자는 제외
    MemberList::difference_type nMemberNum = 
        std::count_if(m_MemberList.begin(), m_MemberList.end(), IsCurrentMember());

	if (0 == nMemberNum)
	{
		ERRLOG1(g_Log, "GID:%10u 길드원이 없는 길드입니다.", m_dwGID);
		return 0;
	}

	unsigned long dwGoldPerChar = dwGold / nMemberNum;
	unsigned long dwDeductGold = dwGoldPerChar * nMemberNum;

	if (m_dwGold < dwDeductGold || 0 == dwGoldPerChar)
	{
		return 0;
	}

	// 한번도 로그인 하지않아 캐시DB에도 없는 길드원의 경우 DB에 직접 돈을 넣어준다. 
	// (실제로는 길드원 전원이 업데이트되지만 접속한 캐릭터들은 정보가 리플레이스되므로 의미없다.)
	if (!DBComponent::GuildDB::ReleaseGold(m_DBComponent, m_dwGID, dwGoldPerChar))
	{
		ERRLOG1(g_Log, "GID:%10u 길드 금고 방출 실패 : DB 에러.", m_dwGID);
		return 0;
	}

	DeductGold(dwDeductGold);

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (IsCurrentMember()(*it))
		{
			MemberInfo memberInfo = *it;

            using namespace DBAgent::DataStorage;

            CSessionData*   lpSessionData = 
                CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

            CCharacterData* lpCharacterData = 0;

			if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
			{
				// 로그아웃한 길드원들에게도 나눠준다.
                lpCharacterData = static_cast<CCharacterData*>(
                    CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));
			}

			if (0 != lpCharacterData)
			{
                CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

                if(charInfoST.Gold <= ULONG_MAX - dwGoldPerChar)
                {
                    charInfoST.Gold += dwGoldPerChar;
                }
                else
                {
                    SERLOG4(g_Log, "GID:%10u / CID:%10 / Gold:%10u / GoldPerChar:%10u / "
                        "길드 금액 방출시에, 돈이 42억을 넘은 사람이 있습니다. 42억으로 맞춥니다", 
                        m_dwGID, memberInfo.m_dwCID, charInfoST.Gold, dwGoldPerChar);

                    charInfoST.Gold = ULONG_MAX;
                }

                lpCharacterData->SetInfo(charInfoST);
			}
		}
	}

	return dwGoldPerChar;
}

unsigned long CGuildDB::SetNewMaster(void)
{
	// 차기 길드 마스터는 랜덤으로 선출하되 직위가 높은 길드원을 우선시 한다.
	MemberList vecShuffleList(m_MemberList.size());
	std::copy(m_MemberList.begin(), m_MemberList.end(), vecShuffleList.begin());
	std::random_shuffle(vecShuffleList.begin(), vecShuffleList.end());

	MemberList::iterator it = vecShuffleList.begin();
	for (; it != vecShuffleList.end(); ++it)
	{
		MemberInfo memberInfo = *it;
		if (MIDDLE_ADMIN == memberInfo.m_MemberListInfo.m_cTitle)
		{
			SetTitle(memberInfo.m_dwCID, MASTER);
			return memberInfo.m_dwCID;
		}
	}

	for (it = vecShuffleList.begin(); it != vecShuffleList.end(); ++it)
	{
		MemberInfo memberInfo = *it;
		if (COMMON == memberInfo.m_MemberListInfo.m_cTitle)
		{
			if(!memberInfo.m_cTactics)
			{
				SetTitle(memberInfo.m_dwCID, MASTER);
				return memberInfo.m_dwCID;
			}					
		}
	}

	return 0;
}

int	 CGuildDB::GetLoginedTotalMemberNum() const
{
	if (0 == m_MemberList.size()) return 0;

	int nLoginedMemberNum = 0;

	MemberList::const_iterator it = m_MemberList.begin();
	MemberList::const_iterator end = m_MemberList.end();

	while (it != end)
	{
		const MemberInfo& memberInfo = *it;

		using namespace DBAgent::DataStorage;

		CSessionData*   lpSessionData = 
			CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

		if (lpSessionData && CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
		{
			++nLoginedMemberNum;
		}

		++it;
	}

	return nLoginedMemberNum;
}

int  CGuildDB::GetLoginedCurrentMemberNum() const
{
	if (0 == m_MemberList.size()) return 0;

	int nLoginedMemberNum = 0;

	MemberList::const_iterator it = m_MemberList.begin();
	MemberList::const_iterator end = m_MemberList.end();

	while (it != end)
	{
		const MemberInfo& memberInfo = *it;

		if (true == IsCurrentMember()(memberInfo))
		{
			using namespace DBAgent::DataStorage;

			CSessionData*   lpSessionData = 
				CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

			if (lpSessionData && CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
			{
				++nLoginedMemberNum;
			}
		}

		++it;
	}

	return nLoginedMemberNum;
}

void CGuildDB::AddAllMemberFame(unsigned char cType, int nFame, Guild::CGuildDB* lpEnemyGuild)
{
	if (0 == m_MemberList.size()) return;

	MemberList::iterator it = m_MemberList.begin();
	MemberList::iterator end = m_MemberList.end();

	while (it != end)
	{
		MemberInfo& memberInfo = *it;

		if (true == IsCurrentMember()(memberInfo))
		{
			using namespace DBAgent::DataStorage;

			CSessionData*   lpSessionData = 
				CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

			CCharacterData* lpCharacterData = 0;

			if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
			{
				// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
				lpCharacterData = static_cast<CCharacterData*>(
					CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));

				if (0 != lpCharacterData)
				{
					CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

					if (Guild::TYPE_VALUE == cType)
					{
						if (nFame < 0 && static_cast<unsigned long>(abs(nFame)) > charInfoST.Fame) 
						{
							charInfoST.Fame = 0;
						}
						else 
						{
							charInfoST.Fame += nFame;
						}
					}
					else if (Guild::TYPE_HALF == cType)
					{
						charInfoST.Fame /= 2;
					}

					lpCharacterData->SetInfo(charInfoST);
				}
			}
			else if(CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
			{
				// 게임에 접속해 있는 유저일 경우
				SERVER_ID serverID;
				serverID.dwID = memberInfo.m_dwServerID;

				// 해당 존 서버에게 패킷 전송
				if(lpEnemyGuild)
				{
					DBAgent::SendPacket::GuildMemberFameUpdate(
						memberInfo.m_dwCID, m_dwGID, cType, nFame, serverID.GetZone(), lpEnemyGuild->GetGID());
				}
				else
				{
					DBAgent::SendPacket::GuildMemberFameUpdate(memberInfo.m_dwCID, m_dwGID, cType, nFame, serverID.GetZone(), 0);
				}
			}
		}

		++it;
	}
}

void CGuildDB::AddLoginedMemberFame(unsigned char cType, int nFame, Guild::CGuildDB* lpEnemyGuild)
{
	if (0 == m_MemberList.size()) return;

	MemberList::iterator it = m_MemberList.begin();
	MemberList::iterator end = m_MemberList.end();

	while (it != end)
	{
		MemberInfo& memberInfo = *it;

		if (true == IsCurrentMember()(memberInfo))
		{
			using namespace DBAgent::DataStorage;

			CSessionData*   lpSessionData = 
				CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

			if (lpSessionData && CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
			{
				// 게임에 접속해 있는 유저일 경우
				SERVER_ID serverID;
				serverID.dwID = memberInfo.m_dwServerID;

				if(lpEnemyGuild)
				{
					// 해당 존 서버에게 패킷 전송
					DBAgent::SendPacket::GuildMemberFameUpdate(
						memberInfo.m_dwCID, m_dwGID, cType, nFame, serverID.GetZone(), lpEnemyGuild->GetGID());
				}
				else
				{
					// 적대 길드가 없으면 0으로 전달.
					// 해당 존 서버에게 패킷 전송
					DBAgent::SendPacket::GuildMemberFameUpdate(
						memberInfo.m_dwCID, m_dwGID, cType, nFame, serverID.GetZone(), 0);
				}
			}
		}

		++it;
	}
}


void CGuildDB::AddMemberFame(unsigned char cType, unsigned long dwCID, int nFame)
{
	if (0 == m_MemberList.size()) return;

	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if ((*it).m_dwCID == dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end()) 
	{ 
		ERRLOG2(g_Log, "GID:%10u 길드 멤버가 아닌 캐릭터의 명성치를 변경하려 합니다. (CID : 0x%08x)", m_dwGID, dwCID);
		return;
	}

	MemberInfo& memberInfo = *it;

	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = 
		CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

	CCharacterData* lpCharacterData = 0;

	if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
	{
		// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
		lpCharacterData = static_cast<CCharacterData*>(
			CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));

		if (0 != lpCharacterData)
		{
			CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

			if (Guild::TYPE_VALUE == cType)
			{
				if (nFame < 0 && static_cast<unsigned long>(abs(nFame)) > charInfoST.Fame) 
				{
					charInfoST.Fame = 0;
				}
				else 
				{
					charInfoST.Fame += nFame;
				}
			}
			else if (Guild::TYPE_HALF == cType)
			{
				charInfoST.Fame /= 2;
			}

			lpCharacterData->SetInfo(charInfoST);
		}
	}
	else if(CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
	{
		// 게임에 접속해 있는 유저일 경우
		SERVER_ID serverID;
		serverID.dwID = memberInfo.m_dwServerID;

		// 해당 존 서버에게 패킷 전송
		DBAgent::SendPacket::GuildMemberFameUpdate(
			memberInfo.m_dwCID, m_dwGID, cType, nFame, serverID.GetZone());
	}
}

void CGuildDB::AddGoldToMaster(unsigned long dwGold)
{
	if (0 == m_MemberList.size()) return;

	MemberInfo& memberInfo = GetMaster();

	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = 
		CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

	CCharacterData* lpCharacterData = 0;

	if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
	{
		// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
		lpCharacterData = static_cast<CCharacterData*>(
			CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));

		if (0 != lpCharacterData)
		{
			CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

			if (charInfoST.Gold <= ULONG_MAX - dwGold)
			{
				charInfoST.Gold += dwGold;
			}
			else
			{
				ERRLOG1(g_Log, "CID : 0x%08x 돈 오버 플로우 발생", memberInfo.m_dwCID);
			}

			lpCharacterData->SetInfo(charInfoST);
		}
	}		
	else if(CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
	{
		// 게임에 접속해 있는 유저일 경우
		SERVER_ID serverID;
		serverID.dwID = memberInfo.m_dwServerID;

		// 해당 존 서버에게 패킷 전송
		DBAgent::SendPacket::GuildMemberGoldUpdate(
			memberInfo.m_dwCID, m_dwGID, dwGold, serverID.GetZone());
	}
}

bool CGuildDB::JoinMember(MemberInfo& memberInfo)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (memberInfo.m_dwCID == it->m_dwCID)
		{
			break;
		}
	}

	if (it != m_MemberList.end()) 
	{ 
		ERRLOG1(g_Log, "GID:%10u 길드 멤버 추가 실패 : 이미 가입되어 있는 멤버입니다.", m_dwGID);
		return false; 
	}

	m_MemberList.push_back(memberInfo);

	if (DELETE_GUILD_MEMBER_NUM <= GetCurrentAllMember())
	{
		::memset(&m_tmCheckMember, 0, sizeof(TIME));

		SYSTEMTIME systemTime;
		::memset(&systemTime, 0, sizeof(SYSTEMTIME));

		DBComponent::GuildDB::UpdateGuildTime(m_DBComponent, m_dwGID, MEMBER_NUM_DOWN, systemTime);
	}

    // 현재 로그인해 있으면 GID를 재설정해준다.
    // 로그인 한 캐릭터만 길드에 가입해 있으므로 로그아웃 DB을 검색할 필요는 없다.

    using namespace DBAgent::DataStorage;

    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);
    CCharacterData* lpCharacterData = 0;

    if (0 != lpSessionData && (0 != (lpCharacterData = lpSessionData->GetCharacterData())))
    {
        lpCharacterData->SetGID(m_dwGID);
    }

	return true;
}

bool CGuildDB::LeaveMember(unsigned long dwCID)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end()) 
	{ 
		ERRLOG2(g_Log, "GID:%10u 길드 멤버 삭제 실패 : 길드원이 아닌 캐릭터입니다. CID:%10u", 
			m_dwGID, dwCID);
		return false; 
	}

	MemberInfo memberInfo = *it;

	if (false == DBComponent::GuildDB::DeleteGuildMember(m_DBComponent, m_dwGID, dwCID, memberInfo.m_MemberListInfo.m_cRank))
	{
		ERRLOG2(g_Log, "GID:%10u 길드 멤버 삭제 실패 : DB에서 삭제 실패. CID:%10u", m_dwGID, dwCID);
		return false;
	}

	// WORK_LIST 2.3 계정 국적 변경 기능 구현
	// 길드 국가 변경으로 탈퇴될시, 탈퇴 대기자후 탈퇴 시, 축출당할시 모두 명성이 감소한다.
//	AddMemberFame(Guild::TYPE_HALF, dwCID, 0);

	// 해당 캐릭터의 공성 병기들을 모두 파괴한다. (남은 HP에 비례하게 자재 갯수와 스타트킷을 돌려준다.)
	CSiegeObjectDBMgr::GetInstance().DestroySiegeArms(dwCID);

	m_MemberList.erase(it);

	if (DELETE_GUILD_MEMBER_NUM > GetCurrentMemberNum())
	{
		SYSTEMTIME systemTime;
		::memset(&systemTime, 0, sizeof(SYSTEMTIME));

		GetLocalTime(&systemTime);

		m_tmCheckMember.Year = systemTime.wYear;
		m_tmCheckMember.Month = systemTime.wMonth;
		m_tmCheckMember.Day = systemTime.wDay;
		m_tmCheckMember.Hour = systemTime.wHour;
		m_tmCheckMember.Minute = systemTime.wMinute;
		m_tmCheckMember.Second = systemTime.wSecond;
		m_tmCheckMember.MSecond = systemTime.wMilliseconds;

		DBComponent::GuildDB::UpdateGuildTime(m_DBComponent, m_dwGID, MEMBER_NUM_DOWN, systemTime);
	}

    using namespace DBAgent::DataStorage;

    CSessionData* lpSessionData = 
        CSessionDataMgr::GetInstance().GetCharLoadedSession(memberInfo.m_dwCID);

    CCharacterData* lpCharacterData = 0;

    // 열린 세션이 없거나. 열렸다 하더라도 데이터가 없는 경우는 로그아웃 데이터에서 검색
    if (0 == lpSessionData || (0 == (lpCharacterData = lpSessionData->GetCharacterData())))
    {
        lpCharacterData = static_cast<CCharacterData*>(
            CCharacterDataMgr::GetInstance().GetLogoutData(memberInfo.m_dwCID));
    }

	if (0 != lpCharacterData)
	{
        // GID를 0 으로 셋.
        lpCharacterData->SetGID(0);		
	}

	return true;
}

bool CGuildDB::UpdateMemberInfo(unsigned long dwCID, MemberListInfo memberListInfo, MemberDetailInfo memberDetailInfo)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end())
	{
		ERRLOG2(g_Log, "GID:%10u 길드원 정보 갱신 실패 : 길드원이 아닙니다. CID:%10u", m_dwGID, dwCID);
		return false;
	}

	MemberInfo& memberInfo = *it;

	memberInfo.m_MemberListInfo = memberListInfo;
	memberInfo.m_MemberDetailInfo = memberDetailInfo;

	const unsigned long dwFameSum = std::accumulate(m_MemberList.begin(), m_MemberList.end(), 0, SumMemberFame());
	const unsigned long dwPreFame = m_dwFame;
	const unsigned char cCurrentMemberNum = GetCurrentAllMember();
	m_dwFame = (0 != cCurrentMemberNum) ? (dwFameSum / cCurrentMemberNum) : 0;

	if (dwPreFame != m_dwFame)
	{
		return DBComponent::GuildDB::UpdateGuildFame(m_DBComponent, m_dwGID, m_dwFame);
	}

	return true;
}

void CGuildDB::KickAllWaitMember()
{
	std::list<unsigned long> lstKickMembers;

	MemberList::iterator itr = m_MemberList.begin();
	while (itr != m_MemberList.end())
	{
		// 용병은 제외
		if (JOIN_WAIT == (*itr).m_MemberListInfo.m_cTitle && 0 == (*itr).m_cTactics)
		{
			lstKickMembers.push_back((*itr).m_dwCID);
		}

		++itr;
	}

	if (!lstKickMembers.empty())
	{
		unsigned short wError = 0;
		std::list<unsigned long>::iterator KickItr = lstKickMembers.begin();
		while (KickItr != lstKickMembers.end())
		{
			if ( LeaveMember( (*KickItr) ) )
			{
				PktGuildCmd pktGuildCmd;
				memset(&pktGuildCmd, 0, sizeof(PktGuildCmd));

				pktGuildCmd.m_dwGID = GetGID() ;
				pktGuildCmd.m_dwSenderID = (*KickItr) ;
				pktGuildCmd.m_dwReferenceID = (*KickItr) ;
				pktGuildCmd.m_wCmd = PktGuildCmd::GC_KICK ;

				if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktGuildCmd), 
					sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
				{
					DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
						reinterpret_cast<char*>(&pktGuildCmd), 
						sizeof(PktGuildCmd), CmdGuildCmd));
				}
			}

			++KickItr;
		}
	}
}

void CGuildDB::SendLoginOutMember(unsigned long dwCID, unsigned long dwServerID)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end()) 
	{ 
		ERRLOG2(g_Log, "GID:%10u 길드원 로그인/아웃 실패 : 길드원이 아닙니다. CID:%10u", m_dwGID, dwCID);
		return; 
	}

	MemberInfo& LoginOutMemberInfo = *it;
	LoginOutMemberInfo.m_dwServerID = dwServerID;

	char* szSenderName = LoginOutMemberInfo.m_strName;

	// 로그아웃 시간 체크 (길드 삭제시 필요)
	if (0 == dwServerID)
	{
		// 가입대기자는 길드원으로 취급하지 않는다.
		if (JOIN_WAIT != LoginOutMemberInfo.m_MemberListInfo.m_cTitle)
		{
			SYSTEMTIME systemTime;
			::memset(&systemTime, 0, sizeof(SYSTEMTIME));

			// 마스터가 일정기간(TRANFER_MASTER_TIME)동안 로그인하지 않으면 자동으로 권한이 양도된다.
			if (MASTER == LoginOutMemberInfo.m_MemberListInfo.m_cTitle)
			{
				GetLocalTime(&systemTime);

				m_tmGMLastLogout.Year = systemTime.wYear;
				m_tmGMLastLogout.Month = systemTime.wMonth;
				m_tmGMLastLogout.Day = systemTime.wDay;
				m_tmGMLastLogout.Hour = systemTime.wHour;
				m_tmGMLastLogout.Minute = systemTime.wMinute;
				m_tmGMLastLogout.Second = systemTime.wSecond;
				m_tmGMLastLogout.MSecond = systemTime.wMilliseconds;

				DBComponent::GuildDB::UpdateGuildTime(m_DBComponent, m_dwGID, MASTER_LOGOUT, systemTime);
			}

			// 일정기간(DELETE_GUILD_LOGOUT_TIME)동안 길드원 중 아무도 로그인하지 않으면 길드가 해체된다.
			MemberList::iterator it = m_MemberList.begin();
			for (; it != m_MemberList.end(); ++it)
			{
				MemberInfo memberInfo = *it;

				if (0 != memberInfo.m_dwServerID)
				{
					break;
				}
			}

			if (it == m_MemberList.end())
			{
				GetLocalTime(&systemTime);

				m_tmLastLogout.Year = systemTime.wYear;
				m_tmLastLogout.Month = systemTime.wMonth;
				m_tmLastLogout.Day = systemTime.wDay;
				m_tmLastLogout.Hour = systemTime.wHour;
				m_tmLastLogout.Minute = systemTime.wMinute;
				m_tmLastLogout.Second = systemTime.wSecond;
				m_tmLastLogout.MSecond = systemTime.wMilliseconds;

				DBComponent::GuildDB::UpdateGuildTime(m_DBComponent, m_dwGID, LAST_LOGOUT, systemTime);
			}
		}
	}
	else
	{
		if (MASTER == LoginOutMemberInfo.m_MemberListInfo.m_cTitle)
		{
			::memset(&m_tmGMLastLogout, 0, sizeof(TIME));
		}

		::memset(&m_tmLastLogout, 0, sizeof(TIME));
	}

    DBAgent::SendPacket::GuildCmd(NULL, m_dwGID, dwCID, dwServerID, 
        PktGuildCmd::GC_LOGINOUT, szSenderName, m_strName, PktBase::NO_SERVER_ERR);
}

unsigned char CGuildDB::GetTacticsMember(unsigned long dwCID)
{
	MemberList::iterator it = m_MemberList.begin();

	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			return (*it).m_cTactics;			
		}
	}

	return 0;
}

void CGuildDB::SetTacticsMember(unsigned long dwCID, unsigned char cTactics)
{
	MemberList::iterator it = m_MemberList.begin();

	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			(*it).m_cTactics = cTactics;			
			return;
		}
	}	
}

void CGuildDB::SendJoinTacticsMember(CSendStream* lpSendStream, unsigned long dwCID, unsigned char cTitle, unsigned char CType)
{
	unsigned short wError = PktBase::NO_SERVER_ERR;

	using namespace DBAgent::DataStorage;

	CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);
	CCharacterData* lpCharacterData = 0;

	if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
	{
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		const CHAR_INFOST& charInfoST = lpCharacterData->GetInfo();

		const int MAX_PACKET_DATA = sizeof(PktGuildCmd) + sizeof(MemberInfo);
		char szPacketData[MAX_PACKET_DATA];

		PktGuildCmd*    lpPktGCAck = reinterpret_cast<PktGuildCmd*>(szPacketData);
		MemberInfo*     lpMemberInfo = reinterpret_cast<MemberInfo*>(lpPktGCAck + 1);
		memset(lpMemberInfo, 0, sizeof(MemberInfo));

		lpMemberInfo->m_dwCID		= dwCID;
		lpMemberInfo->m_dwServerID	= lpSessionData->GetServerID();
		lpMemberInfo->m_cTactics	= Guild::TACTICS_WAIT;
		strncpy(lpMemberInfo->m_strName, charInfoST.Name, Guild::MAX_MEMBER_NAME_LEN);

		// 가입은 무조건 끝으로 한 후 가입 상황에 따라 직위를 바꿔준다.
		lpMemberInfo->m_MemberListInfo.m_cTitle = cTitle;

		lpMemberInfo->m_MemberListInfo.m_cLevel = charInfoST.Level;
		lpMemberInfo->m_MemberListInfo.m_cClass = static_cast<unsigned char>(charInfoST.Class);

		lpMemberInfo->m_MemberDetailInfo.m_dwFame = charInfoST.Fame;
		lpMemberInfo->m_MemberDetailInfo.m_dwGold = charInfoST.Gold;

		if (!JoinMember(*lpMemberInfo))
		{
			ERRLOG1(g_Log, "GID:%10u 길드 용병 멤버 추가 실패 : JoinMember실패.", m_dwGID);

			wError = PktBase::SERVER_ERROR;            
		}
		else if (!DBComponent::GuildDB::InsertGuildMember(m_DBComponent, m_dwGID, lpMemberInfo->m_dwCID, 
			lpMemberInfo->m_MemberListInfo.m_cTitle, lpMemberInfo->m_MemberListInfo.m_cRank))
		{			
			ERRLOG1(g_Log, "GID:%10u 길드 용병 멤버 추가 실패 : DB에 삽입 실패.", m_dwGID);

			wError = PktBase::SERVER_ERROR;
			LeaveMember(lpMemberInfo->m_dwCID);
		}

		DBComponent::GuildDB::UpdateGuildMemberTactics(m_DBComponent, GetGID(), dwCID, Guild::TACTICS_WAIT);

		// 서열 조정을 위해 가입시킨 후 직위를 적용시켜준다.
		SetTitle(dwCID, cTitle);

		MemberList::iterator it = m_MemberList.begin();
		for (; it != m_MemberList.end(); ++it)
		{
			if (dwCID == it->m_dwCID)
			{
				break;
			}
		}

		MemberInfo memberInfo = *it;

		lpMemberInfo->m_MemberListInfo.m_cTitle = memberInfo.m_MemberListInfo.m_cTitle;			

		lpPktGCAck->m_dwGID			= m_dwGID;
		lpPktGCAck->m_dwSenderID	= dwCID;
		lpPktGCAck->m_dwReferenceID = cTitle;
		lpPktGCAck->m_wCmd			= CType;

		strncpy(lpPktGCAck->m_szGuildName, m_strName, Guild::MAX_GUILD_NAME_LEN);
		strncpy(lpPktGCAck->m_szSenderName, charInfoST.Name, Guild::MAX_MEMBER_NAME_LEN);

		lpPktGCAck->InitPtHead(sizeof(PktGuildCmd) + sizeof(MemberInfo), CmdGuildCmd, 0, wError);	

		if(PacketWrap::WrapHeader(szPacketData, MAX_PACKET_DATA, CmdGuildCmd, 0, wError))
		{
			if (PktBase::NO_SERVER_ERR == wError)
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(
					CSendPacketAllServer(szPacketData, MAX_PACKET_DATA, CmdGuildCmd));
			}
			else if(0 != lpSendStream)
			{
				lpSendStream->PutBuffer(szPacketData, MAX_PACKET_DATA, CmdGuildCmd);
			}
		}
	}
}

void CGuildDB::SendJoinMember(CSendStream* lpSendStream, unsigned long dwCID, unsigned char cTitle)
{
	unsigned short wError = PktBase::NO_SERVER_ERR;

    using namespace DBAgent::DataStorage;

    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);
    CCharacterData* lpCharacterData = 0;

    if (0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
        const CHAR_INFOST& charInfoST = lpCharacterData->GetInfo();
		const CHAR_INFOEX& charInfoEX = lpCharacterData->GetInfoEx();

        const int MAX_PACKET_DATA = sizeof(PktGuildCmd) + sizeof(MemberInfo);
        char szPacketData[MAX_PACKET_DATA];

        PktGuildCmd*    lpPktGCAck = reinterpret_cast<PktGuildCmd*>(szPacketData);
        MemberInfo*     lpMemberInfo = reinterpret_cast<MemberInfo*>(lpPktGCAck + 1);
        memset(lpMemberInfo, 0, sizeof(MemberInfo));

        lpMemberInfo->m_dwCID = dwCID;
        lpMemberInfo->m_dwServerID = lpSessionData->GetServerID();
        strncpy(lpMemberInfo->m_strName, charInfoST.Name, Guild::MAX_MEMBER_NAME_LEN);
        
        // 가입은 무조건 끝으로 한 후 가입 상황에 따라 직위를 바꿔준다.
        lpMemberInfo->m_MemberListInfo.m_cTitle = JOIN_WAIT;

        lpMemberInfo->m_MemberListInfo.m_cLevel = charInfoST.Level;
        lpMemberInfo->m_MemberListInfo.m_cClass = static_cast<unsigned char>(charInfoST.Class);
		lpMemberInfo->m_MemberListInfo.m_cGuildWarFlag = charInfoEX.GuildWarFlag;

        lpMemberInfo->m_MemberDetailInfo.m_dwFame = charInfoST.Fame;
        lpMemberInfo->m_MemberDetailInfo.m_dwGold = charInfoST.Gold;

		if (!JoinMember(*lpMemberInfo))
		{
            ERRLOG1(g_Log, "GID:%10u 길드 멤버 추가 실패 : JoinMember실패.", m_dwGID);

			wError = PktBase::SERVER_ERROR;            
		}
        else if (!DBComponent::GuildDB::InsertGuildMember(m_DBComponent, m_dwGID, lpMemberInfo->m_dwCID, 
			lpMemberInfo->m_MemberListInfo.m_cTitle, lpMemberInfo->m_MemberListInfo.m_cRank))
		{			
            ERRLOG1(g_Log, "GID:%10u 길드 멤버 추가 실패 : DB에 삽입 실패.", m_dwGID);

			wError = PktBase::SERVER_ERROR;
            LeaveMember(lpMemberInfo->m_dwCID);
		}

		// 서열 조정을 위해 가입시킨 후 직위를 적용시켜준다.
		SetTitle(dwCID, cTitle);

		MemberList::iterator it = m_MemberList.begin();
		for (; it != m_MemberList.end(); ++it)
		{
			if (dwCID == it->m_dwCID)
			{
				break;
			}
		}

		MemberInfo memberInfo = *it;

		lpMemberInfo->m_MemberListInfo.m_cTitle = memberInfo.m_MemberListInfo.m_cTitle;

		lpPktGCAck->m_dwGID = m_dwGID;
		lpPktGCAck->m_dwSenderID = dwCID;
		lpPktGCAck->m_dwReferenceID = cTitle;
		lpPktGCAck->m_wCmd = PktGuildCmd::GC_JOIN;

		strncpy(lpPktGCAck->m_szGuildName, m_strName, Guild::MAX_GUILD_NAME_LEN);
		strncpy(lpPktGCAck->m_szSenderName, charInfoST.Name, Guild::MAX_MEMBER_NAME_LEN);
        
        lpPktGCAck->InitPtHead(sizeof(PktGuildCmd) + sizeof(MemberInfo), CmdGuildCmd, 0, wError);	

        if(PacketWrap::WrapHeader(szPacketData, MAX_PACKET_DATA, CmdGuildCmd, 0, wError))
        {
		    if (PktBase::NO_SERVER_ERR == wError)
		    {
                DBAgent::CGameDispatch::GetDispatchTable().Process(
                    CSendPacketAllServer(szPacketData, MAX_PACKET_DATA, CmdGuildCmd));
		    }
		    else if(0 != lpSendStream)
		    {
                lpSendStream->PutBuffer(szPacketData, MAX_PACKET_DATA, CmdGuildCmd);
		    }
        }
    }
}

