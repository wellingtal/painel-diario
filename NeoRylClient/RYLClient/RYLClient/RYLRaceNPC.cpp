
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"

#include <Creature/EnemyCheck.h>
#include <Creature/Character/CharacterClass.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendQuest.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendLoginOut.h>
#include <Network/ClientSocket/Send/SendBGServer.h>
#include <Network/ClientSocket/Send/SendCastle.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "WinInput.h"
#include "BaseGraphicsLayer.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "GUITextEdit.h"

#include "RYLCampManager.h"
#include "RYLCamp.h"
#include "RYLCastleManager.h"
#include "RYLCastle.h"

#include "RYLStringTable.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLChattingDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLStatusRetrainDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLEmblemUpgradeDlg.h"
#include "RYLCastleEmblemRightDlg.h"
#include "RYLCampRightDlg.h"
#include "RYLCampCreatingDlg.h"
#include "RYLCampUpgradeDlg.h"
#include "RYLCampRepairDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLChangeNameDlg.h"
#include "RYLCampChangeDlg.h"
#include "RYLCampShowTaxInfoDlg.h"
#include "RYLMiningCampDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLWorldWeaponLoadDlg.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLCompleteQuestPanel.h"
#include "RYLCastleStatusFrameDlg.h"

#include "RYLGameTimeManager.h"
#include <GameTime/GameTimeConstants.h>

#include "battleground client/BattleGroundChangeMoneyDlg.h"
#include "battleground client/BattleGroundStatusDlg.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "GMMemory.h"

#include "../EventHandler/gameprocess/BroadcastProcess.h"

#ifdef __NEWNPCSCRIPT
	BOOL CRYLRaceBase::InitPopup(unsigned long dwUID, unsigned long dwKindPos, unsigned long dwKindPopup)
#else
	BOOL CRYLRaceBase::InitPopup(unsigned long dwKindPopup, unsigned long dwUID, unsigned long dwKindPos)
#endif
{
	CRYLGameScene*	pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGameData*	pGame	= CRYLGameData::Instance() ;

	if ( pScene->GetVaultDlg()->GetVisible() ||
		pScene->GetBlacksmithDlg()->GetVisible() ||			
		pScene->GetNewTradeDlg()->GetVisible() ||				
		pScene->GetStreetStallDlg()->GetVisible() ||
		pScene->GetEmblemUpgradeDlg()->GetVisible() )	     
	{
		return FALSE ;
	}

	const unsigned long dwEnemyType = CRYLGameData::GetClientEnemyType(dwUID);
	if ( EnemyCheck::EC_ENEMY == dwEnemyType || ( EnemyCheck::EC_NEUTRAL == dwEnemyType && POPUP_PC != dwKindPopup ) )
	{
		m_dwKindPopup = 0;
		m_dwPopupSelectChrID = 0;
		m_bPopupShow = FALSE;
		m_dwKindPos = 0;

		return FALSE;
	} 
	else
	{
		m_dwKindPopup = dwKindPopup;
		m_dwPopupSelectChrID = dwUID;
		m_bPopupShow = TRUE;
		m_dwKindPos = dwKindPos;
	}

	PopupMenu	pmAddMenu ;
	m_lstPopupMenu.clear();

	switch (m_dwKindPopup)
	{
#ifndef __NEWNPCSCRIPT
		case POPUP_PC:
			InitPopupPC( &pmAddMenu ) ;
			break;

		case POPUP_GUARD:
		case POPUP_A_GUARD:
			InitPopupGuard( &pmAddMenu ) ;
			break;

		case POPUP_NORMAL:
		case POPUP_A_NORMAL:
			InitPopupNormal( &pmAddMenu ) ;
			break;

		case POPUP_NOBIND:
		case POPUP_A_NOBIND:
			InitPopupNoBind( &pmAddMenu ) ;
			break;

		case POPUP_WEAPONMERCHANT:
		case POPUP_ARMOURMERCHANT:
		case POPUP_DARKWEAPONMERCHANT:
		case POPUP_DARKARMOURMERCHANT:
		case POPUP_DARKETCMERCHANT:
		case POPUP_A_WEAPONMERCHANT:
		case POPUP_A_ARMOURMERCHANT:
		case POPUP_A_DARKWEAPONMERCHANT:
		case POPUP_A_DARKARMOURMERCHANT:
		case POPUP_A_DARKETCMERCHANT:
			InitPopup2KindMerchant( &pmAddMenu ) ;
			break;

		case POPUP_ETCMERCHANT:
		case POPUP_A_ETCMERCHANT:
		case POPUP_MEDALSHOP:
		case POPUP_MEDALSHOP_WEAPON:
		case POPUP_MEDALSHOP_ARMOUR:			
		case POPUP_A_MEDALSHOP:
		case POPUP_A_MEDALSHOP_WEAPON:
		case POPUP_A_MEDALSHOP_ARMOUR:
		case POPUP_ADVANCEDADVENTURE:
		case POPUP_ADVANCEDFIGHTER:
		case POPUP_ADVANCEDROGUE:		
		case POPUP_ADVANCEDMAGE:	
		case POPUP_ADVANCEDACOLYTEE:
		case POPUP_ADVANCEDBEGINNER:
		case POPUP_ADVANCEDCOMBATANT:
		case POPUP_ADVANCEDOPPICATER:
			InitPopupMerchant( &pmAddMenu ) ;
			break;

		case POPUP_COUNSELLER:
		case POPUP_A_COUNSELLER:
			InitPopupCounseller( &pmAddMenu ) ;
			break;

		case POPUP_BLACKSMITH:
		case POPUP_A_BLACKSMITH:
			InitPopupBlackSmith( &pmAddMenu ) ;
			break;

		case POPUP_ADVENTURETRAINER:
		case POPUP_A_BEGINNERTRAINER:
			InitPopupAdventureTrainer( &pmAddMenu ) ;
			break;

		case POPUP_FIGHTERTRAINER:
		case POPUP_A_COMBATANTTRAINER:
			InitPopupFigtherTrainer( &pmAddMenu ) ;

			if ( (pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Fighter && m_dwKindPopup == POPUP_FIGHTERTRAINER) ||
				(pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Combatant && m_dwKindPopup == POPUP_A_COMBATANTTRAINER) )
			{
				pmAddMenu.m_dwPopupKind = POPUPMENU_JOBCHANGE;
				strcpy(pmAddMenu.m_strPopupStr, CRYLStringTable::m_strString[9]);
				m_lstPopupMenu.push_back(pmAddMenu);
			}
			break;

		case POPUP_ROGUETRAINER:
		case POPUP_A_OPPICATERTRAINER:
			InitPopupRogueTrainer( &pmAddMenu ) ;

			if ( (pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Rogue && m_dwKindPopup == POPUP_ROGUETRAINER) ||
				(pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Officiator && m_dwKindPopup == POPUP_A_OPPICATERTRAINER) )
			{
				pmAddMenu.m_dwPopupKind = POPUPMENU_JOBCHANGE;
				strcpy(pmAddMenu.m_strPopupStr, CRYLStringTable::m_strString[9]);
				m_lstPopupMenu.push_back(pmAddMenu);
			}
			break;

		case POPUP_MAGETRAINER:
			InitPopupMageTrainer( &pmAddMenu ) ;
			break;

		case POPUP_ACOLYTETRAINER:
			InitPopupAcolyteTrainer( &pmAddMenu ) ;
			break;

		case POPUP_GUILDMAKER :
		case POPUP_A_GUILDMAKER :
			InitPopupGuildMaker( &pmAddMenu ) ;
			break ;

		case POPUP_TELEPORTER:
		case POPUP_A_TELEPORTER:
			InitPopupTeleporter( &pmAddMenu, dwUID ) ;
			break;

		case POPUP_CONTAINER:
		case POPUP_A_CONTAINER:
			InitPopupContainer( &pmAddMenu ) ;
			break;

		case POPUP_PEACEGUILD:
		case POPUP_A_PEACEGUILD:
			InitPopupPeaceGuild( &pmAddMenu ) ;
			break;

		case POPUP_STATUSCLEANER:
		case POPUP_A_STATUSCLEANER:
			InitPopupStatuscleaner( &pmAddMenu ) ;
			break;

		case POPUP_CLASSCLEANER:
		case POPUP_A_CLASSCLEANER:
			InitPopupClasscleaner( &pmAddMenu ) ;
			break;

		case POPUP_COMPENSATION:
			InitPopupCompensation( &pmAddMenu ) ;
			break;

		case POPUP_SKILLMERCHANT:
			InitPopupSkillMerchant( &pmAddMenu ) ;
			break;

		case POPUP_BATTLEGROUND_MANAGER:
		case POPUP_A_BATTLEGROUND_MANAGER:
			InitPopupBattleGroundManager( &pmAddMenu ) ;
			break;

		case POPUP_MONOEY_CHANGER:
		case POPUP_A_MONOEY_CHANGER:
			InitPopupMoneyChanger( &pmAddMenu ) ;
			break;

		case POPUP_BATTLEGROUND_EXITER:
		case POPUP_A_BATTLEGROUND_EXITER:
			InitPopupBattleGroundExiter( &pmAddMenu ) ;
			break;

		case POPUP_CHANGENAME:			// 이름 변경
			InitPopupChangeName( &pmAddMenu );
			break;
#endif
		// ---------------------------------------------------
		// 공성관련 POPUP 메뉴

		case POPUP_EMBLEM :		
			InitPopupEmblem( &pmAddMenu ) ;
			break ;

		case POPUP_GATE	:			
			InitPopupGate( &pmAddMenu ) ;		
			break ;

		case POPUP_BACKDOOR	:			
			InitPopupBackDoor( &pmAddMenu ) ;
			break ;

		case POPUP_CAMP	:		
			InitPopupCamp( &pmAddMenu ) ;
			break ;

		case POPUP_CAMP_SHOP :
			InitPopupCampShop( &pmAddMenu ) ;
			break;

		case POPUP_WORLDWEAPON:
			InitPopupWorldWeapon( &pmAddMenu );
			break;

		case POPUP_MINING_CAMP :
			InitPopupCampMining( &pmAddMenu ) ;
			break;

		case POPUP_CASTLE_ARMS_NPC :			
			InitPopupCastleArmsNPC( &pmAddMenu ) ;
			break ;

		case POPUP_CASTLE_GUARD :	
			InitPopupCastleGuard( &pmAddMenu ) ;
			break ;

		case POPUP_SHORT_RANGE_CASTLE_ARMS :
			InitPopupShortRangeCastleArms( &pmAddMenu ) ;
			break ;

		case POPUP_LONG_RANGE_CASTLE_ARMS :
			InitPopupLongRangeCastleArms( &pmAddMenu ) ;
			break ;

		case POPUP_SHORT_RANGE_SIEGE_ARMS :
			InitPopupShortRangeSiegeArms( &pmAddMenu ) ;
			break ;

		case POPUP_LONG_RANGE_SIEGE_ARMS :
			InitPopupLongRangeSiegeArms( &pmAddMenu ) ;
			break ;

		case POPUP_AIRSHIP :
			InitPopupAirShip( &pmAddMenu ) ;
			break ;
		// ---------------------------------------------------

		default:
#ifdef __NEWNPCSCRIPT
			{
				unsigned long dwCreatureType = pGame->GetClientIdentity(dwUID);
				switch(pGame->GetClientIdentity(dwUID))
				{
					case Creature::CT_PC:
						InitPopupPC( &pmAddMenu ) ;
						break;

					case Creature::CT_NPC:
						{
							NPCNode *pNode = g_NPCList.GetNPCNode(dwUID);
							if (pNode)
							{
								vector<LPPopupNode>::iterator it;
								for(it = pNode->m_lstPopup.begin(); it != pNode->m_lstPopup.end(); ++it)
								{
									pmAddMenu.m_dwPopupKind = (*it)->m_dwPopupKind;
									strcpy(pmAddMenu.m_strPopupStr, (*it)->m_strPopupString.c_str());
			//						switch(pmAddMenu.m_dwPopupKind)	// 팝업에 따른 여러 예외처리
			//						{
			//						}
									m_lstPopupMenu.push_back(pmAddMenu);
								}
							}
						}
						break;

					default:
						m_dwKindPopup = 0;
						m_dwPopupSelectChrID = 0;
						m_bPopupShow = FALSE;
						m_dwKindPos = 0;

						return TRUE;
						break;
				}
			}
#else
			m_dwKindPopup = 0;
			m_dwPopupSelectChrID = 0;
			m_bPopupShow = FALSE;
			m_dwKindPos = 0;

			return TRUE;
#endif
			break;

	}

	InitPopupQuest( &pmAddMenu, dwUID ) ;

	m_dwPopupWindowLength = 15;
	for (unsigned long i = 0; i < m_lstPopupMenu.size(); ++i)
	{
		unsigned long lengthtemp = g_TextEdit.GetStringLength(m_lstPopupMenu[i].m_strPopupStr);
		if (m_dwPopupWindowLength < lengthtemp) 
		{
			m_dwPopupWindowLength = lengthtemp;
		}
	}

	InitPopupPosition() ;

	// Simple Mouse Mode 로 변경
	if ( pGame && !pGame->m_bMouseMode && m_bPopupShow )
	{
		pGame->m_bSimpleMouse = TRUE ;
	}

	return TRUE;
}

