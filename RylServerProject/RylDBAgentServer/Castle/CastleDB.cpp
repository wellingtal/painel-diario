#include "stdafx.h"
#include "CastleDB.h"

#include <Creature/Siege/SiegeObjectDB.h>
#include <Creature/Siege/CastleGateDB.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <DB/DBComponent.h>
#include <Castle/CastleDBComponent.h>
#include <Castle/CastleDBMgr.h>
#include <GameTime/GameTimeDBMgr.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Utility/Setup/ServerSetup.h>
#include <Log/ServerLog.h>


CCastleDB::CCastleDB(CDBComponent& DBComponent, const CastleInfoDB& CastleInfo)
:   m_dwCastleID(Castle::CASTLE_BIT | CastleInfo.m_dwCastleID), 
    m_cNation(CastleInfo.m_cNation), 
    m_cZone(CastleInfo.m_cZone),
	m_cChannel(CastleInfo.m_cChannel),
	m_cNameID(CastleInfo.m_cNameID),
	m_cSiegeCount(CastleInfo.m_cSiegeCount),
	m_cInvincibleCount(CastleInfo.m_cInvincibleCount),
	m_wTotalSiegeCount(CastleInfo.m_wTotalSiegeCount),
	m_dwTotalTaxMoney(CastleInfo.m_dwTotalTaxMoney),
	m_wItemID(CastleInfo.m_wItemID),
	m_cItemNum(CastleInfo.m_cItemNum),
	m_bEnableSiege(false),
    m_DBComponent(DBComponent)
{
	::memcpy(&m_CastleRight, CastleInfo.m_szRight, sizeof(CastleRight));

	m_BackDoorPos[0] = CastleInfo.m_InPos;
	m_BackDoorPos[1] = CastleInfo.m_OutPos;

	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);

	CheckEnableSiege( CGameTimeDBMgr::GetInstance().IsSiegeWarTime() );
}

CCastleDB::~CCastleDB()
{

}

bool	CCastleDB::ChangeCastleMaster(unsigned char cNation)
{
	m_cNation = cNation;
	m_cSiegeCount = 0;
	m_cInvincibleCount = Castle::INVINCIBLE_COUNT;
	m_wTotalSiegeCount = 0;
	m_dwTotalTaxMoney = 0;
	m_wItemID = 0;
	m_cItemNum = 0;

	m_bEnableSiege = (m_cInvincibleCount > 0) ? false : true;
	
	m_CastleRight = CastleRight();
	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);
	m_AccumulatedMineralMap.clear();
	m_TemporaryMineralMap.clear();

	// DB 에 정보 저장
	DBComponent::CastleDB::UpdateCastleInfo(m_DBComponent, m_dwCastleID, m_cNation, m_cSiegeCount, m_cInvincibleCount,
											m_wTotalSiegeCount, m_dwTotalTaxMoney, m_wItemID, m_cItemNum);
	DBComponent::CastleDB::DeleteCastleTaxInfo(m_DBComponent, m_dwCastleID);
	DBComponent::CastleDB::DeleteCastleMineralInfo(m_DBComponent, m_dwCastleID);
	DBComponent::CastleDB::UpdateCastleRight(m_DBComponent, m_dwCastleID, reinterpret_cast<char *>(&m_CastleRight), sizeof(CastleRight));

	// 성 오브젝트들의 소유 국가들을 모두 변경한다.
	ChangeCastleObjectNation(cNation);
	
	// 수성 병기를 병기 관리 NPC 로 돌려놓는다.
	DestroyAllCastleArms();

	// 공성 가능 정보 변경 전송
	CGameTimeDBMgr::GetInstance().SendGameTimeInfo( GetCastleID() );
	return true;
}

bool	CCastleDB::InsertCastleObject(CSiegeObjectDB* lpCastleObject)
{
	if (lpCastleObject)
	{
		lpCastleObject->SetNation(m_cNation);
		lpCastleObject->SetZoneChannel(m_cZone, m_cChannel);

		return m_CastleObjectMap.insert(std::make_pair(lpCastleObject->GetCID(), lpCastleObject)).second;
	}

	return false;
}


bool	CCastleDB::DeleteCastleObject(unsigned long dwCID)
{
	return true;
}

bool	CCastleDB::DeleteCastleObject(CSiegeObjectDB* lpSiegeObject)
{
	return true;
}

