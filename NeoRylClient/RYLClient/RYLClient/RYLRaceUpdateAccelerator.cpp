
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLClientMain.h"

#include <Creature/Siege/SiegeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GUITextEdit.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLMessageBoxManager.h"
#include "RYLTabButton.h"
#include "RYLStringTable.h"
#include "RYLEdit.h"
#include "..\\StringFilter.h"

#include "RYLHumanInventoryFrameDlg.h"
#include "RYLSocietyFrameDlg.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLHelpFrameDlg.h"
#include "RYLSystemDlg.h"
#include "RYLPartyPanel.h"
#include "RYLChattingDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLMIniMapDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLChangeNameDlg.h"
#include "RYLSiegeTimeDlg.h"

#include "RYLRaceBase.h"
#include "GMMemory.h"

#include <Creature/EnemyCheck.h>

#include "InputJoystick.h"
#include "RYLJoystickConfigurePanel.h"

void CRYLRaceBase::UpdateGameAccelerator(void)
{
	if ( CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() )
		return;

	CGMUICustomEdit*    pEdit = NULL;
	CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
	if ( pEdit->GetFocusState() )
		return;

	if ( g_MessageBoxManager.GetIsExistModal() )
		return;

	CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

	if( !pScene )
		return;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

	if( !pSelfCreature )
		return;
	
	if ( pScene->GetCastleDoorblockDlg()->GetVisible() )
		return ;

	if ( pScene->GetGuildWarPenaltyDlg()->GetVisible() )
		return ;

	if(EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))
		return;
	
	//=============================================================================
	// 조이스틱 감도 설정 (2006-05-18 by hackermz)
	// std::min, std::max가 안되는데 왜일까..-_-a
	if (1 == g_ClientMain.GetUseJoystick())
	{
		bool bShowMsg = false;
		short sSensitivity = static_cast<short>(CInputJoystick::GetInstance()->GetSensitivity());

		if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
			g_DeviceInput.KeyDown( VK_DELETE ) )	// 감도 -5
		{
			bShowMsg = true;

			if ( 10 > (sSensitivity - 5) )
			{
				return;
			}

			sSensitivity -= 5;
			CInputJoystick::GetInstance()->SetSensitivity(static_cast<unsigned char>(sSensitivity));
		}
		else if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
			g_DeviceInput.KeyDown( VK_END ) )	// 감도 +5
		{
			bShowMsg = true;

			if ( 250 < (sSensitivity + 5) )
			{
				return;
			}

			sSensitivity += 5;
			CInputJoystick::GetInstance()->SetSensitivity(static_cast<unsigned char>(sSensitivity));
		}
		else if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
			g_DeviceInput.KeyDown( VK_INSERT ) )	// 감도 -10
		{
			bShowMsg = true;

			if ( 10 > (sSensitivity - 10) )
			{
				return;
			}

			sSensitivity -= 10;
			CInputJoystick::GetInstance()->SetSensitivity(static_cast<unsigned char>(sSensitivity));
		}
		else if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
			g_DeviceInput.KeyDown( VK_HOME ) )	// 감도 +10
		{
			bShowMsg = true;

			if ( 250 < (sSensitivity + 5) )
			{
				return;
			}
			
			sSensitivity += 10;
			CInputJoystick::GetInstance()->SetSensitivity(static_cast<unsigned char>(sSensitivity));
		}

		if(true == bShowMsg)
		{
			CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
			CHAR	strMessage[ MAX_PATH ] ;

			sprintf(strMessage, CRYLStringTable::m_strString[ 4225 ], sSensitivity);
			pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;				
		}
	}
	//=============================================================================

	// 인벤토리창
	if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		(g_DeviceInput.KeyDown('I') || KeyControl::IsKeyPress(KeyControl::KEY_COLUM_CHAR)) )/*[I key] for inventory panel hayzohar*/
	{
		CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
		if ( pInventoryDlg->GetTabButton()->GetFocusTabButton() == 0 )
		{
			pInventoryDlg->SetVisible( !pInventoryDlg->GetVisible() );
		}
		else
		{
			pInventoryDlg->ShowInventory();
			pInventoryDlg->SetVisible( TRUE );
		}

		// Simple Mouse Mode 로 변경
		CRYLGameData*	pGame = CRYLGameData::Instance();
		if ( pGame && !pGame->m_bMouseMode )
		{
			if ( !pScene->GetStatusAndQuestFrameDlg()->GetVisible() && !pScene->GetSocietyFrameDlg()->GetVisible() &&
				!pScene->GetGuildFrameDlg()->GetVisible() && !pScene->GetLargeMapDlg()->GetVisible() &&
				!pScene->GetHumanInventoryFrameDlg()->GetVisible() && !pScene->GetSystemDlg()->GetVisible() &&
				!pScene->GetBlacksmithDlg()->GetVisible() && !pScene->GetExchangeDlg()->GetVisible() &&
				!pScene->GetJobChangeDlg()->GetVisible() && !pScene->GetOptionDlg()->GetVisible() &&
				!pScene->GetQuestDetailDataDlg()->GetVisible() && !pScene->GetQuestOpeningDlg()->GetVisible() &&
				!pScene->GetRankingDlg()->GetVisible() && !pScene->GetStatusRetrainDlg()->GetVisible() &&
				!pScene->GetStreetStallDlg()->GetVisible() && !pScene->GetNewTradeDlg()->GetVisible() &&
				!pScene->GetVaultCertificationdDlg()->GetVisible() && !pScene->GetVaultDlg()->GetVisible() &&
				!pScene->GetVaultPasswordDlg()->GetVisible() && !pScene->GetEmblemUpgradeDlg()->GetVisible() )
			{
				pGame->m_bSimpleMouse = FALSE;
			}
			else 
			{
				pGame->m_bSimpleMouse = TRUE;
			}
		}
	}
	/*
	// 어빌리티
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && (g_DeviceInput.KeyDown('L'))  )
	{
		CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
		if ( pInventoryDlg->GetTabButton()->GetFocusTabButton() == 2 )
		{
			pInventoryDlg->SetVisible( !pInventoryDlg->GetVisible() );
		}
		else
		{
			pInventoryDlg->ShowAbility();
			pInventoryDlg->SetVisible( TRUE );
		}
	} 
	*/
