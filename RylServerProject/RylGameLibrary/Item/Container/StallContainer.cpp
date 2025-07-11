
#include "stdafx.h"

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/ChatPacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>

#include <Creature/Character/Character.h>
#include <Creature/Siege/CampShop.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>

#include <Map/FieldMap/Cell.h>
#include <Log/ItemLog.h>
#include <Utility/Math/Math.h>

#include "StallContainer.h"
#include "ContainerConstant.h"

using namespace Item;


// ------------------------------------------------------------------------------------------
// CShopContainer

CShopContainer::CShopContainer()
{
	Clear();
}

CShopContainer::~CShopContainer()
{
	Destroy();
}

bool CShopContainer::Initialize(unsigned long dwCID, unsigned char cWidth, unsigned char cHeight)
{
    return CArrayContainer::Initialize(dwCID, cWidth, cHeight, 1);
}

void CShopContainer::Clear(void)
{
	std::fill_n(m_aryCustomerID, int(MAX_CUSTOMER_NUM), 0);
}

void CShopContainer::Destroy(void)
{
    if (NULL != m_lppItems)
    {
        delete [] m_lppItems;
        m_lppItems = NULL;
    }
}

CItem* CShopContainer::GetItem(ItemPos itemPos)
{
	return CArrayContainer::GetItem(itemPos);
}

bool CShopContainer::RemoveItem(ItemPos itemPos)
{
	CItem* lpItem = GetItem(itemPos);
	if (NULL != lpItem)
	{
		lpItem->SetStallPrice(0);
		if (true == CArrayContainer::RemoveItem(itemPos))
		{
			lpItem->MoveItem(lpItem->GetPos());
			return true;
		}
	}

	return false;
}

bool CShopContainer::Enter(CCharacter* lpCustomer)
{
    if (NULL != lpCustomer && 0 != m_dwCID)
    {
	    for (int nIndex = 0; nIndex < MAX_CUSTOMER_NUM; nIndex++)
	    {
		    if (0 == m_aryCustomerID[nIndex])
		    {
			    m_aryCustomerID[nIndex] = lpCustomer->GetCID();
			    lpCustomer->GetStall().SetOtherOwner(m_dwCID);
			    SendCharStallItemInfo(lpCustomer);

			    // 노점상로그 : 입장(손님CID)
				CAggresiveCreature* lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(m_dwCID);
				if (NULL != lpCreature)
				{
	                GAMELOG::LogStallEnterLeave(*lpCreature, lpCustomer->GetCID(), true);
				}
			    return true;
		    }
        }
    }

	return false;
}

bool CShopContainer::Leave(CCharacter* lpCustomer)
{
    if (NULL != lpCustomer && 0 != m_dwCID)
    {
	    for (int nIndex = 0; nIndex < MAX_CUSTOMER_NUM; nIndex++)
	    {
		    if (m_aryCustomerID[nIndex] == lpCustomer->GetCID())
		    {
			    // 노점상로그 : 퇴장(손님CID)
				CAggresiveCreature* lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(m_dwCID);
				if (NULL != lpCreature)
				{
	                GAMELOG::LogStallEnterLeave(*lpCreature, lpCustomer->GetCID(), false);
				}

			    lpCustomer->GetStall().SetOtherOwner(0);

				std::copy(m_aryCustomerID + nIndex + 1, m_aryCustomerID + MAX_CUSTOMER_NUM, m_aryCustomerID + nIndex);
			    m_aryCustomerID[MAX_CUSTOMER_NUM - 1] = 0;

				return true;
		    }
	    }
    }

	return false;
}

unsigned char CShopContainer::GetCurrentCustomerNum(void)
{
	unsigned char cCurrentNum = 0;
	
	for (int nIndex = 0; nIndex < MAX_CUSTOMER_NUM; ++nIndex)
	{
		if (0 != m_aryCustomerID[nIndex])
		{
			++cCurrentNum;
		}
	}

	return cCurrentNum;
}

void CShopContainer::SwapPosAllItem(void)
{
    CItem** lppItem         = m_lppItems;
    CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;
    
    for (; lppItem != lppItemPastEnd; ++lppItem)
    {
        CItem* lpItem = *lppItem;
        if (NULL != lpItem && m_lpNullItem != lpItem)
        {
			lpItem->SwapPos();
        }
    }
}