void CRYLRaceBase::InitPopupPosition(void)
{
	switch (m_dwKindPos)
	{
		case 0:
		{
			vector3 vecResult, vecChrPos;
			float fW;
			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
			if (pTempCreature)
			{
				vecChrPos = *( pTempCreature->GetPosition() );
				BaseGraphicsLayer::TransformVector(vecChrPos, vecResult, fW);
				if (vecResult.z > 1.0f)
				{
					m_bPopupShow = FALSE;
					return;
				}

				m_nPopupWindowX = vecResult.x + 50.0f - 0.5;
				m_nPopupWindowY = vecResult.y - 200.f;
			} 
			else
			{
				m_dwKindPopup = 0;
				m_dwPopupSelectChrID = 0;
				m_bPopupShow = FALSE;
				m_dwKindPos = 0;

				return;
			}
			break;
		}

		case 1:
		{
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			m_nPopupWindowX = ptMousePos->x;
			m_nPopupWindowY = ptMousePos->y;
			break;
		}
	}

	if ( m_nPopupWindowX + 32 + m_dwPopupWindowLength > static_cast<unsigned long>(BaseGraphicsLayer::m_lScreenSx) )  
	{ 
		m_nPopupWindowX -= (m_nPopupWindowX + 32 + m_dwPopupWindowLength) - BaseGraphicsLayer::m_lScreenSx; 
	}

	if (m_nPopupWindowY + 24 + m_lstPopupMenu.size() * 18 > static_cast<unsigned long>(BaseGraphicsLayer::m_lScreenSy)) 
	{ 
		m_nPopupWindowY -= (m_nPopupWindowY + 24 + m_lstPopupMenu.size() * 18) - BaseGraphicsLayer::m_lScreenSy; 
	}
}

