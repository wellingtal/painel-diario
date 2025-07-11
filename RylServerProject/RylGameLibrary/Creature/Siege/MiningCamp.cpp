#include "stdafx.h"
#include "MiningCamp.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Item/ItemStructure.h>
#include <Item/ItemFactory.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/MineralVeinMgr.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>
#include <Castle/CastleBlessMgr.h>


#include <Utility/Math/Math.h>


CMiningCamp::CMiningCamp(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID,
						 unsigned long dwHP, unsigned short wObjectType, unsigned char cState,
						 unsigned char cSubState, unsigned char cUpgradeStep, unsigned char cMaterial,
						 unsigned char cSiegeCount, const CampRight& campRight, bool bFullHP)
:	CCamp(MonsterCreate, dwCampID, dwGID, dwHP, wObjectType, cState, cSubState, cUpgradeStep, cMaterial, cSiegeCount, campRight, bFullHP)
{
}

CMiningCamp::~CMiningCamp()
{
}


bool	CMiningCamp::Destroy(unsigned long dwOffencerGID)
{
	CCamp::Destroy(dwOffencerGID);

	CCell* lpCell = CCellManager::GetInstance().GetCell(0,
		static_cast<unsigned long>(GetCurrentPos().m_fPointX), 
		static_cast<unsigned long>(GetCurrentPos().m_fPointY), 
		static_cast<unsigned long>(GetCurrentPos().m_fPointZ));

	if (NULL == lpCell)
	{
		ERRLOG4(g_Log, "CampID:0x%08x �������� ����� ä������ ��ġ�� �̻��մϴ�. X:%.1f, Y:%.1f, Z:%.1f", 
			GetCampID(), GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ);
		return false;
	}

	// �ӽ� ���� ���
	MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
	MineralInfoMap::iterator end = m_TemporaryMineralMap.end();

	while (itr != end)
	{
		unsigned short wMineralID = itr->first;
		unsigned short wAmount = itr->second;
        
		while (wAmount > 0)
		{
			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem( wMineralID );
			if (NULL == lpItem)
			{
				ERRLOG2(g_Log, "CampID:0x%08x ä���� ��� ���� ������ ������ �����Ͽ����ϴ�. ItemID:%d", GetCampID(), wMineralID);
				break;
			}

			unsigned char cItemNum = lpItem->GetMaxNumOrDurability();
			if (wAmount < cItemNum)
			{
				cItemNum = static_cast<unsigned char>(wAmount);
				lpItem->SetNumOrDurability(cItemNum);
				wAmount = 0;
			}
			else
			{
				lpItem->SetNumOrDurability(cItemNum);
				wAmount -= cItemNum;
			}
			
			CCell::ItemInfo itemInfo;
			const Position Pos(GetCurrentPos().m_fPointX + Math::Random::ComplexRandom(40) - 20,
							   GetCurrentPos().m_fPointY,
							   GetCurrentPos().m_fPointZ + Math::Random::ComplexRandom(40) - 20);

			lpCell->SetItem(Pos, lpItem, 0, dwOffencerGID, 
				(0 == dwOffencerGID) ? CCell::NONE : CCell::GUILD, itemInfo);
		}

		++itr;
	}

	// ���� ���� ���
	itr = m_AccumulatedMineralMap.begin();
	end = m_AccumulatedMineralMap.end();

	while (itr != end)
	{
		unsigned short wMineralID = itr->first;
		unsigned short wAmount = itr->second;

		while (wAmount > 0)
		{
			Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem( wMineralID );
			if (NULL == lpItem)
			{
				ERRLOG2(g_Log, "CampID:0x%08x ä���� ��� ���� ������ ������ �����Ͽ����ϴ�. ItemID:%d", GetCampID(), wMineralID);
				break;
			}

			unsigned char cItemNum = lpItem->GetMaxNumOrDurability();
			if (wAmount < cItemNum)
			{
				cItemNum = static_cast<unsigned char>(wAmount);
				lpItem->SetNumOrDurability(cItemNum);
				wAmount = 0;
			}
			else
			{
				lpItem->SetNumOrDurability( cItemNum );
				wAmount -= cItemNum;
			}

			CCell::ItemInfo itemInfo;
			const Position Pos(GetCurrentPos().m_fPointX + Math::Random::ComplexRandom(40) - 20,
							   GetCurrentPos().m_fPointY,
							   GetCurrentPos().m_fPointZ + Math::Random::ComplexRandom(40) - 20);

			lpCell->SetItem(Pos, lpItem, 0, dwOffencerGID, 
				(0 == dwOffencerGID) ? CCell::NONE : CCell::GUILD, itemInfo);
		}

		++itr;
	}

	return true;
}

