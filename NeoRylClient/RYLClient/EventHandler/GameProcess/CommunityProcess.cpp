
#include "../GameHandler.h"
#include "RYLClientMain.h"
#include "RYLMessageBox.h"
#include "RYLTabButton.h"
#include "../../GUITextEdit.h"
#include "../LogFunctions.h"

//hz added for sound effect
#include "SoundMgr.h"

#include <Network/ClientSocket/Parse/ParseCommunity.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketCommand.h>

#include <Community/Guild/GuildConstants.h>
#include <Creature/Monster/MonsterMgr.h>

// ----------------------------------------------------------------------------

#include "RYLStringTable.h"

#include "RYLRaceBase.h"
#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLCommunityData.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildMemberPanel.h"
#include "RYLGuildTacticsPanel.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLHostilityDlg.h"

#include "RYLSocietyFrameDlg.h"
#include "RYLPartyPanel.h"
#include "RYLFriendPanel.h"
#include "RYLRefusalPanel.h"
#include "RYLPartyFindPanel.h"
#include "RYLMiniPartyDlg.h"

#include "RYLChattingDlg.h"
#include "RYLLargeMapFrameDlg.h"

#include "CommunityProcess.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "BroadcastProcess.h"
#include "RYLCampManager.h"
#include "RYLCamp.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharPartyInfo);
GAME_EVENT_FUNC(ParseCharPartyCmd);
GAME_EVENT_FUNC(ParseCharPartyCmdInfo);
GAME_EVENT_FUNC(ParseCharPartyMemData);
GAME_EVENT_FUNC(ParseCharPartyMemInfo);
GAME_EVENT_FUNC(ParseCharPartyFind);
GAME_EVENT_FUNC(ParseCharFriend);
GAME_EVENT_FUNC(ParseCharFriendRegistered);
GAME_EVENT_FUNC(ParseCharFriendDB);
GAME_EVENT_FUNC(ParseCharAuthorizePanel);
GAME_EVENT_FUNC(ParseCharFameInfo);
GAME_EVENT_FUNC(ParseCharDeadInfo);

// ----------------------------------------------------------------------------
// 길드 관련

GAME_EVENT_FUNC( ParseCharCreateGuild ) ;
GAME_EVENT_FUNC( ParseCharGuildCmd ) ;
GAME_EVENT_FUNC( ParseCharGuildMark ) ;
GAME_EVENT_FUNC( ParseCharGuildLevel ) ;
GAME_EVENT_FUNC( ParseCharGuildList ) ;
GAME_EVENT_FUNC( ParseCharGuildRight ) ;
GAME_EVENT_FUNC( ParseCharGuildMemberList ) ;
GAME_EVENT_FUNC( ParseCharMyGuildInfo ) ;
GAME_EVENT_FUNC( ParseCharGuildSafe ) ;
GAME_EVENT_FUNC( ParseCharGuildInclination ) ;
GAME_EVENT_FUNC( ParseCharGuildRelation ) ;
GAME_EVENT_FUNC( ParseCharGuildHostilityList ) ;
GAME_EVENT_FUNC( ParseCharGuildRelationInfo ) ;

/*void KillSpreeEffect(unsigned long KillCount, int KillSpreeLevel)
{		
	//hayzohar start
	char Temp2[MAX_PATH];//added this to Killstreak play
	char Temp1[10][MAX_PATH];//added this to stop play
	sprintf(Temp1[0], "%s\\Sound\\slain.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[1], "%s\\Sound\\doublekill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[2], "%s\\Sound\\triplekill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[3], "%s\\Sound\\quadrakill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[4], "%s\\Sound\\pentakill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[5], "%s\\Sound\\hexakill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[6], "%s\\Sound\\multikill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[7], "%s\\Sound\\ultrakill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[8], "%s\\Sound\\fantasticill.wav", g_ClientMain.m_strClientPath);
	sprintf(Temp1[9], "%s\\Sound\\unbelievable.wav", g_ClientMain.m_strClientPath);
	CSound* pOldSound; 
	for(int i = 0; i < 10; i++)
	{
		CSoundMgr::_GetInstance()->GetSound( pOldSound, Temp1[i] ); 
		if(pOldSound->IsPlaying())
			pOldSound->Stop();
	}

	CEffScript* test_script = new CEffScript;
	char Temp[MAX_PATH];
	int nLevel[3] = { 100, 300, 500 };
	switch(KillSpreeLevel)
	{
	case 5:
		sprintf(Temp2, "%s\\Sound\\dormanating.wav", g_ClientMain.m_strClientPath);
		break;

	case 8:
		sprintf(Temp2, "%s\\Sound\\godlike.wav", g_ClientMain.m_strClientPath);
		break;

	case 12:
		sprintf(Temp2, "%s\\Sound\\rampage.wav", g_ClientMain.m_strClientPath);
		break;

	case 18:
		sprintf(Temp2, "%s\\Sound\\Unstopable.wav", g_ClientMain.m_strClientPath);
		break;

	case 24:
		sprintf(Temp2, "%s\\Sound\\HolyShit.wav", g_ClientMain.m_strClientPath);
		break;

	case 30:
		sprintf(Temp2, "%s\\Sound\\WickedSick.wav", g_ClientMain.m_strClientPath);
		break;

	case 40:
		sprintf(Temp2, "%s\\Sound\\ComboWhore.wav", g_ClientMain.m_strClientPath);
		break;
	}
	switch(KillCount)
	{
	case 1:
		{
				test_script->GetScriptBinData("slain.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\slain.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 2:
		{
				test_script->GetScriptBinData("Double.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\doublekill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 3:
		{
				test_script->GetScriptBinData("triple.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\triplekill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 4:
		{
				test_script->GetScriptBinData("quadra.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\quadrakill.wav", g_ClientMain.m_strClientPath);
		}
		break;
	case 5:
		{
				test_script->GetScriptBinData("penta.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\pentakill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 6:
		{
				test_script->GetScriptBinData("hexa.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\hexakill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 7:
		{
				test_script->GetScriptBinData("multi.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\multikill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 8:
		{
				test_script->GetScriptBinData("ultra.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\ultrakill.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 9:
		{
				test_script->GetScriptBinData("fanta.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\fantastic.wav", g_ClientMain.m_strClientPath);
		}
		break;

	case 10:
		{
				test_script->GetScriptBinData("unbel.esf");
				test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
				sprintf(Temp, "%s\\Sound\\unbelievable.wav", g_ClientMain.m_strClientPath);
		}
		break;

	}
	CSound* pSound; 
	CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
	pSound->Play(); 
	
	//hayzohar end
}*/

bool GameProcess::RegisterCommunityHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount          = 0;
    unsigned long dwIgnoreNonPlayPacket = !(BIT(CLIENT_GAME) | BIT(CLIENT_GAMELOADING));

	dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyInfo, ParseCharPartyInfo, dwIgnoreNonPlayPacket) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyCmd, ParseCharPartyCmd, dwIgnoreNonPlayPacket) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyCmdInfo, ParseCharPartyCmdInfo, dwIgnoreNonPlayPacket) ? 0 : 1; 
    dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyMemData, ParseCharPartyMemData, dwIgnoreNonPlayPacket) ? 0 : 1; 
    dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyMemInfo, ParseCharPartyMemInfo, dwIgnoreNonPlayPacket) ? 0 : 1; 
    dwErrorCount += GameEventHandler.AddHandler(CmdCharPartyFind, ParseCharPartyFind) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdFriendAck, ParseCharFriend) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdFriendAddRequest, ParseCharFriendRegistered) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdFriendDB,    ParseCharFriendDB) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharAuthorizePanel, ParseCharAuthorizePanel) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharFameInfo, ParseCharFameInfo) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharDeadInfo, ParseCharDeadInfo) ? 0 : 1;

	// by ichabod
    dwErrorCount += GameEventHandler.AddHandler( CmdCreateGuild, ParseCharCreateGuild ) ? 0 : 1 ;	
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildCmd, ParseCharGuildCmd ) ? 0 : 1 ;		
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildMark, ParseCharGuildMark ) ? 0 : 1 ;	
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildLevel, ParseCharGuildLevel ) ? 0 : 1 ;	
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildList, ParseCharGuildList ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildRight, ParseCharGuildRight ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildMemberList, ParseCharGuildMemberList ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdMyGuildInfo, ParseCharMyGuildInfo ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildSafe, ParseCharGuildSafe ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildInclination, ParseCharGuildInclination ) ? 0 :1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildRelation, ParseCharGuildRelation ) ? 0 : 1 ;	
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildHostilityList, ParseCharGuildHostilityList ) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler( CmdGuildRelationInfo, ParseCharGuildRelationInfo ) ? 0 : 1 ;
	
    return 0 == dwErrorCount;
}