BOOL CRYLRaceBase::SelectNPC(unsigned long dwTargetID, BOOL bButton)
{
	if (g_CharacterData.m_bCharacterTargetNear || 
		g_DeviceInput.GetIsLeftMouseUp() || 
		(!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && g_DeviceInput.KeyDown('E')))
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( !pSelfCreature )
			return FALSE;

		CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if ( !pSelfData )
			return FALSE;

		CRYLGameData*	pGame = CRYLGameData::Instance();
		if (g_CharacterData.m_bCharacterTargetNear)
		{
			g_CharacterData.m_bCharacterTargetNear = FALSE;
			g_CharacterData.m_dwNormalTargetID = 0xFFFFFFFF;
		}

		const unsigned long dwEnemyType = CRYLGameData::GetClientEnemyType( dwTargetID );
		if ( EnemyCheck::EC_ENEMY == dwEnemyType )
		{
			return FALSE;
		}

		switch ( CRYLGameData::GetClientIdentity( dwTargetID ) )
		{
			case Creature::CT_PC:
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );
				if ( !pTempCreature )
					break;

				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
				if ( pTempData )
				{
					if (g_DeviceInput.GetIsLeftMouseUp())
					{
						if (bButton)
						{
							if ( !pTempData->m_strStreetStallName.empty() )
							{
								if ( CRYLNetworkData::Instance()->m_dwMyChrID == pTempCreature->GetCharID() )
									return TRUE;

								CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
								pScene->GUIVisible( FALSE );

								CRYLNetworkData::SendStallLeave();
								SendPacket::CharStallEnter( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, pTempCreature->GetCharID() );
								return TRUE;
							}
						}

						if ( !g_DeviceInput.KeyHold( VK_SHIFT ) )
							break;
					}

					if ( g_DeviceInput.KeyDown( 'E' ) )
						break;

					if ( pTempCreature == pSelfCreature )
						break;

#ifdef __NEWNPCSCRIPT
					if ( InitPopup( dwTargetID, 0 ) )
#else
					if ( InitPopup( POPUP_PC, dwTargetID, 0 ) )
#endif
						return TRUE;
				}
				break;
			}

			case Creature::CT_NPC:
			case Creature::CT_STRUCT:
			{
				if ( EnemyCheck::EC_NEUTRAL == dwEnemyType )
				{
					return FALSE ;
				}

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );
				if ( !pTempCreature )
					break;

				// 10 m 안에서만 NPC 판넬을 클릭할 수 있다.
				vector3 vecTemp = *( pSelfCreature->GetPosition() ) - *( pTempCreature->GetPosition() ) ;
				if ( vecTemp.GetLens() > 1000 )
					break;
			
				NPCNode *pNode = g_NPCList.GetNPCNode( dwTargetID );
				if ( pNode )
				{
					static const unsigned long dwHUMAN = 0x00000FFF ;
					static const unsigned long dwAKHAN = 0x00FF0000 ;

					CClass::RaceType eRaceType = CClass::GetRace(pSelfData->m_usClass);
 
					// 이로직은 AddPopup 에 인간, 아칸이 따른 팝업으로 지정이 되어있으면 문제가 발생한다.
					vector<PopupNode*>::iterator iter = pNode->m_vecPopup.begin() ;
					while( iter != pNode->m_vecPopup.end() ) 
					{
						PopupNode* lpTmpNode = ( *iter ) ;
						
						if ( CClass::HUMAN == eRaceType )
						{
							if ( lpTmpNode->m_ulLimitClass == dwAKHAN )
							{
								dwTargetID = 0xFFFFFFFF ;
								break ;
							}
						}
						else if ( CClass::AKHAN == eRaceType )
						{
							if ( lpTmpNode->m_ulLimitClass == dwHUMAN )
							{
								dwTargetID = 0xFFFFFFFF ;
								break ;
							}
						}
						
						++ iter ;
					}

					//  렌홀드 성안에 NPC는 수성측에서만 사용할 수 있다.
					if(pNode->m_dwTownOrNameID==0x70000001)
					{
						CRYLCastle*	lpCastle = CRYLCastleManager::Instance()->GetCurrentCastle();

						if(lpCastle)
						{
							if(lpCastle->GetNation()!=CRYLGameData::Instance()->m_cAccountNation )
							{
								dwTargetID = 0xFFFFFFFF;
								break ;
							}							
						}						
					}

                    if ( InitNPCPopup( pNode, dwTargetID ) )
					{
						char strTemp[ MAX_PATH ];
						CRYLStringTable::EraseToken( strTemp, pNode->m_strNpcName, "\\" );
						sprintf(strTemp, "%s: %s", strTemp, pNode->GetGreeting());
						// edith 2009.04.26 NPC선택햇을때 메시지 출력 방지
						//CRYLChattingDlg::Instance()->AddChatMessage(strTemp, pNode->m_dwUID, 130);

						return TRUE;
					}
				}
				break;
			}

			case Creature::CT_SIEGE_OBJECT :
			{
/*				if ( EnemyCheck::EC_NEUTRAL == dwEnemyType )
				{
					return FALSE ;
				}
*/
				RYLCreature*    pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );
				CharDataNode*   pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwTargetID );

				if ( pTempCreature && pTempData )
				{
					if (g_DeviceInput.GetIsLeftMouseUp())
					{
						if (bButton)
						{
							if ( CRYLNetworkData::Instance()->m_dwMyChrID == pTempCreature->GetCharID() ) 
								return TRUE;
						}

						if ( !g_DeviceInput.KeyHold( VK_SHIFT ) ) 
							break;
					}

					if ( g_DeviceInput.KeyDown( 'E' ) )
						break;

					if ( pTempCreature == pSelfCreature ) 
						break;

					// 팝업 메뉴가 뜨지 않는 길드 요새 상태
					if (Siege::CANCELING == pTempCreature->GetState() || 
						Siege::DESTROYING == pTempCreature->GetState() || 
						Siege::CHANGING == pTempCreature->GetState() ||
						Siege::UPGRADING == pTempCreature->GetState() ||
						Siege::REPARING == pTempCreature->GetState())
					{
						break;
					}

					if (Siege::KARTERANT_WEAPON == pTempCreature->GetObjectType() ||
						Siege::MERKADIA_WEAPON == pTempCreature->GetObjectType())
					{
						if (Siege::COMPLETE == pTempCreature->GetState() &&
							(Siege::WEAPON_CHARGE == pTempCreature->GetSubState() ||
							Siege::WEAPON_FIRE == pTempCreature->GetSubState()))
						{
							break;
						}
					}

					unsigned long dwPopupKind = 0;
					switch ( pTempCreature->GetObjectType() )
					{
						case Siege::EMBLEM:
						{
							if ( Siege::DEVELOPING != pTempCreature->GetState() )
							{
								dwPopupKind = POPUP_EMBLEM;
							}
							break ;
						}

						case Siege::GATE:						dwPopupKind = POPUP_GATE;						break;
						case Siege::BACKDOOR:					dwPopupKind = POPUP_BACKDOOR;					break;

						case Siege::CAMP:
						case Siege::MINING_CAMP :
						case Siege::CAMP_SHOP :
						case Siege::KARTERANT_WEAPON :
						case Siege::MERKADIA_WEAPON :
						{
							CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
							CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

							if (pSelfData->m_ulGuildID != pTempData->m_ulGuildID && 
								pTempCreature->GetObjectType() == Siege::CAMP_SHOP)
							{
								if (pTempCreature->GetState() != Siege::CHANGING)
								{
									// 타 길드원의 길드 요새 상점 이용 (팝업 메뉴 없이 다이얼로그가 뜬다.)
									pScene->GUIVisible( FALSE );

									m_dwPopupSelectChrID = pTempCreature->GetCharID();
									CRYLNetworkData::SendStallEnter(m_dwPopupSelectChrID);
								}

								return TRUE;
							}

							if ( ( pSelfData->m_ulGuildID == pTempData->m_ulGuildID ) &&
								( pASDlg->GetTitle() == Guild::MASTER ) && 
								( pTempCreature->GetState() == Siege::DEVELOPING ) )
							{
								CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
								CRYLCampCreatingDlg* pCampDlg = ( CRYLCampCreatingDlg* )pScene->GetCampCreatingDlg() ;

								RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID ) ;
								if ( pTempCreature )
								{
									CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
									if ( pCamp )
									{
										pCampDlg->SetCampID( pCamp->GetCampID() ) ;
										pCampDlg->SetVisible( TRUE ) ;
									}
								}

								return TRUE ;
							}

							if ( pTempCreature->GetState() != Siege::CANCELING )
							{
								switch( pTempCreature->GetObjectType() )
								{
									case Siege::MINING_CAMP :		dwPopupKind = POPUP_MINING_CAMP;	break;
									case Siege::CAMP_SHOP :			dwPopupKind = POPUP_CAMP_SHOP;		break;
									case Siege::CAMP :				dwPopupKind = POPUP_CAMP;			break;
									case Siege::KARTERANT_WEAPON :	dwPopupKind = POPUP_WORLDWEAPON;	break;
									case Siege::MERKADIA_WEAPON :	dwPopupKind = POPUP_WORLDWEAPON;	break;

									default:						dwPopupKind = POPUP_CAMP;			break;
								}
							}
							break ;
						}

						case Siege::CASTLE_ARMS_NPC:			dwPopupKind = POPUP_CASTLE_ARMS_NPC;			break;
						case Siege::GUARD:						dwPopupKind = POPUP_CASTLE_GUARD;				break;
						case Siege::SHORT_RANGE_CASTLE_ARMS:	dwPopupKind = POPUP_SHORT_RANGE_CASTLE_ARMS;	break;
						case Siege::LONG_RANGE_CASTLE_ARMS:		dwPopupKind = POPUP_LONG_RANGE_CASTLE_ARMS;		break;

						case Siege::SHORT_RANGE_SIEGE_ARMS:
						{
							if ( pSelfCreature->GetCharID() == pTempCreature->GetOwnerID() )
							{
								if ( pTempCreature->GetState() == Siege::DEVELOPING )
								{
									CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
									pScene->GetSiegeArmsCreatingDlg()->SetVisible( TRUE ) ;
									m_dwSiegeArmsSelectID = dwTargetID ;
								}
								else
								{
									dwPopupKind = POPUP_SHORT_RANGE_SIEGE_ARMS;
								}
							}
							break ;
						}

						case Siege::LONG_RANGE_SIEGE_ARMS:	
						{
							if ( pSelfCreature->GetCharID() == pTempCreature->GetOwnerID() )
							{
								if ( pTempCreature->GetState() == Siege::DEVELOPING )
								{
									CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
									pScene->GetSiegeArmsCreatingDlg()->SetVisible( TRUE ) ;
									m_dwSiegeArmsSelectID = dwTargetID ;
								}
								else
								{
									dwPopupKind = POPUP_LONG_RANGE_SIEGE_ARMS;
								}
							}
							break ;
						}

						case Siege::AIRSHIP:					
						{
							if ( pTempCreature->GetState() == Siege::DEVELOPING )
							{
								if ( pSelfCreature->GetCharID() == pTempCreature->GetOwnerID() )
								{
									CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
									pScene->GetSiegeArmsCreatingDlg()->SetVisible( TRUE ) ;
									m_dwSiegeArmsSelectID = dwTargetID ;
								}
							}
							else
							{
								dwPopupKind = POPUP_AIRSHIP;
							}
							break ;
						}
					}
					
					if ( 0 != dwPopupKind)
					{
						if ( CheckUsableSiegePopup( pTempCreature->GetObjectType(), 
													pSelfCreature->GetCharID(), pTempCreature->GetOwnerID(),
													pSelfData->m_ulGuildID,	pTempData->m_ulGuildID,
													pSelfData->m_cNation, pTempData->m_cNation ) )
						{
#ifdef __NEWNPCSCRIPT
							if ( InitPopup( dwTargetID, 0, dwPopupKind ) ) 
#else
							if ( InitPopup( dwPopupKind, dwTargetID, 0 ) ) 
#endif
							{
								CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
								CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
								CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

								PopupMenu*					pMenu	= NULL;
								CampRight*					pCampRight	= NULL;
								CastleRight*				pCastleRight = NULL;
								CRYLCamp*					lpCamp = NULL;
								CRYLCastle*					lpCastle = NULL;

								switch ( pTempCreature->GetObjectType() )
								{
									case Siege::CAMP:
									case Siege::MINING_CAMP:
									case Siege::CAMP_SHOP:
									case Siege::KARTERANT_WEAPON:
									case Siege::MERKADIA_WEAPON:
									{
										lpCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
										if ( lpCamp )
										{
											pCampRight = lpCamp->GetCampRight() ;
										}
									}
									break;

									case Siege::EMBLEM:
									case Siege::GATE:
									case Siege::BACKDOOR:
									case Siege::CASTLE_ARMS_NPC:
									case Siege::GUARD:
									case Siege::SHORT_RANGE_CASTLE_ARMS:
									case Siege::LONG_RANGE_CASTLE_ARMS:
									{
										lpCastle = CRYLCastleManager::Instance()->FindCastle( pTempCreature->GetCastleID() ) ;
										if ( lpCastle )
										{
											pCastleRight = lpCastle->GetCastleRight() ;
										}
									}
									break;

									case Siege::SHORT_RANGE_SIEGE_ARMS:
									case Siege::LONG_RANGE_SIEGE_ARMS:
									case Siege::AIRSHIP:
									{
									}
									break;
								}


								switch ( pTempCreature->GetObjectType() )
								{
									case Siege::CAMP:
									{
										if ( Siege::MAX_UPGRADE_NUM == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPUPGRADE);
										}

										if ( 0 == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind(POPUPMENU_CHANGESHOP, POPUPMENU_CHANGESHOP);
											DisablePopupMenuKind(POPUPMENU_CHANGEMINING, POPUPMENU_CHANGEMINING);
										}

										CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
										if ( pTempData && pTempData->m_lCurrHP == pTempData->m_ulMaxHP )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPREPAIR);
										}

										if ( pASDlg->GetTitle() != Guild::MASTER )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPRIGHTSETTING, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPDESTROY, POPUPMENU_CAMPNORIGHTMASTER);

											if (NULL == pCampRight || 0 == pASDlg->GetTitle() ||
												pASDlg->GetTitle() > pCampRight->m_aryCampRight[ CampRight::CAMP_CONTROL ])
											{
												DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGESHOP, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGEMINING, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPNORIGHTMASTER);
											}
										}
										break;
									}

									case Siege::MINING_CAMP :
									{
										if ( Siege::MAX_UPGRADE_NUM == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPUPGRADE);
										}

										CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
										if ( pTempData && pTempData->m_lCurrHP == pTempData->m_ulMaxHP )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPREPAIR);
										}

										if ( pASDlg->GetTitle() != Guild::MASTER )
										{
											DisablePopupMenuKind(POPUPMENU_MININGCAMPMENU, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPRIGHTSETTING, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPDESTROY, POPUPMENU_CAMPNORIGHTMASTER);

											if (NULL == pCampRight || 0 == pASDlg->GetTitle() ||
												pASDlg->GetTitle() > pCampRight->m_aryCampRight[ CampRight::CAMP_CONTROL ])
											{
												DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGEBASE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGESHOP, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPNORIGHTMASTER);
											}
										}
										break;
									}

									case Siege::CAMP_SHOP :
									{
										if ( Siege::MAX_UPGRADE_NUM == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPUPGRADE);
										}

										CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
										if ( pTempData && pTempData->m_lCurrHP == pTempData->m_ulMaxHP )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPREPAIR);
										}

										if ( pASDlg->GetTitle() != Guild::MASTER )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPSHOPADMIN, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPRIGHTSETTING, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPDESTROY, POPUPMENU_CAMPNORIGHTMASTER);

											if (NULL == pCampRight || 0 == pASDlg->GetTitle() ||
												pASDlg->GetTitle() > pCampRight->m_aryCampRight[ CampRight::CAMP_CONTROL ])
											{
												DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGEBASE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CHANGEMINING, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPNORIGHTMASTER);
											}
										}
										break;
									}

									case Siege::KARTERANT_WEAPON :
									case Siege::MERKADIA_WEAPON :
									{
										if ( Siege::MAX_UPGRADE_NUM == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPUPGRADE);
										}

										CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
										if ( pTempData && pTempData->m_lCurrHP == pTempData->m_ulMaxHP )
										{
											DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPREPAIR);
										}

										if ( pASDlg->GetTitle() != Guild::MASTER )
										{
											DisablePopupMenuKind(POPUPMENU_WEAPONLOAD, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_WEAPONLAUNCH, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPRIGHTSETTING, POPUPMENU_CAMPNORIGHTMASTER);
											DisablePopupMenuKind(POPUPMENU_CAMPDESTROY, POPUPMENU_CAMPNORIGHTMASTER);

											if (NULL == pCampRight || 0 == pASDlg->GetTitle() ||
												pASDlg->GetTitle() > pCampRight->m_aryCampRight[ CampRight::CAMP_CONTROL ])
											{
												DisablePopupMenuKind(POPUPMENU_CAMPUPGRADE, POPUPMENU_CAMPNORIGHTMASTER);
												DisablePopupMenuKind(POPUPMENU_CAMPREPAIR, POPUPMENU_CAMPNORIGHTMASTER);
											}
										}
										else		
										{
											// 길드마스터 일 경우
											if( !pTempCreature )
												break;

											switch( pTempCreature->GetSubState() )
											{
												case Siege::WEAPON_EMPTY :
												{
													DisablePopupMenuKind(POPUPMENU_WEAPONLAUNCH, POPUPMENU_WEAPONLAUNCH);
													break;
												}

												case Siege::WEAPON_CHARGE :
												{
													DisablePopupMenuKind(POPUPMENU_WEAPONLOAD, POPUPMENU_WEAPONLOAD);
													DisablePopupMenuKind(POPUPMENU_WEAPONLAUNCH, POPUPMENU_WEAPONLAUNCH);
													break;
												}

												case Siege::WEAPON_READY :
												{
													DisablePopupMenuKind(POPUPMENU_WEAPONLOAD, POPUPMENU_WEAPONLOAD);
													break;
												}

												case Siege::WEAPON_FIRE :
												{
													DisablePopupMenuKind(POPUPMENU_WEAPONLOAD, POPUPMENU_WEAPONLOAD);
													DisablePopupMenuKind(POPUPMENU_WEAPONLAUNCH, POPUPMENU_WEAPONLAUNCH);
													break;
												}
											
											}
										}
										break;
									}

									case Siege::EMBLEM :
									{
										if ( Siege::MAX_EMBLEM_UPGRADE == pTempCreature->GetUpgradeStep() )
										{
											DisablePopupMenuKind( POPUPMENU_CASTLEEMBLEMUPGRADE, POPUPMENU_CASTLEEMBLEMUPGRADE ) ;
										}

										if ( pASDlg->GetTitle() != Guild::MASTER )
										{
											DisablePopupMenuKind( POPUPMENU_CASTLERIGHTSETTING, POPUPMENU_CASTLENORIGHTMASTER );

											if (NULL == pCastleRight || 0 == pASDlg->GetTitle() ||
												pASDlg->GetTitle() > pCastleRight->m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ])
											{
												DisablePopupMenuKind( POPUPMENU_CASTLEEMBLEMUPGRADE, POPUPMENU_CASTLENORIGHTADMIN ) ;
											}
										}

										break;
									}
								}
							
								return TRUE ;
							}
						}
					}
					break ;
				}
			}
		}
	}

	return FALSE ;
}

