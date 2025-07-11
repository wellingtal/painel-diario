#include "StdAfx.h"
#include "MineralVeinDBMgr.h"

#include <Log/ServerLog.h>
#include <DB/DBComponent.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Castle/CastleDBComponent.h>

CMineralVeinDBMgr&	CMineralVeinDBMgr::GetInstance()
{
	static CMineralVeinDBMgr ms_this;
	return ms_this;
}

CMineralVeinDBMgr::CMineralVeinDBMgr()
: m_lpDBComponent(0)
{
}

CMineralVeinDBMgr::~CMineralVeinDBMgr()
{
	Destroy();
}

bool	CMineralVeinDBMgr::Initialize(CDBComponent& DBComponent)
{
	m_lpDBComponent = &DBComponent;

	if (!ReadFertilityDB())
	{
		ERRLOG0(g_Log, "DB 에서 지력정보를 읽는데 실패했습니다.");
		return false;
	}

	return true;
}

void	CMineralVeinDBMgr::Destroy()
{
	m_mapFertility.clear();
}

bool	CMineralVeinDBMgr::ReadFertilityDB()
{
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT TblFertilityInfo.nVeinColor, "
		"TblFertilityInfo.nFertility, "
		"TblFertilityInfo.tnZone, "
		"TblFertilityInfo.tnChannel "
		"FROM TblFertilityInfo ");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "Fertility DB 얻어오기 실패");
		return false;
	}

	FertilityInfoDB* fertilityInfo = new FertilityInfoDB[MAX_ROWS];
	memset(fertilityInfo, 0, sizeof(FertilityInfoDB) * MAX_ROWS);
	
	while (m_lpDBComponent->GetData((void**)fertilityInfo, sizeof(FertilityInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (FertilityInfoDB* lpFertilityInfo = fertilityInfo; nGetRows > 0; --nGetRows, ++lpFertilityInfo)
		{
			unsigned long dwZoneChannel = static_cast<unsigned long>(lpFertilityInfo->m_cZone << 16 | lpFertilityInfo->m_cChannel);

			FertilityZoneMap::iterator itr = m_mapFertility.find(dwZoneChannel);
			if ( itr == m_mapFertility.end() )
			{
				FertilityInfoMap tempFertilityInfoMap;
				tempFertilityInfoMap.insert(std::make_pair(lpFertilityInfo->m_dwVeinColor, lpFertilityInfo->m_dwFertility)).second;

				m_mapFertility.insert(std::make_pair(dwZoneChannel, tempFertilityInfoMap)).second;
			}
			else
			{
				FertilityInfoMap& tempFertilityInfoMap = itr->second;
				tempFertilityInfoMap.insert(std::make_pair(lpFertilityInfo->m_dwVeinColor, lpFertilityInfo->m_dwFertility)).second;
			}
		}

		memset(fertilityInfo, 0, sizeof(FertilityInfoDB) * MAX_ROWS);
	}

	delete [] fertilityInfo;

	return true;
}

bool	CMineralVeinDBMgr::SendFertilityInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel)
{
	char szBuffer[2048];
	unsigned short	usBufferSize = 0;
	unsigned long	dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);

	PktFertilityInfo* lpPktFertilityInfo = reinterpret_cast<PktFertilityInfo*>(szBuffer);
	
	FertilityZoneMap::iterator pos = m_mapFertility.find(dwZoneChannel);
	if ( pos != m_mapFertility.end() )
	{
		FertilityInfoMap& fertilityMap = pos->second;

		unsigned long* lpValues = reinterpret_cast<unsigned long*>( lpPktFertilityInfo + 1 );

		for (FertilityInfoMap::iterator itr = fertilityMap.begin(); itr != fertilityMap.end(); ++itr)
		{
			*lpValues = itr->first;
			++lpValues;
			*lpValues = itr->second;
			++lpValues;

			usBufferSize += sizeof(unsigned long) * 2;
		}

		lpPktFertilityInfo->m_wNum = static_cast<unsigned short>( fertilityMap.size() );
		lpPktFertilityInfo->m_wSize = usBufferSize;

		return SendStream.WrapCompress(szBuffer, static_cast<unsigned short>(sizeof(PktFertilityInfo) + usBufferSize), CmdFertilityInfo, 0, 0);
	}

	return false;
}

bool	CMineralVeinDBMgr::UpdateFertilityInfo(unsigned char cZone, unsigned char cChannel,
											   unsigned short wNum, unsigned long* lpFertilityInfo)
{
	unsigned long	dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);
	FertilityZoneMap::iterator pos = m_mapFertility.find(dwZoneChannel);
	if ( pos != m_mapFertility.end() )
	{
		FertilityInfoMap& fertilityMap = pos->second;

		unsigned long dwVeinColor, dwFertility;

		for (int i=0; i<wNum; ++i)
		{
			dwVeinColor = *lpFertilityInfo;		++lpFertilityInfo;
			dwFertility = *lpFertilityInfo;		++lpFertilityInfo;

			FertilityInfoMap::iterator itr = fertilityMap.find( dwVeinColor );
			if ( itr != fertilityMap.end() )
			{
				if (itr->second != dwFertility)
				{
					itr->second = dwFertility;
					DBComponent::CastleDB::UpdateFertilityInfo(*m_lpDBComponent, cZone, cChannel, dwVeinColor, dwFertility);
				}
			}
		}
	}

	return true;
}