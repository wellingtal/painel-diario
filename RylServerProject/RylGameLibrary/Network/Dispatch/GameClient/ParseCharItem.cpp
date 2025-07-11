
#include "stdafx.h"

#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ExchangeContainer.h>
#include <Item/Container/DepositContainer.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/CellManager.h>

#include <Creature/CreatureManager.h>
#include <Creature/NPC/NPC.h>
#include <Creature/Character/Character.h>
#include <Creature/Siege/CampShop.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>

#include <Community/Party/Party.h>
#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Castle/Castle.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleMgr.h>
#include <Castle/CastleBlessMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Skill/SkillMgr.h>

#include "ParseCharItem.h"
#include "SendCharItem.h"
#include "GameClientDispatch.h"

#define LOCK_FIFTH_SKILL(SkillType) \
	if(CSkillMgr::MAX_SKILL_LOCKCOUNT - 2 == lpSender->GetSkillLockCount(##SkillType) && CSkillMgr::MAX_SKILL_LEVEL == lpSender->GetSkillLevel(##SkillType))\
	{\
		if (false == lpSender->SkillFifthLock(SkillType))\
		{\
			usError = PktTI::FAIL_MOVE;\
			ERRLOG1(g_Log, "CID:%10u 5�ܰ轺ų �� ��� ������ 5�ܰ� ��ų�� ���� ���� ���ϴ� ������ �߻� �߽��ϴ�.", dwCID);\
		}\
	}\

#define UNLOCK_FIFTH_SKILL(SkillType) \
	if(CSkillMgr::MAX_SKILL_LOCKCOUNT - 1 == lpSender->GetSkillLockCount(##SkillType))\
	{\
		if(0 == lpSender->GetSkillLevel(##SkillType))\
		{\
			if (false == lpSender->SkillFifthUnlock(##SkillType))\
			{\
				usError = PktTI::FAIL_MOVE;\
				ERRLOG2(g_Log, "CID:%10u 5�ܰ轺ų �� ��� ������ 0x%8x�� 5�ܰ� ��ų ���� Ǯ�� ���ϴ� ������ �߻� �߽��ϴ�.", dwCID, ##SkillType);\
			}\
		}\
		else if(0 < lpSender->GetSkillLevel(##SkillType))\
		{\
			usError = PktTI::FAIL_MOVE_FIFTHSKILL_LOCKITEM;\
			ERRLOG2(g_Log, "CID:%10u 5�ܰ轺ų �� ��� �����Ϸ��� 0x%8x�� 5�ܰ� ��ų ������ 0�̾�� �մϴ�.", dwCID, ##SkillType);\
		}\
	}\

namespace GameClientParsePacket
{

	// ������ �ȱ� ó��
	bool ProcessItemSell(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr);

	// ������ ��� ó��
	bool ProcessItemBuy(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr);

	// ������ ���� ó��
	bool ProcessItemPickUp(CGameClientDispatch& GameClientDispatch, 
		unsigned __int64 nObjectID, Item::ItemPos itemPos, unsigned short& usError);

	inline bool IsPutDeposit(TakeType takeType)
	{
		return (takeType.m_srcPos.m_cPos != TakeType::TS_DEPOSIT && takeType.m_dstPos.m_cPos == TakeType::TS_DEPOSIT);
	}

	inline bool IsPutDeposit(unsigned char cSrc, unsigned char cDst)
	{
		return (cSrc != TakeType::TS_DEPOSIT && cDst == TakeType::TS_DEPOSIT);
	}

	inline unsigned long PutDepositMoney(CCharacter* lpCharacter)
	{
		return lpCharacter->GetStatus().m_nLevel * 4;//hz put in deposit gold
	}
}


// ������ �̵� �� ����
bool GameClientParsePacket::ParseCharTakeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktTI), GameClientDispatch);

	PktTI*      lpPktTI     = static_cast<PktTI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short  usError     = 0;
	TakeType        takeType    = lpPktTI->m_TakeType;    

	unsigned long   dwCID       = lpPktTI->m_dwCharID;
	unsigned long   dwTakeMoney = 0;
	Item::CItem*    lpMoveItem  = lpCharacter->GetItem(takeType.m_srcPos);

	bool bTakeMoney = false;

	if (NULL == lpMoveItem)
	{
		usError = PktTI::FAIL_MOVE;
		ERRLOG5(g_Log, "CID:%10u ������ �̵� ���� Src�� �������� �����ϴ�. SRC:(%2d:0x%04x), DST(%2d:0x%04x)", 
			lpCharacter->GetCID(), takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex, 
			takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);

		Item::CItemContainer* lpItemContainer = lpCharacter->GetItemContainer(takeType.m_srcPos.m_cPos);
		if (NULL != lpItemContainer)
		{
			lpItemContainer->DumpItemInfo();
		}
	}
	else
	{
		bTakeMoney = IsPutDeposit(lpMoveItem->GetRealPos().m_cPos, takeType.m_dstPos.m_cPos);

		if (bTakeMoney)
		{
			if (!lpCharacter->GetDeposit().IsLogin())
			{
				usError = PktTI::FAIL_MOVE;
				ERRLOG1(g_Log, "CID:%10u â�� �α��ε��� ���� ���¿��� �������� �ű�� �Ͽ����ϴ�.", dwCID);
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktTI::FAIL_MOVE;
					ERRLOG1(g_Log, "CID:%10u â�� ������ �ñ�⿡ ���� �����մϴ�.", dwCID);
				}
			}
		}
	}

	GAMELOG::sMinItemInfo minItemInfo;
	CCharacter* lpSender = CCreatureManager::GetInstance().GetCharacter(lpPktTI->m_dwCharID);

	if (0 == usError)
	{
		if(CServerSetup::GetInstance().UseContents(GameRYL::FIFTHSKILL_LOCKITEM))
		{
			// 5�ܰ� ��� ������ ����
			if(lpMoveItem && takeType.m_srcPos.m_cPos == TakeType::TS_EQUIP)
			{
				switch(lpMoveItem->GetPrototypeID())
				{
					case 551 : UNLOCK_FIFTH_SKILL(0x8503); break;
					case 552 : UNLOCK_FIFTH_SKILL(0x8603); break;
					case 553 : UNLOCK_FIFTH_SKILL(0x8704); break;
					case 554 : UNLOCK_FIFTH_SKILL(0x8804); break;
					case 555 : UNLOCK_FIFTH_SKILL(0x8902); break;
					case 556 : UNLOCK_FIFTH_SKILL(0x8A05); break;
					case 557 : UNLOCK_FIFTH_SKILL(0x8B04); break;
					case 558 : UNLOCK_FIFTH_SKILL(0x8C02); break;
					case 5351 : UNLOCK_FIFTH_SKILL(0x9302); break;
					case 5352 : UNLOCK_FIFTH_SKILL(0x9402); break;
					case 5353 : UNLOCK_FIFTH_SKILL(0x9502); break;
					case 5354 : UNLOCK_FIFTH_SKILL(0x9606); break;
					case 5355 : UNLOCK_FIFTH_SKILL(0x9603); break;
					case 5356 : UNLOCK_FIFTH_SKILL(0x9706); break;
					case 5357 : UNLOCK_FIFTH_SKILL(0x9804); break;
					default	 : break;
				}
			}
		}

		minItemInfo.InitMinItemInfo(lpMoveItem);

		if (!lpCharacter->MoveItem(takeType))
		{
			ERRLOG5(g_Log, "CID:%10u ������ �̵� ����. SRC:(%2d:0x%04x), DST(%2d:0x%04x)", 
				lpCharacter->GetCID(), takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex,
				takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
			usError = PktTI::FAIL_MOVE;
		}
		else 
		{
			if (bTakeMoney)
			{			
				// �̵� ������, takeType�� src�� TakeType::TS_DEPOSIT�� �ƴϰ�, dst�� TakeType::TS_DEPOSIT�� ��츸 �����Ѵ�.
				unsigned long dwLastGold = lpCharacter->GetGold();
				lpCharacter->DeductGold(dwTakeMoney, false);
				GAMELOG::LogTakeGold(*lpCharacter, dwLastGold, lpCharacter->GetGold(), dwTakeMoney,
					TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STORE_USE, 0);
			}

			if(CServerSetup::GetInstance().UseContents(GameRYL::FIFTHSKILL_LOCKITEM))
			{
				// 5�ܰ� ��� ������ ����
				if(lpMoveItem && takeType.m_dstPos.m_cPos == TakeType::TS_EQUIP)
				{
					switch(lpMoveItem->GetPrototypeID())
					{
						case 551 : LOCK_FIFTH_SKILL(0x8503); break;
						case 552 : LOCK_FIFTH_SKILL(0x8603); break;
						case 553 : LOCK_FIFTH_SKILL(0x8704); break;
						case 554 : LOCK_FIFTH_SKILL(0x8804); break;
						case 555 : LOCK_FIFTH_SKILL(0x8902); break;
						case 556 : LOCK_FIFTH_SKILL(0x8A05); break;
						case 557 : LOCK_FIFTH_SKILL(0x8B04); break;
						case 558 : LOCK_FIFTH_SKILL(0x8C02); break;
						case 5351 : LOCK_FIFTH_SKILL(0x9302); break;
						case 5352 : LOCK_FIFTH_SKILL(0x9402); break;
						case 5353 : LOCK_FIFTH_SKILL(0x9502); break;
						case 5354 : LOCK_FIFTH_SKILL(0x9606); break;
						case 5355 : LOCK_FIFTH_SKILL(0x9603); break;
						case 5356 : LOCK_FIFTH_SKILL(0x9706); break;
						case 5357 : LOCK_FIFTH_SKILL(0x9804); break;
						default	 : break;
					}
				}
			}
		}
	}

	// ������ �̵� �α׸� ����ϴ�.
	GAMELOG::LogMoveItem(*lpCharacter, takeType, minItemInfo, usError);

	// �����͸� �����Ѵ�.
	return GameClientSendPacket::SendCharTakeItem(GameClientDispatch.GetSendStream(), 
		lpCharacter->GetCID(), takeType, usError);
}


// ������ ����
bool GameClientParsePacket::ParseCharSwapItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSwI), GameClientDispatch);

	PktSwI*     lpPktSwl     = static_cast<PktSwI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short  usError = 0;
	TakeType        swapSrc = lpPktSwl->m_SwapSrc;
	TakeType        swapDst = lpPktSwl->m_SwapDst;

	Item::CItem*    lpSrcItem   = lpCharacter->GetItem(swapSrc.m_srcPos);
	Item::CItem*    lpDstItem   = lpCharacter->GetItem(swapDst.m_srcPos);
	unsigned long   dwTakeMoney = 0;
	unsigned long   dwCID       = lpPktSwl->m_dwCharID;


	if (NULL == lpSrcItem)
	{
		usError = PktSwI::FAIL_SRC_MOVE;
		ERRLOG3(g_Log, "CID:%10u Swap���� : Src��ġ���� �������� ���� �� �����ϴ�. (%d, 0x%04x)", 
			dwCID, swapSrc.m_srcPos.m_cPos, swapSrc.m_srcPos.m_cIndex);
	}
	else if (NULL == lpDstItem)
	{
		usError = PktSwI::FAIL_DST_MOVE;
		ERRLOG3(g_Log, "CID:%10u Swap���� : Dst��ġ���� �������� ���� �� �����ϴ�. (%d, 0x%04x)", 
			dwCID, swapDst.m_srcPos.m_cPos, swapDst.m_srcPos.m_cIndex);
	}
	else
	{
		bool bTakeMoney = IsPutDeposit(lpSrcItem->GetRealPos().m_cPos, swapSrc.m_dstPos.m_cPos) || 
			IsPutDeposit(lpSrcItem->GetRealPos().m_cPos, swapDst.m_dstPos.m_cPos); 

		if (bTakeMoney)
		{
			if (!lpCharacter->GetDeposit().IsLogin())
			{
				usError = PktTI::FAIL_MOVE;
				ERRLOG1(g_Log, "CID:%10u â�� �α��ε��� ���� ���¿��� �������� �����Ϸ� �Ͽ����ϴ�.", dwCID);
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktSwI::CANT_SWAP;
					ERRLOG1(g_Log, "CID:%10u â�� ������ �ñ�⿡ ���� �����մϴ�.", dwCID);
				}
			}
		}

		if (0 == usError)
		{

			if (!lpCharacter->SwapItem(swapSrc, swapDst)) 
			{
				ERRLOG9(g_Log, "CID:%10u ���� �Ұ���. SRCTake:(%2d:0x%04x)->(%2d:0x%04x), DSTTake(%2d:0x%04x)->(%2d:0x%04x)",
					lpCharacter->GetCID(), 
					swapSrc.m_srcPos.m_cPos, swapSrc.m_srcPos.m_cIndex, swapSrc.m_dstPos.m_cPos, swapSrc.m_dstPos.m_cIndex,
					swapDst.m_srcPos.m_cPos, swapDst.m_srcPos.m_cIndex, swapDst.m_dstPos.m_cPos, swapDst.m_dstPos.m_cIndex);
				usError = PktSwI::CANT_SWAP;
			}
			else
			{
				if (bTakeMoney)
				{
					unsigned long dwLastGold = lpCharacter->GetGold();
					lpCharacter->DeductGold(dwTakeMoney, false);
					GAMELOG::LogTakeGold(*lpCharacter, dwLastGold, lpCharacter->GetGold(), dwTakeMoney,
						TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STORE_USE, 0);
				}

				if(CServerSetup::GetInstance().UseContents(GameRYL::FIFTHSKILL_LOCKITEM))
				{
					CCharacter* lpSender = CCreatureManager::GetInstance().GetCharacter(lpPktSwl->m_dwCharID);

					if(1 == swapDst.m_srcPos.m_cPos)
					{
						switch(lpDstItem->GetPrototypeID())
						{
							case 551 : UNLOCK_FIFTH_SKILL(0x8503); break;
							case 552 : UNLOCK_FIFTH_SKILL(0x8603); break;
							case 553 : UNLOCK_FIFTH_SKILL(0x8704); break;
							case 554 : UNLOCK_FIFTH_SKILL(0x8804); break;
							case 555 : UNLOCK_FIFTH_SKILL(0x8902); break;
							case 556 : UNLOCK_FIFTH_SKILL(0x8A05); break;
							case 557 : UNLOCK_FIFTH_SKILL(0x8B04); break;
							case 558 : UNLOCK_FIFTH_SKILL(0x8C02); break;
							case 5351 : UNLOCK_FIFTH_SKILL(0x9302); break;
							case 5352 : UNLOCK_FIFTH_SKILL(0x9402); break;
							case 5353 : UNLOCK_FIFTH_SKILL(0x9502); break;
							case 5354 : UNLOCK_FIFTH_SKILL(0x9606); break;
							case 5355 : UNLOCK_FIFTH_SKILL(0x9603); break;
							case 5356 : UNLOCK_FIFTH_SKILL(0x9706); break;
							case 5357 : UNLOCK_FIFTH_SKILL(0x9804); break;
							default	 : break;
						}
					}
					
					if(1 == swapSrc.m_dstPos.m_cPos)
					{
						switch(lpSrcItem->GetPrototypeID())
						{
						case 551 : LOCK_FIFTH_SKILL(0x8503); break;
						case 552 : LOCK_FIFTH_SKILL(0x8603); break;
						case 553 : LOCK_FIFTH_SKILL(0x8704); break;
						case 554 : LOCK_FIFTH_SKILL(0x8804); break;
						case 555 : LOCK_FIFTH_SKILL(0x8902); break;
						case 556 : LOCK_FIFTH_SKILL(0x8A05); break;
						case 557 : LOCK_FIFTH_SKILL(0x8B04); break;
						case 558 : LOCK_FIFTH_SKILL(0x8C02); break;
						case 5351 : LOCK_FIFTH_SKILL(0x9302); break;
						case 5352 : LOCK_FIFTH_SKILL(0x9402); break;
						case 5353 : LOCK_FIFTH_SKILL(0x9502); break;
						case 5354 : LOCK_FIFTH_SKILL(0x9606); break;
						case 5355 : LOCK_FIFTH_SKILL(0x9603); break;
						case 5356 : LOCK_FIFTH_SKILL(0x9706); break;
						case 5357 : LOCK_FIFTH_SKILL(0x9804); break;
						default	 : break;
						}
					}
				}
			}
		}
	}

	// ������ ���� �α�
	GAMELOG::LogSwapItem(*lpCharacter, swapSrc, swapDst, lpSrcItem, lpDstItem, usError);

	return GameClientSendPacket::SendCharSwapItem(GameClientDispatch.GetSendStream(), lpCharacter->GetCID(), 
		swapSrc, swapDst, usError);
}


// ������ ����
bool GameClientParsePacket::ParseCharRepairItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRpI), GameClientDispatch);

	PktRpI*     lpPktRpl    = static_cast<PktRpI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short  usError = PktBase::NO_SERVER_ERR;

	Item::ItemPos   itemPos     = lpPktRpl->m_itemPos;
	Item::CItem*    lpItem      = lpCharacter->GetItem(itemPos);
	unsigned char   cDurability = (NULL != lpItem) ? lpItem->GetNumOrDurability() : NULL;

	unsigned long dwRepairGold = lpCharacter->RepairItem(lpPktRpl->m_dwCharID, itemPos, usError);

	GAMELOG::LogRepairItem(*lpCharacter, lpItem, dwRepairGold, cDurability, usError);
	return GameClientSendPacket::SendCharRepairItem(GameClientDispatch.GetSendStream(), lpCharacter->GetCID(), 
		dwRepairGold, itemPos, usError);
}

// ������ ���� (��� ������ ��� ����)
bool GameClientParsePacket::ParseCharRepairAllItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRpAI), GameClientDispatch);

	PktRpAI*     lpPktRpAI	= static_cast<PktRpAI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short usError = PktBase::NO_SERVER_ERR;

	unsigned long dwRepairGold = lpCharacter->RepairAllItem(lpPktRpAI->m_dwCharID, usError);
