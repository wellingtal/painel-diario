
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
			ERRLOG1(g_Log, "CID:%10u 5단계스킬 락 장비 장착시 5단계 스킬의 락을 걸지 못하는 에러가 발생 했습니다.", dwCID);\
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
				ERRLOG2(g_Log, "CID:%10u 5단계스킬 락 장비 해제시 0x%8x의 5단계 스킬 락을 풀지 못하는 에러가 발생 했습니다.", dwCID, ##SkillType);\
			}\
		}\
		else if(0 < lpSender->GetSkillLevel(##SkillType))\
		{\
			usError = PktTI::FAIL_MOVE_FIFTHSKILL_LOCKITEM;\
			ERRLOG2(g_Log, "CID:%10u 5단계스킬 락 장비를 해제하려면 0x%8x의 5단계 스킬 레벨이 0이어야 합니다.", dwCID, ##SkillType);\
		}\
	}\

namespace GameClientParsePacket
{

	// 아이템 팔기 처리
	bool ProcessItemSell(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr);

	// 아이템 사기 처리
	bool ProcessItemBuy(CGameClientDispatch& GameClientDispatch, PktTr* lpPktTr);

	// 아이템 집기 처리
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


// 아이템 이동 및 스택
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
		ERRLOG5(g_Log, "CID:%10u 아이템 이동 실패 Src에 아이템이 없습니다. SRC:(%2d:0x%04x), DST(%2d:0x%04x)", 
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
				ERRLOG1(g_Log, "CID:%10u 창고에 로그인되지 않은 상태에서 아이템을 옮기려 하였습니다.", dwCID);
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktTI::FAIL_MOVE;
					ERRLOG1(g_Log, "CID:%10u 창고에 물건을 맡기기에 돈이 부족합니다.", dwCID);
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
			// 5단계 장비 아이템 해제
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
			ERRLOG5(g_Log, "CID:%10u 아이템 이동 실패. SRC:(%2d:0x%04x), DST(%2d:0x%04x)", 
				lpCharacter->GetCID(), takeType.m_srcPos.m_cPos, takeType.m_srcPos.m_cIndex,
				takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex);
			usError = PktTI::FAIL_MOVE;
		}
		else 
		{
			if (bTakeMoney)
			{			
				// 이동 성공시, takeType의 src가 TakeType::TS_DEPOSIT이 아니고, dst가 TakeType::TS_DEPOSIT일 경우만 과금한다.
				unsigned long dwLastGold = lpCharacter->GetGold();
				lpCharacter->DeductGold(dwTakeMoney, false);
				GAMELOG::LogTakeGold(*lpCharacter, dwLastGold, lpCharacter->GetGold(), dwTakeMoney,
					TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STORE_USE, 0);
			}

			if(CServerSetup::GetInstance().UseContents(GameRYL::FIFTHSKILL_LOCKITEM))
			{
				// 5단계 장비 아이템 장착
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

	// 아이템 이동 로그를 남깁니다.
	GAMELOG::LogMoveItem(*lpCharacter, takeType, minItemInfo, usError);

	// 데이터를 전송한다.
	return GameClientSendPacket::SendCharTakeItem(GameClientDispatch.GetSendStream(), 
		lpCharacter->GetCID(), takeType, usError);
}


// 아이템 스왑
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
		ERRLOG3(g_Log, "CID:%10u Swap실패 : Src위치에서 아이템을 얻어올 수 없습니다. (%d, 0x%04x)", 
			dwCID, swapSrc.m_srcPos.m_cPos, swapSrc.m_srcPos.m_cIndex);
	}
	else if (NULL == lpDstItem)
	{
		usError = PktSwI::FAIL_DST_MOVE;
		ERRLOG3(g_Log, "CID:%10u Swap실패 : Dst위치에서 아이템을 얻어올 수 없습니다. (%d, 0x%04x)", 
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
				ERRLOG1(g_Log, "CID:%10u 창고에 로그인되지 않은 상태에서 아이템을 스왑하려 하였습니다.", dwCID);
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktSwI::CANT_SWAP;
					ERRLOG1(g_Log, "CID:%10u 창고에 물건을 맡기기에 돈이 부족합니다.", dwCID);
				}
			}
		}

		if (0 == usError)
		{

			if (!lpCharacter->SwapItem(swapSrc, swapDst)) 
			{
				ERRLOG9(g_Log, "CID:%10u 스왑 불가능. SRCTake:(%2d:0x%04x)->(%2d:0x%04x), DSTTake(%2d:0x%04x)->(%2d:0x%04x)",
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

	// 아이템 스왑 로그
	GAMELOG::LogSwapItem(*lpCharacter, swapSrc, swapDst, lpSrcItem, lpDstItem, usError);

	return GameClientSendPacket::SendCharSwapItem(GameClientDispatch.GetSendStream(), lpCharacter->GetCID(), 
		swapSrc, swapDst, usError);
}


// 아이템 수리
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

// 아이템 수리 (장비 아이템 모두 수리)
bool GameClientParsePacket::ParseCharRepairAllItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRpAI), GameClientDispatch);

	PktRpAI*     lpPktRpAI	= static_cast<PktRpAI*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned short usError = PktBase::NO_SERVER_ERR;

	unsigned long dwRepairGold = lpCharacter->RepairAllItem(lpPktRpAI->m_dwCharID, usError);
/*
	// RepairAllItem 안으로 들어갔다.
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

// 캐쉬 아이템 사용
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

	// 아이템 사용시 실패하면 로그를 남기고 오류를 리턴
	if(!lpUseItem->UseCashItem(lpSender, lpRecver, itemPos, usError))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

		// 리턴되는 에러중 죽은 캐릭터가 아이템을 사용하려고 할때 
		// 에러는 0인데 false 가 리턴될수가 있다. 
		// 이러면 내구도가 0으로 날라가기 때문에 아이템을 0개로 확인해 지워버린다.
		unsigned char cRemainItemNum = lpUseItem->GetNumOrDurability();

		return GameClientSendPacket::SendCharUseCashItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
	}

	return true;
}


// 아이템 사용
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

	// 아이템 사용시 실패하면 로그를 남기고 오류를 리턴
	if(!lpUseItem->Use(lpSender, lpRecver, itemPos, usError))
	{
		GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

		// 리턴되는 에러중 죽은 캐릭터가 아이템을 사용하려고 할때 
		// 에러는 0인데 false 가 리턴될수가 있다. 
		// 이러면 내구도가 0으로 날라가기 때문에 아이템을 0개로 확인해 지워버린다.
		unsigned char cRemainItemNum = lpUseItem->GetNumOrDurability();

		return GameClientSendPacket::SendCharUseItem(GameClientDispatch.GetSendStream(), 
			lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
	}

	// 아이템 사용 로그를 남긴다.
	GAMELOG::LogUseItem(*lpCharacter, itemPos, lpUseItem, usError);

	// 아이템 갯수가 0개이면 아이템을 삭제한다.
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

		DELITEMLOG(DETLOG1(g_Log, "this:0X%08x 아이템을 삭제합니다.", lpUseItem));	
		DELETE_ITEM(lpUseItem);
	}

	return GameClientSendPacket::SendCharUseItem(GameClientDispatch.GetSendStream(), 
		lpPktUI->m_dwSender, lpPktUI->m_dwRecver, itemPos, cRemainItemNum, usError);
}


// 아이템 거래
bool GameClientParsePacket::ParseCharTradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	/*	
	// 'TRC_BUY_EQUIP' 명령의 경우 가변길이 아이템이 붙을 수 있으므로 패킷 크기 검사를 하지 않습니다.
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

// NPC 장비 상점 정보
bool GameClientParsePacket::ParseCharEquipShopInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktEquipShopInfo), GameClientDispatch);

	// 장비상의 기획이 완전 랜덤으로 변경됨에 따라 이 패킷은 사용하지 않습니다. (2004-07-26 by 로딘)
	ERRLOG0(g_Log, "클라이언트가 더이상 사용하지 않는 장비상 정보 요청 패킷을 보냈습니다.");
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


// 아이템 집기
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


// 아이템 떨구기
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
		ERRLOG3(g_Log, "CID:%10u 창고에서 바로 땅으로 버릴 수 없습니다. (%2d,0x%04x)", 
			dwCID, itemPos.m_cPos, itemPos.m_cIndex);
	}
	else
	{
		// 셀 얻기
		CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;
		if (NULL == lpCell) 
		{
			usError = PktPDAck::FAIL_GET_CELL; 
			ERRLOG1(g_Log, "CID:%10u 셀 얻기 실패", dwCID);
		}
		else
		{
			// 인벤/장비 로부터 얻기
			Item::CItem* lpItem = lpCharacter->Drop(itemPos, cNum);
			if (NULL == lpItem) 
			{
				usError = PktPDAck::FAIL_DROP_INVEN;
				ERRLOG4(g_Log, "CID:%10u 인벤/장비로 부터 떨구기 실패 (%2d:0x%04x)(%2d)",
					dwCID, itemPos.m_cPos, itemPos.m_cIndex, cNum);		    
			}
		/*	else
			{
				// 스택 아이템이 이상합니다.(0개)
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					usError = PktPDAck::FAIL_ZERO_ITEM;

					DELETE_ITEM(lpItem);
				}
				else
				{
					// 아이템 필드에 떨구기
				//	lpCell->SetItem(lpCharacter->GetCurrentPos(), lpItem, 0, dwCID, CCell::NONE, itemInfo);//hz removed this for dropfix

					// 퀘스트 트리거 체크
					lpCharacter->CheckTrigger(Quest::TRIGGER_PUTON, lpItem->GetPrototypeID(), lpCharacter->GetCurrentPos(), 1);
				}
			}*/
		}
	}

	// 아이템 떨구기 보냄
	GAMELOG::LogDropItem(*lpCharacter, itemPos, itemInfo.m_lpItem, 0, usError);
	return GameClientSendPacket::SendCharPullDown(GameClientDispatch.GetSendStream(),
		dwCID, itemPos, itemInfo, usError);        
}