void	CCastleDB::LoseOwnership()
{
	m_cNation = 0;
	m_cSiegeCount = 0;
	m_cInvincibleCount = 0;
	m_wTotalSiegeCount = 0;
	m_dwTotalTaxMoney = 0;
	m_wItemID = 0;
	m_cItemNum = 0;
	m_bEnableSiege = true;

	m_CastleRight = CastleRight();
	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);
	m_AccumulatedMineralMap.clear();
	m_TemporaryMineralMap.clear();

	// DB 업데이트..
	DBComponent::CastleDB::UpdateCastleInfo(m_DBComponent, m_dwCastleID, m_cNation, m_cSiegeCount, m_cInvincibleCount,
											m_wTotalSiegeCount, m_dwTotalTaxMoney, m_wItemID, m_cItemNum);
	DBComponent::CastleDB::DeleteCastleTaxInfo(m_DBComponent, m_dwCastleID);
	DBComponent::CastleDB::DeleteCastleMineralInfo(m_DBComponent, m_dwCastleID);
	DBComponent::CastleDB::UpdateCastleRight(m_DBComponent, m_dwCastleID, reinterpret_cast<char *>(&m_CastleRight), sizeof(CastleRight));

	// 성 오브젝트 중립화
	ChangeCastleObjectNation(0);

	// 수성 병기를 병기 관리 NPC 로 돌려놓는다.
	DestroyAllCastleArms();

	// 성 소유 중립화 명령 전송
	CSiegeObjectDB* lpEmblem = GetCastleEmblem();
	if (lpEmblem)
	{
		lpEmblem->SendCastleCmd(0, PktCastleCmd::CASTLE_LOSE_OWNERSHIP);
	}

	// 공성 가능 정보 변경 전송
	CGameTimeDBMgr::GetInstance().SendGameTimeInfo( GetCastleID() );
}

void	CCastleDB::CloseCastleGate()
{
	CSiegeObjectDB* lpGate = GetCastleGate();
	if (lpGate)
	{
		// HP 를 Full 로 하는것은 게임 서버에서 얼마로 채울것인지 보내준다.
		static_cast<CCastleGateDB* >(lpGate)->ForceClose();
	}
}

void	CCastleDB::OpenCastleGate()
{
	CSiegeObjectDB* lpGate = GetCastleGate();
	if (lpGate)
	{
		// HP 를 Full 로 하는것은 게임 서버에서 얼마로 채울것인지 보내준다.
		static_cast<CCastleGateDB* >(lpGate)->ForceOpen();
	}
}

void	CCastleDB::ChangeCastleObjectNation(unsigned char cNation)
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CastleObjectMap::iterator end = m_CastleObjectMap.end();

	CSiegeObjectDB* lpCastleObject = NULL;
	while (itr != end)
	{
		lpCastleObject = itr->second;
		if (lpCastleObject)
		{
			lpCastleObject->SetNation(cNation);
		}

		++itr;
	}
}

void	CCastleDB::DestroyAllCastleArms()
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CastleObjectMap::iterator end = m_CastleObjectMap.end();

	CSiegeObjectDB* lpCastleObject = NULL;
	while (itr != end)
	{
		lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->IsCastleArms())
		{
			lpCastleObject->Destroy();	// 수성 병기들은 병기 관리 NPC 로 돌려놓는다.

			// 중계서버에서 파괴된 각자의 패킷을 보내지 않고,
			// 중계서버와 게임서버가 각자 처리하도록 해놓았음..
			//lpCastleObject->SendCastleCmd(0, PktCastleCmd::CASTLE_DESTROY_ARMS);
		}

		++itr;
	}
}

CSiegeObjectDB*	CCastleDB::GetCastleEmblem()
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CSiegeObjectDB* lpCastleObject = NULL;

	while (itr != m_CastleObjectMap.end())
	{
		lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->IsEmblem())
		{
			return lpCastleObject;
		}

		++itr;
	}

	return NULL;
}

CSiegeObjectDB* CCastleDB::GetCastleGate()
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CSiegeObjectDB* lpCastleObject = NULL;

	while (itr != m_CastleObjectMap.end())
	{
		lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->IsGate())
		{
			return lpCastleObject;
		}

		++itr;
	}

	return NULL;
}

CSiegeObjectDB*	CCastleDB::GetCastleObject(unsigned long dwCastleObjectID)
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.find(dwCastleObjectID);
	if (itr != m_CastleObjectMap.end()) return itr->second;
	
	return NULL;
}

