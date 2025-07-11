//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundClientProcess.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 5. Tuesday
//-----------------------------------------------------------------------------------------------------

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>
#include <Network/ClientSocket/Parse/ParseBGServer.h>
#include <Network/ClientSocket/Send/SendLoginout.h>

#include "BattleGroundClientProcess.h"
#include "../GameHandler.h"
#include "../../RYLClient/BattleGround Client/BattleGroundPrevResultDlg.h"
#include "../../RYLClient/BattleGround Client/BattleGroundStatusDlg.h"
#include "../../RYLClient/BattleGround Client/BattleGroundGameStatusDlg.h"
#include "../../RYLClient/BattleGround Client/BattleGroundChangeMoneyDlg.h"
#include "../../RYLClient/BattleGround Client/BattleGroundResultDlg.h"

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLRaceBaseDefine.h"
#include "RYLSceneManager.h"
#include "RYLLoadingScene.h"
#include "RYLGamedata.h"
#include "RYLGameScene.h"
#include "RYLNetWorkData.h"
#include "RYLStringTable.h"

#include "RYLMessageBox.h"
#include "RYLLargeMapFrameDlg.h"

#include <mmsystem.h>
#include "SoundMgr.h"

#include "RYLCreatureManager.h"

#include <string>
#include "GMMemory.h"
using namespace std;

GAME_EVENT_FUNC( ParseBGServerMapList );
GAME_EVENT_FUNC( ParseBGServerResultList );
GAME_EVENT_FUNC( ParseBGServerMoveZone );
GAME_EVENT_FUNC( ParseBGServerChangeMoney );

//-----------------------------------------------------------------------------------------------------
// Hander
bool GameProcess::RegisterBattleGroundClientHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;
    
    dwErrorCount += GameEventHandler.AddHandler( CmdBGServerMapList, ParseBGServerMapList ) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler( CmdBGServerResultList, ParseBGServerResultList ) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler( CmdBGServerMoveZone, ParseBGServerMoveZone ) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler( CmdBGServerMileageChange, ParseBGServerChangeMoney ) ? 0 : 1;

    return 0 == dwErrorCount;
}