// 아이템 나누기
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
			"CID:%10u 아이템 스플릿 실패 : (%d, %d) 위치에서 아이템을 얻어올 수 없습니다.",
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
					"CID:%10u 창고에 로그인되지 않은 상태에서 아이템을 나누려 하였습니다.", dwCID));
			}
			else
			{
				dwTakeMoney = PutDepositMoney(lpCharacter);
				if (lpCharacter->GetGold() < dwTakeMoney)
				{
					usError = PktSpItAck::FAIL_SPLIT;
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u 창고에 물건을 맡기기에 돈이 부족합니다.", dwCID));
				}
			}
		}

		if (0 == usError)
		{
			if (!lpSrcItem->IsSet(Item::DetailData::STACKABLE)) 
			{
				usError = PktSpItAck::FAIL_SPLIT;
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u 아이템 스플릿 실패 : 나눌 수 없는 아이템(스택 불가)입니다. 종류 ID : %d",
					dwCID, lpSrcItem->GetPrototypeID()));		    

			}
			else if (lpSrcItem->GetNumOrDurability() <= takeType.m_cNum)
			{
				usError = PktSpItAck::FAIL_SPLIT;
				ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
					"CID:%10u 아이템 스플릿 실패 : Source에 있는 아이템의 개수가 나누려고 하는 아이템의 개수보다 작거나 같습니다."
					"Source의 현재 아이템 개수 %d, 나누려고 하는 아이템의 개수 %d",
					dwCID, lpSrcItem->GetNumOrDurability(), takeType.m_cNum));		        
			}
			else
			{
				// 아이템 생성함.
				lpSplitItem = Item::CItemFactory::GetInstance().CreateItem(lpSrcItem->GetItemInfo());

				if (NULL == lpSplitItem) 
				{
					usError = PktSpItAck::FAIL_SPLIT;
					ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
						"CID:%10u 아이템 스플릿 실패 : 아이템을 생성할 수 없습니다.", dwCID));
				}
				else
				{
					lpSplitItem->SetNumOrDurability(takeType.m_cNum);

					// 아이템 놓기
					if (!lpCharacter->SetItem(takeType.m_dstPos, lpSplitItem)) 
					{
						usError = PktSpItAck::FAIL_SPLIT;
						ERRLOG(nLength = _snprintf(szErrorBuffer, MAX_ERROR_BUFFER, 
							"CID:%10u 아이템 스플릿 실패 : (%d, %d)쪼개서 내려놓고자 하는 자리에, 아이템을 내려 놓을 수 없습니다.",
							dwCID, takeType.m_dstPos.m_cPos, takeType.m_dstPos.m_cIndex));

						DELETE_ITEM(lpSplitItem);
					}
					else 
					{
						// 최종 성공
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

	// 아이템 스플릿 로그 남기기
	GAMELOG::LogSplitItem(*lpCharacter, takeType, lpSrcItem, lpSplitItem, usError);
	return GameClientSendPacket::SendCharSplitItem(GameClientDispatch.GetSendStream(), 
		dwCID, lpSplitItem, takeType, usError);
}


// 돈 다루기
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
		ERRLOG3(g_Log, "CID:%10u 돈 이동 실패. 창고에 로그인 않은 상태입니다. (%2d,%2d)",
			dwCID, cSrcPos, cDstPos);            
		usError = PktTG::FAIL_MOVE;
	}
	else if (!lpCharacter->MoveGold(dwGold, cSrcPos, cDstPos, usError)) 
	{
		// 에러 코드는 안에서 Set함.
		ERRLOG6(g_Log, "CID:%10u 돈 이동 실패. (Pos/Gold) Src:(%d/%d), Dst:(%d/%d), 이동 금액:%d", 
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
				ERRLOG2(g_Log, "CID:%10u 탭 번호가 이상합니다. TabNum:%d", dwCID, cTabNum);
			}
			else if (dwLastGold < dwTabBuyPrice[cTabNum])
			{
				usError = PktDeposit::INSUFFICIENT_MONEY;
				ERRLOG3(g_Log, "CID:%10u 돈이 부족합니다. CurrentGold:%d, TabBuyPrice:%d",
					dwCID, dwLastGold, dwTabBuyPrice[cTabNum]);
			}
			else if (!Deposit.BuyTab(cTabNum))
			{
				usError = PktDeposit::INVALID_ARGUMENT;
				ERRLOG2(g_Log, "CID:%10u 탭 번호가 이상합니다. TabNum:%d", dwCID, cTabNum);
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

		// 패스워드 바꿈.
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
					ERRLOG1(g_Log, "CID:%10u DBAgent에 창고 암호를 저장할 수 없습니다.", dwCID);
				}                
			}
			else
			{
				usError = PktDeposit::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:%10u DBAgent세션을 얻을 수 없습니다.", dwCID);
			}
		}
		else
		{
			usError = PktDeposit::AUTH_FAILED;
			ERRLOG1(g_Log, "CID:%10u 창고 암호를 바꿀 수 없습니다. 암호가 다릅니다.", dwCID);
		}
		break;
	};

	return GameClientSendPacket::SendCharDepositCmd(GameClientDispatch.GetSendStream(), lpPktDeposit->m_cCmd, 0, 0, usError);
}


