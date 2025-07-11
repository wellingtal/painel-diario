#include "stdafx.h"
#include "Castle.h"
#include "CastleMgr.h"

#include <Creature/Siege/SiegeObject.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Item/ItemStructure.h>
#include <Item/ItemFactory.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

CCastle::CCastle(const CastleInfoDB& CastleInfo)
: m_dwCastleID(CastleInfo.m_dwCastleID), m_cNation(CastleInfo.m_cNation), m_cZone(CastleInfo.m_cZone),
  m_cChannel(CastleInfo.m_cChannel), m_cNameID(CastleInfo.m_cNameID), m_cSiegeCount(CastleInfo.m_cSiegeCount),
  m_cInvincibleCount(CastleInfo.m_cInvincibleCount), m_wTotalSiegeCount(CastleInfo.m_wTotalSiegeCount),
  m_dwTotalTaxMoney(CastleInfo.m_dwTotalTaxMoney), m_wItemID(CastleInfo.m_wItemID), m_cTotalItemNum(CastleInfo.m_cItemNum),
  m_fBonusRate(0), m_bEnableSiege(false), m_cTempTaxUpdateCount(0), m_dwTemporayTempTaxMoney(0)
{
	::memcpy(&m_CastleRight, CastleInfo.m_szRight, sizeof(CastleRight));

	m_BackDoorPos[0] = CastleInfo.m_InPos;
	m_BackDoorPos[1] = CastleInfo.m_OutPos;

	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);
	::memset(&m_cItemNum, 0, sizeof(unsigned char) * Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM);

	// Index 별 아이템 갯수 초기화
	if (0 != m_wItemID)
	{
		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo( m_wItemID );
		if (NULL != lpItemInfo)
		{
			unsigned char cItemNum = m_cTotalItemNum;
			for (int i = 0; i < Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM && cItemNum > 0; ++i)
			{
				if (cItemNum <= lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack)
				{
					m_cItemNum[ i ] = cItemNum;
					cItemNum = 0;
				}
				else
				{
					m_cItemNum[ i ] = lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack;
					cItemNum -= lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack;
				}
			}
		}
	}
}

CCastle::~CCastle()
{
}

bool	CCastle::SetTaxInfo(const CastleTaxInfo& castleTaxInfo)
{
	if (castleTaxInfo.m_cTaxType < Castle::MAX_TAX_TYPE)
	{
		m_CastleTax[ castleTaxInfo.m_cTaxType ] = castleTaxInfo;
		return true;
	}
	
	return false;
}

bool	CCastle::SetMineralInfo(unsigned short wMineralID, unsigned short wAmount, unsigned char cFlag)
{
	if (wAmount > 0)
	{
		switch (cFlag)
		{
			case Castle::ACCUMULATED_MINERAL:
			{
				m_AccumulatedMineralMap.insert( std::make_pair(wMineralID, wAmount) ).second;
			}
			break;

			case Castle::TEMPORARY_MINERAL:
			{
				m_TemporaryMineralMap.insert( std::make_pair(wMineralID, wAmount) ).second;
			}
			break;

			default:	return false;
		}
	}

	return true;
}

void	CCastle::UpdateTaxMoveInfo(unsigned long* dwMoveTempMoney, unsigned long* dwResultTaxMoney)
{
	PktCastleTaxMove pktCTM;
	pktCTM.m_dwCastleID = m_dwCastleID;

	for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
	{
		m_CastleTax[ i ].m_dwTempTaxMoney -= dwMoveTempMoney[ i ];
		m_CastleTax[ i ].m_dwTaxMoney = dwResultTaxMoney[ i ];
		
		pktCTM.m_dwMoveTempMoney[ i ] = dwMoveTempMoney[ i ];	
		pktCTM.m_dwResultTaxMoney[ i ] = dwResultTaxMoney[ i ];
	}

// CASTLE_TODO : 성이 길드 소유가 아니므로 패킷을 보낼수 없다.

	// 클라이언트에 패킷 전송 (길드원에게만 전송한다.)
//	char* szPacket = reinterpret_cast<char *>(&pktCTM);
//	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove, 0, 0))
//	{
//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//		if (lpGuild)
//		{
//			lpGuild->SendAllMember(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove);
//		}
//	}

	// 임시 광물 세금을 누적 광물 세금으로 옮기고 DB 중계 서버로 보내준다.
	MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
	MineralInfoMap::iterator end = m_TemporaryMineralMap.end();

	while (itr != end)
	{
		AddMineral(Siege::ACCUMULATED_MINERAL, itr->first, itr->second);
		++itr;
	}

	m_TemporaryMineralMap.clear();

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (0 == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "에이전트 얻기 실패.");	
	}
	else
	{
		// DB 중계 서버로 임시 광물 세금 및 누적 광물 세금 리스트 보내기
		SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::TEMPORARY_MINERAL);
		SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::ACCUMULATED_MINERAL);
	}
}