/*
	// 소셜모션
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()  )
	{
		CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
		if ( pInventoryDlg->GetTabButton()->GetFocusTabButton() == 3 )
		{
			pInventoryDlg->SetVisible( !pInventoryDlg->GetVisible() );
		}
		else
		{
			pInventoryDlg->ShowSocialAction();
			pInventoryDlg->SetVisible( TRUE );
		}
	} 
*/	// 스킬창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		(g_DeviceInput.KeyDown('K')  || KeyControl::IsKeyPress(KeyControl::KEY_COLUM_SKILL)) )/*[K key] for skills panel hayzohar*/
	{
		CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
		if ( pInventoryDlg->GetTabButton()->GetFocusTabButton() == 1 )
		{
			pInventoryDlg->SetVisible( !pInventoryDlg->GetVisible() );
		}
		else
		{
			pInventoryDlg->ShowSkill();
			pInventoryDlg->SetVisible( TRUE );
		}
	} 
	// 사회창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		(g_DeviceInput.KeyDown('Y') ||  KeyControl::IsKeyPress(KeyControl::KEY_COLUM_PARTY)) )/*[Y key] for party panel hayzohar*/
	{
		CRYLSocietyFrameDlg* pSocietyDlg = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg();
		pSocietyDlg->SetVisible( !pSocietyDlg->GetVisible() );

		if ( TRUE == pSocietyDlg->GetVisible() )
		{
			reinterpret_cast<CRYLPartyPanel*>(pSocietyDlg->GetPartyPanel())->UpdatePartyList();
			reinterpret_cast<CRYLPartyPanel*>(pSocietyDlg->GetPartyPanel())->UpdateControl();
		}
	} 
	// 스테이터스 & 퀘스트 창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		(g_DeviceInput.KeyDown('U') ||  KeyControl::IsKeyPress(KeyControl::KEY_COLUM_INVEN)) )/*[U key] for status panel hayzohar*/
	{
		if(!pScene)
			return;

		CRYLStatusAndQuestFrameDlg* pQuestDlg = ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg();
		if( !pQuestDlg || !pQuestDlg->GetTabButton())
			return;

		if ( pQuestDlg->GetTabButton() && pQuestDlg->GetTabButton()->GetFocusTabButton() == 0 )
		{
			pQuestDlg->SetVisible( !pQuestDlg->GetVisible() );
		}
		else
		{
			pQuestDlg->ShowStatusPanel();
			pQuestDlg->SetVisible( TRUE );
		}
	}
	// 퀘스트창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()&& 
		(g_DeviceInput.KeyDown('T') || KeyControl::IsKeyPress(KeyControl::KEY_COLUM_QUEST)) )/*[T key] for quests panel hayzohar*/
	{
		CRYLStatusAndQuestFrameDlg* pStatusDlg = ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg();
		if ( pStatusDlg->GetTabButton()->GetFocusTabButton() == 1 )
		{
			pStatusDlg->SetVisible( !pStatusDlg->GetVisible() );
		}
		else
		{
			pStatusDlg->ShowExcutionQuestPanel();
			pStatusDlg->SetVisible( TRUE );
		}
	} 
	// 도움말
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown('H') )/*[H key] for help panel hayzohar*/
	{
		CRYLHelpFrameDlg* pHelpDlg = ( CRYLHelpFrameDlg* )pScene->GetHelpDlg();
		pHelpDlg->SetVisible( !pHelpDlg->GetVisible() );
	} 
	// 옵션창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown(VK_PAUSE) )/*[Pause key] for menu hayzohar*/
	{
		CRYLSystemDlg* pSystemDlg = ( CRYLSystemDlg* )pScene->GetSystemDlg();
		pSystemDlg->SetVisible( !pSystemDlg->GetVisible() );

		// Simple Mouse Mode 로 변경
		CRYLGameData*	pGame = CRYLGameData::Instance();
		if ( pGame && !pGame->m_bMouseMode && pSystemDlg->GetVisible() ) pGame->m_bSimpleMouse = TRUE;
	} 
	// 길드창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown('G') )/*[G key] for guild panel hayzohar*/
	{
		CRYLGuildFrameDlg* pGuildDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();

		// edith 2009.06.20 길드창 띄울때 다른창 닫는 루틴 제거 (원복)
//		if(!pGuildDlg->GetVisible())
//			pScene->GUIVisible( FALSE ); // Minotaurs 길드창띄우기

		pGuildDlg->SetVisible( !pGuildDlg->GetVisible() );
	}
	// 라지맵
