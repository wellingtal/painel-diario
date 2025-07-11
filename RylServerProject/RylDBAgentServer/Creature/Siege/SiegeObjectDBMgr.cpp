
#include "stdafx.h"

#include <DB/DBComponent.h>
#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Castle/CastleDBComponent.h>
#include <Castle/CastleDBMgr.h>
#include <Castle/CastleDB.h>

#include <GameTime/GameTimeDBMgr.h>

#include <Community/Guild/GuildDBMgr.h>

#include "SiegeObjectDBMgr.h"
#include "SiegeObjectDB.h"
#include "CampDB.h"
#include "MiningCampDB.h"
#include "CampShopDB.h"
#include "WorldWeaponDB.h"
#include "SiegeArmsDB.h"
#include "CastleEmblemDB.h"
#include "CastleGateDB.h"
#include "CastleArmsDB.h"


CSiegeObjectDB*	CSiegeObjectDBMgr::CreateCastleObjectDB(const CastleObjectInfoDB& CastleObject)
{
	switch (CastleObject.m_wObjectType)
	{
	case Siege::EMBLEM :
		return (NULL != m_lpDBComponent) ? new CCastleEmblemDB(*m_lpDBComponent, CastleObject) : NULL;

	case Siege::GATE :
		return (NULL != m_lpDBComponent) ? new CCastleGateDB(*m_lpDBComponent, CastleObject) : NULL;
	
	case Siege::CASTLE_ARMS_NPC :
	case Siege::SHORT_RANGE_CASTLE_ARMS :
	case Siege::LONG_RANGE_CASTLE_ARMS :
	case Siege::GUARD :
		return (NULL != m_lpDBComponent) ? new CCastleArmsDB(*m_lpDBComponent, CastleObject) : NULL;

	default:
		return (NULL != m_lpDBComponent) ? new CSiegeObjectDB(*m_lpDBComponent, CastleObject) : NULL;
	}

	return NULL;
}

CSiegeObjectDB* CSiegeObjectDBMgr::CreateCampDB(const CampInfoDB& CampInfo)
{
    return (NULL != m_lpDBComponent) ? new CCampDB(*m_lpDBComponent, CampInfo) : NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateMiningCampDB(const CampInfoDB& CampInfo)
{
	return (NULL != m_lpDBComponent) ? new CMiningCampDB(*m_lpDBComponent, CampInfo) : NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateCampShopDB(const CampInfoDB& CampInfo)
{
	return (NULL != m_lpDBComponent) ? new CCampShopDB(*m_lpDBComponent, CampInfo) : NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateWorldWeaponDB(const CampInfoDB& CampInfo)
{
	return (NULL != m_lpDBComponent) ? new CWorldWeaponDB(*m_lpDBComponent, CampInfo) : NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateCampDB(unsigned long dwCampID, unsigned long dwGID, 
													unsigned char cZone, unsigned char cChannel, Position Pos)
{
    return (NULL != m_lpDBComponent) ? new CCampDB(*m_lpDBComponent, dwCampID, dwGID, cZone, cChannel, Pos) : NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateWorldWeaponDB(unsigned long dwCampID, unsigned long dwGID, unsigned short wObjectType,
														   unsigned char cZone, unsigned char cChannel, Position Pos)
{
	return (NULL != m_lpDBComponent) ? new CWorldWeaponDB(*m_lpDBComponent, dwCampID, dwGID, wObjectType, cZone, cChannel, Pos) : NULL;
}

CSiegeObjectDB* CSiegeObjectDBMgr::CreateSiegeArmsDB(unsigned long dwOwnerID, unsigned char cNation, 
                                                       unsigned short wObjectType, unsigned char cUpgradeStep, 
                                                       unsigned char cZone, unsigned char cChannel, Position Pos)
{
    return (NULL != m_lpDBComponent) ? 
        new CSiegeArmsDB(*m_lpDBComponent, dwOwnerID, cNation, wObjectType, cUpgradeStep, cZone, cChannel, Pos) : NULL;
}

void CSiegeObjectDBMgr::DeleteSiegeObjectDB(CSiegeObjectDB* lpSiegeObjectDB)
{
	if ( lpSiegeObjectDB )
	{
		delete lpSiegeObjectDB;
		lpSiegeObjectDB = NULL;
	}
}



CSiegeObjectDBMgr& CSiegeObjectDBMgr::GetInstance()
{
    static CSiegeObjectDBMgr siegeObjectDBMgr;
    return siegeObjectDBMgr;
}


CSiegeObjectDBMgr::CSiegeObjectDBMgr()
:   m_lpDBComponent(0)
{

}

CSiegeObjectDBMgr::~CSiegeObjectDBMgr()
{
	if (m_SiegeObjectMap.size() > 0)
	{
		SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
		SiegeObjectMap::iterator end = m_SiegeObjectMap.end();
		CSiegeObjectDB* lpSiegeObject = NULL;

		while (itr != end)
		{
			lpSiegeObject = itr->second;
			if (lpSiegeObject)
			{
                DeleteSiegeObjectDB(lpSiegeObject);
				lpSiegeObject = NULL;
			}

			++itr;
		}

		m_SiegeObjectMap.clear();
		m_CampMap.clear();
	}
}

bool	CSiegeObjectDBMgr::Initialize(CDBComponent& DBComponent)
{
    m_lpDBComponent = &DBComponent;

	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP))
	{
		return true;
	}
	
	// 주의!! 성 정보를 먼저 읽어야 한다.
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		if (false == ReadCastleObjectInfoDB())	{ return false;	}
	}

	// 주의!! 항상 월드 웨폰 정보를 먼저 읽어야한다.
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::CAMP_UPGRADE))
	{
		if (false == ReadWorldWeaponInfoDB())	{ return false; } 
	}
	
	if (false == ReadCampInfoDB())			{ return false; }

	return true;
}

void	CSiegeObjectDBMgr::Process()
{
	if (m_SiegeObjectMap.empty())
	{
		return;
	}

	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	CSiegeObjectDB* lpSiegeObject = NULL;
	std::list<unsigned long>				DeleteObjectList;
	std::map<unsigned long, unsigned short>	ChangeObjectMap;

	Siege::eReturn ret;
	while (itr != m_SiegeObjectMap.end())
	{
		lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			ret = lpSiegeObject->Process();

			switch (ret)
			{
				case Siege::RET_DESTROY_CAMP:			
				case Siege::RET_DESTROY_SIEGE_ARMS:
				{
					DeleteObjectList.push_back(lpSiegeObject->GetCID());
				}
				break;

				case Siege::RET_CHANGE_TYPE:
				{
					CCampDB* lpCamp = reinterpret_cast<CCampDB*>(lpSiegeObject);
					if ( lpCamp )
					{
						ChangeObjectMap.insert( std::make_pair(lpCamp->GetCampID(), lpCamp->GetChangeType()) );
					}
				}
				break;
			}
		}

		++itr;
	}

	// 삭제할 오브젝트 삭제
	std::list<unsigned long>::iterator DeleteItr = DeleteObjectList.begin();
	while (DeleteItr != DeleteObjectList.end())
	{
		DeleteSiegeObject((*DeleteItr));
		++DeleteItr;
	}

	// 변형할 오브젝트 변형
	std::map<unsigned long, unsigned short>::iterator ChangeItr = ChangeObjectMap.begin();
	while (ChangeItr != ChangeObjectMap.end())
	{
		ChangeCampType(ChangeItr->first, ChangeItr->second);
		++ChangeItr;
	}
}

