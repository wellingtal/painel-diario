
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include <Creature/Character/CharacterClass.h>

#include "WinInput.h"
#include "FrameTimer.h"
#include "AmbienceManager.h"
#include "BGMManager.h"

#include "SkillSystem.h"

#include "RYLTimer.h"
#include "RYLMessageBoxManager.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLCharacterSelectScene.h"

#include "RYLChannelDlg.h"
#include "RYLCharacterInfoDlg.h"
#include "RYLCharacterCreationDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLNoticeManager.h"
#include "RYLOptionDlg.h"
// WORK_LIST 2.3 ���� ���� ���� ��� ����
#include "RYLNationSelectDlg.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "GMMemory.h"

void	CRYLRaceBase::UpdateChrSelect(void) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance();
	pGame->m_lpSceneManager->UpdateScene(CFrameTimer::GetUpdateTimer( GRYLTimer.GetSceneManagerTimerID() ));
	pGame->m_lpSceneManager->GetCamera()->PlayAnimate();

	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureListHead();
	for ( int i = 0; i < MAX_CHAR_SLOT; ++i )
	{
		if ( pTempCreature )
		{
			CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

			if ( pTempModel && pTempModel->IsActionFinished( 0 ) )
			{
				pTempModel->SetMotion( "WAIT" );
			}
		}

		pTempCreature = RYLCreatureManager::Instance()->GetCreatureListNext();
	}

	BOOL bClick = FALSE, bEdge = FALSE;
	CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::GetCurrentScene();

	switch (pGame->m_dwClientState)
	{
		case CHRSELECT_INITSELECT:
			( ( CRYLCharacterInfoDlg* )pScene->GetCharacterInfoDlg() )->InitValue();
			pGame->m_dwClientState = CHRSELECT_SELECT;
			break;

		case CHRSELECT_SELECT:
			( ( CRYLCharacterInfoDlg* )pScene->GetCharacterInfoDlg() )->CharacterSelctUpdate();
			break; 

		case CHRSELECT_INITSTART:
			( ( CRYLCharacterInfoDlg* )pScene->GetCharacterInfoDlg() )->InitDeleteValue();
			pGame->m_dwClientState = CHRSELECT_START;
			break;

		case CHRSELECT_SELECTCHANNEL:
			( ( CRYLChannelDlg* )pScene->GetChannelDlg() )->InitChannel();
			pScene->GetChannelDlg()->SetVisible( TRUE );
			pGame->m_dwClientState = -1 ;
			break;

		case CHRSELECT_START:
			( ( CRYLCharacterInfoDlg* )pScene->GetCharacterInfoDlg() )->EnableButton();
			( ( CRYLCharacterInfoDlg* )pScene->GetCharacterInfoDlg() )->Update( bClick, bEdge );
			break;

		case CHRSELECT_INITCREATE:
			( ( CRYLCharacterCreationDlg* )pScene->GetCharacterCreation() )->InitValue() ;
			( ( CRYLCharacterCreationDlg* )pScene->GetCharacterCreation() )->SetVisible( TRUE ) ;
			pGame->m_dwClientState = CHRSELECT_CREATE;
			break;

		case CHRSELECT_CREATE:
			( ( CRYLCharacterCreationDlg* )pScene->GetCharacterCreation() )->Update( bClick, bEdge ) ;
			break;
	}

	// WORK_LIST 2.3 ���� ���� ���� ��� ����
	if ( m_dwChangeNationResult & MB_YES )
	{
		// ���� ���� ���� ��Ŷ ����
		// 2 = TYPE_CHANGE
		CRYLNetworkData::SendSelectAccountNation( 2, ( ( CRYLNationSelectDlg* )pScene->GetNationSelectDlg() )->GetSelectNation() ) ;
		m_dwChangeNationResult = 0 ;
	}
	else if ( m_dwChangeNationResult & MB_NO || m_dwChangeNationResult & MB_EXIT )
	{
		m_dwChangeNationResult = 0 ;
	}
}

