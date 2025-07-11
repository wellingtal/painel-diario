#include "stdafx.h"

#include <Utility/CheckSum/Crc32Static.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include "GuildUtil.h"
#include "Guild.h"
#include "GuildMgr.h"


using namespace Guild;


CGuildMgr& CGuildMgr::GetInstance()
{
    static CGuildMgr guildMgr;
    return guildMgr;
}

CGuildMgr::CGuildMgr(void)
{
}

CGuildMgr::~CGuildMgr(void)
{
	Destroy();
}

void CGuildMgr::Destroy(void)
{
	std::for_each(m_GuildMap.begin(), m_GuildMap.end(), FnDeleteSecond());
	m_GuildMap.clear();
	m_GuildNameMap.clear();
}

bool CGuildMgr::CreateGuild(unsigned long dwMasterID, unsigned long dwGuildID, unsigned char cInclination, char* szGuildName)
{
	CGuild* lpGuild = new CGuild(dwGuildID, cInclination, szGuildName);

	if (false == m_GuildMap.insert(GuildMap::value_type(dwGuildID, lpGuild)).second)
	{
		delete lpGuild;
		return false;
	}

	if (false == m_GuildNameMap.insert(GuildNameMap::value_type(std::string(szGuildName), lpGuild)).second)
	{
		m_GuildMap.erase(dwGuildID);
		delete lpGuild;
		return false;
	}

	PktCreateGuild pktCG;
	pktCG.m_dwCID = 0;
	pktCG.m_dwGID = dwGuildID;
	pktCG.m_cInclination = cInclination;
	::memcpy(pktCG.m_szGuildName, szGuildName, MAX_GUILD_NAME_LEN);

	char* szPacket = reinterpret_cast<char *>(&pktCG);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCreateGuild), CmdCreateGuild, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCreateGuild), CmdCreateGuild);
	}

	return true;
}

bool CGuildMgr::GetMemberList(unsigned long dwGID, const MemberList* pMemberList)
{
	GuildMap::iterator it = m_GuildMap.find(dwGID);
	
	if (it == m_GuildMap.end()) 
	{ 
		return false;
	}

	CGuild* lpGuild = it->second;

	if(lpGuild)
	{
		pMemberList = &lpGuild->m_MemberList;
		return true;
	}

	return false;
}

bool CGuildMgr::DissolveGuild(unsigned long dwGID)
{
	GuildMap::iterator it = m_GuildMap.find(dwGID);
	if (it == m_GuildMap.end()) 
    { 
        return false;
    }

    typedef std::vector<unsigned long> DissolveCIDList;

	CGuild* lpGuild = it->second;

    MemberList& memberList = lpGuild->m_MemberList;

    MemberList::iterator member_pos = memberList.begin();
    MemberList::iterator member_end = memberList.end();

    DissolveCIDList dissolveList;
    dissolveList.reserve(memberList.size());

    while (member_pos != member_end)
    {
        dissolveList.push_back(member_pos->m_dwCID);
		++member_pos;
    }

    DissolveCIDList::iterator dissolve_pos = dissolveList.begin();
    DissolveCIDList::iterator dissolve_end = dissolveList.end();

	while (dissolve_pos != dissolve_end)
	{
		lpGuild->LeaveMember(*dissolve_pos);
		++dissolve_pos;
	}

	m_GuildNameMap.erase(lpGuild->m_strName);
	
	m_GuildMap.erase(it);
    delete lpGuild;

	PktCreateGuild pktCG;
	pktCG.m_dwGID = dwGID;
	
	char* szPacket = reinterpret_cast<char*>(&pktCG);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCreateGuild), CmdCreateGuild, 0, PktCreateGuild::FAIL_DISSOLVE_GUILD))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCreateGuild), CmdCreateGuild);
	}

	return true;
}

CGuild* CGuildMgr::GetGuild(unsigned long dwGID)
{
	GuildMap::iterator it = m_GuildMap.find(dwGID);
	if (it == m_GuildMap.end()) { return NULL; }

	return it->second;
}

