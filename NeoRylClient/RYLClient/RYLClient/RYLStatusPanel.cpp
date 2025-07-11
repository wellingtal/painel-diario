#include <winsock2.h>
#include "RYLStatusPanel.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLSpriteEX.h"

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

#include "RYLStatusDlg.h"
#include "RYLNetworkData.h"


#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include "GMMemory.h"

CRYLStatusPanel*	g_pStatusPanel = NULL ;

CRYLStatusPanel::CRYLStatusPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pKillStreak					= NULL ;
	m_pNameLabel					= NULL ;	// 이름	
	m_pNameValueLabel				= NULL ;	// 이름 값
	m_pGuildLabel					= NULL ;	// 길드
	m_pGuildValueLabel				= NULL ;	// 길드 값
	m_pClassLabel					= NULL ;	// 클레스
	m_pClassValueLabel				= NULL ;	// 클레스 값
	m_pMarkLabek					= NULL ;	// 마크
	m_pLevelLabel					= NULL ;	// 레벨
	m_pLevelValueLabel				= NULL ;	// 레벨 값
	m_pExperienceLabel				= NULL ;	// 경험점
	m_pExperienceBalueLabel			= NULL ;	// 경험점 값
	m_pFameLabel					= NULL ;	// 명성
	m_pFameValueLabel				= NULL ;	// 명성 값
	m_pServiceMedalLabel			= NULL ;	// 공헌 매달
	m_pServiceMedalValueLabel		= NULL ;	// 공헌 매달 값
	m_pScoreLabel					= NULL ;	// 스코어
	m_pScoreValueLabel				= NULL ;	// 스코어 값
	m_pSTRLabel						= NULL ;	// STR
	m_pSTRValueLabel				= NULL ;	// STR 값
	m_pDEXLabel						= NULL ;	// DEX
	m_pDEXValueLabel				= NULL ;	// DEX 값
	m_pCONLabel						= NULL ;	// CON
	m_pCONValueLabel				= NULL ;	// CON 값
	m_pINTLabel						= NULL ;	// INT
	m_pINTValueLabel				= NULL ;	// INT 값
	m_pWISLabel						= NULL ;	// WIS
	m_pWISValueLabel				= NULL ;	// WIS 값

	m_pStrikingPowerLabel			= NULL ;	// 공격력		
	m_pStrikingPowerValueLabel		= NULL ;	// 공력력 값
	m_pDefensivePowerLabel			= NULL ;	// 방어력
	m_pDefensivePowerValueLabel		= NULL ;	// 방어력 값
	m_pArmorRateValueLabel			= NULL ;	// 방어율 값

	m_pHPLabel						= NULL ;	// HP
	m_pNowHPValueLabel				= NULL ;	// 현재 HP 값
	m_pMaxHPValueLabel				= NULL ;	// 최대 HP 값

	m_pMPLabel						= NULL ;	// MP
	m_pNowMPValueLabel				= NULL ;	// 현재 MP 값
	m_pMaxMPValueLabel				= NULL ;	// 최대 MP 값

	m_pBonusPointLabel				= NULL ;	// 보너스 포인트
	m_pBonusPointValueLabel			= NULL ;	// 보너스 포인트 값

	m_pHitRatioLabel				= NULL ;	// 명중
	m_pHitRatioValueLabel			= NULL ;	// 명중 값		
	m_pEvasionLabel					= NULL ;	// 회피
	m_pEvasionValueLabel			= NULL ;	// 회피 값

	m_pMagicPowerLabel				= NULL ;	// 마법력
	m_pMagicPowerValueLabel			= NULL ;	// 마법력 값

	m_pBlockPowerLabel				= NULL ;	// 블록률
	m_pBlockPowerValueLabel			= NULL ;	// 블록률 값

	m_pCriticalLabel				= NULL ;	// 크리티컬
	m_pCriticalValueLabel			= NULL ;	// 크리티컬 값

	m_pMagicResistLabel				= NULL ;	// 마법저항
	m_pMagicResistValueLabel		= NULL ;	// 마법저항 값

	m_pRankingButton				= NULL ;	// 랭킹 버튼
	m_pHPMPButton					= NULL ;	// mp / hp;
	m_pEnchantButton				= NULL ;	// 인첸트 창

	m_pNationLabel					= NULL ;	// 국적
	m_pNationValueLabel				= NULL ;	// 국적 값

	m_pGuildMarkSprite				= NULL ;	// 길드 마크


	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 1 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( 1 +(i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 7,     2, 243, 90, 0xffB99E6B, 0xc0000000,  1 ) ; // was 2, 243, 146,
	pImageDIB->MakeFrameRect( 7,   97, 243, 107, 0xffB99E6B, 0xc0000000,  1 ) ; // was 153, 243, 109,
	pImageDIB->MakeFrameRect( 7,   114, 243, 220, 0xffB99E6B, 0xc0000000,  1 ) ; // was 153, 243, 109,


	for ( i = 0 ; i < 3 ; i ++ )
	{
		pImageDIB->MakeFrameRect( 67, 6 + ( i * 20 ), 101, 19, 0xff959595, 0xc0000000,  1 ) ;
	//	pImageDIB->MakeFrameRect( 67, 86 + ( i * 20 ), 101, 19, 0xff959595, 0xc0000000,  1 ) ;
	}
	pImageDIB->MakeFrameRect( 205, 6, 37, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 67, 66, 175, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 205, 26, 37, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 205, 46, 37, 19, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 395 ) ;

	g_pStatusPanel	= this ;
}