void	CCastle::UpdateSiegeCountInfo(unsigned char cSiegeCount, unsigned char cInvincibleCount, unsigned short wTotalSiegeCount)
{
	m_cSiegeCount = cSiegeCount;
	m_cInvincibleCount = cInvincibleCount;
	m_wTotalSiegeCount = wTotalSiegeCount;

	PktCastleSiegeCount pktCSC;
	pktCSC.m_dwCastleID = m_dwCastleID;
	pktCSC.m_cSiegeCount = m_cSiegeCount;
	pktCSC.m_cInvincibleCount = m_cInvincibleCount;
	pktCSC.m_wTotalSiegeCount = m_wTotalSiegeCount;

// CASTLE_TODO : 성이 길드 소유가 아니므로 패킷을 보낼수 없다.

	// 클라이언트에 패킷 전송 (길드원에게만 전송한다.)
//	char* szPacket = reinterpret_cast<char *>(&pktCSC);
//	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleSiegeCount), CmdCastleSiegeCount, 0, 0))
//	{
//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//		if (lpGuild)
//		{
//			lpGuild->SendAllMember(szPacket, sizeof(PktCastleSiegeCount), CmdCastleSiegeCount);
//		}
//	}
}


void	CCastle::AllRespawn(unsigned char cExceptNation)
{
	CSiegeObject* lpCastleEmblem = GetCastleEmblem();
	if (lpCastleEmblem)
	{
		CCell* lpCenterCell = lpCastleEmblem->GetCellPos().m_lpCell;
		if (lpCenterCell)
		{
			int nCellWidth = Siege::BROADCAST_CELL_SIZE * 2 - 1;
			int nCellHeight = Siege::BROADCAST_CELL_SIZE * 2 - 1;
			int i, j;

			CCell* lpStartCell = NULL;
			for (i=1; i<Siege::BROADCAST_CELL_SIZE; ++i)
			{
				lpStartCell = lpCenterCell->GetConnectCell(CCell::LEFT);
				if (NULL == lpStartCell)
				{
					nCellWidth = nCellWidth - (Siege::BROADCAST_CELL_SIZE - i);
					break;
				}

				lpCenterCell = lpStartCell;
			}

			for (i=1; i<Siege::BROADCAST_CELL_SIZE; ++i)
			{
				lpStartCell = lpCenterCell->GetConnectCell(CCell::UP);
				if (NULL == lpStartCell)
				{
					nCellHeight = nCellHeight - (Siege::BROADCAST_CELL_SIZE - i);
					break;
				}

				lpCenterCell = lpStartCell;
			}

			lpStartCell = lpCenterCell;

			for (i=0; i<nCellHeight; ++i)
			{
				CCell* lpTempCell = lpStartCell;
				for (j=0; j<nCellWidth; ++j)
				{
					if (lpTempCell)
					{
						lpTempCell->RespawnAllCharacter(cExceptNation);
						lpTempCell = lpTempCell->GetConnectCell(CCell::RIGHT);
					}
					else break;
				}

				lpStartCell = lpStartCell->GetConnectCell(CCell::DOWN);
				if (NULL == lpStartCell) break;
			}
		}
	}
}

bool	CCastle::ChangeCastleMaster(unsigned char cNation)
{
	m_cNation = cNation;
	m_cSiegeCount = 0;
	m_cInvincibleCount = Castle::INVINCIBLE_COUNT + 1;
	m_wTotalSiegeCount = 0;
	m_dwTotalTaxMoney = 0;
	m_wItemID = 0;
	m_cTotalItemNum = 0;
	::memset(&m_cItemNum, 0, sizeof(unsigned char) * Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM);
	m_bEnableSiege = false;

	m_CastleRight = CastleRight();
	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);
	m_AccumulatedMineralMap.clear();
	m_TemporaryMineralMap.clear();

	// 성 관련 오브젝트 소유 국가 변경
	ChangeCastleObjectNation(cNation);

	// 수성 병기를 병기 관리 NPC 로 돌려놓는다.
	// 탑승자는 사망처리 시킨다.
	DestroyAllCastleArms(true);
	