CGuild* CGuildMgr::GetGuild(char* szName)
{
	GuildNameMap::iterator it = m_GuildNameMap.find(szName);
	if (it == m_GuildNameMap.end()) { return NULL; }

	return it->second;
}


bool CGuildMgr::SetRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType)
{
	CGuild* lpGuild = GetGuild(dwGID);
	CGuild* lpTargetGuild = GetGuild(dwTargetGID);
	
	if (NULL == lpGuild || NULL == lpTargetGuild) { return false; }

	if(Guild::TARGET_NEUTRALITY == cRelationType)
	{
		if (!lpGuild->SetRelation(dwTargetGID, cRelationType, Guild::BY_TARGET))
		{
			return false;
		}
		if (!lpTargetGuild->SetRelation(dwGID, cRelationType, Guild::BY_MINE))
		{
			return false;
		}

		return true;
	}

	lpGuild->SetRelation(dwTargetGID, cRelationType, Guild::BY_MINE);

	lpTargetGuild->SetRelation(dwGID, cRelationType, Guild::BY_TARGET);

	return true;
}


bool CGuildMgr::SendGuildList(unsigned long dwCID, unsigned char cSortCmd, unsigned char cPage, unsigned char cNum, GuildCheckSumNode* lpNode)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID:0x%08x 존재하지 않는 캐릭터가 길드 리스트를 요청하였습니다.", dwCID);
		return false;
	}

	if (cNum > PktGuildList::MAX_NUM_PER_PAGE)
	{
		ERRLOG2(g_Log, "CID:0x%08x 길드 리스트의 한 페이지에 찍히는 수를 초과하여 요청하였습니다. 요청노드수:%d", 
			dwCID, cNum);
		cNum = PktGuildList::MAX_NUM_PER_PAGE;
	}

	const int   MAX_BUFFER = sizeof(PktGuildList) + PktGuildList::MAX_NUM_PER_PAGE * sizeof(GuildLargeInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktGuildList* lpPktGLAck = reinterpret_cast<PktGuildList *>(szBuffer);
	GuildSmallInfoNode*	lpSmallInfoNode	= reinterpret_cast<GuildSmallInfoNode *>(lpPktGLAck + 1);                

    lpPktGLAck->m_dwCID = dwCID;
	lpPktGLAck->m_cSortCmd = cSortCmd;
	lpPktGLAck->m_cPage = cPage;

	lpPktGLAck->m_cSmallNodeNum = 0;
	lpPktGLAck->m_cLargeNodeNum = 0;

	GuildLargeInfoNode aryCurrentInfoList[PktGuildList::MAX_NUM_PER_PAGE];
	// 무작위 요청의 경우 원하는 GID를 알아둔다.
	if (Guild::GUILD_REQUEST == cSortCmd)
	{
		for (int nIndex = 0; nIndex < cNum; ++nIndex)
		{
			aryCurrentInfoList[nIndex].m_dwGID = (lpNode + nIndex)->m_dwGID;
		}
	}

	GetSortingPageList(lpCharacter, cSortCmd, cPage, aryCurrentInfoList);	

	int nClientIndex = 0;
	int nServerIndex = 0;

	// 무작위 요청은 무조건 전체 정보를 보내준다. (대부분 마크가 없어서 요청한 경우임)
	if (Guild::GUILD_REQUEST != cSortCmd)
	{
		for (; nClientIndex < cNum && nServerIndex < PktGuildList::MAX_NUM_PER_PAGE; ++nClientIndex, ++nServerIndex)
		{
			unsigned long dwCheckSum = 0;
			CCrc32Static::BufferCrc32(reinterpret_cast<const char *>(&aryCurrentInfoList[nServerIndex]), 
				sizeof(GuildSmallInfoNode), dwCheckSum);

			if (NULL != lpNode)
			{
				if (lpNode->m_dwGID == aryCurrentInfoList[nServerIndex].m_dwGID)
				{
					// 클라이언트와 서버의 체크섬 정보가 틀릴 경우 간략 정보를 보낸다.
					if (lpNode->m_dwCheckSum != dwCheckSum)
					{
						*lpSmallInfoNode = static_cast<GuildSmallInfoNode>(aryCurrentInfoList[nServerIndex]);
						++lpSmallInfoNode;
						++lpPktGLAck->m_cSmallNodeNum;
					}

					// 클라이언트와 서버의 GID 정보가 일치할 경우 전체 정보를 보내지 않는다.
					aryCurrentInfoList[nServerIndex].m_dwGID = 0;
				}
			}

			++lpNode;
		}
	}

	GuildLargeInfoNode* lpLargeInfoNode = reinterpret_cast<GuildLargeInfoNode *>(lpSmallInfoNode);

	for (nServerIndex = 0; nServerIndex < PktGuildList::MAX_NUM_PER_PAGE; ++nServerIndex)
	{
		if (0 != aryCurrentInfoList[nServerIndex].m_dwGID)
		{
			*lpLargeInfoNode = aryCurrentInfoList[nServerIndex];
			++lpLargeInfoNode;
			++lpPktGLAck->m_cLargeNodeNum;
		}
	}

	unsigned char cSmallNodeNum = lpPktGLAck->m_cSmallNodeNum;
	unsigned char cLargeNodeNum = lpPktGLAck->m_cLargeNodeNum;
	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktGuildList) + 
			cSmallNodeNum * sizeof(GuildSmallInfoNode) + cLargeNodeNum * sizeof(GuildLargeInfoNode), 
			CmdGuildList, 0, 0);
	}

	return true;
}

