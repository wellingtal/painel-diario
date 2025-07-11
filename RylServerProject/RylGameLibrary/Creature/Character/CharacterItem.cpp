
#include "stdafx.h"

#include <Skill/SkillTable.h>
#include <GameEvent/GameEventMgr.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>


#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ItemContainer.h>
#include <Item/Container/ExchangeContainer.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Creature/Siege/SiegeObjectMgr.h>
#include <Castle/CastleMgr.h>

#include <Map/FieldMap/CellManager.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "Character.h"


using namespace Item;

bool CCharacter::AddGold(unsigned long dwGold, bool bNotice)
{
    if (m_DBData.m_Info.Gold <= ULONG_MAX - dwGold)
    {
        m_DBData.m_Info.Gold += dwGold;

		// 길드원 정보 업데이트
		CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
		if (NULL != lpGuild)
		{
			lpGuild->UpdateMemberInfo(m_dwCID, m_DBData.m_Info.Gold, Guild::TYPE_GOLD);
		}

		if (true == bNotice)
		{
			if (NULL != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharTakeGold(m_lpGameClientDispatch->GetSendStream(), 
					m_dwCID, dwGold, TakeType::TS_NONE, TakeType::TS_INVEN, 0);
			}
		}

        return true;
    }

    ERRLOG2(g_Log, "CID:0x%08x 인벤토리 돈 오버플로우가 발생했습니다. : %dGold", m_dwCID, dwGold);
    return false;
}

bool CCharacter::DeductGold(unsigned long dwGold, bool bNotice)
{
    if (dwGold <= m_DBData.m_Info.Gold)
    { 
        m_DBData.m_Info.Gold -= dwGold;

		// 길드원 정보 업데이트
		CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
		if (NULL != lpGuild)
		{
			lpGuild->UpdateMemberInfo(m_dwCID, m_DBData.m_Info.Gold, Guild::TYPE_GOLD);
		}

		if (true == bNotice)
		{
			if (NULL != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharTakeGold(m_lpGameClientDispatch->GetSendStream(), 
					m_dwCID, dwGold, TakeType::TS_INVEN, TakeType::TS_NONE, 0);
			}
		}

		return true;
    }
    
    ERRLOG3(g_Log, "CID:0x%08x 인벤토리 돈 언더플로우가 발생했습니다. : 소지 %dGold 필요 %dGold", m_dwCID, m_DBData.m_Info.Gold, dwGold);
    return false;
}

bool CCharacter::DeductMileage(unsigned long dwMileage, bool bNotice)
{
	if (dwMileage <= m_DBData.m_Info.Mileage)
	{ 
		m_DBData.m_Info.Mileage -= dwMileage;
		return true;
	}

	ERRLOG3(g_Log, "CID:0x%08x 인벤토리 메달 언더플로우가 발생했습니다. : 소지 %dMileage 필요 %dMileage", m_dwCID, m_DBData.m_Info.Mileage, dwMileage);
	return false;
}

unsigned long CCharacter::GetGold(unsigned char cPos)
{
    unsigned long dwGold = 0;

	switch (cPos)
	{
		case TakeType::TS_INVEN:	dwGold = GetGold();            break;
		case TakeType::TS_EXCHANGE:	dwGold = m_Exchange.GetGold(); break;
		case TakeType::TS_DEPOSIT:  dwGold = m_Deposit.GetGold();  break;
	}

    return dwGold;
}


bool CCharacter::MoveGold(unsigned long dwGold, unsigned char cSrcPos, unsigned char cDstPos, 
                          unsigned short& usError)
{
    usError = 0;
    bool bResult = false;

	switch (cSrcPos)
	{
		case TakeType::TS_INVEN:	bResult = DeductGold(dwGold, false);		break;
		case TakeType::TS_EXCHANGE:	bResult = m_Exchange.DeductGold(dwGold);	break;
		case TakeType::TS_DEPOSIT:  bResult = m_Deposit.DeductGold(dwGold);		break;
		default:                    bResult = false;							break;
	}

    if (false == bResult)
    {
        usError = PktTG::FAIL_MOVE;        
        return false;
    }

    switch (cDstPos)
	{
		case TakeType::TS_INVEN:	bResult = AddGold(dwGold, false);			break;
		case TakeType::TS_EXCHANGE:	bResult = m_Exchange.AddGold(dwGold);   	break;
		case TakeType::TS_DEPOSIT:  bResult = m_Deposit.AddGold(dwGold);    	break;
		default:                    bResult = false;                        	break;
	}

    if (false == bResult || 0 != usError)
    {
        usError = PktTG::GOLD_OVERFLOW;

        switch (cSrcPos)
	    {
			case TakeType::TS_INVEN:	bResult = AddGold(dwGold, false);		break;
			case TakeType::TS_EXCHANGE:	bResult = m_Exchange.AddGold(dwGold);   break;
			case TakeType::TS_DEPOSIT:  bResult = m_Deposit.AddGold(dwGold);    break;
	    }

        return false;
    }

	return true;
}

