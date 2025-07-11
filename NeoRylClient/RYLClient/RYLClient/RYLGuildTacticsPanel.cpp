
#include <winsock2.h>

#include "RYLGuildTacticsPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLGameData.h"
#include "RYLRaceBase.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLStringTable.h"
#include "GMUIScreen.h"

#include "RYLStringGrid.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "RYLListBox.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLChattingDlg.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLMessageBox.h"

#include "RYLGameTimeManager.h"

#include <Network/ClientSocket/Send/SendCommunity.h>

#include <Community/Guild/GuildConstants.h>
#include "GMMemory.h"


CRYLGuildTacticsPanel*	g_pGuildTacticsPanel = NULL ;

CRYLGuildTacticsPanel::CRYLGuildTacticsPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pGuildTacticsListLabel			= NULL ;		// 길드원 리스트
	m_pSortMethodLabel				= NULL ;		// 정렬방식
	m_pSortMethodValueLabel 		= NULL ;		// 정렬방식값
	m_pSortPrevButton 				= NULL ;		// 정렬방식 이전 방식
	m_pSortNextButton 				= NULL ;		// 정렬방식 다음 방식

	m_pRankLabel 					= NULL ;		// 서열
	m_pClassLabel 					= NULL ;		// 클레스
	m_pLevelLabel					= NULL ;		// 레벨
	m_pFameLabel					= NULL ;		// 명성
	m_pClassLabel					= NULL ;		// 클래스

	m_pTacticsNameLabel				= NULL ;		// 용병 이름
	m_pTacticsNameValueLabel		= NULL ;		// 용병 이름 값
	m_pTacticsEntryButton 			= NULL ;		// 용병 허가 버튼
	m_pTacticsLeaveButton			= NULL ;		// 용병 퇴출/탈퇴 버튼

	m_pTacticsStringGrid			= NULL ;		// 길드 용병 리스트

	m_dwStartTacticsInfo			= 0 ;
	m_dwServerPage					= 0 ;
	m_dwMaxTacticsInfo				= 100 ;	
	m_iCurrentLine					= 0 ;
	m_iTotalTacticsNum				= 0;


	strcpy( m_szSortMethodName[ 0 ], CRYLStringTable::m_strString[ 1144 ] ) ;		// 이름순
	strcpy( m_szSortMethodName[ 1 ], CRYLStringTable::m_strString[ 1142 ] ) ;		// 레벨순
	strcpy( m_szSortMethodName[ 2 ], CRYLStringTable::m_strString[ 1143 ] ) ;		// 명성순
	strcpy( m_szSortMethodName[ 3 ], CRYLStringTable::m_strString[ 1141 ] ) ;		// 클래스순
	strcpy( m_szSortMethodName[ 4 ], CRYLStringTable::m_strString[ 3981 ] ) ;		// 신청중인 용병
	strcpy( m_szSortMethodName[ 5 ], CRYLStringTable::m_strString[ 3982 ] ) ;		// 활동중인 용병

	m_iSortMethod[ 0 ] = Guild::MEMBER_NAME ;		// 이름순
	m_iSortMethod[ 1 ] = Guild::MEMBER_LEVEL ;		// 레벨순
	m_iSortMethod[ 2 ] = Guild::MEMBER_FAME ;		// 명성순
	m_iSortMethod[ 3 ] = Guild::MEMBER_CLASS ;		// 클래스순
	m_iSortMethod[ 4 ] = Guild::TACTICS_JOIN_WAIT ;	// 신청중인 용병
	m_iSortMethod[ 5 ] = Guild::TACTICS_ACTIVE ;		// 활동중인 용병

	strcpy( m_szTacticsTitle[ 0 ], " " ) ;
	strcpy( m_szTacticsTitle[ 1 ], CRYLStringTable::m_strString[ 1148 ] ) ;		// 길드 마스터
	strcpy( m_szTacticsTitle[ 2 ], CRYLStringTable::m_strString[ 1149 ] ) ;		// 중간 관리자
	strcpy( m_szTacticsTitle[ 3 ], CRYLStringTable::m_strString[ 1150 ] ) ;		// 일반 길드원
	strcpy( m_szTacticsTitle[ 4 ], CRYLStringTable::m_strString[ 1151 ] ) ;		// 탈퇴 대기자
	strcpy( m_szTacticsTitle[ 5 ], CRYLStringTable::m_strString[ 1152 ] ) ;		// 가입 대기자

	strcpy( m_szTacticsClass[ 0 ], " " ) ;
	strcpy( m_szTacticsClass[ 1 ], CRYLStringTable::m_strString[ 451 ] ) ;
	strcpy( m_szTacticsClass[ 2 ], CRYLStringTable::m_strString[ 452 ] ) ;
	strcpy( m_szTacticsClass[ 3 ], CRYLStringTable::m_strString[ 453 ] ) ;
	strcpy( m_szTacticsClass[ 4 ], CRYLStringTable::m_strString[ 454 ] ) ;
	strcpy( m_szTacticsClass[ 5 ], CRYLStringTable::m_strString[ 455 ] ) ;
	strcpy( m_szTacticsClass[ 6 ], CRYLStringTable::m_strString[ 456 ] ) ;
	strcpy( m_szTacticsClass[ 7 ], CRYLStringTable::m_strString[ 457 ] ) ;
	strcpy( m_szTacticsClass[ 8 ], CRYLStringTable::m_strString[ 458 ] ) ;
	strcpy( m_szTacticsClass[ 9 ], CRYLStringTable::m_strString[ 459 ] ) ;
	strcpy( m_szTacticsClass[ 10 ], CRYLStringTable::m_strString[ 460 ] ) ;
	strcpy( m_szTacticsClass[ 11 ], CRYLStringTable::m_strString[ 461 ] ) ;
	strcpy( m_szTacticsClass[ 12 ], CRYLStringTable::m_strString[ 462 ] ) ;
	strcpy( m_szTacticsClass[ 13 ], CRYLStringTable::m_strString[ 463 ] ) ;
	strcpy( m_szTacticsClass[ 14 ], CRYLStringTable::m_strString[ 464 ] ) ;
	strcpy( m_szTacticsClass[ 15 ], CRYLStringTable::m_strString[ 465 ] ) ;
	strcpy( m_szTacticsClass[ 16 ], CRYLStringTable::m_strString[ 466 ] ) ;
	strcpy( m_szTacticsClass[ 17 ], CRYLStringTable::m_strString[ 467 ] ) ;
	strcpy( m_szTacticsClass[ 18 ], CRYLStringTable::m_strString[ 468 ] ) ;
	strcpy( m_szTacticsClass[ 19 ], CRYLStringTable::m_strString[ 469 ] ) ;
	strcpy( m_szTacticsClass[ 20 ], CRYLStringTable::m_strString[ 470 ] ) ;
	strcpy( m_szTacticsClass[ 21 ], CRYLStringTable::m_strString[ 471 ] ) ;
	strcpy( m_szTacticsClass[ 22 ], CRYLStringTable::m_strString[ 472 ] ) ;
	strcpy( m_szTacticsClass[ 23 ], CRYLStringTable::m_strString[ 473 ] ) ;
	strcpy( m_szTacticsClass[ 24 ], CRYLStringTable::m_strString[ 474 ] ) ;

	m_szCurrentName[ 0 ] = '\0' ;

	m_iCurrentSort	= 0 ;
	m_iPageState	= 0 ;
	m_iCurrentSel	= 0 ;

	m_dwRstEntry			= 0L ;
	m_dwRstLeaveForChina	= 0L ;
	m_dwRstLeave			= 0L ;

	m_pTacticsInfo	= NULL ;

	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 328, 375, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 73 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 256, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	}

	SetRect( &rt, 0, 43, 256, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, (i-1) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 256, (i-1) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 7,  27, 313, 190, 0xffB99E6B, 0xc0000000,  1 ) ;		// 금색 테두리 위
	pImageDIB->MakeFrameRect( 7, 238, 313, 108, 0xffB99E6B, 0xc0000000,  1 ) ;		// 금색 테두리 아래

	pImageDIB->MakeFrameRect( 13, 50,  304,  1, 0xff959595, 0xc0000000,  1 ) ;		// 한줄 라인

	pImageDIB->MakeFrameRect( 199,   4, 106,  19, 0xff959595, 0xc0000000,  1 ) ;	// 정렬 방식
	pImageDIB->MakeFrameRect( 128, 261, 123,  19, 0xff959595, 0xc0000000,  1 ) ;	// 용병 이름

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 328 ) ;
	SetHeight( 375 ) ;

	g_pGuildTacticsPanel = this ;
}

