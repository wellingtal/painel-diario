#include "RYLGameData.h"
#include "RYLNetWorkData.h"
#include "RYLRaceBase.h"

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Item/ItemFactory.h>

#include "GUITooltipItem.h"

#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameTimeManager.h"

#include "RYLGuildInfoPanel.h"
#include "RYLGuildFrameDlg.h"
#include "RYLMiningAmountProcessDlg.h"
#include "RYLQuestOpeningDlg.h"
#include "RYLChattingDlg.h"
#include "RYLInventoryPanel.h"
#include "GMMemory.h"

CRYLInventoryPanel::CRYLInventoryPanel(CGMUICustomDlg *pParentDlg)
:	CGMUICustomPanel(pParentDlg), m_dwBackInven(0), 
	m_dwSplitResult(0), m_dwSplitValue(0), m_dwRepairResult(0), 
	m_dwRuneResult(0), m_dwRuneDestuctionResult(0), m_dwSocketResult(0),
    m_lpSplitItem(NULL), m_lpRepairItem(NULL), m_lpRuneExistItem(NULL), m_lpSocketItem(NULL)
{
		firstAbility = true;//cant learn skill after learn ability
}

CRYLInventoryPanel::~CRYLInventoryPanel(void)
{
}

void CRYLInventoryPanel::RenderRectBGColor(LPDIRECT3DDEVICE8 lpD3DDevice, Item::EquipmentPos::Type eIndex, 
										   int nLeft, int nTop, int nRight, int nBottom)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	__int64 ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_EQUIP, eIndex);

	if (ddItem != ITEM_FAILED)
	{
		pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + nLeft, m_pGMUIParentDlg->GetTop() + nTop, 
			m_pGMUIParentDlg->GetLeft() + nRight, m_pGMUIParentDlg->GetTop() + nBottom, 0xFF0000FF, 0x80);
	} 
	else
	{
		int nPos = 0;
		CItemInstance* lpExistItem = pGame->m_csStatus.m_lstEquipment[eIndex];

		if (TRUE == pGame->m_csStatus.GetIsInstallSocket(lpExistItem, pGame->m_lpPickItem) || 
			TRUE == pGame->m_csStatus.GetIsRuneSocket(lpExistItem, pGame->m_lpPickItem, nPos))
		{
			pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + nLeft, m_pGMUIParentDlg->GetTop() + nTop, 
				m_pGMUIParentDlg->GetLeft() + nRight, m_pGMUIParentDlg->GetTop() + nBottom, 0xFFFFFFFF, 0x80);
		} 
		else
		{
			pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + nLeft, m_pGMUIParentDlg->GetTop() + nTop, 
				m_pGMUIParentDlg->GetLeft() + nRight, m_pGMUIParentDlg->GetTop() + nBottom, 0xFFFF0000, 0x80);
		}
	}
}

bool CRYLInventoryPanel::IsSelectableFifthItem(unsigned short wSkillID)
{
	CRYLGameData* pGame	= CRYLGameData::Instance();
	SKILLSLOT* lpSkillSlot = pGame->m_csStatus.GetSkillSlot(wSkillID);
	if(lpSkillSlot)
	{
		if(4 <= lpSkillSlot->SKILLINFO.cLockCount && 0 < lpSkillSlot->SKILLINFO.cSkillLevel)
		{
			char strMessage[MAX_PATH];
			sprintf(strMessage, CRYLStringTable::m_strString[4234]);
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(strMessage);

			return false;
		}
	}

	return true;
}


