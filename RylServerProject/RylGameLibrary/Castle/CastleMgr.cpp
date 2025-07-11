#include "stdafx.h"
#include "CastleMgr.h"

#include <Castle/Castle.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleBlessMgr.h>

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/CreatureManager.h>
#include <Creature/Character/CharRespawnMgr.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>



using namespace Castle;
using namespace Siege;


CCastleMgr::CCastleMgr()
{
	Initialize();
}

CCastleMgr::~CCastleMgr()
{
	Destroy();
}

CCastleMgr& CCastleMgr::GetInstance()
{
	static CCastleMgr ms_this;
	return ms_this;
}

bool	CCastleMgr::Initialize()
{
	return true;
}

void	CCastleMgr::Destroy()
{
	if (0 == m_CastleMap.size()) return;

	CastleMap::iterator CastleItr = m_CastleMap.begin();
	CastleMap::iterator CastleEnd = m_CastleMap.end();

	while (CastleItr != CastleEnd)
	{
		CCastle* lpCastle = CastleItr->second;

		if (lpCastle)
		{
			delete lpCastle;
			lpCastle = NULL;
		}

		++CastleItr;
	}

	m_CastleMap.clear();
}

void		CCastleMgr::Process()
{
	if (0 == m_CastleMap.size()) return;

	CastleMap::iterator CastleItr = m_CastleMap.begin();
	CastleMap::iterator CastleEnd = m_CastleMap.end();

	while (CastleItr != CastleEnd)
	{
		CCastle* lpCastle = CastleItr->second;

		if (lpCastle) lpCastle->Process();

		++CastleItr;
	}
}

CCastle*	CCastleMgr::GetCastle(unsigned long dwCastleID)
{
	CastleMap::iterator itr = m_CastleMap.find(dwCastleID);
	if (itr != m_CastleMap.end())
	{
		return itr->second;
	}

	return NULL;
}

CCastle*	CCastleMgr::GetCastleByNameID(unsigned long dwCastleNameID)
{
	CastleMap::iterator itr = m_CastleMap.begin();

	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (lpCastle && lpCastle->GetNameID() == dwCastleNameID)
		{
			return lpCastle;
		}

		++itr;
	}

	return NULL;
}

// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.

//CCastle*	CCastleMgr::GetCastleByGID(unsigned long dwGID)
//{
//	CastleMap::iterator itr = m_CastleMap.begin();
//
//	while (itr != m_CastleMap.end())
//	{
//		CCastle* lpCastle = itr->second;
//		if (lpCastle && lpCastle->GetGID() == dwGID)
//		{
//			return lpCastle;
//		}
//
//		++itr;
//	}
//
//	return NULL;
//}

CCastle*	CCastleMgr::GetCastleInBlessArea(const Position& Pos)
{
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (NULL != lpCastle)
		{
            CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
			if (NULL != lpEmblem)
			{
				float fBlessArea = CCastleBlessMgr::GetInstance().GetBlessArea(
					lpCastle->GetTotalGainTaxCount(), lpEmblem->GetUpgradeStep());

                const float fDX = Pos.m_fPointX - lpEmblem->GetPosition().m_fPointX;
				const float fDZ = Pos.m_fPointZ - lpEmblem->GetPosition().m_fPointZ;
				const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) );

				if (fDistance <= fBlessArea)
				{
					return lpCastle;
				}
			}
		}

		++itr;
	}

	return NULL;
}

unsigned char	CCastleMgr::GetCastleNum() const
{
	return static_cast<unsigned char>(m_CastleMap.size());
}

unsigned char	CCastleMgr::GetCastleNum(unsigned char cNation, unsigned char cZone) const
{
	unsigned char cNum = 0;
	CastleMap::const_iterator itr = m_CastleMap.begin();
	CastleMap::const_iterator end = m_CastleMap.end();

	while (itr != end)
	{
		const CCastle* pCastle = itr->second;
		if (pCastle &&
			pCastle->GetNation() == cNation &&
			pCastle->GetZone() == cZone)
		{
			++cNum;
		}

		++itr;
	}

	return cNum;
}