// CASTLE_TODO : 캐릭터 리스폰 기능은 빠지므로 주석처리
//	// 해당 국가원을 제외한 모든 캐릭터를 리스폰
//	AllRespawn(cNation);

	// 해당성의 수성 병기의 주인이 변경되었음을 반경 5셀의 캐릭터들에게 알린다.
	// 새로 점령한 길드의 길드원에게만 알려도 될듯한데..
	// (이미 해당 길드원 이외의 캐릭터는 리스폰 되기때문...)
	CSiegeObjectMgr::GetInstance().SendChangeMaster(m_dwCastleID, cNation);
	
	// 존의 모든 캐릭터에게 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= m_dwCastleID;
	pktCC.m_dwCastleObjectID	= 0;
	pktCC.m_cState				= 0;
	pktCC.m_dwValue1			= cNation;	// 새로운 성 소유 국가
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_CHANGE_MASTER;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}

	return true;
}

void	CCastle::LoseOwnership()
{
	m_cNation = 0;
	m_cSiegeCount = 0;
	m_cInvincibleCount = 0;
	m_wTotalSiegeCount = 0;
	m_dwTotalTaxMoney = 0;
	m_wItemID = 0;
	m_cTotalItemNum = 0;
	::memset(&m_cItemNum, 0, sizeof(unsigned char) * Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM);
	m_bEnableSiege = true;

	m_CastleRight = CastleRight();
	::memset(&m_CastleTax, 0, sizeof(CastleTaxInfo) * Castle::MAX_TAX_TYPE);
	m_AccumulatedMineralMap.clear();
	m_TemporaryMineralMap.clear();

	// 성 관련 오브젝트 소유 국가 변경
	ChangeCastleObjectNation(Creature::STATELESS);

	// 수성 병기를 병기 관리 NPC 로 돌려놓는다.
	// 탑승자는 사망처리 시킨다.
	DestroyAllCastleArms(true);

	// 해당성의 수성 병기의 주인이 변경되었음을 반경 5셀의 캐릭터들에게 알린다.
	// 새로 점령한 길드의 길드원에게만 알려도 될듯한데..
	// (이미 해당 길드원 이외의 캐릭터는 리스폰 되기때문...)
	CSiegeObjectMgr::GetInstance().SendLoseOwnership(m_dwCastleID);

	// 존의 모든 캐릭터에게 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= m_dwCastleID;
	pktCC.m_dwCastleObjectID	= 0;
	pktCC.m_cState				= 0;
	pktCC.m_dwValue1			= 0;
	pktCC.m_dwValue2			= 0;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_LOSE_OWNERSHIP;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
	}
}

void	CCastle::ChangeCastleObjectNation(unsigned char cNation)
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CastleObjectMap::iterator end = m_CastleObjectMap.end();

	CSiegeObject* lpCastleObject = NULL;
	while (itr != end)
	{
		lpCastleObject = itr->second;
		if (lpCastleObject)
		{
			// 해당 성의 수성 병기의 소유 국가 변경
			lpCastleObject->SetNation(cNation);

			if (lpCastleObject->IsEmblem())
			{
				lpCastleObject->SetState(Siege::COMPLETE);
			}
		}

		++itr;
	}
}

void	CCastle::DestroyAllCastleArms(bool bKillRider)
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
	CastleObjectMap::iterator end = m_CastleObjectMap.end();

	CSiegeObject* lpCastleObject = NULL;
	while (itr != end)
	{
		lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->IsCastleArms())
		{
			// 해당 성의 수성 병기에 타고 있던 모든 캐릭터들을 내리게 한다.
			if (lpCastleObject->IsRidable())
			{
				unsigned long dwRiderCID = lpCastleObject->GetRiderCID();
				lpCastleObject->GetOff(dwRiderCID);

				if (bKillRider)
				{
					CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(dwRiderCID);
					if (0 != lpRider)
					{
						lpRider->Kill(NULL);
					}
				}
			}

			// 해당 성의 수성 병기를 병기 관리 NPC로 돌려놓는다.
			lpCastleObject->Destroy(0, false);
		}

		++itr;
	}
}

