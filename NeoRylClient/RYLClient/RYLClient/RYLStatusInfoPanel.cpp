//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLStatusInfoPanel.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 상태 정보 패널
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include <winsock2.h>
#include "RYLStatusPanel.h"
#include "RYLButton.h"
#include "RYLLabel.h"

#include "RYLGameData.h"
#include "RYLStringTable.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLSceneManager.h"
#include "RYLRankingDlg.h"
#include "RYLGameScene.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "RYLNetworkData.h"
#include "RYLCharacterDataManager.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendEtc.h>

#include "RYLStatusInfoPanel.h"
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// global variables
//--------------------------------------------------------------------------------------------------------------------------
CRYLStatusInfoPanel* g_pStatusInfoPanel = NULL;

//--------------------------------------------------------------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLStatusInfoPanel
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
CRYLStatusInfoPanel::CRYLStatusInfoPanel( CGMUICustomDlg *pParentDlg ): CGMUICustomPanel( pParentDlg )
{
    m_pSTRLabel 				    = NULL;
    m_pSTRValueLabel 				= NULL;
    m_pDEXLabel 					= NULL;
    m_pDEXValueLabel 				= NULL;
    m_pCONLabel 					= NULL;
    m_pCONValueLabel 				= NULL;
    m_pINTLabel 					= NULL;
    m_pINTValueLabel 				= NULL;
    m_pWISLabel 					= NULL;
    m_pWISValueLabel 				= NULL;

    m_pTipString1Label              = NULL;
    m_pTipString2Label              = NULL;
    m_pTipString3Label              = NULL;
    m_pTipString4Label              = NULL;

    m_pBonusPointLabel 		        = NULL;
    m_pBonusPointValueLabel 		= NULL;

    m_pRightHandLabel               = NULL;
    m_pLeftHandLabel                = NULL;

    m_pDamageLabel                  = NULL;
    m_pRightDamageValueLabel        = NULL;
    m_pLeftDamageValueLabel         = NULL;
    m_pHitRationLabel               = NULL;
    m_pRightHitRationValueLabel     = NULL;
    m_pLeftHitRationValueLabel      = NULL;
    m_pCriticalLabel                = NULL;
    m_pRightCriticalValueLabel      = NULL;
    m_pLeftCriticalValueLabel       = NULL;

    m_pDefensivePowerLabel          = NULL;
    m_pDefensivePowerValueLabel     = NULL;
    m_pDefensiveRateValueLabel      = NULL;
    m_pEvasionLabel                 = NULL;
    m_pEvasionValueLabel            = NULL;
    m_pBlockPowerLabel              = NULL;
    m_pBlockPowerValueLabel         = NULL;
    m_pBlockRateValueLabel          = NULL;

    m_pMagicPowerLabel              = NULL;
    m_pMagicPowerValueLabel         = NULL;
    m_pMagicResistLabel             = NULL;
    m_pMagicResistValueLabel        = NULL;

    m_pStrPointIncBtn               = NULL;
    m_pDexPointIncBtn               = NULL;
    m_pConPointIncBtn               = NULL;
    m_pIntPointIncBtn               = NULL;
    m_pWisPointIncBtn               = NULL;

    CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	int i = 0 ;
	for ( ; i < 75 ; ++i )
	{
		SetRect( &rt, 0, 38, 256, 43 );
		pImageDIB->DrawRect( &rt, 0, 1 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	}
	SetRect( &rt, 0, 42, 256, 52 );
	pImageDIB->DrawRect( &rt, 0, ( 1 +(i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );

    pImageDIB->MakeFrameRect( 12, 4, 234, 122, 0xff959595, 0xc0000000,  1 );
    for ( i = 0 ; i < 5 ; ++i )
	{
		pImageDIB->MakeFrameRect( 54, 15 + ( i * 20 ), 37, 18, 0xff959595, 0xc0000000,  1 );
    }
    pImageDIB->MakeFrameRect( 124,  15, 116, 83, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect( 203, 103,  37, 18, 0xff959595, 0xc0000000,  1 );

    pImageDIB->MakeFrameRect(  12, 132, 234, 96, 0xff959595, 0xc0000000,  1 );
    for ( i = 0 ; i < 3 ; ++i )
	{
        static int nPosX1 = 66;
        static int nPosX2 = 155;
        static int nPosY  = 156;

		pImageDIB->MakeFrameRect( nPosX1, nPosY + ( i * 20 ), 84, 18, 0xff959595, 0xc0000000,  1 );
        pImageDIB->MakeFrameRect( nPosX2, nPosY + ( i * 20 ), 84, 18 , 0xff959595, 0xc0000000,  1 );
    }

    pImageDIB->MakeFrameRect(  12, 234, 234, 75, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect(  67, 238,  85, 19, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect( 156, 238,  85, 19, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect(  67, 258,  85, 19, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect(  67, 278,  85, 19, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect( 156, 278,  85, 19, 0xff959595, 0xc0000000,  1 );

    pImageDIB->MakeFrameRect(  12, 315, 234, 55, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect(  67, 319,  85, 19, 0xff959595, 0xc0000000,  1 );
    pImageDIB->MakeFrameRect(  67, 339,  85, 19, 0xff959595, 0xc0000000,  1 );

    pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	
	AttachPanelImage( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );
	
	SetWidth( 256 );
	SetHeight( 395 );

    g_pStatusInfoPanel = this;
}                                       

CRYLStatusInfoPanel::~CRYLStatusInfoPanel()
{
	FinalCtrl() ;
}

//--------------------------------------------------------------------------------------------------------------------------
// members
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::InitCtrl()
VOID CRYLStatusInfoPanel::InitCtrl()
{
    RECT rt;

    int nPosY = -36;

    // STR
	m_pSTRLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pSTRLabel->SetLeft( 26 );
	m_pSTRLabel->SetTop( nPosY + 56 );
	m_pSTRLabel->SetFontColor(0xffffffff);
	m_pSTRLabel->SetAutoSize( TRUE );
	m_pSTRLabel->SetCaption( "STR" );
	m_pSTRLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1801 ], 1801, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pSTRLabel );

	// STR 값
	m_pSTRValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pSTRValueLabel->SetLeft( 61 );
	m_pSTRValueLabel->SetTop( nPosY + 55 );
	m_pSTRValueLabel->SetFontColor(0xffffffff);
	m_pSTRValueLabel->SetAutoSize( TRUE );
	m_pSTRValueLabel->SetCaption( "" );
	m_pSTRValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1801 ], 1801, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pSTRValueLabel );

	// DEX
	m_pDEXLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDEXLabel->SetLeft( 26 );
	m_pDEXLabel->SetTop( nPosY + 75 );
	m_pDEXLabel->SetFontColor(0xffffffff);
	m_pDEXLabel->SetAutoSize( TRUE );
	m_pDEXLabel->SetCaption( "DEX" );
	m_pDEXLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1802 ], 1802, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pDEXLabel );
                                  
	// DEX 값
	m_pDEXValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDEXValueLabel->SetLeft( 61 );
	m_pDEXValueLabel->SetTop( nPosY + 75 );
	m_pDEXValueLabel->SetFontColor(0xffffffff);
	m_pDEXValueLabel->SetAutoSize( TRUE );
	m_pDEXValueLabel->SetCaption( "" );
	m_pDEXValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1802 ], 1802, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pDEXValueLabel );

	// CON
	m_pCONLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pCONLabel->SetLeft( 24 );
	m_pCONLabel->SetTop( nPosY + 95 );
	m_pCONLabel->SetFontColor(0xffffffff);
	m_pCONLabel->SetAutoSize( TRUE );
	m_pCONLabel->SetCaption( "CON" );
	m_pCONLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1803 ], 1803, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pCONLabel );

	// CON 값
	m_pCONValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pCONValueLabel->SetLeft( 61 );
	m_pCONValueLabel->SetTop( nPosY + 95 );
	m_pCONValueLabel->SetFontColor(0xffffffff);
	m_pCONValueLabel->SetAutoSize( TRUE );
	m_pCONValueLabel->SetCaption( "" );
	m_pCONValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1803 ], 1803, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pCONValueLabel );

	// INT	
	m_pINTLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pINTLabel->SetLeft( 31 );
	m_pINTLabel->SetTop( nPosY + 115 );
	m_pINTLabel->SetFontColor(0xffffffff);
	m_pINTLabel->SetAutoSize( TRUE );
	m_pINTLabel->SetCaption( "INT" );
	m_pINTLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1804 ], 1804, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pINTLabel );

	// INT 값
	m_pINTValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pINTValueLabel->SetLeft( 61);
	m_pINTValueLabel->SetTop( nPosY + 115 );
	m_pINTValueLabel->SetFontColor(0xffffffff);
	m_pINTValueLabel->SetAutoSize( TRUE );
	m_pINTValueLabel->SetCaption( "" );
	m_pINTValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1804 ], 1804, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pINTValueLabel );

	// WIS
	m_pWISLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pWISLabel->SetLeft( 29 );
	m_pWISLabel->SetTop( nPosY + 135 );
	m_pWISLabel->SetFontColor(0xffffffff);
	m_pWISLabel->SetAutoSize( TRUE );
	m_pWISLabel->SetCaption( "WIS" );
	m_pWISLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1805 ], 1805, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pWISLabel );

	// WIS 값
	m_pWISValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pWISValueLabel->SetLeft( 61 );
	m_pWISValueLabel->SetTop( nPosY + 135 );
	m_pWISValueLabel->SetFontColor(0xffffffff);
	m_pWISValueLabel->SetAutoSize( TRUE );
	m_pWISValueLabel->SetCaption( "" );
	m_pWISValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1805 ], 1805, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pWISValueLabel );

    // '다음 레벨에'
    m_pTipString1Label = new CRYLLabel( m_pGMUIParentDlg );		
	m_pTipString1Label->SetLeft( 131 );
	m_pTipString1Label->SetTop( nPosY + 58 );
	m_pTipString1Label->SetFontColor(0xffffffff);
	m_pTipString1Label->SetAutoSize( TRUE );
	m_pTipString1Label->SetCaption( CRYLStringTable::m_strString[ 3468 ] );
	CGMUICustomPanel::AddChild( m_pTipString1Label );

    // 보너스 1
    m_pTipString2Label = new CRYLLabel( m_pGMUIParentDlg );		
	m_pTipString2Label->SetLeft( 131 );
	m_pTipString2Label->SetTop( nPosY + 77 );
	m_pTipString2Label->SetFontColor(0xffffffff);
	m_pTipString2Label->SetAutoSize( TRUE );
	m_pTipString2Label->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pTipString2Label );

    // 보너스 2
    m_pTipString3Label = new CRYLLabel( m_pGMUIParentDlg );		
	m_pTipString3Label->SetLeft( 131 );
	m_pTipString3Label->SetTop( nPosY + 96 );
	m_pTipString3Label->SetFontColor(0xffffffff);
	m_pTipString3Label->SetAutoSize( TRUE );
	m_pTipString3Label->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pTipString3Label );

    // '보너스가 있습니다.'
    m_pTipString4Label = new CRYLLabel( m_pGMUIParentDlg );		
	m_pTipString4Label->SetLeft( 131 );
	m_pTipString4Label->SetTop( nPosY + 115 );
	m_pTipString4Label->SetFontColor(0xffffffff);
	m_pTipString4Label->SetAutoSize( TRUE );
	m_pTipString4Label->SetCaption( CRYLStringTable::m_strString[ 3469 ] );
	CGMUICustomPanel::AddChild( m_pTipString4Label );

    // 보너스 포인트
	m_pBonusPointLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pBonusPointLabel->SetLeft( 124 );
	m_pBonusPointLabel->SetTop( nPosY + 143 );
	m_pBonusPointLabel->SetFontColor(0xffffffff);
	m_pBonusPointLabel->SetAutoSize( TRUE );
	m_pBonusPointLabel->SetCaption( CRYLStringTable::m_strString[ 1910 ] );
	m_pBonusPointLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1811 ], 1811, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pBonusPointLabel );

	// 보너스 포인트 값
	m_pBonusPointValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pBonusPointValueLabel->SetLeft( 210 );
	m_pBonusPointValueLabel->SetTop( nPosY + 143 );
	m_pBonusPointValueLabel->SetFontColor(0xffffffff);
	m_pBonusPointValueLabel->SetAutoSize( TRUE );
	m_pBonusPointValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pBonusPointValueLabel );

    // 오른손
	m_pRightHandLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pRightHandLabel->SetLeft( 92 );
	m_pRightHandLabel->SetTop( nPosY + 176 );
	m_pRightHandLabel->SetFontColor(0xffffffff);
	m_pRightHandLabel->SetAutoSize( TRUE );
	m_pRightHandLabel->SetCaption( CRYLStringTable::m_strString[ 3470 ] );
	CGMUICustomPanel::AddChild( m_pRightHandLabel );

    // 왼손
	m_pLeftHandLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pLeftHandLabel->SetLeft( 187 );
	m_pLeftHandLabel->SetTop( nPosY + 176 );
	m_pLeftHandLabel->SetFontColor(0xffffffff);
	m_pLeftHandLabel->SetAutoSize( TRUE );
	m_pLeftHandLabel->SetCaption( CRYLStringTable::m_strString[ 3471 ] );
	CGMUICustomPanel::AddChild( m_pLeftHandLabel );

    // 데미지
	m_pDamageLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDamageLabel->SetLeft( 25 );
	m_pDamageLabel->SetTop( nPosY + 197 );
	m_pDamageLabel->SetFontColor(0xffffffff);
	m_pDamageLabel->SetAutoSize( TRUE );
	m_pDamageLabel->SetCaption( CRYLStringTable::m_strString[ 3472 ] );
	CGMUICustomPanel::AddChild( m_pDamageLabel );

	// 오른손 데미지 값
	m_pRightDamageValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pRightDamageValueLabel->SetLeft( 79 );
	m_pRightDamageValueLabel->SetTop( nPosY + 197 );
	m_pRightDamageValueLabel->SetFontColor(0xffffffff);
	m_pRightDamageValueLabel->SetAutoSize( TRUE );
	m_pRightDamageValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pRightDamageValueLabel );

    // 왼손 데미지 값
	m_pLeftDamageValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pLeftDamageValueLabel->SetLeft( 168 );
	m_pLeftDamageValueLabel->SetTop( nPosY + 197 );
	m_pLeftDamageValueLabel->SetFontColor(0xffffffff);
	m_pLeftDamageValueLabel->SetAutoSize( TRUE );
	m_pLeftDamageValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pLeftDamageValueLabel );

    // 명중률
	m_pHitRationLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pHitRationLabel->SetLeft( 31 );
	m_pHitRationLabel->SetTop( nPosY + 217 );
	m_pHitRationLabel->SetFontColor(0xffffffff);
	m_pHitRationLabel->SetAutoSize( TRUE );
	m_pHitRationLabel->SetCaption( CRYLStringTable::m_strString[ 3473 ] );
	CGMUICustomPanel::AddChild( m_pHitRationLabel );

	// 오른손 명중률 값
	m_pRightHitRationValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pRightHitRationValueLabel->SetLeft( 97 );
	m_pRightHitRationValueLabel->SetTop( nPosY + 216 );
	m_pRightHitRationValueLabel->SetFontColor(0xffffffff);
	m_pRightHitRationValueLabel->SetAutoSize( TRUE );
	m_pRightHitRationValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pRightHitRationValueLabel );

    // 왼손 명중률 값
	m_pLeftHitRationValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pLeftHitRationValueLabel->SetLeft( 186 );
	m_pLeftHitRationValueLabel->SetTop( nPosY + 216 );
	m_pLeftHitRationValueLabel->SetFontColor(0xffffffff);
	m_pLeftHitRationValueLabel->SetAutoSize( TRUE );
	m_pLeftHitRationValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pLeftHitRationValueLabel );

    // 크리티컬
	m_pCriticalLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pCriticalLabel->SetLeft( 18 );
	m_pCriticalLabel->SetTop( nPosY + 237 );
	m_pCriticalLabel->SetFontColor(0xffffffff);
	m_pCriticalLabel->SetAutoSize( TRUE );
	m_pCriticalLabel->SetCaption( CRYLStringTable::m_strString[ 3474 ] );
	CGMUICustomPanel::AddChild( m_pCriticalLabel );

	// 오른손 크리티컬 값
	m_pRightCriticalValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pRightCriticalValueLabel->SetLeft( 98 );
	m_pRightCriticalValueLabel->SetTop( nPosY + 237 );
	m_pRightCriticalValueLabel->SetFontColor(0xffffffff);
	m_pRightCriticalValueLabel->SetAutoSize( TRUE );
	m_pRightCriticalValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pRightCriticalValueLabel );

    // 왼손 크리티컬 값
	m_pLeftCriticalValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pLeftCriticalValueLabel->SetLeft( 187 );
	m_pLeftCriticalValueLabel->SetTop( nPosY + 237 );
	m_pLeftCriticalValueLabel->SetFontColor(0xffffffff);
	m_pLeftCriticalValueLabel->SetAutoSize( TRUE );
	m_pLeftCriticalValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pLeftCriticalValueLabel );

    // 방어력
	m_pDefensivePowerLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDefensivePowerLabel->SetLeft( 25 );
	m_pDefensivePowerLabel->SetTop( nPosY + 278 );
	m_pDefensivePowerLabel->SetFontColor(0xffffffff);
	m_pDefensivePowerLabel->SetAutoSize( TRUE );
	m_pDefensivePowerLabel->SetCaption( CRYLStringTable::m_strString[ 3475 ] );
	CGMUICustomPanel::AddChild( m_pDefensivePowerLabel );

	// 방어력 값
	m_pDefensivePowerValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDefensivePowerValueLabel->SetLeft( 98 );
	m_pDefensivePowerValueLabel->SetTop( nPosY + 278 );
	m_pDefensivePowerValueLabel->SetFontColor(0xffffffff);
	m_pDefensivePowerValueLabel->SetAutoSize( TRUE );
	m_pDefensivePowerValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pDefensivePowerValueLabel );

    // 방어률 값
	m_pDefensiveRateValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pDefensiveRateValueLabel->SetLeft( 187 );
	m_pDefensiveRateValueLabel->SetTop( nPosY + 278 );
	m_pDefensiveRateValueLabel->SetFontColor(0xffffffff);
	m_pDefensiveRateValueLabel->SetAutoSize( TRUE );
	m_pDefensiveRateValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pDefensiveRateValueLabel );

    // 회피
	m_pEvasionLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pEvasionLabel->SetLeft( 31 );
	m_pEvasionLabel->SetTop( nPosY + 298 );
	m_pEvasionLabel->SetFontColor(0xffffffff);
	m_pEvasionLabel->SetAutoSize( TRUE );
	m_pEvasionLabel->SetCaption( CRYLStringTable::m_strString[ 3476 ] );
	CGMUICustomPanel::AddChild( m_pEvasionLabel );

	// 회피 값
	m_pEvasionValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pEvasionValueLabel->SetLeft( 98 );
	m_pEvasionValueLabel->SetTop( nPosY + 298 );
	m_pEvasionValueLabel->SetFontColor(0xffffffff);
	m_pEvasionValueLabel->SetAutoSize( TRUE );
	m_pEvasionValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pEvasionValueLabel );

    // 블록
	m_pBlockPowerLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pBlockPowerLabel->SetLeft( 31 );
	m_pBlockPowerLabel->SetTop( nPosY + 318 );
	m_pBlockPowerLabel->SetFontColor(0xffffffff);
	m_pBlockPowerLabel->SetAutoSize( TRUE );
	m_pBlockPowerLabel->SetCaption( CRYLStringTable::m_strString[ 3477 ] );
	CGMUICustomPanel::AddChild( m_pBlockPowerLabel );

	// 블록 값
	m_pBlockPowerValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pBlockPowerValueLabel->SetLeft( 98 );
	m_pBlockPowerValueLabel->SetTop( nPosY + 318 );
	m_pBlockPowerValueLabel->SetFontColor(0xffffffff);
	m_pBlockPowerValueLabel->SetAutoSize( TRUE );
	m_pBlockPowerValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pBlockPowerValueLabel );

    // 블록률 값
	m_pBlockRateValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pBlockRateValueLabel->SetLeft( 187 );
	m_pBlockRateValueLabel->SetTop( nPosY + 318 );
	m_pBlockRateValueLabel->SetFontColor(0xffffffff);
	m_pBlockRateValueLabel->SetAutoSize( TRUE );
	m_pBlockRateValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pBlockRateValueLabel );

    // 마법력
	m_pMagicPowerLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pMagicPowerLabel->SetLeft( 25 );
	m_pMagicPowerLabel->SetTop( nPosY + 359 );
	m_pMagicPowerLabel->SetFontColor(0xffffffff);
	m_pMagicPowerLabel->SetAutoSize( TRUE );
	m_pMagicPowerLabel->SetCaption( CRYLStringTable::m_strString[ 3478 ] );
	CGMUICustomPanel::AddChild( m_pMagicPowerLabel );

	// 마법력 값
	m_pMagicPowerValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pMagicPowerValueLabel->SetLeft( 98 );
	m_pMagicPowerValueLabel->SetTop( nPosY + 359 );
	m_pMagicPowerValueLabel->SetFontColor(0xffffffff);
	m_pMagicPowerValueLabel->SetAutoSize( TRUE );
	m_pMagicPowerValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pMagicPowerValueLabel );

    // 마법저항
	m_pMagicResistLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pMagicResistLabel->SetLeft( 18 );
	m_pMagicResistLabel->SetTop( nPosY + 379 );
	m_pMagicResistLabel->SetFontColor(0xffffffff);
	m_pMagicResistLabel->SetAutoSize( TRUE );
	m_pMagicResistLabel->SetCaption( CRYLStringTable::m_strString[ 3479 ] );
	CGMUICustomPanel::AddChild( m_pMagicResistLabel );

	// 마법저항 값
	m_pMagicResistValueLabel = new CRYLLabel( m_pGMUIParentDlg );		
	m_pMagicResistValueLabel->SetLeft( 98 );
	m_pMagicResistValueLabel->SetTop( nPosY + 379 );
	m_pMagicResistValueLabel->SetFontColor(0xffffffff);
	m_pMagicResistValueLabel->SetAutoSize( TRUE );
	m_pMagicResistValueLabel->SetCaption( "" );
	CGMUICustomPanel::AddChild( m_pMagicResistValueLabel );

    CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 );

	// STR 포인트 증가,감소 버튼
	m_pStrPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pStrPointIncBtn->SetLeft( 94 );
	m_pStrPointIncBtn->SetTop( nPosY + 50 );
	m_pStrPointIncBtn->OnClickEvent = DlgClick ;
	m_pStrPointIncBtn->SetInstance( RYLSTATUSPANEL::STRPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pStrPointIncBtn->SetVisible( FALSE );
	m_pStrPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pStrPointIncBtn );

	// DEX 포인트 증가,감소 버튼
	m_pDexPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pDexPointIncBtn->SetLeft( 94 );
	m_pDexPointIncBtn->SetTop( nPosY + 70 );
	m_pDexPointIncBtn->OnClickEvent = DlgClick ;
	m_pDexPointIncBtn->SetInstance( RYLSTATUSPANEL::DEXPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pDexPointIncBtn->SetVisible( FALSE );
	m_pDexPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pDexPointIncBtn );

	// CON 포인트 증가,감소 버튼
	m_pConPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pConPointIncBtn->SetLeft( 94 );
	m_pConPointIncBtn->SetTop( nPosY + 90 );
	m_pConPointIncBtn->OnClickEvent = DlgClick ;
	m_pConPointIncBtn->SetInstance( RYLSTATUSPANEL::CONPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pConPointIncBtn->SetVisible( FALSE );
	m_pConPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pConPointIncBtn );

	// INT 포인트 증가,감소 버튼
	m_pIntPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pIntPointIncBtn->SetLeft( 94 );
	m_pIntPointIncBtn->SetTop( nPosY + 110 );
	m_pIntPointIncBtn->OnClickEvent = DlgClick ;
	m_pIntPointIncBtn->SetInstance( RYLSTATUSPANEL::INTPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pIntPointIncBtn->SetVisible( FALSE );
	m_pIntPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pIntPointIncBtn );

	// WIS 포인트 증가,감소 버튼
	m_pWisPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pWisPointIncBtn->SetLeft( 94 );
	m_pWisPointIncBtn->SetTop( nPosY + 130 );
	m_pWisPointIncBtn->OnClickEvent = DlgClick ;
	m_pWisPointIncBtn->SetInstance( RYLSTATUSPANEL::WISPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pWisPointIncBtn->SetVisible( FALSE );
	m_pWisPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pWisPointIncBtn );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::FinalCtrl()