bool CShopContainer::StallPriceOut(unsigned long* szStallPriceBuffer_Out, unsigned char& cItemNum_Out) const
{
    CItem** lppItem         = m_lppItems;
    CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;
    
    for (; lppItem != lppItemPastEnd; ++lppItem)
    {
        CItem* lpItem = *lppItem;
        if (NULL != lpItem && m_lpNullItem != lpItem)
        {
			szStallPriceBuffer_Out[cItemNum_Out] = lpItem->GetBuyPrice();
			cItemNum_Out++;
        }
    }
    
    return true;
}

bool CShopContainer::Close(void)
{
	for (int nIndex = 0; nIndex < MAX_CUSTOMER_NUM && 0 != m_aryCustomerID[0]; ++nIndex)
	{
		CCharacter* lpCustomer = CCreatureManager::GetInstance().GetCharacter(m_aryCustomerID[0]);
		if (NULL != lpCustomer)
		{
			// Leave 함수 내부에서 인덱스의 변화가 있으므로 0번 인덱스에 손님이 없을 때까지 계속 처리하면 된다.
			Leave(lpCustomer);

			CGameClientDispatch* lpDispatch = lpCustomer->GetDispatcher();
			if (NULL != lpDispatch) 
			{
				GameClientSendPacket::SendCharStallEnter(lpDispatch->GetSendStream(), lpCustomer->GetCID(), 0, 0);
			}
		}
	}

	Clear();

	return true;
}

bool CShopContainer::SendAllCustomer(const char* szPacket, const unsigned long dwPacketSize, 
									 bool bIncludeOwner, unsigned char cCMD_In)
{
	for (int nIndex = 0; nIndex < MAX_CUSTOMER_NUM; nIndex++)
	{
		if (0 == m_aryCustomerID[nIndex]) { break; }

		CCharacter* lpCustomer = CCreatureManager::GetInstance().GetCharacter(m_aryCustomerID[nIndex]);
		if (NULL != lpCustomer)
		{
			CGameClientDispatch* lpDispatch = lpCustomer->GetDispatcher();
			if (NULL != lpDispatch)
			{
				lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
			}
		}
	}

	return true;
}

bool CShopContainer::SendRemoveItem(TakeType takeType, unsigned char cCmd, const char* strBuyUser)
{
	PktStRI pktStRI;

	pktStRI.m_dwCharID = m_dwCID;
	pktStRI.m_dwShopID = m_dwCID;
	pktStRI.m_TakeType = takeType;
	pktStRI.m_dwPrice = 0;
	pktStRI.m_cCmd = cCmd;
	strcpy(pktStRI.m_BuyUser, strBuyUser);

	char* szPacket = reinterpret_cast<char*>(&pktStRI);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktStRI), CmdCharStallRegisterItem, 0, PktBase::NO_SERVER_ERR))
	{
		return SendAllCustomer(szPacket, sizeof(PktStRI), true, CmdCharStallRegisterItem);
	}

	return false;
}

bool CShopContainer::SendCharStallEnter(unsigned long dwCustomerID, unsigned long dwOwnerID)
{
	PktStE pktStE;

	pktStE.m_dwCustomerID = dwCustomerID;
	pktStE.m_dwOwnerID = dwOwnerID;

	char* szPacket = reinterpret_cast<char*>(&pktStE);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktStE), CmdCharStallEnter, 0, 0))
	{
		return SendAllCustomer(szPacket, sizeof(PktStE), true, CmdCharStallEnter);
	}

	return false;
}

// ------------------------------------------------------------------------------------------
// CCharacterShopContainer

CCharacterShopContainer::CCharacterShopContainer()
:	m_dwOtherOwnerID(0)
{
	Clear();
}

CCharacterShopContainer::~CCharacterShopContainer()
{
	Destroy();
}

void CCharacterShopContainer::Clear(void)
{
	std::fill_n(m_strStallName, int(MAX_STALL_NAME_LEN), 0);

	CShopContainer::Clear();
}

void CCharacterShopContainer::RollBackAllItem(void)
{
	for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
	{
        const unsigned short nHeightPos = m_nXSize * nHeight;

		for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
		{
            CItem* lpItem = m_lppItems[nHeightPos + nWidth];
			if (NULL == lpItem || m_lpNullItem == lpItem) { continue; }

			lpItem->MoveItem(lpItem->GetPos());
			lpItem->SetStallPrice(0);
		}
	}

	std::fill_n(m_lppItems, int(m_nXSize*m_nYSize), reinterpret_cast<CItem*>(NULL));
}