void	CCastleDB::CheckEnableSiege(bool bIsSiegeTime)
{
	if (bIsSiegeTime == m_bEnableSiege)
	{
		return;
	}	

	m_bEnableSiege = false;
	if (bIsSiegeTime && 0 == m_cInvincibleCount)
	{
		m_bEnableSiege = true;
	}
}


void	CCastleDB::UpdateSiegeCount()
{
	SERVER_ID serverID;
	serverID.sID.Type       = CServerSetup::GameServer;
	serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
	serverID.sID.Channel    = m_cChannel;
	serverID.sID.ID         = m_cZone;

	GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
		DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

	if (m_bEnableSiege)
	{
		++m_wTotalSiegeCount;
	}

	if (m_cInvincibleCount)
	{
		--m_cInvincibleCount;
	}

	++m_cSiegeCount;
	if (m_cSiegeCount >= Castle::TEMP_TAX_GAIN_COUNT)
	{
		m_cSiegeCount = 0;
		unsigned long dwMoveTempMoney[ Castle::MAX_TAX_TYPE ] = { 0, };

		for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
		{
			dwMoveTempMoney[ i ] = m_CastleTax[ i ].m_dwTempTaxMoney;
			m_CastleTax[ i ].m_dwTaxMoney += m_CastleTax[ i ].m_dwTempTaxMoney;
			m_CastleTax[ i ].m_dwTempTaxMoney = 0;

			// DB 업데이트
			DBComponent::CastleDB::UpdateCastleTaxMoney(m_DBComponent, m_dwCastleID, i, m_CastleTax[ i ].m_dwTempTaxMoney, m_CastleTax[ i ].m_dwTaxMoney);
		}

		// 누적 세금 정보 전송
		if (NULL != lpGameDispatch)
		{
			CSendStream& SendStream = lpGameDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(sizeof(PktCastleTaxMove));
			if (NULL != lpBuffer)
			{
				PktCastleTaxMove* lpPktCastleTaxMove = reinterpret_cast<PktCastleTaxMove*>(lpBuffer);

				lpPktCastleTaxMove->m_dwCastleID = m_dwCastleID;
				for (int j=0; j<Castle::MAX_TAX_TYPE; ++j)
				{
					lpPktCastleTaxMove->m_dwMoveTempMoney[ j ] = dwMoveTempMoney[ j ];
					lpPktCastleTaxMove->m_dwResultTaxMoney[ j ] = m_CastleTax[ j ].m_dwTaxMoney;
				}

				SendStream.WrapHeader(sizeof(PktCastleTaxMove), CmdCastleTaxMove, 0, 0);
			}
		}
	}

	// DB 업데이트
	DBComponent::CastleDB::UpdateCastleSiegeCount(m_DBComponent, m_dwCastleID, m_cSiegeCount, m_cInvincibleCount, m_wTotalSiegeCount);

	// 공성 횟수 정보 전송
	if (NULL != lpGameDispatch)
	{
		CSendStream& SendStream = lpGameDispatch->GetSendStream();

		char* lpBuffer = SendStream.GetBuffer(sizeof(PktCastleSiegeCount));
		if (NULL != lpBuffer)
		{
			PktCastleSiegeCount* lpPktCastleSiegeCount = reinterpret_cast<PktCastleSiegeCount*>(lpBuffer);

			lpPktCastleSiegeCount->m_dwCastleID = m_dwCastleID;
			lpPktCastleSiegeCount->m_cSiegeCount = m_cSiegeCount;
			lpPktCastleSiegeCount->m_cInvincibleCount = m_cInvincibleCount;
			lpPktCastleSiegeCount->m_wTotalSiegeCount = m_wTotalSiegeCount;

			SendStream.WrapHeader(sizeof(PktCastleSiegeCount), CmdCastleSiegeCount, 0, 0);
		}
	}

	// 세율 조정 가능 변수 초기화
	for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
	{
		m_CastleTax[ i ].m_cTaxChangable = Castle::TAX_ENABLE;
				
		// DB 업데이트
		DBComponent::CastleDB::UpdateCastleTaxChangable(m_DBComponent, m_dwCastleID, i, Castle::TAX_ENABLE);
	}
}