/*
	// RepairAllItem ������ ����.
	if (0 != dwRepairGold && usError == PktBase::NO_SERVER_ERR)
	{
		if (dwRepairGold > lpCharacter->GetGold())
		{
			usError = PktRpAI::NOT_ENOUGH_GOLD;
		}
	}
*/
	return GameClientSendPacket::SendCharRepairAllItem(GameClientDispatch.GetSendStream(), 
		lpCharacter->GetCID(), dwRepairGold, usError);
}

// ĳ�� ������ ���
bool GameClientParsePacket::ParseCharUseCashItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktUI), GameClientDispatch);

	PktUI*      lpPktUI     = static_cast<PktUI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	Item::ItemPos   itemPos = lpPktUI->m_itemPos;
	Item::CUseItem* lpUseItem = Item::CUseItem::DowncastToUseItem(lpCharacter->GetItem(itemPos));

	CCharacter* lpSender = CCreatureManager::GetInstance().GetCharacter(lpPktUI->m_dwSender);
	CCharacter* lpRecver = CCreatureManager::GetInstance().GetCharacter(lpPktUI->m_dwRecver);

	unsigned short usError = PktBase::NO_SERVER_ERR;

	if (NULL == lpUseItem || NULL == lpSender || NULL == lpRecver || 
		true == lpSender->GetEnchantInfo().GetFlag(Skill::SpellID::Stun) ||
		true == lpSender->GetEnchantInfo().GetFlag(Skill::SpellID::StoneForm) || 
		lpUseItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > lpSender->GetRealmPoint() || 
		(true == lpUseItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		!CGameTimeMgr::GetInstance().IsRealmWarTime()) ||
		(true == lpUseItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		CGameTimeMgr::GetInstance().IsRealmWarTime() && lpSender->GetRealmWarFlag()==Creature::WAR_OFF))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, PktUI::USE_FAILED);

		return GameClientSendPacket::SendCharUseCashItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, 0, PktUI::USE_FAILED);
	}

	// ������ ���� �����ϸ� �α׸� ����� ������ ����
	if(!lpUseItem->UseCashItem(lpSender, lpRecver, itemPos, usError))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

		// ���ϵǴ� ������ ���� ĳ���Ͱ� �������� ����Ϸ��� �Ҷ� 
		// ������ 0�ε� false �� ���ϵɼ��� �ִ�. 
		// �̷��� �������� 0���� ���󰡱� ������ �������� 0���� Ȯ���� ����������.
		unsigned char cRemainItemNum = lpUseItem->GetNumOrDurability();

		return GameClientSendPacket::SendCharUseCashItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
	}

	return true;
}


