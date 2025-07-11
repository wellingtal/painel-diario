///////////////////////////////////////////////////////////////////////////////////
// File : "CreatureManager.cpp"
//
// Related Header File : "CreatureManager.h"
//
// Original Author : 장진영
//
// Creation Date : 2002-09-06
//
// Specification Document : None
//
// Purpose : Creature의 리스트를 관리하는 클래스
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/Math/Math.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/packetStruct/CharCommunityPacket.h>

#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/NPC/NPC.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/CampShop.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/CastleEmblem.h>
#include <Creature/Character/CharRespawnMgr.h>

#include <Community/Party/Party.h>

#include <Castle/Castle.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>

#include "CreatureManager.h"
#include "Creature.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CCreatureManager::CCreatureManager()
:   m_CharacterPool(sizeof(CCharacter)),
	m_dwLastUpdateTime(0), m_bAutoBalance(true), m_bRest(false)
{
	m_RespawnPoint[POINT_HUMAN1] = Position(1133, 57, 1429);
	m_RespawnPoint[POINT_HUMAN2] = Position(1217, 48, 1827);
	m_RespawnPoint[POINT_HUMAN3] = Position(1526, 33, 1685);
	m_RespawnPoint[POINT_AKHAN1] = Position(1310, 28, 1168);
	m_RespawnPoint[POINT_AKHAN2] = Position(1619, 49, 998);
	m_RespawnPoint[POINT_AKHAN3] = Position(1724, 53, 1444);

	std::fill_n(m_wCharacterNum, int(CClass::MAX_RACE), 0);
}


CCreatureManager::~CCreatureManager()
{
    DestroyAll();
}

int isize;
int icur;

struct FnDeleteSecond
{   
    template<typename PairType>
        bool operator() (PairType& pair) { if(NULL != pair.second) { delete pair.second; } return true; }
};

struct FnLeaveParty
{
	template<typename PairType>
		bool operator() (PairType& pair)
	{
		if(NULL != pair.second)
		{
			CParty* lpParty = (pair.second)->GetParty();
			if (lpParty)
			{
				lpParty->Leave((pair.second)->GetCID(), 0, (pair.second)->GetMapIndex());
			}
			++icur;
		}
		return true; 
	}
};


CCreatureManager& CCreatureManager::GetInstance()
{	
    static CCreatureManager creatureManager;
	return creatureManager;
}

void CCreatureManager::DestoryCharacterList()
{
	CharacterMap::iterator pos =  m_CharacterMap.begin();
	CharacterMap::iterator end =  m_CharacterMap.end();

	for(; pos != end; ++pos)        
	{
		DeleteCharacter(pos->second);
	}

	m_CharacterMap.clear();
	m_CharacterNameMap.clear();
    m_LogoutWaitList.clear();
}

void CCreatureManager::DestoryMonsterList()
{
	isize = m_MonsterMap.size();
	icur = 0;

	std::for_each(m_MonsterMap.begin(), m_MonsterMap.end(), FnLeaveParty());
    std::for_each(m_MonsterMap.begin(), m_MonsterMap.end(), FnDeleteSecond());
	m_MonsterMap.clear();
	m_AdminMonsterUIDMap.clear();
}

void CCreatureManager::DestorySiegeObjectList()
{
	m_SiegeObjectMap.clear();
}

void CCreatureManager::DestoryNPCList()
{
    std::for_each(m_NPCMap.begin(), m_NPCMap.end(), FnDeleteSecond());
	m_NPCMap.clear();
}

void CCreatureManager::DestroyAll()
{
    DestoryCharacterList();
    DestoryMonsterList();
	DestorySiegeObjectList();
    DestoryNPCList();
}