//-----------------------------------------------------------------------------------------------------
// ParseBGServerMapList()
GAME_EVENT_FUNC( ParseBGServerMapList )
{
                bool    bAll;
    unsigned    char    cMapInfoNodeNum;
    BGServerMapInfoNode pMapInfo[PktBGServerMapList::MAX_MAP_NUM];

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBGServerMapList( lpPktBase, &bAll, &cMapInfoNodeNum, pMapInfo );

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CRYLGameScene*		            pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
        CRYLBattleGroundStatusDlg*      pStatusDlg      = ( CRYLBattleGroundStatusDlg* )pScene->GetBattleGroundStatusDlg();
        CRYLBattleGroundGameStatusDlg*  pGameStatusDlg  = ( CRYLBattleGroundGameStatusDlg* )pScene->GetBattleGroundGameStatusDlg();

        if( bAll )
        {
            ChannelInfo tempInfo;

            for( int h = 0 ; h < cMapInfoNodeNum ; ++h )
            {
                pStatusDlg->CheckChannelStart( h, pMapInfo[ h ].m_bPlaying );
            }

            pStatusDlg->ClearChannelInfo();

            for( int h = 0 ; h < cMapInfoNodeNum ; ++h )
            {
				tempInfo.m_ucChannelNum         = h + 1;
                tempInfo.m_bPlaying             = pMapInfo[ h ].m_bPlaying;
                tempInfo.m_cMapType             = pMapInfo[ h ].m_cMapType;
                tempInfo.m_cMaxCharNumOfNation  = pMapInfo[ h ].m_cMaxCharNumOfNation;
                tempInfo.m_cRemainMin           = pMapInfo[ h ].m_cRemainMin;
                tempInfo.m_wTargetScore         = pMapInfo[ h ].m_wTargetScore;

                for( int i = 0 ; i < ChannelInfo::MAX_RACE ; ++i )
                {
                    tempInfo.m_cCurrentCharNum[ i ] = pMapInfo[ h ].m_cCurrentCharNum[ i ];
                    tempInfo.m_wScore[ i ]          = pMapInfo[ h ].m_wScore[ i ];
                }

                pStatusDlg->ChangeChennelStatus( h, &tempInfo );
            }
        }
        else
        {
            GameInfo    tempInfo;

            tempInfo.m_cMapType             = pMapInfo[ 0 ].m_cMapType;
            tempInfo.m_cRemainMin           = pMapInfo[ 0 ].m_cRemainMin;
            tempInfo.m_wTargetScore         = pMapInfo[ 0 ].m_wTargetScore;

            for( int i = 0 ; i < ChannelInfo::MAX_RACE ; ++i )
            {
                tempInfo.m_cCurrentCharNum[ i ] = pMapInfo[ 0 ].m_cCurrentCharNum[ i ];
                tempInfo.m_wScore[ i ]          = pMapInfo[ 0 ].m_wScore[ i ];
            }

            pGameStatusDlg->ChangeGameStatus( &tempInfo );
        }
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------
// ParseBGServerResultList()
CEffScript*  g_effResult = NULL;

GAME_EVENT_FUNC( ParseBGServerResultList )
{
                bool        bAll;
    unsigned    char        cResultInfoNodeNum;
    BGServerResultInfoNode  lpResultInfo[PktBGServerMapList::MAX_MAP_NUM];

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBGServerResultList( lpPktBase, &bAll, &cResultInfoNodeNum, lpResultInfo );

    if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CRYLGameScene*		            pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
        
        if( bAll )
        {
            PrevChannelResult tempResult;
            CRYLBattleGroundPrevResultDlg*  pDlg = ( CRYLBattleGroundPrevResultDlg* )pScene->GetBattleGroundPrevResultDlg();
        
            for( int h = 0 ; h < cResultInfoNodeNum ; ++h )
            {
                tempResult.m_cWinRace = lpResultInfo[ h ].m_cWinRace;
                for( int i = 0 ; i < PrevChannelResult::MAX_RACE ; ++i )
                {
                    tempResult.m_wScore[ i ] = lpResultInfo[ h ].m_wScore[ i ];
                }

                pDlg->ChangePrevChannelResult( h, &tempResult );
            }

            pDlg->SetVisible( TRUE );
        }
        else
        {
            // "/smith"
            g_CharacterData.m_bFreelook = FALSE;

            // 게임 결과 넘기기
            GameResult tempGameResult;
            CRYLBattleGroundResultDlg*  pDlg = ( CRYLBattleGroundResultDlg* )pScene->GetBattleGroundResultDlg();

            tempGameResult.m_cWinRace	= lpResultInfo[ 0 ].m_cWinRace;
            tempGameResult.m_cPlayMin   = lpResultInfo[ 0 ].m_cPlayMin;
            tempGameResult.m_cKill      = lpResultInfo[ 0 ].m_cKill;
            tempGameResult.m_cKilled    = lpResultInfo[ 0 ].m_cKilled;
            tempGameResult.m_wAward     = lpResultInfo[ 0 ].m_wAward;
            for( int i = 0 ; i < GameInfo::MAX_RACE ; ++i )
            {
                tempGameResult.m_wScore[ i ] = lpResultInfo[ 0 ].m_wScore[ i ];
            }

            pDlg->SetGameResult( &tempGameResult );
            pDlg->SetVisible( TRUE );

            //-- 자신이 참여 했던 경기의 결과 --//
            CEffScript* effResult;
            string      strSoundFileName = g_ClientMain.m_strClientPath;

            effResult = new CEffScript;
            effResult->SetInterfaceSet(true);

            // 승패 결과에 따른 effect 로드
            if( lpResultInfo[ 0 ].m_cWinRace == 2 )
            {
                // Draw
                effResult->GetScriptBinData( "inter_draw.esf" );
                strSoundFileName += "\\sound\\am\\BG Client\\BG Client - Draw.ogg";
            }
            else if( CRYLGameData::Instance()->m_dwClientRace == lpResultInfo[ 0 ].m_cWinRace )
            {
                // Win
                effResult->GetScriptBinData( "inter_winner.esf" );
                strSoundFileName += "\\sound\\am\\BG Client\\BG Client - Win.ogg";
            }
            else
            {
                // Lose
                effResult->GetScriptBinData( "inter_loser.esf" );
                strSoundFileName += "\\sound\\am\\BG Client\\BG Client - Lose.ogg";
            }
        
            effResult->m_InterfacePos[0]  = CGMUIScreen::GetInstance()->GetWidth() / 2;
            effResult->m_InterfacePos[1]  = CGMUIScreen::GetInstance()->GetHeight() / 2 - 175;
            effResult->SetScale( 0.3f );  

            // 나중에 이펙트 지워주기 위해서 포인터를 잡고 있는다.
            g_effResult = CSceneManager::m_EffectManager.AddInterfaceScript( effResult );

            char    Temp[128] = { 0, };
            CStreamingSound* pPrevSound; 
            sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - 5 minutes remain.ogg", g_ClientMain.m_strClientPath );
            CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
            pPrevSound->Stop();
            sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Conquer.ogg", g_ClientMain.m_strClientPath );
            CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
            pPrevSound->Stop();
            sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Death match.ogg", g_ClientMain.m_strClientPath );
            CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
            pPrevSound->Stop();

            CStreamingSound* pCurrentSound; 
            CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, const_cast<char*>( strSoundFileName.c_str() ) ); 
			pCurrentSound->Play();
        }
    }
    return false;
}

