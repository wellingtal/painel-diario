
#include <Log/ServerLog.h>
#include <Utility/Math/Math.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <Castle/CastleMgr.h>
#include <Map/FieldMap/Cell.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Creature/Siege/SiegeConstants.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "GuildUtil.h"
#include "GuildMgr.h"
#include "Guild.h"


using namespace Guild;

bool Guild::CheckGuildName(const char* szGuildName)
{
    // ����, �ۼ�Ʈ, ��������ǥ, ū����ǥ, ��������, ��ǥ, ` 
    const char* szRestrictedGuildName = " %'\"\\,`";
    
    return strlen(szGuildName) == strcspn(szGuildName, szRestrictedGuildName);
}

CGuild::CGuild(void)
:	m_dwGID(0), m_cInclination(0), m_cLevel(0), m_dwFame(0), m_dwGold(0), m_cGuildPointFlag(0), m_cGuildSafe(-1), m_dwGuildMiles(0)
{
	std::fill_n(m_strName, int(MAX_GUILD_NAME_LEN), 0);
	std::fill_n(m_szMark, int(MAX_MARK_SIZE), 0);

	memset(&m_tmLastLogout, 0, sizeof(TIME));
	memset(&m_tmCheckMember, 0, sizeof(TIME));
	memset(&m_tmGMLastLogout, 0, sizeof(TIME));
	memset(&m_tmChangeInclination, 0, sizeof(TIME));
	
	// ������ �ð� ����.

	::memset(&m_tmGuildPoint, 0, sizeof(TIME));

	CTime		nowTime = CTime::GetCurrentTime();
	CTimeSpan	tm(0, 0, TIME_GUILD_POINT, 0);    
	CTime		tmPrev = nowTime-tm;

	m_cGuildPointFlag		= 0;
	m_tmGuildPoint.Year		= tmPrev.GetYear();
	m_tmGuildPoint.Month	= tmPrev.GetMonth();
	m_tmGuildPoint.Day		= tmPrev.GetDay();
	m_tmGuildPoint.Hour		= tmPrev.GetHour();
	m_tmGuildPoint.Minute	= tmPrev.GetMinute();
	m_tmGuildPoint.Second	= tmPrev.GetSecond();

	m_GuildRight = GuildRight(false);
}

CGuild::CGuild(unsigned long dwGID, unsigned char cInclination, char* szName)
:	m_dwGID(dwGID), m_cInclination(cInclination), m_cLevel(0), m_dwFame(0), m_dwGold(0), m_cGuildPointFlag(0), m_cGuildSafe(-1), m_dwGuildMiles(0)
{
	strncpy(m_strName, szName, MAX_GUILD_NAME_LEN);
	std::fill_n(m_szMark, int(MAX_MARK_SIZE), 0);

	memset(&m_tmLastLogout, 0, sizeof(TIME));
	memset(&m_tmCheckMember, 0, sizeof(TIME));
	memset(&m_tmGMLastLogout, 0, sizeof(TIME));
	memset(&m_tmChangeInclination, 0, sizeof(TIME));
	
	// ������ �ð� ����.

	::memset(&m_tmGuildPoint, 0, sizeof(TIME));

	CTime		nowTime = CTime::GetCurrentTime();
	CTimeSpan	tm(0, 0, TIME_GUILD_POINT, 0);    
	CTime		tmPrev = nowTime-tm;

	m_cGuildPointFlag		= 0;
	m_tmGuildPoint.Year		= tmPrev.GetYear();
	m_tmGuildPoint.Month	= tmPrev.GetMonth();
	m_tmGuildPoint.Day		= tmPrev.GetDay();
	m_tmGuildPoint.Hour		= tmPrev.GetHour();
	m_tmGuildPoint.Minute	= tmPrev.GetMinute();
	m_tmGuildPoint.Second	= tmPrev.GetSecond();

	m_GuildRight = GuildRight(false);
}

CGuild::CGuild(GuildInfoDB& guildInfo)
{
	m_cGuildSafe = -1;
	m_dwGID = guildInfo.m_dwGID;

	strncpy(m_strName, guildInfo.m_strName, MAX_GUILD_NAME_LEN);

	m_cInclination = guildInfo.m_cInclination;

	m_cLevel = guildInfo.m_cLevel;

	m_dwFame = guildInfo.m_dwFame;
	m_dwGold = guildInfo.m_dwGold;
	m_dwGuildMiles = 0;

	m_tmLastLogout = guildInfo.m_tmLastLogout;
	m_tmCheckMember = guildInfo.m_tmCheckMember;
	m_tmGMLastLogout = guildInfo.m_tmGMLastLogout;
	m_tmChangeInclination = guildInfo.m_tmChangeInclination;

	::memcpy(m_szMark, guildInfo.m_szMark, MAX_MARK_SIZE);
	::memcpy(&m_GuildRight, guildInfo.m_szRight, sizeof(GuildRight));

	// ������ �ð� ����.

	::memset(&m_tmGuildPoint, 0, sizeof(TIME));

	CTime		nowTime = CTime::GetCurrentTime();
	CTimeSpan	tm(0, 0, TIME_GUILD_POINT, 0);    
	CTime		tmPrev = nowTime-tm;
	
	m_cGuildPointFlag		= 0;
	m_tmGuildPoint.Year		= tmPrev.GetYear();
	m_tmGuildPoint.Month	= tmPrev.GetMonth();
	m_tmGuildPoint.Day		= tmPrev.GetDay();
	m_tmGuildPoint.Hour		= tmPrev.GetHour();
	m_tmGuildPoint.Minute	= tmPrev.GetMinute();
	m_tmGuildPoint.Second	= tmPrev.GetSecond();
}

CGuild::~CGuild(void)
{
}

void CGuild::ChangeMemberName(unsigned long dwCID, const char* szChangeName)
{
    MemberList::iterator pos = m_MemberList.begin();
    MemberList::iterator end = m_MemberList.end();

    for(; pos != end; ++pos)
    {
        MemberInfo& memberInfo = *pos;

        if (memberInfo.m_dwCID == dwCID)
        {
            strncpy(memberInfo.m_strName, szChangeName, Guild::MAX_MEMBER_NAME_LEN);
            memberInfo.m_strName[ Guild::MAX_MEMBER_NAME_LEN - 1 ] = 0;
            break;
        }
    }
}

unsigned short CGuild::SetMark(unsigned long dwSenderID, char *szMark)
{
	if (NULL == szMark)
	{
		ERRLOG1(g_Log, "CID:0x%08x ������ ��ũ�� �����ϴ�.", dwSenderID);
		return PktGuildMark::FAIL_INVALID_MARK;
	}

	CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter(dwSenderID);
	if (NULL == lpMaster)
	{
		ERRLOG1(g_Log, "CID:0x%08x ��� ��ũ�� �����Ϸ��� ĳ���Ͱ� �������� �ʽ��ϴ�.", dwSenderID);
		return PktGuildMark::FAIL_INVALID_CHARACTER;
	}

	// GM�� ��� ����� ��ũ�� ������ �� �ִ�.
	if (0 == lpMaster->GetAdminLevel())
	{
		if (dwSenderID != GetMaster().m_dwCID)
		{
			ERRLOG1(g_Log, "CID:0x%08x �����Ͱ� �ƴ� ĳ���Ͱ� ��� ��ũ�� �����Ϸ� �մϴ�.", dwSenderID);
			return PktGuildMark::FAIL_NOT_MASTER;
		}
	}

	if (Guild::SET_MARK_GOLD > m_dwGold)
	{
		ERRLOG1(g_Log, "GID:0x%08x ��� ��ũ�� ������ �ݾ��� �����մϴ�.", m_dwGID);
		return PktGuildMark::FAIL_NOT_ENOUGH_GOLD;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (NULL != lpDBAgentDispatch)
	{
		if (true == GameClientSendPacket::SendCharGuildMark(lpDBAgentDispatch->GetSendStream(), 
			dwSenderID, m_dwGID, szMark, Guild::SET_MARK_GOLD, PktBase::NO_SERVER_ERR))
		{
			return PktBase::NO_SERVER_ERR;
		}
	}

	return PktBase::SERVER_ERROR;
}

unsigned short CGuild::SetMark(unsigned long dwSenderID, char *szMark, unsigned long dwGold)
{
	::memcpy(m_szMark, szMark, MAX_MARK_SIZE);
	m_dwGold = dwGold;

	PktGuildMark pktGM;
	pktGM.m_dwCID = dwSenderID;
	pktGM.m_dwGID = m_dwGID;
	::memcpy(pktGM.m_szMark, m_szMark, MAX_MARK_SIZE);
	pktGM.m_dwGold = m_dwGold;

	char* szPacket = reinterpret_cast<char *>(&pktGM);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildMark), CmdGuildMark, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktGuildMark), CmdGuildMark);
	}

	return 0;
}