bool CCreatureManager::AddCreature(CCreature* lpCreature)
{
	unsigned long dwCID = lpCreature->GetCID();

    bool bResult = false;

	switch (Creature::GetCreatureType(dwCID))
	{
	case Creature::CT_PC:
		{
			CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpCreature);
			bResult = m_CharacterMap.insert(std::make_pair(dwCID, lpCharacter)).second;

			if (true == bResult) 
			{
				unsigned long nHashedKey = Math::HashFunc::sdbmHash(
					reinterpret_cast<const unsigned char*>(lpCharacter->GetCharacterName()));

				m_CharacterNameMap.insert(std::make_pair(nHashedKey, lpCharacter));

                CClass::RaceType eRace = lpCharacter->GetRace();
                if(eRace < CClass::MAX_RACE)
                {
                    ++m_wCharacterNum[eRace];
                }                
			}

			// CharSphereTree 에 노드 추가
			if (NULL != lpCharacter)
			{
				CCharSphereTree::GetInstance().AddCharacter(dwCID, lpCharacter, lpCharacter->GetCurrentPos() );
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		{
			bResult = m_MonsterMap.insert(std::make_pair(dwCID, reinterpret_cast<CMonster *>(lpCreature))).second;

			unsigned short wKindID = static_cast<unsigned short>( (dwCID & Creature::MONSTER_KIND_BIT) );
			if ( bResult && m_AdminMonsterUIDMap.end() == m_AdminMonsterUIDMap.find(wKindID) )
			{
				m_AdminMonsterUIDMap.insert(std::make_pair(wKindID, INIT_UID)).second;
			}
		}
		break;

	case Creature::CT_NPC:
		{
			bResult = m_NPCMap.insert(std::make_pair(dwCID, reinterpret_cast<CNPC *>(lpCreature))).second;
		}
		break;

	case Creature::CT_SIEGE_OBJECT:
		{
			bResult = m_SiegeObjectMap.insert(std::make_pair(dwCID, reinterpret_cast<CSiegeObject *>(lpCreature))).second;
		}
		break;
	}

    return bResult;
}


bool CCreatureManager::DeleteCreature(unsigned long dwCID)
{
	switch (Creature::GetCreatureType(dwCID))
	{
	case Creature::CT_PC:
		{
			CharacterMap::iterator pos = m_CharacterMap.find(dwCID);
			if (pos == m_CharacterMap.end())
			{
				ERRLOG1(g_Log, "CID:0x%08x 캐릭터를 지울 수 없습니다. 맵에 없습니다.", dwCID);		    
				return false;
			}
			else
			{
				CCharacter* lpDeleteCharacter = pos->second;

				const char*     szCharacterName = lpDeleteCharacter->GetCharacterName(); 
				unsigned long   nHashedKey = Math::HashFunc::sdbmHash(
					reinterpret_cast<const unsigned char*>(szCharacterName));

				std::pair<CharacterMultimap::iterator, CharacterMultimap::iterator> findpair = 
					m_CharacterNameMap.equal_range(nHashedKey);

				for (;findpair.first != findpair.second;)
				{
					if(0 == strncmp(findpair.first->second->GetCharacterName(),
						szCharacterName, CHAR_INFOST::MAX_NAME_LEN))
					{
						m_CharacterNameMap.erase(findpair.first);
						break;
					}
					else
					{
						++findpair.first;
					}
				}

                CClass::RaceType eRace = lpDeleteCharacter->GetRace();
                if(eRace < CClass::MAX_RACE)
                {
                    --m_wCharacterNum[eRace];
                }

				DeleteCharacter(lpDeleteCharacter);
				m_CharacterMap.erase(pos);
			}
		}
		break;

	case Creature::CT_MONSTER:
	case Creature::CT_SUMMON:
	case Creature::CT_STRUCT:
		{
			MonsterMap::iterator pos = m_MonsterMap.find(dwCID);
			if (pos == m_MonsterMap.end()) 
			{
				return false;
			}

            delete pos->second;
			m_MonsterMap.erase(pos);
		}
		break;

	case Creature::CT_NPC:
		{
			NPCMap::iterator pos = m_NPCMap.find(dwCID);
			if (pos == m_NPCMap.end())
			{
				return false;
			}

			delete pos->second;
			m_NPCMap.erase(dwCID);
		}
		break;

	case Creature::CT_SIEGE_OBJECT:
		{
			SiegeObjectMap::iterator pos = m_SiegeObjectMap.find(dwCID);
			if (pos == m_SiegeObjectMap.end()) 
			{
				return false;
			}

			// 메모리 해제는 하지 않는다.
			// 공성 오브젝트 관리는 CSiegeObjectMgr 에서 한다!!
			CSiegeObject* lpSiegeObject = pos->second;
			if (lpSiegeObject && lpSiegeObject->GetCellPos().m_lpCell)
			{
				// 셀에서 삭제
				lpSiegeObject->GetCellPos().m_lpCell->DeleteCreature(lpSiegeObject->GetCID());
			}

			m_SiegeObjectMap.erase(pos);
		}
		break;
	}

    return true;
}


CCreature*	CCreatureManager::GetCreature(unsigned long dwCID) 
{
	return (Creature::CT_NPC == Creature::GetCreatureType(dwCID)) ?
			reinterpret_cast<CCreature *>(GetNPC(dwCID)) : reinterpret_cast<CCreature *>(GetAggresiveCreature(dwCID));
}	

CAggresiveCreature* CCreatureManager::GetAggresiveCreature(unsigned long dwCID)
{
	switch (Creature::GetCreatureType(dwCID))
	{
		case Creature::CT_PC:			return reinterpret_cast<CAggresiveCreature *>(GetCharacter(dwCID));
		case Creature::CT_SIEGE_OBJECT:	return reinterpret_cast<CAggresiveCreature *>(GetSiegeObject(dwCID));
		
		case Creature::CT_MONSTER:
		case Creature::CT_SUMMON:
		case Creature::CT_STRUCT:
			return reinterpret_cast<CAggresiveCreature *>(GetMonster(dwCID));
	}

	return NULL;
}

CCharacter* CCreatureManager::GetCharacter(unsigned long dwCID)
{
	CharacterMap::iterator pos = m_CharacterMap.find(dwCID);
	return (pos != m_CharacterMap.end()) ? pos->second : NULL;
}

CCharacter* CCreatureManager::GetCharacter(const char* szCharacterName)
{	
	unsigned long nHashedKey = Math::HashFunc::sdbmHash(
		reinterpret_cast<const unsigned char*>(szCharacterName));

	std::pair<CharacterMultimap::iterator, CharacterMultimap::iterator> findpair = 
		m_CharacterNameMap.equal_range(nHashedKey);

	CCharacter* lpCharacter = 0;
	for(;findpair.first != findpair.second;)
	{
		lpCharacter = findpair.first->second;
		if(0 == strncmp(lpCharacter->GetCharacterName(),
			szCharacterName, CHAR_INFOST::MAX_NAME_LEN))
		{
			return lpCharacter;
		}
		else
		{
			++findpair.first;
		}
	}

	return NULL;
}

CMonster* CCreatureManager::GetMonster(unsigned long dwCID)
{
	MonsterMap::iterator pos = m_MonsterMap.find(dwCID);
	return (pos != m_MonsterMap.end()) ? pos->second : NULL;
}

CSiegeObject*	CCreatureManager::GetSiegeObject(unsigned long dwCID)
{
	SiegeObjectMap::iterator pos = m_SiegeObjectMap.find(dwCID);
	return (pos != m_SiegeObjectMap.end()) ? pos->second : NULL;
}

CNPC* CCreatureManager::GetNPC(unsigned long dwCID)
{
	NPCMap::iterator pos = m_NPCMap.find(dwCID);
	return (pos != m_NPCMap.end()) ? pos->second : NULL;
}


void CCreatureManager::EnqueueLogout(CCharacter* lpCharacter) 
{
	m_LogoutWaitList.push_back(lpCharacter);
}

bool CCreatureManager::CancelLogout(CCharacter* lpCharacter) 
{ 
	CharacterList::iterator end = m_LogoutWaitList.end();
	CharacterList::iterator pos = std::find(m_LogoutWaitList.begin(), end, lpCharacter);

	if(pos != end)
	{
	    (*pos)->ResetLogoutCount();
	    m_LogoutWaitList.erase(pos);
	    return true;	
	}

    return false;
}

bool CCreatureManager::ProcessCharacterLogout(void)
{
	if(!m_LogoutWaitList.empty()) 
    {
	    CharacterList::iterator LogoutItr = std::partition(m_LogoutWaitList.begin(),
		    m_LogoutWaitList.end(), std::mem_fun(&CCharacter::StillAlive));

	    std::for_each(LogoutItr, m_LogoutWaitList.end(), 
            std::bind2nd(std::mem_fun1(&CCharacter::Logout), DBUpdateData::LOGOUT));

	    m_LogoutWaitList.erase(LogoutItr, m_LogoutWaitList.end());
        return true;
    }

	return false;
}

void CCreatureManager::ProcessSummonMonsterDead(void)
{
	MonsterMap::iterator pos = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();

	for (; pos != end; )
	{
		CMonster* lpMonster = pos->second;

		if (true == lpMonster->IsDeadSummonMonster())
		{
			pos = m_MonsterMap.erase(pos);
			delete lpMonster;
		}
		else
		{
			++pos;
		}
	}
}

void CCreatureManager::SetNationToCastleNPC(unsigned long dwCastleID, unsigned char cNation)
{
	if (m_NPCMap.empty())
	{
		return;
	}

	Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastle(dwCastleID);
	if (NULL == lpCastle)
	{
		return;
	}

	NPCMap::iterator pos = m_NPCMap.begin();
	NPCMap::iterator end = m_NPCMap.end();

	while ( pos != end )
	{
		CNPC* lpNPC = pos->second;

		if (lpNPC && lpNPC->GetCastleNameID() == lpCastle->GetNameID() && lpNPC->IsBelongToCastle())
		{
			lpNPC->SetNation(cNation);
		}

		++pos;
	}
}

CCharacter* CCreatureManager::CreateCharacter(unsigned long dwCID, unsigned long dwSessionID)
{
    void* lpMemory = m_CharacterPool.malloc();
    return (NULL != lpMemory) ? new (lpMemory) CCharacter(dwCID, dwSessionID) : NULL;
}

void CCreatureManager::DeleteCharacter(CCharacter* lpCharacter)
{
	// CharSphereTree 에서 노드 제거
	CCharSphereTree::GetInstance().DeleteCharacter(lpCharacter->GetCID());

    lpCharacter->~CCharacter();
	m_CharacterPool.free(lpCharacter);    
}


bool CCreatureManager::IsSummonee(unsigned long dwCID) 
{ 
	return Creature::IsSummonMonster(dwCID);
}

Item::CShopContainer* CCreatureManager::GetShopContainer(unsigned long dwCID)
{
	if (Creature::CT_PC == Creature::GetCreatureType(dwCID))
	{
		CCharacter* lpOwner = GetCharacter(dwCID);
		if (NULL != lpOwner)
		{
			return &(lpOwner->GetStall());
		}
	}
	else if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(dwCID))
	{
		CSiegeObject* lpSiegeObject = GetSiegeObject(dwCID);
		if (Siege::CAMP_SHOP == lpSiegeObject->GetObjectType() &&
			Siege::CHANGING != lpSiegeObject->GetState())
		{
			CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(lpSiegeObject);
			if (NULL != lpCampShop)
			{
				return &(lpCampShop->GetContainer());
			}
		}
	}

	return NULL;
}

void CCreatureManager::SendAllCharacter(const char* szBuffer, unsigned long dwLength, 
                                        unsigned char cCMD_In, bool bSendAllMap)
{
	CharacterMap::iterator pos = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();

    CCharacter* lpCharacter = 0;
    CGameClientDispatch* lpDispatch = 0;

	for (; pos != end; ++pos)
	{
		CCharacter* lpCharacter = pos->second;

		if (0 != lpCharacter && 
            (bSendAllMap || 0 == lpCharacter->GetMapIndex()) &&
            0 != (lpDispatch = lpCharacter->GetDispatcher()))
		{
			lpDispatch->GetSendStream().PutBuffer(szBuffer, dwLength, cCMD_In);
		}
	}
}