// 공성시간이 끝난후 명성을 보상한다.
void	CSiegeObjectDBMgr::UpdateRewardFame()
{
	if (m_CampMap.empty())
	{
		return;
	}

	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();
	CCampDB* lpCamp = NULL;

	while (itr != end)
	{
		lpCamp = itr->second;
		if (NULL != lpCamp)
		{
			switch (lpCamp->GetObjectType())
			{
			case Siege::CAMP:
				lpCamp->UpdateRewardFame(100);
				break;
			case Siege::MINING_CAMP :
				lpCamp->UpdateRewardFame(200);
				break;
			case Siege::CAMP_SHOP :
				lpCamp->UpdateRewardFame(250);
				break;
			}
		}

		++itr;
	}
}


// 공성 시간 지난 횟수 증가 및 업데이트
void	CSiegeObjectDBMgr::UpdateSiegeCount()
{
	if (m_CampMap.empty())
	{
		return;
	}

	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();
	CCampDB* lpCamp = NULL;
	unsigned char cSiegeCount;

	while (itr != end)
	{
		lpCamp = itr->second;
		if (NULL != lpCamp)
		{
			switch (lpCamp->GetObjectType())
			{
				case Siege::MINING_CAMP :
				{
					if (Siege::MINING_ON == lpCamp->GetSubState())
					{
						cSiegeCount = lpCamp->GetSiegeCount();
						++cSiegeCount;

						// 게임서버에서 채굴 처리를 하므로, 다시 리셋 처리를 해줘야한다.!!
						if (Siege::MINING_CAMP_GAIN_COUNT <= cSiegeCount)
							cSiegeCount = 0;

						lpCamp->SetSiegeCount(cSiegeCount);
						DBComponent::CastleDB::UpdateCampSiegeCount(*m_lpDBComponent, lpCamp->GetCampID(), cSiegeCount);
					}
				}
				break;

				case Siege::CAMP_SHOP :
				{
					cSiegeCount = lpCamp->GetSiegeCount();
					++cSiegeCount;

					lpCamp->SetSiegeCount(cSiegeCount);
					DBComponent::CastleDB::UpdateCampSiegeCount(*m_lpDBComponent, lpCamp->GetCampID(), cSiegeCount);
				}
				break;
			}
		}

		++itr;
	}
}

// 월드 웨폰 파괴정보의 공성 시간 지난 횟수 증가
void	CSiegeObjectDBMgr::DecreaseWeaponRemainSiegeTime()
{
	WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.begin();
	WorldWeaponInfoMap::iterator end = m_WorldWeaponInfoMap.end();

	while (itr != end)
	{
		bool bUpdate = false;
		WorldWeaponInfo& WeaponInfo = itr->second;

		// 남은 공성 시간 감소
		if (WeaponInfo.m_cKarRemainSiegeTime > 0)
		{
			--WeaponInfo.m_cKarRemainSiegeTime;
			bUpdate = true;
		}
		if (WeaponInfo.m_cMerRemainSiegeTime > 0)
		{
			--WeaponInfo.m_cMerRemainSiegeTime;
			bUpdate = true;
		}

		if (bUpdate)
		{
			unsigned char cZone = static_cast<unsigned char>( (itr->first & 0xFFFF0000) >> 16 );
			unsigned char cChannel = static_cast<unsigned char>( (itr->first & 0x0000FFFF) );

			if (0 == WeaponInfo.m_cKarRemainSiegeTime && 0 == WeaponInfo.m_cMerRemainSiegeTime)
			{
				DBComponent::CastleDB::DeleteWorldWeaponInfo(*m_lpDBComponent, cZone, cChannel);
			}
			else
			{
				DBComponent::CastleDB::UpdateWorldWeaponInfo(*m_lpDBComponent, cZone, cChannel, WeaponInfo.m_cKarRemainSiegeTime, WeaponInfo.m_cMerRemainSiegeTime);
			}
		}
		
		++itr;
	}
}

CSiegeObjectDB*	CSiegeObjectDBMgr::CreateCastleObject(CastleObjectInfoDB& CastleObject)
{
    if (NULL != m_lpDBComponent)
    {
	    CSiegeObjectDB* lpCastleObject = CreateCastleObjectDB(CastleObject);

	    if (lpCastleObject)
	    {
		    if ( !m_SiegeObjectMap.insert(std::make_pair(lpCastleObject->GetCID(), lpCastleObject)).second )
		    {
			    DeleteSiegeObject(lpCastleObject->GetCID());
			    return NULL;
		    }

		    return lpCastleObject;
	    }
    }

	return NULL;
}