bool CGuild::SetLevel(unsigned long dwMaster, unsigned char cLevel)
{
	if (dwMaster != GetMaster().m_dwCID)
	{
		ERRLOG1(g_Log, "CID:0x%08x �����Ͱ� �ƴ� ������ ��� ������ �����Ϸ� �մϴ�.", dwMaster);
		return false;
	}

	if (Guild::MAX_LEVEL <= cLevel)
	{
		ERRLOG1(g_Log, "GID:0x%08x ���� ��ġ�� �ʰ��� ��� ������ �Ƿ��մϴ�.", m_dwGID);
		return false;
	}

	const unsigned long dwSetLevelFame = ms_arySetLevelFame[cLevel];

	if (dwSetLevelFame > m_dwFame)
	{
		ERRLOG1(g_Log, "GID:0x%08x ��� ������ ������ ���� �����մϴ�.", m_dwGID);
		return false;
	}
/*
	const unsigned long dwSetLevelGold = 
		(GameRYL::CHINA == CServerSetup::GetInstance().GetNationType()) ? 
		ms_arySetLevelGoldForChina[cLevel] : ms_arySetLevelGold[cLevel];
*/
	const unsigned long dwSetLevelGold = ms_arySetLevelGold[cLevel];

	if (dwSetLevelGold > m_dwGold)
	{
		ERRLOG1(g_Log, "GID:0x%08x ��� ������ ������ �ݾ��� �����մϴ�.", m_dwGID);
		return false;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (NULL != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildLevel(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, cLevel, dwSetLevelGold, PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool CGuild::SetLevel(unsigned char cLevel, unsigned long dwGold)
{
	m_cLevel = cLevel;
	m_dwGold = dwGold;

	PktGuildLevel pktGL;
	pktGL.m_dwUID = m_dwGID;
	pktGL.m_cLevel = m_cLevel;
	pktGL.m_dwGold = dwGold;

	char* szPacket = reinterpret_cast<char *>(&pktGL);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildLevel), CmdGuildLevel, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildLevel), CmdGuildLevel);
	}

	return true;
}

bool CGuild::SetRight(unsigned long dwMaster, GuildRight guildRight)
{
	if (dwMaster != GetMaster().m_dwCID)
	{
		ERRLOG1(g_Log, "CID:0x%08x �����Ͱ� �ƴ� ������ ��� ������ �����Ϸ� �մϴ�.", dwMaster);
		return false;
	}

	int nResult = guildRight.IsValid();
	if (MAX_RIGHT_SIZE != nResult)
	{
		ERRLOG3(g_Log, "CID:0x%08x �������� ��� �������� �����Ϸ� �մϴ�. ����:%d/%d", 
			dwMaster, nResult, guildRight.m_aryRight[nResult]);
		return false;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildRight(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, guildRight, PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool CGuild::SetRight(GuildRight guildRight)
{
	m_GuildRight = guildRight;

	PktGuildRight pktGR;
	pktGR.m_dwUID = m_dwGID;
	pktGR.m_GuildRight = guildRight;

	char* szPacket = reinterpret_cast<char *>(&pktGR);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildRight), CmdGuildRight, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildRight), CmdGuildRight);
	}

	return true;
}

bool CGuild::SetTitle(unsigned long dwSuperior, unsigned long dwFollower, unsigned char cTitle, unsigned short &wError)
{
	MemberList::iterator superiorIt = m_MemberList.end();
	MemberList::iterator followerIt = m_MemberList.end();

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwSuperior == it->m_dwCID)
		{
			superiorIt = it;
		}

		if (dwFollower == it->m_dwCID)
		{
			followerIt = it;
		}
	}

	if (superiorIt == m_MemberList.end() || followerIt == m_MemberList.end())
	{
		ERRLOG3(g_Log, "GID:0x%08x ������ �ƴ� ĳ���Ͱ� ������ �ٲٷ� �մϴ�. SuperiorID:0x%08x, FollowerID:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberInfo& superiorInfo = *superiorIt;
	MemberInfo& followerInfo = *followerIt;

	// �߰� ������ ����/����
	if (MIDDLE_ADMIN == followerInfo.m_MemberListInfo.m_cTitle)
	{
		if (MASTER != superiorInfo.m_MemberListInfo.m_cTitle && dwSuperior != dwFollower)
		{
			ERRLOG2(g_Log, "CID:0x%08x �����ͳ� �߰������ڰ� �ƴ� ������ �߰� ������ ����/�����Ϸ� �մϴ�.", 
				superiorInfo.m_dwCID, followerInfo.m_dwCID);
			wError = PktBase::SERVER_ERROR;
			return false;	
		}		
	}

	// �߰� ������ �Ӹ�
	if (MIDDLE_ADMIN == cTitle)
	{
		if (MASTER != superiorInfo.m_MemberListInfo.m_cTitle)
		{
			ERRLOG1(g_Log, "CID:0x%08x �����Ͱ� �ƴ� ������ �߰� �����ڸ� �Ӹ��Ϸ� �մϴ�.", superiorInfo.m_dwCID);
			wError = PktBase::SERVER_ERROR;
			return false;	
		}
	}

	// ���� ���� �㰡
	if (JOIN_WAIT == followerInfo.m_MemberListInfo.m_cTitle)
	{
		if (superiorInfo.m_MemberListInfo.m_cTitle > m_GuildRight.m_aryRight[GuildRight::PERMIT_JOIN])
		{
			ERRLOG1(g_Log, "CID:0x%08x ������ ���� ������ ���� ���� �㰡�� �Ϸ� �մϴ�.", superiorInfo.m_dwCID);
			wError = PktBase::SERVER_ERROR;
			return false;	
		}

		// edith 2009.02.28 ���� CGuild::SetTitle(unsigned long dwCID, unsigned char cTitle)���� �Ϲݱ�������
		// �ٲ��ָ� �׶��� �޸𸮰� 3���� �ٲ�µ� ���� �޸𸮴� 5�� �����־� ��� ��������̴�.
		// �׷��� ���⼭ 3���� �ٲٸ� �޸𸮻󿡼� �ٷ� 3���� �ٲ��. 
		// �켱 �㰡�� �Ѵ�.
		followerInfo.m_MemberListInfo.m_cTitle = cTitle;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwFollower, cTitle, "��������", m_strName, PktGuildCmd::GC_SETUP_TITLE, PktBase::NO_SERVER_ERR);
	}

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::SetTitle(unsigned long dwCID, unsigned char cTitle)
{
	MemberList::iterator followerIt = m_MemberList.end();
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			followerIt = it;
		}
	}

	if (followerIt == m_MemberList.end()) 
	{ 
		ERRLOG2(g_Log, "GID:0x%08x ������ �ƴ� ĳ������ ������ �ٲٷ� �մϴ�. FollowerID:0x%08x", 
			m_dwGID, dwCID);
		return false; 
	}

	MemberInfo& memberInfo = *followerIt;

	memberInfo.m_MemberListInfo.m_cTitle = cTitle;
	
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
	if (NULL != lpCharacter)
	{
		// ä�� ������ ���� ������Ʈ
		lpCharacter->SetGID(m_dwGID);
	}

	PktGuildCmd pktGC;

	pktGC.m_dwGID = m_dwGID;

	pktGC.m_dwSenderID = dwCID;
	pktGC.m_dwReferenceID = cTitle;

	strncpy(pktGC.m_szGuildName, m_strName, MAX_GUILD_NAME_LEN);
	strncpy(pktGC.m_szSenderName, memberInfo.m_strName, MAX_MEMBER_NAME_LEN);

	pktGC.m_wCmd = PktGuildCmd::GC_SETUP_TITLE;

	char* szPacket = reinterpret_cast<char *>(&pktGC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
	}

	CalculateGuildFame();

	return true;
}

void CGuild::SetGuildMiles(unsigned long dwGuildMiles)
{
	m_dwGuildMiles = dwGuildMiles;
}

bool CGuild::SetTacticsTitle(unsigned long dwCID, unsigned char cTitle)
{
	MemberList::iterator followerIt = m_MemberList.begin();
	for (; followerIt != m_MemberList.end(); ++followerIt)
	{
		if (dwCID == followerIt->m_dwCID)
		{
			break;
		}
	}

	if (followerIt == m_MemberList.end()) 
	{ 
		ERRLOG2(g_Log, "GID:0x%08x ������ �ƴ� ĳ������ ������ �ٲٷ� �մϴ�. FollowerID:0x%08x", 
			m_dwGID, dwCID);
		return false; 
	}

	MemberInfo& memberInfo = *followerIt;

	memberInfo.m_MemberListInfo.m_cTitle = cTitle;	
	memberInfo.m_cTactics = Guild::TACTICS;

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
	if (NULL != lpCharacter)
	{
		// ä�� ������ ���� ������Ʈ
		lpCharacter->SetGID(m_dwGID);
		lpCharacter->SetTacticsFlag(Guild::TACTICS);
	}

	PktGuildCmd pktGC;

	pktGC.m_dwGID = m_dwGID;

	pktGC.m_dwSenderID = dwCID;
	pktGC.m_dwReferenceID = cTitle;

	strncpy(pktGC.m_szGuildName, m_strName, MAX_GUILD_NAME_LEN);
	strncpy(pktGC.m_szSenderName, memberInfo.m_strName, MAX_MEMBER_NAME_LEN);

	pktGC.m_wCmd = PktGuildCmd::GC_TACTICS_TITLE;

	char* szPacket = reinterpret_cast<char *>(&pktGC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
	}

	CalculateGuildFame();
	return true;
}