// 오토 루팅
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

		// 스택 가능한 아이템인 경우, 개수제한 확인
		if (!(bStackable && (cNumOrDurability < cNum)))
		{
			// 스택이 불가능하거나, 전부 파는 경우에는 아이템 제거
			if (!bStackable || (bStackable && (cNum == lpItem->GetNumOrDurability())))
			{
				bEraseItem = true;
			}

			dwGold = lpItem->GetSellPrice(false) * (bStackable ? cNum : 1);
			const unsigned long dwOriginalGold = dwGold;

			// 길드 요새 상점에 판매하는 경우 세율이 적용된다.
			if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(dwNPCID))
			{
				CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(dwNPCID));
				if (NULL != lpCampShop)
				{
					unsigned long dwTax = static_cast<unsigned long>(dwOriginalGold * (lpCampShop->GetTax() / 100.0f));
					dwGold -= dwTax;

					// CASTLE_TODO : 성이 길드 소유가 아니므로 막아둔다.
/*
					// edith 세금 부분 추가 (주석처리 뺐음)
					// 길드 요새가 성의 축복 영역에 들어있다면, 축복 보너스와 세금 처리를 한다.
					Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleInBlessArea(lpCampShop->GetPosition());
					if (NULL != lpCastle && 0 != lpCastle->GetGID())
					{
						Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCampShop->GetGID() );
						if (NULL != lpGuild && false == lpGuild->IsEnemyGuild(lpCastle->GetGID()))
						{
							unsigned char cBlessBonus = Castle::CCastleBlessMgr::GetInstance().GetBonusPercent(
								lpCastle->GetTotalGainTaxCount(), lpCastle->GetUpgradeStep());

							// 성의 축복 보너스 만큼 더 해준다.
							unsigned long dwBlessBonusTax = static_cast<unsigned long>(dwOriginalGold * (cBlessBonus / 100.0f));
							dwTax += dwBlessBonusTax;

							// 성에 길드 요새 Gold 세금을 낸다.
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
			// 성에 속한 NPC 라면 거래 세율 적용
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

			// edith 2010.01.02 길드 적립금 보너스 처리루틴. 우선 매번 돈을 넣고. 확인한다.
			CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCharacter->GetGID());
			if (0 != lpGuild)
			{
				float fD = lpGuild->GetGuildBonus(lpCharacter->GetRealmPoint(), 2.0f);

				unsigned long dwGuildGold = lpGuild->GetGuildMiles() + (int)(dwGold * fD);
				// 길드에 금화를 적립한다. 현재 로직 작업 안됨.

				if(dwGuildGold >= 100000 && lpCharacter->GetGuildSafe() == -1 && lpGuild->GetGuildSafe() == -1)
				{
					unsigned short wError	= PktBase::NO_SERVER_ERR;

					// dwCharGold : 패킷을 보낸 대상의 돈
					// dwGold : 클라이언트가 보내온 금액
					lpCharacter->SetGuildSafe(PktGuildSafe::SC_DEPOSIT);
					lpGuild->SetGuildSafe(PktGuildSafe::SC_DEPOSIT);

					// 정상 처리
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
			ERRLOG5(g_Log, "CID:%10u (%2d, 0x%04x)의 아이템 개수 : %d개 팔려는 아이템 개수 : %d개", 
				dwCID, itemPos.m_cPos, itemPos.m_cIndex, cNumOrDurability, cNum);
		}
	}
	else
	{
		usError = PktTr::FAIL_DROP_INVEN;
		ERRLOG3(g_Log, "CID:%10u 아이템이 (%2d, 0x%04x)에 없습니다", dwCID, itemPos.m_cPos, itemPos.m_cIndex);            
	}

	// 거래 아이템 보내기 ( 팔기 )
	GAMELOG::LogTradeItem(*lpCharacter, dwNPCID, dwGold, lpItem, itemPos, PktTr::TRC_SELL, usError);

	if (true == bEraseItem)
	{
		if (true == lpCharacter->RemoveItem(itemPos))
		{
			DELITEMLOG(DETLOG2(g_Log, "this:0X%08x CID:%10u의 아이템을 삭제합니다.", lpItem, m_dwCID));

			DELETE_ITEM(lpItem);
		}
		else
		{
			ERRLOG3(g_Log, "CID:%10u 아이템을 (%2d, 0x%04x)위치로부터 지우는 데 실패했습니다.", 
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
		// 길드전 시간에 절대 길드 요새 상점에 있는 아이템은 구매할수 없다.
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
							// 적대 길드 요새 상점.
							usError = PktTr::FAIL_ENEMY_ITEM;
							goto SEND_PACKET;
						}
					}
				}
			}				
		}

		// 랜덤 옵션이 붙은 장비 구입
		if (PktTr::TRC_BUY_EQUIP == cCmd)
		{
			if (0 == lpPktTr->m_wSize)
			{
				ERRLOG2(g_Log, "CID:%10u 판매자ID:0x%08x 아이템 정보를 보내지 않고 옵션이 달린 장비를 구매하려 합니다.", 
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

				// 이후엔 돈으로 구입하는 것과 동일
				cCmd = PktTr::TRC_BUY_WITH_GOLD;
			}
		}

		// 상위 트레이너에게 쿠폰으로 스킬북 구입
		if (PktTr::TRC_BUY_WITH_SKILL_COUPON == cCmd)
		{
			// 스킬북 쿠폰
			lpCoupon = lpCharacter->GetItem(CouponPos);
			if (NULL == lpCoupon)
			{
				ERRLOG3(g_Log, "CID:%10u 아이템 구입 실패 : 요청한 위치에 스킬북 쿠폰이 없습니다. Pos:%d, Index:%d", 
					lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);

				usError = PktTr::FAIL_ITEM_BUY;
				goto SEND_PACKET;
			}

			wCouponID = lpCoupon->GetPrototypeID();
		}

		// 현재 소지금과, 아이템 종류, 개수를 받아서 아이템 생성
		lpItem = lpCreature->SellToCharacter(lpCharacter, wBuyItemID, takeType, lpItem, dwPrice, wCouponID, usError);

		if(0 != usError)
		{
			ERRLOG1(g_Log, "CID:%10u 아이템 구입 실패 : 해킹 의심 패킷.", lpCharacter->GetCID());

			// 구매에 실패하였으므로 -1 서버에러를 보낸다.
			GameClientSendPacket::SendCharTradeItem(GameClientDispatch.GetSendStream(), lpCharacter, dwOwnerID, CouponPos, 
				lpItem, itemPos, takeType.m_cNum, 1);

			// 강제로 로그아웃 시킨다.
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
				ERRLOG1(g_Log, "CID:%10u 아이템 구입 실패 : 인벤토리 컨테이너를 얻을 수 없습니다.", lpCharacter->GetCID());

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
							ERRLOG3(g_Log, "CID:%10u 아이템 구입 실패 : 스킬북 쿠폰의 갯수가 이상합니다. Pos:%d, Index:%d", 
								lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);

							DELITEMLOG(ERRLOG2(g_Log, "this:0X%08x CID:%10u 의 아이템을 삭제합니다.", lpItem, dwCustomerID));

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
								ERRLOG3(g_Log, "CID:%10u 아이템 구입 실패 : 스킬북 쿠폰을 제거할 수 없습니다. Pos:%d, Index:%d", 
									lpCharacter->GetCID(), CouponPos.m_cPos, CouponPos.m_cIndex);
								bResult = false;
								break;
							}
						}
						break;
					}

				default:
					ERRLOG2(g_Log, "CID:%10u 아이템 사기의 명령이 이상합니다. Cmd:%d", lpCharacter->GetCID(), cCmd);
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

					ERRLOG4(g_Log, "CID:%10u 아이템 종류:%d를 (%2d:%2d)에 아이템 넣기 실패.", 
						dwCustomerID, wBuyItemID, itemPos.m_cPos, itemPos.m_cIndex);

					DELITEMLOG(ERRLOG2(g_Log, "this:0X%08x CID:%10u 의 아이템을 삭제합니다.", lpItem, dwCustomerID));

					DELETE_ITEM(lpItem);				

					dwPrice = 0;
					usError = PktTr::FAIL_PUT_INVEN;
				}
			}
			else
			{
				// 충분히 발생할 수 있는 상황으로 로그를 찍을 필요는 없다.
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
				ERRLOG2(g_Log, "CID:%10u 판매자ID:0x%08x NPC또는 노점상이 아이템을 판매할 수 없습니다.", 
					dwCustomerID, dwOwnerID);
			}
		}
	}
	else
	{
		usError = PktTr::FAIL_WRONG_NPC;
		ERRLOG2(g_Log, "CID:%10u 존재하지 않은 상인입니다. 사기 실패. CID:%10u", dwCustomerID, dwOwnerID);   
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

	// TS_NONE은 돈인 경우
	if (TakeType::TS_NONE != itemPos.m_cPos && TakeType::TS_INVEN != itemPos.m_cPos)
	{
		usError = PktPUAck::FAIL_PICKUP_INVEN;
		ERRLOG1(g_Log, "CID:%10u 인벤토리가 아닌 곳으로 아이템을 집으려 했습니다.", dwCID);
	}
	else
	{
		// 셀 얻기
		CCell* lpCell = CCellManager::GetInstance().GetCell(nObjectID);
		if (NULL == lpCell) 
		{
			usError = PktPUAck::FAIL_GET_CELL;		
			ERRLOG2(g_Log, "CID:%10u 셀 얻기 실패. OID:0x%08x", dwCID, nObjectID);	
		}
		else
		{
			// 필드로 부터 아이템 얻기
			CCell::ErrorCode eErrorCode = lpCell->GetItem(dwCID, nObjectID, &lpItem, dwGold);

			switch (eErrorCode)
			{
			case CCell::S_SUCCESS:
				{
					if (0 != dwGold)
					{
						// 돈 추가
						lpCharacter->AddGold(dwGold, false);
					}
					else if (NULL != lpItem)		    
					{
						// 인벤/장비에 넣기
						cNum = lpItem->IsSet(Item::DetailData::STACKABLE) ? lpItem->GetNumOrDurability() : 1;
						cCreatedItemNumOrDurability = lpItem->GetNumOrDurability();

						if (false == lpCharacter->GiveItem(lpItem))
						{
							cNum = 0;
							usError = PktPUAck::FAIL_PICKUP_INVEN;
							ERRLOG1(g_Log, "CID:%10u 인벤에 넣기 실패", dwCID);
						}
						else
						{
//							SERLOG4(g_Log, "[PickUpItem]CID:%10u, itempPos:%2u , ItemUID: %d, usError:%d",
//								lpCharacter->GetCID(), itemPos.m_cPos, lpItem->GetUID(), usError);

							GAMELOG::LogPickupItem(*lpCharacter, itemPos, lpItem, dwGold, usError);

							// GievItem 으로 스택된 경우
							if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
							{
								DELETE_ITEM(lpItem);
							}

							return true;
						}

						// 서버에서 빈 공간을 체크하도록 루틴 변경 (2005-04-28 by 로딘)
						/*
						if (false == lpCharacter->Pickup(lpItem, itemPos)) 
						{
						cNum = 0;
						usError = PktPUAck::FAIL_PICKUP_INVEN;
						ERRLOG3(g_Log, "CID:%10u 인벤에 넣기 실패. (%2d, 0x%04x)", dwCID, itemPos.m_cPos, itemPos.m_cIndex);
						}
						*/
					}
					break;
				}

			case CCell::S_AUTO_ROUTING:			// 오토루팅됨
				usError = PktPUAck::FAIL_AUTO_ROUTING;
				break;

			case CCell::E_NOT_ENOUGH_MEMORY:		
				usError = PktBase::SERVER_ERROR;
				ERRLOG2(g_Log, "CID:%10u 메모리가 충분하지 않습니다. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_NOT_OWNER_OF_ITEM:    // 소유권이 없음
				usError = PktPUAck::FAIL_NOT_OWNER_OF_ITEM;
				//					DETLOG2(g_Log, "CID:%10u 아이템 얻기 실패. 아이템 소유권이 없습니다. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_NOT_ITEM:             // 아이템이 없음 
				usError = PktPUAck::FAIL_NOT_ITEM;
				//					DETLOG2(g_Log, "CID:%10u 아이템 얻기 실패. 다른 사람이 먼저 집었을 수 있습니다. OID:0x%08x", dwCID, nObjectID);
				break;

			case CCell::E_CREATE_ITEM_FAIL:     // 아이템 생성 실패
				usError = PktPUAck::FAIL_CREATE_ITEM;
				ERRLOG3(g_Log, "CID:%10u 아이템 생성 실패. OID:0x%08x, ErrorCode = %d", dwCID, nObjectID, usError);
				break;	

			case CCell::E_GOLD_OVERFLOW:		// 돈을 주으면 최대 소지금액을 초과할 경우
				usError = PktPUAck::FAIL_GOLD_OVERFLOW;
				break;

			default:
				usError = PktPUAck::FAIL_GET_ITEM;
				ERRLOG3(g_Log, "CID:%10u 아이템 얻기 실패. OID:0x%08x, ErrorCode = %d", dwCID, nObjectID, usError);
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
					// 아이템이 스택되어서 제거되는 경우, 로그에 개수가 제대로 남게 하기 위해서 개수 조작.
					lpItem->SetNumOrDurability(cCreatedItemNumOrDurability);
				}
			}

			// 아이템 집기 로그 남김 - 소유권 없음과 아이템 없음 에러는 자주 발생할 수 있다.
			GAMELOG::LogPickupItem(*lpCharacter, itemPos, lpItem, dwGold, usError);

			if (NULL != lpItem && 0 == cCurrentNumOrDurability)
			{
				// 아이템이 스택되어서 제거되는 경우였으므로, 제거할 수 있도록 개수를 0으로 함.
				lpItem->SetNumOrDurability(0);
			}
		}

		if (NULL != lpItem)
		{
			if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
			{
				DELITEMLOG(DETLOG1(g_Log, "this:0X%08x 아이템을 삭제합니다.", lpItem));

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

	// 필드 위 아이템 집기 보냄
	return GameClientSendPacket::SendCharPickUp(GameClientDispatch.GetSendStream(), 
		dwCID, nObjectID, dwGold, lpItem, itemPos, cNum, usError);
}