bool CRYLInventoryPanel::SetEquip(Item::EquipmentPos::Type eIndex)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	Item::ItemPos itemPos;
	itemPos.m_cPos = TakeType::TS_EQUIP;
	itemPos.m_cIndex = eIndex;

	__int64 ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_EQUIP, eIndex);

	if (ddItem != ITEM_FAILED)
	{
		if (ddItem == ITEM_EMPTY)
		{
			// 아이템 이동
			pNetwork->SendTakeItem(pGame->m_lpPickItem, &itemPos, pGame->m_lpPickItem->GetNowDurability());
		} 
		else
		{
			//--//	start..
			short aryEquipmentAttr[Item::Attribute::MAX_ATTRIBUTE_NUM] = { 0, };
			Item::CEquipment* lpEquip = reinterpret_cast<Item::CEquipment*>(pGame->m_csStatus.m_lstEquipment[itemPos.m_cIndex]->m_lpItemBase);
			if (NULL != lpEquip)
			{
				lpEquip->GetAttribute(aryEquipmentAttr, Item::Attribute::MAX_ATTRIBUTE_NUM);
			}

			if (aryEquipmentAttr[Item::Attribute::SKILL_POINT] > 0)
			{
				unsigned short wDiff = pGame->m_csStatus.GetSkillPoint() - pGame->m_csStatus.m_Skill.wSkillNum;

				// 남아있는 스킬 포인트가 착용 아이템로 인해 증가한 수치보다 작을때
				if (wDiff < aryEquipmentAttr[Item::Attribute::SKILL_POINT])
				{
					short aryPickItemAttr[Item::Attribute::MAX_ATTRIBUTE_NUM] = { 0, };
					lpEquip = reinterpret_cast<Item::CEquipment*>(pGame->m_lpPickItem->m_lpItemBase);
					if (NULL != lpEquip)
					{
						lpEquip->GetAttribute(aryPickItemAttr, Item::Attribute::MAX_ATTRIBUTE_NUM);
					}

					if (aryPickItemAttr[Item::Attribute::SKILL_POINT] < aryEquipmentAttr[Item::Attribute::SKILL_POINT] &&
						aryEquipmentAttr[Item::Attribute::SKILL_POINT] - aryPickItemAttr[Item::Attribute::SKILL_POINT] > wDiff)
					{
						unsigned short wDeleteSkillNum = aryEquipmentAttr[Item::Attribute::SKILL_POINT] - wDiff;
						if (aryEquipmentAttr[Item::Attribute::SKILL_POINT] > 0)
						{
							wDeleteSkillNum -= aryPickItemAttr[Item::Attribute::SKILL_POINT];
						}

						char strMessage[MAX_PATH];
						sprintf(strMessage, CRYLStringTable::m_strString[122], wDeleteSkillNum);
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(strMessage);

						return true;
					}
				}
			}

			// 각성 스킬이 5단계 0스킬 보다 큰 경우 5단계 스킬 각성 아이템이 선택이 되지 않도록 막는다.
			switch(lpEquip->GetPrototypeID())
			{
				case 551 : if(false == IsSelectableFifthItem(0x8503)) { return true; } break;
				case 552 : if(false == IsSelectableFifthItem(0x8603)) { return true; } break;
				case 553 : if(false == IsSelectableFifthItem(0x8704)) { return true; } break;
				case 554 : if(false == IsSelectableFifthItem(0x8804)) { return true; } break;
				case 555 : if(false == IsSelectableFifthItem(0x8902)) { return true; } break;
				case 556 : if(false == IsSelectableFifthItem(0x8A05)) { return true; } break;
				case 557 : if(false == IsSelectableFifthItem(0x8B04)) { return true; } break;
				case 558 : if(false == IsSelectableFifthItem(0x8C02)) { return true; } break;
				case 5351 : if(false == IsSelectableFifthItem(0x9302)) { return true; } break;
				case 5352 : if(false == IsSelectableFifthItem(0x9402)) { return true; } break;
				case 5353 : if(false == IsSelectableFifthItem(0x9502)) { return true; } break;
				case 5354 : if(false == IsSelectableFifthItem(0x9606)) { return true; } break;
				case 5355 : if(false == IsSelectableFifthItem(0x9603)) { return true; } break;
				case 5356 : if(false == IsSelectableFifthItem(0x9706)) { return true; } break;
				case 5357 : if(false == IsSelectableFifthItem(0x9804)) { return true; } break;
				default	 : break;
			}

			// 스왑하기
			pNetwork->SendSwapItem(pGame->m_lpPickItem, &itemPos, pGame->m_csStatus.m_lstEquipment[itemPos.m_cIndex]);
		}

		return true;
	} 
	else
	{
		CItemInstance* lpExistItem = pGame->m_csStatus.m_lstEquipment[itemPos.m_cIndex];
		if (NULL != lpExistItem)
		{
			// 소켓 박기
			if (TRUE == pGame->m_csStatus.GetIsInstallSocket(lpExistItem, pGame->m_lpPickItem))
			{
				Item::CEquipment *lpEquip = Item::CEquipment::DowncastToEquipment(lpExistItem->m_lpItemBase);

				unsigned char cType = 0;
				unsigned char cIndex = 0;
				for (; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					if (true == lpEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
					{
						break;
					}
				}

				if (cIndex == Item::Attribute::MAX_ATTRIBUTE_NUM)
				{
					m_dwSocketResult = MB_YES;
				}
				else
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[4223], MB_YES | MB_NO | MB_EXIT);
					lpMessageBox->SetResult(&m_dwSocketResult);
					m_dwSocketResult = 0;
				}

				m_lpSocketItem = lpExistItem;

				return true;
			}

			// 룬 소켓 박기
			int nPosition = 0;
			if (TRUE == pGame->m_csStatus.GetIsRuneSocket(lpExistItem, pGame->m_lpPickItem, nPosition))
			{
				if (Item::EtcItemID::DESTRUCTION_RUNE != pGame->m_lpPickItem->m_lpItemBase->GetPrototypeID())
				{
					MessageInstallRune(lpExistItem, true);
				}
				else
				{
					short aryEquipmentAttr[Item::Attribute::MAX_ATTRIBUTE_NUM] = { 0, };
					Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpExistItem->m_lpItemBase);
					if (NULL != lpEquip)
					{
						lpEquip->GetAttribute(aryEquipmentAttr, Item::Attribute::MAX_ATTRIBUTE_NUM);
					}

					if (aryEquipmentAttr[Item::Attribute::SKILL_POINT] > 0)
					{
						unsigned short wDiff = pGame->m_csStatus.GetSkillPoint() - pGame->m_csStatus.m_Skill.wSkillNum;

						// 남아있는 스킬 포인트가 착용 아이템으로 인해 증가한 수치보다 작을때
						if (wDiff < aryEquipmentAttr[Item::Attribute::SKILL_POINT])
						{
							unsigned short wDeleteSkillNum = aryEquipmentAttr[Item::Attribute::SKILL_POINT] - wDiff;
							char strMessage[MAX_PATH];
							sprintf(strMessage, CRYLStringTable::m_strString[ 123 ], wDeleteSkillNum);
							CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
							lpMessageBox->Create(strMessage);
							return true;
						}
					}

					// 각성 스킬이 5단계 0스킬 보다 큰 경우 5단계 스킬 각성 아이템이 선택이 되지 않도록 막는다.					
					switch(lpEquip->GetPrototypeID())
					{
						case 551 : if(false == IsSelectableFifthItem(0x8503)) { return true; } break;
						case 552 : if(false == IsSelectableFifthItem(0x8603)) { return true; } break;
						case 553 : if(false == IsSelectableFifthItem(0x8704)) { return true; } break;
						case 554 : if(false == IsSelectableFifthItem(0x8804)) { return true; } break;
						case 555 : if(false == IsSelectableFifthItem(0x8902)) { return true; } break;
						case 556 : if(false == IsSelectableFifthItem(0x8A05)) { return true; } break;
						case 557 : if(false == IsSelectableFifthItem(0x8B04)) { return true; } break;
						case 558 : if(false == IsSelectableFifthItem(0x8C02)) { return true; } break;
						case 5351 : if(false == IsSelectableFifthItem(0x9302)) { return true; } break;
						case 5352 : if(false == IsSelectableFifthItem(0x9402)) { return true; } break;
						case 5353 : if(false == IsSelectableFifthItem(0x9502)) { return true; } break;
						case 5354 : if(false == IsSelectableFifthItem(0x9606)) { return true; } break;
						case 5355 : if(false == IsSelectableFifthItem(0x9603)) { return true; } break;
						case 5356 : if(false == IsSelectableFifthItem(0x9706)) { return true; } break;
						case 5357 : if(false == IsSelectableFifthItem(0x9804)) { return true; } break;
						default	 : break;
					}

					MessageDestructionRune(lpExistItem);
				}

				return true;
			}
		}
	}

	return false;
}