bool CGuild::InsertRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelation)
{
	if (dwGID == m_dwGID)
	{
		return m_Relation[Guild::BY_MINE].insert( std::make_pair(dwTargetGID, cRelation) ).second;
	}
	else if (dwTargetGID == m_dwGID)
	{
		return m_Relation[Guild::BY_TARGET].insert( std::make_pair(dwGID, cRelation) ).second;
	}

	return false;
}

bool CGuild::SetRelation(unsigned long dwTargetGID, unsigned char cRelationType, unsigned char cActorType)
{
	PktGuildRelation pktGR;
	ZeroMemory(&pktGR, sizeof(PktGuildRelation));

	CGuild* lpTargetGuild = CGuildMgr::GetInstance().GetGuild(dwTargetGID);
	if (0 != lpTargetGuild)
	{
		strncpy(pktGR.m_szGuildName, lpTargetGuild->GetName(), Guild::MAX_GUILD_NAME_LEN);
	}

	switch (cRelationType)
	{
		case Guild::HOSTILITY:			pktGR.m_cSubCmd = PktGuildRelation::GR_HOSTILITY;		break;
		case Guild::ALERT_HOSTILITY:	pktGR.m_cSubCmd = PktGuildRelation::GR_ALERT;			break;
		case Guild::COUNTER_HOSTILITY:	pktGR.m_cSubCmd = PktGuildRelation::GR_COUNTER;			break;
		case Guild::NEUTRALITY:			pktGR.m_cSubCmd = PktGuildRelation::GR_PEACE;			break;
		case Guild::TARGET_NEUTRALITY:	pktGR.m_cSubCmd = PktGuildRelation::GR_HOSTILITYDEL;	break;
	}

	switch (cActorType)
	{
		case Guild::BY_MINE:
		{
			if(cRelationType == Guild::TARGET_NEUTRALITY)
			{
				pktGR.m_dwGID = dwTargetGID;
				pktGR.m_dwTargetGID = GetGID();
			}
			else
			{
				pktGR.m_dwGID = GetGID();
				pktGR.m_dwTargetGID = dwTargetGID;
			}
			break;
		}

		case Guild::BY_TARGET:
		{
			if(cRelationType == Guild::TARGET_NEUTRALITY)
			{
				pktGR.m_dwGID = GetGID();
				pktGR.m_dwTargetGID = dwTargetGID;
			}
			else
			{
				pktGR.m_dwGID = dwTargetGID;
				pktGR.m_dwTargetGID = GetGID();
			}
			break;
		}
	}
	
	switch (cRelationType)
	{
		case Guild::TARGET_NEUTRALITY:
		{
			if(cActorType == Guild::BY_TARGET)
			{
				RelationMap::iterator itr = m_Relation[cActorType].find(dwTargetGID);
				if (itr != m_Relation[cActorType].end())
				{
					m_Relation[cActorType].erase(itr);
				}
				else
				{
					return false;
				}
			}
			break;
		}

		case Guild::HOSTILITY:
		case Guild::ALERT_HOSTILITY:
		case Guild::COUNTER_HOSTILITY:
		{
			RelationMap::iterator itr = m_Relation[cActorType].find(dwTargetGID);
			if (itr != m_Relation[cActorType].end())
			{
				itr->second = cRelationType;
			}
			else
			{
				m_Relation[cActorType].insert(std::make_pair(dwTargetGID, cRelationType)).second;
			}

			break;
		}

		case Guild::NEUTRALITY:
		{
			if(cActorType == Guild::BY_MINE) 
			{
				RelationMap::iterator itr = m_Relation[cActorType].find(dwTargetGID);
				if (itr != m_Relation[cActorType].end())
				{
					m_Relation[cActorType].erase(itr);
				}
				else
				{
					return false;
				}
			}
			if(cActorType == Guild::BY_TARGET) 
			{
				RelationMap::iterator itr = m_Relation[cActorType].find(dwTargetGID);
				if (itr != m_Relation[cActorType].end())
				{
					itr->second = Guild::TARGET_NEUTRALITY;
				}
				else
				{
					return false;
				}
			}

			break;
		}
	}

	char* szPacket = reinterpret_cast<char *>(&pktGR);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildRelation), CmdGuildRelation, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildRelation), CmdGuildRelation);
	}

	return true;
}

unsigned char	CGuild::GetRelation(unsigned long dwGID, unsigned char cActorType)
{
	if (GetGID() == dwGID) return MINE;

	RelationMap::iterator itr = m_Relation[cActorType].find(dwGID);
	if (itr != m_Relation[cActorType].end())
	{
		return itr->second;
	}

	return NEUTRALITY;
}

int	CGuild::GetNumOfRelation(unsigned char cRelationType, unsigned char cActorType)
{
	if (Guild::ALL_HOSTILITY == cRelationType)
	{
		return static_cast<int>(m_Relation[cActorType].size());
	}

	int iCount = 0;
	RelationMap::iterator itr = m_Relation[cActorType].begin();
	while (itr != m_Relation[cActorType].end())
	{
		if (itr->second == cRelationType)
		{
			++iCount ;
		}

		++ itr ;
	}

	return iCount ;
}

unsigned long	CGuild::GetRelationGID(unsigned char cRelationType, unsigned char cActorType)
{
	RelationMap::iterator itr = m_Relation[cActorType].begin();
	while (itr != m_Relation[cActorType].end())
	{
		if (itr->second == cRelationType)
		{
			return itr->first;
		}

		++ itr ;
	}

	return 0;
}


void CGuild::CalculateGuildFame(void)
{
	const unsigned long dwFameSum = std::accumulate(m_MemberList.begin(), m_MemberList.end(), 0, SumMemberFame());

	// edith 2008.10.16 ��� ���� ���� ������ �������� ��ü������ ��ġ ����ϰ� ����.
//	const unsigned char cCurrentMemberNum = GetCurrentMemberNum();
//	m_dwFame = (0 != cCurrentMemberNum) ? (dwFameSum / cCurrentMemberNum) : 0;

	const unsigned char cMemberNum = ms_aryMaxMemberNum[m_cLevel];
	m_dwFame = (0 != cMemberNum) ? (dwFameSum / cMemberNum) : 0;
}

MemberInfo CGuild::GetMaster(void)
{
	if (true == m_MemberList.empty()) 
	{
		ERRLOG1(g_Log, "GID:0x%08x ���� ����Դϴ�.", m_dwGID);
		return MemberInfo(); 
	}

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;

		if (MASTER == memberInfo.m_MemberListInfo.m_cTitle)
		{
			return memberInfo;
		}
	}

	ERRLOG1(g_Log, "GID:0x%08x ��� �����Ͱ� �����ϴ�.", m_dwGID);
	return MemberInfo();
}

unsigned char CGuild::GetTitle(unsigned long dwCID)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			MemberInfo& memberInfo = *it;
			return memberInfo.m_MemberListInfo.m_cTitle;
		}
	}

	return MAX_TITLE;
}

const unsigned char CGuild::GetTotalMemberNum(void) const
{
	return static_cast<unsigned char>(m_MemberList.size());
}

const unsigned char CGuild::GetCurrentMemberNum(void) const
{
	return static_cast<unsigned char>(std::count_if(m_MemberList.begin(), m_MemberList.end(), IsCurrentMember()));
}

const unsigned char CGuild::GetMemberNum(void) const
{
	return static_cast<unsigned char>(std::count_if(m_MemberList.begin(), m_MemberList.end(), IsNotTacticsMember()));
}

const unsigned char CGuild::GetCurrentAllMember(void) const
{
	return GetCurrentMemberNum() + GetTacticsNum();
}

const unsigned char CGuild::GetTacticsNum(void) const
{
	return static_cast<unsigned char>(std::count_if(m_MemberList.begin(), m_MemberList.end(), IsTacticsMember()));
}

const unsigned char CGuild::GetLoginMemberNum(void) const	// �������� ������
{
	return static_cast<unsigned char>(std::count_if(m_MemberList.begin(), m_MemberList.end(), IsCurrentLoginMember()));
}

const unsigned char CGuild::GetMaxMemberNum(void) const		// �ִ� ������
{
	return ms_aryMaxMemberNum[m_cLevel];
}