GAME_EVENT_FUNC(ParseCharPartyInfo)
{
	BOOL			bCreateParty = FALSE ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( 0 == pGame->m_csStatus.m_PartyInfo.m_cMemberNum )
	{
		bCreateParty = TRUE ;
	}

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPartyInfo(lpPktBase, &pGame->m_csStatus.m_PartyInfo);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        PARTY_EX& partyEx = pGame->m_csStatus.m_PartyInfo;

		CRYLGameScene*		 pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLSocietyFrameDlg* pDlg		= ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
		CRYLPartyPanel*		 pPanel		= ( CRYLPartyPanel* )pDlg->GetPartyPanel() ;
		CRYLPartyFindPanel*	 pFindPanel = (CRYLPartyFindPanel*)pDlg->GetPartyFindPanel();
		
		pPanel->UpdatePartyList() ;
		pPanel->UpdateControl() ;

		GRYLCommunityDataInstance()->m_bPartyFind = false;
		GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

		pFindPanel->UpdateCheckBox(GRYLCommunityDataInstance()->m_bPartyFind, GRYLCommunityDataInstance()->m_bPartyPlayerFind);


		if ( bCreateParty && 0 < pGame->m_csStatus.m_PartyInfo.m_cMemberNum )
		{
			CRYLSceneObject* pSceneObject = CRYLSceneManager::Instance()->GetCurrentScene();
			if ( GAME_SCENE == pSceneObject->GetSceneID() )
			{
				CRYLMiniPartyDlg* pMiniDlg = ( CRYLMiniPartyDlg* )pScene->GetMiniPartyDlg() ;
				pMiniDlg->SetVisible( TRUE ) ;
			}
		}

		return true;
	}
    else
	{
		pGame->m_csStatus.m_PartyInfo.m_cMemberNum = 0;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharPartyCmd)
{
	unsigned long	dwSenderID = 0, dwRecvID = 0, dwPartyID = 0;
	unsigned short	wCmd = 0;

	char			strName[ 32 ] ;
	AddressInfo		pAddressInfo;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	
    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPartyCmd(lpPktBase, &pAddressInfo, 
        strName, &dwSenderID, &dwRecvID, &dwPartyID, &wCmd);
	
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch(wCmd)
		{
        case PktPC::PC_INVITE:
			if (pGame->m_lpInterface)
			{
				if (pGame->m_lpInterface->GetBusy())
				{
                    CRYLNetworkData::SendPartyCmd(dwSenderID, PktPC::PC_REFUSE);
				}
                else
				{
					pGame->m_lpInterface->m_dwPartyCmdResult      = 0;
                    pGame->m_lpInterface->m_dwPartyCmdType        = PktPC::PC_INVITE;
					pGame->m_lpInterface->m_dwPartyCmdSenderID    = dwSenderID;
					char strText[200];
					sprintf(strText, CRYLStringTable::m_strString[276], strName);
					CRYLMessageBox *lpMessageBox;
					lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(strText, MB_EXIT | MB_YES | MB_NO);
					lpMessageBox->SetResult(&pGame->m_lpInterface->m_dwPartyCmdResult);
					lpMessageBox->SetEnableEnter(true);
					lpMessageBox->SetModal(FALSE);
				}
			}
			break;

        case PktPC::PC_ACCEPT:
			{
				char strText[200];
				sprintf(strText, CRYLStringTable::m_strString[277], strName);
				pGame->m_csStatus.AddPartyPlayer(dwSenderID, dwRecvID, strName);

				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

    			pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			}
			break;

        case PktPC::PC_REFUSE:
			{
				GRYLCommunityDataInstance()->AddRefuseList(dwSenderID);

				char strText[200];
				sprintf(strText, CRYLStringTable::m_strString[278], strName);
				pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
			break;

        case PktPC::PC_LOGIN:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

				SERVER_ID ServerID;
				ServerID.dwID = dwRecvID;
				
				// 서버 아이디가 다르면 나중에 추가 //
				if(ServerID.GetZone()==CRYLNetworkData::Instance()->m_dwPresentZone)
				{					
					char *strName = pGame->m_csStatus.LoginPartyPlayer(dwSenderID, dwRecvID);

					if (0 != strName)
					{
						int memberIndex = 0;
						for (; memberIndex < PARTY::MAX_MEM; ++memberIndex)
						{
							if (pGame->m_csStatus.m_PartyInfo.MemberCID[memberIndex] == dwSenderID)
							{
								break;                        
							}
						}

						if (memberIndex < PARTY::MAX_MEM)
						{
							CCharStatus::PartyExtraInfo& partyExInfo = 
								pGame->m_csStatus.m_aryPartyExtraInfo[memberIndex];

							if (0 == partyExInfo.m_cSkipLoginOutNum)
							{
								char strText[200];
								sprintf(strText, CRYLStringTable::m_strString[279], strName);
								pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
							}
							else 
							{
								--partyExInfo.m_cSkipLoginOutNum;
							}
						}
					}
				}				
            }
			break;

        case PktPC::PC_LOGOUT:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

                char *strName = pGame->m_csStatus.LogoutPartyPlayer(dwSenderID);

                if (0 != strName)
                {
                    int memberIndex = 0;
                    for (; memberIndex < PARTY::MAX_MEM; ++memberIndex)
                    {
                        if (pGame->m_csStatus.m_PartyInfo.MemberCID[memberIndex] == dwSenderID)
                        {
                            break;                        
                        }
                    }

                    if (memberIndex < PARTY::MAX_MEM)
                    {
                        CCharStatus::PartyExtraInfo& partyExInfo = 
                            pGame->m_csStatus.m_aryPartyExtraInfo[memberIndex];						

                        if (0 == partyExInfo.m_cSkipLoginOutNum)
                        {
                            char strText[200];
                            sprintf(strText, CRYLStringTable::m_strString[280], strName);
                            pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
                        }
                        else 
                        {
                            --partyExInfo.m_cSkipLoginOutNum;
                        }
                    }
                }
			}
			break;

        case PktPC::PC_BANISH:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

				if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					pGame->m_csStatus.DestroyParty();

					// 미니 파티창 닫기
					CRYLGameScene*		 pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLMiniPartyDlg*    pMiniDlg	= ( CRYLMiniPartyDlg* )pScene->GetMiniPartyDlg() ;
					pMiniDlg->SetVisible( FALSE ) ;

					pChat->AddMessage(CRYLStringTable::m_strString[281], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					CRYLNetworkData::SendCharShape();
				} 
				else
				{
					for (int i = 0; i < PARTY::MAX_MEM; i++)
					{
						if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
						{
							char strText[200];
							sprintf(strText, CRYLStringTable::m_strString[282], pGame->m_csStatus.m_PartyInfo.Name[i]);
							pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
							break;
						}
					}
					
					pGame->m_csStatus.DeletePartyPlayer(dwSenderID);
				}
			}
			break;

        case PktPC::PC_SECESSION:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

				if (dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					pGame->m_csStatus.DestroyParty();
					pChat->AddMessage(CRYLStringTable::m_strString[283], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
					CRYLNetworkData::SendCharShape();
				} 
				else
				{
					for (int i = 0; i < PARTY::MAX_MEM; i++)
					{
						if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
						{
							char strText[200];
							sprintf(strText, CRYLStringTable::m_strString[284], pGame->m_csStatus.m_PartyInfo.Name[i]);
							pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
							break;
						}
					}

					pGame->m_csStatus.DeletePartyPlayer(dwSenderID);
				}
			}
			break;

        case PktPC::PC_TRANSFER:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

				for (int i = 0; i < 10; i++)//was 10 hayzohar
				{
					if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
					{
						char strText[200];
						sprintf(strText, CRYLStringTable::m_strString[285], pGame->m_csStatus.m_PartyInfo.Name[i]);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						break;
					}
				}
				pGame->m_csStatus.m_PartyInfo.m_dwLeaderID = dwSenderID;
			}
			break;

        case PktPC::PC_DESTROY:
			{
				GRYLCommunityDataInstance()->m_bPartyFind = false;
				GRYLCommunityDataInstance()->m_bPartyPlayerFind = false;

				pGame->m_csStatus.DestroyParty();

				// 미니 파티창 닫기
				CRYLGameScene*		 pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLMiniPartyDlg*    pMiniDlg	= ( CRYLMiniPartyDlg* )pScene->GetMiniPartyDlg() ;
				pMiniDlg->SetVisible( FALSE ) ;

				pChat->AddMessage(CRYLStringTable::m_strString[286], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
			break;

        case PktPC::PC_FINDPARTY:
			GRYLCommunityDataInstance()->m_bPartyFind = dwRecvID;
			break;

        case PktPC::PC_FINDMEMBER:
			GRYLCommunityDataInstance()->m_bPartyPlayerFind = dwRecvID;
			break;

        case PktPC::PC_AUTOROUTING_ON:
			{
				if (dwSenderID)
				{
					for (int i = 0; i < 10; i++)//was 10 hayzohar
					{
						if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
						{
							pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[i] = true;
							char strText[200];
							sprintf(strText, CRYLStringTable::m_strString[483], pGame->m_csStatus.m_PartyInfo.Name[i]);
							pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
							break;
						}
					}
				} 
                else
				{
					for (int i = 0; i < 10; i++)//was 10 hayzohar
					{
						if (pGame->m_csStatus.m_PartyInfo.ServerID[i])
						{
							pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[i] = true;
						}
					}

					pChat->AddMessage(CRYLStringTable::m_strString[484], CRYLNetworkData::Instance()->m_dwMyChrID, 132);
				}
			}
			break;

        case PktPC::PC_AUTOROUTING_OFF:
			{
				if (dwSenderID)
				{
					for (int i = 0; i < 10; i++)//was 10 hayzohar
					{
						if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
						{
							pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[i] = false;
							char strText[200];
							sprintf(strText, CRYLStringTable::m_strString[485], pGame->m_csStatus.m_PartyInfo.Name[i]);
							pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
							break;
						}
					}
				} 
                else
				{
					for (int i = 0; i < 10; i++)//was 10 hayzohar
					{
						if (pGame->m_csStatus.m_PartyInfo.ServerID[i])
						{
							pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[i] = false;
						}
					}

					pChat->AddMessage(CRYLStringTable::m_strString[486], CRYLNetworkData::Instance()->m_dwMyChrID, 132);
				}
			}

			break;
		}
		
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLSocietyFrameDlg* pDlg   = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
		CRYLPartyPanel*		 pPanel = ( CRYLPartyPanel* )pDlg->GetPartyPanel() ;
		CRYLPartyFindPanel*	 pFindPanel = (CRYLPartyFindPanel*)pDlg->GetPartyFindPanel();
		
		pPanel->UpdatePartyList() ;
		pPanel->UpdateControl() ;

		pFindPanel->UpdateCheckBox(GRYLCommunityDataInstance()->m_bPartyFind, GRYLCommunityDataInstance()->m_bPartyPlayerFind);
		return true;
	} 
	else
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLSocietyFrameDlg* pDlg   = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
		CRYLPartyFindPanel*	 pFindPanel = (CRYLPartyFindPanel*)pDlg->GetPartyFindPanel();
		pFindPanel->UpdateCheckBox(GRYLCommunityDataInstance()->m_bPartyFind, GRYLCommunityDataInstance()->m_bPartyPlayerFind);

		CRYLMessageBox *lpMessageBox;
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktPC::CHAR_JOINED:	// 이미 파티에 가입되어 있는 경우 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[322]);
				break;

			case PktPC::FAIL_ACCEPT:	// 가입 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[323]);
				break;

			case PktPC::FAIL_BANISH:	// 퇴장 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[324]);
				break;

			case PktPC::FAIL_SECESSION:	// 탈퇴 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[325]);
				break;

			case PktPC::FAIL_TRANSFER:	// 양도 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[326]);
				break;

			case PktPC::OVER_MEMBER:	// 파티 인원 초과 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[327]);
				break;

			case PktPC::FAIL_REJECT:	// 파티 거부
				pChat->AddMessage(CRYLStringTable::m_strString[366], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case PktPC::ALREADY_PARTY:	// 이미 파티에 가입한 상태에서 파티찾기 시도
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2759]);
				break;

			case PktPC::FAIL_NOT_PARTY:	// 파티에 속하지 않고 파티원 찾기 시도
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2760]);
				break;

			case PktPC::FAIL_NOT_LEADER:	// 파티의 리더가 아닌 경우
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2761]);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharPartyCmdInfo)
{
	unsigned long dwMemberID = 0, dwPartyID = 0;
	unsigned short wPartyCmd = 0;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPartyCmdInfo(lpPktBase, &dwMemberID, &dwPartyID, &wPartyCmd);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        CharDataNode* pCharData = RYLCharacterDataManager::Instance()->GetCharData( dwMemberID );
        if ( !pCharData )
            return false;

        if ( pCharData )
        {
            switch(wPartyCmd)
			{
            case PktPC::PC_SECESSION:
            case PktPC::PC_BANISH:
                {
                    pCharData->m_ulPartyID = 0;
                }
				break;

            case PktPC::PC_ACCEPT:
                {
                    pCharData->m_ulPartyID = dwPartyID;
                }
				break;

            case PktPC::PC_DESTROY:
				{
                    RYLCharacterDataManager::Instance()->SwapPartyID( dwPartyID, 0 );
				}
				break;
			}
        }

		return true;
	}

	return false;
}


