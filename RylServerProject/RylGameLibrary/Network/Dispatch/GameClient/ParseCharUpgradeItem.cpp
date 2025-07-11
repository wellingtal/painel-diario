
#include "stdafx.h"

#include "ParseCharUpgradeItem.h"
#include "SendCharUpgradeItem.h"
#include "GameClientDispatch.h"

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include <Item/Item.h>
#include <Item/ItemStructure.h>
#include <Item/ItemFactory.h>
#include <Item/ItemMgr.h>

#include <Creature/Character/Character.h>
#include <Creature/Npc/NPC.h>

#include <Castle/Castle.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleMgr.h>


// ������ �ν��� ����
bool GameClientParsePacket::ParseCharInstallSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktIS), GameClientDispatch);

	PktIS*      lpPktIS     = static_cast<PktIS *>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID	= lpPktIS->m_dwCharID;
	Item::ItemPos EquipPos	= lpPktIS->m_equipPos;
	Item::ItemPos GemPos	= lpPktIS->m_gemPos;

    Item::CEquipment*   lpEquipment = Item::CEquipment::DowncastToEquipment(lpCharacter->GetItem(EquipPos));
	Item::CItem*        lpGem       = lpCharacter->GetItem(GemPos);

	unsigned short wError = PktBase::NO_SERVER_ERR;
	if (NULL != lpEquipment)
	{
        if (NULL != lpGem)
        {
            wError = lpEquipment->InstallSocket(*lpGem);
        }
        else
        {
            wError = Item::CEquipment::E_GET_GEM_FAILED;
        }
	}
	else
	{
		wError = Item::CEquipment::E_GET_EQUIP_FAILED;
	}

	GAMELOG::LogInstallSocket(*lpCharacter, TakeType(GemPos, EquipPos, 1),	
		lpGem, lpEquipment, static_cast<GAMELOG::ERRType>(wError));
	
	// �� ������ 0�̸� �������� �����մϴ�.
	if (NULL != lpGem)
	{
		if (0 == lpGem->GetNumOrDurability())
		{
			if (lpCharacter->RemoveItem(GemPos))
			{
				DELETE_ITEM(lpGem);
			}
			else
			{
				wError = Item::CEquipment::E_REMOVE_GEM_FAILED;
                ERRLOG1(g_Log, "CID:0x%08x ��ġ�� ������ �����ϴ� �� �����߽��ϴ�", lpCharacter->GetCID());
			}
		}
	}

	if (PktBase::NO_SERVER_ERR == wError && TakeType::TS_EQUIP == EquipPos.m_cPos)
	{
		lpCharacter->CalculateStatusData(false);
	}

    if (PktBase::NO_SERVER_ERR != wError)
    {
        lpEquipment = NULL;
    }

	return GameClientSendPacket::SendCharInstallSocket(GameClientDispatch.GetSendStream(), dwCharID, 
		EquipPos, GemPos, lpEquipment, wError);
}