bool	CMiningCamp::OnOff(unsigned char cSubState)
{
	m_cSubState = cSubState;

	if (Siege::MINING_OFF == m_cSubState)
	{
		m_cSiegeCount = 0;

		// �ӽ� ���� ������
		m_TemporaryMineralMap.clear();
	}
	else if (Siege::MINING_ON == m_cSubState)
	{
		m_cSiegeCount = 0;
	}

	// �ش� ������ �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= m_cSubState;
	pktCC.m_dwValue2		= Siege::MINING_CAMP_GAIN_COUNT - m_cSiegeCount;
	pktCC.m_cSubCmd			= PktCampCmd::MINING_CAMP_ON_OFF;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

bool	CMiningCamp::GainMineral(unsigned short wMineralID, unsigned short wAmount)
{
	unsigned short wError = PktCampCmd::NO_SERVER_ERR;
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
							ERRLOG2(g_Log, "CampID:0x%08x ������ ��ũ��Ʈ�� �������� �ʴ� ������ �����Ϸ� �Ͽ����ϴ�. ItemID:%d", GetCampID(), wMineralID);
							wError = PktCampCmd::SERVER_ERROR;
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
							wError = PktCampCmd::SERVER_ERROR;
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

					if (wError == PktCampCmd::NO_SERVER_ERR)
					{
						itr->second -= wAmount;
						wAmount = itr->second;

						if (0 == itr->second)
						{
							m_AccumulatedMineralMap.erase( itr );
						}
					}
				}
				else
				{
					ERRLOG1(g_Log, "CID : 0x%08x ��� �����Ͱ� �������� �ʽ��ϴ�.", lpGuild->GetMaster().m_dwCID);
					wError = PktCampCmd::SERVER_ERROR;
				}
			}
			else
			{
				ERRLOG1(g_Log, "GID:0x%08x �ش� ��尡 �������� �ʽ��ϴ�.", m_dwGID);
				wError = PktCampCmd::SERVER_ERROR;
			}
		}
		else
		{
			ERRLOG4(g_Log, "���� ������ ���������� ���� ���� ������ ȹ���Ϸ� �մϴ�. CampID:0x%08x, MineralID:%d, CurrentAmount:%d, GainAmount:%d",
				GetCampID(), wMineralID, itr->second, wAmount);
			wError = PktCampCmd::SERVER_ERROR;
		}
	}

	// �ش� ����� �ִ� �ݰ� 5�� �̳��� ����
	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= wMineralID;
	pktCC.m_dwValue2		= wAmount;		// ���� ���� �ִ� ������ �����ش�.
	pktCC.m_cSubCmd			= PktCampCmd::MINING_CAMP_GAIN_MINERAL;

	if (wError == PktCampCmd::SERVER_ERROR)
	{
		pktCC.m_dwValue1		= 0;
		pktCC.m_dwValue2		= 0;
	}

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, wError))
	{
		// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
		//SendToRadiusCell(szPacket, sizeof(PktCampCmd), CmdCampCmd);
		SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCampCmd), CmdCampCmd);
	}

	return true;
}

bool	CMiningCamp::AddMineral(unsigned char cFlag, const CampMineralInfo& campMineralInfo)
{
	if (campMineralInfo.m_wAmount > 0)
	{
		switch (cFlag)
		{
			case Siege::ACCUMULATED_MINERAL:
			{
				m_AccumulatedMineralMap.insert( std::make_pair(campMineralInfo.m_wMineralID, campMineralInfo.m_wAmount) ).second;
			}
			break;

			case Siege::TEMPORARY_MINERAL:
			{
				m_TemporaryMineralMap.insert( std::make_pair(campMineralInfo.m_wMineralID, campMineralInfo.m_wAmount) ).second;
			}
			break;

			default:	return false;
		}
	}

	return true;
}

bool	CMiningCamp::AddMineral(unsigned char cFlag, unsigned short wMineralID, unsigned short wAmount)
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

