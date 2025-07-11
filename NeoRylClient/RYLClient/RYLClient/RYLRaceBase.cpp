
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include <Skill/SkillMgr.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>

#include "WinInput.h"
#include "SkillNode.h"

#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLBlacksmithDlg.h"
#include "RYLExchangeDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLTradeDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLChattingDlg.h"
#include "RYLCampShopDlg.h"

#include "RYLRaceBase.h"
#include "GMMemory.h"

#include "RYLQuickSlotDlg.h"

CRYLRaceBase::CRYLRaceBase()
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	m_bShowHelp = FALSE;

	CTexture::SetPath(INTERFACETEXTUREPATH);
	m_lpChrButtonTexture = new CTexture;
	m_lpChrButtonTexture->Load("namebox.dds");

	m_lpPopupTexture = new CTexture;
	m_lpPopupTexture->Load("msgbox.dds");

	m_lpSocket = new CTexture;
	m_lpSocket->Load("slot.dds");

	m_lpCommonWindow = new CTexture;
	m_lpCommonWindow->Load("clw.dds");

	m_lpHPGauge = new CRYLSprite;
//	m_lpHPGauge->Create(0, 0, 1, 32, 82, 37, m_lpCommonWindow);
	m_lpHPGauge->Create(0, 0, 0, 30, 102, 42, m_lpCommonWindow);

	m_lpMarkIcon = new CTexture;
	m_lpMarkIcon->Load("RankingIcon.dds");

	// 카르테란트
	m_lpMark[0] = new CRYLSprite;
	m_lpMark[0]->Create(0, 0, 0, 13, 12, 25, m_lpMarkIcon);
	m_lpMark[1] = new CRYLSprite;
	m_lpMark[1]->Create(0, 0, 13, 13, 25, 25, m_lpMarkIcon);
	m_lpMark[2] = new CRYLSprite;
	m_lpMark[2]->Create(0, 0, 26, 13, 38, 25, m_lpMarkIcon);
	m_lpMark[3] = new CRYLSprite;
	m_lpMark[3]->Create(0, 0, 39, 13, 51, 25, m_lpMarkIcon);
	m_lpMark[4] = new CRYLSprite;
	m_lpMark[4]->Create(0, 0, 52, 13, 64, 25, m_lpMarkIcon);
	m_lpMark[5] = new CRYLSprite;
	m_lpMark[5]->Create(0, 0, 65, 13, 77, 25, m_lpMarkIcon);
	m_lpMark[6] = new CRYLSprite;
	m_lpMark[6]->Create(0, 0, 78, 13, 90, 25, m_lpMarkIcon);
	m_lpMark[7] = new CRYLSprite;
	m_lpMark[7]->Create(0, 0, 91, 13, 103, 25, m_lpMarkIcon);
	m_lpMark[8] = new CRYLSprite;
	m_lpMark[8]->Create(0, 0, 104, 13, 116, 25, m_lpMarkIcon);
	m_lpMark[9] = new CRYLSprite;
	m_lpMark[9]->Create(0, 0, 117, 13, 129, 25, m_lpMarkIcon);

	// 메르카디아
	m_lpMark[10] = new CRYLSprite;
	m_lpMark[10]->Create(0, 0, 0, 0, 12, 12, m_lpMarkIcon);
	m_lpMark[11] = new CRYLSprite;
	m_lpMark[11]->Create(0, 0, 13, 0, 25, 12, m_lpMarkIcon);
	m_lpMark[12] = new CRYLSprite;
	m_lpMark[12]->Create(0, 0, 26, 0, 38, 12, m_lpMarkIcon);
	m_lpMark[13] = new CRYLSprite;
	m_lpMark[13]->Create(0, 0, 39, 0, 51, 12, m_lpMarkIcon);
	m_lpMark[14] = new CRYLSprite;
	m_lpMark[14]->Create(0, 0, 52, 0, 64, 12, m_lpMarkIcon);
	m_lpMark[15] = new CRYLSprite;
	m_lpMark[15]->Create(0, 0, 65, 0, 77, 12, m_lpMarkIcon);
	m_lpMark[16] = new CRYLSprite;
	m_lpMark[16]->Create(0, 0, 78, 0, 90, 12, m_lpMarkIcon);
	m_lpMark[17] = new CRYLSprite;
	m_lpMark[17]->Create(0, 0, 91, 0, 103, 12, m_lpMarkIcon);
	m_lpMark[18] = new CRYLSprite;
	m_lpMark[18]->Create(0, 0, 104, 0, 116, 12, m_lpMarkIcon);
	m_lpMark[19] = new CRYLSprite;
	m_lpMark[19]->Create(0, 0, 117, 0, 129, 12, m_lpMarkIcon);

	// 써드네이션
	m_lpMark[20] = new CRYLSprite;
	m_lpMark[20]->Create(0, 0, 0, 26, 12, 38, m_lpMarkIcon);
	m_lpMark[21] = new CRYLSprite;
	m_lpMark[21]->Create(0, 0, 13, 26, 25, 38, m_lpMarkIcon);
	m_lpMark[22] = new CRYLSprite;
	m_lpMark[22]->Create(0, 0, 26, 26, 38, 38, m_lpMarkIcon);
	m_lpMark[23] = new CRYLSprite;
	m_lpMark[23]->Create(0, 0, 39, 26, 51, 38, m_lpMarkIcon);
	m_lpMark[24] = new CRYLSprite;
	m_lpMark[24]->Create(0, 0, 52, 26, 64, 38, m_lpMarkIcon);
	m_lpMark[25] = new CRYLSprite;
	m_lpMark[25]->Create(0, 0, 65, 26, 77, 38, m_lpMarkIcon);
	m_lpMark[26] = new CRYLSprite;
	m_lpMark[26]->Create(0, 0, 78, 26, 90, 38, m_lpMarkIcon);
	m_lpMark[27] = new CRYLSprite;
	m_lpMark[27]->Create(0, 0, 91, 26, 103, 38, m_lpMarkIcon);
	m_lpMark[28] = new CRYLSprite;
	m_lpMark[28]->Create(0, 0, 104, 26, 116, 38, m_lpMarkIcon);
	m_lpMark[29] = new CRYLSprite;
	m_lpMark[29]->Create(0, 0, 117, 26, 129, 38, m_lpMarkIcon);

	// 공헌 훈장 텍스쳐 //
	m_lpRealmPointTexture = new CTexture;
	m_lpRealmPointTexture->Load("Star01.DDS");

	m_lpRealmPoint = new CRYLSprite;
	m_lpRealmPoint->Create(0, 0, 0, 0, 15, 15, m_lpRealmPointTexture);

	// 용병 마크 텍스쳐 
	m_lpTacticsMarkTexture = new CTexture;
	m_lpTacticsMarkTexture->Load("mercenary.DDS");

	m_lpTacticsMark = new CRYLSprite;
	m_lpTacticsMark->Create(0, 0, 0, 0, 32, 18, m_lpTacticsMarkTexture);
	
	m_bPopupShow = FALSE;
	pGame->m_dwStartQuickSlot = 0;

	m_dwPartyCmdResult		= 0;
	m_dwExchangeCmdResult	= 0;
	m_dwDuelCmdResult		= 0;
	m_dwPartyDuelCmdResult	= 0;
	m_dwLevelUp				= 0;
	m_dwTacticsReq			= 0;

	m_bQuickRideExchange = FALSE;

	m_bQuickWeaponExchange = FALSE;
	m_bWeaponExchageAble = TRUE;
	m_bShowSkillError = FALSE;

	m_lpSkillGaugeFrame = NULL;
	m_lpSkillGauge[0] = NULL;
	m_lpSkillGauge[1] = NULL;
	m_lpSkillGauge[2] = NULL;
	m_lpSkillGauge[3] = NULL;
	m_lpSkillGauge[4] = NULL;

	m_lpTarget[0] = NULL;
	m_lpTarget[1] = NULL;

	m_lpQuiver = NULL;
	m_lpQuiverD = NULL;

	m_nPreSelectItem = 0xffffffff;

	m_dwCastleDoorUpgradeResult		= 0 ;		// 성문 업그레이드
	m_dwCastleDoorRepairResult		= 0 ;		// 성문 수리
	m_dwCastleDoorRecoveryResult	= 0 ;		// 성문 복구

	m_dwArmsType					= 0 ;		// 개발 하려는 수성병기 타입
	m_dwCastleArmsDevelopeResult	= 0 ;		// 수성병기 개발
	m_dwCastleArmsRideResult		= 0 ;		// 수성병기 탑승
	m_dwCastleArmsRepairResult		= 0 ;		// 수성병기 수리
	m_dwCastleArmsUpgradeResult		= 0 ;		// 수성병기 업그레이드
	m_dwCastleArmsDestroyResult		= 0 ;		// 수성병기 파괴

	m_dwSiegeArmsRideResult					= 0;			// edith 공성병기탑승오류 버그수정, 변수초가외 문제.
	m_dwSiegeArmsRepair						= 0;
	m_dwSiegeArmsStartKitRecoveryResult		= 0 ;			// 공성병기 스타터킷으로 복구
	m_dwSiegeArmsSelectID					= 0xffffffff ;

	m_dwCampCreateResult			= 0 ;		// 진지 생성
	m_dwCampCareResult				= 0 ;		// 진지 손질하기
	m_dwCampRepairResult			= 0 ;		// 진지 수리
	m_dwCampDestoryResult			= 0 ;		// 진지 파괴
	m_dwCampRepairPrice				= 0 ;		// 진지 수리 비용
	m_dwWorldWeaponCreateResult		= 0 ;		// 월드 웨폰 생성
	m_dwWorldWeaponDestroyResult	= 0 ;		// 월드 웨폰 파괴

	// WORK_LIST 2.3 계정 국적 변경 기능 구현
	m_dwChangeNationResult			= 0 ;		// 계정 국적 변경 관련

	m_dwNormalTargetChrID	= 0xFFFFFFFF;
	m_dwSpecialTargetChrID	= 0xFFFFFFFF;

	m_pCastingEffect		= NULL;

	for(unsigned char cIndex =0; cIndex < MAX_STATUE_SPRITE; cIndex++)
	{
		m_lpStatue[cIndex] = NULL;
	}
}