CSiegeObjectDB*	CSiegeObjectDBMgr::CreateCamp(CampInfoDB& CampInfo, unsigned long dwCID)
{
    if (NULL != m_lpDBComponent)
    {
	    CSiegeObjectDB* lpCamp = CreateCampDB(CampInfo);

	    if (lpCamp)
	    {
			// CID 셋팅
			if (dwCID)
			{
				lpCamp->SetCID(dwCID);
			}
			else
			{
				lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
			}
		    
			// 리스트에 추가
		    if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		    {
			    DeleteSiegeObject(lpCamp);
			    return NULL;
		    }

		    if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
		    {
			    DeleteSiegeObject(lpCamp);
			    return NULL;
		    }

		    return lpCamp;
	    }
    }

	return NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateMiningCamp(CampInfoDB& CampInfo, unsigned long dwCID)
{
	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP_UPGRADE))
	{
		return NULL;
	}

	if (NULL != m_lpDBComponent)
	{
		CSiegeObjectDB* lpCamp = CreateMiningCampDB(CampInfo);

		if ( lpCamp )
		{
			const int MAX_ROWS = 10240;
			int nGetRows = 0;

			char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
			_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
				"SELECT TblCampMineralInfo.nCampID, "
				"TblCampMineralInfo.snMineralID, "
				"TblCampMineralInfo.snAmount, "
				"TblCampMineralInfo.tnFlag "
				"FROM TblGuildInfo, TblCampInfo, TblCampMineralInfo "
				"WHERE TblGuildInfo.nGuildID = TblCampInfo.nGuildID "
				"AND TblCampInfo.nCampID = TblCampMineralInfo.nCampID "
				"AND TblCampMineralInfo.nCampID = %d "
				"AND TblGuildInfo.OldServerGroupID = %d",
				CampInfo.m_dwCampID, CServerSetup::GetInstance().GetAgentServerType());

			szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

			if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
			{
				ERRLOG0(g_Log, "CampMineral DB 얻어오기 실패");
				return false;
			}

			CMiningCampDB* lpMiningCamp = reinterpret_cast<CMiningCampDB*>(lpCamp);
			CampMineralInfo tempMineralInfo;
			CampMineralInfoDB* campMineralInfo = new CampMineralInfoDB[MAX_ROWS];
			memset(campMineralInfo, 0, sizeof(CampMineralInfoDB) * MAX_ROWS);
			
			while (m_lpDBComponent->GetData((void**)campMineralInfo, sizeof(CampMineralInfoDB), MAX_ROWS, &nGetRows))
			{   
				if (0 == nGetRows) { break; }

				for (CampMineralInfoDB* lpCampMineralInfo = campMineralInfo; nGetRows > 0; --nGetRows, ++lpCampMineralInfo)
				{
					tempMineralInfo.m_wMineralID = lpCampMineralInfo->m_wMineralID;
					tempMineralInfo.m_wAmount = lpCampMineralInfo->m_wAmount;
					
					lpMiningCamp->AddMineral( lpCampMineralInfo->m_cFlag, tempMineralInfo );
				}

				memset(campMineralInfo, 0, sizeof(CampMineralInfoDB) * MAX_ROWS);
			}

			delete [] campMineralInfo;

			// CID 셋팅
			if (dwCID)
			{
				lpCamp->SetCID(dwCID);
			}
			else
			{
				lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
			}

			// 리스트에 추가
			if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
			{
				DeleteSiegeObject(lpCamp);
				return NULL;
			}

			if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
			{
				DeleteSiegeObject(lpCamp);
				return NULL;
			}

			return lpCamp;
		}
	}

	return NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateCampShop(CampInfoDB& CampInfo, unsigned long dwCID)
{
	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP_UPGRADE))
	{
		return NULL;
	}

	if (NULL != m_lpDBComponent)
	{
		CSiegeObjectDB* lpCamp = CreateCampShopDB(CampInfo);

		if ( lpCamp )
		{
			const int MAX_ROWS = 256;
			int nGetRows = 0;

			char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
			_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
                "SELECT TblCampShopInfo.CampID, "
				"TblCampShopInfo.Item, "
				"TblCampShopInfo.TempMoney, "
				"TblCampShopInfo.Tax "
				"FROM TblGuildInfo, TblCampInfo, TblCampShopInfo "
				"WHERE TblGuildInfo.nGuildID = TblCampInfo.nGuildID "
				"AND TblCampInfo.nCampID = TblCampShopInfo.CampID "
				"AND TblCampShopInfo.CampID = %d "
				"AND TblGuildInfo.OldServerGroupID = %d", 
				CampInfo.m_dwCampID, CServerSetup::GetInstance().GetAgentServerType());

			szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

			if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
			{
				ERRLOG0(g_Log, "CampShop DB 얻어오기 실패");
				return false;
			}

			CCampShopDB* lpCampShop = reinterpret_cast<CCampShopDB*>(lpCamp);
			CampShopInfoDB* campShopInfo = new CampShopInfoDB[MAX_ROWS];
			memset(campShopInfo, 0, sizeof(CampShopInfoDB) * MAX_ROWS);
			
			while (m_lpDBComponent->GetData((void**)campShopInfo, sizeof(CampShopInfoDB), MAX_ROWS, &nGetRows))
			{   
				if (0 == nGetRows) { break; }

				for (CampShopInfoDB* lpCampShopInfo = campShopInfo; nGetRows > 0; --nGetRows, ++lpCampShopInfo)
				{
					if (0 != lpCampShopInfo->m_dwTotalSize)
					{
						// 전체 버퍼 크기(값) = 전체 버퍼 크기(저장소) + 아이템 갯수(저장소) + 아이템 버퍼 + 가격 버퍼
						unsigned long dwBufferSize = 
							lpCampShopInfo->m_dwTotalSize - sizeof(unsigned long) - sizeof(unsigned char) - 
							sizeof(unsigned long) * lpCampShopInfo->m_cItemNum;

						lpCampShop->SetItemContainer(
							reinterpret_cast<const char*>(lpCampShopInfo->m_aryItemContainer), dwBufferSize, lpCampShopInfo->m_cItemNum);
					}

					lpCampShop->SetTempSafe(lpCampShopInfo->m_dwTempSafe);
					lpCampShop->SetTax(lpCampShopInfo->m_cTax);
				}

				memset(campShopInfo, 0, sizeof(CampShopInfoDB) * MAX_ROWS);
			}

			delete [] campShopInfo;

			// CID 셋팅
			if (dwCID)
			{
				lpCamp->SetCID(dwCID);
			}
			else
			{
				lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
			}

			// 리스트에 추가
			if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
			{
				DeleteSiegeObject(lpCamp);
				return NULL;
			}

			if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
			{
				DeleteSiegeObject(lpCamp);
				return NULL;
			}

			return lpCamp;
		}
	}

	return NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateWorldWeapon(CampInfoDB& CampInfo, unsigned long dwCID)
{
	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP_UPGRADE))
	{
		return NULL;
	}

	if (NULL != m_lpDBComponent)
	{
		CSiegeObjectDB* lpCamp = NULL;
		unsigned char cNation = Creature::STATELESS;
		switch (CampInfo.m_wObjectType)
		{
			case Siege::KARTERANT_WEAPON:	cNation = Creature::KARTERANT;		break;
			case Siege::MERKADIA_WEAPON:	cNation = Creature::MERKADIA;		break;
		}

		// -------------------------------------------------------------------- //
		// 두개의 월드 웨폰이 생성되었는지 체크 및 월드 웨폰 소유 국가 정보 설정
		bool bEnableCreateWorldWeapon = true;
		unsigned long dwZoneChannel = static_cast<unsigned long>(CampInfo.m_cZone << 16 | CampInfo.m_cChannel);
		
		WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
		if (itr != m_WorldWeaponInfoMap.end())
		{
			WorldWeaponInfo& WeaponInfo = itr->second;

			if (Creature::STATELESS != WeaponInfo.m_cWorldWeaponNation)
			{
				bEnableCreateWorldWeapon = false;
			}
			else
			{
				switch (cNation)
				{
					case Creature::KARTERANT:
					{
						if (WeaponInfo.m_cKarRemainSiegeTime > 0)
						{
							bEnableCreateWorldWeapon = false;
						}
					}
					break;

					case Creature::MERKADIA:
					{
						if (WeaponInfo.m_cMerRemainSiegeTime > 0)
						{
							bEnableCreateWorldWeapon = false;
						}
					}
					break;
				}
			}

			if (bEnableCreateWorldWeapon)
			{
				WeaponInfo.m_cWorldWeaponNation = cNation;
			}
		}
		else
		{
			WorldWeaponInfo tempInfo;
			tempInfo.m_cWorldWeaponNation = cNation;
			tempInfo.m_cKarRemainSiegeTime = 0;
			tempInfo.m_cMerRemainSiegeTime = 0;

			m_WorldWeaponInfoMap.insert(std::make_pair(dwZoneChannel, tempInfo)).second;
		}
		// -------------------------------------------------------------------- //

		if (bEnableCreateWorldWeapon)
		{
			lpCamp = CreateWorldWeaponDB(CampInfo);

			if ( lpCamp )
			{
				// CID 셋팅
				if (dwCID)
				{
					lpCamp->SetCID(dwCID);
				}
				else
				{
					lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
				}

				// 리스트에 추가
				if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
				{
					DeleteSiegeObject(lpCamp);
					return NULL;
				}

				if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
				{
					DeleteSiegeObject(lpCamp);
					return NULL;
				}

				return lpCamp;
			}
		}
		else
		{
			ERRLOG0(g_Log, "이미 생성된 월드 웨폰이 2개 이상입니다.");
		}
	}

	return NULL;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateCamp(unsigned long dwGID, unsigned char cZone, unsigned char cChannel, POS Pos)
{
	unsigned long dwCampID = 0;
	unsigned short wError = 0;

	if (0 == m_lpDBComponent ||
		false == DBComponent::CastleDB::InsertCampInfo(*m_lpDBComponent, dwGID, Siege::CAMP, cZone, cChannel, Pos, &dwCampID, &wError))
	{
		SERLOG0(g_Log, "길드 요새 객체를 DB 에서 생성하는데 실패하였습니다.");
		return NULL;
	}

	if (0 == dwCampID)
	{
		return NULL;
	}

	dwCampID = (Castle::CAMP_BIT | dwCampID);

	CSiegeObjectDB* lpCamp = CreateCampDB(dwCampID, dwGID, cZone, cChannel, Pos);

	if (lpCamp)
	{
		lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			ERRLOG1(g_Log, "길드 요새 추가 실패 : CampID : 0x%10u", lpCamp->GetCampID());
			return NULL;
		}

		if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			ERRLOG1(g_Log, "길드 요새 추가 실패 : CampID : 0x%10u", lpCamp->GetCampID());
			return NULL;
		}
	}

	return lpCamp;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateWorldWeapon(unsigned long dwGID, unsigned short wObjectType, unsigned char cZone,
														 unsigned char cChannel, POS Pos)
{
	unsigned long dwCampID = 0;
	unsigned short wError = 0;

	if (0 == m_lpDBComponent ||
		false == DBComponent::CastleDB::InsertCampInfo(*m_lpDBComponent, dwGID, wObjectType, cZone, cChannel, Pos, &dwCampID, &wError))
	{
		SERLOG0(g_Log, "월드 웨폰 객체를 DB 에서 생성하는데 실패하였습니다.");
		return NULL;
	}

	if (0 == dwCampID)
	{
		return NULL;
	}

	dwCampID = (Castle::CAMP_BIT | dwCampID);

	CSiegeObjectDB* lpCamp = CreateWorldWeaponDB(dwCampID, dwGID, wObjectType, cZone, cChannel, Pos);

	if (lpCamp)
	{
		lpCamp->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			ERRLOG1(g_Log, "캠프 추가 실패 : CampID : 0x%08x", lpCamp->GetCampID());
			return NULL;
		}

		if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCampDB*>(lpCamp))).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			ERRLOG1(g_Log, "캠프 추가 실패 : CampID : 0x%08x", lpCamp->GetCampID());
			return NULL;
		}

		unsigned char cNation = Creature::STATELESS;
		switch (wObjectType)
		{
			case Siege::KARTERANT_WEAPON:	cNation = Creature::KARTERANT;		break;
			case Siege::MERKADIA_WEAPON:	cNation = Creature::MERKADIA;		break;
		}

		// -------------------------------------------------------------------- //
		// 두개의 월드 웨폰이 생성되었는지 체크 및 월드 웨폰 소유 국가 정보 설정
		bool bEnableCreateWorldWeapon = true;
		unsigned long dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);

		WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
		if (itr != m_WorldWeaponInfoMap.end())
		{
			WorldWeaponInfo& WeaponInfo = itr->second;
			if (Creature::STATELESS != WeaponInfo.m_cWorldWeaponNation)
			{
				bEnableCreateWorldWeapon = false;
			}
			else
			{
				switch (cNation)
				{
					case Creature::KARTERANT:
					{
						if (WeaponInfo.m_cKarRemainSiegeTime > 0)
						{
							bEnableCreateWorldWeapon = false;
						}
					}
					break;

					case Creature::MERKADIA:
					{
						if (WeaponInfo.m_cMerRemainSiegeTime > 0)
						{
							bEnableCreateWorldWeapon = false;
						}
					}
					break;
				}
			}

			if (bEnableCreateWorldWeapon)
			{
				WeaponInfo.m_cWorldWeaponNation = cNation;
			}
		}
		else
		{
			WorldWeaponInfo tempInfo;
			tempInfo.m_cWorldWeaponNation = cNation;
			tempInfo.m_cKarRemainSiegeTime = 0;
			tempInfo.m_cMerRemainSiegeTime = 0;

			m_WorldWeaponInfoMap.insert(std::make_pair(dwZoneChannel, tempInfo)).second;
		}
		// -------------------------------------------------------------------- //

		if (false == bEnableCreateWorldWeapon)
		{
			ERRLOG0(g_Log, "이미 생성된 월드 웨폰이 존재합니다.");
		}
	}

	return lpCamp;
}