bool CRYLInventoryPanel::UsePortal(CItemInstance* lpItem)
{
	Item::CUseItem* lpPortal = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
	if (NULL != lpPortal)
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
		if(pGame->m_csStatus.m_cRidePos == 1)
			return false;

		////////////////////////////////////////////////////////////////////////////////////////////
		// 사용 클래스 제한.
		const Item::UseLimit *lpLimit = &lpItem->GetItemInfo().m_UseLimit;
		if(lpLimit->m_dwClassLimit != 0)
		{
			if (!(lpLimit->m_dwClassLimit & (0x00000001 << (pGame->m_csStatus.m_Info.Class - 1))))
			{
				return false;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
		pGame->SetPortalCounter(1000);
#else
		pGame->SetPortalCounter(15000);
#endif
		pGame->m_lpPortalScroll = lpItem;

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseItem(CItemInstance* lpItem)
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (NULL == pSelfCreature)
	{
		return false;
	}

	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	if (lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > pSelfCreature->GetRealmPoint())
	{
		char szErr[MAX_PATH] = { 0, };
		sprintf(szErr, CRYLStringTable::m_strString[ 3865 ], lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint);
		CRYLChattingDlg::Instance()->AddMessage(szErr, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return false;
	}

	if (true == lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		!CRYLGameTimeManager::Instance()->IsRealmWarTime())
	{
		CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3866 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return false;
	}

	if(lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && CRYLGameTimeManager::Instance()->IsRealmWarTime() && 
		pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
	{
		CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3872 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return false;
	}

	switch (lpItem->GetItemType())
	{
		case Item::ItemType::SKILL_BOOK:			return UseSkillBook(lpItem);
		case Item::ItemType::POTION:				return UsePotion(lpItem);

		case Item::ItemType::PORTAL:
		case Item::ItemType::FIXED_PORTAL:
			return UsePortal(lpItem);

		case Item::ItemType::CASH_ITEM:
			return UseCashItem(lpItem);

		case Item::ItemType::EXP_BOOK:
		case Item::ItemType::CASH_BOOK:
		case Item::ItemType::EVENT_LOTTERY:
			return UseLottery(lpItem);

		case Item::ItemType::FIRE_CRACKER:			return UseFireCracker(lpItem);

		case Item::ItemType::SKILL_ITEM:	
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 3870 ]);
			return false;
		}

		case Item::ItemType::CAMP_KIT:				return UseCampKit(lpItem);
		case Item::ItemType::MINERAL_KIT:			return UseMineralKit(lpItem);

		case Item::ItemType::SHORT_RANGE_ARMS_KIT:
		case Item::ItemType::LONG_RANGE_ARMS_KIT:
		case Item::ItemType::AIRSHIP_KIT:
			return UseStartKit(lpItem);

		case Item::ItemType::WORLDWEAPON_K_KIT:		return UseWorldWeaponKit(lpItem, Creature::KARTERANT);
		case Item::ItemType::WORLDWEAPON_M_KIT:		return UseWorldWeaponKit(lpItem, Creature::MERKADIA);

		case Item::ItemType::QUEST_ITEM:			return UseQuestItem(lpItem);
	}

	return false;
}

bool CRYLInventoryPanel::UseQuestItem(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	if (NULL != lpItem)
	{
		unsigned short wQuestID = lpItem->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_wQuestID;
		if (0 == wQuestID)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 3577 ]);
			return true;
		}

		QuestNode* lpQuest = g_QuestList.GetQuest(wQuestID);
		if (NULL != lpQuest)
		{
			CRYLGameData* pGame = CRYLGameData::Instance();

			unsigned short aryComplete[1000], aryQuest[QuestInstance::MAX_EXECUTING_QUEST];
			ZeroMemory(aryComplete, sizeof(aryComplete));
			ZeroMemory(aryQuest, sizeof(aryQuest));

			unsigned long i = 0;
			list<LPQuestInstance>::iterator it;

			for (it = pGame->m_csStatus.m_lstQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstQuest.end(); ++it, ++i)
			{
				aryQuest[i] = (*it)->m_dwQuestID;
			}

			for (it = pGame->m_csStatus.m_lstCompleteQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstCompleteQuest.end(); ++it, ++i)
			{
				aryComplete[i] = (*it)->m_dwQuestID;
			}

			unsigned long dwResult = lpQuest->CheckQuest(pGame->m_csStatus.m_Info.Level, pGame->m_csStatus.m_Info.Fame, pGame->m_csStatus.m_Info.Class,
				pGame->m_cAccountNation, aryComplete, static_cast<unsigned short>(i), aryQuest);

			switch (dwResult)
			{
				case QuestNode::RES_SUCCESS:
				{
					CRYLGameScene*			pScene		= (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
					CRYLQuestOpeningDlg*	pQuestDlg	= (CRYLQuestOpeningDlg*)pScene->GetQuestOpeningDlg();

					pQuestDlg->SetVisible(TRUE);
					pQuestDlg->SetQuestItemPos(lpItem->m_lpItemBase->GetPos());
					pQuestDlg->SetText(0, wQuestID, lpQuest->m_strQuestTitle, lpQuest->m_strQuestLevel,
						lpQuest->m_strQuestDesc, lpQuest->m_strQuestAward);
					break;
				}

				case QuestNode::RES_MISMATCH_NATION:
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[2951]);
					break;
				}

				case QuestNode::RES_MISMATCH_CLASS:	
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[957]);
					break;
				}

				case QuestNode::RES_ALREADY_TAKEN:	
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[958]);
					break;
				}

				case QuestNode::RES_NOT_COMPLETE_PRIOR_QUEST:
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					QuestNode* pCompletedQuest = g_QuestList.GetQuest(lpQuest->m_dwCompletedQuest);
					if (pCompletedQuest)
					{
						char strMessage[MAX_PATH];
						sprintf(strMessage, CRYLStringTable::m_strString[63], pCompletedQuest->m_strQuestTitle);
						lpMessageBox->Create(strMessage);
					} 
					else
					{
						lpMessageBox->Create(CRYLStringTable::m_strString[960]);
					}
					break;
				}

				case QuestNode::RES_HIGH_THAN_MAX_LEVEL:
				case QuestNode::RES_10_LOW_THAN_MIN_LEVEL:
				case QuestNode::RES_LOW_LEVEL:		
				{
					char strMessage[MAX_PATH];
					sprintf(strMessage, CRYLStringTable::m_strString[961], lpQuest->m_wMinLevel, lpQuest->m_wMaxLevel);
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(strMessage);
					break;
				}

				case QuestNode::RES_LOW_FAME :					// 명성부족
				case QuestNode::RES_HIGH_THAN_MAX_FAME :		// 명성 높다
				{
					char strMessage[MAX_PATH];
					sprintf(strMessage, CRYLStringTable::m_strString[973], lpQuest->m_dwMinFame, lpQuest->m_dwMaxFame);
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(strMessage);
					break;
				}
			}

			return true;
		}
	}

	return false;
}