CRYLGuildTacticsPanel::~CRYLGuildTacticsPanel() 
{
	ClearList() ;
	FinalCtrl() ;
}


VOID	CRYLGuildTacticsPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 용병 리스트
	m_pGuildTacticsListLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildTacticsListLabel->SetLeft( 6 ) ;
	m_pGuildTacticsListLabel->SetTop( 8 ) ; 
	m_pGuildTacticsListLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 11, 8, 140, 19 ) ;
	m_pGuildTacticsListLabel->SetAlignRect( rt ) ;
	m_pGuildTacticsListLabel->SetAlign( LEFT ) ;
	m_pGuildTacticsListLabel->SetCaption( CRYLStringTable::m_strString[ 2050 ] ) ;
	m_pGuildTacticsListLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildTacticsListLabel ) ;

	// 정렬 방식
	m_pSortMethodLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSortMethodLabel->SetLeft( 149 ) ;
	m_pSortMethodLabel->SetTop( 8 ) ;
	m_pSortMethodLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 133, 8, 196, 19 ) ;
	m_pSortMethodLabel->SetAlignRect( rt ) ;
	m_pSortMethodLabel->SetAlign( RIGHT ) ;
	m_pSortMethodLabel->SetCaption( CRYLStringTable::m_strString[ 2051 ] ) ;
	m_pSortMethodLabel->SetAutoSize( TRUE ) ; 
	CGMUICustomPanel::AddChild( m_pSortMethodLabel ) ;

	// 정렬방식값
	m_pSortMethodValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSortMethodValueLabel->SetLeft( 202 ) ;
	m_pSortMethodValueLabel->SetTop( 8 ) ;
	m_pSortMethodValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 199, 8, 304, 19 ) ;
	m_pSortMethodValueLabel->SetAlignRect( rt ) ;
	m_pSortMethodValueLabel->SetAlign( CENTER ) ;
	m_pSortMethodValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSortMethodValueLabel ) ;

	// 정렬방식 이전 방식
	m_pSortPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pSortPrevButton->SetLeft( 307 ) ;
	m_pSortPrevButton->SetTop( 1 ) ;
	m_pSortPrevButton->OnClickEvent = DlgClick ;
	m_pSortPrevButton->SetInstance( RYLGUILDTACTICSPANEL::SORTPREV_BUTTON ) ;
	SetRect( &rt, 199, 187, 211, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 187, 225, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 187, 239, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 187, 211, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pSortPrevButton ) ;

	// 정렬방식 다음 방식
	m_pSortNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pSortNextButton->SetLeft( 307 ) ;
	m_pSortNextButton->SetTop( 14 ) ;
	m_pSortNextButton->OnClickEvent = DlgClick ;
	m_pSortNextButton->SetInstance( RYLGUILDTACTICSPANEL::SORTNEXT_BUTTON ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 201, 225, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 201, 239, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pSortNextButton ) ;

	// 멤버 이전 페이지
	m_pPagePrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPagePrevButton->SetLeft( 292 ) ;
	m_pPagePrevButton->SetTop( 220 ) ;
	m_pPagePrevButton->OnClickEvent = DlgClick ;
	m_pPagePrevButton->SetInstance( RYLGUILDTACTICSPANEL::PAGEPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPagePrevButton ) ;

	// 멤버 다음 페이지
	m_pPageNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPageNextButton->SetLeft( 307 ) ;
	m_pPageNextButton->SetTop( 220 ) ;
	m_pPageNextButton->OnClickEvent = DlgClick ;
	m_pPageNextButton->SetInstance( RYLGUILDTACTICSPANEL::PAGENEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPageNextButton ) ;

	// 서열
	m_pRankLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pRankLabel->SetLeft( 20 ) ;
	m_pRankLabel->SetTop( 34 ) ;
	m_pRankLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 13, 34, 41, 45 ) ;
	m_pRankLabel->SetAlignRect( rt ) ;
	m_pRankLabel->SetAlign( CENTER ) ;
	m_pRankLabel->SetCaption( CRYLStringTable::m_strString[ 2013 ] ) ;
	m_pRankLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pRankLabel ) ;

	// 이름
	m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel->SetLeft( 81 ) ;
	m_pNameLabel->SetTop( 34 ) ;
	m_pNameLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 41, 34, 134, 45 ) ;
	m_pNameLabel->SetAlignRect( rt ) ;
	m_pNameLabel->SetAlign( CENTER ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	m_pNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel ) ;

	// 레벨
	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel->SetLeft( 150 ) ;
	m_pLevelLabel->SetTop( 34 ) ;
	m_pLevelLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 117, 34, 193, 45 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( CENTER ) ;
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel ) ;

	// 명성
	m_pFameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pFameLabel->SetLeft( 207 ) ;
	m_pFameLabel->SetTop( 34 ) ;
	m_pFameLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 193, 34, 230, 45 ) ;
	m_pFameLabel->SetAlignRect( rt ) ;
	m_pFameLabel->SetAlign( CENTER ) ;
	m_pFameLabel->SetCaption( CRYLStringTable::m_strString[1932] ) ;
	m_pFameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pFameLabel ) ;

	// 클래스
	m_pClassLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabel->SetLeft( 260 ) ;
	m_pClassLabel->SetTop( 34 ) ;
	m_pClassLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 247, 34, 315, 45 ) ;
	m_pClassLabel->SetAlignRect( rt ) ;
	m_pClassLabel->SetAlign( CENTER ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pClassLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 용병 이름
	m_pTacticsNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pTacticsNameLabel->SetLeft( 76 ) ;
	m_pTacticsNameLabel->SetTop( 265 ) ;
	m_pTacticsNameLabel->SetFontColor( 0xffffffff ) ;
	m_pTacticsNameLabel->SetCaption( CRYLStringTable::m_strString[3983] ) ;
	m_pTacticsNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pTacticsNameLabel ) ;

	// 용병 이름 값
	m_pTacticsNameValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pTacticsNameValueLabel->SetLeft( 0 ) ;
	m_pTacticsNameValueLabel->SetTop( 0 ) ;
	m_pTacticsNameValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 128, 265, 250, 276 ) ;
	m_pTacticsNameValueLabel->SetAlignRect( rt ) ;
	m_pTacticsNameValueLabel->SetAlign( CENTER ) ;
	m_pTacticsNameValueLabel->SetCaption( "" ) ;
	m_pTacticsNameValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pTacticsNameValueLabel ) ;

	// 용병 허가
	m_pTacticsEntryButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pTacticsEntryButton->SetLeft( 91 ) ;
	m_pTacticsEntryButton->SetTop( 298 ) ;
	m_pTacticsEntryButton->SetCaption( CRYLStringTable::m_strString[3984] ) ;
	m_pTacticsEntryButton->SetFontColor( 0xffffffff) ;
	m_pTacticsEntryButton->SetColor( 0xffffffff) ;
	m_pTacticsEntryButton->OnClickEvent = DlgClick ;
	m_pTacticsEntryButton->SetInstance( RYLGUILDTACTICSPANEL::TACTICSENTRY_BUTTON ) ;
	m_pTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pTacticsEntryButton ) ;

	// 용병 퇴출 / 탈퇴
	m_pTacticsLeaveButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pTacticsLeaveButton->SetLeft( 170 ) ;
	m_pTacticsLeaveButton->SetTop( 298 ) ;
	m_pTacticsLeaveButton->SetCaption( CRYLStringTable::m_strString[3985] ) ;
	m_pTacticsLeaveButton->SetFontColor( 0xffffffff) ;
	m_pTacticsLeaveButton->SetColor( 0xffffffff) ;
	m_pTacticsLeaveButton->OnClickEvent = DlgClick ;
	m_pTacticsLeaveButton->SetInstance( RYLGUILDTACTICSPANEL::TACTICSLEAVE_BUTTON ) ;
	m_pTacticsLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pTacticsLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pTacticsLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pTacticsLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pTacticsLeaveButton ) ;


	// 용병 리스트
	m_pTacticsStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pTacticsStringGrid->SetLeft( 13 ) ;
	m_pTacticsStringGrid->SetTop( 59 ) ;
	m_pTacticsStringGrid->SetWidth( 292 ) ;
	m_pTacticsStringGrid->SetHeight( 154 ) ;
	m_pTacticsStringGrid->SetColCount( 10 ) ;
	m_pTacticsStringGrid->SetFontColor( 0xffffffff ) ;
	m_pTacticsStringGrid->SetInstance( RYLGUILDTACTICSPANEL::TACTICS_STRINGGRID ) ;
	m_pTacticsStringGrid->OnClickEvent = DlgClick ;
	m_pTacticsStringGrid->OnMouseUpEvent = DlgMouseUp ;
	CGMUICustomPanel::AddChild( m_pTacticsStringGrid ) ;
}