// 세금 회수
void	CCastle::TakeTaxMoney(unsigned char cType, unsigned long dwTaxMoney)
{
	if (Castle::CAMP_MINERAL_TAX == cType)
	{
		m_dwTotalTaxMoney += dwTaxMoney;
	}
	else
	{
		m_CastleTax[cType].m_dwTaxMoney -= dwTaxMoney;
		m_dwTotalTaxMoney += dwTaxMoney;
	}

/*
	// edith 세금 부분 추가 (주석처리 뺐음)
	// CASTLE_TOOD : 성이 길드 소유가 아니므로 패킷을 보낼수 없다.

	// 해당 길드원에게 전송
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= m_dwCastleID;
	pktCC.m_dwCastleObjectID	= 0;
	pktCC.m_cState				= 0;
	pktCC.m_dwValue1			= cType;
	pktCC.m_dwValue2			= dwTaxMoney;
	pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_TAKE_TAXMONEY;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
	{
		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
		if (lpGuild)
		{
			lpGuild->SendAllMember(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		}
	}
	//
*/
}

// 광물 세금 회수
bool	CCastle::GainMineral(unsigned short wMineralID, unsigned short wAmount)
{
/*
	// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.
	// edith 세금 부분 추가 (주석처리 뺐음)

	unsigned short wError = PktCastleCmd::NO_SERVER_ERR;
	MineralInfoMap::iterator itr = m_AccumulatedMineralMap.find(wMineralID);

	if ( itr != m_AccumulatedMineralMap.end() )
	{
		if ( itr->second >= wAmount )
		{
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( m_dwGID );
			if ( lpGuild )
			{
				CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter( lpGuild->GetMaster().m_dwCID );
				if ( lpMaster )
				{
					unsigned short tempAmount = wAmount;
					
					while (tempAmount != 0)
					{
						Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wMineralID);
						if (NULL == lpItem)
						{
							ERRLOG2(g_Log, "CastleID:0x%08x 아이템 스크립트에 존재하지 않는 광물을 생성하려 하였습니다. ItemID:%d", m_dwCastleID, wMineralID);
							wError = PktCastleCmd::SERVER_ERROR;
							break;
						}

						if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
						{
							if (lpItem->GetMaxNumOrDurability() > tempAmount)
							{
								lpItem->SetNumOrDurability( static_cast<unsigned char>(tempAmount) );
								tempAmount = 0;
							}
							else
							{
								lpItem->SetNumOrDurability( lpItem->GetMaxNumOrDurability() );
								tempAmount -= lpItem->GetMaxNumOrDurability();
							}
						}
						else
						{
							--tempAmount;
						}

						if (false == lpMaster->GiveItem(lpItem))
						{
							ERRLOG1(g_Log, "CID:0x%08x 누적 광물 아이템을 받는데 실패하였습니다.", lpGuild->GetMaster().m_dwCID);
							DELETE_ITEM(lpItem);
							wError = PktCastleCmd::SERVER_ERROR;
							break;
						}

						// GievItem 으로 스택된 경우
						if (NULL != lpItem)
						{
							if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
							{
								DELETE_ITEM(lpItem);
							}
						}
					}

					if (wError == PktCastleCmd::NO_SERVER_ERR)
					{
						itr->second -= wAmount;
						wAmount = itr->second;	// 남은 양

						if (0 == itr->second)
						{
							m_AccumulatedMineralMap.erase( itr );
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "CID : 0x%08x 길드 마스터가 존재하지 않습니다.", lpGuild->GetMaster().m_dwCID);
					wError = PktCastleCmd::SERVER_ERROR;
				}
			}
			else
			{
				ERRLOG1(g_Log, "GID:0x%08x 해당 길드가 존재하지 않습니다.", m_dwGID);
				wError = PktCastleCmd::SERVER_ERROR;
			}
		}
		else
		{
			ERRLOG4(g_Log, "지금 누적된 광물수보다 많은 양의 광물을 획득하려 합니다. CastleID:0x%08x, MineralID:%d, CurrentAmount:%d, GainAmount:%d",
					m_dwCastleID, wMineralID, itr->second, wAmount);
			wError = PktCastleCmd::SERVER_ERROR;
		}
	}

	// 길드 마스터에게 전송
	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
	if (NULL != lpGuild)
	{
		CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter(lpGuild->GetMaster().m_dwCID);
		if (NULL != lpMaster)
		{
			CGameClientDispatch* lpDispatch = lpMaster->GetDispatcher();
			if (NULL != lpDispatch)
			{
				return GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), m_dwCastleID, 0,
					wMineralID, wAmount, PktCastleCmd::CASTLE_GAIN_MINERAL, wError);
			}
		}
	}
	///
*/
	return true;
}

// 업그레이드 보석 아이템 넣기
void	CCastle::AddUpgradeItem(unsigned short wItemID, int iIndex, unsigned char cNum)
{
	if (0 == m_cTotalItemNum)
	{
		m_wItemID = wItemID;
	}

	m_cTotalItemNum += cNum;
	m_cItemNum[ iIndex ] += cNum;
}

