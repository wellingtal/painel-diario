
#include <winsock2.h>
//hz added this for mp bar
#include "RYLClientMain.h"
//hz end
#include "RYLGameScene.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetWorkData.h"
//hayzohar
#include <fstream>

#include "RYLJoystickConfigureDlg.h"
#include "RylGuildWarPenaltyDlg.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLSocietyFrameDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLStatusDlg.h"
#include "RYLDialogControlDlg.h"
#include "RYLQuickSlotDlg.h"
#include "RYLTradeDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLMIniMapDlg.h"
#include "RYLQuestOpeningDlg.h"
#include "RYLQuestDetailDataDlg.h"
#include "RYLBlacksmithDlg.h"
#include "RYLChattingDlg.h"
#include "RYLStatusRetrainDlg.h"
#include "RYLRankingDlg.h"
#include "RYLBattleGroundDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLHelpFrameDlg.h"
#include "RYLRankingDlg.h"
#include "RYLExchangeDlg.h"
#include "RYLJobChangeDlg.h"
#include "RYLSystemDlg.h"
#include "RYLOptionDlg.h"
#include "RYLVaultDlg.h"
#include "RYLVaultPasswordDlg.h"
#include "RYLMiniPartyDlg.h"
#include "RYLSummonDlg.h"
#include "RYLEnchantDlg.h"
#include "RYLGMReportDlg.h"
#include "RYLItemReparationDlg.h"
#include "RYLItemCompositionDlg.h"
#include "RYLTempInventoryDlg.h"
#include "RYLCastleStatusFrameDlg.h"
#include "RYLCastleMineralDlg.h"
#include "RYLCastleEmblemRightDlg.h"
#include "RYLCastleDoorblockDlg.h"
#include "RYLArmsCreatMenuDlg.h"
#include "RYLEmblemUpgradeDlg.h"
#include "RYLCampRightDlg.h"
#include "RYLSiegeArmsCreatingDlg.h"
#include "RYLCampUpgradeDlg.h"
#include "RYLCampRepairDlg.h"
#include "RYLCampCreatingDlg.h"
#include "RYLCampShowTaxInfoDlg.h"
#include "RYLMiningAmountDlg.h"
#include "RYLMiningCampDlg.h"
#include "RYLMiningAmountProcessDlg.h"
#include "RYLCampChangeDlg.h"
#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLWorldWeaponLoadDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLArmsHPDlg.h"

#include "RYLSiegeTimeDlg.h"

#include "RYLChangeNameDlg.h"

#include "RYLRespawnDlg.h"
#include "RYLBattleWaitDlg.h"
#include "RYLSkillDescDlg.h"
#include "RYLAbilityDescDlg.h"

#include "GMUITooltipManager.h"
#include "GMImageDib.h"
#include "FrameTimer.h"

#include "RYLEdit.h"
#include "RYLLoadingScene.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLSpriteList.h"
#include "RYLTimer.h"

#include "BattleGround Client/BattleGroundPrevResultDlg.h"
#include "BattleGround Client/BattleGroundStatusDlg.h"
#include "BattleGround Client/BattleGroundGameStatusDlg.h"
#include "BattleGround Client/BattleGroundChangeMoneyDlg.h"
#include "BattleGround Client/BattleGroundResultDlg.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"

CRYLGameScene::CRYLGameScene( INT iID ) : CRYLSceneObject( iID )
{
	/*//hayzohar start:
	string line;
	std::ifstream myfile("loginhelper.bin",ios::binary);
	if(myfile.is_open())
	{
		getline(myfile,line);
	}
	else 
	{
		MessageBox(NULL, "Something went wrong \n use File check if this error repeats.","GP-Client Error!", MB_OK);
		exit(10);
	}
	myfile.close();

	if(line == "www.RingOfWar.com")
	{
		std::ofstream WriteToFile("loginhelper.bin",ios::binary);
			if(WriteToFile.is_open())
			{
				WriteToFile<<"S";
			}
			else 
			{
				MessageBox(NULL, "Something went wrong \n use File check if this error repeats.","GP-Client Error!", MB_OK);
				exit(10);
			}
			WriteToFile.close();
	}
	else
	{
		MessageBox(NULL, "Something went wrong \n use File check if this error repeats.","GP-Client Error!", MB_OK);
		exit(12);
	}
	//hayzohar end*/
	m_RYLStatusAndQuestFrameDlg		= NULL ;		m_bRYLStatusAndQuestFrameDlg		= FALSE;	
	m_RYLSocietyFrameDlg			= NULL ;		m_bRYLSocietyFrameDlg				= FALSE;
	m_RYLGuildFrameDlg				= NULL ;		m_bRYLGuildFrameDlg					= FALSE;
	m_RYLStatusDlg					= NULL ;		m_bRYLStatusDlg						= FALSE;
	m_RYLStatusDlg2					= NULL ;		m_bRYLStatusDlg2					= FALSE;//hz guigui mp
	m_RYLStatusDlg3					= NULL ;		m_bRYLStatusDlg3					= FALSE;//hz guigui exp
	m_RYLDialogControlDlg			= NULL ;		m_bRYLDialogControlDlg				= FALSE;			
	m_RYLQuickSlotDlg				= NULL ;		m_bRYLQuickSlotDlg					= FALSE;			
	m_RYLHumanInventoryFrameDlg 	= NULL ;		m_bRYLHumanInventoryFrameDlg		= FALSE; 		
    m_RYLNewTradeDlg            	= NULL ;		m_bRYLNewTradeDlg					= FALSE;
	m_RYLStreetStallDlg				= NULL ;		m_bRYLStreetStallDlg				= FALSE;			
	m_RYLMiniMapDlg					= NULL ;		m_bRYLMiniMapDlg					= FALSE;				
	m_RYLQuestOpeningDlg			= NULL ;		m_bRYLQuestOpeningDlg				= FALSE;		
	m_RYLQuestDetailDataDlg			= NULL ;		m_bRYLQuestDetailDataDlg			= FALSE;		
	m_RYLBlacksmithDlg				= NULL ;		m_bRYLBlacksmithDlg					= FALSE;			
	m_RYLChattingDlg				= NULL ;		m_bRYLChattingDlg					= FALSE;
	m_RYLStatusRetrainDlg			= NULL ;		m_bRYLStatusRetrainDlg				= FALSE;			
	m_RYLRankingDlg					= NULL ;		m_bRYLRankingDlg					= FALSE;			
	m_RYLBattleGroundDlg			= NULL ;		m_bRYLBattleGroundDlg				= FALSE;		
	m_RYLLargeMapFrameDlg			= NULL ;		m_bRYLLargeMapFrameDlg				= FALSE;
	m_RYLHelpFrameDlg				= NULL ;		m_bRYLHelpFrameDlg					= FALSE;
	m_RYLExchangeDlg				= NULL ;		m_bRYLExchangeDlg					= FALSE;			
	m_RYLJobChangeDlg				= NULL ;		m_bRYLJobChangeDlg					= FALSE;			
	m_RYLSystemDlg					= NULL ;		m_bRYLSystemDlg						= FALSE;
	m_RYLOptionDlg					= NULL ;		m_bRYLOptionDlg						= FALSE;				
	m_RYLVaultDlg					= NULL ;		m_bRYLVaultDlg						= FALSE;				
	m_RYLVaultPasswordDlg			= NULL ;		m_bRYLVaultPasswordDlg				= FALSE;		
	m_RYLVaultCertificationdDlg		= NULL ;		m_bRYLVaultCertificationdDlg		= FALSE; 
	m_RYLMiniPartyDlg				= NULL ;		m_bRYLMiniPartyDlg					= FALSE;			
	m_RYLSummonDlg					= NULL ;		m_bRYLSummonDlg						= FALSE;				
	m_RYLEnchantDlg					= NULL ;		m_bRYLEnchantDlg					= FALSE;
    m_RYLBattleGroundPrevResultDlg  = NULL;			m_bRYLBattleGroundPrevResultDlg		= FALSE;
    m_RYLBattleGroundStatusDlg      = NULL;			m_bRYLBattleGroundStatusDlg			= FALSE;
    m_RYLBattleGroundGameStatusDlg  = NULL;			m_bRYLBattleGroundGameStatusDlg		= FALSE;
    m_RYLBattleGroundChangeMoneyDlg = NULL;			m_bRYLBattleGroundChangeMoneyDlg	= FALSE;
    m_RYLBattleGroundResultDlg      = NULL;			m_bRYLBattleGroundResultDlg			= FALSE;
    m_RYLGMReportDlg	            = NULL;			m_bRYLGMReportDlg					= FALSE;
	m_RYLChangeNameDlg				= NULL;			m_bRYLChangeNameDlg					= FALSE;

	m_RYLJoystickConfigureDlg		= NULL;			m_bRYLJoystickConfigureDlg			= FALSE;

	m_RYLSiegeTimeDlg				= NULL;			m_bRYLSiegeTimeDlg					= FALSE;
	m_RYLGuildWarPenaltyDlg			= NULL;			m_bRYLGuildWarPenaltyDlg			= FALSE;

	// 리밸런싱, 아이템 관련 다이얼로그				
	m_RYLItemReparationDlg			= NULL;			m_bRYLItemReparationDlg				= FALSE;
	m_RYLItemCompositionDlg			= NULL;			m_bRYLItemCompositionDlg			= FALSE;
	m_RYLTempInventoryDlg			= NULL;			m_bRYLTempInventoryDlg				= FALSE;
													
	//-------------------------------------------	
	// 공성관련										

	m_RYLCastleStatusFrameDlg		= NULL ;		m_bRYLCastleStatusFrameDlg			= FALSE;
	m_RYLCastleMineralDlg			= NULL ;		m_bRYLCastleMineralDlg				= FALSE;
	m_RYLCastleEmblemRightDlg		= NULL ;		m_bRYLCastleEmblemRightDlg			= FALSE;
	m_RYLCastleDoorblockDlg			= NULL ;		m_bRYLCastleDoorblockDlg			= FALSE;
	m_RYLArmsCreatMenuDlg			= NULL ;		m_bRYLArmsCreatMenuDlg				= FALSE;
	m_RYLEmblemUpgradeDlg			= NULL ;		m_bRYLEmblemUpgradeDlg				= FALSE;
	m_RYLCampRightDlg				= NULL ;		m_bRYLCampRightDlg					= FALSE;
	m_RYLSiegeArmsCreatingDlg		= NULL ;		m_bRYLSiegeArmsCreatingDlg			= FALSE;
	m_RYLCampUpgradeDlg				= NULL ;		m_bRYLCampUpgradeDlg				= FALSE;
	m_RYLCampRepairDlg				= NULL ;		m_bRYLCampRepairDlg					= FALSE;
	m_RYLCampCreatingDlg			= NULL ;		m_bRYLCampCreatingDlg				= FALSE;
	m_RYLCampShowTaxInfoDlg			= NULL ;		m_bRYLCampShowTaxInfoDlg			= FALSE;
	m_RYLMiningAmountDlg			= NULL ;		m_bRYLMiningAmountDlg				= FALSE;
	m_RYLMiningCampDlg				= NULL ;		m_bRYLMiningCampDlg					= FALSE;
	m_RYLMiningAmountProcessDlg		= NULL ;		m_bRYLMiningAmountProcessDlg		= FALSE;
	m_RYLCampChangeDlg				= NULL ;		m_bRYLCampChangeDlg					= FALSE;
	m_RYLWorldWeaponLaunchDlg		= NULL ;		m_bRYLWorldWeaponLaunchDlg			= FALSE;
	m_RYLWorldWeaponLoadDlg			= NULL ;		m_bRYLWorldWeaponLoadDlg			= FALSE;
	m_RYLCampShopDlg				= NULL ;		m_bRYLCampShopDlg					= FALSE;
	m_RYLArmsHPDlg					= NULL ;		m_bRYLArmsHPDlg						= FALSE;
														
	m_RYLRespawnDlg					= NULL ;		m_bRYLRespawnDlg					= FALSE;
	m_RYLBattleWaitDlg				= NULL ;		m_bRYLBattleWaitDlg					= FALSE;
	m_RYLGMReportDlg				= NULL ;		m_bRYLGMReportDlg					= FALSE;
	m_RYLSkillTooltipDlg			= NULL ;		m_bRYLSkillTooltipDlg				= FALSE;
	m_RYLAbilityTooltipDlg			= NULL ; 		m_bRYLAbilityTooltipDlg				= FALSE;
													
    m_pUpMovieBarImage              = NULL ;		
    m_pDownMovieBarImage            = NULL ;		

	m_f2ndBarSY						= 0 ;
}

