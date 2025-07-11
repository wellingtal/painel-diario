#include "stdafx.h"

#include "CastleDB.h"
#include "CastleDBMgr.h"

#include <DB/DBComponent.h>
#include <Log/ServerLog.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/TokenlizedFile.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Creature/Siege/SiegeConstants.h>
#include <Creature/Siege/SiegeObjectDB.h>
#include <Creature/Siege/SiegeObjectDBMgr.h>

#include <Castle/CastleConstants.h>
#include <Castle/CastleDB.h>
#include <Castle/CastleDBComponent.h>

#include <atltime.h>

using namespace Castle;
using namespace Siege;


CCastleDB* CCastleDBMgr::CreateCastleDB(const CastleInfoDB& CastleInfo)
{
    return 0 != m_lpDBComponent ? new CCastleDB(*m_lpDBComponent, CastleInfo) : 0;
}

void CCastleDBMgr::DeleteCastleDB(CCastleDB* lpCastleDB)
{
    delete lpCastleDB;
}


CCastleDBMgr& CCastleDBMgr::GetInstance()
{
    static CCastleDBMgr castleDBMgr;
    return castleDBMgr;
}


CCastleDBMgr::CCastleDBMgr()
:   m_lpDBComponent(0), 
    m_CastlePool(sizeof(CCastleDB))
{
}

CCastleDBMgr::~CCastleDBMgr()
{
	Destroy();
}

bool	CCastleDBMgr::Initialize(CDBComponent& DBComponent)
{
    m_lpDBComponent = &DBComponent;

	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		if (false == ReadCastleInfoDB()) return false;
		if (false == ReadCastleTaxInfoDB()) return false;
		if (false == ReadCastleMineralInfoDB()) return false;
	}

	return true;
}

void	CCastleDBMgr::Destroy()
{
	if (0 == m_CastleMap.size()) return;

	CastleMap::iterator CastleItr = m_CastleMap.begin();
	CastleMap::iterator CastleEnd = m_CastleMap.end();

	while (CastleItr != CastleEnd)
	{
        CCastleDB* lpCastleDB = CastleItr->second;

		if (lpCastleDB)
		{
            DeleteCastleDB(lpCastleDB);
		}

		++CastleItr;
	}

	m_CastleMap.clear();
}


void	CCastleDBMgr::CheckEnableSiegeForAllCastle(bool bIsSiegeTime)
{
	CCastleDB* lpCastle;
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		lpCastle = itr->second;
		if (lpCastle)
		{
			lpCastle->CheckEnableSiege(bIsSiegeTime);
		}

		++itr;
	}
}

void	CCastleDBMgr::UpdateSiegeCount()
{
	CCastleDB* lpCastle;
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		lpCastle = itr->second;
		if (lpCastle)
		{
			lpCastle->UpdateSiegeCount();
		}

		++itr;
	}
}

void	CCastleDBMgr::DestroyAllCastleArms()
{
	CCastleDB* lpCastle;
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		lpCastle = itr->second;
		if (lpCastle)
		{
			lpCastle->DestroyAllCastleArms();
		}

		++itr;
	}
}

void	CCastleDBMgr::CloseCastleGate()
{
	CCastleDB* lpCastle;
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		lpCastle = itr->second;
		
		if (lpCastle)
		{
			lpCastle->CloseCastleGate();
		}

		++itr;
	}
}

void	CCastleDBMgr::OpenCastleGate()
{
	CCastleDB* lpCastle;
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		lpCastle = itr->second;
		if (lpCastle && !lpCastle->GetEnableSiege())
		{
			lpCastle->OpenCastleGate();
		}

		++itr;
	}
}