// 업그레이드 보석 아이템 빼기
void	CCastle::DeleteUpgradeItem(int iIndex, unsigned char cNum)
{
	m_cTotalItemNum -= cNum;
	m_cItemNum[ iIndex ] -= cNum;

	if (0 == m_cTotalItemNum)
	{
		m_wItemID = 0;
	}
}

// 업그레이드 보석 아이템 초기화
void	CCastle::InitUpgradeItem()
{
	m_wItemID = 0;
	m_cTotalItemNum = 0;
	::memset(&m_cItemNum, 0, sizeof(unsigned char) * Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM);
}

// 임시 세금 축적
void	CCastle::AddTempTaxMoney(unsigned char cType, unsigned long dwMoney)
{
	// 특정 회수 이상 업데이트 되면 DB 중계 서버로 전송해준다.
	++m_cTempTaxUpdateCount;
	m_dwTemporayTempTaxMoney += dwMoney;
	m_CastleTax[ cType ].m_dwTempTaxMoney += dwMoney;

	if (m_cTempTaxUpdateCount >= Castle::MAX_TEMP_TAX_UPDATE_COUNT || m_dwTemporayTempTaxMoney >= Castle::MAX_TEMP_TAX_AMOUNT)
	{
		m_cTempTaxUpdateCount = 0;
		m_dwTemporayTempTaxMoney = 0;

		// 중계 서버로 임시 세금 정보 업데이트 ( fBonusRate 도 적용해준다. )
		SendTempTaxInfo();
	}
}


// 광물 세금 축척
bool	CCastle::AddMineral(unsigned char cFlag, unsigned short wMineralID, unsigned short wAmount)
{
	switch (cFlag)
	{
		case Siege::ACCUMULATED_MINERAL:
		{
			MineralInfoMap::iterator pos = m_AccumulatedMineralMap.find( wMineralID );
			if ( pos != m_AccumulatedMineralMap.end() )
			{
				pos->second += wAmount;
				if ( pos->second > Siege::MAX_MINERAL_NUM )
				{
					pos->second = Siege::MAX_MINERAL_NUM;
				}
			}
			else if (wAmount > 0)
			{
				m_AccumulatedMineralMap.insert( std::make_pair(wMineralID, wAmount) ).second;
			}
		}
		break;

		case Siege::TEMPORARY_MINERAL:
		{
			MineralInfoMap::iterator pos = m_TemporaryMineralMap.find( wMineralID );
			if ( pos != m_TemporaryMineralMap.end() )
			{
				pos->second += wAmount;
				if ( pos->second > Siege::MAX_MINERAL_NUM )
				{
					pos->second = Siege::MAX_MINERAL_NUM;
				}
			}
			else if (wAmount > 0)
			{
				m_TemporaryMineralMap.insert( std::make_pair(wMineralID, wAmount) ).second;
			}
		}
		break;

		default:	return false;
	}

	return true;
}

// 중계서버로 임시 세금 정보 업데이트
void	CCastle::SendTempTaxInfo()
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (NULL == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "중계 서버로 임시 세금 정보 업데이트 실패!! : 에이전트 얻기 실패.");
	}
	else
	{
		PktCastleTaxMove pktCTM;
		pktCTM.m_dwCastleID = m_dwCastleID;

		for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
		{
			m_CastleTax[ i ].m_dwTempTaxMoney += static_cast<unsigned long>(m_CastleTax[ i ].m_dwTempTaxMoney * m_fBonusRate);
			pktCTM.m_dwMoveTempMoney[ i ] = m_CastleTax[ i ].m_dwTempTaxMoney;
			pktCTM.m_dwResultTaxMoney[ i ] = m_CastleTax[ i ].m_dwTaxMoney;
		}

		char* szPacket = reinterpret_cast<char *>(&pktCTM);
		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove, 0, 0))
		{
			lpDBAgentDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove);
		}
	}
}