CRYLRaceBase::~CRYLRaceBase()
{
	if (m_lpSkillGaugeFrame)	{ delete m_lpSkillGaugeFrame;	m_lpSkillGaugeFrame = NULL; }

	if (m_lpSkillGauge[0]) 		{ delete m_lpSkillGauge[0]; 	m_lpSkillGauge[0] = NULL; 	}
	if (m_lpSkillGauge[1]) 		{ delete m_lpSkillGauge[1]; 	m_lpSkillGauge[1] = NULL; 	}
	if (m_lpSkillGauge[2]) 		{ delete m_lpSkillGauge[2]; 	m_lpSkillGauge[2] = NULL; 	}
	if (m_lpSkillGauge[3]) 		{ delete m_lpSkillGauge[3]; 	m_lpSkillGauge[3] = NULL; 	}
	if (m_lpSkillGauge[4]) 		{ delete m_lpSkillGauge[4]; 	m_lpSkillGauge[4] = NULL; 	}

	if (m_lpTarget[0]) 			{ delete m_lpTarget[0]; 		m_lpTarget[0] = NULL; 		}
	if (m_lpTarget[1]) 			{ delete m_lpTarget[1]; 		m_lpTarget[1] = NULL; 		}
	if (m_lpQuiver) 			{ delete m_lpQuiver;			m_lpQuiver = NULL; 			}
	if (m_lpQuiverD) 			{ delete m_lpQuiverD;			m_lpQuiverD = NULL; 			}

	if (m_lpSocket) 			{ delete m_lpSocket;			m_lpSocket = NULL; 			}

	delete m_lpMarkIcon;
	m_lpMarkIcon = NULL ;

	for (int i = 0; i < MAX_RANKING_SPRITE; i++)
	{
		if (NULL != m_lpMark[i]) 
		{ 
			delete m_lpMark[i]; 
			m_lpMark[i] = NULL; 
		}
	}

	for (int i = 0; i < MAX_STATUE_SPRITE; i++)
	{
		if (NULL != m_lpStatue[i]) 
		{ 
			delete m_lpStatue[i]; 
			m_lpStatue[i] = NULL; 
		}
	}	

	if(m_lpRealmPointTexture)
	{
		delete m_lpRealmPointTexture;
		m_lpRealmPointTexture = NULL;
	}

	if(m_lpTacticsMarkTexture)
	{
		delete m_lpTacticsMarkTexture;
		m_lpTacticsMarkTexture = NULL;
	}

	if(m_lpRealmPoint)
	{
		delete m_lpRealmPoint;
		m_lpRealmPoint = NULL;
	}	

	if(m_lpTacticsMark)
	{
		delete m_lpTacticsMark;
		m_lpTacticsMark = NULL;
	}		

	delete m_lpChrButtonTexture;
	delete m_lpPopupTexture;
	delete m_lpCommonWindow;
	delete m_lpHPGauge;

	DestroyAllObject();
}

