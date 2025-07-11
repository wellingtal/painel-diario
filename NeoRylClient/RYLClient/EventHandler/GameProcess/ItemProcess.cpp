#include <winsock2.h>

#include "../GameHandler.h"
#include "../../GUITextEdit.h"

#include "ItemProcess.h"
#include "SoundMgr.h"
#include "FrameTimer.h"

#include "RYLClientMain.h"

#include <Skill/SkillMgr.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include <Item/ItemMgr.h>
#include <Item/Item.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Parse/ParseItem.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>

#include "RYLProfile.h"
#include "RYLStringTable.h"
#include "RYLListBox.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLTimer.h"

#include "RYLRaceBase.h"
#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLCommunityData.h"
#include "RYLChattingDlg.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLBlacksmithDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLChattingDlg.h"
#include "RYLVaultDlg.h"
#include "RYLVaultPasswordDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLHumanInventoryFrameDlg.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "BroadcastProcess.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "RYLTradeDlg.h"

#include <string>
#include <sstream>
#include "GMMemory.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "RYLGameTimeManager.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildWarPenaltyDlg.h"

void GameProcess::LogInvenData(unsigned __int64 nObjectID)
{
    CRYLGameData* lpGame = CRYLGameData::Instance();

    char szMessage[PktChat::PktChatMaxSize] = { 0, };
    int nTotalLength = 0;
    int nLength = 0;

	for (int nZ = 0; nZ < CCharStatus::INVENTORY_TAB; ++nZ)
    {
        nLength = _snprintf(szMessage, 
            PktChat::PktChatMaxSize - 1, CRYLStringTable::m_strString[ 2774 ], nZ + 1);

        if (0 < nLength)
        {
            szMessage[nLength] = 0;
            SendPacket::CharChat(g_GameSession, PktChat::CLIENT_LOG, szMessage, nLength + 1, 0);
        }

        for (int nY = 0; nY < CCharStatus::INVENTORY_HEIGHT; ++nY)
        {
            for (int nX = 0; nX < CCharStatus::INVENTORY_WIDTH; ++nX)
            {
                nLength = _snprintf(szMessage + nTotalLength, 
                    PktChat::PktChatMaxSize - nTotalLength - 1, 
                    "0x%016I64X ", lpGame->m_csStatus.m_aryInven[nZ][nY][nX]);

                if (0 < nLength)
                {
                    nTotalLength += nLength;
                }
            }

            szMessage[nTotalLength] = 0;
            SendPacket::CharChat(g_GameSession, PktChat::CLIENT_LOG, szMessage, nTotalLength + 1, 0);

            nTotalLength = 0;
        }
    }
        
    list<CItemInstance *>::iterator pos = lpGame->m_csStatus.m_lstInventory.begin();
    list<CItemInstance *>::iterator end = lpGame->m_csStatus.m_lstInventory.end();

    for (; pos != end; ++pos)
    {
        CItemInstance* lpItemInstance = *pos;
        Item::CItem*   lpItem = lpItemInstance->m_lpItemBase;

        // UID, ��ǥ, ����, 
        nLength = _snprintf(szMessage, PktChat::PktChatMaxSize - 1, 
            "ItemUID:0x%016I64X / PrototypeID:%6d / ItemPos(%d:0x%03X) / NumOrDurability:%3d",
            lpItem->GetUID(), lpItem->GetPrototypeID(), 
            lpItem->GetPos().m_cPos, lpItem->GetPos().m_cIndex,
            lpItem->GetNumOrDurability());

        if (0 < nLength)
        {
            szMessage[nLength] = 0;
            SendPacket::CharChat(g_GameSession, PktChat::CLIENT_LOG, szMessage, nLength, 0);
        }
    }

    if (0LL != nObjectID)
    {
        std::list<LPFieldInstance>::iterator field_pos = lpGame->m_lstFieldItem.begin();
        std::list<LPFieldInstance>::iterator field_end = lpGame->m_lstFieldItem.end();

        for (; field_pos != field_end; ++field_pos)
        {
            LPFieldInstance lpFieldInstance =  *field_pos;

            if (lpFieldInstance->m_nOID == nObjectID)
            {
                nLength = _snprintf(szMessage, PktChat::PktChatMaxSize - 1, 
                    "FieldObject : OID:0x%016I64X / InstanceID:%10d / TypeID:%10d(0x%08X) / ItemPos(%5d/%5d/%5d) / Num:%3d",
                    lpFieldInstance->m_nOID, lpFieldInstance->m_dwInstanceID, 
                    lpFieldInstance->m_dwTypeID, lpFieldInstance->m_dwTypeID,
                    lpFieldInstance->m_usXPos, lpFieldInstance->m_usYPos, lpFieldInstance->m_usZPos,
                    lpFieldInstance->m_cNum);

                if (0 < nLength)
                {
                    szMessage[nLength] = 0;
                    SendPacket::CharChat(g_GameSession, PktChat::CLIENT_LOG, szMessage, nLength + 1, 0);
                }
            }
        }
    }
}


GAME_EVENT_FUNC(ParseCharPickUp);
GAME_EVENT_FUNC(ParseCharPickUpInfo);
GAME_EVENT_FUNC(ParseCharAutoRouting);
GAME_EVENT_FUNC(ParseCharDisappearItem);
GAME_EVENT_FUNC(ParseCharPullDown);
GAME_EVENT_FUNC(ParseCharPullDownInfo);
GAME_EVENT_FUNC(ParseCharTakeItem);
GAME_EVENT_FUNC(ParseCharSwapItem);
GAME_EVENT_FUNC(ParseCharSplitItem);
GAME_EVENT_FUNC(ParseCharTradeItem);
GAME_EVENT_FUNC(ParseCharRepairItem);
GAME_EVENT_FUNC(ParseCharRepairAllItem) ;
GAME_EVENT_FUNC(ParseCharCastObject);
GAME_EVENT_FUNC(ParseCharCastObjectInfo);
GAME_EVENT_FUNC(ParseCharUseItem);
GAME_EVENT_FUNC(ParseCharInstallSocket);
GAME_EVENT_FUNC(ParseCharInstallRune);
GAME_EVENT_FUNC(ParseCharUpgradeItem);
GAME_EVENT_FUNC(ParseDeposit);
GAME_EVENT_FUNC(ParseDepositUpdate);
GAME_EVENT_FUNC(ParseCharExchangeCmd);
GAME_EVENT_FUNC(ParseCharExchangeItem);
GAME_EVENT_FUNC(ParseCharStallOpen);
GAME_EVENT_FUNC(ParseCharStallRegisterItem);
GAME_EVENT_FUNC(ParseCharStallEnter);
GAME_EVENT_FUNC(ParseCharStallItemInfo);
GAME_EVENT_FUNC(ParseCharTakeItems);
GAME_EVENT_FUNC(ParseCharTakeGold);
GAME_EVENT_FUNC(ParseCharItemChemical);
GAME_EVENT_FUNC(ParseCharEquipShopInfo);
GAME_EVENT_FUNC(ParseCharItemOptionGraft);
GAME_EVENT_FUNC(ParseCharItemCompensation);
GAME_EVENT_FUNC(ParseCharUseCashItem);


bool GameProcess::RegisterItemHandler(CGameEventHandler& GameEventHandler)
{
	unsigned long dwErrorCount = 0;

//	dwErrorCount += GameEventHandler.AddHandler(CmdCharLevelUp, ParseCharLevelUp, 
//		!(BIT(CLIENT_GAME) | BIT(CLIENT_GAMELOADING))) ? 0 : 1;

	dwErrorCount += GameEventHandler.AddHandler(CmdCharPickUp, ParseCharPickUp) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharPickUpInfo, ParseCharPickUpInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharAutoRouting, ParseCharAutoRouting) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharDisappearItem, ParseCharDisappearItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharPullDown, ParseCharPullDown) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharPullDownInfo, ParseCharPullDownInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharTakeItem, ParseCharTakeItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharSwapItem, ParseCharSwapItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharSplitItem, ParseCharSplitItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharTradeItem, ParseCharTradeItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRepairItem, ParseCharRepairItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRepairAllItem, ParseCharRepairAllItem ) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharCastObject, ParseCharCastObject) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharCastObjectInfo, ParseCharCastObjectInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharUseItem, ParseCharUseItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharInstallSocket, ParseCharInstallSocket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharUpgradeItem, ParseCharUpgradeItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdDeposit, ParseDeposit) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdDepositUpdate, ParseDepositUpdate) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharExchangeCmd, ParseCharExchangeCmd) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharExchangeItem, ParseCharExchangeItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharStallOpen, ParseCharStallOpen) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharStallRegisterItem, ParseCharStallRegisterItem) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharStallEnter, ParseCharStallEnter) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharStallItemInfo, ParseCharStallItemInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharTakeItems, ParseCharTakeItems) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharTakeGold, ParseCharTakeGold) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharItemChemical, ParseCharItemChemical) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharEquipShopInfo, ParseCharEquipShopInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharItemOptionGraft, ParseCharItemOptionGraft) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharInstallRuneSocket, ParseCharInstallRune) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharItemCompensation, ParseCharItemCompensation) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharUseCashItem, ParseCharUseCashItem) ? 0 : 1;
	

	return (0 == dwErrorCount);
}