//-----------------------------------------------------------------------------------------------------
// ParseBGServerMoveZone()
GAME_EVENT_FUNC( ParseBGServerMoveZone )
{
    unsigned char   cZone;
	unsigned char	cMoveType;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBGServerMoveZone( lpPktBase, &cZone, &cMoveType );

    if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CRYLNetworkData::Instance()->m_dwNextZone = CRYLNetworkData::Instance()->m_dwPresentZone;
	    CRYLNetworkData::Instance()->m_dwPresentZone = cZone;
	    CRYLGameData::Instance()->m_lpInterface->InitZone( CRYLNetworkData::Instance()->m_dwPresentZone );

        CRYLLoadingScene::Instance()->InitResourceObject( CRYLNetworkData::Instance()->m_dwPresentZone );
        CRYLGameData::Instance()->m_dwClientMode = CLIENT_GAMELOADING;
        CRYLGameData::Instance()->m_dwClientState = GAME_PLAYING;
    	
	    CRYLGameScene*		            pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
        CRYLBattleGroundGameStatusDlg*  pGameStatusDlg  = ( CRYLBattleGroundGameStatusDlg* )pScene->GetBattleGroundGameStatusDlg();
        CRYLBattleGroundStatusDlg*      pStatusDlg      = ( CRYLBattleGroundStatusDlg* )pScene->GetBattleGroundStatusDlg();
        CRYLLargeMapFrameDlg*			pLargeMapDlg	= ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
	    pLargeMapDlg->LoadLargeMapTexture() ;
        pLargeMapDlg->SetVisible( FALSE );

        pStatusDlg->SetVisible( FALSE );

        RYLCreature*            pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( !pSelfCreature )
            return false;

        CZ3DGeneralChrModel*    pSelfModel = pSelfCreature->GetCharModel();
        if( !pSelfModel )
            return false;

        switch( CRYLGameData::Instance()->m_dwClientRace )
        {
			case CClass::RaceType::HUMAN :
            {
                switch( cZone )
                {
	                case 10:
                    {
                        pSelfModel->SetPosition( vector3( 110571.0f, 3300.0f, 96689.0f) );
                        pSelfModel->SetDirection( 0.015f );
                        pGameStatusDlg->SetVisible( TRUE );

						// 네오모드 처리
						if (cMoveType == PktBGServerMoveZone::TYPE_SPECTATOR)
						{
							g_CharacterData.m_bFreelook = TRUE;
							g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
							g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();

							CSceneManager::GetCamera()->m_matPosition._41 = 110571.0f;
							CSceneManager::GetCamera()->m_matPosition._42 = 3500.0f;
							CSceneManager::GetCamera()->m_matPosition._43 = 96689.0f;
						}
                    }
                    break;

					case 11:
                    {
                        unsigned int unStartOrigin = rand() % 100;
                        pSelfModel->SetPosition( vector3( 216448.0f, 121000.0f, 100428.0f) );
                        pSelfModel->SetDirection( 1.580f );
                        
                        pGameStatusDlg->SetVisible( FALSE );
						g_CharacterData.m_bFreelook = FALSE;	// 네오모드 풀기 "/smith"

                        // effect 지워주기
                        if( g_effResult )
                        {
                            CSceneManager::m_EffectManager.DeleteInterfaceScript( g_effResult );
                            g_effResult = NULL;
                        }

                        CRYLBattleGroundResultDlg*  pGameResultDlg = ( CRYLBattleGroundResultDlg* )pScene->GetBattleGroundResultDlg();
                        pGameResultDlg->SetVisible( FALSE );
                    }
                    break;

					case 13:
                    {
                        pSelfModel->SetPosition( vector3( 109694.0f, 8600.0f, 97979.0f) );
                        pSelfModel->SetDirection( 0.067f );
                        
                        pGameStatusDlg->SetVisible( TRUE );

						// 네오모드 처리
						if (cMoveType == PktBGServerMoveZone::TYPE_SPECTATOR)
						{
							g_CharacterData.m_bFreelook = TRUE;
							g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
							g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();

							CSceneManager::GetCamera()->m_matPosition._41 = 110571.0f;
							CSceneManager::GetCamera()->m_matPosition._42 = 9000.0f;
							CSceneManager::GetCamera()->m_matPosition._43 = 96689.0f;
						}
                    }
                    break;
                }
            }
            break;

			case CClass::RaceType::AKHAN:
            {
                switch( cZone )
                {
					case 10:
                    {
                        pSelfModel->SetPosition( vector3( 110577.0f, 3300.0f, 123987.0f) );
                        pSelfModel->SetDirection( 3.145f );
                        
                        pGameStatusDlg->SetVisible( TRUE );

                        // 네오모드 처리
						if (cMoveType == PktBGServerMoveZone::TYPE_SPECTATOR)
						{
							g_CharacterData.m_bFreelook = TRUE;
							g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
							g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();
							CSceneManager::GetCamera()->m_matPosition._41 = 110577.0f;
							CSceneManager::GetCamera()->m_matPosition._42 = 3500.0f;
							CSceneManager::GetCamera()->m_matPosition._43 = 123987.0f;
						}
                    }
                    break;

					case 11:
                    {
                        pSelfModel->SetPosition( vector3( 211838.0f, 113141.0f, 184065.0f) );
                        pSelfModel->SetDirection( 1.580f );
                        
                        pGameStatusDlg->SetVisible( FALSE );

                        // effect 지워주기
                        if( g_effResult )
                        {
                            CSceneManager::m_EffectManager.DeleteInterfaceScript( g_effResult );
                            g_effResult = NULL;
                        }

                        CRYLBattleGroundResultDlg*  pGameResultDlg = ( CRYLBattleGroundResultDlg* )pScene->GetBattleGroundResultDlg();
                        pGameResultDlg->SetVisible( FALSE );
                    }
                    break;

					case 13:
                    {
                        pSelfModel->SetPosition( vector3( 110129.0f, 8600.0f, 123520.0f) );
                        pSelfModel->SetDirection( 3.094f );
                        
                        pGameStatusDlg->SetVisible( TRUE );

                        // 네오모드 처리
						if (cMoveType == PktBGServerMoveZone::TYPE_SPECTATOR)
						{
							g_CharacterData.m_bFreelook = TRUE;
							g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
							g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();
							CSceneManager::GetCamera()->m_matPosition._41 = 110129.0f;
							CSceneManager::GetCamera()->m_matPosition._42 = 9000.0f;
							CSceneManager::GetCamera()->m_matPosition._43 = 123520.0f;
						}
                    }
                    break;
                }
            }
            break;
        }
    }
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktBGServerMoveZone::FAIL_FULL_MAP:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2347 ]);
				break;

			case PktBGServerMoveZone::FAIL_FIX_RATE:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2348 ]);
				break;
		}
	}

    return false;
}

