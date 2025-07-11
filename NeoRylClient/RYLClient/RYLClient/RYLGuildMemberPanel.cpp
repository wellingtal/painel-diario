
#include <winsock2.h>

#include "RYLGuildMemberPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLEdit.h"
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

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include <GameTime/GameTimeConstants.h>
#include "RYLGameTimeManager.h"
#include "GMMemory.h"


CRYLGuildMemberPanel*	g_pGuildMemberPanel = NULL ;

CRYLGuildMemberPanel::CRYLGuildMemberPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pGuildMemberListLabel			= NULL ;		// 길드원 리스트
	m_pSortMethodLabel				= NULL ;		// 정렬방식
	m_pSortMethodValueLabel 		= NULL ;		// 정렬방식값
	m_pSortPrevButton 				= NULL ;		// 정렬방식 이전 방식
	m_pSortNextButton 				= NULL ;		// 정렬방식 다음 방식
	
	m_pRankLabel 					= NULL ;		// 서열
	m_pClassLabel 					= NULL ;		// 클레스
	m_pTitleLabel 					= NULL ;		// 직위
	m_pLevelLabel					= NULL ;		// 레벨
	m_pClassLabel					= NULL ;		// 클래스
	
	m_pCurrentFameLabel 			= NULL ;		// 현재 명성
	m_pCurrentFameValueLabel 		= NULL ;		// 현재 명성 값
	m_pCurrentGoldLabel 			= NULL ;		// 소지 금액
	m_pCurrentValueGoldLabel 		= NULL ;		// 소지 금액 값
	m_pCurrentPositionLabel 		= NULL ;		// 현재 위치
	m_pCurrentPositionValueLabel	= NULL ;		// 현재 위치 값
	m_pCurrentGuildWarFlagLabel		= NULL ;		// 길드 전쟁 참여
	m_pCurrentGuildWarFlagValueLabel = NULL ;		// 길드 전챙 참여 값

	m_pRightSettingButton 			= NULL ;		// 권한 설정
	m_pGuildLeaveButton 			= NULL ;		// 길드 탈퇴
	m_pGuildLeaveCanelButton		= NULL ;		// 길드 탈퇴 취소
	m_pGuildWarJoin					= NULL ;		// 길드 전쟁 참여
	m_pGuildWarLeave				= NULL ;		// 길드 전쟁 불참

	m_dwStartMemberInfo				= 0 ;
	m_dwServerPage					= 0 ;
	m_dwMaxMemberInfo				= 100 ;	
	m_iCurrentLine					= 0 ;
	m_iTotalMemberNum				= 0;

	for ( int i = 0 ; i < 7 ; i ++ )
		m_dwResultMessageBox[ i ] = 0L ;

	for (int i = 0 ; i < 2 ; i ++ )
		m_dwResultProposalMessageBox[ i ] = 0L ;

	strcpy( m_szSortMethodName[ 0 ], CRYLStringTable::m_strString[ 1140 ] ) ;		// 서열
	strcpy( m_szSortMethodName[ 1 ], CRYLStringTable::m_strString[ 1141 ] ) ;		// 클래스순
	strcpy( m_szSortMethodName[ 2 ], CRYLStringTable::m_strString[ 1142 ] ) ;		// 레벨순
	strcpy( m_szSortMethodName[ 3 ], CRYLStringTable::m_strString[ 1143 ] ) ;		// 명성순
	strcpy( m_szSortMethodName[ 4 ], CRYLStringTable::m_strString[ 1144 ] ) ;		// 이름순
	strcpy( m_szSortMethodName[ 5 ], CRYLStringTable::m_strString[ 1145 ] ) ;		// 소지금액순
	strcpy( m_szSortMethodName[ 6 ], CRYLStringTable::m_strString[ 1146 ] ) ;		// 위치별
	strcpy( m_szSortMethodName[ 7 ], CRYLStringTable::m_strString[ 1147 ] ) ;		// 신청중인 플레이어
	strcpy( m_szSortMethodName[ 8 ], CRYLStringTable::m_strString[ 3965 ] ) ;		// 전쟁 참여중인 플레이어
	strcpy( m_szSortMethodName[ 9 ], CRYLStringTable::m_strString[ 3966 ] ) ;		// 전쟁 불참중인 플레이어

	m_iSortMethod[ 0 ] = Guild::MEMBER_TITLE ;			// 직위순
	m_iSortMethod[ 1 ] = Guild::MEMBER_CLASS ;			// 클래스별 소트
	m_iSortMethod[ 2 ] = Guild::MEMBER_LEVEL ;			// 레벨별 소트
	m_iSortMethod[ 3 ] = Guild::MEMBER_FAME ;			// 명성별 소트
	m_iSortMethod[ 4 ] = Guild::MEMBER_NAME ;			// 이름별 소트
	m_iSortMethod[ 5 ] = Guild::MEMBER_GOLD ;			// 소지금액별 소트
	m_iSortMethod[ 6 ] = Guild::MEMBER_POSITION ;		// 위치별 소트
	m_iSortMethod[ 7 ] = Guild::MEMBER_JOIN_WAIT ;		// 신청중인 플레이어
	m_iSortMethod[ 8 ] = Guild::MEMBER_WAR_ON ;			// 길드 전쟁 참여중인 멤버 (이름순으로 소트)
	m_iSortMethod[ 9 ] = Guild::MEMBER_WAR_OFF ;		// 길드 전쟁 불참중인 멤버 (이름순으로 소트)
	
	strcpy( m_szMembertitle[ 0 ], " " ) ;
	strcpy( m_szMembertitle[ 1 ], CRYLStringTable::m_strString[ 1148 ] ) ;		// 길드 마스터
	strcpy( m_szMembertitle[ 2 ], CRYLStringTable::m_strString[ 1149 ] ) ;		// 중간 관리자
	strcpy( m_szMembertitle[ 3 ], CRYLStringTable::m_strString[ 1150 ] ) ;		// 일반 길드원
	strcpy( m_szMembertitle[ 4 ], CRYLStringTable::m_strString[ 1151 ] ) ;		// 탈퇴 대기자
	strcpy( m_szMembertitle[ 5 ], CRYLStringTable::m_strString[ 1152 ] ) ;		// 가입 대기자

	
	strcpy( m_szGeneralGuild[ 0 ], CRYLStringTable::m_strString[ 2040 ] ) ;		// 길드 마스터 권한양도
	strcpy( m_szGeneralGuild[ 1 ], CRYLStringTable::m_strString[ 2041 ] ) ;		// 중간관리자 임명
	strcpy( m_szGeneralGuild[ 2 ], CRYLStringTable::m_strString[ 2042 ] ) ;		// 중간관리자 해임
	strcpy( m_szGeneralGuild[ 3 ], CRYLStringTable::m_strString[ 2043 ] ) ;		// 중간관리자 포기				
	strcpy( m_szGeneralGuild[ 4 ], CRYLStringTable::m_strString[ 2044 ] ) ;		// 길드에서 축출
	strcpy( m_szGeneralGuild[ 5 ], CRYLStringTable::m_strString[ 2047 ] ) ;		// 귓말 보내기

	strcpy( m_szMemberClass[ 0 ], " " ) ;
	strcpy( m_szMemberClass[ 1 ], CRYLStringTable::m_strString[ 451 ] ) ;
	strcpy( m_szMemberClass[ 2 ], CRYLStringTable::m_strString[ 452 ] ) ;
	strcpy( m_szMemberClass[ 3 ], CRYLStringTable::m_strString[ 453 ] ) ;
	strcpy( m_szMemberClass[ 4 ], CRYLStringTable::m_strString[ 454 ] ) ;
	strcpy( m_szMemberClass[ 5 ], CRYLStringTable::m_strString[ 455 ] ) ;
	strcpy( m_szMemberClass[ 6 ], CRYLStringTable::m_strString[ 456 ] ) ;
	strcpy( m_szMemberClass[ 7 ], CRYLStringTable::m_strString[ 457 ] ) ;
	strcpy( m_szMemberClass[ 8 ], CRYLStringTable::m_strString[ 458 ] ) ;
	strcpy( m_szMemberClass[ 9 ], CRYLStringTable::m_strString[ 459 ] ) ;
	strcpy( m_szMemberClass[ 10 ], CRYLStringTable::m_strString[ 460 ] ) ;
	strcpy( m_szMemberClass[ 11 ], CRYLStringTable::m_strString[ 461 ] ) ;
	strcpy( m_szMemberClass[ 12 ], CRYLStringTable::m_strString[ 462 ] ) ;
	strcpy( m_szMemberClass[ 13 ], CRYLStringTable::m_strString[ 463 ] ) ;
	strcpy( m_szMemberClass[ 14 ], CRYLStringTable::m_strString[ 464 ] ) ;
	strcpy( m_szMemberClass[ 15 ], CRYLStringTable::m_strString[ 465 ] ) ;
	strcpy( m_szMemberClass[ 16 ], CRYLStringTable::m_strString[ 466 ] ) ;
	strcpy( m_szMemberClass[ 17 ], CRYLStringTable::m_strString[ 467 ] ) ;
	strcpy( m_szMemberClass[ 18 ], CRYLStringTable::m_strString[ 468 ] ) ;
	strcpy( m_szMemberClass[ 19 ], CRYLStringTable::m_strString[ 469 ] ) ;
	strcpy( m_szMemberClass[ 20 ], CRYLStringTable::m_strString[ 470 ] ) ;
	strcpy( m_szMemberClass[ 21 ], CRYLStringTable::m_strString[ 471 ] ) ;
	strcpy( m_szMemberClass[ 22 ], CRYLStringTable::m_strString[ 472 ] ) ;
	strcpy( m_szMemberClass[ 23 ], CRYLStringTable::m_strString[ 473 ] ) ;
	strcpy( m_szMemberClass[ 24 ], CRYLStringTable::m_strString[ 474 ] ) ;

	strcpy( m_szCurrentPos[ 0 ], CRYLStringTable::m_strString[ 1153 ] ) ;
	strcpy( m_szCurrentPos[ 1 ], CRYLStringTable::m_strString[ 1154 ] ) ;
	strcpy( m_szCurrentPos[ 2 ], CRYLStringTable::m_strString[ 1155 ] ) ;
	strcpy( m_szCurrentPos[ 3 ], CRYLStringTable::m_strString[ 1156 ] ) ;
	strcpy( m_szCurrentPos[ 4 ], CRYLStringTable::m_strString[ 1157 ] ) ;
	strcpy( m_szCurrentPos[ 5 ], CRYLStringTable::m_strString[ 1158 ] ) ; 
	strcpy( m_szCurrentPos[ 6 ], CRYLStringTable::m_strString[ 1159 ] ) ;
	strcpy( m_szCurrentPos[ 7 ],  " " ) ;
	strcpy( m_szCurrentPos[ 8 ], CRYLStringTable::m_strString[ 1160 ] ) ;
	strcpy( m_szCurrentPos[ 9 ], CRYLStringTable::m_strString[ 1166 ] ) ;
	strcpy( m_szCurrentPos[ 10 ], " " ) ;
	strcpy( m_szCurrentPos[ 11 ], " " ) ;
	strcpy( m_szCurrentPos[ 12 ], CRYLStringTable::m_strString[ 2734 ] ) ;
	strcpy( m_szCurrentPos[ 13 ], " " ) ;
	strcpy( m_szCurrentPos[ 14 ], " " ) ;
	strcpy( m_szCurrentPos[ 15 ], " " ) ;
	strcpy( m_szCurrentPos[ 16 ], CRYLStringTable::m_strString[ 1156 ] ) ;
	strcpy( m_szCurrentPos[ 17 ], CRYLStringTable::m_strString[ 1156 ] ) ;
	strcpy( m_szCurrentPos[ 18 ], CRYLStringTable::m_strString[ 1156 ] ) ;
	

	m_szCurrentName[ 0 ] = '\0' ;
	m_szCurrentFarm[ 0 ] = '\0' ;
	m_szCurrentGold[ 0 ] = '\0' ;
	m_iCurrentPos		 = -1 ;

	m_iCurrentSort	= 0 ;
	m_iPageState	= 0 ;
	m_iCurrentSel	= 0 ;

	m_dwRstLeave			= 0L ;
	m_dwRstLeaveForChina	= 0L ;
	m_dwRstLeaveCancel		= 0L ;

	m_pMemberInfo	= NULL ;

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
	
	pImageDIB->MakeFrameRect( 7,  27, 313, 190, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7, 238, 313, 108, 0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 199, 4,  106,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 13, 50,  304,  1, 0xff959595, 0xc0000000,  1 ) ;
    pImageDIB->MakeFrameRect( 74, 250,  83,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 222, 250,  83,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 280,  83,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 312,  83,  19, 0xff959595, 0xc0000000,  1 ) ;
		
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 328 ) ;
	SetHeight( 375 ) ;

	g_pGuildMemberPanel = this ;
}