bool CCharacterShopContainer::SetItem(ItemPos itemPos, CItem* lpItem)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
	if (NULL != lpCharacter)
	{
		if (false == lpCharacter->GetInventory().SetItem(lpItem->GetPos(), lpItem)) 
		{ 
			SERLOG1(g_Log, "CID:%10u 노점상에 아이템 등록시 인벤토리에 링크를 남기는 데 실패하였습니다.", 
				lpCharacter->GetCID());
			return false; 
		}

		if (false == CArrayContainer::SetItem(itemPos, lpItem)) 
		{ 
			SERLOG1(g_Log, "CID:%10u 노점상에 아이템을 올리는데 실패하였습니다.", lpCharacter->GetCID());
			if (false == lpCharacter->GetInventory().RemoveItem(lpItem->GetPos()))
			{
				SERLOG1(g_Log, "CID:%10u 노점상에 아이템 등록에 실패하여 인벤토리에서 그 링크를 지우는데 실패하였습니다.", 
					lpCharacter->GetCID());
			}
			return false; 
		}

		const int MAX_BUFFER = sizeof(PktStIInfo) + MAX_ITEM_SIZE + sizeof(unsigned long);
		char szBuffer[MAX_BUFFER];

		lpItem->SwapPos();
		size_t nItemSize = MAX_ITEM_SIZE;		
		lpItem->SerializeOut(szBuffer + sizeof(PktStIInfo), nItemSize);
		if (0 == nItemSize) { return false; }
		lpItem->SwapPos();

		unsigned long* dwStallPrice = reinterpret_cast<unsigned long*>(szBuffer + sizeof(PktStIInfo) + nItemSize);
		*dwStallPrice = lpItem->GetBuyPrice();

		PktStIInfo* lpPktStIInfo = reinterpret_cast<PktStIInfo*>(szBuffer);

		lpPktStIInfo->m_dwOwnerID = m_dwCID;

		std::copy(m_aryCustomerID, m_aryCustomerID + MAX_CUSTOMER_NUM, lpPktStIInfo->m_dwCustomerID);

		lpPktStIInfo->m_dwItemSize = static_cast<unsigned long>(nItemSize);
		lpPktStIInfo->m_cItemNum = 1;

		if (PacketWrap::WrapCrypt(szBuffer, static_cast<unsigned short>(sizeof(PktStIInfo) + nItemSize + sizeof(unsigned long)), 
			CmdCharStallItemInfo, 0, 0))
		{
			return SendAllCustomer(szBuffer, 
				static_cast<unsigned long>(sizeof(PktStIInfo) + nItemSize + sizeof(unsigned long)), 
				false, CmdCharStallItemInfo);
		}
	}
	
	return false;
}

bool CCharacterShopContainer::Open(char *strStallName)
{
	if (0 != strcmp(m_strStallName, "") || 0 == m_dwCID) { return false; }

	strncpy(m_strStallName, strStallName, MAX_STALL_NAME_LEN);

	// 노점상로그 : 열기(노점상 이름)
	CAggresiveCreature* lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(m_dwCID);
	if (NULL != lpCreature)
	{
	    GAMELOG::LogStallOpenClose(*lpCreature, m_strStallName, true);
	}
	return true;
}

bool CCharacterShopContainer::Close(void)
{
	CShopContainer::Close();

	// 노점상로그 : 닫기(노점상 이름)
	CAggresiveCreature* lpCreature = CCreatureManager::GetInstance().GetAggresiveCreature(m_dwCID);
	if (NULL != lpCreature)
	{
	    GAMELOG::LogStallOpenClose(*lpCreature, m_strStallName, false);
	}
	
    RollBackAllItem();

	return true;
}

bool CCharacterShopContainer::SendCharStallItemInfo(CCharacter *lpCustomer)
{
	const int MAX_BUFFER = sizeof(PktStIInfo) + 
		((MAX_ITEM_SIZE + sizeof(unsigned long)) * ContainerConstant::STALL_WIDTH * ContainerConstant::STALL_HEIGHT);

    char szBuffer[MAX_BUFFER];

	SwapPosAllItem();
	unsigned long dwItemSize = MAX_ITEM_SIZE * m_nXSize * m_nYSize;		
	SerializeOut(szBuffer + sizeof(PktStIInfo), dwItemSize);
	SwapPosAllItem();

	unsigned char cItemNum = 0;
	StallPriceOut(reinterpret_cast<unsigned long*>(szBuffer + sizeof(PktStIInfo) + dwItemSize), cItemNum);

	CGameClientDispatch* lpDispatch = lpCustomer->GetDispatcher();
	if (NULL != lpDispatch)
	{
		return GameClientSendPacket::SendCharStallItemInfo(lpDispatch->GetSendStream(), 
			m_dwCID, m_aryCustomerID, 0, 0, szBuffer, dwItemSize, cItemNum);
	}

	return false;
}