const float CGuild::GetGuildBonus(unsigned char cRealmPoint, float fPercent)	// ��� ���ʽ� ���� �Լ�
{
	unsigned char MaxNum = GetMaxMemberNum();
	// edith 2010.01.11 ��� ���ʽ��� ���� �ο����� �ƴ� �α������� ����ο����� ���ʽ��� �ް� ����.
	unsigned char CurNum = GetLoginMemberNum();
//	unsigned char CurNum = GetCurrentMemberNum();

	return ((float)cRealmPoint * fPercent) * ( (float)CurNum / (float)MaxNum ) / 100.0f;
}

const bool CGuild::IsPeaceMode(void)
{
/*
	// ���� ��������...
	if (Creature::ALMIGHTY_PIRATE == m_cInclination)
	{
		// ���� ������ ������ ��ȭ ���
		if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
		{
			return true;
		}
	}
*/
/*
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
	{
		// ī������ �ִ��� ����� �ð��� �ƴҶ� ��ȭ ���
		if (SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone() && 
			!CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			return true;
		}
	}
*/
	return false;
}

bool CGuild::AddGold(unsigned long dwGold)
{
	if (m_dwGold <= ULONG_MAX - dwGold)
	{
		m_dwGold += dwGold;
		return true;
	}

	ERRLOG2(g_Log, "GID:0x%08x ��� �ݰ� �����÷ο찡 �߻��߽��ϴ�. : %dGold", m_dwGID, dwGold);
	return false;
}

bool CGuild::DeductGold(unsigned long dwGold)
{
	if (dwGold <= m_dwGold)
	{ 
		m_dwGold -= dwGold;
		return true;
	}

	ERRLOG2(g_Log, "GID:0x%08x ��� �ݰ� ����÷ο찡 �߻��߽��ϴ�. : %dGold", m_dwGID, dwGold);
	return false;
}

void CGuild::ReleaseGold(unsigned long dwGold)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (true == IsCurrentMember()(*it))
		{
			MemberInfo& memberInfo = *it;
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
			if (NULL != lpCharacter)
			{
				lpCharacter->AddGold(dwGold, false);
			}
		}
	}
}

bool CGuild::AutoRouting(CAggresiveCreature* lpPickkingCreature, Item::CItem* lpItem, unsigned long& dwGold)
{
	if (!lpPickkingCreature) { return false; }

	CCharacter *aryNearCharacterList[ 50 ];

	// ���� ���ѿ� ������� ��ó ��Ƽ�� ��ο��� �й�
	const int nNearMemberNum = GetNearMemberList(lpPickkingCreature->GetCellPos().m_lpCell, aryNearCharacterList);
	if (0 == nNearMemberNum) { return false; }

	if (0 != dwGold)
	{
		unsigned long dwEachGold = dwGold / nNearMemberNum;
		dwGold = dwEachGold + (dwGold - dwEachGold * nNearMemberNum);

		for (int nIndex = 0; nIndex < nNearMemberNum; ++nIndex)
		{
			CCharacter* lpNearCharacter = aryNearCharacterList[nIndex];
			if (lpNearCharacter == lpPickkingCreature) { continue; }

			lpNearCharacter->AddGold(dwEachGold, true);
		}

		return false;
	}
	else if (NULL != lpItem)
	{
		// ���� ���� ���
		unsigned char cRandomNum = 
			static_cast<unsigned char>(Math::Random::ComplexRandom(nNearMemberNum));
		CCharacter* lpOrderCharacter = aryNearCharacterList[cRandomNum];

		if (lpPickkingCreature == lpOrderCharacter)
		{
			return false;
		}
		else
		{
			if (true == lpOrderCharacter->GiveItem(lpItem))
			{
				PktAutoRouting pktAR;
				pktAR.m_dwCharID = lpOrderCharacter->GetCID();
				pktAR.m_nObjectID = 0;

				pktAR.m_wItemID = lpItem->GetPrototypeID();
				pktAR.m_cNum = 0;

				pktAR.m_cCmd = PktAutoRouting::ARC_POSSIBLE;

				char* szPacket = reinterpret_cast<char*>(&pktAR);
				if (PacketWrap::WrapCrypt(szPacket, sizeof(PktAutoRouting), CmdCharAutoRouting, 0, 0))
				{
					SendAllMember(szPacket, sizeof(PktAutoRouting), CmdCharAutoRouting);
				}

				// GievItem ���� ���õ� ���, AutoRouting() ȣ���� ������ Item �� �����Ѵ�.
				return true;
			}
		}
	}

	return false;
}

int  CGuild::GetNearMemberList(CCell *pCell, CCharacter **aryNearCharacterList)
{
	if (NULL == pCell) { return 0; }

	int nNearMemberNum = 0;
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		const MemberInfo& memberInfo = *it;

		if (IsCurrentMember()(memberInfo))
		{
			CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
			if (NULL != lpMember)
			{
				if (0 == lpMember->GetStatus().m_nNowHP)
				{
					continue;
				}

				if (pCell->IsNearCell(lpMember->GetCellPos().m_lpCell))
				{
					aryNearCharacterList[nNearMemberNum] = lpMember;
					++nNearMemberNum;
				}
			}
		}
	}

	return nNearMemberNum;
}

void CGuild::UpgradeMemberRespawnSpeedByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;

		if (true == IsCurrentMember()(memberInfo))
		{
			CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
			if (NULL != lpMember)
			{
				lpMember->UpgradeRespawnSpeedByEmblem(cUpgradeType, cUpgradeStep);
			}
		}
	}
}

void CGuild::DegradeMemberRespawnSpeedByEmblem()
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;

		if (true == IsCurrentMember()(memberInfo))
		{
			CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
			if (NULL != lpMember)
			{
				lpMember->DegradeRespawnSpeedByEmblem();
			}
		}
	}
}


bool CGuild::IsEnemyGuild(unsigned long dwGID)
{
	if (!m_Relation[Guild::BY_MINE].empty())
	{
		RelationMap::iterator itr = m_Relation[Guild::BY_MINE].find(dwGID);
		if (itr != m_Relation[Guild::BY_MINE].end())
		{
			if (Guild::HOSTILITY == itr->second ||
				Guild::TARGET_NEUTRALITY == itr->second ||
				Guild::ALERT_HOSTILITY == itr->second ||
				Guild::COUNTER_HOSTILITY == itr->second)
			{
				return true;
			}
		}
	}

	if (!m_Relation[Guild::BY_TARGET].empty())
	{
		RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].find(dwGID);
		if (itr != m_Relation[Guild::BY_TARGET].end())
		{
			if (Guild::HOSTILITY == itr->second ||
				Guild::TARGET_NEUTRALITY == itr->second ||
				Guild::ALERT_HOSTILITY == itr->second ||
				Guild::COUNTER_HOSTILITY == itr->second)
			{
				return true;
			}
		}
	}

	return false;
}

