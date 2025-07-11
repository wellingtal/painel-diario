#include "stdafx.h"
#include "VirtualArea.h"

#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

using namespace VirtualArea;


CVirtualArea::CVirtualArea(const VirtualArea::ProtoType* lpProtoType, unsigned short wMapIndex)
:	m_CellData(NULL), m_dwVID(0), m_wMapIndex(wMapIndex), m_pVirtualMonsterMgr(NULL)
{
	m_CharacterList.clear();
	m_SpectatorList.clear();

	if (lpProtoType)
	{
		m_dwVID = lpProtoType->m_dwVID;
		CreateCell(lpProtoType->m_wWidth, lpProtoType->m_wHeight, wMapIndex);
	}
}


CVirtualArea::~CVirtualArea()
{
	if (NULL != m_CellData)
	{
		delete [] m_CellData;
		m_CellData = NULL;
	}

	m_CharacterList.clear();
	m_SpectatorList.clear();

	if (m_pVirtualMonsterMgr)
	{
		delete m_pVirtualMonsterMgr;
		m_pVirtualMonsterMgr = NULL;
	}
}



bool	CVirtualArea::CreateCell(unsigned short wWidth, unsigned short wHeight, unsigned short wMapIndex)
{
	int nX = 0;
	int nZ = 0;

	m_CellData = new CCell[wWidth * wHeight];
	if (NULL == m_CellData) 
	{
		ERRLOG0(g_Log, "셀을 할당하는데 실패했습니다.");
		return false;
	}

	// Make Cell Link
	for (nZ = 0; nZ < wHeight; ++nZ) 
	{
		for (nX = 0; nX < wWidth; ++nX) 
		{
			// UP, UpperLeft, UpperRight
			if (nZ > 0)	
			{
				m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::UP, &m_CellData[nX + (nZ - 1) * wWidth]);

				if (nX > 0)
				{
					m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::UPPERLEFT, &m_CellData[nX - 1 + (nZ - 1) * wWidth]);
				}

				if (nX < wWidth - 1)
				{
					m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::UPPERRIGHT, &m_CellData[nX + 1 + (nZ - 1) * wWidth]);
				}
			}

			// Down, DnLeft, DnRight
			if (nZ < wWidth - 1)		
			{
				m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::DOWN, &m_CellData[nX + (nZ + 1) * wWidth]);

				if (nX > 0)
				{
					m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::LOWERLEFT, &m_CellData[nX - 1 + (nZ + 1) * wWidth]);
				}

				if (nX < wWidth - 1)
				{
					m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::LOWERRIGHT, &m_CellData[nX + 1 + (nZ + 1) * wWidth]);
				}
			}

			// Left
			if (nX > 0)
			{
				m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::LEFT, &m_CellData[nX - 1 + nZ * wWidth]);
			}

			// Right
			if (nX < wWidth - 1)	
			{
				m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::RIGHT, &m_CellData[nX + 1 + nZ * wWidth]);			
			}

			// itself
			m_CellData[nX + nZ * wWidth].SetConnectCell(CCell::NO, &m_CellData[nX + nZ * wWidth]);
		}
	}

    // 2004 / 06 / 14 : sparrowhawk changed.
    // 링크를 생성한 후 초기화를 해야 한다.
    for (nZ = 0; nZ < wHeight; ++nZ)
    {
        for (nX = 0; nX < wWidth; ++nX) 
        {
            m_CellData[nX + nZ * wWidth].Initialize(nX, nZ);
            m_CellData[nX + nZ * wWidth].SetMapIndex(wMapIndex);
        }
    }

	return true;
}



void	CVirtualArea::CreateVirtualMonsterManager()
{
	if (m_pVirtualMonsterMgr)
	{
		delete m_pVirtualMonsterMgr;
		m_pVirtualMonsterMgr = NULL;
	}

    m_pVirtualMonsterMgr = new CVirtualMonsterMgr();
}


void	CVirtualArea::ProcessAllMonster()
{
	if (m_pVirtualMonsterMgr)
	{
		m_pVirtualMonsterMgr->ProcessAllMonster();
	}
}