/*	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
		(g_DeviceInput.KeyDown('L') ||  KeyControl::IsKeyPress(KeyControl::KEY_COLUM_MAPONOFF)) )
	{
		CRYLLargeMapFrameDlg* pLargeDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
		pLargeDlg->SetFocusPanel(0);
		pLargeDlg->SetVisible( !pLargeDlg->GetVisible() );

		// for Battle Ground Client
		if ( ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || 
			CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE ) &&
			pLargeDlg->GetVisible() )
		{
			USHORT usTemp = 0;
			pLargeDlg->ShowLargeMap( usTemp );
		}
	}
*/	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
		(g_DeviceInput.KeyDown('L') ||  KeyControl::IsKeyPress(KeyControl::KEY_COLUM_MAPONOFF)) )/*[M key] for Map on/off hayzohar*/ // WAS M
	{
		CRYLLargeMapFrameDlg* pLargeDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
		pLargeDlg->SetFocusPanel(0);
		pLargeDlg->SetVisible( !pLargeDlg->GetVisible() ); 

		// for Battle Ground Client
//hayzohar was ( ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE ) &&pLargeDlg->GetVisible() )
		     if( ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE ) &&pLargeDlg->GetVisible() )
		{
			USHORT usTemp = 0;
			pLargeDlg->ShowLargeMap( usTemp );
		}
	}
	// 전장 상태 확인창
	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown('O') )/*[O key] for siege window hayzohar*/
	{
		CRYLSiegeTimeDlg* pSiegeTimeDlg = (CRYLSiegeTimeDlg*)pScene->GetSiegeTimeDlg();
		pSiegeTimeDlg->SetVisible(!pSiegeTimeDlg->GetVisible());
	}

	else if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown('V') )/*[V key] for names view hayzohar*/
	{
		CRYLGameData::Instance()->ChangeViewNameMode() ;
	}
}

