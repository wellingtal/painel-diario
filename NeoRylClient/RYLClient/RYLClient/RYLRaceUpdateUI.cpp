
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include <Skill/SkillMgr.h>
#include <Creature/EnemyCheck.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendCastle.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include "WinInput.h"
#include "FrameTimer.h"
#include "BaseGraphicsLayer.h"

#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"
#include "RYLEdit.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLCampManager.h"
#include "RYLCamp.h"

#include "RYLGuildFrameDlg.h"
#include "RYLChattingDlg.h"
#include "RYLGuildInfoPanel.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "RYLGameTimeManager.h"

#include "RYLJobChangeDlg.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"

void CRYLRaceBase::UpdateGameEtc( float fCreatureUpdate )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (KeyControl::IsKeyState(KeyControl::KEY_COLUM_ZOOMIN))
	{
		g_DeviceInput.m_lMMoveZ -= 1;
	}
	else if (KeyControl::IsKeyState(KeyControl::KEY_COLUM_ZOOMOUT))
	{
		g_DeviceInput.m_lMMoveZ += 1;
	}

	if (pGame->m_bShowCursor)
		g_CharacterData.SetCamera(0, 0, g_DeviceInput.m_lMMoveZ);
	else
		g_CharacterData.SetCamera(g_DeviceInput.m_ptMovePos.x, g_DeviceInput.m_ptMovePos.y, g_DeviceInput.m_lMMoveZ);

	NPCSpawn();
	g_CharacterData.Update(fCreatureUpdate);
}