VOID CRYLStatusInfoPanel::FinalCtrl()
{
    GM_DELETE( m_pSTRLabel );
    GM_DELETE( m_pSTRValueLabel );
    GM_DELETE( m_pDEXLabel );
    GM_DELETE( m_pDEXValueLabel );
    GM_DELETE( m_pCONLabel );
    GM_DELETE( m_pCONValueLabel );
    GM_DELETE( m_pINTLabel );
    GM_DELETE( m_pINTValueLabel );
    GM_DELETE( m_pWISLabel );
    GM_DELETE( m_pWISValueLabel );

    GM_DELETE( m_pTipString1Label );
    GM_DELETE( m_pTipString2Label );
    GM_DELETE( m_pTipString3Label );
    GM_DELETE( m_pTipString4Label );

    GM_DELETE( m_pBonusPointLabel );
    GM_DELETE( m_pBonusPointValueLabel );

    GM_DELETE( m_pRightHandLabel );
    GM_DELETE( m_pLeftHandLabel );

    GM_DELETE( m_pDamageLabel );
    GM_DELETE( m_pRightDamageValueLabel );
    GM_DELETE( m_pLeftDamageValueLabel );
    GM_DELETE( m_pHitRationLabel );
    GM_DELETE( m_pRightHitRationValueLabel );
    GM_DELETE( m_pLeftHitRationValueLabel );
    GM_DELETE( m_pCriticalLabel );
    GM_DELETE( m_pRightCriticalValueLabel );
    GM_DELETE( m_pLeftCriticalValueLabel );

    GM_DELETE( m_pDefensivePowerLabel );
    GM_DELETE( m_pDefensivePowerValueLabel );
    GM_DELETE( m_pDefensiveRateValueLabel );
    GM_DELETE( m_pEvasionLabel );
    GM_DELETE( m_pEvasionValueLabel );
    GM_DELETE( m_pBlockPowerLabel );
    GM_DELETE( m_pBlockPowerValueLabel );
    GM_DELETE( m_pBlockRateValueLabel );

    GM_DELETE( m_pMagicPowerLabel );
    GM_DELETE( m_pMagicPowerValueLabel );
    GM_DELETE( m_pMagicResistLabel );
    GM_DELETE( m_pMagicResistValueLabel );

    GM_DELETE( m_pStrPointIncBtn );
    GM_DELETE( m_pDexPointIncBtn );
    GM_DELETE( m_pConPointIncBtn );
    GM_DELETE( m_pIntPointIncBtn );
    GM_DELETE( m_pWisPointIncBtn );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::Update()
HRESULT CRYLStatusInfoPanel::Update()
{
    static char strString[MAX_PATH];
    CRYLGameData *pGame = CRYLGameData::Instance();

    // STR 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.STR );
	m_pSTRValueLabel->SetCaption( strString );
	
	// DEX 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.DEX );
	m_pDEXValueLabel->SetCaption( strString );
	
	// CON 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.CON );
	m_pCONValueLabel->SetCaption( strString );

	// INT 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.INT );
	m_pINTValueLabel->SetCaption( strString );
	
	// WIS 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.WIS );
	m_pWISValueLabel->SetCaption( strString );

    SetTipString();

    // 보너스 포인트 값
	sprintf( strString, "%4d", pGame->m_csStatus.m_Info.IP );
	m_pBonusPointValueLabel->SetCaption( strString );
	if ( pGame->m_csStatus.m_Info.IP && ( pGame->m_csStatus.m_Info.HP != 0 ) )
	{
		m_pStrPointIncBtn->SetVisible( TRUE );
		m_pDexPointIncBtn->SetVisible( TRUE );
		m_pConPointIncBtn->SetVisible( TRUE );
		m_pIntPointIncBtn->SetVisible( TRUE );
		m_pWisPointIncBtn->SetVisible( TRUE );
	}
	else
	{
		m_pStrPointIncBtn->SetVisible( FALSE );
		m_pDexPointIncBtn->SetVisible( FALSE );
		m_pConPointIncBtn->SetVisible( FALSE );
		m_pIntPointIncBtn->SetVisible( FALSE );
		m_pWisPointIncBtn->SetVisible( FALSE );
	}

    // 공력력 값 오른손 marco2013
	sprintf(strString, "%4d - %4d", pGame->m_csStatus.GetMinDamage(), pGame->m_csStatus.GetMaxDamage());
	long m_lDamage = pGame->m_csStatus.GetMinDamage() + ( pGame->m_csStatus.GetMaxDamage() - pGame->m_csStatus.GetMinDamage()) / 2;
	long m_lRealDamage = pGame->m_csStatus.GetBaseMinDamage() + ( pGame->m_csStatus.GetBaseMaxDamage() - pGame->m_csStatus.GetBaseMinDamage()) / 2;
	if( m_lDamage < m_lRealDamage )
	{
		m_pRightDamageValueLabel->SetCaption( strString );
		m_pRightDamageValueLabel->SetFontColor( 0xffff0000 );
	} 
    else if(m_lDamage > m_lRealDamage)
	{
		m_pRightDamageValueLabel->SetCaption( strString );
		m_pRightDamageValueLabel->SetFontColor( 0xff00ff00 );
	} 
    else
	{
		m_pRightDamageValueLabel->SetCaption( strString );
		m_pRightDamageValueLabel->SetFontColor( 0xffffffff );
	}

    // 명중률 값 오른손
	sprintf( strString, "%4d", pGame->m_csStatus.GetHitRate() );
	m_pRightHitRationValueLabel->SetCaption( strString );

	sprintf( strString,  CRYLStringTable::m_strString[ 2280 ], pGame->m_csStatus.GetHitRate(), pGame->m_csStatus.GetHitRate()/2, pGame->m_csStatus.GetHitRate()/3 );
	m_pRightHitRationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, strString, 1812, TOOLTIP_PANEL );

	if( pGame->m_csStatus.GetHitRate() < pGame->m_csStatus.GetBaseHitRate() )		// 원래 값 보다 낮아 졌을 때
	{
		m_pRightHitRationValueLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetHitRate() > pGame->m_csStatus.GetBaseHitRate() )	// 원래 값 보다 높아 졌을 떄
	{
		m_pRightHitRationValueLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pRightHitRationValueLabel->SetFontColor( 0xffffffff );
	}

    // 크리티컬 확률 오른손
	sprintf(strString, "%3d", pGame->m_csStatus.GetCritical() );
	m_pRightCriticalValueLabel->SetCaption( strString );
	sprintf(strString,  CRYLStringTable::m_strString[ 2282 ], pGame->m_csStatus.GetCriticalRate(), pGame->m_csStatus.GetCriticalMagnification() );
	m_pRightCriticalValueLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL );

    // 공격력 값 왼손
    // 왼손공격력 계산을 위해서 임시로 WeaponPos를 변경한다. 계산 후 다시 원상 복귀.
    pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, false, true );

    sprintf(strString, "%4d - %4d", pGame->m_csStatus.GetMinDamage(), pGame->m_csStatus.GetMaxDamage());
	m_lDamage = pGame->m_csStatus.GetMinDamage() + ( pGame->m_csStatus.GetMaxDamage() - pGame->m_csStatus.GetMinDamage()) / 2;
	m_lRealDamage = pGame->m_csStatus.GetBaseMinDamage() + ( pGame->m_csStatus.GetBaseMaxDamage() - pGame->m_csStatus.GetBaseMinDamage()) / 2;
    if( m_lDamage < m_lRealDamage )
	{
		m_pLeftDamageValueLabel->SetCaption( strString );
		m_pLeftDamageValueLabel->SetFontColor( 0xffff0000 );
	} 
    else if(m_lDamage > m_lRealDamage)
	{
		m_pLeftDamageValueLabel->SetCaption( strString );
		m_pLeftDamageValueLabel->SetFontColor( 0xff00ff00 );
	} 
    else
	{
		m_pLeftDamageValueLabel->SetCaption( strString );
		m_pLeftDamageValueLabel->SetFontColor( 0xffffffff );
	}

    // 명중률 값 왼손
	sprintf( strString, "%4d", pGame->m_csStatus.GetHitRate() );
	m_pLeftHitRationValueLabel->SetCaption( strString );

	sprintf( strString,  CRYLStringTable::m_strString[ 2280 ], pGame->m_csStatus.GetHitRate(), pGame->m_csStatus.GetHitRate()/2, pGame->m_csStatus.GetHitRate()/3 );
	m_pLeftHitRationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, strString, 1812, TOOLTIP_PANEL );

	if( pGame->m_csStatus.GetHitRate() < pGame->m_csStatus.GetBaseHitRate() )		// 원래 값 보다 낮아 졌을 때
	{
		m_pLeftHitRationValueLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetHitRate() > pGame->m_csStatus.GetBaseHitRate() )	// 원래 값 보다 높아 졌을 떄
	{
		m_pLeftHitRationValueLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pLeftHitRationValueLabel->SetFontColor( 0xffffffff );
	}

    // 크리티컬 확률 왼손
    sprintf(strString, "%3d", pGame->m_csStatus.GetCritical() );
	m_pLeftCriticalValueLabel->SetCaption( strString );
	sprintf(strString,  CRYLStringTable::m_strString[ 2282 ], pGame->m_csStatus.GetCriticalRate(), pGame->m_csStatus.GetCriticalMagnification() );
	m_pLeftCriticalValueLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL );
    
    pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, true, true );

    // 방어력 값
	sprintf(strString, "%4d", pGame->m_csStatus.GetArmor());
	if ( pGame->m_csStatus.GetArmor() < pGame->m_csStatus.GetBaseArmor() )
	{
		m_pDefensivePowerValueLabel->SetCaption( strString );
		m_pDefensivePowerValueLabel->SetFontColor( 0xffff0000 );

		m_pDefensiveRateValueLabel->SetFontColor( 0xffff0000 );
	} 
	else if( pGame->m_csStatus.GetArmor() > pGame->m_csStatus.GetBaseArmor() )
	{
		m_pDefensivePowerValueLabel->SetCaption( strString );
		m_pDefensivePowerValueLabel->SetFontColor( 0xff00ff00 );

		m_pDefensiveRateValueLabel->SetFontColor( 0xff00ff00 );
	} 
	else
	{
		m_pDefensivePowerValueLabel->SetCaption( strString );
		m_pDefensivePowerValueLabel->SetFontColor( 0xffffffff );

		m_pDefensiveRateValueLabel->SetFontColor( 0xffffffff );
	}
    // 방어률 값
	sprintf(strString, "%2.2f%%", pGame->m_csStatus.GetDefenceRate() );
	m_pDefensiveRateValueLabel->SetCaption( strString );

    // 회피 값
	sprintf( strString, "%4d", pGame->m_csStatus.GetEvade() );
	m_pEvasionValueLabel->SetCaption( strString );

	sprintf( strString,  CRYLStringTable::m_strString[ 2281 ], pGame->m_csStatus.GetEvade(), pGame->m_csStatus.GetEvade()*2, pGame->m_csStatus.GetEvade()*3 );
	m_pEvasionValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, strString, 1812, TOOLTIP_PANEL );

	if( pGame->m_csStatus.GetEvade() < pGame->m_csStatus.GetBaseEvade() )			// 원래 값 보다 낮아 졌을 때
	{
		m_pEvasionValueLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetEvade() > pGame->m_csStatus.GetBaseEvade() )		// 원래 값 보다 높아 졌을 떄
	{
		m_pEvasionValueLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pEvasionValueLabel->SetFontColor( 0xffffffff );
	}

    // 블록 값
	sprintf(strString, "%4d", pGame->m_csStatus.GetBlock() );
	if ( pGame->m_csStatus.GetBlockRate() < pGame->m_csStatus.GetBaseBlockRate() )
	{
		m_pBlockPowerValueLabel->SetCaption( strString );
	} 
	else if( pGame->m_csStatus.GetBlockRate() > pGame->m_csStatus.GetBaseBlockRate() )
	{
		m_pBlockPowerValueLabel->SetCaption( strString );
	} 
	else
	{
		m_pBlockPowerValueLabel->SetCaption( strString );
	}
	sprintf(strString, "%2.2f%%", pGame->m_csStatus.GetBlockRate());
    m_pBlockRateValueLabel->SetCaption( strString );

    // 마법력 값
	sprintf( strString, "%4d", pGame->m_csStatus.GetMagicPower() );
	m_pMagicPowerValueLabel->SetCaption( strString );

    // 마법저항 값
	sprintf(strString, "%4d", pGame->m_csStatus.GetMagicResist() );
	m_pMagicResistValueLabel->SetCaption( strString );
	sprintf(strString, CRYLStringTable::m_strString[ 2284 ], pGame->m_csStatus.GetMagicResistRate() );
	m_pMagicResistValueLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL );

    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::DlgClick()