void CCreatureManager::SendFindPartyMsg(unsigned long dwSenderCID, const char* szCompressedPacket, unsigned long dwPacketSize)
{
	CharacterMap::iterator pos = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();

	CCharacter* lpSender = GetCharacter(dwSenderCID);
	if (NULL != lpSender)
	{
		for (; pos != end; ++pos)
		{
			CCharacter* lpCharacter = pos->second;

			if (NULL != lpCharacter)
			{
				if (0 != lpCharacter->GetMapIndex()) continue;
				if (lpCharacter == lpSender || EnemyCheck::EC_ENEMY == lpSender->IsEnemy(lpCharacter)) continue;
				
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->GetSendStream().PutBuffer(szCompressedPacket, dwPacketSize, CmdCharChat);
				}
			}
		}
	}
}


void CCreatureManager::CalculateEliteBonus(unsigned short *usPeopleNum)
{
	// 오토 밸런스가 꺼지면 엘리트 보너스가 바뀌지 않는다.
	if (false == m_bAutoBalance) { return; }

	EliteBonus::EliteBonusData NewEliteBonus;

	unsigned short wMin = 0;
	unsigned short wMax = 0;

	if (usPeopleNum[CClass::HUMAN] < usPeopleNum[CClass::AKHAN])
	{
		wMin = usPeopleNum[CClass::HUMAN] - 1;
		wMax = usPeopleNum[CClass::AKHAN] - 1;
		NewEliteBonus.m_cNation = CClass::HUMAN;
	}
	else
	{
		wMin = usPeopleNum[CClass::AKHAN] - 1;
		wMax = usPeopleNum[CClass::HUMAN] - 1;
		NewEliteBonus.m_cNation = CClass::AKHAN;
	}

	if (0 == wMin)
	{
		NewEliteBonus.m_cLevel = EliteBonus::MAX_BONUS_LEVEL;
	}
	else
	{
		float fRate = static_cast<float>(wMax) / static_cast<float>(wMin);

		if		(1.5f <= fRate && fRate < 2.0f)	{ NewEliteBonus.m_cLevel = 1; }
		else if (2.0f <= fRate && fRate < 2.5f)	{ NewEliteBonus.m_cLevel = 2; }
		else if (2.5f <= fRate && fRate < 3.0f)	{ NewEliteBonus.m_cLevel = 3; }
		else if (3.0f <= fRate && fRate < 3.5f)	{ NewEliteBonus.m_cLevel = 4; }
		else if (3.5f <= fRate)					{ NewEliteBonus.m_cLevel = 5; }
	}

	if (0 == NewEliteBonus.m_cLevel) 
	{ 
		NewEliteBonus.m_cNation = CClass::MAX_RACE; 
	}

	SetEliteBonus(NewEliteBonus);
}

class CSendEliteBonus
{
public:

	bool operator () (CCharacter* lpCharacter)
	{
		if (NULL != lpCharacter)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharEliteBonus(
					lpDispatch->GetSendStream(), lpCharacter->GetEliteBonus());
			}
		}

		return false;
	}
};

void CCreatureManager::SetEliteBonus(EliteBonus::EliteBonusData eliteBonus)
{
	if (m_EliteBonus.m_cNation != eliteBonus.m_cNation || m_EliteBonus.m_cLevel != eliteBonus.m_cLevel)
	{
		m_EliteBonus.m_cNation = eliteBonus.m_cNation;
		m_EliteBonus.m_cLevel = eliteBonus.m_cLevel;

		ProcessAllCharacter(CSendEliteBonus());
	}
}

float CCreatureManager::GetFameBonus(CClass::RaceType eRace, FightResult eResult)
{
	if (eRace == m_EliteBonus.m_cNation)
	{
		switch (eResult)
		{
			case WIN:
			{
				switch (m_EliteBonus.m_cLevel)
				{
					case 1:		return 1.2f;
					case 2:		return 1.4f;
					case 3:		return 1.6f;
					case 4:		return 1.8f;
					case 5:		return 2.0f;
				}
				break;
			}

			case LOSE:
			{
				switch (m_EliteBonus.m_cLevel)
				{
					case 1:		return 0.9f;
					case 2:		return 0.8f;
					case 3:		return 0.7f;
					case 4:		return 0.6f;
					case 5:		return 0.5f;
				}
				break;
			}
		}
	}

	return 1.0f;
}


void CCreatureManager::PushRespawnQueue(CCharacter* lpCharacter, unsigned char cPointNumber)
{
	unsigned short wError = PktBase::NO_SERVER_ERR;
	long lTime = 0;

	if (m_wCharacterNum[CClass::HUMAN] + m_wCharacterNum[CClass::AKHAN] > CServerSetup::GetInstance().GetBattleLimit())
	{
		if (m_wCharacterNum[CClass::HUMAN] > m_wCharacterNum[CClass::AKHAN] * 1.5 && 
			CClass::HUMAN == lpCharacter->GetRace())
		{
			wError = PktBGRsAck::FAIL_PERSONNEL_OVER;
		}
		if (m_wCharacterNum[CClass::AKHAN] > m_wCharacterNum[CClass::HUMAN] * 1.5 && 
			CClass::AKHAN == lpCharacter->GetRace())
		{
			wError = PktBGRsAck::FAIL_PERSONNEL_OVER;
		}
	}

	if (PktBase::NO_SERVER_ERR == wError)
	{
		int nIndex = 0;
		for (; nIndex < MAX_POINT_NUM; ++nIndex)
		{
			if (false == m_lstRespawn[nIndex].empty()) { break; }
		}
		if (nIndex == MAX_POINT_NUM)
		{
			m_dwLastUpdateTime = CPulse::GetInstance().GetLastTick();
		}

		PopRespawnQueue(lpCharacter);

		if (0 != m_wCharacterNum[CClass::HUMAN] && 0 != m_wCharacterNum[CClass::AKHAN])
		{
			switch (lpCharacter->GetRace())
			{
				case CClass::HUMAN:
					lTime = m_wCharacterNum[CClass::HUMAN] / m_wCharacterNum[CClass::AKHAN] * 5000;
					break;

				case CClass::AKHAN:
					lTime = m_wCharacterNum[CClass::AKHAN] / m_wCharacterNum[CClass::HUMAN] * 5000;
					break;
			}
		}

		BattleGroundRespawnInfo tempInfo(lpCharacter, m_RespawnPoint[cPointNumber], lTime);
		m_lstRespawn[cPointNumber].push_back(tempInfo);
	}

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		GameClientSendPacket::SendCharBattleGroundRespawn(lpDispatch->GetSendStream(), lpCharacter->GetCID(), 
			static_cast<unsigned short>(m_lstRespawn[cPointNumber].size()), static_cast<unsigned short>(m_lstRespawn[cPointNumber].size()), 
			static_cast<unsigned short>(lTime / 1000), m_wCharacterNum[CClass::HUMAN], m_wCharacterNum[CClass::AKHAN], wError);
	}
}