void CRYLRaceBase::UpdateGameMsgBox( BOOL& bClick, BOOL& bEdge )
{
	if (g_MessageBoxManager.m_lstMessageBox.size())
	{
		if (g_MessageBoxManager.GetIsExistModal())
		{
			bEdge = TRUE;
			bClick = TRUE; // MINOTAURS POPUP이 떠도 이동 가능
		}

		if (g_MessageBoxManager.Update()) 
		{
			return;
		}
	}

	// 파티 초대 승인
	if (m_dwPartyCmdResult & MB_YES)
	{
		switch (m_dwPartyCmdType)
		{
			case ClientConstants::PartyCmd_Invite:
				CRYLNetworkData::SendPartyCmd(m_dwPartyCmdSenderID, ClientConstants::PartyCmd_Accept);
				break;
		}

		m_dwPartyCmdResult = 0;
		m_dwPartyCmdType = 0;

		return;
	} 
	else if (m_dwPartyCmdResult & MB_NO || m_dwPartyCmdResult & MB_EXIT)
	{
		switch (m_dwPartyCmdType)
		{
			case ClientConstants::PartyCmd_Invite:
				CRYLNetworkData::SendPartyCmd(m_dwPartyCmdSenderID, ClientConstants::PartyCmd_Refuse);
				break;
		}

		m_dwPartyCmdResult = 0;
		m_dwPartyCmdType = 0;

		return;
	}

	// 용병 지원 승인.
	if(m_dwTacticsReq & MB_YES)
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, CRYLNetworkData::Instance()->m_dwMyChrID, Guild::COMMON, PktGuildCmd::GC_TACTICS_REQUEST) ;
		m_dwTacticsReq = 0;
	}

	// 전직창 출력.
	if (m_dwLevelUp & MB_YES)
	{
		CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		CRYLJobChangeDlg*	pJobDlg	= ( CRYLJobChangeDlg* )pScene->GetJobChangeDlg();

		RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		
		if( pSelfCreature )
		{
			CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

			if( pSelfData )
			{
				switch(pSelfData->m_usClass)
				{
				case CClass::Fighter:
					pJobDlg->SetText(CRYLStringTable::m_strString[3896]);
					break;
				case CClass::Rogue:
					pJobDlg->SetText(CRYLStringTable::m_strString[3897]);
					break;
				case CClass::Mage:
					pJobDlg->SetText(CRYLStringTable::m_strString[3898]);
					break;
				case CClass::Acolyte:
					pJobDlg->SetText(CRYLStringTable::m_strString[3899]);
					break;
				case CClass::Combatant:
					pJobDlg->SetText(CRYLStringTable::m_strString[3900]);
					break;
				case CClass::Officiator:
					pJobDlg->SetText(CRYLStringTable::m_strString[3901]);
					break;
				}

				pJobDlg->SetVisible( TRUE );
			}
		}		
		
		m_dwLevelUp = 0;
	}	

	// 거래 요청 승인
	if (m_dwExchangeCmdResult & MB_YES)
	{
		switch (m_dwExchangeCmdType)
		{
			case ClientConstants::ExchangeCmd_Propose:
				CRYLNetworkData::SendExchangeCmd(m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Accept);
				break;
		}

		m_dwExchangeCmdResult = 0;
		m_dwExchangeCmdType = 0;

		return;
	} 
	else if (m_dwExchangeCmdResult & MB_NO || m_dwExchangeCmdResult & MB_EXIT)
	{
		switch (m_dwExchangeCmdType)
		{
			case ClientConstants::ExchangeCmd_Propose:
				CRYLNetworkData::SendExchangeCmd(m_dwExchangeCmdSenderID, ClientConstants::ExchangeCmd_Refuse);
				break;
		}

		m_dwExchangeCmdResult = 0;
		m_dwExchangeCmdType = 0;

		return;
	}

	// 파티 듀얼 신청/승인
	if (m_dwPartyDuelCmdResult & MB_YES)
	{
		switch (m_dwPartyDuelCmdType)
		{
			case ClientConstants::DuelCmd_RequestPartyInfo:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwPartyDuelCmdSenderID, ClientConstants::DuelCmd_PartyPropose);
				break;

			case ClientConstants::DuelCmd_PartyPropose:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwPartyDuelCmdSenderID, ClientConstants::DuelCmd_PartyReady);
				break;
		}

		m_dwPartyDuelCmdResult = 0;
		m_dwPartyDuelCmdType = 0;

		return;
	} 
	else if (m_dwPartyDuelCmdResult & MB_NO || m_dwPartyDuelCmdResult & MB_EXIT)
	{
		switch (m_dwPartyDuelCmdType)
		{
			case ClientConstants::DuelCmd_PartyPropose:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwPartyDuelCmdSenderID, ClientConstants::DuelCmd_Refuse);
				break;
		}

		m_dwPartyDuelCmdResult = 0;
		m_dwPartyDuelCmdType = 0;

		return;
	}

	// 개인 듀얼 신청/승인
	if (m_dwDuelCmdResult & MB_YES)
	{
		switch (m_dwDuelCmdType)
		{
			case ClientConstants::DuelCmd_Propose:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwDuelCmdSenderID, ClientConstants::DuelCmd_Ready);
				break;

			case 100:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwDuelCmdSenderID, ClientConstants::DuelCmd_Propose);
				CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[507], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;
		}

		m_dwDuelCmdResult = 0;
		m_dwDuelCmdType = 0;

		return;
	} 
	else if (m_dwDuelCmdResult & MB_NO || m_dwDuelCmdResult & MB_EXIT)
	{
		switch (m_dwDuelCmdType)
		{
			case ClientConstants::DuelCmd_Propose:
				SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwDuelCmdSenderID, ClientConstants::DuelCmd_Refuse);
				break;
		}

		m_dwDuelCmdResult = 0;
		m_dwDuelCmdType = 0;

		return;
	}

	//----------------------------------------------------------------------------------------------------
	// 공성 관련 메시지 박스

	// 진지 생성 (진지 생성에는 PopupSelectChrID 가 없다)
	if ( m_dwCampCreateResult & MB_YES ) 
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( pSelfCreature )
		{
			CharDataNode* pSelfNode = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() ) ;
			if ( pSelfNode )
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

				bool bIsPeaceMode = false ;
				//by Hades Kang - 평화 길드는 없어짐.
				/*
				GUILDLARGEINFOEX* pMyGuildInfo = pPanel->FindGuild( pSelfNode->m_ulGuildID ) ;
				if ( NULL != pMyGuildInfo )
				{
					bIsPeaceMode = pMyGuildInfo->IsPeaceMode() ;
				}
				*/

				// 평화길드 체크, 마을 체크, 진지를 지을수 있는 곳인지 체크
				bool bCampBuildEnableFrist = false, bCampBuildEnableSecond = false, bCampBuildEnableThird = true ;
				if ( CSceneManager::ms_pSceneEventMgr )
				{
					// edith 2009.01.16 마을 체크로직을 세이프티 체크로직으로 변경
					bCampBuildEnableFrist = CSceneManager::ms_pSceneEventMgr->IsEnableStructCamp() ;
				}

				bCampBuildEnableSecond = CSceneManager::m_CharacterManager.GetChrBuildEnable() ;

				float fHeight = CSceneManager::m_HeightField.GetHeight( (*pSelfCreature->GetPosition()) );
				if ( pSelfCreature->GetPosition()->y > fHeight + 10.0f )
				{
					bCampBuildEnableThird = false ;
				}

				if ( bCampBuildEnableFrist && bCampBuildEnableSecond && bCampBuildEnableThird && !bIsPeaceMode )
				{
					if ( CRYLGameData::Instance()->m_lpCampStartKit )
					{
						CItemInstance *lpItem = CRYLGameData::Instance()->m_lpCampStartKit  ;

						Item::CUseItem *lpStartKit = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
						if ( !lpStartKit ) 
							return ;

						CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
						g_pClientSocket->SetStatusFlag(NS_USEITEM);
						SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpStartKit->GetPos());
					}
				}
				else
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;

					if ( bIsPeaceMode )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3025 ] ) ;
					else if ( !bCampBuildEnableFrist )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3026 ] ) ;
					else if ( !bCampBuildEnableSecond )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3027 ] ) ;
					else if ( !bCampBuildEnableThird )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3028 ] ) ;
					else
						pMessageBox->Create( CRYLStringTable::m_strString[ 2667 ] ) ;
				}
			}
		}

		m_dwCampCreateResult = 0 ; 
	}
	else if ( m_dwCampCreateResult & MB_NO || m_dwCampCreateResult & MB_EXIT ) 
	{
		CRYLGameData::Instance()->m_lpCampStartKit = NULL ;

		m_dwCampCreateResult = 0 ;
	}

	
	// 월드 웨폰 생성 (월드 웨폰 생성에는 PopupSelectChrID 가 없다)
	if ( m_dwWorldWeaponCreateResult & MB_YES ) 
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( pSelfCreature )
		{
			CharDataNode* pSelfNode = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() ) ;
			if ( pSelfNode )
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

				bool bIsPeaceMode = false ;
				//by Hades Kang - 평화 길드는 없어짐.
				/*
				GUILDLARGEINFOEX* pMyGuildInfo = pPanel->FindGuild( pSelfNode->m_ulGuildID ) ;
				if ( NULL != pMyGuildInfo )
				{
				bIsPeaceMode = pMyGuildInfo->IsPeaceMode() ;
				}
				*/

				// 평화길드 체크, 마을 체크, 진지를 지을수 있는 곳인지 체크
				bool bCampBuildEnableFrist = false, bCampBuildEnableSecond = false, bCampBuildEnableThird = true ;
				if ( CSceneManager::ms_pSceneEventMgr )
				{
					bCampBuildEnableFrist = CSceneManager::ms_pSceneEventMgr->IsEnableStructCamp() ;
				}

				bCampBuildEnableSecond = CSceneManager::m_CharacterManager.GetChrBuildEnable() ;

				float fHeight = CSceneManager::m_HeightField.GetHeight( (*pSelfCreature->GetPosition()) );
				if ( pSelfCreature->GetPosition()->y > fHeight + 10.0f )
				{
					bCampBuildEnableThird = false ;
				}

				if ( bCampBuildEnableFrist && bCampBuildEnableSecond && bCampBuildEnableThird && !bIsPeaceMode )
				{
					if ( CRYLGameData::Instance()->m_lpWorldWeaponKit )
					{
						CItemInstance *lpItem = CRYLGameData::Instance()->m_lpWorldWeaponKit  ;
						Item::CUseItem *lpStartKit = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
						if ( !lpStartKit ) 
							return ;

						CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
						g_pClientSocket->SetStatusFlag(NS_USEITEM);
						SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpStartKit->GetPos());
					}
				}
				else
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;

					if ( bIsPeaceMode )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3025 ] ) ;
					else if ( !bCampBuildEnableFrist )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3026 ] ) ;
					else if ( !bCampBuildEnableSecond )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3027 ] ) ;
					else if ( !bCampBuildEnableThird )
						pMessageBox->Create( CRYLStringTable::m_strString[ 3028 ] ) ;
					else
						pMessageBox->Create( CRYLStringTable::m_strString[ 2667 ] ) ;
				}
			}
		}

		m_dwWorldWeaponCreateResult = 0 ; 
	}
	else if ( m_dwWorldWeaponCreateResult & MB_NO || m_dwWorldWeaponCreateResult & MB_EXIT ) 
	{
		CRYLGameData::Instance()->m_lpWorldWeaponKit = NULL ;

		m_dwWorldWeaponCreateResult = 0 ;
	}



	RYLCreature*  pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
	if ( !pTempCreature )
		return;

	// 성문 업그레이드
	if ( m_dwCastleDoorUpgradeResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleDoorUpgradePrice(), 
			0,
			PktCastleCmd::CASTLE_UPGRADE_GATE ) ;

		m_dwCastleDoorUpgradeResult = 0;
	}
	else if ( m_dwCastleDoorUpgradeResult & MB_CANCEL || m_dwCastleDoorUpgradeResult & MB_EXIT ) 
	{
		m_dwCastleDoorUpgradeResult = 0;
	}

	// 성문 수리
	if ( m_dwCastleDoorRepairResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleDoorRepairPrice(), 
			0,
			PktCastleCmd::CASTLE_REPAIR_GATE ) ;

		m_dwCastleDoorRepairResult = 0;
	}
	else if ( m_dwCastleDoorRepairResult & MB_CANCEL || m_dwCastleDoorRepairResult & MB_EXIT ) 
	{
		m_dwCastleDoorRepairResult = 0;
	}

	// 성문 복구
	if ( m_dwCastleDoorRecoveryResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleDoorRecoveryPrice(), 
			0,
			PktCastleCmd::CASTLE_RESTORE_GATE ) ;

		m_dwCastleDoorRecoveryResult = 0;
	}
	else if ( m_dwCastleDoorRecoveryResult & MB_CANCEL || m_dwCastleDoorRecoveryResult & MB_EXIT ) 
	{
		m_dwCastleDoorRecoveryResult = 0;
	}

	// 수성병기 개발
	if ( m_dwCastleArmsDevelopeResult & MB_CONFIRM )
	{
		CRYLGameData::SetAutoRun(FALSE);

		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleArmsDevelopePrice(m_dwArmsType),
			m_dwArmsType,
			PktCastleCmd::CASTLE_CREATE_ARMS ) ;

		m_dwArmsType = 0 ;
		m_dwCastleArmsDevelopeResult = 0 ;
	}
	else if ( m_dwCastleArmsDevelopeResult & MB_CANCEL || m_dwCastleArmsDevelopeResult & MB_EXIT )
	{
		m_dwArmsType = 0 ;
		m_dwCastleArmsDevelopeResult = 0 ;
	}

	// 수성병기 탑승
	if ( m_dwCastleArmsRideResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			0, 0,
			PktCastleCmd::CASTLE_RIDE_ARMS ) ;

		m_dwCastleArmsRideResult = 0 ;
	}
	else if ( m_dwCastleArmsRideResult & MB_CANCEL || m_dwCastleArmsRideResult & MB_EXIT )
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		pGame->SetEnableMove( TRUE ) ;
		m_dwCastleArmsRideResult = 0 ;
	}

	// 수성병기 수리
	if ( m_dwCastleArmsRepairResult & MB_CONFIRM )
	{
		if ( CRYLGameData::Instance()->m_csStatus.m_Info.Gold < pTempCreature->GetCastleArmsRepairPrice() )
		{
			pTempCreature->SetRepairFullHP( FALSE ) ;
		}

		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleArmsRepairPrice(), 0,
			PktCastleCmd::CASTLE_REPAIR_ARMS ) ;

		m_dwCastleArmsRepairResult = 0 ;
	}
	else if ( m_dwCastleArmsRepairResult & MB_CANCEL || m_dwCastleArmsRepairResult & MB_EXIT ) 
	{
		pTempCreature->SetRepairFullHP( TRUE ) ;
		m_dwCastleArmsRepairResult = 0;
	}

	// 수성병기 업그레이드
	if ( m_dwCastleArmsUpgradeResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			pTempCreature->GetCastleDoorUpgradePrice(), 0,
			PktCastleCmd::CASTLE_UPGRADE_ARMS ) ;

		m_dwCastleArmsUpgradeResult = 0 ;
	}
	else if ( m_dwCastleArmsUpgradeResult & MB_CANCEL || m_dwCastleArmsUpgradeResult & MB_EXIT ) 
	{
		m_dwCastleArmsUpgradeResult = 0;
	}

	// 수성병기 파괴
	if ( m_dwCastleArmsDestroyResult & MB_CONFIRM )
	{
		SendPacket::CharCastleCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			pTempCreature->GetCastleID(),   
			m_dwPopupSelectChrID, 
			0, 0,
			PktCastleCmd::CASTLE_DESTROY_ARMS ) ;

		m_dwCastleArmsDestroyResult = 0 ;
	}
	else if ( m_dwCastleArmsDestroyResult & MB_CANCEL || m_dwCastleArmsDestroyResult & MB_EXIT ) 
	{
		m_dwCastleArmsDestroyResult = 0;
	}

	// 공성병기 탑승
	if ( m_dwSiegeArmsRideResult & MB_CONFIRM )
	{
		SendPacket::CharSiegeArmsCmd( g_GameSession, 
			CRYLNetworkData::Instance()->m_dwMyChrID, 
			m_dwPopupSelectChrID, 
			PktSiegeArmsCmd::SIEGE_RIDE_ARMS );

		m_dwSiegeArmsRideResult = 0 ;
	}
	else if ( m_dwSiegeArmsRideResult & MB_CANCEL || m_dwSiegeArmsRideResult & MB_EXIT )
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		pGame->SetEnableMove( TRUE ) ;
		m_dwSiegeArmsRideResult = 0 ;
	}

	// 공성병기 스타터킷으로 복구
	if ( m_dwSiegeArmsStartKitRecoveryResult & MB_YES )
	{
		SendPacket::CharSiegeArmsCmd( g_GameSession,
			CRYLNetworkData::Instance()->m_dwMyChrID,
			m_dwPopupSelectChrID, PktSiegeArmsCmd::SIEGE_TO_STARTKIT ) ;

		m_dwSiegeArmsStartKitRecoveryResult = 0 ;
	}
	else if ( m_dwSiegeArmsStartKitRecoveryResult & MB_NO || m_dwSiegeArmsStartKitRecoveryResult & MB_EXIT ) 
	{
		m_dwSiegeArmsStartKitRecoveryResult = 0;
	}

	// 공성 병기 수리.
	if ( m_dwSiegeArmsRepair & MB_YES )
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		{
			SendPacket::CharSiegeArmsCmd( g_GameSession, pSelfCreature->GetCharID(), 
				m_dwPopupSelectChrID, PktSiegeArmsCmd::SIEGE_REPAIR_ARMS );
		}
		
		m_dwSiegeArmsRepair = 0 ;
	}
	else if ( m_dwSiegeArmsStartKitRecoveryResult & MB_NO || m_dwSiegeArmsStartKitRecoveryResult & MB_EXIT ) 
	{
		m_dwSiegeArmsRepair = 0;
	}	

	// 진지 손질하기
	if ( m_dwCampCareResult & MB_YES )
	{
		m_dwCampCareResult = 0 ;
	}
	else if ( m_dwCampCareResult & MB_NO || m_dwCampCareResult & MB_EXIT ) 
	{
		m_dwCampCareResult = 0 ;
	}

	// 진지 수리
	if ( m_dwCampRepairResult & MB_YES )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
		if ( pTempCreature ) 
		{
			CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
			if ( pCamp )
			{
				SendPacket::CharCampCmd( g_GameSession, 
					CRYLNetworkData::Instance()->m_dwMyChrID, 
					pCamp->GetCampID(), 
					m_dwCampRepairPrice, 
					0,
					PktCampCmd::CAMP_REPAIR ) ;
			}
		}

		m_dwCampRepairResult = 0 ;
	}
	else if ( m_dwCampRepairResult & MB_NO || m_dwCampRepairResult & MB_EXIT ) 
	{
		m_dwCampRepairResult = 0 ;
	}

	// 진지 파괴
	if ( m_dwCampDestoryResult & MB_YES )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID ) ;
		if ( pTempCreature ) 
		{
			CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() ) ;
			if ( pCamp )
			{
				SendPacket::CharCampCmd( g_GameSession, 
					CRYLNetworkData::Instance()->m_dwMyChrID, 
					pCamp->GetCampID(), 
					0, 
					0,
					PktCampCmd::CAMP_TO_STARTKIT ) ;
			}
		}

		m_dwCampDestoryResult = 0 ;
	}
	else if ( m_dwCampDestoryResult & MB_NO || m_dwCampDestoryResult & MB_EXIT )
	{
		m_dwCampDestoryResult = 0 ;
	}
}    