GAME_EVENT_FUNC(ParseCharPickUp)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwGold, dwSize;
	unsigned __int64 nObjectID;
	unsigned char	cNum;
	char*			lpItemBuffer;
	Item::ItemPos	pItemIndex;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPickUp(lpPktBase, &dwChrID, 
		&nObjectID, &pItemIndex, &dwGold, &dwSize, &cNum, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		char strSoundFilename[256];

		if (dwSize)		// �������϶�
		{
			char strString[200];
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			lpItem->SetItemInfo(lpItemBuffer, (size_t &)dwSize);

			if ( TakeType::TS_TEMPINVEN == lpItem->GetItemPos() )
			{
				pGame->m_csStatus.AddTempInventory( lpItem );
				return TRUE;
			}


			pGame->m_csStatus.AddInventoryItem(lpItem);
			

			if (lpItem->GetIsEnableStack())
			{
				sprintf(strString, CRYLStringTable::m_strString[272], lpItem->GetItemInfo().m_SpriteData.m_szName, lpItem->GetNowDurability());
				pChat->AddMessage(strString, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			} 
			else
			{
				char strItemName[MAX_PATH];
				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem->m_lpItemBase);
				if (NULL == lpEquip || 
					Item::ItemType::SHIRT == lpItem->GetItemType() || 
					Item::ItemType::TUNIC == lpItem->GetItemType() || 
					Item::ItemType::ARROW == lpItem->GetItemType() || 
					Item::ItemType::BOLT == lpItem->GetItemType() || 
					Item::ItemType::AMMO == lpItem->GetItemType())
				{
					sprintf(strItemName, "%s", lpItem->GetItemInfo().m_SpriteData.m_szName);
				}
				// �׷��̵尡 ���Ǵ� ����� ��� �׷��̵���� ����� �ݴϴ�.
				else
				{
					char strGrade[MAX_PATH];

					Item::Grade::GradeInfo gradeInfo = lpEquip->GetItemGrade();
					switch ( gradeInfo.m_eItemGrade )
					{
						/*
						case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
						case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " AAA" );		break;
						case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AA" );		break;
						case Item::EquipType::A_GRADE:		strcpy( strGrade, " A" );		break;
						case Item::EquipType::B_GRADE:		strcpy( strGrade, " B" );		break;
						case Item::EquipType::C_GRADE:		strcpy( strGrade, " C" );		break;
						case Item::EquipType::D_GRADE:		strcpy( strGrade, " D" );		break;
						case Item::EquipType::F_GRADE:		strcpy( strGrade, " F" );		break;
						case Item::EquipType::X_GRADE:		strcpy( strGrade, "" );			break;	//--//
					*/
						case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
						case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " S" );		break;
						case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AAA" );		break;
						case Item::EquipType::A_GRADE:		strcpy( strGrade, " AA" );		break;
						case Item::EquipType::B_GRADE:		strcpy( strGrade, " A" );		break;
						case Item::EquipType::C_GRADE:		strcpy( strGrade, " B" );		break;
						case Item::EquipType::D_GRADE:		strcpy( strGrade, " C" );		break;
						case Item::EquipType::F_GRADE:		strcpy( strGrade, " D" );		break;
						case Item::EquipType::X_GRADE:		strcpy( strGrade, "" );			break;	//--//
					}

					// edith 2008.01.25 ä��â�� �׷��̵� ��±��� ����
					if(gradeInfo.m_cPlus == 0)
					{
						if(lpEquip->GetSeasonRecord() == 3)
					      //sprintf(strItemName, "%s %s%s", CRYLStringTable::m_strString[246], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
							sprintf(strItemName, "%s", CRYLStringTable::m_strString[246], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
						else if(lpEquip->GetSeasonRecord() == 2)
						  //sprintf(strItemName, "%s %s%s", CRYLStringTable::m_strString[238], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
							sprintf(strItemName, "%s", CRYLStringTable::m_strString[238], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
						else
						//	sprintf(strItemName, "%s%s", lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
							sprintf(strItemName, "%s", lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade);
					}
					else
					{
						if(lpEquip->GetSeasonRecord() == 3)
//							sprintf(strItemName, "%s %s%s(+%d)", CRYLStringTable::m_strString[246], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
                            sprintf(strItemName, "%s", CRYLStringTable::m_strString[246], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
						else if(lpEquip->GetSeasonRecord() == 2)
//							sprintf(strItemName, "%s %s%s(+%d)", CRYLStringTable::m_strString[238], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
                            sprintf(strItemName, "%s", CRYLStringTable::m_strString[238], lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
						else 
//							sprintf(strItemName, "%s%s(+%d)", lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
                            sprintf(strItemName, "%s", lpItem->GetItemInfo().m_SpriteData.m_szName, strGrade, gradeInfo.m_cPlus);
					}
//					
					strcat(strItemName, strGrade);
					for (int i = 0; i < gradeInfo.m_cPlus; ++i)
					{
						strcat(strItemName, "+");
					}
//
				}

				sprintf(strString, CRYLStringTable::m_strString[273], strItemName);
				pChat->AddMessage(strString, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			}

			unsigned short wProtoTypeID = lpItem->GetProtoTypeID();		// �����۸�
			if (wProtoTypeID == 9915 || wProtoTypeID == 9916 || wProtoTypeID == 9917)
			{
				sprintf(strSoundFilename,"%s\\Interface\\pang.mnd", SOUNDFILEPATH);
			} 
			else
			{
				sprintf(strSoundFilename,"%s\\Interface\\money.mnd",SOUNDFILEPATH);
			}
			CSound* pSound; 
			CSoundMgr::_GetInstance()->GetSound( pSound, strSoundFilename ); 
			pSound->Play(); 
		} 
		else
		{
			if (dwGold && dwGold > 0)
			{
				pGame->m_csStatus.m_Info.Gold += dwGold;
				
				char strString[200];
				sprintf(strString, CRYLStringTable::m_strString[274], dwGold);
				pChat->AddMessage(strString, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

				sprintf(strSoundFilename,"%s\\Interface\\money.mnd",SOUNDFILEPATH);

				if (pGame->m_csStatus.m_Info.Gold >= 4000000000)
				{
					sprintf(strString, CRYLStringTable::m_strString[ 216 ], CRYLStringTable::MakePrintGold(ULONG_MAX));
					pChat->AddMessage( strString, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
				}

				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, strSoundFilename ); 
				pSound->Play(); 
			} 
			else
			{
				char strString[200];
				CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
				if (lpExistItem)
				{
					sprintf(strString, CRYLStringTable::m_strString[272], lpExistItem->GetItemInfo().m_SpriteData.m_szName, cNum);
					pChat->AddMessage(strString, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

					// ������ �Ͼ��.
					unsigned dwNum = lpExistItem->GetNowDurability() + cNum;
					lpExistItem->SetNowDurability(dwNum);
					pGame->m_csStatus.SetQuickSlot(lpExistItem);

					sprintf(strSoundFilename,"%s\\Interface\\money.mnd",SOUNDFILEPATH);
					CSound* pSound; 
					CSoundMgr::_GetInstance()->GetSound( pSound, strSoundFilename ); 
					pSound->Play(); 
				}
			}
		}

		return TRUE;
	} 
	else
	{
		if (CRYLNetworkData::Instance()->m_dwNetworkError == 2 || 
			CRYLNetworkData::Instance()->m_dwNetworkError == 3 || 
			CRYLNetworkData::Instance()->m_dwNetworkError == 6)
		{
			pGame->DeleteFieldObject(nObjectID);
		}

		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = �� ��� ���� 
			case 3:					// 3 = ���κ��� ������ ��� ���� 
			case 6:					// 6 = �������� ���� 
				pChat->AddMessage(CRYLStringTable::m_strString[316], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case 4:					// 4 = �κ��� �ֱ� ���� 
				pChat->AddMessage(CRYLStringTable::m_strString[318], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
                GameProcess::LogInvenData(nObjectID);
				break;

			case 5:					// 5 = �������� ���� 
				pChat->AddMessage(CRYLStringTable::m_strString[317], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case 7:					// 7 = ������ ���� ���� 
				pChat->AddMessage(CRYLStringTable::m_strString[2273], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case 8:					// 8 = ������� �� �Դϴ�.
				break;

			case 9:					// 9 = �ִ� ���� �ݾ��� �ʰ��� ���
				pChat->AddMessage(CRYLStringTable::m_strString[217], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
                break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharPickUpInfo)
{
	unsigned long		dwChrID;
	unsigned __int64	nObjectID;
	CRYLGameData*		pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPickUpInfo(lpPktBase, &dwChrID, &nObjectID);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->DeleteFieldObject(nObjectID);

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharAutoRouting)
{
	unsigned long		dwChrID = 0;
	unsigned __int64	nObjectID = 0;
	unsigned short		wItemID = 0;
	unsigned char		cNum = 0;
	unsigned char		cCmd = 0;

	CRYLGameData*		pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg*	pChat = CRYLChattingDlg::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
		ParsePacket::HandleCharAutoRouting(lpPktBase, &dwChrID, &nObjectID, &wItemID, &cNum, &cCmd);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch (cCmd)
		{
			case ClientConstants::ARCmd_Order:
			{
				Item::ItemPos pItemIndex;
				
				if (pGame->m_csStatus.GetAbleInven(wItemID, cNum, pItemIndex))
				{
					g_pClientSocket->SetStatusFlag(NS_TAKEITEM);
				} 
				else
				{
					pItemIndex.m_cIndex = 0;
					pItemIndex.m_cPos = 0;
				}

				SendPacket::CharAutoRouting(g_GameSession, 
                    CRYLNetworkData::Instance()->m_dwMyChrID, nObjectID, wItemID, pItemIndex);
				break;
			}

			case ClientConstants::ARCmd_Possible:	// ���� �ֿ���.
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwChrID );

                if ( pTempData )
                {
                    const Item::ItemInfo *lpProtoType = g_ItemMgr.GetItemInfo( wItemID );

					if ( lpProtoType )
					{
						char strMessage[ MAX_PATH ];
						sprintf( strMessage, CRYLStringTable::m_strString[ 427 ], pTempData->m_strName.c_str(), lpProtoType->m_SpriteData.m_szName );
						pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
					}
                }
				break;
			}

			case ClientConstants::ARCmd_Impossible:	// ���� �ݴ°� �����ߴ�.
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwChrID );

                if ( pTempData )
                {
                    char strMessage[MAX_PATH];
					sprintf( strMessage, CRYLStringTable::m_strString[ 428 ], pTempData->m_strName.c_str() );
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
                }
				break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharDisappearItem)
{
	unsigned long	dwChrID = 0;
	Item::ItemPos	pItemIndex;
	unsigned char	cNum = 0;
	unsigned char	cCmd = 0;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharDisappearItem(lpPktBase, &dwChrID, &pItemIndex, &cNum, &cCmd);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;	

		if (NULL != pGame->m_lpPickItem)
		{
			switch (pGame->m_lpPickItem->GetItemPos())
			{
				case TakeType::TS_INVEN:
					pGame->m_csStatus.AddInventoryItem(pGame->m_lpPickItem);
					break;
			}
		}

		CItemInstance* lpTemp = pGame->m_csStatus.GetInventoryItem(pItemIndex);
		if (NULL != lpTemp)
		{
			if (cCmd == PktDisappearItem::DIC_LINKED_QUEST)
			{
				char strMessage[MAX_PATH] = { 0, };
				sprintf(strMessage, CRYLStringTable::m_strString[ 67 ], 
					lpTemp->GetItemInfo().m_SpriteData.m_szName);
				CRYLChattingDlg::Instance()->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			}

			if (cNum == 0)
			{
				pGame->m_csStatus.DeleteInventoryItem(lpTemp);
				delete lpTemp;
				lpTemp = NULL;
			} 
			else
			{
				lpTemp->SetNowDurability(cNum);
			}
		}

		return TRUE;
	}

	return FALSE;
}


GAME_EVENT_FUNC(ParseCharPullDown)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	Item::ItemPos	pItemIndex;
	FieldObject		FieldObject;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPullDown(lpPktBase, &dwChrID, &pItemIndex, &FieldObject);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			if ( TakeType::TS_TEMPINVEN == pItemIndex.m_cPos )
			{
				g_pClientSocket->SetStatusFlag( NS_NOWAITING );
				CItemInstance* pItem = pGame->m_csStatus.RemoveTopTempInventory();
				if ( pItem )
					delete pItem;
			}
			else if ( pGame->m_lpPickItem )
			{
				delete pGame->m_lpPickItem;
				pGame->SetPickItem(NULL);
			}

			if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
			{
				pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
			}

			pGame->m_bPullDownFlag = FALSE;
			return TRUE;
		}
	} 
	else
	{
		if (CRYLNetworkData::Instance()->m_dwNetworkError == 5)
		{
			delete pGame->m_lpPickItem;
			pGame->SetPickItem(NULL);

			if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
			{
				pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
			}
		}

		pGame->m_bPullDownFlag = FALSE;

		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					//2 = �� ��� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[319]);
				break;

			case 3:					//3 = �κ����� ������ ��� ���� 
			case 4:					//4 = ������ ������ ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[320]);
				break;

			case 5:					//5 = �������� �������Դϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[321]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharPullDownInfo)
{
	unsigned long	dwChrID;
	FieldObject		FieldObject;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPullDownInfo(lpPktBase, &dwChrID, &FieldObject);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->AddFieldObject(&FieldObject);
		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharTakeItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	TakeType		pTakeType;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharTakeItem(lpPktBase, &dwChrID, &pTakeType);

	CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
	if (NULL != lpPacketProfileMgr)
	{
		lpPacketProfileMgr->Recv("PktTI");
	}

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID != CRYLNetworkData::Instance()->m_dwMyChrID) 
		{
			return FALSE;
		}

		if (NULL == pGame->m_lpPickItem)
		{
			pGame->m_lpPickItem = pGame->m_csStatus.GetItem(pTakeType.m_srcPos);
			pGame->m_csStatus.RemoveItem(pTakeType.m_srcPos);
		}

		if (pGame->m_lpPickItem->GetIsEnableStack())	// ���� ������ ����
		{
			CItemInstance* lpInstance = NULL;
			switch (pTakeType.m_dstPos.m_cPos)
			{
				case TakeType::TS_INVEN:
				{
					if ( TakeType::TS_TEMPINVEN == pTakeType.m_srcPos.m_cPos )
					{
						g_pClientSocket->SetStatusFlag( NS_NOWAITING );
						pGame->m_csStatus.RemoveTopTempInventory();
					}

					lpInstance = pGame->m_csStatus.GetInventoryItem(pTakeType.m_dstPos.m_cIndex);
					break;
				}

				case TakeType::TS_EQUIP:
				{
					lpInstance = pGame->m_csStatus.m_lstEquipment[pTakeType.m_dstPos.m_cIndex];
					break;
				}

				case TakeType::TS_EXTRA:
				{
					switch (pTakeType.m_dstPos.m_cIndex)
					{
						case Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS:
						case Item::ExtraSpacePos::UPGRADE_MATERIAL_POS:
						{
							lpInstance = pGame->m_csStatus.m_lpUpgrade[pTakeType.m_dstPos.m_cIndex];
							break;
						}

						case Item::ExtraSpacePos::KIT_MATERIAL_POS:
						{
							lpInstance = pGame->m_csStatus.m_lpSiegeArmsMaterial;
							break;
						}
					}
					break;
				}

				case TakeType::TS_EXCHANGE:
				{
					lpInstance = pGame->m_csStatus.GetExchangeItem(pTakeType.m_dstPos.m_cIndex);
					break;
				}

				case TakeType::TS_DEPOSIT:
				{
					pGame->m_csStatus.PayDepositPrice(pGame->m_lpPickItem->m_ipPreIndex.m_cPos, pTakeType.m_dstPos.m_cPos);

					lpInstance = pGame->m_csStatus.GetDepositItem(pTakeType.m_dstPos.m_cIndex);
					break;
				}
			}

			if (NULL != lpInstance)
			{
				// �������� �������� �ִ�.
				unsigned long dwNum = lpInstance->GetNowDurability() + pTakeType.m_cNum;
				lpInstance->SetNowDurability(dwNum);
				pGame->m_csStatus.SetQuickSlot(lpInstance);

				if (pGame->m_lpPickItem->GetNowDurability() == pTakeType.m_cNum)
				{
					delete pGame->m_lpPickItem;
					pGame->SetPickItem(NULL);
				} 
				else
				{
					dwNum = pGame->m_lpPickItem->GetNowDurability() - pTakeType.m_cNum;
					pGame->m_lpPickItem->SetNowDurability(dwNum);
				}
			} 
			else
			{
				// �������� �������� ����.
				pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
				pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);

				// �������� �ٽ� ��� �ִ� �������� ��ġ�϶��� NULL ������ �ϸ� �ȵȴ�.
				if ( !(pTakeType.m_dstPos.m_cPos == TakeType::TS_TEMP && pTakeType.m_dstPos.m_cIndex == Item::ExtraSpacePos::HOLDITEM_POS) )
				{
                    pGame->SetPickItem(NULL);
				}
			}
		} 
		else
		{	
			// �׳� ����� ���
			switch (pTakeType.m_dstPos.m_cPos)
			{
				case TakeType::TS_EQUIP:
				{
					pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
					pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);

					CRYLNetworkData::SendCharShape();
					pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

					pGame->SetPickItem(NULL);
					break;
				}

				case TakeType::TS_INVEN:
				{
					if ( TakeType::TS_TEMPINVEN == pTakeType.m_srcPos.m_cPos )
					{
						g_pClientSocket->SetStatusFlag( NS_NOWAITING );
						pGame->m_csStatus.RemoveTopTempInventory();
					}

					pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
					pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);
					pGame->SetPickItem(NULL);
					break;
				}

				case TakeType::TS_DEPOSIT:
				{
					pGame->m_csStatus.PayDepositPrice(pGame->m_lpPickItem->m_ipPreIndex.m_cPos, pTakeType.m_dstPos.m_cPos);

					pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
					pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);
					pGame->SetPickItem(NULL);
					break;
				}

				case TakeType::TS_TEMP:
				{
					if (Item::ExtraSpacePos::HOLDITEM_POS == pTakeType.m_dstPos.m_cIndex)
					{
						pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
						pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);
					}
					else
					{
						pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
						pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);
						pGame->SetPickItem(NULL);
					}
					break;
				}

				default:
				{
					pGame->m_lpPickItem->SetPos(pTakeType.m_dstPos);
					pGame->m_csStatus.SetItem(pTakeType.m_dstPos, pGame->m_lpPickItem);
					pGame->SetPickItem(NULL);
					break;
				}
			}
		}

		if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
		{
			pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktTI::FAIL_MOVE:					// 2 = ������ �̵� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[309]);
				break;
		case PktTI::FAIL_MOVE_FIFTHSKILL_LOCKITEM:	// 3 = �������� 5�ܰ� ��ų ������ �̵� ����
			if(CRYLNetworkData::Instance()->UseContents(GameRYL::FIFTHSKILL_LOCKITEM))
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4234]);

			}
			break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharSwapItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	TakeType		pSrcType, pDstType;

	CRYLGameData*		pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance() ;

	pNetwork->m_dwNetworkError = ParsePacket::HandleCharSwapItem(lpPktBase, &dwChrID, &pSrcType, &pDstType);

	if (0 == pNetwork->m_dwNetworkError)
	{
		if (dwChrID == pNetwork->m_dwMyChrID)
		{
			// ������ ��ġ�� �������� ������ �´�.
			CItemInstance* lpTemp = pGame->m_csStatus.GetItem(pDstType.m_srcPos);
			switch (pDstType.m_srcPos.m_cPos)
			{
				case TakeType::TS_INVEN:
				{
					CItemInstance *lpStallItem = pGame->m_csStatus.GetStreetTradeItem(lpTemp->m_dwItemUID);
					if (lpStallItem)
					{
						TakeType pTakeType;
						pTakeType.m_srcPos.m_cPos = TakeType::TS_STALL;
						pTakeType.m_srcPos.m_cIndex = 0;
						pTakeType.m_dstPos = lpStallItem->m_lpItemBase->GetPos();

						if (lpStallItem->GetIsEnableStack())
						{
							pTakeType.m_cNum = lpStallItem->GetNowDurability();
						} 
						else
						{
							pTakeType.m_cNum = 1;
						}

						pGame->m_bStallRegFlag = TRUE;
//						g_pClientSocket->SetStatusFlag(NS_STALLREGISTERITEM);
						SendPacket::CharStallRegisterItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
							pTakeType, 0, PktStRI::SC_CANCEL);
					}

					pGame->m_csStatus.DeleteInventoryItem(lpTemp);
					break;
				}

				case TakeType::TS_EXCHANGE:
					pGame->m_csStatus.DeleteExchangeItem(lpTemp);
					break;

				case TakeType::TS_DEPOSIT:
					pGame->m_csStatus.DeleteDepositItem(lpTemp);
					break;
			}

			lpTemp->SetPos(pDstType.m_dstPos);

			// ���� ��� �ִ� ��� 
			if (NULL == pGame->m_lpPickItem)
			{
				CItemInstance* lpTemp = pGame->m_csStatus.GetItem(pSrcType.m_srcPos);
				switch (pSrcType.m_srcPos.m_cPos)
				{
					case TakeType::TS_INVEN:
					{
						CItemInstance *lpStallItem = pGame->m_csStatus.GetStreetTradeItem(lpTemp->m_dwItemUID);
						if (lpStallItem)
						{
							TakeType pTakeType;
							pTakeType.m_srcPos.m_cPos = TakeType::TS_STALL;
							pTakeType.m_srcPos.m_cIndex = 0;
							pTakeType.m_dstPos = lpStallItem->m_lpItemBase->GetPos();

							if (lpStallItem->GetIsEnableStack())
							{
								pTakeType.m_cNum = lpStallItem->GetNowDurability();
							} 
							else
							{
								pTakeType.m_cNum = 1;
							}

							pGame->m_bStallRegFlag = TRUE;
//							g_pClientSocket->SetStatusFlag(NS_STALLREGISTERITEM);
							SendPacket::CharStallRegisterItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
								pTakeType, 0, PktStRI::SC_CANCEL);
						}
						break;
					}
				}
				pGame->m_csStatus.RemoveItem(pSrcType.m_srcPos);

				if (NULL != lpTemp)
				{
					pGame->SetPickItem(lpTemp);
				} 
				else
				{
					return FALSE;
				}
			}

			// Ÿ�� ��ġ�� ��� �ִ� �������� �д�.
			pGame->m_csStatus.PayDepositPrice(pGame->m_lpPickItem->m_ipPreIndex.m_cPos, pSrcType.m_dstPos.m_cPos);
			pGame->m_lpPickItem->SetPos(pSrcType.m_dstPos);

			pGame->m_csStatus.SetItem(pSrcType.m_dstPos, pGame->m_lpPickItem);
			if (TakeType::TS_EQUIP == pSrcType.m_dstPos.m_cPos)
			{
				CRYLNetworkData::SendCharShape();
				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			}

			pGame->SetPickItem(lpTemp);

			if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
			{
				pGame->m_lpInterface->ProcessExchangeCmd(pNetwork->m_dwMyChrID, 100);
			}

			return TRUE;
		}
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (pNetwork->m_dwNetworkError)
		{
			case 2:					// 2 = �ҽ� �̵� ���� 
			case 3:					// 3 = ��� �̵� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[310]);
				break;

			case 4:					// 4 = ���� �Ұ��� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[311]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharSplitItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	TakeType		pTakeType;
	unsigned char	cSize;
	size_t			dwSize;
	char*			lpItemBuffer;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSplitItem(lpPktBase, &dwChrID, &pTakeType, &cSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID != CRYLNetworkData::Instance()->m_dwMyChrID) return FALSE;

		dwSize = cSize;

		CItemInstance *lpExistItem = pGame->m_csStatus.GetItem(pTakeType.m_srcPos);
		if (NULL == lpExistItem) 
		{
			return FALSE;
		}

		CItemInstance *lpItem;
		lpItem = new CItemInstance;
		lpItem->SetItemInfo(lpItemBuffer, dwSize);
		lpItem->SetPos(pTakeType.m_dstPos);

		if (TakeType::TS_DEPOSIT == pTakeType.m_dstPos.m_cPos)
		{
			pGame->m_csStatus.PayDepositPrice(pTakeType.m_srcPos.m_cPos, pTakeType.m_dstPos.m_cPos);
		}

		pGame->m_csStatus.SetItem(pTakeType.m_dstPos, lpItem);

		delete pGame->m_lpPickItem;
		pGame->SetPickItem(NULL);

		if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
		{
			pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = ������ ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[350]);
				break;
		}
	}

	return FALSE;
}
//hz added this for shop hack test
void CheckItemWithStat(unsigned short ProtID, CItemInstance* lpItem )
{
	switch(ProtID)
	{
	case 9104:
		if (lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[12] != 240)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			MessageBox(GetActiveWindow(), "Shop hack detected!", "Ryl Tides of War ERR::HADE.001", MB_OK);
			exit(1);
		}
		break;

	case 9105:
		if (lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[12] != 200)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			MessageBox(GetActiveWindow(), "Shop hack detected!", "Ryl Tides of War ERR::HADE.002", MB_OK);
			exit(1);
		}
		break;

	case 9106:
		if (lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[12] != 200)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			MessageBox(GetActiveWindow(), "Shop hack detected!", "Ryl Tides of War ERR::HADE.003", MB_OK);
			exit(1);
		}
		break;

	case 9107:
		if (lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[12] != 240)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			MessageBox(GetActiveWindow(), "Shop hack detected!", "Ryl Tides of War ERR::HADE.004", MB_OK);
			exit(1);
		}
		break;

	case 9108:
		if (lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[12] != 300)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			MessageBox(GetActiveWindow(), "Shop hack detected!", "Ryl Tides of War ERR::HADE.005", MB_OK);
			exit(1);
		}
		break;
	}
}
//hz end
GAME_EVENT_FUNC(ParseCharTradeItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwNPCID, dwGold = 0, dwMileage = 0;
	size_t			dwSize;
	unsigned short	wSize;
	unsigned char	cNum;
	Item::ItemPos	pCouponIndex;
	Item::ItemPos	pItemIndex;
	char*			lpItemBuffer;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
		ParsePacket::HandleCharTradeItem(lpPktBase, &dwChrID, &dwNPCID, &dwGold, &dwMileage, &pCouponIndex, &pItemIndex, &cNum, &wSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		dwSize = wSize;
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			pGame->m_csStatus.m_Info.Gold = dwGold;
			pGame->m_csStatus.m_Info.Mileage = dwMileage;

			if (0 == wSize)
			{
				// �� ��
				if (pGame->m_lpPickItem && pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
				{
					pGame->m_csStatus.SetQuickSlot(pGame->m_lpPickItem);

					delete pGame->m_lpPickItem;
					pGame->SetPickItem(NULL);
				} 
				else
				{
					CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex);
					if (lpItem)
					{
						if (lpItem->GetIsEnableStack())
						{
							unsigned long dwNum = lpItem->GetNowDurability() - cNum;
							lpItem->SetNowDurability(dwNum);
						} 
						else
						{
							lpItem->SetNowDurability(0);
						}

						pGame->m_csStatus.SetQuickSlot(lpItem);

						// edith 2008.02.19 Ctrl + ������ �Ǹ� ���� �������� ���� �߰�.
						if(pGame->m_lpTradeItem && pGame->m_lpTradeItem->GetProtoTypeID() == lpItem->GetProtoTypeID())
						{
							// Ʈ���̵� �� �������� ������ ������ �ʱ�ȭ
							pGame->m_iRYLAutoTradeNum = 0;
							pGame->m_lpTradeItem = NULL;
						}

						if (lpItem->GetNowDurability() == 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpItem);
							delete lpItem;
							lpItem = NULL;
						}
					}
				}
			}
			else
			{

				/* /hz code shop bug fix
				// �� ��
				CItemInstance* lpItem = new CItemInstance;
				lpItem->SetItemInfo(lpItemBuffer, dwSize);
				unsigned short ProtID= lpItem->GetProtoTypeID();

				for(int jk=0;jk<22;jk++)//hz item bug shop bug fix
				{
					if(jk == 12)
						continue;
					if(lpItem->GetItemInfo().m_EquipAttribute.m_usAttributeValue[jk] != 0)
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						MessageBox(GetActiveWindow(), "Shop hack detected!", "Ring Of War Error", MB_OK);
						exit(1);
					}
				}
				CheckItemWithStat(ProtID,lpItem);

				pGame->m_csStatus.AddInventoryItem(lpItem);
				*/

				// �� ��
				CItemInstance* lpItem = new CItemInstance;
				lpItem->SetItemInfo(lpItemBuffer, dwSize);
				pGame->m_csStatus.AddInventoryItem(lpItem);

				if (TakeType::TS_NONE != pCouponIndex.m_cPos)
				{
					CItemInstance *lpCoupon = pGame->m_csStatus.GetInventoryItem(pCouponIndex.m_cIndex);
					if (NULL != lpCoupon)
					{
						lpCoupon->SetNowDurability(lpCoupon->GetNowDurability() - cNum);
						if (lpCoupon->GetNowDurability() <= 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpCoupon);
							delete lpCoupon;
							lpCoupon = NULL;
						} 
						else
						{
							pGame->m_csStatus.SetQuickSlot(lpCoupon);
						}
					}
				}
			}

			return TRUE;
		}
	} 
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktTr::FAIL_DROP_INVEN:		
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[312]);
				break;

			case PktTr::FAIL_WRONG_NPC:				
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[313]);
				break;

			case PktTr::FAIL_ITEM_BUY:					
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[314]);
				break;

			case PktTr::FAIL_PUT_INVEN:				
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[315]);
				break;

			case PktTr::FAIL_FULL_INVEN:					
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 13 ]);
				break;
			case PktTr::FAIL_ENEMY_ITEM:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[3580]);
				break;
			case PktTr::FAIL_GOLD_OVERFLOW:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[218]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharRepairItem)
{
	unsigned long	dwChrID, dwGold;
	Item::ItemPos	pItemIndex;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRepairItem(lpPktBase, &dwChrID, &pItemIndex, &dwGold);
	
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
            CItemInstance* lpItem = 0;
            bool bBrokenRepaired = false;

			switch (pItemIndex.m_cPos)
			{
			case TakeType::TS_EQUIP:
                lpItem = pGame->m_csStatus.m_lstEquipment[pItemIndex.m_cIndex];
				break;

			case TakeType::TS_INVEN:
				lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
				break;
			}

			if (lpItem)
			{
                bBrokenRepaired = (0 == lpItem->GetNowDurability());
				lpItem->SetNowDurability(lpItem->GetMaxDurability());
			}

            pGame->m_csStatus.m_Info.Gold -= min(dwGold, pGame->m_csStatus.m_Info.Gold);

            if (bBrokenRepaired && TakeType::TS_EQUIP == pItemIndex.m_cPos)
            {
                // �԰� �ִ� ����̰�, �μ����� ��� ����������, ��� �Ӽ� ����� �ٽ� �Ѵ�.
                pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
            }
		}

		return TRUE;
	}
	else
	{
		// �ش翡�� ����
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktRpI::NOT_ENOUGH_GOLD:				// ������ �ִ� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1132]);				
				return FALSE;
			case PktRpI::FAIL_NOT_CREATURE:				// ũ�������İ� �������� �ʽ��ϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1133]);				
				return FALSE;
			case PktRpI::FAIL_NOT_NPCZONE:				// NPC�� ����ȣ�� �ٸ��ϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1134]);				
				return FALSE;
			case PktRpI::FAIL_NOT_REPAIR:				// �����������̾���.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1135]);				
				return FALSE;
			case PktRpI::FAIL_NOT_POSITEM:				// �������� ��ġ������.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1136]);				
				return FALSE;
			case PktRpI::FAIL_NOT_STACKABLE:			// �Ҹ𼺾������̴�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1137]);				
				return FALSE;
			case PktRpI::FAIL_NOT_EQUIP:				// ���������̾ƴϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1138]);				
				return FALSE;
			case PktRpI::FAIL_FULL_DRUA:				// �������� Ǯ�̴�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1139]);				
				return FALSE;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharRepairAllItem)
{
	unsigned long	dwChrID, dwGold;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRepairAllItem( lpPktBase, &dwChrID, &dwGold);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
            bool bBrokenRepaired = false;

			for ( int i = 0 ; i < 15 ; i ++ )
			{
                CItemInstance* lpItem = pGame->m_csStatus.m_lstEquipment[ i ];

				if (lpItem && lpItem->GetIsEnableRepair())
				{
                    if (!bBrokenRepaired && 0 == lpItem->GetNowDurability())
                    {
                        bBrokenRepaired = true;
                    }

					lpItem->SetNowDurability(lpItem->GetMaxDurability());
				}
			}

            pGame->m_csStatus.m_Info.Gold -= min(dwGold, pGame->m_csStatus.m_Info.Gold);

            if (bBrokenRepaired)
            {
                // �μ����� ��� ����������, ��� �Ӽ� ����� �ٽ� �Ѵ�.
                pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
            }
		}

		return TRUE;
	}
	else
	{
		// �ش翡�� ����
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktRpAI::NOT_ENOUGH_GOLD:				// ������ �ִ� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1132]);				
				return FALSE;
			case PktRpAI::FAIL_NOT_CREATURE:				// ũ�������İ� �������� �ʽ��ϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1133]);				
				return FALSE;
			case PktRpAI::FAIL_NOT_NPCZONE:				// NPC�� ����ȣ�� �ٸ��ϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1134]);				
				return FALSE;
			case PktRpAI::FAIL_NOT_REPAIR:				// �����������̾���.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1135]);				
				return FALSE;
		}
	}

	return FALSE;
}