bool CRYLInventoryPanel::UseSkillBook(CItemInstance* lpItem)
{
	if (NULL != lpItem)
	{
		CRYLGameData*		pGame		= CRYLGameData::Instance();
		CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();
		if((lpItem->GetSkillID() <0x8000)&&(firstAbility))//hz ability learn
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			char strMessage[MAX_PATH];
			sprintf(strMessage, "Abilities lock your skill tree.\\To be able to learn skills again,\\Delete your abilities first.");
			lpMessageBox->Create(strMessage);
			firstAbility=false;
		}

		// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
		if(pGame->m_csStatus.m_cRidePos == 1)
			return false;

		CRYLMessageBox* lpMessageBox = NULL;

		unsigned long dwResult = pGame->m_csStatus.CheckUseSkillBook(lpItem);
		switch (dwResult)
		{
			case 0:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[163]);	break;

			case 1:
			{
				CEffScript* test_script = new CEffScript;

				test_script->SetInterfaceSet(true);
				test_script->GetScriptBinData("inter_book.esf");	//wasnt commented out

				test_script->m_InterfacePos[0] = (lpItem->m_wPutX) + 26;
				test_script->m_InterfacePos[1] = (lpItem->m_wPutY) + 26;

				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);

				g_pClientSocket->SetStatusFlag(NS_USEITEM);
				SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpItem->m_lpItemBase->GetPos());
				break;
			}

			case 2:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[164]);	break;
			case 3:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[165]);	break;
			case 4:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[163]);	break;
			case 5:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[166]);	break;
			case 6:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[167]);	break;
			case 7:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[163]);	break;
			case 8:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[168]);	break;
			case 9:	lpMessageBox = new CRYLMessageBox;	lpMessageBox->Create(CRYLStringTable::m_strString[169]);	break;
		}

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UsePotion(CItemInstance* lpItem)
{
	Item::CUseItem* lpPotion = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
	if (NULL != lpPotion)
	{
		CRYLGameData*		pGame		= CRYLGameData::Instance();
		CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

		// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
		if(pGame->m_csStatus.m_cRidePos == 1)
			return false;

		if (true == pGame->m_csStatus.CheckUsePotion(lpItem))
		{
			// edith 2009.01.30 포션을 먹으면 멈칫 거리는 현상이 아래 로직을 주석처리하니 
			// 멈칫 거리는 현상이 사라졌다. 
//			g_pClientSocket->SetStatusFlag(NS_USEITEM);
			if (SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpPotion->GetPos()))
			{
				pGame->SetPotionCoolDown(lpPotion->GetItemInfo().m_UseItemInfo.m_usSkill_ID);//hz fix for cooldown hack?
			}
		}

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseCashItem(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	if(pGame->m_bUseCashFlag)
		return false;

	Item::CUseItem* lpLottery = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
	if (NULL != lpLottery)
	{
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance();

		CEffScript* test_script = new CEffScript;

		test_script->SetInterfaceSet(true);
		test_script->GetScriptBinData("inter_book.esf");	

		test_script->m_InterfacePos[0] = (lpItem->m_wPutX) + 26;
		test_script->m_InterfacePos[1] = (lpItem->m_wPutY) + 26;

		CSceneManager::m_EffectManager.AddInterfaceScript(test_script);

		pGame->m_bUseCashFlag = TRUE;
		g_pClientSocket->SetStatusFlag(NS_USECASHITEM);
		SendPacket::CharUseCashItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpLottery->GetPos());

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseLottery(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	Item::CUseItem* lpLottery = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
	if (NULL != lpLottery)
	{
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance();

		CEffScript* test_script = new CEffScript;

		test_script->SetInterfaceSet(true);
		test_script->GetScriptBinData("inter_book.esf");	

		test_script->m_InterfacePos[0] = (lpItem->m_wPutX) + 26;
		test_script->m_InterfacePos[1] = (lpItem->m_wPutY) + 26;

		CSceneManager::m_EffectManager.AddInterfaceScript(test_script);

		g_pClientSocket->SetStatusFlag(NS_USEITEM);
		SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpLottery->GetPos());

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseFireCracker(CItemInstance *lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	Item::CUseItem* lpFireCracker = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
	if (NULL != lpFireCracker)
	{
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance();

		CEffScript* test_script = new CEffScript;

		test_script->SetInterfaceSet(true);
		test_script->GetScriptBinData("inter_book.esf");	

		test_script->m_InterfacePos[0] = (lpItem->m_wPutX) + 13;
		test_script->m_InterfacePos[1] = (lpItem->m_wPutY) + 26;

		CSceneManager::m_EffectManager.AddInterfaceScript(test_script);

		g_pClientSocket->SetStatusFlag(NS_USEITEM);
		SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpFireCracker->GetPos());

		return true;
	}

	return false;
}

void CRYLInventoryPanel::MessageInstallRune(CItemInstance* lpExistItem, bool bCurrentEquip)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	if (NULL != pGame->m_lpPickItem)
	{
		const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(pGame->m_lpPickItem->m_lpItemBase->GetPrototypeID());
		if (NULL == lpItemInfo)
		{
			return;
		}

		char strString[1024];
		char strType[MAX_PATH];

		BOOL bLimit = FALSE;
		int dwValue = lpItemInfo->m_UseLimit.m_wLimitValue;

		switch (lpItemInfo->m_UseLimit.m_cLimitStatus)
		{
			case Skill::StatusLimit::STR:
				strcpy(strType, CRYLStringTable::m_strString[68]);
				if (pGame->m_csStatus.GetSTR() < dwValue)
					bLimit = TRUE;
				break;

			case Skill::StatusLimit::DEX:
				strcpy(strType, CRYLStringTable::m_strString[69]);
				if (pGame->m_csStatus.GetDEX() < dwValue)
					bLimit = TRUE;
				break;

			case Skill::StatusLimit::CON:
				strcpy(strType, CRYLStringTable::m_strString[70]);
				if (pGame->m_csStatus.GetCON() < dwValue)
					bLimit = TRUE;
				break;

			case Skill::StatusLimit::INT:
				strcpy(strType, CRYLStringTable::m_strString[71]);
				if (pGame->m_csStatus.GetINT() < dwValue)
					bLimit = TRUE;
				break;

			case Skill::StatusLimit::WIS:
				strcpy(strType, CRYLStringTable::m_strString[72]);
				if (pGame->m_csStatus.GetWIS() < dwValue)
					bLimit = TRUE;
				break;

			case Skill::StatusLimit::LEVEL:
				strcpy(strType, CRYLStringTable::m_strString[3558]);
				if (pGame->m_csStatus.GetLevel() < dwValue)
					bLimit = TRUE;
				break;

			default:
				strcpy(strType, "");
				break;
		}

		CRYLMessageBox* lpMessageBox = NULL;
		if (NULL != lpExistItem && true == bCurrentEquip)
		{
			if (bLimit)
			{
				sprintf(strString, CRYLStringTable::m_strString[3559] , strType, dwValue);
				strcat(strString, CRYLStringTable::m_strString[3560]);
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(strString);
				m_dwRuneResult = 0;
				return;
			}
		}

		sprintf(strString, CRYLStringTable::m_strString[3559], strType, dwValue);
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(strString, MB_YES | MB_NO | MB_EXIT);
		lpMessageBox->SetResult(&m_dwRuneResult);
		m_lpRuneExistItem = lpExistItem;
		m_dwRuneResult = 0;
	}
}

void CRYLInventoryPanel::MessageDestructionRune(CItemInstance* lpExistItem)
{
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(CRYLStringTable::m_strString[3561], MB_YES | MB_NO | MB_EXIT);
	lpMessageBox->SetResult(&m_dwRuneDestuctionResult);
	m_lpRuneExistItem = lpExistItem;
	m_dwRuneDestuctionResult = 0;
}

bool CRYLInventoryPanel::ProcessMessageBoxResult(void)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	if (m_dwRepairResult & MB_YES)
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			m_dwRepairResult = 0;
			m_lpRepairItem = NULL;

			return true;
		}

		// 수리하기 (패킷 전송)
		SendPacket::CharRepairItem(g_GameSession, pGame->m_dwTradingNpcID, &m_lpRepairItem->m_lpItemBase->GetPos());

		m_dwRepairResult = 0;
		m_lpRepairItem = NULL;
		return true;
	} 
	else if (m_dwRepairResult & MB_NO || m_dwRepairResult & MB_EXIT)
	{
		m_dwRepairResult = 0;
		m_lpRepairItem = NULL;
		return true;
	}

	if (m_dwRuneResult & MB_YES)
	{
		// 룬 (패킷 전송)
		int pos = 0;
		if (pGame->m_csStatus.GetIsRuneSocket(m_lpRuneExistItem, pGame->m_lpPickItem, pos))
		{
			SendPacket::CharRuneInstallSocket(g_GameSession, pNetwork->m_dwMyChrID, pos,
				&m_lpRuneExistItem->m_lpItemBase->GetPos(), &pGame->m_lpPickItem->m_lpItemBase->GetPos());
		}

		m_dwRuneResult= 0;
		m_lpRuneExistItem = NULL;
		return true;
	} 
	else if (m_dwRuneResult & MB_NO || m_dwRuneResult & MB_EXIT)
	{
		m_dwRuneResult = 0;
		m_lpRuneExistItem = NULL;
		return true;
	}

	if (m_dwRuneDestuctionResult & MB_YES)
	{
		// 룬 소멸 (패킷 전송)
		int pos = 0;
		if (pGame->m_csStatus.GetIsRuneSocket(m_lpRuneExistItem, pGame->m_lpPickItem, pos))
		{
			SendPacket::CharRuneRemoveSocket(g_GameSession, pNetwork->m_dwMyChrID, pos,
				&m_lpRuneExistItem->m_lpItemBase->GetPos(), &pGame->m_lpPickItem->m_lpItemBase->GetPos());
		}

		m_dwRuneDestuctionResult= 0;
		m_lpRuneExistItem = NULL;
		return true;
	} 
	else if (m_dwRuneDestuctionResult & MB_NO || m_dwRuneDestuctionResult & MB_EXIT)
	{
		m_dwRuneDestuctionResult = 0;
		m_lpRuneExistItem = NULL;
		return true;
	}

	if (m_dwSplitResult & MB_YES)
	{
		// 나누기 (패킷 전송)
		if (m_dwSplitValue)
		{
			if (m_dwSplitValue >= m_lpSplitItem->GetNowDurability())
			{
				pGame->SetPickItem(m_lpSplitItem, FALSE);
				pGame->m_csStatus.DeleteInventoryItem(m_lpSplitItem);
			} 
			else
			{
				pGame->m_lpPickItem = new CItemInstance;
				pGame->m_lpPickItem->m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(m_lpSplitItem->GetProtoTypeID());
				pGame->m_lpPickItem->m_lpItemBase->MoveItem(m_lpSplitItem->m_lpItemBase->GetPos());
				pGame->m_lpPickItem->SetNowDurability(m_dwSplitValue);

				pGame->m_lpPickItem->SetItemPos(TakeType::TS_INVEN);
				pGame->m_lpPickItem->m_dwItemUID = m_lpSplitItem->m_dwItemUID;
				pGame->m_lpPickItem->m_lCoolDownTimeCount = m_lpSplitItem->m_lCoolDownTimeCount;
				pGame->m_lpPickItem->m_lpSprite = new CRYLSprite;
				pGame->m_lpPickItem->m_lpSprite->Create(0, 0, 
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinX,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinY,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxX,
					pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);
				pGame->m_lpPickItem->m_lpTooltip = new CGUITooltipItem;
				((CGUITooltipItem *)pGame->m_lpPickItem->m_lpTooltip)->SetItem(pGame->m_lpPickItem);

				unsigned long dwSplitValue = m_lpSplitItem->GetNowDurability() - m_dwSplitValue;
				m_lpSplitItem->SetNowDurability(dwSplitValue);

				pGame->m_csStatus.SetQuickSlot(m_lpSplitItem);
			}
		}

		m_dwSplitResult = 0;
		m_dwSplitValue  = 0;
		m_lpSplitItem   = NULL;

		return true;
	} 
	else if (m_dwSplitResult & MB_NO || m_dwSplitResult & MB_EXIT)
	{
		m_dwSplitResult = 0;
		m_dwSplitValue  = 0;
		m_lpSplitItem   = NULL;

		return true;
	}

	if (m_dwSocketResult & MB_YES)
	{
		// 소켓에 보석 박기 (패킷 전송)
		SendPacket::CharInstallSocket(g_GameSession, pNetwork->m_dwMyChrID,
			&m_lpSocketItem->m_lpItemBase->GetPos(), &pGame->m_lpPickItem->m_lpItemBase->GetPos());

		m_dwSocketResult = 0;
		m_lpSocketItem = NULL;
		return true;
	} 
	else if (m_dwSocketResult & MB_NO || m_dwSocketResult & MB_EXIT)
	{
		m_dwSocketResult = 0;
		m_lpSocketItem = NULL;
		return true;
	}

	return false;
}