void CCreatureManager::PopRespawnQueue(CCharacter* lpCharacter)
{
	for (int nIndex = 0; nIndex < MAX_POINT_NUM; ++nIndex)
	{
		if (true == m_lstRespawn[nIndex].empty()) { continue; }

		for (RespawnQueue::iterator it = m_lstRespawn[nIndex].begin(); it != m_lstRespawn[nIndex].end(); )
		{
			BattleGroundRespawnInfo tempInfo = *it;

			if (tempInfo.m_lpCharacter == lpCharacter)
			{
				it = m_lstRespawn[nIndex].erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void CCreatureManager::ProcessRespawnQueue(void)
{
	for (int nIndex = 0; nIndex < MAX_POINT_NUM; ++nIndex)
	{
		if (true == m_lstRespawn[nIndex].empty()) { continue; }

		unsigned short wTurn = 0;

		for (RespawnQueue::iterator it = m_lstRespawn[nIndex].begin(); it != m_lstRespawn[nIndex].end();)
		{
			BattleGroundRespawnInfo tempInfo = *it;
				
			tempInfo.m_lLeftTime -= (CPulse::GetInstance().GetLastTick() - m_dwLastUpdateTime);

			if (0 >= tempInfo.m_lLeftTime)
			{
				tempInfo.m_lpCharacter->Respawn(CCharRespawnMgr::RST_BATTLE, tempInfo.m_RespawnPos);
                
                if (0 != tempInfo.m_lpCharacter->GetPID())
                {		
                    // 파티원 리스폰을 알린다.
                    GameClientSendPacket::SendCharDeadToParty(tempInfo.m_lpCharacter, 0, PktDeadInfo::RESPAWN);
                }

				it = m_lstRespawn[nIndex].erase(it);
			}
			else
			{
				++wTurn;

				CGameClientDispatch* lpDispatch = tempInfo.m_lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					GameClientSendPacket::SendCharBattleGroundRespawn(lpDispatch->GetSendStream(), tempInfo.m_lpCharacter->GetCID(), 
						wTurn, static_cast<unsigned short>(m_lstRespawn[nIndex].size()), static_cast<unsigned short>(tempInfo.m_lLeftTime / 1000), 
						m_wCharacterNum[CClass::HUMAN], m_wCharacterNum[CClass::AKHAN], 0);
				}

				++it;
			}
		}	
	}
}

bool CCreatureManager::SendRespawnQueue(unsigned long dwCID)
{
	CCharacter* lpCharacter = GetCharacter(dwCID);
	if (NULL == lpCharacter) { return false; }

	PopRespawnQueue(lpCharacter);

	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL == lpDispatch) { return false; }

	CSendStream& SendStream = lpDispatch->GetSendStream();
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktRsWaitQueueAck));
	if (NULL == lpBuffer) { return false; }

	PktRsWaitQueueAck* lpPktRsWQAck = reinterpret_cast<PktRsWaitQueueAck *>(lpBuffer);
	lpPktRsWQAck->m_dwCharID = dwCID;
	
	switch (lpCharacter->GetRace())
	{
		case CClass::HUMAN:
			lpPktRsWQAck->m_wWaitNum[0] = static_cast<unsigned short>(m_lstRespawn[POINT_HUMAN1].size());
			lpPktRsWQAck->m_wWaitNum[1] = static_cast<unsigned short>(m_lstRespawn[POINT_HUMAN2].size());
			lpPktRsWQAck->m_wWaitNum[2] = static_cast<unsigned short>(m_lstRespawn[POINT_HUMAN3].size());
			break;

		case CClass::AKHAN:
			lpPktRsWQAck->m_wWaitNum[0] = static_cast<unsigned short>(m_lstRespawn[POINT_AKHAN1].size());
			lpPktRsWQAck->m_wWaitNum[1] = static_cast<unsigned short>(m_lstRespawn[POINT_AKHAN2].size());
			lpPktRsWQAck->m_wWaitNum[2] = static_cast<unsigned short>(m_lstRespawn[POINT_AKHAN3].size());
			break;

		default:
			return false;
	}

	return SendStream.WrapCrypt(sizeof(PktRsWaitQueueAck), CmdCharRespawnWaitQueue, 0, 0);
}

unsigned char CCreatureManager::GetBonusTurn(const unsigned short wMapIndex)
{
	const static unsigned char INIT_DEFAULT_VALUE = 0x80;
	unsigned char cResult = INIT_DEFAULT_VALUE;

	MonsterMap::iterator pos = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();

	for (; pos != end; ++pos)
	{
		CMonster* lpMonster = pos->second;
		if (NULL == lpMonster) 
		{
			continue;
		}
		if (lpMonster->GetMapIndex() != wMapIndex) 
		{
			continue;
		}

		// BG_TODO : 배틀 그라운드 서버군의 석상에서 마일리지를 줄 경우 수정이 필요하다!!
		CStatue* lpStatue = lpMonster->DowncastToStatue();
		if (NULL != lpStatue && lpStatue->GetStatus().m_nNowHP > 0)
		{
			// 1레벨
			if (MonsterInfo::STATUE_HUMAN_COMPLETE1 != lpStatue->GetKID() && 
				MonsterInfo::STATUE_AKHAN_COMPLETE1 != lpStatue->GetKID())
			{
				return CClass::MAX_RACE;
			}
			// 2레벨
			else if (MonsterInfo::STATUE_HUMAN_COMPLETE2 != lpStatue->GetKID() && 
				MonsterInfo::STATUE_AKHAN_COMPLETE2 != lpStatue->GetKID())
			{
				return CClass::MAX_RACE;
			}

			if (INIT_DEFAULT_VALUE == cResult)
			{
				cResult = lpStatue->GetNation();
			}
			else
			{
				// 모든 석상이 한 종족의 것일 때만 보너스를 받을 수 있다.
				if (cResult != lpStatue->GetNation())
				{
					return CClass::MAX_RACE;
				}
			}
		}
	}

	if (Creature::KARTERANT == cResult)
	{
		return CClass::HUMAN;
	}

	if (Creature::MERKADIA == cResult)
	{
		return CClass::AKHAN;
	}

	return CClass::MAX_RACE;
}


void CCreatureManager::ProcessBattleGround()
{
	// Zone 이 Stron War일경우만 
	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		if(CGameTimeMgr::GetInstance().IsRealmWarTime())
		{
			// edith 2010.01.16 모든 석상존에서 1분에 한번씩 메달을 부여한다.
			ProcessAllCharacter(CBlessMileageInfo(0));


			// 1분에 1번씩 공헌 메달을 나눠준다.
			MonsterMap::iterator mon_pos = m_MonsterMap.begin();
			MonsterMap::iterator mon_end = m_MonsterMap.end();

			for (; mon_pos != mon_end; ++mon_pos)
			{
				// 석상만 공헌 메달을 나눠준다.
				CStatue* lpStatue = (mon_pos->second)->DowncastToStatue();
				if (NULL != lpStatue && lpStatue->GetStatus().m_nNowHP > 0)
				{
					lpStatue->GiveMileage(1);//medal shower
				}
			}
		}

		// 죽은 상태로 1분 이상 리스폰 하지 않고 있는 시체 처리
		CharacterMap::iterator char_pos = m_CharacterMap.begin();
		CharacterMap::iterator char_end = m_CharacterMap.end();
		
		CPulse& pulse = CPulse::GetInstance();

		unsigned long dwLastTick = pulse.GetLastTick();		
		unsigned long dwRespawnTick = CCharacter::BATTLE_GROUND_PULSE * pulse.GetTicksPerPulse();//check the m_monstermap too hayzohar

		for (; char_pos != char_end; ++char_pos)
		{
			CCharacter* lpCharacter = char_pos->second;

			if (0 != lpCharacter && 0 == lpCharacter->GetStatus().m_nNowHP)
			{
				if (lpCharacter->GetLastTime() + dwRespawnTick < dwLastTick)
				{
					lpCharacter->AutoRespawn();
				}
			}
		}
	}
