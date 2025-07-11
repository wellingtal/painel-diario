
#include "stdafx.h"

#include <Utility/DelimitedFile.h>
#include <Utility/Math/Math.h>

#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include "LotteryEvent.h"


const char* CLotteryEvent::ms_szEventScriptFileName = "./Script/Game/LotteryEvent.txt";


CLotteryEvent::CLotteryEvent()
:	m_bActive(false)
{
}

CLotteryEvent::~CLotteryEvent()
{
}


bool CLotteryEvent::Initialize(const char* szFileName)
{
	CDelimitedFile MainDelimitedFile;	

	if (false == MainDelimitedFile.Open((NULL == szFileName) ? ms_szEventScriptFileName : szFileName))
	{
		ERRLOG0(g_Log, "복권 이벤트 스크립트 파일 열기에 실패하였습니다.");
		return false;
	}

	if (false == MainDelimitedFile.ReadSection())
	{
		ERRLOG0(g_Log, "복권 이벤트 스크립트의 섹션이 잘못되었습니다.");
		return false;
	}

	while (true == MainDelimitedFile.ReadLine())
	{
		LotteryEventItem item;

		MainDelimitedFile.ReadData(item.m_usItemID);
		MainDelimitedFile.ReadData(item.m_cProbability);

		if (NULL == Item::CItemMgr::GetInstance().GetItemInfo(item.m_usItemID))
		{
			ERRLOG1(g_Log, "아이템 스크립트에 존재하지 않는 아이템이 복권으로 지정되어 있습니다. ItemID:%d", 
				item.m_usItemID);
			return false;
		}

		char szBuffer[10];
		itoa(item.m_usItemID, szBuffer, 10);

		const unsigned long dwLine = MainDelimitedFile.GetSection(std::string(szBuffer));
		if (0 == dwLine)
		{
			ERRLOG1(g_Log, "복권 이벤트 스크립트에 알맞은 섹션이 없습니다. ItemID:%d", item.m_usItemID);
			return false;
		}

		CDelimitedFile PrizeDelimitedFile;	

		if (false == PrizeDelimitedFile.Open((NULL == szFileName) ? ms_szEventScriptFileName : szFileName, dwLine))		
		{
			ERRLOG0(g_Log, "복권 이벤트 스크립트 파일 열기에 실패하였습니다.");
			return false;
		}

		while (true == PrizeDelimitedFile.ReadLine())
		{
			LotteryEventPrize prize;

			PrizeDelimitedFile.ReadData(prize.m_usPrizeID);
			PrizeDelimitedFile.ReadData(prize.m_cProbability);
			PrizeDelimitedFile.ReadData(prize.m_cNum);

			item.m_usPrizeSumProb += prize.m_cProbability;

			if (NULL == Item::CItemMgr::GetInstance().GetItemInfo(prize.m_usPrizeID))
			{
				ERRLOG2(g_Log, "아이템 스크립트에 존재하지 않는 아이템이 복권의 상품으로 지정되어 있습니다. "
					"ItemID:%d, PrizeID:%d", item.m_usItemID, prize.m_usPrizeID);
				return false;
			}

			item.m_aryPrize.push_back(prize);
		}

		m_aryItem.push_back(item);
	}
	
	return true;
}

unsigned short CLotteryEvent::GetLottery(void)
{
	unsigned long dwSum = 0;
	std::vector<LotteryEventItem>::iterator ItemIt;

	for (ItemIt = m_aryItem.begin(); ItemIt != m_aryItem.end(); ++ItemIt)
	{
		LotteryEventItem& item = *ItemIt;
		dwSum += item.m_cProbability;
	}

	unsigned long dwRandomNum = Math::Random::ComplexRandom(dwSum);
	unsigned long dwSpaceSum = 0;

	for (ItemIt = m_aryItem.begin(); ItemIt != m_aryItem.end(); ++ItemIt)
	{
		LotteryEventItem& item = *ItemIt;

		dwSpaceSum += item.m_cProbability;
		if (dwRandomNum < dwSpaceSum)
		{
			return item.m_usItemID;
		}
	}

	return 0;
}

Item::CItem* CLotteryEvent::PrizeLottery(unsigned short usLotteryID)
{
	for (std::vector<LotteryEventItem>::iterator ItemIt = m_aryItem.begin(); ItemIt != m_aryItem.end(); ++ItemIt)
	{
		LotteryEventItem& item = *ItemIt;

		if (usLotteryID == item.m_usItemID)
		{
			unsigned long dwRandomNum = Math::Random::ComplexRandom(item.m_usPrizeSumProb);
			unsigned long dwSpaceSum = 0;

			for (std::vector<LotteryEventPrize>::iterator PrizeIt = item.m_aryPrize.begin(); 
				PrizeIt != item.m_aryPrize.end(); ++PrizeIt)
			{
				LotteryEventPrize& prize = *PrizeIt;

				dwSpaceSum += prize.m_cProbability;
				if (dwRandomNum < dwSpaceSum)
				{
					if (0 == prize.m_usPrizeID)
					{
						return NULL;
					}

					Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(prize.m_usPrizeID);
					if (NULL != lpItem)
					{
						lpItem->SetNumOrDurability(min(prize.m_cNum, lpItem->GetMaxNumOrDurability()));
					}
					return lpItem;
				}
			}
		}
	}

	ERRLOG1(g_Log, "복권 이벤트 목록에 없는 아이템의 상품을 찾고 있습니다. ItemID:%d", usLotteryID);
	return NULL;
}