void CRYLRaceBase::UpdateGame_W_KeyDown(void)
{
	if ( CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() )
		return;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( g_DeviceInput.KeyDown( VK_ESCAPE ) )
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( !pSelfCreature )
			return;

		if ( g_CharacterData.GetWeaponControlKind() != CHARACTER_CONTROL && 
			pGame->GetClientIdentity( pSelfCreature->GetCharID() ) == Creature::CT_SIEGE_OBJECT &&
			g_pClientSocket->GetStatusFlag() != NS_GETOFFARMS )
		{
			switch ( pSelfCreature->GetObjectType() )
			{
				case SHORT_RANGE_CASTLE_ARMS :
				case LONG_RANGE_CASTLE_ARMS :
				{
					SendPacket::CharCastleCmd( g_GameSession, 
						pSelfCreature->GetRiderID(0), 
						pSelfCreature->GetCastleID(),   
						pSelfCreature->GetCharID(), 
						0, 0, 
						PktCastleCmd::CASTLE_GETOFF_ARMS ) ;
					break ;
				}

				case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
				case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
				case AIRSHIP :
				{
					SendPacket::CharSiegeArmsCmd( g_GameSession, 
						pSelfCreature->GetOwnerID(), 
						pSelfCreature->GetCharID(), 
						PktSiegeArmsCmd::SIEGE_GETOFF_ARMS ) ;
					break ;
				}
			}

			g_pClientSocket->SetStatusFlag( NS_GETOFFARMS ) ;
			return ;
		}
	}

	CRYLGameScene* pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

	CGMUICustomEdit*    pEdit = NULL;
	CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
	if ( pEdit && pEdit->GetFocusState() )
	{
		if (g_DeviceInput.KeyDown( VK_ESCAPE ))
		{
			if (pScene->GetVaultPasswordDlg()->GetVisible() || 
				pScene->GetVaultCertificationdDlg() || 
				pScene->GetWorldWeaponLaunchDlg()->GetVisible() ||
				pScene->GetStreetStallDlg()->GetVisible() || 
				pScene->GetCampShopDlg()->GetVisible())
			{
				pScene->GetVaultPasswordDlg()->SetVisible( FALSE );
				pScene->GetVaultCertificationdDlg()->SetVisible( FALSE );

				pScene->GetWorldWeaponLaunchDlg()->SetVisible( FALSE );

				pScene->GetStreetStallDlg()->SetVisible( FALSE );
				pScene->GetCampShopDlg()->SetVisible( FALSE );

				pScene->GetHumanInventoryFrameDlg()->SetVisible( FALSE );
			}
		}
		return;
	}

	// edith 2008.01.22 WASD 누를때 키보드 모드를 바꾸는거 추가
	if (FALSE == GetIsExchange() && 
		(TRUE == g_DeviceInput.KeyDown(VK_ESCAPE) || FALSE == CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()) &&
		(TRUE == g_DeviceInput.KeyDown(VK_ESCAPE) || 
		TRUE == pGame->m_bMouseMode && TRUE == g_DeviceInput.KeyDown('W') || 
		TRUE == pGame->m_bMouseMode && TRUE == g_DeviceInput.KeyDown('S') || 
		TRUE == pGame->m_bMouseMode && TRUE == g_DeviceInput.KeyDown('A') || 
		TRUE == pGame->m_bMouseMode && TRUE == g_DeviceInput.KeyDown('D') ))
	{	
		if ((pScene->GetNewTradeDlg()->GetVisible() ||
			pScene->GetVaultDlg()->GetVisible() ||
			pScene->GetSystemDlg()->GetVisible() || 
			pScene->GetStatusAndQuestFrameDlg()->GetVisible() || 	
			pScene->GetSocietyFrameDlg()->GetVisible() || 		
			pScene->GetGuildFrameDlg()->GetVisible() || 			
			pScene->GetHumanInventoryFrameDlg()->GetVisible() || 	
			pScene->GetQuestOpeningDlg()->GetVisible() || 		
			pScene->GetQuestDetailDataDlg()->GetVisible() || 		
			pScene->GetBlacksmithDlg()->GetVisible() || 			
			pScene->GetItemCompositionDlg()->GetVisible() || 			
			pScene->GetStatusRetrainDlg()->GetVisible() || 		
			pScene->GetRankingDlg()->GetVisible() || 				
			pScene->GetJobChangeDlg()->GetVisible() || 			
			pScene->GetOptionDlg()->GetVisible() || 				
			pScene->GetLargeMapDlg()->GetVisible() ||
			pScene->GetHelpDlg()->GetVisible() ||
			pScene->GetBattleGroundChangeMoney()->GetVisible() ||
			pScene->GetBattleGroundPrevResultDlg()->GetVisible() ||
			pScene->GetBattleGroundStatusDlg()->GetVisible() ||
			pScene->GetEmblemUpgradeDlg()->GetVisible() ||
			pScene->GetCampUpgradeDlg()->GetVisible() ||
			pScene->GetWorldWeaponLoadDlg()->GetVisible() ||
			pScene->GetWorldWeaponLaunchDlg()->GetVisible() ||
			pScene->GetCampRightDlg()->GetVisible() ||
			pScene->GetCampChangeDlg()->GetVisible() ||
			pScene->GetCampShowTaxInfoDlg()->GetVisible() ||
			pScene->GetMiningAmountProcessDlg()->GetVisible() ||
			pScene->GetMiningCampDlg()->GetVisible() ||
			pScene->GetMiningAmountDlg()->GetVisible() ||
			pScene->GetCampCreatingDlg()->GetVisible() ||
			pScene->GetCastleStatusFrameDlg()->GetVisible() ||
			pScene->GetCastleMineralDlg()->GetVisible() ||
			pScene->GetCastleEmblemRightDlg()->GetVisible() ||
			pScene->GetSiegeTimeDlg()->GetVisible() ||
			pScene->GetGMReportDlg()->GetVisible() ||
			pScene->GetJoystickConfigureDlg()->GetVisible() ||
			((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetAuthoritySettingDlg()->GetVisible() ||
			((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetHostilityDlg()->GetVisible() ||
			m_bPopupShow || g_MessageBoxManager.GetIsExistModal()) && 
			g_DeviceInput.KeyDown(VK_ESCAPE)) 
		{
			pScene->GetNewTradeDlg()->SetVisible( FALSE );
			pScene->GetVaultDlg()->SetVisible( FALSE );
			pScene->GetSystemDlg()->SetVisible( FALSE );
			pScene->GetStatusAndQuestFrameDlg()->SetVisible( FALSE );
			pScene->GetSocietyFrameDlg()->SetVisible( FALSE );
			pScene->GetGuildFrameDlg()->SetVisible( FALSE );
			pScene->GetHumanInventoryFrameDlg()->SetVisible( FALSE );
			pScene->GetQuestOpeningDlg()->SetVisible( FALSE );
			pScene->GetQuestDetailDataDlg()->SetVisible( FALSE );
			pScene->GetBlacksmithDlg()->SetVisible( FALSE );
			pScene->GetItemCompositionDlg()->SetVisible( FALSE );
			pScene->GetStatusRetrainDlg()->SetVisible( FALSE );
			pScene->GetRankingDlg()->SetVisible( FALSE );
			pScene->GetJobChangeDlg()->SetVisible( FALSE );
			pScene->GetOptionDlg()->SetVisible( FALSE );
			pScene->GetLargeMapDlg()->SetVisible( FALSE );
			pScene->GetHelpDlg()->SetVisible( FALSE );
			pScene->GetBattleGroundChangeMoney()->SetVisible( FALSE );
			pScene->GetBattleGroundPrevResultDlg()->SetVisible( FALSE );
			pScene->GetBattleGroundStatusDlg()->SetVisible( FALSE );
			pScene->GetEmblemUpgradeDlg()->SetVisible( FALSE );
			pScene->GetMiningAmountProcessDlg()->SetVisible( FALSE );
			pScene->GetMiningCampDlg()->SetVisible( FALSE );
			pScene->GetMiningAmountDlg()->SetVisible( FALSE );
			pScene->GetCampUpgradeDlg()->SetVisible( FALSE );
			pScene->GetWorldWeaponLoadDlg()->SetVisible( FALSE );
			pScene->GetWorldWeaponLaunchDlg()->SetVisible( FALSE );
			pScene->GetCampRightDlg()->SetVisible( FALSE );		
			pScene->GetCampChangeDlg()->SetVisible( FALSE );
			pScene->GetCampShowTaxInfoDlg()->SetVisible( FALSE );
			pScene->GetCampCreatingDlg()->SetVisible( FALSE );
			pScene->GetCastleStatusFrameDlg()->SetVisible( FALSE );
			pScene->GetCastleMineralDlg()->SetVisible( FALSE );
			pScene->GetCastleEmblemRightDlg()->SetVisible( FALSE );
			pScene->GetSiegeTimeDlg()->SetVisible(FALSE);
			pScene->GetGMReportDlg()->SetVisible(FALSE);
			pScene->GetJoystickConfigureDlg()->SetVisible(FALSE);
			((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetAuthoritySettingDlg()->SetVisible( FALSE ) ;
			((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetHostilityDlg()->SetVisible( FALSE ) ;

			m_bPopupShow = FALSE;
			m_dwPopupSelect = 0xFFFFFFFF;

			g_MessageBoxManager.DestroyList(true);

			if (m_bShowHelp)
			{
				m_bShowHelp = FALSE;
			}
		} 
//		else if (g_DeviceInput.KeyDown(VK_ESCAPE) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_FORWARD))
		else if (g_DeviceInput.KeyDown(VK_ESCAPE)
			|| g_DeviceInput.KeyDown('W') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_FORWARD)
			|| g_DeviceInput.KeyDown('S') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_BACKWARD)
			|| g_DeviceInput.KeyDown('A') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_LEFT)
			|| g_DeviceInput.KeyDown('D') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_RIGHT))	// edith 2008.01.22 WASD
		{
			if ( pGame->m_bSimpleMouse )
			{
				pGame->m_bSimpleMouse = FALSE;
			}

			pGame->m_bMouseMode = !pGame->m_bMouseMode;
			CEffScript* test_script = new CEffScript;


			if (pGame->m_bMouseMode)
			{			
				g_CharacterData.MouseModeChange();
				pGame->m_bShowCursor = TRUE;
				test_script->GetScriptBinData("inter_interface-type-mouse.esf");	
			} 
			else
			{
				g_CharacterData.MouseModeChange();
				pGame->m_bShowCursor = FALSE;
				CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[141], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				test_script->GetScriptBinData("inter_interface-type-keyboard.esf");
			}

			test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
			test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;

			CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
		}
	}

	if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && !g_MessageBoxManager.GetIsExistModal())
	{
		if ( g_DeviceInput.KeyDown('W') || g_DeviceInput.KeyDown('A') || g_DeviceInput.KeyDown('S') || g_DeviceInput.KeyDown('D') ||
			KeyControl::IsKeyState(KeyControl::KEY_COLUM_FORWARD) ||
			KeyControl::IsKeyState(KeyControl::KEY_COLUM_BACKWARD) ||
			KeyControl::IsKeyState(KeyControl::KEY_COLUM_LEFT) ||
			KeyControl::IsKeyState(KeyControl::KEY_COLUM_RIGHT))
		{
			pGame->m_bSimpleMouse = FALSE;
		}
	}
}


void CRYLRaceBase::CloseInterface(void)
{
	CRYLGameScene* pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

	pScene->GetNewTradeDlg()->SetVisible( FALSE );
	pScene->GetVaultDlg()->SetVisible( FALSE );
	pScene->GetSystemDlg()->SetVisible( FALSE );
	pScene->GetStatusAndQuestFrameDlg()->SetVisible( FALSE );
	pScene->GetSocietyFrameDlg()->SetVisible( FALSE );
	pScene->GetGuildFrameDlg()->SetVisible( FALSE );
	pScene->GetHumanInventoryFrameDlg()->SetVisible( FALSE );
	pScene->GetQuestOpeningDlg()->SetVisible( FALSE );
	pScene->GetQuestDetailDataDlg()->SetVisible( FALSE );
	pScene->GetBlacksmithDlg()->SetVisible( FALSE );
	pScene->GetItemCompositionDlg()->SetVisible( FALSE );
	pScene->GetStatusRetrainDlg()->SetVisible( FALSE );
	pScene->GetRankingDlg()->SetVisible( FALSE );
	pScene->GetJobChangeDlg()->SetVisible( FALSE );
	pScene->GetOptionDlg()->SetVisible( FALSE );
	pScene->GetLargeMapDlg()->SetVisible( FALSE );
	pScene->GetHelpDlg()->SetVisible( FALSE );
	pScene->GetBattleGroundChangeMoney()->SetVisible( FALSE );
	pScene->GetBattleGroundPrevResultDlg()->SetVisible( FALSE );
	pScene->GetBattleGroundStatusDlg()->SetVisible( FALSE );
	pScene->GetEmblemUpgradeDlg()->SetVisible( FALSE );
	pScene->GetMiningAmountProcessDlg()->SetVisible( FALSE );
	pScene->GetMiningCampDlg()->SetVisible( FALSE );
	pScene->GetMiningAmountDlg()->SetVisible( FALSE );
	pScene->GetCampUpgradeDlg()->SetVisible( FALSE );
	pScene->GetWorldWeaponLoadDlg()->SetVisible( FALSE );
	pScene->GetWorldWeaponLaunchDlg()->SetVisible( FALSE );
	pScene->GetCampRightDlg()->SetVisible( FALSE );		
	pScene->GetCampChangeDlg()->SetVisible( FALSE );
	pScene->GetCampShowTaxInfoDlg()->SetVisible( FALSE );
	pScene->GetCampCreatingDlg()->SetVisible( FALSE );
	pScene->GetCastleStatusFrameDlg()->SetVisible( FALSE );
	pScene->GetCastleMineralDlg()->SetVisible( FALSE );
	pScene->GetCastleEmblemRightDlg()->SetVisible( FALSE );
	pScene->GetSiegeTimeDlg()->SetVisible(FALSE);
	pScene->GetGMReportDlg()->SetVisible(FALSE);
	pScene->GetJoystickConfigureDlg()->SetVisible(FALSE);
	((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetAuthoritySettingDlg()->SetVisible( FALSE ) ;
	((CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg())->GetHostilityDlg()->SetVisible( FALSE ) ;

	m_bPopupShow = FALSE;
	m_dwPopupSelect = 0xFFFFFFFF;

	g_MessageBoxManager.DestroyList(true);

	if (m_bShowHelp)
	{
		m_bShowHelp = FALSE;
	}
}



/*void CRYLRaceBase::UpdateGame_AltTab_KeyDown( void )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	//hz added this to disable Alt+Tab:
	if ((TRUE == g_DeviceInput.KeyDown(VK_TAB))&&(TRUE == g_DeviceInput.KeyDown(VK_MENU)))
	{
		exit(1);
	}

	//hz added this to disable Alt+Tab:
	if ((TRUE == g_DeviceInput.KeyHold(VK_TAB))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU)))
	{
		exit(1);
	}

	if ((TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_TAB)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_LWIN))&&(TRUE ==  KeyPressed('D')))
	{
		exit(1);
	}
	if ((TRUE ==  KeyPressed('D'))&&(TRUE == g_DeviceInput.KeyHold(VK_LWIN)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_RWIN))&&(TRUE ==  KeyPressed('D')))
	{
		exit(1);
	}
	if (TRUE == g_DeviceInput.KeyHold(VK_RWIN))
	{
		exit(1);
	}
	if (TRUE == g_DeviceInput.KeyHold(VK_LWIN))
	{
		exit(1);
	}
	if ((TRUE ==  KeyPressed('D'))&&(TRUE == g_DeviceInput.KeyHold(VK_RWIN)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_ESCAPE)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_ESCAPE))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU)))
	{
		exit(1);
	} 
	if ((TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		exit(1);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		exit(1);
	}

	//hz end
}*/

void CRYLRaceBase::UpdateGame_NUMPAD_KeyDown(void)
{
	CRYLGameScene*	 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if ( pScene->GetVaultPasswordDlg()->GetVisible() || 
		pScene->GetVaultCertificationdDlg()->GetVisible() || 
		pScene->GetBattleGroundChangeMoney()->GetVisible() )
	{
		return;
	}

	CRYLWorldWeaponLaunchDlg* pLaunchDlg = ( CRYLWorldWeaponLaunchDlg* )pScene->GetWorldWeaponLaunchDlg();
	CRYLCampShopDlg* pShopDlg = ( CRYLCampShopDlg* )pScene->GetCampShopDlg();
	CRYLStreetStallDlg* pStallDlg = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg();

	if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
		 !pLaunchDlg->GetPosXEdit()->GetFocusState() &&
		 !pLaunchDlg->GetPosYEdit()->GetFocusState() &&
		 !pShopDlg->GetChattingEdit()->GetFocusState() &&
		 !pStallDlg->GetChattingEdit()->GetFocusState() &&
		 !g_MessageBoxManager.GetIsExistModal() )
	{
		if (!pGame->m_lQuickChatTimer)
		{
			BOOL bChatEnable = FALSE;
			char strMacroChat[1024];

			// edith 2009.05.30 NUM패드를 이용한 매크로 챗.			/*[NUMPAD keys] for macro chat KEYMSG.txt hayzohar*/
			if (g_DeviceInput.KeyDown(VK_NUMPAD1))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[581], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD2))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[582], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD3))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[583], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD4))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[584], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD5))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[585], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD6))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[586], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD7))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[587], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD8))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[588], 1024);
			} 
			else if (g_DeviceInput.KeyDown(VK_NUMPAD9))
			{
				bChatEnable = TRUE;
				strncpy(strMacroChat, CRYLStringTable::m_strString[589], 1024);
			}

			if(bChatEnable)
			{
				g_StringFilter.ConvertFilter(strMacroChat);
				CRYLNetworkData::SendChatMessage(NULL, strMacroChat, CHATMODE_CHAT);
				pGame->m_lQuickChatTimer = 5000;
			}
		} 
		else
		{
			if (g_DeviceInput.KeyDown(VK_NUMPAD1) || g_DeviceInput.KeyDown(VK_NUMPAD2) || 
				g_DeviceInput.KeyDown(VK_NUMPAD3) || g_DeviceInput.KeyDown(VK_NUMPAD4) ||
				g_DeviceInput.KeyDown(VK_NUMPAD5) || g_DeviceInput.KeyDown(VK_NUMPAD6) || 
				g_DeviceInput.KeyDown(VK_NUMPAD7) || g_DeviceInput.KeyDown(VK_NUMPAD8) ||
				g_DeviceInput.KeyDown(VK_NUMPAD9))
			{
				char strMessage[MAX_PATH];
				unsigned long t = pGame->m_lQuickChatTimer / 1000;
				if (t)
				{
					sprintf(strMessage, CRYLStringTable::m_strString[950], t);
				} 
				else
				{
					strcpy(strMessage, CRYLStringTable::m_strString[951]);
				}
				CRYLChattingDlg::Instance()->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
		}
	}     

}