BOOL CRYLRaceBase::GetIsTrade(void) 
{
	CRYLGameScene*		pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

	CRYLStreetStallDlg*	pStreet		= ( CRYLStreetStallDlg*	)pScene->GetStreetStallDlg();
    CRYLNewTradeDlg*	pTrade		= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg();
	CRYLBlacksmithDlg*	pBlacksmith = ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg();
	CRYLCampShopDlg*	pCampShop	= ( CRYLCampShopDlg* )pScene->GetCampShopDlg();

	if (TRUE == pStreet->GetVisible() || 
		TRUE == pTrade->GetVisible() || 
		TRUE == pBlacksmith->GetVisible() || 
		TRUE == pCampShop->GetVisible())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRYLRaceBase::GetBusy(void) 
{
	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	

	if( pScene->GetNewTradeDlg()->GetVisible() ||					
		pScene->GetStreetStallDlg()->GetVisible() ||				
		pScene->GetQuestOpeningDlg()->GetVisible() ||			
		pScene->GetBlacksmithDlg()->GetVisible() ||				
		pScene->GetStatusRetrainDlg()->GetVisible() ||			
		pScene->GetExchangeDlg()->GetVisible() ||				
		pScene->GetJobChangeDlg()->GetVisible() ||				
		pScene->GetSystemDlg()->GetVisible() ||					
		pScene->GetOptionDlg()->GetVisible() ||					
		pScene->GetVaultDlg()->GetVisible() ||					
		pScene->GetVaultPasswordDlg()->GetVisible() ||			
		pScene->GetVaultCertificationdDlg()->GetVisible() ||
		pScene->GetEmblemUpgradeDlg()->GetVisible() ||
		pScene->GetMiningAmountProcessDlg()->GetVisible() ||
		pScene->GetMiningCampDlg()->GetVisible() ||
		pScene->GetMiningAmountDlg()->GetVisible() 
		)
	{
		return TRUE;
	}

	return FALSE;
}

void CRYLRaceBase::ProcessExchangeCmd(unsigned long dwSenderID, unsigned short wCmd)
{
	CRYLGameScene*				pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLHumanInventoryFrameDlg*	pInvenDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
	CRYLExchangeDlg*			pExchangeDlg	= ( CRYLExchangeDlg* )pScene->GetExchangeDlg();

	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	switch (wCmd)
	{
		case PktExC::EXC_ACCEPT:
			pInvenDlg->SetVisible( TRUE );
			pExchangeDlg->SetVisible( TRUE );
			break;

		case PktExC::EXC_OK:
			pExchangeDlg->ExchangeAccept( dwSenderID );
			break;

		case PktExC::EXC_CANCEL:
			pExchangeDlg->ExchangeCancel( dwSenderID );
			break;

		case PktExC::EXC_EXCHANGE:
			pExchangeDlg->ProcessExchange();

			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[153], pNetwork->m_dwMyChrID, 128);
			break;

		case PktExC::EXC_QUIT:
			pExchangeDlg->ExchangeQuit();

			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[154], pNetwork->m_dwMyChrID, 129);
			break;

		case PktExC::EXC_LOCK:
			pExchangeDlg->ExchangeLock( dwSenderID );
			break;

		case PktExC::EXC_UNLOCK:
			pExchangeDlg->ExchangeUnLock( dwSenderID );
			break;

		case 100:
			if ( !pGame->m_lpPickItem && 
				pExchangeDlg->m_bSingleWindow && 
				pGame->m_csStatus.m_lstExchange.empty() )
			{
				pExchangeDlg->SetVisible( FALSE );
			}
			break;
	}
}