GAME_EVENT_FUNC(ParseCharCastObject)
{
	return FALSE;
}

GAME_EVENT_FUNC(ParseCharCastObjectInfo)
{
	unsigned long	dwChrID, dwRecvChrID;
	CastObject		pCastObject;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastObjectInfo(lpPktBase, &dwChrID, &dwRecvChrID, &pCastObject);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{   
		/////////////////////////////////////////////////////////////////////////////////////////
		// ��ų ��ũ��Ʈ�� �ߺ��Ǵ� ���� ���� ���� - by Vincent - 2004 / 10 / 1
		// �ڱ� �ڽ��� SkillNode.cpp ���� ��ų�� ����� �� ����Ʈ�� �ٿ��ش�.
		if ( dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID )
			return TRUE ;

        if ( pCastObject.m_cObjectType <= 43 )
		{
			unsigned short wSkill = pCastObject.m_wTypeID;
			unsigned short wGrade = pCastObject.m_cObjectLevel;

            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID );
            if ( !pTempCreature )
                return true;

            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwChrID );
            if ( !pTempData )
                return true;

            if (wSkill & 0x8000)
			{
				const Skill::ProtoType *lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType(wSkill);
				const Skill::ProtoType *lpSkillType = NULL;
                
                if ( lpSkillTypeTemp )
                    lpSkillType = &lpSkillTypeTemp[ wGrade ];

				if ( pTempCreature && lpSkillType )
				{
                    if ( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) ) // Hide �� ĳ���Ͱ� ����� ����Ʈ�� ��¾���
                        return true;

                    pTempCreature->CancelSkill();

					// �����ϰ�� EndSkill ���� ������ ���� ������ �� �� �ִ�. 
					// �׷���� ���ʹ� ���ĵ� ���¿��� �̵��ϰų� Ư�� ������ �ϰ� �ȴ�. �װ� ������.
					if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_MONSTER )
						// edith 2009.08.06 ��ų���ݽ� �׼� ���¸� Action�� ����
						pTempCreature->SetAction(CA_ATTACK, CA_ATTACK);
					else
					{
						// edith 2009.08.06 ������ ��ų�� ������ ������ ��������Ѵ�.
//						pTempCreature->SetAction(CA_ATTACK, CA_ATTACK);
						pTempCreature->EndSkill( (char*)lpSkillType->m_szCastingFileName );
					}

                    CEffScript *test_script;
//					_ASSERTE(_CrtCheckMemory());
					test_script = new CEffScript;
					if ( test_script )
					{
						if ( pTempData->m_cSex == 1 )
                        {
							test_script->SetSkillInfo( 1, pCastObject.m_cObjectType );
						}
						else
						{
							test_script->SetSkillInfo( 2, pCastObject.m_cObjectType );
						}

						bool bSuccess;

						// by Hades Kang - option dialog < skill effect >
						if ( RYLCreatureManager::Instance()->GetSelfCreature()->GetCharID() == dwChrID )
						{
							bSuccess = test_script->GetScriptBinData(  (char*)lpSkillType->m_szEffectFileName, pGame->m_bSelfEffect );
						}
						else
						{

							switch( pGame->GetClientEnemyType( dwChrID ) )
							{
								case EnemyCheck::EC_ENEMY:
									bSuccess = test_script->GetScriptBinData(  (char*)lpSkillType->m_szEffectFileName, pGame->m_bEnemyEffect );
									break;

								case EnemyCheck::EC_FRIEND:
									bSuccess = test_script->GetScriptBinData(  (char*)lpSkillType->m_szEffectFileName, pGame->m_bFriendEffect );
									break;

								case EnemyCheck::EC_NEUTRAL:
									bSuccess = test_script->GetScriptBinData(  (char*)lpSkillType->m_szEffectFileName, pGame->m_bNeutralEffect );
									break;
							}
						}

						if ( bSuccess )
						{
							// ��ų ���� Ÿ�� ����Ʈ ó��
							if ( wSkill == 0x8603 ) // chain action 
								g_CharacterData.m_SkillAttackValue = lpSkillType->m_cStrikeNum; // Hit Count Setting

                            RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( dwRecvChrID );

                            if ( 0 == pCastObject.m_DstPos.fPointY && pTargetCreature )
							{
								pCastObject.m_DstPos.fPointY = pTargetCreature->GetPosition()->y;
							}

							test_script->SetStartPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );
							test_script->SetEndPos( pCastObject.m_DstPos.fPointX, pCastObject.m_DstPos.fPointY, pCastObject.m_DstPos.fPointZ );
							
							// ��ų ����Ʈ ������ ���� - Vincent
                            if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_MONSTER )
							{
                                const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF );
								if (NULL != pMonster)
								{
									test_script->SetScale( pTempCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
								}
							}
							// ---------------------------------

							if ( pTargetCreature )
							{
                                CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
                                CZ3DGeneralChrModel* pTargetModel = pTargetCreature->GetCharModel();
                                
                                test_script->SetChr( pTempModel, pTargetModel );
							}

                            float fChrDir = pTempCreature->GetDirection();
							fChrDir = -fChrDir;
							fChrDir -= FLOAT_PHI / 2.0f;

							vector3 vecChrToward( cosf(fChrDir), 0.0f, sinf(fChrDir) );
							vecChrToward.Normalize();

							test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
							test_script->SetPlayer( pTempCreature, pTempCreature->GetCharID() );
							test_script->SetMine( false );
							test_script->SetBSkill( true );

							if ( !CSceneManager::m_EffectManager.AddEffScript( test_script ) )			// ����Ʈ ��ũ��Ʈ�� ������ ������ �ε���
							{
								delete test_script;
							}
						} 
                        else
						{
							delete test_script;
						}
                    }
                }
            } 
            else if ( wSkill == AtType::BOW || wSkill == AtType::CROSSBOW )
			{
				if ( pTempCreature )
				{
                    if ( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )  // Hide �� ĳ���Ͱ� ����� ����Ʈ�� ��¾���
                        return true; 

					CEffScript *test_script = new CEffScript;
					if ( test_script )
					{
						if ( pTempData->m_cSex == 1 )
                        {
							test_script->SetSkillInfo( 1, pCastObject.m_cObjectType );
                        }
						else
                        {
							test_script->SetSkillInfo( 2, pCastObject.m_cObjectType );
                        }

						if ( test_script->GetScriptBinData( "arrow_normal.esf" ) )
						{
							test_script->SetStartPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );

							// ��ų ����Ʈ ������ ���� - Vincent
                            if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_MONSTER )
							{
								const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF);
								if (NULL != pMonster)
								{
									test_script->SetScale( pTempCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
								}
							}
							// ---------------------------------

                            RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( dwRecvChrID );

                            if ( pTargetCreature )
                            {
                                test_script->SetEndPos( (D3DXVECTOR3*)pTargetCreature->GetPosition(), 1 );

								CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
                                CZ3DGeneralChrModel* pTargetModel = pTargetCreature->GetCharModel();

                                test_script->SetChr( pTempModel, pTargetModel );
                            }

                            float fChrDir = pTempCreature->GetDirection();
							fChrDir = -fChrDir;
							fChrDir -= FLOAT_PHI / 2.0f;

							vector3 vecChrToward( cosf(fChrDir), 0.0f, sinf(fChrDir) );
							vecChrToward.Normalize();
							test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

							test_script->SetMine( false );
							test_script->SetBSkill( true );

							if ( !CSceneManager::m_EffectManager.AddEffScript( test_script ) )			// ����Ʈ ��ũ��Ʈ�� ������ ������ �ε���
							{
								delete test_script;
							}
						} else
						{
							delete test_script;
						}
					}
				}
			}
        } 
        else if ( pCastObject.m_cObjectType == 200 )
		{
			CEffScript *test_script = new CEffScript;

			if ( test_script )
			{
				bool bSuccess;

				// by Hades Kang - option dialog < skill effect >
				switch( pGame->GetClientEnemyType( dwChrID ) )
				{
					case EnemyCheck::EC_ENEMY:
						bSuccess = test_script->GetScriptBinData( "recall2.esf", pGame->m_bEnemyEffect );
						break;

					case EnemyCheck::EC_FRIEND:
						bSuccess = test_script->GetScriptBinData( "recall2.esf", pGame->m_bFriendEffect );
						break;

					case EnemyCheck::EC_NEUTRAL:
						bSuccess = test_script->GetScriptBinData( "recall2.esf", pGame->m_bNeutralEffect );
						break;
				}

				if ( bSuccess )
				{
					// ��ų ���� Ÿ�� ����Ʈ ó��
					test_script->SetStartPos( pCastObject.m_DstPos.fPointX, pCastObject.m_DstPos.fPointY, pCastObject.m_DstPos.fPointZ );
					test_script->SetEndPos( pCastObject.m_DstPos.fPointX, pCastObject.m_DstPos.fPointY, pCastObject.m_DstPos.fPointZ );

					// ��ų ����Ʈ ������ ���� - Vincent
					if ( CRYLGameData::GetClientIdentity( dwChrID ) == Creature::CT_MONSTER )
					{
						const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( dwChrID & 0x0000FFFF );
						if (NULL != pMonster)
						{
							test_script->SetScale( pMonster->m_MonsterInfo.m_fSize * pMonster->m_MonsterInfo.m_fSkillEffectSize );
						}
					}
					// ---------------------------------

                    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                    if ( !pSelfCreature )
                        return false;
                    
                    float fChrDir = pSelfCreature->GetDirection();
					fChrDir = -fChrDir;
					fChrDir -= FLOAT_PHI / 2.0f;

					vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
					vecChrToward.Normalize();

					test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );
					test_script->SetMine( false );
					test_script->SetBSkill( true );

					if ( !CSceneManager::m_EffectManager.AddEffScript( test_script ) )			// ����Ʈ ��ũ��Ʈ�� ������ ������ �ε���
					{
						delete test_script;
					}
				} 
                else
				{
					delete test_script;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

GAME_EVENT_FUNC(ParseCharUseCashItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwSenderID, dwRecverID;
	Item::ItemPos	pItemIndex;
    unsigned char   cRemainItemNum = 0;

    CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->m_bUseCashFlag = FALSE;
    
 	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharUseItem(
        lpPktBase, dwSenderID, dwRecverID, pItemIndex, cRemainItemNum);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
		if (NULL != lpItem)
		{
			switch (lpItem->GetItemType())
			{
				case Item::ItemType::CASH_ITEM:
				{
					if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
					{
						pGame->m_lPremiumTime += 1440*lpItem->GetItemInfo().m_UseItemInfo.m_dwAmount;
						pGame->m_iPremiumType = 1;

						// ������ ����.
						lpItem->SetNowDurability(cRemainItemNum);
						// �������� 0�̸� �κ����� ����
						if (lpItem->GetNowDurability() <= 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpItem);
							delete lpItem;
							lpItem = NULL;
						} 
						else
						{
							// �����Կ� ���� �������� ������ ������������
							pGame->m_csStatus.SetQuickSlot(lpItem);
						}

					}
					break;
				}
			}
		} 
		return TRUE;
	} 
	else
	{
		if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
		{
			pGame->m_lpCampStartKit = NULL ;
			pGame->m_lpWorldWeaponKit = NULL ;
			pGame->m_lpStartKit = NULL ;
			
			delete pGame->m_csStatus.m_lpSiegeArmsMaterial ;
			pGame->m_csStatus.m_lpSiegeArmsMaterial = NULL ;
		}
			
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktUI::USE_FAILED :			// ��� ���� 
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[328]);
				break;
			}
			
			case PktUI::FAIL_POSITION :			// ��ġ�� �̻��մϴ�.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3568 ]);
				break;
			}
						
			case PktUI::NOT_LIMIT_LEVEL:		// 80 ���� �̸��� ���.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4118 ] ) ;
				break;
			}
			break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharUseItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwSenderID, dwRecverID;
	Item::ItemPos	pItemIndex;
    unsigned char   cRemainItemNum = 0;

    CRYLGameData*	pGame = CRYLGameData::Instance() ;
    
 	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharUseItem(
        lpPktBase, dwSenderID, dwRecverID, pItemIndex, cRemainItemNum);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
		if (NULL != lpItem)
		{
			switch (lpItem->GetItemType())
			{
				case Item::ItemType::SHORT_RANGE_ARMS_KIT:
				case Item::ItemType::LONG_RANGE_ARMS_KIT:
				case Item::ItemType::AIRSHIP_KIT:
				{
					if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
					{
						pGame->m_csStatus.DeleteInventoryItem(lpItem);
						delete lpItem;
						lpItem = NULL;
						
						pGame->m_lpCampStartKit = NULL ;
						pGame->m_lpWorldWeaponKit = NULL ;
						pGame->m_lpStartKit = NULL ;
					}

					break;
				}

				case Item::ItemType::CAMP_KIT:				
				case Item::ItemType::WORLDWEAPON_K_KIT:
				case Item::ItemType::WORLDWEAPON_M_KIT:
				{
					if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
					{
						pGame->m_csStatus.DeleteInventoryItem(lpItem);
						delete lpItem;
						lpItem = NULL;

						pGame->m_lpCampStartKit = NULL ;
						pGame->m_lpWorldWeaponKit = NULL ;
						pGame->m_lpStartKit = NULL ;
					}
					break;
				}

				case Item::ItemType::EXP_BOOK:
				case Item::ItemType::CASH_BOOK:
				case Item::ItemType::QUEST_ITEM:
				case Item::ItemType::EVENT_LOTTERY:
				{
					if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
					{
						// ������ ����.
						lpItem->SetNowDurability(cRemainItemNum);
						// �������� 0�̸� �κ����� ����
						if (lpItem->GetNowDurability() <= 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpItem);
							delete lpItem;
							lpItem = NULL;
						} 
						else
						{
							// �����Կ� ���� �������� ������ ������������
							pGame->m_csStatus.SetQuickSlot(lpItem);
						}
					}
					break;
				}


				case Item::ItemType::MINERAL_KIT:
				case Item::ItemType::SKILL_BOOK:
				case Item::ItemType::PORTAL:
				case Item::ItemType::POTION:
				case Item::ItemType::AMMO:
				case Item::ItemType::FIRE_CRACKER:
				case Item::ItemType::FIXED_PORTAL:
				case Item::ItemType::SKILL_ITEM:
				{
					// ������ ����.
					lpItem->SetNowDurability(cRemainItemNum);
					// �������� 0�̸� �κ����� ����
					if (lpItem->GetNowDurability() <= 0)
					{
						pGame->m_csStatus.DeleteInventoryItem(lpItem);
						delete lpItem;
						lpItem = NULL;
					} 
					else
					{
						// �����Կ� ���� �������� ������ ������������
						pGame->m_csStatus.SetQuickSlot(lpItem);
					}
					break;
				}
			}
		} 

		// ���� �����ۿ� ���� ���Ȼ��¸� ������Ʈ�Ѵ�.
		pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		return TRUE;
	} 
	else
	{
		if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID && dwSenderID == dwRecverID)
		{
			pGame->m_lpCampStartKit = NULL ;
			pGame->m_lpWorldWeaponKit = NULL ;
			pGame->m_lpStartKit = NULL ;
			
			delete pGame->m_csStatus.m_lpSiegeArmsMaterial ;
			pGame->m_csStatus.m_lpSiegeArmsMaterial = NULL ;
		}
			
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktUI::USE_FAILED :			// ��� ���� 
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[328]);
				break;
			}
			
			case PktUI::NOT_SIEGE_TIME :		// ���� �ð��� �ƴ�
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2621]);
				break;
			}
			
			case PktUI::NOT_EXIST_GUILD :		// ��尡 ���ų�, ��忡 �������� �ʾ���
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2582]);
				break;
			}
			
			case PktUI::NOT_MASTER :			// ��� �����Ͱ� �ƴ�
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2601]);
				break;
			}

			case PktUI::NOT_ENOUGH_FAME :		// ��� �� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2622]);
				break;
			}

			case PktUI::ANOTHER_DEV_CAMP :		// �������� �ٸ� ������ ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2623]);
				break;
			}

			case PktUI::INNER_RADIUS :			// �ٸ� ������ ���� �ݰ� �ȿ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2624]);
				break;
			}

			case PktUI::OUTER_RADIUS :			// �Ʊ� ������ ���� �ۿ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2625]);
				break;
			}

			case PktUI::NOT_ENOUGH_MATERIAL :	// ���� ���� ���� ���� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2626]);
				break;
			}

			case PktUI::IN_SAFETYZONE :			// ������ ���� �� ���� ��
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2627]);
				break;
			}

			case PktUI::FAIL_PEACE_GUILD :		// ��ȭ ��� ���
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2628]);
				break;
			}

			case PktUI::FAIL_CAPITAL_ZONE :		// ī�������� �������� 
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3459 ]);
				break;
			}