CRYLGuildMemberPanel::~CRYLGuildMemberPanel() 
{
	ClearList() ;
	FinalCtrl() ;
}


VOID	CRYLGuildMemberPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pGeneralGuildPopup = new CRYLListBox( m_pGMUIParentDlg, FALSE ) ;
	m_pGeneralGuildPopup->SetLeft( 0 ) ;
	m_pGeneralGuildPopup->SetTop( 0 ) ;
	m_pGeneralGuildPopup->SetWidth( 130 ) ;
	m_pGeneralGuildPopup->SetHeight( 116 ) ;

	m_pGeneralGuildPopup->SetItemMerginX( 5 ) ;
    m_pGeneralGuildPopup->SetItemMerginY( 10 ) ;
	m_pGeneralGuildPopup->SetItemHeight( 16 ) ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 130, 116, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xff646464 ) ;
	pImageDIB->MakeFrameRect( 3, 3 , 124, 110, 0xff959595, 0xff646464,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	m_pGeneralGuildPopup->AttachListBoxImage( ( CGMTexture*)pImage ) ;
	m_pGeneralGuildPopup->OnClickEvent = DlgClick ;
	m_pGeneralGuildPopup->SetInstance( RYLGUILDMEMBERPANEL::GENERALGUILD_POPUP ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2040 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2041 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2042 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2043 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2044 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->AddItem( CRYLStringTable::m_strString[ 2047 ], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
	m_pGeneralGuildPopup->SetVisible( FALSE ) ;

	CGMUICustomPanel::AddChild( m_pGeneralGuildPopup ) ;

	// 신청길드원 팝업 메뉴
	m_pProposalGuildPopup = new CRYLListBox( m_pGMUIParentDlg, FALSE ) ;
	m_pProposalGuildPopup->SetLeft( 3 ) ;
	m_pProposalGuildPopup->SetTop( 32 ) ;
	m_pProposalGuildPopup->SetWidth( 80 ) ;
	m_pProposalGuildPopup->SetHeight( 36 ) ;

	m_pProposalGuildPopup->SetItemMerginX( 18 ) ;
    m_pProposalGuildPopup->SetItemMerginY( 5 ) ;
	m_pProposalGuildPopup->SetItemHeight( 14 ) ;
	
    CRYLSpriteList::Instance()->CreateGMImage( 80, 36, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xff646464 ) ;
	pImageDIB->MakeFrameRect( 3, 3, 74, 30, 0xff959595, 0xff646464,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	m_pProposalGuildPopup->AttachListBoxImage( ( CGMTexture*)pImage ) ;
	m_pProposalGuildPopup->OnClickEvent = DlgClick ;
	m_pProposalGuildPopup->SetInstance( RYLGUILDMEMBERPANEL::PROPOSALGUILD_POPUP ) ;
	m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2048 ], 0xffffffff, 0xff00ff00, 0xffff0000 ) ;
	m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2049 ], 0xffffffff, 0xff00ff00, 0xffff0000 ) ;
	m_pProposalGuildPopup->SetVisible( FALSE ) ;

	CGMUICustomPanel::AddChild( m_pProposalGuildPopup ) ;

	// 길드원 리스트
	m_pGuildMemberListLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildMemberListLabel->SetLeft( 6 ) ;
	m_pGuildMemberListLabel->SetTop( 8 ) ; 
	m_pGuildMemberListLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 11, 8, 140, 19 ) ;
	m_pGuildMemberListLabel->SetAlignRect( rt ) ;
	m_pGuildMemberListLabel->SetAlign( LEFT ) ;
	m_pGuildMemberListLabel->SetCaption( CRYLStringTable::m_strString[ 2050 ] ) ;
	m_pGuildMemberListLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildMemberListLabel ) ;

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
	m_pSortPrevButton->SetInstance( RYLGUILDMEMBERPANEL::SORTPREV_BUTTON ) ;
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
	m_pSortNextButton->SetInstance( RYLGUILDMEMBERPANEL::SORTNEXT_BUTTON ) ;
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
	m_pPagePrevButton->SetInstance( RYLGUILDMEMBERPANEL::PAGEPREV_BUTTON ) ;
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
	m_pPageNextButton->SetInstance( RYLGUILDMEMBERPANEL::PAGENEXT_BUTTON ) ;
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

	// 직위
	m_pTitleLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pTitleLabel->SetLeft( 150 ) ;
	m_pTitleLabel->SetTop( 34 ) ;
	m_pTitleLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 134, 34, 210, 45 ) ;
	m_pTitleLabel->SetAlignRect( rt ) ;
	m_pTitleLabel->SetAlign( CENTER ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 2052 ] ) ;
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pTitleLabel ) ;

	// 레벨
	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel->SetLeft( 207 ) ;
	m_pLevelLabel->SetTop( 34 ) ;
	m_pLevelLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 210, 34, 247, 45 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( CENTER ) ;
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel ) ;

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

	// 현재 명성
	m_pCurrentFameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentFameLabel->SetLeft( 22 ) ;
	m_pCurrentFameLabel->SetTop( 254 ) ;
	m_pCurrentFameLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentFameLabel->SetCaption( CRYLStringTable::m_strString[ 2055 ] ) ;
	m_pCurrentFameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentFameLabel ) ;

	// 현재 명성 값
	m_pCurrentFameValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentFameValueLabel->SetLeft( 75 ) ;
	m_pCurrentFameValueLabel->SetTop( 254 ) ;
	m_pCurrentFameValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 75, 254, 155, 265 ) ;
	m_pCurrentFameValueLabel->SetAlignRect( rt ) ;
	m_pCurrentFameValueLabel->SetAlign( CENTER ) ;
	m_pCurrentFameValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentFameValueLabel ) ;

	// 소지 금액
	m_pCurrentGoldLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGoldLabel->SetLeft( 22 ) ;
	m_pCurrentGoldLabel->SetTop( 284 ) ;
	m_pCurrentGoldLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentGoldLabel->SetCaption( CRYLStringTable::m_strString[ 1960 ] ) ;
	m_pCurrentGoldLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGoldLabel ) ;

	// 소지 금액 값
	m_pCurrentValueGoldLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentValueGoldLabel->SetLeft( 75 ) ;
	m_pCurrentValueGoldLabel->SetTop( 284 ) ;
	m_pCurrentValueGoldLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 75, 284, 155, 295 ) ;
	m_pCurrentValueGoldLabel->SetAlignRect( rt ) ;
	m_pCurrentValueGoldLabel->SetAlign( CENTER ) ;
	m_pCurrentValueGoldLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentValueGoldLabel ) ;

	// 현재 위치
	m_pCurrentPositionLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentPositionLabel->SetLeft( 22 ) ;
	m_pCurrentPositionLabel->SetTop( 316 ) ;
	m_pCurrentPositionLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentPositionLabel->SetCaption( CRYLStringTable::m_strString[ 2056 ] ) ;
	m_pCurrentPositionLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentPositionLabel ) ;

	// 현재 위치 값
	m_pCurrentPositionValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentPositionValueLabel->SetLeft( 75 ) ;
	m_pCurrentPositionValueLabel->SetTop( 316 ) ;
	m_pCurrentPositionValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 75, 316, 155, 327 ) ;
	m_pCurrentPositionValueLabel->SetAlignRect( rt ) ;
	m_pCurrentPositionValueLabel->SetAlign( CENTER ) ;
	m_pCurrentPositionValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentPositionValueLabel ) ;

	// 길드 전챙 참여
	m_pCurrentGuildWarFlagLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGuildWarFlagLabel->SetLeft( 170 ) ;
	m_pCurrentGuildWarFlagLabel->SetTop( 254 ) ;
	m_pCurrentGuildWarFlagLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentGuildWarFlagLabel->SetCaption( CRYLStringTable::m_strString[3967] ) ;
	m_pCurrentGuildWarFlagLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGuildWarFlagLabel ) ;

	// 길드 전쟁 참여 값
	m_pCurrentGuildWarFlagValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGuildWarFlagValueLabel->SetLeft( 223 ) ;
	m_pCurrentGuildWarFlagValueLabel->SetTop( 254 ) ;
	m_pCurrentGuildWarFlagValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 223, 254, 303, 265 ) ;
	m_pCurrentGuildWarFlagValueLabel->SetAlignRect( rt ) ;
	m_pCurrentGuildWarFlagValueLabel->SetAlign( CENTER ) ;
	m_pCurrentGuildWarFlagValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGuildWarFlagValueLabel ) ;

	// 권한 설정
	m_pRightSettingButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pRightSettingButton->SetLeft( 167 ) ;
	m_pRightSettingButton->SetTop( 309 ) ;
	m_pRightSettingButton->SetCaption( CRYLStringTable::m_strString[ 2057 ] ) ;
	m_pRightSettingButton->SetFontColor( 0xffffffff) ;
	m_pRightSettingButton->SetColor( 0xffffffff) ;
	m_pRightSettingButton->OnClickEvent = DlgClick ;
	m_pRightSettingButton->SetInstance( RYLGUILDMEMBERPANEL::RIGHTSETTING_BUTTON ) ;
	m_pRightSettingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRightSettingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRightSettingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRightSettingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pRightSettingButton ) ;

	// 길드 탈퇴	
	m_pGuildLeaveButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildLeaveButton->SetLeft( 239 ) ;
	m_pGuildLeaveButton->SetTop( 309 ) ;
	m_pGuildLeaveButton->SetCaption( CRYLStringTable::m_strString[ 2058 ] ) ;
	m_pGuildLeaveButton->SetFontColor( 0xffffffff) ;
	m_pGuildLeaveButton->SetColor( 0xffffffff) ;
	m_pGuildLeaveButton->OnClickEvent = DlgClick ;
	m_pGuildLeaveButton->SetInstance( RYLGUILDMEMBERPANEL::GUILDLEAVE_BUTTON ) ;
	m_pGuildLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildLeaveButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildLeaveButton ) ;

	// 길드 탈퇴 취소
	m_pGuildLeaveCanelButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildLeaveCanelButton->SetLeft( 239 ) ;
	m_pGuildLeaveCanelButton->SetTop( 309 ) ;
	m_pGuildLeaveCanelButton->SetCaption( CRYLStringTable::m_strString[ 2059 ] ) ;
	m_pGuildLeaveCanelButton->SetFontColor( 0xffffffff) ;
	m_pGuildLeaveCanelButton->SetColor( 0xffffffff) ;
	m_pGuildLeaveCanelButton->SetVisible( FALSE ) ;
	m_pGuildLeaveCanelButton->OnClickEvent = DlgClick ;
	m_pGuildLeaveCanelButton->SetInstance( RYLGUILDMEMBERPANEL::GUILDLEAVECANCEL_BUTTON ) ;
	m_pGuildLeaveCanelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildLeaveCanelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildLeaveCanelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildLeaveCanelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildLeaveCanelButton ) ;

	// 길드 전쟁 참여
	m_pGuildWarJoin = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildWarJoin->SetLeft( 167 ) ;
	m_pGuildWarJoin->SetTop( 277 ) ;
	m_pGuildWarJoin->SetCaption( CRYLStringTable::m_strString[3968] ) ;
	m_pGuildWarJoin->SetFontColor( 0xffffffff) ;
	m_pGuildWarJoin->SetColor( 0xffffffff) ;
	m_pGuildWarJoin->OnClickEvent = DlgClick ;
	m_pGuildWarJoin->SetInstance( RYLGUILDMEMBERPANEL::GUILDWARJOIN_BUTTON ) ;
	m_pGuildWarJoin->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildWarJoin->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildWarJoin->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildWarJoin->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildWarJoin ) ;

	// 길드 전쟁 불참
	m_pGuildWarLeave = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildWarLeave->SetLeft( 239 ) ;
	m_pGuildWarLeave->SetTop( 277 ) ;
	m_pGuildWarLeave->SetCaption( CRYLStringTable::m_strString[3969] ) ;
	m_pGuildWarLeave->SetFontColor( 0xffffffff) ;
	m_pGuildWarLeave->SetColor( 0xffffffff) ;
	m_pGuildWarLeave->OnClickEvent = DlgClick ;
	m_pGuildWarLeave->SetInstance( RYLGUILDMEMBERPANEL::GUILDWARLEAVE_BUTTON ) ;
	m_pGuildWarLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildWarLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildWarLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildWarLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildWarLeave ) ;

	// 멤버 리스트
	m_pMemberStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pMemberStringGrid->SetLeft( 13 ) ;
	m_pMemberStringGrid->SetTop( 59 ) ;
	m_pMemberStringGrid->SetWidth( 292 ) ;
	m_pMemberStringGrid->SetHeight( 154 ) ;
	m_pMemberStringGrid->SetColCount( 10 ) ;
	m_pMemberStringGrid->SetFontColor( 0xffffffff ) ;
	m_pMemberStringGrid->SetInstance( RYLGUILDMEMBERPANEL::MEMBER_STRINGGRID ) ;
	m_pMemberStringGrid->OnClickEvent = DlgClick ;
	m_pMemberStringGrid->OnMouseUpEvent = DlgMouseUp ;
	CGMUICustomPanel::AddChild( m_pMemberStringGrid ) ;
}