// ������ �ռ�
bool GameClientParsePacket::ParseCharItemChemical(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktItemChemical), GameClientDispatch);

	PktItemChemical* lpPktIC = static_cast<PktItemChemical *>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID		= lpPktIC->m_dwCharID;
	Item::ItemPos PickkingPos	= lpPktIC->m_pickkingPos;
	Item::ItemPos TargetPos		= lpPktIC->m_targetPos;
	unsigned char cPickkingNum	= lpPktIC->m_cPickkingNum;

	unsigned short wError		= PktBase::NO_SERVER_ERR;

	Item::CItem* lpPickkingItem	= lpCharacter->GetItem(PickkingPos);
	Item::CItem* lpTargetItem	= lpCharacter->GetItem(TargetPos);
	Item::CItem* lpResultItem	= NULL;

	Item::ChemicalInfo chemicalInfo;

	unsigned short wPickkingItemID = 0;
	unsigned char cPickkingItemNum = 0;

	if (NULL == lpPickkingItem || NULL == lpTargetItem)
	{
		ERRLOG5(g_Log, "CID:0x%08x �������� �ʴ� �������Դϴ�. "
			"��� �ִ� ������ ��ġ : (%d, 0x%04x), �ռ� ��� ������ ��ġ : (%d, 0x%04x)", 
			dwCharID, PickkingPos.m_cPos, PickkingPos.m_cIndex, TargetPos.m_cPos, TargetPos.m_cIndex);
		lpCharacter->GetInventory().DumpItemInfo();
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		// ���ø� �� �ٷ� �ռ��Ϸ��� ��� �ӽ� �������� ���ø����ش�.
		if (0 < cPickkingNum && cPickkingNum < lpPickkingItem->GetNumOrDurability())
		{
			Item::ItemPos HoldPos(TakeType::TS_TEMP, Item::ExtraSpacePos::HOLDITEM_POS);			
			Item::CItem* lpHoldItem = lpCharacter->GetItem(HoldPos);
			if (NULL != lpHoldItem)
			{
				ERRLOG1(g_Log, "CID:0x08x ������ �ռ� ���� : �̹� ��� �ִ� �������� �ִ� ���¿��� ���ø� ������ �ռ��� �õ��մϴ�.", 
					dwCharID);
				wError = PktBase::SERVER_ERROR;
				goto SEND_PACKET;
			}

			Item::CItem* lpSplitItem = Item::CItemFactory::GetInstance().CreateItem(lpPickkingItem->GetItemInfo());
			if (NULL == lpSplitItem) 
			{
				ERRLOG1(g_Log, "CID:0x08x ������ �ռ� ���� : �������� ������ �� �����ϴ�.", dwCharID);
				wError = PktBase::SERVER_ERROR;
				goto SEND_PACKET;
			}

			lpSplitItem->SetNumOrDurability(cPickkingNum);

			if (false == lpCharacter->SetItem(HoldPos, lpSplitItem)) 
			{
				ERRLOG1(g_Log, "CID:0x08x ������ �ռ� ���� : �ӽ� ������ ���ø��� �������� ���� �� �����ϴ�.", dwCharID);
				wError = PktBase::SERVER_ERROR;

				DELETE_ITEM(lpSplitItem);				

				goto SEND_PACKET;
			}

			lpPickkingItem->SetNumOrDurability(lpPickkingItem->GetNumOrDurability() - cPickkingNum);

			// ���ø��� �������� ��� �ִ� �������� ��ġ�ؼ� �ռ��� �����Ѵ�.
			lpPickkingItem = lpSplitItem;
			PickkingPos = lpPickkingItem->GetPos();
		}

		chemicalInfo.m_wPickkingItemID = lpPickkingItem->GetPrototypeID();
		chemicalInfo.m_cPickkingItemNum = lpPickkingItem->GetNumOrDurability();

		chemicalInfo.m_wTargetItemID = lpTargetItem->GetPrototypeID();
		chemicalInfo.m_cTargetItemNum = lpTargetItem->GetNumOrDurability();

		wError = Item::CItemMgr::GetInstance().GetChemicalResult(chemicalInfo);

		// ���⼭ �Ϸ�� ����Ʈ, �����Ƽ�� �˻��Ѵ�.
		if (PktBase::NO_SERVER_ERR == wError && chemicalInfo.m_usQuestID != 0)
		{
			// �Ϸ� ����Ʈ �˻�
			if( !lpCharacter->HasHistoryQuest(chemicalInfo.m_usQuestID) )
				wError = Item::Chemical::F_NOT_QUEST;
		}
		if (PktBase::NO_SERVER_ERR == wError && chemicalInfo.m_usAbilityID != 0)
		{
			// �����Ƽ �˻�
			short sLockCount = lpCharacter->GetSkillLockCount(chemicalInfo.m_usAbilityID);
			if(chemicalInfo.m_cAbilityLevel <= sLockCount)
				wError = PktBase::NO_SERVER_ERR;
			else
				wError = Item::Chemical::F_NOT_ABILITY;
		}

		if (PktBase::NO_SERVER_ERR == wError)
		{
			// ��� �ִ� ������ ó��
			if (0 == chemicalInfo.m_cPickkingItemNum)
			{
				if (true == lpCharacter->RemoveItem(PickkingPos))
				{
					DELETE_ITEM(lpPickkingItem);
				}
				else
				{
					ERRLOG1(g_Log, "CID:0x%08x ��� �ִ� ������ ���ſ� �����Ͽ����ϴ�.", dwCharID);
					wError = PktBase::SERVER_ERROR;
					goto SEND_PACKET;
				}
			}
			else
			{
				lpPickkingItem->SetNumOrDurability(chemicalInfo.m_cPickkingItemNum);

				wPickkingItemID = chemicalInfo.m_wPickkingItemID;
				cPickkingItemNum = chemicalInfo.m_cPickkingItemNum;
			}

			// �ռ� ��� ������ ó��
			if (0 == chemicalInfo.m_cTargetItemNum)
			{
				if (true == lpCharacter->RemoveItem(TargetPos))
				{
					DELETE_ITEM(lpTargetItem);
				}
				else
				{
					ERRLOG1(g_Log, "CID:0x%08x �ռ� ��� ������ ���ſ� �����Ͽ����ϴ�.", dwCharID);
					wError = PktBase::SERVER_ERROR;
					goto SEND_PACKET;
				}
			}
			else
			{
				lpTargetItem->SetNumOrDurability(chemicalInfo.m_cTargetItemNum);

				lpCharacter->RemoveItem(TargetPos);
				lpCharacter->SetItem(PickkingPos, lpTargetItem);

				wPickkingItemID = chemicalInfo.m_wTargetItemID;
				cPickkingItemNum = chemicalInfo.m_cTargetItemNum;
			}

			// �ռ� ����� ó��
			lpResultItem = Item::CItemFactory::GetInstance().CreateItem(chemicalInfo.m_wResultItemID);
			lpResultItem->SetNumOrDurability(chemicalInfo.m_cResultItemNum);		
			lpCharacter->SetItem(TargetPos, lpResultItem);
		}
	}