/*			
			case PktUI::FAIL_ALMIGHTY_GROUND :		// ���� �������� ���� �Ҽ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3459 ]);
				break;
			}
*/
			case PktUI::FAIL_STONE_BATTLE :		// ��ũ ī�������� ���� �Ҽ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3579 ]);
				break;
			}
			
			case PktUI::FAIL_EXIST_ANOTHER_WEAPON :	// �̹� �ٸ� ���� ������ ������
			{
				char strMessage[MAX_PATH];
				sprintf(strMessage, CRYLStringTable::m_strString[ 3194 ], CRYLStringTable::m_strString[ 3460 ]);
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(strMessage);
				break;
			}

			case PktUI::FAIL_ALMIGHTY_PIRATE :		// ���� �������� ���� �Ұ�
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3461 ]);
				break;
			}

			case PktUI::FAIL_REMAIN_1_DAY :		// 1���� ���� �ð��Ŀ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3462 ]);
				break;
			}

			case PktUI::FAIL_REMAIN_2_DAY :		// 2���� ���� �ð��Ŀ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3463 ]);
				break;
			}

			case PktUI::FAIL_REMAIN_3_DAY :		// 3���� ���� �ð��Ŀ� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3464 ]);
				break;
			}

			case PktUI::FAIL_POSITION :			// ��ġ�� �̻��մϴ�.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3568 ]);
				break;
			}
						
			case PktUI::NOT_EXIST_QUEST :		// ����Ʈ�� �������� ����
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2710]);
				break;
			}

			case PktUI::FAIL_HAS_QUEST :		// �̹� ������ �ִ� ����Ʈ
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2711]);
				break;
			}

			case PktUI::FAIL_FULL_QUEST :		// ���� �� �ִ� ����Ʈ���� �� á��
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2712]);
				break;
			}

			case PktUI::FAIL_NOT_GT_GUILD_WAR_TIME :	// ����� �ð��� �ƴ�
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3861 ] ) ;
				break;
			}

			case PktUI::OUTER_RADIUS_SIEGE:			// ���� �ۿ��� ���� ���⸦ ����.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4069 ] ) ;
				break;
			}
			case PktUI::FAIL_HAS_SIEGE_ARMS:		// ���� ���⸦ 1�� �̻� �����Ϸ��� ������.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4088 ] ) ;
				break;
			}
			case PktUI::FAIL_MATERIAL_COUNT:		// ������ ���簡 �����Ҷ�.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4113 ] ) ;
				break;
			}
			case PktUI::NOT_GUILD_MASTER:			// ��� �����͸� ��������.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4115 ] ) ;
				break;
			}
			case PktUI::NOT_CASTLE_NATION:			// �������϶�.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4116 ] ) ;
				break;
			}
			case PktUI::FAIL_MAX_SIEGE_OBJECT:		// ������ �ִ밪���� �����Ǿ��� ���.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4117 ] ) ;
				break;
			}
			case PktUI::NOT_LIMIT_LEVEL:		// 80 ���� �̸��� ���.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4118 ] ) ;
				break;
			}
			break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharInstallSocket)
{
	unsigned long	dwChrID;
	Item::ItemPos	pEquipItemIndex, pSocketItemIndex;
	char*			lpItemBuffer;
	size_t			dwSize;
	unsigned char	cSize;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharInstallSocket(
		lpPktBase, &dwChrID, &pEquipItemIndex, &pSocketItemIndex, &cSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		dwSize = cSize;
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			CItemInstance* lpItem = NULL;
			switch (pEquipItemIndex.m_cPos)
			{
				case TakeType::TS_EQUIP:
					lpItem = pGame->m_csStatus.m_lstEquipment[pEquipItemIndex.m_cIndex];
					break;

				case TakeType::TS_INVEN:
					lpItem = pGame->m_csStatus.GetInventoryItem(pEquipItemIndex.m_cIndex);
					pGame->m_csStatus.DeleteInventoryItem(lpItem);
					break;
			}

			bool bLimitResultBefore = false;

			Item::CEquipment* lpBeforeEquip = reinterpret_cast<Item::CEquipment *>(lpItem->m_lpItemBase);
			if (NULL != lpBeforeEquip)
			{
				unsigned char cType = 0;

				for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					if (true == lpBeforeEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
					{
						bLimitResultBefore = true;
						break;
					}
				}
			}

			// ���� �������� ����
			delete lpItem;
			lpItem = NULL;

			// ������ ���� �� ���������� ����
			lpItem = new CItemInstance;
			lpItem->SetItemInfo(lpItemBuffer, dwSize);

			switch (pEquipItemIndex.m_cPos)
			{
				case TakeType::TS_EQUIP:
					pGame->m_csStatus.AddEquipItem(pEquipItemIndex.m_cIndex, lpItem);
					break;

				case TakeType::TS_INVEN:
					pGame->m_csStatus.AddInventoryItem(lpItem);
					break;
			}

			bool bLimitResultAfter = false;

			Item::CEquipment* lpAfterEquip = reinterpret_cast<Item::CEquipment *>(lpItem->m_lpItemBase);
			if (NULL != lpAfterEquip)
			{
				unsigned char cType = 0;

				for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					if (true == lpAfterEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
					{
						bLimitResultAfter = true;
						break;
					}
				}
			}

			// ���� �ʰ� �����ۿ� ���� �ȳ� ����
			CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
			if (true == bLimitResultBefore || true == bLimitResultAfter)
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 4222 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
			}

			delete pGame->m_lpPickItem;
			pGame->SetPickItem(NULL);

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}

		if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
		{
			pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// ��� ��� ���� 
			case 4:					// ��� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[346]);
				break;

			case 3:					// ���� ��� ���� 
			case 5:					// ���� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[347]);
				break;

			case 6:					// ���� ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[348]);
				break;

			case 7:					// ������ �ʰ��Ǿ����ϴ�.
			case 8:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[530]);
				break;
		
			case 9:					// ���ϰ����� 7���� �Ǿ�� ��밡���մϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[531]);
				break;

			case 10:				// ���Ͽ� �ν���� ������ ���� �ش�������� ��� �Ұ����մϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[532]);
				break;
		}
	}

	return FALSE;
}