CSiegeObjectDB*		CSiegeObjectDBMgr::CreateSiegeArms(unsigned long dwOwnerID, unsigned char cNation, unsigned char cZone, unsigned char cChannel,
														 unsigned short wObjectType, unsigned char cUpgradeStep, POS Pos)
{
    CSiegeObjectDB* lpSiegeArms = 0;

    if (NULL != m_lpDBComponent)
    {
	    lpSiegeArms = CreateSiegeArmsDB(dwOwnerID, cNation, wObjectType, cUpgradeStep, cZone, cChannel, Pos);

	    if (lpSiegeArms)
	    {
		    lpSiegeArms->SetCID(Creature::SIEGE_OBJECT_BIT | GetBlankIndex());
		    if ( !m_SiegeObjectMap.insert(std::make_pair(lpSiegeArms->GetCID(), lpSiegeArms)).second )
		    {
			    DeleteSiegeObject(lpSiegeArms->GetCID());
			    return NULL;
		    }
	    }
    }

	return lpSiegeArms;
}

CCampDB*	CSiegeObjectDBMgr::GetCamp(unsigned long dwCampID)
{
	CampMap::iterator itr = m_CampMap.find(dwCampID);
	if (itr != m_CampMap.end())
	{
		return itr->second;
	}

	return NULL;
}

CSiegeObjectDB*	CSiegeObjectDBMgr::GetSiegeObject(unsigned long dwObjectID)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.find(dwObjectID);
	if (itr != m_SiegeObjectMap.end())
	{
		return itr->second;
	}

	return NULL;
}