SEND_PACKET:

	return GameClientSendPacket::SendCharItemChemical(GameClientDispatch.GetSendStream(), dwCharID, 
		PickkingPos, TargetPos, wPickkingItemID, cPickkingItemNum, lpResultItem, wError);
}

// ������ ���׷��̵�
bool GameClientParsePacket::ParseCharUpgradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktUgI), GameClientDispatch);

    PktUgI*     lpPktUgl    = static_cast<PktUgI *>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
    
    using namespace Item;

    unsigned long   dwCID               = lpCharacter->GetCID();
	unsigned long	dwNPCID				= lpPktUgl->m_dwNPCID;
    unsigned short  wError				= Item::CEquipment::S_SUCCESS;
    unsigned long   dwUsedGold          = 0;
    unsigned char   cCurrentMineralNum  = 0;
    unsigned char   cValue				= 0;
        
    Item::CItem* lpDstItem = lpCharacter->GetItem(ItemPos(TakeType::TS_EXTRA, ExtraSpacePos::UPGRADE_EQUIPMENT_POS));
    Item::CItem* lpMineral = lpCharacter->GetItem(ItemPos(TakeType::TS_EXTRA, ExtraSpacePos::UPGRADE_MATERIAL_POS));

    Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpDstItem);

    unsigned char cLastUpgradeLevel = 0;

    if (NULL == lpDstItem)
    {
        wError = Item::CEquipment::E_GET_EQUIP_FAILED;
    }
    else if (NULL == lpMineral)
    {
        wError = Item::CEquipment::E_GET_MINERAL_FAILED;        
    }
    else if (NULL == lpEquipment || !lpEquipment->IsUpgradable())
    {
        wError = Item::CEquipment::E_NOT_EQUIP;
    }
    else
    {
		if(lpMineral->GetPrototypeID() == Item::EtcItemID::AWAKENCORE)
			cLastUpgradeLevel = lpEquipment->GetCoreLevel();
		else
			cLastUpgradeLevel = lpEquipment->GetUpgradeLevel();

        // lpEquipment�� ���⼭�� ���
        wError = lpEquipment->UpgradeItem(*lpMineral, lpCharacter->GetGold(), dwUsedGold, cValue);

		// ���� ���� NPC ���, ���� ���� ����
		CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
		if (NULL != lpNPC && lpNPC->IsBelongToCastle())
		{
			Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByNameID(lpNPC->GetCastleNameID());
			if (NULL != lpCastle)
			{
				const unsigned long dwResmeltTax = static_cast<unsigned long>(dwUsedGold * (lpCastle->GetTax(Castle::RESMELT_TAX) / 100.0f));

				if (dwResmeltTax > 0)
				{
					lpCastle->AddTempTaxMoney(Castle::RESMELT_TAX, dwResmeltTax);
					dwUsedGold += dwResmeltTax;
				}
			}
		}

		if (0 < dwUsedGold)
        {
			if (dwUsedGold > lpCharacter->GetGold())
			{
				wError = Item::CEquipment::E_NOT_ENOUGH_MONEY;
			}
			else
			{
				// ���� ����
				lpCharacter->DeductGold(dwUsedGold, false);
			}
        }

        // ������ �� ���������� ���� ����
        cCurrentMineralNum = lpMineral->GetNumOrDurability();
        if (0 == cCurrentMineralNum)
        {
            if (false == lpCharacter->RemoveItem(lpMineral->GetPos()))
            {
                wError = Item::CEquipment::E_REMOVE_GEM_FAILED;
                ERRLOG1(g_Log, "CID:0x%08x ���׷��̵�� �� ����� ���� ���ſ� �����߽��ϴ�.", dwCID);
            }
            else
            {
                DELETE_ITEM(lpMineral);
            }
        }

        // ���׷��̵� ���нÿ� ��� ���ְ� ��ö ����
        if (wError == Item::CEquipment::S_BROKEN_EQUIP)
        {
            if (false == lpCharacter->RemoveItem(ItemPos(TakeType::TS_EXTRA, 
                ExtraSpacePos::UPGRADE_EQUIPMENT_POS)))
            {
                wError = Item::CEquipment::E_REMOVE_EQUIP_FAILED;
                ERRLOG1(g_Log, "CID:0x%08x ���׷��̵� ���н� ��� ���ſ� �����߽��ϴ�.", dwCID);
            }
            else
            {         
                DELETE_ITEM(lpDstItem);
                lpDstItem = Item::CItemFactory::GetInstance().CreateItem(EtcItemID::SCRAPS);

                if (NULL == lpDstItem)
                {
                    wError = Item::CEquipment::E_CREATE_SCRAP_FAILED;
                    ERRLOG1(g_Log, "CID:0x%08x ���׷��̵� ���н� ��ö ������ �����߽��ϴ�.", dwCID);
                }
                else
                {
                    if (false == lpCharacter->SetItem(ItemPos(TakeType::TS_EXTRA, 
                        ExtraSpacePos::UPGRADE_EQUIPMENT_POS), lpDstItem))
                    {
                        wError = Item::CEquipment::E_SET_SCRAP_FAILED;
                        ERRLOG1(g_Log, "CID:0x%08x ������ ��ö�� ���׷��̵�â�� ���� �� �����߽��ϴ�.", dwCID);

                        DELETE_ITEM(lpDstItem);
                    }
                }
            }
        }
    }

    if (wError == Item::CEquipment::S_BROKEN_EQUIP || wError == Item::CEquipment::S_SUCCESS)
    {
        wError = Item::CEquipment::S_SUCCESS;
    }
    else
    {
        lpDstItem = NULL;
        lpMineral = NULL;
    }

    GAMELOG::LogUpgradeItem(*lpCharacter, lpCharacter->GetGold(), dwUsedGold, 
        cLastUpgradeLevel, lpDstItem, lpMineral, cValue, wError);

    return GameClientSendPacket::SendCharUpgradeItem(GameClientDispatch.GetSendStream(),
        dwCID, lpCharacter->GetGold(), lpDstItem, cCurrentMineralNum, cValue, wError);
}

