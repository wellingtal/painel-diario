
#include "stdafx.h"

#include <Utility/Math/Math.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ContainerConstant.h>

#include <Map/FieldMap/CellManager.h>

#include <Castle/Castle.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleMgr.h>
#include <Castle/CastleBlessMgr.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include "CampShop.h"

CCampShop::CCampShop(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
					 unsigned long dwHP, unsigned short wObjectType, unsigned char cState,
					 unsigned char cSubState, unsigned char cUpgradeStep, unsigned char cMaterial,
					 unsigned char cSiegeCount, const CampRight& campRight, bool bFullHP)
:	CCamp(MonsterCreate, dwCampID, dwGID, dwHP, wObjectType, cState, cSubState, cUpgradeStep, cMaterial, cSiegeCount, campRight, bFullHP),
	m_dwTempSafe(0), m_cTax(0)
{
    m_Container.Initialize(m_dwCID, ContainerConstant::CAMPSHOP_WIDTH, ContainerConstant::CAMPSHOP_HEIGHT);
}

CCampShop::~CCampShop(void)
{
}

void CCampShop::SetTax(unsigned char cTax)
{
	m_cTax = cTax;

	PktCampCmd pktCC;
	pktCC.m_dwCID			= m_dwCID;
	pktCC.m_dwCampID		= GetCampID();
	pktCC.m_cState			= m_cState;
	pktCC.m_dwValue1		= m_cTax;
	pktCC.m_dwValue2		= 0;
	pktCC.m_cSubCmd			= PktCampCmd::CAMP_SHOP_CHANGE_TAX;

	char* szPacket = reinterpret_cast<char *>(&pktCC);
	if (true == PacketWrap::WrapCrypt(szPacket, sizeof(PktCampCmd), CmdCampCmd, 0, 0))
	{
		m_Container.SendAllCustomer(szPacket, sizeof(PktCampCmd), false, CmdCampCmd);
	}
}

bool CCampShop::AddGold(unsigned long dwGold)
{
    if (m_dwTempSafe <= ULONG_MAX - dwGold)
    {
        m_dwTempSafe += dwGold;
		m_Container.IncreaseUpdateCount();
        return true;
    }

    ERRLOG2(g_Log, "CID:%10u 길드 요새 상점의 임시 금고에 돈 오버플로우가 발생했습니다. : %dGold", m_dwCID, dwGold);
    return false;
}

bool CCampShop::DeductGold(unsigned long dwGold)
{
	if (dwGold <= m_dwTempSafe)
	{ 
		m_dwTempSafe -= dwGold;
		m_Container.IncreaseUpdateCount();
		return true;
	}

	ERRLOG2(g_Log, "CID:%10u 길드 요새 상점의 임시 금고에 돈 언더플로우가 발생했습니다. : %dGold", m_dwCID, dwGold);
	return false;
}

void CCampShop::DBUpdate(bool bForce)
{
	if (true == m_Container.CheckUpdateCount() || true == bForce)
	{
		const int MAX_BUFFER_SIZE = sizeof(PktCampShopInfo) + CampShopInfoDB::MAX_CONTAINER_SIZE;
		char szBuffer[MAX_BUFFER_SIZE] = { 0, };
		unsigned long dwBufferSize = CampShopInfoDB::MAX_CONTAINER_SIZE;
		unsigned short wTotalSize = sizeof(PktCampShopInfo);

		PktCampShopInfo* lpPktCampShopInfo = reinterpret_cast<PktCampShopInfo* >(szBuffer);
		char* lpItemBuffer = reinterpret_cast<char* >(lpPktCampShopInfo + 1);

		m_Container.SerializeOut(lpItemBuffer, dwBufferSize);
		wTotalSize += static_cast<unsigned short>(dwBufferSize);

		unsigned long* lpItemPriceBuffer = reinterpret_cast<unsigned long*>(lpItemBuffer + dwBufferSize);
		unsigned char cItemNum = 0;
		
		m_Container.StallPriceOut(lpItemPriceBuffer, cItemNum);
		wTotalSize += sizeof(unsigned long) * cItemNum;

		lpPktCampShopInfo->m_dwCampID						= m_dwOwnerID;
		lpPktCampShopInfo->m_CampShopInfo.m_dwTempSafe		= m_dwTempSafe;
		lpPktCampShopInfo->m_CampShopInfo.m_cTax			= m_cTax;
		lpPktCampShopInfo->m_CampShopInfo.m_dwBufferSize	= dwBufferSize;
		lpPktCampShopInfo->m_CampShopInfo.m_cItemNum		= cItemNum;

		GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
		if (NULL != lpDBAgentDispatch)
		{
			CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
			SendStream.WrapCompress(szBuffer, wTotalSize, CmdCampShopInfo, 0, PktBase::NO_SERVER_ERR);
		}
	}
}