/*
	// Zone 이 3번 BattleGround 일 경우의 처리만 담당..
	if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
	{
		// 1분에 1번씩 공헌 메달을 나눠준다.
		MonsterMap::iterator mon_pos = m_MonsterMap.begin();
		MonsterMap::iterator mon_end = m_MonsterMap.end();

		for (; mon_pos != mon_end; ++mon_pos)
		{
			// 석상만 공헌 메달을 나눠준다.
			CStatue* lpStatue = (mon_pos->second)->DowncastToStatue();
			if (NULL != lpStatue && lpStatue->GetStatus().m_nNowHP > 0)
			{
				lpStatue->GiveMileage(0);
			}
		}

		// 죽은 상태로 1분 이상 리스폰 하지 않고 있는 시체 처리
		CharacterMap::iterator char_pos = m_CharacterMap.begin();
		CharacterMap::iterator char_end = m_CharacterMap.end();
		
		CPulse& pulse = CPulse::GetInstance();

		unsigned long dwLastTick = pulse.GetLastTick();		
		unsigned long dwRespawnTick = CCharacter::BATTLE_GROUND_PULSE * pulse.GetTicksPerPulse();

		for (; char_pos != char_end; ++char_pos)
		{
			CCharacter* lpCharacter = char_pos->second;

			if (0 != lpCharacter && 0 == lpCharacter->GetStatus().m_nNowHP)
			{
				if (lpCharacter->GetLastTime() + dwRespawnTick < dwLastTick)
				{
					lpCharacter->AutoRespawn();
				}
			}
		}
	}
*/
/*
	// 서버가 배틀 그라운드 일 경우에만
	if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
	{
		SYSTEMTIME systemTime;
		GetSystemTime(&systemTime);

		if ((systemTime.wMinute >= CCreatureManager::STATUE_REST_TIME_1ST_START && systemTime.wMinute <= CCreatureManager::STATUE_REST_TIME_1ST_END) ||
			(systemTime.wMinute >= CCreatureManager::STATUE_REST_TIME_2ND_START && systemTime.wMinute <= CCreatureManager::STATUE_REST_TIME_2ND_END))
		{
			if (true == m_bRest) return;
			else
			{
				// 배틀 그라운드에 있는 모든 유저들을 이동 시킨다.
				for (CharacterMap::iterator it = m_CharacterMap.begin(); it != m_CharacterMap.end(); ++it)
				{
					CCharacter* lpCharacter = it->second;
					if (NULL != lpCharacter)
					{
						lpCharacter->AutoRespawn();
					}
				}

				// 모든 석상을 다 죽이고, 아무것도 소환하지 않는다.
				for (MonsterMap::iterator it = m_MonsterMap.begin(); it != m_MonsterMap.end(); ++it)
				{
					CStatue* lpStatue = (it->second)->DowncastToStatue();
					if (NULL != lpStatue)
					{
						lpStatue->Rest();
					}
				}

				m_bRest = true;
			}
		}
		else if (systemTime.wMinute >= CCreatureManager::STATUE_RESPAWN_TIME_1ST || systemTime.wMinute >= CCreatureManager::STATUE_RESPAWN_TIME_2ND)
		{
			// 죽은 상태로 1분 이상 리스폰 하지 않고 있는 시체 처리
			for (CharacterMap::iterator it = m_CharacterMap.begin(); it != m_CharacterMap.end(); ++it)
			{
				CCharacter* lpCharacter = it->second;
				if (NULL != lpCharacter && 0 == lpCharacter->GetStatus().m_nNowHP)
				{
					// TODO : 배틀 그라운드 수정
					// Tick 과 Pluse 가 썩여있다. CPulse 를 Base Library 의 업데이트로 잡고 있는 상황이라서
					// DEFAULT_TICKS_PER_PULSE 를 사용하였다.
					if (CPulse::GetInstance().GetLastTick() > 
						lpCharacter->GetLastTime() + CCharacter::BATTLE_GROUND_PULSE * CPulse::DEFAULT_TICKS_PER_PULSE)
					{
						lpCharacter->AutoRespawn();
					}
				}
			}

			// 석상 초기화 상태로 변화 처리
			if (false == m_bRest) return;
			else
			{
				// TODO : 순서가 중요하다. Arrangement Script 파일에 들어간 순서대로 해야한다.
				// 휴먼, 중립, 아칸 석상을 순서대로 하나씩 소환한다.
				enum { HUMAN_STATUE = 0, NUETRALITY_STATUE = 1, AKHAN_STATUE = 2, NONE = 100 };
				DWORD dwKind, dwOldKind = NONE;
				
				for (MonsterMap::iterator it = m_MonsterMap.begin(); it != m_MonsterMap.end(); ++it)
				{
					CStatue* lpStatue = (it->second)->DowncastToStatue();
					if (NULL != lpStatue)
					{
						// 종류를 얻어온다. (HUMAN = 0, NUETRALITY = 1, AKHAN = 2)
						dwKind = (lpStatue->GetCID() & ~(Creature::MONSTER_BIT | Creature::MONSTER_KIND_BIT)) >> 16;
						if (dwOldKind != dwKind)
						{
							switch (dwKind)
							{
							case HUMAN_STATUE:
								lpStatue = lpStatue->GetLinkStatue(MonsterInfo::STATUE_HUMAN_COMPLETE);
								lpStatue->InitMonster(lpStatue->GetOriginalPos(), CCell::CellMoveType::LOGINOUT);
								break;

							case NUETRALITY_STATUE:
								lpStatue = lpStatue->GetLinkStatue(MonsterInfo::STATUE_NEUTRALITY);
								lpStatue->InitMonster(lpStatue->GetOriginalPos(), CCell::CellMoveType::LOGINOUT);
								break;

							case AKHAN_STATUE:
								lpStatue = lpStatue->GetLinkStatue(MonsterInfo::STATUE_AKHAN_COMPLETE);
								lpStatue->InitMonster(lpStatue->GetOriginalPos(), CCell::CellMoveType::LOGINOUT);
								break;
							}

							dwOldKind = dwKind;
						}
					}
				}

				m_bRest = false;
			}
		}
	}
*/
}


// 몬스터를 생성할때 CID가 겹치지 않게 하기 위해서 호출하는 함수
unsigned short CCreatureManager::GetAvailableMonsterUID(unsigned short wKindID)
{
	if (m_AdminMonsterUIDMap.end() == m_AdminMonsterUIDMap.find(wKindID))
	{
		m_AdminMonsterUIDMap.insert(std::make_pair(wKindID, INIT_UID)).second;
	}

	unsigned long nUID = ((m_AdminMonsterUIDMap[wKindID] << 16) | wKindID);
	if (NULL != GetMonster(nUID))
	{
		if (m_AdminMonsterUIDMap[wKindID] == INIT_UID)
		{
			// 여유 공간이 없다면...
			return NO_BLANK_UID;
		}

		m_AdminMonsterUIDMap[wKindID] = INIT_UID - 1;
		return INIT_UID;
	}

    return m_AdminMonsterUIDMap[wKindID]--;
}

bool CCreatureManager::ChangeCharacterName(unsigned long dwCID, const char* szChangedName)
{   
    
    CCharacter* lpCharacter = GetCharacter(dwCID);

    if(0 != lpCharacter)
    {
        const char* szCurrentName = lpCharacter->GetCharacterName();

        unsigned long dwOriginalHashedKey = Math::HashFunc::sdbmHash(
            reinterpret_cast<const unsigned char*>(szCurrentName));

        unsigned long dwChangeHashKey = Math::HashFunc::sdbmHash(
            reinterpret_cast<const unsigned char*>(szChangedName));

        std::pair<CharacterMultimap::iterator, CharacterMultimap::iterator> findpair = 
            m_CharacterNameMap.equal_range(dwOriginalHashedKey);

        for (;findpair.first != findpair.second;)
        {
            if(0 == strncmp(findpair.first->second->GetCharacterName(),
                szCurrentName, CHAR_INFOST::MAX_NAME_LEN))
            {
                m_CharacterNameMap.erase(findpair.first);
                break;
            }
            else
            {
                ++findpair.first;
            }
        }        

        lpCharacter->SetCharacterName(szChangedName);
        m_CharacterNameMap.insert(std::make_pair(dwChangeHashKey, lpCharacter));
        return true;
    }

    return false;
}

// 길드전과 국가전 전쟁 시간시에 존이동 처리 함수
void CCreatureManager::MoveCharacterToGuildWarZone(unsigned char cType, bool bOn)
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;
	unsigned char cFlag = 0;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter)
		{
			switch (cType)
			{
				case GameTime::GUILD:	cFlag = lpCharacter->GetGuildWarFlag();		break;
				case GameTime::REALM:	cFlag = lpCharacter->GetRealmWarFlag();		break;

				default:
				{
					++itr;
					continue;
				}
			}

			if (bOn && (Creature::WAR_ON == cFlag || Creature::WAR_INSTANCE == cFlag))
			{
				lpCharacter->MoveToGuildWarZone();
			}
			else if (!bOn && Creature::WAR_OFF == cFlag)
			{
				lpCharacter->MoveToGuildWarZone();
			}
		}

		++itr;
	}
}

