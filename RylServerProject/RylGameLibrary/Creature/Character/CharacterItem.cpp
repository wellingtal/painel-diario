
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

		// ���� ���� ������Ʈ
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

    ERRLOG2(g_Log, "CID:0x%08x �κ��丮 �� �����÷ο찡 �߻��߽��ϴ�. : %dGold", m_dwCID, dwGold);
    return false;
}

bool CCharacter::DeductGold(unsigned long dwGold, bool bNotice)
{
    if (dwGold <= m_DBData.m_Info.Gold)
    { 
        m_DBData.m_Info.Gold -= dwGold;

		// ���� ���� ������Ʈ
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
    
    ERRLOG3(g_Log, "CID:0x%08x �κ��丮 �� ����÷ο찡 �߻��߽��ϴ�. : ���� %dGold �ʿ� %dGold", m_dwCID, m_DBData.m_Info.Gold, dwGold);
    return false;
}

bool CCharacter::DeductMileage(unsigned long dwMileage, bool bNotice)
{
	if (dwMileage <= m_DBData.m_Info.Mileage)
	{ 
		m_DBData.m_Info.Mileage -= dwMileage;
		return true;
	}

	ERRLOG3(g_Log, "CID:0x%08x �κ��丮 �޴� ����÷ο찡 �߻��߽��ϴ�. : ���� %dMileage �ʿ� %dMileage", m_dwCID, m_DBData.m_Info.Mileage, dwMileage);
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
        ERRLOG3(g_Log, "CID:0x%08x SrcPos�Ǵ� DstPos�� �߸��Ǿ����ϴ�. SrcPos:%d, DstPos:%d",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
        return false;
    }

	if (TakeType::TS_TEMPINVEN == takeType.m_srcPos.m_cPos)
	{
		if (TakeType::TS_QSLOT == takeType.m_dstPos.m_cPos || 
			TakeType::TS_STALL == takeType.m_dstPos.m_cPos)
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : �ӽ� �κ��丮���� �߸��� ������ �������� �ű�� �մϴ�. "
				"(%d ���� %d �� �̵�)", m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
			return false;
		}
	}

	if (TakeType::TS_TEMPINVEN == takeType.m_dstPos.m_cPos)
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : �ӽ� �κ��丮�� �������� �ű�� �մϴ�. (%d ���� %d �� �̵�)", 
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
		return false;
	}

	// Src���� �������� ���´�.
	Item::CItem* lpSrcItem = GetItem(takeType.m_srcPos);
	if (NULL == lpSrcItem) 
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : (%d, 0x%04x) ���� �������� ���� �� �����ϴ�.",
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

        lpSrcContainer->DumpItemInfo();		
        return false;
	}

	// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ.
	if(bChk)
	{
		bool bResult = CheckItemRuneSkill(takeType.m_srcPos, Item::ItemPos(0, 0), false);

		if(!bResult)
		{
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// edith 2009.02.28 ������ ��ȯ�� �κ����� �������� ���� ��ȯâ�� �����۰� Swip�Ҷ� �����
	// ���׸� �����ϱ� ���� �Ʒ� ������ ���ø��Լ��� �߰���.

	BOOL bOri = FALSE;
	// �켱 �������̰�, Ÿ���� Ʋ����, ������ Ʋ������ ���� ó���ϰ� �Ѵ�.
	if(lpSrcItem->IsSet(Item::DetailData::STACKABLE) && takeType.m_cNum != lpSrcItem->GetNumOrDurability())
	{
		// Dst���� �������� ���´�.
		Item::CItem* lpDstItem = GetItem(takeType.m_dstPos);
		// ���� Dest ���� �ְ� �ҽ��� ���� ����� �������̶�� ��ġ�� �����ϼ��� �ִ�.
		if(lpDstItem && lpSrcItem->GetPrototypeID() == lpDstItem->GetPrototypeID())
		{
			bOri = TRUE;
		}
		else
		{
			// �������� ������.
			if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum)
			{
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u ������ �̵� ���� : Source�� �ִ� �������� ������ �������� �ϴ� �������� �������� �۰ų� �����ϴ�."
					"Source�� ���� ������ ���� %d, �������� �ϴ� �������� ���� %d",
					m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum));		        
				return false;
			}
			else
			{
				// ������ ������.
				Item::CItem* lpSplitItem = Item::CItemFactory::GetInstance().CreateItem(lpSrcItem->GetItemInfo());
				if (NULL == lpSplitItem) 
				{
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u ������ �̵� ���� : �������� ������ �� �����ϴ�.", m_dwCID));
					return false;
				}

				lpSplitItem->SetNumOrDurability(takeType.m_cNum);

				// �ڸ��� ��� ������ ������ �̵�
				if (NULL == lpDstItem)
				{	
					if (false == SetItem(takeType.m_dstPos, lpSplitItem))		
					{
						ERRLOG5(g_Log, "CID:0x%08x ������ �̵� ���� : Ŭ���̾�Ʈ�� ��(%d, 0x%04x) ��ġ�� Set�� �� �����ϴ�."
							" ����ID:%d, ������ ������(����):%d",
							m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex, 
							lpSrcItem->GetPrototypeID(), lpSrcItem->GetNumOrDurability());

						DELETE_ITEM(lpSplitItem);

						goto MOVE_ROLLBACK;
					}

					// ���� ����
					lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);
				}
				else
				{
					ERRLOG5(g_Log, "CID:0x%08x ������ �̵� ���� : Ŭ���̾�Ʈ�� ��(%d, 0x%04x) ��ġ�� Set�� �� �����ϴ�."
						" ����ID:%d, ������ ������(����):%d",
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
		// �ڽ��� ��ġ�� ��ġ�� ���� �����ϴ� ���� �����ϱ� ���� �����صд�.
		if (false == RemoveItem(takeType.m_srcPos))
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : (%d, 0x%04x) ���� �������� ������ �� �����ϴ�.",
				m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
	        
			lpSrcContainer->DumpItemInfo();
			return false;
		}

		// Dst���� �������� ���´�.
		Item::CItem* lpDstItem = GetItem(takeType.m_dstPos);

		// �ڸ��� ��� ������ ������ �̵�
		if (NULL == lpDstItem)
		{	
			if (false == SetItem(takeType.m_dstPos, lpSrcItem))		
			{
				ERRLOG5(g_Log, "CID:0x%08x ������ �̵� ���� : Ŭ���̾�Ʈ�� ��(%d, 0x%04x) ��ġ�� Set�� �� �����ϴ�."
					" ����ID:%d, ������ ������(����):%d",
					m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex, 
					lpSrcItem->GetPrototypeID(), lpSrcItem->GetNumOrDurability());
				goto MOVE_ROLLBACK;
			}
		}
		// ������ ���ɼ��� �ִ�.
		else
		{	
			if (TakeType::TS_STALL == takeType.m_dstPos.m_cPos)
			{
				ERRLOG5(g_Log, "CID:0x%08x ������ �̵� ���� : ������ �� ���� ��ġ. SrcPos : (%d, 0x%04x), DstPos : (%d, 0x%04x)", 
					m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
				goto MOVE_ROLLBACK;
			}

			if (false == lpSrcItem->IsSet(DetailData::STACKABLE) || !lpDstItem->IsSet(DetailData::STACKABLE))
			{
				ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : ������ �� ���� ������. SrcItem�� ����ID %d, DstItem�� ����ID %d", 
					m_dwCID, lpSrcItem->GetPrototypeID(), lpDstItem->GetPrototypeID());
				goto MOVE_ROLLBACK;
			}
				
			if (lpSrcItem->GetPrototypeID() != lpDstItem->GetPrototypeID())  
			{
				ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : ������ �ٸ� ������ ���� �÷� ���ҽ��ϴ�."
					"SrcItem�� ����ID %d, DstItem�� ����ID %d", m_dwCID, lpSrcItem->GetPrototypeID(), lpDstItem->GetPrototypeID());				
				goto MOVE_ROLLBACK;
			}

			if (lpSrcItem->GetNumOrDurability() < takeType.m_cNum) 
			{
				ERRLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : Source�� �ִ� �������� ������ �ű���� �ϴ� �������� �������� �۽��ϴ�."
					"SrcItem���� %d, �ű���� �� ���� %d", m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum);
				goto MOVE_ROLLBACK;
			}

			if (lpDstItem->GetNumOrDurability() + takeType.m_cNum > lpDstItem->GetMaxNumOrDurability()) 
			{
				ERRLOG4(g_Log, "CID:0x%08x ������ �̵� ���� : �ִ� ���� �̻� �������� �÷� ���� �� �����ϴ�"
					" ���� �ִ� �������� ���� %d, �������� �� �������� ���� %d, ������ �ִ� �ִ� ���� %d",
					m_dwCID, lpDstItem->GetNumOrDurability(), takeType.m_cNum, lpDstItem->GetMaxNumOrDurability());
				goto MOVE_ROLLBACK;
			}

			// ������ ���� ����
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
				// �̹� Remove�� �������̹Ƿ� ������ ��� ����.
				DELETE_ITEM(lpSrcItem);
			}
			else 
			{	
				// ���� �ִ� �������� �ٽ� Set�Ѵ�.
				if (!SetItem(takeType.m_srcPos, lpSrcItem))
				{
					SERLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : (%d, 0x%04x) ���ڸ��� ���� ���� �� ����. �������� ������.",
						m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

					lpSrcContainer->DumpItemInfo();
					SERLOG0(g_Log, "Src������ ���� ����");
					
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
		SERLOG3(g_Log, "CID:0x%08x ������ �̵� ���� : (%d, 0x%04x) ���ڸ��� ���� ���� �� ����. �������� ������.",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);

        lpSrcContainer->DumpItemInfo();
        SERLOG0(g_Log, "Src������ ���� ����");
        
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
        ERRLOG3(g_Log, "CID:0x%08x SrcPos�Ǵ� DstPos�� �߸��Ǿ����ϴ�. SrcPos:%d, DstPos:%d",
            m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_dstPos.m_cPos);
        return false;
    }

	if (TakeType::TS_TEMPINVEN == takeType.m_srcPos.m_cPos)
	{
		ERRLOG1(g_Log, "CID:0x%08x ������ ���ø� ���� : �ӽ� �κ��丮�� �������� ���ø��Ϸ� �մϴ�.", m_dwCID);
		return false;
	}

	// Src���� �������� ���´�.
	Item::CItem* lpSrcItem = GetItem(takeType.m_srcPos);
	if (NULL == lpSrcItem) 
	{ 
		ERRLOG3(g_Log, "CID:0x%08x ������ ���ø� ���� : (%d, 0x%04x) ��ġ���� �������� ���� �� �����ϴ�.",
			m_dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex);
        lpSrcContainer->DumpItemInfo();
		return false;
	}

    if (false == lpSrcItem->IsSet(DetailData::STACKABLE)) 
	{
		ERRLOG2(g_Log, "CID:0x%08x ������ ���ø� ���� : ���� �� ���� ������(���� �Ұ�)�Դϴ�. ���� ID : %d",
			m_dwCID, lpSrcItem->GetPrototypeID());
		return false;
	}
	if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum) 
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ ���ø� ���� : Source�� �ִ� �������� ������ �������� �ϴ� �������� �������� �۰ų� �����ϴ�."
			"Source�� ���� ������ ���� %d, �������� �ϴ� �������� ���� %d",
			m_dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum);
		return false;
	}

	// ������ �����Ѵ�.
	Item::CItem* lpSplitItem = CItemFactory::GetInstance().CreateItem(lpSrcItem->GetPrototypeID());

	if (NULL == lpSplitItem) 
	{
		ERRLOG1(g_Log, "CID:0x%08x ������ ���ø� ���� : �������� ������ �� �����ϴ�.", m_dwCID);
		return false;
	}

	lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);
	lpSplitItem->SetNumOrDurability(takeType.m_cNum);

	if (!SetItem(takeType.m_dstPos, lpSplitItem)) 
	{
		ERRLOG3(g_Log, "CID:0x%08x ������ ���ø� ���� : (%d, 0x%04x)�ɰ��� ���������� �ϴ� �ڸ���, �������� ���� ���� �� �����ϴ�.",
			m_dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
		
        DELETE_ITEM(lpSplitItem);

        lpDstContainer->DumpItemInfo();
		return false;
	}

    // Set�� ������ ���� �����Ѵ�.
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
	������ ������ ��.

	���[�ε���:5] ������A �� �κ�[�ε���:9]�� ������B �� ����	= Src[1, 5] Dst[2, 9] / Src[2, 9] Dst[6, X]
	* ������ A�� ������ B�� �ڸ��� ����, ������ B�� ���콺�� ��� �ִ� ����. �� �ӽ� ������ ������ ��
	���콺 ������B �� �κ�[�ε���:4]�� ������C �� �ٽ� ����		= Src[6, X] Det[2, 4] / Src[2, 4] Dst[6, X]
	���콺 ������C �� �ٽ� �κ�[�ε���:1]�� �ڸ��� ���´�.		= Src[6, X] Det[2, 1] : ������ �ƴ� ������ ����(����)
*/	
	if (TakeType::TS_TEMP != DstTake.m_dstPos.m_cPos) 
	{
		ERRLOG1(g_Log, "CID:0x%08x DstTake�� DstPos�� TS_TEMP�� �ƴմϴ�.", m_dwCID);
		return false;
	}

    Item::ItemPos   HoldPos(TakeType::TS_TEMP, Item::ExtraSpacePos::HOLDITEM_POS);			
	Item::CItem*    lpHoldItem = m_ExtraSpace.GetItem(HoldPos);

	// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ.
	bool bChk = CheckItemRuneSkill(SrcTake.m_srcPos, DstTake.m_srcPos, true);

	if(!bChk)
	{
		return false;
	}	

	if (NULL == lpHoldItem) 
	{
        // �ӽ� ������ �������� ����. �ӽ� �������� ������ ��� ����.
		if (false == MoveItem(DstTake, false))
		{
			ERRLOG1(g_Log, "CID:0x%08x �ӽ� �������� �������� �ű� �� �����ϴ�", m_dwCID);
			return false;
		}

        // �������� ���� ���´�.
		if (false == MoveItem(SrcTake, false))
		{
			ERRLOG1(g_Log, "CID:0x%08x SrcTake�� �����߽��ϴ�", m_dwCID);
            
            // ������ Swap�� RollBack�մϴ�
			if (false == MoveItem(TakeType(DstTake.m_dstPos, DstTake.m_srcPos, DstTake.m_cNum)))
			{
				ERRLOG1(g_Log, "CID:0x%08x ������ ���� RollBack�� �����߽��ϴ�", m_dwCID);
			}

			return false;
		}
	}
	else
	{
		// �̹� �ӽ� ������ �������� ����. �� ���� SrcTake.m_srcPos.m_cPos�� TS_TEMP���� �Ѵ�.
		if (TakeType::TS_TEMP != SrcTake.m_srcPos.m_cPos)
		{
			ERRLOG1(g_Log, "CID:0x%08x �̹� �ӽ� ������ �������� �ִµ�, SrcTake�� SrcPos�� TS_TEMP�� �ƴմϴ�.", m_dwCID);
			return false;
		}

        // �ӽ� �������� ������ ����.
		if (false == m_ExtraSpace.RemoveItem(HoldPos)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x �ӽ� �������� �������� �����ϴ� �� �����߽��ϴ�.", m_dwCID);
			return false;
		}

        // �ӽ� �������� ������ �ű�
		if (false == MoveItem(DstTake)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x �ӽ� �������� �������� �ű� �� �����ϴ�", m_dwCID);

			if (false == m_ExtraSpace.SetItem(HoldPos, lpHoldItem))
			{
				ERRLOG1(g_Log, "CID:0x%08x �ӽ� ���� ������ RollBack�� �����߽��ϴ�.", m_dwCID);
			}

			return false;
		}

        // �ӽ� �������� ���´� �������� �������´�.
		if (false == SetItem(SrcTake.m_dstPos, lpHoldItem)) 
		{
			ERRLOG1(g_Log, "CID:0x%08x �ӽ� ������ �ִ� �������� �ű� �� �����ϴ�", m_dwCID);

			if (false == MoveItem(TakeType(DstTake.m_dstPos, DstTake.m_srcPos, DstTake.m_cNum)))
			{
				ERRLOG1(g_Log, "CID:0x%08x ������ ���� RollBack�� �����߽��ϴ�", m_dwCID); 
			}

			if (false == m_ExtraSpace.SetItem(HoldPos, lpHoldItem)) 
			{
				ERRLOG1(g_Log, "CID:0x%08x �ӽ� ���� ������ RollBack�� �����߽��ϴ�.", m_dwCID);
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
        ERRLOG1(g_Log, "CID:0x%08x �������� NULL�Դϴ�.", m_dwCID);
        return false;
    }

	Item::CItem* lpPrevItem = m_Inventory.GetItem(dstPos);

	if (NULL == lpPrevItem) 
	{
        // ���� �������� ���ڸ��� ���´�.
		if (false == SetItem(dstPos, lpItem)) 
		{
			ERRLOG3(g_Log, "CID:0x%08x �������� ��� ���� �� �����߽��ϴ�. (%d, 0x%04x)",
                m_dwCID, dstPos.m_cPos, dstPos.m_cIndex);
            m_Inventory.DumpItemInfo();
			return false;
		}		
	}
	else 
	{	
        // �̹� �ִ� �ڸ��� �������� �����Ѵ�.
		if (false == lpPrevItem->IsSet(DetailData::STACKABLE)) 
		{
            ERRLOG3(g_Log, "CID:0x%08x ������ �� ���� �������Դϴ�. ItemID:%d, PrevItemID:%d", 
                m_dwCID, lpItem->GetPrototypeID(), lpPrevItem->GetPrototypeID());
			m_Inventory.DumpItemInfo();
			return false;
		}

		if (lpPrevItem->GetPrototypeID() != lpItem->GetPrototypeID()) 
		{ 
            ERRLOG3(g_Log, "CID:0x%08x ���� ID�� �ٸ��ϴ�. ItemID:%d, PrevItemID:%d", 
                m_dwCID, lpItem->GetPrototypeID(), lpPrevItem->GetPrototypeID());
			m_Inventory.DumpItemInfo();
			return false;
		}

		if (lpPrevItem->GetNumOrDurability() + lpItem->GetNumOrDurability() > lpPrevItem->GetMaxNumOrDurability()) 
		{ 
			ERRLOG1(g_Log, "CID:0x%08x ������ �� �����ϴ�. : "
				"������ �� �ִ� �ִ� ������ ���� �̻��� �����Ϸ� �Ͽ����ϴ�.", m_dwCID);
			m_Inventory.DumpItemInfo();
			return false;
		}

		// edith 2008.01.10 �κ��� �ִ� �ߺ��������� �Ծ����� ����Ʈ Ʈ���� �ߵ�
//		if (false == m_bAdminToolFlag)
		{
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_dwCID);
			if (NULL == lpCharacter)
			{
				return false;
			}

			// ����Ʈ Ʈ���� �ߵ�
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
        ERRLOG2(g_Log, "CID:0x%08x SrcPos�� �߸��Ǿ����ϴ�. SrcPos:%d", m_dwCID, SrcPos.m_cPos);
        return NULL;
    }

	Item::CItem* lpItem = GetItem(SrcPos);
	if (NULL == lpItem)
	{
		ERRLOG3(g_Log, "CID:0x%08x �������� ���ͼ� Drop�� �� �����ϴ� (%d, 0x%04x)", 
            m_dwCID, SrcPos.m_cPos, SrcPos.m_cIndex);
        lpSrcContainer->DumpItemInfo();
		return NULL;
	}

	// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ.
	bool bChk = CheckItemRuneSkill(SrcPos, Item::ItemPos(0, 0), false);

	if(!bChk)
	{
		return NULL;
	}

	if (lpItem->IsSet(DetailData::STACKABLE) && cNum < lpItem->GetNumOrDurability())
	{
		// ���� ������ ������ ����		
		lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - cNum);

        // �� �������� �����Ѵ�.
        unsigned short usProtoTypeID = lpItem->GetPrototypeID();
		lpItem = CItemFactory::GetInstance().CreateItem(usProtoTypeID);
		if (NULL == lpItem) 
		{
			ERRLOG2(g_Log, "CID:0x%08x �������� ����� Drop�� �� �����ϴ�. %d������ �������� ������ �� �����ϴ�.",
                m_dwCID, usProtoTypeID);
			return NULL;
		}

		// �� ������ ������ Set��
		lpItem->SetNumOrDurability(cNum);		
	}
	else 
	{
		if (false == RemoveItem(SrcPos)) 
		{
			ERRLOG3(g_Log, "CID:0x%08x ������ ��ġ�� ���� �� �����ϴ� (%d, 0x%04x)" ,
                m_dwCID, SrcPos.m_cPos, SrcPos.m_cIndex);
            lpSrcContainer->DumpItemInfo();
			return NULL;
		}
	}
 
	return lpItem;
}

bool CCharacter::CheckItemRuneSkill(Item::ItemPos SrcPos, Item::ItemPos DstPos, bool bChk)
{
	// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ.
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
		// �κ��丮�� �ڸ��� �����Ƿ� �ӽ� �κ��丮�� �־��ش�.
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
		ERRLOG3(g_Log, "CID:0x%08x ���� ��ġ�� ��ǰ�� ���µ� �����Ͽ����ϴ�. ����:%d, ��ǰ:%d" ,
			m_dwCID, usItemID, lpItem->GetPrototypeID());
		return false;
	}

	GAMELOG::LogUseLottery(*this, lpItem, 0);

	// GievItem ���� ���õ� ���
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
		ERRLOG0(g_Log, "������Ʈ ��� ����.");
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
				ERRLOG1(g_Log, "CID:%10u ��� ����� �Ǽ��� �� ���� �ð��� ��� ������ �õ��մϴ�.", m_dwCID);
				wError = PktUI::FAIL_NOT_GT_GUILD_WAR_TIME;
				return false;
			}*/

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u ��� ����� �Ǽ��Ϸ��� ��ġ�� �� ���� ���Դϴ�. X:%f, Y:%f, Z:%f",
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
				ERRLOG4(g_Log, "CID:%10u ��� ����� �Ǽ��Ϸ��� ��ġ�� �� ���� ���Դϴ�. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}
			
			if (true == CCellManager::GetInstance().IsSafetyZone(GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:%10u ��� ����� ���� �� ���� ���� ��� ����� �������� �մϴ�.", m_dwCID);
				wError = PktUI::IN_SAFETYZONE;
				return false;
			}

			if (0 == GetGID())
			{
				ERRLOG1(g_Log, "CID:%10u ��忡 �������� �ʰ� ��� ��� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID);
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
			if (NULL == lpGuild)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u �������� �ʴ� ����� �����Ͱ� ��� ��� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID, GetGID());
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			if (m_dwCID != lpGuild->GetMaster().m_dwCID)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u ����� �����Ͱ� �ƴ��ڰ� ��� ��� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID, GetGID());
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
					ERRLOG2(g_Log, "CID:%10u Fame:%d ��ġ�� ������ ��� �����Ͱ� ��� ����� �����Ϸ� �մϴ�.", m_dwCID, lpGuild->GetFame());
					wError = PktUI::NOT_ENOUGH_FAME;
					return false;
				}

			}

			if (true == CSiegeObjectMgr::GetInstance().ExistBuildingOrDestroyingCamp(lpGuild->GetGID()))
			{
				ERRLOG1(g_Log, "CID:%10u ������, �����, �ı����� �ٸ� ��� ����� �ֽ��ϴ�.", m_dwCID);
				wError = PktUI::ANOTHER_DEV_CAMP;
				return false;
			}
			
			if (true == Castle::CCastleMgr::GetInstance().ExistCastleInRadius( GetCurrentPos() ) ||
				true == CSiegeObjectMgr::GetInstance().ExistCampInRadius( GetCurrentPos() ))
			{
				ERRLOG1(g_Log, "CID:%10u ��� ����� �������� ��ġ�� �ٸ� ��� ����� �� ��¡���� �ݰ� �����Դϴ�.", m_dwCID);
				wError = PktUI::INNER_RADIUS;
				return false;
			}

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "������Ʈ ��� ����.");
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
				ERRLOG1(g_Log, "CID:%10u ���� ������ �Ǽ��� �� ���� �ð��� ���� ���� ������ �õ��մϴ�.", m_dwCID);
				wError = PktUI::FAIL_NOT_GT_GUILD_WAR_TIME;
				return false;
			}*/

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u ��� ����� �Ǽ��Ϸ��� ��ġ�� �� ���� ���Դϴ�. X:%f, Y:%f, Z:%f",
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
				ERRLOG1(g_Log, "CID:%10u ���� ������ ������ �����ϴ�.", m_dwCID);
				return false;
			}

			if (false == CCellManager::GetInstance().IsCampCreateArea(GetCurrentPos()))
			{
				ERRLOG4(g_Log, "CID:%10u ��� ����� �Ǽ��Ϸ��� ��ġ�� �� ���� ���Դϴ�. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				return false;
			}

			if (true == CCellManager::GetInstance().IsSafetyZone(GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:%10u ���� ������ ���� �� ���� ���� ���� ������ �������� �մϴ�.", m_dwCID);
				wError = PktUI::IN_SAFETYZONE;
				return false;
			}

			if (0 == GetGID())
			{
				ERRLOG1(g_Log, "CID:%10u ��忡 �������� �ʰ� ���� ���� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID);
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( GetGID() );
			if (NULL == lpGuild)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u �������� �ʴ� ����� �����Ͱ� ���� ���� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID, GetGID());
				wError = PktUI::NOT_EXIST_GUILD;
				return false;
			}

			if (m_dwCID != lpGuild->GetMaster().m_dwCID)
			{
				ERRLOG2(g_Log, "CID:%10u GID:0x%10u ����� �����Ͱ� �ƴ��ڰ� ���� ���� ���� ��Ÿ��Ŷ�� ����Ϸ� �մϴ�.", m_dwCID, GetGID());
				wError = PktUI::NOT_MASTER;
				return false;
			}

			const unsigned long dwFameForCampBuilding = 
				(SERVER_ID::GROUP_TEST == CServerSetup::GetInstance().GetServerGroup()) ? 
					Siege::FAME_FOR_WEAPON_BUILDING_TEST : Siege::FAME_FOR_WEAPON_BUILDING;

			if (lpGuild->GetFame() < dwFameForCampBuilding)
			{
				ERRLOG2(g_Log, "CID:%10u Fame:%d ��ġ�� ������ ��� �����Ͱ� ���� ������ �����Ϸ� �մϴ�.", m_dwCID, lpGuild->GetFame());
				wError = PktUI::NOT_ENOUGH_FAME;
				return false;
			}

			if (true == CSiegeObjectMgr::GetInstance().ExistBuildingOrDestroyingCamp(lpGuild->GetGID()))
			{
				ERRLOG1(g_Log, "CID:%10u ������, �����, �ı����� �ٸ� ��� ����� �ֽ��ϴ�.", m_dwCID);
				wError = PktUI::ANOTHER_DEV_CAMP;
				return false;
			}

			if (true == Castle::CCastleMgr::GetInstance().ExistCastleInRadius( GetCurrentPos() ) ||
				true == CSiegeObjectMgr::GetInstance().ExistCampInRadius( GetCurrentPos() ))
			{
				ERRLOG1(g_Log, "CID:%10u ���� ������ �������� ��ġ�� �ٸ� ��� ����� �� ��¡���� �ݰ� �����Դϴ�.", m_dwCID);
				wError = PktUI::INNER_RADIUS;
				return false;
			}

			GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
			if (0 == lpDBAgentDispatch)
			{
				ERRLOG0(g_Log, "������Ʈ ��� ����.");
				wError = PktUI::SERVER_ERROR;
			}
			else
			{
				// ���ÿ� �ΰ��� ���� ó���� ���� ���ؼ�...
				CSiegeObjectMgr::GetInstance().RequestWorldWeaponCreation();

				return GameClientSendPacket::SendCharCreateCamp(lpDBAgentDispatch->GetSendStream(), GetCID(), GetGID(), wObjectType, m_CurrentPos);
			}
		}
		break;

		case Siege::SHORT_RANGE_SIEGE_ARMS:
		case Siege::LONG_RANGE_SIEGE_ARMS:
		case Siege::AIRSHIP:
		{
			// ���� ��������
			CItem* lpMaterial = GetItem(ItemPos(TakeType::TS_EXTRA, ExtraSpacePos::KIT_MATERIAL_POS));

			if (NULL == lpMaterial)
			{
				ERRLOG1(g_Log, "CID:0x%08x ���� ���⸦ ������� ���簡 �����ϴ�.", m_dwCID);
				wError = PktUI::NOT_ENOUGH_MATERIAL;
				goto lb_fail;
			}

			// edith 2008.08.09 ���� �������� �������� �ʾұ� ������ �Ʒ��� �ӽ÷� ZONE3������ 
			// �����ߴ�. �Ŀ� �������� �ٸ� ������ �����Ѵ�.
			if(CServerSetup::GetInstance().GetServerZone()!=SERVER_ID::CAPITAL)
			{
				ERRLOG1(g_Log, "CID:%10u ī�������� �ƴմϴ�.", m_dwCID);
				goto lb_fail;
			}

			if (!CCellManager::GetInstance().CheckPositionInZone( GetCurrentPos() ))
			{
				ERRLOG4(g_Log, "CID:%10u ���� ���⸦ ������� �ϴ� ��ġ�� �� ���� ���Դϴ�. X:%f, Y:%f, Z:%f",
					m_dwCID, GetCurrentPos().m_fPointX, GetCurrentPos().m_fPointY, GetCurrentPos().m_fPointZ );
				wError = PktUI::FAIL_POSITION;
				goto lb_fail;
			}

			if ( GameTime::ST_SIEGE_WAR != CGameTimeMgr::GetInstance().GetCurrentSiegeTime() )
			{
				ERRLOG1(g_Log, "CID:0x%08x ���� �ð��� �ƴ� ��Ȳ���� ���� ���⸦ ������� �մϴ�.", m_dwCID);
				wError = PktUI::NOT_SIEGE_TIME;
				goto lb_fail;
			}

			if (false == CCastleMgr::GetInstance().ExistSiegeInRadius( GetCurrentPos()))
			{
				ERRLOG1(g_Log, "CID:0x%08x ���� �ۿ��� ���� ���⸦ �����Ϸ��� �߽��ϴ�.", m_dwCID);
				wError = PktUI::OUTER_RADIUS_SIEGE; 
				goto lb_fail;
			}
			   
			if(CSiegeObjectMgr::GetInstance().HasSiegeArms(GetCID()))
			{
				wError = PktUI::FAIL_HAS_SIEGE_ARMS;
				goto lb_fail;
			}

			// ���� ���� üũ.
			if(CCastleMgr::GetInstance().HasCastleArms(GetCID()))
			{
				wError = PktUI::FAIL_HAS_SIEGE_ARMS;
				goto lb_fail;
			}

			// ��� ������ üũ.
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

			// Level üũ.
			if(GetLevel()<Siege::CREATE_LEVEL_LIMIT)
			{
				wError = PktUI::NOT_LIMIT_LEVEL;
				goto lb_fail;
			}

			// �������� 
			if(CCastleMgr::GetInstance().GetNation()==GetNation())
			{
				wError = PktUI::NOT_CASTLE_NATION;
				goto lb_fail;
			}

			// ���� ���Ⱑ 10�� �̸��϶�.
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
				ERRLOG0(g_Log, "������Ʈ ��� ����.");
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
							ERRLOG1(g_Log, "CID:0x%08x ���� ���� ��ŸƮŶ ���� �� ����� ���� ���ſ� �����߽��ϴ�.", m_dwCID);
							wError = PktUI::SERVER_ERROR;
						}
						else
						{
							DELETE_ITEM(lpMaterial);
						}
					}
					else
					{
						// ���� ������ �κ��丮�� �̵�.
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

    // �̹� ������ �������� �ƴ� ��� ����
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
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / CreationID:%10u / ������ ���� : ������ ���� ����", 
                giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID);
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / CreationID:%10u / ������ ���� : �̹� ������ �������̶� �� ������ �� �����ϴ�", 
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID);
    }

    return false;
}