bool CRYLInventoryPanel::ProcessItemOverlapEvent(__int64 ddItem, Item::ItemPos itemPos)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	CItemInstance* lpExistItem = pGame->m_csStatus.GetInventoryItem(ddItem);
	if (NULL != lpExistItem)
	{
		if (TRUE == pGame->m_lpPickItem->GetIsEnableStack())
		{
			CItemInstance* lpStallItem = pGame->m_csStatus.GetStreetTradeItem(lpExistItem->m_dwItemUID);
		 	if (NULL == lpStallItem)
			{
				// 소켓에 보석 박기
				if (pGame->m_csStatus.GetIsInstallSocket(lpExistItem, pGame->m_lpPickItem))
				{
					Item::CEquipment *lpEquip = Item::CEquipment::DowncastToEquipment(lpExistItem->m_lpItemBase);

					unsigned char cType = 0;
					unsigned char cIndex = 0;
					for (; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
					{
						if (true == lpEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
						{
							break;
						}
					}

					if (cIndex == Item::Attribute::MAX_ATTRIBUTE_NUM)
					{
						m_dwSocketResult = MB_YES;
					}
					else
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[4223], MB_YES | MB_NO | MB_EXIT);
						lpMessageBox->SetResult(&m_dwSocketResult);
						m_dwSocketResult = 0;
					}

					m_lpSocketItem = lpExistItem;

					return true;
				}

				// 아이템 스택
				if (lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID() && 
					lpExistItem->GetNowDurability() < lpExistItem->GetMaxDurability())
				{
					if (itemPos.GetXIndex() == lpExistItem->m_lpItemBase->GetPos().GetXIndex() && 
						itemPos.GetYIndex() == lpExistItem->m_lpItemBase->GetPos().GetYIndex())
					{
						unsigned char cNum = lpExistItem->GetMaxDurability() - lpExistItem->GetNowDurability();
						if (cNum >= pGame->m_lpPickItem->GetNowDurability())
						{
							cNum = pGame->m_lpPickItem->GetNowDurability();
						}

						pNetwork->SendTakeItem(pGame->m_lpPickItem, &itemPos, cNum);
						return true;
					}
				}

				// 룬 소켓
				int nPos = 0;
				if (TRUE == pGame->m_csStatus.GetIsRuneSocket(lpExistItem, pGame->m_lpPickItem, nPos))
				{
					if (Item::EtcItemID::DESTRUCTION_RUNE != pGame->m_lpPickItem->m_lpItemBase->GetPrototypeID())
					{
						MessageInstallRune(lpExistItem, false);									
					}
					else
					{
						MessageDestructionRune(lpExistItem);
					}

					return true;
				}

				// 아이템 합성
				unsigned char cResult = 0;
				if (TRUE == pGame->m_csStatus.GetIsChemicalitem(lpExistItem, pGame->m_lpPickItem, cResult))
				{
					g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

					SendPacket::CharItemChemical(g_GameSession, pNetwork->m_dwMyChrID,
						&pGame->m_lpPickItem->m_lpItemBase->GetPos(), &lpExistItem->m_lpItemBase->GetPos(), 
						pGame->m_lpPickItem->m_lpItemBase->GetNumOrDurability());

					return true;
				}
				else
				{
					CRYLMessageBox* lpMessageBox = NULL;
					switch (cResult)
					{
						case Item::Chemical::F_WRONG_RECIPE:
							break;

						case Item::Chemical::F_WRONG_RATE:
							lpMessageBox = new CRYLMessageBox;
							lpMessageBox->Create(CRYLStringTable::m_strString[2252]);
							break;

						case Item::Chemical::F_NOT_ENOUGH_ITEM:
							lpMessageBox = new CRYLMessageBox;
							lpMessageBox->Create(CRYLStringTable::m_strString[2253]);
							break;

						default:
							break;
					}
				}
			}
		}

		// 스왑하기
		CItemInstance* lpInvenItem = NULL;
		if (pGame->m_lpPickItem->GetItemPos() != TakeType::TS_TEMP)
		{
			lpInvenItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
		}

		if (NULL == lpInvenItem)
		{
			pNetwork->SendSwapItem(pGame->m_lpPickItem, &itemPos, lpExistItem);
		}
	}

	return false;
}

