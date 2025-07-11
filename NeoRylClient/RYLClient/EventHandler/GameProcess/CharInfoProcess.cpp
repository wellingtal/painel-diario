
#include "CharInfoProcess.h"
#include "../GameHandler.h"
#include "RYLClientMain.h"
#include "RYLMessageBox.h"
#include "../../GUITextEdit.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Parse/ParseCharInfo.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Skill/SkillMgr.h>

#include "RYLStringTable.h"
#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLChattingDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLSkillPanel.h"

#include "RYLJobChangeDlg.h"

#include <Creature/Character/CharacterClass.h>

#include "RYLRaceBase.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharLevelUp);
GAME_EVENT_FUNC(ParseCharIncreasePoint);
GAME_EVENT_FUNC(ParseCharHPRegen);
GAME_EVENT_FUNC(ParseCharClassUpgrade);
GAME_EVENT_FUNC(ParseCharQuickSlotMove);
GAME_EVENT_FUNC(ParseCharEliteBonus);

bool GameProcess::RegisterCharInfoHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;
    unsigned long dwIgnoreNonPlayPacket = !(BIT(CLIENT_GAME) | BIT(CLIENT_GAMELOADING));

    dwErrorCount += GameEventHandler.AddHandler(CmdCharLevelUp, ParseCharLevelUp, dwIgnoreNonPlayPacket) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharIncreasePoint, ParseCharIncreasePoint) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharHPRegen, ParseCharHPRegen) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharClassUpgrade, ParseCharClassUpgrade) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharQuickSlotMove, ParseCharQuickSlotMove) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdEliteBonus, ParseCharEliteBonus) ? 0 : 1;

    return (0 == dwErrorCount);
}