CRYLStatusPanel::~CRYLStatusPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLStatusPanel::InitCtrl() 
{
	RECT rt ;
	// 이름
	m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pNameLabel->SetLeft( 39 ) ;
	m_pNameLabel->SetTop( 10 ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	SetRect( &rt, 14, 10, 63, 22 ) ;
	m_pNameLabel->SetAlignRect( rt ) ;
	m_pNameLabel->SetAlign( RIGHT ) ;
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager,  CRYLStringTable::m_strString[ 1792 ], 1792, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel ) ;

	// 이름 값
	m_pNameValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pNameValueLabel->SetLeft( 78 ) ;
	m_pNameValueLabel->SetTop( 10 ) ;
	m_pNameValueLabel->SetFontColor (0xffffffff);
	m_pNameValueLabel->SetCaption( "" ) ;
	SetRect( &rt, 68, 10, 166, 22 ) ;
	m_pNameValueLabel->SetAlignRect( rt ) ;
	m_pNameValueLabel->SetAlign( CENTER ) ;
	m_pNameValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameValueLabel ) ;

	// 국적
	m_pNationLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNationLabel->SetLeft( 179 ) ;
	m_pNationLabel->SetTop( 10 ) ;
	m_pNationLabel->SetFontColor (0xffffffff);
	m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2022 ] ) ;
	SetRect( &rt, 180, 10, 202, 22 ) ;
	m_pNationLabel->SetAlignRect( rt ) ;
	m_pNationLabel->SetAlign( RIGHT ) ;
	m_pNationLabel->SetAutoSize( TRUE ) ;
	m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2278 ], 2278, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pNationLabel ) ;

	// 국적 값
	m_pNationValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNationValueLabel->SetLeft( 216 ) ;
	m_pNationValueLabel->SetTop( 10 ) ;
	m_pNationValueLabel->SetFontColor (0xffffffff);
	m_pNationValueLabel->SetCaption( "" ) ;
	SetRect( &rt, 206, 10, 241, 22 ) ;
	m_pNationValueLabel->SetAlignRect( rt ) ;
	m_pNationValueLabel->SetAlign( CENTER ) ;
	m_pNationValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNationValueLabel ) ;

	// 길드
	m_pGuildLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pGuildLabel->SetLeft( 39 ) ;
	m_pGuildLabel->SetTop( 30 ) ;
	m_pGuildLabel->SetFontColor (0xffffffff);
	m_pGuildLabel->SetCaption( CRYLStringTable::m_strString[ 1927 ] ) ;
	SetRect( &rt, 14, 30, 63, 42 ) ;
	m_pGuildLabel->SetAlignRect( rt ) ;
	m_pGuildLabel->SetAlign( RIGHT ) ;
	m_pGuildLabel->SetAutoSize( TRUE ) ;
	m_pGuildLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1793 ], 1793, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pGuildLabel ) ;

	// 길드 값
	m_pGuildValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pGuildValueLabel->SetLeft( 78 ) ;
	m_pGuildValueLabel->SetTop( 30 ) ;
	m_pGuildValueLabel->SetFontColor (0xffffffff);
	m_pGuildValueLabel->SetCaption( "" ) ;
	SetRect( &rt, 68, 30, 166, 42 ) ;
	m_pGuildValueLabel->SetAlignRect( rt ) ;
	m_pGuildValueLabel->SetAlign( CENTER ) ;
	m_pGuildValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildValueLabel ) ;

	// 클레스
	m_pClassLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pClassLabel->SetLeft( 27 ) ;
	m_pClassLabel->SetTop( 50 ) ;
	m_pClassLabel->SetFontColor (0xffffffff);
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ]	) ;
	SetRect( &rt, 14, 50, 63, 62 ) ;
	m_pClassLabel->SetAlignRect( rt ) ;
	m_pClassLabel->SetAlign( RIGHT ) ;
	m_pClassLabel->SetAutoSize( TRUE ) ;
	m_pClassLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1795 ], 1795, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel ) ;

	// 클레스 값
	m_pClassValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pClassValueLabel->SetLeft( 78 ) ;
	m_pClassValueLabel->SetTop( 50 ) ;
	m_pClassValueLabel->SetFontColor (0xffffffff);
	m_pClassValueLabel->SetCaption( "" ) ;
	SetRect( &rt, 68, 50, 166, 62 ) ;
	m_pClassValueLabel->SetAlignRect( rt ) ;
	m_pClassValueLabel->SetAlign( CENTER ) ;
	m_pClassValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassValueLabel ) ;

	// 마크
	m_pMarkLabek = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pMarkLabek->SetLeft( 179 ) ;
	m_pMarkLabek->SetTop( 30 ) ;
	m_pMarkLabek->SetFontColor (0xffffffff);
	m_pMarkLabek->SetCaption( CRYLStringTable::m_strString[ 1929 ] ) ;
	SetRect( &rt, 180, 30, 202, 42 ) ;
	m_pMarkLabek->SetAlignRect( rt ) ;
	m_pMarkLabek->SetAlign( RIGHT ) ;
	m_pMarkLabek->SetAutoSize( TRUE ) ;
	m_pMarkLabek->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1794 ], 1794, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pMarkLabek ) ;

	// 레벨
	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pLevelLabel->SetLeft( 179 ) ;
	m_pLevelLabel->SetTop( 50 ) ;
	m_pLevelLabel->SetFontColor (0xffffffff);
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	SetRect( &rt, 180, 50, 202, 62 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( RIGHT ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	m_pLevelLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1796 ], 1796, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel ) ;

	// 레벨 값
	m_pLevelValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pLevelValueLabel->SetLeft( 211 ) ;
	m_pLevelValueLabel->SetTop( 50 ) ;
	m_pLevelValueLabel->SetFontColor (0xffffffff);
	m_pLevelValueLabel->SetCaption( "" ) ;
	SetRect( &rt, 206, 50, 241, 62 ) ;
	m_pLevelValueLabel->SetAlignRect( rt ) ;
	m_pLevelValueLabel->SetAlign( CENTER ) ;
	m_pLevelValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelValueLabel ) ;

	// 경험점
	m_pExperienceLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pExperienceLabel->SetLeft( 27 ) ;
	m_pExperienceLabel->SetTop( 70 ) ;
	m_pExperienceLabel->SetFontColor (0xffffffff);
	m_pExperienceLabel->SetCaption( CRYLStringTable::m_strString[ 1931 ] ) ;
	SetRect( &rt, 14, 70, 63, 82 ) ;
	m_pExperienceLabel->SetAlignRect( rt ) ;
	m_pExperienceLabel->SetAlign( RIGHT ) ;
	m_pExperienceLabel->SetAutoSize( TRUE ) ;
	m_pExperienceLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pExperienceLabel ) ;

	// 경험점 값
	m_pExperienceBalueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pExperienceBalueLabel->SetLeft( 78 ) ;
	m_pExperienceBalueLabel->SetTop( 70 ) ;
	m_pExperienceBalueLabel->SetFontColor (0xffffffff);
	m_pExperienceBalueLabel->SetCaption( "" ) ;
	SetRect( &rt, 68, 70, 241, 82 ) ;
	m_pExperienceBalueLabel->SetAlignRect( rt ) ;
	m_pExperienceBalueLabel->SetAlign( CENTER ) ;
	m_pExperienceBalueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pExperienceBalueLabel ) ;


	// PATCH NOTES NEW LABEL BY AVALANSA
	m_pPatchNotesAvalansaLabel = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesAvalansaLabel->SetLeft( 32 ) ;
	m_pPatchNotesAvalansaLabel->SetTop( 101 ) ;
	m_pPatchNotesAvalansaLabel->SetFontColor (0xffD7DF01);
	m_pPatchNotesAvalansaLabel->SetCaption( CRYLStringTable::m_strString[ 5000 ] ) ;
	SetRect( &rt, 22, 101, 241, 114 ) ;
	m_pPatchNotesAvalansaLabel->SetAlignRect( rt ) ;
	m_pPatchNotesAvalansaLabel->SetAlign( CENTER ) ;
	m_pPatchNotesAvalansaLabel->SetAutoSize( TRUE ) ;
//	m_pPatchNotesAvalansaLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesAvalansaLabel ) ;


	m_pPatchNotesLine1 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine1->SetLeft( 22 ) ;
	m_pPatchNotesLine1->SetTop( 125 ) ;
	m_pPatchNotesLine1->SetFontColor (0xffF7D358);
	m_pPatchNotesLine1->SetCaption( CRYLStringTable::m_strString[ 5001 ] ) ;
	m_pPatchNotesLine1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine1 ) ;

	m_pPatchNotesLine2 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine2->SetLeft( 22 ) ;
	m_pPatchNotesLine2->SetTop( 140 ) ;
	m_pPatchNotesLine2->SetFontColor (0xffF7D358);
	m_pPatchNotesLine2->SetCaption( CRYLStringTable::m_strString[ 5002 ] ) ;
	m_pPatchNotesLine2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine2 ) ;

	m_pPatchNotesLine3 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine3->SetLeft( 22 ) ;
	m_pPatchNotesLine3->SetTop( 155 ) ;
	m_pPatchNotesLine3->SetFontColor (0xffF7D358);
	m_pPatchNotesLine3->SetCaption( CRYLStringTable::m_strString[ 5003 ] ) ;
	m_pPatchNotesLine3->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine3 ) ;

	m_pPatchNotesLine4 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine4->SetLeft( 22 ) ;
	m_pPatchNotesLine4->SetTop( 170 ) ;
	m_pPatchNotesLine4->SetFontColor (0xffF7D358);
	m_pPatchNotesLine4->SetCaption( CRYLStringTable::m_strString[ 5004 ] ) ;
	m_pPatchNotesLine4->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine4 ) ;

	m_pPatchNotesLine5 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine5->SetLeft( 22 ) ;
	m_pPatchNotesLine5->SetTop( 185 ) ;
	m_pPatchNotesLine5->SetFontColor (0xffF7D358);
	m_pPatchNotesLine5->SetCaption( CRYLStringTable::m_strString[ 5005 ] ) ;
	m_pPatchNotesLine5->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine5 ) ;

	m_pPatchNotesLine6 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine6->SetLeft( 22 ) ;
	m_pPatchNotesLine6->SetTop( 200 ) ;
	m_pPatchNotesLine6->SetFontColor (0xffF7D358);
	m_pPatchNotesLine6->SetCaption( CRYLStringTable::m_strString[ 5006 ] ) ;
	m_pPatchNotesLine6->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine6 ) ;

	m_pPatchNotesLine7 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine7->SetLeft( 22 ) ;
	m_pPatchNotesLine7->SetTop( 215 ) ;
	m_pPatchNotesLine7->SetFontColor (0xffF7D358);
	m_pPatchNotesLine7->SetCaption( CRYLStringTable::m_strString[ 5007 ] ) ;
	m_pPatchNotesLine7->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine7 ) ;

	m_pPatchNotesLine8 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine8->SetLeft( 22 ) ;
	m_pPatchNotesLine8->SetTop( 230 ) ;
	m_pPatchNotesLine8->SetFontColor (0xffF7D358);
	m_pPatchNotesLine8->SetCaption( CRYLStringTable::m_strString[ 5008 ] ) ;
	m_pPatchNotesLine8->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine8 ) ;

	m_pPatchNotesLine9 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine9->SetLeft( 22 ) ;
	m_pPatchNotesLine9->SetTop( 245 ) ;
	m_pPatchNotesLine9->SetFontColor (0xffF7D358);
	m_pPatchNotesLine9->SetCaption( CRYLStringTable::m_strString[ 5009 ] ) ;
	m_pPatchNotesLine9->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine9 ) ;

	m_pPatchNotesLine10 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine10->SetLeft( 22 ) ;
	m_pPatchNotesLine10->SetTop( 260 ) ;
	m_pPatchNotesLine10->SetFontColor (0xffF7D358);
	m_pPatchNotesLine10->SetCaption( CRYLStringTable::m_strString[ 5010 ] ) ;
	m_pPatchNotesLine10->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine10 ) ;

	m_pPatchNotesLine11 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine11->SetLeft( 22 ) ;
	m_pPatchNotesLine11->SetTop( 275 ) ;
	m_pPatchNotesLine11->SetFontColor (0xffF7D358);
	m_pPatchNotesLine11->SetCaption( CRYLStringTable::m_strString[ 5011 ] ) ;
	m_pPatchNotesLine11->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine11 ) ;

	m_pPatchNotesLine12 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine12->SetLeft( 22 ) ;
	m_pPatchNotesLine12->SetTop( 290 ) ;
	m_pPatchNotesLine12->SetFontColor (0xffF7D358);
	m_pPatchNotesLine12->SetCaption( CRYLStringTable::m_strString[ 5012 ] ) ;
	m_pPatchNotesLine12->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine12 ) ;

	m_pPatchNotesLine13 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine13->SetLeft( 22 ) ;
	m_pPatchNotesLine13->SetTop( 305 ) ;
	m_pPatchNotesLine13->SetFontColor (0xffF7D358);
	m_pPatchNotesLine13->SetCaption( CRYLStringTable::m_strString[ 5013 ] ) ;
	m_pPatchNotesLine13->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine13 ) ;

	m_pPatchNotesLine14 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine14->SetLeft( 22 ) ;
	m_pPatchNotesLine14->SetTop( 320 ) ;
	m_pPatchNotesLine14->SetFontColor (0xffF7D358);
	m_pPatchNotesLine14->SetCaption( CRYLStringTable::m_strString[ 5014 ] ) ;
	m_pPatchNotesLine14->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine14 ) ;

	m_pPatchNotesLine15 = new CRYLLabel( m_pGMUIParentDlg ) ;		
	m_pPatchNotesLine15->SetLeft( 22 ) ;
	m_pPatchNotesLine15->SetTop( 335 ) ;
	m_pPatchNotesLine15->SetFontColor (0xffF7D358);
	m_pPatchNotesLine15->SetCaption( CRYLStringTable::m_strString[ 5015 ] ) ;
	m_pPatchNotesLine15->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPatchNotesLine15 ) ;



	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	// 랭킹
	m_pRankingButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pRankingButton->SetLeft( 98 ) ;
	m_pRankingButton->SetTop( 343 ) ;
	m_pRankingButton->SetCaption( CRYLStringTable::m_strString[ 1943 ] ) ;
	m_pRankingButton->SetFontColor( 0xffffffff) ;
	m_pRankingButton->SetColor( 0xffffffff) ;
	m_pRankingButton->OnClickEvent = DlgClick ;
	m_pRankingButton->SetInstance( RYLSTATUSPANEL::RANKING_BUTTON ) ;
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRankingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRankingButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2250 ], 2250, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pRankingButton ) ;

	// MP/HP
	m_pHPMPButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pHPMPButton->SetLeft( 173 ) ;
	m_pHPMPButton->SetTop( 343 ) ;
	m_pHPMPButton->SetCaption( CRYLStringTable::m_strString[ 1944 ] ) ;
	m_pHPMPButton->SetFontColor( 0xffffffff) ;
	m_pHPMPButton->SetColor( 0xffffffff) ;
	m_pHPMPButton->OnClickEvent = DlgClick ;
	m_pHPMPButton->SetInstance( RYLSTATUSPANEL::HP_MP_BUTTON ) ;
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pHPMPButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pHPMPButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1818 ], 1818, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pHPMPButton ) ;
	
	// 인챈트창
	m_pEnchantButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pEnchantButton->SetLeft( 11 ) ;
	m_pEnchantButton->SetTop( 343 ) ;
	m_pEnchantButton->SetCaption( CRYLStringTable::m_strString[ 1945 ] ) ;
	m_pEnchantButton->SetFontColor( 0xffffffff) ;
	m_pEnchantButton->SetColor( 0xffffffff) ;
	m_pEnchantButton->OnClickEvent = DlgClick ;
	m_pEnchantButton->SetInstance( RYLSTATUSPANEL::ENCHANT_BUTTON ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pEnchantButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1820 ], 1820, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pEnchantButton ) ;

	// 길드 마크
	m_pGuildMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pGuildMarkSprite->SetLeft( 213 );
	m_pGuildMarkSprite->SetTop( 29 );
	m_pGuildMarkSprite->SetWidth( 12 );
	m_pGuildMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pGuildMarkSprite );
}