bool	CMiningCamp::SendMineralInfo(CSendStream& SendStream, unsigned char cMineralType)
{
	unsigned short wSize = 0;
	char szBuffer[1024];
	PktMiningCampMineralInfo*	lpPktMineralInfo = reinterpret_cast<PktMiningCampMineralInfo*>(szBuffer);
	CampMineralInfo*			lpCampMineralInfo = reinterpret_cast<CampMineralInfo*>(lpPktMineralInfo + 1);

	lpPktMineralInfo->m_dwCampID		= GetCampID();
	lpPktMineralInfo->m_dwRemainTime	= Siege::MINING_CAMP_GAIN_COUNT;
	lpPktMineralInfo->m_cState			= m_cSubState;
	lpPktMineralInfo->m_cMineralType	= cMineralType;

	if ( Siege::MINING_ON == m_cSubState && 0 != m_cSiegeCount )
	{
		lpPktMineralInfo->m_dwRemainTime = static_cast<unsigned long>( Siege::MINING_CAMP_GAIN_COUNT - m_cSiegeCount );
	}

	switch ( cMineralType )
	{
		case Siege::ACCUMULATED_MINERAL:
		{
			lpPktMineralInfo->m_cNum = static_cast<unsigned char>( m_AccumulatedMineralMap.size() );

			MineralInfoMap::iterator itr = m_AccumulatedMineralMap.begin();
			MineralInfoMap::iterator end = m_AccumulatedMineralMap.end();

			while (itr != end)
			{
				lpCampMineralInfo->m_wMineralID = itr->first;
				lpCampMineralInfo->m_wAmount	= itr->second;

				++itr;
				++lpCampMineralInfo;

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
				lpCampMineralInfo->m_wMineralID = itr->first;
				lpCampMineralInfo->m_wAmount	= itr->second;

				++itr;
				++lpCampMineralInfo;

				wSize += sizeof(CampMineralInfo);
			}

			lpPktMineralInfo->m_wSize = wSize;
		}
		break;
	}
	
	return SendStream.WrapCompress(szBuffer, static_cast<unsigned short>(sizeof(PktMiningCampMineralInfo) + wSize), CmdMiningCampMineralInfo, 0, 0);
}

