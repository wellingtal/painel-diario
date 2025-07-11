
#include "../GameHandler.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Parse/ParseQuest.h>

#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLExecutionQuestPanel.h"
#include "RYLCompleteQuestPanel.h"
#include "RYLQuestDetailDataDlg.h"
#include "RYLLargeMapFrameDlg.h"

#include "QuestProcess.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharStartQuest);
GAME_EVENT_FUNC(ParseCharCancelQuest);
GAME_EVENT_FUNC(ParseCharOperateTrigger);
GAME_EVENT_FUNC(ParseCharQuestInfo);
GAME_EVENT_FUNC(ParseCharEndQuest);

bool GameProcess::RegisterQuestHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;

    dwErrorCount += GameEventHandler.AddHandler(CmdCharStartQuest, ParseCharStartQuest) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharCancelQuest, ParseCharCancelQuest) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharOperateTrigger, ParseCharOperateTrigger) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdQuestDB, ParseCharQuestInfo) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharEndQuest, ParseCharEndQuest) ? 0 : 1;

    return 0 == dwErrorCount;
}

GAME_EVENT_FUNC(ParseCharStartQuest)
{
	unsigned long   dwChrID = 0, dwNpcID = 0;
	unsigned short  wQuestID = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStartQuest(lpPktBase, &dwChrID, &dwNpcID, &wQuestID);

	if (NS_REQUESTQUEST == g_pClientSocket->GetStatusFlag())
	{
		g_pClientSocket->SetStatusFlag( NS_NOWAITING ) ;
	}

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if(dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			LPQuestNode lpQuest = NULL ;

			if (0 == dwNpcID)
			{
				// 아이템 퀘스트 혹은 위치 트리거 퀘스트이다.
				// 혹은 운영자 명령으로 받은 퀘스트 이다.
				lpQuest = g_QuestList.GetQuest( wQuestID ) ;
			}
			else
			{
				NPCNode* lpNode = g_NPCList.GetNPCNode( dwNpcID ) ;
				if ( lpNode )
				{
					lpQuest = lpNode->GetQuestforID( wQuestID ) ;
				}
			}

			if ( lpQuest )
			{
				// CharStatus에 있는 퀘스트 구조체와 스크립트에 있는 퀘스트를 링크시킨다.
				// 자동적으로 페이즈1이 시작
				LPQuestInstance lpQuestInstance = new QuestInstance;
				lpQuestInstance->m_dwQuestID = wQuestID;
				lpQuestInstance->m_lpQuestScript = lpQuest;

				lpQuestInstance->StartPhase(1);
				lpQuestInstance->m_lpQuestIcon = new CRYLSprite;
				lpQuestInstance->m_lpQuestIcon->Create(0, 0, lpQuestInstance->m_lpQuestScript->m_wPosX1,
														lpQuestInstance->m_lpQuestScript->m_wPosY1,
														lpQuestInstance->m_lpQuestScript->m_wPosX2,
														lpQuestInstance->m_lpQuestScript->m_wPosY2, NULL);
				pGame->m_csStatus.m_lstQuest.push_front(lpQuestInstance);



				CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLStatusAndQuestFrameDlg* pDlg = ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
				CRYLExecutionQuestPanel* pPanel	 = ( CRYLExecutionQuestPanel* )pDlg->GetExecutionQuestPanel() ;
				pPanel->InsertExecutionQuest( lpQuestInstance ) ;
			}
		}

		return true;
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktStartQuest::FAIL_NOT_NPC:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2713 ]);
				break;
			}

			case PktStartQuest::FAIL_NOT_HAVE_QUEST:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2714 ]);
				break;
			}

			case PktStartQuest::FAIL_FULL_QUEST:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2712 ]);
				break;
			}

			case PktStartQuest::FAIL_HAS_QUEST:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2711 ]);
				break;
			}

			case PktStartQuest::FAIL_TOO_FAR:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2715 ]);
				break;
			}

			case PktStartQuest::NOT_EXIST_QUEST:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2716 ]);
				break;
			}

			case PktStartQuest::FAIL_TOO_FAR_PARTY:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2717 ]);
				break;
			}

			case PktStartQuest::FAIL_MIN_LEVEL :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2972 ]);
				break;
			}

			case PktStartQuest::FAIL_MAX_LEVEL :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2973 ]);
				break;
			}

			case PktStartQuest::FAIL_MIN_FAME :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2972 ]);
				break;
			}

			case PktStartQuest::FAIL_MAX_FAME :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2973 ]);
				break;
			}

			case PktStartQuest::FAIL_WRONG_NATION :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2951]);
				break;
			}

			case PktStartQuest::FAIL_WRONG_CLASS :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[957]);
				break;
			}

			case PktStartQuest::FAIL_NOT_COMPLETE_PRIOR_QUEST :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[960]);
				break;
			}
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharCancelQuest)
{
	unsigned long	dwChrID  = 0;
	unsigned short	wQuestID = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCancelQuest(lpPktBase, &dwChrID, &wQuestID);

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if(CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			list<LPQuestInstance>::iterator it;
			CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLStatusAndQuestFrameDlg* pDlg = ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
			CRYLExecutionQuestPanel* pPanel	 = ( CRYLExecutionQuestPanel* )pDlg->GetExecutionQuestPanel() ;
			
			for(it = pGame->m_csStatus.m_lstQuest.begin(); it != pGame->m_csStatus.m_lstQuest.end(); it++)
			{
				if((*it)->m_dwQuestID == wQuestID)
				{
					INT iIndex = pGame->m_csStatus.DeleteQuest((*it));
					
					pPanel->DeleteIter( iIndex ) ;
					CRYLLargeMapFrameDlg*	 pMapFrameDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;
					pMapFrameDlg->ShowQuestTarget( NULL ) ;
					break;
				}
			}
		}

		return true;
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktCancelQuest::FAIL_NOT_HAVE_QUEST :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2718 ]);
				break;
			}

			case PktCancelQuest::FAIL_DISABLE_CANCEL :
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2719 ]);
				break;
			}
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharOperateTrigger)
{
	unsigned long   dwChrID  = 0;
	unsigned short  wQuestID = 0;
	unsigned char   cPhase = 0, cTrigger = 0, cCount = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharOperateTrigger(lpPktBase, 
        &dwChrID, &wQuestID, &cPhase, &cTrigger, &cCount);

	BOOL bSuccessTrigger;
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		bSuccessTrigger = true;	
	} else
	{
		bSuccessTrigger = false;
	}

	if(dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		LPQuestInstance lpQuest = pGame->m_csStatus.GetProcessQuest(wQuestID);
		if(lpQuest)
		{
			if(bSuccessTrigger)
			{
				lpQuest->m_lstTriggerCount[cTrigger] = cCount;

				if(lpQuest->m_lstTriggerCount[cTrigger] == 0)
				{
					if(lpQuest->m_dwPresentPhase == cPhase)
					{
						// 현재 페이즈
						lpQuest->StartTrigger(lpQuest->m_lpPhaseScript->m_lstTrigger[cTrigger], bSuccessTrigger);
					} else
					{
						// 다른 페이즈
						lpQuest->StartPhase(cPhase);
						lpQuest->StartTrigger(lpQuest->m_lpPhaseScript->m_lstTrigger[cTrigger], bSuccessTrigger);
					}
				}

				CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;
				// edith 2008.02.12 퀘스트상태 업데이트 로직
				if ( pScene )
				{
					CRYLQuestDetailDataDlg* pDlg = static_cast<CRYLQuestDetailDataDlg*>( pScene->GetQuestDetailDataDlg() ) ;
					pDlg->UpdateQuestMsg( lpQuest );

					if(pScene->GetQuestDetailDataDlg()->GetVisible())
						pDlg->SetDetailDataDlg( lpQuest ) ;
				}
			} 
			else
			{
				if (CRYLNetworkData::Instance()->m_dwNetworkError == PktOperateTrigger::FAIL_EVENT_GET_ERROR)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3569 ]);
				}
				else
				{
					lpQuest->StartTrigger(lpQuest->m_lpPhaseScript->m_lstTrigger[cTrigger], bSuccessTrigger);
				}
			}
		} 
		else
		{
			// 실행중이지 않은 퀘스트
		}
	}

	return true;
}