CSiegeObjectDBMgr::CampMap::iterator	CSiegeObjectDBMgr::DeleteCamp(unsigned long dwCampID)
{
	CampMap::iterator itr = m_CampMap.find(dwCampID);

	if (itr != m_CampMap.end())
	{
		CCampDB* lpCamp = itr->second;
		if (lpCamp)
		{
			// 길드전 시간에 모든 길드 요새가 부셔지는 경우에는 경계 적대 관계를 해지 시킨다.
			if (CGameTimeDBMgr::GetInstance().IsGuildWarTime() && !HasCampByGID(lpCamp->GetGID(), lpCamp->GetCampID()))
			{				
				Guild::CGuildDBMgr::GetInstance().CancelAlertHostility(lpCamp->GetGID());
			}
			
			SiegeObjectMap::iterator pos = m_SiegeObjectMap.find( lpCamp->GetCID() );
			if (pos != m_SiegeObjectMap.end())
			{
				m_SiegeObjectMap.erase(pos);
			}

			// DB 에서 삭제
            if (NULL != m_lpDBComponent)
            {
                DBComponent::CastleDB::DeleteCampInfo(*m_lpDBComponent, dwCampID, lpCamp->GetObjectType());
            }
			            
            DeleteSiegeObjectDB(lpCamp);
			lpCamp = NULL;
		}

		return m_CampMap.erase(itr);
	}

	return m_CampMap.end();
}

bool			CSiegeObjectDBMgr::DeleteSiegeObject(unsigned long dwCID)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.find(dwCID);
	SiegeObjectMap::iterator end = m_SiegeObjectMap.end();

	if (itr != end)
	{
		CSiegeObjectDB* lpObject = itr->second;
		if (lpObject)
		{
			if (lpObject->IsCamp())
			{
//				return ( m_CampMap.end() != DeleteCamp(lpObject->GetCampID()) );
				DeleteCamp(lpObject->GetCampID());
				return true;
			}

            DeleteSiegeObjectDB(lpObject);            
			lpObject = NULL;
		}

		m_SiegeObjectMap.erase(itr);
	}

	return true;
}

bool			CSiegeObjectDBMgr::DeleteSiegeObject(CSiegeObjectDB* lpObject)
{
	if (lpObject == NULL) return false;
	return DeleteSiegeObject(lpObject->GetCID());
}

bool			CSiegeObjectDBMgr::HasCampByGID(unsigned long dwGID, unsigned long dwExceptCampID)
{
	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();
	CCampDB* lpCamp = NULL;

	while (itr != end)
	{
		lpCamp = itr->second;
		if (0 != lpCamp && lpCamp->GetCampID() != dwExceptCampID && lpCamp->GetGID() == dwGID)
		{
			return true;
		}

		++itr;
	}

	return false;
}


bool			CSiegeObjectDBMgr::ReadCampInfoDB()
{
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT TblCampInfo.nCampID, "
		"TblCampInfo.nGuildID, "
		"TblCampInfo.nHP, "
		"TblCampInfo.tnZone, "
		"TblCampInfo.tnChannel, "
		"TblCampInfo.snObjectType, "
		"TblCampInfo.tnState, "
		"TblCampInfo.tnSubState, "
		"TblCampInfo.tnUpgradeStep, "
		"TblCampInfo.RemainTime, "
		"TblCampInfo.LastUseTime, "
		"TblCampInfo.bRight, "
		"TblCampInfo.fPosX, "
		"TblCampInfo.fPosY, "
		"TblCampInfo.fPosZ, "
		"TblCampInfo.tnMaterial, "
		"TblCampInfo.tnSiegeCount "
		"FROM TblGuildInfo, TblCampInfo "
		"WHERE TblGuildInfo.nGuildID = TblCampInfo.nGuildID AND TblGuildInfo.OldServerGroupID = %d",
		CServerSetup::GetInstance().GetAgentServerType());

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG2(g_Log, "Camp DB 얻어오기 실패 : %s %s", 
			m_lpDBComponent->GetErrorString(), m_lpDBComponent->GetQueryBuffer());
		return false;
	}

	CampInfoDB* campInfo = new CampInfoDB[MAX_ROWS];
	memset(campInfo, 0, sizeof(CampInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)campInfo, sizeof(CampInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (CampInfoDB* lpCampInfo = campInfo; nGetRows > 0; --nGetRows, ++lpCampInfo)
		{
			switch (lpCampInfo->m_wObjectType)
			{
				case Siege::CAMP:				CreateCamp(*lpCampInfo);		break;
				case Siege::MINING_CAMP:		CreateMiningCamp(*lpCampInfo);	break;
				case Siege::CAMP_SHOP:			CreateCampShop(*lpCampInfo);	break;
				case Siege::KARTERANT_WEAPON:	CreateWorldWeapon(*lpCampInfo);	break;
				case Siege::MERKADIA_WEAPON:	CreateWorldWeapon(*lpCampInfo);	break;
			}
		}

		memset(campInfo, 0, sizeof(CampInfoDB) * MAX_ROWS);
	}

	delete [] campInfo;

//	CreateDummyCamp();
	return true;
}

bool	CSiegeObjectDBMgr::ReadWorldWeaponInfoDB()
{
	const int MAX_ROWS = 20;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT TblWorldWeaponInfo.tnZone, "
		"TblWorldWeaponInfo.tnChannel, "
		"TblWorldWeaponInfo.tnKarRemainSiegeTime, "
		"TblWorldWeaponInfo.tnMerRemainSiegeTime "
		"FROM TblWorldWeaponInfo ");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "WorldWeaponInfo DB 얻어오기 실패");
		return false;
	}

	WorldWeaponInfoDB* weaponInfo = new WorldWeaponInfoDB[MAX_ROWS];
	memset(weaponInfo, 0, sizeof(WorldWeaponInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)weaponInfo, sizeof(WorldWeaponInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (WorldWeaponInfoDB* lpWeaponInfo = weaponInfo; nGetRows > 0; --nGetRows, ++lpWeaponInfo)
		{
			if (NULL != lpWeaponInfo)
			{
				unsigned long dwZoneChannel = static_cast<unsigned long>(lpWeaponInfo->m_cZone << 16 | lpWeaponInfo->m_cChannel);
				WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
				if (itr == m_WorldWeaponInfoMap.end())
				{
					WorldWeaponInfo tempInfo;
					tempInfo.m_cWorldWeaponNation = Creature::STATELESS;
					tempInfo.m_cKarRemainSiegeTime = lpWeaponInfo->m_cKarRemainSiegeTime;
					tempInfo.m_cMerRemainSiegeTime = lpWeaponInfo->m_cMerRemainSiegeTime;

					m_WorldWeaponInfoMap.insert(std::make_pair(dwZoneChannel, tempInfo)).second;
				}
				else
				{
					ERRLOG2(g_Log, "월드 웨폰 정보가 같은 존에 2개 이상 존재합니다. Zone : %d, Channel : %d",
						lpWeaponInfo->m_cZone, lpWeaponInfo->m_cChannel);
				}
			}
		}

		memset(weaponInfo, 0, sizeof(WorldWeaponInfoDB) * MAX_ROWS);
	}

	delete [] weaponInfo;
	return true;
}

