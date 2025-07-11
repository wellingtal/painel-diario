#ifndef _DUELCELL_MANAGER_H_
#define _DUELCELL_MANAGER_H_

#pragma once

#include "../FieldMap/Cell.h"
#include <Pattern/Singleton.h>

class CDuelCellManager : public CSingleton<CDuelCellManager>
{
public:
	
	~CDuelCellManager()	{	}

	CCell* CreateCell(unsigned long dwCID);
	bool DestroyCell(unsigned long dwCID);

	CCell* GetCell(unsigned long dwCellID);	// 셀 번호로 포인트 얻기

	inline void LowerResolution(int nHighX, int nHighZ, int *nLowX, int *nLowZ);
	inline void HigherResolution(int nLowX, int nLowZ, int *nHighX, int *nHighZ);

	template<typename FnRefCell>
		inline bool ProcessAllCell(FnRefCell fnRefCell)
    {
		if (0 == m_CellData.size()) return false;

		for(DuelCellMap::iterator itr = m_CellData.begin(); itr != m_CellData.end(); ++itr)
		{
            fnRefCell(*itr->second);
		}

		return true;
	}

	template<typename FnRefCell, typename Arg1>
		inline bool ProcessAllCell(FnRefCell fnRefCell, Arg1 arg1)
    {
		if (0 == m_CellData.size()) return false;

		for(DuelCellMap::iterator itr = m_CellData.begin(); itr != m_CellData.end(); ++itr)
		{
            fnRefCell(*itr->second, arg1);
		}

		return true;
    }

	// --------------------------------------------------------------------------------------------
	// 셀 통계 내는 함수들

	bool CheckCellAggresiveCreatures(void);		// 셀 내에 겹치는 CID가 있는지 검사.
	bool CheckCellStatus(void);					// 셀 내에 사용자 분포 출력.


private:

	CDuelCellManager()	{	}

	// --------------------------------------------------------------------------------------------
	// member variable
	typedef std::map<unsigned long, CCell*> DuelCellMap;

	DuelCellMap m_CellData;

    static CDuelCellManager ms_this;
};

inline CCell* CDuelCellManager::GetCell(unsigned long dwCellID) 
{ 
	DuelCellMap::iterator it = m_CellData.find(dwCellID);
	return (it != m_CellData.end()) ? it->second : NULL;
}

inline void CDuelCellManager::LowerResolution(int nHighX, int nHighZ, int *nLowX, int *nLowZ) 
{
	*nLowX = nHighX >> CCell::CELL_RESOLUTION;
	*nLowZ = nHighZ >> CCell::CELL_RESOLUTION;
}


inline void CDuelCellManager::HigherResolution(int nLowX, int nLowZ, int *nHighX, int *nHighZ) {
	*nHighX = nLowX << CCell::CELL_RESOLUTION;
	*nHighZ = nLowZ << CCell::CELL_RESOLUTION;
}

#endif+