bool CGuild::InviteMember(unsigned long dwMember, unsigned long dwGuest, unsigned short &wError)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwMember == it->m_dwCID)
		{
			break;
		}
	}

	if (it == m_MemberList.end()) 
	{
		ERRLOG2(g_Log, "GID:0x%08x ������ �ƴ� ĳ���Ͱ� �ʴ��Ϸ� �մϴ�. MemberID:0x%08x", 
			m_dwGID, dwMember);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberInfo& memberInfo = *it;
	if (memberInfo.m_MemberListInfo.m_cTitle > m_GuildRight.m_aryRight[GuildRight::INVITE_MEMBER])
	{
		ERRLOG4(g_Log, "GID:0x%08x �ʴ� ������ ���� ĳ���Ͱ� �ʴ��Ϸ� �մϴ�. CID:0x%08x, Title:%d, Right:%d", 
			m_dwGID, dwMember, memberInfo.m_MemberListInfo.m_cTitle, m_GuildRight.m_aryRight[GuildRight::INVITE_MEMBER]);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	CCharacter* lpGuest = CCreatureManager::GetInstance().GetCharacter(dwGuest);
	if (NULL == lpGuest)
	{
		ERRLOG3(g_Log, "GID:0x%08x �������� �ʴ� ĳ���͸� ��忡 ��û�Ͽ����ϴ�. MemberID:0x%08x, GuestID:0x%08x", 
			m_dwGID, dwMember, dwGuest);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	CGameClientDispatch* lpDispatch = lpGuest->GetDispatcher();
	if (NULL == lpDispatch)
	{
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	if (GetNation() != lpGuest->GetNation())
	{
		wError = PktGuildCmd::FAIL_NATION;

		// �ʴ븦 ���� ������Ե� ���� ������ �ٲٶ�� ���� �޼����� ������.
		GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
			m_dwGID, dwGuest, dwMember, lpGuest->GetCharacterName(), m_strName, PktGuildCmd::GC_INVITE, wError);

		return false;
	}

	if (0 != lpGuest->GetGID())
	{
		wError = PktGuildCmd::FAIL_ALREADY_MEMBER;
		return false;
	}

	if (ms_aryMaxMemberNum[m_cLevel] <= GetCurrentAllMember())
	{
		wError = PktGuildCmd::FAIL_OVER_MEMBER;
		return false;
	}

	return GameClientSendPacket::SendCharGuildCmd(lpDispatch->GetSendStream(), 
		m_dwGID, dwGuest, dwMember, lpGuest->GetCharacterName(), m_strName, PktGuildCmd::GC_INVITE, PktBase::NO_SERVER_ERR);
}


bool CGuild::JoinMember(unsigned long dwCID, unsigned char cTitle, unsigned short &wError)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == lpCharacter)
	{
		ERRLOG2(g_Log, "GID:0x%08x �������� �ʴ� ĳ���Ͱ� ��忡 �����Ϸ� �մϴ�. CID:0x%08x", 
			m_dwGID, dwCID);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	// edith 2008.02.27 ��尡�� ���� ���� ����
	if (Creature::LEVEL_ABLE_WAR > lpCharacter->GetLevel())
	{
		ERRLOG2(g_Log, "GID:0x%08x ��忡 ������ �� ���� �����Դϴ�. CID:0x%08x", 
			m_dwGID, dwCID);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	if (GetNation() != lpCharacter->GetNation())
	{
		ERRLOG4(g_Log, "GID:0x%08x '���� ����'�� �ٸ� ������ ���� ��忡�� �����Ͻ� �� �����ϴ�. "
			"CID:0x%08x, ��������:%d, ��屹��:%d", m_dwGID, dwCID, lpCharacter->GetNation(), GetNation());
		wError = PktGuildCmd::FAIL_NATION;
		return false;
	}

	if (0 != lpCharacter->GetGID())
	{
		wError = PktGuildCmd::FAIL_ALREADY_MEMBER;
		return false;
	}

	if (ms_aryMaxMemberNum[m_cLevel] <= GetCurrentAllMember())
	{
		wError = PktGuildCmd::FAIL_OVER_MEMBER;
		return false;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwCID, cTitle, "��������", m_strName, PktGuildCmd::GC_JOIN, PktBase::NO_SERVER_ERR);
	}

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::TacticsMember(unsigned long dwCID, unsigned char cTitle, unsigned char cType, unsigned short &wError)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);

	if (!lpCharacter)
	{	
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	if (lpCharacter->GetLevel()<Guild::TACTICE_JON_LEVEL)
	{
		wError = PktGuildCmd::FAIL_TACTICS_LIMITLEVEL;
		return false;
	}

	if (GetNation() != lpCharacter->GetNation())
	{
		wError = PktGuildCmd::FAIL_NATION;
		return false;
	}

	if(lpCharacter->GetGID())
	{
		wError = PktGuildCmd::FAIL_ALREADY_MEMBER;
		return false;
	}

	if (ms_aryMaxMemberNum[m_cLevel] <= GetCurrentAllMember())
	{
		wError = PktGuildCmd::FAIL_TACTICS_OVER_MEMBER;
		return false;
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
		
	if (lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwCID, cTitle, "", m_strName, cType, PktBase::NO_SERVER_ERR);
	}	

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::SetTacticsTitle(unsigned long dwSuperior, unsigned long dwFollower, unsigned short wCmd, unsigned char cTitle, unsigned char cType, unsigned short &wError)
{
	MemberList::iterator superiorIt = m_MemberList.end();
	MemberList::iterator followerIt = m_MemberList.end();

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwSuperior == it->m_dwCID)
		{
			superiorIt = it;
		}

		if (dwFollower == it->m_dwCID)
		{
			followerIt = it;
		}
	}

	if (superiorIt == m_MemberList.end() || followerIt == m_MemberList.end())
	{
		ERRLOG3(g_Log, "GID:0x%08x ������ �ƴ� ĳ���Ͱ� ������ �ٲٷ� �մϴ�. SuperiorID:0x%08x, FollowerID:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberInfo& superiorInfo = *superiorIt;
	MemberInfo& followerInfo = *followerIt;

	if(cTitle!=COMMON)
	{
		return false;
	}

	if(followerInfo.m_cTactics!=Guild::TACTICS_WAIT)
	{
		return false;
	}

	if(!cType)
	{
		// ���� ���� �㰡
		if (JOIN_WAIT == followerInfo.m_MemberListInfo.m_cTitle)
		{
			if (superiorInfo.m_MemberListInfo.m_cTitle > Guild::MIDDLE_ADMIN)
			{
				ERRLOG1(g_Log, "CID:0x%08x ������ ���� ������ ���� ���� �㰡�� �Ϸ� �մϴ�.", superiorInfo.m_dwCID);
				wError = PktBase::SERVER_ERROR;
				return false;	
			}
		}
	}	

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwFollower, cTitle, "��������", m_strName, wCmd, PktBase::NO_SERVER_ERR);
	}

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::JoinMemberDB(GuildMemberDB& guildMemberDB)
{
	if (GetCurrentAllMember() >= ms_aryMaxMemberNum[m_cLevel]) 
	{ 
		ERRLOG1(g_Log, "GID:0x%08x ��� ��� �߰� ���� : ������ �ʰ��Ͽ����ϴ�.", m_dwGID);
		return false; 
	}

	MemberListInfo memberListInfo(static_cast<unsigned char>(guildMemberDB.m_dwRank), 
								  static_cast<unsigned char>(guildMemberDB.m_dwTitle), 
								  guildMemberDB.m_cLevel,
								  static_cast<unsigned char>(guildMemberDB.m_wClass),
								  guildMemberDB.m_cGuildWarFlag);
	MemberDetailInfo memberDetailInfo(guildMemberDB.m_dwFame, guildMemberDB.m_dwGold);

	MemberInfo memberInfo;
	memberInfo.m_dwCID = guildMemberDB.m_dwCID;
	strncpy(memberInfo.m_strName, guildMemberDB.m_strName, Guild::MAX_MEMBER_NAME_LEN);

	memberInfo.m_dwServerID		= 0;
	memberInfo.m_LeaveGuildTime = guildMemberDB.m_LeaveGuildTime;
	memberInfo.m_cTactics		= guildMemberDB.m_cTactics;

	memberInfo.m_MemberListInfo = memberListInfo;
	memberInfo.m_MemberDetailInfo = memberDetailInfo;

	return JoinMember(memberInfo);
}

bool CGuild::JoinMember(MemberInfo& memberInfo)
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
		ERRLOG1(g_Log, "GID:0x%08x ��� ��� �߰� ���� : �̹� ���ԵǾ� �ִ� ����Դϴ�.", m_dwGID);
		return false; 
	}

	m_MemberList.push_back(memberInfo);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
	if (NULL != lpCharacter)
	{
		lpCharacter->SetGID(m_dwGID);
	}

	CalculateGuildFame();
	return true;
}

bool CGuild::IsMember(unsigned long dwCID)
{
	MemberList::iterator it = m_MemberList.begin();
	
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID)
		{
			return true;
		}
	}
	
	return false;
}

bool CGuild::IsTacticsWaitMember(unsigned long dwCID)
{
	MemberList::iterator it = m_MemberList.begin();
	for (; it != m_MemberList.end(); ++it)
	{
		if (dwCID == it->m_dwCID && Guild::TACTICS_WAIT == it->m_cTactics)
		{
			return true;
		}
	}

	return false;
}