// ������ ���
bool GameClientParsePacket::ParseCharUseItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktUI), GameClientDispatch);

	PktUI*      lpPktUI     = static_cast<PktUI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	Item::ItemPos   itemPos = lpPktUI->m_itemPos;
	Item::CUseItem* lpUseItem = Item::CUseItem::DowncastToUseItem(lpCharacter->GetItem(itemPos));

	CCharacter* lpSender = CCreatureManager::GetInstance().GetCharacter(lpPktUI->m_dwSender);
	CCharacter* lpRecver = CCreatureManager::GetInstance().GetCharacter(lpPktUI->m_dwRecver);

	unsigned short usError = PktBase::NO_SERVER_ERR;

	if (NULL == lpUseItem || NULL == lpSender || NULL == lpRecver || 
		true == lpSender->GetEnchantInfo().GetFlag(Skill::SpellID::Stun) ||
		true == lpSender->GetEnchantInfo().GetFlag(Skill::SpellID::StoneForm) || 
		lpUseItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > lpSender->GetRealmPoint() || 
		(true == lpUseItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		!CGameTimeMgr::GetInstance().IsRealmWarTime()) ||
		(true == lpUseItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		CGameTimeMgr::GetInstance().IsRealmWarTime() && lpSender->GetRealmWarFlag()==Creature::WAR_OFF))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, PktUI::USE_FAILED);

		return GameClientSendPacket::SendCharUseItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, 0, PktUI::USE_FAILED);
	}

	// ������ ���� �����ϸ� �α׸� ����� ������ ����
	if(!lpUseItem->Use(lpSender, lpRecver, itemPos, usError))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

		// ���ϵǴ� ������ ���� ĳ���Ͱ� �������� ����Ϸ��� �Ҷ� 
		// ������ 0�ε� false �� ���ϵɼ��� �ִ�. 
		// �̷��� �������� 0���� ���󰡱� ������ �������� 0���� Ȯ���� ����������.
		unsigned char cRemainItemNum = lpUseItem->GetNumOrDurability();

		return GameClientSendPacket::SendCharUseItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
	}

	// ������ ��� �α׸� �����.
	GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

	// ������ ������ 0���̸� �������� �����Ѵ�.
	unsigned char cRemainItemNum = lpUseItem->GetNumOrDurability();
	if (0 == cRemainItemNum)
	{
		if (lpCharacter->RemoveItem(itemPos))
		{
			if (TakeType::TS_STALL == lpUseItem->GetRealPos().m_cPos)
			{
				lpCharacter->GetStall().RemoveItem(lpUseItem->GetRealPos());
			}
		}

		DELITEMLOG(DETLOG1(g_Log, "this:0X%08x �������� �����մϴ�.", lpUseItem));	
		DELETE_ITEM(lpUseItem);
	}

	return GameClientSendPacket::SendCharUseItem(GameClientDispatch.GetSendStream(), 
		lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
}


// ������ �ŷ�
bool GameClientParsePacket::ParseCharTradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	/*	
	// 'TRC_BUY_EQUIP' ����� ��� �������� �������� ���� �� �����Ƿ� ��Ŷ ũ�� �˻縦 ���� �ʽ��ϴ�.
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktTr), GameClientDispatch);
	*/

	PktTr* lpPktTr = static_cast<PktTr*>(lpPktBase);

	switch (lpPktTr->m_cCmd)
	{
	case PktTr::TRC_BUY_WITH_GOLD:
	case PktTr::TRC_BUY_WITH_MILEAGE:
	case PktTr::TRC_BUY_EQUIP:
	case PktTr::TRC_BUY_WITH_SKILL_COUPON:
		return ProcessItemBuy(GameClientDispatch, lpPktTr);

	case PktTr::TRC_SELL:
		return ProcessItemSell(GameClientDispatch, lpPktTr);
	}

	return false;
}