VOID	CRYLGuildMemberPanel::FinalCtrl() 
{
	GM_DELETE( m_pGuildMemberListLabel ) ; 			// 길드원 리스트
	GM_DELETE( m_pSortMethodLabel ) ;				// 정렬방식
	GM_DELETE( m_pSortMethodValueLabel ) ; 			// 정렬방식값
	GM_DELETE( m_pSortPrevButton ) ;				// 정렬방식 이전 방식
	GM_DELETE( m_pSortNextButton ) ;				// 정렬방식 다음 방식
	GM_DELETE( m_pPagePrevButton ) ;				// 멤버 이전 페이지
	GM_DELETE( m_pPageNextButton ) ;				// 멤버 다음 페이지
	
	GM_DELETE( m_pRankLabel ) ; 					// 서열
	GM_DELETE( m_pNameLabel ) ; 					// 이름
	GM_DELETE( m_pTitleLabel ) ;					// 직위
	GM_DELETE( m_pLevelLabel ) ;					// 레벨
	GM_DELETE( m_pClassLabel ) ;					// 클래스
	
	GM_DELETE( m_pCurrentFameLabel ) ;				// 현재 명성
	GM_DELETE( m_pCurrentFameValueLabel ) ; 		// 현재 명성 값
	GM_DELETE( m_pCurrentGoldLabel ) ;				// 소지 금액
	GM_DELETE( m_pCurrentValueGoldLabel ) ; 		// 소지 금액 값
	GM_DELETE( m_pCurrentPositionLabel ) ;			// 현재 위치
	GM_DELETE( m_pCurrentPositionValueLabel ) ;		// 현재 위치 값
	GM_DELETE( m_pCurrentGuildWarFlagLabel ) ;		// 길드 전쟁 참여
	GM_DELETE( m_pCurrentGuildWarFlagValueLabel ) ;	// 길드 전쟁 참여 값

	GM_DELETE( m_pRightSettingButton ) ;			// 권한 설정
	GM_DELETE( m_pGuildLeaveButton ) ;				// 길드 탈퇴	
	GM_DELETE( m_pGuildLeaveCanelButton ) ;			// 길드 탈퇴 취소
	GM_DELETE( m_pGuildWarJoin ) ;					// 길드 전쟁 참여
	GM_DELETE( m_pGuildWarLeave ) ;					// 길드 전쟁 불참

	GM_DELETE( m_pMemberStringGrid ) ;				// 길드 멤버 리스트 

	GM_DELETE( m_pGeneralGuildPopup ) ;				// 일반길드원 팝업 메뉴
	GM_DELETE( m_pProposalGuildPopup ) ;			// 신청길드원 팝업 메뉴
}


