#include "stdafx.h"
#include "VirtualMonsterMgr.h"

#include <Creature/Creature.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>

#include <Network/Dispatch/GameClient/SendCharAttack.h>



CVirtualMonsterMgr::CVirtualMonsterMgr()
: m_usSummonCount(0)
{
}

CVirtualMonsterMgr::~CVirtualMonsterMgr()
{
	DestroyMonsterList();
}



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
		}
		return true; 
	}
};

void	CVirtualMonsterMgr::DestroyMonsterList()
{
	std::for_each(m_MonsterMap.begin(), m_MonsterMap.end(), FnLeaveParty());
	std::for_each(m_MonsterMap.begin(), m_MonsterMap.end(), FnDeleteSecond());
	m_MonsterMap.clear();
	m_AdminMonsterUIDMap.clear();
}

bool	CVirtualMonsterMgr::AddMonster(CMonster* lpMonster)
{
	if (NULL == lpMonster) return false;

	unsigned long dwCID = lpMonster->GetCID();
	bool bResult = false;

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(dwCID);

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType)
	{
		bResult = m_MonsterMap.insert(std::make_pair(dwCID, reinterpret_cast<CMonster *>(lpMonster))).second;

		unsigned short wKindID = static_cast<unsigned short>( (dwCID & Creature::MONSTER_KIND_BIT) );
		if ( bResult && m_AdminMonsterUIDMap.end() == m_AdminMonsterUIDMap.find(wKindID) )
		{
			m_AdminMonsterUIDMap.insert(std::make_pair(wKindID, INIT_UID)).second;
		}
	}
	
	return bResult;
}