void CRYLRaceBase::UpdateGame( BOOL &bClick, BOOL &bEdge )
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	// ��ٿ� Ÿ�� üũ
	if ( pGame->m_csStatus.m_lpSiegeArmsSkill )
		UpdateGameSiegeCoolDownTimeCheck() ;
	else
		UpdateGameCoolDownTimeCheck() ;

	g_ClientMain.SetDayTimer();

	pGame->m_lpSceneManager->UpdateScene(CFrameTimer::GetUpdateTimer( GRYLTimer.GetSceneManagerTimerID() ));
	g_SkillSystem.Update();

	CRYLGameScene*			 pGameScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLLargeMapFrameDlg*	 pMapDlg		= ( CRYLLargeMapFrameDlg* )pGameScene->GetLargeMapDlg() ;

	if ( pMapDlg->GetRespawnEnable() ) 
	{
		bClick = TRUE;
		pGame->m_bShowCursor = TRUE;
	}

	if (pGame->m_bMouseMode)
	{
		// pGame->m_bShowCursor ���� FALSE �̸� ī�޶� ȸ���Ѵ�.
		// edith mouse
#ifdef _NEW_KEY_
		if (g_DeviceInput.KeyHold(VK_SPACE))
#else
		if (g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.KeyHold(VK_SPACE))
#endif
		{
			// �̰ɷ� ���콺�� ȸ���Ѵ�?
			pGame->m_bShowCursor = FALSE;
		} 
		else
		{
			if (pGameScene->GetNewTradeDlg()->GetVisible() || 
				pGameScene->GetExchangeDlg()->GetVisible() ||
				pGameScene->GetBlacksmithDlg()->GetVisible() ||
				pGameScene->GetStreetStallDlg()->GetVisible() ||
				pGameScene->GetVaultDlg()->GetVisible() ||
				pGameScene->GetStatusRetrainDlg()->GetVisible() ||
				pGameScene->GetJobChangeDlg()->GetVisible() ||
				pGameScene->GetMiningAmountProcessDlg()->GetVisible() ||
				pGameScene->GetMiningCampDlg()->GetVisible() ||
				pGameScene->GetMiningAmountDlg()->GetVisible() ||
				pGameScene->GetEmblemUpgradeDlg()->GetVisible() )
			{
				bClick = TRUE ;
			}

			// ä�� ���̾�αװ� ���� ������
			CRYLCharacterSelectScene* pCharSelectScene = 
				( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ;
			if ( pCharSelectScene->GetChannelDlg()->GetVisible() )
			{
				bClick = TRUE ;
			}

			pGame->m_bShowCursor = TRUE ;

#ifdef _NEW_KEY_
			// edith �������̽� ������ Ŭ���� �ȵǾ������� ȸ���� �ϰ��Ѵ�.
			if(g_DeviceInput.GetIsRightMousePress() && !bClick)
				pGame->m_bShowCursor = FALSE;
#endif
		}
	} 
	else
	{
		if (g_CharacterData.m_bFreelook)
		{
			pGame->m_bShowCursor = FALSE;
		}
		else
		{

			if ( g_DeviceInput.KeyHold(VK_SHIFT) || 
				pGameScene->GetNewTradeDlg()->GetVisible() || 
				pGameScene->GetExchangeDlg()->GetVisible() ||
				pGameScene->GetBlacksmithDlg()->GetVisible() ||
				pGameScene->GetStreetStallDlg()->GetVisible() ||
				pGameScene->GetCampShopDlg()->GetVisible() ||
				pGameScene->GetVaultDlg()->GetVisible() ||
				pGameScene->GetStatusRetrainDlg()->GetVisible() ||
				pGameScene->GetJobChangeDlg()->GetVisible() ||
				pGameScene->GetEmblemUpgradeDlg()->GetVisible() ||
				pGameScene->GetMiningAmountProcessDlg()->GetVisible() ||
				pGameScene->GetMiningCampDlg()->GetVisible() ||
				pGameScene->GetMiningAmountDlg()->GetVisible() ||
#ifdef _NEW_KEY_
				(g_MessageBoxManager.GetIsExistModal() || m_bPopupShow)
				&& !g_DeviceInput.GetIsRightMousePress()) // Minotaurs MsgBox�˻�
#else
				(g_MessageBoxManager.GetIsExistModal() || m_bPopupShow)
				&& !g_DeviceInput.GetIsMiddleMousePress())
#endif


			{
				bClick = TRUE;
				pGame->m_bShowCursor = TRUE;
			}
			else
			{
				if(pGame->m_bSimpleMouse)
					pGame->m_bShowCursor = TRUE;
				else
					pGame->m_bShowCursor = FALSE;
			}

			// ä�� ���̾�αװ� ���� ������
			CRYLCharacterSelectScene* pCharSelectScene  
				= ( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ;
			if ( pCharSelectScene->GetChannelDlg()->GetVisible() )
			{
				bClick = TRUE ;
			}

		}
	}

	// edith �ش�â�� ���������� �����͸� ��� 
//	if(pGame->m_bRYLVaultClick || pGame->m_bRYLBlacksmithClick || pGame->m_bRYLTradeDlgClick ||
//		pGame->m_bRYLStreetStallDlgClick || pGame->m_bRYLHumanInventoryFrameDlgClick || pGame->m_bRYLExchangeDlgClick )

	if(pGame->m_bRYLStreetStallDlgClick || pGame->m_bRYLHumanInventoryFrameDlgClick || pGame->m_bRYLExchangeDlgClick )
	{
		pGame->m_bShowCursor = TRUE;
	}

	// edith ���콺�� �߾����� �̵�. 
	if (!pGame->m_bShowCursor)
	{
		if (g_DeviceInput.GetIsFocus()) 
			g_DeviceInput.SetCursorPos( g_ClientMain.m_iScreenWidth / 2, ( g_ClientMain.m_iScreenHeight - 100 ) / 2 ) ;
	}

	float fCreatureUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID()) * 2.0f;

	// ��Ŷ ��ٸ���
	if (g_pClientSocket->GetStatusFlag() != NS_NOWAITING || 
		CRYLNetworkData::Instance()->m_nsNetworkStateforCastObject != NS_NOWAITING)		
	{
		g_CharacterData.SetCamera();
		g_CharacterData.UpdateCreatureControl(FALSE);
		NPCSpawn();
		g_CharacterData.Update(fCreatureUpdate);

		if (pGame->m_lpPickItem)		// ���콺�� �����ִ� ������ ��ġ ����
		{
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			pGame->m_lpPickItem->m_wPutX = ptMousePos->x;
			pGame->m_lpPickItem->m_wPutY = ptMousePos->y;
		}
		return;
	}

	// ���� ������Ʈ
	CRYLNoticeManager::Instance()->UpdateNotice( fCreatureUpdate ) ; 

	// ����Ű
	UpdateGame_W_KeyDown() ;

	// Quick Chatting Timer
	UpdateGame_NUMPAD_KeyDown() ;

	// SimpleMode
	UpdateGame_TAB_KeyDown( bClick, bEdge ) ;

	// �ΰ��� ���⸦ �ٲܼ� �ִ�.
	if ( CClass::RaceType::HUMAN == pGame->m_dwClientRace )
	{
		CRYLGameData::ChangeWeapon();
	}
	CRYLGameData::ChangeRide();

	UpdateGame_F_KeyDown() ;

	// AutoRun Mode
	UpdateGame_R_KeyDown() ;
	//UpdateGame_AltTab_KeyDown();//hz added this alt+tab disable


	// �̴ϸ� Ȯ��/���
	UpdateGame_ForMiniMap_KeyHold() ;

	// ���ӿ� ���õ� �κ�, �������ͽ� ���� �������
	UpdateGameAccelerator() ;

	if (m_bShowHelp)
	{
		// ����
		UpdateGameHelp( fCreatureUpdate ) ;
		return;
	}

	// ��ƿ ĳ���� ���� ������Ʈ
	switch ( pGame->m_dwClientRace )
	{
		case CClass::RaceType::HUMAN:	UpdateGameStillCasting( "ready_arrow.esf" ) ;		break;
		case CClass::RaceType::AKHAN:	UpdateGameStillCasting( "ready_akanarrow.esf" ) ;	break;
	}

	// ī�޶�, ũ���� ������Ʈ	
	UpdateGameEtc( fCreatureUpdate ) ;

	// �޽��� �ڽ� ������Ʈ
	UpdateGameMsgBox( bClick, bEdge ) ;

	// Alt + G ȿ��
	UpdateGameAltnG( bEdge ) ;

	// �������̽� ������Ʈ
	UpdateGameInterface( bClick, bEdge ) ;

	// ������ �� ��Ÿ ������Ʈ
	UpdateGameItem( bClick, bEdge ) ;
}