HRESULT		CRYLGuildMemberPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	m_pSortMethodValueLabel->SetCaption( m_szSortMethodName[ m_iCurrentSort ] ) ;

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

	ResultGeneralGuildPopup() ;
	ResultProposalGuildPopup() ;
	
	return S_OK ;
}

HRESULT		CRYLGuildMemberPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLGuildMemberPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

// 정렬 방식 페이지 버튼
VOID	CRYLGuildMemberPanel::SortMethodPrev()
{
	if ( m_iCurrentSort == 0 )
		m_iCurrentSort = GUILDMEMBERSORT::MAX_SORT_METHOD ;
	else
		-- m_iCurrentSort ;

	m_dwServerPage = 0 ;
	m_dwStartMemberInfo  = 0 ;
	RequestGuildMemberList() ;
}

VOID	CRYLGuildMemberPanel::SortMethodNext() 
{
	if ( m_iCurrentSort == GUILDMEMBERSORT::MAX_SORT_METHOD )
		m_iCurrentSort = 0 ;
	else
		++ m_iCurrentSort ;

	m_dwServerPage = 0 ;
	m_dwStartMemberInfo  = 0 ;
	RequestGuildMemberList() ;
}

// 길드 리스트 페이지 버튼
VOID	CRYLGuildMemberPanel::MemberListPrev() 
{
	m_iPageState = GUILDMEMBERPAGEEX::PAGE_PREV ;
	RequestGuildMemberList() ;
}