GAME_EVENT_FUNC(ParseCharQuestInfo)
{
	unsigned long   dwChrID = 0;
	unsigned short  wEQNum  = 0, wHQNum = 0;
	PktQuestDB::ExecutingQuest *lpEQ = NULL;
	unsigned short *lpHQ = NULL;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
		
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharQuestInfo(lpPktBase, &dwChrID, &wEQNum, &lpEQ, &wHQNum, &lpHQ);

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// -----------------------------------------------------------------------------------------------------
		// 이전에 가지고 있던 메모리 해제

		CRYLGameScene*		  pScene    = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLLargeMapFrameDlg* pLargeMapDlg		= ( CRYLLargeMapFrameDlg * )pScene->GetLargeMapDlg() ;

		pLargeMapDlg->ShowQuestTarget( NULL ) ;

		if ( pGame->m_csStatus.m_lstQuest.size() )
		{
			std::list<LPQuestInstance>::iterator itr = pGame->m_csStatus.m_lstQuest.begin() ;
			
			while( itr != pGame->m_csStatus.m_lstQuest.end() )
			{
				LPQuestInstance lpQuest = ( *itr ) ;
				GM_DELETE( lpQuest ) ;
				++ itr ;
			}
		}
		pGame->m_csStatus.m_lstQuest.clear();


		if ( pGame->m_csStatus.m_lstCompleteQuest.size() )
		{
			std::list<LPQuestInstance>::iterator itr = pGame->m_csStatus.m_lstCompleteQuest.begin() ;
			
			while( itr != pGame->m_csStatus.m_lstCompleteQuest.end() )
			{
				LPQuestInstance lpQuest = ( *itr ) ;
				GM_DELETE( lpQuest ) ;
				++ itr ;
			}
		}
		pGame->m_csStatus.m_lstCompleteQuest.clear();

		CRYLStatusAndQuestFrameDlg* pDlg				= ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
		CRYLExecutionQuestPanel* pExecutingQuestPanel	= ( CRYLExecutionQuestPanel* )pDlg->GetExecutionQuestPanel() ;
		CRYLCompleteQuestPanel* pCompleteQuestPanel		= ( CRYLCompleteQuestPanel* )pDlg->GetCompleteQuestPanel() ;
		pExecutingQuestPanel->ClearSlot() ;
		pCompleteQuestPanel->ClearSlot() ;

		// -----------------------------------------------------------------------------------------------------

		unsigned long i;
		for(i = 0; i < wEQNum; i++)
		{
			LPQuestNode lpQuest = g_QuestList.GetQuest(lpEQ[i].m_wQuestID);
			if(lpQuest)
			{
				LPQuestInstance lpQuestInstance = new QuestInstance;
				lpQuestInstance->m_dwQuestID = lpEQ[i].m_wQuestID;
				lpQuestInstance->m_lpQuestScript = lpQuest;
				lpQuestInstance->m_dwPresentPhase = lpEQ[i].m_cPhase;

				if(lpQuestInstance->m_dwPresentPhase>lpQuest->m_wMaxPhase)
					lpQuestInstance->m_dwPresentPhase = lpQuest->m_wMaxPhase;

				lpQuestInstance->m_lpPhaseScript = lpQuestInstance->m_lpQuestScript->GetPhase(lpQuestInstance->m_dwPresentPhase);
				lpQuestInstance->m_lpQuestIcon = new CRYLSprite;
				lpQuestInstance->m_lpQuestIcon->Create(0, 0, lpQuestInstance->m_lpQuestScript->m_wPosX1,
					lpQuestInstance->m_lpQuestScript->m_wPosY1, lpQuestInstance->m_lpQuestScript->m_wPosX2,
					lpQuestInstance->m_lpQuestScript->m_wPosY2, NULL);
				for(unsigned long j = 0; j < PktQuestDB::MAX_TRIGGER; j++)
				{
					lpQuestInstance->m_lstTriggerCount.push_back((unsigned long)lpEQ[i].m_cTriggerCount[j]);
				}

				pGame->m_csStatus.m_lstQuest.push_front(lpQuestInstance);
				pExecutingQuestPanel->InsertExecutionQuest( lpQuestInstance ) ;
			}
		}

		for(i = 0; i < wHQNum; i++)
		{
			LPQuestNode lpQuest = g_QuestList.GetQuest(lpHQ[i]);
			if(lpQuest)
			{
				LPQuestInstance lpQuestInstance = new QuestInstance;
				lpQuestInstance->m_dwQuestID = lpHQ[i];
				lpQuestInstance->m_lpQuestScript = lpQuest;
				lpQuestInstance->m_dwPresentPhase = 1;
				lpQuestInstance->m_lpPhaseScript = lpQuestInstance->m_lpQuestScript->GetPhase(lpQuestInstance->m_dwPresentPhase);
				lpQuestInstance->m_lpQuestIcon = new CRYLSprite;
				lpQuestInstance->m_lpQuestIcon->Create(0, 0, lpQuestInstance->m_lpQuestScript->m_wPosX1,
					lpQuestInstance->m_lpQuestScript->m_wPosY1, lpQuestInstance->m_lpQuestScript->m_wPosX2,
					lpQuestInstance->m_lpQuestScript->m_wPosY2, NULL);

				pGame->m_csStatus.m_lstCompleteQuest.push_front(lpQuestInstance);
				pCompleteQuestPanel->InsertQuestSlot( lpQuestInstance ) ;
			}
		}
		return true;
	}/**/

	return true;
}