void CRYLRaceBase::HideInterfaceforDead(void) 
{
	m_bPopupShow = FALSE;
	m_dwPopupSelect = 0xFFFFFFFF;

	g_MessageBoxManager.DestroyList();

	if (m_bShowHelp)
	{
		m_bShowHelp = FALSE;
		CRYLGameData*	pGame = CRYLGameData::Instance();
	}

	CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	pScene->GUIVisible( FALSE );
}

bool CRYLRaceBase::PickItem(DWORD nSelectItem)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return false;

	FieldInstance* pInstance = pGame->GetFieldObject(nSelectItem);
	if (NULL != pInstance)
	{
		vector3 vecItem = vector3(pInstance->m_vecPos.x, 0.0f, pInstance->m_vecPos.z);
		vector3 vecSelf = *( pSelfCreature->GetPosition() );
		vecSelf.y = 0.0f;
		vecSelf -= vecItem;
		if (vecSelf.GetLens() < 300.0f)
		{
			CRYLNetworkData::SendFieldObjectPickUp(pInstance->m_nOID, Item::ItemPos());		

// 서버에서 빈 공간을 체크하도록 루틴 변경 (2005-04-28 by 로딘)
/*
			Item::ItemPos	pItemIndex;

			bool bAbleInven = false;

			if (!(pInstance->m_dwTypeID & FieldObject::Gold))
			{
				bAbleInven = pGame->m_csStatus.GetAbleInven(pInstance, pItemIndex);
			} 
			else
			{
				bAbleInven = true;
				pItemIndex.m_cIndex = 0;
				pItemIndex.m_cPos = 0;
			}

			if (bAbleInven)
			{
				CRYLNetworkData::SendFieldObjectPickUp(pInstance->m_nOID, pItemIndex);
			} 
			else
			{
				CRYLChattingDlg::Instance()->AddMessage(
                    CRYLStringTable::m_strString[13], pNetwork->m_dwMyChrID, 129);
			}
*/

			return true;
		}
	}

	return false;
}

void CRYLRaceBase::DestroyAllObject(void)
{
	for (vector<CTexture *>::iterator itTexture = m_lstTexture.begin(); itTexture != m_lstTexture.end(); ++itTexture)
	{
		delete (*itTexture);
	}
	m_lstTexture.clear();

	CGUIWindow* pWindow;
	for (vector<CGUIWindow *>::iterator itWindow = m_lstWindow.begin(); itWindow != m_lstWindow.end(); ++itWindow)
	{
		pWindow = (*itWindow);
		delete (*itWindow);
	}
	m_lstWindow.clear();

	// Minotaurs 퀘스트 스크롤 콘테이너 비우기
	std::map<unsigned long,int>::iterator itr = m_mapQuestQuestion.begin();
	while(itr != m_mapQuestQuestion.end())
	{
		CSceneManager::DeleteInstanceObject(itr->second);
		++itr;
	}
	m_mapQuestQuestion.clear();

	itr = m_mapQuestExclamation.begin();
	while(itr != m_mapQuestExclamation.end())
	{
		CSceneManager::DeleteInstanceObject(itr->second);
		++itr;
	}
	m_mapQuestExclamation.clear();
}

BOOL CRYLRaceBase::CheckSocialAction(char* token)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();

	const int MAX_ID = 3;
	unsigned short ckSkillID[MAX_ID] = { 0x4001, 0x4002, 0x4003 };

	char szCmd[MAX_ID][5][16] = {
		{ "/laugh",		"/despair",		"/shy",		"/surprised",	"/roar" },
		{ "/wave",		"/nod",			"/no",		"/thank",		"/salute" },
		{ "/cheer",		"/sorry",		"/talk",	"/charge",		"/comfort" }
	};

	for(int id = 0; id < MAX_ID; ++id)
	{
		for(int lock = 0; lock < 5; ++lock)
		{
			if(strlen(szCmd[id][lock]) == 0)
				continue;

			if(strcmp(szCmd[id][lock], token) == 0)
			{
				if(ckSkillID[id] == 0)
					return TRUE;

				return pGame->m_csStatus.UseSocialAction(ckSkillID[id], lock);
			}
		}
	}

	return FALSE;
}