VOID	CRYLStatusPanel::FinalCtrl() 
{
	GM_DELETE( m_pNameLabel ) ;							// 이름	
	GM_DELETE( m_pNameValueLabel ) ;					// 이름 값
	GM_DELETE( m_pGuildLabel ) ;						// 길드
	GM_DELETE( m_pGuildValueLabel ) ;					// 길드 값
	GM_DELETE( m_pClassLabel ) ;						// 클레스
	GM_DELETE( m_pClassValueLabel ) ;					// 클레스 값
	GM_DELETE( m_pMarkLabek ) ;							// 마크
	GM_DELETE( m_pLevelLabel ) ;						// 레벨
	GM_DELETE( m_pLevelValueLabel ) ;					// 레벨 값
	GM_DELETE( m_pExperienceLabel ) ;					// 경험점
	GM_DELETE( m_pExperienceBalueLabel ) ;				// 경험점 값

	GM_DELETE( m_pNationLabel );						// 국적
	GM_DELETE( m_pNationValueLabel );					// 국적값

	GM_DELETE( m_pGuildMarkSprite );					// 길드 마크
}


HRESULT		CRYLStatusPanel::Update()
{
	static char strString[MAX_PATH];
	static INT iLeft = 160, iRight = 242;
	CRYLGameData *pGame = CRYLGameData::Instance() ;

	if(!pGame)
		return S_OK;

	// 이름 값
	m_pNameValueLabel->SetCaption( pGame->m_csStatus.m_Info.Name ) ;
	
	// 길드 값
	if ( 0 != pGame->m_csStatus.m_Info.GID )
	{
		CRYLGameScene*			pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*		pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*		pPanel		= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		LPGUILDLARGEINFOEX		lpGuildLargeInfo = pPanel->FindGuild( pGame->m_csStatus.m_Info.GID ) ;

		if ( lpGuildLargeInfo )	m_pGuildValueLabel->SetCaption( lpGuildLargeInfo->m_szName ) ;

		// 길드 마크 출력
		CRYLSprite* pSprite = pPanel->GetGuildSprite( pGame->m_csStatus.m_Info.GID );
		if ( pSprite )
		{
			if(m_pGuildMarkSprite)
			{
				m_pGuildMarkSprite->SetVisible( TRUE );
				m_pGuildMarkSprite->AttachImageTexture( pSprite );
			}
		}
		else
		{
			if(m_pGuildMarkSprite)
				m_pGuildMarkSprite->SetVisible( FALSE );
		}
	}
	else
	{
		m_pGuildValueLabel->SetCaption( "" ) ;
		m_pGuildMarkSprite->SetVisible( FALSE );
	}

	// 국가 값
	if ( Creature::MERKADIA == pGame->m_cAccountNation )
	{
		m_pNationValueLabel->SetCaption( "MK" ) ;//was MK
		m_pNationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2286 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::KARTERANT == pGame->m_cAccountNation )
	{
		m_pNationValueLabel->SetCaption( "KR" ) ;//was KR
		m_pNationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2285 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::ALMIGHTY_PIRATE == pGame->m_cAccountNation )	// 신의 해적단
	{
		m_pNationValueLabel->SetCaption( "AP" ) ;
		m_pNationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2287 ], 1812, TOOLTIP_PANEL ) ;
	}

	// 클레스 값
	m_pClassValueLabel->SetCaption( pGame->GetClassName( pGame->m_csStatus.m_Info.Class ) ) ;
		
	// 작위 값
	int nFame = pGame->m_csStatus.m_Info.Fame ;
	int nFameNameField = 0;

	if( 1 <= nFame && nFame <= 2000 )					nFameNameField = 1;
	else if( 2001 <= nFame && nFame <= 10000 )			nFameNameField = 2;
	else if( 10001 <= nFame && nFame <= 25000 )			nFameNameField = 3;
	else if( 25001 <= nFame && nFame <= 50000 )			nFameNameField = 4;
	else if( 50001 <= nFame )							nFameNameField = 5;
	
	int nClassInfo = 0;		

	switch( pGame->m_csStatus.m_Info.Class )
	{	
		// Defender
		case 5:			nClassInfo = 1;			break;
		// Warrior	
		case 6:			nClassInfo = 2;			break;		
		// Assasin
		case 7:			nClassInfo = 3;			break;
		// Archer
		case 8:			nClassInfo = 4;			break;		
		// Sorcerer	
		case 9:			nClassInfo = 5;			break;
		// Enchanter
		case 10:		nClassInfo = 6;			break;
		// Priest
		case 11:		nClassInfo = 7;			break;
		// Cleric
		case 12:		nClassInfo = 8;			break;			
		// Templer
		case 19:		nClassInfo = 9;			break;
		// Attacker
		case 20:		nClassInfo = 10;		break;
		// Gunner
		case 21:		nClassInfo = 11;		break;		
		// Rune Ope	
		case 22:		nClassInfo = 12;		break;
		// Life Ope	
		case 23:		nClassInfo = 13;		break;
		// Shadow Ope
		case 24:		nClassInfo = 14;		break;
	}

	

	// 레벨 값
	sprintf( strString, "%3d", pGame->m_csStatus.m_Info.Level ) ;
	m_pLevelValueLabel->SetCaption( strString ) ;

	unsigned long dwExp, dwExpMax;
	char strExp[2], strExpMax[2];
	if(1000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000;
		strcpy(strExp, "K");
	} else if(1000000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000000;
		strcpy(strExp, "M");
	} else if(1000000000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000000000;
		strcpy(strExp, "G");
	} else
	{
		dwExp = pGame->m_csStatus.m_Info.Exp;
		strcpy(strExp, "");
	}

	if(1000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000;
		strcpy(strExpMax, "K");
	} else if(1000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000;
		strcpy(strExpMax, "M");
	} else if(1000000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000000;
		strcpy(strExpMax, "G");
	} else
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax;
		strcpy(strExpMax, "");
	}

	sprintf(strString, "%8d%s / %8d%s(%4.2f%%)", dwExp, strExp, dwExpMax, strExpMax, (float)((double)pGame->m_csStatus.m_Info.Exp / (double)pGame->m_csStatus.m_llExpMax * 100.0f));

	m_pExperienceBalueLabel->SetCaption( strString ) ;


	return S_OK ;
}