// NPC ��� ���� ����
bool GameClientParsePacket::ParseCharEquipShopInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktEquipShopInfo), GameClientDispatch);

	// ������ ��ȹ�� ���� �������� ����ʿ� ���� �� ��Ŷ�� ������� �ʽ��ϴ�. (2004-07-26 by �ε�)
	ERRLOG0(g_Log, "Ŭ���̾�Ʈ�� ���̻� ������� �ʴ� ���� ���� ��û ��Ŷ�� ���½��ϴ�.");
	return true;

	/*
	PktEquipShopInfo*	lpPktEquipShopInfo	= static_cast<PktEquipShopInfo *>(lpPktBase);
	CCharacter*			lpCharacter			= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long 	dwCharID	= lpPktEquipShopInfo->m_dwCharID;
	unsigned long 	dwNPCID		= lpPktEquipShopInfo->m_dwNPCID;

	unsigned char 	cRace		= lpPktEquipShopInfo->m_cRace;
	unsigned char 	cTabPage	= lpPktEquipShopInfo->m_cTabPage;

	unsigned short	wError		= PktBase::NO_SERVER_ERR;

	unsigned short	aryItemID[UCHAR_MAX]	= { 0, };
	unsigned char	aryItemGrade[UCHAR_MAX]	= { 0, };

	CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
	unsigned char cNum = lpNPC->GetEquipShopInfo(cRace, cTabPage, aryItemID, aryItemGrade);
	unsigned long dwDisplayTime = lpNPC->GetDisplayTime();

	if (0 == cNum)
	{
	wError = PktBase::SERVER_ERROR;
	}

	return GameClientSendPacket::SendCharEquipShopInfo(GameClientDispatch.GetSendStream(), 
	dwCharID, dwNPCID, dwDisplayTime, cRace, cTabPage, cNum, aryItemID, aryItemGrade, wError);
	*/
}


// ������ ����
bool GameClientParsePacket::ParseCharPickUp(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPU), GameClientDispatch);

	PktPU*      lpPktPU     = static_cast<PktPU*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long		dwCID	    = lpCharacter->GetCID();
	unsigned __int64	nObjectID	= lpPktPU->m_nObjectID;  
	Item::ItemPos		itemPos     = lpPktPU->m_itemPos;
	unsigned short		usError		= PktBase::NO_SERVER_ERR;

	return ProcessItemPickUp(GameClientDispatch, nObjectID, itemPos, usError);
}


// ������ ������
bool GameClientParsePacket::ParseCharPullDown(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPD), GameClientDispatch);

	PktPD*      lpPktPD     = static_cast<PktPD*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long   dwCID       = lpPktPD->m_dwCharID;
	Item::ItemPos   itemPos     = lpPktPD->m_itemPos;    
	unsigned short  usError     = 0;
	unsigned char   cNum        = lpPktPD->m_cNum;

	CCell::ItemInfo itemInfo;

	if (TakeType::TS_DEPOSIT == itemPos.m_cPos || TakeType::TS_EXCHANGE == itemPos.m_cPos)
	{
		usError = PktPDAck::FAIL_DROP_FIELD; 
		ERRLOG3(g_Log, "CID:%10u â���� �ٷ� ������ ���� �� �����ϴ�. (%2d,0x%04x)", 
			dwCID, itemPos.m_cPos, itemPos.m_cIndex);
	}
	else
	{
		// �� ���
		CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;
		if (NULL == lpCell) 
		{
			usError = PktPDAck::FAIL_GET_CELL; 
			ERRLOG1(g_Log, "CID:%10u �� ��� ����", dwCID);
		}
		else
		{
			// �κ�/��� �κ��� ���
			Item::CItem* lpItem = lpCharacter->Drop(itemPos, cNum);
			if (NULL == lpItem) 
			{
				usError = PktPDAck::FAIL_DROP_INVEN;
				ERRLOG4(g_Log, "CID:%10u �κ�/���� ���� ������ ���� (%2d:0x%04x)(%2d)",
					dwCID, itemPos.m_cPos, itemPos.m_cIndex, cNum);		    
			}
		/*	else
			{
				// ���� �������� �̻��մϴ�.(0��)
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					usError = PktPDAck::FAIL_ZERO_ITEM;

					DELETE_ITEM(lpItem);
				}
				else
				{
					// ������ �ʵ忡 ������
				//	lpCell->SetItem(lpCharacter->GetCurrentPos(), lpItem, 0, dwCID, CCell::NONE, itemInfo);//hz removed this for dropfix

					// ����Ʈ Ʈ���� üũ
					lpCharacter->CheckTrigger(Quest::TRIGGER_PUTON, lpItem->GetPrototypeID(), lpCharacter->GetCurrentPos(), 1);
				}
			}*/
		}
	}

	// ������ ������ ����
	GAMELOG::LogDropItem(*lpCharacter, itemPos, itemInfo.m_lpItem, 0, usError);
	return GameClientSendPacket::SendCharPullDown(GameClientDispatch.GetSendStream(),
		dwCID, itemPos, itemInfo, usError);        
}

// ������ ������
bool GameClientParsePacket::ParseCharSplitItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSplt), GameClientDispatch);

	PktSplt*    lpPktSplt   = static_cast<PktSplt*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	TakeType        takeType    = lpPktSplt->m_TakeType;
	unsigned long   dwCID       = lpCharacter->GetCID();
	unsigned long   dwTakeMoney = 0;
	unsigned short  usError     = 0;

	Item::CItem* lpSrcItem = lpCharacter->GetItem(takeType.m_srcPos);
	Item::CItem* lpSplitItem = NULL;

	ERRLOG(
		const int   MAX_ERROR_BUFFER = 4096;
	char        szErrorBuffer[MAX_ERROR_BUFFER] = "";
	int         nLength = 0;
	);

	if (NULL == lpSrcItem)
	{
		usError = PktSpItAck::FAIL_SPLIT;
		ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
			"CID:%10u ������ ���ø� ���� : (%d, %d) ��ġ���� �������� ���� �� �����ϴ�.",
			dwCID, takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex));        
	}
	else
	{
		bool bTakeMoney = IsPutDeposit(lpSrcItem->GetRealPos().m_cPos, takeType.m_dstPos.m_cPos);

		if (bTakeMoney)
		{
			if (!lpCharacter->GetDeposit().IsLogin())
			{
				usError = PktTI::FAIL_MOVE;
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u â�� �α��ε��� ���� ���¿��� �������� ������ �Ͽ����ϴ�.", dwCID));
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktSpItAck::FAIL_SPLIT;
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u â�� ������ �ñ�⿡ ���� �����մϴ�.", dwCID));
				}
			}
		}

		if (0 == usError)
		{
			if (!lpSrcItem->IsSet(Item::DetailData::STACKABLE)) 
			{
				usError = PktSpItAck::FAIL_SPLIT;
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u ������ ���ø� ���� : ���� �� ���� ������(���� �Ұ�)�Դϴ�. ���� ID : %d",
					dwCID, lpSrcItem->GetPrototypeID()));		    

			}
			else if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum)
			{
				usError = PktSpItAck::FAIL_SPLIT;
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u ������ ���ø� ���� : Source�� �ִ� �������� ������ �������� �ϴ� �������� �������� �۰ų� �����ϴ�."
					"Source�� ���� ������ ���� %d, �������� �ϴ� �������� ���� %d",
					dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum));		        
			}
			else
			{
				// ������ ������.
				lpSplitItem = Item::CItemFactory::GetInstance().CreateItem(lpSrcItem->GetItemInfo());

				if (NULL == lpSplitItem) 
				{
					usError = PktSpItAck::FAIL_SPLIT;
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u ������ ���ø� ���� : �������� ������ �� �����ϴ�.", dwCID));
				}
				else
				{
					lpSplitItem->SetNumOrDurability(takeType.m_cNum);

					// ������ ����
					if (!lpCharacter->SetItem(takeType.m_dstPos, lpSplitItem)) 
					{
						usError = PktSpItAck::FAIL_SPLIT;
						ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
							"CID:%10u ������ ���ø� ���� : (%d, %d)�ɰ��� ���������� �ϴ� �ڸ���, �������� ���� ���� �� �����ϴ�.",
							dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex));

						DELETE_ITEM(lpSplitItem);
					}
					else 
					{
						// ���� ����
						lpSrcItem->SetNumOrDurability(lpSrcItem->GetNumOrDurability() - takeType.m_cNum);

						if (bTakeMoney)
						{
							unsigned long dwLastGold = lpCharacter->GetGold();
							lpCharacter->DeductGold(dwTakeMoney, false);
							GAMELOG::LogTakeGold(*lpCharacter, dwLastGold, lpCharacter->GetGold(), dwTakeMoney,
								TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STORE_USE, 0);
						}
					}
				}                
			}
		}
	}

	ERRLOG(if (0 < nLength) { ERRLOG0(g_Log, szErrorBuffer); });

	// ������ ���ø� �α� �����
	GAMELOG::LogSplitItem(*lpCharacter, takeType, lpSrcItem, lpSplitItem, usError);
	return GameClientSendPacket::SendCharSplitItem(GameClientDispatch.GetSendStream(), 
		dwCID, lpSplitItem, takeType, usError);
}