// ������ �ɼ� �̽�
bool GameClientParsePacket::ParseCharItemOptionGraft(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktItemOptionGraft), GameClientDispatch);

	PktItemOptionGraft*     lpPktIOG	= static_cast<PktItemOptionGraft *>(lpPktBase);
	CCharacter*				lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long   dwCID			= lpPktIOG->m_dwCharID;
	unsigned char	cAttributeType	= lpPktIOG->m_cAttributeType;

	unsigned short	wError			= PktBase::NO_SERVER_ERR;
	unsigned long   dwUsedGold		= 0;

	using namespace Item;

	Item::CEquipment* lpOriginalItem = Item::CEquipment::DowncastToEquipment(
		lpCharacter->GetItem(Item::ItemPos(TakeType::TS_EXTRA, Item::ExtraSpacePos::GRAFT_ORIGINAL_POS)));
	Item::CEquipment* lpSacrificeItem = Item::CEquipment::DowncastToEquipment(
		lpCharacter->GetItem(Item::ItemPos(TakeType::TS_EXTRA, Item::ExtraSpacePos::GRAFT_SACRIFICE_POS)));

    size_t  nOldItemSize = 0;
    size_t  nUseItemSize = 0;

    char    szOldItemData[Item::MAX_ITEM_SIZE];
    char    szUseItemData[Item::MAX_ITEM_SIZE];

    unsigned long dwOldGold = lpCharacter->GetGold();

	if (NULL == lpOriginalItem || NULL == lpSacrificeItem)
	{
		// �ɼ� �̽��� ��� �����ϴ�.
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		// edith ������ �ɼ��̽� ��Ŷ

        // �α� ����� ���� item serialize�� �Ѵ�.
        nOldItemSize = Item::MAX_ITEM_SIZE;
        if(!lpOriginalItem->SerializeOut(szOldItemData, nOldItemSize))
        {
            nOldItemSize = 0;
        }

        nUseItemSize = Item::MAX_ITEM_SIZE;
        if(!lpSacrificeItem->SerializeOut(szUseItemData, nUseItemSize))
        {
            nUseItemSize = 0;
        }

		bool bUpgradeLevelLimit = false;

		wError = lpOriginalItem->OptionGraft(lpSacrificeItem, bUpgradeLevelLimit, cAttributeType, lpCharacter->GetGold(), dwUsedGold);

		if (PktBase::NO_SERVER_ERR == wError || wError == 5)	// 5�� �ɼ��̽� ���з� �ݾװ� ������ ����.
		{
			// ��� �ݾ�
 			lpCharacter->DeductGold(dwUsedGold, false);

			// ���� ������ ����
			if (false == lpCharacter->RemoveItem(lpSacrificeItem->GetPos()))
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x ������ �ɼ� �̽� ���� : ���� ������ ���ſ� �����Ͽ����ϴ�.", dwCID);
			}
			else
			{
				DELETE_ITEM(lpSacrificeItem);
			}

			// ���� �������� ��� ���������� �̵�
			lpCharacter->MoveItem(TakeType(lpOriginalItem->GetPos(), 
				ItemPos(TakeType::TS_EXTRA, ExtraSpacePos::GRAFT_RESULT_POS), 1));
		}
	}

	// ���� �α�
    GAMELOG::LogItemAttachOption(*lpCharacter, dwOldGold, cAttributeType, 
        szOldItemData, nOldItemSize, szUseItemData, nUseItemSize, lpOriginalItem, wError);

	return GameClientSendPacket::SendCharItemOptionGraft(GameClientDispatch.GetSendStream(),
		dwCID, lpCharacter->GetGold(), lpOriginalItem, wError);
}