bool CRYLInventoryPanel::UseStartKit(CItemInstance *lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	if (NULL != CRYLGameData::Instance()->m_lpStartKit)
	{
		// 아직 스타트 킷을 사용하지 않았다.
		return false; 
	}

	// 공성전 시간에만 구축 가능 (종료 5분전에는 구축 불가능)
	/*if ( GameTime::ST_SIEGE_WAR != CRYLGameTimeManager::Instance()->GetNowSiegeTime() )
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4064 ]);
		return false;
	}*/

/*	if (SERVER_ID::STONE_WAR1 > CRYLNetworkData::Instance()->m_dwPresentZone || CRYLNetworkData::Instance()->m_dwPresentZone > SERVER_ID::STONE_WAR3)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4068 ]);
		return false;
	}
*/
	if (SERVER_ID::CAPITAL != CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4068 ]);
		return false;
	}
	CRYLGameData::Instance()->m_lpStartKit = lpItem;

	CRYLGameScene* pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
	pScene->GetArmsCreatMenuDlg()->SetVisible(TRUE);

	return true;
}

bool CRYLInventoryPanel::UseMineralKit(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

	if (NULL != pSelfCreature)
	{
		CharDataNode* pSelfNode = RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());

		if (NULL != pSelfNode)
		{
			CRYLGameScene*				pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
			CRYLMiningAmountProcessDlg* pDlg = (CRYLMiningAmountProcessDlg*)pScene->GetMiningAmountProcessDlg();

			// 평화길드 체크, 마을 체크, 진지를 지을수 있는 곳인지 체크
			bool bCampBuildEnableFrist = false, bCampBuildEnableSecond = true;
			if (CSceneManager::ms_pSceneEventMgr)
			{
				bCampBuildEnableFrist = CSceneManager::ms_pSceneEventMgr->IsEnableStructCamp();
			}

			float fHeight = CSceneManager::m_HeightField.GetHeight((*pSelfCreature->GetPosition()));
			if (pSelfCreature->GetPosition()->y > fHeight + 10.0f)
			{
				bCampBuildEnableSecond = false;
			}

			if (bCampBuildEnableFrist && bCampBuildEnableSecond)
			{
				if (!pDlg->GetVisible())
				{
					pDlg->SetKitItem(lpItem);
					pDlg->SetVisible(TRUE);
				}
			}
			else
			{
				CRYLMessageBox* pMessageBox = new CRYLMessageBox;
				pMessageBox->Create(CRYLStringTable::m_strString[3563]);				
			}
		}

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseCampKit(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	if (NULL != CRYLGameData::Instance()->m_lpCampStartKit)
	{
		// 아직 스타트 킷을 사용하지 않았다.
		return false; 
	}

	// 길드전 시간에만 구축 가능 (종료 30분전에는 구축 불가능)
	/*if ( GameTime::GT_GUILD_WAR != CRYLGameTimeManager::Instance()->GetNowGuildTime() )
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4062 ]);
		return false;
	}*/
/*
	if (SERVER_ID::ZONE9 == CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4231 ]);
		return false;
	}
*/
	// 현재 길드요새는 카나번에다만 지을수 있으니 이렇게 하면 다 걸러진다.
	if (SERVER_ID::CAPITAL != CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 3579 ]);
//		pMessageBox->Create(CRYLStringTable::m_strString[ 3459 ]);	// 이상해서 바꿨음.
		return false;
	}