// �� �ٷ��
bool GameClientParsePacket::ParseCharTakeGold(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktTG), GameClientDispatch);

	PktTG*      lpPktTG     = static_cast<PktTG*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long   dwCID   = lpCharacter->GetCID();
	unsigned long   dwGold  = lpPktTG->m_dwGold;
	unsigned short  usError = 0;
	unsigned char   cSrcPos = lpPktTG->m_cSrcPos;
	unsigned char   cDstPos = lpPktTG->m_cDstPos;

	if ((cSrcPos == TakeType::TS_DEPOSIT || cDstPos == TakeType::TS_DEPOSIT) &&
		!lpCharacter->GetDeposit().IsLogin())
	{
		ERRLOG3(g_Log, "CID:%10u �� �̵� ����. â�� �α��� ���� �����Դϴ�. (%2d,%2d)",
			dwCID, cSrcPos, cDstPos);            
		usError = PktTG::FAIL_MOVE;
	}
	else if (!lpCharacter->MoveGold(dwGold, cSrcPos, cDstPos, usError)) 
	{
		// ���� �ڵ�� �ȿ��� Set��.
		ERRLOG6(g_Log, "CID:%10u �� �̵� ����. (Pos/Gold) Src:(%d/%d), Dst:(%d/%d), �̵� �ݾ�:%d", 
			dwCID, cSrcPos, lpCharacter->GetGold(cSrcPos), cDstPos, lpCharacter->GetGold(cDstPos), dwGold);		
	}

	GAMELOG::LogTakeGold(*lpCharacter, lpCharacter->GetGold(cSrcPos), lpCharacter->GetGold(cDstPos), 
		dwGold, cSrcPos, cDstPos, GAMELOG::sTakeGoldLogV2::MOVE_GOLD, usError);

	return GameClientSendPacket::SendCharTakeGold(GameClientDispatch.GetSendStream(),
		dwCID, dwGold, cSrcPos, cDstPos, usError);
}


bool GameClientParsePacket::ParseCharDepositCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktDeposit), GameClientDispatch);

	PktDeposit* lpPktDeposit    = static_cast<PktDeposit*>(lpPktBase);
	CCharacter* lpCharacter     = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	Item::CDepositContainer& Deposit = lpCharacter->GetDeposit();
	unsigned long   dwCID   = lpCharacter->GetCID();
	char*           szData  = lpPktDeposit->m_szData;
	unsigned short  usError = 0;


	switch(lpPktDeposit->m_cCmd)
	{
	case PktDeposit::LOGIN:

		if (!Deposit.Login(szData, Deposit::PASSWORD_LENGTH, szData[Deposit::PASSWORD_LENGTH]))
		{
			usError = PktDeposit::AUTH_FAILED;
		}
		break;

	case PktDeposit::BUY_TAB:

		{
			const unsigned int  MAX_TAB = 4;
			const unsigned long dwTabBuyPrice[MAX_TAB] = { 0, 1000, 10000, 50000 };
			const unsigned long dwLastGold = lpCharacter->GetGold();

			const unsigned char cTabNum = szData[0];

			if (MAX_TAB <= cTabNum)
			{
				usError = PktDeposit::INVALID_ARGUMENT;
				ERRLOG2(g_Log, "CID:%10u �� ��ȣ�� �̻��մϴ�. TabNum:%d", dwCID, cTabNum);
			}
			else if (dwLastGold < dwTabBuyPrice[cTabNum])
			{
				usError = PktDeposit::INSUFFICIENT_MONEY;
				ERRLOG3(g_Log, "CID:%10u ���� �����մϴ�. CurrentGold:%d, TabBuyPrice:%d",
					dwCID, dwLastGold, dwTabBuyPrice[cTabNum]);
			}
			else if (!Deposit.BuyTab(cTabNum))
			{
				usError = PktDeposit::INVALID_ARGUMENT;
				ERRLOG2(g_Log, "CID:%10u �� ��ȣ�� �̻��մϴ�. TabNum:%d", dwCID, cTabNum);
			}
			else
			{                
				lpCharacter->DeductGold(dwTabBuyPrice[cTabNum], false);
				GAMELOG::LogTakeGold(*lpCharacter, dwLastGold, lpCharacter->GetGold(), dwTabBuyPrice[cTabNum],
					TakeType::TS_DEPOSIT, TakeType::TS_DEPOSIT, GAMELOG::sTakeGoldLogV2::BUY_STORE_TAB, 0);
			}

		} break;

	case PktDeposit::LOGOUT:

		if (!Deposit.IsLogin())
		{
			usError = PktDeposit::SERVER_ERROR;
		}
		else
		{
			lpCharacter->DBUpdateForce(DBUpdateData::UPDATE);
			Deposit.Logout();
		}
		break;

	case PktDeposit::CHANGE_PASS:

		// �н����� �ٲ�.
		if (Deposit.ChangePassword(szData, Deposit::PASSWORD_LENGTH, 
			szData + Deposit::PASSWORD_LENGTH, Deposit::PASSWORD_LENGTH))
		{
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
				CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

			if (0 != lpDBAgentDispatch)
			{
				if (!GameClientSendPacket::SendCharDepositPasswordToDBAgent(lpDBAgentDispatch->GetSendStream(), 
					GameClientDispatch.GetUID(), szData + Deposit::PASSWORD_LENGTH, Deposit::PASSWORD_LENGTH))
				{
					usError = PktDeposit::SERVER_ERROR;
					ERRLOG1(g_Log, "CID:%10u DBAgent�� â�� ��ȣ�� ������ �� �����ϴ�.", dwCID);
				}                
			}
			else
			{
				usError = PktDeposit::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:%10u DBAgent������ ���� �� �����ϴ�.", dwCID);
			}
		}
		else
		{
			usError = PktDeposit::AUTH_FAILED;
			ERRLOG1(g_Log, "CID:%10u â�� ��ȣ�� �ٲ� �� �����ϴ�. ��ȣ�� �ٸ��ϴ�.", dwCID);
		}
		break;
	};

	return GameClientSendPacket::SendCharDepositCmd(GameClientDispatch.GetSendStream(), lpPktDeposit->m_cCmd, 0, 0, usError);
}