void CRYLRaceBase::ProcessSkill()
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return;

	// edith FALSE->TRUE  스킬발동시 체크로직
	// 스킬 발동
	CRYLGameData::CheckTargetforSkill(m_dwSpecialTargetChrID, FALSE);

	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	if (pGame->m_csStatus.m_lpQuickSelected)
	{
		if (g_DeviceInput.KeyUp(VK_CONTROL) && m_dwSpecialTargetChrID != pNetwork->m_dwMyChrID)
		{
			// 컨트롤을 띠었는데 타겟이 내가 아니면 캐스팅을 중지해라
			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			pSelfCreature->SetStillCasting( false );

			return;
		}

		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();

		const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(pGame->m_csStatus.m_lpQuickSelected->GetSkillID());
		if (pSkillType == NULL)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			pSelfCreature->SetStillCasting( false );
			if (!m_bShowSkillError)
			{
				pChat->AddMessage(CRYLStringTable::m_strString[144], pNetwork->m_dwMyChrID, 129);
				m_bShowSkillError = TRUE;
			}

			return;
		}
		
		if (m_dwSpecialTargetChrID & Creature::MONSTER_BIT)
		{
			unsigned long dwMonID = m_dwSpecialTargetChrID & 0x0000FFFF;
			if((3100 < dwMonID && dwMonID < 3500) && ((0x9E03 == pSkillType->m_usSkill_ID) || (0x9E04 == pSkillType->m_usSkill_ID)))
			{
				bool bExt = true;//false;
				const int CNT = 13;
				const unsigned short usSkillID[CNT] = { 0x9E00, 0x9E01, 0x9E02, 0x9E03, 0x9E04, 
														0x9E03, 0x9E04, 0x9E03, 0x9E04, 0x9E03, 
														0x9E04, 0x9E03, 0x9E04 };

				const unsigned short usMonID[CNT] = { 3100, 3150, 3200, 3225, 3250, 
														3275, 3300, 3325, 3350, 3375, 
														3400, 3425, 3450 };

				const char cLock[CNT] = { 0, 0, 0, 0, 0,
										1, 1, 2, 2, 3, 
										3, 4, 4 };

				const unsigned char cSkillLockCount = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();

				/*if(0x9E03 == pSkillType->m_usSkill_ID && (usMonID[0] < dwMonID && dwMonID <= usMonID[1]) )
					bExt = true;

				if(0x9E04 == pSkillType->m_usSkill_ID && (usMonID[1] < dwMonID && dwMonID <= usMonID[2]))
					bExt = true;

				if(!bExt)
				{
					for(int xx = 1; xx < CNT; ++xx)
					{
						if(usSkillID[xx] == pSkillType->m_usSkill_ID &&
							(usMonID[xx-1] < dwMonID && dwMonID <= usMonID[xx])(dwMonID == usMonID[xx]) &&
							cSkillLockCount >= cLock[xx] )
						{
								bExt = true;
								break;
						}
					}
				}*/

				if(!bExt || Skill::Type::GATHER != pSkillType->m_eSkillType) // && Skill::Type::INSTANCE != pSkillType->m_eSkillType)
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
					return;
				}
			}
			else
			{
				if(Skill::Type::GATHER == pSkillType->m_eSkillType)
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
					return;
				}
			}
		}

		// edith 2009.07.16 점프중 스킬 버그 테스트 루틴 dwResult값이 11일때 오류를 출력 
		// 11일때를 디버깅하자.
		unsigned long dwResult = pGame->m_csStatus.CheckUseSkill(); // Minotaurs
		if (0 != dwResult)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			pSelfCreature->SetStillCasting( false );

			// 사용한 능력이 스킬 형식이 아니면 아래에서 처리를한다.
			switch (pGame->m_csStatus.m_lpQuickSelected->GetItemType())
			{
			case Item::ItemType::POTION:
			case Item::ItemType::EXP_BOOK:
			case Item::ItemType::CASH_BOOK:
			case Item::ItemType::CASH_ITEM:
			case Item::ItemType::EVENT_LOTTERY:
			case Item::ItemType::PORTAL:
			case Item::ItemType::FIRE_CRACKER:
			case Item::ItemType::SKILL_BOOK:
			case Item::ItemType::FIXED_PORTAL:
			case Item::ItemType::SKILL_ITEM:
				CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLQuickSlotDlg*	pDlg	= static_cast<CRYLQuickSlotDlg*>(pScene->GetQuickSlotDlg());
				
				// 위의 설정이면 아이템을 사용한걸로 처리한다.
				pDlg->UseItem(pGame->m_csStatus.m_lpQuickSelected);

				return;
			}

			if (FALSE == m_bShowSkillError)
			{
				char* lpBuffer = NULL;
				switch (dwResult)
				{
					case 1:		lpBuffer = CRYLStringTable::m_strString[814];					break;
					case 2:		lpBuffer = CRYLStringTable::m_strString[143];					break;
					case 3:		lpBuffer = CRYLStringTable::m_strString[815];					break;
					case 4:		lpBuffer = CRYLStringTable::m_strString[817];					break;
					case 5:		lpBuffer = CRYLStringTable::m_strString[2722];					break;

					case 6:		
					case 7:		lpBuffer = CRYLStringTable::m_strString[2723];					break;

					case 8:		lpBuffer = CRYLStringTable::m_strString[2724];					break;
					case 9:		sprintf(lpBuffer, CRYLStringTable::m_strString[4236], 10000);	break;
					case 10:	sprintf(lpBuffer, CRYLStringTable::m_strString[4237], 4000);	break;
					case 11:	lpBuffer = CRYLStringTable::m_strString[4238];					break;

				}

				if (NULL != lpBuffer)
				{
					pChat->AddMessage(lpBuffer, pNetwork->m_dwMyChrID, 129);
				}

				m_bShowSkillError = TRUE;
			}

			return;
		}

		// edith 2008.05.11 파트2용 챈트아이콘이다 로우용 아이디로 수정해줘야한다.
		if (pGame->m_csStatus.m_lpQuickSelected->GetSkillIDbyChantID(Skill::SpellID::Stealth))
		{
			char strText[MAX_PATH];
			BOOL bError = FALSE;

			if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Slow ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[816]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hold ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[802]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[803]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Frozen ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[804]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Poisoned ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[805]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::LowerStrength ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[806]);
			} 
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::ArmorBroken ))
			{ 
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[2224]);
			}
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::DeEnchant ))
			{ 
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[2224]);
			}
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Fired ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[4228]);
			}
			else if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::CurseOfBlind ))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[4229]);
			}
			if (bError)
			{
				g_CharacterData.m_bSkillExecuted = FALSE;
				pSelfCreature->CancelSkill();
				pSelfCreature->SetStillCasting( false );
				if (!m_bShowSkillError)
				{
					pChat->AddMessage(strText, pNetwork->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;
			}
		}

		// edith 캐스팅을 걸 타겟을 선택한다.
		unsigned long dwFunction;
		if ( CRYLGameData::CheckStillCasting(dwFunction))
		{
			if (dwFunction != 0xFFFFFFFF) 
				m_dwSpecialTargetChrID = dwFunction;

		}
/*
		// edith 2008.01.21 스킬사용시 타겟이 없을때 자기 자신에게 걸수있는 스킬을 지정한다.
		// edith 2008.05.10 타겟이 없으면 자기자신한테 시전하는 기능을 뺀다.
		if(m_dwSpecialTargetChrID == 0xFFFFFFFF)
		{
			if(pSkillType->m_eTargetType == Skill::Target::FRIEND)
			{
				m_dwSpecialTargetChrID = CRYLNetworkData::Instance()->m_dwMyChrID;
			}
		}
*/
		switch (pSkillType->m_eTargetType)
		{
			case Skill::Target::DEAD_ENEMY:
			case Skill::Target::FRIEND:
			case Skill::Target::ENEMY:
			case Skill::Target::DEAD_FRIEND:
			case Skill::Target::FRIEND_OBJECT:
			case Skill::Target::ENEMY_OBJECT:
			case Skill::Target::PARTY:
			case Skill::Target::FRIEND_EXCEPT_SELF:
				if (pSkillType->m_fMaxRange != 0.0f && m_dwSpecialTargetChrID == 0xFFFFFFFF)
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[145], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}

					return;
				}
				break;
		}


		if (Item::ItemType::SKILL_ITEM != pGame->m_csStatus.m_lpQuickSelected->GetItemType())
		{
			if ( Skill::Type::CAST != pSkillType->m_eSkillType )
			{
				const unsigned char cSkillLockCount = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();
				const unsigned char cSkillLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();

				// 마나 체크 부분
				if ((pSkillType[cSkillLockCount].m_StartMP + pSkillType[cSkillLockCount].m_LockMP + 
					(pSkillType[cSkillLockCount].m_LevelMP + pSkillType[cSkillLockCount].m_LockMP) * cSkillLevel) > 
					static_cast<unsigned short>(pGame->m_csStatus.GetCurrentMP()))
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( FALSE );
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[146], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}

					return;
				}
			}
			else		
			{
				// edith 캐스팅할때 검사한다.
				// 캐스트 타입은 맨 처음것만 확인
				const unsigned char cSkillLockCount = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();
				const unsigned char cSkillLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();

				if ((pSkillType->m_StartMP + pSkillType[cSkillLockCount].m_LockMP + 
					pSkillType->m_LevelMP + pSkillType[cSkillLockCount].m_LockMP) > 
					static_cast<unsigned short>(pGame->m_csStatus.GetCurrentMP()))
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[146], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}

					return;
				}
			}
		}

		BOOL bUseLimit = TRUE;
		unsigned char itemType = 0, itemDurability = 0;

		switch (pSkillType->m_eUseLimit)
		{
			case Skill::UseLimit::FOR_FIGHTER:
				if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetNowDurability();
					if ( itemType == Item::ItemType::ONEHANDED_SWORD ||
						itemType == Item::ItemType::ONEHANDED_AXE ||
						itemType == Item::ItemType::ONEHANDED_BLUNT ||
						itemType == Item::ItemType::TWOHANDED_BLUNT ||
						itemType == Item::ItemType::TWOHANDED_AXE ||
						itemType == Item::ItemType::TWOHANDED_SWORD)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[147], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::FOR_FIGHTER_TWOHANDED:
				if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetNowDurability();
					if (itemType == Item::ItemType::TWOHANDED_BLUNT ||
						itemType == Item::ItemType::TWOHANDED_AXE ||
						itemType == Item::ItemType::TWOHANDED_SWORD)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[800], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::WITH_DAGGER:
				if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetNowDurability();
					if (itemType == Item::ItemType::DAGGER)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[148], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::FOR_ALL_ONEHAND:
				if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetNowDurability();
					if (itemType == Item::ItemType::ONEHANDED_SWORD ||
						itemType == Item::ItemType::ONEHANDED_BLUNT ||
						itemType == Item::ItemType::ONEHANDED_AXE)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[149], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::RANGED:
				if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetNowDurability();
					if (itemType == Item::ItemType::BOW ||
						itemType == Item::ItemType::CROSSBOW)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[150], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::NONE:
				bUseLimit = FALSE;
				break;

			case Skill::UseLimit::WITH_WEAPON:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::COM_BLUNT ||
						itemType == Item::ItemType::COM_SWORD ||
						itemType == Item::ItemType::OPP_AXE ||
						itemType == Item::ItemType::OPP_HAMMER ||
						itemType == Item::ItemType::OPP_SLUSHER)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[149], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::WITH_CLOW:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::OPP_TALON)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[500], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::LEFT_ARM:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetNowDurability();
					if (itemDurability > 0)
					{
						bUseLimit = FALSE;
					}
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} 
				else
				{
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[501], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::GUARD_ARM:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::SKILL_A_GUARD)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[502], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} 
				else
				{
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[502], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::ATTACK_ARM:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::SKILL_A_ATTACK)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[503], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} 
				else
				{
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[503], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::GUN_ARM:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::SKILL_A_GUN)
					{
						// 건사용시 탄환 소비
						unsigned short wAmmo = 4003;
						CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(wAmmo);
						if (lpItem)
						{
							if (itemDurability > 0)
							{
								bUseLimit = FALSE;
							}
							else
							{
								if (!m_bShowSkillError)
								{
									pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
									m_bShowSkillError = TRUE;
								}
							}
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[504], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[505], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} 
				else
				{
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[505], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::KNIFE_ARM:
				if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					itemType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType();
					itemDurability = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetNowDurability();
					if (itemType == Item::ItemType::SKILL_A_KNIFE)
					{
						if (itemDurability > 0)
						{
							bUseLimit = FALSE;
						}
						else
						{
							if (!m_bShowSkillError)
							{
								pChat->AddMessage(CRYLStringTable::m_strString[271], pNetwork->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} 
					else
					{
						if (!m_bShowSkillError)
						{
							pChat->AddMessage(CRYLStringTable::m_strString[506], pNetwork->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} 
				else
				{
					if (!m_bShowSkillError)
					{
						pChat->AddMessage(CRYLStringTable::m_strString[506], pNetwork->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;
		}

		if (bUseLimit)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			pSelfCreature->SetStillCasting( false );
			return;
		}

		unsigned long dwLowerAni = pSelfCreature->GetLowerActionID();
		if (dwLowerAni != CA_CASTING && dwLowerAni != CA_AIMEDSHOT1 && dwLowerAni != CA_AIMEDSHOT2)
		{
			if (pSkillType->m_usSkill_ID == 0x8803 || pSkillType->m_usSkill_ID == 0x9506)				// 에임드 샷
			{
				pSelfCreature->BeginCast("AIMED_SHOT1");
			} else
			{
				// 캐스팅 동작 애니메이션을 설정한다.
				pSelfCreature->BeginCast("CASTING");
			}
		}

		if (Skill::Type::INSTANCE == pSkillType->m_eSkillType || Skill::Type::GATHER == pSkillType->m_eSkillType)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			// edith 2009.04.26 점프중 스킬사용 막기
			if ( pSelfCreature->GetFlying(true) && !strcmp(pSkillType->m_szCastingFileName, "SHOT"))
			{
				pGame->m_bSkillFlag = TRUE;

				pSelfCreature->EndSkill("JUMP_SHOT");
				pSelfCreature->m_ulSkillComboCount = 0;
				pSelfCreature->m_bSkillCombo = false;

				// 스킬을 집어넣는다.
				unsigned long dwLock = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();
				unsigned long dwLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();
				pGame->m_csStatus.CheckUseSkillforStatus(dwLock, dwLevel);

				CSkillNode* pSkillNode = new CSkillNode;
				pSkillNode->Create(m_dwSpecialTargetChrID, 
					pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), dwLock, dwLevel, 
					pGame->m_csStatus.m_lpQuickSelected);

				pSelfCreature->SetStillCasting( false );

				m_bShowSkillError = TRUE;
			} 
			else if (pSkillType->m_bGauge)
			{
				pGame->m_bSkillFlag = TRUE;

				pSkillType = &pSkillType[pSelfCreature->m_ulSkillComboCount];
				if (strcmp(pSkillType->m_szCastingFileName, "NULL"))
				{
					pSelfCreature->EndSkill((char *)pSkillType->m_szCastingFileName);
				}

				// 스킬을 집어넣는다.
				unsigned long dwGrade, dwLevel;
				dwGrade = pSelfCreature->m_ulSkillComboCount;
				if (pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount() == pSelfCreature->m_ulSkillComboCount)
				{
					dwLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();
					pSelfCreature->m_ulSkillComboCount = 0;
					pSelfCreature->m_bSkillCombo = false;
				} 
				else
				{
					dwLevel = 6;
					pSelfCreature->m_ulSkillComboCount++;
					pSelfCreature->m_bSkillCombo = TRUE;
				}

				CSkillNode* pSkillNode = new CSkillNode;
				pSkillNode->Create(m_dwSpecialTargetChrID, 
					pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), dwGrade, dwLevel, 
					pGame->m_csStatus.m_lpQuickSelected);

				pSelfCreature->SetStillCasting( false );

				m_bShowSkillError = TRUE;
			} 
			else
			{
				pGame->m_bSkillFlag = TRUE;

				if (strcmp(pSkillType->m_szCastingFileName, "NULL"))
				{
					pSelfCreature->EndSkill((char *)pSkillType->m_szCastingFileName);
				}

				pSelfCreature->m_ulSkillComboCount = 0;
				pSelfCreature->m_bSkillCombo = false;

				// 스킬을 집어넣는다.
				unsigned long dwLock = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();
				unsigned long dwLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();
				pGame->m_csStatus.CheckUseSkillforStatus(dwLock, dwLevel);

				CSkillNode* pSkillNode = new CSkillNode;
				pSkillNode->Create(m_dwSpecialTargetChrID, 
					pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), dwLock, dwLevel, 
					pGame->m_csStatus.m_lpQuickSelected);

				pSelfCreature->SetStillCasting( false );

				m_bShowSkillError = TRUE;
			}
		} 
		else if (Skill::Type::ACTION == pSkillType->m_eSkillType)
		{
			unsigned long dwLock = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();
			unsigned long dwLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();

			// 스킬 레벨에 맞는 스킬을 지정한다.
			pSkillType = &pSkillType[dwLock];
			if (strcmp(pSkillType->m_szCastingFileName, "NULL"))
			{
				pSelfCreature->EndSkill((char *)pSkillType->m_szCastingFileName);
			}

			pSelfCreature->m_ulSkillComboCount = 0;
			pSelfCreature->m_bSkillCombo = false;

			// 스킬을 집어넣는다.
			pGame->m_csStatus.CheckUseSkillforStatus(dwLock, dwLevel);

			// edith 2009.08.06 캐릭터가 스킬을 쓸때 동작을 초기화한다.
			CRYLNetworkData::SendMovingPacket( CA_WAIT, CA_WAIT,
				pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()) );

			pGame->SetSkillCoolDown(dwLock, pSkillType->m_cEndCoolDown);
			
			BOOL bGod = TRUE;		// 무적 상태
			BOOL bCanDead = FALSE;	// 죽은 사람
			g_CharacterData.MakeSkillAttackInfo(m_dwSpecialTargetChrID, 
				pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), dwLock, 
				dwLevel, pSkillType->m_cStrikeNum, bGod, bCanDead);

/*
			CSkillNode* pSkillNode = new CSkillNode;
			pSkillNode->Create(m_dwSpecialTargetChrID, 
				pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), dwLock, dwLevel, 
				pGame->m_csStatus.m_lpQuickSelected);
*/
			pSelfCreature->SetStillCasting( false );

			m_bShowSkillError = TRUE;
		}
		else if (g_CharacterData.m_bSkillExecuted)
		{
			// edith
			// 이부분이 스킬을 쐈을때 이팩트가 날라가는 부분이다.

			g_CharacterData.m_bSkillExecuted = FALSE;
			pSelfCreature->CancelSkill();
			pSelfCreature->m_ulSkillComboCount = 0;
			pSelfCreature->m_bSkillCombo = false;

			if (RYLCreatureManager::Instance()->GetCreature( m_dwSpecialTargetChrID ) )
			{
				if (strcmp(pSkillType->m_szCastingFileName, "NULL"))
				{
					pSelfCreature->EndSkill((char *)pSkillType->m_szCastingFileName);
				}

				// 스킬을 집어넣는다.
				CSkillNode* pSkillNode = new CSkillNode;
				pSkillNode->Create(m_dwSpecialTargetChrID, 
					pGame->m_csStatus.m_lpQuickSelected->GetSkillID(),
					g_CharacterData.m_dwCastingSkillGrade, g_CharacterData.m_dwCastingSkillTick, 
					pGame->m_csStatus.m_lpQuickSelected);

				pSelfCreature->SetStillCasting( false );
			} 
			else
			{
				// 날씨 밝아지게 하는거 ??
				float fTime = CSceneManager::GetWeatherTime();
				if (( fTime < 16.0f) && (fTime > 9.0f))
					CCameraControl::SetFadeOut(1.0f,1.0f,30.0f);
			}

			m_bShowSkillError = TRUE;
		}
	} 
	else
	{
		g_CharacterData.m_bSkillExecuted = FALSE;
		pSelfCreature->CancelSkill();
		pSelfCreature->SetStillCasting( false );
		pSelfCreature->m_ulSkillComboCount = 0;
		pSelfCreature->m_bSkillCombo = false;
	}
}

void CRYLRaceBase::ChangeKeyboardMode(void) 
{
	CRYLGameData*		pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	if ( pGame->m_bMouseMode == FALSE )
		return ;

	pGame->m_bSimpleMouse = FALSE ;
	pGame->m_bMouseMode   = FALSE ;

	g_CharacterData.MouseModeChange();
	pGame->m_bShowCursor = FALSE;
	CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[141], pNetwork->m_dwMyChrID, 128);

	CEffScript* test_script = new CEffScript;
	test_script->GetScriptBinData("inter_interface-type-keyboard.esf");
	test_script->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
	test_script->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;

	CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
}

BOOL CRYLRaceBase::GetIsExchange(void)
{
	CRYLGameScene*		pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLExchangeDlg*	pExchangeDlg	= ( CRYLExchangeDlg* )pScene->GetExchangeDlg() ;

	return pExchangeDlg->GetVisible() ;
}