VOID CRYLStatusInfoPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
    if( !g_pStatusInfoPanel )
        return;

    switch( Sender->GetInstance() )
	{
    case RYLSTATUSPANEL::STRPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_STR);
        }
		break ;

	case RYLSTATUSPANEL::DEXPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_DEX);
        }
		break ;

	case RYLSTATUSPANEL::CONPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_CON);
        }
		break ;

	case RYLSTATUSPANEL::INTPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_INT);
        }
		break ;

	case RYLSTATUSPANEL::WISPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_WIS);
        }
		break ;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::SetTipString()
VOID CRYLStatusInfoPanel::SetTipString()
{
    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
    if( !pSelfData )
        return;

    static string strTipString1;
    static string strTipString2;

    switch( pSelfData->m_usClass )
	{
		case 1:							// 파이터
            {
                strTipString1 = "STR + 1";
                strTipString2 = "CON + 1";
            }
			break;

		case 2:							// 로그
            {
                strTipString1 = "STR + 1";
                strTipString2 = "DEX + 1";
            }
			break;

		case 3:							// 메이지
            {
                strTipString1 = "INT + 1";
                strTipString2 = "DEX + 1";
            }
			break;

		case 4:							// 어콜라이트
            {
                strTipString1 = "WIS + 1";
                strTipString2 = "CON + 1";
            }
			break;

		case 5:							// 디펜더
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "STR + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 6:							// 워리어
            {
                strTipString1 = "STR + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 7:							// 어쌔신
            {
                strTipString1 = "DEX + 1";
                strTipString2 = "STR + 1";
            }
			break;

		case 8:							// 아쳐
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 1";
                    strTipString2 = "DEX + 2";
                }
                else
                {
                    strTipString1 = "STR + 2";
                    strTipString2 = "DEX+ 1";
                }
            }
			break;

		case 9:							// 소서러
            {
                strTipString1 = "INT + 2";
                strTipString2 = "DEX+ 1";
            }
			break;

		case 10:						// 인챈터
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "INT + 2";
                    strTipString2 = "DEX + 1";
                }
                else
                {
                    strTipString1 = "INT + 1";
                    strTipString2 = "DEX + 2";
                }
            }
			break;

		case 11:						// 프리스트
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "WIS + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "WIS + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 12:						// 클레릭
            {
                strTipString1 = "WIS + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 17:						// 컴배턴트
            {
                strTipString1 = "";
                strTipString2 = "CON + 1";
            }
			break;

		case 18:						// 오피세이터
            {
                strTipString1 = "";
                strTipString2 = "DEX + 1";
            }
			break;

		case 19:						// 템플러
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "STR + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 20:						// 어태커
            {
                strTipString1 = "STR + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 21:						// 거너
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "DEX + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "DEX + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 22:						// 룬 오프
            {
                strTipString1 = "INT + 2";
                strTipString2 = "DEX + 1";
            }
			break;

		case 23:						// 라이프 오프
            {
                strTipString1 = "WIS + 2";
                strTipString2 = "DEX + 1";
            }
			break;

		case 24:						// 쉐도우 오프
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "DEX + 2";
                    strTipString2 = "STR + 1";
                }
                else
                {
                    strTipString1 = "DEX + 1";
                    strTipString2 = "STR+ 2";
                }
            }
			break;
	}

    m_pTipString2Label->SetCaption( const_cast<LPSTR>( strTipString1.c_str() ) );
    m_pTipString3Label->SetCaption( const_cast<LPSTR>( strTipString2.c_str() ) );
}