void CRYLRaceBase::UpdateGameInterface( BOOL& bClick, BOOL& bEdge )
{
	if (FALSE == bEdge)
	{
		if (m_bPopupShow)
		{
			bEdge = TRUE;
			bClick = TRUE;

			if (TRUE == UpdatePopup()) 
			{
				return;
			}

			else if (g_DeviceInput.KeyDown('W') || g_DeviceInput.KeyDown('S') || g_DeviceInput.KeyDown('A') || g_DeviceInput.KeyDown('D'))
			{
				m_bPopupShow = FALSE;
			}
		}

		if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 ||  //was ZONE3
			CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE)
		{
			if ( m_lstWindow[ WINDOW_STONESTATUE ]->m_bShow ) 
			{
				// 명예의 석상 리스폰 창
				m_lstWindow[ WINDOW_STONESTATUE ]->Update( bClick, bEdge );	
			}
		}
	}
}

void CRYLRaceBase::UpdateGameItem( BOOL& bClick, BOOL& bEdge )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return;

	if (bEdge)
	{
		m_dwNormalTargetChrID = 0xFFFFFFFF;

		if (g_CharacterData.m_bAutoTargetAttack)
			m_dwRangedWeaponTargetChrID = g_CharacterData.m_dwAutoTargetID;
		else
			m_dwRangedWeaponTargetChrID = 0xFFFFFFFF;

		if (!pSelfCreature->GetStillCasting())
			m_dwSpecialTargetChrID = 0xFFFFFFFF;
	} 
	else
	{
		if (!pGame->m_bMouseMode)
		{
			// 캐릭터 선택
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			m_dwNormalTargetChrID = pGame->GetScreenPosChr(*ptMousePos, TARGET_SELECTCHAR, 1000.0f);

			// edith  퀵스킬의 거리를 체크하기 위한 로직
			CRYLGameData::CheckTargetLength(m_dwNormalTargetChrID);

			if (pGame->m_csStatus.m_bRangedWeapon)
			{
				// edith 2009.01.29 아처 사정거리 로직.
				float fLength = 1800.0f;
				if (pGame->m_csStatus.m_Info.Class == CClass::Archer)
				{
					CItemInstance* lpWeapon = 
						pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];

					if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::BOW)
						fLength += 1200.0f;
					else if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::CROSSBOW)
						fLength += 600.0f;
				}
				else if (pGame->m_csStatus.m_Info.Class == CClass::Enchanter)
				{
					CItemInstance* lpWeapon = 
						pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];

					if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::BOW)
						fLength += 300.0f;
				}

				// 롱웨폰 크로스헤어용 타겟 아이디 설정
				m_dwRangedWeaponTargetChrID = pGame->GetScreenPosChr(*ptMousePos, Skill::Target::ENEMY, fLength);
				if (m_dwRangedWeaponTargetChrID != 0xFFFFFFFF)
				{
					if ( CRYLGameData::GetClientEnemyType( m_dwRangedWeaponTargetChrID ) == EnemyCheck::EC_ENEMY )
					{
						g_CharacterData.m_bAttackable = FALSE;

						CItemInstance* lpWeapon = 
							pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];
						CItemInstance* lpArrow = 
							pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos];

						// edith 무기가 장거리 무기일때 중앙 어택 포인터색 설정.
						if (NULL != lpWeapon && NULL != lpArrow)
						{
							if ((lpWeapon->GetItemType() == Item::ItemType::BOW && lpArrow->GetItemType() == Item::ItemType::ARROW) || 
								(lpWeapon->GetItemType() == Item::ItemType::CROSSBOW && lpArrow->GetItemType() == Item::ItemType::BOLT))
							{
								g_CharacterData.m_bAttackable = TRUE;
							}
						}
					} 
					else
					{
						g_CharacterData.m_bAttackable = FALSE;
						m_dwRangedWeaponTargetChrID = 0xFFFFFFFF;
					}
				}
				else
				{
					g_CharacterData.m_bAttackable = FALSE;
				}
			} 
			else
			{
				unsigned short wType = pSelfCreature->GetObjectType();

				if(EnemyCheck::IsCastleArms(wType) || EnemyCheck::IsSiegeArms(wType))
				{
					//if(pSelfCreature->GetRideEnable())
					{
						float fLength = 7000.0f;

						m_dwRangedWeaponTargetChrID = pGame->GetScreenPosChr(*ptMousePos, Skill::Target::ENEMY, fLength);
						if (m_dwRangedWeaponTargetChrID != 0xFFFFFFFF)
						{
							if ( CRYLGameData::GetClientEnemyType( m_dwRangedWeaponTargetChrID ) == EnemyCheck::EC_ENEMY )
							{
								g_CharacterData.m_bAttackable = TRUE;															
							} 
							else
							{
								g_CharacterData.m_bAttackable = FALSE;
								m_dwRangedWeaponTargetChrID = 0xFFFFFFFF;
							}
						}
						else
						{
							g_CharacterData.m_bAttackable = FALSE;
						}
					}
				}
				else
				{
					m_dwRangedWeaponTargetChrID = 0xFFFFFFFF;
					g_CharacterData.m_bAttackable = TRUE;
				}				
			}
			
			if (pGame->m_csStatus.m_lpQuickSelected && !pSelfCreature->GetStillCasting())
			{
				const Skill::ProtoType *pSkill = g_SkillMgr.GetSkillProtoType(pGame->m_csStatus.m_lpQuickSelected->GetSkillID());
				
				if ( pSkill )
				{					
					pSkill = &pSkill[pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount()];
					m_dwSpecialTargetChrID = pGame->GetScreenPosChr(*ptMousePos, pSkill->m_eTargetType, pSkill->m_fMaxRange * 100.0f);

					if(m_dwSpecialTargetChrID==pSelfCreature->GetCharID())
					{
						// edith 2009.01.29 아처 사정거리 로직.
						float fLength = 1800.0f;
						if (pGame->m_csStatus.m_Info.Class == CClass::Archer)
						{
							CItemInstance* lpWeapon = 
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];

							if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::BOW)
								fLength += 1200.0f;
							else if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::CROSSBOW)
								fLength += 600.0f;
						}
						else if (pGame->m_csStatus.m_Info.Class == CClass::Enchanter)
						{
							CItemInstance* lpWeapon = 
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];

							if(lpWeapon && lpWeapon->GetItemType() == Item::ItemType::BOW)
								fLength += 300.0f;
						}

						// 롱웨폰 크로스헤어용 타겟 아이디 설정
						m_dwRangedWeaponTargetChrID = pGame->GetScreenPosChr(*ptMousePos, Skill::Target::ENEMY, fLength);

						if(m_dwRangedWeaponTargetChrID!=0xFFFFFFFF)
						{
							m_dwSpecialTargetChrID = 0xFFFFFFFF;
						}
					}
				} 
				else
				{
					m_dwSpecialTargetChrID = 0xFFFFFFFF;
				}
			}
		} 
		else
		{
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			m_dwNormalTargetChrID = g_CharacterData.m_dwNormalTargetID;
			m_dwSpecialTargetChrID = g_CharacterData.m_dwSpecialTargetID;

			if (pGame->m_csStatus.m_bRangedWeapon)
			{
				g_CharacterData.m_bAttackable = FALSE;

				CItemInstance* lpWeapon = 
					pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos];
				CItemInstance* lpArrow = 
					pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos];

				if (NULL != lpWeapon && NULL != lpArrow)
				{
					if ((lpWeapon->GetItemType() == Item::ItemType::BOW && lpArrow->GetItemType() == Item::ItemType::ARROW) || 
						(lpWeapon->GetItemType() == Item::ItemType::CROSSBOW && lpArrow->GetItemType() == Item::ItemType::BOLT))
					{
						g_CharacterData.m_bAttackable = TRUE;
					}
				}
			} 
			else
			{
				g_CharacterData.m_bAttackable = TRUE;
			}

			m_dwRangedWeaponTargetChrID = g_CharacterData.m_dwNormalTargetMonsterID;
		}

		// NPC 판넬 찍기
		UpdateGameNPCPanel();

		if (!pSelfCreature->GetIsDead())
		{
			if (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
				!pGame->m_lpPickItem && RYLCreatureManager::Instance()->GetCreatureListHead() )
			{
				// 아이템 집기
				POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
				BOOL bOverMouse = FALSE;
				unsigned long nSelectItem = pGame->m_lpSceneManager->MouseOnCheckInstanceObject(ptMousePos->x, ptMousePos->y);
				if (nSelectItem != 0xFFFFFFFF)
				{
					FieldInstance* pInstanceItem = pGame->GetFieldObject(nSelectItem);
					if (pInstanceItem)
					{
						vector3 vecPos = *( pSelfCreature->GetPosition() );
						vecPos.y = 0.0f;
						vector3 vecItem = vector3(pInstanceItem->m_vecPos.x, 0.0f, pInstanceItem->m_vecPos.z);
						vector3 vecLen = vecPos - vecItem;
						if (vecLen.GetLens() < 300.0f)
						{
							bOverMouse = TRUE;
						} 
						else
						{
							bOverMouse = FALSE;
							nSelectItem = 0xFFFFFFFF;
						}
					} 
					else
					{
						bOverMouse = FALSE;
						nSelectItem = 0xFFFFFFFF;
					}
				}

				static CEffScript* pMousePointEff = NULL;
				if (pMousePointEff && nSelectItem == 0xFFFFFFFF)
				{
					pMousePointEff->SetStartPos(0.0f,0.0f,0.0f);			
					pMousePointEff->SetEndPos(0.0f,0.0f,0.0f);
				}

				if (nSelectItem == 0xFFFFFFFF)
				{
					if (pGame->m_lstFieldItem.size())
					{
						list<FieldInstance *>::iterator it;
						vector3 vecPos = *( pSelfCreature->GetPosition() );
						vecPos.y = 0.0f;
						vector3 vecItem, vecLen;
						for (it = pGame->m_lstFieldItem.begin(); it != pGame->m_lstFieldItem.end(); it++)
						{
							vecItem = vector3((*it)->m_vecPos.x, 0.0f, (*it)->m_vecPos.z);
							vecLen = vecPos - vecItem;
							if (vecLen.GetLens() < 300.0f)
							{
								nSelectItem = (*it)->m_dwInstanceID;
								break;
							}
						}
					}
				}

				if (nSelectItem != 0xFFFFFFFF)
				{
					if ((g_DeviceInput.GetIsLeftMouseDown() && bOverMouse) || (!CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() && 
						(g_DeviceInput.KeyDown('E') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_ITEM)) )) //hayzohar
					{
						if (true == PickItem(nSelectItem))
						{
							return;
						}
					}
					else
					{
						FieldInstance* pInstance = pGame->GetFieldObject(nSelectItem);
						if (pInstance)
						{
							vector3 vecItem = vector3(pInstance->m_vecPos.x, 0.0f, pInstance->m_vecPos.z);
							if (vecItem.y <= 50.0f)
								vecItem.y = CSceneManager::m_HeightField.GetHeight(vecItem);

							if (pMousePointEff == NULL)
							{
								pMousePointEff = new CEffScript;
								CSceneManager::AddEffectScript(pMousePointEff);
								pMousePointEff->GetScriptBinData("inter-item.esf");
							}		

							pMousePointEff->SetStartPos(vecItem.x, vecItem.y + 30.0f, vecItem.z);			
							pMousePointEff->SetEndPos(vecItem.x, vecItem.y + 30.0f,vecItem.z);												
						}					
					}
				}
				m_nPreSelectItem = nSelectItem;

				// NPC 선택하기
				if (m_dwNormalTargetChrID != 0xFFFFFFFF)
				{
					if (GetBusy())
					{
						g_CharacterData.m_bCharacterTargetNear = FALSE;
						g_CharacterData.m_dwNormalTargetID = 0xFFFFFFFF;
					} 
					else
					{
						if (SelectNPC(m_dwNormalTargetChrID, m_bClickButton)) 
							return;
					}
				}
			}

			// 아이템 버리기
			if (pGame->m_lpPickItem && g_DeviceInput.GetIsLeftMouseDown() &&
				(pGame->m_lpPickItem->GetItemPos() != TakeType::TS_EXCHANGE &&
				pGame->m_lpPickItem->GetItemPos() != TakeType::TS_DEPOSIT))			
			{
				// 스킬 아이콘
				if (TRUE == pGame->m_lpPickItem->m_bSkill)
				{
					if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_SSLOT)
					{
						delete pGame->m_lpPickItem;
						pGame->SetPickItem(NULL);
					} 
					else if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
					{	
						// 패킷을 보내서 아예 없애버린다. 
						CRYLNetworkData::SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_NONE, 0);
					}
				}
				// 일반 아이템
				else
				{
					if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[ 64 ]);
					}
					else
					{
						if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
						{
							CRYLNetworkData::SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_NONE, 0);
						} 
						else if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_STALL)
						{
							delete pGame->m_lpPickItem;
							pGame->SetPickItem(NULL);
						} 
						else
						{
							BOOL bAutoDrop = FALSE;
							if(g_DeviceInput.KeyHold(VK_SHIFT))
								bAutoDrop = TRUE;

							// edith 2009.08.19 B그레이드이상의 장비템이면 떨굴때 물어본다.
							Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(pGame->m_lpPickItem->m_lpItemBase);
							if(lpEquip && 
								(lpEquip->GetItemGrade().m_eItemGrade <= Item::EquipType::B_GRADE 
								|| lpEquip->GetMaxSocketNum() > 0 )	)
								bAutoDrop = FALSE;
						/*	else
								bAutoDrop = TRUE;*/

							//if(bAutoDrop == FALSE)//always ask to drop item
							//{
							/*	CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
								lpMessageBox->SetWordBreak(TRUE);
								lpMessageBox->Create(CRYLStringTable::m_strString[ 259 ], MB_YES | MB_NO | MB_EXIT);
								lpMessageBox->SetEnableEnter(FALSE);
								lpMessageBox->SetResult(&pGame->m_dwDropPickResult);
								pGame->m_dwDropPickResult = 0; */
							//}

						/*	if ( bAutoDrop && g_MessageBoxManager.m_lstMessageBox.empty() )
							{
								g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

								unsigned long dwNum = (pGame->m_lpPickItem->GetIsEnableStack()) ? pGame->m_lpPickItem->GetNowDurability() : 1;
								if(CRYLNetworkData::SendFieldObjectPullDown(dwNum, pGame->m_lpPickItem->m_lpItemBase->GetPos()))
									pGame->m_bPullDownFlag = TRUE;
								else
									pGame->m_bPullDownFlag = FALSE;
							}*/

						}
					}
				}

				return;
			}
		}
	}

	// 마우스에 집혀있는 아이템 위치 보정
	if (pGame->m_lpPickItem)		
	{
		POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
		pGame->m_lpPickItem->m_wPutX = ptMousePos->x;
		pGame->m_lpPickItem->m_wPutY = ptMousePos->y;
	}

	if (g_CharacterData.m_bFreelook)
	{
		if (CFrameTimer::GetUpdateTimer(g_CharacterData.m_nCharMovingPacketTimerID) >= 1.0f)
		{
			vector3 vecMove = *( pSelfCreature->GetPosition() );
			CRYLNetworkData::SendMovingPacket( CA_WAIT, CA_WAIT, 0.0f, vecMove);
		}
	} 
	else
	{
		CGMUICustomEdit* pEdit = NULL;
		CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );

		if ( bClick || pEdit->GetFocusState() )
		{
			g_CharacterData.UpdateCreatureControl(FALSE, bEdge);
		}
		else
		{
			CRYLGameScene* pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			if ( pGame->m_lpSceneManager->m_pCameraScript->ISPlay() || 
				pScene->GetCastleDoorblockDlg()->GetVisible() || pScene->GetGuildWarPenaltyDlg()->GetVisible() )
			{
				g_CharacterData.UpdateCreatureControl(FALSE, bEdge);
			} 
			else
			{
				g_CharacterData.UpdateCreatureControl(!bClick, bEdge);
			}
		}
	}

	if ( RYLCreatureManager::Instance()->GetCreatureListHead() == NULL )
		return;

	if (pGame->m_bMouseMode)
	{
		m_dwSpecialTargetChrID = g_CharacterData.m_dwSpecialTargetID;
	}

	if (!g_CharacterData.m_bAttackResult)
	{
		if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
		{
			unsigned long dwType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();

			switch (dwType)
			{
				case Item::ItemType::BOW:
					CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[151], 
						CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					break;

				case Item::ItemType::CROSSBOW:
					CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[152], 
						CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					break;
			}
		}

		g_CharacterData.m_bAttackResult = TRUE;
	}

	if (g_DeviceInput.GetIsRightMouseUp())
	{
		m_bShowSkillError = FALSE;
	}

	// 스킬 발동
	if ( ( g_CharacterData.m_bSkillExecuted || pSelfCreature->GetStillCasting() ) )
	{
		if(pGame->m_csStatus.m_lpQuickSelected)
		{
			unsigned short wItemID = pGame->m_csStatus.m_lpQuickSelected->GetProtoTypeID();

			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

			if (NULL == pSelfCreature)
			{
				return;
			}

			CItemInstance* lpItem = pGame->m_csStatus.m_lpQuickSelected;

			if (lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > pSelfCreature->GetRealmPoint())
			{			
				return;
			}

			if (true == lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
				!CRYLGameTimeManager::Instance()->IsRealmWarTime())
			{				
				return;
			}

			if(lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && CRYLGameTimeManager::Instance()->IsRealmWarTime() && 
				pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
			{
				return;
			}
		}

		ProcessSkill();
	}
}