VOID	CRYLGuildTacticsPanel::FinalCtrl() 
{
	GM_DELETE( m_pGuildTacticsListLabel ) ; 			// 길드원 리스트
	GM_DELETE( m_pSortMethodLabel ) ;				// 정렬방식
	GM_DELETE( m_pSortMethodValueLabel ) ; 			// 정렬방식값
	GM_DELETE( m_pSortPrevButton ) ;				// 정렬방식 이전 방식
	GM_DELETE( m_pSortNextButton ) ;				// 정렬방식 다음 방식
	GM_DELETE( m_pPagePrevButton ) ;				// 멤버 이전 페이지
	GM_DELETE( m_pPageNextButton ) ;				// 멤버 다음 페이지

	GM_DELETE( m_pRankLabel ) ; 					// 서열
	GM_DELETE( m_pNameLabel ) ; 					// 이름
	GM_DELETE( m_pLevelLabel ) ;					// 레벨
	GM_DELETE( m_pFameLabel ) ;						// 명성
	GM_DELETE( m_pClassLabel ) ;					// 클래스

	GM_DELETE( m_pTacticsNameLabel ) ;				// 용병 이름
	GM_DELETE( m_pTacticsNameValueLabel ) ;			// 용병 이름 값
	GM_DELETE( m_pTacticsEntryButton ) ;			// 용병 허가 버튼
	GM_DELETE( m_pTacticsLeaveButton ) ;			// 용병 퇴출/탈퇴 버튼

	GM_DELETE( m_pTacticsStringGrid ) ;				// 길드 용병 리스트
}