bool CCharacter::MoveItem(const TakeType takeType, bool bChk)
{
	PERFORMANCE_CHECK(FunctionTimingCheck);

    Item::CItemContainer* lpSrcContainer = GetItemContainer(takeType.m_srcPos.m_cPos);
    Item::CItemContainer* lpDstContainer = GetItemContainer(takeType.m_dstPos.m_cPos);

	ERRLOG(
		const int   MAX_ERROR_BUFFER = 4096;
	char        szErrorBuffer[MAX_ERROR_BUFFER] = "";
	int         nLength = 0;
	);

    if (NULL == lpSrcContainer || NULL == lpDstContainer)
    {
        ERRLOG3(g_Log, "CID:0x%08x SrcPos또는 DstPos가 잘못되었습니다. SrcPos:%d, DstPos:%d",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
        return false;
    }

	if (TakeType::TS_TEMPINVEN == takeType.m_srcPos.m_cPos)
	{
		if (TakeType::TS_QSLOT == takeType.m_dstPos.m_cPos || 
			TakeType::TS_STALL == takeType.m_dstPos.m_cPos)
		{
			ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : 임시 인벤토리에서 잘못된 곳으로 아이템을 옮기려 합니다. "
				"(%d 에서 %d 로 이동)", m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
			return false;
		}
	}

	if (TakeType::TS_TEMPINVEN == takeType.m_dstPos.m_cPos)
	{
		ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : 임시 인벤토리로 아이템을 옮기려 합니다. (%d 에서 %d 로 이동)", 
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
		return false;
	}

	// Src에서 아이템을 얻어온다.
	Item::CItem* lpSrcItem = GetItem(takeType.m_srcPos);
	if (NULL == lpSrcItem) 
	{
		ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 에서 아이템을 얻어올 수 없습니다.",
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

        lpSrcContainer->DumpItemInfo();		
        return false;
	}

	// 룬 아이템이 장착 되어 있으면 스킬 포인트 체크.
	if(bChk)
	{
		bool bResult = CheckItemRuneSkill(takeType.m_srcPos, Item::ItemPos(0, 0), false);

		if(!bResult)
		{
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// edith 2009.02.28 아이템 교환시 인벤에서 아이템을 나눠 교환창의 아이템과 Swip할때 생기는
	// 버그를 수정하기 위해 아래 아이템 스플릿함수가 추가됨.

	BOOL bOri = FALSE;
	// 우선 스택형이고, 타입이 틀리고, 개수가 틀릴때만 따로 처리하게 한다.
	if(lpSrcItem->IsSet(Item::DetailData::STACKABLE) && takeType.m_cNum != lpSrcItem->GetNumOrDurability())
	{
		// Dst에서 아이템을 얻어온다.
		Item::CItem* lpDstItem = GetItem(takeType.m_dstPos);
		// 만약 Dest 값이 있고 소스와 같은 모양의 아이템이라면 합치는 로직일수가 있다.
		if(lpDstItem && lpSrcItem->GetPrototypeID() == lpDstItem->GetPrototypeID())
		{
			bOri = TRUE;
		}
		else
		{
			// 아이템을 나눈다.
			if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum)
			{
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u 아이템 이동 실패 : Source에 있는 아이템의 개수가 나누려고 하는 아이템의 개수보다 작거나 같습니다."
					"Source의 현재 아이템 개수 %d, 나누려고 하는 아이템의 개수 %d",
					m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum));		        
				return false;
			}
			else
			{
				// 아이템 생성함.
				Item::CItem* lpSplitItem = Item::CItemFactory::GetInstance().CreateItem(lpSrcItem->GetItemInfo());
				if (NULL == lpSplitItem) 
				{
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u 아이템 이동 실패 : 아이템을 생성할 수 없습니다.", m_dwCID));
					return false;
				}

				lpSplitItem->SetNumOrDurability(takeType.m_cNum);

				// 자리가 비어 있으면 아이템 이동
				if (NULL == lpDstItem)
				{	
					if (false == SetItem(takeType.m_dstPos, lpSplitItem))		
					{
						ERRLOG5(g_Log, "CID:0x%08x 아이템 이동 실패 : 클라이언트가 준(%d, 0x%04x) 위치에 Set할 수 없습니다."
							" 종류ID:%d, 아이템 내구도(개수):%d",
							m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex, 
							lpSrcItem->GetPrototypeID(), lpSrcItem->GetNumOrDurability());

						DELETE_ITEM(lpSplitItem);

						goto MOVE_ROLLBACK;
					}

					// 최종 성공
					lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);
				}
				else
				{
					ERRLOG5(g_Log, "CID:0x%08x 아이템 이동 실패 : 클라이언트가 준(%d, 0x%04x) 위치에 Set할 수 없습니다."
						" 종류ID:%d, 아이템 내구도(개수):%d",
						m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex, 
						lpSrcItem->GetPrototypeID(), lpSrcItem->GetNumOrDurability());

					goto MOVE_ROLLBACK;
				}
			}
		}
	}
	else 
		bOri = TRUE;

	if(bOri)
	{
		// 자신의 위치에 겹치는 것이 실패하는 것을 방지하기 위해 제거해둔다.
		if (false == RemoveItem(takeType.m_srcPos))
		{
			ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 에서 아이템을 리셋할 수 없습니다.",
				m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
	        
			lpSrcContainer->DumpItemInfo();
			return false;
		}

		// Dst에서 아이템을 얻어온다.
		Item::CItem* lpDstItem = GetItem(takeType.m_dstPos);

		// 자리가 비어 있으면 아이템 이동
		if (NULL == lpDstItem)
		{	
			if (false == SetItem(takeType.m_dstPos, lpSrcItem))		
			{
				ERRLOG5(g_Log, "CID:0x%08x 아이템 이동 실패 : 클라이언트가 준(%d, 0x%04x) 위치에 Set할 수 없습니다."
					" 종류ID:%d, 아이템 내구도(개수):%d",
					m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex, 
					lpSrcItem->GetPrototypeID(), lpSrcItem->GetNumOrDurability());
				goto MOVE_ROLLBACK;
			}
		}
		// 스택일 가능성이 있다.
		else
		{	
			if (TakeType::TS_STALL == takeType.m_dstPos.m_cPos)
			{
				ERRLOG5(g_Log, "CID:0x%08x 아이템 이동 실패 : 스택할 수 없는 위치. SrcPos : (%d, 0x%04x), DstPos : (%d, 0x%04x)", 
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
				goto MOVE_ROLLBACK;
			}

			if (false == lpSrcItem->IsSet(DetailData::STACKABLE) || !lpDstItem->IsSet(DetailData::STACKABLE))
			{
				ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : 스택할 수 없는 아이템. SrcItem의 종류ID %d, DstItem의 종류ID %d", 
					m_dwCID, lpSrcItem->GetPrototypeID(), lpDstItem->GetPrototypeID());
				goto MOVE_ROLLBACK;
			}
				
			if (lpSrcItem->GetPrototypeID() != lpDstItem->GetPrototypeID())  
			{
				ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : 종류가 다른 아이템 위에 올려 놓았습니다."
					"SrcItem의 종류ID %d, DstItem의 종류ID %d", m_dwCID, lpSrcItem->GetPrototypeID(), lpDstItem->GetPrototypeID());				
				goto MOVE_ROLLBACK;
			}

			if (lpSrcItem->GetNumOrDurability() < takeType.m_cNum) 
			{
				ERRLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : Source에 있는 아이템의 개수가 옮기려고 하는 아이템의 개수보다 작습니다."
					"SrcItem개수 %d, 옮기려고 한 개수 %d", m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum);
				goto MOVE_ROLLBACK;
			}

			if (lpDstItem->GetNumOrDurability() + takeType.m_cNum > lpDstItem->GetMaxNumOrDurability()) 
			{
				ERRLOG4(g_Log, "CID:0x%08x 아이템 이동 실패 : 최대 개수 이상 아이템을 올려 놓을 수 없습니다"
					" 놓여 있는 아이템의 개수 %d, 놓으려고 한 아이템의 개수 %d, 놓을수 있는 최대 개수 %d",
					m_dwCID, lpDstItem->GetNumOrDurability(), takeType.m_cNum, lpDstItem->GetMaxNumOrDurability());
				goto MOVE_ROLLBACK;
			}

			// 아이템 개수 조정
			lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);
			lpDstItem->SetNumOrDurability(lpDstItem->GetNumOrDurability() + takeType.m_cNum);

			if (takeType.m_dstPos.m_cPos == TakeType::TS_EXCHANGE)
			{
				CCharacter* lpCharacter = m_Exchange.GetExchangeCharacter();
				if (NULL != lpCharacter)
				{
					CGameClientDispatch* lpGameClientDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpGameClientDispatch)
					{
						GameClientSendPacket::SendCharExchangeItem(lpGameClientDispatch->GetSendStream(), 
							m_dwCID, 0, lpDstItem, takeType.m_dstPos, true, false);
					}
				}
			}

			if (0 == lpSrcItem->GetNumOrDurability())
			{
				// 이미 Remove한 아이템이므로 지워도 상관 없다.
				DELETE_ITEM(lpSrcItem);
			}
			else 
			{	
				// 남아 있는 아이템을 다시 Set한다.
				if (!SetItem(takeType.m_srcPos, lpSrcItem))
				{
					SERLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 제자리에 돌려 놓을 수 없음. 아이템을 제거함.",
						m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

					lpSrcContainer->DumpItemInfo();
					SERLOG0(g_Log, "Src아이템 덤프 성공");
					
					DELETE_ITEM(lpSrcItem);
				}
			}
		}
	}

	return true;

