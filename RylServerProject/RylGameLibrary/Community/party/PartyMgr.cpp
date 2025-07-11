#include "stdafx.h"

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Stream/SendStream.h>

#include "PartyMgr.h"


CPartyMgr& CPartyMgr::GetInstance()
{
    static CPartyMgr partyMgr;
    return partyMgr;    
}


CPartyMgr::CPartyMgr()
:	m_tempUID(0)
{ 
	m_PartyMap.clear();
	m_MemberFindPartyMap.clear();
	m_PartyFindMemberMap.clear();
}

CPartyMgr::~CPartyMgr() 
{ 
	DestoryPartyList();
}


void CPartyMgr::DestoryPartyList()
{
//	ERRLOG1(g_Log, "EDITH :%d 파티가 존재합니다.", m_PartyMap.size());

	for (PartyMap::iterator it = m_PartyMap.begin(); it != m_PartyMap.end(); ++it)
	{
		delete it->second;
	}

    m_PartyMap.clear();

	// edith 2008.05.29 파티 맵 초기화 추가
	m_MemberFindPartyMap.clear();
	m_PartyFindMemberMap.clear();
}


void CPartyMgr::UpdatePartyData()
{
    PartyMap::iterator pos = m_PartyMap.begin();
    PartyMap::iterator end = m_PartyMap.end();

    for (; pos != end; ++pos)
    {
        pos->second->UpdateData();
    }
}


bool CPartyMgr::RequestPartyInfoToDB(unsigned long dwLeaderCID, unsigned long dwPartyUID)
{
	return true;
}


bool CPartyMgr::AddParty(CParty *pParty)
{
	unsigned long dwPartyUID = pParty->GetUID();

	PartyMap::iterator it = m_PartyMap.find(dwPartyUID);
	if (it != m_PartyMap.end()) { 
		return false; 
	}

	// edith 2008.05.29 파티맵 수정
	// m_PartyMap.insert(PartyMap::value_type(pParty->GetUID(), pParty)).second;

	m_PartyMap.insert(PartyMap::value_type(pParty->GetUID(), pParty));
	return GetParty(dwPartyUID);
}


CParty* CPartyMgr::GetParty(unsigned long dwPartyUID)
{
	PartyMap::iterator it = m_PartyMap.find(dwPartyUID);
	if (it == m_PartyMap.end()) { 
		return NULL; 
	}

	return it->second;
}


bool CPartyMgr::DeleteParty(unsigned long dwPartyUID)
{
	PartyMap::iterator it = m_PartyMap.find(dwPartyUID);
	if (it == m_PartyMap.end()) { 
		return false; 
	}

	delete it->second;
	m_PartyMap.erase(it);
	
	return true;
}


bool CPartyMgr::AddFindPartyList(unsigned long dwCID)
{
	MemberFindPartyMap::iterator it = m_MemberFindPartyMap.find(dwCID);
	if (it != m_MemberFindPartyMap.end()) { 
		return false; 
	}

	CCharacter* pCharacter = CCreatureManager::GetInstance().GetCharacter(dwCID);
	if (NULL == pCharacter) {
		return false;
	}

	return m_MemberFindPartyMap.insert(MemberFindPartyMap::value_type(dwCID, pCharacter)).second;
}


bool CPartyMgr::DeleteFindPartyList(unsigned long dwCID)
{
	MemberFindPartyMap::iterator it = m_MemberFindPartyMap.find(dwCID);
	if (it == m_MemberFindPartyMap.end()) { 
		return false; 
	}

	m_MemberFindPartyMap.erase(it);

	return true;
}


bool CPartyMgr::AddFindMemberList(unsigned long dwPartyUID)
{
	PartyFindMemberMap::iterator it = m_PartyFindMemberMap.find(dwPartyUID);
	if (it != m_PartyFindMemberMap.end()) 
	{ 
		ERRLOG1(g_Log, "PID:0x%08x 파티원 찾기 리스트에 이미 있는 파티를 추가하려 합니다.", dwPartyUID);
		return false; 
	}

	CParty* pParty = CPartyMgr::GetInstance().GetParty(dwPartyUID);
	if (NULL == pParty) 
	{
		ERRLOG1(g_Log, "PID:0x%08x 존재하지 않는 파티입니다.", dwPartyUID);
		return false;
	}

	return m_PartyFindMemberMap.insert(PartyFindMemberMap::value_type(dwPartyUID, pParty)).second;
}


bool CPartyMgr::DeleteFindMemberList(unsigned long dwPartyUID)
{
	PartyFindMemberMap::iterator it = m_PartyFindMemberMap.find(dwPartyUID);
	if (it == m_PartyFindMemberMap.end()) 
	{ 
		return false; 
	}

	m_PartyFindMemberMap.erase(it);

	return true;
}