void CCreatureManager::MoveCharacterToRealmWarZone(unsigned char cType, bool bOn)
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;
	unsigned char cFlag = 0;

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter)
		{
			switch (cType)
			{
				case GameTime::GUILD:	cFlag = lpCharacter->GetGuildWarFlag();		break;
				case GameTime::REALM:	cFlag = lpCharacter->GetRealmWarFlag();		break;

				default:
				{
					++itr;
					continue;
				}
			}

			if (bOn && (Creature::WAR_ON == cFlag || Creature::WAR_INSTANCE == cFlag))
			{
				lpCharacter->MoveToRealmWarZone();
			}
			else if (!bOn && Creature::WAR_OFF == cFlag)
			{
				// 강제참가.
				if (0 != lpDBAgentDispatch)
					GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), lpCharacter->GetCID(), GameTime::REALM, Creature::WAR_INSTANCE, 0);

//				lpCharacter->MoveToRealmWarZone();
			}
		}

		++itr;
	}
}

void CCreatureManager::ClearGuildWarInstanceFlag()
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter && Creature::WAR_INSTANCE == lpCharacter->GetGuildWarFlag())
		{
			lpCharacter->SetGuildWarFlag(Creature::WAR_OFF);

			// 패킷 전송
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendWarOnOff(lpDispatch->GetSendStream(), lpCharacter->GetCID(), GameTime::GUILD, Creature::WAR_OFF, 0);
			}
		}

		++itr;
	}
}

void CCreatureManager::ClearRealmWarInstanceFlag()
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter && Creature::WAR_INSTANCE == lpCharacter->GetRealmWarFlag())
		{
			lpCharacter->SetRealmWarFlag(Creature::WAR_OFF);

			// 패킷 전송
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendWarOnOff(lpDispatch->GetSendStream(), lpCharacter->GetCID(), GameTime::REALM, Creature::WAR_OFF, 0);
			}
		}

		++itr;
	}
}

void CCreatureManager::SetRealmStatueInfo(unsigned long dwIndex, unsigned short wKID, unsigned long dwLostHP)
{
	MonsterMap::iterator itr = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		unsigned long dwStatueIndex = (lpStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
		if (dwStatueIndex == dwIndex)
		{
			lpStatue->Rest();
			lpStatue = lpStatue->GetLinkStatue(wKID);
			lpStatue->InitMonster(lpStatue->GetOriginalPos());
			lpStatue->GetStatus().m_nNowHP = static_cast<unsigned short>(lpStatue->GetStatus().m_StatusInfo.m_nMaxHP - dwLostHP);

			// 석상 리스폰 포인트 추가
			if (lpStatue->EnableRespawn() && MonsterInfo::IsRaceCompleteStatueKID(lpStatue->GetKID()))
			{
				CCharRespawnMgr::GetInstance().AddRealmStatueRespawnPoint(dwStatueIndex, lpStatue->GetNation(), lpStatue->GetCurrentPos());
			}
			return;
		}

		++itr;
	}
}

void CCreatureManager::InitRealmStatue()
{
	MonsterMap::iterator itr = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	SERLOG0(g_Log, "석상 정보 초기화를 시작합니다.");

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		unsigned short wKID = lpStatue->GetKID();
		if (wKID != MonsterInfo::GetDefaultStatueKID(wKID))
		{
			lpStatue->Rest();
			lpStatue = lpStatue->GetLinkStatue(MonsterInfo::GetDefaultStatueKID(wKID));
			lpStatue->InitMonster(lpStatue->GetOriginalPos());

			// 중계서버에 중립 KID 로 변경 정보 전송
			lpStatue->SendKIDUpdate();

			// 석상 리스폰 포인트 삭제
			unsigned long dwStatueIndex = (lpStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
			CCharRespawnMgr::GetInstance().DeleteRealmStatueRespawnPoint(dwStatueIndex);
		}

		lpStatue->GetStatus().m_nNowHP = lpStatue->GetStatus().m_StatusInfo.m_nMaxHP;

		// 중계서버에 HP 만땅 정보 전송
		lpStatue->SendHPUpdate(true);
		
		++itr;
	}
}

void CCreatureManager::InitRealmLoadingStatue()
{
	MonsterMap::iterator itr = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	SERLOG0(g_Log, "석상 정보 중립화를 시작합니다.");
	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		unsigned short wKID = lpStatue->GetKID();
		if (MonsterInfo::IsLoadingStatueKID(wKID))
		{
			lpStatue->Rest();
			lpStatue = lpStatue->GetLinkStatue(MonsterInfo::GetDefaultStatueKID(wKID));
			lpStatue->InitMonster(lpStatue->GetOriginalPos());

			// 중계서버에 중립 KID 로 변경 정보 전송
			lpStatue->SendKIDUpdate();
		}

		lpStatue->GetStatus().m_nNowHP = lpStatue->GetStatus().m_StatusInfo.m_nMaxHP;

		// 중계서버에 HP 만땅 정보 전송
		lpStatue->SendHPUpdate(true);

		++itr;
	}
}

int	CCreatureManager::GetRealmStatueNum()
{
	int nCount = 0;
	MonsterMap::const_iterator itr = m_MonsterMap.begin();
	MonsterMap::const_iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		++nCount;
		++itr;
	}

	return nCount;
}


bool CCreatureManager::SendRealmStatueDisplayInfo(CSendStream& SendStream)
{
	int nRealmStatueNum = GetRealmStatueNum();
	const int BUFFER_SIZE = sizeof(PktStatueCmd) + sizeof(StatueDisplayInfo) * nRealmStatueNum;
	char* lpBuffer = SendStream.GetBuffer(BUFFER_SIZE);
	if (NULL == lpBuffer) { return false; }

	PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd*>(lpBuffer);
	StatueDisplayInfo* lpDisplayInfo = reinterpret_cast<StatueDisplayInfo*>(lpPktStatueCmd + 1);

	lpPktStatueCmd->m_wSubCmd	= PktStatueCmd::CMD_DISPLAY_INFO;
	lpPktStatueCmd->m_wSize		= static_cast<unsigned short>(BUFFER_SIZE - sizeof(PktStatueCmd));
	lpPktStatueCmd->m_dwValue	= nRealmStatueNum;

	MonsterMap::iterator itr = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		lpDisplayInfo->m_dwIndex		= (lpStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
		lpDisplayInfo->m_dwKID			= lpStatue->GetKID();
		lpDisplayInfo->m_cNation		= lpStatue->GetNation();
		lpDisplayInfo->m_bSummoning		= MonsterInfo::IsLoadingStatueKID(lpStatue->GetKID());
		lpDisplayInfo->m_bAttacked		= false;
		lpDisplayInfo->m_Pos.fPointX	= lpStatue->GetCurrentPos().m_fPointX;
		lpDisplayInfo->m_Pos.fPointY	= lpStatue->GetCurrentPos().m_fPointY;
		lpDisplayInfo->m_Pos.fPointZ	= lpStatue->GetCurrentPos().m_fPointZ;

		++lpDisplayInfo;
		++itr;
	}

	return SendStream.WrapCrypt(BUFFER_SIZE, CmdStatueCmd, 0, 0);
}