GAME_EVENT_FUNC(ParseCharPartyMemData)
{
    unsigned long   dwPartyID		= 0;	// 파티 아이디.
    unsigned long   dwSenderID		= 0;	// 보낸 아이디.
	unsigned long   dwGID			= 0;	// 이전 RenferenceID 도 공용으로 사용.
	unsigned long	dwServerID		= 0;	// 서버아이디.	
	unsigned short	wClass			= 0;	// 클래스.
	char			cLevel			= 0;	// 레벨.
    const char*     szSenderName	= 0;	// 보낸 캐릭터 이름.
    unsigned short  wCmd			= 0;	// 서버 명령어.

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPartyMemData(lpPktBase, dwPartyID, dwSenderID, dwGID, dwServerID, 
																	wClass, cLevel, szSenderName, wCmd);
    
    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
        char szChatMessage[MAX_PATH];

        switch(wCmd)
        {
			// 다른존에 있는 캐릭터에 정보가 넘어온다 //
			case PktDD::SCmdInsertPartyMem:
			case PktDD::SCmdLoginPartyMem:
				{
					CRYLGameData* pGame = CRYLGameData::Instance();

					for (int i = 0; i < PARTY::MAX_MEM; ++i)
					{
						if(pGame->m_csStatus.m_PartyInfo.MemberCID[i]==dwSenderID)
						{							
							pGame->m_csStatus.m_PartyInfo.m_cLevel[i]			= cLevel;
							pGame->m_csStatus.m_PartyInfo.m_wClass[i]			= wClass;
							pGame->m_csStatus.m_PartyInfo.ServerID[i]			= dwServerID;
							pGame->m_csStatus.m_PartyInfo.m_dwGID[i]			= dwGID;							
							pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[i]	= true;

							SERVER_ID ServerID;
							ServerID.dwID = dwServerID;

							if(ServerID.GetZone()!=CRYLNetworkData::Instance()->m_dwPresentZone && dwServerID!=0)
							{
								char *strName = pGame->m_csStatus.LoginPartyPlayer(dwSenderID, dwServerID);

								if (0 != strName)
								{
									int memberIndex = 0;
									for (; memberIndex < PARTY::MAX_MEM; ++memberIndex)
									{
										if (pGame->m_csStatus.m_PartyInfo.MemberCID[memberIndex] == dwSenderID)
										{
											break;                        
										}
									}

									if (memberIndex < PARTY::MAX_MEM)
									{
										CCharStatus::PartyExtraInfo& partyExInfo = 
											pGame->m_csStatus.m_aryPartyExtraInfo[memberIndex];

										if (0 == partyExInfo.m_cSkipLoginOutNum)
										{
											CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

											char strText[200];
											sprintf(strText, CRYLStringTable::m_strString[279], strName);
											pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
										}
										else 
										{
											--partyExInfo.m_cSkipLoginOutNum;
										}
									}
								}
							}							
							break;
						}
					}
				}
				break;

	        case PktDD::SCmdMoveZonePartyMem:            
				{
					SERVER_ID serverID;
					serverID.dwID = dwServerID;

					const char* szZoneName = CRYLNetworkData::Instance()->GetZoneName(serverID.GetZone());

					if (GameRYL::KOREA == CRYLNetworkData::Instance()->m_eInternationalCode)
					{
						sprintf(szChatMessage, CRYLStringTable::m_strString[ 2931 ], szSenderName, szZoneName) ;
					}
					else
					{
						sprintf(szChatMessage, CRYLStringTable::m_strString[ 2932 ], 
							szSenderName, szZoneName, serverID.GetChannel() + 1) ;
					}

					CRYLGameData* lpGame = CRYLGameData::Instance();

					for (int i = 0; i < PARTY::MAX_MEM; i++)
					{
						if (lpGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwSenderID)
						{
							lpGame->m_csStatus.m_aryPartyExtraInfo[i].m_cSkipLoginOutNum = 2;
					
							lpGame->m_csStatus.m_PartyInfo.ServerID[i]	= dwSenderID;
							
						}
					}

					CRYLChattingDlg::Instance()->AddMessage(szChatMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 132) ;					
				}
				break;

		    case PktDD::SCmdLevelUpPartyMem:
				{
					sprintf(szChatMessage, CRYLStringTable::m_strString[2933], szSenderName, cLevel) ;
					CRYLChattingDlg::Instance()->AddMessage(szChatMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 132) ;

					CRYLGameData* lpGame = CRYLGameData::Instance();

					for (int i = 0; i < PARTY::MAX_MEM; ++i)
					{
						if(lpGame->m_csStatus.m_PartyInfo.MemberCID[i]==dwSenderID)
						{
							lpGame->m_csStatus.m_PartyInfo.m_cLevel[i]	= cLevel;
						}
					}
				}				
				break;

			case PktDD::SCmdChangeClassPartyMem:
				{
					CRYLGameData* lpGame = CRYLGameData::Instance();

					for (int i = 0; i < PARTY::MAX_MEM; ++i)
					{
						if(lpGame->m_csStatus.m_PartyInfo.MemberCID[i]==dwSenderID)
						{
							lpGame->m_csStatus.m_PartyInfo.m_wClass[i]	= wClass;
						}
					}

					sprintf(szChatMessage, CRYLStringTable::m_strString[2934], 
						szSenderName, CRYLGameData::Instance()->GetClassName(wClass));

					CRYLChattingDlg::Instance()->AddMessage(szChatMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 132) ;
				}				
				break;
			case PktDD::SCmdChangeGuildPartyMem:
				{
					CRYLGameData* lpGame = CRYLGameData::Instance();

					for (int i = 0; i < PARTY::MAX_MEM; ++i)
					{
						if(lpGame->m_csStatus.m_PartyInfo.MemberCID[i]==dwSenderID)
						{							
							lpGame->m_csStatus.m_PartyInfo.m_dwGID[i]	= dwGID;
						}
					}					
				}				
				break;
        }

        return true;
    }

    return false;
}


GAME_EVENT_FUNC(ParseCharPartyMemInfo)
{
    PartyMemberData partyMemberData[PARTY::MAX_MEM];

    unsigned int nPartyMemberNum = PARTY::MAX_MEM;

    CRYLNetworkData::Instance()->m_dwNetworkError = 
        ParsePacket::HandleCharPartyMemInfo(lpPktBase, partyMemberData, nPartyMemberNum);

    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
        PartyMemberData* lpMemDataPos = partyMemberData;
        PartyMemberData* lpMemDataEnd = partyMemberData + nPartyMemberNum;

        CRYLGameData* lpGame = CRYLGameData::Instance();
        RYLCharacterDataManager* lpRylCharacterDataMgr = RYLCharacterDataManager::Instance();
        RYLCreatureManager* lpRylCreatureManager = RYLCreatureManager::Instance();

        PARTY_EX& partyEX = lpGame->m_csStatus.m_PartyInfo;

        for (; lpMemDataPos != lpMemDataEnd; ++lpMemDataPos)
        {
            unsigned long   dwCID = lpMemDataPos->m_dwCID;
            unsigned short  usDataChanged = lpMemDataPos->m_usDataChanged;
            
            for ( int i = 0 ; i < partyEX.m_cMemberNum ; ++i )
            {
                if ( dwCID == partyEX.MemberCID[ i ] )
                {
                    CCharStatus::PartyExtraInfo& partyExtraInfo = lpGame->m_csStatus.m_aryPartyExtraInfo[i];
                            
                    if (usDataChanged & PartyMemberData::CHANGED_XPOS)       		{ partyEX.m_Position[i].fPointX = lpMemDataPos->m_fXPos; }
                    if (usDataChanged & PartyMemberData::CHANGED_YPOS)       		{ partyEX.m_Position[i].fPointY = lpMemDataPos->m_fYPos; }                
                    if (usDataChanged & PartyMemberData::CHANGED_ZPOS)       		{ partyEX.m_Position[i].fPointZ = lpMemDataPos->m_fZPos; }
                    if (usDataChanged & PartyMemberData::CHANGED_MAX_HP)     		{ partyExtraInfo.m_usMaxHP  = lpMemDataPos->m_usMaxHP; }
                    if (usDataChanged & PartyMemberData::CHANGED_MAX_MP)     		{ partyExtraInfo.m_usMaxMP  = lpMemDataPos->m_usMaxMP; }
                    if (usDataChanged & PartyMemberData::CHANGED_CUR_HP)     		{ partyExtraInfo.m_usCurrHP = lpMemDataPos->m_usCurHP; }
                    if (usDataChanged & PartyMemberData::CHANGED_CUR_MP)     		{ partyExtraInfo.m_usCurrMP = lpMemDataPos->m_usCurMP; }
                    if (usDataChanged & PartyMemberData::CHANGED_LEVEL)      		{ partyEX.m_cLevel[i] = lpMemDataPos->m_cLevel; }
                    if (usDataChanged & PartyMemberData::CHANGED_CLASS)      		{ partyEX.m_wClass[i] = lpMemDataPos->m_cClass; }
					if (usDataChanged & PartyMemberData::CHANGED_ENCHANT_INFO_0)	{ partyExtraInfo.m_EnchantInfo.m_dwStatusFlag[0] = lpMemDataPos->m_dwEnchantInfo[0]; }
					if (usDataChanged & PartyMemberData::CHANGED_ENCHANT_INFO_1)    { partyExtraInfo.m_EnchantInfo.m_dwStatusFlag[1] = lpMemDataPos->m_dwEnchantInfo[1]; }
					if (usDataChanged & PartyMemberData::CHANGED_ENCHANT_INFO_2)    { partyExtraInfo.m_EnchantInfo.m_dwStatusFlag[2] = lpMemDataPos->m_dwEnchantInfo[2]; }
					if (usDataChanged & PartyMemberData::CHANGED_ENCHANT_INFO_3)    { partyExtraInfo.m_EnchantInfo.m_dwStatusFlag[3] = lpMemDataPos->m_dwEnchantInfo[3]; }
                }
            }
        }

        CRYLGameScene*		 lpScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
        CRYLSocietyFrameDlg* lpDlg		= ( CRYLSocietyFrameDlg* )lpScene->GetSocietyFrameDlg();
        CRYLPartyPanel*		 lpPanel	= ( CRYLPartyPanel* )lpDlg->GetPartyPanel();

        if ( lpPanel->GetVisible() )
        {
            lpPanel->UpdatePartyList();
            lpPanel->UpdateControl();
        }

        return true;
    }

    return false;
}

GAME_EVENT_FUNC(ParseCharDeadInfo)
{
    CRYLNetworkData::Instance()->m_dwNetworkError = lpPktBase->GetError();
        
    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
        PktDeadInfo* lpPktDeadInfo = static_cast<PktDeadInfo*>(lpPktBase);

        CRYLGameData* lpGame = CRYLGameData::Instance();

        if (lpPktDeadInfo->m_dwDeadCID != CRYLNetworkData::Instance()->m_dwMyChrID)
        {
            for (int i = 0; i < PARTY::MAX_MEM; ++i)
            {
                if (lpGame->m_csStatus.m_PartyInfo.MemberCID[i] == lpPktDeadInfo->m_dwDeadCID)
                {
                    CCharStatus::PartyExtraInfo& partyExtraInfo = lpGame->m_csStatus.m_aryPartyExtraInfo[i];

                    partyExtraInfo.m_usCurrHP = lpPktDeadInfo->m_usDeadMemCurrHP;
                    partyExtraInfo.m_usCurrMP = lpPktDeadInfo->m_usDeadMemCurrMP;
                }
            }
            
            const int MAX_BUFFER = 128;
            char szMessage[MAX_BUFFER] = "";

            unsigned long dwColor = 0xFFFFFFFF;
/*
			// 죽은 녀석이 나랑 국적이 다르면, 클래스 이름으로 대체한다.
			if ( 0 != lpPktDeadInfo->m_cDeadNation && lpGame->m_cAccountNation != lpPktDeadInfo->m_cDeadNation )
			{
				if (!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					strcpy( lpPktDeadInfo->m_szDeadName, CRYLStringTable::GetJobName( lpPktDeadInfo->m_cDeadClass ) ) ;
				}
			}
*/
            switch(lpPktDeadInfo->m_cTypeCode)
            {
            case PktDeadInfo::DEAD:

                if (Creature::CT_MONSTER == Creature::GetCreatureType(lpPktDeadInfo->m_dwReferenceID))
                {
                    const CMonsterMgr::MonsterProtoType* lpMonster = 
                        CMonsterMgr::GetInstance().GetMonsterProtoType(lpPktDeadInfo->m_dwReferenceID & 0x0000FFFF);

                    if (0 == lpMonster)
                    {
                        sprintf(szMessage, CRYLStringTable::m_strString[2935], lpPktDeadInfo->m_szDeadName);
                    }
                    else
                    {
                        sprintf(szMessage, CRYLStringTable::m_strString[2936], 
                            lpPktDeadInfo->m_szDeadName, lpMonster->m_MonsterInfo.m_strName);
                    }
                }
                else if (0 != lpPktDeadInfo->m_szRefGuildName[0])
                {
/*
					// 죽이거나 살린 녀석이 나랑 국적이 다르면, 클래스 이름으로 대체한다.
					if ( 0 != lpPktDeadInfo->m_cRefNation && lpGame->m_cAccountNation != lpPktDeadInfo->m_cRefNation )
					{
						if (!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
						{
							strcpy( lpPktDeadInfo->m_szRefName, CRYLStringTable::GetJobName( lpPktDeadInfo->m_cRefClass ) ) ;
						}
					}
*/
                    sprintf(szMessage, CRYLStringTable::m_strString[2937], lpPktDeadInfo->m_szDeadName, 
                        lpPktDeadInfo->m_szRefGuildName, lpPktDeadInfo->m_szRefName);
                }
                else
                {
					if (0 != lpPktDeadInfo->m_szRefName[0])
					{
/*
						// 죽이거나 살린 녀석이 나랑 국적이 다르면, 클래스 이름으로 대체한다.
						if ( 0 != lpPktDeadInfo->m_cRefNation && lpGame->m_cAccountNation != lpPktDeadInfo->m_cRefNation )
						{
							if (!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
							{
								strcpy( lpPktDeadInfo->m_szRefName, CRYLStringTable::GetJobName( lpPktDeadInfo->m_cRefClass ) ) ;
							}
						}
*/
						sprintf(szMessage, CRYLStringTable::m_strString[2938], 
							lpPktDeadInfo->m_szDeadName, lpPktDeadInfo->m_szRefName);
					}
					else
					{
						sprintf(szMessage, CRYLStringTable::m_strString[142], lpPktDeadInfo->m_szDeadName);
					}
                }

                dwColor = 133;
                break;

            case PktDeadInfo::RESPAWN:

                sprintf(szMessage, CRYLStringTable::m_strString[2939], 
                    lpPktDeadInfo->m_szDeadName);

                dwColor = 134;
                break;

            case PktDeadInfo::RESURRECT:

                if (0 != lpPktDeadInfo->m_szRefName[0])
                {
                    sprintf(szMessage, CRYLStringTable::m_strString[2940], 
                        lpPktDeadInfo->m_szRefName, lpPktDeadInfo->m_szDeadName);
                }
                else
                {
                    sprintf(szMessage, CRYLStringTable::m_strString[2941], 
                        lpPktDeadInfo->m_szDeadName);
                }

                dwColor = 134;
                break;
            }
            
            CRYLChattingDlg::Instance()->AddMessage(szMessage, CRYLNetworkData::Instance()->m_dwMyChrID, dwColor) ;

            // 파티원이면 파티창 갱신
            if (lpPktDeadInfo->m_dwDeadPID == lpGame->m_csStatus.m_PartyInfo.m_dwPartyID)
            {
                CRYLGameScene*		 lpScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
                CRYLSocietyFrameDlg* lpDlg	= ( CRYLSocietyFrameDlg* )lpScene->GetSocietyFrameDlg();
                CRYLPartyPanel*		 lpPanel	= ( CRYLPartyPanel* )lpDlg->GetPartyPanel();

                if ( lpPanel->GetVisible() )
                {
                    lpPanel->UpdatePartyList();
                    lpPanel->UpdateControl();
                }
            }
        }

        return true;
    }

    return false;
}