MOVE_ROLLBACK:    

    lpDstContainer->DumpItemInfo();

	if (!SetItem(takeType.m_srcPos, lpSrcItem)) 
	{
		SERLOG3(g_Log, "CID:0x%08x 아이템 이동 실패 : (%d, 0x%04x) 제자리에 돌려 놓을 수 없음. 아이템을 제거함.",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

        lpSrcContainer->DumpItemInfo();
        SERLOG0(g_Log, "Src아이템 덤프 성공");
        
        DELETE_ITEM(lpSrcItem);
	}

	return false;
}


Item::CItem* CCharacter::SplitItem(const TakeType takeType)
{
    Item::CItemContainer* lpSrcContainer = GetItemContainer(takeType.m_srcPos.m_cPos);
    Item::CItemContainer* lpDstContainer = GetItemContainer(takeType.m_dstPos.m_cPos);

    if (NULL == lpSrcContainer || NULL == lpDstContainer)
    {
        ERRLOG3(g_Log, "CID:0x%08x SrcPos또는 DstPos가 잘못되었습니다. SrcPos:%d, DstPos:%d",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
        return false;
    }

	if (TakeType::TS_TEMPINVEN == takeType.m_srcPos.m_cPos)
	{
		ERRLOG1(g_Log, "CID:0x%08x 아이템 스플릿 실패 : 임시 인벤토리의 아이템을 스플릿하려 합니다.", m_dwCID);
		return false;
	}

	// Src에서 아이템을 얻어온다.
	Item::CItem* lpSrcItem = GetItem(takeType.m_srcPos);
	if (NULL == lpSrcItem) 
	{ 
		ERRLOG3(g_Log, "CID:0x%08x 아이템 스플릿 실패 : (%d, 0x%04x) 위치에서 아이템을 얻어올 수 없습니다.",
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
        lpSrcContainer->DumpItemInfo();
		return false;
	}

    if (false == lpSrcItem->IsSet(DetailData::STACKABLE)) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 아이템 스플릿 실패 : 나눌 수 없는 아이템(스택 불가)입니다. 종류 ID : %d",
			m_dwCID, lpSrcItem->GetPrototypeID());
		return false;
	}
	if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum) 
	{
		ERRLOG3(g_Log, "CID:0x%08x 아이템 스플릿 실패 : Source에 있는 아이템의 개수가 나누려고 하는 아이템의 개수보다 작거나 같습니다."
			"Source의 현재 아이템 개수 %d, 나누려고 하는 아이템의 개수 %d",
			m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum);
		return false;
	}

	// 아이템 생성한다.
	Item::CItem* lpSplitItem = CItemFactory::GetInstance().CreateItem(lpSrcItem->GetPrototypeID());

	if (NULL == lpSplitItem) 
	{
		ERRLOG1(g_Log, "CID:0x%08x 아이템 스플릿 실패 : 아이템을 생성할 수 없습니다.", m_dwCID);
		return false;
	}

	lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);
	lpSplitItem->SetNumOrDurability(takeType.m_cNum);

	if (!SetItem(takeType.m_dstPos, lpSplitItem)) 
	{
		ERRLOG3(g_Log, "CID:0x%08x 아이템 스플릿 실패 : (%d, 0x%04x)쪼개서 내려놓고자 하는 자리에, 아이템을 내려 놓을 수 없습니다.",
			m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
		
        DELETE_ITEM(lpSplitItem);

        lpDstContainer->DumpItemInfo();
		return false;
	}

    // Set이 성공한 다음 전송한다.
	if (takeType.m_srcPos.m_cPos == TakeType::TS_EXCHANGE) 
	{
        CCharacter* lpCharacter = m_Exchange.GetExchangeCharacter();
        if (NULL != lpCharacter)
        {
            CGameClientDispatch* lpGameClientDispatch = lpCharacter->GetDispatcher();
            if (NULL != lpGameClientDispatch)
            {
                GameClientSendPacket::SendCharExchangeItem(lpGameClientDispatch->GetSendStream(), 
                    m_dwCID, 0, lpSrcItem, takeType.m_dstPos, false, false);
            }
        }
	}

	return lpSplitItem;
}


