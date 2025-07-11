//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCharacterInfoPanel.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 캐릭터 정보 패널
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include <winsock2.h>

#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "GMImageDib.h"

#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLSpriteEx.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendEtc.h>

#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>

#include "RYLRankingDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLStatusPanel.h"
#include "RYLCharacterInfoPanel.h"
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// global variables
//--------------------------------------------------------------------------------------------------------------------------
CRYLCharacterInfoPanel* g_pCharacterInfoPanel = NULL;

//--------------------------------------------------------------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLCharacterInfoPanel
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
CRYLCharacterInfoPanel::CRYLCharacterInfoPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
    m_pGMUIParentDlg                = pParentDlg;

    m_pPanelNameLabel               = NULL;
	m_pKillStreak					= NULL;
    m_pNameLabel                    = NULL;
    m_pNameValueLabel               = NULL;
    m_pClassLabel                   = NULL;
    m_pClassValueLabel              = NULL;
    m_pLevelLabel                   = NULL;
    m_pLevelValueLabel              = NULL;
    m_pFameLabel                    = NULL;
    m_pFameValueLabel               = NULL;
    m_pServiceMedalLabel            = NULL;
    m_pServiceMedalValueLabel       = NULL;
    m_pGuildLabel                   = NULL;
    m_pGuildValueLabel              = NULL;
    m_pNationalityLabel             = NULL;
    m_pNationalityValueLabel        = NULL;

    m_pFameMarkSprite               = NULL;
    m_pGuildMarkSprite              = NULL;
    m_pNationMarkSprite             = NULL;

    m_pHPLabel                      = NULL;
    m_pHPValueLabel                 = NULL;
    m_pHPProgressBar                = NULL;
    m_pMPLabel                      = NULL;
    m_pMPValueLabel                 = NULL;
    m_pMPProgressBar                = NULL;
    m_pEXPLabel                     = NULL;
    m_pEXPValueLabel                = NULL;
    m_pEXPProgressBar               = NULL;

    m_pRankingButton                = NULL;
    m_pHPMPButton                   = NULL;

    CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;
	RECT			rt;

    CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	int i = 0;
	for(  ; i < 75 ; ++i )
	{
		SetRect( &rt, 0, 38, 256, 43 );
		pImageDIB->DrawRect( &rt, 0, 1 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	}
	SetRect( &rt, 0, 42, 256, 52 );
	pImageDIB->DrawRect( &rt, 0, ( 1 +(i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	
	pImageDIB->MakeFrameRect( 12,  25, 234, 155, 0xffB99E6B, 0xc0000000,  1 );
	pImageDIB->MakeFrameRect( 12, 186, 234,  72, 0xffB99E6B, 0xc0000000,  1 );

	for( i = 0 ; i < 10 ; ++i )
	{
        static int nPosX = 73;
        static int nPosY = 29;

        if( i < 7 )
        {
            pImageDIB->MakeFrameRect( nPosX, nPosY + ( i * 20 ), 101, 19, 0xff959595, 0xc0000000,  1 );
        }
        else
        {
            nPosY = 50;
            pImageDIB->MakeFrameRect( nPosX, nPosY + ( i * 20 ), 100, 18, 0xff959595, 0xc0000000,  1 );
        }

        if( i == 3 || i == 5 ) //|| i = 6 )
        {
            pImageDIB->MakeFrameRect( nPosX + 104, nPosY + ( i * 20 ), 37, 19, 0xff959595, 0xc0000000,  1 );
        }
	}

	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	
	AttachPanelImage( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );
	
	SetWidth( 256 );
	SetHeight( 395 );

    g_pCharacterInfoPanel = this ;
}

CRYLCharacterInfoPanel::~CRYLCharacterInfoPanel()
{
	FinalCtrl() ;
}

//--------------------------------------------------------------------------------------------------------------------------
// member function definitions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::InitCtrl() 
VOID	CRYLCharacterInfoPanel::InitCtrl() 
{
    int nPosY = -36;
    // 패널 이름
    m_pPanelNameLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pPanelNameLabel->SetLeft( 15 );
	m_pPanelNameLabel->SetTop( nPosY + 45 );
	m_pPanelNameLabel->SetFontColor(0xffffffff);
	m_pPanelNameLabel->SetAutoSize( TRUE );
	m_pPanelNameLabel->SetCaption( CRYLStringTable::m_strString[ 2790 ] );
	CGMUICustomPanel::AddChild( m_pPanelNameLabel );

    // 이름
    m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pNameLabel->SetLeft( 46 );
	m_pNameLabel->SetTop( nPosY + 69 );
	m_pNameLabel->SetFontColor(0xffffffff);
	m_pNameLabel->SetAutoSize( TRUE );
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1926 ] );
	m_pNameLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager,  CRYLStringTable::m_strString[ 1792 ], 1792, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pNameLabel );

    // 이름 값
	m_pNameValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pNameValueLabel->SetLeft( 82 );
	m_pNameValueLabel->SetTop( nPosY + 69 );
	m_pNameValueLabel->SetFontColor(0xffffffff);
	m_pNameValueLabel->SetAutoSize( TRUE );
	m_pNameValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pNameValueLabel );

    // 클레스
	m_pClassLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pClassLabel->SetLeft( 34 );
	m_pClassLabel->SetTop( nPosY + 89 );
	m_pClassLabel->SetFontColor(0xffffffff);
	m_pClassLabel->SetAutoSize( TRUE );
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ]	);
	m_pClassLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1795 ], 1795, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pClassLabel );

	// 클레스 값
	m_pClassValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pClassValueLabel->SetLeft( 93 );
	m_pClassValueLabel->SetTop( nPosY + 89 );
	m_pClassValueLabel->SetFontColor(0xffffffff);
	m_pClassValueLabel->SetAutoSize( TRUE );
	m_pClassValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pClassValueLabel );

	m_pKillStreak = new CRYLLabel( m_pGMUIParentDlg );
	m_pKillStreak->SetLeft( 82 );
	m_pKillStreak->SetTop( nPosY + 169 );
	m_pKillStreak->SetFontColor(0xffffffff);
	m_pKillStreak->SetAutoSize( TRUE );
	m_pKillStreak->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pKillStreak );
    // 레벨
	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pLevelLabel->SetLeft( 46 );
	m_pLevelLabel->SetTop( nPosY + 109 );
	m_pLevelLabel->SetFontColor(0xffffffff);
	m_pLevelLabel->SetAutoSize( TRUE );
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] );
	m_pLevelLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1796 ], 1796, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pLevelLabel );

	// 레벨 값
	m_pLevelValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pLevelValueLabel->SetLeft( 112 );
	m_pLevelValueLabel->SetTop( nPosY + 109 );
	m_pLevelValueLabel->SetFontColor(0xffffffff);
	m_pLevelValueLabel->SetAutoSize( TRUE );
	m_pLevelValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pLevelValueLabel );

    // 명성
	m_pFameLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pFameLabel->SetLeft( 46 );
	m_pFameLabel->SetTop( nPosY + 129 );
	m_pFameLabel->SetFontColor(0xffffffff);
	m_pFameLabel->SetAutoSize( TRUE );
	m_pFameLabel->SetCaption( CRYLStringTable::m_strString[ 1932 ] );
	m_pFameLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1798 ], 1798, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pFameLabel );

	// 명성 값
	m_pFameValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pFameValueLabel->SetLeft( 82 );
	m_pFameValueLabel->SetTop( nPosY + 129 );
	m_pFameValueLabel->SetFontColor(0xffffffff);
	m_pFameValueLabel->SetAutoSize( TRUE );
	m_pFameValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pFameValueLabel );

    // 공헌 메달
	m_pServiceMedalLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pServiceMedalLabel->SetLeft( 21 );
	m_pServiceMedalLabel->SetTop( nPosY + 149 );
	m_pServiceMedalLabel->SetFontColor(0xffffffff);
	m_pServiceMedalLabel->SetAutoSize( TRUE );
	m_pServiceMedalLabel->SetCaption( CRYLStringTable::m_strString[ 1933 ] );
	m_pServiceMedalLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1799 ], 1799, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pServiceMedalLabel );

	// 공헌 메달 값
	m_pServiceMedalValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pServiceMedalValueLabel->SetLeft( 82 );
	m_pServiceMedalValueLabel->SetTop( nPosY + 149 );
	m_pServiceMedalValueLabel->SetFontColor(0xffffffff);
	m_pServiceMedalValueLabel->SetAutoSize( TRUE );
	m_pServiceMedalValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pServiceMedalValueLabel );

    // 길드
	m_pGuildLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pGuildLabel->SetLeft( 46 );
	m_pGuildLabel->SetTop( nPosY + 169 );
	m_pGuildLabel->SetFontColor(0xffffffff);
	m_pGuildLabel->SetAutoSize( TRUE );
	m_pGuildLabel->SetCaption( CRYLStringTable::m_strString[ 1927 ] );
	m_pGuildLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1793 ], 1793, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pGuildLabel );

	// 길드 값
	m_pGuildValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pGuildValueLabel->SetLeft( 82 );
	m_pGuildValueLabel->SetTop( nPosY + 169 );
	m_pGuildValueLabel->SetFontColor(0xffffffff);
	m_pGuildValueLabel->SetAutoSize( TRUE );
	m_pGuildValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pGuildValueLabel );

    // 국적
	m_pNationalityLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pNationalityLabel->SetLeft( 46 );
	m_pNationalityLabel->SetTop( nPosY + 189 );
	m_pNationalityLabel->SetFontColor(0xffffffff);
	m_pNationalityLabel->SetAutoSize( TRUE );
	m_pNationalityLabel->SetCaption( CRYLStringTable::m_strString[ 2022 ] );
	m_pNationalityLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2278 ], 2278, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pNationalityLabel );

	// 국적 값
	m_pNationalityValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pNationalityValueLabel->SetLeft( 82 );
	m_pNationalityValueLabel->SetTop( nPosY + 189 );
	m_pNationalityValueLabel->SetFontColor(0xffffffff);
	m_pNationalityValueLabel->SetAutoSize( TRUE );
	m_pNationalityValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pNationalityValueLabel );

    // 계급장
    m_pFameMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pFameMarkSprite->SetLeft( 188 );
	m_pFameMarkSprite->SetTop( 92 );
	m_pFameMarkSprite->SetWidth( 12 );
	m_pFameMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pFameMarkSprite );

    // 길드 마크
    m_pGuildMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pGuildMarkSprite->SetLeft( 188 );
	m_pGuildMarkSprite->SetTop( 132 );
	m_pGuildMarkSprite->SetWidth( 12 );
	m_pGuildMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pGuildMarkSprite );

    // 국가 마크
    m_pNationMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pNationMarkSprite->SetLeft( 188 );
	m_pNationMarkSprite->SetTop( 152 );
	m_pNationMarkSprite->SetWidth( 12 );
	m_pNationMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pNationMarkSprite );

    //
    RECT rt;
    CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

    // HP
	m_pHPLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pHPLabel->SetLeft( 46 );
	m_pHPLabel->SetTop( nPosY + 230 );
	m_pHPLabel->SetFontColor(0xffffffff);
	m_pHPLabel->SetAutoSize( TRUE );
	m_pHPLabel->SetCaption( "HP" );
	m_pHPLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1809 ], 1809, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pHPLabel );
    // HP 값
    m_pHPValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pHPValueLabel->SetLeft( 92 );
	m_pHPValueLabel->SetTop( nPosY + 230 );
	m_pHPValueLabel->SetFontColor(0xffffffff);
	m_pHPValueLabel->SetAutoSize( TRUE );
	m_pHPValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pHPValueLabel );

    // HP 프로그레스바
    m_pHPProgressBar = new CRYLProgressBar( m_pGMUIParentDlg );
	m_pHPProgressBar->SetLeft( 83 );
	m_pHPProgressBar->SetTop( nPosY + 228 );
	m_pHPProgressBar->SetWidth( 82 );
	m_pHPProgressBar->SetHeight( 14 );
	m_pHPProgressBar->SetMax( 10 );

    CRYLSpriteList::Instance()->CreateGMImage( 82, 14, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 0, 82, 14 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_BAR_256x256 ) );
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	m_pHPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage );
	m_pHPProgressBar->SetPosition( 10 );
	CGMUICustomPanel::AddChild( m_pHPProgressBar );
	GM_DELETE( pImageDIB );

	// MP
	m_pMPLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pMPLabel->SetLeft( 46 );
	m_pMPLabel->SetTop( nPosY + 249 );
	m_pMPLabel->SetFontColor(0xffffffff);
	m_pMPLabel->SetAutoSize( TRUE );
	m_pMPLabel->SetCaption( "MP" );
	m_pMPLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1810 ], 1810, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pMPLabel );
    // MP 값
    m_pMPValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pMPValueLabel->SetLeft( 92 );
	m_pMPValueLabel->SetTop( nPosY + 249 );
	m_pMPValueLabel->SetFontColor(0xffffffff);
	m_pMPValueLabel->SetAutoSize( TRUE );
	m_pMPValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pMPValueLabel );

    // MP 프로그레스바
    m_pMPProgressBar = new CRYLProgressBar( m_pGMUIParentDlg );
	m_pMPProgressBar->SetLeft( 83 );
	m_pMPProgressBar->SetTop( nPosY + 247 );
	m_pMPProgressBar->SetWidth( 82 );
	m_pMPProgressBar->SetHeight( 14 );
	m_pMPProgressBar->SetMax( 10 );

    CRYLSpriteList::Instance()->CreateGMImage( 82, 14, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 17, 82, 31 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_BAR_256x256 ) );
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	m_pMPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage );
	m_pMPProgressBar->SetPosition( 10 );
	CGMUICustomPanel::AddChild( m_pMPProgressBar );
	GM_DELETE( pImageDIB );

    // EXP
    m_pEXPLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pEXPLabel->SetLeft( 46 );
	m_pEXPLabel->SetTop( nPosY + 269 );
	m_pEXPLabel->SetFontColor(0xffffffff);
	m_pEXPLabel->SetAutoSize( TRUE );
	m_pEXPLabel->SetCaption( "EXP" );
	m_pEXPLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1810 ], 1810, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pEXPLabel );
    // EXP 값
    m_pEXPValueLabel = new CRYLLabel( m_pGMUIParentDlg );
	m_pEXPValueLabel->SetLeft( 92 );
	m_pEXPValueLabel->SetTop( nPosY + 269 );
	m_pEXPValueLabel->SetFontColor(0xffffffff);
	m_pEXPValueLabel->SetAutoSize( TRUE );
	m_pEXPValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pEXPValueLabel );

    // EXP 프로그레스바
    m_pEXPProgressBar = new CRYLProgressBar( m_pGMUIParentDlg );
	m_pEXPProgressBar->SetLeft( 83 );
	m_pEXPProgressBar->SetTop( nPosY + 268 );
	m_pEXPProgressBar->SetWidth( 82 );
	m_pEXPProgressBar->SetHeight( 14 );
	m_pEXPProgressBar->SetMax( 10 );

    CRYLSpriteList::Instance()->CreateGMImage( 82, 14, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 34, 82, 48 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_BAR_256x256 ) );
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	m_pEXPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage );
	m_pEXPProgressBar->SetPosition( 10 );
	CGMUICustomPanel::AddChild( m_pEXPProgressBar );
	GM_DELETE( pImageDIB );

    CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 );
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance();
	// 랭킹 버튼
	m_pRankingButton = new CRYLButton( m_pGMUIParentDlg );
	m_pRankingButton->SetLeft( 106 );
	m_pRankingButton->SetTop( nPosY + 343 );
	m_pRankingButton->SetCaption( CRYLStringTable::m_strString[ 1943 ] );
	m_pRankingButton->SetFontColor( 0xffffffff);
	m_pRankingButton->SetColor( 0xffffffff);
	m_pRankingButton->OnClickEvent = DlgClick ;
	m_pRankingButton->SetInstance( RYLSTATUSPANEL::RANKING_BUTTON );
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pRankingButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2250 ], 2250, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pRankingButton );

    // HP/HP 버튼
	m_pHPMPButton = new CRYLButton( m_pGMUIParentDlg );
	m_pHPMPButton->SetLeft( 175 );
	m_pHPMPButton->SetTop( nPosY + 343 );
	m_pHPMPButton->SetCaption( CRYLStringTable::m_strString[ 1944 ] );
	m_pHPMPButton->SetFontColor( 0xffffffff);
	m_pHPMPButton->SetColor( 0xffffffff);
	m_pHPMPButton->OnClickEvent = DlgClick ;
	m_pHPMPButton->SetInstance( RYLSTATUSPANEL::HP_MP_BUTTON );
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pHPMPButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1818 ], 1818, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pHPMPButton );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::FinalCtrl() 
VOID CRYLCharacterInfoPanel::FinalCtrl() 
{
    GM_DELETE( m_pPanelNameLabel );
	GM_DELETE( m_pKillStreak );
    GM_DELETE( m_pNameLabel );
    GM_DELETE( m_pNameValueLabel );
    GM_DELETE( m_pClassLabel );
    GM_DELETE( m_pClassValueLabel );
    GM_DELETE( m_pLevelLabel );
    GM_DELETE( m_pLevelValueLabel );
    GM_DELETE( m_pFameLabel );
    GM_DELETE( m_pFameValueLabel );
    GM_DELETE( m_pServiceMedalLabel );
    GM_DELETE( m_pServiceMedalValueLabel );
    GM_DELETE( m_pGuildLabel );
    GM_DELETE( m_pGuildValueLabel );
    GM_DELETE( m_pNationalityLabel );
    GM_DELETE( m_pNationalityValueLabel );

    GM_DELETE( m_pFameMarkSprite );
    GM_DELETE( m_pGuildMarkSprite );
    GM_DELETE( m_pNationMarkSprite );

    GM_DELETE( m_pHPLabel );
    GM_DELETE( m_pHPValueLabel );
    GM_DELETE( m_pHPProgressBar );
    GM_DELETE( m_pMPLabel );
    GM_DELETE( m_pMPValueLabel );
    GM_DELETE( m_pMPProgressBar );
    GM_DELETE( m_pEXPLabel );
    GM_DELETE( m_pEXPValueLabel );
    GM_DELETE( m_pEXPProgressBar );

    GM_DELETE( m_pRankingButton );
    GM_DELETE( m_pHPMPButton );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::Show()
VOID CRYLCharacterInfoPanel::Show( CGMUIObject* Sender )
{
    m_pFameMarkSprite->SetVisible( TRUE );
    m_pGuildMarkSprite->SetVisible( TRUE );
    m_pNationMarkSprite->SetVisible( TRUE );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::Hide()
VOID CRYLCharacterInfoPanel::Hide( CGMUIObject* Sender )
{
    m_pFameMarkSprite->SetVisible( FALSE );
    m_pGuildMarkSprite->SetVisible( FALSE );
    m_pNationMarkSprite->SetVisible( FALSE );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::Update()
HRESULT CRYLCharacterInfoPanel::Update()
{
    static char strString[MAX_PATH];
	CRYLGameData *pGame = CRYLGameData::Instance();
    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
    if( !pSelfData )
        return S_FALSE;
	switch ( pGame->m_csStatus.m_Killstreak )
	{
	case 0:
		sprintf( strString, "None" ); 
		break;

	case 5:
		sprintf( strString, "Dominating!( x5 )" ); 
		break;

	case 8:
		sprintf( strString, "Godlike!( x8 )" ); 
		break;

	case 12:
		sprintf( strString, "Rampage!( x12 )" ); 
		break;

	case 18:
		sprintf( strString, "Unstopable!( x18 )" ); 
		break;

	case 24:
		sprintf( strString, "Holy Shit!( x24 )" ); 
		break;

	case 30:
		sprintf( strString, "Wicked Sick!( x30 )" ); 
		break;

	case 40:
		sprintf( strString, "Combo Whore!( x40 )" ); 
		break;

	}
	int ks;

	m_pKillStreak->SetCaption ( strString );

	// 이름 값
	m_pNameValueLabel->SetCaption( pGame->m_csStatus.m_Info.Name );
    
    // 클레스 값
	m_pClassValueLabel->SetCaption( pGame->GetClassName( pGame->m_csStatus.m_Info.Class ) );

    // 레벨 값
	sprintf( strString, "%3d", pGame->m_csStatus.m_Info.Level );
	m_pLevelValueLabel->SetCaption( strString );

    // 명성 값
	sprintf(strString, "%10d", pGame->m_csStatus.m_Info.Fame);
	m_pFameValueLabel->SetCaption( strString );

    CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();

	// 명성에 따른 계급장
	unsigned long dwClass = 0;
	unsigned long dwFame = pSelfData->m_ulFame;

	if (0 < dwFame && dwFame <= 2000)				{ dwClass = 1; 	} 
	else if (2000 < dwFame && dwFame <= 10000)		{ dwClass = 2; 	} 
	else if (10000 < dwFame && dwFame <= 25000)		{ dwClass = 3; 	} 
	else if (25000 < dwFame && dwFame <= 50000)		{ dwClass = 4; 	} 
	else if (50000 < dwFame && dwFame <= 75000)		{ dwClass = 5; 	} 
	else if (75000 < dwFame && dwFame <= 100000)	{ dwClass = 6; 	} 
	else if (100000 < dwFame && dwFame <= 150000)	{ dwClass = 7; 	} 
	else if (150000 < dwFame && dwFame <= 250000)	{ dwClass = 8; 	} 
	else if (250000 < dwFame && dwFame <= 500000)	{ dwClass = 9; 	} 
	else if (500000 < dwFame)						{ dwClass = 10;	} 

	if( 0 != dwClass )
	{
		// 국가를 파악한다.
		// // WORK_LIST 2.1 계정 국적 추가
		unsigned char cNation   = pGame->m_cAccountNation;
		unsigned long dwGID     = pSelfData->m_ulGuildID;
		if ( dwGID )
		{
			LPGUILDLARGEINFOEX  pMyGuildInfo = pPanel->FindGuild( dwGID );
			if ( NULL != pMyGuildInfo )
			{
				cNation = pMyGuildInfo->m_cInclination ; 
			}
		}
/*		else
		{
			switch( pSelfData->m_cRace )
			{
				case CClass::RaceType::HUMAN:	cNation = Creature::KARTERANT;	break;
				case CClass::RaceType::AKHAN:	cNation = Creature::MERKADIA;	break;
				default:						cNation = Creature::STATELESS;	break;
			}
		}
*/

		// 출력할 계급장을 결정한다.
        unsigned char cIndex = CRYLRaceBase::MAX_RANKING_SPRITE;
		switch( cNation )
		{
			case Creature::KARTERANT:			cIndex = (dwClass - 1);			break;
			case Creature::MERKADIA:			cIndex = (dwClass - 1) + 10;	break;
			case Creature::ALMIGHTY_PIRATE:		cIndex = (dwClass - 1) + 20;	break;
		}

		if( pGame->m_lpInterface->m_lpMark[ cIndex ] )
		{
			m_pFameMarkSprite->SetVisible( TRUE );
			m_pFameMarkSprite->AttachImageTexture( pGame->m_lpInterface->m_lpMark[ cIndex ] );
		}
	}
	else
	{
		m_pFameMarkSprite->SetVisible( FALSE );
	}

    // 공헌 메달 값
	sprintf( strString, "%10d", pGame->m_csStatus.m_Info.Mileage );
	m_pServiceMedalValueLabel->SetCaption( strString );

    // 길드 값
	if ( 0 != pGame->m_csStatus.m_Info.GID )
	{
		LPGUILDLARGEINFOEX		lpGuildLargeInfo = pPanel->FindGuild( pGame->m_csStatus.m_Info.GID );
		if ( lpGuildLargeInfo )	m_pGuildValueLabel->SetCaption( lpGuildLargeInfo->m_szName );
	}
	else 
    {
        m_pGuildValueLabel->SetCaption( "" );
    }

    // 길드 마크 출력
    LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice();

    CRYLSprite* pSprite = pPanel->GetGuildSprite( pSelfData->m_ulGuildID );
	if ( pSprite )
	{
        m_pGuildMarkSprite->SetVisible( TRUE );
        m_pGuildMarkSprite->AttachImageTexture( pSprite );
	}

    // 국가 값
	// WORK_LIST 2.1 계정 국적 추가
	if ( Creature::MERKADIA == pGame->m_cAccountNation )
	{
		m_pNationalityValueLabel->SetCaption( CRYLStringTable::m_strString[ 2264 ] );
		m_pNationalityValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2286 ], 1812, TOOLTIP_PANEL );
	}
	else if ( Creature::KARTERANT == pGame->m_cAccountNation )
	{
		m_pNationalityValueLabel->SetCaption( CRYLStringTable::m_strString[ 2233 ] );
		m_pNationalityValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2285 ], 1812, TOOLTIP_PANEL );
	}
	else if ( Creature::ALMIGHTY_PIRATE == pGame->m_cAccountNation )
	{
		m_pNationalityValueLabel->SetCaption( CRYLStringTable::m_strString[ 2268 ] );
		m_pNationalityValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2287 ], 1812, TOOLTIP_PANEL );
	}

	// HP/MP/EXP bar
	sprintf(strString, "%d / %d", static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP), pGame->m_csStatus.GetMaxHP() );
	m_pHPValueLabel->SetCaption( strString );

	m_pHPProgressBar->SetMax( pGame->m_csStatus.GetMaxHP() );
    if( static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP) < pGame->m_csStatus.GetMaxHP() )
    {   
        m_pHPProgressBar->SetPosition( static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP) );
    }
    else
    { 
        m_pHPProgressBar->SetPosition( pGame->m_csStatus.GetMaxHP() );
    }

    sprintf(strString, "%d / %d", static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP), pGame->m_csStatus.GetMaxMP() );
	m_pMPValueLabel->SetCaption( strString );

	m_pMPProgressBar->SetMax( pGame->m_csStatus.GetMaxMP() );
    if( static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) < pGame->m_csStatus.GetMaxMP() )
    {   
        m_pMPProgressBar->SetPosition( static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) );
    }
    else
    { 
        m_pMPProgressBar->SetPosition( pGame->m_csStatus.GetMaxMP() );
    }

	// EXP bar
    sprintf(strString, "%4.2f %%", static_cast<float>( (double)pGame->m_csStatus.m_Info.Exp / (double)pGame->m_csStatus.m_llExpMax * 100.0f ) );
	m_pEXPValueLabel->SetCaption( strString );

	unsigned long dwExp, dwExpMax , type;

	type = 1;
	if (1000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000;
		type = 1000;
	} 
	else if (1000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000;
		type = 1000000;
	} 
	else if (1000000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000000;
		type = 1000000000;
	} 
	else
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax;
		type = 1;
	}

	dwExp = pGame->m_csStatus.m_Info.Exp / type;

	m_pEXPProgressBar->SetMax( dwExpMax );
	m_pEXPProgressBar->SetPosition( dwExp );

    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCharacterInfoPanel::DlgClick
VOID CRYLCharacterInfoPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	switch( Sender->GetInstance() )
	{
	case RYLSTATUSPANEL::RANKING_BUTTON	:
		{
			if ( TRUE == ( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->GetVisible() )
			{
				( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->SetVisible( FALSE );
			}
			else
			{
				SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 0, 0 );
			}
		}		
		break ;

	case RYLSTATUSPANEL::HP_MP_BUTTON :
		{
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::GetCurrentScene() );
			if ( pScene )
			{
				pScene->GetStatusDlg()->SetVisible( !pScene->GetStatusDlg()->GetVisible() );

				// Visible Flag Setting
				pScene->SetVisibleInfo( RYLMAINWINDOW::STATUS_WINDOW, pScene->GetStatusDlg()->GetVisible() );
			}
		}
		break ; 
	}
}