CRYLGameScene::~CRYLGameScene() 
{
	//hayzohar
	//std::ofstream WriteToFile("loginhelper.bin",ios::binary);
	//if(WriteToFile.is_open())
	//{
	//	WriteToFile<<"www.RingOfWar.com";
	//}
	//hayzohar		
	DeleteResourceObject() ;
	GM_RELEASE( m_pUpMovieBarImage ) ;
	GM_RELEASE( m_pDownMovieBarImage ) ;
}                                
	
HRESULT			CRYLGameScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT			CRYLGameScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLGameData::Instance()->m_lpInterface->RenderGame( lpD3DDevice ) ;	
		
	return S_OK ;	
}

HRESULT			CRYLGameScene::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	if ( m_RYLMiniMapDlg->GetVisible() )
		m_RYLMiniMapDlg->FinalRender( lpD3DDevice ) ;

	if ( m_RYLQuickSlotDlg->GetVisible() )
		m_RYLQuickSlotDlg->FinalRender( lpD3DDevice ) ;
	
	if ( m_RYLGuildFrameDlg->GetVisible() ) 
		m_RYLGuildFrameDlg->FinalRender( lpD3DDevice ) ;

	if ( m_RYLHumanInventoryFrameDlg->GetVisible() )
		m_RYLHumanInventoryFrameDlg->FinalRender( lpD3DDevice ) ;

    if ( m_RYLNewTradeDlg->GetVisible() )
        m_RYLNewTradeDlg->FinalRender( lpD3DDevice ) ;

	if ( m_RYLStreetStallDlg->GetVisible() )
		m_RYLStreetStallDlg->FinalRender( lpD3DDevice ) ;

	if ( m_RYLBlacksmithDlg->GetVisible() )
		m_RYLBlacksmithDlg->FinalRender( lpD3DDevice ) ;
        
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_lpInterface->FinalRenderGame( lpD3DDevice ) ;

	CRYLLoadingScene::Instance()->Render( lpD3DDevice ) ;

	static int sInc = 0 ;
	if ( pGame->m_bMovieVersion )
	{	
		float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetMovieBarTimer()) ;
		if ( fUpdate >= 1.0f )
		{
			if ( m_pUpMovieBarImage->GetHeight() <= sInc )
				sInc = m_pUpMovieBarImage->GetHeight() ;
			else
				sInc += 4 ;
		}

		RECT rt ;
		SetRect( &rt, 0, 0, 
					  m_pUpMovieBarImage->GetWidth(), sInc ) ;
		m_pUpMovieBarImage->DrawRect( rt, true ) ;

		SetRect( &rt, 0, ( m_f2ndBarSY + m_pUpMovieBarImage->GetHeight() ) - sInc, 
							m_pDownMovieBarImage->GetWidth(), ( m_f2ndBarSY + m_pUpMovieBarImage->GetHeight() ) ) ;
		m_pDownMovieBarImage->DrawRect( rt ) ;			
	}
	else
	{
		if ( sInc <= 0 )
		{
			sInc = 0 ;
		}
		else
		{
			float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetMovieBarTimer()) ;
			if ( fUpdate >= 1.0f )
			{
				sInc -= 4 ;
			}

			RECT rt ;
			SetRect( &rt, 0, 0, 
				m_pUpMovieBarImage->GetWidth(), sInc ) ;
			m_pUpMovieBarImage->DrawRect( rt, true ) ;

			SetRect( &rt, 0, ( m_f2ndBarSY + m_pUpMovieBarImage->GetHeight() ) - sInc, 
				m_pDownMovieBarImage->GetWidth(), ( m_f2ndBarSY + m_pUpMovieBarImage->GetHeight() ) ) ;
			m_pDownMovieBarImage->DrawRect( rt ) ;
		}
	}

	return S_OK ;
}

HRESULT			CRYLGameScene::Update() 
{
	if ( g_CharacterData.m_bSummon )
	{
		m_RYLSummonDlg->SetVisible( TRUE ) ;
	}
	else
	{
		m_RYLSummonDlg->SetVisible( FALSE ) ;
	}

	BOOL bClick = FALSE;
	BOOL bEdge = FALSE;

	KeyControl::Process();	

	CGMUIScreen::GetInstance()->Update( bClick, bEdge ) ;	

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	if(pGame)
	{
		static DWORD TickCount = 0;
		DWORD dwTickCount = ::GetTickCount();

		if(dwTickCount - TickCount > 1000)
		{
			TickCount = dwTickCount;
			pGame->UpdateEnchantTime();
		}
	}


	( ( CRYLStreetStallDlg * )m_RYLStreetStallDlg )->UpdateMessageBox() ;
	CRYLGameData::Instance()->m_lpInterface->UpdateGame( bClick, bEdge );	


	/*
	//------------------------------------------------------------------------------------------
	// 공성관련
	if ( m_RYLCastleEmblemRightDlg->GetVisible() )			m_RYLCastleEmblemRightDlg->Update( bClick, bEdge ) ;
	if ( m_RYLTaxrateDlg->GetVisible() )					m_RYLTaxrateDlg->Update( bClick, bEdge ) ;
	if ( m_RYLCastleDoorblockDlg->GetVisible() )			m_RYLCastleDoorblockDlg->Update( bClick, bEdge ) ;
	if ( m_RYLTaxpresentDlg->GetVisible() )					m_RYLTaxpresentDlg->Update( bClick, bEdge ) ;
	if ( m_RYLArmsCreatMenuDlg->GetVisible() )				m_RYLArmsCreatMenuDlg->Update( bClick, bEdge ) ;
	if ( m_RYLEmblemUpgradeDlg->GetVisible() )				m_RYLEmblemUpgradeDlg->Update( bClick, bEdge ) ;
	if ( m_RYLCampRightDlg->GetVisible() )					m_RYLCampRightDlg->Update( bClick, bEdge ) ;
	*/

	return S_OK ;
}

VOID			CRYLGameScene::InitResourceByRace(void) 
{
	GM_DELETE( m_RYLHumanInventoryFrameDlg ) ;
	m_RYLHumanInventoryFrameDlg 	= new CRYLHumanInventoryFrameDlg ;

	GM_DELETE( m_RYLRankingDlg ) ;
	m_RYLRankingDlg 				= new CRYLRankingDlg ;
}