bool CCampShop::SerializeIn(unsigned long dwTempSafe, unsigned char cTax, 
							char* lpItemBuffer, unsigned long dwBufferSize, unsigned char cItemNum)
{
	m_dwTempSafe = dwTempSafe;
	m_cTax = cTax;

	if (false == m_Container.SerializeIn(lpItemBuffer, dwBufferSize))
	{
		ERRLOG1(g_Log, "CID:0x%08x 길드 요새 상점의 아이템 목록을 SerializeIn 하는데 실패하였습니다.", m_dwCID);
		return false;
	}

	return m_Container.StallPriceIn(reinterpret_cast<unsigned long*>(lpItemBuffer + dwBufferSize), cItemNum);
}

Item::CItem* CCampShop::SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
										Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError)
{
	Item::CItem* lpItem = m_Container.GetItem(takeType.m_srcPos);
	if (NULL == lpItem) { return NULL; }

	unsigned long  dwCustomerCID = lpCustomer->GetCID();
	unsigned long  dwCurrentGold = lpCustomer->GetGold();
    unsigned long  dwBuyPrice    = lpItem->GetBuyPrice();
    unsigned short usPrototypeID = lpItem->GetPrototypeID();

	dwPrice = dwBuyPrice * takeType.m_cNum;
	unsigned long dwTakeGold = dwPrice;
    
	if (dwPrice > dwCurrentGold)
	{
		ERRLOG2(g_Log, "길드 요새 상점 오류 : 돈이 부족합니다. 가격:%d, 소지금:%d", dwPrice, dwCurrentGold);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
	}

	if (false == lpCustomer->GetInventory().TestItem(takeType.m_dstPos, lpItem->GetPrototypeID(), takeType.m_cNum))
	{
		Item::CItemContainer* lpItemContainer = lpCustomer->GetItemContainer(takeType.m_dstPos.m_cPos);
		if (NULL != lpItemContainer)
		{
			lpItemContainer->DumpItemInfo();
		}
		else
		{
			ERRLOG1(g_Log, "CID:%10u 아이템 덤프를 출력할 수 없습니다.", dwCustomerCID);
		}

		ERRLOG4(g_Log, "CID:%10u 아이템 종류:%d를 (%2d:%2d)에 아이템 넣기 실패.", 
			dwCustomerCID, lpItem->GetPrototypeID(), takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);

		return NULL;
	}

	bool bStackable = lpItem->IsSet(Item::DetailData::STACKABLE);
	unsigned char cNumOrDurability = lpItem->GetNumOrDurability();
	Item::ItemPos ItemPos = lpItem->GetPos();

	// 스택 가능한 아이템인 경우, 개수제한 확인
	if (!(bStackable && (cNumOrDurability < takeType.m_cNum)))
	{
		// 스택이 불가능하거나, 전부 파는 경우에는 아이템 제거
		if (!bStackable || (bStackable && (takeType.m_cNum == lpItem->GetNumOrDurability())))
		{
			if (false == m_Container.RemoveItem(takeType.m_srcPos))
			{
				ERRLOG3(g_Log, "CID:%10u 아이템을 (%2d, %4x)위치로부터 지우는 데 실패했습니다.", 
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
				GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
				return NULL;
			}
			else
			{
				m_Container.SendRemoveItem(takeType, PktStRI::SC_CAMP_SELL, "");
			}
		}
		else
		{
			lpItem->SetNumOrDurability(cNumOrDurability - takeType.m_cNum);
			m_Container.SendRemoveItem(takeType, PktStRI::SC_CAMP_SELL, "");

			lpItem = Item::CItemFactory::GetInstance().CreateItem(usPrototypeID);
			if (NULL == lpItem)
			{
				ERRLOG1(g_Log, "길드 요새 상점 오류 : 아이템 생성 실패. ProtoTypeID : %d", usPrototypeID);
				GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
				return NULL;
			}

			lpItem->SetNumOrDurability(takeType.m_cNum);
		}
/*
		// CASTLE_TODO : 성이 길드 소유가 아니므로 성의 축복 보너스 기능은 막아둔다.
		// edith 세금 부분 추가 (주석처리 뺐음)
		// 길드 요새가 성의 축복 영역에 들어있다면, 축복 보너스와 세금 처리를 한다.
		Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleInBlessArea( GetPosition() );
		if (NULL != lpCastle && 0 != lpCastle->GetGID())
		{
			Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
			if (NULL != lpGuild && false == lpGuild->IsEnemyGuild(lpCastle->GetGID()))
			{
				unsigned char cBlessBonus = Castle::CCastleBlessMgr::GetInstance().GetBonusPercent(
					lpCastle->GetTotalGainTaxCount(), lpCastle->GetUpgradeStep());

				// 성의 축복 보너스 만큼 더해준다.
				unsigned long dwBlessBonusTax = static_cast<unsigned long>(dwPrice * (cBlessBonus / 100.0f));
				dwTakeGold += dwBlessBonusTax;

				// 성에 길드 요새 Gold 세금을 낸다.
				unsigned long dwCastleTax = static_cast<unsigned long>(dwPrice * (lpCastle->GetTax(Castle::CAMP_GOLD_TAX) / 100.0f));
				dwCastleTax = std::min(dwCastleTax, dwTakeGold);

				if (dwCastleTax > 0)
				{
					lpCastle->AddTempTaxMoney(Castle::CAMP_GOLD_TAX, dwCastleTax);
					dwTakeGold -= dwCastleTax;
				}
			}
		}
*/
		AddGold(dwTakeGold);

        DETLOG4(g_Log, "길드 요새 상점 : %d 타입 아이템 판매로 %u 만큼의 돈을 얻었습니다."
            " 아이템의 개별 가격은 %u이고 판매 개수는 %d 입니다",
            usPrototypeID, dwPrice, dwBuyPrice, takeType.m_cNum);
	}
	else
	{
		ERRLOG4(g_Log, "길드 요새 상점 오류 : (%2d, %4x)의 아이템 개수 : %d개 팔려는 아이템 개수 : %d개", 
			takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, cNumOrDurability, takeType.m_cNum);
		GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::FAIL_ITEM_BUY);
		return NULL;
	}

	GAMELOG::LogTradeItem(*this, dwCustomerCID, dwPrice, lpItem, takeType.m_srcPos, PktTr::TRC_SELL, PktTr::NO_SERVER_ERR);
	return lpItem;
}