// 거두어 들인 세금을 길드 금고로
void	CCastleDB::TakeTaxMoney(unsigned char cType, unsigned long dwTakeTaxMoney)
{
// CASTLE_TODO : 성이 길드 소유가 아니므로 일단 구현부분을 주석 처리 해둔다.
/*
	// edith 세금 부분 추가 (주석처리 뺐음)
	Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
        Guild::CGuildDBMgr::GetInstance().GetGuild(m_dwGID));

	if (lpGuild)
	{
		if (Castle::CAMP_MINERAL_TAX == cType)
		{
			m_dwTotalTaxMoney += dwTakeTaxMoney;

			// DB 업데이트
			DBComponent::CastleDB::UpdateCastleTotalTaxMoney(m_DBComponent, m_dwCastleID, m_dwTotalTaxMoney);
		}
		else
		{
			lpGuild->AddGold(dwTakeTaxMoney);
			m_CastleTax[ cType ].m_dwTaxMoney -= dwTakeTaxMoney;
			m_dwTotalTaxMoney += dwTakeTaxMoney;

			// DB 업데이트
			DBComponent::CastleDB::UpdateCastleTaxMoney(m_DBComponent, m_dwCastleID, cType,
				m_CastleTax[cType].m_dwTempTaxMoney, m_CastleTax[cType].m_dwTaxMoney);

			DBComponent::CastleDB::UpdateCastleTotalTaxMoney(m_DBComponent, m_dwCastleID, m_dwTotalTaxMoney);
		}
	}
*/
}

// 광물 세금 회수
bool	CCastleDB::GainMineral(unsigned short wMineralID, unsigned short wAmount)
{
	MineralInfoMap::iterator itr = m_AccumulatedMineralMap.find(wMineralID);

	if (itr != m_AccumulatedMineralMap.end())
	{
		if (itr->second < wAmount)
		{
			ERRLOG4(g_Log, "획득할려는 아이템 수보다 현재 누적된 아이템 수가 더 적습니다.( CastleID : 0x%08x, MineralID : %d, CurrentAmount : %d, GainAmount : %d )",
				m_dwCastleID, wMineralID, itr->second, wAmount);
			return false;
		}
		else
		{
			if (itr->second == wAmount)
			{
				DBComponent::CastleDB::DeleteCastleMineralInfo(m_DBComponent, m_dwCastleID, Siege::ACCUMULATED_MINERAL, wMineralID);
				m_AccumulatedMineralMap.erase(itr);
			}
			else
			{
				itr->second -= wAmount;
				DBComponent::CastleDB::UpdateCastleMineralInfo(m_DBComponent, m_dwCastleID, Siege::ACCUMULATED_MINERAL, wMineralID, itr->second);
			}

			return true;
		}
	}

	return false;
}

// 게임 서버로부터 임시 세금정보 업데이트
bool	CCastleDB::SetTempTaxMoney(unsigned long* dwTempTaxMoney, unsigned long* dwTaxMoney)
{
	if (NULL == dwTempTaxMoney || NULL == dwTaxMoney)
	{
		return false;
	}

	for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
	{
		// 누적 세금 정보가 같을 때만 업데이트한다.!!
		// (GS -> DBA : 임시 세금 정보 패킷전송, DBA -> GS : 임시 세금 -> 누적세금 패킷전송 간의 시간차로 인한 문제가 없게하기 위해)
		if (m_CastleTax[ i ].m_dwTaxMoney == dwTaxMoney[ i ])
		{
			m_CastleTax[ i ].m_dwTempTaxMoney = dwTempTaxMoney[ i ];

			// DB 업데이트
			DBComponent::CastleDB::UpdateCastleTaxMoney(m_DBComponent, m_dwCastleID, i,
				m_CastleTax[i].m_dwTempTaxMoney, m_CastleTax[i].m_dwTaxMoney);
		}
	}

	return true;
}

// 게임 서버로부터 광물 세금정보 업데이트
bool	CCastleDB::SetMineralInfo(unsigned char cMineralType, unsigned char cNum, CastleMineral* lpMineralInfo)
{
	switch (cMineralType)
	{
		case Siege::TEMPORARY_MINERAL :
		{
			DBComponent::CastleDB::DeleteCastleMineralInfo(m_DBComponent, m_dwCastleID, cMineralType);
			m_TemporaryMineralMap.clear();

			for (int i=0; i<cNum; ++i)
			{
				DBComponent::CastleDB::InsertCastleMineralInfo(m_DBComponent, m_dwCastleID, cMineralType,
					lpMineralInfo->m_wMineralID, lpMineralInfo->m_wAmount);

				InsertMineralInfo(cMineralType, *lpMineralInfo);
				++lpMineralInfo;
			}
		}
		break;

		case Siege::ACCUMULATED_MINERAL :
		{
			DBComponent::CastleDB::DeleteCastleMineralInfo(m_DBComponent, m_dwCastleID, cMineralType);
			m_AccumulatedMineralMap.clear();

			for (int i=0; i<cNum; ++i)
			{
				DBComponent::CastleDB::InsertCastleMineralInfo(m_DBComponent, m_dwCastleID, cMineralType,
					lpMineralInfo->m_wMineralID, lpMineralInfo->m_wAmount);

				InsertMineralInfo(cMineralType, *lpMineralInfo);
				++lpMineralInfo;
			}
		}
		break;

		default:	return false;
	}

	return true;
}


