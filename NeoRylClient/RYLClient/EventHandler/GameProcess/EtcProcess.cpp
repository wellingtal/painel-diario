
#include <winsock2.h>

#include "EtcProcess.h"

#include "../GameHandler.h"
#include "../../GUITextEdit.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/ClientSocket/Parse/ParseEtc.h>
#include <Network/ClientSocket/Send/SendItem.h>

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLRaceBaseDefine.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h" 
#include "RYLLoadingScene.h"

#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLStatusRetrainDlg.h"
#include "RYLChattingDlg.h"
#include "RYLNoticeManager.h"
#include "RYLLargeMapFrameDlg.h"

#include "SoundMgr.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharBillingTimeoutInfo);
GAME_EVENT_FUNC(ParseCharBillingTimeCheckNotify);
GAME_EVENT_FUNC(ParseCharStateRedistribution);
GAME_EVENT_FUNC(ParseCharStatusRetrain);
GAME_EVENT_FUNC(ParseControlOption);
GAME_EVENT_FUNC(ParseCharLotteryResult);
GAME_EVENT_FUNC(ParseCharBindPos);
GAME_EVENT_FUNC(ParseCharAdmin);
GAME_EVENT_FUNC(ParseCharNameChange);
GAME_EVENT_FUNC(ParseExtraEvent);

bool GameProcess::RegisterEtcHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;

    dwErrorCount += GameEventHandler.AddHandler(CmdBillingTimeoutNotify, ParseCharBillingTimeoutInfo) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdBillingTimeCheckNotify, ParseCharBillingTimeCheckNotify) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharStateRedistribution, ParseCharStateRedistribution) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharStatusRetrain, ParseCharStatusRetrain) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharControlOption, ParseControlOption) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharLotteryResult, ParseCharLotteryResult) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharBindPosition, ParseCharBindPos) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharAdminCmd, ParseCharAdmin) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharNameChange, ParseCharNameChange) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdExtraEvent, ParseExtraEvent) ? 0 : 1;

    return 0 == dwErrorCount;
}

GAME_EVENT_FUNC(ParseCharBillingTimeoutInfo)
{
	unsigned long	dwChrID = 0;
	unsigned char	cRemainMinute = 0, cBillingType = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBillingTimeoutInfo(lpPktBase, 
        &dwChrID, &cRemainMinute, &cBillingType);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA)
		{
			if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
			{
				pGame->m_nPlayTime    = cRemainMinute;
				CRYLNetworkData::Instance()->m_cBillingType = cBillingType;

				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[436], pGame->m_nPlayTime);
				CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

				pGame->m_lBillingTimer = 60000;
			}
		} 
        else
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create("CharBillingTimeoutInfo");
		}

		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharBillingTimeCheckNotify)
{
	unsigned long dwChrID;
	unsigned char cRemainMinute, cBillingType;
    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBillingTimeCheckNotify(lpPktBase, 
        &dwChrID, &cRemainMinute, &cBillingType);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if(dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[481], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
		}
		return true;
	}

	return false;
}