bool CCharacterShopContainer::SendCharStallOpen(char *strStallName)
{
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
	if (NULL != lpCharacter)
	{
		CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;
		if (NULL == lpCell)
		{
			ERRLOG1(g_Log, "CID:%10u 셀 얻기 실패.", m_dwCID);
			return false;
		}

		PktStO pktStO;
        memset(&pktStO, 0, sizeof(PktStO));

		pktStO.m_dwCharID = m_dwCID;
		strncpy(pktStO.m_StallName, strStallName, PktStO::MAX_STALL_NAME_LEN);
        pktStO.m_StallName[PktStO::MAX_STALL_NAME_LEN - 1] = 0;

		lpCell->SendAllNearCellCharacter(&pktStO, sizeof(PktStO), CmdCharStallOpen);
		return true;
	}
	
	return false;
}

bool CCharacterShopContainer::SendAllCustomer(const char* szPacket, const unsigned long dwPacketSize, 
											  bool bIncludeOwner, unsigned char cCMD_In)
{
	if (true == bIncludeOwner)
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
		if (NULL != lpCharacter)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();       
			if (NULL != lpDispatch)
			{
				lpDispatch->GetSendStream().PutBuffer(szPacket, dwPacketSize, cCMD_In);
			}
		}
	}

	return CShopContainer::SendAllCustomer(szPacket, dwPacketSize, bIncludeOwner, cCMD_In);
}


// ------------------------------------------------------------------------------------------
// CCampShopContainer

CCampShopContainer::CCampShopContainer(void)
:	m_cUpdateCount(0)
{
}

CCampShopContainer::~CCampShopContainer(void)
{
}

bool CCampShopContainer::SetItem(ItemPos itemPos, CItem* lpItem)
{
	if (false == CArrayContainer::SetItem(itemPos, lpItem)) 
	{ 
		SERLOG1(g_Log, "CID:%10u 노점상에 아이템을 올리는데 실패하였습니다.", m_dwCID);
		return false; 
	}

	const int MAX_BUFFER = sizeof(PktStIInfo) + MAX_ITEM_SIZE + sizeof(unsigned long);
	char szBuffer[MAX_BUFFER];

	size_t nItemSize = MAX_ITEM_SIZE;		
	lpItem->SerializeOut(szBuffer + sizeof(PktStIInfo), nItemSize);
	if (0 == nItemSize) { return false; }

	unsigned long* dwStallPrice = reinterpret_cast<unsigned long*>(szBuffer + sizeof(PktStIInfo) + nItemSize);
	*dwStallPrice = lpItem->GetBuyPrice();

	PktStIInfo* lpPktStIInfo = reinterpret_cast<PktStIInfo*>(szBuffer);

	lpPktStIInfo->m_dwOwnerID = m_dwCID;

	CCampShop* lpShop = 
		reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(m_dwCID));
	if (NULL != lpShop)
	{
		lpPktStIInfo->m_dwTempSafe = lpShop->GetTempSafe();
		lpPktStIInfo->m_cTax = lpShop->GetTax();
	}

	std::copy(m_aryCustomerID, m_aryCustomerID + MAX_CUSTOMER_NUM, lpPktStIInfo->m_dwCustomerID);

	lpPktStIInfo->m_dwItemSize = static_cast<unsigned long>(nItemSize);
	lpPktStIInfo->m_cItemNum = 1;

	if (PacketWrap::WrapCrypt(szBuffer, static_cast<unsigned short>(sizeof(PktStIInfo) + nItemSize + sizeof(unsigned long)), 
		CmdCharStallItemInfo, 0, 0))
	{
		IncreaseUpdateCount();
		return SendAllCustomer(szBuffer, 
			static_cast<unsigned long>(sizeof(PktStIInfo) + nItemSize + sizeof(unsigned long)), 
			false, CmdCharStallItemInfo);
	}
	
	return false;
}

bool CCampShopContainer::RemoveItem(ItemPos itemPos)
{
	IncreaseUpdateCount();
	return CShopContainer::RemoveItem(itemPos);
}