void	CCastleDB::SetTax(unsigned char cType, unsigned char cTax)	// 세율 변경
{
	if (cType < Castle::MAX_TAX_TYPE)
	{
		m_CastleTax[ cType ].m_cTax	= cTax;
		m_CastleTax[ cType ].m_cTaxChangable = Castle::TAX_DISABLE;

		DBComponent::CastleDB::UpdateCastleTax(m_DBComponent, m_dwCastleID, cType, cTax);
		DBComponent::CastleDB::UpdateCastleTaxChangable(m_DBComponent, m_dwCastleID, cType, Castle::TAX_DISABLE);
	}
}


bool	CCastleDB::SetTaxInfo(const CastleTaxInfoDB& taxInfo)
{
	if (taxInfo.m_cTaxType < Castle::MAX_TAX_TYPE)
	{
		m_CastleTax[ taxInfo.m_cTaxType ].m_cTaxType		= taxInfo.m_cTaxType;
		m_CastleTax[ taxInfo.m_cTaxType ].m_cTax			= taxInfo.m_cTax;				
		m_CastleTax[ taxInfo.m_cTaxType ].m_dwTempTaxMoney	= taxInfo.m_dwTempTaxMoney;	
		m_CastleTax[ taxInfo.m_cTaxType ].m_dwTaxMoney		= taxInfo.m_dwTaxMoney;
		m_CastleTax[ taxInfo.m_cTaxType ].m_cTaxChangable	= taxInfo.m_cTaxChangable;

		return true;
	}
	else
	{
		// TODO : Log
	}

	return false;
}

bool	CCastleDB::SetMineralInfo(const CastleMineralInfoDB& mineralInfo)
{
	if (mineralInfo.m_wAmount > 0)
	{
		switch (mineralInfo.m_cFlag)
		{
			case Siege::ACCUMULATED_MINERAL :
			{
				m_AccumulatedMineralMap.insert( std::make_pair(mineralInfo.m_wMineralID, mineralInfo.m_wAmount) ).second;
			}
			break;

			case Siege::TEMPORARY_MINERAL :
			{
				m_TemporaryMineralMap.insert( std::make_pair(mineralInfo.m_wMineralID, mineralInfo.m_wAmount) ).second;
			}
			break;

			default:	return false;
		}
	}

	return true;
}

bool	CCastleDB::InsertMineralInfo(unsigned char cMineralType, const CastleMineral& mineralInfo)
{
	if (mineralInfo.m_wAmount > 0)
	{
		switch (cMineralType)
		{
			case Siege::ACCUMULATED_MINERAL :
			{
				m_AccumulatedMineralMap.insert( std::make_pair(mineralInfo.m_wMineralID, mineralInfo.m_wAmount) ).second;
			}
			break;

			case Siege::TEMPORARY_MINERAL :
			{
				m_TemporaryMineralMap.insert( std::make_pair(mineralInfo.m_wMineralID, mineralInfo.m_wAmount) ).second;
			}
			break;

			default:	return false;
		}
	}

	return true;
}

void	CCastleDB::SetRight(CastleRight castleRight)	// 관리 권한 변경
{
	m_CastleRight = castleRight;

	DBComponent::CastleDB::UpdateCastleRight(m_DBComponent, m_dwCastleID, reinterpret_cast<char*>(&m_CastleRight), sizeof(CastleRight));
}

void	CCastleDB::SetUpgradeItemInfo(unsigned short wItemID, unsigned char cItemNum)
{
	m_wItemID = wItemID;
	m_cItemNum = cItemNum;

	DBComponent::CastleDB::UpdateCastleUpgradeItemInfo(m_DBComponent, m_dwCastleID, m_wItemID, m_cItemNum);
}