bool CGuild::KickMember(unsigned long dwSuperior, unsigned long dwFollower, unsigned short &wError)
{
	if (dwFollower == GetMaster().m_dwCID)
	{
		ERRLOG3(g_Log, "GID:0x%08x ű ����� ��� �������Դϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberList::iterator superiorIt = m_MemberList.end();
	MemberList::iterator followerIt = m_MemberList.end();

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwSuperior == it->m_dwCID)
		{
			superiorIt = it;
		}

		if (dwFollower == it->m_dwCID)
		{
			followerIt = it;
		}
	}

	if (superiorIt == m_MemberList.end() || followerIt == m_MemberList.end())
	{
		ERRLOG3(g_Log, "GID:0x%08x ������ �ƴ� ĳ���Ͱ� ű�Ϸ� �մϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberInfo& superiorInfo = *superiorIt;
	MemberInfo& followerInfo = *followerIt;

	if (superiorInfo.m_MemberListInfo.m_cTitle > m_GuildRight.m_aryRight[GuildRight::KICK_MEMBER])
	{
		if (dwSuperior != dwFollower)
		{
			ERRLOG3(g_Log, "GID:0x%08x ������ ���� ������ ű�Ϸ� �մϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
				m_dwGID, dwSuperior, dwFollower);
			wError = PktBase::SERVER_ERROR;
			return false;
		}
		else
		{
/*
			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				// �������� ���� ��� ��� ��帶������ ���� ���� Ż���� ���� �ִ�.
				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwFollower);
				if (NULL != lpCharacter)
				{
					if (lpCharacter->GetFame() < FAME_LEAVE_PENALTY)
					{
						ERRLOG2(g_Log, "GID:0x%08x ������ ���� ĳ���Ͱ� �������� ���ξ��� Ż���Ϸ� �մϴ�. CID:0x%08x", 
							m_dwGID, dwFollower);
						wError = PktBase::SERVER_ERROR;
						return false;
					}

					lpCharacter->SetFame(lpCharacter->GetFame() - FAME_LEAVE_PENALTY);

					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpDispatch)
					{
						GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpCharacter, 
							"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);						
					}
				}
			}
			else
			{
*/
				// ���� ����ڴ� �ڱ��ڽ��� ű�Ҽ� �ִ�. (Ż�� ��û�� �ϸ� �ٷ� ű ó��)
				if (JOIN_WAIT != followerInfo.m_MemberListInfo.m_cTitle)
				{
					ERRLOG3(g_Log, "GID:0x%08x �߸��� ű�� �����Ϸ� �մϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
						m_dwGID, dwSuperior, dwFollower);
					wError = PktBase::SERVER_ERROR;
					return false;
				}
//			}
		}
	}

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwFollower, dwSuperior, "����Ż��", m_strName, PktGuildCmd::GC_KICK, PktBase::NO_SERVER_ERR);
	}

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::KickTacticsMember(unsigned long dwSuperior, unsigned long dwFollower, unsigned short wCmd, unsigned short &wError)
{
	if (dwFollower == GetMaster().m_dwCID)
	{
		ERRLOG3(g_Log, "GID:0x%08x ű ����� ��� �������Դϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberList::iterator superiorIt = m_MemberList.end();
	MemberList::iterator followerIt = m_MemberList.end();

	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		if (dwSuperior == it->m_dwCID)
		{
			superiorIt = it;
		}

		if (dwFollower == it->m_dwCID)
		{
			followerIt = it;
		}
	}

	if (superiorIt == m_MemberList.end() || followerIt == m_MemberList.end())
	{
		ERRLOG3(g_Log, "GID:0x%08x ������ �ƴ� ĳ���Ͱ� ű�Ϸ� �մϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
			m_dwGID, dwSuperior, dwFollower);
		wError = PktBase::SERVER_ERROR;
		return false;
	}

	MemberInfo& superiorInfo = *superiorIt;
	MemberInfo& followerInfo = *followerIt;

	if((superiorInfo.m_dwCID!=followerInfo.m_dwCID) && superiorInfo.m_MemberListInfo.m_cTitle > Guild::MIDDLE_ADMIN)	
	{
		if (dwSuperior != dwFollower)
		{
			ERRLOG3(g_Log, "GID:0x%08x ������ ���� ������ ű�Ϸ� �մϴ�. ű�ϴ»��:0x%08x, ű���ϴ»��:0x%08x", 
				m_dwGID, dwSuperior, dwFollower);
			wError = PktBase::SERVER_ERROR;
			return false;
		}		
	}
/*
	if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
	{
		// �������� ���� ��� ��� ��帶������ ���� ���� Ż���� ���� �ִ�.
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwFollower);
		if (NULL != lpCharacter)
		{
			if (lpCharacter->GetFame() < FAME_LEAVE_PENALTY)
			{
				ERRLOG2(g_Log, "GID:0x%08x ������ ���� ĳ���Ͱ� �������� ���ξ��� Ż���Ϸ� �մϴ�. CID:0x%08x", 
					m_dwGID, dwFollower);
				wError = PktBase::SERVER_ERROR;
				return false;
			}

			lpCharacter->SetFame(lpCharacter->GetFame() - FAME_LEAVE_PENALTY);

			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			
			if (lpDispatch)
			{
				GameClientSendPacket::SendCharFameInfo(lpDispatch->GetSendStream(), lpCharacter, 
					"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);						
			}
		}
	}	
*/
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if(lpDBAgentDispatch)
	{
		return GameClientSendPacket::SendCharGuildCmd(lpDBAgentDispatch->GetSendStream(), 
			m_dwGID, dwFollower, dwSuperior, "����Ż��", m_strName, wCmd, PktBase::NO_SERVER_ERR);
	}

	wError = PktBase::SERVER_ERROR;
	return false;
}

bool CGuild::LeaveTacticsMember(unsigned long dwCID, unsigned long dwReferenceID, unsigned short wCmd)
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
		ERRLOG2(g_Log, "GID:0x%08x ��� ��� ���� ���� : ������ �ƴ� ĳ�����Դϴ�. CID:0x%08x", 
			m_dwGID, dwCID);
		return false; 
	}

	MemberInfo& memberInfo = *it;

	PktGuildCmd pktGuildCmd;

	pktGuildCmd.m_dwGID = m_dwGID;

	pktGuildCmd.m_dwSenderID = dwCID;
	pktGuildCmd.m_dwReferenceID = memberInfo.m_cTactics;

	strncpy(pktGuildCmd.m_szSenderName, memberInfo.m_strName, MAX_MEMBER_NAME_LEN);
	strncpy(pktGuildCmd.m_szGuildName, m_strName, MAX_GUILD_NAME_LEN);

	pktGuildCmd.m_wCmd = wCmd;

	char* szPacket = reinterpret_cast<char *>(&pktGuildCmd);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
	}

	m_MemberList.erase(it);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	
	if (NULL != lpCharacter)
	{		
		lpCharacter->SetGID(0);
		lpCharacter->SetTacticsFlag(0);
	}

	CalculateGuildFame();
	return true;
}

bool CGuild::LeaveMember(unsigned long dwCID)
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
		ERRLOG2(g_Log, "GID:0x%08x ��� ��� ���� ���� : ������ �ƴ� ĳ�����Դϴ�. CID:0x%08x", 
			m_dwGID, dwCID);
		return false; 
	}

	MemberInfo& memberInfo = *it;

	PktGuildCmd pktGuildCmd;

	pktGuildCmd.m_dwGID = m_dwGID;

	pktGuildCmd.m_dwSenderID = dwCID;
	pktGuildCmd.m_dwReferenceID = 0;

	strncpy(pktGuildCmd.m_szSenderName, memberInfo.m_strName, MAX_MEMBER_NAME_LEN);

	pktGuildCmd.m_wCmd = PktGuildCmd::GC_KICK;

	char* szPacket = reinterpret_cast<char *>(&pktGuildCmd);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktGuildCmd), CmdGuildCmd, 0, 0))
	{
		SendAllMember(szPacket, sizeof(PktGuildCmd), CmdGuildCmd);
	}

	m_MemberList.erase(it);

	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL != lpCharacter)
	{
		lpCharacter->SetGID(0);
	}

	CalculateGuildFame();
	return true;
}

bool CGuild::LogInOutMember(unsigned long dwCID, unsigned long dwServerID)
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
		ERRLOG3(g_Log, "GID:0x%08x ������ �ƴ� ĳ������ �α���/�α׾ƿ��Ϸ� �մϴ�. "
			"���� �������� ������ Ʋ�� ���ɼ��� �ֽ��ϴ�. CID:0x%08x, ServerID:0x%08x", m_dwGID, dwCID, dwServerID);
		return false;
	}

	MemberInfo& memberInfo = *it;
	memberInfo.m_dwServerID = dwServerID;

	return true;
}

bool CGuild::UpdateMemberInfo(unsigned long dwCID, unsigned long dwValue, unsigned char cCmd)
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
		ERRLOG2(g_Log, "GID:0x%08x ������ �ƴ� ĳ������ ������ ������Ʈ�Ϸ� �մϴ�. CID:0x%08x", m_dwGID, dwCID);
		return false;
	}

	MemberInfo& memberInfo = *it;

	switch (cCmd)
	{
		case TYPE_LEVEL:	
			memberInfo.m_MemberListInfo.m_cLevel = static_cast<unsigned char>(dwValue);	
			memberInfo.m_cUpdateCount = MEMBER_INFO_UPDATE_COUNT; 
			break;

		case TYPE_CLASS:	
			memberInfo.m_MemberListInfo.m_cClass = static_cast<unsigned char>(dwValue);	
			memberInfo.m_cUpdateCount = MEMBER_INFO_UPDATE_COUNT; 
			break;

		case TYPE_FAME:		
			memberInfo.m_MemberDetailInfo.m_dwFame = dwValue;	
			memberInfo.m_cUpdateCount = MEMBER_INFO_UPDATE_COUNT; 
			break;

		case TYPE_GOLD:		
			memberInfo.m_MemberDetailInfo.m_dwGold = dwValue;	
			++memberInfo.m_cUpdateCount; 
			break;

		case TYPE_WARFLAG:
			memberInfo.m_MemberListInfo.m_cGuildWarFlag = static_cast<unsigned char>(dwValue);
			memberInfo.m_cUpdateCount = MEMBER_INFO_UPDATE_COUNT;
			break;
	}

	if (MEMBER_INFO_UPDATE_COUNT == memberInfo.m_cUpdateCount)
	{
		memberInfo.m_cUpdateCount = 0;

		GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
			CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if (0 != lpDBAgentDispatch)
		{
			return GameClientSendPacket::SendCharUpdateGuildMemberInfo(lpDBAgentDispatch->GetSendStream(), 
				m_dwGID, dwCID, memberInfo, PktBase::NO_SERVER_ERR);
		}
	}

	return true;
}