void CRYLRaceBase::UpdateGameNPCPanel( void )
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	m_bClickButton = FALSE;

	vector<unsigned long> vecCharID;
	RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

	std::vector<unsigned long>::reverse_iterator rpos = vecCharID.rbegin();
	std::vector<unsigned long>::reverse_iterator rend = vecCharID.rend();

	for (; rpos != rend; ++rpos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*rpos); 
		if ( NULL == pTempCreature )
			continue;

		if ( pTempCreature->GetIsUsed() && !pTempCreature->GetIsDead() )
		{
			CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
			if ( NULL == pTempData )
				continue;

			vector3 vecResult;
			float fW;

			vector3 vecPos = *( pTempCreature->GetNamePosition() );

			BaseGraphicsLayer::TransformVector(vecPos, vecResult, fW);
			if (vecResult.z < 0.0f) continue;
			if (vecResult.z > 1.0f) continue;

			vecPos = *( pTempCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );

			if (vecPos.GetLens() < 2000.0f)
			{
				unsigned long dwLength = 0;

				switch ( CRYLGameData::GetClientIdentity(pTempCreature->GetCharID()))
				{
					case Creature::CT_PC:
					{
						if ( !pTempData->m_strStreetStallName.empty() )
						{
							dwLength = CRYLStringTable::GetStringWidth(pTempData->m_strStreetStallName.c_str());
							if (g_DeviceInput.InRect(vecResult.x - (dwLength / 2 + 21), vecResult.y - 20, 
								vecResult.x + (dwLength / 2 + 23), vecResult.y + 10))
							{
								m_dwNormalTargetChrID = pTempCreature->GetCharID();
								m_bClickButton = TRUE;
							}
						}
						break;
					}

					case Creature::CT_NPC:
					{
						float fHeight = 30.0f ;
						char szTemp[ MAX_PATH ], szName[ MAX_PATH ] ;
						strcpy( szTemp, pTempData->m_strName.c_str() ) ;

						char* lpToken = strtok( szTemp, "\\" ) ;
						if ( lpToken )
						{
							strcpy( szName, lpToken ) ;
							dwLength = CRYLStringTable::GetStringWidth( szName ) ;

							lpToken = strtok(NULL, "\\");
							if ( lpToken )
							{
								unsigned long dwJobLength = CRYLStringTable::GetStringWidth( lpToken ) ;
								dwLength = ( dwLength > dwJobLength ) ? dwLength : dwJobLength ;
								fHeight = 60.0f ;
							}
						}
						else
						{
							dwLength = CRYLStringTable::GetStringWidth( pTempData->m_strName.c_str() );
						}

						if (g_DeviceInput.InRect(vecResult.x - (dwLength / 2 + 21), vecResult.y, 
							vecResult.x + (dwLength / 2 + 23), vecResult.y + fHeight))
						{
							m_dwNormalTargetChrID = pTempCreature->GetCharID();
							m_bClickButton = TRUE;
						}
						break;
					}

					case Creature::CT_SIEGE_OBJECT:
					{
						dwLength = CRYLStringTable::GetStringWidth( pTempData->m_strName.c_str() );
						if (g_DeviceInput.InRect(vecResult.x - (dwLength / 2 + 21), vecResult.y, 
							vecResult.x + (dwLength / 2 + 23), vecResult.y + 30))
						{
							m_dwNormalTargetChrID = pTempCreature->GetCharID();
							m_bClickButton = TRUE;
						}
						break;
					}
				}
			}
		}
	}
}