HRESULT		CRYLGuildTacticsPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	m_pSortMethodValueLabel->SetCaption( m_szSortMethodName[ m_iCurrentSort ] ) ;

/*
	// 길드 탈퇴 메시지 박스 (중국용)
	if ( m_dwRstLeaveForChina & MB_YES )
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		if (pGame->m_csStatus.m_Info.Fame < Guild::FAME_LEAVE_PENALTY)
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[1165]);
		}
		else
		{
			CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
			SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, 
				pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, PktGuildCmd::GC_KICK ) ;

			m_pGuildLeaveButton->SetVisible( FALSE ) ;
			m_pGuildLeaveCanelButton->SetVisible( TRUE ) ;		
		}

		m_dwRstLeaveForChina = 0L ;
	}

	// 길드 탈퇴 메시지 박스 (일반적인 경우)
	if ( m_dwRstLeaveForChina & MB_NO || m_dwRstLeave & MB_YES )
	{
		CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

		if ( pDlg->GetTitle() == Guild::JOIN_WAIT )
		{
			SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, PktGuildCmd::GC_KICK ) ;
		}
		else
		{
			SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, pNetwork->m_dwMyChrID, Guild::LEAVE_WAIT, PktGuildCmd::GC_SETUP_TITLE ) ;
		}

		m_dwRstLeave = 0L ;
		m_dwRstLeaveForChina = 0L ;

		m_pGuildLeaveButton->SetVisible( FALSE ) ;
		m_pGuildLeaveCanelButton->SetVisible( TRUE ) ;
	}

	// 길드 탈퇴 취소 메시지 박스
	if ( m_dwRstLeaveCancel & MB_YES )
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

		SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, pNetwork->m_dwMyChrID, Guild::COMMON, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwRstLeaveCancel = 0L ;

		m_pGuildLeaveButton->SetVisible( TRUE ) ;
		m_pGuildLeaveCanelButton->SetVisible( FALSE ) ;
	}
*/

	return S_OK ;
}

HRESULT		CRYLGuildTacticsPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLGuildTacticsPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

// 정렬 방식 페이지 버튼
VOID	CRYLGuildTacticsPanel::SortMethodPrev()
{
	if ( m_iCurrentSort == 0 )
		m_iCurrentSort = GUILDTACTICSSORT::MAX_SORT_METHOD ;
	else
		-- m_iCurrentSort ;

	m_dwServerPage = 0 ;
	m_dwStartTacticsInfo  = 0 ;
	RequestGuildTacticsList() ;
}

VOID	CRYLGuildTacticsPanel::SortMethodNext() 
{
	if ( m_iCurrentSort == GUILDTACTICSSORT::MAX_SORT_METHOD )
		m_iCurrentSort = 0 ;
	else
		++ m_iCurrentSort ;

	m_dwServerPage = 0 ;
	m_dwStartTacticsInfo  = 0 ;
	RequestGuildTacticsList() ;
}

