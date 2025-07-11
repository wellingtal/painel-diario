#include "stdafx.h"
#include "MiningCampDB.h"
#include "SiegeObjectDBMgr.h"

#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>

#include <Log/ServerLog.h>

CMiningCampDB::CMiningCampDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo)
: CCampDB(DBComponent, CampInfo)
{
}

CMiningCampDB::~CMiningCampDB()
{
}

bool	CMiningCampDB::OnOff(unsigned long dwSubState)
{
	if ( Siege::MINING_ON == dwSubState && Siege::MINING_ON != m_cSubState )
	{
		m_cSiegeCount = 0;
		m_cSubState = Siege::MINING_ON;
		DBComponent::CastleDB::UpdateCampSubState(m_DBComponent, GetCampID(), m_cSubState);
		DBComponent::CastleDB::UpdateCampSiegeCount(m_DBComponent, GetCampID(), m_cSiegeCount);
	}
	else if ( Siege::MINING_OFF == dwSubState && Siege::MINING_OFF != m_cSubState )
	{
		m_cSiegeCount = 0;
		m_cSubState = Siege::MINING_OFF;
		DBComponent::CastleDB::UpdateCampSubState(m_DBComponent, GetCampID(), m_cSubState);
		DBComponent::CastleDB::UpdateCampSiegeCount(m_DBComponent, GetCampID(), m_cSiegeCount);

		// 임시 보관된 광물 날리기
		DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::TEMPORARY_MINERAL);
		m_TemporaryMineralMap.clear();
	}
	else
	{
        return false;
	}

	return true;
}

bool	CMiningCampDB::GainMineral(unsigned short wMineralID, unsigned short wAmount)
{
	MineralInfoMap::iterator itr = m_AccumulatedMineralMap.find(wMineralID);

	if (itr != m_AccumulatedMineralMap.end())
	{
		if (itr->second < wAmount)
		{
			ERRLOG4(g_Log, "획득할려는 아이템 수보다 현재 누적된 아이템 수가 더 적습니다.( CampID : 0x%08x, MineralID : %d, CurrentAmount : %d, GainAmount : %d )",
				GetCampID(), wMineralID, itr->second, wAmount);
			return false;
		}
		else
		{
			if (itr->second == wAmount)
			{
				DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::ACCUMULATED_MINERAL, wMineralID);
				m_AccumulatedMineralMap.erase(itr);
			}
			else
			{
				itr->second -= wAmount;
				DBComponent::CastleDB::UpdateMiningCampMineralInfo(m_DBComponent, GetCampID(), Siege::ACCUMULATED_MINERAL, wMineralID, itr->second);
			}

			return true;
		}
	}

	return false;
}

bool	CMiningCampDB::AddMineral(unsigned char cFlag, const CampMineralInfo& campMineralInfo)
{
	if (campMineralInfo.m_wAmount > 0)
	{
		switch (cFlag)
		{
			case Siege::ACCUMULATED_MINERAL :
			{
				m_AccumulatedMineralMap.insert( std::make_pair(campMineralInfo.m_wMineralID, campMineralInfo.m_wAmount) ).second;
			}
			break;

			case Siege::TEMPORARY_MINERAL :
			{
				m_TemporaryMineralMap.insert( std::make_pair(campMineralInfo.m_wMineralID, campMineralInfo.m_wAmount) ).second;
			}
			break;

			default:	return false;
		}
	}

	return true;
}

bool	CMiningCampDB::UpdateMineralInfo(unsigned char cFlag, unsigned char cNum, CampMineralInfo* lpMineralInfo)
{
	switch (cFlag)
	{
		case Siege::TEMPORARY_MINERAL :
		{
			DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), cFlag);
			m_TemporaryMineralMap.clear();
			
			for (int i=0; i<cNum; ++i)
			{
				DBComponent::CastleDB::InsertMiningCampMineralInfo(m_DBComponent, GetCampID(), cFlag,
					lpMineralInfo->m_wMineralID, lpMineralInfo->m_wAmount);

				AddMineral(cFlag, *lpMineralInfo);
				++lpMineralInfo;
			}
		}
		break;

		case Siege::ACCUMULATED_MINERAL :
		{
			DBComponent::CastleDB::DeleteMiningCampMineralInfo(m_DBComponent, GetCampID(), cFlag);
			m_AccumulatedMineralMap.clear();

			for (int i=0; i<cNum; ++i)
			{
				DBComponent::CastleDB::InsertMiningCampMineralInfo(m_DBComponent, GetCampID(), cFlag,
					lpMineralInfo->m_wMineralID, lpMineralInfo->m_wAmount);

				AddMineral(cFlag, *lpMineralInfo);
				++lpMineralInfo;
			}

			m_cSiegeCount = 0;
			DBComponent::CastleDB::UpdateCampSiegeCount(m_DBComponent, GetCampID(), m_cSiegeCount);
		}
		break;

		default:	return false;
	}
	
	return true;
}


bool	CMiningCampDB::SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out)
{
	if (CCampDB::SerializeOut(szBuffer_Out, usBufferSize_Out, dwCID_Out))
	{
		char* lpBuffer = szBuffer_Out + usBufferSize_Out;
		
		if (!lpBuffer)
		{
			return false;
		}

		// 채굴기 정보
		unsigned short wSize = 0;
		MiningCampInfo* lpMiningCampInfo = reinterpret_cast<MiningCampInfo*>(lpBuffer);

		lpMiningCampInfo->m_cAccumulatedNum = static_cast<unsigned char>(m_AccumulatedMineralMap.size());
		lpMiningCampInfo->m_cTemporaryNum = static_cast<unsigned char>(m_TemporaryMineralMap.size());

		wSize += sizeof(MiningCampInfo);

		// 누적 광물 정보
		CampMineralInfo* lpCampMineralInfo = reinterpret_cast<CampMineralInfo*>(lpMiningCampInfo + 1);
		for (MineralInfoMap::iterator itr = m_AccumulatedMineralMap.begin();
			 itr != m_AccumulatedMineralMap.end(); ++itr, ++lpCampMineralInfo)
		{
			lpCampMineralInfo->m_wMineralID = itr->first;
			lpCampMineralInfo->m_wAmount = itr->second;

			wSize += sizeof(CampMineralInfo);
		}

		// 임시 광물 정보
		for (MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
			itr != m_TemporaryMineralMap.end();	++itr, ++lpCampMineralInfo)
		{
			lpCampMineralInfo->m_wMineralID = itr->first;
			lpCampMineralInfo->m_wAmount = itr->second;

			wSize += sizeof(CampMineralInfo);
		}

		lpMiningCampInfo->m_wSize = wSize;
		usBufferSize_Out += wSize;

		return true;
	}

	return false;
}