GAME_EVENT_FUNC(ParseCharPartyFind)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long dwChrID = 0;
	unsigned char cMemberFindPartyNum = 0, cPartyFindMemberNum = 0;
	LPMemberFindParty lpMember = 0;
	LPPartyFindMember lpParty = 0;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPartyFind(lpPktBase, &dwChrID, 
        &cMemberFindPartyNum, &cPartyFindMemberNum, &lpMember, &lpParty);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		unsigned long i;
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

		GRYLCommunityDataInstance()->m_cNumMember = cMemberFindPartyNum;
		ZeroMemory( GRYLCommunityDataInstance()->m_mfpMember, sizeof( GRYLCommunityDataInstance()->m_mfpMember));
		for (i = 0; i < cMemberFindPartyNum; i++)
		{
			strcpy( GRYLCommunityDataInstance()->m_mfpMember[i].m_strName, lpMember[i].m_strName);
			GRYLCommunityDataInstance()->m_mfpMember[i].m_cLevel = lpMember[i].m_cLevel;
			GRYLCommunityDataInstance()->m_mfpMember[i].m_cClass = lpMember[i].m_cClass;

			// 임시로 막아두기
/*			
    		char szMessage[ MAX_PATH ] ;
            sprintf( szMessage, CRYLStringTable::m_strString[ 2908 ],
				lpMember[i].m_strName, pGame->GetClassName(lpMember[i].m_cClass), lpMember[i].m_cLevel );

			pChat->AddChatMessage( szMessage, CRYLNetworkData::Instance()->m_dwMyChrID, PktChat::FIND_PARTY ) ;
			pChat->SaveText( szMessage, PktChat::FIND_PARTY ) ;
*/
		}

		GRYLCommunityDataInstance()->m_cNumParty = cPartyFindMemberNum;
		ZeroMemory(GRYLCommunityDataInstance()->m_pfmParty, sizeof(GRYLCommunityDataInstance()->m_pfmParty));
		for (i = 0; i < cPartyFindMemberNum; i++)
		{
			strcpy(GRYLCommunityDataInstance()->m_pfmParty[i].m_strLeaderName, lpParty[i].m_strLeaderName);
			GRYLCommunityDataInstance()->m_pfmParty[i].m_cAverageLevel = lpParty[i].m_cAverageLevel;
			GRYLCommunityDataInstance()->m_pfmParty[i].m_cMemberNum = lpParty[i].m_cMemberNum;
		}

		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLSocietyFrameDlg* pSocietyDlg    = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
		CRYLPartyFindPanel*	 pPartyFindDlg  = ( CRYLPartyFindPanel* )pSocietyDlg->GetPartyFindPanel() ;

		pPartyFindDlg->UpdateMemberForPartyMember() ;
		pPartyFindDlg->UpdatePartyForMemberParty() ;
		
		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharFriend)
{
	unsigned char	cCmd		= 0;
	unsigned long	dwCID		= 0;
	unsigned long	dwGID		= 0;
	unsigned short	wClass		= 0;
	char			cLevel		= 0;
	unsigned long	dwServerID	= 0;

	char*			strName = NULL ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharFriendAck(lpPktBase, &cCmd, &dwCID, &strName, &dwGID, &wClass, &cLevel, &dwServerID);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	char strMessage[MAX_PATH];
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch(cCmd)
		{
			// edith 2009.08.28 NoFriend 추가
			case PktFriendAck::NOFRIEND_ACK:
				// 같은 아이디이면 명령어를 내린녀석이고. 아이디가 다르면 삭제명령어를 받게된 유저.
				if(dwCID != CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					pGame->m_csStatus.DeleteFriend(dwCID);
					sprintf(strMessage, CRYLStringTable::m_strString[408], strName);
					pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
				else
				{
					sprintf(strMessage, CRYLStringTable::m_strString[414], strName);
					pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
				break;

			case PktFriendAck::ADD_FRIEND_ACK:
				pGame->m_csStatus.AddFriend(dwCID, strName, dwGID, wClass, cLevel, dwServerID);
				sprintf(strMessage, CRYLStringTable::m_strString[407], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::MOVE_BAN_TO_FRIEND:
				pGame->m_csStatus.DeleteBan(dwCID);
				pGame->m_csStatus.AddFriend(dwCID, strName, dwGID, wClass, cLevel, dwServerID);
				sprintf(strMessage, CRYLStringTable::m_strString[407], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::REMOVE_FRIEND_ACK:
				pGame->m_csStatus.DeleteFriend(dwCID);
				sprintf(strMessage, CRYLStringTable::m_strString[408], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::FRIEND_LOGIN:
				{
					FriendInfo *lpFriend = pGame->m_csStatus.GetFriend(dwCID);
					if (lpFriend)
					{
						lpFriend->SetLoginStatus(true);
						
						lpFriend->m_dwServerID	= dwServerID;
						lpFriend->m_wClass		= wClass;
						lpFriend->m_dwGID		= dwGID;
						lpFriend->m_cLevel		= cLevel;						

                        if (!pGame->m_csStatus.IsPartyMember(dwCID))
                        {
						    sprintf(strMessage, CRYLStringTable::m_strString[409], strName);
						    pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
                        }
                    } 
				}
				break;
			case PktFriendAck::BAN_LOGIN:
				{
					BanInfo *lpBan = pGame->m_csStatus.GetBan(dwCID);

					if(lpBan)
					{
						lpBan->m_dwServerID	=  dwServerID;						
						lpBan->m_cLevel		= cLevel;
						lpBan->m_wClass		= wClass;
						lpBan->m_dwGID		= dwGID;
					} 
				}
				break;
			case PktFriendAck::BAN_LOGOUT:
				{
					BanInfo *lpBan = pGame->m_csStatus.GetBan(dwCID);

					if(lpBan)
					{
						lpBan->m_dwServerID	=  0;						
					} 
				}
				break;

			case PktFriendAck::FRIEND_LOGOUT:
				{
					FriendInfo *lpFriend = pGame->m_csStatus.GetFriend(dwCID);
					if (lpFriend)
					{
						if(!lpFriend->IsMoveZone())
						{
							lpFriend->m_dwServerID	= 0;
						}						

						lpFriend->SetLoginStatus(false);

                        if (!pGame->m_csStatus.IsPartyMember(dwCID))
                        {
                            sprintf(strMessage, CRYLStringTable::m_strString[410], strName);
						    pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
                        }						
					}
				}
				break;

			case PktFriendAck::ADD_BAN_ACK:
				pGame->m_csStatus.AddBan(dwCID, strName, dwGID, wClass, cLevel, dwServerID);
				sprintf(strMessage, CRYLStringTable::m_strString[411], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::MOVE_FRIEND_TO_BAN:
				pGame->m_csStatus.DeleteFriend(dwCID);
				pGame->m_csStatus.AddBan(dwCID, strName, dwGID, wClass, cLevel, dwServerID);
				sprintf(strMessage, CRYLStringTable::m_strString[411], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::REMOVE_BAN_ACK:
				pGame->m_csStatus.DeleteBan(dwCID);
				sprintf(strMessage, CRYLStringTable::m_strString[412], strName);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::BAN_INFO_UPDATE:		// 거부 정보 업데이트.
				{
					BanInfo* lpBan = pGame->m_csStatus.GetBan(dwCID);
					
					if(lpBan)
					{			
						lpBan->m_cLevel		= cLevel;
						lpBan->m_wClass		= wClass;
						lpBan->m_dwGID		= dwGID;
						lpBan->m_dwServerID	= dwServerID;						
					}
				}
				break;

			case PktFriendAck::FRIEND_INFO_UPDATE:		// 친구 정보 업데이트.
				{
					FriendInfo *lpFriend = pGame->m_csStatus.GetFriend(dwCID);

					if(lpFriend)
					{			
						lpFriend->m_cLevel		= cLevel;
						lpFriend->m_wClass		= wClass;
						lpFriend->m_dwGID		= dwGID;
						lpFriend->m_dwServerID	= dwServerID;

						lpFriend->SetMoveZone(FriendInfo::MOVE_ZONE);
					}
				}
				break;
		}

		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLSocietyFrameDlg* pSocietyDlg = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
		CRYLFriendPanel*	 pFriendDlg  = ( CRYLFriendPanel* )pSocietyDlg->GetFriendPanel() ;
		CRYLRefusalPanel*	 pRefusalPanel = ( CRYLRefusalPanel* )pSocietyDlg->GetRefusalPanel() ;
		
		pFriendDlg->FriendListUpdate() ;
		pRefusalPanel->ResusalListUpdate() ;
		
		if(cCmd==PktFriendAck::FRIEND_INFO_UPDATE)
		{
			pFriendDlg->UpdateWhisperCharacter(strName) ;
		}
		else if(cCmd==PktFriendAck::BAN_INFO_UPDATE)
		{
			pRefusalPanel->UpdateWhisperCharacter(strName) ;
		}

		if(cCmd==PktFriendAck::MOVE_FRIEND_TO_BAN || cCmd==PktFriendAck::REMOVE_FRIEND_ACK 
			|| cCmd==PktFriendAck::FRIEND_LOGIN || cCmd==PktFriendAck::FRIEND_LOGOUT)
		{
			pFriendDlg->ClearWhisperCharacter(strName);
		}

		if(cCmd==PktFriendAck::MOVE_BAN_TO_FRIEND || cCmd==PktFriendAck::REMOVE_BAN_ACK 
			|| cCmd==PktFriendAck::BAN_LOGIN  || cCmd==PktFriendAck::BAN_LOGOUT)
		{
			pRefusalPanel->ClearWhisperCharacter(strName);
		}

		return true;
	}
	else
	{
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktFriendAck::NOT_LOGINED:
				pChat->AddMessage(CRYLStringTable::m_strString[ 3567 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::LIST_FULL:
				pChat->AddMessage(CRYLStringTable::m_strString[ 2909 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::REJECTED:
				pChat->AddMessage(CRYLStringTable::m_strString[ 2910 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;

			case PktFriendAck::FAIL_ENEMY:
				pChat->AddMessage(CRYLStringTable::m_strString[ 2911 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharFriendRegistered)
{
	char			strName[16] = "" ;
	unsigned long	dwChrID = 0 ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharFriendRegistered(lpPktBase, &dwChrID, strName);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		char strMessage[MAX_PATH];

        if (pGame->m_csStatus.GetIsFriend(dwChrID))
		{
            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if ( !pSelfCreature )
                return false;

            CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
            if ( !pSelfData )
                return false;

            sprintf(strMessage, CRYLStringTable::m_strString[413], strName, pSelfData->m_strName.c_str() );
			pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
		}
        else
		{
/*
			// 414번 스크립트는 nofriend 에서 사요하게 변경. 
			// 원래 414번 사용안했음.
			sprintf(strMessage, CRYLStringTable::m_strString[414], strName);

			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->SetWordBreak(true);
			lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&CRYLCommunityData::Instance()->m_dwFriendResult);
			CRYLCommunityData::Instance()->m_dwFriendResult = 0;
			strcpy(CRYLCommunityData::Instance()->m_strFriendName, strName);
*/

			sprintf(strMessage, CRYLStringTable::m_strString[ 2912 ], strName, strName);
			pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
		}

		return true;
	}

	return false;
}




GAME_EVENT_FUNC(ParseCharFriendDB)
{
    char *lpFriendList = NULL;
    unsigned long dwSize = 0;
    unsigned char cDataType = 0;
    CRYLGameData* pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharFriendDB(lpPktBase, &cDataType, &lpFriendList, &dwSize);

    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
        CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
        CRYLSocietyFrameDlg* pSocietyDlg = ( CRYLSocietyFrameDlg* )pScene->GetSocietyFrameDlg() ;
        CRYLFriendPanel*	 pFriendDlg  = ( CRYLFriendPanel* )pSocietyDlg->GetFriendPanel() ;
        CRYLRefusalPanel*	 pRefusalPanel = ( CRYLRefusalPanel* )pSocietyDlg->GetRefusalPanel() ;

        switch(cDataType)
        {
        case PktFriendDB::FRIEND_LIST:
            {
                unsigned long dwNum = dwSize / sizeof(FriendInfo);

                FriendInfo *lpFriendInfo;
                for (unsigned long i  = 0; i < dwNum; i++)
                {
                    lpFriendInfo = new FriendInfo;

                    lpFriendInfo->m_dwCID			= ((FriendInfo *)lpFriendList)->m_dwCID;
                    lpFriendInfo->m_dwStatusFlag	= ((FriendInfo *)lpFriendList)->m_dwStatusFlag;
					lpFriendInfo->m_dwGID			= ((FriendInfo *)lpFriendList)->m_dwGID;
					lpFriendInfo->m_wClass			= ((FriendInfo *)lpFriendList)->m_wClass;
					lpFriendInfo->m_cLevel			= ((FriendInfo *)lpFriendList)->m_cLevel;
					lpFriendInfo->m_dwServerID		= ((FriendInfo *)lpFriendList)->m_dwServerID;
                    
					strcpy(lpFriendInfo->m_szName, ((FriendInfo *)lpFriendList)->m_szName);
                    pGame->m_csStatus.m_lstFriendList.push_back(lpFriendInfo);
                    lpFriendList += sizeof(FriendInfo);
                }
                pFriendDlg->FriendListUpdate() ;
            }
            break;

        case PktFriendDB::BAN_LIST:
            {
                unsigned long dwNum = dwSize / sizeof(BanInfo);
                BanInfo *lpBanInfo;
                for (unsigned long i  = 0; i < dwNum; i++)
                {
                    lpBanInfo = new BanInfo;
                    lpBanInfo->m_dwCID		= ((BanInfo *)lpFriendList)->m_dwCID;
					lpBanInfo->m_dwGID		= ((BanInfo *)lpFriendList)->m_dwGID;
					lpBanInfo->m_wClass		= ((BanInfo *)lpFriendList)->m_wClass;
					lpBanInfo->m_cLevel		= ((BanInfo *)lpFriendList)->m_cLevel;
					lpBanInfo->m_dwServerID	= ((BanInfo *)lpFriendList)->m_dwServerID;

                    strcpy(lpBanInfo->m_szName, ((BanInfo *)lpFriendList)->m_szName);
                    pGame->m_csStatus.m_lstBanList.push_back(lpBanInfo);
                    lpFriendList += sizeof(BanInfo);
                }

                pRefusalPanel->ResusalListUpdate() ;
            }
            break;
        }

        return true;
    }

    return false;
}



GAME_EVENT_FUNC(ParseCharAuthorizePanel)
{
	unsigned long	dwCasterID = 0;
	char			strCasterName[16] = "";
	unsigned char	cCmd = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAuthorizePanel(lpPktBase, &dwCasterID, strCasterName, &cCmd);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
			char strMessage[MAX_PATH];

			switch(cCmd)
			{
				case ClientConstants::APCmd_Resurrection:
				{
					// 리스폰 돈 후에 레저렉션 창이 뜨는것을 막는다.
					if ( pGame->m_csStatus.m_Info.HP == 0 )
					{
						if (pGame->m_lpInterface)
						{
							CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							CRYLLargeMapFrameDlg*	 pMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;
							pMapDlg->SetShowLargeMap( FALSE ) ;					
							pMapDlg->SetVisible( FALSE ) ;
						}
						sprintf(strMessage, CRYLStringTable::m_strString[422], strCasterName);

						CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
						lpMessageBox->SetWordBreak(true);
						lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
						lpMessageBox->SetResult(&pGame->m_dwAuthorizeResult);
						pGame->m_dwAuthorizeResult = 0;
						pGame->m_dwAuthorizeCasterID = dwCasterID;
						pGame->m_cAuthorizeCmd = cCmd;
					}
				}
				break;

			case ClientConstants::APCmd_Recall:
				{
					// 죽은 상태로 리콜이 되는것은 막는다.
					if ( static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP) > 0 )
					{
						sprintf(strMessage, CRYLStringTable::m_strString[423], strCasterName);

						CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
						lpMessageBox->SetWordBreak(true);
						lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
						lpMessageBox->SetResult(&pGame->m_dwAuthorizeResult);
						//lpMessageBox->SetEnableBack(TRUE);
						//lpMessageBox->SetModal(FALSE);
						pGame->m_dwAuthorizeResult = 0;
						pGame->m_dwAuthorizeCasterID = dwCasterID;
						pGame->m_cAuthorizeCmd = cCmd;
					}
				}
				break;
			}


		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharFameInfo)
{
	unsigned long	KillCount=0;
	unsigned long	Times[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned long	dwChrID = 0;
	unsigned char	cOtherNation = 0;
	unsigned char	cOtherClass = 0;
	unsigned long	dwFame = 0;
	unsigned long	dwMileage = 0;
	unsigned char	cRaceRank = 0;
	unsigned char	cClassRank = 0;
	unsigned char	cCmd = 0;
	int				KillSpreeLevel = 0;
	char	szWinCharName[PktFIAck::MAX_NAME_LEN];
	char	szLoseCharName[PktFIAck::MAX_NAME_LEN];

	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharFameInfo(lpPktBase, &dwChrID,
																					&cOtherNation, &cOtherClass,
                                                                                    &dwFame, &dwMileage,
																					&cRaceRank, &cClassRank,
																					&cCmd,
																					szWinCharName, szLoseCharName, &KillCount, &KillSpreeLevel);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	//if(((cCmd >= 5)&&(cCmd <= 8))||(cCmd == 14))
	//	KillSpreeEffect(KillCount,KillSpreeLevel);

//	pGame->m_csStatus.SetKillStreak(KillSpreeLevel);

/*	if((cCmd >= 1)&&(cCmd <= 3))
	{
		char Temp[MAX_PATH];
		int nLevel[3] = { 100, 300, 500 };
		sprintf(Temp, "%s\\Sound\\islain.wav", g_ClientMain.m_strClientPath);
		CSound* pSound; 
		CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
		pSound->Play(); 

		if( KillSpreeLevel >= 5 )
		{
			char Temp2[MAX_PATH];
			int nLevel2[3] = { 100, 300, 500 };
			sprintf(Temp2, "%s\\Sound\\ShutDown.wav", g_ClientMain.m_strClientPath);
			CSound* pSound2; 
			CSoundMgr::_GetInstance()->GetSound( pSound2, Temp2 ); 
			pSound2->Play(); 
		}
	}*/
	

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		char strText[MAX_PATH];

		switch (cCmd)
		{
			case PktFIAck::FAME_INFO:
				break;

			case PktFIAck::FAME_LOSE_DUEL:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2762], szWinCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2762], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2762], szWinCharName);
				}
                break;

			case PktFIAck::FAME_LOSE_CONTINUE:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[493], szWinCharName);
//					sprintf(strText, CRYLStringTable::m_strString[493], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[493], szWinCharName);
				}
				break;

			case PktFIAck::FAME_LOSE:	
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2763], szWinCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2763], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2763], szWinCharName);
				}
				break;

			case PktFIAck::FAME_LOSE_MEMBER:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2794], szLoseCharName, szWinCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2794], szLoseCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2794], szLoseCharName, szWinCharName);
				}
				break;





			case PktFIAck::FAME_WIN_DUEL:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2765], szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2765], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2765], szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN_LARGE_GAP:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2764], szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2764], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2764], szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN_CONTINUE:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[487], szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[487], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[487], szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN:		
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2766], szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2766], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2766], szLoseCharName);
				}
				break;


			case PktFIAck::FAME_WIN_MEMBER_DUEL:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2795], szWinCharName, szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2795], szWinCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2795], szWinCharName, szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN_MEMBER_LARGE_GAP:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2796], szWinCharName, szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2796], szWinCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2796], szWinCharName, szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN_MEMBER_CONTINUE:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2797], szWinCharName, szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2797], szWinCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2797], szWinCharName, szLoseCharName);
				}
				break;

			case PktFIAck::FAME_WIN_MEMBER:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[2798], szWinCharName, szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[2798], szWinCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[2798], szWinCharName, szLoseCharName);
				}
				break;

			case PktFIAck::FAME_LOSE_LEVELMIN:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[496], szWinCharName);