GAME_EVENT_FUNC(ParseCharStateRedistribution)
{
	ChState			pState ;
	SKILL			pSkill ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStateRedistribution(lpPktBase, &pState, &pSkill);
	
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_csStatus.m_Info.Chance--;
		if (pGame->m_csStatus.m_Info.Chance <= 0) pGame->m_csStatus.m_Info.Chance = 0;

		pGame->m_csStatus.m_Info.STR = pState.m_wSTR;
		pGame->m_csStatus.m_Info.DEX = pState.m_wDEX;
		pGame->m_csStatus.m_Info.CON = pState.m_wCON;
		pGame->m_csStatus.m_Info.INT = pState.m_wINT;
		pGame->m_csStatus.m_Info.WIS = pState.m_wWIS;
		pGame->m_csStatus.m_Info.IP = pState.m_wIP;

		// 스킬 초기화
		ZeroMemory(&pGame->m_csStatus.m_Skill, sizeof(pGame->m_csStatus.m_Skill));
		for (int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (pGame->m_csStatus.m_lstSkillSlot[i])
			{
				delete pGame->m_csStatus.m_lstSkillSlot[i];
				pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
			}
		}

		pGame->m_csStatus.m_lpQuickSelected = NULL;

		for (int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
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

		// 스킬 추가
		pGame->m_csStatus.m_Skill = pSkill;

		pGame->m_csStatus.SetClassSkill();
		for (int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill)
			{
				pGame->m_csStatus.m_lstSkillSlot[i] = new CItemInstance;
				pGame->m_csStatus.m_lstSkillSlot[i]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill,
					pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cLockCount, pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cSkillLevel,
                    TakeType::TS_SSLOT, i, 0);
			} else
			{
				pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
			}
		}

		pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

		pGame->m_csStatus.CheckGratherQuickSlot();

		return true;
	} 
	else
	{
		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:
				pChat->AddMessage(CRYLStringTable::m_strString[811], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharStatusRetrain)
{
	ChState			pState ;
	SKILL			pSkill ;
	Item::ItemPos	pItemIndex ;
	unsigned long	dwGold ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStatusRetrain(lpPktBase, &pState, &pSkill, &pItemIndex, &dwGold);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_csStatus.m_Info.STR = pState.m_wSTR;
		pGame->m_csStatus.m_Info.DEX = pState.m_wDEX;
		pGame->m_csStatus.m_Info.CON = pState.m_wCON;
		pGame->m_csStatus.m_Info.INT = pState.m_wINT;
		pGame->m_csStatus.m_Info.WIS = pState.m_wWIS;
		pGame->m_csStatus.m_Info.IP = pState.m_wIP;

		// 스킬 초기화
		ZeroMemory(&pGame->m_csStatus.m_Skill, sizeof(pGame->m_csStatus.m_Skill));
		for (int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (pGame->m_csStatus.m_lstSkillSlot[i])
			{
				delete pGame->m_csStatus.m_lstSkillSlot[i];
				pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
			}
		}

		pGame->m_csStatus.m_lpQuickSelected = NULL;

		for (int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
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

		// 스킬 추가
		pGame->m_csStatus.m_Skill = pSkill;

		pGame->m_csStatus.m_Info.Gold = dwGold;

		// 아이템 삭제
		CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
		if (lpItem)
		{
			unsigned long dwNum = lpItem->GetNowDurability() - 1;
			lpItem->SetNowDurability(dwNum);
			if (lpItem->GetNowDurability() <= 0)
			{
				pGame->m_csStatus.DeleteInventoryItem(lpItem);
				delete lpItem;
				lpItem = NULL;
			} 
			else
			{
				pGame->m_csStatus.SetQuickSlot(lpItem);
			}
		} 

		pGame->m_csStatus.SetClassSkill();
		for (int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill)
			{
				pGame->m_csStatus.m_lstSkillSlot[i] = new CItemInstance;
				pGame->m_csStatus.m_lstSkillSlot[i]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.wSkill,
					pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cLockCount, pGame->m_csStatus.m_Skill.SSlot[i].SKILLINFO.cSkillLevel,
                    TakeType::TS_SSLOT, i, 0);
			} 
			else
			{
				pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
			}
		}

		pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

		CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLStatusRetrainDlg* pDlg		 = ( CRYLStatusRetrainDlg* )pScene->GetStatusRetrainDlg() ;
		if ( pGame->m_lpInterface && pDlg )
		{
			pDlg->SetVisible( FALSE ) ;
		}
		
		pGame->m_csStatus.CheckGratherQuickSlot();

		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseControlOption)
{
	RejectOption	roOption ;
	unsigned long	dwChrID = 0 ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleControlOption(lpPktBase, &dwChrID, &roOption);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_roOption.m_wReject = roOption.m_wReject;
		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharLotteryResult)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID = 0, dwSize = 0;
	char*			lpItemBuffer = NULL;
	Item::ItemPos	itemIndex;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharLotteryResult(lpPktBase, &dwChrID, &itemIndex, &dwSize, &lpItemBuffer);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 복권 결과 아이템의 지급 방식이 서버 베이스로 변경됨에 따라 본 패킷은 에러 처리에만 사용됩니다. (2005-02-16 by 로딘)
		return true;
	} 
	else
	{
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

		switch(CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktLotteryResult::FAIL_BLANK:
				pChat->AddMessage(CRYLStringTable::m_strString[488], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharBindPos)
{
	CRYLNetworkData::Instance()->SetNetworkFlag( NF_NOWAITING ) ;
	unsigned long	dwChrID ;
	unsigned char	cCmd ;
	char			cZone ;
	POS				psPos ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBindPos(lpPktBase, &dwChrID, &cCmd, &psPos, &cZone);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return false;

        CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
        if ( !pSelfModel )
            return false;

		switch(cCmd)
		{
			case 0:
				pChat->AddMessage(CRYLStringTable::m_strString[292], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;
 
			case 1:
			{
				if (cZone == 100)
				{
					CRYLNetworkData::Instance()->m_dwNextZone = CRYLNetworkData::Instance()->m_dwPresentZone;
					CRYLNetworkData::Instance()->m_dwPresentZone = cZone;
					pGame->m_lpInterface->InitZone(CRYLNetworkData::Instance()->m_dwPresentZone);
					
					CRYLGameScene*			 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLLargeMapFrameDlg*	 pDlg	 = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;
					pDlg->LoadLargeMapTexture() ;

					pGame->m_dwClientState = GAME_PLAYING;
				}

				vector3 vecPos = vector3(psPos.fPointX * 100.0f, psPos.fPointY * 100.0f, psPos.fPointZ * 100.0f);
                pSelfModel->SetPosition( vecPos );
				pChat->AddMessage(CRYLStringTable::m_strString[293], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				break;
			}

			case 2:
			{
				vector3 vecPos = vector3(psPos.fPointX * 100.0f, psPos.fPointY * 100.0f, psPos.fPointZ * 100.0f);
                pSelfModel->SetPosition( vecPos );
				pChat->AddMessage(CRYLStringTable::m_strString[293], CRYLNetworkData::Instance()->m_dwMyChrID, 128);

                /*
				// 리콜 스킬
				CEffScript *test_script;
				test_script = new CEffScript;
				if (test_script)
				{
					if (test_script->GetScriptBinData("recall2.esf", pGame->m_bSelfEffect))
					{
						// 스킬 사용시 타격 이팩트 처리
						test_script->SetStartPos(vecPos.x, vecPos.y, vecPos.z);
						test_script->SetEndPos(vecPos.x, vecPos.y, vecPos.z);

                        float fChrDir = pSelfCreature->GetDirection();
						fChrDir = -fChrDir;
						fChrDir -= FLOAT_PHI / 2.0f;

						vector3 vecChrToward;
						vecChrToward.x = cosf(fChrDir);
						vecChrToward.z = sinf(fChrDir);	
						vecChrToward.y = 0.0f;
						vecChrToward.Normalize();
						test_script->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);
						test_script->SetMine(true);
						test_script->SetBSkill(true);

						if (!CSceneManager::m_EffectManager.AddEffScript(test_script))			// 이펙트 스크립트를 읽으면 나오는 인덱스
						{
							delete test_script;
						}
					} 
					else
					{
						delete test_script;
					}
				}

                list<unsigned long> SendCharacterList;

                RYLCreatureManager::Instance()->GetSendCharacterAroundList( SendCharacterList );

                for ( list<unsigned long>::iterator iter = SendCharacterList.begin() ; iter != SendCharacterList.end() ; ++iter )
                {
                    g_pClientSocket->RegisterInstanceUDPList( (*iter) );
                }                

                CastObject castObject;
                castObject.m_wTypeID = 0;
                castObject.m_cObjectType = 200;
                castObject.m_cObjectLevel = 0;
                castObject.m_DstPos.fPointX = vecPos.x;
                castObject.m_DstPos.fPointY = vecPos.y;
                castObject.m_DstPos.fPointZ = vecPos.z;

				SendPacket::CharCastObjectInfo(g_GameSession, 
					CRYLNetworkData::Instance()->m_dwMyChrID, CRYLNetworkData::Instance()->m_dwMyChrID, castObject);

                */                
				break;
			}
		}

		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharAdmin)
{
	unsigned short usCmd;

	CRYLNetworkData::Instance()->SetNetworkFlag( NF_NOWAITING ) ;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAdmin(lpPktBase, &usCmd);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if(!pSelfCreature )
			return true;

		switch(usCmd)
		{
		case PktAdmin::NEO:
			{
				g_CharacterData.m_bFreelook		= TRUE;
				g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
				g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();
			}
			break;
		case PktAdmin::SMITH:
			{

				CRYLGameData::SetAutoRun( FALSE ) ;
				CRYLGameData::Instance()->m_bUpdateMove = FALSE ;
				g_CharacterData.m_bFreelook = FALSE;
			}
			break;
		}

		return true;
	}
	else
	{
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktAdmin::E_LOGOUT_CHAR:
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[2771], CRYLNetworkData::Instance()->m_dwMyChrID, 13 ) ;
			}
			break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharNameChange)
{
	unsigned char cChangeNameCount;
	char *szChangedName;
	Item::ItemPos	pItemIndex;

	CRYLNetworkData::Instance()->SetNetworkFlag( NF_NOWAITING );
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharNameChange(lpPktBase, szChangedName, &cChangeNameCount, &pItemIndex);

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->m_lpRenameWarrantItem = NULL;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
		if(pScene->GetChangeNameDlg()) pScene->GetChangeNameDlg()->SetVisible(FALSE);

		CRYLGameData::Instance()->m_cChangeNameCount = cChangeNameCount;

		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if(pSelfCreature)
		{
			CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
			if(pSelfData) pSelfData->m_strName = szChangedName;
		}
		strcpy(CRYLGameData::Instance()->m_csStatus.m_Info.Name, szChangedName);

		// edith 2008.11.11 계명허가서를 사용했으니 돌 내구도를 1 줄인다.
		if(pItemIndex.m_cPos == TakeType::TS_INVEN)
		{
			// 아이템 삭제
			CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
			if (lpItem)
			{
				unsigned long dwNum = lpItem->GetNowDurability() - 1;
				lpItem->SetNowDurability(dwNum);
				if (lpItem->GetNowDurability() <= 0)
				{
					pGame->m_csStatus.DeleteInventoryItem(lpItem);
					delete lpItem;
					lpItem = NULL;
				} 
				else
				{
					pGame->m_csStatus.SetQuickSlot(lpItem);
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
			case PktCharNameChange::ERR_SERVER:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[296]);
			}
			break;

			case PktCharNameChange::ERR_CANNOT_FIND_CHARACTER:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2945]);
			}
			break;

			case PktCharNameChange::ERR_NAMECHANGE_ITEM:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2946]);
			}
			break;

			case PktCharNameChange::ERR_INVALID_NAME:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2947]);
			}
			break;

			case PktCharNameChange::ERR_CANNOT_USE_NAME:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2948]);
			}
			break;

			case PktCharNameChange::ERR_ALREADY_USE_NAME:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2949]);
			}
			break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseExtraEvent)
{
	// edith 2009.09.05 신규 이벤트 제작부분

	unsigned long	m_dwUserID;         // 유저 ID
	unsigned long	m_dwCharID;         // 캐릭터 ID
	unsigned long	m_dwEventID;			// 이벤트 ID
	unsigned long	m_dwValue1;			// 이벤트 ID
	unsigned long	m_dwValue2;			// 이벤트 ID

	CRYLNetworkData::Instance()->SetNetworkFlag( NF_NOWAITING );
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleExtraEvent(lpPktBase, &m_dwUserID, &m_dwCharID, &m_dwEventID, &m_dwValue1, &m_dwValue2);

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// 30랩 7일 골드박스 이벤트 
		if(m_dwValue1 == 1)
		{
			int day = 0;

			if(m_dwValue2 == 30)
				day = 7;
			else if(m_dwValue2 == 60)
				day = 15;
			else if(m_dwValue2 == 90)
				day = 30;

			pGame->m_lPremiumTime += 1440*day;
			pGame->m_iPremiumType = 1;

			pChat->AddMessage(CRYLStringTable::m_strString[4801], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
		}
		return true;
	} 
	else
	{
/*
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktCharNameChange::ERR_SERVER:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[296]);
			}
			break;
		}
*/
	}

	return false;
}