VOID	CRYLGuildMemberPanel::MemberListNext() 
{
	m_iPageState = GUILDMEMBERPAGEEX::PAGE_NEXT ;
	RequestGuildMemberList() ;
}

// 권한 설정
VOID	CRYLGuildMemberPanel::RightSettingButton() 
{
}

VOID	CRYLGuildMemberPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pGuildMemberPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLGUILDMEMBERPANEL::SORTPREV_BUTTON :
			g_pGuildMemberPanel->SortMethodPrev() ;
			break ;

		case RYLGUILDMEMBERPANEL::SORTNEXT_BUTTON :
			g_pGuildMemberPanel->SortMethodNext() ;
			break ;

		case RYLGUILDMEMBERPANEL::PAGEPREV_BUTTON :
			g_pGuildMemberPanel->MemberListPrev() ;
			break ;

		case RYLGUILDMEMBERPANEL::PAGENEXT_BUTTON :
			g_pGuildMemberPanel->MemberListNext() ;
			break ;

		case RYLGUILDMEMBERPANEL::RIGHTSETTING_BUTTON :
		{
			CRYLAuthoritySettingDlg* pAuthoritySettingDlg = 
				( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )g_pGuildMemberPanel->m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;

			pAuthoritySettingDlg->SetAuthorityValue( pAuthoritySettingDlg->GetGuildRight(), pAuthoritySettingDlg->GetTitle() );
			pAuthoritySettingDlg->SetVisible( TRUE );
			break ;
		}

		case RYLGUILDMEMBERPANEL::GUILDLEAVE_BUTTON :
			g_pGuildMemberPanel->GuildLeaveButtonEvent() ; 
			break ;

		case RYLGUILDMEMBERPANEL::GUILDLEAVECANCEL_BUTTON :
			g_pGuildMemberPanel->GuildLeaveCancelButtonEvent() ;
			break ;

		case RYLGUILDMEMBERPANEL::GUILDWARJOIN_BUTTON :
			g_pGuildMemberPanel->GuildWarJoinButtonEvent() ;
			break ;

		case RYLGUILDMEMBERPANEL::GUILDWARLEAVE_BUTTON :
			g_pGuildMemberPanel->GuildWarLeaveButtonEvent() ;
			break ;

		case RYLGUILDMEMBERPANEL::MEMBER_STRINGGRID :
			g_pGuildMemberPanel->SelectMemberInfo() ;
			break ;

		case RYLGUILDMEMBERPANEL::GENERALGUILD_POPUP :
			g_pGuildMemberPanel->ProcessGeneralGuildPopup() ;
			break ;

		case RYLGUILDMEMBERPANEL::PROPOSALGUILD_POPUP :
			g_pGuildMemberPanel->PorcessProposalGuildPopup() ;
			break ;
	}	
}

VOID	CRYLGuildMemberPanel::GuildLeaveButtonEvent() 
{
	CRYLAuthoritySettingDlg* pDlg = 
		( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;

	CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
/*
	if (GameRYL::CHINA == CRYLNetworkData::Instance()->m_eInternationalCode && Guild::MASTER != pDlg->GetTitle())
	{
		lpMessageBox->Create( CRYLStringTable::m_strString[ 1164 ], MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwRstLeaveForChina ) ;
	}
	else
	{
*/
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

		if(pSelfCreature)
		{
			if(pSelfCreature->GetTactics()==Guild::TACTICS || pSelfCreature->GetTactics()==Guild::TACTICS_WAIT)
			{
				lpMessageBox->Create( CRYLStringTable::m_strString[4050]) ;
			}
			else
			{
				lpMessageBox->Create( CRYLStringTable::m_strString[ 1161 ], MB_YES | MB_NO | MB_EXIT ) ;
				lpMessageBox->SetResult( &m_dwRstLeave ) ;
			}
		}		
//	}
}

VOID	CRYLGuildMemberPanel::GuildLeaveCancelButtonEvent() 
{
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
	lpMessageBox->Create( CRYLStringTable::m_strString[ 1162 ], MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwRstLeaveCancel ) ;
}

VOID	CRYLGuildMemberPanel::GuildWarJoinButtonEvent()
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( pSelfCreature && Creature::WAR_OFF == pSelfCreature->GetGuildWarFlag() )
	{
		if ( !CRYLGameTimeManager::Instance()->IsGuildWarTime() )
		{
			SendPacket::WarOnOff( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_ON, GameTime::GUILD ) ;
		}
		else
		{
			CRYLMessageBox *lpConfirmMgsBox = new CRYLMessageBox ;
			lpConfirmMgsBox->SetWordBreak( TRUE ) ;
			lpConfirmMgsBox->Create( CRYLStringTable::m_strString[3970], MB_YES | MB_NO | MB_EXIT ) ;
			lpConfirmMgsBox->SetResult( &CRYLGameData::Instance()->m_dwGuildWarJoinLeaveResult ) ;
			CRYLGameData::Instance()->m_dwGuildWarJoinLeaveResult = 0 ;
		}
	}
}

VOID	CRYLGuildMemberPanel::GuildWarLeaveButtonEvent()
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( pSelfCreature && Creature::WAR_ON == pSelfCreature->GetGuildWarFlag() ) 
	{
		if ( pSelfCreature->GetTactics() == Guild::TACTICS )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4059] ) ;
			return ;
		}

		if ( CRYLGameTimeManager::Instance()->IsGuildWarTime() )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[3971] ) ;
			return ;
		}

		SendPacket::WarOnOff( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_OFF, GameTime::GUILD ) ;
	}
}

VOID	CRYLGuildMemberPanel::DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pGuildMemberPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLGUILDMEMBERPANEL::MEMBER_STRINGGRID :
		{
			if ( Button == MB_RIGHTCLICK )
				g_pGuildMemberPanel->ShowGuildMenuPopup( X, Y ) ;
		}
		break ;
	}
}


HRESULT		CRYLGuildMemberPanel::UpdateGuildMemberPanel() 
{
	RequestGuildMemberList() ;	

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

			break;
		}
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();	

	if ( pSelfCreature && pSelfCreature->GetTactics() == Guild::TACTICS )
	{		
		m_pRightSettingButton->SetVisible( FALSE ) ;	
		m_pGuildLeaveCanelButton->SetEnable( FALSE ) ;
	}

	m_pCurrentFameValueLabel->SetCaption( "" ) ;
	m_pCurrentValueGoldLabel->SetCaption( "" ) ;
	m_pCurrentPositionValueLabel->SetCaption( "" ) ;
	m_pCurrentGuildWarFlagValueLabel->SetCaption( "" ) ;

	return S_OK ;
}