BOOL CRYLRaceBase::UpdatePopup(void)
{
	CRYLGameScene*				pScene              = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGameData*				pGame               = CRYLGameData::Instance() ;
	CRYLStatusAndQuestFrameDlg* pDlg				= ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
	CRYLCompleteQuestPanel*		pCompleteQuestPanel	= ( CRYLCompleteQuestPanel* )pDlg->GetCompleteQuestPanel() ;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return false;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return false;

	switch (m_dwKindPos)
	{
		case 0:
		{
			vector3 vecResult, vecChrPos;
			float fW;

			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
			if ( !pTempCreature )
			{
				m_bPopupShow = FALSE;
				m_dwPopupSelect = 0xFFFFFFFF;

				return TRUE;
			}

			//vecChrPos = *( pTempCreature->GetNamePosition() );
			vecChrPos = *( pTempCreature->GetPosition() );
			BaseGraphicsLayer::TransformVector(vecChrPos, vecResult, fW);
			if (vecResult.z > 1.0f)
			{
				m_bPopupShow = FALSE;
				return FALSE;
			}

			m_nPopupWindowX = vecResult.x + 50.0f - 0.5;
			m_nPopupWindowY = vecResult.y - 200.f;
			break;
		}

		case 1:
		{
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			m_nPopupWindowX = ptMousePos->x;
			m_nPopupWindowY = ptMousePos->y;
			break;
		}
	}

	m_dwPopupSelect = 0xFFFFFFFF;
	for (unsigned long i = 0; i < m_lstPopupMenu.size(); i++)
	{
		if (g_DeviceInput.InRect(m_nPopupWindowX + 10, m_nPopupWindowY + 13 + i * 18, 
			m_nPopupWindowX + 20 + m_dwPopupWindowLength, m_nPopupWindowY + 31 + i * 18))
		{
			m_dwPopupSelect = i;
			if (g_DeviceInput.GetIsLeftMouseUp())
			{
#ifdef __NEWNPCSCRIPT
				// 에러 메세지
				if (m_lstPopupMenu[m_dwPopupSelect].m_dwLimitLevel)
				{
					if ( pSelfData->m_cLevel < m_lstPopupMenu[m_dwPopupSelect].m_dwLimitLevel )
					{
						// 레벨이 적고 퀘스트 트리거 일때 에러 메세지창보다 우아하게 퀘스트 스크립트의 에러메세지를 읽어온다.
						// 그러나 소스는 우아하지 않다. ㅡ,.ㅡ;;;
						if (m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind == POPUPMENU_QUESTTRIGGER)
						{
							NPCNode *pNode = g_NPCList.GetNPCNode( m_dwPopupSelectChrID );
							if ( pNode )
							{
								LPQuestInstance lpQuest = pGame->m_csStatus.GetProcessQuest(m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID);
								if (lpQuest)
								{
									unsigned long dwTriggerCount = 0;
									TriggerNode* pTrigger = pGame->m_csStatus.GetTriggertoLevelTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger && !pTrigger->m_strElseMsg.empty())
									{
										CRYLMessageBox* pMessageBox = new CRYLMessageBox;
										pMessageBox->Create(pTrigger->m_strElseMsg.c_str());
									}
									continue;
								}
							}
						}

						CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
						char strMessage[MAX_PATH];
						sprintf(strMessage, CRYLStringTable::m_strString[61], m_lstPopupMenu[m_dwPopupSelect].m_dwLimitLevel);
						lpMessageBox->Create(strMessage);
						continue;
					}
				}

				// 에러 메세지
				if (m_lstPopupMenu[m_dwPopupSelect].m_dwLimitFame)
				{
					if ( pSelfData->m_ulFame < m_lstPopupMenu[m_dwPopupSelect].m_dwLimitFame )
					{
						// 레벨이 적고 퀘스트 트리거 일때 에러 메세지창보다 우아하게 퀘스트 스크립트의 에러메세지를 읽어온다.
						// 그러나 소스는 우아하지 않다. ㅡ,.ㅡ;;;
						if (m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind == POPUPMENU_QUESTTRIGGER)
						{
							NPCNode *pNode = g_NPCList.GetNPCNode( m_dwPopupSelectChrID );
							if ( pNode )
							{
								LPQuestInstance lpQuest = pGame->m_csStatus.GetProcessQuest(m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID);
								if (lpQuest)
								{
									unsigned long dwTriggerCount = 0;
									TriggerNode* pTrigger = pGame->m_csStatus.GetTriggertoFameTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger && !pTrigger->m_strElseMsg.empty())
									{
										CRYLMessageBox* pMessageBox = new CRYLMessageBox;
										pMessageBox->Create(pTrigger->m_strElseMsg.c_str());
									}
									continue;
								}
							}
						}

						CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
						char strMessage[MAX_PATH];
						sprintf(strMessage, CRYLStringTable::m_strString[61], m_lstPopupMenu[m_dwPopupSelect].m_dwLimitFame);
						lpMessageBox->Create(strMessage);
						continue;
					}
				}

				if (m_lstPopupMenu[m_dwPopupSelect].m_dwLimitQuest)
				{
					if ( !pCompleteQuestPanel->GetIsCompleteQuest( m_lstPopupMenu[m_dwPopupSelect].m_dwLimitQuest ) )
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox;
						QuestNode* pCompletedQuest = g_QuestList.GetQuest(m_lstPopupMenu[m_dwPopupSelect].m_dwLimitQuest);
						if (pCompletedQuest)
						{
							char strMessage[MAX_PATH];
							sprintf(strMessage, CRYLStringTable::m_strString[62], pCompletedQuest->m_strQuestTitle);
							pMessageBox->Create(strMessage);
						} 
						else
						{
							pMessageBox->Create(CRYLStringTable::m_strString[960]);
						}
						continue;
					}
				}
#endif
				switch (m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind)
				{
					case POPUPMENU_EXCHANGE:
					{
						pScene->GUIVisible( FALSE );
						if ( !GRYLCommunityDataInstance()->GetIsRefuse( m_dwPopupSelectChrID ) )
						{
							CRYLNetworkData::SendExchangeCmd( m_dwPopupSelectChrID, ClientConstants::ExchangeCmd_Propose );
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[16], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						} 
						else
						{
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;
					}

					case POPUPMENU_DUEL:
					{
						if (!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							m_dwDuelCmdResult = 0;
							m_dwDuelCmdType = 100;
							m_dwDuelCmdSenderID = m_dwPopupSelectChrID;
							char strText[200];

							CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( m_dwPopupSelectChrID );
							if ( pTempData )
							{
/*
								// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
								if (0 != pTempData->m_cNation && pGame->m_cAccountNation != pTempData->m_cNation &&
									!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
								{
									sprintf( strText, CRYLStringTable::m_strString[17], pGame->GetClassName( pTempData->m_usClass ),
											 pTempData->m_cLevel, pGame->GetClassName( pTempData->m_usClass ) );
								}
								else
								{
*/
									sprintf( strText, CRYLStringTable::m_strString[17], pTempData->m_strName.c_str(),
											 pTempData->m_cLevel, pGame->GetClassName( pTempData->m_usClass ) );
//								}

								CRYLMessageBox* pMessageBox = new CRYLMessageBox;
								pMessageBox->Create(strText, MB_EXIT | MB_YES | MB_NO);
								pMessageBox->SetResult(&m_dwDuelCmdResult);
							}
						} 
						else
						{
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;
					}

					case POPUPMENU_PARTYINVATE:
					{
						if (!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							CRYLNetworkData::SendPartyCmd(m_dwPopupSelectChrID, ClientConstants::PartyCmd_Invite);
							CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[10], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						} 
						else
						{
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;
					}

					case POPUPMENU_STREETSTALL:
						pScene->GUIVisible( FALSE );
						CRYLNetworkData::SendStallEnter(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_LEADER:
						SendPacket::CharPartyCmd(g_GameSession, m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID,
							pGame->m_csStatus.m_PartyInfo.m_dwPartyID, ClientConstants::PartyCmd_Transfer);

						CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[11], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						break;

					case POPUPMENU_VANISH:
						SendPacket::CharPartyCmd(g_GameSession, m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID,
							pGame->m_csStatus.m_PartyInfo.m_dwPartyID, ClientConstants::PartyCmd_Banish);

						CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[12], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						break;

					case POPUPMENU_DIALOG:
					{
						// Dialog에서는 Quest 아이디를 쓰지 않으니까 임시로 이 변수에다가 Page값을 저장하여 둔다.
						unsigned long unDialogPage = m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID;

						CRYLMessageBox *pMessageBox = new CRYLMessageBox;
						if ( !pMessageBox->ShowDialog( m_dwPopupSelectChrID, unDialogPage ) )
						{
							g_MessageBoxManager.DestroyList();
						}
						break;
					}

					case POPUPMENU_SAVEPOS:
					{
						POS pPos;
						pPos.fPointX = pSelfCreature->GetPosition()->x / 100.0f;
						pPos.fPointY = pSelfCreature->GetPosition()->y / 100.0f;
						pPos.fPointZ = pSelfCreature->GetPosition()->z / 100.0f;
						CRYLNetworkData::Instance()->SetNetworkFlag( NF_BINDPOS ) ; 
						SendPacket::CharBindPos(g_GameSession, m_dwPopupSelectChrID, 0, &pPos, 0);
						break;
					}

                    case POPUPMENU_TRADE:
                        {   
                            pScene->GUIVisible( FALSE ); // Minotaurs 상점 창 띄우기
#ifdef __NEWNPCSCRIPT
							LPTradeJobNode lpJobNode = static_cast<LPTradeJobNode>(m_lstPopupMenu[m_dwPopupSelect].m_lpJobNode);
							if (lpJobNode) InitTrade( m_dwPopupSelectChrID, lpJobNode->m_dwContainerID );
#else
                            // TradeDlg에서는 Quest 아이디를 쓰지 않으니까 임시로 이 변수에다가 순서값을 저장하여 둔다.
                            unsigned long unDialogOrder = m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID;

						    InitTrade( m_dwPopupSelectChrID, unDialogOrder, m_lstPopupMenu[m_dwPopupSelect].m_usCapacity );
#endif
                        }
                        break;

					/*case POPUPMENU_TRADE0:
						pScene->GUIVisible( FALSE );
						InitTrade( m_dwKindPopup, m_dwPopupSelectChrID, 2 );
						break;

					case POPUPMENU_TRADE1:
						pScene->GUIVisible( FALSE );
						InitTrade( m_dwKindPopup, m_dwPopupSelectChrID, 0 );
						break;

					case POPUPMENU_TRADE2:
						pScene->GUIVisible( FALSE );
						InitTrade( m_dwKindPopup, m_dwPopupSelectChrID, 1 );
						break;*/

					case POPUPMENU_UPGRADE:
						pScene->GUIVisible( FALSE );
						InitItemUpgrade(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_OPTIONCOMPOSITION:
						pScene->GUIVisible( FALSE ) ;
						InitItemComposition( m_dwPopupSelectChrID );

					case POPUPMENU_UNLOCK:
						break;

					case POPUPMENU_STATUSRETRAIN:
						InitStatusRetrain(CRYLStatusRetrainDlg::DLG_RETRAIN);
						break;

					case POPUPMENU_JOBCHANGE:
						InitClassChange(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_ZONEMOVE:
					{
						NPCNode* pNode = g_NPCList.GetNPCNode(m_dwPopupSelectChrID);
						if (pNode)
						{
							// 컨텐츠 : 다크 카나번 국가 전쟁
							if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
							{
								if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
								{								
									using namespace GameTime;

									if (CRYLGameTimeManager::Instance()->IsRealmWarReadyTime() ||
										CRYLGameTimeManager::Instance()->IsRealmWarTime())
									{
										// 타크 카나번에서 국가전쟁 시간일 경우(국가전쟁 귀속).
										if (pSelfCreature->GetRealmWarFlag() != Creature::WAR_OFF)
										{
											CRYLMessageBox* pMessageBox = new CRYLMessageBox;									
											pMessageBox->Create(CRYLStringTable::m_strString[3822]);
											break;
										}
									}								
								}
							}


							if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE))
							{
								if(CRYLNetworkData::Instance()->m_dwPresentZone==SERVER_ID::CAPITAL)
								{
									if (CRYLGameTimeManager::Instance()->IsGuildWarTime())
									{
										if(pSelfCreature->GetGuildWarFlag() != Creature::WAR_OFF)
										{
											CRYLMessageBox* pMessageBox = new CRYLMessageBox;									
											pMessageBox->Create(CRYLStringTable::m_strString[3824]);
											break;
										}
									}
								}
							}

#ifdef __NEWNPCSCRIPT
							LPWarpJobNode lpJobNode = static_cast<LPWarpJobNode>(m_lstPopupMenu[m_dwPopupSelect].m_lpJobNode);
							if (lpJobNode)
							{
								if (pSelfData->m_bPeaceMode &&
									((pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN && lpJobNode->m_dwZoneNumber == SERVER_ID::ZONE5) ||
									(pGame->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN && lpJobNode->m_dwZoneNumber == SERVER_ID::ZONE4) ))
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox;
									pMessageBox->SetWordBreak(TRUE);
									pMessageBox->Create(CRYLStringTable::m_strString[807]);
								} 
								else if ((pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN && lpJobNode->m_dwZoneNumber == SERVER_ID::ZONE2) ||
									(pGame->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN && lpJobNode->m_dwZoneNumber == SERVER_ID::ZONE1))
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox;
									pMessageBox->Create(CRYLStringTable::m_strString[801]);
								} 
								else
								{
									// 컨텐츠 : 다크 카나번 국가 전쟁
									if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
									{
										if (CRYLGameTimeManager::Instance()->IsRealmWarReadyTime() || CRYLGameTimeManager::Instance()->IsRealmWarTime())
										{
											if((/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= lpJobNode->m_dwZoneNumber && lpJobNode->m_dwZoneNumber <= SERVER_ID::STONE_WAR3) && pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
											{
												CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
												lpMessageBox->SetWordBreak(true);
												lpMessageBox->Create(CRYLStringTable::m_strString[3581], MB_YES | MB_NO | MB_EXIT);
												lpMessageBox->SetResult( &pGame->m_dwRealmWarJoinResult );
												pGame->m_dwRealmWarJoinResult = 0;
												break;
											}
										}
									}

									POS pSpawnPoint;
									pSpawnPoint.fPointX = lpJobNode->m_fPosX;
									pSpawnPoint.fPointY = lpJobNode->m_fPosY;
									pSpawnPoint.fPointZ = lpJobNode->m_fPosZ;
									SendPacket::CharMoveZone(g_GameSession, lpJobNode->m_dwZoneNumber, -1, pSpawnPoint);
								}
							}
#else
							WarpNode* pZone = pNode->GetWarpNode(0);
							if (pZone)
							{
								if (pSelfData->m_bPeaceMode &&
									((pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN && pZone->m_wZoneNumber == SERVER_ID::ZONE5) ||
									(pGame->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN && pZone->m_wZoneNumber == SERVER_ID::ZONE4) ))
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox;
									pMessageBox->SetWordBreak(TRUE);
									pMessageBox->Create(CRYLStringTable::m_strString[807]);
								} 
								/*else if ((pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN && pZone->m_wZoneNumber == SERVER_ID::ZONE2) ||
									(pGame->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN && pZone->m_wZoneNumber == SERVER_ID::ZONE1))
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox;
									pMessageBox->Create(CRYLStringTable::m_strString[801]);
								} *///wasnt commented out zone move fix
								else
								{
									// 컨텐츠 : 다크 카나번 국가 전쟁
									if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
									{
										if (CRYLGameTimeManager::Instance()->IsRealmWarReadyTime() || CRYLGameTimeManager::Instance()->IsRealmWarTime())
										{						
											if((/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= pZone->m_wZoneNumber && pZone->m_wZoneNumber <= SERVER_ID::STONE_WAR3) && pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
											{
												CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
												lpMessageBox->SetWordBreak(true);
												lpMessageBox->Create(CRYLStringTable::m_strString[3581], MB_YES | MB_NO | MB_EXIT);
												lpMessageBox->SetResult( &pGame->m_dwRealmWarJoinResult );
												pGame->m_dwRealmWarJoinResult = 0;
												break;
											}
										}
									}

									POS pSpawnPoint;
									pSpawnPoint.fPointX = pZone->m_fPosX;
									pSpawnPoint.fPointY = pZone->m_fPosY;
									pSpawnPoint.fPointZ = pZone->m_fPosZ;
									SendPacket::CharMoveZone(g_GameSession, pZone->m_wZoneNumber, -1, pSpawnPoint);
								}
							}
#endif
						}
						break;
					}

					case POPUPMENU_DIALOG2:
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox;
						if (!pMessageBox->ShowDialog(m_dwPopupSelectChrID, 2))
						{
							g_MessageBoxManager.DestroyList();
						}
						break;
					}

					case POPUPMENU_DEPOSIT:
						InitDeposit();
						break;

					case POPUPMENU_PARTYDUEL:
						if (!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
								m_dwPopupSelectChrID, ClientConstants::DuelCmd_RequestPartyInfo);
						} 
						else
						{
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;

					case POPUPMENU_QUEST:
						// 퀘스트 창이 뜬다.
						InitQuest(m_dwPopupSelectChrID, m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID);
						break;

					case POPUPMENU_QUESTTRIGGER:
						{
							NPCNode *pNode = g_NPCList.GetNPCNode( m_dwPopupSelectChrID );
							if ( pNode )
							{
								LPQuestInstance lpQuest = pGame->m_csStatus.GetProcessQuest(m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID);
								if (lpQuest)
								{
									unsigned long dwTriggerCount = 0;
									TriggerNode* pTrigger = pGame->m_csStatus.GetTriggertoTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger)
									{
										SendPacket::CharOperateTrigger( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
											(unsigned char)TRIGGER_TALK, (unsigned char)dwTriggerCount,
											(unsigned char)lpQuest->m_lstTriggerCount[ dwTriggerCount ] );
										break;
									}

									pTrigger = pGame->m_csStatus.GetTriggertoLevelTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger)
									{
										SendPacket::CharOperateTrigger( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
											(unsigned char)TRIGGER_LEVEL_TALK, (unsigned char)dwTriggerCount,
											(unsigned char)lpQuest->m_lstTriggerCount[ dwTriggerCount ] );
										break;
									}

									pTrigger = pGame->m_csStatus.GetTriggertoFameTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger)
									{
										SendPacket::CharOperateTrigger( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
											(unsigned char)TRIGGER_FAME_TALK, (unsigned char)dwTriggerCount,
											(unsigned char)lpQuest->m_lstTriggerCount[ dwTriggerCount ] );
										break;
									}


									pTrigger = pGame->m_csStatus.GetTriggertoAbilityTalkInQuest(lpQuest, &dwTriggerCount, m_dwPopupSelectChrID);
									if (pTrigger)
									{
										SendPacket::CharOperateTrigger( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
											(unsigned char)TRIGGER_ABILITY_TALK, (unsigned char)dwTriggerCount,
											(unsigned char)lpQuest->m_lstTriggerCount[ dwTriggerCount ] );
										break;
									}

								}
							}
						}
						break;

					case POPUPMENU_FRIEND:
					{
						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( m_dwPopupSelectChrID );
						if ( pTempData )
						{
							SendPacket::CharFriendAddRequest(g_GameSession, 
								const_cast<char*>( pTempData->m_strName.c_str() ), PktFriendAddReq::ADD_FRIEND_REQ);
						}
						break;
					}

					case POPUPMENU_BAN:
					{
						pScene->GUIVisible( FALSE );

						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( m_dwPopupSelectChrID );
						if ( pTempData )
						{
							SendPacket::CharFriendAddRequest(g_GameSession, 
								const_cast<char*>( pTempData->m_strName.c_str() ), PktFriendAddReq::BAN_FRIEND_REQ);
						}
						break;
					}

					case POPUPMENU_PEACE:
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox;

						// 길드에 가입 되어 있으면 변경 불가!
						if ( pSelfData->m_ulGuildID )
						{
							CHAR	buff[256];
							
							CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
							CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
							GUILDLARGEINFOEX* pInfo = pPanel->FindGuild( pSelfData->m_ulGuildID );

							if ( pInfo )
								sprintf( buff, CRYLStringTable::m_strString[ 2720 ], pInfo->m_szName );
							else 
								sprintf( buff, CRYLStringTable::m_strString[ 2721 ] );

							pMessageBox->Create( buff );
						}
						else
						{
							pMessageBox->SetWordBreak(FALSE);

							if (pSelfData->m_bPeaceMode)
							{
								CRYLCommunityData::Instance()->m_dwPeaceMode = 0;
								pMessageBox->Create(CRYLStringTable::m_strString[808], MB_YES | MB_NO | MB_EXIT);
							} 
							else
							{
								CRYLCommunityData::Instance()->m_dwPeaceMode = 1;
								pMessageBox->Create(CRYLStringTable::m_strString[809], MB_YES | MB_NO | MB_EXIT);
							}

							pMessageBox->SetResult(&CRYLCommunityData::Instance()->m_dwPeaceModeResult);
						}
						break;
					}

					case POPUPMENU_FOLLOW:
					{
						g_CharacterData.m_bAutoFollowMode = TRUE;
						g_CharacterData.m_dwAutoFollowChrID = m_dwPopupSelectChrID;
						break;
					}

					case POPUPMENU_STATUSCLEAN:
					{
/*
						if (pGame->m_csStatus.m_Info.Chance <= 0)
						{
							CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[811], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						else
						{
							InitStatusRetrain(CRYLStatusRetrainDlg::DLG_REDSTRIBUTION);
						}
*/
						InitStatusRetrain(CRYLStatusRetrainDlg::DLG_REDSTRIBUTION);
						break;
					}

					case POPUPMENU_CLASSCLEAN:
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox;
						if (pGame->m_csStatus.m_Info.Class == 21)
						{
							pMessageBox->Create(CRYLStringTable::m_strString[812], MB_YES | MB_NO | MB_EXIT);
							pMessageBox->SetResult(&pGame->m_dwClassCleanResult);

							pGame->m_dwClassCleanResult = 0;
						} 
						else
						{
							pMessageBox->Create(CRYLStringTable::m_strString[813]);
						}
						break;
					}

					case POPUPMENU_COMPENSATION:
						InitCompensation();
						break;

					case POPUPMENU_BATTLEGROUND_MANAGE:
					{
						CRYLGameScene*		        pScene     = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						CRYLBattleGroundStatusDlg*  pStatusDlg = ( CRYLBattleGroundStatusDlg* )pScene->GetBattleGroundStatusDlg();
						pScene->GUIVisible( FALSE );

						SendPacket::CharBGServerMapList( g_GameSession );
						pStatusDlg->SetVisible( TRUE );
						break;
					}

					case POPUPMENU_BATTLEGROUNDPREV_RESULT:
					{
						CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						pScene->GUIVisible( FALSE );

						SendPacket::CharBGServerResultList( g_GameSession );
						break;
					}

					case POPUPMENU_MONEY_CHANGE:
					{
						CRYLGameScene*		            pScene          = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
						CRYLBattleGroundChangeMoneyDlg* pChangeMoneyDlg = (CRYLBattleGroundChangeMoneyDlg*)pScene->GetBattleGroundChangeMoney();

						pScene->GUIVisible( FALSE );
						pChangeMoneyDlg->SetVisible( TRUE );
						break;
					}

					case POPUPMENU_BATTLEGROUND_EXIT:
					{
						CRYLLargeMapFrameDlg* pLargeDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
						pLargeDlg->SetVisible( FALSE );

						SendPacket::CharBGServerMoveZone( g_GameSession, 0, 0 );
						break;
					}

					case POPUPMENU_CHANGENAME:
					{
						CRYLChangeNameDlg* pChangeNameDlg = ( CRYLChangeNameDlg* )pScene->GetChangeNameDlg();
						pChangeNameDlg->SetVisible( TRUE );
						break;
					}
					// --------------------------------------------------------------------------------------
					// 길드 관련

					case POPUPMENU_GUILDINVITE:
					{
						SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, 
							m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_INVITE );
						break;
					}

					case POPUPMENU_MAKEGUILD:
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
						pDlg->CheckGuildStipulation();
						break;
					}

					case POPUPMENU_GUILDKICK:
					{
						SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, 
							m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_KICK );
						break;
					}
					// --------------------------------------------------------------------------------------
					// 공성 관련

					// 성 상징물
					case POPUPMENU_TAXCONTROL :						// 판매 물건 세율 조정
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( pTempCreature->GetCastleID() );

							if ( pCastle )
							{
								CRYLGameScene* pScene = (CRYLGameScene* )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
                                CRYLCastleStatusFrameDlg* pDlg = (CRYLCastleStatusFrameDlg*)pScene->GetCastleStatusFrameDlg() ;

								if ( pDlg )
								{
									pDlg->SetCastleID( pCastle->GetCastleID() ) ;

									if ( !pDlg->GetVisible() )
										pDlg->SetVisible( TRUE ) ;
								}
							}
						}
						break ;
					}

					case POPUPMENU_CASTLERIGHTSETTING :				// 성 구조물에 대한 권한 설정
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						CRYLCastleEmblemRightDlg* pDlg = ( CRYLCastleEmblemRightDlg* )pScene->GetCastleEmblemRightDlg();

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							pDlg->SetCastleID( pTempCreature->GetCastleID() );
							pDlg->SetVisible( TRUE );
						}
						break ;
					}

					case POPUPMENU_CASTLENORIGHTMASTER :			// 성 상징물 관련 - 권한 없음, 길드 마스터만 가능
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
						break ;
					}

					case POPUPMENU_CASTLENORIGHTADMIN :				// 성 상징물 관련 - 권한 없음, 길드 마스터 + 관리자 가능
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						CastleRight* pRight	= NULL;
						if ( pTempCreature )
						{
							CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pTempCreature->GetCastleID() ) ;
							if ( lpCastle )
							{
								pRight = lpCastle->GetCastleRight() ;
							}
						}
						
						if ( pRight )
						{
							switch( pRight->m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] )
							{
								case Guild::MASTER:
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
									break;

								case Guild::MIDDLE_ADMIN:
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;
									break;

								case Guild::COMMON:
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3542 ] ) ;
									break;
						
								default:
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;
									break;
							}
						}
						else
						{
							lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;
						}

						break;
					}

					case POPUPMENU_CASTLEEMBLEMUPGRADE :			// 성 소유 상징물의 업그레이드
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						CRYLEmblemUpgradeDlg* pDlg = ( CRYLEmblemUpgradeDlg* )pScene->GetEmblemUpgradeDlg();

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( pTempCreature->GetUpgradeStep() < Siege::MAX_EMBLEM_UPGRADE )
							{
								pDlg->SetCastleAndEmblemID( pTempCreature->GetCastleID(), m_dwPopupSelectChrID ) ;
								pDlg->SetEmblemInfo( m_dwPopupSelectChrID ) ;
								pDlg->SetVisible( TRUE ) ;	
							}
							else
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 2630 ] ) ;
							}
						}
						break ;
					}

					case POPUPMENU_CASTLEEMBLEMUPGRADEEXPLANATION :	// 성 소유 업그레이드 설명 보기explanation
						break ;

					// 성문
					case POPUPMENU_CASTLEOPEN :						// 성문 열기
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							SendPacket::CharCastleCmd( g_GameSession, 
								CRYLNetworkData::Instance()->m_dwMyChrID, 
								pTempCreature->GetCastleID(),   
								m_dwPopupSelectChrID, 
								0, 0, 
								PktCastleCmd::CASTLE_GATE_OPEN ) ;
						}
						break ;
					}

					case POPUPMENU_CASTLECLOSE :					// 성문 닫기
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							SendPacket::CharCastleCmd( g_GameSession, 
								CRYLNetworkData::Instance()->m_dwMyChrID, 
								pTempCreature->GetCastleID(),   
								m_dwPopupSelectChrID, 
								0, 0, 
								PktCastleCmd::CASTLE_GATE_CLOSE ) ;
						}
						break ;
					}

					case POPUPMENU_CASTLEDEFENCE :					// 성문 막기
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							SendPacket::CharCastleCmd( g_GameSession, 
								CRYLNetworkData::Instance()->m_dwMyChrID, 
								pTempCreature->GetCastleID(),   
								m_dwPopupSelectChrID, 
								0, 0, 
								PktCastleCmd::CASTLE_GATE_PROTECT ) ;
						}
						break ;
					}

					case POPUPMENU_CASTLEUPGRADE :					// 성문 업그레이드
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						char szBuff[ 128 ] ;
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature)
						{
							sprintf( szBuff, CRYLStringTable::m_strString[3022],
								pTempCreature->GetCastleDoorUpgradePrice(), pTempCreature->GetCastleDoorUpgradeTime() ) ;
							lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
							lpMessageBox->SetResult( &m_dwCastleDoorUpgradeResult ) ;
						}
					}
					break ;

					case POPUPMENU_CASTLEREPAIR :					// 성문 수리
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						char szBuff[ 128 ] ;
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[3023], pTempCreature->GetCastleDoorRepairPrice() ) ;
							lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
							lpMessageBox->SetResult( &m_dwCastleDoorRepairResult ) ;
						}
						break ;
					}

					case POPUPMENU_CASTLERECOVERY :					// 성문 복구
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature && Siege::DESTROYED == pTempCreature->GetState() )
						{
							CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
							char szBuff[ 128 ] ;

							sprintf( szBuff, CRYLStringTable::m_strString[3024],
								pTempCreature->GetCastleDoorRecoveryPrice(), pTempCreature->GetCastleDoorRecoveryTime() ) ;
							lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
							lpMessageBox->SetResult( &m_dwCastleDoorRecoveryResult ) ;
						}
						break ;
					}

					// 뒷문
					case POPUPMENU_CASTLEIN	:						// 성 안으로
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
						if ( pTempCreature )
						{
							SendPacket::CharCastleCmd( g_GameSession, 
								CRYLNetworkData::Instance()->m_dwMyChrID, 
								pTempCreature->GetCastleID(),
								m_dwPopupSelectChrID,
								Siege::INSIDE, 0, 
								PktCastleCmd::CASTLE_BACKDOOR ) ;
						}
						break ;
					}

					case POPUPMENU_CASTLEOUT :						// 성 밖으로
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
						if ( pTempCreature )
						{
							SendPacket::CharCastleCmd( g_GameSession, 
								CRYLNetworkData::Instance()->m_dwMyChrID, 
								pTempCreature->GetCastleID(),
								m_dwPopupSelectChrID,
								Siege::OUTSIDE, 0, 
								PktCastleCmd::CASTLE_BACKDOOR ) ;
						}
						break ;
					}

					// 진지
					case POPUPMENU_CAMPPOSSAVE :					// 위치 저장
						break ;

					case POPUPMENU_CAMPCARE	:						// 진지 손질하기
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						char szBuff[ 128 ] ;
						sprintf( szBuff, CRYLStringTable::m_strString[ 2629 ] ) ;
						lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM ) ;
						lpMessageBox->SetResult( &m_dwCampCareResult ) ;
						break ;
					}

					case POPUPMENU_CAMPUPGRADE :					// 진지 업그레이드
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampUpgradeDlg* pDlg = ( CRYLCampUpgradeDlg* )pScene->GetCampUpgradeDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( pTempCreature->GetState() == Siege::COMPLETE )
							{
								if ( pTempCreature->GetUpgradeStep() < Siege::MAX_UPGRADE_NUM )
								{
									CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
									if ( pCamp )
									{
										pDlg->SetCampID( pCamp->GetCampID(), Siege::GetUpgradeMaterialNum( pTempCreature->GetObjectType() )) ;
										pDlg->SetVisible( TRUE ) ;	
									}
								}
								else
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 2630 ] ) ;
								}
							}
							else
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 2631 ] ) ;
							}
						}
						break ;
					}
					
					case POPUPMENU_CHANGESHOP:					// 상점으로 변환
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampChangeDlg* pDlg = ( CRYLCampChangeDlg* )pScene->GetCampChangeDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( 0 < pTempCreature->GetUpgradeStep() )
							{
								int nCnt = 0;
								nCnt = Siege::GetChangingTypeMaterialNum( pTempCreature->GetObjectType(), Siege::CAMP_SHOP, pTempCreature->GetUpgradeStep() );

								pDlg->SetCampChange( pTempCreature->GetCampID(),Siege::CAMP_SHOP, nCnt );
								pDlg->SetVisible( TRUE ) ;
							}
							else
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3048 ] ) ;
							}
						}
						break;
					}

					case POPUPMENU_CHANGEMINING:				// 채굴기로 변환
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampChangeDlg* pDlg = ( CRYLCampChangeDlg* )pScene->GetCampChangeDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( 0 < pTempCreature->GetUpgradeStep() )
							{
								int nCnt = 0;
								nCnt = Siege::GetChangingTypeMaterialNum( pTempCreature->GetObjectType(), Siege::MINING_CAMP, pTempCreature->GetUpgradeStep() );
								pDlg->SetCampChange( pTempCreature->GetCampID(),Siege::MINING_CAMP, nCnt );
								pDlg->SetVisible( TRUE ) ;
							}
							else
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3048 ] ) ;
							}
						}
						break;
					}
					case POPUPMENU_CHANGEBASE:					// 기본형으로 변환
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampChangeDlg* pDlg = ( CRYLCampChangeDlg* )pScene->GetCampChangeDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							int nCnt = 0;
							nCnt = Siege::GetChangingTypeMaterialNum( pTempCreature->GetObjectType(), Siege::CAMP, pTempCreature->GetUpgradeStep() );
							pDlg->SetCampChange( pTempCreature->GetCampID(),Siege::CAMP, nCnt);
							pDlg->SetVisible( TRUE ) ;
						}

						break;
					}

					case POPUPMENU_MININGCAMPMENU:
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLMiningCampDlg* pDlg = ( CRYLMiningCampDlg* )pScene->GetMiningCampDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							pDlg->SetCamp( pTempCreature->GetCampID() );
							pDlg->SetVisible( TRUE ) ;
						}
						break;
					}

					case POPUPMENU_CAMPSHOPMENU:
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						pScene->GUIVisible( FALSE );

						CRYLCampShopDlg* pDlg = ( CRYLCampShopDlg* )pScene->GetCampShopDlg();
						pDlg->SetAdminMode(false);
						
						CRYLNetworkData::SendStallEnter(m_dwPopupSelectChrID);
						break;
					}

					case POPUPMENU_CAMPSHOPADMIN:
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						pScene->GUIVisible( FALSE );

						CRYLCampShopDlg* pDlg = ( CRYLCampShopDlg* )pScene->GetCampShopDlg();
						pDlg->SetAdminMode(true);

						CRYLNetworkData::SendStallEnter(m_dwPopupSelectChrID);
						break;
					}
					
					case POPUPMENU_WEAPONLOAD :
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLWorldWeaponLoadDlg* pDlg = ( CRYLWorldWeaponLoadDlg* )pScene->GetWorldWeaponLoadDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							switch( pTempCreature->GetSubState() )
							{
							case Siege::WEAPON_EMPTY :
								{
									pDlg->SetWeaponLoad( pTempCreature->GetCampID() );
									pDlg->SetVisible( TRUE ) ;
								}
								break;
							case Siege::WEAPON_CHARGE :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3543 ] );
								}
								break;
							case Siege::WEAPON_READY :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3544 ] );
								}
								break;
							case Siege::WEAPON_FIRE :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3544 ] );
								}
								break;
							}
						}

						break;
					}

					case POPUPMENU_WEAPONLAUNCH :
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLWorldWeaponLaunchDlg* pDlg = ( CRYLWorldWeaponLaunchDlg* )pScene->GetWorldWeaponLaunchDlg();
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							switch( pTempCreature->GetSubState() )
							{
							case Siege::WEAPON_EMPTY :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3545 ] );
								}
								break;
							case Siege::WEAPON_CHARGE :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 3546 ] );
								}
								break;
							case Siege::WEAPON_READY :
								{
									pDlg->SetCamp( pTempCreature->GetCampID() );
									pDlg->SetVisible( TRUE ) ;
								}
								break;
							case Siege::WEAPON_FIRE :
								{
									CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[3546] );
								}
								break;
							}

						}

						break;
					}

					case POPUPMENU_CAMPNORIGHTMASTER :
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
						break;
					}

					case POPUPMENU_CAMPNORIGHTADMIN :
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						CampRight*					pRight	= NULL;
						if( pTempCreature )
						{
							CRYLCamp* lpCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
							if ( lpCamp )
							{
								pRight = lpCamp->GetCampRight() ;
							}
						}
						if( pRight )
						{
							switch( pRight->m_aryCampRight[ CampRight::CAMP_CONTROL ] )
							{
							case Guild::MASTER:
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
								break;

							case Guild::MIDDLE_ADMIN:
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;
								break;

							case Guild::COMMON:
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3542 ] ) ;
								break;
							default:
								lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;
								break;
							}
						}
						else
							lpMessageBox->Create( CRYLStringTable::m_strString[ 3541 ] ) ;

						break;
					}

					case POPUPMENU_WEAPONREPAIR :
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampRepairDlg* pDlg = ( CRYLCampRepairDlg* )pScene->GetCampRepairDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() ) ;
							if ( pTempData )
							{
								if ( pTempCreature->GetState() == Siege::COMPLETE )
								{
									if ( pTempData->m_lCurrHP < static_cast<long>( pTempData->m_ulMaxHP ) )
									{
										CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
										if ( pCamp )
										{
											pDlg->SetWorldWeaponDlg( TRUE );
											pDlg->SetCampID( pCamp->GetCampID() ) ;
											pDlg->SetVisible( TRUE ) ;	
										}
									}
									else
									{
										CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
										lpMessageBox->Create( CRYLStringTable::m_strString[ 2634 ] ) ;
									}
								}
								else
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 2635 ] ) ;
								}
							}
						}
						break ;
					}

					case POPUPMENU_WEAPONRIGHTSETTING :
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampRightDlg* pDlg = ( CRYLCampRightDlg* )pScene->GetCampRightDlg() ;


						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							pDlg->SetWorldWeaponDlg( TRUE );
							pDlg->SetCampID( pTempCreature->GetCampID() ) ;
							pDlg->SetVisible( TRUE ) ;
						}
						break ;
					}
					case POPUPMENU_WEAPONUPGRADE :
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampUpgradeDlg* pDlg = ( CRYLCampUpgradeDlg* )pScene->GetCampUpgradeDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( pTempCreature->GetState() == Siege::COMPLETE )
							{
								if ( pTempCreature->GetUpgradeStep() < Siege::MAX_UPGRADE_NUM )
								{
									CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
									if ( pCamp )
									{
										pDlg->SetWorldWeaponDlg( TRUE );
										pDlg->SetCampID( pCamp->GetCampID(), Siege::GetUpgradeMaterialNum( pTempCreature->GetObjectType() )) ;
										pDlg->SetVisible( TRUE ) ;	
									}
								}
								else
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 2630 ] ) ;
								}
							}
							else
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 2631 ] ) ;
							}
						}
						break ;
					}

					case POPUPMENU_WEAPONDESTROY :
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 3416 ], MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
						lpMessageBox->SetResult( &m_dwCampDestoryResult ) ;
						break ;
					}

					case POPUPMENU_CAMPREPAIR :						// 진지 수리하기
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampRepairDlg* pDlg = ( CRYLCampRepairDlg* )pScene->GetCampRepairDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() ) ;
							if ( pTempData )
							{
								if ( pTempCreature->GetState() == Siege::COMPLETE )
								{
									if ( pTempData->m_lCurrHP < static_cast<long>( pTempData->m_ulMaxHP ) )
									{
										CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
										if ( pCamp )
										{
											pDlg->SetCampID( pCamp->GetCampID() ) ;
											pDlg->SetVisible( TRUE ) ;	
										}
									}
									else
									{
										CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
										lpMessageBox->Create( CRYLStringTable::m_strString[ 2634 ] ) ;
									}
								}
								else
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[ 2635 ] ) ;
								}
							}
						}
						break ;
					}

					case POPUPMENU_CAMPRIGHTSETTING	:				// 진지 관리 권한 설정
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampRightDlg* pDlg = ( CRYLCampRightDlg* )pScene->GetCampRightDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							pDlg->SetCampID( pTempCreature->GetCampID() ) ;
							pDlg->SetVisible( TRUE ) ;
						}
						break ;
					}

					case POPUPMENU_CAMPDESTROY :					// 진지 파괴
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 2636 ], MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
						lpMessageBox->SetResult( &m_dwCampDestoryResult ) ;
						break ;
					}

					case POPUPMENU_CAMPEXPLANATION :				// 진지 도움말 (설명)
					{
						// 스크립트가 너무 길어서 메모리에서는 잘라서 보관합니다.
						char szBuff[4096] = { 0, };
						for (int i = 0 ; i < 5; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3600 + i ]);
						}

						if ( GameRYL::SERVER_TEST == CRYLNetworkData::Instance()->m_eServerType )
						{
							char szTempBuff[ MAX_PATH ] = { 0, };
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3605 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING_TEST ) );
							strcat(szBuff, szTempBuff);
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3606 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING_TEST ) );
							strcat(szBuff, szTempBuff);
						}
						else
						{
							char szTempBuff[ MAX_PATH ] = { 0, };
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3605 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING ) );
							strcat(szBuff, szTempBuff);
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3606 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING ) );
							strcat(szBuff, szTempBuff);
						}

						for (int i = 7; i < 37; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3600 + i ]);
						}

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM ) ;
						lpMessageBox->SetWordBreak( TRUE ) ;
						break ;
					}

					case POPUPMENU_CAMPSHOPEXPLANATION :		// 길드 요새 상점 도움말
					{
						// 스크립트가 너무 길어서 메모리에서는 잘라서 보관합니다.
						char szBuff[4096] = { 0, };
						for (int i = 0 ; i < 39; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3650 + i ]);
						}

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM ) ;
						lpMessageBox->SetWordBreak( TRUE ) ;
						break ;
					}

					case POPUPMENU_MININGCAMPEXPLANATION :		// 채굴기 도움말
					{
						// 스크립트가 너무 길어서 메모리에서는 잘라서 보관합니다.
						char szBuff[4096] = { 0, };
						for (int i = 0 ; i < 38; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3700 + i ]);
						}

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM ) ;
						lpMessageBox->SetWordBreak( TRUE ) ;
						break ;
					}

					case POPUPMENU_WEAPONEXPLANATION :			// 월드 웨폰 도움말
					{
						// 스크립트가 너무 길어서 메모리에서는 잘라서 보관합니다.
						char szBuff[4096] = { 0, };
						for (int i = 0 ; i < 5; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3750 + i ]);
						}

						if ( GameRYL::SERVER_TEST == CRYLNetworkData::Instance()->m_eServerType )
						{
							char szTempBuff[ MAX_PATH ] = { 0, };
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3755 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING_TEST ) );
							strcat(szBuff, szTempBuff);
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3756 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING_TEST ) );
							strcat(szBuff, szTempBuff);
						}
						else
						{
							char szTempBuff[ MAX_PATH ] = { 0, };
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3755 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING ) );
							strcat(szBuff, szTempBuff);
							sprintf(szTempBuff, CRYLStringTable::m_strString[ 3756 ], CRYLStringTable::MakePrintGold( Siege::FAME_FOR_CAMP_BUILDING ) );
							strcat(szBuff, szTempBuff);
						}

						for (int i = 7; i < 41; ++i)
						{
							strcat(szBuff, CRYLStringTable::m_strString[ 3750 + i ]);
						}

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM ) ;
						lpMessageBox->SetWordBreak( TRUE ) ;
						break ;
					}

					case POPUPMENU_SHOWTAXINFO :				// 요새 상점, 채굴기에 적용된 세율 확인 메뉴
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampShowTaxInfoDlg* pDlg = ( CRYLCampShowTaxInfoDlg* )pScene->GetCampShowTaxInfoDlg() ;

						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							switch ( pTempCreature->GetObjectType() )
							{
								case Siege::CAMP_SHOP :		pDlg->SetCampShop( *pTempCreature->GetPosition() ) ;	break ;
								case Siege::MINING_CAMP :	pDlg->SetMiningCamp( *pTempCreature->GetPosition() ) ;	break ;
							}

							pDlg->SetVisible( TRUE ) ;
						}
						break ;
					}

					// 근거리 공성병기
					case POPUPMENU_SHORTSIEGEARMSRIDE :				// 근거리 공성병기 탑승
					case POPUPMENU_LONGSIEGEARMSRIDE :				// 원거리 공성병기 탑승
					case POPUPMENU_DROPSHIPRIDE :					// 수송선 공성병기 탑승
					{
						pGame->SetEnableMove( FALSE );

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 4084 ], MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
						lpMessageBox->SetResult( &m_dwSiegeArmsRideResult ) ;
						m_dwSiegeArmsRideResult = 0 ;
						break ;
					}

					case POPUPMENU_SHORTSIEGEARMSREPAIR	:			// 근거리 공성병기 수리
					case POPUPMENU_LONGSIEGEARMSREPAIR :			// 원거리 공성병기 수리
					case POPUPMENU_DROPSHIPREPAIR :					// 수송선 공성병기 수리
					{						
						RYLCreature* pSiegeArmCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );

						if(pSiegeArmCreature)
						{
							CharDataNode* pSiegeArmData	= RYLCharacterDataManager::Instance()->GetCharData(pSiegeArmCreature->GetCharID());

							if(pSiegeArmData)
							{							
								unsigned long dwHp		= pSiegeArmData->m_ulMaxHP - pSiegeArmData->m_lCurrHP;
								unsigned long dwPerHp	= pSiegeArmData->m_ulMaxHP/10;						
								unsigned long dwNum		= (dwHp/dwPerHp);

								if((dwHp%dwPerHp)!=0)
								{
									dwNum++;
								}

								if(!dwNum)
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create( CRYLStringTable::m_strString[4072]) ;														
								}
								else
								{
									char szInfo[0xff] = {0, };
									sprintf(szInfo, CRYLStringTable::m_strString[4071], dwNum);									

									CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
									lpMessageBox->Create(szInfo, MB_YES | MB_NO | MB_CANCEL ) ;
									lpMessageBox->SetResult( &m_dwSiegeArmsRepair ) ;									
								}
							}							
						}
						
						break ;
					}
						

					case POPUPMENU_SHORTSIEGEARMSSTARTKITRECOVERY :	// 근거리 공성병기 스타터 킷 복구
					case POPUPMENU_LONGSIEGEARMSSTARTKITRECOVERY :	// 원거리 공성병기 스타터 킷 복구
					case POPUPMENU_DROPSHIPRECOVERY :				// 수송선 공성병기 스타터 킷 복구
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 2637 ], MB_YES | MB_NO | MB_CANCEL ) ;
						lpMessageBox->SetResult( &m_dwSiegeArmsStartKitRecoveryResult ) ;
						break ;
					}

					// 근거리 수성병기
					case POPUPMENU_SHORTCASTLEARMSRIDE :			// 근거리 수성병기 탑승
					case POPUPMENU_LONGCASTLEARMSRIDE :				// 원거리 수성병기 탑승
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( Siege::GUARD == pTempCreature->GetObjectType() ||
								 Siege::COMPLETE != pTempCreature->GetState() ||
								 pTempCreature->GetOwnerID() != CRYLNetworkData::Instance()->m_dwMyChrID ||
								 0 != pTempCreature->GetRiderID( 0 ) )
							{
								CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4083 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
							}
							else
							{
								pGame->SetEnableMove( FALSE );

								CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 4084 ], MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
								lpMessageBox->SetResult( &m_dwCastleArmsRideResult ) ;
								m_dwCastleArmsRideResult = 0 ;
							}
						}
						break ;
					}

					case POPUPMENU_SHORTCASTLEARMSDESTROY :			// 근거리 수성병기 파괴
					case POPUPMENU_LONGCASTLEARMSDESTROY :			// 원거리 수성병기 파괴
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
						if ( pTempCreature )
						{
							if ( Siege::COMPLETE == pTempCreature->GetState() &&
								 pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID &&
								 0 == pTempCreature->GetRiderID(0) )
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
								lpMessageBox->Create( CRYLStringTable::m_strString[ 2638 ], MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
								lpMessageBox->SetResult( &m_dwCastleArmsDestroyResult ) ;
								m_dwCastleArmsDestroyResult = 0 ;
							}
							else
							{
								CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4094 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
							}
						}
						break;
					}

					// 원거리 수성병기
					case POPUPMENU_LONGCASTLEARMSREPAIR :			// 원거리 수성병기 수리
					case POPUPMENU_SHORTCASTLEARMSREPAIR :			// 근거리 수성병기 수리
					case POPUPMENU_CASTLEGUARDREPAIR :				// 가드 수리
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
						if ( pTempCreature )
						{
							CharDataNode* pTempData	= RYLCharacterDataManager::Instance()->GetCharData( m_dwPopupSelectChrID ) ;
							if ( pTempData )
							{
								if ( pTempData->m_ulMaxHP != pTempData->m_lCurrHP &&
									 0 == pTempCreature->GetRiderID(0) &&
									 pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID &&
									 Siege::COMPLETE == pTempCreature->GetState() )
								{
									if ( CRYLGameData::Instance()->m_csStatus.m_Info.Gold < Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT )
									{
										CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4090 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
									}
									else
									{
										char szBuff[ 128 ] ;
										sprintf( szBuff, CRYLStringTable::m_strString[ 2639 ],
											CRYLStringTable::MakePrintGold( pTempCreature->GetCastleArmsRepairPrice() ),
											pTempCreature->GetCastleArmsRepairTime() ) ;

										CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
										lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
										lpMessageBox->SetResult( &m_dwCastleArmsRepairResult ) ;
										m_dwCastleArmsRepairResult = 0 ;
									}
								}
								else
								{
									CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4089 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
								}
							}						
						}
						break ;
					}

					case POPUPMENU_LONGCASTLEARMSUPGRADE :			// 원거리 수성병기 업그레이드
					case POPUPMENU_SHORTCASTLEARMSUPGRADE :			// 근거리 수성병기 업그레이드
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						char szBuff[ 128 ] ;
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[ 2640 ], pTempCreature->GetCastleDoorUpgradePrice() ) ;
							lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
							lpMessageBox->SetResult( &m_dwCastleArmsUpgradeResult ) ;
						}
						break ;
					}

					// 병기 개발 NPC
					case POPUPMENU_CASTLEDEFENCEARMSMENU :			// 성 방어 병기 설명 보기
						break ;

					case POPUPMENU_GUARDDEVELOP :					// 가드 개발
					case POPUPMENU_SHORTDEFENCEARMSDEVELOP :		// 근거리 방어용 병기 개발
					case POPUPMENU_LONGDEFENCEARMSDEVELOP :			// 원거리 방아용 병기 개발
					{
						RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
						if ( pTempCreature )
						{
							if ( GameTime::ST_SIEGE_WAR != CRYLGameTimeManager::Instance()->GetNowSiegeTime() )
							{
								CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
								pMessageBox->Create( CRYLStringTable::m_strString[ 4064 ] ) ;
							}
							else
							{
								m_dwArmsType = Siege::GUARD ;
								int iIndex = 0 ;

								if ( POPUPMENU_GUARDDEVELOP == m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind )
								{
									m_dwArmsType = Siege::GUARD ;
									iIndex = 0 ;
								}
								else if ( POPUPMENU_SHORTDEFENCEARMSDEVELOP == m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind )
								{
									m_dwArmsType = Siege::SHORT_RANGE_CASTLE_ARMS ;
									iIndex = 1 ;
								}
								else if ( POPUPMENU_LONGDEFENCEARMSDEVELOP == m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind )
								{
									m_dwArmsType = Siege::LONG_RANGE_CASTLE_ARMS ;
									iIndex = 2 ;
								}

								if ( Siege::CASTLE_ARMS_NPC != pTempCreature->GetObjectType() )
								{
									CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4077 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
								}
								else if ( pTempCreature->GetCastleArmsDevelopePrice( m_dwArmsType ) > CRYLGameData::Instance()->m_csStatus.m_Info.Gold )
								{
									CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 4078 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
								}
								else
								{
									char szBuff[ 128 ] ;
									sprintf( szBuff, CRYLStringTable::m_strString[ 4073 ],
										CRYLStringTable::m_strString[ 4074 + iIndex ],
										CRYLStringTable::MakePrintGold( pTempCreature->GetCastleArmsDevelopePrice( m_dwArmsType ) ),
										pTempCreature->GetCastleArmsDevelopeTime( m_dwArmsType ) ) ;

									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( szBuff, MB_EXIT | MB_CONFIRM | MB_CANCEL ) ;
									lpMessageBox->SetResult( &m_dwCastleArmsDevelopeResult ) ;
									m_dwCastleArmsDevelopeResult = 0 ;
								}
							}
						}
						break ;
					}

					// 용병 관련.
					case POPUPMENU_TACTICS:
					{
						RYLCreature* pSelfCreature		= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );
						RYLCreature* pTargetCreature	= RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );

						if(pSelfCreature && pTargetCreature)
						{
							CharDataNode* pTargetData	= RYLCharacterDataManager::Instance()->GetCharData( pTargetCreature->GetCharID() );
							CharDataNode* pSelfData		= RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
							
							if(pTargetData && pSelfData)
							{
								CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
								CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
								CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg();								
								CRYLGuildInfoPanel*			pPanel = (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();
										
								// 길드 가입자.
								if(pGame->m_csStatus.m_Info.GID && !pSelfCreature->GetTactics())
								{									
									CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3595], pSelfData->m_ulCharID, 128);
									break;
								}
								// 용병 가입.
								if(pGame->m_csStatus.m_Info.GID && pSelfCreature->GetTactics()==Guild::TACTICS)
								{
									CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3597], pSelfData->m_ulCharID, 128);
									break;
								}							
								// 용병 신청중.
								if(pGame->m_csStatus.m_Info.GID && pSelfCreature->GetTactics()==Guild::TACTICS_WAIT)
								{
									LPGUILDLARGEINFOEX  pMyGuildInfo = pPanel->FindGuild(pTargetData->m_ulGuildID);

									if(pMyGuildInfo)
									{										
										char szInfo[0xff] = {0,};
										sprintf(szInfo, CRYLStringTable::m_strString[3596], pMyGuildInfo->m_szName);
										CRYLChattingDlg::Instance()->AddChatMessage(szInfo, pSelfData->m_ulCharID, 128);
										break;
									}
								}
								// 레벨 미만.
								if(pSelfData->m_cLevel<Guild::TACTICE_JON_LEVEL)
								{
									CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3598], pSelfData->m_ulCharID, 128);
									break;									
								}
								// 타국적.
								if(pTargetData->m_cNation!=pSelfData->m_cNation)
								{																											
									CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3599], pSelfData->m_ulCharID, 128);
									break;
								}								
								// 길드 미 소속자.
								if(!pTargetData->m_ulGuildID)
								{
									CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3877], pSelfData->m_ulCharID, 128);
									break;
								}
																														
								// 조건 만족 서버에 패킷을 보낸다.
								SendPacket::CharGuildCmd( g_GameSession, pTargetData->m_ulGuildID, 
									CRYLNetworkData::Instance()->m_dwMyChrID, m_dwPopupSelectChrID, PktGuildCmd::GC_TACTICS );								
							}
						}
						break;
					}		

					case POPUPMENU_TACTICS_JOIN:
					{
						RYLCreature* pSelfCreature		= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );
						RYLCreature* pTargetCreature	= RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );

						if(pSelfCreature && pTargetCreature)
						{
							CharDataNode* pTargetData	= RYLCharacterDataManager::Instance()->GetCharData( pTargetCreature->GetCharID() );
							CharDataNode* pSelfData		= RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

							if(pTargetData && pSelfData)
							{
								if(pSelfData->m_ulGuildID)
								{
									// 레벨 미만.
									if(pSelfData->m_cLevel<Guild::TACTICE_JON_LEVEL)
									{
										CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3880], pSelfData->m_ulCharID, 128);
										break;
									}																		

									// 타국적.
									if(pSelfData->m_cNation!=pTargetData->m_cNation)
									{
										CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3881], pSelfData->m_ulCharID, 128);
										break;
									}
									
									// 용병 가입자.
									if(pTargetCreature->GetTactics()==Guild::TACTICS)
									{										
										CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3883], pSelfData->m_ulCharID, 128);
										break;
									}
									// 용병 신청자.
									if(pTargetCreature->GetTactics()==Guild::TACTICS_WAIT)
									{										
										CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[4055], pSelfData->m_ulCharID, 128);
										break;
									}

									// 길드 소속자
									if(pTargetData->m_ulGuildID)
									{
										CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3882], pSelfData->m_ulCharID, 128);
										break;
									}
								}

								SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, 
									m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_TACTICS_JOIN );
							}							
						}
						break;
					}
				}

				m_bPopupShow = FALSE;
				m_dwPopupSelect = 0xFFFFFFFF;

				return TRUE;
			}

			break;	// for문 탈출
		}
	}

	return FALSE;
}

