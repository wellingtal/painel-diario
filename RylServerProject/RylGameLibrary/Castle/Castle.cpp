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

	// Index �� ������ ���� �ʱ�ȭ
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

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ��Ŷ�� ������ ����.

	// Ŭ���̾�Ʈ�� ��Ŷ ���� (�������Ը� �����Ѵ�.)
//	char* szPacket = reinterpret_cast<char *>(&pktCTM);
//	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove, 0, 0))
//	{
//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(m_dwGID);
//		if (lpGuild)
//		{
//			lpGuild->SendAllMember(szPacket, sizeof(PktCastleTaxMove), CmdCastleTaxMove);
//		}
//	}

	// �ӽ� ���� ������ ���� ���� �������� �ű�� DB �߰� ������ �����ش�.
	MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
	MineralInfoMap::iterator end = m_TemporaryMineralMap.end();

	while (itr != end)
	{
		AddMineral(Siege::ACCUMULATED_MINERAL, itr->first, itr->second);
		++itr;
	}

	m_TemporaryMineralMap.clear();

	// �߰� ������ ��Ŷ ����
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (0 == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "������Ʈ ��� ����.");	
	}
	else
	{
		// DB �߰� ������ �ӽ� ���� ���� �� ���� ���� ���� ����Ʈ ������
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

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ��Ŷ�� ������ ����.

	// Ŭ���̾�Ʈ�� ��Ŷ ���� (�������Ը� �����Ѵ�.)
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

	// �� ���� ������Ʈ ���� ���� ����
	ChangeCastleObjectNation(cNation);

	// ���� ���⸦ ���� ���� NPC �� �������´�.
	// ž���ڴ� ���ó�� ��Ų��.
	DestroyAllCastleArms(true);
	
// CASTLE_TODO : ĳ���� ������ ����� �����Ƿ� �ּ�ó��
//	// �ش� �������� ������ ��� ĳ���͸� ������
//	AllRespawn(cNation);

	// �ش缺�� ���� ������ ������ ����Ǿ����� �ݰ� 5���� ĳ���͵鿡�� �˸���.
	// ���� ������ ����� �������Ը� �˷��� �ɵ��ѵ�..
	// (�̹� �ش� ���� �̿��� ĳ���ʹ� ������ �Ǳ⶧��...)
	CSiegeObjectMgr::GetInstance().SendChangeMaster(m_dwCastleID, cNation);
	
	// ���� ��� ĳ���Ϳ��� ����
	PktCastleCmd pktCC;
	pktCC.m_dwCastleID			= m_dwCastleID;
	pktCC.m_dwCastleObjectID	= 0;
	pktCC.m_cState				= 0;
	pktCC.m_dwValue1			= cNation;	// ���ο� �� ���� ����
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

	// �� ���� ������Ʈ ���� ���� ����
	ChangeCastleObjectNation(Creature::STATELESS);

	// ���� ���⸦ ���� ���� NPC �� �������´�.
	// ž���ڴ� ���ó�� ��Ų��.
	DestroyAllCastleArms(true);

	// �ش缺�� ���� ������ ������ ����Ǿ����� �ݰ� 5���� ĳ���͵鿡�� �˸���.
	// ���� ������ ����� �������Ը� �˷��� �ɵ��ѵ�..
	// (�̹� �ش� ���� �̿��� ĳ���ʹ� ������ �Ǳ⶧��...)
	CSiegeObjectMgr::GetInstance().SendLoseOwnership(m_dwCastleID);

	// ���� ��� ĳ���Ϳ��� ����
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
			// �ش� ���� ���� ������ ���� ���� ����
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
			// �ش� ���� ���� ���⿡ Ÿ�� �ִ� ��� ĳ���͵��� ������ �Ѵ�.
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

			// �ش� ���� ���� ���⸦ ���� ���� NPC�� �������´�.
			lpCastleObject->Destroy(0, false);
		}

		++itr;
	}
}

// ���� ȸ��
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
	// edith ���� �κ� �߰� (�ּ�ó�� ����)
	// CASTLE_TOOD : ���� ��� ������ �ƴϹǷ� ��Ŷ�� ������ ����.

	// �ش� �������� ����
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