void	CVirtualArea::ProcessMonsterRegenHPAndMP()
{
	if (m_pVirtualMonsterMgr)
	{
		m_pVirtualMonsterMgr->ProcessMonsterRegenHPAndMP();
	}
}

void	CVirtualArea::ProcessSummonMonsterDead()
{
	if (m_pVirtualMonsterMgr)
	{
		m_pVirtualMonsterMgr->ProcessSummonMonsterDead();
	}
}

void	CVirtualArea::ProcessDeleteItem()
{
	if (NULL == m_CellData) 
	{ 
		return;
	}

	CCell* lpCellPastEnd = m_CellData + GetWidth() * GetHeight();

	for (CCell* lpCell = m_CellData; lpCell != lpCellPastEnd; ++lpCell)
	{
		lpCell->CheckDeleteItem();
	}
}


bool	CVirtualArea::ProcessAllCellPrepareBroadCast()
{
	if (NULL == m_CellData) 
	{ 
		return false; 
	}

	CCell* lpCellPastEnd = m_CellData + GetWidth() * GetHeight();

	for (CCell* lpCell = m_CellData; lpCell != lpCellPastEnd; ++lpCell)
	{
		lpCell->PrepareBroadCast();
	}

	return true;
}


bool	CVirtualArea::ProcessAllCellBroadCast(unsigned long dwCurrentPulse)
{
	if (NULL == m_CellData) 
	{ 
		return false; 
	}

	CCell* lpCellPastEnd = m_CellData + GetWidth() * GetHeight();

	for (CCell* lpCell = m_CellData; lpCell != lpCellPastEnd; ++lpCell)
	{
		lpCell->BroadCast(dwCurrentPulse);
	}

	return true;
}


unsigned short	CVirtualArea::GetStartX()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 0;
	}

	return pProtoType->m_wStartX;
}


unsigned short	CVirtualArea::GetStartZ()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 0;
	}

	return pProtoType->m_wStartZ;
}


unsigned short	CVirtualArea::GetWidth()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 0;
	}

	return pProtoType->m_wWidth;
}

unsigned short	CVirtualArea::GetHeight()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 0;
	}

	return pProtoType->m_wHeight;
}

unsigned char	CVirtualArea::GetVirtualZone()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 0;
	}

	return pProtoType->m_cZone;
}

Position	CVirtualArea::GetStartPosition(unsigned char cNation)
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return Position(0, 0, 0);
	}

	if (cNation >= CClass::MAX_RACE)
	{
		ERRLOG1(g_Log, "VirtualArea StartPos 얻기 실패!! Nation : %d", cNation);
		return Position(0, 0, 0);
	}

	return pProtoType->m_StartPos[cNation];
}


Position	CVirtualArea::GetRespawnPosition(unsigned char cNation, int nIndex)
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return Position(0, 0, 0);
	}

	if (cNation >= CClass::MAX_RACE || nIndex < 0 || nIndex >= VirtualArea::MAX_VIRTUAL_AREA_RESPAWN_POINT)
	{
		ERRLOG3(g_Log, "VirtualArea RespawnPos 얻기 실패!! Nation : %d, Index : %d, MAX_VIRTUAL_AREA_RESPAWN_POINT : %d",
				cNation, nIndex, VirtualArea::MAX_VIRTUAL_AREA_RESPAWN_POINT);
		return Position(0, 0, 0);
	}

	return pProtoType->m_RespawnPos[cNation][nIndex];
}

unsigned char	CVirtualArea::GetMaxRespawnPos()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return 1;
	}

	return pProtoType->m_cMaxRespawnPos;
}

const char*		CVirtualArea::GetMapTypeName()
{
	const VirtualArea::ProtoType* pProtoType = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualAreaProtoType(m_dwVID);
	if (NULL == pProtoType)
	{
		ERRLOG1(g_Log, "VID : 0x%x VirtualAreaProtoType 을 얻기 실패!!", m_dwVID);
		return "에러야!!";
	}

	return pProtoType->m_szMapType;
}