bool CGuild::UpdateMemberInfo(unsigned long dwCID, MemberListInfo memberListInfo, MemberDetailInfo memberDetailInfo)
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
		ERRLOG2(g_Log, "GID:0x%08x ������ �ƴ� ĳ������ ������ ������Ʈ�Ϸ� �մϴ�. "
			"���� �������� ������ Ʋ�� ���ɼ��� �ֽ��ϴ�. CID:0x%08x", m_dwGID, dwCID);
		return false;
	}

	MemberInfo& memberInfo = *it;

	memberInfo.m_MemberListInfo = memberListInfo;
	memberInfo.m_MemberDetailInfo = memberDetailInfo;

	CalculateGuildFame();

	return true;
}

const GuildLargeInfoNode CGuild::GetLargeInfo(unsigned char cIndexOfPage, unsigned short wRank, CGuild* lpRelationGuild)
{
	// lpRelationGuild �� ��û�� Ŭ���̾�Ʈ �ڽ��� ����̴�.!!
	// ����, ������ lpRelationGuild �̴�.!!
	unsigned char cRelationByMine = NEUTRALITY;
	unsigned char cRelationByTarget = NEUTRALITY;

	if (NULL != lpRelationGuild)
	{
		RelationMap::iterator itr;
		itr = m_Relation[Guild::BY_MINE].find(lpRelationGuild->GetGID());
		if (itr != m_Relation[Guild::BY_MINE].end())
		{
			// ���� ��尡 lpRelationGuild(Ŭ���̾�Ʈ�� ��û ���)���� ���踦 �ξ����Ƿ�,
			// lpRelationGuild(Ŭ���̾�Ʈ ��û ���)�� �������� ���� ��忡 ���� ���谡 �ξ��� ���̴�.
			// ����, cRelationByTarget �� ���� �ִ´�.
			cRelationByTarget = itr->second;
		}

		itr = m_Relation[Guild::BY_TARGET].find(lpRelationGuild->GetGID());
		if (itr != m_Relation[Guild::BY_TARGET].end())
		{
			// ���� ��尡 lpRelationGuild(Ŭ���̾�Ʈ�� ��û ���)�� ���� ���谡 �ξ��� ����̹Ƿ�,
			// lpRelationGuild(Ŭ���̾�Ʈ ��û ���)�� �������� ���� ��忡 ��� ���踦 ���� ���̴�.
			// ����, cRelationByMine �� ���� �ִ´�.
			cRelationByMine = itr->second;
		}

		// �켱 ���� ��带 Ÿ������ ������ ��尡 �ִ��� ���� �˻��Ѵ�.
		if(cRelationByTarget == NEUTRALITY)
		{
			// �̺κ��� ���� ���� �ǽ��� ������ Ÿ�ٱ�尡 �ǽ��� �ȹ޾��� ��Ȳ�� ���Ѵ�.
			cRelationByTarget = lpRelationGuild->GetRelation(GetGID(), BY_TARGET);
		}

		if (lpRelationGuild->GetGID() == GetGID())
		{
			cRelationByMine = MINE;
			cRelationByTarget = MINE;
		}

	}

	GuildLargeInfoNode tempNode(m_dwGID, cIndexOfPage, m_cInclination, wRank, m_dwFame, m_cLevel, 
		GetCurrentMemberNum(), GetMaster().m_strName, m_strName, m_szMark, cRelationByMine, cRelationByTarget);

	return tempNode;
}

bool CGuild::SendGuildSafe(unsigned long dwCID, char* szCharName, unsigned char cCmd)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
		if (NULL != lpCharacter)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharGuildSafe(lpDispatch->GetSendStream(), 
					dwCID, m_dwGID, m_dwGold, lpCharacter->GetGold(), cCmd, szCharName, PktBase::NO_SERVER_ERR);
			}
		}
	}

	return true;
}

bool CGuild::SendMemberList(CCharacter* lpCharacter, unsigned char cMemberType, unsigned char cSortCmd, unsigned char cPage)
{
/*
	// �׽�Ʈ
	int iTest = 0;
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;
		if (JOIN_WAIT == memberInfo.m_MemberListInfo.m_cTitle && 0 == memberInfo.m_cTactics)
		{
			iTest++;
		}
	}
*/
	PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned char cStartOfPage = cPage * PktGuildMemberList::MAX_NUM_PER_PAGE;
	unsigned char cTotalMemberNum = 0;
	
	switch (cMemberType)
	{
		case Guild::TYPE_MEMBER:	cTotalMemberNum = GetMemberNum();	break;
		case Guild::TYPE_TACTICS:	cTotalMemberNum = GetTacticsNum();	break;
	}

	if (cTotalMemberNum < cStartOfPage)
	{
		ERRLOG4(g_Log, "CID:0x%08x ��û �Ұ����� �������� ��û�Ͽ����ϴ�. ���Ÿ��:%d, �����:%d, ������:%d", 
			lpCharacter->GetCID(), cMemberType, cTotalMemberNum, cPage);
		return false;
	}

	unsigned char cSortNum = std::min(cTotalMemberNum, static_cast<unsigned char>(cStartOfPage + PktGuildMemberList::MAX_NUM_PER_PAGE));
	std::vector<MemberInfo> srcVector(cTotalMemberNum);
	std::vector<MemberInfo> sortVector(cSortNum);

	switch (cMemberType)
	{
		case Guild::TYPE_MEMBER:	std::remove_copy_if(m_MemberList.begin(), m_MemberList.end(), srcVector.begin(), IsTacticsMember());		break;
		case Guild::TYPE_TACTICS:	std::remove_copy_if(m_MemberList.begin(), m_MemberList.end(), srcVector.begin(), IsNotTacticsMember());	break;
	}

	switch (cSortCmd)
	{
		case Guild::MEMBER_TITLE:			
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberTitle());
			break;

		case Guild::MEMBER_CLASS:
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberClass());
			break;

		case Guild::MEMBER_LEVEL:
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberLevel());
			break;

		case Guild::MEMBER_FAME:	
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberFame());
			break;

		case Guild::MEMBER_NAME:
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberName());
			break;

		case Guild::MEMBER_GOLD:
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberGold());
			break;

		case Guild::MEMBER_POSITION:
			std::partial_sort_copy(srcVector.begin(), srcVector.end(), 
				sortVector.begin(), sortVector.end(), CompareMemberPosition());
			break;

		case Guild::MEMBER_JOIN_WAIT:
		{
			sortVector.clear();

			for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
			{
				MemberInfo& memberInfo = *it;
				if (JOIN_WAIT == memberInfo.m_MemberListInfo.m_cTitle && 0 == memberInfo.m_cTactics)
				{
					sortVector.push_back(memberInfo);
				}
			}

			std::sort(sortVector.begin(), sortVector.end(), CompareMemberName());
			cTotalMemberNum = static_cast<unsigned char>(sortVector.size());
			break;
		}

		case Guild::MEMBER_WAR_ON:		// ��� ���� �������� ��� (�̸������� ��Ʈ)
		{
			sortVector.clear();

			for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
			{
				MemberInfo& memberInfo = *it;
				if (Creature::WAR_ON == memberInfo.m_MemberListInfo.m_cGuildWarFlag && 0 == memberInfo.m_cTactics)
				{
					sortVector.push_back(memberInfo);
				}
			}

			std::sort(sortVector.begin(), sortVector.end(), CompareMemberName());
			cTotalMemberNum = static_cast<unsigned char>(sortVector.size());
			break;
		}

		case Guild::MEMBER_WAR_OFF:	// ��� ���� �������� ��� (�̸������� ��Ʈ)
		{
			sortVector.clear();

			for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
			{
				MemberInfo& memberInfo = *it;
				if (Creature::WAR_OFF == memberInfo.m_MemberListInfo.m_cGuildWarFlag && 0 == memberInfo.m_cTactics)
				{
					sortVector.push_back(memberInfo);
				}
			}

			std::sort(sortVector.begin(), sortVector.end(), CompareMemberName());
			cTotalMemberNum = static_cast<unsigned char>(sortVector.size());
			break;
		}

		case Guild::TACTICS_JOIN_WAIT:	// ��û���� �뺴 (���� ���� ��)
		{
			sortVector.clear();

			for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
			{
				MemberInfo& memberInfo = *it;
				if (JOIN_WAIT == memberInfo.m_MemberListInfo.m_cTitle && 0 != memberInfo.m_cTactics)
				{
					sortVector.push_back(memberInfo);
				}
			}

			std::sort(sortVector.begin(), sortVector.end(), CompareMemberLevel());
			cTotalMemberNum = static_cast<unsigned char>(sortVector.size());
			break;
		}

		case Guild::TACTICS_ACTIVE:		// Ȱ������ �뺴 (���� ���� ��)
		{
			sortVector.clear();

			for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
			{
				MemberInfo& memberInfo = *it;
				if (JOIN_WAIT != memberInfo.m_MemberListInfo.m_cTitle && 0 != memberInfo.m_cTactics)
				{
					sortVector.push_back(memberInfo);
				}
			}

			std::sort(sortVector.begin(), sortVector.end(), CompareMemberLevel());
			cTotalMemberNum = static_cast<unsigned char>(sortVector.size());
			break;
		}
	}

	const int   MAX_BUFFER = sizeof(PktGuildMemberList) + PktGuildMemberList::MAX_NUM_PER_PAGE * sizeof(GuildMemberInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktGuildMemberList* lpPktGMLAck = reinterpret_cast<PktGuildMemberList *>(szBuffer);
	GuildMemberInfoNode* lpMemberInfoNode = reinterpret_cast<GuildMemberInfoNode *>(lpPktGMLAck + 1);                

	lpPktGMLAck->m_dwCID = lpCharacter->GetCID();

	lpPktGMLAck->m_cMemberType = cMemberType;
	lpPktGMLAck->m_cSortCmd = cSortCmd;
	lpPktGMLAck->m_cPage = cPage;

	lpPktGMLAck->m_cTotalMemberNum = cTotalMemberNum;
	lpPktGMLAck->m_cNodeNum = 0;

	for (unsigned char cIndex = cStartOfPage; 
		cIndex < cStartOfPage + PktGuildMemberList::MAX_NUM_PER_PAGE && cIndex < sortVector.size(); 
		++cIndex, ++lpMemberInfoNode, ++lpPktGMLAck->m_cNodeNum)
	{
		MemberInfo& memberInfo = sortVector[cIndex];

		lpMemberInfoNode->m_dwCID = memberInfo.m_dwCID;
		strncpy(lpMemberInfoNode->m_szName, memberInfo.m_strName, Guild::MAX_MEMBER_NAME_LEN);
		lpMemberInfoNode->m_dwServerID = memberInfo.m_dwServerID;
		lpMemberInfoNode->m_cTactics = memberInfo.m_cTactics;

		lpMemberInfoNode->m_dwFame = memberInfo.m_MemberDetailInfo.m_dwFame;
		lpMemberInfoNode->m_dwGold = memberInfo.m_MemberDetailInfo.m_dwGold;

		lpMemberInfoNode->m_cRank = memberInfo.m_MemberListInfo.m_cRank;
		lpMemberInfoNode->m_cTitle = memberInfo.m_MemberListInfo.m_cTitle;
		lpMemberInfoNode->m_cLevel = memberInfo.m_MemberListInfo.m_cLevel;
		lpMemberInfoNode->m_cClass = memberInfo.m_MemberListInfo.m_cClass;
		lpMemberInfoNode->m_cGuildWarFlag = memberInfo.m_MemberListInfo.m_cGuildWarFlag;
	}

	unsigned char cNodeNum = lpPktGMLAck->m_cNodeNum;
	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktGuildMemberList) + 
			cNodeNum * sizeof(GuildMemberInfoNode), CmdGuildMemberList, 0, 0);
	}

	return true;
}