// 길드 리스트 페이지 버튼
VOID	CRYLGuildTacticsPanel::TacticsListPrev() 
{
	m_iPageState = GUILDTACTICSPAGEEX::PAGE_PREV ;
	RequestGuildTacticsList() ;
}

VOID	CRYLGuildTacticsPanel::TacticsListNext() 
{
	m_iPageState = GUILDTACTICSPAGEEX::PAGE_NEXT ;
	RequestGuildTacticsList() ;
}

VOID	CRYLGuildTacticsPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pGuildTacticsPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLGUILDTACTICSPANEL::SORTPREV_BUTTON :
		g_pGuildTacticsPanel->SortMethodPrev() ;
		break ;

	case RYLGUILDTACTICSPANEL::SORTNEXT_BUTTON :
		g_pGuildTacticsPanel->SortMethodNext() ;
		break ;

	case RYLGUILDTACTICSPANEL::PAGEPREV_BUTTON :
		g_pGuildTacticsPanel->TacticsListPrev() ;
		break ;

	case RYLGUILDTACTICSPANEL::PAGENEXT_BUTTON :
		g_pGuildTacticsPanel->TacticsListNext() ;
		break ;

	case RYLGUILDTACTICSPANEL::TACTICSENTRY_BUTTON :
		g_pGuildTacticsPanel->TacticsEntryButtonEvent() ; 
		break ;

	case RYLGUILDTACTICSPANEL::TACTICSLEAVE_BUTTON :
		g_pGuildTacticsPanel->TacticsLeaveButtonEvent() ;
		break ;

	case RYLGUILDTACTICSPANEL::TACTICS_STRINGGRID :
		g_pGuildTacticsPanel->SelectTacticsInfo() ;
		break ;
	}
}

VOID	CRYLGuildTacticsPanel::TacticsEntryButtonEvent() 
{
	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
	
	UCHAR ucTitle = pDlg->GetTitle() ;

	if(ucTitle>=Guild::MASTER && ucTitle<=Guild::MIDDLE_ADMIN)
	{		
		CRYLGameData* pGame = CRYLGameData::Instance();
		
		INT iIndex = m_pTacticsStringGrid->GetItemIndex() ;

		if( iIndex == -1 || m_vecTacticsInfo.empty() )
		{
			return;
		}

		GuildMemberInfoNode* pMember = m_vecTacticsInfo[ iIndex ] ;

		if(pMember)
		{
			if(pMember->m_cTactics==Guild::TACTICS_WAIT)
			{
				SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, pMember->m_dwCID, Guild::COMMON, PktGuildCmd::GC_TACTICS_TITLE ) ;
			}			
		}		
	}
	else
	{
		CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3892], CRYLNetworkData::Instance()->m_dwMyChrID, 128);		
	}
}

VOID	CRYLGuildTacticsPanel::TacticsLeaveButtonEvent() 
{
	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;

	UCHAR ucTitle = pDlg->GetTitle() ;

	if(CRYLGameTimeManager::Instance()->IsGuildWarTime())
	{
		CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3906], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
	}
	else
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		INT iIndex = m_pTacticsStringGrid->GetItemIndex() ;

		if( iIndex == -1 || m_vecTacticsInfo.empty() )
		{
			return;
		}

		GuildMemberInfoNode* pMember = m_vecTacticsInfo[ iIndex ] ;

		if(ucTitle>=Guild::MASTER && ucTitle<=Guild::MIDDLE_ADMIN)
		{
			if(pMember)
			{
				// 용병 퇴출.
				SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, CRYLNetworkData::Instance()->m_dwMyChrID, pMember->m_dwCID, PktGuildCmd::GC_TACTICS_KICK ) ;
			}
		}
		else
		{
			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

			if(pSelfCreature)
			{
				if(pSelfCreature->GetTactics())
				{
					if(pMember)
					{
						if(pMember->m_dwCID!=CRYLNetworkData::Instance()->m_dwMyChrID)
						{
							CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3907], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						}
						else
						{
							// 용병 탈퇴.
							SendPacket::CharGuildCmd( g_GameSession, pGame->m_csStatus.m_Info.GID, CRYLNetworkData::Instance()->m_dwMyChrID, pMember->m_dwCID, PktGuildCmd::GC_TACTICS_LEAVE ) ;
						}
					}
				}
				else
				{
					CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3907], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}
			}
		}
	}	
}

VOID	CRYLGuildTacticsPanel::DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pGuildTacticsPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLGUILDTACTICSPANEL::TACTICS_STRINGGRID :
		{
//			if ( Button == MB_RIGHTCLICK )
//				g_pGuildTacticsPanel->ShowGuildMenuPopup( X, Y ) ;
		}
		break ;
	}
}