std::vector< GuildMemberInfoNode* >::iterator		CRYLGuildMemberPanel::FindGuildMemberIter( GuildMemberInfoNode* pMemberInfo ) 
{
	std::vector< GuildMemberInfoNode* >::iterator	iter = m_vecMemberInfo.begin() ;

	while( iter != m_vecMemberInfo.end() )
	{
		if ( ( *iter ) == pMemberInfo )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecMemberInfo.end() ;
}

std::vector< GuildMemberInfoNode* >::iterator		CRYLGuildMemberPanel::FindGuildMemberIter( DWORD dwCID ) 
{
	std::vector< GuildMemberInfoNode* >::iterator	iter = m_vecMemberInfo.begin() ;

	while( iter != m_vecMemberInfo.end() )
	{
		if ( ( *iter )->m_dwCID == dwCID )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecMemberInfo.end() ;
}

void	CRYLGuildMemberPanel::ClearList(void)
{
	if ( !m_mapMemberInfo.empty() ) 
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.begin() ;

		while ( iter != m_mapMemberInfo.end() )
		{
			delete iter->second ;
			++ iter ;
		}

		m_mapMemberInfo.clear() ;
	}

	if ( !m_vecMemberInfo.empty() ) 
	{
		m_vecMemberInfo.clear() ;
	}
}

VOID	CRYLGuildMemberPanel::RequestGuildMemberList()
{
	INT iRequestPage = GetCurrentPageLine( m_iPageState ) ;
	m_iPageState = GUILDMEMBERPAGEEX::PAGE_CURRENT;

	ClearList() ;
	SendPacket::CharGuildMemberList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Guild::TYPE_MEMBER, m_iSortMethod[ m_iCurrentSort ], m_dwServerPage / 10 ) ;
}


VOID	CRYLGuildMemberPanel::ProcessGuildMemberList( unsigned char cSortCmd, 
												      unsigned char cPage, 
												      unsigned char cNodeNum, 
													  unsigned char cTotalMemberNum,
												      GuildMemberInfoNode* lpMemberNode )
{
	m_iCurrentSort = FindSortIndex( cSortCmd ) ;
	m_dwServerPage = cPage *  10 ;

	m_iTotalMemberNum	= cTotalMemberNum - 1;

	m_vecMemberInfo.clear();
	
	// 요청한 페이지를 Map에 Update한다.
	for ( int i = 0 ; i < cNodeNum ; i ++ )
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( lpMemberNode[ i ].m_dwCID ) ;

		if ( iter != m_mapMemberInfo.end() )
		{
			*iter->second = lpMemberNode[ i ] ;
		}
		else
		{
			GuildMemberInfoNode* pMember = NULL ;
			pMember = new GuildMemberInfoNode ;

			memcpy( pMember, &lpMemberNode[ i ], sizeof( GuildMemberInfoNode ) ) ;
			m_mapMemberInfo.insert( pair< INT, GuildMemberInfoNode* >( pMember->m_dwCID, pMember ) ) ;
		}
	}

	// Map의 정보를 Vector에 갱신한다.
	for (int nIndex = 0; nIndex < cNodeNum ; ++nIndex )
	{
		std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( lpMemberNode[ nIndex ].m_dwCID ) ;
		
		if ( iter != m_mapMemberInfo.end() )
		{
			std::vector< GuildMemberInfoNode* >::iterator   iter1 = FindGuildMemberIter( lpMemberNode[ nIndex ].m_dwCID ) ;
			if ( cSortCmd == Guild::MEMBER_JOIN_WAIT )
			{
				if ( iter->second->m_cTitle == Guild::JOIN_WAIT )
				{
					m_vecMemberInfo.push_back( iter->second ) ;
				}
				else 
					continue ;
			}
			else
			{
				m_vecMemberInfo.push_back( iter->second ) ;
			}
		}
	}	
	UpdateGuildStringGrid() ;
}

INT		CRYLGuildMemberPanel::GetCurrentPageLine( INT bPageState )
{
	if ( m_vecMemberInfo.empty() ) { return 0 ; }

	switch ( bPageState )
	{
		case GUILDMEMBERPAGEEX::PAGE_NEXT :
		{
			m_dwServerPage += 10 ;

			// 마지막 페이지 보정
			if ( static_cast<long>(m_dwServerPage) > m_iTotalMemberNum )	
			{
				m_dwServerPage = 0 ;
			}
			break ;
		}

		case GUILDMEMBERPAGEEX::PAGE_PREV :
		{
			if ( m_dwServerPage >= 10 )
				m_dwServerPage -= 10 ;
			else							// 순환
				m_dwServerPage = m_iTotalMemberNum;
			break ;
		}

		case GUILDMEMBERPAGEEX::PAGE_CURRENT :
		{
			return m_vecMemberInfo.size() ;
		}
			
		default :
		{
			return 0 ;
		}
	}

	return m_vecMemberInfo.size();
}

unsigned char CRYLGuildMemberPanel::GetClass( DWORD dwCID ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( dwCID ) ;

	if ( iter != m_mapMemberInfo.end() )
	{
		return iter->second->m_cClass;
	}

	return 0;
}

unsigned char CRYLGuildMemberPanel::GetLevel( DWORD dwCID ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( dwCID ) ;

	if ( iter != m_mapMemberInfo.end() )
	{
		return iter->second->m_cLevel;
	}

	return 1;
}

INT		CRYLGuildMemberPanel::GetTitle( DWORD dwCID )
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( dwCID ) ;

	if ( iter != m_mapMemberInfo.end() )
	{
		return iter->second->m_cTitle;
	}

	return -1;
}

VOID	CRYLGuildMemberPanel::UpdateGuildMemberRight( DWORD dwCID, UCHAR ucTitle ) 
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( dwCID ) ;

	if ( iter != m_mapMemberInfo.end() )
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

VOID	CRYLGuildMemberPanel::UpdateGuildWarJoinFlag( DWORD dwCID, UCHAR ucFlag )
{
	std::map< INT, GuildMemberInfoNode* >::iterator		iter = m_mapMemberInfo.find( dwCID ) ;
	if ( iter != m_mapMemberInfo.end() )
	{
		iter->second->m_cGuildWarFlag = ucFlag ;
	}

	switch ( ucFlag )
	{
		case Creature::WAR_ON :		m_pCurrentGuildWarFlagValueLabel->SetCaption( CRYLStringTable::m_strString[3973] ) ;	break ;
		case Creature::WAR_OFF :	m_pCurrentGuildWarFlagValueLabel->SetCaption( CRYLStringTable::m_strString[3972] ) ;	break ;
	}
}

HRESULT		CRYLGuildMemberPanel::UpdateGuildStringGrid() 
{
	m_pMemberStringGrid->ClearItem() ;
	
	m_dwMaxMemberInfo = m_vecMemberInfo.size() ;	
	std::vector< GuildMemberInfoNode* >::iterator it;

	if ( !m_vecMemberInfo.empty() )
	{
		int cnt;
		cnt = GetCurrentPageLine( GUILDMEMBERPAGEEX::PAGE_CURRENT );
		for ( int i = 0; i < cnt ; i ++ )
		{
			it = m_vecMemberInfo.begin() + i;

			unsigned long dwColor = D3DCOLOR_RGBA( 255, 255, 255, 255);			

			// 서열
			CHAR Buff[ 64 ] ;
			sprintf( Buff, "%d", i + 1 ) ;
			m_pMemberStringGrid->AddItem( 0, Buff, 28, dwColor ) ;

			// 이름
			m_pMemberStringGrid->AddItem( 1, (*it)->m_szName, 93, dwColor ) ;
			
			// 직위
			m_pMemberStringGrid->AddItem( 2, m_szMembertitle[ ( *it )->m_cTitle], 76, dwColor ) ;
			
			// 레벨
			sprintf( Buff, "%d", (*it)->m_cLevel ) ;
			m_pMemberStringGrid->AddItem( 3, Buff, 37, dwColor) ;

			// 클래스
			m_pMemberStringGrid->AddItem( 4, m_szMemberClass[ (*it)->m_cClass ], 58, dwColor ) ;
		}
	}
	
	return S_OK ;
}