bool CGuildMgr::SendGuildRelationInfo(unsigned long dwCID, unsigned long dwGID)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG1(g_Log, "CID:0x%08x 존재하지 않는 캐릭터가 길드 관계 리스트를 요청하였습니다.", dwCID);
		return false;
	}

	CGuild* lpRequestGuild = GetGuild(dwGID);
	if (NULL == lpRequestGuild)
	{
		ERRLOG1(g_Log, "GID:0x%08x 존재하지 않는 길드의 관계 리스트를 요청하였습니다.", dwGID);
		return false;
	}

	const int   MAX_BUFFER = sizeof(PktGuildRelationInfo) + Guild::MAX_RELATION_NUM * sizeof(GuildRelationInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktGuildRelationInfo* lpPktGRAck = reinterpret_cast<PktGuildRelationInfo *>(szBuffer);
	GuildRelationInfoNode*	lpRelationNode = reinterpret_cast<GuildRelationInfoNode *>(lpPktGRAck + 1);

	lpPktGRAck->m_dwCID = dwCID;
	lpPktGRAck->m_dwGID = dwGID;
	lpPktGRAck->m_wNodeNum = 0;

	CGuild* lpGuild = NULL;
	GuildLargeInfoNode LargeInfo;

	GuildMap::iterator itr = m_GuildMap.begin();
	while (itr != m_GuildMap.end())
	{
		lpGuild = itr->second;
		if (NULL != lpGuild)
		{
			LargeInfo = lpGuild->GetLargeInfo(0, 0, lpRequestGuild);

			if (Guild::NEUTRALITY != LargeInfo.m_cRelationByMine ||
				Guild::NEUTRALITY != LargeInfo.m_cRelationByTarget)
			{
				++lpPktGRAck->m_wNodeNum;

				lpRelationNode->m_dwGID = LargeInfo.m_dwGID;
				lpRelationNode->m_cRelationByMine = LargeInfo.m_cRelationByMine;
				lpRelationNode->m_cRelationByTarget = LargeInfo.m_cRelationByTarget;
				++lpRelationNode;
			}
		}

		++itr;
	}

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktGuildRelationInfo) +
									   lpPktGRAck->m_wNodeNum * sizeof(GuildRelationInfoNode),
									   CmdGuildRelationInfo, 0, 0);
	}

	return false;
}