HRESULT			CRYLGameScene::InitResourceObject(void) 
{
	LoadMainWindowPosFromReg() ;

	// 인벤토리와 랭킹창은 생성되는 컨트롤들이 종족의 영향을 받으므로 캐릭터 선택 후에 생성한다.
//	m_RYLHumanInventoryFrameDlg 	= new CRYLHumanInventoryFrameDlg ;
	//m_RYLRankingDlg					= new CRYLRankingDlg ;//was commented out hz

	m_RYLStatusAndQuestFrameDlg		= new CRYLStatusAndQuestFrameDlg ;
	m_RYLSocietyFrameDlg			= new CRYLSocietyFrameDlg ;
	m_RYLGuildFrameDlg				= new CRYLGuildFrameDlg ;

	m_RYLStatusDlg					= new CRYLStatusDlg( m_MainWindowInfo.m_nStatusLeft, m_MainWindowInfo.m_nStatusTop, m_MainWindowInfo.m_nStatusExtensionLevel ) ;

	//m_RYLStatusDlg					= new CRYLStatusDlg( 0, m_MainWindowInfo.m_nStatusTop, m_MainWindowInfo.m_nStatusExtensionLevel,2 ) ;
	
//	m_RYLStatusDlg2					= new CRYLStatusDlg( (g_ClientMain.m_iScreenWidth - 119), m_MainWindowInfo.m_nStatusTop, m_MainWindowInfo.m_nStatusExtensionLevel,3 ) ;//hz guigui
//	m_RYLStatusDlg3					= new CRYLStatusDlg( m_MainWindowInfo.m_nStatusLeft, m_MainWindowInfo.m_nStatusTop, m_MainWindowInfo.m_nStatusExtensionLevel,1 ) ;//hz guigui exp
	m_RYLDialogControlDlg			= new CRYLDialogControlDlg( m_MainWindowInfo.m_nDlgControlLeft, m_MainWindowInfo.m_nDlgControlTop, m_MainWindowInfo.m_bDlgControlVertical ) ;
	m_RYLQuickSlotDlg				= new CRYLQuickSlotDlg( m_MainWindowInfo.m_nQuickSlotLeft, m_MainWindowInfo.m_nQuickSlotTop, m_MainWindowInfo.m_bQuickSlotVertical ) ;
    m_RYLNewTradeDlg                = new CRYLNewTradeDlg ;
	m_RYLStreetStallDlg				= new CRYLStreetStallDlg ;
	m_RYLQuestOpeningDlg			= new CRYLQuestOpeningDlg ;
	m_RYLQuestDetailDataDlg			= new CRYLQuestDetailDataDlg ;
	m_RYLBlacksmithDlg				= new CRYLBlacksmithDlg ;
	m_RYLStatusRetrainDlg			= new CRYLStatusRetrainDlg ;
	m_RYLBattleGroundDlg			= new CRYLBattleGroundDlg ;
	m_RYLExchangeDlg				= new CRYLExchangeDlg ;
	m_RYLJobChangeDlg				= new CRYLJobChangeDlg ;
	m_RYLOptionDlg					= new CRYLOptionDlg ;
	m_RYLVaultDlg					= new CRYLVaultDlg ;
	m_RYLVaultPasswordDlg			= new CRYLVaultPasswordDlg ;
	m_RYLVaultCertificationdDlg 	= new CRYLVaultCertificationdDlg ;
	m_RYLMiniPartyDlg				= new CRYLMiniPartyDlg ;
	m_RYLSummonDlg					= new CRYLSummonDlg ;
	m_RYLEnchantDlg					= new CRYLEnchantDlg( m_MainWindowInfo.m_nEnchantLeft, m_MainWindowInfo.m_nEnchantTop ) ;
    m_RYLBattleGroundPrevResultDlg  = new CRYLBattleGroundPrevResultDlg;
    m_RYLBattleGroundStatusDlg      = new CRYLBattleGroundStatusDlg;
    m_RYLBattleGroundGameStatusDlg  = new CRYLBattleGroundGameStatusDlg;
    m_RYLBattleGroundChangeMoneyDlg = new CRYLBattleGroundChangeMoneyDlg;
    m_RYLBattleGroundResultDlg      = new CRYLBattleGroundResultDlg;
	m_RYLGMReportDlg	            = new CRYLGMReportDlg;
	m_RYLItemReparationDlg			= new CRYLItemReparationDlg;
	m_RYLItemCompositionDlg			= new CRYLItemCompositionDlg;
	m_RYLTempInventoryDlg			= new CRYLTempInventoryDlg;
	m_RYLChangeNameDlg				= new CRYLChangeNameDlg;

	m_RYLSiegeTimeDlg				= new CRYLSiegeTimeDlg(m_MainWindowInfo.m_nSiegeLeft, m_MainWindowInfo.m_nSiegeTop);	

	m_RYLJoystickConfigureDlg		= new CRYLJoystickConfigureDlg;

	m_RYLGuildWarPenaltyDlg			= new CRYLGuildWarPenaltyDlg;	

	if ( !m_RYLSystemDlg )
		m_RYLSystemDlg				= new CRYLSystemDlg ;

	if ( !m_RYLChattingDlg )
		m_RYLChattingDlg			= new CRYLChattingDlg( m_MainWindowInfo.m_nChatLeft, m_MainWindowInfo.m_nChatTop, m_MainWindowInfo.m_nChatDlgType ) ;
	
	( ( CRYLChattingDlg* )m_RYLChattingDlg )->m_pChatEdit->SetFocusState( FALSE ) ;

	//----------------------------------------------------------------
	// 공성관련

	m_RYLCastleStatusFrameDlg		= new CRYLCastleStatusFrameDlg ;
	m_RYLCastleMineralDlg			= new CRYLCastleMineralDlg ;
	m_RYLCastleEmblemRightDlg		= new CRYLCastleEmblemRightDlg ;
	m_RYLCastleDoorblockDlg			= new CRYLCastleDoorblockDlg ;
	m_RYLArmsCreatMenuDlg			= new CRYLArmsCreatMenuDlg ;
	m_RYLEmblemUpgradeDlg			= new CRYLEmblemUpgradeDlg ;
	m_RYLCampRightDlg				= new CRYLCampRightDlg ;
	m_RYLSiegeArmsCreatingDlg		= new CRYLSiegeArmsCreatingDlg ;
	m_RYLCampUpgradeDlg				= new CRYLCampUpgradeDlg ;
	m_RYLCampRepairDlg				= new CRYLCampRepairDlg ;
	m_RYLCampCreatingDlg			= new CRYLCampCreatingDlg ;
	m_RYLCampShowTaxInfoDlg			= new CRYLCampShowTaxInfoDlg ;
	
	m_RYLMiningAmountDlg			= new CRYLMiningAmountDlg ;
 	m_RYLMiningCampDlg				= new CRYLMiningCampDlg ;
	m_RYLMiningAmountProcessDlg		= new CRYLMiningAmountProcessDlg ;
	m_RYLCampChangeDlg				= new CRYLCampChangeDlg ;

	m_RYLWorldWeaponLaunchDlg		= new CRYLWorldWeaponLaunchDlg;
	m_RYLWorldWeaponLoadDlg			= new CRYLWorldWeaponLoadDlg;

	m_RYLCampShopDlg				= new CRYLCampShopDlg;
	m_RYLArmsHPDlg					= new CRYLArmsHPDlg ;

	m_RYLRespawnDlg					= new CRYLRespawnDlg ;
	m_RYLBattleWaitDlg				= new CRYLBattleWaitDlg ;
		
	m_RYLMiniMapDlg					= new CRYLMIniMapDlg(m_MainWindowInfo.m_nMapLeft, m_MainWindowInfo.m_nMapTop, m_MainWindowInfo.m_nMapType) ;
	m_RYLLargeMapFrameDlg			= new CRYLLargeMapFrameDlg ;
	m_RYLHelpFrameDlg				= new CRYLHelpFrameDlg ;

	m_RYLSkillTooltipDlg			= new CRYLSkillDescDlg ;
	m_RYLAbilityTooltipDlg			= new CRYLAbilityDescDlg ;

	return S_OK ;
}

HRESULT			CRYLGameScene::DeleteResourceObject() 
{
	// 레지스트리에 값을 저장하는 루틴은 종료 카운트를 셋팅하는 부분으로 옮겼음!!
	//SaveMainWindowPosToReg() ;

	GM_DELETE( m_RYLStatusAndQuestFrameDlg ) ;
	GM_DELETE( m_RYLSocietyFrameDlg ) ;
	GM_DELETE( m_RYLGuildFrameDlg ) ;
	GM_DELETE( m_RYLHumanInventoryFrameDlg ) ;
	GM_DELETE( m_RYLStatusDlg ) ;
	GM_DELETE( m_RYLStatusDlg2 ) ;//hz guigui
	GM_DELETE( m_RYLStatusDlg3 ) ;//hz guigui exp
	GM_DELETE( m_RYLDialogControlDlg ) ;
	GM_DELETE( m_RYLQuickSlotDlg ) ;
    GM_DELETE( m_RYLNewTradeDlg ) ;
	GM_DELETE( m_RYLStreetStallDlg ) ;
	GM_DELETE( m_RYLMiniMapDlg ) ;
	GM_DELETE( m_RYLQuestOpeningDlg ) ;
	GM_DELETE( m_RYLQuestDetailDataDlg ) ;
	GM_DELETE( m_RYLBlacksmithDlg ) ;
    GM_DELETE( m_RYLChattingDlg ) ;
	GM_DELETE( m_RYLStatusRetrainDlg ) ;
	GM_DELETE( m_RYLRankingDlg ) ;
	GM_DELETE( m_RYLBattleGroundDlg ) ;
	GM_DELETE( m_RYLLargeMapFrameDlg ) ;
	GM_DELETE( m_RYLHelpFrameDlg ) ;
	GM_DELETE( m_RYLExchangeDlg ) ;
	GM_DELETE( m_RYLJobChangeDlg ) ;
	GM_DELETE( m_RYLSystemDlg ) ;
	GM_DELETE( m_RYLOptionDlg ) ;
	GM_DELETE( m_RYLVaultDlg ) ;
	GM_DELETE( m_RYLVaultPasswordDlg ) ;
	GM_DELETE( m_RYLVaultCertificationdDlg ) ;
	GM_DELETE( m_RYLMiniPartyDlg ) ;
	GM_DELETE( m_RYLSummonDlg ) ;
	GM_DELETE( m_RYLEnchantDlg ) ;
    GM_DELETE( m_RYLBattleGroundPrevResultDlg );
    GM_DELETE( m_RYLBattleGroundStatusDlg );
    GM_DELETE( m_RYLBattleGroundGameStatusDlg );
    GM_DELETE( m_RYLBattleGroundChangeMoneyDlg );
    GM_DELETE( m_RYLBattleGroundResultDlg );
    GM_DELETE( m_RYLGMReportDlg );
	GM_DELETE( m_RYLChangeNameDlg );

	GM_DELETE( m_RYLSiegeTimeDlg );	

	GM_DELETE( m_RYLJoystickConfigureDlg );		

	GM_DELETE( m_RYLGuildWarPenaltyDlg );	

	// 리밸런싱, 아이템 관련 다이얼로그
	GM_DELETE( m_RYLItemReparationDlg );
	GM_DELETE( m_RYLItemCompositionDlg );
	GM_DELETE( m_RYLTempInventoryDlg );

	//----------------------------------------------
	// 공성관련

	GM_DELETE( m_RYLCastleStatusFrameDlg ) ;
	GM_DELETE( m_RYLCastleMineralDlg ) ;
	GM_DELETE( m_RYLCastleEmblemRightDlg ) ;
	GM_DELETE( m_RYLCastleDoorblockDlg ) ;
	GM_DELETE( m_RYLArmsCreatMenuDlg ) ;
	GM_DELETE( m_RYLEmblemUpgradeDlg ) ;
	GM_DELETE( m_RYLCampRightDlg ) ;
	GM_DELETE( m_RYLSiegeArmsCreatingDlg ) ;
	GM_DELETE( m_RYLCampUpgradeDlg ) ;
	GM_DELETE( m_RYLCampRepairDlg ) ;
	GM_DELETE( m_RYLCampCreatingDlg ) ;
	GM_DELETE( m_RYLCampShowTaxInfoDlg ) ;

	GM_DELETE( m_RYLMiningAmountDlg );
	GM_DELETE( m_RYLMiningCampDlg );
	GM_DELETE( m_RYLMiningAmountProcessDlg );
	GM_DELETE( m_RYLCampChangeDlg );

	GM_DELETE( m_RYLWorldWeaponLaunchDlg );
	GM_DELETE( m_RYLWorldWeaponLoadDlg );

	GM_DELETE( m_RYLCampShopDlg );
	GM_DELETE( m_RYLArmsHPDlg );

	GM_DELETE( m_RYLRespawnDlg ) ;
	GM_DELETE( m_RYLBattleWaitDlg ) ;

	GM_DELETE( m_RYLSkillTooltipDlg ) ;
	GM_DELETE( m_RYLAbilityTooltipDlg ) ;
	
	return S_OK ;
}