HRESULT		CRYLGuildTacticsPanel::UpdateGuildTacticsPanel() 
{
	RequestGuildTacticsList() ;	
/*
	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
	UCHAR ucTitle = pDlg->GetTitle() ;

	switch (ucTitle)
	{
	case Guild::JOIN_WAIT:
		{
			m_pRightSettingButton->SetVisible( TRUE ) ;			// 권한 설정
			m_pRightSettingButton->SetEnable( FALSE ) ;			

			m_pGuildLeaveButton->SetVisible( TRUE ) ;			// 길드 탈퇴	
			m_pGuildLeaveButton->SetEnable( FALSE ) ;			

			m_pGuildLeaveCanelButton->SetVisible( FALSE ) ;		// 길드 탈퇴 취소
			m_pGuildLeaveCanelButton->SetEnable( FALSE ) ;		

			m_pGuildWarJoin->SetEnable( FALSE ) ;				// 길드 전쟁 참여

			m_pGuildWarLeave->SetEnable( FALSE ) ;				// 길드 전쟁 불참

			break;
		}

	case Guild::LEAVE_WAIT:
		{
			m_pRightSettingButton->SetVisible( TRUE ) ;			// 권한 설정
			m_pRightSettingButton->SetEnable( TRUE ) ;			

			m_pGuildLeaveButton->SetVisible( FALSE ) ;			// 길드 탈퇴	
			m_pGuildLeaveButton->SetEnable( FALSE ) ;			

			m_pGuildLeaveCanelButton->SetVisible( TRUE ) ;		// 길드 탈퇴 취소
			m_pGuildLeaveCanelButton->SetEnable( TRUE ) ;		

			m_pGuildWarJoin->SetEnable( FALSE ) ;				// 길드 전쟁 참여

			m_pGuildWarLeave->SetEnable( FALSE ) ;				// 길드 전쟁 불참

			break;
		}

	default:
		{
			m_pRightSettingButton->SetVisible( TRUE ) ;			// 권한 설정
			m_pRightSettingButton->SetEnable( TRUE ) ;			

			// 길드 탈퇴는 길드전이 시작하기 전에만 가능하다.
			if ( !CRYLGameTimeManager::Instance()->IsGuildWarTime() )
			{
				m_pGuildLeaveButton->SetVisible( TRUE ) ;		// 길드 탈퇴	
				m_pGuildLeaveButton->SetEnable( TRUE ) ;			
			}
			else
			{
				m_pGuildLeaveButton->SetVisible( TRUE ) ;		// 길드 탈퇴
				m_pGuildLeaveButton->SetEnable( FALSE ) ;			
			}

			m_pGuildLeaveCanelButton->SetVisible( FALSE ) ;		// 길드 탈퇴 취소
			m_pGuildLeaveCanelButton->SetEnable( FALSE ) ;

			m_pGuildWarJoin->SetEnable( FALSE ) ;				// 길드 전쟁 참여

			m_pGuildWarLeave->SetEnable( FALSE ) ;				// 길드 전쟁 불참

			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );
			if ( pSelfCreature )
			{
				if ( Creature::WAR_OFF == pSelfCreature->GetGuildWarFlag() )
				{
					m_pGuildWarJoin->SetEnable( TRUE ) ;
				}
				else if ( !CRYLGameTimeManager::Instance()->IsGuildWarTime() )
				{
					m_pGuildWarLeave->SetEnable( TRUE ) ;
				}
			}

			break;
		}
	}
*/
	return S_OK ;
}

std::vector< GuildMemberInfoNode* >::iterator		CRYLGuildTacticsPanel::FindGuildTacticsIter( GuildMemberInfoNode* pTacticsInfo ) 
{
	std::vector< GuildMemberInfoNode* >::iterator	iter = m_vecTacticsInfo.begin() ;

	while( iter != m_vecTacticsInfo.end() )
	{
		if ( ( *iter ) == pTacticsInfo )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecTacticsInfo.end() ;
}

std::vector< GuildMemberInfoNode* >::iterator		CRYLGuildTacticsPanel::FindGuildTacticsIter( DWORD dwCID ) 
{
	std::vector< GuildMemberInfoNode* >::iterator	iter = m_vecTacticsInfo.begin() ;

	while( iter != m_vecTacticsInfo.end() )
	{
		if ( ( *iter )->m_dwCID == dwCID )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecTacticsInfo.end() ;
}

void	CRYLGuildTacticsPanel::ClearList(void)
{
	if ( !m_mapTacticsInfo.empty() ) 
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.begin() ;

		while ( iter != m_mapTacticsInfo.end() )
		{
			delete iter->second ;
			++ iter ;
		}

		m_mapTacticsInfo.clear() ;
	}

	if ( !m_vecTacticsInfo.empty() ) 
	{
		m_vecTacticsInfo.clear() ;
	}
}

VOID	CRYLGuildTacticsPanel::RequestGuildTacticsList()
{
	INT iRequestPage = GetCurrentPageLine( m_iPageState ) ;
	m_iPageState = GUILDTACTICSPAGEEX::PAGE_CURRENT;

	ClearList() ;
	SendPacket::CharGuildMemberList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Guild::TYPE_TACTICS, m_iSortMethod[ m_iCurrentSort ], m_dwServerPage / 10 ) ;
}