GAME_EVENT_FUNC(ParseCharEndQuest)
{
	unsigned long	dwChrID;
	unsigned short	wQuestID;
	bool			bSave;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharEndQuest(lpPktBase, &dwChrID, &wQuestID, &bSave);

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if(dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			LPQuestInstance lpQuest = pGame->m_csStatus.GetProcessQuest(wQuestID);
			if(lpQuest)
			{
				CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLStatusAndQuestFrameDlg* pDlg = ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
				CRYLLargeMapFrameDlg*	pLargeMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;

				pLargeMapDlg->ShowQuestTarget( NULL ) ;

				if(lpQuest->m_lpQuestScript->m_bSave)
				{
					pGame->m_csStatus.AddCompleteQuest(lpQuest);
					CRYLCompleteQuestPanel* pPanel	 = ( CRYLCompleteQuestPanel* )pDlg->GetCompleteQuestPanel() ;
					pPanel->InsertQuestSlot( lpQuest ) ;

                    pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, true, true );
				} 

				INT iIndex = pGame->m_csStatus.DeleteQuest(lpQuest);
				CRYLExecutionQuestPanel* pExePanel	 = ( CRYLExecutionQuestPanel* )pDlg->GetExecutionQuestPanel() ;
				pExePanel->DeleteIter( iIndex ) ;

				pExePanel->HideDetailDataDlg( lpQuest ) ;

				pGame->m_bQuestComplete = TRUE ;
			} 
			else
			{
				// 실행중이지 않은 퀘스트
			}
		}

		return true;
	}

	return true;
}