// ������ ���� �Ǹ�
bool GameClientParsePacket::ParseCharItemCompensation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktItemCompensation), GameClientDispatch);

	PktItemCompensation*	lpPktPIC	= static_cast<PktItemCompensation *>(lpPktBase);
	CCharacter*				lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long   dwCID			= lpPktPIC->m_dwCharID;
	unsigned short	wError			= PktBase::NO_SERVER_ERR;

	Item::CItem*	lpOriginalItem	= lpCharacter->GetItem(Item::ItemPos(TakeType::TS_EXTRA, Item::ExtraSpacePos::COMPENSATION_POS));
	unsigned long	dwOldGold		= lpCharacter->GetGold();

	Item::CEquipment* lpOriginalEquip = Item::CEquipment::DowncastToEquipment(lpOriginalItem);
    if (NULL == lpOriginalEquip)
	{
		ERRLOG1(g_Log, "CID:0x%08x ���� �Ǹ� ���� : ���� �Ǹ��� �������� �����ϴ�.", lpCharacter->GetCID());
		wError = PktBase::SERVER_ERROR;
	}
	else
	{
		Item::CEquipment* lpResultItem = NULL;
		long lResultGold = 0;

		wError = lpOriginalEquip->Compensation(
			static_cast<unsigned char>(lpCharacter->GetClass()), static_cast<unsigned char>(lpCharacter->GetLevel()), 
			lpCharacter->GetCharStatus(), &lpResultItem, lResultGold);

		if (NULL == lpResultItem)
		{
			ERRLOG1(g_Log, "CID:0x%08x ���� �Ǹ� ���� : ���� �Ǹſ� �����Ͽ����ϴ�.", lpCharacter->GetCID());
			wError = PktBase::SERVER_ERROR;
		}

		if (0 > lResultGold && static_cast<unsigned long>(abs(lResultGold)) > lpCharacter->GetGold())
		{
			ERRLOG1(g_Log, "CID:0x%08x ���� �Ǹ� ���� : ���� �����մϴ�.", lpCharacter->GetCID());
			wError = PktBase::SERVER_ERROR;
		}

		if (PktBase::NO_SERVER_ERR == wError)
		{
			if (false == lpCharacter->RemoveItem(lpOriginalEquip->GetPos()))
			{
				ERRLOG1(g_Log, "CID:0x%08x ���� �Ǹ� ���� : ���� �������� �����ϴµ� �����Ͽ����ϴ�.", lpCharacter->GetCID());
				wError = PktBase::SERVER_ERROR;
				DELETE_ITEM(lpResultItem);
			}
			else
			{
				if (false == lpCharacter->GiveItem(lpResultItem))
				{
					ERRLOG1(g_Log, "CID:0x%08x ���� �Ǹ� ���� : ���� �������� �ִµ� �����Ͽ����ϴ�.", lpCharacter->GetCID());
					wError = PktBase::SERVER_ERROR;

					lpCharacter->SetItem(Item::ItemPos(TakeType::TS_EXTRA, 
                        Item::ExtraSpacePos::COMPENSATION_POS), lpOriginalItem);
					
                    DELETE_ITEM(lpResultItem);                    
				}
				else
				{
					if (0 < lResultGold)
					{
						lpCharacter->AddGold(lResultGold, true);
					}
					else
					{
						lpCharacter->DeductGold(abs(lResultGold), true);
					}

					GAMELOG::LogItemCompensation(*lpCharacter, dwOldGold, lpOriginalItem, lpResultItem, 0);
					GameClientSendPacket::SendCharItemCompensation(GameClientDispatch.GetSendStream(), dwCID, 0);

					DELETE_ITEM(lpOriginalItem);

					// GiveItem ���� ���õ� ���
					if (lpResultItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpResultItem->GetNumOrDurability())
					{
						DELETE_ITEM(lpResultItem);
					}

					return true;
				}
			}
		}
	}

    GAMELOG::LogItemCompensation(*lpCharacter, dwOldGold, 0, 0, wError);
	return GameClientSendPacket::SendCharItemCompensation(GameClientDispatch.GetSendStream(), dwCID, wError);
}