bool CGuildMgr::GetSortingPageList(CCharacter* lpCharacter, unsigned char cSortCmd, unsigned char cPage, 
								   GuildLargeInfoNode* aryCurrentInfoList)
{
	CGuild* lpMyGuild = GetGuild(lpCharacter->GetGID());
	unsigned short wStartOfPage = cPage * PktGuildList::MAX_NUM_PER_PAGE;

	// 명성순으로 소트
	typedef std::pair<unsigned long, CGuild *> GuildPair;
	unsigned short wGuildNum = static_cast<unsigned short>(m_GuildMap.size());

	if (wGuildNum < wStartOfPage)
	{
		if (false == m_GuildMap.empty())
		{
			ERRLOG3(g_Log, "CID:0x%08x 요청 불가능한 페이지를 요청하였습니다. 길드수:%d, 페이지:%d", 
				lpCharacter->GetCID(), wGuildNum, cPage);
		}

		for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
		{
			aryCurrentInfoList[cIndex].m_dwGID = 0;
		}

		return false;
	}

	std::vector<GuildPair> sortVector;
	GuildPair NullPair(0, (Guild::CGuild*)NULL);
	sortVector.reserve(wGuildNum);
	sortVector.assign(wGuildNum, NullPair);

	std::partial_sort_copy(m_GuildMap.begin(), m_GuildMap.end(), sortVector.begin(), sortVector.end(), CompareGuildFame());
	if (true == sortVector.empty()) 
	{ 
		for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
		{
			aryCurrentInfoList[cIndex].m_dwGID = 0;
		}

		return true; 
	}
	unsigned short wVectorSize = static_cast<unsigned short>(sortVector.size());

	switch (cSortCmd)
	{
		case Guild::GUILD_FAME:
		{
			unsigned char cIndex = 0;
			unsigned short wRank = wStartOfPage;

			for (; cIndex < PktGuildList::MAX_NUM_PER_PAGE && wRank < wVectorSize; ++cIndex, ++wRank)
			{
				CGuild* lpGuild = GetGuild(sortVector[wRank].first);
				if (NULL == lpGuild) 
				{ 
					ERRLOG1(g_Log, "GID:0x%08x 존재하지 않는 길드가 소트 벡터에 들어있습니다.", sortVector[wRank].first);
					break; 
				}

				aryCurrentInfoList[cIndex] = lpGuild->GetLargeInfo(cIndex, wRank + 1, lpMyGuild);
			}
			break;
		}

		case Guild::GUILD_NAME:
		{
			GuildNameMap::iterator it = m_GuildNameMap.begin();
			std::advance(it, wStartOfPage);

			for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE && it != m_GuildNameMap.end(); 
				++cIndex, ++it)
			{
				CGuild* lpGuild = it->second;
				if (NULL == lpGuild) { break; }

				for (unsigned short wRank = 0; wRank < wGuildNum; ++wRank)
				{
					if (sortVector[wRank].second == lpGuild)
					{
						aryCurrentInfoList[cIndex] = lpGuild->GetLargeInfo(cIndex, wRank + 1, lpMyGuild);
						break;
					}
				}
			}
			break;
		}

		case Guild::GUILD_HOSTILITY:
		{
			if ( NULL != lpMyGuild )
			{
				unsigned short wGuildNum = static_cast<unsigned short>(m_GuildNameMap.size());
				std::vector<unsigned long> sortVector;
				sortVector.reserve(wGuildNum);

				unsigned char cRelationByMine, cRelationByTarget ;
				for (GuildNameMap::iterator it = m_GuildNameMap.begin(); it != m_GuildNameMap.end(); ++it)
				{
					CGuild* lpGuild = it->second;
					if (NULL == lpGuild) { break; }

					cRelationByMine = lpMyGuild->GetRelation( lpGuild->GetGID() ) ;
					cRelationByTarget = lpGuild->GetRelation( lpMyGuild->GetGID() ) ;

					if (Guild::HOSTILITY == cRelationByMine ||
						Guild::ALERT_HOSTILITY == cRelationByMine ||
						Guild::COUNTER_HOSTILITY == cRelationByMine ||
						Guild::TARGET_NEUTRALITY == cRelationByMine ||
						Guild::HOSTILITY == cRelationByTarget ||
						Guild::ALERT_HOSTILITY == cRelationByTarget ||
						Guild::COUNTER_HOSTILITY == cRelationByTarget ||
						Guild::TARGET_NEUTRALITY == cRelationByTarget)
					{
						sortVector.push_back(lpGuild->GetGID());
					}
				}

				if (true == sortVector.empty())
				{ 
					for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
					{
						aryCurrentInfoList[cIndex].m_dwGID = 0;
					}

					return true; 
				}

				unsigned short wVectorSize = static_cast<unsigned short>(sortVector.size());
				unsigned char cIndex = 0;
				unsigned short wRank = wStartOfPage;

				for (; cIndex < PktGuildList::MAX_NUM_PER_PAGE && wRank < wVectorSize; ++cIndex, ++wRank)
				{
					CGuild* lpGuild = GetGuild(sortVector[wRank]);
					if (NULL == lpGuild) 
					{ 
						ERRLOG1(g_Log, "GID:0x%08x 존재하지 않는 길드가 소트 벡터에 들어있습니다.", sortVector[wRank]);
						break; 
					}

					aryCurrentInfoList[cIndex] = lpGuild->GetLargeInfo(cIndex, wRank + 1, lpMyGuild);
				}

			}
			break;
		}

		case Guild::GUILD_NEUTRALITY:
		{
			if ( NULL != lpMyGuild )
			{
				unsigned short wGuildNum = static_cast<unsigned short>(m_GuildNameMap.size());
				std::vector<unsigned long> sortVector;
				sortVector.reserve(wGuildNum);

				unsigned char cRelationByMine, cRelationByTarget ;
				for (GuildNameMap::iterator it = m_GuildNameMap.begin(); it != m_GuildNameMap.end(); ++it)
				{
					CGuild* lpGuild = it->second;
					if (NULL == lpGuild) { break; }

					cRelationByMine = lpMyGuild->GetRelation( lpGuild->GetGID() ) ;
					cRelationByTarget = lpGuild->GetRelation( lpMyGuild->GetGID() ) ;

					if (Guild::NEUTRALITY == cRelationByMine &&	Guild::NEUTRALITY == cRelationByTarget)
					{
						sortVector.push_back(lpGuild->GetGID());
					}
				}

				if (true == sortVector.empty())
				{ 
					for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
					{
						aryCurrentInfoList[cIndex].m_dwGID = 0;
					}

					return true; 
				}

				unsigned short wVectorSize = static_cast<unsigned short>(sortVector.size());
				unsigned char cIndex = 0;
				unsigned short wRank = wStartOfPage;

				for (; cIndex < PktGuildList::MAX_NUM_PER_PAGE && wRank < wVectorSize; ++cIndex, ++wRank)
				{
					CGuild* lpGuild = GetGuild(sortVector[wRank]);
					if (NULL == lpGuild) 
					{ 
						ERRLOG1(g_Log, "GID:0x%08x 존재하지 않는 길드가 소트 벡터에 들어있습니다.", sortVector[wRank]);
						break; 
					}

					aryCurrentInfoList[cIndex] = lpGuild->GetLargeInfo(cIndex, wRank + 1, lpMyGuild);
				}

			}
			break;
		}

		case Guild::GUILD_MINE:
		{
			if (NULL != lpMyGuild)
			{
				for (unsigned short wRank = 0; wRank < wGuildNum; ++wRank)
				{
					if (sortVector[wRank].second == lpMyGuild)
					{
						aryCurrentInfoList[0] = GuildLargeInfoNode(lpMyGuild->m_dwGID, 0, lpMyGuild->m_cInclination, 
							wRank + 1, lpMyGuild->m_dwFame, lpMyGuild->m_cLevel, lpMyGuild->GetCurrentMemberNum(), 
							lpMyGuild->GetMaster().m_strName, lpMyGuild->m_strName, lpMyGuild->m_szMark, MINE, MINE);
						break;
					}
				}
			}
			break;
		}

		case Guild::GUILD_REQUEST:
		{
			for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
			{
				if (0 == aryCurrentInfoList[cIndex].m_dwGID) { break; }

				GuildMap::iterator it = m_GuildMap.find(aryCurrentInfoList[cIndex].m_dwGID);
				if (it == m_GuildMap.end())
				{
					// 서버와 클라이언트의 시간차로 없어진 길드 정보를 요청할 수도 있습니다. (이 경우는 무시)
					aryCurrentInfoList[cIndex].m_dwGID = 0;
					continue;
				}

				CGuild* lpGuild = it->second;
				if (NULL == lpGuild) { break; }

				for (unsigned short wRank = 0; wRank < wGuildNum; ++wRank)
				{
					if (sortVector[wRank].second == lpGuild)
					{
						aryCurrentInfoList[cIndex] = lpGuild->GetLargeInfo(cIndex, wRank + 1, lpMyGuild);
						break;
					}
				}
			}
			break;
		} 

		default:
		{
			for (unsigned char cIndex = 0; cIndex < PktGuildList::MAX_NUM_PER_PAGE; ++cIndex)
			{
				aryCurrentInfoList[cIndex].m_dwGID = 0;
			}

			return false;
		}
	}

	return true;
}

