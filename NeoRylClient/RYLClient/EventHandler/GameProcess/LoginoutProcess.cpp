
#include "LoginoutProcess.h"

#include "../GameHandler.h"
#include "FrameTimer.h"

#include "RYLClientMain.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Parse/ParseLoginout.h>
#include <Network/ClientSocket/Send/SendLoginout.h>

#include "RYLMessageBox.h"
#include "RYLStringTable.h"
#include "RYLTimer.h"

#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLCommunityData.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLSkillPanel.h"
#include "RYLAbilityPanel.h"

#include "RYLCharacterSelectScene.h"
#include "RYLChannelDlg.h"

#include "BroadcastProcess.h"
#include "RYLChattingDlg.h"

#include <mmsystem.h>
#include "GMMemory.h"

#include <Network/Packet/PacketStruct/GuildPacket.h>

#include "RYLJoystickConfigureDlg.h"
#include "RYLJoystickConfigurePanel.h"


GAME_EVENT_FUNC(ParseCharLogin);
GAME_EVENT_FUNC(ParseCharLogout);
GAME_EVENT_FUNC(ParseChatLoginAck);
GAME_EVENT_FUNC(ParseCharMoveZone);
GAME_EVENT_FUNC(ParseServerZone);
GAME_EVENT_FUNC(ParseCSAuth);
GAME_EVENT_FUNC(ParseKeyInfo);

bool GameProcess::RegisterLoginoutHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;

    dwErrorCount += GameEventHandler.AddHandler(CmdCharLogin, ParseCharLogin) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharLogout, ParseCharLogout) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdChatLogin, ParseChatLoginAck) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharMoveZone, ParseCharMoveZone) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdServerZone, ParseServerZone) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCSAuth, ParseCSAuth) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdKeyInfo, ParseKeyInfo) ? 0 : 1;

    return (0 == dwErrorCount);
}