bool CCharacter::SwapItem(const TakeType SrcTake, const TakeType DstTake)
{
/*	
	아이템 스왑의 예.

	장비[인덱스:5] 아이템A 를 인벤[인덱스:9]의 아이템B 와 스왑	= Src[1, 5] Dst[2, 9] / Src[2, 9] Dst[6, X]
	* 아이템 A는 아이템 B의 자리로 가고, 아이템 B는 마우스에 들려 있는 상태. 즉 임시 공간에 저장이 됨
	마우스 아이템B 를 인벤[인덱스:4]의 아이템C 와 다시 스왑		= Src[6, X] Det[2, 4] / Src[2, 4] Dst[6, X]
	마우스 아이템C 를 다시 인벤[인덱스:1]의 자리에 놓는다.		= Src[6, X] Det[2, 1] : 스왑이 아닌 아이템 집기(놓기)
*/	
	if (TakeType::TS_TEMP != DstTake.m_dstPos.m_cPos) 
	{
		ERRLOG1(g_Log, "CID:0x%08x DstTake의 DstPos가 TS_TEMP가 아닙니다.", m_dwCID);
		return false;
	}

    Item::ItemPos   HoldPos(TakeType::TS_TEMP, Item::ExtraSpacePos::HOLDITEM_POS);			
	Item::CItem*    lpHoldItem = m_ExtraSpace.GetItem(HoldPos);

	// 룬 아이템이 장착 되어 있으면 스킬 포인트 체크.
	bool bChk = CheckItemRuneSkill(SrcTake.m_srcPos, DstTake.m_srcPos, true);

	if(!bChk)
	{
		return false;
	}	

	if (NULL == lpHoldItem) 
	{
        // 임시 공간에 아이템이 없음. 임시 공간으로 아이템 들어 낸다.
		if (false == MoveItem(DstTake, false))
		{
			ERRLOG1(g_Log, "CID:0x%08x 임시 공간으로 아이템을 옮길 수 없습니다", m_dwCID);
			return false;
		}

        // 아이템을 내려 놓는다.
		if (false == MoveItem(SrcTake, false))
		{
			ERRLOG1(g_Log, "CID:0x%08x SrcTake를 실패했습니다", m_dwCID);
            
            // 아이템 Swap을 RollBack합니다
			if (false == MoveItem(TakeType(DstTake.m_dstPos, DstTake.m_srcPos, DstTake.m_cNum)))
			{
				ERRLOG1(g_Log, "CID:0x%08x 아이템 스왑 RollBack이 실패했습니다", m_dwCID);
			}

			return false;
		}
	}
	else
	{
		// 이미 임시 공간에 아이템이 있음. 이 경우는 SrcTake.m_srcPos.m_cPos가 TS_TEMP여야 한다.
		if (TakeType::TS_TEMP != SrcTake.m_srcPos.m_cPos)
		{
			ERRLOG1(g_Log, "CID:0x%08x 이미 임시 공간에 아이템이 있는데, SrcTake의 SrcPos가 TS_TEMP가 아닙니다.", m_dwCID);
			return false;
		}

        // 임시 공간에서 아이템 제거.
		if (false == m_ExtraSpace.RemoveItem(HoldPos)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x 임시 공간에서 아이템을 제거하는 데 실패했습니다.", m_dwCID);
			return false;
		}

        // 임시 공간으로 아이템 옮김
		if (false == MoveItem(DstTake)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x 임시 공간으로 아이템을 옮길 수 없습니다", m_dwCID);

			if (false == m_ExtraSpace.SetItem(HoldPos, lpHoldItem))
			{
				ERRLOG1(g_Log, "CID:0x%08x 임시 공간 아이템 RollBack에 실패했습니다.", m_dwCID);
			}

			return false;
		}

        // 임시 공간에어 얻어냈던 아이템을 내려놓는다.
		if (false == SetItem(SrcTake.m_dstPos, lpHoldItem)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x 임시 공간에 있던 아이템을 옮길 수 없습니다", m_dwCID);

			if (false == MoveItem(TakeType(DstTake.m_dstPos, DstTake.m_srcPos, DstTake.m_cNum)))
			{
				ERRLOG1(g_Log, "CID:0x%08x 아이템 스왑 RollBack이 실패했습니다", m_dwCID); 
			}

			if (false == m_ExtraSpace.SetItem(HoldPos, lpHoldItem)) 
			{
				ERRLOG1(g_Log, "CID:0x%08x 임시 공간 아이템 RollBack에 실패했습니다.", m_dwCID);
			}

			return false;
		}
	}

	return true;
}