VOID			CRYLGameScene::GameSceneDelete() 
{
	//==============================================
	// CharacterSelect Scene을 이동하기위해 현제 이벤트를 받고 있는 RYL System Dialog를
	// 제외한 나머지를 해제해주고 이벤트가 끝난후에 m_RYLSystemDlg를 해제해 준다.
	//GM_DELETE( m_RYLSystemDlg ) ;
	//GM_DELETE( m_RYLChattingDlg ) ;


	GM_DELETE( m_RYLJoystickConfigureDlg ) ;

	GM_DELETE( m_RYLSiegeTimeDlg ) ;

	GM_DELETE( m_RYLGuildWarPenaltyDlg ) ;

	GM_DELETE( m_RYLStatusAndQuestFrameDlg ) ;
	GM_DELETE( m_RYLSocietyFrameDlg ) ;
	GM_DELETE( m_RYLGuildFrameDlg ) ;
	GM_DELETE( m_RYLHumanInventoryFrameDlg ) ;
	GM_DELETE( m_RYLStatusDlg ) ;
	GM_DELETE( m_RYLStatusDlg2 ) ;//hz guigui
	GM_DELETE( m_RYLStatusDlg3 ) ;//hz guigui exp
	GM_DELETE( m_RYLDialogControlDlg ) ;
	GM_DELETE( m_RYLQuickSlotDlg ) ;
    GM_DELETE( m_RYLNewTradeDlg ) ;
	GM_DELETE( m_RYLStreetStallDlg ) ;
	GM_DELETE( m_RYLMiniMapDlg ) ;
	GM_DELETE( m_RYLQuestOpeningDlg ) ;
	GM_DELETE( m_RYLQuestDetailDataDlg ) ;
	GM_DELETE( m_RYLBlacksmithDlg ) ;
    GM_DELETE( m_RYLStatusRetrainDlg ) ;
	GM_DELETE( m_RYLRankingDlg ) ;
	GM_DELETE( m_RYLBattleGroundDlg ) ;
	GM_DELETE( m_RYLLargeMapFrameDlg ) ;
	GM_DELETE( m_RYLHelpFrameDlg ) ;
	GM_DELETE( m_RYLExchangeDlg ) ;
	GM_DELETE( m_RYLJobChangeDlg ) ;
	GM_DELETE( m_RYLOptionDlg ) ;
	GM_DELETE( m_RYLVaultDlg ) ;
	GM_DELETE( m_RYLVaultPasswordDlg ) ;
	GM_DELETE( m_RYLVaultCertificationdDlg ) ;
	GM_DELETE( m_RYLMiniPartyDlg ) ;
	GM_DELETE( m_RYLSummonDlg ) ;
	GM_DELETE( m_RYLEnchantDlg ) ;
    GM_DELETE( m_RYLBattleGroundPrevResultDlg );
    GM_DELETE( m_RYLBattleGroundStatusDlg );
    GM_DELETE( m_RYLBattleGroundGameStatusDlg );
    GM_DELETE( m_RYLBattleGroundChangeMoneyDlg );
    GM_DELETE( m_RYLBattleGroundResultDlg );
    GM_DELETE( m_RYLGMReportDlg );

	//---------------------------------------------
	// 공성 관련

	GM_DELETE( m_RYLCastleStatusFrameDlg ) ;
	GM_DELETE( m_RYLCastleMineralDlg ) ;
	GM_DELETE( m_RYLCastleEmblemRightDlg ) ;
	GM_DELETE( m_RYLCastleDoorblockDlg ) ;
	GM_DELETE( m_RYLArmsCreatMenuDlg ) ;
	GM_DELETE( m_RYLEmblemUpgradeDlg ) ;
	GM_DELETE( m_RYLCampRightDlg ) ;
	GM_DELETE( m_RYLSiegeArmsCreatingDlg ) ;
	GM_DELETE( m_RYLCampUpgradeDlg ) ;
	GM_DELETE( m_RYLCampRepairDlg ) ;
	GM_DELETE( m_RYLCampCreatingDlg ) ;
	GM_DELETE( m_RYLCampShowTaxInfoDlg ) ;

	GM_DELETE( m_RYLMiningAmountDlg );
	GM_DELETE( m_RYLMiningCampDlg );
	GM_DELETE( m_RYLMiningAmountProcessDlg );
	GM_DELETE( m_RYLCampChangeDlg );
	GM_DELETE( m_RYLWorldWeaponLaunchDlg );
	GM_DELETE( m_RYLWorldWeaponLoadDlg );
	GM_DELETE( m_RYLCampShopDlg );
	GM_DELETE( m_RYLArmsHPDlg );

	GM_DELETE( m_RYLRespawnDlg ) ;
	GM_DELETE( m_RYLBattleWaitDlg ) ;

	GM_DELETE( m_RYLSkillTooltipDlg ) ;
	GM_DELETE( m_RYLAbilityTooltipDlg );
}

VOID			CRYLGameScene::DeleteSystemDlg() 
{
	GM_DELETE( m_RYLSystemDlg ) ;
}

HRESULT			CRYLGameScene::FinalCleanup() 
{
	return S_OK ;
}

HRESULT			CRYLGameScene::MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) 
{
	return S_OK ;
}

VOID			CRYLGameScene::SceneShow() 
{
	if ( m_RYLStatusDlg )				m_RYLStatusDlg->SetVisible( TRUE ) ;
	if ( m_RYLStatusDlg2 )				m_RYLStatusDlg2->SetVisible( TRUE ) ;//hz guigui
	if ( m_RYLStatusDlg3 )				m_RYLStatusDlg3->SetVisible( TRUE ) ;//hz guigui
	if ( m_RYLDialogControlDlg )		m_RYLDialogControlDlg->SetVisible( TRUE ) ;
	if ( m_RYLQuickSlotDlg)				m_RYLQuickSlotDlg->SetVisible( TRUE ) ;
	if ( m_RYLMiniMapDlg )				m_RYLMiniMapDlg->SetVisible( TRUE ) ;
	if ( m_RYLChattingDlg )				m_RYLChattingDlg->SetVisible( TRUE ) ;
	if ( m_RYLEnchantDlg )				m_RYLEnchantDlg->SetVisible( TRUE ) ;
	if ( m_RYLSiegeTimeDlg )			m_RYLSiegeTimeDlg->SetVisible( TRUE ) ;

	if ( m_RYLSkillTooltipDlg )			m_RYLSkillTooltipDlg->SetVisible( TRUE ) ;
	if ( m_RYLAbilityTooltipDlg )		m_RYLAbilityTooltipDlg->SetVisible( TRUE ) ;

	//if ( m_RYLStatusDlg && !(m_MainWindowInfo.m_dwVisibleFlag & RYLMAINWINDOW::STATUS_WINDOW) )			m_RYLStatusDlg->SetVisible( FALSE ) ;
	if ( m_RYLQuickSlotDlg && !(m_MainWindowInfo.m_dwVisibleFlag & RYLMAINWINDOW::QUICKSLOT_WINDOW) )	m_RYLQuickSlotDlg->SetVisible( FALSE ) ;
	if ( m_RYLMiniMapDlg && !(m_MainWindowInfo.m_dwVisibleFlag & RYLMAINWINDOW::MINIMAP_WINDOW) )		m_RYLMiniMapDlg->SetVisible( FALSE ) ;
	if ( m_RYLEnchantDlg && !(m_MainWindowInfo.m_dwVisibleFlag & RYLMAINWINDOW::ENCHANT_WINDOW) )		m_RYLEnchantDlg->SetVisible( FALSE ) ;
	if ( m_RYLSiegeTimeDlg && !(m_MainWindowInfo.m_dwVisibleFlag & RYLMAINWINDOW::SIEGETIME_WINDOW) )	m_RYLSiegeTimeDlg->SetVisible( FALSE ) ;
}

VOID			CRYLGameScene::SceneHide() 
{
	MovieMode( FALSE ) ;
}

