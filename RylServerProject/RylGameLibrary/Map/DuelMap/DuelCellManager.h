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

	CCell* GetCell(unsigned long dwCellID);	// �� ��ȣ�� ����Ʈ ���

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
	// �� ��� ���� �Լ���

	bool CheckCellAggresiveCreatures(void);		// �� ���� ��ġ�� CID�� �ִ��� �˻�.
	bool CheckCellStatus(void);					// �� ���� ����� ���� ���.


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