bool CCharacter::Pickup(Item::CItem* lpItem, ItemPos dstPos) 
{
    if (NULL == lpItem)
    {
        ERRLOG1(g_Log, "CID:0x%08x 아이템이 NULL입니다.", m_dwCID);
        return false;
    }

	Item::CItem* lpPrevItem = m_Inventory.GetItem(dstPos);

	if (NULL == lpPrevItem) 
	{
        // 받은 아이템을 빈자리에 놓는다.
		if (false == SetItem(dstPos, lpItem)) 
		{
			ERRLOG3(g_Log, "CID:0x%08x 아이템을 집어서 놓는 데 실패했습니다. (%d, 0x%04x)",
                m_dwCID, dstPos.m_cPos, dstPos.m_cIndex);
            m_Inventory.DumpItemInfo();
			return false;
		}		
	}
	else 
	{	
        // 이미 있는 자리에 아이템을 스택한다.
		if (false == lpPrevItem->IsSet(DetailData::STACKABLE)) 
		{
            ERRLOG3(g_Log, "CID:0x%08x 스택할 수 없는 아이템입니다. ItemID:%d, PrevItemID:%d", 
                m_dwCID, lpItem->GetPrototypeID(), lpPrevItem->GetPrototypeID());
			m_Inventory.DumpItemInfo();
			return false;
		}

		if (lpPrevItem->GetPrototypeID() != lpItem->GetPrototypeID()) 
		{ 
            ERRLOG3(g_Log, "CID:0x%08x 종류 ID가 다릅니다. ItemID:%d, PrevItemID:%d", 
                m_dwCID, lpItem->GetPrototypeID(), lpPrevItem->GetPrototypeID());
			m_Inventory.DumpItemInfo();
			return false;
		}

		if (lpPrevItem->GetNumOrDurability() + lpItem->GetNumOrDurability() > lpPrevItem->GetMaxNumOrDurability()) 
		{ 
			ERRLOG1(g_Log, "CID:0x%08x 스택할 수 없습니다. : "
				"스택할 수 있는 최대 아이템 개수 이상을 스택하려 하였습니다.", m_dwCID);
			m_Inventory.DumpItemInfo();
			return false;
		}

		// edith 2008.01.10 인벤에 있는 중복아이템을 먹었을때 퀘스트 트리거 발동
//		if (false == m_bAdminToolFlag)
		{
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
			if (NULL == lpCharacter)
			{
				return false;
			}

			// 퀘스트 트리거 발동
			short wCount = (true == lpItem->IsSet(Item::DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;
			lpCharacter->CheckTrigger(Quest::TRIGGER_PICK, lpItem->GetPrototypeID(), Position(), wCount);
		}

		lpPrevItem->SetNumOrDurability(lpPrevItem->GetNumOrDurability() + lpItem->GetNumOrDurability());
		lpItem->SetNumOrDurability(0);
	}

	return true;
}


Item::CItem* CCharacter::Drop(ItemPos SrcPos, unsigned char cNum)
{
    Item::CItemContainer* lpSrcContainer = GetItemContainer(SrcPos.m_cPos);

    if (NULL == lpSrcContainer)
    {
        ERRLOG2(g_Log, "CID:0x%08x SrcPos가 잘못되었습니다. SrcPos:%d", m_dwCID, SrcPos.m_cPos);
        return NULL;
    }

	Item::CItem* lpItem = GetItem(SrcPos);
	if (NULL == lpItem)
	{
		ERRLOG3(g_Log, "CID:0x%08x 아이템을 얻어와서 Drop할 수 없습니다 (%d, 0x%04x)", 
            m_dwCID, SrcPos.m_cPos, SrcPos.m_cIndex);
        lpSrcContainer->DumpItemInfo();
		return NULL;
	}

	// 룬 아이템이 장착 되어 있으면 스킬 포인트 체크.
	bool bChk = CheckItemRuneSkill(SrcPos, Item::ItemPos(0, 0), false);

	if(!bChk)
	{
		return NULL;
	}

	if (lpItem->IsSet(DetailData::STACKABLE) && cNum < lpItem->GetNumOrDurability())
	{
		// 이전 아이템 개수를 줄임		
		lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - cNum);

        // 새 아이템을 생성한다.
        unsigned short usProtoTypeID = lpItem->GetPrototypeID();
		lpItem = CItemFactory::GetInstance().CreateItem(usProtoTypeID);
		if (NULL == lpItem) 
		{
			ERRLOG2(g_Log, "CID:0x%08x 아이템을 나누어서 Drop할 수 없습니다. %d종류의 아이템을 생성할 수 없습니다.",
                m_dwCID, usProtoTypeID);
			return NULL;
		}

		// 새 아이템 개수를 Set함
		lpItem->SetNumOrDurability(cNum);		
	}
	else 
	{
		if (false == RemoveItem(SrcPos)) 
		{
			ERRLOG3(g_Log, "CID:0x%08x 아이템 위치를 지울 수 없습니다 (%d, 0x%04x)" ,
                m_dwCID, SrcPos.m_cPos, SrcPos.m_cIndex);
            lpSrcContainer->DumpItemInfo();
			return NULL;
		}
	}
 
	return lpItem;
}

bool CCharacter::CheckItemRuneSkill(Item::ItemPos SrcPos, Item::ItemPos DstPos, bool bChk)
{
	// 룬 아이템이 장착 되어 있으면 스킬 포인트 체크.
	bool bEquip = GetEquipments().GetItem(SrcPos) == NULL ? false : true;

	if(bEquip)
	{
		Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(GetItem(SrcPos));

		if(lpEquipment)
		{
			return lpEquipment->GetRuneCheckSkill(this, DstPos, bChk);			
		}
	}

	return true;
}

using namespace Broadcast2nd;
const CNetworkEquipGrade CCharacter::GetNetworkEquipGrade(void)
{
	CNetworkEquipGrade equipGrade(
        CNetworkEquipGrade::TransferNetworkEquipGrade(m_Equipments.GetItem(ItemPos(TakeType::TS_EQUIP, Item::EquipmentPos::HELM))), 
        CNetworkEquipGrade::TransferNetworkEquipGrade(m_Equipments.GetItem(ItemPos(TakeType::TS_EQUIP, Item::EquipmentPos::ARMOUR))), 
        CNetworkEquipGrade::TransferNetworkEquipGrade(m_Equipments.GetRightHand()), 
        CNetworkEquipGrade::TransferNetworkEquipGrade(m_Equipments.GetLeftHand()));

	return equipGrade;
}

bool CCharacter::GiveItem(Item::CItem* lpItem)
{
	Item::CItem* lpPrevItem = NULL;
	unsigned char cNum = (lpItem->IsSet(DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;

	Item::ItemPos itemPos = m_Inventory.GetBlankPos(lpItem->GetPrototypeID(), cNum, true, &lpPrevItem);
	if (TakeType::TS_INVEN == itemPos.m_cPos)
	{
		if (true == Pickup(lpItem, itemPos))
		{
			bool bStack = (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability()) ? true : false;

			if (NULL != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharPickUp(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
					0, 0, (true == bStack) ? NULL : lpItem, itemPos, cNum, PktBase::NO_SERVER_ERR);
			}

			return true;
		}
	}
	else
	{
		// 인벤토리에 자리가 없으므로 임시 인벤토리에 넣어준다.
		return SetItem(Item::ItemPos(TakeType::TS_TEMPINVEN, 0), lpItem);
	}

	return false;
}

bool CCharacter::TestItem(Item::CItem* lpItem)
{
	Item::CItem* lpPrevItem = NULL;
	unsigned char cNum = (lpItem->IsSet(DetailData::STACKABLE)) ? lpItem->GetNumOrDurability() : 1;

	Item::ItemPos itemPos = m_Inventory.GetBlankPos(lpItem->GetPrototypeID(), cNum, true, &lpPrevItem);
	
	if (TakeType::TS_NONE != itemPos.m_cPos)
	{
		if(!m_Inventory.TestItem(itemPos, lpItem->GetPrototypeID(), cNum))
		{
			goto lb_fail;
		}
		else
		{
			return true;
		}
	}
	else
	{

lb_fail:

		return (m_TempInven.IsFull()) ? false : true;		
	}

	return false;
}


bool CCharacter::UseLottery(unsigned short usItemID)
{
	CLotteryEvent& LotteryEvent = CGameEventMgr::GetInstance().GetLotteryEvent();

	Item::CItem* lpItem = LotteryEvent.PrizeLottery(usItemID);

	if (NULL == lpItem)
	{
		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharLotteryResult(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				ItemPos(0, 0), NULL, PktLotteryResult::FAIL_BLANK);
		}
		return true;
	}

	Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
	if (NULL != lpEquip)
	{
		lpEquip->SetNewEquip();
	}

	if (false == GiveItem(lpItem))
	{
		ERRLOG3(g_Log, "CID:0x%08x 복권 위치에 상품을 놓는데 실패하였습니다. 복권:%d, 상품:%d" ,
			m_dwCID, usItemID, lpItem->GetPrototypeID());
		return false;
	}

	GAMELOG::LogUseLottery(*this, lpItem, 0);

	// GievItem 으로 스택된 경우
	if (lpItem->IsSet(DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
	{
		DELETE_ITEM(lpItem);
	}
	return true;
}

bool CCharacter::UseCashItem(unsigned long dwSender, unsigned long dwReceiver, Item::ItemPos itemPos, unsigned short wCashType, unsigned short& wError)
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (0 == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "에이전트 얻기 실패.");
		wError = PktUI::SERVER_ERROR;
		return false;
	}

	return GameClientSendPacket::SendCharUseCashItem(lpDBAgentDispatch->GetSendStream(), 
		dwSender, dwReceiver, itemPos, (unsigned char)wCashType, 0);
}

bool CCharacter::UseStartKit(unsigned short wObjectType, unsigned short& wError)
{
	switch (wObjectType)
	{
		case Siege::CAMP:
		{			
			/*if (GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
			{
				ERRLOG1(g_Log, "CID:%10u 길드 요새를 건설할 수 없는 시간에 요새 구축을 시도합니다.", m_dwCID);
				wError = PktUI::FAIL_NOT_GT_GUILD_WAR_TIME;
				return false;
			}*/

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u 길드 요새를 건설하려는 위치가 존 영역 밖입니다. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}

			if (SERVER_ID::CAPITAL != CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_CAPITAL_ZONE;
				return false;
			}

/*
			if (SERVER_ID::STONE_WAR1 <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				wError = PktUI::FAIL_STONE_BATTLE;
				return false;
			}

			if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_ALMIGHTY_GROUND;
				return false;
			}

			if (SERVER_ID::ZONE9 == CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_NEWZONE;
				return false;
			}
*/
			if (false == CCellManager::GetInstance().IsCampCreateArea(GetCurrentPos()))
			{
				ERRLOG4(g_Log, "CID:%10u 길드 요새를 건설하려는 위치가 존 영역 밖입니다. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}
			
			if (true == CCellManager::GetInstance().IsSafetyZone(GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:%10u 길드 요새를 세울 수 없는 곳에 길드 요새를 지을려고 합니다.", m_dwCID);
				wError = PktUI::IN_SAFETYZONE;
				return false;
			}

			if (0 == GetGID())
			{
				ERRLOG1(g_Log, "CID:%10u 길드에 가입하지 않고서 길드 요새 생성 스타터킷을 사용하려 합니다.", m_dwCID);
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
			if (NULL == lpGuild)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u 존재하지 않는 길드의 마스터가 길드 요새 생성 스타터킷을 사용하려 합니다.", m_dwCID, GetGID());
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			if (m_dwCID != lpGuild->GetMaster().m_dwCID)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u 길드의 마스터가 아닌자가 길드 요새 생성 스타터킷을 사용하려 합니다.", m_dwCID, GetGID());
				wError = PktUI::NOT_MASTER;
				return false;
			}

			if(GetAdminLevel() != 3)
			{

				unsigned long dwFameForCampBuilding = 
					(SERVER_ID::GROUP_TEST == CServerSetup::GetInstance().GetServerGroup()) ? 
					Siege::FAME_FOR_CAMP_BUILDING_TEST : Siege::FAME_FOR_CAMP_BUILDING;

				if (lpGuild->GetFame() < dwFameForCampBuilding)
				{
					ERRLOG2(g_Log, "CID:%10u Fame:%d 명성치가 부족한 길드 마스터가 길드 요새를 생성하려 합니다.", m_dwCID, lpGuild->GetFame());
					wError = PktUI::NOT_ENOUGH_FAME;
					return false;
				}

			}

			if (true == CSiegeObjectMgr::GetInstance().ExistBuildingOrDestroyingCamp(lpGuild->GetGID()))
			{
				ERRLOG1(g_Log, "CID:%10u 구축중, 취소중, 파괴중인 다른 길드 요새가 있습니다.", m_dwCID);
				wError = PktUI::ANOTHER_DEV_CAMP;
				return false;
			}
			
			if (true == Castle::CCastleMgr::GetInstance().ExistCastleInRadius( GetCurrentPos() ) ||
				true == CSiegeObjectMgr::GetInstance().ExistCampInRadius( GetCurrentPos() ))
			{
				ERRLOG1(g_Log, "CID:%10u 길드 요새를 지을려는 위치가 다른 길드 요새나 성 상징물의 반경 안쪽입니다.", m_dwCID);
				wError = PktUI::INNER_RADIUS;
				return false;
			}

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "에이전트 얻기 실패.");
				wError = PktUI::SERVER_ERROR;
			}
			else
			{
				return GameClientSendPacket::SendCharCreateCamp(lpDBAgentDispatch->GetSendStream(), GetCID(), GetGID(), Siege::CAMP, GetCurrentPos());
			}
		}
		break;

		case Siege::KARTERANT_WEAPON:
		case Siege::MERKADIA_WEAPON:
		{
			/*if (GameTime::GT_GUILD_WAR != CGameTimeMgr::GetInstance().GetCurrentGuildTime())
			{
				ERRLOG1(g_Log, "CID:%10u 월드 웨폰을 건설할 수 없는 시간에 월드 웨폰 구축을 시도합니다.", m_dwCID);
				wError = PktUI::FAIL_NOT_GT_GUILD_WAR_TIME;
				return false;
			}*/

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u 길드 요새를 건설하려는 위치가 존 영역 밖입니다. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}

			if (SERVER_ID::CAPITAL != CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_CAPITAL_ZONE;
				return false;
			}
/*
			if (SERVER_ID::STONE_WAR1 <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
			{
				wError = PktUI::FAIL_STONE_BATTLE;
				return false;
			}

			if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_ALMIGHTY_GROUND;
				return false;
			}

			if (SERVER_ID::ZONE9 == CServerSetup::GetInstance().GetServerZone())
			{
				wError = PktUI::FAIL_NEWZONE;
				return false;
			}
*/
			if (false == CSiegeObjectMgr::GetInstance().EnableCreateWorldWeapon(GetNation(), wError))
			{
				ERRLOG1(g_Log, "CID:%10u 월드 웨폰을 지을수 없습니다.", m_dwCID);
				return false;
			}

			if (false == CCellManager::GetInstance().IsCampCreateArea(GetCurrentPos()))
			{
				ERRLOG4(g_Log, "CID:%10u 길드 요새를 건설하려는 위치가 존 영역 밖입니다. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}

			if (true == CCellManager::GetInstance().IsSafetyZone(GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:%10u 월드 웨폰을 세울 수 없는 곳에 월드 웨폰을 지을려고 합니다.", m_dwCID);
				wError = PktUI::IN_SAFETYZONE;
				return false;
			}

			if (0 == GetGID())
			{
				ERRLOG1(g_Log, "CID:%10u 길드에 가입하지 않고서 월드 웨폰 생성 스타터킷을 사용하려 합니다.", m_dwCID);
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
			if (NULL == lpGuild)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u 존재하지 않는 길드의 마스터가 월드 웨폰 생성 스타터킷을 사용하려 합니다.", m_dwCID, GetGID());
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			if (m_dwCID != lpGuild->GetMaster().m_dwCID)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u 길드의 마스터가 아닌자가 월드 웨폰 생성 스타터킷을 사용하려 합니다.", m_dwCID, GetGID());
				wError = PktUI::NOT_MASTER;
				return false;
			}

			const unsigned long dwFameForCampBuilding = 
				(SERVER_ID::GROUP_TEST == CServerSetup::GetInstance().GetServerGroup()) ? 
					Siege::FAME_FOR_WEAPON_BUILDING_TEST : Siege::FAME_FOR_WEAPON_BUILDING;

			if (lpGuild->GetFame() < dwFameForCampBuilding)
			{
				ERRLOG2(g_Log, "CID:%10u Fame:%d 명성치가 부족한 길드 마스터가 월드 웨폰을 생성하려 합니다.", m_dwCID, lpGuild->GetFame());
				wError = PktUI::NOT_ENOUGH_FAME;
				return false;
			}

			if (true == CSiegeObjectMgr::GetInstance().ExistBuildingOrDestroyingCamp(lpGuild->GetGID()))
			{
				ERRLOG1(g_Log, "CID:%10u 구축중, 취소중, 파괴중인 다른 길드 요새가 있습니다.", m_dwCID);
				wError = PktUI::ANOTHER_DEV_CAMP;
				return false;
			}

			if (true == Castle::CCastleMgr::GetInstance().ExistCastleInRadius( GetCurrentPos() ) ||
				true == CSiegeObjectMgr::GetInstance().ExistCampInRadius( GetCurrentPos() ))
			{
				ERRLOG1(g_Log, "CID:%10u 월드 웨폰을 지을려는 위치가 다른 길드 요새나 성 상징물의 반경 안쪽입니다.", m_dwCID);
				wError = PktUI::INNER_RADIUS;
				return false;
			}

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "에이전트 얻기 실패.");
				wError = PktUI::SERVER_ERROR;
			}
			else
			{
				// 동시에 두개의 생성 처리를 막기 위해서...
				CSiegeObjectMgr::GetInstance().RequestWorldWeaponCreation();

				return GameClientSendPacket::SendCharCreateCamp(lpDBAgentDispatch->GetSendStream(), GetCID(), GetGID(), wObjectType, m_CurrentPos);
			}
		}
		break;

		case Siege::SHORT_RANGE_SIEGE_ARMS:
		case Siege::LONG_RANGE_SIEGE_ARMS:
		case Siege::AIRSHIP:
		{
			// 자재 가져오기
			CItem* lpMaterial = GetItem(ItemPos(TakeType::TS_EXTRA, ExtraSpacePos::KIT_MATERIAL_POS));

			if (NULL == lpMaterial)
			{
				ERRLOG1(g_Log, "CID:0x%08x 공성 병기를 만들려는 자재가 없습니다.", m_dwCID);
				wError = PktUI::NOT_ENOUGH_MATERIAL;
				goto lb_fail;
			}

			// edith 2008.08.09 현재 공선전을 지정하지 않았기 때문에 아래를 임시로 ZONE3번으로 
			// 지정했다. 후에 공선전은 다른 존으로 지정한다.
			if(CServerSetup::GetInstance().GetServerZone()!=SERVER_ID::CAPITAL)
			{
				ERRLOG1(g_Log, "CID:%10u 카나번존이 아닙니다.", m_dwCID);
				goto lb_fail;
			}

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u 공성 병기를 만들려고 하는 위치가 존 영역 밖입니다. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				goto lb_fail;
			}

			if ( GameTime::ST_SIEGE_WAR != CGameTimeMgr::GetInstance().GetCurrentSiegeTime() )
			{
				ERRLOG1(g_Log, "CID:0x%08x 공성 시간이 아닌 상황에서 공성 병기를 만들려고 합니다.", m_dwCID);
				wError = PktUI::NOT_SIEGE_TIME;
				goto lb_fail;
			}

			if (false == CCastleMgr::GetInstance().ExistSiegeInRadius( GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:0x%08x 영역 밖에서 공성 병기를 생성하려고 했습니다.", m_dwCID);
				wError = PktUI::OUTER_RADIUS_SIEGE; 
				goto lb_fail;
			}
			   
			if(CSiegeObjectMgr::GetInstance().HasSiegeArms(GetCID()))
			{
				wError = PktUI::FAIL_HAS_SIEGE_ARMS;
				goto lb_fail;
			}

			// 수성 병기 체크.
			if(CCastleMgr::GetInstance().HasCastleArms(GetCID()))
			{
				wError = PktUI::FAIL_HAS_SIEGE_ARMS;
				goto lb_fail;
			}

			// 길드 마스터 체크.
			if(!GetGID())
			{
				wError = PktUI::NOT_GUILD_MASTER;
				goto lb_fail;
			}
			else
			{
				CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());

				if(lpGuild)
				{
					MemberInfo memberInfo = lpGuild->GetMaster();

					if(memberInfo.m_dwCID!=GetCID())
					{
						wError = PktUI::NOT_GUILD_MASTER;
						goto lb_fail;
					}					
				}
			}

			// Level 체크.
			if(GetLevel()<Siege::CREATE_LEVEL_LIMIT)
			{
				wError = PktUI::NOT_LIMIT_LEVEL;
				goto lb_fail;
			}

			// 수성측은 
			if(CCastleMgr::GetInstance().GetNation()==GetNation())
			{
				wError = PktUI::NOT_CASTLE_NATION;
				goto lb_fail;
			}

			// 공성 병기가 10개 미만일때.
			if(lpMaterial->GetNumOrDurability()<Siege::SIEGE_ARMS_UPGRADE_MATERIAL_NUM)
			{
				wError = PktUI::FAIL_MATERIAL_COUNT;
				goto lb_fail;
			}

			if(CSiegeObjectMgr::GetInstance().GetSiegeObjectCount()>=MAX_SIEGE_OBJECT)
			{
				wError = PktUI::FAIL_MAX_SIEGE_OBJECT;
				goto lb_fail;
			}

lb_fail:
			if(wError)
			{
				if(lpMaterial)
				{
					RemoveItem(lpMaterial->GetPos());
					GiveItem(lpMaterial);
				}
				return false;
			}

			unsigned char cUpgradeStep = 0;
            
			int nRemain = lpMaterial->GetNumOrDurability() - Siege::SIEGE_ARMS_UPGRADE_MATERIAL_NUM;

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "에이전트 얻기 실패.");
				wError = PktUI::SERVER_ERROR;
			}
			else
			{
				if (GameClientSendPacket::SendCharCreateSiegeArms(lpDBAgentDispatch->GetSendStream(), 
                    GetCID(), GetNation(), wObjectType, cUpgradeStep, m_CurrentPos))
				{
					lpMaterial->SetNumOrDurability(nRemain);

					if (0 == nRemain)
					{
						if (!RemoveItem(lpMaterial->GetPos()))
						{
							ERRLOG1(g_Log, "CID:0x%08x 공성 병기 스타트킷 사용시 다 사용한 자재 제거에 실패했습니다.", m_dwCID);
							wError = PktUI::SERVER_ERROR;
						}
						else
						{
							DELETE_ITEM(lpMaterial);
						}
					}
					else
					{
						// 남은 아이템 인벤토리로 이동.
						RemoveItem(lpMaterial->GetPos());
						GiveItem(lpMaterial);												
					}

					return true;
				}
			}
		}
		break;
	}

	return false;
}