bool		CSiegeObjectDBMgr::ReadCastleObjectInfoDB()
{
	const int MAX_ROWS = 10240;
	int nGetRows = 0;

	Castle::CCastleDB* lpCastle = NULL;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1,
		"SELECT "
		"TblCastleCreatureInfo.nCreatureID, "
		"TblCastleCreatureInfo.nCastleID, "
		"TblCastleCreatureInfo.nOwnerID, "
		"TblCastleCreatureInfo.nHP, "
		"TblCastleCreatureInfo.snObjectType, "
		"TblCastleCreatureInfo.fDirection, "
		"TblCastleCreatureInfo.tnState, "
		"TblCastleCreatureInfo.tnSubState, "
		"TblCastleCreatureInfo.tnUpgradeStep, "
		"TblCastleCreatureInfo.tnUpgradeType, "
		"TblCastleCreatureInfo.RemainTime, "
		"TblCastleCreatureInfo.LastUseTime, "
		"TblCastleCreatureInfo.fPosX, "
		"TblCastleCreatureInfo.fPosY, "
		"TblCastleCreatureInfo.fPosZ  "
		"FROM TblCastleInfo, TblCastleCreatureInfo WHERE "
		"TblCastleInfo.nCastleID = TblCastleCreatureInfo.nCastleID");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "성 오브젝트 DB 얻어오기 실패");
		return false;
	}

	CastleObjectInfoDB* castleObjectInfo = new CastleObjectInfoDB[MAX_ROWS];
	memset(castleObjectInfo, 0, sizeof(CastleObjectInfoDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)castleObjectInfo, sizeof(CastleObjectInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (CastleObjectInfoDB* lpCastleObjectInfo = castleObjectInfo; nGetRows > 0; --nGetRows, ++lpCastleObjectInfo)
		{
			lpCastleObjectInfo->m_dwCastleID = (Castle::CASTLE_BIT | lpCastleObjectInfo->m_dwCastleID);
			if (NULL == lpCastle || lpCastle->GetCastleID() != lpCastleObjectInfo->m_dwCastleID)
			{
				lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle(lpCastleObjectInfo->m_dwCastleID);
				if (NULL == lpCastle)
				{
					ERRLOG2(g_Log, "CID:0x%08x 존재하지않는 성에 속한 오브젝트입니다. CastleID:0x%08x",
						lpCastleObjectInfo->m_dwCID, lpCastleObjectInfo->m_dwCastleID);
					continue;
				}
			}

			CSiegeObjectDB* lpCastleObject = CreateCastleObject(*lpCastleObjectInfo);
			lpCastle->InsertCastleObject(lpCastleObject);
		}

		memset(castleObjectInfo, 0, sizeof(CastleObjectInfoDB) * MAX_ROWS);
	}

	delete [] castleObjectInfo;
	return true;
}

void	CSiegeObjectDBMgr::CreateDummyCamp()
{
	const unsigned long DUMMY_OBJECT_BIT = 0x00010000;
	const unsigned long MAX_DUMMY_CAMP = 200;
	const float BASE_POS_X = 2500.0f;
	const float BASE_POS_Z = 2500.0f;

	CampInfoDB camp;

	for (unsigned long nCount = 0; nCount < MAX_DUMMY_CAMP; ++nCount)
	{
		camp.m_dwCampID = Castle::CAMP_BIT + DUMMY_OBJECT_BIT + nCount;
		camp.m_dwGID = 1;
		camp.m_dwHP = 300;
		camp.m_cZone = 12;
		camp.m_cChannel = 0;
		camp.m_cState = Siege::COMPLETE;
		camp.m_cUpgradeStep = 0;
//		camp.m_TimeValue
//		camp.m_LastUseTime
//		camp.m_szRight[CampRight::MAX_RIGHT_NUM]
		camp.m_Pos.fPointX = static_cast<float>(BASE_POS_X + cosf(static_cast<float>(nCount)) * (rand() % 100));
		camp.m_Pos.fPointY = 0.0f;
		camp.m_Pos.fPointZ = static_cast<float>(BASE_POS_Z + sinf(static_cast<float>(nCount)) * (rand() % 100));

		CreateCamp(camp);
	}
}


unsigned long	CSiegeObjectDBMgr::GetBlankIndex()
{
	static unsigned long globalIndex = Siege::VIRTUAL_CID_START_NUM;
	return globalIndex++;

/*
	if (0 == m_SiegeObjectMap.size()) return 1;

	SiegeObjectMap::reverse_iterator LastItr = m_SiegeObjectMap.rbegin();
	unsigned long dwLastIndex = LastItr->first;

	return (dwLastIndex & ~Creature::SIEGE_OBJECT_BIT) + 1;
*/
}

int			CSiegeObjectDBMgr::DestroyCamp(unsigned long dwGID)
{
	int nDestroyCount = 0;
	if (0 == dwGID || 0 == m_CampMap.size()) return 0;

	CampMap::iterator itr = m_CampMap.begin();

	while (itr != m_CampMap.end())
	{
		CCampDB* lpCamp = itr->second;
		if (NULL == lpCamp)
		{
			ERRLOG0(g_Log, "NULL 진지가 포함되어 있습니다.");
			++itr;
			continue;
		}

		if (lpCamp->GetGID() != dwGID)
		{
			++itr;
			continue;
		}

		lpCamp->Destroy();

        SERVER_ID serverID;

        serverID.sID.Type       = CServerSetup::GameServer;
        serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
        serverID.sID.Channel    = lpCamp->GetChannel();
        serverID.sID.ID         = lpCamp->GetZone();

        GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
            DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

        if (0 != lpGameDispatch)
        {
            CSendStream& SendStream = lpGameDispatch->GetSendStream();

            char* lpBuffer = SendStream.GetBuffer(sizeof(PktCampCmd));
            if (0 != lpBuffer)
            {
                PktCampCmd* lpPktCampCmd = reinterpret_cast<PktCampCmd*>(lpBuffer);

                lpPktCampCmd->m_dwCID       = lpCamp->GetCID();
                lpPktCampCmd->m_dwCampID    = lpCamp->GetCampID();
                lpPktCampCmd->m_cState      = lpCamp->GetState();
                lpPktCampCmd->m_dwValue1    = 0;
                lpPktCampCmd->m_dwValue2    = 0;
                lpPktCampCmd->m_cSubCmd     = PktCampCmd::CAMP_DESTROY;

                SendStream.WrapHeader(sizeof(PktCampCmd), CmdCampCmd, 0, 0);
            }
        }

		itr = DeleteCamp(lpCamp->GetCampID());
		++nDestroyCount;
	}

	return nDestroyCount;
}

// 길드 탈퇴시 해당 캐릭터의 공성병기를 아이템으로 돌려주기
void			CSiegeObjectDBMgr::DestroySiegeArms(unsigned long dwOnwerCID)
{
	if (0 == m_SiegeObjectMap.size()) return;

	std::list<unsigned long>	DeleteList;
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();

	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObjectDB* lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			if (lpSiegeObject->IsSiegeArms() && lpSiegeObject->GetOwnerID() == dwOnwerCID)
			{
				lpSiegeObject->ToStartKit();
				lpSiegeObject->SendSiegeArmsCmd(0, PktSiegeArmsCmd::SIEGE_TO_STARTKIT_COMPLETE);

				lpSiegeObject->Destroy();
				DeleteList.push_back(lpSiegeObject->GetCID());
			}
		}

		++itr;
	}

	// 객체 삭제
	std::list<unsigned long>::iterator DeleteItr = DeleteList.begin();
	while (DeleteItr != DeleteList.end())
	{
		DeleteSiegeObject(*DeleteItr);
		++DeleteItr;
	}
}