VOID			CRYLGameScene::MovieMode( BOOL bMovie ) 
{
	if ( bMovie )
	{
		if ( m_bRYLJoystickConfigureDlg )				m_RYLJoystickConfigureDlg->SetVisible( TRUE ) ;	
		if ( m_bRYLGuildWarPenaltyDlg )					m_RYLGuildWarPenaltyDlg->SetVisible( TRUE ) ;	
		if ( m_bRYLSiegeTimeDlg )						m_RYLSiegeTimeDlg->SetVisible( TRUE ) ;	
		if ( m_bRYLStatusAndQuestFrameDlg )				m_RYLStatusAndQuestFrameDlg->SetVisible( TRUE ) ;	
		if ( m_bRYLSocietyFrameDlg )					m_RYLSocietyFrameDlg->SetVisible( TRUE ) ;
		if ( m_bRYLGuildFrameDlg )						m_RYLGuildFrameDlg->SetVisible( TRUE ) ;
		if ( m_bRYLHumanInventoryFrameDlg )				m_RYLHumanInventoryFrameDlg->SetVisible( TRUE ) ; 
		if ( m_bRYLStatusDlg )							m_RYLStatusDlg->SetVisible( TRUE ) ;
		if ( m_bRYLStatusDlg2 )							m_RYLStatusDlg2->SetVisible( TRUE ) ;//hz guigui
		if ( m_bRYLStatusDlg3 )							m_RYLStatusDlg3->SetVisible( TRUE ) ;//hz guigui
		if ( m_bRYLDialogControlDlg )					m_RYLDialogControlDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLQuickSlotDlg )						m_RYLQuickSlotDlg->SetVisible( TRUE ) ;			

		if ( m_bRYLNewTradeDlg )						m_RYLNewTradeDlg->SetVisible( TRUE ) ;				
		if ( m_bRYLStreetStallDlg )						m_RYLStreetStallDlg->SetVisible( TRUE ) ;			
		if ( m_bRYLMiniMapDlg )							m_RYLMiniMapDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLQuestOpeningDlg )					m_RYLQuestOpeningDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLQuestDetailDataDlg )					m_RYLQuestDetailDataDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLBlacksmithDlg )						m_RYLBlacksmithDlg->SetVisible( TRUE ) ;			
		if ( m_bRYLStatusRetrainDlg )					m_RYLStatusRetrainDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLRankingDlg )							m_RYLRankingDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLBattleGroundDlg )					m_RYLBattleGroundDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLLargeMapFrameDlg )					m_RYLLargeMapFrameDlg->SetVisible( TRUE );

		if ( m_bRYLHelpFrameDlg )						m_RYLHelpFrameDlg->SetVisible( TRUE );
		if ( m_bRYLExchangeDlg )						m_RYLExchangeDlg->SetVisible( TRUE ) ;			
		if ( m_bRYLJobChangeDlg )						m_RYLJobChangeDlg->SetVisible( TRUE ) ;			
		if ( m_bRYLOptionDlg )							m_RYLOptionDlg->SetVisible( TRUE ) ;				
		if ( m_bRYLVaultDlg )							m_RYLVaultDlg->SetVisible( TRUE ) ;				
		if ( m_bRYLVaultPasswordDlg )					m_RYLVaultPasswordDlg->SetVisible( TRUE ) ;		
		if ( m_bRYLVaultCertificationdDlg )				m_RYLVaultCertificationdDlg->SetVisible( TRUE ) ; 
		if ( m_bRYLMiniPartyDlg )						m_RYLMiniPartyDlg->SetVisible( TRUE ) ;			
		if ( m_bRYLSummonDlg )							m_RYLSummonDlg->SetVisible( TRUE ) ;				
		if ( m_bRYLSystemDlg )							m_RYLSystemDlg->SetVisible( TRUE ) ;

		if ( m_bRYLChattingDlg )						m_RYLChattingDlg->SetVisible( TRUE ) ;
		if ( m_bRYLEnchantDlg )							m_RYLEnchantDlg->SetVisible( TRUE ) ;
		if ( m_bRYLBattleGroundPrevResultDlg )			m_RYLBattleGroundPrevResultDlg->SetVisible( TRUE );
		if ( m_bRYLBattleGroundStatusDlg )				m_RYLBattleGroundStatusDlg->SetVisible( TRUE );
		if ( m_bRYLBattleGroundGameStatusDlg )			m_RYLBattleGroundGameStatusDlg->SetVisible( TRUE );
		if ( m_bRYLBattleGroundChangeMoneyDlg )			m_RYLBattleGroundChangeMoneyDlg->SetVisible( TRUE );
		if ( m_bRYLBattleGroundResultDlg )				m_RYLBattleGroundResultDlg->SetVisible( TRUE );
        if ( m_bRYLGMReportDlg )						m_RYLGMReportDlg->SetVisible( TRUE );
		if ( m_bRYLItemReparationDlg )					m_RYLItemReparationDlg->SetVisible( TRUE );
		if ( m_bRYLItemCompositionDlg )					m_RYLItemCompositionDlg->SetVisible( TRUE );

		if ( m_bRYLTempInventoryDlg )					m_RYLTempInventoryDlg->SetVisible( TRUE );
		if ( m_bRYLCastleStatusFrameDlg )				m_RYLCastleStatusFrameDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCastleMineralDlg )					m_RYLCastleMineralDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCastleEmblemRightDlg )				m_RYLCastleEmblemRightDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCastleDoorblockDlg )					m_RYLCastleDoorblockDlg->SetVisible( TRUE ) ;
		if ( m_bRYLArmsCreatMenuDlg )					m_RYLArmsCreatMenuDlg->SetVisible( TRUE ) ;
		if ( m_bRYLEmblemUpgradeDlg )					m_RYLEmblemUpgradeDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCampRightDlg )						m_RYLCampRightDlg->SetVisible( TRUE ) ;
		if ( m_bRYLSiegeArmsCreatingDlg )				m_RYLSiegeArmsCreatingDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCampUpgradeDlg )						m_RYLCampUpgradeDlg->SetVisible( TRUE ) ;

		if ( m_bRYLCampRepairDlg )						m_RYLCampRepairDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCampCreatingDlg )					m_RYLCampCreatingDlg->SetVisible( TRUE ) ;
		if ( m_bRYLCampShowTaxInfoDlg )					m_RYLCampShowTaxInfoDlg->SetVisible( TRUE ) ;
		if ( m_bRYLMiningAmountDlg )					m_RYLMiningAmountDlg->SetVisible( TRUE );
		if ( m_bRYLMiningCampDlg )						m_RYLMiningCampDlg->SetVisible( TRUE );
		if ( m_bRYLMiningAmountProcessDlg )				m_RYLMiningAmountProcessDlg->SetVisible( TRUE );
		if ( m_bRYLCampChangeDlg )						m_RYLCampChangeDlg->SetVisible( TRUE );
		if ( m_bRYLWorldWeaponLaunchDlg )				m_RYLWorldWeaponLaunchDlg->SetVisible( TRUE );
		if ( m_bRYLWorldWeaponLoadDlg )					m_RYLWorldWeaponLoadDlg->SetVisible( TRUE );
		if ( m_bRYLCampShopDlg )						m_RYLCampShopDlg->SetVisible( TRUE );

		if ( m_bRYLArmsHPDlg )							m_RYLArmsHPDlg->SetVisible( TRUE );
		if ( m_bRYLRespawnDlg	)						m_RYLRespawnDlg->SetVisible( TRUE ) ;
		if ( m_bRYLBattleWaitDlg )						m_RYLBattleWaitDlg->SetVisible( TRUE ) ;
		if ( m_bRYLSkillTooltipDlg )					m_RYLSkillTooltipDlg->SetVisible( TRUE ) ;
		if ( m_bRYLAbilityTooltipDlg )					m_RYLAbilityTooltipDlg->SetVisible( TRUE ) ;
		if ( m_bRYLChangeNameDlg )						m_RYLChangeNameDlg->SetVisible( TRUE ) ;
	}
	else
	{
		if ( m_RYLJoystickConfigureDlg )				m_bRYLJoystickConfigureDlg			= m_RYLJoystickConfigureDlg->GetVisible() ;	
		if ( m_RYLSiegeTimeDlg )						m_bRYLSiegeTimeDlg					= m_RYLSiegeTimeDlg->GetVisible() ;	
		if ( m_RYLGuildWarPenaltyDlg )					m_bRYLGuildWarPenaltyDlg			= m_RYLGuildWarPenaltyDlg->GetVisible();		
		if ( m_RYLStatusAndQuestFrameDlg )				m_bRYLStatusAndQuestFrameDlg		= m_RYLStatusAndQuestFrameDlg->GetVisible() ;	
		if ( m_RYLSocietyFrameDlg )						m_bRYLSocietyFrameDlg				= m_RYLSocietyFrameDlg->GetVisible() ;
		if ( m_RYLGuildFrameDlg )						m_bRYLGuildFrameDlg					= m_RYLGuildFrameDlg->GetVisible() ;
		if ( m_RYLHumanInventoryFrameDlg )				m_bRYLHumanInventoryFrameDlg		= m_RYLHumanInventoryFrameDlg->GetVisible() ; 
		if ( m_RYLStatusDlg )							m_bRYLStatusDlg						= m_RYLStatusDlg->GetVisible() ;	
		if ( m_RYLStatusDlg2 )							m_bRYLStatusDlg2					= m_RYLStatusDlg2->GetVisible() ;//hz guigui
		if ( m_RYLStatusDlg3 )							m_bRYLStatusDlg3					= m_RYLStatusDlg3->GetVisible() ;//hz guigui
		if ( m_RYLDialogControlDlg )					m_bRYLDialogControlDlg				= m_RYLDialogControlDlg->GetVisible() ;		
		if ( m_RYLQuickSlotDlg )						m_bRYLQuickSlotDlg					= m_RYLQuickSlotDlg->GetVisible() ;			
		if ( m_RYLNewTradeDlg )							m_bRYLNewTradeDlg					= m_RYLNewTradeDlg->GetVisible() ;
		if ( m_RYLStreetStallDlg )						m_bRYLStreetStallDlg				= m_RYLStreetStallDlg->GetVisible() ;			
		if ( m_RYLMiniMapDlg )							m_bRYLMiniMapDlg					= m_RYLMiniMapDlg->GetVisible() ;				
		if ( m_RYLQuestOpeningDlg )						m_bRYLQuestOpeningDlg				= m_RYLQuestOpeningDlg->GetVisible() ;		
		if ( m_RYLQuestDetailDataDlg )					m_bRYLQuestDetailDataDlg			= m_RYLQuestDetailDataDlg->GetVisible() ;		
		if ( m_RYLBlacksmithDlg )						m_bRYLBlacksmithDlg					= m_RYLBlacksmithDlg->GetVisible() ;			
		if ( m_RYLStatusRetrainDlg )					m_bRYLStatusRetrainDlg				= m_RYLStatusRetrainDlg->GetVisible() ;		
		if ( m_RYLRankingDlg )							m_bRYLRankingDlg					= m_RYLRankingDlg->GetVisible() ;				
		if ( m_RYLBattleGroundDlg )						m_bRYLBattleGroundDlg				= m_RYLBattleGroundDlg->GetVisible() ;		
		if ( m_RYLLargeMapFrameDlg )					m_bRYLLargeMapFrameDlg				= m_RYLLargeMapFrameDlg->GetVisible();
		if ( m_RYLHelpFrameDlg )						m_bRYLHelpFrameDlg					= m_RYLHelpFrameDlg->GetVisible();
		if ( m_RYLExchangeDlg )							m_bRYLExchangeDlg					= m_RYLExchangeDlg->GetVisible() ;			
		if ( m_RYLJobChangeDlg )						m_bRYLJobChangeDlg					= m_RYLJobChangeDlg->GetVisible() ;			
		if ( m_RYLOptionDlg )							m_bRYLOptionDlg						= m_RYLOptionDlg->GetVisible() ;				
		if ( m_RYLVaultDlg )							m_bRYLVaultDlg						= m_RYLVaultDlg->GetVisible() ;				
		if ( m_RYLVaultPasswordDlg )					m_bRYLVaultPasswordDlg				= m_RYLVaultPasswordDlg->GetVisible() ;		
		if ( m_RYLVaultCertificationdDlg )				m_bRYLVaultCertificationdDlg		= m_RYLVaultCertificationdDlg->GetVisible() ; 
		if ( m_RYLMiniPartyDlg )						m_bRYLMiniPartyDlg					= m_RYLMiniPartyDlg->GetVisible() ;			
		if ( m_RYLSummonDlg )							m_bRYLSummonDlg 					= m_RYLSummonDlg->GetVisible() ;				
		if ( m_RYLSystemDlg )							m_bRYLSystemDlg 					= m_RYLSystemDlg->GetVisible() ;
		if ( m_RYLChattingDlg )							m_bRYLChattingDlg					= m_RYLChattingDlg->GetVisible() ;
		if ( m_RYLEnchantDlg )							m_bRYLEnchantDlg					= m_RYLEnchantDlg->GetVisible() ;
		if ( m_RYLBattleGroundPrevResultDlg )			m_bRYLBattleGroundPrevResultDlg 	= m_RYLBattleGroundPrevResultDlg->GetVisible();
		if ( m_RYLBattleGroundStatusDlg )				m_bRYLBattleGroundStatusDlg			= m_RYLBattleGroundStatusDlg->GetVisible();
		if ( m_RYLBattleGroundGameStatusDlg )			m_bRYLBattleGroundGameStatusDlg 	= m_RYLBattleGroundGameStatusDlg->GetVisible();
		if ( m_RYLBattleGroundChangeMoneyDlg )			m_bRYLBattleGroundChangeMoneyDlg	= m_RYLBattleGroundChangeMoneyDlg->GetVisible();
		if ( m_RYLBattleGroundResultDlg )				m_bRYLBattleGroundResultDlg			= m_RYLBattleGroundResultDlg->GetVisible();
		if ( m_RYLGMReportDlg )	                        m_bRYLGMReportDlg					= m_RYLGMReportDlg->GetVisible();
		if ( m_RYLItemReparationDlg )					m_bRYLItemReparationDlg				= m_RYLItemReparationDlg->GetVisible();
		if ( m_RYLItemCompositionDlg )					m_bRYLItemCompositionDlg			= m_RYLItemCompositionDlg->GetVisible();
		if ( m_RYLTempInventoryDlg )					m_bRYLTempInventoryDlg				= m_RYLTempInventoryDlg->GetVisible();
		if ( m_RYLCastleStatusFrameDlg )				m_bRYLCastleStatusFrameDlg			= m_RYLCastleStatusFrameDlg->GetVisible();
		if ( m_RYLCastleMineralDlg )					m_bRYLCastleMineralDlg				= m_RYLCastleMineralDlg->GetVisible();
		if ( m_RYLCastleEmblemRightDlg )				m_bRYLCastleEmblemRightDlg			= m_RYLCastleEmblemRightDlg->GetVisible() ;
		if ( m_RYLCastleDoorblockDlg )					m_bRYLCastleDoorblockDlg			= m_RYLCastleDoorblockDlg->GetVisible() ;
		if ( m_RYLArmsCreatMenuDlg )					m_bRYLArmsCreatMenuDlg				= m_RYLArmsCreatMenuDlg->GetVisible() ;
		if ( m_RYLEmblemUpgradeDlg )					m_bRYLEmblemUpgradeDlg				= m_RYLEmblemUpgradeDlg->GetVisible() ;
		if ( m_RYLCampRightDlg )						m_bRYLCampRightDlg					= m_RYLCampRightDlg->GetVisible() ;
		if ( m_RYLSiegeArmsCreatingDlg )				m_bRYLSiegeArmsCreatingDlg			= m_RYLSiegeArmsCreatingDlg->GetVisible() ;
		if ( m_RYLCampUpgradeDlg )						m_bRYLCampUpgradeDlg				= m_RYLCampUpgradeDlg->GetVisible() ;
		if ( m_RYLCampRepairDlg )						m_bRYLCampRepairDlg					= m_RYLCampRepairDlg->GetVisible() ;
		if ( m_RYLCampCreatingDlg )						m_bRYLCampCreatingDlg				= m_RYLCampCreatingDlg->GetVisible() ;
		if ( m_RYLCampShowTaxInfoDlg )					m_bRYLCampShowTaxInfoDlg			= m_RYLCampShowTaxInfoDlg->GetVisible() ;
		if ( m_RYLMiningAmountDlg )						m_bRYLMiningAmountDlg				= m_RYLMiningAmountDlg->GetVisible() ;
		if ( m_RYLMiningCampDlg )						m_bRYLMiningCampDlg					= m_RYLMiningCampDlg->GetVisible() ;
		if ( m_RYLMiningAmountProcessDlg )				m_bRYLMiningAmountProcessDlg		= m_RYLMiningAmountProcessDlg->GetVisible() ;
		if ( m_RYLCampChangeDlg )						m_bRYLCampChangeDlg					= m_RYLCampChangeDlg->GetVisible() ;
		if ( m_RYLWorldWeaponLaunchDlg )				m_bRYLWorldWeaponLaunchDlg			= m_RYLWorldWeaponLaunchDlg->GetVisible();
		if ( m_RYLWorldWeaponLoadDlg )					m_bRYLWorldWeaponLoadDlg			= m_RYLWorldWeaponLoadDlg->GetVisible();
		if ( m_RYLCampShopDlg )							m_bRYLCampShopDlg					= m_RYLCampShopDlg->GetVisible();
		if ( m_RYLArmsHPDlg )							m_bRYLArmsHPDlg						= m_RYLArmsHPDlg->GetVisible();
		if ( m_RYLRespawnDlg	)						m_bRYLRespawnDlg					= m_RYLRespawnDlg->GetVisible() ;
		if ( m_RYLBattleWaitDlg )						m_bRYLBattleWaitDlg					= m_RYLBattleWaitDlg->GetVisible() ;
		if ( m_RYLSkillTooltipDlg )						m_bRYLSkillTooltipDlg				= m_RYLSkillTooltipDlg->GetVisible() ;
		if ( m_RYLAbilityTooltipDlg )					m_bRYLAbilityTooltipDlg				= m_RYLAbilityTooltipDlg->GetVisible() ;
		if ( m_RYLChangeNameDlg )						m_bRYLChangeNameDlg					= m_RYLChangeNameDlg->GetVisible() ;
		
		if ( m_RYLJoystickConfigureDlg )				m_RYLJoystickConfigureDlg->SetVisible(FALSE) ;	
		if ( m_RYLSiegeTimeDlg )						m_RYLSiegeTimeDlg->SetVisible(FALSE) ;	
		if ( m_RYLGuildWarPenaltyDlg )					m_RYLGuildWarPenaltyDlg->SetVisible(FALSE) ;	
		if ( m_RYLStatusAndQuestFrameDlg )				m_RYLStatusAndQuestFrameDlg->SetVisible( FALSE ) ;
		if ( m_RYLSocietyFrameDlg )						m_RYLSocietyFrameDlg->SetVisible( FALSE ) ;
		if ( m_RYLGuildFrameDlg )						m_RYLGuildFrameDlg->SetVisible( FALSE ) ;
		if ( m_RYLHumanInventoryFrameDlg )				m_RYLHumanInventoryFrameDlg->SetVisible( FALSE ) ; 
		if ( m_RYLStatusDlg )							m_RYLStatusDlg->SetVisible( FALSE ) ;				
		if ( m_RYLStatusDlg2 )							m_RYLStatusDlg2->SetVisible( FALSE ) ;			//hz guigui
		if ( m_RYLStatusDlg3 )							m_RYLStatusDlg3->SetVisible( FALSE ) ;			//hz guigui
		if ( m_RYLDialogControlDlg )					m_RYLDialogControlDlg->SetVisible( FALSE ) ;		
		if ( m_RYLQuickSlotDlg )						m_RYLQuickSlotDlg->SetVisible( FALSE ) ;			
        if ( m_RYLNewTradeDlg )							m_RYLNewTradeDlg->SetVisible( FALSE ) ;				
		if ( m_RYLStreetStallDlg )						m_RYLStreetStallDlg->SetVisible( FALSE ) ;			
		if ( m_RYLMiniMapDlg )							m_RYLMiniMapDlg->SetVisible( FALSE ) ;		
		if ( m_RYLQuestOpeningDlg )						m_RYLQuestOpeningDlg->SetVisible( FALSE ) ;		
		if ( m_RYLQuestDetailDataDlg )	 				m_RYLQuestDetailDataDlg->SetVisible( FALSE ) ;		
		if ( m_RYLBlacksmithDlg )						m_RYLBlacksmithDlg->SetVisible( FALSE ) ;			
		if ( m_RYLStatusRetrainDlg )					m_RYLStatusRetrainDlg->SetVisible( FALSE ) ;		
		if ( m_RYLRankingDlg )							m_RYLRankingDlg->SetVisible( FALSE ) ;		
		if ( m_RYLBattleGroundDlg )						m_RYLBattleGroundDlg->SetVisible( FALSE ) ;		
		if ( m_RYLLargeMapFrameDlg )					m_RYLLargeMapFrameDlg->SetVisible( FALSE );
		if ( m_RYLHelpFrameDlg )						m_RYLHelpFrameDlg->SetVisible( FALSE );
		if ( m_RYLExchangeDlg )							m_RYLExchangeDlg->SetVisible( FALSE ) ;			
		if ( m_RYLJobChangeDlg )						m_RYLJobChangeDlg->SetVisible( FALSE ) ;			
		if ( m_RYLOptionDlg )							m_RYLOptionDlg->SetVisible( FALSE ) ;				
		if ( m_RYLVaultDlg )							m_RYLVaultDlg->SetVisible( FALSE ) ;				
		if ( m_RYLVaultPasswordDlg )					m_RYLVaultPasswordDlg->SetVisible( FALSE ) ;		
		if ( m_RYLVaultCertificationdDlg )				m_RYLVaultCertificationdDlg->SetVisible( FALSE ) ; 
		if ( m_RYLMiniPartyDlg )						m_RYLMiniPartyDlg->SetVisible( FALSE ) ;			
		if ( m_RYLSummonDlg )							m_RYLSummonDlg->SetVisible( FALSE ) ;				
		if ( m_RYLSystemDlg )							m_RYLSystemDlg->SetVisible( FALSE ) ;
		if ( m_RYLChattingDlg )							m_RYLChattingDlg->SetVisible( FALSE ) ;
		if ( m_RYLEnchantDlg )							m_RYLEnchantDlg->SetVisible( FALSE ) ;
		if ( m_RYLBattleGroundPrevResultDlg )			m_RYLBattleGroundPrevResultDlg->SetVisible( FALSE );
		if ( m_RYLBattleGroundStatusDlg )				m_RYLBattleGroundStatusDlg->SetVisible( FALSE );
		if ( m_RYLBattleGroundGameStatusDlg )			m_RYLBattleGroundGameStatusDlg->SetVisible( FALSE );
		if ( m_RYLBattleGroundChangeMoneyDlg )			m_RYLBattleGroundChangeMoneyDlg->SetVisible( FALSE );
		if ( m_RYLBattleGroundResultDlg )				m_RYLBattleGroundResultDlg->SetVisible( FALSE );
        if ( m_RYLGMReportDlg )		                    m_RYLGMReportDlg->SetVisible( FALSE );
		if ( m_RYLItemReparationDlg )					m_RYLItemReparationDlg->SetVisible( FALSE );
		if ( m_RYLItemCompositionDlg )					m_RYLItemCompositionDlg->SetVisible( FALSE );
		if ( m_RYLTempInventoryDlg )					m_RYLTempInventoryDlg->SetVisible( FALSE );
		if ( m_RYLCastleStatusFrameDlg )				m_RYLCastleStatusFrameDlg->SetVisible( FALSE );
		if ( m_RYLCastleMineralDlg )					m_RYLCastleMineralDlg->SetVisible( FALSE );
		if ( m_RYLCastleEmblemRightDlg )				m_RYLCastleEmblemRightDlg->SetVisible( FALSE ) ;
		if ( m_RYLCastleDoorblockDlg )					m_RYLCastleDoorblockDlg->SetVisible( FALSE ) ;
		if ( m_RYLArmsCreatMenuDlg )					m_RYLArmsCreatMenuDlg->SetVisible( FALSE ) ;
		if ( m_RYLEmblemUpgradeDlg )					m_RYLEmblemUpgradeDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampRightDlg )						m_RYLCampRightDlg->SetVisible( FALSE ) ;
		if ( m_RYLSiegeArmsCreatingDlg )				m_RYLSiegeArmsCreatingDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampUpgradeDlg )						m_RYLCampUpgradeDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampRepairDlg )						m_RYLCampRepairDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampCreatingDlg )						m_RYLCampCreatingDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampShowTaxInfoDlg )					m_RYLCampShowTaxInfoDlg->SetVisible( FALSE ) ;
		if ( m_RYLMiningAmountDlg )						m_RYLMiningAmountDlg->SetVisible( FALSE ) ;
		if ( m_RYLMiningCampDlg )						m_RYLMiningCampDlg->SetVisible( FALSE ) ;
		if ( m_RYLMiningAmountProcessDlg )				m_RYLMiningAmountProcessDlg->SetVisible( FALSE ) ;
		if ( m_RYLCampChangeDlg )						m_RYLCampChangeDlg->SetVisible( FALSE );
		if ( m_RYLWorldWeaponLaunchDlg )				m_RYLWorldWeaponLaunchDlg->SetVisible( FALSE );
		if ( m_RYLWorldWeaponLoadDlg )					m_RYLWorldWeaponLoadDlg->SetVisible( FALSE );
		if ( m_RYLCampShopDlg )							m_RYLCampShopDlg->SetVisible( FALSE );
		if ( m_RYLArmsHPDlg )							m_RYLArmsHPDlg->SetVisible( FALSE );
		if ( m_RYLRespawnDlg	)						m_RYLRespawnDlg->SetVisible( FALSE ) ;
		if ( m_RYLBattleWaitDlg )						m_RYLBattleWaitDlg->SetVisible( FALSE ) ;
		if ( m_RYLSkillTooltipDlg )						m_RYLSkillTooltipDlg->SetVisible( FALSE ) ;
		if ( m_RYLAbilityTooltipDlg )					m_RYLAbilityTooltipDlg->SetVisible( FALSE ) ;
		if ( m_RYLChangeNameDlg )						m_RYLChangeNameDlg->SetVisible( FALSE ) ;
	}
}