VOID	CRYLGuildMemberPanel::ShowGuildMenuPopup(INT iX, INT iY ) 
{	
	iY -= 50 ;
	INT iIndex = m_pMemberStringGrid->GetItemIndex() ;
	if ( iIndex == -1 || true == m_vecMemberInfo.empty() ) { return ; }

	GuildMemberInfoNode* pMember = m_vecMemberInfo[ iIndex ] ;
	
	if ( pMember )
	{
		m_pMemberInfo = pMember ;
		if ( m_iSortMethod[ m_iCurrentSort ] == Guild::MEMBER_JOIN_WAIT )
		{
			CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
		
			UCHAR ucTitle = pDlg->GetTitle() ;
			GuildRight* pRight = pDlg->GetGuildRight() ;

			if ( ucTitle == Guild::JOIN_WAIT || ucTitle == Guild::LEAVE_WAIT ||  pMember->m_cTitle == Guild::COMMON )
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
			GeneralGuildEnableEvent( pMember->m_cTitle ) ;
			return ;
		}
	}
}

VOID		CRYLGuildMemberPanel::SelectMemberInfo() 
{
	if ( m_pProposalGuildPopup->GetVisible() )
	{
		m_pProposalGuildPopup->SetVisible( FALSE ) ;
	}

	if ( m_pGeneralGuildPopup->GetVisible() )
	{
		m_pGeneralGuildPopup->SetVisible( FALSE ) ;
	}

	INT iIndex = m_pMemberStringGrid->GetItemIndex() ;
	if ( iIndex == -1 )		
	{
		return ;
	}

	if ( static_cast<size_t>(iIndex) < m_vecMemberInfo.size() )
	{
		GuildMemberInfoNode* pMember = m_vecMemberInfo[ iIndex ] ;	
		if ( pMember )
		{
			m_pMemberInfo = pMember ;

			// 현재 명성 값
			m_pCurrentFameValueLabel->SetCaption( CRYLStringTable::MakePrintGold( pMember->m_dwFame ) ) ;		

			// 소지 금액 값
			m_pCurrentValueGoldLabel->SetCaption( CRYLStringTable::MakePrintGold( pMember->m_dwGold ) ) ;		

			// 현재 위치 값	
			m_iCurrentPos = ( pMember->m_dwServerID & 0xFF000000 ) >> 24 ;
			m_pCurrentPositionValueLabel->SetCaption( m_szCurrentPos[ m_iCurrentPos ] ) ;	

			// 길드 전쟁 참여 값
			if ( Creature::WAR_OFF == m_pMemberInfo->m_cGuildWarFlag )
			{
				m_pCurrentGuildWarFlagValueLabel->SetCaption( CRYLStringTable::m_strString[3972] ) ;
			}
			else
			{
				m_pCurrentGuildWarFlagValueLabel->SetCaption( CRYLStringTable::m_strString[3973] ) ;
			}
		}
	}
}

VOID	CRYLGuildMemberPanel::GeneralGuildEnableEvent( UCHAR ucTitleVal ) 
{
	for ( int i = 0 ; i < 6 ; i ++ )
	{
		m_bGeneralGuild[ i ] = TRUE ;
	}

	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
	UCHAR ucTitle = pDlg->GetTitle() ;
	GuildRight* pGuild = pDlg->GetGuildRight() ;

	// 일반길드원, 탈퇴 대기자, 가입 대기자 들은 모두 귓말 보내기만 가능
	if ( ucTitle == Guild::COMMON || ucTitle == Guild::LEAVE_WAIT || ucTitle == Guild::JOIN_WAIT )
	{
		for ( int i = 0 ;i < 5 ; i ++ )
		{
			m_bGeneralGuild[ i ] = FALSE ;
		}
	}

	if ( ucTitle == Guild::MIDDLE_ADMIN )
	{
		m_bGeneralGuild[ 0 ] = FALSE ;
		m_bGeneralGuild[ 1 ] = FALSE ;
		m_bGeneralGuild[ 2 ] = FALSE ;
		
		if ( ucTitleVal == Guild::MASTER )
		{
			// 선택한것이 길드 마스터이면 귓말 보내기만 가능
			for ( int i = 2 ;i < 5 ; i ++ )
			{
				m_bGeneralGuild[ i ] = FALSE ;
			}
		}

		// 멤버 축출 권한이 없거나 있더라도 자신의 직위 이상의 길드원이라면...
		if ( pGuild->m_aryRight[ GuildRight::KICK_MEMBER ] == Guild::MASTER || ucTitleVal <= ucTitle )
		{
			m_bGeneralGuild[ 4 ] = FALSE ;
		}

		// 자기 자신이 아닌경우에는 중간관리자 비활성
		if ( GetMemberInfo()->m_dwCID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{ 
			m_bGeneralGuild[ 3 ] = TRUE ;
			m_bGeneralGuild[ 4 ] = FALSE ;
		}
		else
		{
			m_bGeneralGuild[ 3 ] = FALSE ;
		}
	}

	if ( ucTitle == Guild::MASTER )
	{
		// 길드 마스터인 자신이, 자신을 선택한 경우
		// 모두 비활성화
		if ( ucTitleVal == Guild::MASTER ) 
		{
			for ( int i = 0 ;i < 6 ; i ++ )
			{
				m_bGeneralGuild[ i ] = FALSE ;
			}
		}

		// 길드 마스터가 중간관리자를 선택한경우
		// 중간관리자 임명, 중간관리자 포기 비활성화
		if ( ucTitleVal == Guild::MIDDLE_ADMIN )
		{
			m_bGeneralGuild[ 1 ] = FALSE ;
			m_bGeneralGuild[ 3 ] = FALSE ;
		}

		// 길드 마스터가 일반길드원을 선택한경우
		if ( ucTitleVal == Guild::COMMON )
		{
			m_bGeneralGuild[ 2 ] = FALSE ;
			m_bGeneralGuild[ 3 ] = FALSE ;
		}
		// 길드 마스터가 가입, 탈퇴 대기자를 선택한경우
		if ( ( ucTitleVal == Guild::LEAVE_WAIT ) || ( ucTitleVal == Guild::JOIN_WAIT ) ) 
		{
			m_bGeneralGuild[ 0 ] = FALSE ;
			m_bGeneralGuild[ 1 ] = FALSE ;
			m_bGeneralGuild[ 2 ] = FALSE ;
			m_bGeneralGuild[ 3 ] = FALSE ;
		}
	}
	
	GuildMemberInfoNode*	 pInfo = GetMemberInfo() ;
	if ( pInfo->m_dwCID == CRYLNetworkData::Instance()->m_dwMyChrID )
		m_bGeneralGuild[ 5 ] = FALSE ;

	m_pGeneralGuildPopup->ClearItem() ;
	DWORD dwColor = 0 ;
	for ( int i = 0 ; i < 6 ; i ++ )
	{
		dwColor = ( m_bGeneralGuild[ i ] ) ? 0xFFFFFFFF : 0xFFFF0000 ;
		m_pGeneralGuildPopup->AddItem( m_szGeneralGuild[ i ], dwColor, 0xFF00FF00, 0xFFFF0000 ) ;
	}
}

VOID	CRYLGuildMemberPanel::ProposalGuildEnableEvent() 
{
	m_bProposalGuild[ 0 ] = TRUE ;
	m_bProposalGuild[ 1 ] = TRUE ;

	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
	UCHAR ucTitle = pDlg->GetTitle() ;
	GuildRight* pGuild = pDlg->GetGuildRight() ;

	if ( ucTitle == Guild::MIDDLE_ADMIN )
	{
		if ( pGuild->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::MASTER )
		{
			m_bProposalGuild[ 0 ] = FALSE ;
			m_bProposalGuild[ 1 ] = FALSE ;
		}
	}

	if ( ucTitle == Guild::COMMON ) 
	{
		if ( pGuild->m_aryRight[ GuildRight::PERMIT_JOIN ] == Guild::MIDDLE_ADMIN )
		{
			m_bProposalGuild[ 0 ] = FALSE ;
			m_bProposalGuild[ 1 ] = FALSE ;
		}
	}

	if ( ucTitle == Guild::JOIN_WAIT || ucTitle == Guild::LEAVE_WAIT )
	{
		m_bProposalGuild[ 0 ] = FALSE ;
		m_bProposalGuild[ 1 ] = FALSE ;
	}

	m_pProposalGuildPopup->ClearItem() ;

	if ( m_bProposalGuild[ 0 ] )
		m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2048 ], 0xffffffff, 0xff00ff00, 0xffff0000 ) ;
	else
		m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2048 ], 0xffff0000, 0xff00ff00, 0xffff0000 ) ;

	if ( m_bProposalGuild[ 1 ] )
		m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2049 ], 0xffffffff, 0xff00ff00, 0xffff0000 ) ;
	else
		m_pProposalGuildPopup->AddItem( CRYLStringTable::m_strString[ 2049 ], 0xffff0000, 0xff00ff00, 0xffff0000 ) ;
}