bool	CCastleDBMgr::ReadCastleInfoDB()
{	
	const int MAX_ROWS = 10;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
				"SELECT "
				"TblCastleInfo.nCastleID, "
				"TblCastleInfo.tnNation, "
				"TblCastleInfo.tnZone, "
				"TblCastleInfo.tnChannel, "
				"TblCastleInfo.tnName, "
				"TblCastleInfo.tnSiegeCount, "
				"TblCastleInfo.tnInvincibleCount, "
				"TblCastleInfo.snTotalSiegeCount, "
				"TblCastleInfo.nTotalTaxMoney, "
				"TblCastleInfo.bRight, "
				"TblCastleInfo.fPosInX, "
				"TblCastleInfo.fPosInY, "
				"TblCastleInfo.fPosInZ, "
				"TblCastleInfo.fPosOutX, "
				"TblCastleInfo.fPosOutY, "
				"TblCastleInfo.fPosOutZ, "
				"TblCastleInfo.snItemID, "
				"TblCastleInfo.tnItemNum "
				"FROM TblCastleInfo");

    szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG2(g_Log, "Castle DB 얻어오기 실패 : %s %s", 
			m_lpDBComponent->GetErrorString(), m_lpDBComponent->GetQueryBuffer());
		return false;
	}

	CastleInfoDB* castleInfo = new CastleInfoDB[MAX_ROWS];
	memset(castleInfo, 0, sizeof(CastleInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)castleInfo, sizeof(CastleInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (CastleInfoDB* lpCastleInfo = castleInfo; nGetRows > 0; --nGetRows, ++lpCastleInfo)
		{
			CreateCastle(*lpCastleInfo);
		}

		memset(castleInfo, 0, sizeof(CastleInfoDB) * MAX_ROWS);
	}

	delete [] castleInfo;
	return true;
}

bool		CCastleDBMgr::ReadCastleTaxInfoDB()
{
	const int MAX_ROWS = 1000;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT "
		"TblCastleTaxInfo.nCastleID, "
		"TblCastleTaxInfo.tnType, "
		"TblCastleTaxInfo.tnTax, "
		"TblCastleTaxInfo.nTempMoney, "
		"TblCastleTaxInfo.nTaxMoney, "
		"TblCastleTaxInfo.tnTaxChangable "
		"FROM TblCastleInfo, TblCastleTaxInfo WHERE "
		"TblCastleInfo.nCastleID = TblCastleTaxInfo.nCastleID");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "Castle Tax DB 얻어오기 실패");
		return false;
	}

	CastleTaxInfoDB* castleTaxInfo = new CastleTaxInfoDB[MAX_ROWS];
	memset(castleTaxInfo, 0, sizeof(CastleTaxInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)castleTaxInfo, sizeof(CastleTaxInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (CastleTaxInfoDB* lpCastleTaxInfo = castleTaxInfo; nGetRows > 0; --nGetRows, ++lpCastleTaxInfo)
		{
			unsigned long dwCastleID = Castle::CASTLE_BIT | lpCastleTaxInfo->m_dwCastleID;
			CCastleDB* lpCastle = GetCastle(dwCastleID);
			if (NULL != lpCastle)
			{
				lpCastle->SetTaxInfo(*lpCastleTaxInfo);
			}
		}

		memset(castleTaxInfo, 0, sizeof(CastleTaxInfoDB) * MAX_ROWS);
	}

	delete [] castleTaxInfo;
	return true;
}

bool		CCastleDBMgr::ReadCastleMineralInfoDB()
{
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT "
		"TblCastleMineralInfo.nCastleID, "
		"TblCastleMineralInfo.snMineralID, "
		"TblCastleMineralInfo.snAmount, "
		"TblCastleMineralInfo.tnFlag "
		"FROM TblCastleInfo, TblCastleMineralInfo WHERE "
		"TblCastleInfo.nCastleID = TblCastleMineralInfo.nCastleID");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "Castle Mineral DB 얻어오기 실패");
		return false;
	}

	CastleMineralInfoDB* mineralInfo = new CastleMineralInfoDB[MAX_ROWS];
	memset(mineralInfo, 0, sizeof(CastleMineralInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)mineralInfo, sizeof(CastleMineralInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (CastleMineralInfoDB* lpMineralInfo = mineralInfo; nGetRows > 0; --nGetRows, ++lpMineralInfo)
		{
			unsigned long dwCastleID = Castle::CASTLE_BIT | lpMineralInfo->m_dwCastleID;
			CCastleDB* lpCastle = GetCastle(dwCastleID);
			if (NULL != lpCastle)
			{
				lpCastle->SetMineralInfo(*lpMineralInfo);
			}
		}

		memset(mineralInfo, 0, sizeof(CastleMineralInfoDB) * MAX_ROWS);
	}

	delete [] mineralInfo;
	return true;
}


CCastleDB*	CCastleDBMgr::CreateCastle(const CastleInfoDB& CastleInfo)
{
	CCastleDB* lpCastle = 0;
    
    if (0 != m_lpDBComponent)
    {
        lpCastle = CreateCastleDB(CastleInfo);

	    if (0 != lpCastle)
	    {
		    if ( !m_CastleMap.insert(std::make_pair(lpCastle->GetCastleID(), lpCastle)).second )
		    {
                DeleteCastleDB(lpCastle);
			    lpCastle = NULL;
		    }
	    }
    }

	return lpCastle;
}