// 광물 정보 전송
bool	CCastle::SendMineralInfo(CSendStream& SendStream, unsigned char cMineralType)
{
	unsigned short wSize = 0;
	char szBuffer[1024];
	PktCastleMineralInfo*	lpPktMineralInfo = reinterpret_cast<PktCastleMineralInfo*>(szBuffer);
	CastleMineral*			lpCastleMineral = reinterpret_cast<CastleMineral*>(lpPktMineralInfo + 1);

	lpPktMineralInfo->m_dwCastleID		= m_dwCastleID;
	lpPktMineralInfo->m_cMineralType	= cMineralType;

	switch ( cMineralType )
	{
		case Siege::ACCUMULATED_MINERAL:
		{
			lpPktMineralInfo->m_cNum = static_cast<unsigned char>( m_AccumulatedMineralMap.size() );

			MineralInfoMap::iterator itr = m_AccumulatedMineralMap.begin();
			MineralInfoMap::iterator end = m_AccumulatedMineralMap.end();

			while (itr != end)
			{
				lpCastleMineral->m_wMineralID	= itr->first;
				lpCastleMineral->m_wAmount		= itr->second;

				++itr;
				++lpCastleMineral;

				wSize += sizeof(CampMineralInfo);
			}

			lpPktMineralInfo->m_wSize = wSize;
		}
		break;

	case Siege::TEMPORARY_MINERAL:
		{
			lpPktMineralInfo->m_cNum = static_cast<unsigned char>( m_TemporaryMineralMap.size() );

			MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
			MineralInfoMap::iterator end = m_TemporaryMineralMap.end();

			while (itr != end)
			{
				lpCastleMineral->m_wMineralID	= itr->first;
				lpCastleMineral->m_wAmount		= itr->second;

				++itr;
				++lpCastleMineral;

				wSize += sizeof(CampMineralInfo);
			}

			lpPktMineralInfo->m_wSize = wSize;
		}
		break;
	}

	return SendStream.WrapCompress(szBuffer, static_cast<unsigned short>(sizeof(PktCastleMineralInfo) + wSize), CmdCastleMineralInfo, 0, 0);
}

bool	CCastle::InsertCastleObject(CSiegeObject* lpCastleObject)
{
	if (lpCastleObject)
	{
		return m_CastleObjectMap.insert(std::make_pair(lpCastleObject->GetCID(), lpCastleObject)).second;
	}

	return false;
}


bool	CCastle::DeleteCastleObject(unsigned long dwCID)
{
	return true;
}

bool	CCastle::DeleteCastleObject(CSiegeObject* lpSiegeObject)
{
	return true;
}