void CCampShopContainer::DropItem(CCell* lpCell, const Position& currentPos, unsigned long dwOffencerGID)
{
	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
				CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];
				if (NULL != lpItem)
				{
					RemoveItem(lpItem->GetPos());

					// 등록된 아이템이 사라지는(떨어지는) 케이스이므로 팔린 것처럼 보인다.
					unsigned char cNum = (lpItem->IsSet(DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
					SendRemoveItem(TakeType::TakeType(lpItem->GetPos(), ItemPos(), cNum), PktStRI::SC_CAMP_SELL, "");

					CCell::ItemInfo itemInfo;
					const Position Pos(currentPos.m_fPointX + Math::Random::ComplexRandom(40) - 20,
						currentPos.m_fPointY,
						currentPos.m_fPointZ + Math::Random::ComplexRandom(40) - 20);

					lpCell->SetItem(Pos, lpItem, 0, dwOffencerGID, 
						(0 == dwOffencerGID) ? CCell::NONE : CCell::GUILD, itemInfo);
				}
			}
		}
	}
}

bool CCampShopContainer::StallPriceIn(const unsigned long* szStallPriceBuffer_In, unsigned char cItemNum_In)
{
	CItem** lppItem         = m_lppItems;
	CItem** lppItemPastEnd  = m_lppItems + m_nMaxSize;

	unsigned char cItemIndex = 0;

	for (; lppItem != lppItemPastEnd; ++lppItem)
	{
		CItem* lpItem = *lppItem;
		if (NULL != lpItem && m_lpNullItem != lpItem)
		{
			if (cItemIndex >= cItemNum_In)
			{
				ERRLOG1(g_Log, "CID:0x%08x 길드 요새 상점 DB에 기록된 아이템 갯수가 맞지 않습니다.", m_dwCID);
				return false;
			}

			lpItem->SetStallPrice(szStallPriceBuffer_In[cItemIndex]);
			++cItemIndex;
		}
	}

	return true;
}

bool CCampShopContainer::SendCharStallItemInfo(CCharacter *lpCustomer)
{
	CGameClientDispatch* lpDispatch = lpCustomer->GetDispatcher();
	if (NULL != lpDispatch)
	{
		return SendCharStallItemInfo(lpDispatch->GetSendStream());
	}

	return false;
}