bool CCreatureManager::SendRealmStatueDisplayInfo(unsigned long dwIndex, bool bAttacked)
{
	const int BUFFER_SIZE = sizeof(PktStatueCmd) + sizeof(StatueDisplayInfo) * PktStatueCmd::MAX_STATUE_NUM;
	char szBuffer[BUFFER_SIZE] = {0, };

	PktStatueCmd* lpPktStatueCmd = reinterpret_cast<PktStatueCmd*>(szBuffer);
	StatueDisplayInfo* lpDisplayInfo = reinterpret_cast<StatueDisplayInfo*>(lpPktStatueCmd + 1);

	MonsterMap::iterator itr = m_MonsterMap.begin();//check the difference btw this and statuemap::iterator hayzohar
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;
	unsigned short wSize = 0;
	unsigned long dwCount = 0;

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}
		
		unsigned long dwStatueIndex = (lpStatue->GetCID() & ~Creature::MONSTER_BIT) >> 16;
		if (0 == dwIndex || dwIndex == dwStatueIndex)
		{
			lpDisplayInfo->m_dwIndex		= dwStatueIndex;
			lpDisplayInfo->m_dwKID			= lpStatue->GetKID();
			lpDisplayInfo->m_cNation		= lpStatue->GetNation();
			lpDisplayInfo->m_bSummoning		= MonsterInfo::IsLoadingStatueKID(lpStatue->GetKID());
			lpDisplayInfo->m_bAttacked		= bAttacked;
			lpDisplayInfo->m_Pos.fPointX	= lpStatue->GetCurrentPos().m_fPointX;
			lpDisplayInfo->m_Pos.fPointY	= lpStatue->GetCurrentPos().m_fPointY;
			lpDisplayInfo->m_Pos.fPointZ	= lpStatue->GetCurrentPos().m_fPointZ;

			wSize += sizeof(StatueDisplayInfo);
			++lpDisplayInfo;
			++dwCount;
		}
	
		++itr;
	}

	lpPktStatueCmd->m_wSubCmd	= PktStatueCmd::CMD_DISPLAY_INFO;
	lpPktStatueCmd->m_wSize		= wSize;
	lpPktStatueCmd->m_dwValue	= dwCount;

	if (PacketWrap::WrapCrypt(szBuffer, sizeof(PktStatueCmd) + wSize, CmdStatueCmd, 0, 0))
	{
		SendAllCharacter(szBuffer, sizeof(PktStatueCmd) + wSize, CmdStatueCmd);
		return true;
	}

	return false;
}

void CCreatureManager::CalculateRealmStatueEnchantInfo()
{
	memset(m_RealmEnchantInfo, 0, sizeof(sRealmEnchant) * 2);

	MonsterMap::iterator itr = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();
	CMonster* lpMonsetr = NULL;
	CStatue* lpStatue = NULL;

	while (itr != end)
	{
		lpMonsetr = itr->second;
		if (NULL == lpMonsetr)
		{
			++itr;
			continue;
		}

		lpStatue = lpMonsetr->DowncastToStatue();
		if (NULL == lpStatue)
		{
			++itr;
			continue;
		}

		if (0 == lpStatue->GetStatus().m_nNowHP)
		{
			++itr;
			continue;
		}

		if (MonsterInfo::IsRaceCompleteStatueKID(lpStatue->GetKID()))
		{
			switch (lpStatue->GetNation())
			{
				case Creature::KARTERANT:
				{
					switch (lpStatue->GetRealmStatueEnchantType())
					{
						case MonsterInfo::SE_HP:
							m_RealmEnchantInfo[CClass::HUMAN].m_cHPPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_MP:
							m_RealmEnchantInfo[CClass::HUMAN].m_cMPPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_EXP:
							m_RealmEnchantInfo[CClass::HUMAN].m_cExpPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_DROPRATE:
							m_RealmEnchantInfo[CClass::HUMAN].m_cDropRate += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_NONE:
						default:
							break;
					}
				}
				break;

				case Creature::MERKADIA:
				{
					switch (lpStatue->GetRealmStatueEnchantType())
					{
						case MonsterInfo::SE_HP:
							m_RealmEnchantInfo[CClass::AKHAN].m_cHPPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_MP:
							m_RealmEnchantInfo[CClass::AKHAN].m_cMPPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_EXP:
							m_RealmEnchantInfo[CClass::AKHAN].m_cExpPercent += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_DROPRATE:
							m_RealmEnchantInfo[CClass::AKHAN].m_cDropRate += lpStatue->GetRealmStatueEnchantPercent();
							break;

						case MonsterInfo::SE_NONE:
						default:
							break;
					}
				}
				break;
			}
		}

		++itr;
	}

	// ROW에서는 공선전을 16번에서 하지 않기 때문에 해당 보너스를 제거한다.
	// 공선전을 할 존을 따로 지정해서 추후에 추가한다.
	// 컨텐츠 : 공성전
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		AddCastleBonus();
	}
}

void CCreatureManager::AddRealmStatueEnchant(CCharacter* lpCharacter)
{
	// CalculateRealmStatueEnchantInfo() 함수가 호출되어 있어야한다.!!

	unsigned char cRace = lpCharacter->GetRace();

	// 죽어있는 녀석은 부활후에 버프를 걸어준다.
	if(lpCharacter->IsDead())
	{
		lpCharacter->SetRealmWarBuffer(TRUE);
		return;
	}

	lpCharacter->SetRealmWarBuffer(FALSE);

	if (m_RealmEnchantInfo[cRace].m_cHPPercent > 0)
	{
		Skill::CAddSpell<CPowerStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter,
			Skill::SpellType::STATUE_SPELL, Skill::SpellID::PowerStatue, m_RealmEnchantInfo[cRace].m_cHPPercent,
			CSpell::REALM_TIME))(lpCharacter);
	}

	if (m_RealmEnchantInfo[cRace].m_cMPPercent > 0)
	{
		Skill::CAddSpell<CIntStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter,
			Skill::SpellType::STATUE_SPELL, Skill::SpellID::IntStatue, m_RealmEnchantInfo[cRace].m_cMPPercent,
			CSpell::REALM_TIME))(lpCharacter);
	}

	if (m_RealmEnchantInfo[cRace].m_cExpPercent > 0)
	{
		Skill::CAddSpell<CExpStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter,
			Skill::SpellType::STATUE_SPELL, Skill::SpellID::ExpStatue, m_RealmEnchantInfo[cRace].m_cExpPercent,
			CSpell::REALM_TIME))(lpCharacter);
	}

	if (m_RealmEnchantInfo[cRace].m_cDropRate > 0)
	{
		Skill::CAddSpell<CWealthStatueSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter,
			Skill::SpellType::STATUE_SPELL, Skill::SpellID::WealthStatue, m_RealmEnchantInfo[cRace].m_cDropRate,
			CSpell::REALM_TIME))(lpCharacter);
	}
}

void CCreatureManager::AddRealmStatueEnchant()
{
	// CalculateRealmStatueEnchantInfo() 함수가 호출되어 있어야한다.!!

	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (NULL == lpCharacter)
		{
			++itr;
			continue;
		}

		AddRealmStatueEnchant(lpCharacter);

		++itr;
	}
}

void CCreatureManager::ClearRealmStatueEnchant()
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();
	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (NULL == lpCharacter)
		{
			++itr;
			continue;
		}

		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::PowerStatue);
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::IntStatue);
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::ExpStatue);
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::WealthStatue);

		++itr;
	}
}

void CCreatureManager::AddCastleBonus()
{
	int iHumanCastleNum = Castle::CCastleMgr::GetInstance().GetCastleNum(Creature::KARTERANT, SERVER_ID::CAPITAL);
	int iAkhanCastleNum = Castle::CCastleMgr::GetInstance().GetCastleNum(Creature::MERKADIA, SERVER_ID::CAPITAL);

	if (iHumanCastleNum > 0)
	{
		m_RealmEnchantInfo[CClass::HUMAN].m_cHPPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::HUMAN].m_cHPPercent * 0.3f * iHumanCastleNum );

		m_RealmEnchantInfo[CClass::HUMAN].m_cMPPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::HUMAN].m_cMPPercent * 0.3f * iHumanCastleNum );

		m_RealmEnchantInfo[CClass::HUMAN].m_cExpPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::HUMAN].m_cExpPercent * 0.3f * iHumanCastleNum );

		m_RealmEnchantInfo[CClass::HUMAN].m_cDropRate += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::HUMAN].m_cDropRate * 0.3f * iHumanCastleNum );
	}

	if (iAkhanCastleNum > 0)
	{
		m_RealmEnchantInfo[CClass::AKHAN].m_cHPPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::AKHAN].m_cHPPercent * 0.3f * iAkhanCastleNum );

		m_RealmEnchantInfo[CClass::AKHAN].m_cMPPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::AKHAN].m_cMPPercent * 0.3f * iAkhanCastleNum );

		m_RealmEnchantInfo[CClass::AKHAN].m_cExpPercent += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::AKHAN].m_cExpPercent * 0.3f * iAkhanCastleNum );

		m_RealmEnchantInfo[CClass::AKHAN].m_cDropRate += static_cast<unsigned char>(
			m_RealmEnchantInfo[CClass::AKHAN].m_cDropRate * 0.3f * iAkhanCastleNum );
	}
}