VOID	CRYLStatusPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pStatusPanel )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLSTATUSPANEL::RANKING_BUTTON	:
		{
			if ( FALSE == ( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->GetVisible() )
			{
				( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->SetVisible( TRUE ) ;
				SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 0, 0 ) ;//hz added this for automatic all rank
			}
			else if ( TRUE == ( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->GetVisible() )//i added this hz
			{
				( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetRankingDlg()->SetVisible( FALSE ) ;
			}
			else
			{
				SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 0, 0 ) ;
			}
		}
		break ;

			case RYLSTATUSPANEL::HP_MP_BUTTON :
		{
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::GetCurrentScene() ) ;
			if ( pScene )
			{
				pScene->GetStatusDlg()->SetVisible( !pScene->GetStatusDlg()->GetVisible() ) ;

				// Visible Flag Setting
				pScene->SetVisibleInfo( RYLMAINWINDOW::STATUS_WINDOW, pScene->GetStatusDlg()->GetVisible() ) ;
			}
		}
		break ;

	case RYLSTATUSPANEL::ENCHANT_BUTTON :
		{
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::GetCurrentScene() ) ;
			if ( pScene )
			{
				pScene->GetEnchantDlg()->SetVisible( !pScene->GetEnchantDlg()->GetVisible() ) ;

				pScene->SetVisibleInfo( RYLMAINWINDOW::ENCHANT_WINDOW, pScene->GetEnchantDlg()->GetVisible() ) ;
			}
		}
		break ;
	}
}
