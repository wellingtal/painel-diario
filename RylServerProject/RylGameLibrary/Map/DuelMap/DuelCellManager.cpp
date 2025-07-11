
#include "stdafx.h"

#include <map>

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/DelimitedFile.h>

#include <Creature/CreatureManager.h>
#include <Creature/Monster/Monster.h>

#include "DuelCellManager.h"


CDuelCellManager CDuelCellManager::ms_this;

CCell* CDuelCellManager::CreateCell(unsigned long dwCID)
{
	CCell* lpCell = new CCell();
	if(NULL == lpCell)
		return NULL;

	lpCell->Initialize(0, 0);

	m_CellData.insert(std::make_pair(dwCID, lpCell)).second;
	return lpCell;
}

bool CDuelCellManager::DestroyCell(unsigned long dwCID)
{
    DuelCellMap::iterator itr = m_CellData.find(dwCID);
    if(itr == m_CellData.end())
	{
		return false;
	}

	delete itr->second;
    m_CellData.erase(dwCID);
	return true;
}

// -------------------------------------------------------------------------------------
// 로그용 함수들

struct CheckInfo
{	
	unsigned long m_dwCellID;
	CheckInfo(unsigned long CellID) : m_dwCellID(CellID) { }
};

typedef std::map<unsigned long, CheckInfo>			CIDInCell;
typedef std::multimap<unsigned long, CheckInfo>	    DUPCID;

class CheckMap
{
public:
	CheckMap(CIDInCell& Map) : m_Map(Map) { }
	bool operator() (CAggresiveCreature* lpCreature) { m_Map.erase(lpCreature->GetCID()); return true; }		
protected:
	CIDInCell& m_Map;
};


bool CDuelCellManager::CheckCellAggresiveCreatures()
{
	CIDInCell	AggresiveCreatureMap;
	DUPCID		DuplicateCID;

	unsigned long		dwCID = 0;

	for(DuelCellMap::iterator itr = m_CellData.begin(); itr != m_CellData.end(); ++itr)
	{
		CCell* lpCell = itr->second;
		unsigned long CellID = itr->first;

		CCharacter* lpCharacter = lpCell->GetFirstCharacter();
		while(NULL != lpCharacter)
		{
			dwCID = lpCharacter->GetCID();
			if(!AggresiveCreatureMap.insert(CIDInCell::value_type(dwCID, CheckInfo(CellID))).second)
			{
				DuplicateCID.insert(DUPCID::value_type(dwCID, CheckInfo(CellID)));
			}

			lpCharacter = lpCell->GetNextCharacter();
		}
	}

	CheckMap	check(AggresiveCreatureMap);
	CCreatureManager::GetInstance().ProcessAllCharacter(check);
	CCreatureManager::GetInstance().ProcessAllMonster(check);
	
	FILE* fFile = fopen("CellLog.txt", "wt");
	if(NULL == fFile) { return false; }

	if(DuplicateCID.empty())
	{
		fprintf(fFile, "셀 끼리는 중복되는 CID가 없습니다.");
	}
	else
	{
		for(DUPCID::iterator itr = DuplicateCID.begin(); itr != DuplicateCID.end(); ++itr)
		{
			fprintf(fFile, "셀 안에서 중복되는 CID:0x%08x - CellID:%d", itr->first, 
				itr->second.m_dwCellID);
		}
	}

	if(AggresiveCreatureMap.empty())
	{
		fprintf(fFile, "크리쳐 맵에 있는 아이디 뿐입니다.");
	}
	else
	{
		for(CIDInCell::iterator itr = AggresiveCreatureMap.begin(); itr != AggresiveCreatureMap.end(); ++itr)
		{
			fprintf(fFile, "크리쳐 맵에 없는 CID:0x%08x - CellID:%d", itr->first,
				itr->second.m_dwCellID);
		}
	}

	fclose(fFile);
	return true;
}


bool CDuelCellManager::CheckCellStatus(void)
{
	// 셀 상태를 출력하는 부분, 이전 셀에서 주위 셀 뺐음.
	FILE* fFile = fopen("CellStat.txt", "wt");
	if(NULL == fFile) 
	{
		return false;
	}

	fprintf(fFile, "X,Z,인원수,아이템 수\r\n");

	for(DuelCellMap::iterator itr = m_CellData.begin(); itr != m_CellData.end(); ++itr)
	{
		CCell* lpCell = itr->second;
		unsigned long CellID = itr->first;

		fprintf(fFile, "%4d\t%4d\t%4d\r\n",
			CellID, lpCell->GetCharacterNum(), lpCell->GetItemNum());
	}

	fclose(fFile);
	return true;
}