// ���� ���� ȸ��
bool	CCastle::GainMineral(unsigned short wMineralID, unsigned short wAmount)
{
/*
	// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.
	// edith ���� �κ� �߰� (�ּ�ó�� ����)

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
							ERRLOG2(g_Log, "CastleID:0x%08x ������ ��ũ��Ʈ�� �������� �ʴ� ������ �����Ϸ� �Ͽ����ϴ�. ItemID:%d", m_dwCastleID, wMineralID);
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
							ERRLOG1(g_Log, "CID:0x%08x ���� ���� �������� �޴µ� �����Ͽ����ϴ�.", lpGuild->GetMaster().m_dwCID);
							DELETE_ITEM(lpItem);
							wError = PktCastleCmd::SERVER_ERROR;
							break;
						}

						// GievItem ���� ���õ� ���
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
						wAmount = itr->second;	// ���� ��

						if (0 == itr->second)
						{
							m_AccumulatedMineralMap.erase( itr );
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "CID : 0x%08x ��� �����Ͱ� �������� �ʽ��ϴ�.", lpGuild->GetMaster().m_dwCID);
					wError = PktCastleCmd::SERVER_ERROR;
				}
			}
			else
			{
				ERRLOG1(g_Log, "GID:0x%08x �ش� ��尡 �������� �ʽ��ϴ�.", m_dwGID);
				wError = PktCastleCmd::SERVER_ERROR;
			}
		}
		else
		{
			ERRLOG4(g_Log, "���� ������ ���������� ���� ���� ������ ȹ���Ϸ� �մϴ�. CastleID:0x%08x, MineralID:%d, CurrentAmount:%d, GainAmount:%d",
					m_dwCastleID, wMineralID, itr->second, wAmount);
			wError = PktCastleCmd::SERVER_ERROR;
		}
	}

	// ��� �����Ϳ��� ����
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

// ���׷��̵� ���� ������ �ֱ�
void	CCastle::AddUpgradeItem(unsigned short wItemID, int iIndex, unsigned char cNum)
{
	if (0 == m_cTotalItemNum)
	{
		m_wItemID = wItemID;
	}

	m_cTotalItemNum += cNum;
	m_cItemNum[ iIndex ] += cNum;
}

// ���׷��̵� ���� ������ ����
void	CCastle::DeleteUpgradeItem(int iIndex, unsigned char cNum)
{
	m_cTotalItemNum -= cNum;
	m_cItemNum[ iIndex ] -= cNum;

	if (0 == m_cTotalItemNum)
	{
		m_wItemID = 0;
	}
}

// ���׷��̵� ���� ������ �ʱ�ȭ
void	CCastle::InitUpgradeItem()
{
	m_wItemID = 0;
	m_cTotalItemNum = 0;
	::memset(&m_cItemNum, 0, sizeof(unsigned char) * Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM);
}

// �ӽ� ���� ����
void	CCastle::AddTempTaxMoney(unsigned char cType, unsigned long dwMoney)
{
	// Ư�� ȸ�� �̻� ������Ʈ �Ǹ� DB �߰� ������ �������ش�.
	++m_cTempTaxUpdateCount;
	m_dwTemporayTempTaxMoney += dwMoney;
	m_CastleTax[ cType ].m_dwTempTaxMoney += dwMoney;

	if (m_cTempTaxUpdateCount >= Castle::MAX_TEMP_TAX_UPDATE_COUNT || m_dwTemporayTempTaxMoney >= Castle::MAX_TEMP_TAX_AMOUNT)
	{
		m_cTempTaxUpdateCount = 0;
		m_dwTemporayTempTaxMoney = 0;

		// �߰� ������ �ӽ� ���� ���� ������Ʈ ( fBonusRate �� �������ش�. )
		SendTempTaxInfo();
	}
}


// ���� ���� ��ô
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

// �߰輭���� �ӽ� ���� ���� ������Ʈ
void	CCastle::SendTempTaxInfo()
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (NULL == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "�߰� ������ �ӽ� ���� ���� ������Ʈ ����!! : ������Ʈ ��� ����.");
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

// ���� ���� ����
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

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.

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
			case 1:	// 1�ܰ� ���׷��̵�
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

			case 2: // 2�ܰ� ���׷��̵�
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

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.

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
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� üũ�� �Ҽ� ����.

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

		// ��� ĳ���Ϳ��� ����!! (���� ������ ��� ĳ���Ͱ� �˾ƾ��Ѵ�.!!)
		PktCastleCmd pktCC;
		pktCC.m_dwCID				= 0;
		pktCC.m_dwCastleObjectID	= 0;
		pktCC.m_dwCastleID			= m_dwCastleID;
		pktCC.m_cState				= 0;
		pktCC.m_dwValue1			= cType;		// ���� Ÿ��
		pktCC.m_dwValue2			= cTax;			// ����
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

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ������ �Ҽ� ����.

	// �ش� ����� �����鿡�� ����
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