bool CCharacter::GiveItemByDBAgentRequest(GiveItemInfo& giveItemInfo)
{
    Item::CItemMgr&     itemMgr     = Item::CItemMgr::GetInstance();
    Item::CItemFactory& itemFactory = Item::CItemFactory::GetInstance();

    Item::ItemPos itemPos = Item::ItemPos(TakeType::TS_TEMPINVEN, 0);
    
    GivenItemList::iterator end = m_GivenItemList.end();
    GivenItemList::iterator pos = 
        std::lower_bound(m_GivenItemList.begin(), end, giveItemInfo.m_dwCreateID);

    // 이미 지급한 아이템이 아닐 경우 지급
    if (pos == end || *pos != giveItemInfo.m_dwCreateID)
    {
        Item::CItem* lpItem = CreateItemFromGiveInfo(itemMgr, itemFactory, giveItemInfo, itemPos);

        if (0 != lpItem)
        {
            if (GetTempInven().SetItem(itemPos, lpItem))
            {
                giveItemInfo.m_dwItemUID = lpItem->GetUID();
                m_GivenItemList.insert(pos, giveItemInfo.m_dwCreateID);
                return true;
            }
            else
            {
                DELETE_ITEM(lpItem);
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / CreationID:%10u / 아이템 지급 : 아이템 생성 실패", 
                giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / CreationID:%10u / 아이템 지급 : 이미 지급한 아이템이라 더 지급할 수 없습니다", 
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID);
    }

    return false;
}