VOID	CRYLGuildTacticsPanel::ProcessGuildTacticsList( unsigned char cSortCmd, 
													    unsigned char cPage, 
													    unsigned char cNodeNum, 
													    unsigned char cTotalTacticsNum,
													    GuildMemberInfoNode* lpTacticsNode )
{
	m_iCurrentSort = FindSortIndex( cSortCmd ) ;
	m_dwServerPage = cPage *  10 ;

	m_iTotalTacticsNum	= cTotalTacticsNum - 1;

	m_vecTacticsInfo.clear();

	// 요청한 페이지를 Map에 Update한다.
	for ( int i = 0 ; i < cNodeNum ; i ++ )
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( lpTacticsNode[ i ].m_dwCID ) ;

		if ( iter != m_mapTacticsInfo.end() )
		{
			*iter->second = lpTacticsNode[ i ] ;
		}
		else
		{
			GuildMemberInfoNode* pTactics = NULL ;
			pTactics = new GuildMemberInfoNode ;

			memcpy( pTactics, &lpTacticsNode[ i ], sizeof( GuildMemberInfoNode ) ) ;
			m_mapTacticsInfo.insert( pair< INT, GuildMemberInfoNode* >( pTactics->m_dwCID, pTactics ) ) ;
		}
	}

	// Map의 정보를 Vector에 갱신한다.
	for (int nIndex = 0; nIndex < cNodeNum ; ++nIndex )
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( lpTacticsNode[ nIndex ].m_dwCID ) ;

		if ( iter != m_mapTacticsInfo.end() )
		{
			std::vector< GuildMemberInfoNode* >::iterator   iter1 = FindGuildTacticsIter( lpTacticsNode[ nIndex ].m_dwCID ) ;
			if ( cSortCmd == 7 )
			{
				if ( iter->second->m_cTitle == Guild::JOIN_WAIT )
				{
					m_vecTacticsInfo.push_back( iter->second ) ;
				}
				else 
					continue ;
			}
			else
			{
				m_vecTacticsInfo.push_back( iter->second ) ;
			}
		}
	}	

	UpdateGuildStringGrid() ;
}

INT		CRYLGuildTacticsPanel::GetCurrentPageLine( INT bPageState )
{
	if ( m_vecTacticsInfo.empty() ) { return 0 ; }

	switch ( bPageState )
	{
	case GUILDTACTICSPAGEEX::PAGE_NEXT :
		{
			m_dwServerPage += 10 ;

			// 마지막 페이지 보정
			if ( static_cast<long>(m_dwServerPage) > m_iTotalTacticsNum )	
			{
				m_dwServerPage = 0 ;
			}
			break ;
		}

	case GUILDTACTICSPAGEEX::PAGE_PREV :
		{
			if ( m_dwServerPage >= 10 )
				m_dwServerPage -= 10 ;
			else							// 순환
				m_dwServerPage = m_iTotalTacticsNum;
			break ;
		}

	case GUILDTACTICSPAGEEX::PAGE_CURRENT :
		{
			return m_vecTacticsInfo.size() ;
		}

	default :
		{
			return 0 ;
		}
	}

	return m_vecTacticsInfo.size();
}

INT		CRYLGuildTacticsPanel::GetTitle( DWORD dwCID )
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		return iter->second->m_cTitle;
	}

	return -1;
}

VOID	CRYLGuildTacticsPanel::SetTactics( DWORD dwCID, unsigned char cTacticsFlag )
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		iter->second->m_cTactics = cTacticsFlag;
	}	
}

unsigned char CRYLGuildTacticsPanel::GetTactics(DWORD dwCID)
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		return iter->second->m_cTactics;
	}	

	return 0;
}

VOID	CRYLGuildTacticsPanel::UpdateGuildTacticsRight( DWORD dwCID, UCHAR ucTitle ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		iter->second->m_cTitle = ucTitle ;
	}

	// 만약 내 자신이 갱신되었을경우 CHumanGuildAuthorty의 Title을 갱신	
	if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
	{
		CRYLAuthoritySettingDlg* pAuthoritySettingDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
		pAuthoritySettingDlg->SetTitle( ucTitle ) ;
		pAuthoritySettingDlg->SetAuthorityValue( pAuthoritySettingDlg->GetGuildRight(), ucTitle );

		// 공성 관련 팝업창이 떠 있었다면 닫아준다.
		CRYLGameData* pGame = CRYLGameData::Instance();
		if ( Creature::CT_SIEGE_OBJECT == pGame->GetClientIdentity(pGame->m_lpInterface->m_dwPopupSelectChrID) )
		{
			pGame->m_lpInterface->m_bPopupShow = FALSE;
			pGame->m_lpInterface->m_dwPopupSelect = 0xFFFFFFFF;
		}

		// 요새 업글, 수리, 변형, 월드 웨폰 장전 다이얼로그를 닫는다.
		CRYLGameScene*	 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		pScene->GetCampUpgradeDlg()->SetVisible( FALSE ) ;
		pScene->GetCampChangeDlg()->SetVisible( FALSE ) ;
		pScene->GetCampRepairDlg()->SetVisible( FALSE ) ;
		pScene->GetWorldWeaponLoadDlg()->SetVisible( FALSE ) ;
	}
}

unsigned char CRYLGuildTacticsPanel::GetClass( DWORD dwCID ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		return iter->second->m_cClass;
	}

	return 0;
}

unsigned char CRYLGuildTacticsPanel::GetLevel( DWORD dwCID ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapTacticsInfo.find( dwCID ) ;

	if ( iter != m_mapTacticsInfo.end() )
	{
		return iter->second->m_cLevel;
	}

	return 1;
}