void CRYLRaceBase::UpdateGame_TAB_KeyDown( BOOL &bClick, BOOL &bEdge )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if ( pGame->m_bMouseMode )
		return;

	if (TRUE == g_DeviceInput.KeyDown(VK_TAB))
	{
		CRYLGameScene* pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

		if (FALSE == CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
			FALSE == ((CRYLWorldWeaponLaunchDlg *)pScene->GetWorldWeaponLaunchDlg())->GetPosXEdit()->GetFocusState() && 
			FALSE == ((CRYLWorldWeaponLaunchDlg *)pScene->GetWorldWeaponLaunchDlg())->GetPosYEdit()->GetFocusState())
		{
			pGame->m_bSimpleMouse = !pGame->m_bSimpleMouse;
		}
	}
}

void CRYLRaceBase::UpdateGame_F_KeyDown(void)
{
	CRYLGameScene* pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	if ( ( ( CRYLStreetStallDlg * )pScene->GetStreetStallDlg() )->GetChattingEdit()->GetFocusState() ||
		 ( ( CRYLCampShopDlg * )pScene->GetCampShopDlg() )->GetChattingEdit()->GetFocusState() || 
		 ( ( CRYLChangeNameDlg* )pScene->GetChangeNameDlg() )->GetVisible() )
	{
		return;
	}

	if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		g_DeviceInput.KeyDown('M') && /*[F key] for replying last private msg hayzohar*/
		strcmp(CRYLCommunityData::Instance()->m_strRecentChatChrID, "")	&& 
		!g_MessageBoxManager.GetIsExistModal() )
	{
		char strText[MAX_PATH];
		strcpy( strText, "/s ");
		strcat( strText, CRYLCommunityData::Instance()->m_strRecentChatChrID );
		strcat( strText, " ");

		strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_strRecentChatChrID );
		CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE;
		CRYLChattingDlg::Instance()->m_pChatEdit->SetText( strText );
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( TRUE );

		HIMC hImc;
		hImc = ImmGetContext(g_ClientMain.m_hWnd);
		ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
		ImmReleaseContext(g_ClientMain.m_hWnd, hImc);
	}
}