void CCreatureManager::AddWorldWeaponEnchant(CAggresiveCreature* lpWeapon, unsigned char cNation)
{
	if (Creature::KARTERANT != cNation && Creature::MERKADIA != cNation) return;
	if (NULL == lpWeapon) return;

	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();

	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter)
		{
			lpCharacter->AddWorldWeaponEnchant(lpWeapon, cNation);
		}

		++itr;
	}
}

void CCreatureManager::ClearWorldWeaponEnchant()
{
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();

	CCharacter* lpCharacter = NULL;

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter)
		{
			lpCharacter->ClearWorldWeaponEnchant();
		}

		++itr;
	}
}

void CCreatureManager::ProcessWorldWeaponDamage(int iX, int iZ, unsigned char cUpgradeStep)
{
	CAggresiveCreature* pOffencer = reinterpret_cast<CAggresiveCreature* >( CSiegeObjectMgr::GetInstance().GetWorldWeapon() );
	CAggresiveCreature* Xv=0;//hayzohar
	if (NULL == pOffencer)
	{
		return;
	}

	CSiegeObject* lpSiegeObject = NULL;
	CCharacter* lpCharacter = NULL;
	float fDX, fDZ, fDist;
	int nDamage = 0;
	int nPreviousHP = 0;

	// ---------------------------------------------------------------------------------------------- //
	// 공성 오브젝트들 데미지 처리
	SiegeObjectMap::iterator SiegeItr = m_SiegeObjectMap.begin();
	SiegeObjectMap::iterator SiegeEnd = m_SiegeObjectMap.end();

	while (SiegeItr != SiegeEnd)
	{
		lpSiegeObject = SiegeItr->second;
		if (NULL != lpSiegeObject)
		{
			fDX = lpSiegeObject->GetCurrentPos().m_fPointX - iX;
			fDZ = lpSiegeObject->GetCurrentPos().m_fPointZ - iZ;
			fDist = sqrtf(fDX*fDX + fDZ*fDZ);

			// 범위 안에 들어 있다면...
			if (Siege::WEAPON_DAMAGE_RANGE >= fDist)
			{
				// Damage 의 1/4 에 해당하는 데미지 적용
				nDamage = static_cast<int>(((cUpgradeStep > 0 ) ? Siege::WEAPON_DAMAGE * cUpgradeStep : Siege::WEAPON_DAMAGE) * 0.25f);
				nPreviousHP = lpSiegeObject->GetStatus().m_nNowHP;

				// 성문은 성문 막기를 통해서 블럭률을 높이고, 막고 있는 캐릭터에게 데미지가 나눠져간다.
				if (lpSiegeObject->IsGate())
				{
					reinterpret_cast<CCastleGate*>( lpSiegeObject )->DivideDamage(pOffencer, nDamage);
					reinterpret_cast<CCastleGate*>( lpSiegeObject )->SendProtectGateInfo();
				}

				const unsigned short nThreatAmount = (lpSiegeObject->GetStatus().m_nNowHP < nDamage) ? lpSiegeObject->GetStatus().m_nNowHP : nDamage;
				lpSiegeObject->GetThreat().AddToThreatList(pOffencer, nThreatAmount);

				lpSiegeObject->GetStatus().m_nNowHP = (lpSiegeObject->GetStatus().m_nNowHP > nDamage) ? lpSiegeObject->GetStatus().m_nNowHP - nDamage : 0;

				// 공성 오브젝트라면 HP 정보를 DB중계 서버에 업데이트한다.
				lpSiegeObject->SendHPUpdateToDBAgent();

				if (lpSiegeObject->IsCamp())
				{
					// 공격 당하고 있다는 메세지를 보내준다.
					reinterpret_cast<CCamp*>(lpSiegeObject)->SendAttackedMessage();
				}
				else if (lpSiegeObject->IsEmblem())
				{
					// 공격 당하고 있다는 메세지를 보내준다.
					reinterpret_cast<CCastleEmblem*>(lpSiegeObject)->SendAttackedMessage();
				}

				// 죽었을 경우 (자살 스킬은 무효)
				// 이미 죽어있는 경우에 레져렉션 스킬을 받았을때는 Dead() 함수를 다시 호출하지 않는다.
				if (0 != nPreviousHP && 0 == lpSiegeObject->GetStatus().m_nNowHP && pOffencer != lpSiegeObject)
				{
					lpSiegeObject->Dead(pOffencer);
					lpSiegeObject->GetThreat().ClearAll();
				}

				// Attacked 패킷을 보내준다. (타고 있는 사람에게 공성 오브젝트의 CID 로...)
				CCharacter* lpRider = GetCharacter( lpSiegeObject->GetRiderCID() );
				if (NULL != lpRider)
				{
					CGameClientDispatch* lpDispatch = lpRider->GetDispatcher();
					if (NULL != lpRider)
					{
						AtType Type = {0,};
						Type.m_wType = 1;

						GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), pOffencer, lpSiegeObject,
							Type, 0, nDamage, 0, 0, PktBase::NO_SERVER_ERR);
					}
				}
			}
		}

		++SiegeItr;
	}

	// ---------------------------------------------------------------------------------------------- //


	// ---------------------------------------------------------------------------------------------- //
	// 캐릭터들 데미지 처리
	CharacterMap::iterator itr = m_CharacterMap.begin();
	CharacterMap::iterator end = m_CharacterMap.end();

	while (itr != end)
	{
		lpCharacter = itr->second;
		if (lpCharacter)
		{
			fDX = lpCharacter->GetCurrentPos().m_fPointX - iX;
			fDZ = lpCharacter->GetCurrentPos().m_fPointZ - iZ;
			fDist = sqrtf(fDX*fDX + fDZ*fDZ);

			// 범위 안에 들어 있다면...
			if (Siege::WEAPON_DAMAGE_RANGE >= fDist)
			{
				if (false == lpCharacter->IsRideArms())
				{
					// 절대 데미지
					nDamage = (cUpgradeStep > 0 ) ? Siege::WEAPON_DAMAGE * cUpgradeStep : Siege::WEAPON_DAMAGE;
					nPreviousHP = lpCharacter->GetStatus().m_nNowHP;

					const unsigned short nThreatAmount = (lpCharacter->GetStatus().m_nNowHP < nDamage) ? lpCharacter->GetStatus().m_nNowHP : nDamage;
					lpCharacter->GetThreat().AddToThreatList(pOffencer, nThreatAmount);

					lpCharacter->GetStatus().m_nNowHP = (lpCharacter->GetStatus().m_nNowHP > nDamage) ? lpCharacter->GetStatus().m_nNowHP - nDamage : 0;

					// 죽었을 경우 (자살 스킬은 무효)
					// 이미 죽어있는 경우에 레져렉션 스킬을 받았을때는 Dead() 함수를 다시 호출하지 않는다.
					if (0 != nPreviousHP && 0 == lpCharacter->GetStatus().m_nNowHP && pOffencer != lpCharacter)
					{
						lpCharacter->Dead(pOffencer);
						lpCharacter->GetThreat().ClearAll();
					}

					// Attacked 패킷을 보내준다.
					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpDispatch)
					{
						AtType Type = {0,};
						Type.m_wType = 1;

						GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), pOffencer, lpCharacter,
																Type, 0, nDamage, 0, 0, PktBase::NO_SERVER_ERR);
					}
				}
			}
		}

		++itr;
	}
	// ---------------------------------------------------------------------------------------------- //
}