unsigned char	CCastleMgr::GetCastleNum(unsigned char cNation, unsigned char cZone, const Position& Pos) const
{
	unsigned char cNum = 0;
	CastleMap::const_iterator itr = m_CastleMap.begin();
	CastleMap::const_iterator end = m_CastleMap.end();

	while (itr != end)
	{
		const CCastle* pCastle = itr->second;
		if (pCastle &&
			pCastle->GetNation() == cNation &&
			pCastle->GetZone() == cZone)
		{
			const CSiegeObject* pEmblem = pCastle->GetCastleEmblem();
			if (pEmblem)
			{
				const float fDX = Pos.m_fPointX - pEmblem->GetPosition().m_fPointX;
				const float fDZ = Pos.m_fPointZ - pEmblem->GetPosition().m_fPointZ;
				const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) );

				if (fDistance <= Castle::CASTLE_EXP_BONUS_RADIUS)
				{
					++cNum;
				}
			}
		}

		++itr;
	}

	return cNum;
}

void	CCastleMgr::GetCastleSiegeInfo(CastleSiegeInfo* lpCastleSiegeInfo, unsigned char& cNum, unsigned short& wSize)
{
	if (NULL == lpCastleSiegeInfo)
	{
		return;
	}

	for (CastleMap::iterator itr = m_CastleMap.begin(); itr != m_CastleMap.end(); ++itr)
	{
		CCastle* lpCastle = itr->second;
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


bool		CCastleMgr::ExistCastleInRadius(const Position& Pos)
{
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (lpCastle)
		{
			CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
			if (lpEmblem)
			{
				const float fDX = Pos.m_fPointX - lpEmblem->GetPosition().m_fPointX;
				const float fDZ = Pos.m_fPointZ - lpEmblem->GetPosition().m_fPointZ;
				const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) );

				if (fDistance <= Siege::CAMP_BUILDING_RADIUS)
				{
					return true;
				}
			}
		}

		++itr;
	}

	return false;
}

bool		CCastleMgr::ExistSiegeInRadius(const Position& Pos)
{	
	if(Pos.m_fPointX>=1890 && Pos.m_fPointX<=2205)
	{
		if(Pos.m_fPointZ>=2695 && Pos.m_fPointZ<=2835)
		{
			return true;
		}
	}

	return false;
}

bool		CCastleMgr::SendCastleInfo(CSendStream& SendStream)
{
	CastleMap::iterator itr = m_CastleMap.begin();
	CastleMap::iterator end = m_CastleMap.end();

	while (itr != end)
	{
		CCastle* lpCastle = itr->second;
		
		if (lpCastle)
		{
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktCreateCastle));
			if (lpBuffer)
			{
				PktCreateCastle* lpPktCreateCastle = reinterpret_cast<PktCreateCastle*>(lpBuffer);

				lpPktCreateCastle->m_dwCastleID			= lpCastle->m_dwCastleID;
				lpPktCreateCastle->m_cNation			= lpCastle->m_cNation;
				lpPktCreateCastle->m_cZone				= lpCastle->m_cZone;
				lpPktCreateCastle->m_cNameID			= lpCastle->m_cNameID;
				lpPktCreateCastle->m_cUpgradeStep		= lpCastle->GetUpgradeStep();
				lpPktCreateCastle->m_cInvincibleCount	= lpCastle->m_cInvincibleCount;
				lpPktCreateCastle->m_wTotalSiegeCount	= lpCastle->m_wTotalSiegeCount;
				lpPktCreateCastle->m_dwTotalTaxMoney	= lpCastle->m_dwTotalTaxMoney;
				lpPktCreateCastle->m_fPosX				= lpCastle->GetCastleEmblem()->GetPosition().m_fPointX ;
				lpPktCreateCastle->m_fPosY				= lpCastle->GetCastleEmblem()->GetPosition().m_fPointY ;
				lpPktCreateCastle->m_fPosZ				= lpCastle->GetCastleEmblem()->GetPosition().m_fPointZ ;
				lpPktCreateCastle->m_wItemID			= lpCastle->m_wItemID;
				lpPktCreateCastle->m_CastleRight		= lpCastle->m_CastleRight;

				for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
				{
					lpPktCreateCastle->m_cItemNum[ i ] = lpCastle->m_cItemNum[ i ] ;
				}

				for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
				{
					lpPktCreateCastle->m_CastleTax[ i ] = lpCastle->m_CastleTax[ i ];
				}

				SendStream.WrapCrypt(sizeof(PktCreateCastle), CmdCreateCastle, 0, PktBase::NO_SERVER_ERR);
			}
		}

		++itr;
	}

	return true;
}