CCastleDB*	CCastleDBMgr::GetCastle(unsigned long dwCastleID)
{
	CastleMap::iterator itr = m_CastleMap.find(dwCastleID);
	if (itr != m_CastleMap.end())
	{
		return itr->second;
	}

	return NULL;
}

void	CCastleDBMgr::GetCastleSiegeInfo(CastleSiegeInfo* lpCastleSiegeInfo, unsigned char& cNum, unsigned short& wSize)
{
	if (NULL == lpCastleSiegeInfo)
	{
		return;
	}

	for (CastleMap::iterator itr = m_CastleMap.begin(); itr != m_CastleMap.end(); ++itr)
	{
		CCastleDB* lpCastle = itr->second;
		if (NULL == lpCastle)
		{
			continue;
		}

		lpCastleSiegeInfo->m_dwCastleID = lpCastle->m_dwCastleID;
		lpCastleSiegeInfo->m_bEnableSiege = lpCastle->m_bEnableSiege;
		++lpCastleSiegeInfo;

		++cNum;
		wSize += sizeof(CastleSiegeInfo);
	}
}


void		CCastleDBMgr::SendCastleInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel)
{
	const unsigned short MAX_CASTLE_SEND_BUFFER = 
        sizeof(PktCastleInfo) + sizeof(CastleInfoDB) + sizeof(CastleTaxInfo) * MAX_TAX_TYPE +
		sizeof(CastleMineralInfo) + sizeof(CastleMineral) * MAX_CASTLE_MINERAL_NUM +
		sizeof(CastleObjectInfo) * MAX_CASTLE_OBJECT_NUM;

	char szBuffer[MAX_CASTLE_SEND_BUFFER];
	unsigned short usBufferSize = 0;
	unsigned char  cCastleObjectNum = 0;		// 성 관련 오브젝트 수
	bool bFirst = true;

	for (CastleMap::iterator itr = m_CastleMap.begin(); itr != m_CastleMap.end(); ++itr)
	{
		CCastleDB* lpCastle = itr->second;
		if (NULL == lpCastle)
		{
			ERRLOG0(g_Log, "NULL 성이 포함되어 있습니다.");
			return;
		}

		if (lpCastle->GetZone() != cZone || lpCastle->GetChannel() != cChannel)
		{
			continue;
		}

		if (SerializeOut(lpCastle, szBuffer + sizeof(PktCastleInfo), usBufferSize, cCastleObjectNum))
        {
		    PktCastleInfo* lpPktCastleInfo = reinterpret_cast<PktCastleInfo*>(szBuffer);
		    
		    lpPktCastleInfo->m_cCastleObjectNum = cCastleObjectNum;
		    lpPktCastleInfo->m_wSize            = usBufferSize;
		    lpPktCastleInfo->m_bStartFlag       = bFirst;

            SendStream.WrapCompress(szBuffer, 
                static_cast<unsigned short>(sizeof(PktCastleInfo) + usBufferSize), CmdCastleInfo, 0, 0);
        }

		bFirst = false;
	}
}