void CRYLRaceBase::NPCSpawn(void)
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return;

	vector3 vecSelf = *( pSelfCreature->GetPosition() );
	vecSelf.y = 0.0f;
	vector3 vecLength = vecSelf - g_CharacterData.m_vecPrePosition;
	float fLength = vecLength.GetLens();

	if (fLength > 500.0f)
	{
		g_CharacterData.m_vecPrePosition = *( pSelfCreature->GetPosition() );
		g_CharacterData.m_vecPrePosition.y = 0.0f;
		float fMinX = pSelfCreature->GetPosition()->x - 5000.0f;
		float fMaxX = pSelfCreature->GetPosition()->x + 5000.0f;
		float fMinZ = pSelfCreature->GetPosition()->z - 5000.0f;
		float fMaxZ = pSelfCreature->GetPosition()->z + 5000.0f;

		NPCNode *pNPCNode;
		vector3 vecPos;
		for (unsigned long i = 0; i < g_NPCList.GetNpcNum(); i++)
		{
			pNPCNode = g_NPCList.GetNPCNodeByOrder(i);
			vecPos.x = pNPCNode->m_fPosX * 100.0f;
			vecPos.y = pNPCNode->m_fPosY * 100.0f;
			vecPos.z = pNPCNode->m_fPosZ * 100.0f;
			if (fMinX <= vecPos.x && vecPos.x <= fMaxX && fMinZ <= vecPos.z && vecPos.z <= fMaxZ)
			{	// NPC 추가
				if (strcmp(pNPCNode->m_strNpcSkin, "None"))
				{
#ifdef __NEWNPCSCRIPT
					RYLCreatureManager::Instance()->AddCreature( 
						pNPCNode->m_dwUID, pNPCNode->m_strNpcName, pNPCNode->m_strNpcSkin, vecPos, pNPCNode->m_fDirection, false, pNPCNode->m_unNationality );
#else
					RYLCreatureManager::Instance()->AddCreature( 
						pNPCNode->m_dwUID, pNPCNode->m_strNpcName, pNPCNode->m_strNpcSkin, vecPos, pNPCNode->m_fDirection, false, pNPCNode->m_unNationality );
#endif
				}
			}
		}
	}

	static POINT ptOldSector = { 0, 0 };
	POINT ptSector, ptItem;
	vector3 vecSelfChrPos;

	CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
	if ( !pSelfModel )
		return;

	pSelfModel->GetPosition( vecSelfChrPos );
	ptSector.x =  vecSelfChrPos.x/ 3200.0f;
	ptSector.y =  vecSelfChrPos.z/ 3200.0f;

	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (!pGame->m_lpSceneManager->m_pCameraScript->ISPlay())
	{
		list<LPFieldInstance>::iterator it;

		for (it = pGame->m_lstFieldItem.begin(); it != pGame->m_lstFieldItem.end();)
		{
			ptItem.x = (*it)->m_vecPos.x / 3200.0f;
			ptItem.y = (*it)->m_vecPos.z / 3200.0f;

			if (ptSector.x - 1 <= ptItem.x && ptItem.x <= ptSector.x + 1 && ptSector.y - 1 <= ptItem.y && ptItem.y <= ptSector.y + 1)
			{
				++it;
			} 
			else
			{
				LPFieldInstance pInstance = (*it);
				pGame->m_lpSceneManager->DeleteInstanceObject(pInstance->m_dwInstanceID);
				it = pGame->m_lstFieldItem.erase(it);
				delete pInstance;
			}
		}

		vector<unsigned long> vecCharID;
		RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

		if ( vecCharID.size() > 0 )
		{
			std::vector<unsigned long>::reverse_iterator rpos = vecCharID.rbegin();
			std::vector<unsigned long>::reverse_iterator rend = vecCharID.rend();

			for (; rpos != rend; ++rpos)
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*rpos); 
				if ( NULL == pTempCreature )
					continue;

				if ( pTempCreature == pSelfCreature )
					continue;

				DWORD dwCharID = pTempCreature->GetCharID();

				ptItem.x = pTempCreature->GetPosition()->x / 3200.0f;
				ptItem.y = pTempCreature->GetPosition()->z / 3200.0f;

				if (!((ptSector.x - 1 <= ptItem.x && ptItem.x <= ptSector.x + 1 && 
					ptSector.y - 1 <= ptItem.y && ptItem.y <= ptSector.y + 1) ||
					pGame->m_csStatus.CheckPartyMember(dwCharID)))
				{
					if ( ptItem.x == 0 && ptItem.y == 0 )
						continue;

					switch(CRYLGameData::GetClientIdentity(dwCharID))
					{
					case Creature::CT_SIEGE_OBJECT:
					case Creature::CT_MONSTER:
					case Creature::CT_STRUCT:
					case Creature::CT_PC:
						break;

					default:
						RYLCreatureManager::Instance()->DeleteCreature(dwCharID);
						break;
					}
				}                
			}
		}
	}

	ptOldSector.x=ptSector.x;
	ptOldSector.y=ptSector.y;	
}

class CFindPopupMenu : public std::unary_function<PopupMenu, bool>
{
public:

	explicit CFindPopupMenu(unsigned long dwPopupKind) 
		:	m_dwPopupKind(dwPopupKind) 
	{ }		

	bool operator() (PopupMenu& popupMenu) 
	{ 
		return (m_dwPopupKind == popupMenu.m_dwPopupKind) ? true : false; 
	}


private:

	const unsigned long		m_dwPopupKind;
};

void CRYLRaceBase::DisablePopupMenuKind(unsigned long dwSrcKind, unsigned long dwResultKind)
{
	CFindPopupMenu findPopupMenu(dwSrcKind);
	std::vector<PopupMenu>::iterator it = std::find_if(m_lstPopupMenu.begin(), m_lstPopupMenu.end(), findPopupMenu);

	it->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
	it->m_dwPopupKind = dwResultKind;
}