VOID		CRYLGameScene::GUIVisible( BOOL bVisible ) 
{
	m_RYLGuildFrameDlg->SetVisible( bVisible ) ;				
	m_RYLStatusAndQuestFrameDlg->SetVisible( bVisible ) ;		
	m_RYLSocietyFrameDlg->SetVisible( bVisible ) ;			
	m_RYLQuestOpeningDlg->SetVisible( bVisible ) ;			
	m_RYLQuestDetailDataDlg->SetVisible( bVisible ) ;			
	m_RYLStatusRetrainDlg->SetVisible( bVisible ) ;			
	m_RYLRankingDlg->SetVisible( bVisible ) ;					
	m_RYLLargeMapFrameDlg->SetVisible( bVisible );
	m_RYLHelpFrameDlg->SetVisible( bVisible );
	m_RYLJobChangeDlg->SetVisible( bVisible ) ;				
	m_RYLSystemDlg->SetVisible( bVisible ) ;					
	m_RYLOptionDlg->SetVisible( bVisible ) ;					
	m_RYLVaultPasswordDlg->SetVisible( bVisible ) ;			
	m_RYLVaultCertificationdDlg->SetVisible( bVisible ) ;		
	
	m_RYLVaultDlg->SetVisible( bVisible ) ;					
	m_RYLBlacksmithDlg->SetVisible( bVisible ) ;				
	m_RYLExchangeDlg->SetVisible( bVisible ) ;				
    m_RYLNewTradeDlg->SetVisible( bVisible ) ;					
	m_RYLStreetStallDlg->SetVisible( bVisible ) ;				
	m_RYLHumanInventoryFrameDlg->SetVisible( bVisible ) ;		
    m_RYLBattleGroundPrevResultDlg->SetVisible( bVisible );
    m_RYLBattleGroundStatusDlg->SetVisible( bVisible );
    m_RYLBattleGroundChangeMoneyDlg->SetVisible( bVisible );
    m_RYLGMReportDlg->SetVisible( bVisible );
	m_RYLChangeNameDlg->SetVisible( bVisible );

	m_RYLJoystickConfigureDlg->SetVisible(bVisible);
	m_RYLSiegeTimeDlg->SetVisible( bVisible );
	m_RYLGuildWarPenaltyDlg->SetVisible( bVisible );

	//----------------------------------------------------------------
	// 공성 관련

	m_RYLCastleStatusFrameDlg->SetVisible( bVisible ) ;
	m_RYLCastleMineralDlg->SetVisible( bVisible ) ;
	m_RYLCastleEmblemRightDlg->SetVisible( bVisible ) ;
	m_RYLCastleDoorblockDlg->SetVisible( bVisible ) ;
	m_RYLArmsCreatMenuDlg->SetVisible( bVisible ) ;
	m_RYLEmblemUpgradeDlg->SetVisible( bVisible ) ;
	m_RYLCampRightDlg->SetVisible( bVisible ) ;
	m_RYLSiegeArmsCreatingDlg->SetVisible( bVisible ) ;
	m_RYLCampUpgradeDlg->SetVisible( bVisible ) ;
	m_RYLCampRepairDlg->SetVisible( bVisible ) ;
	m_RYLCampCreatingDlg->SetVisible( bVisible ) ;
	m_RYLCampShowTaxInfoDlg->SetVisible( bVisible ) ;
	m_RYLMiningAmountDlg->SetVisible( bVisible ) ;
	m_RYLMiningCampDlg->SetVisible( bVisible ) ;
	m_RYLMiningAmountProcessDlg->SetVisible( bVisible ) ;
	m_RYLCampChangeDlg->SetVisible( bVisible );
	m_RYLWorldWeaponLaunchDlg->SetVisible( bVisible );
	m_RYLWorldWeaponLoadDlg->SetVisible( bVisible );
	m_RYLCampShopDlg->SetVisible( bVisible );
	m_RYLArmsHPDlg->SetVisible( bVisible );
	m_RYLRespawnDlg->SetVisible( bVisible ) ;
	m_RYLBattleWaitDlg->SetVisible( bVisible ) ;
}