//					sprintf(strText, CRYLStringTable::m_strString[496], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[496], szWinCharName);
				}
				break;

			case PktFIAck::FAME_WIN_LEVELMIN:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[497], szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[497], CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[497], szLoseCharName);
				}
				break;

				// 2797 따라
			case PktFIAck::FAME_WIN_MEMBER_LEVELMIN:
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (0 != cOtherNation && pGame->m_cAccountNation != cOtherNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strText, CRYLStringTable::m_strString[498], szWinCharName, szLoseCharName);
//					sprintf(strText, CRYLStringTable::m_strString[498], szWinCharName, CRYLStringTable::GetJobName(cOtherClass));
				}
				else
				{
					sprintf(strText, CRYLStringTable::m_strString[498], szWinCharName, szLoseCharName);
				}
				break;
		}

		if (PktFIAck::FAME_INFO != cCmd)
		{
			// 살해 정보 메세지
			pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
		}

		// --------------------------------------------------------------------------------------------------------

		char strRace[MAX_PATH], strClass[MAX_PATH];

		if (0 != cRaceRank) 
		{
			sprintf(strRace, CRYLStringTable::m_strString[363], cRaceRank); 
		}
		else 
		{
			strcpy(strRace, CRYLStringTable::m_strString[369]);
		}

		if (0 != cClassRank) 
		{
			sprintf(strClass, CRYLStringTable::m_strString[363], cClassRank); 
		}
		else 
		{
			strcpy(strClass, CRYLStringTable::m_strString[369]);
		}

		unsigned long dwFameVariation = 0;
		unsigned long dwMileageVariation = 0;

		switch (cCmd)
		{
			case PktFIAck::FAME_LOSE:	
				dwFameVariation = pGame->m_csStatus.m_Info.Fame - dwFame;
				sprintf(strText, CRYLStringTable::m_strString[477], 
					dwFame, dwFameVariation, dwMileage, strRace, strClass);
				break;

			case PktFIAck::FAME_WIN:	
			case PktFIAck::FAME_WIN_MEMBER:	
				dwFameVariation = dwFame - pGame->m_csStatus.m_Info.Fame;
				dwMileageVariation = dwMileage - pGame->m_csStatus.m_Info.Mileage;
				sprintf(strText, CRYLStringTable::m_strString[478], 
					dwFame, dwFameVariation, dwMileage, dwMileageVariation, strRace, strClass);
				break;

			default:
				sprintf(strText, CRYLStringTable::m_strString[364], dwFame, dwMileage, strRace, strClass);
				break;
		}

		// 명성 정보 메세지
		pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

		pGame->m_csStatus.m_Info.Fame = dwFame;
		pGame->m_csStatus.m_Info.Mileage = dwMileage;

		CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
		pSelfData->m_ulFame = dwFame;
		return true;
	} 
	
	return false;
}