bool CGuildMgr::SerializeIn(char* lpBuffer_In, unsigned short wBufferSize_In, unsigned char cTotalMemberNum,
							unsigned char cRelationByMineNum, unsigned char cRelationByTargetNum)
{
	GuildInfoDB* lpGuildInfo = reinterpret_cast<GuildInfoDB *>(lpBuffer_In);
	CGuild* lpGuild = new CGuild(*lpGuildInfo);
	if (false == m_GuildMap.insert(GuildMap::value_type(lpGuild->GetGID(), lpGuild)).second)
	{
		delete lpGuild;
		return false;
	}

	if (false == m_GuildNameMap.insert(GuildNameMap::value_type(std::string(lpGuild->GetName()), lpGuild)).second)
	{
		m_GuildMap.erase(lpGuild->GetGID());
		delete lpGuild;
		return false;
	}

	int nIndex = 0;

	GuildMemberDB* lpGuildMember = reinterpret_cast<GuildMemberDB *>(lpGuildInfo + 1);
    for (nIndex = 0; nIndex < cTotalMemberNum; ++nIndex, ++lpGuildMember)
	{
		lpGuild->JoinMemberDB(*lpGuildMember);
	}

	GuildRelationDB* lpGuildRelation = reinterpret_cast<GuildRelationDB*>(lpGuildMember);
	for (nIndex = 0; nIndex < cRelationByMineNum + cRelationByTargetNum; ++nIndex, ++lpGuildRelation)
	{
		lpGuild->InsertRelation(lpGuildRelation->m_dwGID, lpGuildRelation->m_dwTargetGID, lpGuildRelation->m_cRelation);
	}

	return true;	
}