void CGuild::SendAllMember(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;

		CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
		if (NULL != lpMember)
		{
			CGameClientDispatch* lpDispatch = lpMember->GetDispatcher();
			if (NULL != lpDispatch)
			{
				lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
			}
		}
	}
}

void CGuild::SendCurrentMember(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In)
{
	for (MemberList::iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++it)
	{
		MemberInfo& memberInfo = *it;

		if (IsCurrentMember()(memberInfo))
		{
			CCharacter* lpMember = CCreatureManager::GetInstance().GetCharacter(memberInfo.m_dwCID);
			if (NULL != lpMember)
			{
				CGameClientDispatch* lpDispatch = lpMember->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
				}
			}
		}
	}
}

bool CGuild::SendHostilityList(CCharacter* lpCharacter, unsigned char cCurrentPage, unsigned char cPageState)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned short wTotalHostilityNum = static_cast<unsigned short>( m_Relation[Guild::BY_TARGET].size() );
	int iStartOfPage = (cCurrentPage - 1) * PktGuildHostilityList::MAX_NUM_PER_PAGE;
    
	switch (cPageState)
	{
		case PktGuildHostilityList::PAGE_PREV :
		{
			iStartOfPage -= PktGuildHostilityList::MAX_NUM_PER_PAGE;
			if ( iStartOfPage < 0 )
			{
				int iTotalPage = wTotalHostilityNum / PktGuildHostilityList::MAX_NUM_PER_PAGE;
				if (0 != ( wTotalHostilityNum % PktGuildHostilityList::MAX_NUM_PER_PAGE ))
				{
					++iTotalPage ;
				}

				iStartOfPage = (iTotalPage - 1) * PktGuildHostilityList::MAX_NUM_PER_PAGE;
			}
			break;
		}

		case PktGuildHostilityList::PAGE_NEXT :
		{
			iStartOfPage += PktGuildHostilityList::MAX_NUM_PER_PAGE;
			if ( iStartOfPage >= wTotalHostilityNum )
			{
				iStartOfPage = 0 ;
			}

			break;
		}

		case PktGuildHostilityList::PAGE_CURRENT :
		{
			break;
		}
	}
	if(iStartOfPage < 0)
		iStartOfPage = 0;

	GuildHostilityInfoNode tmpNode;
	CGuild* lpGuild = NULL;
	std::vector<GuildHostilityInfoNode> sortVector;
	sortVector.reserve(wTotalHostilityNum);

	RelationMap::iterator itr = m_Relation[Guild::BY_TARGET].begin();
	while (itr != m_Relation[Guild::BY_TARGET].end())
	{
		lpGuild = CGuildMgr::GetInstance().GetGuild( itr->first );
		if (0 != lpGuild)
		{
			strcpy(tmpNode.m_szName, lpGuild->GetName());
		}
		else
		{
			::memset(tmpNode.m_szName, 0, Guild::MAX_GUILD_NAME_LEN);
		}
		
		tmpNode.m_cRelationType = itr->second;
        sortVector.push_back(tmpNode);

		++itr;
	}

	std::sort(sortVector.begin(), sortVector.end(), CompareRelation());
	
	// �ش� �������� ���� ���� ��Ȳ ����Ʈ ������
    const int   MAX_BUFFER = sizeof(PktGuildHostilityList) + (PktGuildHostilityList::MAX_NUM_PER_PAGE + 1) * sizeof(GuildHostilityInfoNode);
	char        szBuffer[MAX_BUFFER];

	PktGuildHostilityList* lpPktGHLAck = reinterpret_cast<PktGuildHostilityList *>(szBuffer);
	GuildHostilityInfoNode* lpHostilityInfoNode = reinterpret_cast<GuildHostilityInfoNode *>(lpPktGHLAck + 1);

	lpPktGHLAck->m_dwCID = lpCharacter->GetCID();
	lpPktGHLAck->m_cCurrentPage = static_cast<unsigned char>( iStartOfPage / PktGuildHostilityList::MAX_NUM_PER_PAGE + 1 );
	lpPktGHLAck->m_cPageState = 0;
	lpPktGHLAck->m_cNodeNum = 0;
	lpPktGHLAck->m_wTotalItemNum = wTotalHostilityNum;

	CGuild* lpHostilityGuild = CGuildMgr::GetInstance().GetGuild( GetRelationGID(Guild::HOSTILITY) );
	if (0 != lpHostilityGuild)
	{
		strcpy(lpPktGHLAck->m_szHostility, lpHostilityGuild->GetName());
	}
	else
	{
		::memset(lpPktGHLAck->m_szHostility, 0, Guild::MAX_GUILD_NAME_LEN);
	}

	CGuild* lpAlertGuild = CGuildMgr::GetInstance().GetGuild( GetRelationGID(Guild::ALERT_HOSTILITY) );
	if (0 != lpAlertGuild)
	{
		strcpy(lpPktGHLAck->m_szAlert, lpAlertGuild->GetName());
	}
	else
	{
		::memset(lpPktGHLAck->m_szAlert, 0, Guild::MAX_GUILD_NAME_LEN);
	}
	
	for (int iIndex = iStartOfPage; 
		iIndex < iStartOfPage + PktGuildHostilityList::MAX_NUM_PER_PAGE && iIndex < (int)sortVector.size(); 
		++iIndex, ++lpHostilityInfoNode, ++lpPktGHLAck->m_cNodeNum)
	{
		GuildHostilityInfoNode hostilityInfo = sortVector[iIndex];

		strncpy(lpHostilityInfoNode->m_szName, hostilityInfo.m_szName, Guild::MAX_MEMBER_NAME_LEN);
		lpHostilityInfoNode->m_cRelationType = hostilityInfo.m_cRelationType;
	}

	unsigned char cNodeNum = lpPktGHLAck->m_cNodeNum;
	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		CSendStream& SendStream = lpDispatch->GetSendStream();
		return SendStream.WrapCompress(szBuffer, sizeof(PktGuildHostilityList) + 
			cNodeNum * sizeof(GuildHostilityInfoNode), CmdGuildHostilityList, 0, 0);
	}

	return true;
}