VOID		CRYLGameScene::SetVisibleInfo( DWORD dwDlgType, BOOL bVisible )
{
	if ( bVisible ) m_MainWindowInfo.m_dwVisibleFlag |= dwDlgType ;
	else m_MainWindowInfo.m_dwVisibleFlag &= ~dwDlgType ;
}

BOOL		CRYLGameScene::GetVisibleInfo( DWORD dwDlgType )
{
	if ( m_MainWindowInfo.m_dwVisibleFlag & dwDlgType ) return TRUE ;
	return FALSE ;
}

VOID		CRYLGameScene::LoadMainWindowPosFromReg()
{
	// edith 2008.01.18 게임 메인창의 UI가
	// 메인 창의 좌표를 레지스트리에서 읽어온다.
	HKEY hMPClientReg ;
	unsigned long dwDisp = 0 ;

	RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\ROW\\MP-Client", //was ROW hayzohar
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hMPClientReg, &dwDisp ) ;

	unsigned long dwSize = sizeof(unsigned long) ;
	unsigned long dwValue = 0 ;
	unsigned long dwWidth = 0 ;
	unsigned long dwHeight = 0 ;

	m_MainWindowInfo.m_bDlgControlVertical = FALSE ;
	m_MainWindowInfo.m_bQuickSlotVertical = FALSE ;

	// 세로 저장 정보 읽어들이기
	if ( ERROR_SUCCESS == RegQueryValueEx( hMPClientReg, "Vertical", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		if ( 1 == LOWORD( dwValue ) ) m_MainWindowInfo.m_bDlgControlVertical = TRUE ;
		if ( 1 == HIWORD( dwValue ) ) m_MainWindowInfo.m_bQuickSlotVertical = TRUE ;
	}

	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Width", 0, NULL, (LPBYTE)&dwWidth, &dwSize ) )
	{
		dwWidth = 1024;
		m_MainWindowInfo.m_bDlgControlVertical = FALSE ;
		m_MainWindowInfo.m_bQuickSlotVertical = FALSE ;
	}

	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Height", 0, NULL, (LPBYTE)&dwHeight, &dwSize ) )
	{
		dwHeight = 768;
	}


	// edith 2008.01.18 레지스트리에서 게임 UI 좌표 읽기

	// 메인 컨트롤 윈도우
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "DlgControl", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		// 245 41
		m_MainWindowInfo.m_nDlgControlLeft = dwWidth-245; 
		m_MainWindowInfo.m_nDlgControlTop = dwHeight-41; 
		m_MainWindowInfo.m_bDlgControlVertical = FALSE ;
	}
	else
	{
		m_MainWindowInfo.m_nDlgControlLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nDlgControlTop = HIWORD( dwValue ) ;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nDlgControlLeft < 0 || 
			m_MainWindowInfo.m_nDlgControlTop < 0 ||
			m_MainWindowInfo.m_nDlgControlLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nDlgControlTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nDlgControlLeft = dwWidth-245; 
			m_MainWindowInfo.m_nDlgControlTop = dwHeight-41; 
			m_MainWindowInfo.m_bDlgControlVertical = FALSE ;
		}
	}

	// 전장 상태 확인 창
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "SiegeTime", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{		
		// 256, 180
		m_MainWindowInfo.m_nSiegeLeft	= dwWidth-256;
		m_MainWindowInfo.m_nSiegeTop	= 130;		
	}
	else
	{
		m_MainWindowInfo.m_nSiegeLeft	= LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nSiegeTop	= HIWORD( dwValue ) ;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nSiegeLeft < 0 || 
			m_MainWindowInfo.m_nSiegeTop < 0 ||
			m_MainWindowInfo.m_nSiegeLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nSiegeTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nSiegeLeft	= dwWidth-256;
			m_MainWindowInfo.m_nSiegeTop	= 130;		
		}
	}

	// 퀵슬롯
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "QuickSlot", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		// 메인 컨트롤 윈도우 왼쪽에 붙기 때문에 메인컨트롤 W값을 빼준다.
		m_MainWindowInfo.m_nQuickSlotLeft = dwWidth-563; 
		m_MainWindowInfo.m_nQuickSlotTop = dwHeight-41; 
		m_MainWindowInfo.m_bQuickSlotVertical = FALSE ;
	}
	else
	{
		m_MainWindowInfo.m_nQuickSlotLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nQuickSlotTop = HIWORD( dwValue ) ;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nQuickSlotLeft < 0 || 
			m_MainWindowInfo.m_nQuickSlotTop < 0 ||
			m_MainWindowInfo.m_nQuickSlotLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nQuickSlotTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nQuickSlotLeft = dwWidth-563; 
			m_MainWindowInfo.m_nQuickSlotTop = dwHeight-41; 
			m_MainWindowInfo.m_bQuickSlotVertical = FALSE ;
		}
	}

	// 상태창
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Status", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		// 238 41
		m_MainWindowInfo.m_nStatusLeft = 0; 
		m_MainWindowInfo.m_nStatusTop = 0; 
	}
	else
	{
		m_MainWindowInfo.m_nStatusLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nStatusTop = HIWORD( dwValue ) ;

        // 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nStatusLeft < 0 || 
			m_MainWindowInfo.m_nStatusTop < 0 ||
			m_MainWindowInfo.m_nStatusLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nStatusTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nStatusLeft = 0; 
			m_MainWindowInfo.m_nStatusTop = 0; 
		}
	}

	// 미니맵
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Map", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		m_MainWindowInfo.m_nMapLeft = dwWidth-140; 
		m_MainWindowInfo.m_nMapTop = 0; 
		m_MainWindowInfo.m_nMapType = 1;
	}
	else
	{
		m_MainWindowInfo.m_nMapLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nMapTop = HIWORD( dwValue ) ;

		if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "MapType", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
			m_MainWindowInfo.m_nMapType = 1;	// 중간값으로 지정
		else
			m_MainWindowInfo.m_nMapType = dwValue;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nMapLeft < 0 || 
			m_MainWindowInfo.m_nMapTop < 0 ||
			m_MainWindowInfo.m_nMapLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nMapTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nMapLeft = dwWidth-140; 
			m_MainWindowInfo.m_nMapTop = 0; 
			m_MainWindowInfo.m_nMapType = 1;
		}
	}

	// 챈트, 인챈트
	// 90 92
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Enchant", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		m_MainWindowInfo.m_nEnchantLeft = 240; 
		m_MainWindowInfo.m_nEnchantTop = 0; 
	}
	else
	{
		m_MainWindowInfo.m_nEnchantLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nEnchantTop = HIWORD( dwValue ) ;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nEnchantLeft < 0 || 
			m_MainWindowInfo.m_nEnchantTop < 0 ||
			m_MainWindowInfo.m_nEnchantLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nEnchantTop >= static_cast<INT>(dwHeight))
		{
			m_MainWindowInfo.m_nEnchantLeft = 240; 
			m_MainWindowInfo.m_nEnchantTop = 0; 
		}
	}

	// 채팅창 타입 정보
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "ChatDlgType", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		m_MainWindowInfo.m_nChatDlgType = 1 ;
	}
	else 
	{
		m_MainWindowInfo.m_nChatDlgType = static_cast<INT>(dwValue) ;
	}

	// 채팅창
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "Chat", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		if(m_MainWindowInfo.m_nChatDlgType == 0) // 319, 121 + 42
		{
			m_MainWindowInfo.m_nChatLeft = dwWidth-319; 
			m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
		}
		else if(m_MainWindowInfo.m_nChatDlgType == 1) // 563, 121 + 42
		{
			m_MainWindowInfo.m_nChatLeft = dwWidth-563; 
			m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
		}
		else if(m_MainWindowInfo.m_nChatDlgType == 2) // 319, 245 + 42
		{
			m_MainWindowInfo.m_nChatLeft = dwWidth-319; 
			m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
		}
		else if(m_MainWindowInfo.m_nChatDlgType == 3) // 563, 245 + 42
		{
			m_MainWindowInfo.m_nChatLeft = dwWidth-563; 
			m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
		}
	}
	else
	{
		m_MainWindowInfo.m_nChatLeft = LOWORD( dwValue ) ;
		m_MainWindowInfo.m_nChatTop = HIWORD( dwValue ) ;

		// 창밖으로 벗어났다면, 기본 세팅으로...
		if (m_MainWindowInfo.m_nChatLeft < 0 || 
			m_MainWindowInfo.m_nChatTop < 0 ||
			m_MainWindowInfo.m_nChatLeft >= static_cast<INT>(dwWidth) || 
			m_MainWindowInfo.m_nChatTop >= static_cast<INT>(dwHeight))
		{
			if(m_MainWindowInfo.m_nChatDlgType == 0) // 319, 121 + 42
			{
				m_MainWindowInfo.m_nChatLeft = dwWidth-319; 
				m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
			}
			else if(m_MainWindowInfo.m_nChatDlgType == 1) // 563, 121 + 42
			{
				m_MainWindowInfo.m_nChatLeft = dwWidth-563; 
				m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
			}
			else if(m_MainWindowInfo.m_nChatDlgType == 2) // 319, 245 + 42
			{
				m_MainWindowInfo.m_nChatLeft = dwWidth-319; 
				m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
			}
			else if(m_MainWindowInfo.m_nChatDlgType == 3) // 563, 245 + 42
			{
				m_MainWindowInfo.m_nChatLeft = dwWidth-563; 
				m_MainWindowInfo.m_nChatTop = dwHeight-(121 + 42); 
			}
		}
	}


	// HP/MP창 확장 레벨
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "StatusDlgExLv", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )
	{
		m_MainWindowInfo.m_nStatusExtensionLevel = 0 ;
	}
	else 
	{
		m_MainWindowInfo.m_nStatusExtensionLevel = static_cast<INT>(dwValue) ;
	}

	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "StatusDlgExLv2", 0, NULL, (LPBYTE)&dwValue, &dwSize ) )//hz guigui
	{
		m_MainWindowInfo.m_nStatusExtensionLevel = 0 ;
	}
	else 
	{
		m_MainWindowInfo.m_nStatusExtensionLevel = static_cast<INT>(dwValue) ;
	}

	// Visible Flag 정보
	if ( ERROR_SUCCESS != RegQueryValueEx( hMPClientReg, "VisibleFlag", 0, NULL, (LPBYTE)&m_MainWindowInfo.m_dwVisibleFlag, &dwSize ) )
	{
		m_MainWindowInfo.m_dwVisibleFlag = RYLMAINWINDOW::ALL_WINDOW ;
	}

	RegCloseKey( hMPClientReg ) ;

	float fMovieHeight = ( ( float )dwWidth / 1.8f ) ;
	int   iBarHeight = ( int )( ( dwHeight - fMovieHeight ) / 2.0f ) ;

	m_f2ndBarSY     = dwHeight - iBarHeight ;

	if(iBarHeight < 60)
	{
		iBarHeight = 60;
		m_f2ndBarSY = dwHeight - iBarHeight;
	}

	CGMImageDib*	pImageDIB = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( dwWidth, ( int )iBarHeight, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xff000000 ) ;

	DWORD dwResultColor = 0x00000000 ;
	for ( int i = 0 ; i < 15 ; i ++ )
	{
		dwResultColor += 0x10000000 ;
		pImageDIB->DrawFrameRect( 0, i, dwWidth, 1, dwResultColor ) ;
	}
	
	m_pUpMovieBarImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pUpMovieBarImage->LoadFromImage( pImageDIB ) ;
	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( dwWidth, ( int )iBarHeight, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xff000000 ) ;
	
	dwResultColor = 0x00000000 ;
	for (int i = 0 ; i < 15 ; i ++ )
	{
		dwResultColor += 0x10000000 ;
		pImageDIB->DrawFrameRect( 0, i, dwWidth, 1, dwResultColor ) ;
	}
	m_pDownMovieBarImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pDownMovieBarImage->LoadFromImage( pImageDIB ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLGameScene::SaveMainWindowPosToReg()
{
	HKEY hMPClientReg ;
	unsigned long dwDisp = 0 ;
	unsigned long DlgControlVert = 0 ;
	unsigned long QuickSlotVert = 0 ;

	RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\ROW\\MP-Client", //was ROW hayzohar 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hMPClientReg, &dwDisp ) ;

	unsigned long dwValue = 0 ;
	unsigned long dwVisibleFlag = RYLMAINWINDOW::ALL_WINDOW ;

	// 메인 컨트롤 윈도우
	if ( m_RYLDialogControlDlg )
	{
		dwValue = MAKELONG( m_RYLDialogControlDlg->GetLeft(), m_RYLDialogControlDlg->GetTop() ) ;
		RegSetValueEx( hMPClientReg, "DlgControl", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// 퀵슬롯
	dwValue = 0 ;
	if ( m_RYLQuickSlotDlg )
	{
		dwValue = MAKELONG( m_RYLQuickSlotDlg->GetLeft(), m_RYLQuickSlotDlg->GetTop() ) ;
		if ( !m_RYLQuickSlotDlg->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::QUICKSLOT_WINDOW ;
		RegSetValueEx( hMPClientReg, "QuickSlot", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}
	
	// 상태창
	dwValue = 0 ;
	if ( m_RYLStatusDlg )
	{
		dwValue = MAKELONG( m_RYLStatusDlg->GetLeft(), m_RYLStatusDlg->GetTop() ) ;
		if ( !m_RYLStatusDlg->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::STATUS_WINDOW ;
		RegSetValueEx( hMPClientReg, "Status", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	dwValue = 0 ;
	if ( m_RYLStatusDlg2 )//hz guigui
	{
		dwValue = MAKELONG( m_RYLStatusDlg2->GetLeft(), m_RYLStatusDlg2->GetTop() ) ;
		if ( !m_RYLStatusDlg2->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::STATUS_WINDOW ;
		RegSetValueEx( hMPClientReg, "Status", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	dwValue = 0 ;
	if ( m_RYLStatusDlg3 )//hz guigui
	{
		dwValue = MAKELONG( m_RYLStatusDlg3->GetLeft(), m_RYLStatusDlg3->GetTop() ) ;
		if ( !m_RYLStatusDlg3->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::STATUS_WINDOW ;
		RegSetValueEx( hMPClientReg, "Status", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}
	
	// 전장 상태 확인 창.
	dwValue = 0 ;
	if ( m_RYLSiegeTimeDlg )
	{
		dwValue = MAKELONG( m_RYLSiegeTimeDlg->GetLeft(), m_RYLSiegeTimeDlg->GetTop() ) ;
		if ( !m_RYLSiegeTimeDlg->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::SIEGETIME_WINDOW ;
		RegSetValueEx( hMPClientReg, "SiegeTime", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// 미니맵
	dwValue = 0 ;
	if ( m_RYLMiniMapDlg )
	{
		dwValue = MAKELONG( m_RYLMiniMapDlg->GetLeft(), m_RYLMiniMapDlg->GetTop() ) ;

		if ( !m_RYLMiniMapDlg->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::MINIMAP_WINDOW ;

		RegSetValueEx( hMPClientReg, "Map", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
		unsigned long dwType = ((CRYLMIniMapDlg*)m_RYLMiniMapDlg)->GetViewType();
		RegSetValueEx( hMPClientReg, "MapType", 0, REG_DWORD, (LPBYTE)&dwType, sizeof(unsigned long) ) ;
	}

	// 챈트, 인챈트
	dwValue = 0 ;
	if ( m_RYLEnchantDlg )
	{
		dwValue = MAKELONG( m_RYLEnchantDlg->GetLeft(), m_RYLEnchantDlg->GetTop() ) ;
		if ( !m_RYLEnchantDlg->GetVisible() ) dwVisibleFlag &= ~RYLMAINWINDOW::ENCHANT_WINDOW ;
		RegSetValueEx( hMPClientReg, "Enchant", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}
	
	// 채팅창
	dwValue = 0 ;
	if ( m_RYLChattingDlg )
	{
		dwValue = MAKELONG( m_RYLChattingDlg->GetLeft(), m_RYLChattingDlg->GetTop() ) ;
		RegSetValueEx( hMPClientReg, "Chat", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// 세로 정보 저장
	if ( m_RYLDialogControlDlg && m_RYLQuickSlotDlg )
	{
		if ( TRUE == static_cast<CRYLDialogControlDlg*>(m_RYLDialogControlDlg)->GetVerticalEnable() ) DlgControlVert = 1 ;
		if ( TRUE == static_cast<CRYLQuickSlotDlg*>(m_RYLQuickSlotDlg)->GetVerticalEnable() ) QuickSlotVert = 1 ;
		dwValue = MAKELONG( DlgControlVert, QuickSlotVert ) ;
		RegSetValueEx( hMPClientReg, "Vertical", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// 채팅창 타입 저장
	if ( m_RYLChattingDlg )
	{
		dwValue = static_cast<INT>( static_cast<CRYLChattingDlg *>(m_RYLChattingDlg)->GetChatDlgType() ) ;
		RegSetValueEx( hMPClientReg, "ChatDlgType", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// HP/MP창 확장 레벨 저장
	if ( m_RYLStatusDlg )
	{
		dwValue = static_cast<INT>( static_cast<CRYLStatusDlg *>(m_RYLStatusDlg)->GetExtensionLevel() ) ;
		RegSetValueEx( hMPClientReg, "StatusDlgExLv", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	if ( m_RYLStatusDlg2 )//hz guigui
	{
		dwValue = static_cast<INT>( static_cast<CRYLStatusDlg *>(m_RYLStatusDlg2)->GetExtensionLevel() ) ;
		RegSetValueEx( hMPClientReg, "StatusDlgExLv2", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	if ( m_RYLStatusDlg3 )//hz guigui
	{
		dwValue = static_cast<INT>( static_cast<CRYLStatusDlg *>(m_RYLStatusDlg3)->GetExtensionLevel() ) ;
		RegSetValueEx( hMPClientReg, "StatusDlgExLv3", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(unsigned long) ) ;
	}

	// VisibleFlag 저장 ( 메인 다이얼로그는 닫을수 없는 창이므로 NULL 이면 게임이 시작되지 않은 상황이다 )
	if ( m_RYLDialogControlDlg )
	{
		RegSetValueEx( hMPClientReg, "VisibleFlag", 0, REG_DWORD, (LPBYTE)&dwVisibleFlag, sizeof(unsigned long) ) ;
	}

	RegCloseKey( hMPClientReg ) ;
}