void CPartyMgr::SendPartyFind(CCharacter* lpCharacter)
{
	if (NULL != lpCharacter) 
    {
        CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
        if (NULL != lpDispatch)
        {
            const int MAX_BUFFER_SIZE = sizeof(MemberFindParty) * PktPFAck::MAX_LIST + 
                sizeof(PartyFindMember) * PktPFAck::MAX_LIST + sizeof(PktPFAck);

	        char        szBuffer[MAX_BUFFER_SIZE];
	        char*       lpBufferPos = szBuffer + sizeof(PktPFAck);
            PktPFAck*   lpPktPFAck  = reinterpret_cast<PktPFAck*>(szBuffer);

	        int     nMemberFindPartyNum = 0;
	        int     nPartyFindMemberNum = 0;

            const int nMyLevel = lpCharacter->GetStatus().m_nLevel;

	        for (MemberFindPartyMap::iterator it = m_MemberFindPartyMap.begin();
                it != m_MemberFindPartyMap.end() && nMemberFindPartyNum < PktPFAck::MAX_LIST; ++it)
	        {
		        CCharacter* lpEntryCharacter = it->second;
    		    
                if (NULL != lpEntryCharacter)// && lpCharacter != lpEntryCharacter) 
                {
                    const int nEntryCharLevel = lpEntryCharacter->GetStatus().m_nLevel;

		            if (::abs(nEntryCharLevel - nMyLevel) <= PktPFAck::MAX_DIFF_LEVEL_VIEW &&
						EnemyCheck::EC_ENEMY != lpCharacter->IsEnemy(lpEntryCharacter))
		            {
			            MemberFindParty* lpEntryMemberFindParty = reinterpret_cast<MemberFindParty*>(lpBufferPos);

			            strncpy(lpEntryMemberFindParty->m_strName, lpEntryCharacter->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
			            lpEntryMemberFindParty->m_cLevel= static_cast<unsigned char>(nEntryCharLevel);
			            lpEntryMemberFindParty->m_cClass = static_cast<unsigned char>(lpEntryCharacter->GetClass());

			            lpBufferPos += sizeof(MemberFindParty);
			            ++nMemberFindPartyNum;
		            }
                }
	        }

	        for (PartyFindMemberMap::iterator it = m_PartyFindMemberMap.begin(); 
                it != m_PartyFindMemberMap.end() && nPartyFindMemberNum < PktPFAck::MAX_LIST; ++it)
	        {
                CParty* lpEntryParty = it->second;
                
		        // 자기가 속한 파티는 제외
                if (NULL != lpEntryParty ) // && !lpEntryParty->IsMember(lpCharacter->GetCID()))
                {
                    const int nAverageLevel = lpEntryParty->GetLoggedMemberAverageLevel();                

		            if (::abs(nAverageLevel - nMyLevel) <= PktPFAck::MAX_DIFF_LEVEL_VIEW)
		            {
			            PartyFindMember* lpEntryPartyFindMember = reinterpret_cast<PartyFindMember*>(lpBufferPos);

			            CCharacter* lpLeaderCharacter = CCreatureManager::GetInstance().GetCharacter(lpEntryParty->GetLeader());
			            if (NULL != lpLeaderCharacter) 
                        {
							if (EnemyCheck::EC_ENEMY != lpCharacter->IsEnemy(lpLeaderCharacter))
							{
								strncpy(lpEntryPartyFindMember->m_strLeaderName, 
									lpLeaderCharacter->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);

								lpEntryPartyFindMember->m_cAverageLevel = static_cast<unsigned char>(nAverageLevel);
								lpEntryPartyFindMember->m_cMemberNum = lpEntryParty->GetMemberNum();

								lpBufferPos += sizeof(PartyFindMember);
								++nPartyFindMemberNum;
							}
                        }
		            }			    
		        }
	        }

	        const unsigned short usPacketSize = static_cast<unsigned short>(sizeof(PktPFAck) + 
                sizeof(MemberFindParty) * nMemberFindPartyNum + sizeof(PartyFindMember) * nPartyFindMemberNum);

            lpPktPFAck->m_dwCharID              = lpCharacter->GetCID();
	        lpPktPFAck->m_cMemberFindPartyNum   = nMemberFindPartyNum;
	        lpPktPFAck->m_cPartyFindMemberNum   = nPartyFindMemberNum;

            lpDispatch->GetSendStream().WrapCompress(szBuffer, usPacketSize, CmdCharPartyFind, 0, 0);
        }
    }
}