CItem* CCampShopContainer::RegisterCancelItem(CCharacter* lpCharacter, TakeType takeType, unsigned long dwPrice, unsigned char cCmd)
{
	CCampShop* lpShop = 
		reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(m_dwCID));
	if (NULL != lpShop)
	{
		if (lpCharacter->GetGID() != lpShop->GetGID())
		{
			ERRLOG4(g_Log, "아이템을 등록/취소하려는 캐릭터와 길드 요새 상점의 ID가 다릅니다. "
				"SenderID:%10u, SenderGID:%10u, CampID:%10u, CampGID:%10u", 
				lpCharacter->GetCID(), lpCharacter->GetGID(), m_dwCID, lpShop->GetGID());
			return NULL;
		}

		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID());
		if (NULL == lpGuild)
		{
			ERRLOG2(g_Log, "아이템을 등록/취소하려는 캐릭터의 길드가 존재하지 않습니다. SenderID:%10u, SenderGID:%10u", 
				lpCharacter->GetCID(), lpCharacter->GetGID());
			return NULL;
		}

		if (lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID())
		{
			ERRLOG2(g_Log, "아이템을 등록/취소하려는 캐릭터가 길드마스터가 아닙니다. SenderID:%10u, SenderGID:%10u", 
				lpCharacter->GetCID(), lpCharacter->GetGID());
			return NULL;
		}

		switch (cCmd)
		{
			case PktStRI::SC_CAMP_REGISTER:
			{
				if (takeType.m_dstPos.m_cPos != TakeType::TS_CAMPSHOP)
				{
					ERRLOG1(g_Log, "CID:%10u 아이템을 길드 요새 상점에 잘못 올려 놓았습니다."
						"RegisterPacket에 DstPos가 Stall이 아닙니다.", lpCharacter->GetCID());
					return NULL;
				}

				if (NULL != GetItem(takeType.m_dstPos))
				{
					ERRLOG3(g_Log, "CID:%10u 놓으려는 위치에 이미 아이템이 존재합니다. 클라이언트가 준 위치:(%d, 0x%04x)", 
						lpCharacter->GetCID(), takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
					return NULL;
				}

				CItem* lpItem = lpCharacter->GetItem(takeType.m_srcPos);
				if (NULL != lpItem)
				{
					if (false == lpItem->GetItemInfo().m_DetailData.m_bExchangeAndDrop )
					{
						ERRLOG1(g_Log, "CID:%10u 아이템을 길드 요새 상점에 잘못 올려 놓았습니다."
							"ExchangeAndDrop가 TRUE가 아닙니다.", lpCharacter->GetCID());

						return NULL;
					}

					if (true == lpCharacter->RemoveItem(takeType.m_srcPos))
					{
						lpItem->SetStallPrice(dwPrice);
						if (true == SetItem(takeType.m_dstPos, lpItem))
						{
							return lpItem;
						}
						else
						{
							lpItem->SetStallPrice(0);
							if (false == lpCharacter->SetItem(takeType.m_srcPos, lpItem)) 
							{
								SERLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 제자리에 돌려 놓을 수 없음. 아이템을 제거함.",
									lpCharacter->GetCID(), takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
								DELETE_ITEM(lpItem);
							}
						}
					}
				}
				break;
			}

			case PktStRI::SC_CAMP_CANCEL:
			{
				if (takeType.m_srcPos.m_cPos != TakeType::TS_CAMPSHOP)
				{
					ERRLOG1(g_Log, "CID:%10u 길드 요새 상점에 올린 아이템을 가져오는데 실패하였습니다."
						"CancelPacket에 SrcPos가 Stall이 아닙니다.", lpCharacter->GetCID());
					return NULL;
				}

				if (NULL != lpCharacter->GetItem(takeType.m_dstPos))
				{
					ERRLOG3(g_Log, "CID:%10u 놓으려는 위치에 이미 아이템이 존재합니다. 클라이언트가 준 위치:(%d, 0x%04x)", 
						lpCharacter->GetCID(), takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
					return NULL;
				}

				CItem* lpItem = GetItem(takeType.m_srcPos);
				if (NULL != lpItem)
				{
					if (true == RemoveItem(takeType.m_srcPos))
					{
						SendRemoveItem(takeType, PktStRI::SC_CAMP_CANCEL, "");
						if (true == lpCharacter->SetItem(takeType.m_dstPos, lpItem))
						{
							return lpItem;
						}
						else
						{
							if (false == SetItem(takeType.m_srcPos, lpItem)) 
							{
								SERLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 제자리에 돌려 놓을 수 없음. 아이템을 제거함.",
									lpCharacter->GetCID(), takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
								DELETE_ITEM(lpItem);
							}
						}
					}
				}
				break;
			}
		}
	}

	return NULL;
}

bool CCampShopContainer::CheckUpdateCount(void)
{
	if (UPDATE_COUNT <= m_cUpdateCount)
	{
		m_cUpdateCount = 0;
		return true;
	}

	return false;
}

bool CCampShopContainer::SendCharStallItemInfo(CSendStream& SendStream)
{
	CCampShop* lpShop = 
		reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(m_dwCID));
	if (NULL != lpShop)
	{
		const int MAX_BUFFER = sizeof(PktStIInfo) + 
			((Item::MAX_ITEM_SIZE + sizeof(unsigned long)) * 
			ContainerConstant::CAMPSHOP_WIDTH * ContainerConstant::CAMPSHOP_HEIGHT);

		char szBuffer[MAX_BUFFER];

		unsigned long dwItemSize = Item::MAX_ITEM_SIZE * m_nXSize * m_nYSize;		
		SerializeOut(szBuffer + sizeof(PktStIInfo), dwItemSize);

		unsigned char cItemNum = 0;
		StallPriceOut(reinterpret_cast<unsigned long*>(szBuffer + sizeof(PktStIInfo) + dwItemSize), cItemNum);

		return GameClientSendPacket::SendCharStallItemInfo(SendStream, m_dwCID, m_aryCustomerID, 
			lpShop->GetTempSafe(), lpShop->GetTax(), szBuffer, dwItemSize, cItemNum);
	}

	return false;
}

bool CCampShopContainer::IsEmpty() const
{
	for (unsigned short nTab = 0; nTab < m_nTabNum; ++nTab)
	{
		for (unsigned short nHeight = 0; nHeight < m_nYSize; ++nHeight)
		{
			for (unsigned short nWidth = 0; nWidth < m_nXSize; ++nWidth)
			{                
				const CItem* lpItem = m_lppItems[m_nSizePerTab * nTab + m_nXSize * nHeight + nWidth];
				if (NULL != lpItem)
				{
                    return false;
				}
			}
		}
	}

	return true;
}