//================================================================================
// by ichabod
// 길드 관련
//================================================================================
GAME_EVENT_FUNC( ParseCharCreateGuild ) 
{
	unsigned long	dwChrID = 0, dwGuildID = 0;
	char			buff[ Guild::MAX_GUILD_NAME_LEN ] ;
	unsigned char	cNation ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCreateGuild(lpPktBase, &dwChrID, &dwGuildID, &cNation, buff ) ;

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 누군가가 길드를 생성한 경우
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			pGame->m_csStatus.m_Info.GID = dwGuildID ;
		}
		else if (0 == dwChrID)
		{
			// 다른 유저가 만든 길드 정보를 GuildInfoMap 에 추가한다.
			CRYLGameScene*		pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLGuildFrameDlg*	pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
			CRYLGuildInfoPanel*	pPanel		= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

			GUILDLARGEINFOEX GuildLargeInfoEx;
			GuildLargeInfoEx.m_dwGID = dwGuildID ;
			GuildLargeInfoEx.m_cInclination = cNation ;
			strncpy(GuildLargeInfoEx.m_szName, buff, Guild::MAX_GUILD_NAME_LEN) ;
			pPanel->InsertGuildInfoMap( GuildLargeInfoEx ) ;
		}
		
		return true;
	}
	else if (PktCreateGuild::FAIL_DISSOLVE_GUILD == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 누군가가 길드를 해체 시킨 경우
		CRYLGameScene*		pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*	pPanel		= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		pPanel->DeleteGuildInfoMap( dwGuildID ) ;

		return true;
	}
	else
	{		
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktCreateGuild::FAIL_ALREADY_NAME :
            // 길드 생성 실패 에러 : 같은 이름이 있습니다.
			pChat->AddMessage( CRYLStringTable::m_strString[1230], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			break;

        case PktCreateGuild::FAIL_INVALID_NAME:
            // 길드 생성 실패 에러 : 적절하지 않은 길드 이름입니다. 공백과 몇몇 특수문자를 제거해 주십시오
            pChat->AddMessage( CRYLStringTable::m_strString[1244], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
            break;
		}
	}
	
	return false;
}

GAME_EVENT_FUNC( ParseCharGuildCmd ) 
{
	unsigned long	dwGID ;
	unsigned long	dwSenderID ;
	unsigned long	dwReferenceID ;
	unsigned short	wCmd ;
	char			szName[ Guild::MAX_MEMBER_NAME_LEN ] ;
	char			szGuild[ Guild::MAX_GUILD_NAME_LEN ] ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildCmd(lpPktBase, &dwGID, &dwSenderID, &dwReferenceID, szGuild, szName, &wCmd  ) ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*			pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*			pInfoPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		CRYLGuildPropertyPanel*		pProPanel	= ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;
		CRYLAuthoritySettingDlg*	pASDlg		= ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;
		CRYLGuildMemberPanel*		pMemPanel	= ( CRYLGuildMemberPanel* )pDlg->GetGuildMemberPanel() ;
		CRYLGuildTacticsPanel*		pTacPanel	= ( CRYLGuildTacticsPanel* )pDlg->GetGuildTacticsPanel() ;

		switch( wCmd ) 
		{
			case PktGuildCmd::GC_LOGINOUT:
			{
				// Send : 로그인/아웃한 멤버
				// Reference : 서버 아이디
				// edith 2009.08.28 길드원 로그인 로그아웃 메시지
				if ( dwSenderID != CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					char strText[200];

					if(dwReferenceID == 0)	// 로그아웃 
					{
						sprintf(strText, CRYLStringTable::m_strString[1246], szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
					}
					else	// 로그인
					{
						sprintf(strText, CRYLStringTable::m_strString[1245], szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 132);
					}
				}
				break;
			}
			case PktGuildCmd::GC_TACTICS_REQUEST:
			{
				// 용병 지원 요청.
				char szInfo[1024] = {0,};
				pGame->m_lpInterface->m_dwTacticsReq = 0;

				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				sprintf(szInfo, CRYLStringTable::m_strString[4035], szGuild, szGuild);
				lpMessageBox->Create(szInfo, MB_YES | MB_NO | MB_EXIT) ;
				lpMessageBox->SetResult(&pGame->m_lpInterface->m_dwTacticsReq) ;
				break;
			}
			case PktGuildCmd::GC_TACTICS:
			case PktGuildCmd::GC_TACTICS_JOIN:
			{
				if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );

					if(pSelfCreature)
					{
						pSelfCreature->SetTactics(Guild::TACTICS_WAIT);
					}

					pGame->m_csStatus.m_Info.GID = dwGID | Guild::MEMBER_WAIT_BIT ;
					
					CHAR szBuff[ 128 ] = {0, };
					sprintf(szBuff, CRYLStringTable::m_strString[3878], szGuild ) ;			
					pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );							

					pInfoPanel->UpdateBtnState() ;

					// 길드 관계 정보만 다시 요청
					SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
				}
				else
				{
					int nTitle = pASDlg->GetTitle();

					if(nTitle==Guild::MASTER || nTitle==Guild::MIDDLE_ADMIN)
					{						
						pChat->AddMessage(CRYLStringTable::m_strString[3879], CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
					}
				}
				break;
			}	
			case PktGuildCmd::GC_TACTICS_TITLE:
			{
				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );

				pTacPanel->UpdateGuildTacticsRight( dwSenderID, dwReferenceID ) ;
				pTacPanel->SetTactics( dwSenderID, Guild::TACTICS ) ;

				if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{					
					RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );

					if(pSelfCreature)
					{
						pSelfCreature->SetTactics(Guild::TACTICS);
					}

					if ( pTempData )
					{					
						pTempData->m_ulGuildID = dwGID ; 
					}

					pGame->m_csStatus.m_Info.GID = dwGID ;
					pInfoPanel->UpdateMyGuildInfo( -1 ) ;

					CHAR szBuff[ 128 ] = {0, };
					sprintf(szBuff, CRYLStringTable::m_strString[3895], szGuild, szGuild ) ;			
					pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 201 );	

					// 길드 관계 정보만 다시 요청
					SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
				}
				else
				{
					if ( pTempData )
					{					
						pTempData->m_ulGuildID = dwGID ; 
					}

					int nTitle = pASDlg->GetTitle();

					if(nTitle==Guild::MASTER || nTitle==Guild::MIDDLE_ADMIN)
					{							
						unsigned short	usClass = pTacPanel->GetClass(dwSenderID);
						unsigned char	cLevel	= pTacPanel->GetLevel(dwSenderID);

						CHAR szBuff[ 128 ] = {0, };
						sprintf(szBuff, CRYLStringTable::m_strString[3894], cLevel, pGame->GetClassName(usClass), szName) ;			
						pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 201 );
					}
				}
								
				break;
			}
			case PktGuildCmd::GC_TACTICS_KICK:			
			case PktGuildCmd::GC_TACTICS_LEAVE:
			{									
				if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );
					CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( CRYLNetworkData::Instance()->m_dwMyChrID );

					if(pSelfCreature && pSelfData)
					{
						CHAR szBuff[ 128 ] = {0, };

						if(pSelfCreature->GetTactics()==Guild::TACTICS_WAIT)
						{						
							sprintf(szBuff, CRYLStringTable::m_strString[3908], szGuild) ;			
							pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );							
						}
						else if(pSelfCreature->GetTactics()==Guild::TACTICS)
						{	
							sprintf(szBuff, CRYLStringTable::m_strString[3909], szGuild) ;
							pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );														
						}
						
						pSelfData->m_ulGuildID	= 0;
						pSelfCreature->SetTactics(0);
					}

					pDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
					pInfoPanel->SetVisible( TRUE ) ;
					pInfoPanel->DeleteMyGuildInfo( dwGID ) ;
					pProPanel->SetVisible( FALSE ) ;
					pMemPanel->SetVisible( FALSE ) ;
					pTacPanel->SetVisible( FALSE ) ;
					pInfoPanel->ClearRelationInfo() ;

					pGame->m_csStatus.m_Info.GID = 0 ;

					pInfoPanel->UpdateBtnState() ;
				}
				else
				{
					RYLCreature*	pCreature = RYLCreatureManager::Instance()->GetCreature(dwSenderID);
					CharDataNode*	pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );

					if(pCreature)
					{
						if(pCreature->GetTactics()==Guild::TACTICS)
						{
							CHAR szBuff[ 128 ] = {0, };
							sprintf(szBuff, CRYLStringTable::m_strString[3911], szName) ;			
							pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
						}
						else if(pCreature->GetTactics()==Guild::TACTICS_WAIT)
						{
							int nTitle = pASDlg->GetTitle();

							if(nTitle==Guild::MASTER || nTitle==Guild::MIDDLE_ADMIN)
							{
								CHAR szBuff[ 128 ] = {0, };
								sprintf(szBuff, CRYLStringTable::m_strString[3910], szName) ;
								pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
							}
						}
					}
					else
					{
						if(dwReferenceID==Guild::TACTICS)
						{
							CHAR szBuff[ 128 ] = {0, };
							sprintf(szBuff, CRYLStringTable::m_strString[3911], szName) ;			
							pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
						}
						else if(dwReferenceID==Guild::TACTICS_WAIT)
						{
							int nTitle = pASDlg->GetTitle();

							if(nTitle==Guild::MASTER || nTitle==Guild::MIDDLE_ADMIN)
							{
								CHAR szBuff[ 128 ] = {0, };
								sprintf(szBuff, CRYLStringTable::m_strString[3910], szName) ;
								pChat->AddMessage(szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
							}
						}
					}
					
					if(pCreature && pTempData)
					{
						pTempData->m_ulGuildID = 0;
						pCreature->SetTactics(0);
					}
				}

				pTacPanel->UpdateGuildStringGrid() ;

				break;
			}
			case PktGuildCmd::GC_INVITE :
			{
				CRYLCommunityData::Instance()->SetMsgBoxState( PktGuildCmd::GC_INVITE, dwGID, dwReferenceID ) ;
				break ;
			}

			case PktGuildCmd::GC_JOIN :
			{
				switch ( dwReferenceID )
				{
					case Guild::COMMON :
					{
                        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );
                        CHAR szBuff[ 128 ] ;

						if ( pTempData )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1231], pTempData->m_strName.c_str(), szGuild ) ;
                            pTempData->m_ulGuildID = dwGID ;
						}
						else
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1232], szName ) ;
						}

						pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 201 ) ;
						if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
						{
							pGame->m_csStatus.m_Info.GID = dwGID ;

							// 길드 관계 정보만 다시 요청
							SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
						}
						break ;	
					}

					case Guild::MASTER :
					{
                        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );

                        CHAR szBuff[ 128 ] ;

						if ( pTempData )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1233], pTempData->m_strName.c_str() ) ;
                            pTempData->m_ulGuildID = dwGID ;
						}
						else
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1233], szName ) ;
						}
						
						pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202 ) ;

						pASDlg->SetTitle( Guild::MASTER ) ;
						pInfoPanel->UpdateGuildScreen() ;
						pProPanel->CheckRightButton() ;
											
						if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
						{
							pGame->m_csStatus.m_Info.GID = dwGID ;

							// 길드 관계 정보만 다시 요청
							SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
						}
						break ;
					}

					case Guild::JOIN_WAIT :
					{
                        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );
                        CHAR szBuff[ 128 ] ;

						if ( pTempData )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1234], pTempData->m_strName.c_str() ) ;
							pTempData->m_ulGuildID = dwGID | Guild::MEMBER_WAIT_BIT ;
						}
						else
						{
							sprintf( szBuff, CRYLStringTable::m_strString[1234], szName ) ;
						}

						pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 200 ) ;

						pMemPanel->UpdateGuildMemberRight( dwSenderID, Guild::JOIN_WAIT ) ;

						// by Hades Kang - 길드 가입시 아이디 처리
						//pGame->m_csStatus.m_Info.GID = dwGID ;
						if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
						{
							pGame->m_csStatus.m_Info.GID = dwGID | Guild::MEMBER_WAIT_BIT;

							// 길드 관계 정보만 다시 요청
							SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
						}

						pInfoPanel->SetVisible( pInfoPanel->GetVisible() );
						break ;
					}
					
				}
				break ;
			}

			case PktGuildCmd::GC_REFUSE :
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );
                CHAR szBuff[ 128 ] ;

				if ( pTempData )
				{
					sprintf( szBuff, CRYLStringTable::m_strString[1235], pTempData->m_strName.c_str() ) ;
				}
				else
				{
					sprintf( szBuff, CRYLStringTable::m_strString[1235], szName ) ;
				}

				pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				break ;
			}

			case PktGuildCmd::GC_ASK_ENTER :
				break ;

			case PktGuildCmd::GC_ASK_LEAVE :
				break ;

			case PktGuildCmd::GC_KICK :
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );
                CHAR szBuff[ 128 ] ;

				if ( pTempData )
				{
					if ( Guild::JOIN_WAIT == pMemPanel->GetTitle( dwSenderID ) )
					{
						sprintf( szBuff, CRYLStringTable::m_strString[ 3457 ], pTempData->m_strName.c_str() ) ;
					}
					else
					{
						sprintf( szBuff, CRYLStringTable::m_strString[1236], pTempData->m_strName.c_str() ) ;
					}

					pTempData->m_ulGuildID = 0 ;
				}
				else
				{
					if ( Guild::JOIN_WAIT == pMemPanel->GetTitle( dwSenderID ) )
					{
						sprintf( szBuff, CRYLStringTable::m_strString[ 3457 ], szName ) ;
					}
					else
					{
						sprintf( szBuff, CRYLStringTable::m_strString[1236], szName ) ;
					}
				}
				
				pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

				if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
					pInfoPanel->SetVisible( TRUE ) ;
					pInfoPanel->DeleteMyGuildInfo( dwGID ) ;
					pProPanel->SetVisible( FALSE ) ;
					pMemPanel->SetVisible( FALSE ) ;
					pInfoPanel->ClearRelationInfo() ;

					pGame->m_csStatus.m_Info.GID = 0 ;
				}

				pMemPanel->UpdateGuildStringGrid() ;
				pInfoPanel->UpdateBtnState() ;

				break ;
			}

			case PktGuildCmd::GC_SETUP_TITLE :
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );

                pMemPanel->UpdateGuildMemberRight( dwSenderID, dwReferenceID ) ;
				
				CHAR szBuff[ 128 ] ;

				if ( pTempData )
				{
					sprintf( szBuff, CRYLStringTable::m_strString[1237], pTempData->m_strName.c_str(), pMemPanel->GetTitleString( dwReferenceID ) ) ;
					pTempData->m_ulGuildID = dwGID ;
				}
				else
				{
					sprintf( szBuff, CRYLStringTable::m_strString[1237], szName, pMemPanel->GetTitleString( dwReferenceID ) ) ;
				}
                
				pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202 ) ;

				if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pGame->m_csStatus.m_Info.GID = dwGID ;
					pInfoPanel->UpdateMyGuildInfo( -1 ) ;

					// 길드 관계 정보만 다시 요청
					SendPacket::CharGuildRelationInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwGID ) ;
				}
				break ;
			}	

			case PktGuildCmd::GC_RANK_UP:
			case PktGuildCmd::GC_RANK_DOWN:
			{
				break;
			}
		}

		pMemPanel->UpdateGuildMemberPanel() ;
	}
	else
	{
		switch( CRYLNetworkData::Instance()->m_dwNetworkError )
		{
			case PktGuildCmd::FAIL_NOT_MEMBER :		// 2
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[1239] ) ;
				break ;
			}

			case PktGuildCmd::FAIL_NOT_RIGHT :		// 3
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[1240] ) ;
				break ;
			}

			case PktGuildCmd::FAIL_ALREADY_MEMBER : // 4
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[1241] ) ;
				break ;
			}

			case PktGuildCmd::FAIL_OVER_MEMBER :	// 5
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[1242] ) ;
				break ;
			}

			case PktGuildCmd::FAIL_NATION :			// 6
			{

				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				if ( wCmd == PktGuildCmd::GC_INVITE )
				{
					if ( dwSenderID == CRYLNetworkData::Instance()->m_dwMyChrID )		//가입자가 국적이 다른 경우라면.
					{
						CHAR szBuff[ 512 ] ;
						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwReferenceID );
						sprintf( szBuff, CRYLStringTable::m_strString[2978], pTempData->m_strName.c_str(), szName ) ;
						lpMessageBox->Create( szBuff ) ;
						break;
					}
				}

				CHAR szBuff[ 512 ] ;
				char* szNation = NULL;
				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwSenderID );
				
				switch( pTempData->m_cNation )
				{
				case Creature::MERKADIA:
					szNation = CRYLStringTable::m_strString[2264];
					break;
				case Creature::KARTERANT:
					szNation = CRYLStringTable::m_strString[2991];
					break;
				case Creature::ALMIGHTY_PIRATE:
					szNation = CRYLStringTable::m_strString[2268];
					break;
				default:
					szNation = CRYLStringTable::m_strString[2268];
					break;
				}
				sprintf( szBuff, CRYLStringTable::m_strString[1243], pTempData->m_strName.c_str(), szNation ) ;
				lpMessageBox->Create( szBuff ) ;
				break ;

			}

			case PktGuildCmd::FAIL_GUILDWAR_TIME_KICK :	// 9
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3873 ] ) ;
				break;
			}
			case PktGuildCmd::FAIL_TACTICS_LOGOUT:
			{
				CHAR szBuff[ 512 ] = {0, };
				sprintf(szBuff, CRYLStringTable::m_strString[4036], szName);
				pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202 ) ;
				break;
			}
		}
	}

	return true ;
}