HRESULT		CRYLGuildTacticsPanel::UpdateGuildStringGrid() 
{
	m_pTacticsStringGrid->ClearItem() ;

	m_dwMaxTacticsInfo = m_vecTacticsInfo.size() ;	
	std::vector< GuildMemberInfoNode* >::iterator it;

	if ( !m_vecTacticsInfo.empty() )
	{
		int cnt;
		cnt = GetCurrentPageLine( GUILDTACTICSPAGEEX::PAGE_CURRENT );
		for ( int i = 0; i < cnt ; i ++ )
		{
			it = m_vecTacticsInfo.begin() + i;

			unsigned long dwColor = D3DCOLOR_RGBA( 255, 255, 255, 255);

			if(( *it )->m_cTactics==Guild::TACTICS_WAIT)
			{
				dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
			}

			// 서열
			CHAR Buff[ 64 ] ;
			sprintf( Buff, "%d", i + 1 ) ;
			m_pTacticsStringGrid->AddItem( 0, Buff, 28, dwColor) ;

			// 이름
			m_pTacticsStringGrid->AddItem( 1, (*it)->m_szName, 93, dwColor) ;

			// 레벨
			sprintf( Buff, "%d", (*it)->m_cLevel ) ;
			m_pTacticsStringGrid->AddItem( 2, Buff, 37, dwColor) ;

			// 명성
			sprintf( Buff, "%d", (*it)->m_dwFame ) ;
			m_pTacticsStringGrid->AddItem( 3, Buff, 76, dwColor) ;

			// 클래스
			m_pTacticsStringGrid->AddItem( 4, m_szTacticsClass[ (*it)->m_cClass ], 58, dwColor) ;
		}
	}

	return S_OK ;
}

/*
VOID	CRYLGuildTacticsPanel::ShowGuildMenuPopup(INT iX, INT iY ) 
{	
	iY -= 50 ;
	INT iIndex = m_pTacticsStringGrid->GetItemIndex() ;
	if ( iIndex == -1 || true == m_vecTacticsInfo.empty() ) { return ; }

	GuildMemberInfoNode* pTactics = m_vecTacticsInfo[ iIndex ] ;

	if ( pTactics )
	{
		m_pTacticsInfo = pTactics ;
		if ( m_iCurrentSort == 7 )
		{
			CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;

			UCHAR ucTitle = pDlg->GetTitle() ;
			GuildRight* pRight = pDlg->GetGuildRight() ;

			if ( ucTitle == Guild::JOIN_WAIT || ucTitle == Guild::LEAVE_WAIT ||  pTactics->m_cTitle == Guild::COMMON )
			{
				return ;
			}

			if ( ucTitle == Guild::MASTER ) 
			{
				m_pProposalGuildPopup->SetVisible( TRUE  ) ;
				m_pProposalGuildPopup->SetLeft( iX ) ;
				m_pProposalGuildPopup->SetTop( iY ) ;
				ProposalGuildEnableEvent() ;
				return ;
			}

			if ( ucTitle == Guild::MIDDLE_ADMIN )
			{
				if ( pRight->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::MASTER )
				{
					return ;
				}
				if ( ( pRight->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::MIDDLE_ADMIN ) ||
					( pRight->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::COMMON ) )
				{
					m_pProposalGuildPopup->SetVisible( TRUE  ) ;
					m_pProposalGuildPopup->SetLeft( iX ) ;
					m_pProposalGuildPopup->SetTop( iY ) ;
					ProposalGuildEnableEvent() ;	
					return ;
				}
			}

			if ( ucTitle == Guild::COMMON )
			{
				if ( pRight->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::COMMON ) 
				{
					m_pProposalGuildPopup->SetVisible( TRUE  ) ;
					m_pProposalGuildPopup->SetLeft( iX ) ;
					m_pProposalGuildPopup->SetTop( iY ) ;
					ProposalGuildEnableEvent() ;
					return ;
				}
			}
		}
		else
		{
			m_pGeneralGuildPopup->SetVisible( TRUE  ) ;
			m_pGeneralGuildPopup->SetLeft( iX ) ;
			m_pGeneralGuildPopup->SetTop( iY ) ;
			GeneralGuildEnableEvent( pTactics->m_cTitle ) ;
			return ;
		}
	}
}
*/

VOID		CRYLGuildTacticsPanel::SelectTacticsInfo() 
{
/*
	if ( m_pProposalGuildPopup->GetVisible() )
	{
		m_pProposalGuildPopup->SetVisible( FALSE ) ;
	}

	if ( m_pGeneralGuildPopup->GetVisible() )
	{
		m_pGeneralGuildPopup->SetVisible( FALSE ) ;
	}
*/
	INT iIndex = m_pTacticsStringGrid->GetItemIndex() ;
	if ( iIndex == -1 )		
	{
		return ;
	}

	if ( static_cast<size_t>(iIndex) < m_vecTacticsInfo.size() )
	{
		GuildMemberInfoNode* pTactics = m_vecTacticsInfo[ iIndex ] ;	
		if ( pTactics )
		{
			m_pTacticsInfo = pTactics ;

			// 용병 이름 값
			m_pTacticsNameValueLabel->SetCaption( m_pTacticsInfo->m_szName ) ;
		}
	}
}

VOID  CRYLGuildTacticsPanel::Hide( CGMUIObject* Sender )
{
//	if ( m_pProposalGuildPopup->GetVisible() )
//		m_pProposalGuildPopup->SetVisible( FALSE ) ;

//	if ( m_pGeneralGuildPopup->GetVisible() )
//		m_pGeneralGuildPopup->SetVisible( FALSE ) ;

	CGMUICustomPanel::Hide( Sender );

	return;
}

INT	CRYLGuildTacticsPanel::FindSortIndex( unsigned char cSortCmd )
{
	for ( int iIdx=0; iIdx<=GUILDTACTICSSORT::MAX_SORT_METHOD; ++iIdx )
	{
		if ( m_iSortMethod[ iIdx ] == cSortCmd )
		{
			return iIdx ;
		}
	}

	return 0 ;
}