bool		CCastleMgr::SerializeIn(char* lpBuffer_In, unsigned short wBufferSize_In, unsigned char cObjectNum)
{
	// lpBuffer_In = CastleInfoDB + CastleTaxInfo * MAX_TAX_TYPE
	//				 + CastleMineralInfo + CastleMineral * (AccumulatedNum + TemporaryNum)
	//				 + CastleObjectInfo * cObjectNum

	CastleInfoDB* lpCastleInfo = reinterpret_cast<CastleInfoDB*>(lpBuffer_In);
	CCastle* lpCastle = new CCastle(*lpCastleInfo);
	if (lpCastle)
	{
		if (false == m_CastleMap.insert(std::make_pair(lpCastle->GetCastleID(), lpCastle)).second)
		{
			delete lpCastle;
			return false;
		}

		CastleTaxInfo* lpCastleTaxInfo = reinterpret_cast<CastleTaxInfo*>( lpCastleInfo + 1 );
		for (int i=0; i<MAX_TAX_TYPE; ++i)
		{
			lpCastle->SetTaxInfo(*lpCastleTaxInfo);
			++lpCastleTaxInfo;
		}

		CastleMineralInfo* lpCastleMineralInfo = reinterpret_cast<CastleMineralInfo*>( lpCastleTaxInfo );
		CastleMineral* lpCastleMinerl = reinterpret_cast<CastleMineral*>( lpCastleMineralInfo + 1 );
		unsigned char cAccumulatedNum = lpCastleMineralInfo->m_cAccumulatedNum;
		unsigned char cTemporaryNum = lpCastleMineralInfo->m_cTemporaryNum;

		for (int i=0; i<cAccumulatedNum; ++i)
		{
			lpCastle->SetMineralInfo( lpCastleMinerl->m_wMineralID, lpCastleMinerl->m_wAmount, Castle::ACCUMULATED_MINERAL );
			++lpCastleMinerl;
		}

		for (int i=0; i<cTemporaryNum; ++i)
		{
			lpCastle->SetMineralInfo( lpCastleMinerl->m_wMineralID, lpCastleMinerl->m_wAmount, Castle::TEMPORARY_MINERAL );
			++lpCastleMinerl;
		}

		CastleObjectInfo* lpCastleObjectInfo = reinterpret_cast<CastleObjectInfo*>( lpCastleMinerl );
		for (int i=0; i<cObjectNum; ++i)
		{
			CSiegeObject* lpCastleObject = CSiegeObjectMgr::GetInstance().CreateCastleObject(*lpCastleObjectInfo);
			if (lpCastleObject)
			{
				lpCastle->InsertCastleObject(lpCastleObject);
			}

			++lpCastleObjectInfo;
		}

		// 상징물 업그레이드 효과 적용
		lpCastle->UpgradeByEmblem();

		// 리스폰 포인트 추가
		CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
		if (lpEmblem && Creature::STATELESS != lpCastle->GetNation())
		{
			CCharRespawnMgr::GetInstance().SetCastleRespawnPointNation(lpCastle->GetNameID(), lpCastle->GetNation());
		}

		// NPC 리스트중에 해당 성에 속한 NPC 의 Nation 을 셋팅
		CCreatureManager::GetInstance().SetNationToCastleNPC(lpCastle->GetCastleID(), lpCastle->GetNation());

		return true;
	}

	return false;
}


void		CCastleMgr::ProcessEmblemRegenHPAndMP()
{
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (lpCastle && lpCastle->GetCastleEmblem())
		{
			lpCastle->GetCastleEmblem()->RegenHPAndMP(0, 0, true);
			lpCastle->GetCastleEmblem()->SendHPUpdateToDBAgent();
		}

		++itr;
	}
}

void		CCastleMgr::EnableTaxChange()
{
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (lpCastle)
		{
			lpCastle->EnableTaxChange();
		}

		++itr;
	}
}

void		CCastleMgr::DestroyAllCastleArms()
{
	CastleMap::iterator itr = m_CastleMap.begin();
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		if (lpCastle)
		{
			lpCastle->DestroyAllCastleArms(false);
		}

		++itr;
	}
}

bool		CCastleMgr::HasCastleArms(unsigned long dwCID)
{
	CastleMap::iterator itr = m_CastleMap.begin();
	
	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;
		
		if (lpCastle)
		{
			if(lpCastle->HasCastleArms(dwCID))
			{
				return true;
			}			
		}

		++itr;
	}

	return false;
}

unsigned char CCastleMgr::GetNation()
{
	CastleMap::iterator itr = m_CastleMap.begin();

	while (itr != m_CastleMap.end())
	{
		CCastle* lpCastle = itr->second;

		if (lpCastle)
		{
			return lpCastle->GetNation();
		}

		++itr;
	}

	return Creature::STATELESS;
	
}