GAME_EVENT_FUNC( ParseCharGuildMark ) 
{
	unsigned long dwSenderID = 0;;
	unsigned long dwGuildID = 0;
	unsigned long dwGold = 0; 

	BYTE	buff[ 433 ] ;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
		ParsePacket::HandleCharGuildMark(lpPktBase, &dwSenderID, &dwGuildID, ( char* )buff, &dwGold ) ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 변경된 마크 및 잔액
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*	 pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		
		pPanel->UpdateGuildMark( dwGuildID, buff, dwGold ) ;
		pPanel->UpdateMyGuildInfo( -1 ) ;
		pPanel->UpdateGuildScreen() ;
		
		return true;
	} 
	else
	{
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwSenderID)
		{
			switch (CRYLNetworkData::Instance()->m_dwNetworkError)
			{
				case PktGuildMark::FAIL_INVALID_GUILD:
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[2767] );
					break;
				}

				case PktGuildMark::FAIL_INVALID_MARK:
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[2768] );
					break;
				}

				case PktGuildMark::FAIL_INVALID_CHARACTER:
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[2769] );
					break;
				}

				case PktGuildMark::FAIL_NOT_MASTER:	
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[2770] );
					break;
				}

				case PktGuildMark::FAIL_NOT_ENOUGH_GOLD:
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[1082] );
					break;
				}
			}
		}
	}

	return false ;
}

GAME_EVENT_FUNC( ParseCharGuildLevel ) 
{
	unsigned long dwGuildID = 0;
	unsigned char cLevel	= 0 ;
	unsigned long dwGold	= 0 ;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildLevel(lpPktBase, &dwGuildID, &cLevel, &dwGold ) ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 길드 레벨 처리
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*	 pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

		pPanel->UpdateGuildLevel( dwGuildID, cLevel, dwGold ) ;
		pPanel->UpdateMyGuildInfo( -1 ) ;
		pPanel->UpdateGuildScreen() ;
		return true;
	} 

	return false  ;
}

GAME_EVENT_FUNC( ParseCharGuildList ) 
{
	unsigned long dwCID = 0 ;
	unsigned char cSortCmd = 0 ;
	unsigned char cPage = 0 ;
	unsigned char cSmallNodeNum = 0 ;
	unsigned char cLargeNodeNum = 0 ;

	GuildSmallInfoNode SmallNode[PktGuildList::MAX_NUM_PER_PAGE];
	GuildLargeInfoNode LargeNode[PktGuildList::MAX_NUM_PER_PAGE];
	

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildList( lpPktBase, 
																	  &dwCID, 
																	  &cSortCmd, 
																	  &cPage, 
																	  &cSmallNodeNum,
																	  SmallNode, 
																	  &cLargeNodeNum, 
 																	  LargeNode) ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*	 pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
				
		pPanel->ProcessGuildInfo( cSortCmd, cPage, 
								  cSmallNodeNum, &( *SmallNode ),
							      cLargeNodeNum, &( *LargeNode ) ) ;

		return true ;
	}

	return false ;
}

GAME_EVENT_FUNC( ParseCharGuildRight ) 
{
	unsigned long dwGID ;
	GuildRight	  Right ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildRight( lpPktBase, &dwGID, &Right ) ;

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		// 권한설정은 길드마스터만이 가능한 기능이기때문에
		// Title은 일반 길드원으로 날림 ㅡㅡ;

		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

		pASDlg->SetAuthorityValue( &Right, Guild::NONE ) ;

		pChat->AddMessage(CRYLStringTable::m_strString[1238], CRYLNetworkData::Instance()->m_dwMyChrID, 202);
		
		return true ;
	}

	return false ;
}

GAME_EVENT_FUNC( ParseCharGuildMemberList ) 
{
	unsigned long dwCID ;
	unsigned char cMemberType ;
	unsigned char cSortCmd ;
	unsigned char cPage ;
	unsigned char cTotalMemberNum ;
	unsigned char cNodeNum ;

	GuildMemberInfoNode		MemberNode[Guild::MAX_MEMBER_NAME_LEN] ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildMemberList( lpPktBase,
																		    &dwCID,
																			&cMemberType,
																			&cSortCmd,
																			&cPage,
																			&cTotalMemberNum, 
																			&cNodeNum,
																			MemberNode ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;

		switch ( cMemberType )
		{
			case Guild::TYPE_MEMBER :
			{
				CRYLGuildMemberPanel*   pMemPanel = ( CRYLGuildMemberPanel* )pDlg->GetGuildMemberPanel() ;
				pMemPanel->ProcessGuildMemberList( cSortCmd, cPage, cNodeNum, cTotalMemberNum, &( *MemberNode ) ) ;
				break ;
			}

			case Guild::TYPE_TACTICS :
			{
				CRYLGuildTacticsPanel*   pTacPanel = ( CRYLGuildTacticsPanel* )pDlg->GetGuildTacticsPanel() ;
				pTacPanel->ProcessGuildTacticsList( cSortCmd, cPage, cNodeNum, cTotalMemberNum, &( *MemberNode ) ) ;
				break ;
			}
		}
				
		return true ;		
	}

	return false ;
}

GAME_EVENT_FUNC( ParseCharMyGuildInfo ) 
{
	unsigned long	dwGold ;
	GuildRight		Right ;
	unsigned char	cTitle ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharMyGuildInfo( lpPktBase, 
																		&dwGold, 
																		&Right, 
																		&cTitle ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*	 pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		CRYLGuildPropertyPanel* pProPanel = ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;
		CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

		pASDlg->SetTitle( cTitle ) ;
		pASDlg->SetAuthorityValue( &Right, cTitle ) ;
		pProPanel->SetCurrentGold( dwGold ) ;
		if ( !( pPanel->GetUseGID( pGame->m_csStatus.m_Info.GID ) ) )
		{
			GuildSmallInfoNode SmallNode ;
			SmallNode.m_dwGID = pGame->m_csStatus.m_Info.GID ;
			SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Guild::GUILD_REQUEST, 0, 1, &SmallNode ) ;
		}

		return true ; 
	}

	return false ;
}