unsigned short	CMiningCamp::GetMineralNum(unsigned char cFlag, unsigned short wMineralID) const
{
	switch (cFlag)
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

unsigned short	CMiningCamp::GetMineralTypeNum(unsigned char cFlag) const
{
	switch (cFlag)
	{
		case Siege::ACCUMULATED_MINERAL:
		{
			return static_cast<unsigned short>( m_AccumulatedMineralMap.size() );
		}
		break;

		case Siege::TEMPORARY_MINERAL:
		{
			return static_cast<unsigned short>( m_TemporaryMineralMap.size() );
		}
		break;
	}

	return 0;
}


void	CMiningCamp::ProcessMining(unsigned long dwProcessType)
{
	// 1. �������̴� ä������ �ӽ� ������ ���� �߰����ش�.
	if ( PktProcessMining::TEMPORARY_PROCESS == dwProcessType )
	{
		MineralVeinInfo* lpVeinInfo = CMineralVeinMgr::GetInstance().GetMineralVein(GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointZ);
		if ( lpVeinInfo && 0 < lpVeinInfo->m_dwNowFertility )
		{
			int nMiningCampNum = CMineralVeinMgr::GetInstance().GetMiningCampNum( lpVeinInfo->m_dwVeinColor );
			if (0 < nMiningCampNum)
			{
				MineralInfoList::const_iterator itr = lpVeinInfo->m_lstMineralInfo.begin();
				MineralInfoList::const_iterator end = lpVeinInfo->m_lstMineralInfo.end();

				unsigned long dwMaxFertility = lpVeinInfo->m_dwMaxFertility;
				unsigned long dwNowFertility = lpVeinInfo->m_dwNowFertility;
				float fRate = (float)dwNowFertility / (float)dwMaxFertility;
				bool bCampMineralChange = false, bCastleMineralChange = false;
				Castle::CCastle* lpBlessCastle = NULL ;

				while (itr != end)
				{
					// ���¿� ���� -_- ������ ������ ���� �����Ǿ��ִ�.
					// �̰��� ���� ������ ������ 0�� �ɼ��� �ֵ�. �� �̷���� ������ �ȳ��� Ȯ���� ���� Ȯ���� �ִ�.

					const MineralInfo& tempInfo = (*itr);
					unsigned short wAmount = static_cast<unsigned short>(
						Math::Random::ComplexRandom(tempInfo.m_cMax + 1, tempInfo.m_cMin) * fRate / nMiningCampNum );

					if (wAmount > 0)
					{
/*
						// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� �ູ ���ʽ� ����� ���Ƶд�.
						// edith ���� �κ� �߰� (�ּ�ó�� ����)
						// ��� ����� ���� �ູ ������ ����ִٸ�, �ູ ���ʽ��� ���� ó���� �Ѵ�.
						lpBlessCastle = Castle::CCastleMgr::GetInstance().GetCastleInBlessArea( GetPosition() );
						if (NULL != lpBlessCastle && 0 != lpBlessCastle->GetGID())
						{
							Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
							if (NULL != lpGuild && false == lpGuild->IsEnemyGuild(lpBlessCastle->GetGID()))
							{
								unsigned char cBlessBonus = Castle::CCastleBlessMgr::GetInstance().GetBonusPercent(
									lpBlessCastle->GetTotalGainTaxCount(), lpBlessCastle->GetUpgradeStep());

								// ���� �ູ ���ʽ� ��ŭ �����ش�.
								unsigned short wBlessBonusAmount = static_cast<unsigned short>(wAmount * (cBlessBonus / 100.0f));
								wBlessBonusAmount = std::max(wBlessBonusAmount, static_cast<unsigned short>(1));

								// ���� ��� ��� ���� ������ ����.
								unsigned short wCastleTaxAmount = static_cast<unsigned short>(wAmount * (lpBlessCastle->GetTax(Castle::CAMP_MINERAL_TAX) / 100.0f));
								wCastleTaxAmount = std::max(wCastleTaxAmount, static_cast<unsigned short>(1));

								wAmount += wBlessBonusAmount;
								wCastleTaxAmount = std::min(wCastleTaxAmount, wAmount);
								wAmount -= wCastleTaxAmount;

								bCastleMineralChange = true;
								lpBlessCastle->AddMineral(Siege::TEMPORARY_MINERAL, tempInfo.m_dwMineralID, wCastleTaxAmount);
							}
						}
						//
*/
						if (wAmount > 0)
						{
							bCampMineralChange = true;
							AddMineral(Siege::TEMPORARY_MINERAL, tempInfo.m_dwMineralID, wAmount);
						}
					}

					++itr;
				}

				// �߰� ������ ��Ŷ ����
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				if (0 == lpDBAgentDispatch)
				{
					ERRLOG0(g_Log, "������Ʈ ��� ����.");	
				}
				else
				{
                    if (bCampMineralChange)
					{
						// �ӽ� ������ �߰� �� ��쿡 DB �߰� ������ �ӽ� ���� ���� ������
						SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::TEMPORARY_MINERAL);
					}

					if (bCastleMineralChange && NULL != lpBlessCastle)
					{
						// �ӽ� ���� ������ �߰� �� ��쿡 DB �߰� ������ �ӽ� ���� ���� ���� ������
						lpBlessCastle->SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::TEMPORARY_MINERAL);
					}
				}
			}
		}
	}


	// 2. 7���� ���� ä������ �ӽ� ������ ���� ������ �Ű��ش�. (�ӽ� ���� ����)
	if ( PktProcessMining::ACCUMULATED_PROCESS == dwProcessType )
	{
		++m_cSiegeCount ;

		if ( m_cSiegeCount >= Siege::MINING_CAMP_GAIN_COUNT )
		{
			MineralInfoMap::iterator itr = m_TemporaryMineralMap.begin();
			MineralInfoMap::iterator end = m_TemporaryMineralMap.end();
			
			while (itr != end)
			{
				AddMineral(Siege::ACCUMULATED_MINERAL, itr->first, itr->second);
				++itr;
			}

			m_cSiegeCount = 0;
			m_TemporaryMineralMap.clear();

			// �߰� ������ ��Ŷ ����
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "������Ʈ ��� ����.");	
			}
			else
			{
				// �߰� ������ ä�� �Ϸ� �޼��� ����
				GameClientSendPacket::SendCharCampMessageToDBAgent(lpDBAgentDispatch->GetSendStream(), GetCampID(),
					PktCampMessage::MSGCMD_MINING_END, PktBase::NO_SERVER_ERR);

				// DB �߰� ������ �ӽ� ���� �� ���� ���� ����Ʈ ������
				SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::TEMPORARY_MINERAL);
				SendMineralInfo(lpDBAgentDispatch->GetSendStream(), Siege::ACCUMULATED_MINERAL);
			}

/*
			// ��� �����Ϳ��� ���� ���� ���� ������
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( m_dwGID );
			if ( lpGuild )
			{
				CCharacter* lpMaster = CCreatureManager::GetInstance().GetCharacter( lpGuild->GetMaster().m_dwCID );
				if ( lpMaster && lpMaster->GetDispatcher() )
				{
					SendMineralInfo(lpMaster->GetDispatcher()->GetSendStream(), Siege::ACCUMULATED_MINERAL);
				}
			}
*/
		}
	}
}