//-----------------------------------------------------------------------------------------------------
// ParseBGServerChangeMoney()
GAME_EVENT_FUNC( ParseBGServerChangeMoney )
{
    unsigned    long    dwCID;
    unsigned    char    cGroup;
    unsigned    char    cCmd;
    unsigned    long    dwGold;
    unsigned    long    dwMileage;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBGServerMileageChange( lpPktBase, &dwCID, &cGroup, &cCmd, &dwGold, &dwMileage );

    CRYLGameScene*		            pScene          = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
    CRYLBattleGroundChangeMoneyDlg* pChangeMoneyDlg = (CRYLBattleGroundChangeMoneyDlg*)pScene->GetBattleGroundChangeMoney();

    if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        switch( cCmd )
        {
        case PktBGServerMileageChange::PktBGSMCCmd::MC_INFO:
            {
                pChangeMoneyDlg->SetValue( cGroup, dwGold, dwMileage );

                pChangeMoneyDlg->SetAcceptButtonValidate( true );
            }
            break;

        case PktBGServerMileageChange::PktBGSMCCmd::MC_RESULT:
            {
                CRYLGameData::Instance()->m_csStatus.m_Info.Mileage = dwMileage;

                pChangeMoneyDlg->SetValue( cGroup, dwGold, dwMileage );
            }
            break;
        }
    }
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktBGServerMileageChange::NOT_USED_STORE:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2349 ]);
				break;

			case PktBGServerMileageChange::NOT_ENOUGH_MEDAL:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2350 ]);
				break;

			case PktBGServerMileageChange::MONEY_OVERFLOW:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2351 ]);
				break;

			case PktBGServerMileageChange::DATA_REQUESTING:
/*
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2352 ]);
*/
				break;

			case PktBGServerMileageChange::NONE_CHARACTER:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2353 ]);
				break;
		}

        pChangeMoneyDlg->SetAcceptButtonValidate( false );        
	}

    return false;
}