VOID	CRYLGuildMemberPanel::ProcessGeneralGuildPopup() 
{
	INT iIndex = m_pGeneralGuildPopup->GetItemIndex() ;

	if ( m_bGeneralGuild[ iIndex ] == FALSE || iIndex == -1 ) 
		return ;

	switch ( iIndex )
	{
		case 0 : // 길드 마스터 권한 양도
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1708], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultMessageBox[ 0 ] ) ;
			m_pGeneralGuildPopup->SetVisible( FALSE ) ;

			break ;
		}

		case 1 :	// 중간관리자 임명
		{
			
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1709], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultMessageBox[ 1 ] ) ;
			m_pGeneralGuildPopup->SetVisible( FALSE ) ;
			
			break ;
		}

		case 2 :	// 중간관리자 해임
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1710], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultMessageBox[ 2 ] ) ;
			m_pGeneralGuildPopup->SetVisible( FALSE ) ;
			break ;
		}

		case 3 :	// 중간관리자 포기
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1711], MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultMessageBox[ 3 ] ) ;
			m_pGeneralGuildPopup->SetVisible( FALSE ) ;
			break ;
		}
	
		case 4 :	// 길드에서 축출
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1712], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultMessageBox[ 4 ] ) ;
			m_pGeneralGuildPopup->SetVisible( FALSE ) ;
			break ;
		}

		case 5 :	// 귓말 보내기
		{
			char strText[MAX_PATH] ;
			strcpy( strText, "/s ") ;
			strcat( strText, m_pMemberInfo->m_szName ) ;
			strcat( strText, " ");

			strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_strRecentChatChrID ) ;
			CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
			CRYLChattingDlg::Instance()->m_pChatEdit->SetText( strText ) ;
			CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus() ;
			CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( TRUE ) ;

			break;
		}
	}
}

VOID		CRYLGuildMemberPanel::ResultGeneralGuildPopup() 
{
	// 길드 마스터 권한 양도
	if ( m_dwResultMessageBox[ 0 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, Guild::MASTER, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwResultMessageBox[ 0 ] = 0L ;
	}
	// 중간관리자 임명
	if ( m_dwResultMessageBox[ 1 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, Guild::MIDDLE_ADMIN, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwResultMessageBox[ 1 ] = 0L ;
	}
	// 중간관리자 해임
	if ( m_dwResultMessageBox[ 2 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, Guild::COMMON, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwResultMessageBox[ 2 ] = 0L ;
	}
	// 중간관리자 포기
	if ( m_dwResultMessageBox[ 3 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, Guild::COMMON, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwResultMessageBox[ 3 ] = 0L ;
	}
	// 길드에서 축출
	if ( m_dwResultMessageBox[ 4 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_KICK ) ;
		m_dwResultMessageBox[ 4 ] = 0L ;
	}
	// 서열 위로
	if ( m_dwResultMessageBox[ 5 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_RANK_UP ) ;
		m_dwResultMessageBox[ 5 ] = 0L ;
	}
	// 서열 아래로
	if ( m_dwResultMessageBox[ 6 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_RANK_DOWN ) ;
		m_dwResultMessageBox[ 6 ] = 0L ;
	}
}


VOID	CRYLGuildMemberPanel::PorcessProposalGuildPopup() 
{
	INT iIndex = m_pProposalGuildPopup->GetItemIndex() ;

	if ( m_bProposalGuild[ iIndex ] == FALSE || iIndex == -1 ) 
		return ;

	switch ( iIndex )
	{
	case 0 : // 길드원 삭제
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1200], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultProposalMessageBox[ 0 ] ) ;
			m_pProposalGuildPopup->SetVisible( FALSE ) ;
		}
		break ;

	case 1 : // 길드원 허가
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			char buff[ 512 ] ;
			sprintf( buff, CRYLStringTable::m_strString[1201], m_pMemberInfo->m_szName ) ;
			lpMessageBox->Create( buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResultProposalMessageBox[ 1 ] ) ;
			m_pProposalGuildPopup->SetVisible( FALSE ) ;
		}
		break ;
	}
}

VOID	CRYLGuildMemberPanel::ResultProposalGuildPopup() 
{
	// 길드원 삭제
	if ( m_dwResultProposalMessageBox[ 0 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_KICK ) ;
		m_dwResultProposalMessageBox[ 0 ] = 0L ;
	}
	// 길드원 허가
	if ( m_dwResultProposalMessageBox[ 1 ] & MB_YES ) 
	{
		SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
			m_pMemberInfo->m_dwCID, Guild::COMMON, PktGuildCmd::GC_SETUP_TITLE ) ;
		m_dwResultProposalMessageBox[ 1 ] = 0L ;
	}
}

VOID  CRYLGuildMemberPanel::Hide( CGMUIObject* Sender )
{
	if ( m_pProposalGuildPopup->GetVisible() )
		m_pProposalGuildPopup->SetVisible( FALSE ) ;

	if ( m_pGeneralGuildPopup->GetVisible() )
		m_pGeneralGuildPopup->SetVisible( FALSE ) ;

	CGMUICustomPanel::Hide( Sender );

	return;
}

INT	CRYLGuildMemberPanel::FindSortIndex( unsigned char cSortCmd )
{
	for ( int iIdx=0; iIdx<=GUILDMEMBERSORT::MAX_SORT_METHOD; ++iIdx )
	{
		if ( m_iSortMethod[ iIdx ] == cSortCmd )
		{
			return iIdx ;
		}
	}

	return 0 ;
}