bool		CCastleDBMgr::SerializeOut(CCastleDB* lpCastle_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out, unsigned char& cDefensiveArmsNum_Out)
{
	if (NULL == lpCastle_In || NULL == lpBuffer_Out) return false;

	wBufferSize_Out = 0;
	cDefensiveArmsNum_Out = 0;

	// 성 정보
	CastleInfoDB* lpCastleInfoDB = reinterpret_cast<CastleInfoDB*>(lpBuffer_Out);

	lpCastleInfoDB->m_dwCastleID			= lpCastle_In->m_dwCastleID;
	lpCastleInfoDB->m_cNation				= lpCastle_In->m_cNation;
	lpCastleInfoDB->m_cZone					= lpCastle_In->m_cZone;
	lpCastleInfoDB->m_cChannel				= lpCastle_In->m_cChannel;
	lpCastleInfoDB->m_cNameID				= lpCastle_In->m_cNameID;
	lpCastleInfoDB->m_cSiegeCount			= lpCastle_In->m_cSiegeCount;
	lpCastleInfoDB->m_cInvincibleCount		= lpCastle_In->m_cInvincibleCount;
	lpCastleInfoDB->m_wTotalSiegeCount		= lpCastle_In->m_wTotalSiegeCount;
	lpCastleInfoDB->m_dwTotalTaxMoney		= lpCastle_In->m_dwTotalTaxMoney;
	lpCastleInfoDB->m_InPos.fPointX			= lpCastle_In->m_BackDoorPos[0].m_fPointX;
	lpCastleInfoDB->m_InPos.fPointY			= lpCastle_In->m_BackDoorPos[0].m_fPointY;
	lpCastleInfoDB->m_InPos.fPointZ			= lpCastle_In->m_BackDoorPos[0].m_fPointZ;
	lpCastleInfoDB->m_OutPos.fPointX		= lpCastle_In->m_BackDoorPos[1].m_fPointX;
	lpCastleInfoDB->m_OutPos.fPointY		= lpCastle_In->m_BackDoorPos[1].m_fPointY;
	lpCastleInfoDB->m_OutPos.fPointZ		= lpCastle_In->m_BackDoorPos[1].m_fPointZ;
	lpCastleInfoDB->m_wItemID				= lpCastle_In->m_wItemID;
	lpCastleInfoDB->m_cItemNum				= lpCastle_In->m_cItemNum;
	::memcpy(&lpCastleInfoDB->m_szRight, &lpCastle_In->m_CastleRight, sizeof(CastleRight));

	wBufferSize_Out += sizeof(CastleInfoDB);

	// 성 세율 및 세금 정보
	CastleTaxInfo* lpCastleTaxInfo = reinterpret_cast<CastleTaxInfo*>( lpCastleInfoDB + 1 );
	for (int i=0; i<MAX_TAX_TYPE; ++i)
	{
        *lpCastleTaxInfo = lpCastle_In->m_CastleTax[ i ];
		++lpCastleTaxInfo;
	}

	wBufferSize_Out += sizeof(CastleTaxInfo) * MAX_TAX_TYPE;

	// 성 광물 세금 정보
	CastleMineralInfo* lpCastleMineralInfo = reinterpret_cast<CastleMineralInfo*>( lpCastleTaxInfo );
	CastleMineral* lpCastleMineral = reinterpret_cast<CastleMineral*>( lpCastleMineralInfo + 1 );

	lpCastleMineralInfo->m_cAccumulatedNum	= static_cast<unsigned char>( lpCastle_In->m_AccumulatedMineralMap.size() );
	lpCastleMineralInfo->m_cTemporaryNum	= static_cast<unsigned char>( lpCastle_In->m_TemporaryMineralMap.size() );
	unsigned short wSize = sizeof(CastleMineralInfo);

	CCastleDB::MineralInfoMap::iterator MineralItr = lpCastle_In->m_AccumulatedMineralMap.begin();
	CCastleDB::MineralInfoMap::iterator MineralEnd = lpCastle_In->m_AccumulatedMineralMap.end();
	while (MineralItr != MineralEnd)
	{
		lpCastleMineral->m_wMineralID	= MineralItr->first;
		lpCastleMineral->m_wAmount		= MineralItr->second;

		wSize += sizeof(CastleMineral);
		++lpCastleMineral;
		++MineralItr;
	}

	MineralItr = lpCastle_In->m_TemporaryMineralMap.begin();
	MineralEnd = lpCastle_In->m_TemporaryMineralMap.end();
	while (MineralItr != MineralEnd)
	{
		lpCastleMineral->m_wMineralID	= MineralItr->first;
		lpCastleMineral->m_wAmount		= MineralItr->second;

		wSize += sizeof(CastleMineral);
		++lpCastleMineral;
		++MineralItr;
	}

	lpCastleMineralInfo->m_wSize = wSize;
	wBufferSize_Out += wSize;

	// 성 관련 오브젝트 정보
	unsigned short wBufferSize;
	CastleObjectInfo* lpCastleObjectInfo = reinterpret_cast<CastleObjectInfo*>( lpCastleMineral );
	CCastleDB::CastleObjectMap::iterator ObjectItr = lpCastle_In->m_CastleObjectMap.begin();
	CCastleDB::CastleObjectMap::iterator ObjectEnd = lpCastle_In->m_CastleObjectMap.end();

	while (ObjectItr != ObjectEnd)
	{
		CSiegeObjectDB* lpCastleObject = ObjectItr->second;
		if (lpCastleObject)
		{
			if (true == CSiegeObjectDBMgr::GetInstance().SerializeOutCastleObject(lpCastleObject,
										reinterpret_cast<char*>(lpCastleObjectInfo), wBufferSize))
			{
				++lpCastleObjectInfo;
				++cDefensiveArmsNum_Out;
				wBufferSize_Out += wBufferSize;
			}
		}

		++ObjectItr;
	}

	return true;
}