/*
	if (SERVER_ID::STONE_WAR1 <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 3579 ]);
		return false;
	}
*/
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (NULL != pSelfCreature)
	{
		if ( Creature::WAR_OFF == pSelfCreature->GetGuildWarFlag() )
		{
			CRYLMessageBox* pMessageBox = new CRYLMessageBox;
			pMessageBox->Create( CRYLStringTable::m_strString[ 4057 ] );
			return false;
		}

		CharDataNode* pSelfNode = RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());
		if (NULL != pSelfNode)
		{
			CRYLGameScene*		 pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
			CRYLGuildFrameDlg*	 pDlg   = (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg();
			CRYLGuildInfoPanel*  pPanel = (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();

			// 평화길드 체크, 마을 체크, 진지를 지을수 있는 곳인지 체크
			bool bCampBuildEnableFrist = false, bCampBuildEnableSecond = false, bCampBuildEnableThird = true;
			if (CSceneManager::ms_pSceneEventMgr)
			{
				bCampBuildEnableFrist = CSceneManager::ms_pSceneEventMgr->IsEnableStructCamp();
			}

			bCampBuildEnableSecond = CSceneManager::m_CharacterManager.GetChrBuildEnable();

			float fHeight = CSceneManager::m_HeightField.GetHeight((*pSelfCreature->GetPosition()));
			if (pSelfCreature->GetPosition()->y > fHeight + 10.0f)
			{
				bCampBuildEnableThird = false;
			}

			if (bCampBuildEnableFrist && bCampBuildEnableSecond && bCampBuildEnableThird)
			{
				CRYLGameData::Instance()->m_lpCampStartKit = lpItem;

				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2666 ], MB_EXIT | MB_YES | MB_NO);
				lpMessageBox->SetResult(&CRYLGameData::Instance()->m_lpInterface->m_dwCampCreateResult);
			}
			else
			{
				CRYLMessageBox* pMessageBox = new CRYLMessageBox;

				if (!bCampBuildEnableFrist)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3026 ]);
				else if (!bCampBuildEnableSecond)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3027 ]);
				else if (!bCampBuildEnableThird)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3028 ]);
				else
					pMessageBox->Create(CRYLStringTable::m_strString[ 2667 ]);
			}
		}

		return true;
	}

	return false;
}