// 공성 시간이 끝났을때 아이템으로 돌려주기
void			CSiegeObjectDBMgr::DestoryAllSiegeArms()
{
	if (0 == m_SiegeObjectMap.size()) return;

	std::list<unsigned long>	DeleteList;
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();

	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObjectDB* lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			if (lpSiegeObject->IsSiegeArms())
			{
				lpSiegeObject->ToStartKit();
				lpSiegeObject->SendSiegeArmsCmd(0, PktSiegeArmsCmd::SIEGE_TO_STARTKIT_COMPLETE);

				lpSiegeObject->Destroy();
				DeleteList.push_back(lpSiegeObject->GetCID());
			}
		}

		++itr;
	}

	// 객체 삭제
	std::list<unsigned long>::iterator DeleteItr = DeleteList.begin();
	while (DeleteItr != DeleteList.end())
	{
		DeleteSiegeObject(*DeleteItr);
		++DeleteItr;
	}
}

bool	CSiegeObjectDBMgr::ChangeCampType(unsigned long dwCampID, unsigned short wChangeType)
{
	CampMap::iterator itr = m_CampMap.find(dwCampID);
	if ( itr == m_CampMap.end() ) return false;

    CCampDB* lpCamp = itr->second;
	if ( lpCamp )
	{
		CampInfoDB campInfoDB;
		unsigned long dwCID			= lpCamp->GetCID();
		campInfoDB.m_dwCampID		= lpCamp->GetCampID();
		campInfoDB.m_dwGID			= lpCamp->GetGID();
		campInfoDB.m_dwHP			= lpCamp->GetNowHP();
		campInfoDB.m_cZone			= lpCamp->GetZone();
		campInfoDB.m_cChannel		= lpCamp->GetChannel();
		campInfoDB.m_wObjectType	= lpCamp->GetChangeType();
		campInfoDB.m_cState			= lpCamp->GetState();
		campInfoDB.m_cSubState		= lpCamp->GetSubState();
		campInfoDB.m_cUpgradeStep	= lpCamp->GetUpgradeStep();
		campInfoDB.m_TimeValue		= lpCamp->GetTimeValue();
		campInfoDB.m_LastUseTime	= lpCamp->GetLastUseTime();
		memcpy(campInfoDB.m_szRight, lpCamp->GetRight().m_aryCampRight, CampRight::MAX_RIGHT_NUM);
		campInfoDB.m_Pos.fPointX	= lpCamp->GetPosition().m_fPointX;
		campInfoDB.m_Pos.fPointY	= lpCamp->GetPosition().m_fPointY;
		campInfoDB.m_Pos.fPointZ	= lpCamp->GetPosition().m_fPointZ;
		campInfoDB.m_cMaterial		= lpCamp->GetMaterialNum();
		campInfoDB.m_cSiegeCount	= 0;

		// 이전 객체 삭제 ( DB 에서 삭제하지는 않는다. )
		SiegeObjectMap::iterator pos = m_SiegeObjectMap.find( lpCamp->GetCID() );
		if (pos != m_SiegeObjectMap.end())
		{
			m_SiegeObjectMap.erase(pos);
		}
		m_CampMap.erase(itr);
		DeleteSiegeObjectDB(lpCamp);
		
		// 새로운 객체로 할당
		CSiegeObjectDB* lpSiegeObject = NULL;
		switch (campInfoDB.m_wObjectType)
		{
			case Siege::CAMP :			lpSiegeObject = CreateCamp(campInfoDB, dwCID);			break;
			case Siege::CAMP_SHOP :		lpSiegeObject = CreateCampShop(campInfoDB, dwCID);		break;
			case Siege::MINING_CAMP :	lpSiegeObject = CreateMiningCamp(campInfoDB, dwCID);	break;
		}

		if ( lpSiegeObject )
		{
			return true;
		}
	}

	return false;
}

void	CSiegeObjectDBMgr::SendWorldWeaponInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel)
{
	char szBuffer[2048];

	unsigned long dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);
	WorldWeaponInfoMap::const_iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
	if (itr != m_WorldWeaponInfoMap.end())
	{
		const WorldWeaponInfo& WeaponInfo = itr->second;

		PktWorldWeaponInfo* lpPktWeaponInfo = reinterpret_cast<PktWorldWeaponInfo* >(szBuffer);

		lpPktWeaponInfo->m_cKarRemainSiegeTime = WeaponInfo.m_cKarRemainSiegeTime;
		lpPktWeaponInfo->m_cMerRemainSiegeTime = WeaponInfo.m_cMerRemainSiegeTime;

		SendStream.WrapCompress(szBuffer, sizeof(PktWorldWeaponInfo), CmdWorldWeaponInfo, 0, 0);
	}
}

void	CSiegeObjectDBMgr::SendCampInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel)
{
	char szBuffer[2048];
	unsigned short	usBufferSize = 0;
	unsigned long	dwCID = 0;
	bool bFirst = true;

	for (CampMap::iterator itr = m_CampMap.begin(); itr != m_CampMap.end(); ++itr)
	{
		CCampDB* lpCamp = itr->second;
		if ( !lpCamp )
		{
			ERRLOG0(g_Log, "NULL Camp가 포함되어 있습니다.");
			return;
		}

		if (lpCamp->GetZone() != cZone || lpCamp->GetChannel() != cChannel) 
		{            
			continue;
		}

		if (lpCamp->SerializeOut(szBuffer + sizeof(PktCampInfo), usBufferSize, dwCID))
		{
			PktCampInfo* lpPktCampInfo = reinterpret_cast<PktCampInfo*>(szBuffer);

			lpPktCampInfo->m_bStartFlag = bFirst;
			lpPktCampInfo->m_dwCID		= dwCID;
			lpPktCampInfo->m_wSize		= usBufferSize;

			SendStream.WrapCompress(szBuffer, static_cast<unsigned short>(sizeof(PktCampInfo) + usBufferSize), CmdCampInfo, 0, 0);
		}

		bFirst = false;
	}
}