// �� ������ ��ġ/����.
bool GameClientParsePacket::ParseCharInstallRuneSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktIRS), GameClientDispatch);

	PktIRS*		lpPktIRS	= static_cast<PktIRS*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long	dwCharID		= lpPktIRS->m_dwCharID;
	Item::ItemPos	EquipPos		= lpPktIRS->m_equipPos;
	Item::ItemPos	RunePos			= lpPktIRS->m_runePos;
	unsigned char	PktType			= lpPktIRS->m_cType;	

	Item::CEquipment* lpEquipment = Item::CEquipment::DowncastToEquipment(lpCharacter->GetItem(EquipPos));
	
	unsigned short wError = PktBase::NO_SERVER_ERR;

	// ����ĭ�� ������ ���������� �˻�.

	bool bEquip = lpCharacter->GetEquipments().GetItem(EquipPos) == NULL ? false : true;

	Item::CItem* lpRune	= lpCharacter->GetItem(RunePos);

	if(PktType==PktIRS::RUNE_INSTALL)
	{
		if(lpEquipment && lpRune)
		{			
			Item::CEquipment::EQUIPMENT_ERROR equipError = lpEquipment->InstallRuneSocket(*lpRune, bEquip, lpCharacter->GetLevel());

			if(equipError!=Item::CEquipment::R_SUCCESS)
			{
				wError = PktBase::SERVER_ERROR;
			}
			else
			{
				if(lpRune)
				{
					if(!lpRune->GetNumOrDurability())
					{
						if(lpCharacter->RemoveItem(RunePos))
						{
							DELETE_ITEM(lpRune);
						}
						else
						{						
							ERRLOG1(g_Log, "CID:0x%08x ��ġ�� ���� �����ϴ� �� �����߽��ϴ�", lpCharacter->GetCID());
						}
					}
				}
			}
		}
		else
		{
			if(!lpEquipment)
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x ���� ��ġ�� ���� �������� �����ϴ�", lpCharacter->GetCID());
			}

			if(!lpRune)
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x ���� �����ۿ� ��ġ�� ���� �����ϴ�", lpCharacter->GetCID());
			}
		}				
	}
	else if(PktType==PktIRS::RUNE_UNINSTALL)
	{
		if(lpEquipment && lpRune)
		{
			// �����ִ� ��ų ����Ʈ �˻�.

			SKILL skill = lpCharacter->GetSkill();

			short sSkillPoint = lpCharacter->GetStatus().m_StatusInfo.m_wSkillPoint - skill.GetSkillNum();			

			if(sSkillPoint<0)
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x ����� ��ų����Ʈ�� �ִ� ��ų����Ʈ���� Ŭ ���", lpCharacter->GetCID());
			}
			else
			{
				Item::CEquipment::EQUIPMENT_ERROR equipError = lpEquipment->UnInstallRuneSocket(*lpRune, bEquip, lpCharacter->GetLevel(), sSkillPoint);

				if(equipError!=Item::CEquipment::R_SUCCESS)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if(lpRune)
					{
						if(!lpRune->GetNumOrDurability())
						{
							if(lpCharacter->RemoveItem(RunePos))
							{
								DELETE_ITEM(lpRune);
							}
							else
							{						
								ERRLOG1(g_Log, "CID:0x%08x �Ҹ� ���� �����ϴ� �� �����߽��ϴ�", lpCharacter->GetCID());
							}
						}
					}
				}				
			}			
		}
		else
		{
			if(!lpEquipment)
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x ���� ������ ���� �������� �����ϴ�", lpCharacter->GetCID());
			}

			if(!lpRune)
			{
				wError = PktBase::SERVER_ERROR;
				ERRLOG1(g_Log, "CID:0x%08x �Ҹ� ���� �����ϴ�", lpCharacter->GetCID());
			}
		}
		
	}

	lpCharacter->CalculateStatusData(false);

	return GameClientSendPacket::SendCharRuneInstallSocket(GameClientDispatch.GetSendStream(), dwCharID, 
		EquipPos, RunePos, lpEquipment, PktType, wError);
}