bool CGuildMgr::SerializeOut(CGuild* lpGuild_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out, 
							 unsigned char& cTotalMemberNum, unsigned char& cRelationByMineNum,	unsigned char& cRelationByTargetNum)
{
	wBufferSize_Out = 0;
	cTotalMemberNum = cRelationByMineNum = cRelationByTargetNum = 0;

	// 길드 데이터
	GuildInfoDB* lpGuildInfoDB = reinterpret_cast<GuildInfoDB *>(lpBuffer_Out);

	lpGuildInfoDB->m_dwGID = lpGuild_In->m_dwGID;

	strncpy(lpGuildInfoDB->m_strName, lpGuild_In->m_strName, MAX_GUILD_NAME_LEN);

	lpGuildInfoDB->m_cInclination = lpGuild_In->m_cInclination;
	lpGuildInfoDB->m_cLevel = lpGuild_In->m_cLevel;

	lpGuildInfoDB->m_dwFame = lpGuild_In->m_dwFame;
	lpGuildInfoDB->m_dwGold = lpGuild_In->m_dwGold;

	lpGuildInfoDB->m_tmLastLogout = lpGuild_In->m_tmLastLogout;
	lpGuildInfoDB->m_tmCheckMember = lpGuild_In->m_tmCheckMember;
	lpGuildInfoDB->m_tmGMLastLogout = lpGuild_In->m_tmGMLastLogout;
	lpGuildInfoDB->m_tmChangeInclination = lpGuild_In->m_tmChangeInclination;

	::memcpy(lpGuildInfoDB->m_szMark, lpGuild_In->m_szMark, MAX_MARK_SIZE);
	::memcpy(lpGuildInfoDB->m_szRight, &lpGuild_In->m_GuildRight, sizeof(GuildRight));

	wBufferSize_Out += sizeof(GuildInfoDB);

	// 멤버 데이터
	GuildMemberDB* lpGuildMemberDB = reinterpret_cast<GuildMemberDB *>(lpGuildInfoDB + 1);
	for (MemberList::iterator MemberIt = lpGuild_In->m_MemberList.begin(); 
		MemberIt != lpGuild_In->m_MemberList.end() && cTotalMemberNum < MAX_MEMBER_NUM; 
		++MemberIt, ++lpGuildMemberDB, ++cTotalMemberNum)
	{
		MemberInfo& memberInfo = *MemberIt;

		lpGuildMemberDB->m_dwGID = lpGuild_In->m_dwGID;
		lpGuildMemberDB->m_dwCID = memberInfo.m_dwCID;
	
		strncpy(lpGuildMemberDB->m_strName, memberInfo.m_strName, MAX_MEMBER_NAME_LEN);

		lpGuildMemberDB->m_dwRank = memberInfo.m_MemberListInfo.m_cRank;
		lpGuildMemberDB->m_dwTitle = memberInfo.m_MemberListInfo.m_cTitle;
		lpGuildMemberDB->m_cLevel = memberInfo.m_MemberListInfo.m_cLevel;
		lpGuildMemberDB->m_wClass = memberInfo.m_MemberListInfo.m_cClass;
		lpGuildMemberDB->m_cGuildWarFlag = memberInfo.m_MemberListInfo.m_cGuildWarFlag;

		lpGuildMemberDB->m_dwFame = memberInfo.m_MemberDetailInfo.m_dwFame;
		lpGuildMemberDB->m_dwGold = memberInfo.m_MemberDetailInfo.m_dwGold;

		lpGuildMemberDB->m_LeaveGuildTime	= memberInfo.m_LeaveGuildTime;
		lpGuildMemberDB->m_cTactics			= memberInfo.m_cTactics;

		wBufferSize_Out += sizeof(GuildMemberDB);
	}

	// 관계 데이터
	GuildRelationDB* lpGuildRelationDB = reinterpret_cast<GuildRelationDB *>(lpGuildMemberDB);
	for (RelationMap::iterator itr = lpGuild_In->m_Relation[Guild::BY_MINE].begin();
		 itr != lpGuild_In->m_Relation[Guild::BY_MINE].end() && cRelationByMineNum < MAX_RELATION_NUM;
		 ++itr, ++lpGuildRelationDB, ++cRelationByMineNum)
	{
		lpGuildRelationDB->m_dwGID			= lpGuild_In->m_dwGID;
		lpGuildRelationDB->m_dwTargetGID	= itr->first;
		lpGuildRelationDB->m_cRelation		= itr->second;

		wBufferSize_Out += sizeof(GuildRelationDB);
	}

	for (RelationMap::iterator itr = lpGuild_In->m_Relation[Guild::BY_TARGET].begin();
		 itr != lpGuild_In->m_Relation[Guild::BY_TARGET].end() && cRelationByTargetNum < MAX_RELATION_NUM;
		 ++itr, ++lpGuildRelationDB, ++cRelationByTargetNum)
	{
		lpGuildRelationDB->m_dwGID			= itr->first;
		lpGuildRelationDB->m_dwTargetGID	= lpGuild_In->m_dwGID;
		lpGuildRelationDB->m_cRelation		= itr->second;

		wBufferSize_Out += sizeof(GuildRelationDB);
	}

	return true;	
}