// �� �ռ�
GAME_EVENT_FUNC(ParseCharInstallRune)
{
	unsigned long	dwChrID;
	Item::ItemPos	pEquipItemIndex, pRuneItemIndex;
	char*			lpItemBuffer;
	size_t			dwSize;
	unsigned char	cSize;
	unsigned char	runePos;
	unsigned char	type;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharInstallRune(lpPktBase, &dwChrID, &pEquipItemIndex, &pRuneItemIndex, &runePos, &type, &cSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		dwSize = cSize;
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			CItemInstance *lpItem;
			switch (pEquipItemIndex.m_cPos)
			{
				case TakeType::TS_EQUIP:
					delete pGame->m_csStatus.m_lstEquipment[pEquipItemIndex.m_cIndex];
					break;

				case TakeType::TS_INVEN:
					lpItem = pGame->m_csStatus.GetInventoryItem(pEquipItemIndex.m_cIndex);
					pGame->m_csStatus.DeleteInventoryItem(lpItem);
					delete lpItem;
					lpItem = NULL;
					break;
			}

			lpItem = new CItemInstance;
			lpItem->SetItemInfo(lpItemBuffer, dwSize);

			switch (pEquipItemIndex.m_cPos)
			{
				case TakeType::TS_EQUIP:
					pGame->m_csStatus.AddEquipItem(pEquipItemIndex.m_cIndex, lpItem);
					break;

				case TakeType::TS_INVEN:
					pGame->m_csStatus.AddInventoryItem(lpItem);
					break;
			}

			delete pGame->m_lpPickItem;
			pGame->SetPickItem(NULL);

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}

		if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
		{
			pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
		}

		return TRUE;
	} else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					//2 = ��� ��� ���� 
			case 4:					//4 = ��� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[346]);
				break;

			case 3:					//3 = �� ��� ���� 
			case 5:					//5 = �� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[347]);
				break;

			case 6:					//6 = �� ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[348]);
				break;
		}
	}

	return FALSE;
}

//������ �ռ�
GAME_EVENT_FUNC(ParseCharItemChemical)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	Item::ItemPos	pPickkingItemIndex;
	Item::ItemPos	pTargetItemIndex;
	unsigned char	cPickingNum;
	unsigned short	wPickingID;
	char*			lpItemBuffer;
	size_t			dwSize;
	unsigned char	cSize;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharItemChemical(lpPktBase, &dwChrID, 
		&pPickkingItemIndex, &pTargetItemIndex, &wPickingID, &cPickingNum, &cSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		dwSize = cSize;
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			CItemInstance *lpExistItem = NULL;
			CItemInstance *lpItem;
			if ( TakeType::TS_INVEN == pTargetItemIndex.m_cPos )
			{
				lpExistItem = pGame->m_csStatus.GetInventoryItem(pTargetItemIndex.m_cIndex);
				
				if ( lpExistItem )
				{
					pGame->m_csStatus.DeleteInventoryItem(lpExistItem);
				}
				lpItem = new CItemInstance;
				lpItem->SetItemInfo(lpItemBuffer, dwSize);
				pGame->m_csStatus.AddInventoryItem(lpItem);
			}

			if ( cPickingNum == 0 )
			{
				Item::ItemPos	pItemIndex;
				pItemIndex = pGame->m_lpPickItem->m_lpItemBase->GetPos();
				delete pGame->m_lpPickItem;
				pGame->SetPickItem(NULL);
				if ( lpExistItem )
				{
					delete lpExistItem;
					lpExistItem = NULL;
				}
			}
			else
			{
				if ( pGame->m_lpPickItem->GetProtoTypeID() != wPickingID )
				{
					Item::ItemPos itemIndex = pGame->m_lpPickItem->m_lpItemBase->GetPos();
					delete pGame->m_lpPickItem;
					pGame->SetPickItem(NULL);

					if ( lpExistItem )
					{
						lpExistItem->SetNowDurability( cPickingNum );
						lpExistItem->SetPos(pPickkingItemIndex);
					}

					pGame->SetPickItem(lpExistItem);
				}
				else
				{
					pGame->m_lpPickItem->SetNowDurability(cPickingNum);
					pGame->m_lpPickItem->SetPos(pPickkingItemIndex);
				}
			}


			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}

		if (pGame->m_csStatus.m_lstExchange.empty() || !pGame->m_csStatus.m_dwExchangeGold)
		{
			pGame->m_lpInterface->ProcessExchangeCmd(CRYLNetworkData::Instance()->m_dwMyChrID, 100);
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;		//���� �޼��� ���
		
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case Item::Chemical::F_WRONG_RECIPE:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2251]);		// �̰��� ��� �ؾ��ϴ���?
				break;
			case Item::Chemical::F_WRONG_RATE:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2252]);
				break;
			case Item::Chemical::F_NOT_ENOUGH_ITEM:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2253]);
				break;
			case Item::Chemical::F_NOT_QUEST:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[4326]);
				break;
			case Item::Chemical::F_NOT_ABILITY:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[4326]);
				break;
		}
		
	}
	return FALSE;
}