bool CCampShop::Destroy(unsigned long dwOffencerGID)
{
	CCamp::Destroy(dwOffencerGID);

	// 입장객을 퇴장시킨다.
	GetContainer().Close();

	CCell* lpCell = CCellManager::GetInstance().GetCell(0,
		static_cast<unsigned long>(GetCurrentPos().m_fPointX), 
		static_cast<unsigned long>(GetCurrentPos().m_fPointY), 
		static_cast<unsigned long>(GetCurrentPos().m_fPointZ));

	if (NULL == lpCell)
	{
		ERRLOG4(g_Log, "CampID:0x%08x 아이템을 드랍할 길드 요새 상점의 위치가 이상합니다. X:%.1f, Y:%.1f, Z:%.1f", 
			GetCampID(), GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ);
		return false;
	}

	// 임시 금고 드랍
	if (0 != m_dwTempSafe)
	{
		CCell::ItemInfo itemInfo;
		const Position Pos(GetCurrentPos().m_fPointX + Math::Random::ComplexRandom(40) - 20,
			GetCurrentPos().m_fPointY,
			GetCurrentPos().m_fPointZ + Math::Random::ComplexRandom(40) - 20);

		lpCell->SetItem(Pos, NULL, m_dwTempSafe, dwOffencerGID, 
			(0 == dwOffencerGID) ? CCell::NONE : CCell::GUILD, itemInfo);
	}

	GetContainer().DropItem(lpCell, GetCurrentPos(), dwOffencerGID);

	return true;
}