GAME_EVENT_FUNC(ParseCharLevelUp)
{
	unsigned long	dwChrID ;
	unsigned char	cLevel ;
	ChState			pChState ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharLevelUp(lpPktBase, &dwChrID, &cLevel, &pChState);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{			
			char a[100];
			sprintf(a, CRYLStringTable::m_strString[291], cLevel);
			pChat->AddMessage(a, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

//////////////////  JOIN GUID CHAR MSG BY AVALANSA
			// 레벨 30 이상일시에 길드에 가입 안하면 길드에 대한 문구 출력.
			if( cLevel > 29 )
			{
				if( !pGame->m_csStatus.m_Info.GID )
				{
					pChat->AddMessage(CRYLStringTable::m_strString[2979], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
			}
///////////////// JOIN GUID CHAR MSG BY AVALANSA

//////////////////  levelup 100 msg chat
			// 레벨 30 이상일시에 길드에 가입 안하면 길드에 대한 문구 출력.
			if( cLevel > 99 )
			{
				if( !pGame->m_csStatus.m_Info.GID )
				{
					pChat->AddMessage(CRYLStringTable::m_strString[5600], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
			}
///////////////// levelup 100 msg chat

			pGame->m_csStatus.m_Info.Level = cLevel;
			pGame->m_csStatus.m_Info.IP = pChState.m_wIP;
			pGame->m_csStatus.m_Info.STR = pChState.m_wSTR;
			pGame->m_csStatus.m_Info.DEX = pChState.m_wDEX;
			pGame->m_csStatus.m_Info.INT = pChState.m_wINT;
			pGame->m_csStatus.m_Info.CON = pChState.m_wCON;
			pGame->m_csStatus.m_Info.WIS = pChState.m_wWIS;

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			pGame->m_csStatus.m_Info.HP = pGame->m_csStatus.GetMaxHP();
			pGame->m_csStatus.m_Info.MP = pGame->m_csStatus.GetMaxMP();

            RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if( !pSelfCreature )
                return false;

            CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
            
			if( !pSelfData )
                return false;

//-------------------
			// edith 2008.02.13 자동 전직 제거
			// edith 2008.02.13 (20 레벨 이상에 캐릭터에 전직창 출력하기)
			CRYLNetworkData* pNetwork = CRYLNetworkData::Instance();
			if ( pNetwork->m_eInternationalCode != GameRYL::KOREA )
			{
				if(cLevel>=10)
				{
					if(CClass::GetJobLevel(pSelfData->m_usClass)==CClass::DEFAULT_CLASS)
					{
						pGame->m_lpInterface->m_dwLevelUp = 0;

						CRYLMessageBox *lpMessageBox;
						lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[3893], MB_EXIT | MB_YES | MB_NO);
						lpMessageBox->SetResult(&pGame->m_lpInterface->m_dwLevelUp);
						lpMessageBox->SetEnableEnter(false);
					}				
				}
			}
//----------------
            pSelfData->m_cLevel = pGame->m_csStatus.m_Info.Level;
            pSelfData->m_lCurrHP= pSelfData->m_ulMaxHP;
            pSelfData->m_lCurrMP= pSelfData->m_ulMaxMP;
//-------------------
			int iMax = 10;
			if(pGame->m_csStatus.m_Info.Race < CClass::RaceType::HUMAN)
				iMax = 10; // was 20

			// edith 2008.03.26 전직 하라는 메시지 출력하기
			if (cLevel >= iMax)
			{
				char strTrainer[MAX_PATH], strClass[3][MAX_PATH], strMessage[MAX_PATH];
				unsigned long dwClass;
				BOOL bShow = true;
				strcpy(strTrainer, CRYLStringTable::m_strString[450 + pGame->m_csStatus.m_Info.Class]);

				switch (pGame->m_csStatus.m_Info.Class)
				{
					case 1:			// Fighter
						dwClass = 2;
						strcpy(strClass[0], CRYLStringTable::m_strString[455]);
						strcpy(strClass[1], CRYLStringTable::m_strString[456]);
						break;

					case 2:			// Rogue
						dwClass = 2;
						strcpy(strClass[0], CRYLStringTable::m_strString[457]);
						strcpy(strClass[1], CRYLStringTable::m_strString[458]);
						break;

					case 3:			// Mage
						dwClass = 2;
						strcpy(strClass[0], CRYLStringTable::m_strString[459]);
						strcpy(strClass[1], CRYLStringTable::m_strString[460]);
						break;

					case 4:			// Acolyte
						dwClass = 2;
						strcpy(strClass[0], CRYLStringTable::m_strString[461]);
						strcpy(strClass[1], CRYLStringTable::m_strString[462]);
						break;

					case 17:		// Combatent
						dwClass = 3;
						strcpy(strClass[0], CRYLStringTable::m_strString[469]);
						strcpy(strClass[1], CRYLStringTable::m_strString[470]);
						strcpy(strClass[2], CRYLStringTable::m_strString[471]);
						break;

					case 18:		// Opiciator
						dwClass = 3;
						strcpy(strClass[0], CRYLStringTable::m_strString[472]);
						strcpy(strClass[1], CRYLStringTable::m_strString[473]);
						strcpy(strClass[2], CRYLStringTable::m_strString[474]);
						break;

					default:
						bShow = false;
						break;
				}

				if (bShow)
				{
					sprintf(strMessage, CRYLStringTable::m_strString[420], cLevel, strTrainer, strClass[0]);
					for(unsigned long i = 1; i < dwClass; i++)
					{
						strcat(strMessage, ", ");
						strcat(strMessage, strClass[i]);
					}
					strcat(strMessage, CRYLStringTable::m_strString[421]);
					pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
			}
//---------------------------		
            /* DEPLECATED
            list<unsigned long> SendCharacterList;
            RYLCreatureManager::Instance()->GetSendCharacterAroundList( SendCharacterList );

            for( list<unsigned long>::iterator iter = SendCharacterList.begin() ; iter != SendCharacterList.end() ; ++iter )
            {
                g_pClientSocket->RegisterInstanceUDPList( (*iter) );
            }
            SendPacket::CharLevelUpInfo( *g_pClientSocket, CRYLNetworkData::Instance()->m_dwMyChrID, cLevel );
            */

            if( pSelfCreature )
            {
                CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();

                if( pSelfModel )
                {
                    vector3 vecPos;
                    pSelfModel->GetPosition( vecPos );

                    CEffScript *pLevelUpEff = new CEffScript;
				    CSceneManager::AddEffectScript( pLevelUpEff );

				    pLevelUpEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
				    pLevelUpEff->SetStartPos( vecPos.x, vecPos.y, vecPos.z );			

                    pLevelUpEff->SetEndPos( (D3DXVECTOR3*)pSelfCreature->GetPosition(), 1 );
                }
            }
		}

	    CRYLGameScene*		            pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
        CRYLLargeMapFrameDlg*			pLargeMapDlg	= ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
		pLargeMapDlg->PlayerLevelChanged();
		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharIncreasePoint)
{
	unsigned long	dwChrID ;
	ChState			pChState ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharIncreasePoint(lpPktBase, &dwChrID, &pChState);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			pGame->m_csStatus.m_Info.IP = pChState.m_wIP;
			pGame->m_csStatus.m_Info.STR = pChState.m_wSTR;
			pGame->m_csStatus.m_Info.DEX = pChState.m_wDEX;
			pGame->m_csStatus.m_Info.INT = pChState.m_wINT;
			pGame->m_csStatus.m_Info.CON = pChState.m_wCON;
			pGame->m_csStatus.m_Info.WIS = pChState.m_wWIS;

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}
		return true;
	} else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					//2 = 필요 증가 포인트 부족 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[349]);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharHPRegen)
{
	unsigned long   dwChrID;
	unsigned short  wHP, wMP;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharHPRegen(lpPktBase, &dwChrID, &wHP, &wMP);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			pGame->m_csStatus.m_Info.HP = wHP;
			pGame->m_csStatus.m_Info.MP = wMP;

            RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if( !pSelfCreature )
                return false;

            CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
            if( pSelfData )
			{
                pSelfData->m_lCurrHP = wHP;
                pSelfData->m_lCurrMP = wMP;
            }
		}
		return true;
	}

	return false;
}


GAME_EVENT_FUNC(ParseCharClassUpgrade)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID, dwFame;
	unsigned char	cLevel, cClass;
	ChState			pState;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharClassUpgrade(lpPktBase, &dwChrID, &cClass, &cLevel, &pState, &dwFame);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			// 전직 전의 정보 세팅 
			pGame->m_csStatus.m_BeforeInfo = pGame->m_csStatus.m_Info;

			pGame->m_csStatus.m_Info.Level = cLevel;
			pGame->m_csStatus.m_Info.Fame = dwFame;

			pGame->m_csStatus.m_Info.Class = (unsigned short)cClass;
			pGame->m_csStatus.m_Info.STR = pState.m_wSTR;
			pGame->m_csStatus.m_Info.DEX = pState.m_wDEX;
			pGame->m_csStatus.m_Info.CON = pState.m_wCON;
			pGame->m_csStatus.m_Info.INT = pState.m_wINT;
			pGame->m_csStatus.m_Info.WIS = pState.m_wWIS;
			pGame->m_csStatus.m_Info.IP = pState.m_wIP;
			pGame->m_csStatus.SetClassSkill();
			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

            RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if( !pSelfCreature )
                return false;

            CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
            if( !pSelfData )
                return false;

            pSelfData->m_usClass = pGame->m_csStatus.m_Info.Class;

			char strText[MAX_PATH];
			sprintf(strText, CRYLStringTable::m_strString[294], pGame->GetClassName(cClass));
			CRYLChattingDlg::Instance()->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

			if (cClass > 4 && cClass != 17 && cClass != 18)
			{
				// 전직 effect script load
				CEffScript *test_script;				
	//			_ASSERTE(_CrtCheckMemory());
				test_script = new CEffScript;
			
				// 전직 effect ptr Setting 
				pGame->m_ClassEffectPtr = test_script;
				/*
				[1], "Fighter"
				[2], "Rogue");
				[3], "Mage");
				[4], "Acolyte");
				[5], "Defender");
				[6], "Warrior");
				[7], "Assasin");
				[8], "Archer");
				[9], "Sorcerer");
				[10], "Enchanter");
				[11], "Priest");
				[12], "Cleric");
				[13], "Adventurer");

				*/
				test_script->SetInterfaceSet(true);
				switch (cClass) {
				
				case 5: //defender
					test_script->GetScriptBinData("inter_job_defender.esf");
					break;
				case 6: //warrior
					test_script->GetScriptBinData("inter_job_warrior.esf");
					break;
				case 7: //Assasin
					test_script->GetScriptBinData("inter_job_assassin1.esf");
					break;
				case 8: //Archer
					test_script->GetScriptBinData("inter_job_archer1.esf");
					break;
				case 9://sorcerer
					test_script->GetScriptBinData("inter_job_sorcerer1.esf");
					break;
				case 10://enchanter
					test_script->GetScriptBinData("inter_job_enchanter1.esf");
					break;
				case 11: //priest
					test_script->GetScriptBinData("inter_job_priest.esf");
					break;
				case 12: //cleric
					test_script->GetScriptBinData("inter_job_cleric.esf");
					break;
				//akhan class
				case 19: //templer
					test_script->GetScriptBinData("inter_job_templar1.esf");
					break;
				case 20: //attacker
					test_script->GetScriptBinData("inter_job_attacker1.esf");
					break;
				case 21: //gunner
					test_script->GetScriptBinData("inter_job_gunner1.esf");
					break;
				case 22: //rune ope
					test_script->GetScriptBinData("inter_job_runeoff1.esf");
					break;
				case 23: //life ope
					test_script->GetScriptBinData("inter_job_lifeoff1.esf");
					break;
				case 24: //shadow ope
					test_script->GetScriptBinData("inter_job_shadowoff1.esf");
					break;

				default:
					break;
				}
				float fX = 640.0f/800.0f;
				float fY = 290.0f/600.0f;

				test_script->m_InterfacePos[0]  = g_ClientMain.m_iScreenWidth*fX;
				test_script->m_InterfacePos[1]  = g_ClientMain.m_iScreenHeight*fY;				
				
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
			} 
			else
			{
				// edith 2008.02.14 leveldown 했을때 스킬 초기화 하는것.
				pGame->m_csStatus.m_Info.Exp = 0;

				// 스킬 초기화
				ZeroMemory(&pGame->m_csStatus.m_Skill, sizeof(pGame->m_csStatus.m_Skill));
				for(int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
				{
					if (pGame->m_csStatus.m_lstSkillSlot[i])
					{
						delete pGame->m_csStatus.m_lstSkillSlot[i];
						pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
					}
				}
/*
				// 스킬창 초기화
				CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
				CRYLSkillPanel* pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;
				pPanel->ClearVirtualSkill();
*/
				pGame->m_csStatus.m_lpQuickSelected = NULL;

				for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
				{
					if (pGame->m_csStatus.m_lstQuickSlot[i])
					{
						delete pGame->m_csStatus.m_lstQuickSlot[i];
						pGame->m_csStatus.m_lstQuickSlot[i] = NULL;
					}
				}

				if (pGame->m_lpPickItem && pGame->m_lpPickItem->m_bSkill)
				{
					delete pGame->m_lpPickItem;
					pGame->m_lpPickItem = NULL;
				}
	
				pGame->m_csStatus.SetClassSkill();
			}

			pGame->m_csStatus.CheckGratherQuickSlot();

			return true;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharQuickSlotMove)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	TakeType		pTakeType ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharQuickSlotMove(lpPktBase, &pTakeType);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (NULL == pGame->m_lpPickItem)
		{
			pGame->m_lpPickItem = pGame->m_csStatus.GetItem(pTakeType.m_srcPos);
		}

		switch (pTakeType.m_srcPos.m_cPos)
		{
			case TakeType::TS_INVEN:	// Move
			{
				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex] = new CItemInstance;
				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]->SetItemInfofromID(
					pGame->m_lpPickItem->GetProtoTypeID(), pTakeType.m_dstPos);	

				CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
				if (lpExistItem)
				{
					unsigned long dwNum = lpExistItem->GetNowDurability() + pGame->m_lpPickItem->GetNowDurability();
					lpExistItem->SetNowDurability(dwNum);

					pGame->m_csStatus.SetQuickSlot(lpExistItem);
					delete pGame->m_lpPickItem;
					pGame->SetPickItem(NULL);
				}
                else
				{
					pGame->m_csStatus.AddInventoryItem(pGame->m_lpPickItem);
					pGame->SetPickItem(NULL);
				}
				return true;
			}

			case TakeType::TS_TEMP:
			{
				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex] = new CItemInstance;
				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]->SetItemInfofromID(
					pGame->m_lpPickItem->GetProtoTypeID(),	pTakeType.m_dstPos);		

				FieldInstance pFI;
				Item::ItemPos pItemIndex;
				pFI.m_dwTypeID = pGame->m_lpPickItem->GetProtoTypeID();		
				pFI.m_cNum = pGame->m_lpPickItem->GetNowDurability();

				if (pGame->m_csStatus.GetAbleInven(&pFI, pItemIndex, true))
				{
					CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
					if (lpExistItem)
					{
                        CRYLNetworkData::SendTakeItem(pGame->m_lpPickItem, 
                            &lpExistItem->m_lpItemBase->GetPos(), 
                            pGame->m_lpPickItem->GetNowDurability());
					} 
                    else
					{
                        CRYLNetworkData::SendTakeItem(pGame->m_lpPickItem, 
                            &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
					}
				}
				return true;
			}
			case TakeType::TS_SSLOT:	// Move
				//by Hades Kang - 2004 12/10  pGame->m_lpPickItem 이 NULL 일 수 있음! ( 네트워크 지연 등등에 의해서 )
				if( pGame->m_lpPickItem )
				{
					CRYLGameData* pGame = CRYLGameData::Instance() ;
					const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( pGame->m_lpPickItem->m_wPreProtoTypeID ) ;
					unsigned short usChildID = 0, usParentID = 0 ;

					if ( lpSkillBase )
					{
						usChildID  = lpSkillBase->m_usChildSkill ;
						usParentID = lpSkillBase->m_usParentSkill ;

						for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
						{
							if (pGame->m_csStatus.m_lstQuickSlot[i])
							{
								if ( pGame->m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usChildID ||
									pGame->m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usParentID )
								{
									pGame->m_lpPickItem->m_lCoolDownTime	  = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime ;
									pGame->m_lpPickItem->m_lCoolDownTimeCount = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount ;
									pGame->m_lpPickItem->m_cCountCooldown     = pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown ;
									break ;
								}								
							}
						}

						unsigned long lCoolDownTimeCount = pGame->m_lpPickItem->m_lCoolDownTimeCount;												
						unsigned char cCountCooldown     = pGame->m_lpPickItem->m_cCountCooldown;

						for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
						{
							if (pGame->m_csStatus.m_lstQuickSlot[i])
							{
								if (pGame->m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == pGame->m_lpPickItem->m_wPreProtoTypeID)
								{
									lCoolDownTimeCount	= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount;
									cCountCooldown		= pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown;
									break;
								}
							}
						}

						pGame->m_lpPickItem->m_lCoolDownTimeCount	= lCoolDownTimeCount;
						pGame->m_lpPickItem->m_cCountCooldown		= cCountCooldown;
					}

					pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex] = pGame->m_lpPickItem;
					pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]->SetPos(pTakeType.m_dstPos);
					
					if (pTakeType.m_cNum == 0)
					{
						CRYLNetworkData::SetChant(pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]);
						pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]);
					}
				}
				else
				{
					pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex] = NULL;
				}
				pGame->SetPickItem(NULL);
				return true;
		}

		switch (pTakeType.m_dstPos.m_cPos)
		{
			case TakeType::TS_QSLOT:	// Swap
			{
				CItemInstance *lpQuick = pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex];

				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex] = pGame->m_lpPickItem;
				pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_dstPos.m_cIndex]->SetPos(pTakeType.m_dstPos);

				if (lpQuick)
				{
					if (lpQuick == pGame->m_csStatus.m_lpQuickSelected)
					{
						pGame->m_csStatus.m_lpQuickSelected = NULL;
					}

					pGame->SetPickItem(lpQuick);
					pGame->m_lpPickItem->SetPos(pTakeType.m_srcPos);
				}
                else
				{
					pGame->SetPickItem(NULL);
				}
				break;
			}

			case TakeType::TS_NONE:		// Delete
			{
				if (pGame->m_lpPickItem &&
                    pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
				{
					delete pGame->m_lpPickItem;
					pGame->SetPickItem(NULL);
				} 
                else
				{
					if (pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_srcPos.m_cIndex])
					{
						delete pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_srcPos.m_cIndex];
						pGame->m_csStatus.m_lstQuickSlot[pTakeType.m_srcPos.m_cIndex] = NULL;
					}
				}
				break;
			}
		}

		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharEliteBonus)
{
	char			cEliteBonus ;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharEliteBonus(lpPktBase, &cEliteBonus);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_cEliteBonus = cEliteBonus;

		return true;
	}

	return false;
}