bool CRYLInventoryPanel::UseWorldWeaponKit(CItemInstance* lpItem, unsigned char cNation)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	// 라이더를 타고 있을땐 포션계열을 못먹게 막는다.
	if(pGame->m_csStatus.m_cRidePos == 1)
		return false;

	// 월드 웨폰을 지을 수 없도록 막는다.
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(CRYLStringTable::m_strString[3874]);
	return false;

	if (NULL != CRYLGameData::Instance()->m_lpWorldWeaponKit)
	{
		// 아직 스타트 킷을 사용하지 않았다.
		return false; 
	}

	// 길드전 시간에만 구축 가능 (종료 30분전에는 구축 불가능)
	if ( GameTime::GT_GUILD_WAR != CRYLGameTimeManager::Instance()->GetNowGuildTime() )
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4063 ]);
		return false;
	}
/*
	if (SERVER_ID::ZONE9 == CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 4231 ]);
		return false;
	}
*/
	if (SERVER_ID::CAPITAL != CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 3579 ]);
//		pMessageBox->Create(CRYLStringTable::m_strString[ 3459 ]);
		return false;
	}
/*
	if (SERVER_ID::STONE_WAR1 <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 3579 ]);
		return false;
	}
*/
	if (cNation != CRYLGameData::Instance()->m_cAccountNation)
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[ 3482 ]);

		return false;
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (pSelfCreature)
	{
		if ( Creature::WAR_OFF == pSelfCreature->GetGuildWarFlag() )
		{
			CRYLMessageBox* pMessageBox = new CRYLMessageBox;
			pMessageBox->Create( CRYLStringTable::m_strString[ 4058 ] );
			return false;
		}

		CharDataNode* pSelfNode = RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());
		if (pSelfNode)
		{
			CRYLGameScene*		 pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
			CRYLGuildFrameDlg*	 pDlg   = (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg();
			CRYLGuildInfoPanel*  pPanel = (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();

			// 평화길드 체크, 마을 체크, 진지를 지을수 있는 곳인지 체크
			bool bCampBuildEnableFrist = false, bCampBuildEnableSecond = false, bCampBuildEnableThird = true;
			if (CSceneManager::ms_pSceneEventMgr)
			{
				bCampBuildEnableFrist = CSceneManager::ms_pSceneEventMgr->IsEnableStructCamp();
			}

			bCampBuildEnableSecond = CSceneManager::m_CharacterManager.GetChrBuildEnable();

			float fHeight = CSceneManager::m_HeightField.GetHeight((*pSelfCreature->GetPosition()));
			if (pSelfCreature->GetPosition()->y > fHeight + 10.0f)
			{
				bCampBuildEnableThird = false;
			}

			if (bCampBuildEnableFrist && bCampBuildEnableSecond && bCampBuildEnableThird)
			{
				CRYLGameData::Instance()->m_lpWorldWeaponKit = lpItem;

				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3483 ], MB_EXIT | MB_YES | MB_NO);
				lpMessageBox->SetResult(&CRYLGameData::Instance()->m_lpInterface->m_dwWorldWeaponCreateResult);
			}
			else
			{
				CRYLMessageBox *pMessageBox;
				pMessageBox = new CRYLMessageBox;

				if (!bCampBuildEnableFrist)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3026 ]);
				else if (!bCampBuildEnableSecond)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3027 ]);
				else if (!bCampBuildEnableThird)
					pMessageBox->Create(CRYLStringTable::m_strString[ 3028 ]);
				else
					pMessageBox->Create(CRYLStringTable::m_strString[ 2667 ]);
			}
		}

		return true;
	}

	return false;
}