void CRYLRaceBase::UpdateGame_R_KeyDown(void)
{
	CRYLGameData*	pGame = CRYLGameData::Instance();
	if ( TRUE == GetBusy() )
	{
		pGame->m_bAutoRunMode = FALSE;
		return;
	}

	CGMUICustomEdit*    pEdit = NULL;
	CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
	if ( pEdit->GetFocusState() )
		return;

	if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
		g_DeviceInput.KeyDown('R') && /*[R key] for Autorun mode hayzohar*/
		!g_MessageBoxManager.GetIsExistModal() )
	{
		pGame->m_bSimpleMouse = FALSE;
		pGame->m_bAutoRunMode = TRUE;
	}
}

void CRYLRaceBase::UpdateGameAltnG( BOOL& bEdge )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();
	if ( pGame->m_bTargetMouseMode && !bEdge )
	{
		if ( g_DeviceInput.GetIsLeftMouseDown() )
		{
			// 타겟 처리
			POINT *ptMouserPos = g_DeviceInput.GetMouseLocal();
			CZ3DGeneralChrModel *pSelectChr = NULL;
			pSelectChr = CSceneManager::PickingCharacter(ptMouserPos->x, ptMouserPos->y);

			if (pSelectChr)
			{
				RYLCreature* pCreature = RYLCreatureManager::Instance()->GetCreature( pSelectChr );
				if ( !pCreature )
					return;

				unsigned long dwType = CRYLGameData::GetClientIdentity( pCreature->GetCharID() );
				if (dwType == Creature::CT_MONSTER)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create("몬스터가 떴다.");
				} 
				else if (dwType == Creature::CT_NPC)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create("NPC로 헤쳐모여");
				} 
				else if (dwType == Creature::CT_PC)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create("PC로 헤쳐모여");
				}
			} 
			else
			{
				vector3 vecClickPos = CSceneManager::PickMousePos( ptMouserPos->x, ptMouserPos->y );

				// 처리
				char strText[200];
				sprintf( strText, "%f, %f, %f", vecClickPos.x, vecClickPos.y, vecClickPos.z );
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( strText );
			}

			pGame->m_bTargetMouseMode = FALSE;
		}
	}
}

void CRYLRaceBase::UpdateGame_ForMiniMap_KeyHold(void)
{
	CGMUICustomEdit*    pEdit = NULL;
	CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
	if ( pEdit->GetFocusState() )
		return;

	if ( !g_MessageBoxManager.GetIsExistModal() )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		CRYLMIniMapDlg*		 pDlg	= ( CRYLMIniMapDlg* )pScene->GetMiniMapDlg();

		if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyHold( VK_ADD ) )
		{
			pDlg->ZoomIn();
		}

		if ( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyHold( VK_SUBTRACT ) )
		{
			pDlg->ZoomOut();
		}
	}
}

void CRYLRaceBase::UpdateGameHelp( float fCreatureUpdate )
{
	g_CharacterData.Update(fCreatureUpdate);
	g_CharacterData.UpdateCreatureControl(FALSE);
	NPCSpawn();

	if (g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsRightMouseUp())
	{
		m_bShowHelp = FALSE;
	}
}