unsigned long CCampShop::RepairItem(Item::CEquipment* lpEquipment, unsigned long& dwCurrentGold)
{
    if (NULL != lpEquipment)
    {
		// 길드 요새 상정의 세율 적용 (수리)
		unsigned long dwTax = static_cast<unsigned long>(lpEquipment->GetRepairPrice() * (m_cTax / 100.0f));
	    const unsigned long dwRepairGold = lpEquipment->GetRepairPrice() + dwTax;

	    if (dwRepairGold <= dwCurrentGold) 
        {
	        dwCurrentGold -= dwRepairGold;
/*
			// CASTLE_TODO : 성이 길드 소유가 아니므로 성의 축복 보너스 기능은 막아둔다.
			// edith 세금 부분 추가 (주석처리 뺐음)
			// 길드 요새가 성의 축복 영역에 들어있다면, 축복 보너스와 세금 처리를 한다.
			Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleInBlessArea( GetPosition() );
			if (NULL != lpCastle && 0 != lpCastle->GetGID())
			{
				Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
				if (NULL != lpGuild && false == lpGuild->IsEnemyGuild(lpCastle->GetGID()))
				{
					unsigned char cBlessBonus = Castle::CCastleBlessMgr::GetInstance().GetBonusPercent(
						lpCastle->GetTotalGainTaxCount(), lpCastle->GetUpgradeStep());

					// 성의 축복 보너스 만큼 더해준다.
					unsigned long dwBlessBonusTax = static_cast<unsigned long>(dwRepairGold * (cBlessBonus / 100.0f));
					dwTax += dwBlessBonusTax;

					// 성에 길드 요새 Gold 세금을 낸다.
					unsigned long dwCastleTax = static_cast<unsigned long>(dwRepairGold * (lpCastle->GetTax(Castle::CAMP_GOLD_TAX) / 100.0f));
					dwCastleTax = std::min(dwCastleTax, dwTax);

					if (dwCastleTax > 0)
					{
						lpCastle->AddTempTaxMoney(Castle::CAMP_GOLD_TAX, dwCastleTax);
						dwTax -= dwCastleTax;
					}
				}
			}
			//
*/
			m_dwTempSafe += dwTax;
            lpEquipment->SetNumOrDurability(lpEquipment->GetMaxNumOrDurability());
	        return dwRepairGold;
        }
    }

    return 0;
}

bool CCampShop::ItemDump(char* pBuffer, int* nBufferSize_InOut) const
{
	using namespace GAMELOG;

	sItemDump*	lpItemDump = reinterpret_cast<sItemDump*>(pBuffer);
	char*		lpItems = reinterpret_cast<char*>(&lpItemDump[1]);

	std::fill_n(lpItemDump->m_usDataSize, int(sItemDump::MAX_DUMP), 0);

	unsigned long   dwSize = 0;
	unsigned short	usTotalSize = sizeof(sItemDump);	

    dwSize = *nBufferSize_InOut;
	m_Container.SerializeOut(lpItems, dwSize);
	usTotalSize += static_cast<unsigned short>(dwSize);

	*nBufferSize_InOut = usTotalSize;
	return true;
}