// ����� �Ⱦ���.
GAME_EVENT_FUNC(ParseCharEquipShopInfo)
{
	unsigned long	dwCharID;
	unsigned long	dwNPCID;
	unsigned long	dwTime;
	unsigned char	cRace;	
	unsigned char	cTabPage;
	unsigned char	cNum;
	unsigned short	aryItemID[ 96 ];
	unsigned char	aryItemGrade[ 96 ];


    CRYLGameData*	pGame = CRYLGameData::Instance() ;
	bool			bChange = false;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharEquipShopInfo( lpPktBase, &dwCharID, &dwNPCID, 
												   &dwTime, &cRace, &cTabPage,  
												   &cNum, aryItemID, aryItemGrade);
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		DWORD currentTime = timeGetTime();
		NPCNode* lpNpcNode = g_NPCList.GetNPCNode( dwNPCID ) ;
		if ( !lpNpcNode )			
		{
			return FALSE;
		}

		// �ð��� ���� ������ ���� �������ٸ� ��� �������� �ٲ��ش�.
		bChange = !( lpNpcNode->SetTime( dwTime ) );						

		CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharItemOptionGraft)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);
	unsigned long	dwCharID;
	unsigned long	dwCurrentGold;
	unsigned char	cSize;
	char*			lpItemBuffer;

	size_t			dwSize;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;
	bool			bChange = false;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharItemOptionGraft( 
		lpPktBase, &dwCharID, &dwCurrentGold, &cSize, &lpItemBuffer );

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError || CRYLNetworkData::Instance()->m_dwNetworkError == Item::CEquipment::EQUIPMENT_ERROR::E_GRAFE_FAILD)
	{
		dwSize = cSize;

		bool bLimitResultBefore = false;

		Item::CEquipment* lpBeforeEquip = 
			reinterpret_cast<Item::CEquipment *>(pGame->m_csStatus.m_lpCompositionOrigin->m_lpItemBase);
		if (NULL != lpBeforeEquip)
		{
			unsigned char cType = 0;

			for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
			{				
				if (true == lpBeforeEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
				{
					bLimitResultBefore = true;
					break;
				}
			}
		}

		GM_DELETE( pGame->m_csStatus.m_lpCompositionResult );
		GM_DELETE( pGame->m_csStatus.m_lpCompositionTempResult );
		GM_DELETE( pGame->m_csStatus.m_lpCompositionOffering );
		GM_DELETE( pGame->m_csStatus.m_lpCompositionOrigin );

		pGame->m_csStatus.m_lpCompositionResult = new CItemInstance;
		pGame->m_csStatus.m_lpCompositionResult->SetItemInfo(lpItemBuffer, dwSize);

		bool bLimitResultAfter = false;

		Item::CEquipment* lpAfterEquip = 
			reinterpret_cast<Item::CEquipment *>(pGame->m_csStatus.m_lpCompositionResult->m_lpItemBase);
		if (NULL != lpAfterEquip)
		{
			unsigned char cType = 0;
			for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
			{
				if (true == lpAfterEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
				{
					bLimitResultAfter = true;
					break;
				}
			}
		}

		pGame->m_csStatus.m_iCompositionResult = 1;

		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
		// edith ������ �ռ� ���и޽���
		if(CRYLNetworkData::Instance()->m_dwNetworkError == Item::CEquipment::EQUIPMENT_ERROR::E_GRAFE_FAILD)
		{
			pGame->m_csStatus.m_iCompositionResult = 0;
			// �ɼ��̽� ����
			pChat->AddMessage( CRYLStringTable::m_strString[ 4252 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		}
		else if (true == bLimitResultBefore || true == bLimitResultAfter)
		{
			pGame->m_csStatus.m_iCompositionResult = 0;
			// �ɼ��Ѱ谪 ���� �޽���
			pChat->AddMessage( CRYLStringTable::m_strString[ 4222 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		}

		Item::ItemPos pos;
		pos.m_cIndex = Item::ExtraSpacePos::GRAFT_RESULT_POS ;
		pos.m_cPos = TakeType::TS_EXTRA;
		pGame->m_csStatus.m_lpCompositionResult->SetPos( pos );
		pGame->m_csStatus.SetGold( dwCurrentGold );

		return TRUE;
	}
/*	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case Item::CEquipment::E_WRONG_ATTRIBUTE:					// 2 = �̽� �Ұ����� �ɼ�
			case Item::CEquipment::E_NOT_EQUAL_KIND:					// 3 = �ٸ� ������ ���
			case Item::CEquipment::E_HIGH_GRADE_ORIGINAL:				// 4 = ������ �׷��̵尡 ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[339]);
				break;
		}
	}
*/
	return FALSE;
}

GAME_EVENT_FUNC(ParseCharUpgradeItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwGold;
	Item::ItemPos	pEquipItemIndex, pSocketItemIndex;
	char*			lpItemBuffer;
	unsigned char	cSize, cMineralNum, cValue;
	size_t			dwSize;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharUpgradeItem(lpPktBase, &dwChrID, &dwGold, &cMineralNum, &cValue, &cSize, &lpItemBuffer);
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		dwSize = cSize;

		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			// -------------------------------------------------------
			// ���� ������ ������ ����

			unsigned long dwLevel = 0;
			bool bLimitResultBefore = false;

			Item::CEquipment* lpBeforeEquip = 
				reinterpret_cast<Item::CEquipment *>(pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpItemBase);
			if (NULL != lpBeforeEquip)
			{
				dwLevel = lpBeforeEquip->GetUpgradeLevel();

				unsigned char cType = 0;
				for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					if (true == lpBeforeEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
					{
						bLimitResultBefore = true;
						break;
					}
				}
			}

			// -------------------------------------------------------

			delete pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS];
			pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS] = new CItemInstance;
			pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->SetItemInfo(lpItemBuffer, dwSize);

			// -------------------------------------------------------
			// ���� ������ ������ ����

			unsigned long dwUpgradeLevel = 0;
			bool bLimitResultAfter = false;

			Item::CEquipment* lpAfterEquip = 
				reinterpret_cast<Item::CEquipment *>(pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpItemBase);

			if (NULL != lpAfterEquip)
			{
				dwUpgradeLevel = lpAfterEquip->GetUpgradeLevel();

				unsigned char cType = 0;
				for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					if (true == lpAfterEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
					{
						bLimitResultAfter = true;
						break;
					}
				}
			}

			// -------------------------------------------------------

			CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
			if (true == bLimitResultBefore && true == bLimitResultAfter)
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 4222 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
			}

			// ���ü��� ������ �ھ� �������̸�
			if(pGame->m_csStatus.m_lpUpgrade[1]->GetItemInfo().m_usProtoTypeID == Item::EtcItemID::AWAKENCORE && cValue == 0)
			{
				lpAfterEquip->SetNewEquip(3);

				// ����
				CEffScript *lpEffect  = new CEffScript;
				lpEffect->SetInterfaceSet(true);
				lpEffect->GetScriptBinData("inter_enhance_success.esf");
				
				CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLBlacksmithDlg*	pDlg	= ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg() ;
				lpEffect->m_InterfacePos[0] = pDlg->GetLeft() + 47 ;
				lpEffect->m_InterfacePos[1] = pDlg->GetTop() + 126  ;

				CSceneManager::m_EffectManager.AddInterfaceScript(lpEffect);
			}
			// ���ü��� ������ ȸ�� �������̸�
			else if(pGame->m_csStatus.m_lpUpgrade[1]->GetItemInfo().m_usProtoTypeID == Item::EtcItemID::ENDURANCESTONE && cValue != 0)
			{
				unsigned char cCurDur = lpAfterEquip->GetNumOrDurability();
				if(cCurDur > cValue)
					cCurDur = cValue;

				// ������ ������ ����
				lpAfterEquip->SetMaxNumOrDurability(cValue);
				lpAfterEquip->SetNumOrDurability(cCurDur);

				// ����
				CEffScript *lpEffect  = new CEffScript;
				lpEffect->SetInterfaceSet(true);
				lpEffect->GetScriptBinData("inter_enhance_success.esf");
				
				CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLBlacksmithDlg*	pDlg	= ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg() ;
				lpEffect->m_InterfacePos[0] = pDlg->GetLeft() + 47 ;
				lpEffect->m_InterfacePos[1] = pDlg->GetTop() + 126  ;

				CSceneManager::m_EffectManager.AddInterfaceScript(lpEffect);
			}
			else if (true == pGame->m_csStatus.m_lpUpgrade[Item::ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->GetIsEnableUpgrade() && dwLevel < dwUpgradeLevel)
			{
				// ����
				CEffScript *lpEffect  = new CEffScript;
				lpEffect->SetInterfaceSet(true);
				lpEffect->GetScriptBinData("inter_enhance_success.esf");
				
				CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLBlacksmithDlg*	pDlg	= ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg() ;
				lpEffect->m_InterfacePos[0] = pDlg->GetLeft() + 47 ;
				lpEffect->m_InterfacePos[1] = pDlg->GetTop() + 126  ;

				CSceneManager::m_EffectManager.AddInterfaceScript(lpEffect);
			} 
			else
			{
				// ���н� ������ ����
				if(cValue != 0)
				{
					lpAfterEquip->SetMaxNumOrDurability(cValue);
					lpAfterEquip->SetNumOrDurability(1);
				}

				// ����
				CEffScript *lpEffect  = new CEffScript;
				lpEffect->SetInterfaceSet(true);
				lpEffect->GetScriptBinData("inter_enhance_failed.esf");
				
				CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLBlacksmithDlg*	pDlg	= ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg() ;
				lpEffect->m_InterfacePos[0] = pDlg->GetLeft() + 47 ;
				lpEffect->m_InterfacePos[1] = pDlg->GetTop() + 126  ;

				CSceneManager::m_EffectManager.AddInterfaceScript(lpEffect);
			}

			if (cMineralNum)
			{
				pGame->m_csStatus.m_lpUpgrade[1]->SetNowDurability(cMineralNum);
			} 
			else
			{
				delete pGame->m_csStatus.m_lpUpgrade[1];
				pGame->m_csStatus.m_lpUpgrade[1] = NULL;
			}

			pGame->m_csStatus.m_Info.Gold = dwGold;

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}

		pGame->m_csStatus.SetUpgrading( FALSE );

		return TRUE;
	} 
	else
	{
		pGame->m_csStatus.SetUpgrading( FALSE );

		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case Item::CEquipment::E_GET_EQUIP_FAILED:					// 2 = ��� ��� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[339]);
				break;

			case Item::CEquipment::E_GET_MINERAL_FAILED:					// 3 = ���� ��� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[340]);
				break;

			case Item::CEquipment::E_NOT_EQUIP:						// 4 = ���׷��̵� ���� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[341]);
				break;

			case Item::CEquipment::E_NOT_MINERAL:						// 5 = ���� ������ �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[342]);
				break;

			case Item::CEquipment::E_NOT_MATCH_MINERAL_TYPE:				// 6 = �䱸 ���� Ʋ�� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[343]);
				break;

			case Item::CEquipment::E_NOT_ENOUGH_MINERAL:					// 7 = �䱸 ���� �� Ʋ�� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[344]);
				break;

			case Item::CEquipment::E_REMOVE_EQUIP_FAILED:					// 8 = ��� ���� ���� 
			case Item::CEquipment::E_CREATE_SCRAP_FAILED:					// 9 = ��ö ���� ���� 
			case Item::CEquipment::E_SET_SCRAP_FAILED:					// 10 = ��ö ���� ����
			case Item::CEquipment::E_REMOVE_MINERAL_FAILED:				// 11 = ���� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[345]);
				break;

			case Item::CEquipment::E_UPGRADE_LEVEL_IS_FULL:				// �� �̻� ���׷��̵带 �� �� ����.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[424]);
				break;

			case Item::CEquipment::E_NOT_ENOUGH_MONEY:				// ���� �����ؼ� ���׷��̵� �� �� ����.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[425]);
				break;

			case Item::CEquipment::E_NOT_UPGRADE_ENDURANCE:			// ������ ȸ�� ������ ��� �Ұ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[345]);
				break;
			case Item::CEquipment::E_NOT_UPGRADE_MINENDURANCE:		// ������ ȸ�� ������ ��� �Ұ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[494]);
				break;

			case Item::CEquipment::E_NOT_UPGRADE_AWAKEN:		// �̹� ������ �������Դϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[495]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseDeposit)
{
	unsigned char	cCmd;
	unsigned long	dwGold;
	char			cSavePass;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleDeposit(lpPktBase, &cCmd, &dwGold, &cSavePass);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// â�� ������
		switch (cCmd)
		{
			case PktDeposit::LOGIN:
				{
				CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLVaultDlg*	pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
				CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
				CRYLVaultCertificationdDlg* pVaultCertificationDlg = ( CRYLVaultCertificationdDlg* )pScene->GetVaultCertificationdDlg() ;
				pVaultDlg->ShowNormal() ;
				pVaultDlg->m_bLogined =  TRUE ;
				pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" ) ;
				pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" ) ;				

				CRYLHumanInventoryFrameDlg* pInvenDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
				pVaultDlg->EnableTabButton() ;
				pVaultDlg->SetVisible( TRUE ) ;
				pInvenDlg->SetVisible( TRUE ) ;

				pVaultPasswordDlg->SetVisible( FALSE ) ;
				pVaultCertificationDlg->SetVisible( FALSE ) ;

				pChat->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;
				pChat->SetFocusChattingEdit();
				}
				break;

			case PktDeposit::BUY_TAB:
				{
					unsigned long dwGold[4] = { 0, 1000, 10000, 50000 };	/*payments for tabs in vault hayzohar*/
					pGame->m_csStatus.m_Info.Gold -= dwGold[pGame->m_csStatus.m_dwMaxTab];
					pGame->m_csStatus.m_dwMaxTab++;

					CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLVaultDlg*	pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
					pVaultDlg->EnableTabButton() ;
				}
				break;

			case PktDeposit::LOGOUT:
				{
					CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLVaultDlg*	pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
					CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
					CRYLVaultCertificationdDlg* pVaultCertificationDlg = ( CRYLVaultCertificationdDlg* )pScene->GetVaultCertificationdDlg() ;

					pVaultDlg->m_bLogined		= FALSE ;
					pVaultPasswordDlg->SetVisible( FALSE ) ;
					pVaultCertificationDlg->SetVisible( FALSE ) ;

					pChat->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;
				}
				break;

			case PktDeposit::CHANGE_PASS:
				if (!pGame->m_csStatus.m_cUsedDeposit)
				{
					CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLVaultDlg*	pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
					CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
				
					SendPacket::StoreLogin(g_GameSession, pVaultPasswordDlg->m_pChangePasswordEdit->GetText(), pVaultDlg->m_cSavePass );
					pGame->m_csStatus.m_cUsedDeposit = 1;
					pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" ) ;
					pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" ) ;
					pVaultPasswordDlg->SetVisible( FALSE ) ;
				} 
				else
				{
					CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLVaultDlg*	pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
					CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;

					pVaultDlg->ShowNormal() ;
					pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" ) ;
					pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" ) ;
					pVaultPasswordDlg->SetVisible( FALSE ) ;
					pChat->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;
				}
				break;

			case PktDeposit::SAVED_PASS:
				pGame->m_csStatus.m_dwDepositGold = dwGold;
				pGame->m_csStatus.m_cSavePass = cSavePass;
				break;
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = INSUFFICIENT_MONEY
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[401]);
				break;

			case 3:					// 3 = AUTH_FAILED
				{
				CRYLGameScene*	   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
				pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" ) ;
				pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" ) ;				

				switch (cCmd)
				{
					case PktDeposit::LOGIN:
						pChat->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_DEPOSITPASS ;
						break;

					case PktDeposit::CHANGE_PASS:
						pChat->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CURPASS ;
						break;
				}
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[402]);
				}
				break;

			case 4:					// 4 = INVALID_ARGUMENT
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[403]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseDepositUpdate)
{
	unsigned long	dwFlag;
	unsigned char	cTab;
	unsigned short	wStoreSize;
	bool			bComplete;
	char*			lpStoreBuffer;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleDepositUpdate(lpPktBase, &dwFlag, &cTab, &bComplete, &wStoreSize, &lpStoreBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_csStatus.m_dwDepositFlag = dwFlag;

		// â�� ������
		unsigned short wBufferSize = 0;
		while (wBufferSize < wStoreSize)
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wStoreSize - wBufferSize;
			lpItem->SetItemInfo(lpStoreBuffer, nDBSize);

			wBufferSize += nDBSize;
			lpStoreBuffer += nDBSize;

			pGame->m_csStatus.AddDepositItem(lpItem, FALSE);
		}

		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::ITEM_TAB1) pGame->m_csStatus.m_dwMaxTab = 1;
		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::ITEM_TAB2) pGame->m_csStatus.m_dwMaxTab = 2;
		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::ITEM_TAB3) pGame->m_csStatus.m_dwMaxTab = 3;
		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::ITEM_TAB4) pGame->m_csStatus.m_dwMaxTab = 4;

		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::USED_DEPOSIT)
			pGame->m_csStatus.m_cUsedDeposit = 1;
		else
			pGame->m_csStatus.m_cUsedDeposit = 0;
		if (pGame->m_csStatus.m_dwDepositFlag & PktDepositUpdateDB::SAVED_PASSWORD)
			pGame->m_csStatus.m_cSavePass = 1;
		else
			pGame->m_csStatus.m_cSavePass = 0;

		if (bComplete)
		{
            // �ε� �޽��� �ڽ� �Ѹ�
            CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
            lpMessageBox->Create(CRYLStringTable::m_strString[ 24 ] ) ;

            // �ε� �޽��� �ڽ��� �� �� �ֵ��� ������ �������� �ϰ� �ε��Ѵ�.
            g_ClientMain.Render();

            CRYLGameData* pGame = CRYLGameData::Instance();

            // â�� �����ͱ��� �� �޾�����, ���ӽ���
            pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
            pGame->m_lpInterface->InitGame();
            
			CFrameTimer::ResetTimer( GRYLTimer.GetSceneManagerTimerID() ) ;
			CRYLSceneManager::Instance()->SetScene( GAME_SCENE ) ;				


			if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::CHINA ) // && pGame->m_dwPlayTime > 0)
			{
				pGame->m_dwNewTime = pGame->m_dwPlayTime;
				pGame->m_StartTime = CTime::GetCurrentTime();
				pGame->m_lPlayTimer = 60000;

				/////////////////////////////////////////////////////////////////////////////////////
				// ä�� �޽����� �޽��� �Ѹ���
				char szBuff[ 128 ] ;
				sprintf( szBuff, CRYLStringTable::m_strString[ 4253 ], pGame->m_dwPlayTime ) ;
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
				pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				/////////////////////////////////////////////////////////////////////////////////////
			}
			else
			{
				pGame->m_StartTime = CTime::GetCurrentTime();
				pGame->m_lPlayTimer = 0;
			}

			// ó���� ����������..
			if(pGame->m_iPremiumType > 0)
			{
				// �����̾� ���񽺸� �ϰ� �ִ�. �̰��� 0�̸� �����̾� ���񽺸� ���� �ʴ´�.
				pGame->m_lNewPremiumTime = 0;
				pGame->m_StartPremiumTime = CTime::GetCurrentTime();
				pGame->m_lPlayPremiumTimer = 60000;
			}
		}

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharExchangeCmd)
{
	unsigned long	dwSendID, dwRecvID;
	unsigned char	cCmd;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharExchangeCmd(lpPktBase, &dwSendID, &dwRecvID, &cCmd);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch (cCmd)
		{
			case ClientConstants::ExchangeCmd_Propose:
				if (pGame->m_lpInterface)
				{
					if (pGame->m_lpInterface->GetBusy())
					{
						CRYLNetworkData::SendExchangeCmd(dwSendID, ClientConstants::ExchangeCmd_Refuse);
					} else
					{
						pGame->m_lpInterface->m_dwExchangeCmdResult = 0;
						pGame->m_lpInterface->m_dwExchangeCmdType = ClientConstants::ExchangeCmd_Propose;
						pGame->m_lpInterface->m_dwExchangeCmdSenderID = dwSendID;
						char strText[200];

                        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSendID );
                        if ( pTempData )
                        {
                            sprintf( strText, CRYLStringTable::m_strString[353], pTempData->m_strName.c_str() );
							CRYLMessageBox *lpMessageBox;
							lpMessageBox = new CRYLMessageBox;
							lpMessageBox->Create(strText, MB_EXIT | MB_YES | MB_NO);
							lpMessageBox->SetResult(&pGame->m_lpInterface->m_dwExchangeCmdResult);
							lpMessageBox->SetEnableEnter(FALSE);
							lpMessageBox->SetModal(FALSE);
						}
					}
				}
				break;

			case ClientConstants::ExchangeCmd_Refuse:
				{
					GRYLCommunityDataInstance()->AddRefuseList(dwSendID);

					char strText[200];
                    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSendID );
                    if ( pTempData )
                    {
                        sprintf( strText, CRYLStringTable::m_strString[354], pTempData->m_strName.c_str() );
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					}
				}
				break;

			case ClientConstants::ExchangeCmd_Accept:
				if (dwSendID == CRYLNetworkData::Instance()->m_dwMyChrID)
					pGame->m_lpInterface->m_dwExchangeCmdSenderID = dwRecvID;
				else
					pGame->m_lpInterface->m_dwExchangeCmdSenderID = dwSendID;

			case ClientConstants::ExchangeCmd_OK:
			case ClientConstants::ExchangeCmd_Cancel:
			case ClientConstants::ExchangeCmd_Exchange:
			case ClientConstants::ExchangeCmd_Quit:
			case ClientConstants::ExchangeCmd_Lock:
			case ClientConstants::ExchangeCmd_UnLock:
				pGame->m_lpInterface->ProcessExchangeCmd(dwSendID, cCmd);
				break;
		}
		return TRUE;
	} 
	else
	{
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:
				pChat->AddMessage(CRYLStringTable::m_strString[352], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case 3:
				pChat->AddMessage(CRYLStringTable::m_strString[355], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case 4:					// 4 = ��ȯ �ź�
				pChat->AddMessage(CRYLStringTable::m_strString[367], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharExchangeItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwSize;
	unsigned char	cType, cNum;
	Item::ItemPos	pItemIndex;
	char*			lpItemBuffer;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharExchangeItem(lpPktBase, &dwChrID, &dwSize, &pItemIndex, &cType, &cNum, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch (cType)
		{
			case 0:		// Item
			{
				if (dwSize)
				{
					CItemInstance *lpItem = pGame->m_csStatus.GetExchangeItem(pItemIndex.m_cIndex, FALSE);
					if (lpItem)
					{
						pGame->m_csStatus.DeleteExchangeItem(lpItem, FALSE);
						delete lpItem;
						lpItem = NULL;
					}

					lpItem = new CItemInstance;
					lpItem->SetItemInfo(lpItemBuffer, (size_t &)dwSize);

					pGame->m_csStatus.AddExchangeItem(lpItem, FALSE);
					pGame->m_lpInterface->InitPos(lpItem);
				} 
				else
				{
					CItemInstance *lpExistItem = pGame->m_csStatus.GetExchangeItem(pItemIndex.m_cIndex, FALSE);
					if (lpExistItem) lpExistItem->SetNowDurability((unsigned long)cNum);
				}
				break;
			}

			case 1:		// Gold
			{
				pGame->m_csStatus.m_dwExchange2Gold = dwSize;
				break;
			}

			case 2:
			{
				CItemInstance *lpExistItem = pGame->m_csStatus.GetExchangeItem(pItemIndex.m_cIndex, FALSE);
				if (lpExistItem)
				{
					pGame->m_csStatus.DeleteExchangeItem(lpExistItem, FALSE);
					delete lpExistItem;
					lpExistItem = NULL;
				}
				break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharStallOpen)
{
	unsigned long	dwChrID;
	char			strStallName[32];
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStallOpen(lpPktBase, &dwChrID, strStallName);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwChrID );

		g_ClientCellMgr.UpdateStallName(dwChrID, strStallName);

        if ( strcmp( strStallName, "" ) )
        {
            if ( pTempData )
            {
                pTempData->m_strStreetStallName = strStallName;

                if ( pTempData->m_ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
                {
                    if ( pGame->m_lpInterface )
                    {
                        pGame->m_lpInterface->InitStreetStall( pTempData->m_ulCharID, const_cast<char*>( pTempData->m_strName.c_str() ) );
                    }
                }
            }
        }
        else
        {
            CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLStreetStallDlg*  pDlg   = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg() ;	
	
			if ( pTempData ) 
			{
				pTempData->m_strStreetStallName.clear();
				
				RYLCreature* pCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID ) ;
				pCreature->DeleteSignpostImg();
			}
			
			if ( pDlg->GetVisible() )
			{
				if ( pDlg->GetOwnerID() == dwChrID )
				{
					pDlg->SetVisible( FALSE ) ;
				}
			}
        }
        
		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharStallRegisterItem)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned char	cCmd;
	unsigned long	dwChrID, dwShopID, dwGold;
	TakeType		pTakeType;
	char			strBuyUser[CHAR_INFOST::MAX_NAME_LEN];

	CRYLGameData*		pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance() ;

	pNetwork->m_dwNetworkError = ParsePacket::HandleCharStallRegisterItem(lpPktBase, &dwChrID, &dwShopID, &pTakeType, &dwGold, &cCmd, strBuyUser);

	if (0 == pNetwork->m_dwNetworkError)
	{
		switch (cCmd)
		{
			case PktStRI::SC_REGISTER:
			case PktStRI::SC_CAMP_REGISTER:
			{	
				CRYLGameScene*	pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLStallDlg*	pDlg	= NULL;

				// ������ ����ߴ�.
				CItemInstance *lpItem = new CItemInstance;
				if (lpItem->SetItemInfo(pGame->m_lpPickItem, pTakeType.m_dstPos))
				{
					lpItem->m_lpItemBase->SetStallPrice(dwGold);
					lpItem->m_bStoreItem = TRUE;
					lpItem->m_bTradeItem = TRUE;
					
					if (cCmd == PktStRI::SC_REGISTER)
					{
						// ������
						pDlg = ( CRYLStallDlg* )pScene->GetStreetStallDlg();
						pDlg->AddItemList(lpItem, true);
						pGame->m_csStatus.AddInventoryItem(pGame->m_lpPickItem);
					}

					pGame->SetPickItem(NULL);
				} 
				else
				{
					pGame->m_bStallRegFlag = TRUE;
					SendPacket::CharStallRegisterItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
						pTakeType, dwGold, PktStRI::SC_CANCEL);
				}
				pGame->m_bStallRegFlag = FALSE;
				break;
			}

			case PktStRI::SC_CANCEL:			
			{
				// minotaurs ������ �ȷȴ�?
				// ������ �����ų� �ȷȴ�.
				CItemInstance *lpItem = pGame->m_csStatus.GetStreetTradeItem(pTakeType.m_srcPos);
				if (lpItem)
				{
				
					CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLStreetStallDlg*  pDlg   = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg() ;

					unsigned long dwBuyPrice = lpItem->m_lpItemBase->GetBuyPrice();
					char strString[256];
					if(lpItem->GetIsEnableStack())
					{
						char tempString[256];
						sprintf( tempString, "%d x %s", pTakeType.m_cNum, CRYLStringTable::MakePrintGold(dwBuyPrice));
						sprintf( strString, CRYLStringTable::m_strString[ 4266 ], 
							strBuyUser,const_cast<char*>(lpItem->GetItemInfo().m_SpriteData.m_szName),tempString) ;
					}
					else
					{
						sprintf( strString, CRYLStringTable::m_strString[ 4266 ], 
							strBuyUser,const_cast<char*>(lpItem->GetItemInfo().m_SpriteData.m_szName),CRYLStringTable::MakePrintGold(dwBuyPrice)) ;

					}

					if (lpItem->GetIsEnableStack() && lpItem->GetNowDurability() > pTakeType.m_cNum)
					{
						unsigned long dwNum = lpItem->GetNowDurability() - pTakeType.m_cNum;
						lpItem->SetNowDurability(dwNum);
					} 					
					else
					{
						pDlg->RemoveBuyItem(lpItem);
						pGame->m_csStatus.DeleteStreetTradeItem(lpItem);
					}

					// Minotaurs 4266 - %s�� %s�� %s�� ���� �Ͽ����ϴ�.
					if(strcmp(strBuyUser, ""))
						CRYLChattingDlg::Instance()->AddStallChatMessage( pDlg, strString, pNetwork->m_dwMyChrID, 140 ) ;
				
				} 
				else if (pGame->m_lpPickItem)
				{
					if (dwChrID == pNetwork->m_dwMyChrID)
					{
						delete pGame->m_lpPickItem;
						pGame->SetPickItem(NULL);
					}
				}
				pGame->m_bStallRegFlag = FALSE;
				break;
			}

			case PktStRI::SC_CAMP_CANCEL:			
			case PktStRI::SC_CAMP_SELL:
			{
				CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLCampShopDlg*	pDlg	= ( CRYLCampShopDlg* )pScene->GetCampShopDlg();

				CItemInstance *lpItem = pDlg->GetItem(pTakeType.m_srcPos);
				if (NULL != lpItem)
				{
					if (TRUE == lpItem->GetIsEnableStack() && lpItem->GetNowDurability() > pTakeType.m_cNum)
					{
						unsigned long dwNum = lpItem->GetNowDurability() - pTakeType.m_cNum;
						lpItem->SetNowDurability(dwNum);
					} 
					else
					{
						pDlg->RemoveItem(lpItem);
						if (true == pDlg->IsAdminMode() && cCmd == PktStRI::SC_CAMP_CANCEL)
						{
							lpItem->SetPos(pTakeType.m_dstPos);
							pGame->SetPickItem(lpItem);
						}
						else
						{
							delete lpItem;
							lpItem = NULL;
						}
					}
				} 
				pGame->m_bStallRegFlag = FALSE;
				break;
			}
		}

		return TRUE;
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktStRI::FAIL_NOT_ITEM:					
		case PktStRI::FAIL_NOT_SELL:					
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[136]);
			break;
		}
	}

	pGame->m_bStallRegFlag = FALSE;

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharStallEnter)
{
	unsigned long	dwCustomerID	= 0;
	unsigned long	dwOwner			= 0;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStallEnter(lpPktBase, &dwCustomerID, &dwOwner);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData*	pGame	= CRYLGameData::Instance() ;
		CRYLGameScene*	pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLStallDlg*	pDlg	= NULL;

		// ����
		if (0 != dwOwner)
		{
			switch (Creature::GetCreatureType(dwOwner))
			{
			case Creature::CT_PC:			pDlg = ( CRYLStallDlg* )pScene->GetStreetStallDlg();	break;	// ������
			case Creature::CT_SIEGE_OBJECT:	pDlg = ( CRYLStallDlg* )pScene->GetCampShopDlg();		break;	// ��� ��� ����
			}

			if (NULL != pDlg)
			{
				if (dwCustomerID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					// ���� �� ��
					pGame->m_bStreetStallEnter = TRUE;

					// ������
					if (Creature::CT_PC == Creature::GetCreatureType(dwOwner))
					{
						CharDataNode* pOwnerData = RYLCharacterDataManager::Instance()->GetCharData( dwOwner );
						if ( pOwnerData )
						{
							if (pGame->m_lpInterface)
							{
								CharDataNode* pCustomerData = RYLCharacterDataManager::Instance()->GetCharData( dwCustomerID ) ;
								if ( pCustomerData )
								{
									// ��� ���� ĳ������ �̸��� Ŭ���� �̸����� ��ü�Ѵ�.
									if ( 0 != pOwnerData->m_cNation && pCustomerData->m_cNation != pOwnerData->m_cNation &&
										!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME) )
									{
										pGame->m_lpInterface->InitStreetStall( pOwnerData->m_ulCharID, const_cast<char*>( pOwnerData->m_strName.c_str() ) );
//										pGame->m_lpInterface->InitStreetStall( pOwnerData->m_ulCharID, CRYLStringTable::GetJobName( pOwnerData->m_usClass ) );
									}
									else
									{
										pGame->m_lpInterface->InitStreetStall( pOwnerData->m_ulCharID, const_cast<char*>( pOwnerData->m_strName.c_str() ) );
									}
								}

								return TRUE;
							}
						}
					}
					// ��� ��� ����
					else if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(dwOwner))
					{
						CRYLCampShopDlg* pCampShopDlg = ( CRYLCampShopDlg* )pDlg;
						if (true == pCampShopDlg->Enter(dwOwner))
						{
							pCampShopDlg->GetChattingListBox()->SetCalcViewItemCount();
							pCampShopDlg->AddChatMessage(CRYLStringTable::m_strString[ 137 ], D3DCOLOR_RGBA(100, 255, 100, 255));
							pCampShopDlg->SetVisible( TRUE );
														
							CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
							pInventoryDlg->ShowInventory();
							pInventoryDlg->SetVisible( TRUE );

							CRYLGameData::SetAutoRun(false);
						}

						return TRUE;
					}
				} 
				else
				{
					// ������ ������ ��
					char strText[MAX_PATH];

					CharDataNode* pCustomerData = RYLCharacterDataManager::Instance()->GetCharData( dwCustomerID );
					if ( pCustomerData )
					{
						CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( CRYLNetworkData::Instance()->m_dwMyChrID );
						if ( pSelfData )
						{
							// ��� ���� ĳ������ �̸��� Ŭ���� �̸����� ��ü�Ѵ�.
							if ( 0 != pCustomerData->m_cNation && pCustomerData->m_cNation != pSelfData->m_cNation &&
								!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME) )
							{
//								sprintf( strText, CRYLStringTable::m_strString[ 392 ], CRYLStringTable::GetJobName( pCustomerData->m_usClass ) ) ;
//								pDlg->AddVisitor( dwCustomerID, CRYLStringTable::GetJobName( pCustomerData->m_usClass ) );
								sprintf( strText, CRYLStringTable::m_strString[ 392 ], pCustomerData->m_strName.c_str() );
								pDlg->AddVisitor( dwCustomerID, const_cast<char*>( pCustomerData->m_strName.c_str() ) );
							}
							else
							{
								sprintf( strText, CRYLStringTable::m_strString[ 392 ], pCustomerData->m_strName.c_str() );
								pDlg->AddVisitor( dwCustomerID, const_cast<char*>( pCustomerData->m_strName.c_str() ) );
							}
						}
					} 
					else
					{
						strcpy( strText, CRYLStringTable::m_strString[ 393 ] );
						pDlg->AddVisitor( dwCustomerID, "--------" );
					}

					pChat->AddStallChatMessage(pDlg, strText, dwCustomerID, 128);
				}
			}
		} 
		// ����
		else
		{
			if (TRUE == pScene->GetStreetStallDlg()->GetVisible())
			{
				// ������
				pDlg = ( CRYLStallDlg* )pScene->GetStreetStallDlg();
			}
			else if (TRUE == pScene->GetCampShopDlg()->GetVisible())
			{
				// ��� ��� ����
				pDlg = ( CRYLStallDlg* )pScene->GetCampShopDlg();
			}

			if (NULL != pDlg)
			{
				if (dwCustomerID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					// ���� ���� ��
					pGame->m_bStreetStallEnter = FALSE;
					pDlg->SetVisible( FALSE ) ;
				} 
				else
				{
					// ������ ���� ��
					char strText[MAX_PATH];

					CharDataNode* pCustomerData = RYLCharacterDataManager::Instance()->GetCharData( dwCustomerID );
					if ( pCustomerData )
					{
						CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( CRYLNetworkData::Instance()->m_dwMyChrID );
						if ( pSelfData )
						{
							// ��� ���� ĳ������ �̸��� Ŭ���� �̸����� ��ü�Ѵ�.
							if ( 0 != pCustomerData->m_cNation && pCustomerData->m_cNation != pSelfData->m_cNation &&
								!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME) )
							{
								sprintf( strText, CRYLStringTable::m_strString[ 394 ], pCustomerData->m_strName.c_str() );
//								sprintf( strText, CRYLStringTable::m_strString[ 394 ], CRYLStringTable::GetJobName( pCustomerData->m_usClass ) );
							}
							else
							{
								sprintf( strText, CRYLStringTable::m_strString[ 394 ], pCustomerData->m_strName.c_str() );
							}
						}
					} 
					else
					{
						strcpy( strText, CRYLStringTable::m_strString[ 395 ] );
					}

					pChat->AddStallChatMessage(pDlg, strText, dwCustomerID, 128);	
					pDlg->DeleteVisitor( dwCustomerID );
				}
			}
		}
	} 
	else
	{
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktStE::FAIL_FULL_STALL:					
				break;

			case PktStE::FAIL_ENTER:
				CRYLNetworkData::SendStallLeave();
				break;

			case PktStE::FAIL_LEAVE:
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharStallItemInfo)
{
	unsigned long	dwSize, *dwPrice;
	unsigned long	dwOwnerID;
	unsigned char	cTax;
	char*			lpBuffer;
	unsigned char	cItemNum; 
	unsigned long	aryCustomerID[PktStIInfo::MAX_CUSTOMER_NUM];
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
		ParsePacket::HandleCharStallItemInfo(lpPktBase, &dwOwnerID, aryCustomerID, &cTax, &dwSize, &lpBuffer, &cItemNum, &dwPrice);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLStallDlg*	pDlg   = NULL;

		Creature::CreatureType eType = Creature::GetCreatureType(dwOwnerID);

		switch (eType)
		{
			// ������
			case Creature::CT_PC:			
				pDlg = ( CRYLStallDlg* )pScene->GetStreetStallDlg() ;	
				break;	

			// ��� ��� ����
			case Creature::CT_SIEGE_OBJECT:	
				pDlg = ( CRYLStallDlg* )pScene->GetCampShopDlg() ;		
				(reinterpret_cast<CRYLCampShopDlg*>(pDlg))->ChangeTax(cTax);
				break;	

			default:						
				return FALSE;
		}

		unsigned short wBufferSize = 0, count = 0;
		while (wBufferSize < dwSize)
		{
			CItemInstance* lpItem = new CItemInstance;
			size_t nDBSize = dwSize - wBufferSize;
			lpItem->SetItemInfo(lpBuffer, nDBSize);
			lpItem->m_bStoreItem = TRUE;
			lpItem->m_bTradeItem = TRUE;
			lpItem->m_lpItemBase->SetStallPrice(dwPrice[count]);
			count++;

			wBufferSize += nDBSize;
			lpBuffer += nDBSize;

			pDlg->AddItemList(lpItem, false);
		}

		for ( int i = 0 ; i < PktStIInfo::MAX_CUSTOMER_NUM ; ++i )
		{
			if ( aryCustomerID[ i ] )
			{
				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( aryCustomerID[ i ] );

				if ( pTempData )
				{
					pDlg->AddVisitor( aryCustomerID[i], const_cast<char*>( pTempData->m_strName.c_str() ) );
				} 
				else
				{
					pDlg->AddVisitor( aryCustomerID[i], "--------" );
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharTakeItems)
{
	// ����� ������� �ʴ� ��Ŷ�Դϴ�. (2004-08-17 by �ε�)
	return FALSE;
}

GAME_EVENT_FUNC(ParseCharTakeGold)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwGold;
	unsigned char	cSrcPos, cDstPos;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharTakeGold(lpPktBase, &dwChrID, &cSrcPos, &cDstPos, &dwGold);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			switch (cSrcPos)
			{
				case TakeType::TS_INVEN:
					if (dwGold <= pGame->m_csStatus.m_Info.Gold)
						pGame->m_csStatus.m_Info.Gold -= dwGold;
					else
						pGame->m_csStatus.m_Info.Gold = 0;
					break;

				case TakeType::TS_EXCHANGE:
					// ��ȯâ���� �� ����
					if (dwGold <= pGame->m_csStatus.m_dwExchangeGold)
						pGame->m_csStatus.m_dwExchangeGold -= dwGold;
					else
						pGame->m_csStatus.m_dwExchangeGold = 0;
					break;

				case TakeType::TS_DEPOSIT:
					if (dwGold <= pGame->m_csStatus.m_dwDepositGold)
						pGame->m_csStatus.m_dwDepositGold -= dwGold;
					else
						pGame->m_csStatus.m_dwDepositGold = 0;
					break;
			}

			switch (cDstPos)
			{
				case TakeType::TS_INVEN:
					if (0xFFFFFFFF - pGame->m_csStatus.m_Info.Gold >= dwGold)
					{
						pGame->m_csStatus.m_Info.Gold += dwGold;
						if (pGame->m_csStatus.m_Info.Gold >= 4000000000)
						{
							char strString[200];
							sprintf( strString, CRYLStringTable::m_strString[ 216 ], CRYLStringTable::MakePrintGold(ULONG_MAX) ) ;
							pChat->AddMessage( strString, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}
					}
					else
					{
						pGame->m_csStatus.m_Info.Gold = 0xFFFFFFFF;
					}
					break;

				case TakeType::TS_EXCHANGE:
					// ��ȯâ�� �� �ֱ�
					if (0xFFFFFFFF - pGame->m_csStatus.m_dwExchangeGold >= dwGold)
						pGame->m_csStatus.m_dwExchangeGold += dwGold;
					else
						pGame->m_csStatus.m_dwExchangeGold = 0xFFFFFFFF;
					break;

				case TakeType::TS_DEPOSIT:
					if (0xFFFFFFFF - pGame->m_csStatus.m_dwDepositGold >= dwGold)
						pGame->m_csStatus.m_dwDepositGold += dwGold;
					else
						pGame->m_csStatus.m_dwDepositGold = 0xFFFFFFFF;
					break;
			}

			if (cSrcPos == TakeType::TS_NONE)
			{
				char strMessage[200];
				sprintf(strMessage, CRYLStringTable::m_strString[274], dwGold);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			}

			return TRUE;
		}
	} else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[399]);
				break;

			case 3:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[400]);
				break;
		}
	}

	return FALSE;
}


GAME_EVENT_FUNC(ParseCharItemCompensation)
{
	DWORD dwChrID = 0;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
		ParsePacket::HandleCharItemCompensation(lpPktBase, &dwChrID);

	switch(CRYLNetworkData::Instance()->m_dwNetworkError)
	{
	case 0:
		break;

	case 1:
		break;

	case 2:
		break;
	};

	return TRUE;
}