CSiegeObject*	CCastle::GetCastleEmblem() const
{
	CastleObjectMap::const_iterator itr = m_CastleObjectMap.begin();
	CSiegeObject* lpCastleObject = NULL;

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

bool	CCastle::HasCastleArms(unsigned long dwCID)
{
	CastleObjectMap::const_iterator itr = m_CastleObjectMap.begin();
	
	CSiegeObject* lpCastleObject = NULL;

	while (itr != m_CastleObjectMap.end())
	{
		lpCastleObject = itr->second;

		if (lpCastleObject && lpCastleObject->IsCastleArms())
		{
			if(lpCastleObject->GetRiderCID()==dwCID || lpCastleObject->GetOwnerID()==dwCID)
			{
				return true;
			}
		}

		++itr;
	}

	return false;
}

CSiegeObject*	CCastle::GetCastleObject(unsigned long dwCastleObjectID)
{
	CastleObjectMap::iterator itr = m_CastleObjectMap.find(dwCastleObjectID);
	if (itr != m_CastleObjectMap.end()) return itr->second;

	return NULL;
}

void	CCastle::UpgradeByEmblem()
{
	CSiegeObject* lpEmblem = GetCastleEmblem();
	if (lpEmblem && lpEmblem->GetUpgradeStep() > 0)
	{
		unsigned char cUpgradeType = lpEmblem->GetUpgradeType();
		unsigned char cUpgradeStep = lpEmblem->GetUpgradeStep();

		CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
		while (itr != m_CastleObjectMap.end())
		{
			CSiegeObject* lpCastleObject = itr->second;
			if (lpCastleObject && !lpCastleObject->IsEmblem())
			{
				lpCastleObject->UpgradeByEmblem(cUpgradeType, cUpgradeStep);
			}

			++itr;
		}

		SetBonusRate(lpEmblem->GetBonusRate());

// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.

//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//		if (lpGuild)
//		{
//			lpGuild->UpgradeMemberRespawnSpeedByEmblem(lpEmblem->GetUpgradeType(), lpEmblem->GetUpgradeStep());
//		}

		CCell* lpCenterCell = lpEmblem->GetCellPos().m_lpCell;
		if (lpCenterCell && cUpgradeType == Siege::DIAMOND)
		{
			switch (cUpgradeStep)
			{
			case 1:	// 1단계 업그레이드
				{
					for (int i=CCell::NO; i<CCell::CONNECT_NUM; ++i)
					{
						CCell* lpCell = lpCenterCell->GetConnectCell(i);
						if (lpCell)
						{
							lpCell->UpgradeByEmblem(lpEmblem->GetCID());
						}
					}
				}
				break;

			case 2: // 2단계 업그레이드
				{
					int i, j, nHeight = 5, nWidth = 5;
					CCell* lpStartCell = NULL;
					CCell* lpCell = lpCenterCell->GetConnectCell(CCell::UP);
					if (lpCell)
					{
						lpStartCell = lpCell;
						lpCell = lpCell->GetConnectCell(CCell::UP);
						if (lpCell)
						{
							lpStartCell = lpCell;
						}
						else
						{
							--nHeight;
						}
					}
					else
					{
						nHeight = 3;
					}

					lpCell = lpStartCell->GetConnectCell(CCell::LEFT);
					if (lpCell)
					{
						lpStartCell = lpCell;
						lpCell = lpCell->GetConnectCell(CCell::LEFT);
						if (lpCell)
						{
							lpStartCell = lpCell;
						}
						else
						{
							--nWidth;
						}
					}
					else
					{
						nWidth = 3;
					}

					lpCell = lpStartCell;
					for (i=0; i<nHeight; ++i)
					{
						lpStartCell = lpCell;
						lpCell->UpgradeByEmblem(lpEmblem->GetCID());

						for (j=1; j<nWidth; ++j)
						{
							lpCell = lpCell->GetConnectCell(CCell::RIGHT);
							if (lpCell)
							{
								lpCell->UpgradeByEmblem(lpEmblem->GetCID());
							}
						}

						do
						{
							lpCell = lpStartCell->GetConnectCell(CCell::DOWN);
							if (NULL == lpCell)
							{
								++i;
							}

						} while (NULL == lpCell && i < nHeight);
					}
				}
				break;
			}
		}
	}
}

void	CCastle::DegradeByEmblem()
{
	CSiegeObject* lpEmblem = GetCastleEmblem();
	if (lpEmblem && lpEmblem->GetUpgradeStep() > 0)
	{
		unsigned char cUpgradeType = lpEmblem->GetUpgradeType();
		unsigned char cUpgradeStep = lpEmblem->GetUpgradeStep();

		CastleObjectMap::iterator itr = m_CastleObjectMap.begin();
		while (itr != m_CastleObjectMap.end())
		{
			CSiegeObject* lpCastleObject = itr->second;
			if (lpCastleObject && !lpCastleObject->IsEmblem())
			{
				lpCastleObject->DegradeByEmblem(cUpgradeType, cUpgradeStep);
			}

			++itr;
		}

		SetBonusRate(0);

// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.

//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//		if (lpGuild)
//		{
//			lpGuild->DegradeMemberRespawnSpeedByEmblem();
//		}

		CCell* lpCenterCell = lpEmblem->GetCellPos().m_lpCell;
		if (lpCenterCell)
		{
			int i, j, nHeight = 5, nWidth = 5;
			CCell* lpStartCell = NULL;
			CCell* lpCell = lpCenterCell->GetConnectCell(CCell::UP);
			if (lpCell)
			{
				lpStartCell = lpCell;
				lpCell = lpCell->GetConnectCell(CCell::UP);
				if (lpCell)
				{
					lpStartCell = lpCell;
				}
				else
				{
					--nHeight;
				}
			}
			else
			{
				nHeight = 3;
			}

			lpCell = lpStartCell->GetConnectCell(CCell::LEFT);
			if (lpCell)
			{
				lpStartCell = lpCell;
				lpCell = lpCell->GetConnectCell(CCell::LEFT);
				if (lpCell)
				{
					lpStartCell = lpCell;
				}
				else
				{
					--nWidth;
				}
			}
			else
			{
				nWidth = 3;
			}

			lpCell = lpStartCell;
			for (i=0; i<nHeight; ++i)
			{
				lpStartCell = lpCell;
				lpCell->DegradeByEmblem();

				for (j=1; j<nWidth; ++j)
				{
					lpCell = lpCell->GetConnectCell(CCell::RIGHT);
					if (lpCell)
					{
						lpCell->DegradeByEmblem();
					}
				}

				do
				{
					lpCell = lpStartCell->GetConnectCell(CCell::DOWN);
					if (NULL == lpCell)
					{
						++i;
					}

				} while (NULL == lpCell && i < nHeight);
			}
		}
	}
}


void	CCastle::Process()
{
}

bool	CCastle::CheckRight(unsigned char cRightType, unsigned long dwCID, unsigned long dwGID)
{
// CASTLE_TODO : 성이 길드 소유가 아니므로 권한 체크를 할수 없다.

//	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//	if (lpGuild)
//	{
//		if (m_CastleRight.Check(cRightType, lpGuild->GetTitle(dwCID)))
//		{
//			return true;
//		}
//	}
	
	return false;
}

void	CCastle::SetEnableSiege(bool bEnable)
{
	m_bEnableSiege = bEnable;
}

void	CCastle::SetTax(unsigned char cType, unsigned char cTax)
{
	if (cType < Castle::MAX_TAX_TYPE)
	{
		m_CastleTax[cType].m_cTax = cTax;
		m_CastleTax[cType].m_cTaxChangable = Castle::TAX_DISABLE;

		// 모든 캐릭터에게 전송!! (세율 정보는 모든 캐릭터가 알아야한다.!!)
		PktCastleCmd pktCC;
		pktCC.m_dwCID				= 0;
		pktCC.m_dwCastleObjectID	= 0;
		pktCC.m_dwCastleID			= m_dwCastleID;
		pktCC.m_cState				= 0;
		pktCC.m_dwValue1			= cType;		// 세율 타입
		pktCC.m_dwValue2			= cTax;			// 세율
		pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_SET_TAX;

		char* szPacket = reinterpret_cast<char *>(&pktCC);
		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
		{
			CCreatureManager::GetInstance().SendAllCharacter(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
		}
	}
}

void	CCastle::SetRight(CastleRight castleRight)
{
	m_CastleRight = castleRight;

// CASTLE_TODO : 성이 길드 소유가 아니므로 권한 설정을 할수 없다.

	// 해당 길드의 길드원들에게 전송
//	Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
//	if (lpGuild)
//	{
//		PktCastleRight pktCR;
//		pktCR.m_dwCID			= 0;
//		pktCR.m_dwCastleID		= m_dwCastleID;
//		pktCR.m_CastleRight		= m_CastleRight;
//
//		char* szPacket = reinterpret_cast<char *>(&pktCR);
//		if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleRight), CmdCastleRight, 0, 0))
//		{
//			lpGuild->SendAllMember(szPacket, sizeof(PktCastleRight), CmdCastleRight);
//		}
//	}
}

bool	CCastle::IsCastleOfNation(unsigned char cNation)
{
	return (cNation == m_cNation);
}

bool	CCastle::IsTaxChangable(unsigned char cType)
{
	if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
	{
		return false;
	}

	if (cType >= Castle::MAX_TAX_TYPE || Castle::TAX_DISABLE == m_CastleTax[cType].m_cTaxChangable)
	{
		return false;
	}

	return true;
}

void	CCastle::EnableTaxChange()
{
	for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
	{
		m_CastleTax[ i ].m_cTaxChangable = Castle::TAX_ENABLE;
	}
}

bool	CCastle::HasAnotherCastleArms(unsigned long dwCID) const
{
	CastleObjectMap::const_iterator itr = m_CastleObjectMap.begin();
	CastleObjectMap::const_iterator end = m_CastleObjectMap.end();

	while (itr != end)
	{
		const CSiegeObject* lpCastleObject = itr->second;
		if (0 != lpCastleObject && lpCastleObject->IsCastleArms())
		{
			if (lpCastleObject->GetOwnerID() == dwCID)
			{
				return true;
			}
		}

		++itr;
	}

	return false;
}

unsigned char	CCastle::GetUpgradeStep() const
{
	const CSiegeObject* lpEmblem = GetCastleEmblem();
	if (NULL != lpEmblem)
	{
		return lpEmblem->GetUpgradeStep();
	}

	return 0;
}

unsigned short	CCastle::GetMineralNum(unsigned char cMineralType, unsigned short wMineralID) const
{
	switch (cMineralType)
	{
		case Siege::ACCUMULATED_MINERAL:
		{
			MineralInfoMap::const_iterator itr = m_AccumulatedMineralMap.find(wMineralID);
			if (itr != m_AccumulatedMineralMap.end())
			{
				return itr->second;
			}
		}
		break;

		case Siege::TEMPORARY_MINERAL:
		{
			MineralInfoMap::const_iterator itr = m_TemporaryMineralMap.find(wMineralID);
			if (itr != m_TemporaryMineralMap.end())
			{
				return itr->second;
			}
		}
		break;
	}

	return 0;
}