GAME_EVENT_FUNC(ParseCharLogin)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	CHAR_POS		Pos;
	bool			bPeace, bCheckRelation ;
	unsigned char	cAdminFlag;
    unsigned char   cNameChangeCount ;
	unsigned char	cGuildWarFlag ;
	unsigned char	cRealmWarFlag ;
	unsigned char	cRealmPoint ;
	unsigned char	cTacticsFlag;
	unsigned int	dwPlayTime;
	long			lPremiumTime;
	int				iPremiumType;
	unsigned long	dwServerID ;
	unsigned short	wEquipSize, wInvenSize, wExtraSize, wExchangeSize, wTempInvenSize;
	char		   *lpEquipBuffer, *lpInvenBuffer, *lpExtraBuffer, *lpExchangeBuffer, *lpTempInvenBuffer;
	QUICK			pQuickSlot;
	SPELL			pSpell;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharLogin(lpPktBase, 
																				 *g_pClientSocket, 
																				 &bPeace, 
																				 &bCheckRelation, 
                                                                                 &cNameChangeCount,
																				 &cAdminFlag, 
																				 &cGuildWarFlag,
																				 &cRealmWarFlag,
																				 &cRealmPoint,
																				 &cTacticsFlag,
																				 &dwPlayTime,	// 추가.
																				 &lPremiumTime, // 프리미엄서비스 추가
																				 &iPremiumType,
																				 &pGame->m_csStatus.m_Info, 
																				 &pGame->m_csStatus.m_Skill,
																				 &pQuickSlot,
																				 &pSpell,
																				 &Pos, 
																				 &GRYLTimer.m_dwServerTime, 
																				 &dwServerID, 
																				 &wEquipSize, 
																				 &lpEquipBuffer, 
																				 &wInvenSize, 
																				 &lpInvenBuffer,
																				 &wExtraSize, 
																				 &lpExtraBuffer,
																				 &wExchangeSize,
																			 	 &lpExchangeBuffer,
																				 &wTempInvenSize, 
																				 &lpTempInvenBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        // Broadcast 데이터를 초기화한다.
        g_ClientCellMgr.Clear();

		GRYLTimer.m_dwStartTime = timeGetTime() ;

		CRYLNetworkData::Instance()->m_dwChatServerID = dwServerID;
		
		CRYLNetworkData::Instance()->m_dwMyChrID = pGame->m_csStatus.m_Info.CID;
		CRYLCommunityData::Instance()->m_bPeaceMode = bPeace;
		CRYLCommunityData::Instance()->m_bCheckRelation = bCheckRelation ;
			
		pGame->m_vecStartPosition.x = Pos.LastPoint.fPointX * 100.0f;
		// edith 2009.05.19 로그인시 위에서 캐릭터 떨어지는거 수정.
		pGame->m_vecStartPosition.y = (Pos.LastPoint.fPointY+2.0f) * 100.0f;
		pGame->m_vecStartPosition.z = Pos.LastPoint.fPointZ * 100.0f;

		// edith 2009.05.19 높이가 1미터 이하면 10미터로 바꿈.
		if(pGame->m_vecStartPosition.y < 100.0f)
			pGame->m_vecStartPosition.y = 1000.0f;

		pGame->m_dwPlayTime = dwPlayTime;
		pGame->m_lPremiumTime = lPremiumTime;
		pGame->m_iPremiumType = iPremiumType;

		pGame->m_cTempAdminFlag	= cAdminFlag;

		// 임시로 기억해둠.. 나중에 SelfCreature 에 값을 세팅해준다.
		pGame->m_cTempGuildWarFlag	= cGuildWarFlag;
		pGame->m_cTempRealmWarFlag	= cRealmWarFlag;
		pGame->m_cTempRealmPoint	= cRealmPoint;
		pGame->m_cTempTacticsFlag	= cTacticsFlag;

		pGame->m_csStatus.m_bSummonGuard = FALSE;
		pGame->m_csStatus.m_dwSummonID = 0xFFFFFFFF;
		g_CharacterData.m_bSummon = FALSE;
		g_CharacterData.m_bSummonCmd = FALSE;

		CRYLGameData::Instance()->m_cChangeNameCount = cNameChangeCount;

		// 장비 아이템
		unsigned short wBufferSize = 0;
		while(wBufferSize < wEquipSize)
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wEquipSize - wBufferSize;
			lpItem->SetItemInfo(lpEquipBuffer, nDBSize);

			wBufferSize += nDBSize;
			lpEquipBuffer += nDBSize;

			if (NULL != lpItem->m_lpItemBase)
			{
				pGame->m_csStatus.AddEquipItem(lpItem->m_lpItemBase->GetPos().m_cIndex, lpItem, FALSE);
			}
		}

		// 인벤 아이템
		wBufferSize = 0;
		while(wBufferSize < wInvenSize)
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wInvenSize - wBufferSize;
			lpItem->SetItemInfo(lpInvenBuffer, nDBSize);

			wBufferSize += nDBSize;
			lpInvenBuffer += nDBSize;

			pGame->m_csStatus.AddInventoryItem(lpItem, FALSE);
		}		

		// Extra 아이템
		wBufferSize = 0;
		while(wBufferSize < wExtraSize)
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wExtraSize - wBufferSize;
			lpItem->SetItemInfo(lpExtraBuffer, nDBSize);

			if (nDBSize)
			{
				wBufferSize += nDBSize;
				lpExtraBuffer += nDBSize;

				BOOL bSound = (TakeType::TS_TEMP == lpItem->GetItemPos()) ? FALSE : TRUE;
				pGame->m_csStatus.SetItem(lpItem->m_lpItemBase->GetPos(), lpItem, bSound);
			} 
			else
			{
				delete lpItem;
				break;
			}
		}
		// 임시 인벤토리
		wBufferSize = 0;
		while( wBufferSize < wTempInvenSize )
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wTempInvenSize - wBufferSize;
			lpItem->SetItemInfo(lpTempInvenBuffer, nDBSize);

			if ( nDBSize )
			{
				wBufferSize += nDBSize ;
				lpTempInvenBuffer += nDBSize ;
				if( lpItem->GetItemPos() == TakeType::TS_TEMPINVEN )
				{
					pGame->m_csStatus.AddTempInventory( lpItem );
				}
				else
				{
					delete lpItem;
				}
			}
			else
			{
				delete lpItem;
				break;
			}
		}

		// Exchange 아이템
		wBufferSize = 0;
		while(wBufferSize < wExchangeSize)
		{
			CItemInstance *lpItem;
			lpItem = new CItemInstance;
			size_t nDBSize = wExchangeSize - wBufferSize;
			lpItem->SetItemInfo(lpExchangeBuffer, nDBSize);

			if (nDBSize)
			{
				wBufferSize += nDBSize;
				lpExchangeBuffer += nDBSize;

				pGame->m_csStatus.AddExchangeItem(lpItem, TRUE, FALSE);
			} else
			{
				delete lpItem;
				break;
			}
		}

		pGame->m_csStatus.SetClassSkill();

		pGame->m_csStatus.m_Skill.wSkillNum = 0;
		pGame->m_csStatus.m_Skill.wSlotNum = 0;
		for(int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill)
			{
				if(pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill >= 0x8000)
					pGame->m_csStatus.m_Skill.wSkillNum += pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cLockCount * 6 + pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cSkillLevel;
				
				++pGame->m_csStatus.m_Skill.wSlotNum;
			}
		}

		for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
		{
			if (pQuickSlot.Slots[i].wID)
			{
				switch(pQuickSlot.Slots[i].nType)
				{
					case QUICKSLOT::SKILL:
						pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
						if (!pGame->m_csStatus.m_lstQuickSlot[i]->SetSkillInfo(pQuickSlot.Slots[i].wID,
							pQuickSlot.Slots[i].nSkillLockCount, pQuickSlot.Slots[i].nSkillLevel, TakeType::TS_QSLOT, i, 0))
						{
							pGame->m_csStatus.m_lstQuickSlot[i]->SetItemPos(TakeType::TS_QSLOT);
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase = new Item::CItem;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetPos(i);
							CRYLNetworkData::SendQuickSlotMove(pGame->m_csStatus.m_lstQuickSlot[i], TakeType::TS_NONE, i);
						}
						break;

					case QUICKSLOT::ITEM:
						{
							Item::ItemPos pItemIndex;
							pItemIndex.m_cIndex = i;
							pItemIndex.m_cPos = TakeType::TS_QSLOT;

							pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
							if (!pGame->m_csStatus.m_lstQuickSlot[i]->SetItemInfofromID(pQuickSlot.Slots[i].wID, pItemIndex))
							{
								pGame->m_csStatus.m_lstQuickSlot[i]->SetItemPos(TakeType::TS_QSLOT);
								pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase = new Item::CItem;
								pGame->m_csStatus.m_lstQuickSlot[i]->SetPos(i);
								CRYLNetworkData::SendQuickSlotMove(pGame->m_csStatus.m_lstQuickSlot[i], TakeType::TS_NONE, i);
							} 
							else
							{
								pGame->m_csStatus.SetQuickSlotItemNumber(pGame->m_csStatus.m_lstQuickSlot[i]);
							}
						}
						break;

					default:
						{
							pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetItemPos(TakeType::TS_QSLOT);
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase = new Item::CItem;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetPos(i);
							CRYLNetworkData::SendQuickSlotMove(pGame->m_csStatus.m_lstQuickSlot[i], TakeType::TS_NONE, i);
						}
						break;
				}
			}
		}

		pGame->m_csStatus.CheckGratherQuickSlot();

		if (pGame->m_csStatus.m_Info.Gold >= 4000000000)
		{
			char strString[200];
			sprintf( strString, CRYLStringTable::m_strString[ 216 ], CRYLStringTable::MakePrintGold(ULONG_MAX) ) ;
			CRYLChattingDlg::Instance()->AddMessage( strString, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		}

		return TRUE;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					//2 = 데이터 얻기 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[307]);
				break;

			case 4:					//4 = 이미 접속중 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[308]);
				break;

			case 24: // 포인트 카드 사용자가 아님
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[445]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharLogout)
{
	return FALSE;
}