CCreature*	CVirtualMonsterMgr::GetCreature(unsigned long dwCID)
{
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(dwCID);

	if (Creature::CT_MONSTER == eCreatureType || Creature::CT_SUMMON == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		return (CCreature *)GetMonster(dwCID);

	return (CCreature *)NULL;
}

CAggresiveCreature*	CVirtualMonsterMgr::GetAggresiveCreature(unsigned long dwCID)
{
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(dwCID);

	if (Creature::CT_MONSTER == eCreatureType || Creature::CT_SUMMON == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		return (CAggresiveCreature *)GetMonster(dwCID);

	return (CAggresiveCreature *)NULL;
}

CMonster*	CVirtualMonsterMgr::GetMonster(unsigned long dwCID)
{
	MonsterMap::iterator pos = m_MonsterMap.find(dwCID);
	return (pos != m_MonsterMap.end()) ? pos->second : NULL;
}


void	CVirtualMonsterMgr::ProcessAllMonster()
{
	MonsterMap::iterator pos = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();

	for (; pos != end; )
	{
		CMonster* lpMonster = pos->second;

		if (lpMonster)
		{
			lpMonster->Process();
		}

		++pos;
	}
}

void	CVirtualMonsterMgr::ProcessMonsterRegenHPAndMP()
{
	MonsterMap::iterator pos = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();

	for (; pos != end; )
	{
		CMonster* lpMonster = pos->second;

		if (lpMonster)
		{
			lpMonster->RegenHPAndMP(0, 0, true);
		}

		++pos;
	}
}

void	CVirtualMonsterMgr::ProcessSummonMonsterDead(void)
{
	MonsterMap::iterator pos = m_MonsterMap.begin();
	MonsterMap::iterator end = m_MonsterMap.end();

	for (; pos != end; )
	{
		CMonster* lpMonster = pos->second;
        
		if (lpMonster && true == lpMonster->IsDeadSummonMonster())
		{
			pos = m_MonsterMap.erase(pos);
			delete lpMonster;
		}

		++pos;
	}
}


bool	CVirtualMonsterMgr::IsSummonee(unsigned long dwCID) 
{ 
	return Creature::IsSummonMonster(dwCID);
}


bool	CVirtualMonsterMgr::SummonMonster(int nKID, Position Pos, CCharacter* lpMaster)
{
	CMonster::MonsterCreateInfo tempInfo;

	tempInfo.m_dwCID = Creature::SUMMON_MONSTER_BIT + (m_usSummonCount << 16) + nKID;
	if (0 != GetMonster(tempInfo.m_dwCID))
	{
		ERRLOG0(g_Log, "���� ��ȯ�� �����Ͽ����ϴ�.");
		return false;
	}

	tempInfo.m_nKID = nKID;
	tempInfo.m_Pos = Pos;

	CMonster* lpSummonMonster = 0;
	if (0 != lpMaster)
	{
		CAggresiveCreature* lpSummonee = lpMaster->GetSummonee();
		if (0 != lpSummonee)
		{
			lpSummonee->Dead(0);
		}
	}

	lpSummonMonster = new CSummonMonster(tempInfo, lpMaster);
	if (0 == lpSummonMonster) 
	{
		ERRLOG0(g_Log, "���� ��ȯ�� �����Ͽ����ϴ�.");
		return false;
	}

	lpSummonMonster->SetMapIndex(lpMaster->GetMapIndex());
	if (false == lpSummonMonster->InitMonster(tempInfo.m_Pos))
	{
		ERRLOG0(g_Log, "��ȯ ���� �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	AddMonster(lpSummonMonster);

	m_usSummonCount++;
	if (CMonster::MAX_MONSTER_UID == m_usSummonCount) { m_usSummonCount = 0; }

	if (0 != lpMaster)
	{
		lpMaster->SetSummonee(lpSummonMonster);

		GameClientSendPacket::SendCharSummon(lpMaster->GetCID(), lpSummonMonster);
	}
	else
	{
		lpSummonMonster->SendMove(CMonster::RUN_ANI_LIMIT_MIN);
	}

	return true;
}


bool	CVirtualMonsterMgr::AdminSummonMonster(int nKID, Position Pos, unsigned short wMapIndex)
{
	CMonster::MonsterCreateInfo tempInfo;
	CMonster* lpNewMonster = 0;

	// nKID �� �������� üũ
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (0 == lpProtoType)
	{
		ERRLOG1(g_Log, "KID : %d  ���͸� ��ȯ�ϴµ� �����Ͽ����ϴ�.", nKID);
		return false;
	}

	// ���� �ִ� ���� ���� �ʰ��ϴ��� üũ
	if (GetMonsterNum() >= CMonster::MAX_MONSTER_UID)
	{
		ERRLOG0(g_Log, "���� ���� ���� ���� �ִ��Դϴ�. ���̻� ���͸� ������ �� �����ϴ�.");
		return false;
	}

	// ���� �ʿ��� ������ ���ڸ� �Ѱ��ش�. (CID �� ����ũ�ϰ� �ϱ� ���ؼ�)
	unsigned long dwUID = GetAvailableMonsterUID(static_cast<unsigned short>(nKID));
	if (dwUID == NO_BLANK_UID)
	{
		ERRLOG1(g_Log, "KindID:%d �ش� ������ ���� ���� �ִ��Դϴ�. ���̻� ���͸� ������ �� �����ϴ�.", nKID);
		return false;
	}

	tempInfo.m_dwCID = (dwUID << 16) + nKID;
	tempInfo.m_nKID = nKID;
	tempInfo.m_dwPID = 0;
	tempInfo.m_Pos = Pos;
	tempInfo.m_bScout = false;
	tempInfo.m_nMovingPattern = 0;
	tempInfo.m_wRespawnArea = CCell::CELL_DISTANCE;

	// MON_TODO : ���� Ÿ�Կ� �°� ����
	switch (lpProtoType->m_MonsterInfo.m_cSkillPattern)
	{
	case MonsterInfo::PATTERN_DEFENDER:
		lpNewMonster = new CDefenderMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_WARRIOR:
		lpNewMonster = new CWarriorMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_ACOLYTE:
		lpNewMonster = new CAcolyteMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_MAGE:
		lpNewMonster = new CMageMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_BOSS:
		lpNewMonster = new CBossMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_NAMED:
		lpNewMonster = new CNamedMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_CHIEF:
		lpNewMonster = new CChiefMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_OBJECT:
		lpNewMonster = new CObjectMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_GUARD:
		lpNewMonster = new CGuardMonster(tempInfo, true);
		break;

	case MonsterInfo::PATTERN_GATHER:
		lpNewMonster = new CGatherMonster(tempInfo, true);
		break;

	default:
		lpNewMonster = new CMonster(tempInfo, true);
		break;
	};

	if (0 == lpNewMonster) 
	{
		ERRLOG0(g_Log, "���� ������ �����Ͽ����ϴ�.");
		return false;
	}

	lpNewMonster->SetMapIndex(wMapIndex);
	if (false == lpNewMonster->InitMonster(tempInfo.m_Pos))
	{
		ERRLOG0(g_Log, "���� �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	return AddMonster(lpNewMonster);
}


unsigned short CVirtualMonsterMgr::GetAvailableMonsterUID(unsigned short wKindID)
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
			// ���� ������ ���ٸ�...
			return NO_BLANK_UID;
		}

		m_AdminMonsterUIDMap[wKindID] = INIT_UID - 1;
		return INIT_UID;
	}

	return m_AdminMonsterUIDMap[wKindID]--;
}