GAME_EVENT_FUNC( ParseCharGuildSafe ) 
{
	unsigned long dwCID ; 
	unsigned long dwSafeGold ;
	unsigned long dwCharGold ;
	unsigned char cCmd ;
	char		  szCharName[Guild::MAX_MEMBER_NAME_LEN];		// 캐릭터 이름
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildSafe( lpPktBase, &dwCID, szCharName, &dwSafeGold, &dwCharGold, &cCmd ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildPropertyPanel*	 pPropertyPanel = ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;
		
		pPropertyPanel->SetSafeAndMyGold( dwCID, szCharName, dwSafeGold, dwCharGold, cCmd ) ;

		if(pGame)
			pGame->m_bGuildSafeResult = FALSE;

		return true ;
	}
	else
	{
		if(pGame)
			pGame->m_bGuildSafeResult = FALSE;

		switch ( CRYLNetworkData::Instance()->m_dwNetworkError ) 
		{
		case PktGuildSafe::FAIL_NOT_ENOUGH_MONEY:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1185] );
				break;
			}
		case PktGuildSafe::FAIL_NOT_CHAR_MONEY:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1189] );
				break;
			}
		case PktGuildSafe::FAIL_NOT_GUILD_DEPOSIT:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1190] );
				break;
			}
		case PktGuildSafe::FAIL_NOT_GUILD_UPDATE:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1191] );
				break;
			}			
		case PktGuildSafe::FAIL_REQUEST_DATA:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2352] );
				break;
			}
		}
	}
	
	return false ;
}

GAME_EVENT_FUNC( ParseCharGuildInclination ) 
{
	unsigned long dwGID ; 
	unsigned char cInclination;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildInclination( lpPktBase, &dwGID, &cInclination );

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

		pPanel->ProcessGuildInclination( dwGID, cInclination );

		if (CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID)
		{
			char strMessage[ MAX_PATH ] = "" ;
			sprintf( strMessage, CRYLStringTable::m_strString[2987], -(Siege::FAME_FOR_DESTROYED_CAMP) ) ;
			CRYLChattingDlg::Instance()->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		}

		CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwGID ) ;

		if ( pCamp )
		{
			pCamp->SetCampNation( cInclination ) ;
		}

		return true ;
	}
	else 
	{
		switch ( CRYLNetworkData::Instance()->m_dwNetworkError ) 
		{
		case PktGuildInclination::FAIL_SO_FAST:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2988] );
				break;
			}

		case PktGuildInclination::FAIL_WRONG_NATION_KARTERANT:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2989] );
				break;
			}

		case PktGuildInclination::FAIL_WRONG_NATION_MERKADIA:
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[2990] );
				break;
			}
		}
	}

	return false;
}

GAME_EVENT_FUNC( ParseCharGuildRelation )
{
	unsigned long	dwCID ;  
	unsigned long	dwGID ;
	unsigned long	dwTargetGID ; 
	unsigned long	dwValue ;
	unsigned char	cSubCmd ;
	char			szGuildName[ Guild::MAX_GUILD_NAME_LEN ] = "" ;
	char			szTempGuildName[ Guild::MAX_GUILD_NAME_LEN ] = "" ;


	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildRelation( lpPktBase, 
																						  &dwCID,
																						  &dwGID, 
																						  &dwTargetGID,
																						  &dwValue,
																						  &cSubCmd,
																						  szGuildName,
																						  szTempGuildName ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*		pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	pDlg			= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel* pPanel			= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		CRYLHostilityDlg*	pHostilityDlg	= ( CRYLHostilityDlg* )pDlg->GetHostilityDlg() ;
		char				szBuffer[ 1024 ] ;

		std::set<unsigned long>	setGID;
		LPGUILDLARGEINFOEX	lpGuild = pPanel->FindGuild( dwGID ) ;
		LPGUILDLARGEINFOEX	lpTargetGuild = pPanel->FindGuild( dwTargetGID ) ;

		setGID.clear() ;

		// 길드 리스트에 없다면, 요청하기 위해 저장해둔다.
		if ( NULL == lpGuild )
		{
			setGID.insert( dwGID & ~Guild::MEMBER_WAIT_BIT ) ;
		}
	
		if ( NULL == lpTargetGuild )
		{
			setGID.insert( dwTargetGID & ~Guild::MEMBER_WAIT_BIT ) ;
		}

		switch ( cSubCmd )
		{
			case PktGuildRelation::GR_HOSTILITY :
			{
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByMine = Guild::HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3916], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					if ( lpGuild ) lpGuild->m_cRelationByTarget = Guild::HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3917], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}

				break ;
			}

			case PktGuildRelation::GR_PEACE :
			{
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByMine = Guild::TARGET_NEUTRALITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3918], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					if ( lpGuild ) lpGuild->m_cRelationByTarget = Guild::TARGET_NEUTRALITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3919], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}

				break ;
			}

			case PktGuildRelation::GR_HOSTILITYDEL :
			{
				// 선언 해지 상태 : 이건 테스트를 좀 해봐야한다.
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					// 이거 제대로 되는지  lpGuild->m_cRelationByTarget 이값이 5여야한다.
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByTarget = Guild::NEUTRALITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3918], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					// 이거 제대로 되는지  lpGuild->m_cRelationByTarget 이값이 5여야한다.
					if ( lpGuild ) lpGuild->m_cRelationByMine = Guild::NEUTRALITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3919], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}

				break ;
			}

			case PktGuildRelation::GR_ALERT :
			{
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByMine = Guild::ALERT_HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3920], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					if ( lpGuild ) lpGuild->m_cRelationByTarget = Guild::ALERT_HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3921], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}

				break ;
			}

			case PktGuildRelation::GR_COUNTER :
			{
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByMine = Guild::COUNTER_HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3922], szGuildName, szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					if ( lpGuild ) lpGuild->m_cRelationByTarget = Guild::COUNTER_HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3923], szGuildName, szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}

				break ;
			}

			case PktGuildRelation::GR_REQUEST :
			{
				if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
				{
					if ( lpTargetGuild ) lpTargetGuild->m_cRelationByMine = Guild::HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3924], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
				else if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwTargetGID )
				{
					if ( lpGuild ) lpGuild->m_cRelationByTarget = Guild::HOSTILITY ;

					sprintf( szBuffer, CRYLStringTable::m_strString[3925], szGuildName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}

				break ;
			}
		}		

		// 현재 길드리스트에 없다면 요청한다.
		if ( !setGID.empty() )
		{
			pPanel->RequestNewGuild( setGID ) ;
		}

		// 관계 변경시 적대 관계 현황창 업데이트
		if ( pHostilityDlg->GetVisible() && 0 != CRYLGameData::Instance()->m_csStatus.m_Info.GID )
		{
			pHostilityDlg->SetGID( CRYLGameData::Instance()->m_csStatus.m_Info.GID ) ;
			pHostilityDlg->RequestHostiliyList() ;
		}
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktGuildRelation::FAIL_NOT_MEMBER :				// 해당 길드의 멤버가 아님
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3926] ) ;
				break ;
			}
				
			case PktGuildRelation::FAIL_NOT_HOSTILITY_TIME :		// 적대 선언을 할수 없는 시간
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3927] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_ALERT_TIME :			// 경계 적대 선언을 할수 없는 시간
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3928] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_COUNTER_TIME :			// 카운터 적대 선언을 할수 없는 시간
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3929] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_MASTER :				// 길드의 마스터가 아님
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3930] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_EXIST_GUILD :			// 길드 ID가 0 이거나 길드가 존재하지 않음
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3931] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_SELF_GUILD :				// 자신의 길드에게 적대/평화/경계/카운터 선언
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3932] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_ALREADY_HOSTILITY :			// 대상 길드와 이미 적대 관계입니다.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3933] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_ALREADY_COUNTER_HOSTILITY :	// 대상 길드와 이미 카운터 적대 관계입니다.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3934] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_ALREADY_ALERT_HOSTILITY :	// 데상 길드와 이미 경계 적대 관계입니다.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3935] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_ALREADY_NEUTRALITY :		// 대상 길드와 이미 중립 관계입니다.
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3936] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_MAX_ALERT_NUM :				// 경계 적대 선언권 초과
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3937] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_HAVE_DEVELOPING_CAMP :	// 구축중인 길드 요새가 없음
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3938] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_NOT_HAVE_ALERT :			// 경계 적대 신청을 받지 않고 카운터 적대 신청을 하려함
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3939] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_TARGET_HOSTILITY_FULL :		// 적대 선언 받을 수 있는 한계치 초과
			{
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3940] ) ;
				break ;
			}

			case PktGuildRelation::FAIL_REQUEST_WINDOW :			// 적대 선언수 초과로 인해 평화 선언후 적대 선언을 하도록 인증
			{
                lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3941] ) ;

				char szBuffer[ 1024 ] ;
				sprintf( szBuffer, CRYLStringTable::m_strString[3942], szTempGuildName, szTempGuildName, szGuildName ) ;

				CRYLMessageBox *lpConfirmMgsBox = new CRYLMessageBox ;
				lpConfirmMgsBox->SetWordBreak( TRUE ) ;
				lpConfirmMgsBox->Create( szBuffer, MB_YES | MB_NO | MB_EXIT ) ;
				lpConfirmMgsBox->SetResult( &CRYLGameData::Instance()->m_dwRequestResult ) ;
				CRYLGameData::Instance()->m_dwRequestResult = 0 ;
				CRYLGameData::Instance()->m_dwTargetGID = dwTargetGID ;
				CRYLGameData::Instance()->m_dwAlreadyHostilityGID = dwValue ;

				break ;
			}

			case PktGuildRelation::FAIL_REQUEST_COUNTER :			// 카운터 적대 선언을 하도록 인증
			{
				CRYLMessageBox *lpConfirmMgsBox = new CRYLMessageBox ;
				lpConfirmMgsBox->SetWordBreak( TRUE ) ;
				lpConfirmMgsBox->Create( CRYLStringTable::m_strString[3943], MB_YES | MB_NO | MB_EXIT ) ;
				lpConfirmMgsBox->SetResult( &CRYLGameData::Instance()->m_dwCounterHostilityResult ) ;
				CRYLGameData::Instance()->m_dwCounterHostilityResult = 0 ;
				CRYLGameData::Instance()->m_dwTargetGID = dwGID ;

				break ;
			}

			default :	break ;
		}
	}

	return FALSE ;
}


GAME_EVENT_FUNC( ParseCharGuildHostilityList )
{
	unsigned long	dwCID ;  
	unsigned char	cCurrentPage;
	unsigned char	cPageState;
	unsigned char	cNodeNum;
	unsigned short	wTotalItemNum;

	char	szHostility[MAX_GUILD_NAME_LEN];
	char	szAlert[MAX_GUILD_NAME_LEN];

	GuildHostilityInfoNode	HostilityNode[PktGuildHostilityList::MAX_NUM_PER_PAGE] ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildHostilityList( lpPktBase, 
																							   &dwCID,
																							   &cCurrentPage,
																							   &cPageState,
																							   &cNodeNum,
																							   &wTotalItemNum,
																							   szHostility,
																							   szAlert,
																							   HostilityNode ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLHostilityDlg*		pHostilityDlg = (CRYLHostilityDlg* )pDlg->GetHostilityDlg() ;

		pHostilityDlg->SetHostilityList( cCurrentPage, wTotalItemNum, szHostility, szAlert, cNodeNum, &( *HostilityNode ) ) ;

		return TRUE ;
	}

	return FALSE ;
}


GAME_EVENT_FUNC( ParseCharGuildRelationInfo )
{
	unsigned long	dwCID ;
	unsigned long	dwGID ;
	unsigned short	wNodeNum ;

	GuildRelationInfoNode	RelationInfoNode[ Guild::MAX_RELATION_NUM ] ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGuildRelationInfo( lpPktBase,
																							  &dwCID,
																							  &dwGID,
																							  &wNodeNum,
																							  RelationInfoNode ) ;

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		CRYLGameScene*			pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*		pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*		pInfo	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

		pInfo->UpdateRelationInfo( wNodeNum, &( *RelationInfoNode ) ) ;

		return TRUE ;
	}

	return FALSE ;
}