// ���� ����
bool GameClientParsePacket::ParseCharAutoRouting(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktAutoRouting), GameClientDispatch);

	PktAutoRouting* lpPktAR = static_cast<PktAutoRouting*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned __int64    nObjectID = lpPktAR->m_nObjectID;
	unsigned long       dwCID   = lpPktAR->m_dwCharID;

	unsigned short      wItemID = lpPktAR->m_wItemID;
	Item::ItemPos       itemPos = lpPktAR->m_itemPos;

	unsigned short      usError = PktBase::NO_SERVER_ERR;
	unsigned char       cCmd    = lpPktAR->m_cCmd;

	if (PktAutoRouting::ARC_POSSIBLE == cCmd)
	{
		const bool bResult = ProcessItemPickUp(GameClientDispatch, nObjectID, itemPos, usError);
		if (false == bResult || PktBase::NO_SERVER_ERR != usError)
		{
			cCmd = PktAutoRouting::ARC_IMPOSSIBLE;
		}
	}

	CCharacterParty* lpParty = reinterpret_cast<CCharacterParty* >(lpCharacter->GetParty());
	if (NULL != lpParty)
	{
		lpParty->SendAutoRouting(dwCID, wItemID, cCmd);
		return true;
	}

	return true;
}



bool GameClientParsePacket::ProcessItemSell(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr)
{
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktTr->GetCmd());

	unsigned long   dwGold      = 0;
	unsigned long   dwCID       = lpPktTr->m_dwCustomerID;
	unsigned long   dwNPCID     = lpPktTr->m_dwOwnerID;

	unsigned short  usError     = 0;
	Item::ItemPos   itemPos     = lpPktTr->m_TakeType.m_srcPos;
	unsigned char   cNum        = lpPktTr->m_TakeType.m_cNum;

	Item::CItem*    lpItem      = lpCharacter->GetItem(itemPos);
	bool            bEraseItem  = false;

	if (NULL != lpItem)
	{
		bool bStackable = lpItem->IsSet(Item::DetailData::STACKABLE);
		unsigned char cNumOrDurability = lpItem->GetNumOrDurability();

		// ���� ������ �������� ���, �������� Ȯ��
		if (!(bStackable && (cNumOrDurability < cNum)))
		{
			// ������ �Ұ����ϰų�, ���� �Ĵ� ��쿡�� ������ ����
			if (!bStackable || (bStackable && (cNum == lpItem->GetNumOrDurability())))
			{
				bEraseItem = true;
			}

			dwGold = lpItem->GetSellPrice(false) * (bStackable ? cNum : 1);
			const unsigned long dwOriginalGold = dwGold;

			// ��� ��� ������ �Ǹ��ϴ� ��� ������ ����ȴ�.
			if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(dwNPCID))
			{
				CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(dwNPCID));
				if (NULL != lpCampShop)
				{
					unsigned long dwTax = static_cast<unsigned long>(dwOriginalGold * (lpCampShop->GetTax() / 100.0f));
					dwGold -= dwTax;

					// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.
/*
					// edith ���� �κ� �߰� (�ּ�ó�� ����)
					// ��� ����� ���� �ູ ������ ����ִٸ�, �ູ ���ʽ��� ���� ó���� �Ѵ�.
					Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleInBlessArea(lpCampShop->GetPosition());
					if (NULL != lpCastle && 0 != lpCastle->GetGID())
					{
						Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCampShop->GetGID() );
						if (NULL != lpGuild && false == lpGuild->IsEnemyGuild(lpCastle->GetGID()))
						{
							unsigned char cBlessBonus = Castle::CCastleBlessMgr::GetInstance().GetBonusPercent(
								lpCastle->GetTotalGainTaxCount(), lpCastle->GetUpgradeStep());

							// ���� �ູ ���ʽ� ��ŭ �� ���ش�.
							unsigned long dwBlessBonusTax = static_cast<unsigned long>(dwOriginalGold * (cBlessBonus / 100.0f));
							dwTax += dwBlessBonusTax;

							// ���� ��� ��� Gold ������ ����.
							unsigned long dwCastleTax = static_cast<unsigned long>(dwOriginalGold * (lpCastle->GetTax(Castle::CAMP_GOLD_TAX) / 100.0f));
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
					lpCampShop->AddGold(dwTax);
				}
			}
			// ���� ���� NPC ��� �ŷ� ���� ����
			else if (Creature::CT_NPC == Creature::GetCreatureType(dwNPCID))
			{
				CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
				if (NULL != lpNPC && lpNPC->IsBelongToCastle())
				{
					Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByNameID(lpNPC->GetCastleNameID());
					if (lpCastle)
					{
						const unsigned long dwTradeTax = static_cast<unsigned long>(dwOriginalGold * (lpCastle->GetTax(Castle::TRADE_TAX) / 100.0f));

						if (dwTradeTax > 0)
						{
							lpCastle->AddTempTaxMoney(Castle::TRADE_TAX, dwTradeTax);
							dwGold -= dwTradeTax;
						}
					}
				}
			}

			// edith 2010.01.02 ��� ������ ���ʽ� ó����ƾ. �켱 �Ź� ���� �ְ�. Ȯ���Ѵ�.
			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID());
			if (0 != lpGuild)
			{
				float fD = lpGuild->GetGuildBonus(lpCharacter->GetRealmPoint(), 2.0f);

				unsigned long dwGuildGold = lpGuild->GetGuildMiles() + (int)(dwGold * fD);
				// ��忡 ��ȭ�� �����Ѵ�. ���� ���� �۾� �ȵ�.

				if(dwGuildGold >= 100000 && lpCharacter->GetGuildSafe() == -1 && lpGuild->GetGuildSafe() == -1)
				{
					unsigned short wError	= PktBase::NO_SERVER_ERR;

					// dwCharGold : ��Ŷ�� ���� ����� ��
					// dwGold : Ŭ���̾�Ʈ�� ������ �ݾ�
					lpCharacter->SetGuildSafe(PktGuildSafe::SC_DEPOSIT);
					lpGuild->SetGuildSafe(PktGuildSafe::SC_DEPOSIT);

					// ���� ó��
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if (0 != lpDBAgentDispatch)				
					{
						GameClientSendPacket::SendCharGuildSafe(lpDBAgentDispatch->GetSendStream(), 
							lpCharacter->GetCID(), lpGuild->GetGID(), 0, dwGuildGold, PktGuildSafe::SC_BONUS, 
							"GuildMiles", wError);

						lpGuild->SetGuildMiles(0); 
					}
				}
				else 
					lpGuild->SetGuildMiles(dwGuildGold);

			}

			float fd = lpCharacter->GetAbilityValue(Skill::Type::AB_SHOP_MEMBERSHIP)/100.0f;
			dwGold += static_cast<unsigned long>((float)dwGold*fd);

			lpCharacter->AddGold(dwGold, false);

			lpItem->SetNumOrDurability(cNumOrDurability - cNum);
		}
		else
		{
			usError = PktTr::FAIL_DROP_INVEN;
			ERRLOG5(g_Log, "CID:%10u (%2d, 0x%04x)�� ������ ���� : %d�� �ȷ��� ������ ���� : %d��", 
				dwCID, itemPos.m_cPos, itemPos.m_cIndex, cNumOrDurability, cNum);
		}
	}
	else
	{
		usError = PktTr::FAIL_DROP_INVEN;
		ERRLOG3(g_Log, "CID:%10u �������� (%2d, 0x%04x)�� �����ϴ�", dwCID, itemPos.m_cPos, itemPos.m_cIndex);            
	}

	// �ŷ� ������ ������ ( �ȱ� )
	GAMELOG::LogTradeItem(*lpCharacter, dwNPCID, dwGold, lpItem, itemPos, PktTr::TRC_SELL, usError);

	if (true == bEraseItem)
	{
		if (true == lpCharacter->RemoveItem(itemPos))
		{
			DELITEMLOG(DETLOG2(g_Log, "this:0X%08x CID:%10u�� �������� �����մϴ�.", lpItem, m_dwCID));

			DELETE_ITEM(lpItem);
		}
		else
		{
			ERRLOG3(g_Log, "CID:%10u �������� (%2d, 0x%04x)��ġ�κ��� ����� �� �����߽��ϴ�.", 
				dwCID, itemPos.m_cPos, itemPos.m_cIndex);
		}
	}

	return GameClientSendPacket::SendCharTradeItem(GameClientDispatch.GetSendStream(), lpCharacter, 
		dwNPCID, Item::ItemPos(), NULL, itemPos, cNum, usError);
}


bool GameClientParsePacket::ProcessItemBuy(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr)
{
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktTr->GetCmd());

	unsigned long   dwCustomerID	= lpPktTr->m_dwCustomerID;
	unsigned long   dwOwnerID		= lpPktTr->m_dwOwnerID;

	unsigned char	cCmd			= lpPktTr->m_cCmd;

	unsigned short	wBuyItemID		= lpPktTr->m_wBuyItemID;
	TakeType		takeType		= lpPktTr->m_TakeType;

	Item::ItemPos	CouponPos		= lpPktTr->m_CouponPos;

	Item::CItem*	lpCoupon		= NULL;
	unsigned short	wCouponID		= 0;

	unsigned long   dwPrice			= 0;
	unsigned short  usError     	= 0;

	Item::ItemPos	itemPos;

	Item::CItem*    lpItem			= NULL;
	CCreature*		lpCreature		= CCreatureManager::GetInstance().GetCreature(dwOwnerID);
	Item::CEquipment* lpEquip;
	bool	setSocket = false;

	if (NULL != lpCreature)
	{  
		// ����� �ð��� ���� ��� ��� ������ �ִ� �������� �����Ҽ� ����.
		if (CGameTimeMgr::GetInstance().IsGuildWarTime())
		{
			if(lpCharacter->GetGID()!=lpCreature->GetGID())
			{
				if(lpCharacter->GetGID() && lpCreature->GetGID())
				{
					Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID());						

					if(lpGuild)
					{							
						if (lpGuild->IsEnemyGuild(lpCreature->GetGID()))
						{
							// ���� ��� ��� ����.
							usError = PktTr::FAIL_ENEMY_ITEM;
							goto SEND_PACKET;
						}
					}
				}
			}				
		}

		// ���� �ɼ��� ���� ��� ����
		if (PktTr::TRC_BUY_EQUIP == cCmd)
		{
			if (0 == lpPktTr->m_wSize)
			{
				ERRLOG2(g_Log, "CID:%10u �Ǹ���ID:0x%08x ������ ������ ������ �ʰ� �ɼ��� �޸� ��� �����Ϸ� �մϴ�.", 
					dwCustomerID, dwOwnerID);

				usError = PktTr::FAIL_ITEM_BUY;
				goto SEND_PACKET;
			}
			else
			{
				Item::CItemFactory& itemFactory = Item::CItemFactory::GetInstance();

				size_t nItemSize = lpPktTr->m_wSize;
				lpItem = itemFactory.CreateItem(reinterpret_cast<char *>(lpPktTr + 1), nItemSize);

				lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					lpEquip->SetUID(itemFactory.GetNewItemUID());
					lpEquip->SetNewEquip();
				}

				// ���Ŀ� ������ �����ϴ� �Ͱ� ����
				cCmd = PktTr::TRC_BUY_WITH_GOLD;
			}
		}

		// ���� Ʈ���̳ʿ��� �������� ��ų�� ����
		if (PktTr::TRC_BUY_WITH_SKILL_COUPON == cCmd)
		{
			// ��ų�� ����
			lpCoupon = lpCharacter->GetItem(CouponPos);
			if (NULL == lpCoupon)
			{
				ERRLOG3(g_Log, "CID:%10u ������ ���� ���� : ��û�� ��ġ�� ��ų�� ������ �����ϴ�. Pos:%d, Index:%d", 
					lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);

				usError = PktTr::FAIL_ITEM_BUY;
				goto SEND_PACKET;
			}

			wCouponID = lpCoupon->GetPrototypeID();
		}

		// ���� �����ݰ�, ������ ����, ������ �޾Ƽ� ������ ����
		lpItem = lpCreature->SellToCharacter(lpCharacter, wBuyItemID, takeType, lpItem, dwPrice, wCouponID, usError);

		if(0 != usError)
		{
			ERRLOG1(g_Log, "CID:%10u ������ ���� ���� : ��ŷ �ǽ� ��Ŷ.", lpCharacter->GetCID());

			// ���ſ� �����Ͽ����Ƿ� -1 ���������� ������.
			GameClientSendPacket::SendCharTradeItem(GameClientDispatch.GetSendStream(), lpCharacter, dwOwnerID, CouponPos, 
				lpItem, itemPos, takeType.m_cNum, 1);

			// ������ �α׾ƿ� ��Ų��.
			GameClientDispatch.Disconnect();
			GameClientDispatch.Disconnected();
			return true;
		}


		if (NULL != lpItem)
		{
			Item::CInventoryContainer* lpInvenContainer = 
				reinterpret_cast<Item::CInventoryContainer*>(lpCharacter->GetItemContainer(TakeType::TS_INVEN));
			if (NULL == lpInvenContainer)
			{
				ERRLOG1(g_Log, "CID:%10u ������ ���� ���� : �κ��丮 �����̳ʸ� ���� �� �����ϴ�.", lpCharacter->GetCID());

				usError = PktTr::FAIL_ITEM_BUY;
				goto SEND_PACKET;
			}			

			Item::CItem* lpPrevItem = NULL;
			itemPos = lpInvenContainer->GetBlankPos(lpItem->GetPrototypeID(), takeType.m_cNum, false, &lpPrevItem);
			if (TakeType::TS_INVEN == itemPos.m_cPos)
			{
				bool bResult = true;

				switch (cCmd)
				{
				case PktTr::TRC_BUY_WITH_GOLD:
					if(!lpCharacter->DeductGold(dwPrice, false))
					{
						bResult = false;
						dwPrice = 0;
						usError = PktTr::FAIL_ITEM_BUY;
						goto SEND_PACKET;
					}
					break;

				case PktTr::TRC_BUY_WITH_MILEAGE:
					if(!lpCharacter->DeductMileage(dwPrice, false))
					{
						bResult = false;
						dwPrice = 0;
						usError = PktTr::FAIL_ITEM_BUY;
						goto SEND_PACKET;
					}
					break;

				case PktTr::TRC_BUY_WITH_SKILL_COUPON:
					{
						if (takeType.m_cNum <= lpCoupon->GetNumOrDurability())
						{
							lpCoupon->SetNumOrDurability(lpCoupon->GetNumOrDurability() - takeType.m_cNum);
						}
						else
						{
							ERRLOG3(g_Log, "CID:%10u ������ ���� ���� : ��ų�� ������ ������ �̻��մϴ�. Pos:%d, Index:%d", 
								lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);

							DELITEMLOG(ERRLOG2(g_Log, "this:0X%08x CID:%10u �� �������� �����մϴ�.", lpItem, dwCustomerID));

							DELETE_ITEM(lpItem);				

							bResult = false;
							dwPrice = 0;
							usError = PktTr::FAIL_ITEM_BUY;
							goto SEND_PACKET;
						}

						if (0 == lpCoupon->GetNumOrDurability())
						{
							if (lpCharacter->RemoveItem(CouponPos))
							{
								DELETE_ITEM(lpCoupon);
							}
							else
							{
								ERRLOG3(g_Log, "CID:%10u ������ ���� ���� : ��ų�� ������ ������ �� �����ϴ�. Pos:%d, Index:%d", 
									lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);
								bResult = false;
								break;
							}
						}
						break;
					}

				default:
					ERRLOG2(g_Log, "CID:%10u ������ ����� ����� �̻��մϴ�. Cmd:%d", lpCharacter->GetCID(), cCmd);
					bResult = false;
					break;
				}

				if (true == bResult)
				{
					bResult = lpCharacter->SetItem(itemPos, lpItem);
				}

				if (false == bResult)
				{
					lpInvenContainer->DumpItemInfo();

					ERRLOG4(g_Log, "CID:%10u ������ ����:%d�� (%2d:%2d)�� ������ �ֱ� ����.", 
						dwCustomerID, wBuyItemID, itemPos.m_cPos, itemPos.m_cIndex);

					DELITEMLOG(ERRLOG2(g_Log, "this:0X%08x CID:%10u �� �������� �����մϴ�.", lpItem, dwCustomerID));

					DELETE_ITEM(lpItem);				

					dwPrice = 0;
					usError = PktTr::FAIL_PUT_INVEN;
				}
			}
			else
			{
				// ����� �߻��� �� �ִ� ��Ȳ���� �α׸� ���� �ʿ�� ����.
				usError = PktTr::FAIL_FULL_INVEN;
			}
		}
		else
		{
			if (Creature::CT_PC == Creature::GetCreatureType(lpCreature->GetCID()) &&
				reinterpret_cast<CCharacter*>(lpCreature)->GetGold() > ULONG_MAX - dwPrice)
			{
				usError = PktTr::FAIL_GOLD_OVERFLOW;
			}
			else
			{
				usError = PktTr::FAIL_ITEM_BUY;
				ERRLOG2(g_Log, "CID:%10u �Ǹ���ID:0x%08x NPC�Ǵ� �������� �������� �Ǹ��� �� �����ϴ�.", 
					dwCustomerID, dwOwnerID);
			}
		}
	}
	else
	{
		usError = PktTr::FAIL_WRONG_NPC;
		ERRLOG2(g_Log, "CID:%10u �������� ���� �����Դϴ�. ��� ����. CID:%10u", dwCustomerID, dwOwnerID);   
	}

SEND_PACKET:

	GAMELOG::LogTradeItem(*lpCharacter, dwOwnerID, dwPrice, lpItem, itemPos, cCmd, usError);
	return GameClientSendPacket::SendCharTradeItem(GameClientDispatch.GetSendStream(), lpCharacter, dwOwnerID, CouponPos, 
		lpItem, itemPos, takeType.m_cNum, usError);
}



bool GameClientParsePacket::ProcessItemPickUp(CGameClientDispatch& GameClientDispatch, 
											  unsigned __int64 nObjectID, Item::ItemPos itemPos, unsigned short& usError)
{
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, 0);

	unsigned long	dwCID		= lpCharacter->GetCID();
	Item::CItem*    lpItem      = NULL;
	unsigned long   dwGold      = 0;
	unsigned char   cNum        = 0;
	unsigned char   cCreatedItemNumOrDurability = 0;

	// TS_NONE�� ���� ���
	if (TakeType::TS_NONE != itemPos.m_cPos && TakeType::TS_INVEN != itemPos.m_cPos)
	{
		usError = PktPUAck::FAIL_PICKUP_INVEN;
		ERRLOG1(g_Log, "CID:%10u �κ��丮�� �ƴ� ������ �������� ������ �߽��ϴ�.", dwCID);
	}
	else
	{
		// �� ���
		CCell* lpCell = CCellManager::GetInstance().GetCell(nObjectID);
		if (NULL == lpCell) 
		{
			usError = PktPUAck::FAIL_GET_CELL;		
			ERRLOG2(g_Log, "CID:%10u �� ��� ����. OID:0x%08x", dwCID, nObjectID);	
		}
		else
		{
			// �ʵ�� ���� ������ ���
			CCell::ErrorCode eErrorCode = lpCell->GetItem(dwCID, nObjectID, &lpItem, dwGold);

			switch (eErrorCode)
			{
			case CCell::S_SUCCESS:
				{
					if (0 != dwGold)
					{
						// �� �߰�
						lpCharacter->AddGold(dwGold, false);
					}
					else if (NULL != lpItem)		    
					{
						// �κ�/��� �ֱ�
						cNum = lpItem->IsSet(Item::DetailData::STACKABLE) ? lpItem->GetNumOrDurability() : 1;
						cCreatedItemNumOrDurability = lpItem->GetNumOrDurability();

						if (false == lpCharacter->GiveItem(lpItem))
						{
							cNum = 0;
							usError = PktPUAck::FAIL_PICKUP_INVEN;
							ERRLOG1(g_Log, "CID:%10u �κ��� �ֱ� ����", dwCID);
						}
						else
						{
//							SERLOG4(g_Log, "[PickUpItem]CID:%10u, itempPos:%2u , ItemUID: %d, usError:%d",
//								lpCharacter->GetCID(), itemPos.m_cPos, lpItem->GetUID(), usError);

							GAMELOG::LogPickupItem(*lpCharacter, itemPos, lpItem, dwGold, usError);

							// GievItem ���� ���õ� ���
							if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
							{
								DELETE_ITEM(lpItem);
							}

							return true;
						}

						// �������� �� ������ üũ�ϵ��� ��ƾ ���� (2005-04-28 by �ε�)
						/*
						if (false == lpCharacter->Pickup(lpItem, itemPos)) 
						{
						cNum = 0;
						usError = PktPUAck::FAIL_PICKUP_INVEN;
						ERRLOG3(g_Log, "CID:%10u �κ��� �ֱ� ����. (%2d, 0x%04x)", dwCID, itemPos.m_cPos, itemPos.m_cIndex);
						}
						*/
					}
					break;
				}

			case CCell::S_AUTO_ROUTING:			// ������õ�
				usError = PktPUAck::FAIL_AUTO_ROUTING;
				break;

			case CCell::E_NOT_ENOUGH_MEMORY:		
				usError = PktBase::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u �޸𸮰� ������� �ʽ��ϴ�. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_NOT_OWNER_OF_ITEM:    // �������� ����
				usError = PktPUAck::FAIL_NOT_OWNER_OF_ITEM;
				//					DETLOG2(g_Log, "CID:%10u ������ ��� ����. ������ �������� �����ϴ�. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_NOT_ITEM:             // �������� ���� 
				usError = PktPUAck::FAIL_NOT_ITEM;
				//					DETLOG2(g_Log, "CID:%10u ������ ��� ����. �ٸ� ����� ���� ������ �� �ֽ��ϴ�. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_CREATE_ITEM_FAIL:     // ������ ���� ����
				usError = PktPUAck::FAIL_CREATE_ITEM;
				ERRLOG3(g_Log, "CID:%10u ������ ���� ����. OID:0x%08x, ErrorCode = %d", dwCID, nObjectID, usError);
				break;	

			case CCell::E_GOLD_OVERFLOW:		// ���� ������ �ִ� �����ݾ��� �ʰ��� ���
				usError = PktPUAck::FAIL_GOLD_OVERFLOW;
				break;

			default:
				usError = PktPUAck::FAIL_GET_ITEM;
				ERRLOG3(g_Log, "CID:%10u ������ ��� ����. OID:0x%08x, ErrorCode = %d", dwCID, nObjectID, usError);
				break;
			}
		}

		if (PktPUAck::FAIL_NOT_OWNER_OF_ITEM != usError && PktPUAck::FAIL_NOT_ITEM != usError)
		{   
			unsigned char cCurrentNumOrDurability = 0;

			if (NULL != lpItem)
			{
				cCurrentNumOrDurability = lpItem->GetNumOrDurability();

				if (0 == cCurrentNumOrDurability)
				{
					// �������� ���õǾ ���ŵǴ� ���, �α׿� ������ ����� ���� �ϱ� ���ؼ� ���� ����.
					lpItem->SetNumOrDurability(cCreatedItemNumOrDurability);
				}
			}

			// ������ ���� �α� ���� - ������ ������ ������ ���� ������ ���� �߻��� �� �ִ�.
			GAMELOG::LogPickupItem(*lpCharacter, itemPos, lpItem, dwGold, usError);

			if (NULL != lpItem && 0 == cCurrentNumOrDurability)
			{
				// �������� ���õǾ ���ŵǴ� ��쿴���Ƿ�, ������ �� �ֵ��� ������ 0���� ��.
				lpItem->SetNumOrDurability(0);
			}
		}

		if (NULL != lpItem)
		{
			if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
			{
				DELITEMLOG(DETLOG1(g_Log, "this:0X%08x �������� �����մϴ�.", lpItem));

				DELETE_ITEM(lpItem);
			}

			if (PktPUAck::NO_SERVER_ERR != usError && 
				PktPUAck::FAIL_AUTO_ROUTING != usError)
			{
				CCell::ItemInfo itemInfo;
				lpCell->SetItem(lpCharacter->GetCurrentPos(), lpItem, 0, 0, CCell::NONE, itemInfo);
			}
		}
	}

	// �ʵ� �� ������ ���� ����
	return GameClientSendPacket::SendCharPickUp(GameClientDispatch.GetSendStream(), 
		dwCID, nObjectID, dwGold, lpItem, itemPos, cNum, usError);
}