void			CSiegeObjectDBMgr::SendSiegeArmsInfo(CSendStream& SendStream, unsigned char cZone, 
                                                     unsigned char cChannel)
{
	if (0 == m_SiegeObjectMap.size()) 
    {
        return;
    }

	for (SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin(); itr != m_SiegeObjectMap.end(); ++itr)
	{
		CSiegeObjectDB* lpSiegeObject = itr->second;
		if (NULL == lpSiegeObject)
		{
			//ERRLOG0(g_Log, "NULL 길드가 포함되어 있습니다.");
			return;
		}

		if (lpSiegeObject->GetZone() != cZone || lpSiegeObject->GetChannel() != cChannel)
        {
            continue;
        }

		if ( lpSiegeObject->IsSiegeArms() )
		{
            char* lpBuffer = SendStream.GetBuffer(sizeof(PktCreateSiegeArms));
            if (0 != lpBuffer)
            {
                PktCreateSiegeArms* lpPktCreateSiegeArmsInfo = 
                    reinterpret_cast<PktCreateSiegeArms*>(lpBuffer);

                lpPktCreateSiegeArmsInfo->m_dwCID		    = lpSiegeObject->m_dwCID;
                lpPktCreateSiegeArmsInfo->m_dwOwnerID	    = lpSiegeObject->m_dwOwnerID;
                lpPktCreateSiegeArmsInfo->m_wObjectType	    = lpSiegeObject->m_wObjectType;
                lpPktCreateSiegeArmsInfo->m_cNation		    = lpSiegeObject->m_cNation;
                lpPktCreateSiegeArmsInfo->m_dwHP			= lpSiegeObject->m_dwHP;
                lpPktCreateSiegeArmsInfo->m_cZone		    = lpSiegeObject->m_cZone;
                lpPktCreateSiegeArmsInfo->m_cChannel		= lpSiegeObject->m_cChannel;
                lpPktCreateSiegeArmsInfo->m_cState		    = lpSiegeObject->m_cState;
                lpPktCreateSiegeArmsInfo->m_cUpgradeStep	= lpSiegeObject->m_cUpgradeStep;
                lpPktCreateSiegeArmsInfo->m_Pos.fPointX	    = lpSiegeObject->m_Pos.m_fPointX;
                lpPktCreateSiegeArmsInfo->m_Pos.fPointY	    = lpSiegeObject->m_Pos.m_fPointY;
                lpPktCreateSiegeArmsInfo->m_Pos.fPointZ	    = lpSiegeObject->m_Pos.m_fPointZ;

                SendStream.WrapHeader(sizeof(PktCreateSiegeArms), CmdCreateSiegeArms, 0, 0);
            }
		}
	}
}


void	CSiegeObjectDBMgr::SendProcessMining(unsigned long dwProcessType)
{
	const unsigned short MAX_BUFFER_SIZE = sizeof(PktProcessMining);
	char szBuffer[MAX_BUFFER_SIZE];

	PktProcessMining* lpPktPM = reinterpret_cast<PktProcessMining* >(szBuffer);

	lpPktPM->m_dwProcessType = dwProcessType;

	if (PacketWrap::WrapHeader(reinterpret_cast<char*>(szBuffer), sizeof(PktProcessMining), CmdProcessMining, 0, 0))
	{
		DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
			reinterpret_cast<char*>(&szBuffer), sizeof(PktProcessMining), CmdProcessMining));
	}
}

bool	CSiegeObjectDBMgr::SerializeOutCastleObject(CSiegeObjectDB* lpObject_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out)
{
	wBufferSize_Out = 0;

	if (lpObject_In->IsCamp() || lpObject_In->IsSiegeArms()) return false;
	else
	{
		CastleObjectInfo* lpCastleObjectInfo = reinterpret_cast<CastleObjectInfo*>(lpBuffer_Out);

		lpCastleObjectInfo->m_dwCID			= lpObject_In->m_dwCID;
		lpCastleObjectInfo->m_dwCastleID	= lpObject_In->GetCastleID();
		lpCastleObjectInfo->m_dwOwnerID		= lpObject_In->m_dwOwnerID;
		lpCastleObjectInfo->m_cNation		= lpObject_In->m_cNation;
		lpCastleObjectInfo->m_wObjectType	= lpObject_In->m_wObjectType;
		lpCastleObjectInfo->m_fDefaultDir	= lpObject_In->m_fDefaultDir;
		lpCastleObjectInfo->m_cState		= lpObject_In->m_cState;
		lpCastleObjectInfo->m_cSubState		= lpObject_In->m_cSubState;
		lpCastleObjectInfo->m_dwHP			= lpObject_In->m_dwHP;
		lpCastleObjectInfo->m_Pos.fPointX	= lpObject_In->m_Pos.m_fPointX;
		lpCastleObjectInfo->m_Pos.fPointY	= lpObject_In->m_Pos.m_fPointY;
		lpCastleObjectInfo->m_Pos.fPointZ	= lpObject_In->m_Pos.m_fPointZ;
		lpCastleObjectInfo->m_cUpgradeStep	= lpObject_In->m_cUpgradeStep;
		lpCastleObjectInfo->m_cUpgradeType	= lpObject_In->m_cUpgradeType;

		wBufferSize_Out = sizeof(CastleObjectInfo);
	}

	return true;
}


unsigned char	CSiegeObjectDBMgr::GetWorldWeaponNation(unsigned char cZone, unsigned char cChannel) const
{
	unsigned long dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);
	WorldWeaponInfoMap::const_iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
	if (itr != m_WorldWeaponInfoMap.end())
	{
		const WorldWeaponInfo& WeaponInfo = itr->second;
		return WeaponInfo.m_cWorldWeaponNation;
	}

	return Creature::STATELESS;
}

void	CSiegeObjectDBMgr::DeleteWorldWeaponNation(unsigned char cZone, unsigned char cChannel)
{
	unsigned long dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);
	WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
	if (itr != m_WorldWeaponInfoMap.end())
	{
		WorldWeaponInfo& WeaponInfo = itr->second;
		WeaponInfo.m_cWorldWeaponNation = Creature::STATELESS;
	}
}

void	CSiegeObjectDBMgr::UpdateWorldWeaponInfo(unsigned char cZone, unsigned char cChannel, unsigned char cOwenrNation,
												 unsigned char cDestroyedWeaponNation, unsigned char cRemainSiegeTime)
{
	unsigned long dwZoneChannel = static_cast<unsigned long>(cZone << 16 | cChannel);
	WorldWeaponInfoMap::iterator itr = m_WorldWeaponInfoMap.find(dwZoneChannel);
	if (itr != m_WorldWeaponInfoMap.end())
	{
        WorldWeaponInfo& WeaponInfo = itr->second;
		WeaponInfo.m_cWorldWeaponNation = cOwenrNation;

		switch (cDestroyedWeaponNation)
		{
			case Creature::KARTERANT:
			{
				WeaponInfo.m_cKarRemainSiegeTime = cRemainSiegeTime;
			}
			break;

			case Creature::MERKADIA:
			{
				WeaponInfo.m_cMerRemainSiegeTime = cRemainSiegeTime;
			}
			break;
		}
	}
	else
	{
		WorldWeaponInfo tempInfo;
		tempInfo.m_cWorldWeaponNation = cOwenrNation;

		switch (cDestroyedWeaponNation)
		{
		case Creature::KARTERANT:
			{
				tempInfo.m_cKarRemainSiegeTime = cRemainSiegeTime;
				tempInfo.m_cMerRemainSiegeTime = 0;
			}
			break;

		case Creature::MERKADIA:
			{
				tempInfo.m_cKarRemainSiegeTime = 0;
				tempInfo.m_cMerRemainSiegeTime = cRemainSiegeTime;
			}
			break;
		}
		
		m_WorldWeaponInfoMap.insert(std::make_pair(dwZoneChannel, tempInfo)).second;
	}
}