GAME_EVENT_FUNC(ParseChatLoginAck)
{
	return FALSE;
}

GAME_EVENT_FUNC(ParseCharMoveZone)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

    const unsigned int MAX_CHANNEL = 5;

	unsigned short	aryChannel[MAX_CHANNEL];
	unsigned char	cZone;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharMoveZone(lpPktBase, &cZone, aryChannel);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		unsigned long dwChannelCount = 0;
		for(unsigned long i = 0; i < MAX_CHANNEL; i++)
		{
			if (aryChannel[i]) dwChannelCount++;
		}

		CRYLNetworkData::Instance()->m_dwNextZone = cZone;

		if (dwChannelCount == 0)
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[482]);
		} 
		else if (dwChannelCount == 1)
		{
			CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
/*
			CRYLSkillPanel*		pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;
			CRYLAbilityPanel*	pAbilityPanel	 = ( CRYLAbilityPanel* )pDlg->GetAbilityPanel() ;
			pPanel->ClearSkillSlot() ;
			pPanel->ClearVirtualSkill() ;

			pAbilityPanel->ClearAbilitySlot();
			pAbilityPanel->ClearVirtualAbility();

			pScene->GetSkillTooltipDlg()->SetVisible( FALSE );
*/
			pDlg->SetVisible(FALSE);

			for(unsigned long i = 0; i < MAX_CHANNEL; i++)
			{
				if (aryChannel[i])
				{
					pGame->m_dwClientMode = CLIENT_ZONEMOVE;
					g_pClientSocket->SetStatusFlag(NS_ZONEMOVE);
					if (GameRYL::SERVER_TEST == CRYLNetworkData::Instance()->m_eServerType)
					{
						SendPacket::ServerZone(g_GameSession, CRYLNetworkData::Instance()->m_dwNextZone, 0);
					} 
					else
					{
						// edith 2008.07.08 1개 채널만 있는 서버를 검출하는 로직. 현재 8, 16번만 사용하고 있어서 다 지운다.
						// edith 2009.07.21 존이 추가됐으니 추 17, 18번 존 추가
						if (CRYLNetworkData::Instance()->m_dwNextZone == 8 || CRYLNetworkData::Instance()->m_dwNextZone == 16 ||
							CRYLNetworkData::Instance()->m_dwNextZone == 17 || CRYLNetworkData::Instance()->m_dwNextZone == 18)
						{
							SendPacket::ServerZone(g_GameSession, CRYLNetworkData::Instance()->m_dwNextZone, 0);
						} 
						else
						{
							SendPacket::ServerZone(g_GameSession, CRYLNetworkData::Instance()->m_dwNextZone, i);
						}
					}

					break;
				}
			}
		} 
		else
		{
			memset( CRYLNetworkData::Instance()->m_aryChannel, 0, sizeof( unsigned short ) * MAX_CHANNEL ) ;
			CRYLNetworkData::Instance()->m_aryChannelNum = 0 ;

			memcpy( CRYLNetworkData::Instance()->m_aryChannel, aryChannel, sizeof( unsigned short ) * MAX_CHANNEL ) ;
			CRYLNetworkData::Instance()->m_aryChannelNum = dwChannelCount ;

			CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ;
			( ( CRYLChannelDlg* )pScene->GetChannelDlg() )->InitChannel() ;
			pScene->GetChannelDlg()->SetVisible( TRUE ) ;
		}
		return TRUE;
	} else
	{
		CRYLMessageBox *lpMessageBox;
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 1:					//1 = 국가전에 귀속된 상태
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			case 3:					//3 = 존이 존재하지 않음.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[482]);
				break;

			case 5:					//5 = 세션이 열려 있지 않음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			// ichabod code
			// 6 error : 배틀그라운드 인원제한으로 인한 로그인 제한.
			case 6:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1581]);
				break;
			// hackermz code
			// 7 error : 레벨제한으로 인한 로그인 제한.
			case 7:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[4120]);
				break;
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseServerZone)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwServerID;
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleServerZone(lpPktBase, *g_pClientSocket, &dwServerID);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		SERVER_ID siServer;
		siServer.dwID = dwServerID;
		CRYLNetworkData::Instance()->m_dwChannelID = siServer.GetChannel();
		CRYLNetworkData::Instance()->m_dwPresentZone = siServer.GetZone();

		pGame->FadeIn();
		pGame->m_bSelectChar = TRUE;

		// LagreMap 닫기
		CRYLGameScene* pGameScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
		pGameScene->GetLargeMapDlg()->SetVisible( FALSE ) ;

		// 채널 닫기
		CRYLCharacterSelectScene* pCharacterSelectScene = 
			static_cast<CRYLCharacterSelectScene*>( CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ) ;
		pCharacterSelectScene->GetChannelDlg()->SetVisible( FALSE ) ;

		g_pClientSocket->SetStatusFlag(NS_SERVERMOVE);

		// Disconnect() 하고 완료되면 연결		
        g_pClientSocket->Disconnect(ClientSocket::GameEventHandler);

        ClientNet::CClientEventHandler* lpGameHandler = CGameEventHandler::Create();
		g_pClientSocket->Open(g_pClientSocket->GetAddressString(ClientSocket::AddressType::MoveZoneAddr),
			g_pClientSocket->GetAddressPort(ClientSocket::AddressType::MoveZoneAddr), lpGameHandler);

		return TRUE;
	}
	else
	{
		if (pGame->m_dwClientMode == CLIENT_ZONEMOVE)
		{
			pGame->m_dwClientMode = CLIENT_GAME;
			CFrameTimer::ResetTimer( GRYLTimer.GetSceneManagerTimerID() ) ;
			CRYLSceneManager::Instance()->SetScene( GAME_SCENE ) ;
		}

		CRYLMessageBox *lpMessageBox;
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = 캐릭터 존이 잘못 되어 있습니다.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[389]);
				break;

			case 4:					// 4 = 인원이 초과
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[390]);
				break;

			case 5:					// 5 = 세션이 열려 있지 않음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;
		}

	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCSAuth)
{
#ifndef NO_GAMEGUARD
	unsigned long dwCharID = 0;
	unsigned long dwAuthCode = 0;

	GG_AUTH_DATA authCode2;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCSAuth(lpPktBase, &dwCharID, &dwAuthCode, &authCode2);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CNPGameLib* lpNPGameLib = CRYLNetworkData::Instance()->m_lpNPGameLib;

        if (0 != lpNPGameLib)
        {
//			DWORD result1 = lpNPGameLib->Auth( dwAuthCode ) ;	// AUTH 1.0 
//			DWORD result2 = lpNPGameLib->Auth2( &authCode2 ) ;	// AUTH 2.0 
			// edith 2009.08.11 게임가드 2.5 업그레이드
			DWORD result1 = 0;
			DWORD result2 = lpNPGameLib->Auth2( &authCode2 ) ;	// AUTH 2.0 
/*
			{
				char msg[1024];
				sprintf(msg, "ParseCSAuth : %x, %x, %x, %x, %d", 
					authCode2.dwIndex,
					authCode2.dwValue1, 
					authCode2.dwValue2, 
					authCode2.dwValue3, result2	);
				MessageBox(NULL, msg, "GG", MB_OK);
			}
*/
        }
		return TRUE;
	}
#endif

	return FALSE;
}

GAME_EVENT_FUNC(ParseKeyInfo)
{
	unsigned long dwCID = 0;
	unsigned long dwKeyInfo[PktKeyInfo::MAX_KEY_INFO];

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleKeyInfo(lpPktBase, &dwCID, dwKeyInfo);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
		CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
		CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

		pJoystickPanel->SetForceFeedback(dwKeyInfo[PktKeyInfo::MAX_KEY_INFO-1]);

		unsigned long* dwSettingKey = pJoystickPanel->GetSettingKey();

		memcpy(dwSettingKey, dwKeyInfo, sizeof(unsigned long)*PktKeyInfo::MAX_KEY_INFO-1);

		if (pJoystickPanel->GetVisible())
		{
			pJoystickPanel->UpdateKeyPage();
			pJoystickPanel->SetKeyIndex();
		}

		return TRUE;
	}

	return FALSE;
}

