
#include <winsock2.h>

#include <Creature/EnemyCheck.h>
#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>

#include <Network/ClientSocket/Send/SendCommunity.h>

#include "GMImageDib.h"
#include "GMUIScreen.h"
#include "GMUICustomDlg.h"

#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLEdit.h"
#include "RYLListbox.h"
#include "RYLComboBox.h"
#include "RYLStringGrid.h"
#include "RYLStringTable.h"
#include "RYLSprite.h"
#include "RYLSpriteEx.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLMessageBox.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLGuildInfoPanel.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildMarkDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLHostilityDlg.h"
#include "RYLChattingDlg.h"

#include <GameTime/GameTimeConstants.h>
#include "RYLGameTimeManager.h"

#include "RYLCampManager.h"
#include "GMMemory.h"

CRYLGuildInfoPanel*	g_pGuildInfoPanel = NULL ;

GUILDLARGEINFOEX::GUILDLARGEINFOEX() : GuildLargeInfoNode()
{
	m_pTexture = NULL ;
	m_pSprite  = NULL ;
}

GUILDLARGEINFOEX::~GUILDLARGEINFOEX()
{
	GM_DELETE( m_pTexture ) ;
	GM_DELETE( m_pSprite ) ;
}

CRYLGuildInfoPanel::CRYLGuildInfoPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pSortLabel  = NULL ;						// 정렬
	m_pSortValueLabel  = NULL ;					// 정렬 값
	m_pSortPrevButton  = NULL ;					// 정렬 방식 버튼( 이전 )
	m_pSortNextButton  = NULL ;					// 정렬 방식 버튼( 다음 )
	
	m_pRankLabel  = NULL ;						// 순위
	m_pGuildNameLabel  = NULL ;					// 길드명
	m_pGuildMasterLabel  = NULL ;				// 길드 마스터
	m_pGuildFameLabel  = NULL ;					// 길드명성
	m_pGuildListStringGrid  = NULL ;			// 길드 그리드
	m_pGuilListPrevButton  = NULL ;				// 길드 리스트 이전 버튼
	m_pGuilListNextButton  = NULL ;				// 길드 리스트 다음 버튼

	m_pGuildNameLabel1  = NULL ;				// 길드명
	m_pGuildNameEdit  = NULL ;					// 길드 값

	m_pGuildEntryButton  = NULL ;				// 길드 가입신청 버튼
	m_pGuildCancelButton = NULL ;				// 길드 가입취소 버튼
	m_pGuildTacticsEntryButton = NULL ;			// 길드 용병 가입신청 버튼
	m_pGuildTacticsCancelButton = NULL ;		// 길드 용병 가입취소 버튼
	
	m_pGuildRelationLabel = NULL ;				// 관계
	m_pGuildRelationValueLabel = NULL ;			// 길드 관계 값

	m_pGuildMemberNumLabel  = NULL ;			// 길드 인원
	m_pGuildMemberNumValueLabel  = NULL ;		// 길드 인원 값

	m_pGuildHostilityInfoButton = NULL ;		// 적대 현황 버튼

	m_pSetGuildRelationLabel = NULL ;			// 관계 설정
	m_pDeclareHostilityButton = NULL ;			// 적대 선언 버튼
	m_pDeclarePeaceButton = NULL ;				// 평화 선언 버튼
	m_pDeclareAlertHostilityButton = NULL ;		// 경계 적대 선언 버튼

	m_pGMUIParentDlg = pParentDlg ;

	strcpy( m_szSortMethodName[ 0 ], CRYLStringTable::m_strString[ 1070 ] ) ;		// 순위(명성)
	strcpy( m_szSortMethodName[ 1 ], CRYLStringTable::m_strString[ 1074 ] ) ;		// 길드명
	strcpy( m_szSortMethodName[ 2 ], CRYLStringTable::m_strString[ 1072 ] ) ;		// 적대길드
	strcpy( m_szSortMethodName[ 3 ], CRYLStringTable::m_strString[ 1073 ] ) ;		// 비우호/비적대길드
	strcpy( m_szSortMethodName[ 4 ], CRYLStringTable::m_strString[ 1075 ] ) ;		// 자신의 길드

	m_iSortMethod[ 0 ] = Guild::GUILD_FAME ;
	m_iSortMethod[ 1 ] = Guild::GUILD_NAME ;
	m_iSortMethod[ 2 ] = Guild::GUILD_HOSTILITY ;
	m_iSortMethod[ 3 ] = Guild::GUILD_NEUTRALITY ;
	m_iSortMethod[ 4 ] = Guild::GUILD_MINE ;

	m_iCurrentSort = 0 ;
	m_dwServerGuildPage = 0 ;
	m_dwStartGuildInfo  = 0 ;
	m_dwPageState		= 0 ;

	m_iCurrentLevel		= 0 ;
	m_iCurrentGID		= 0 ;
	m_iCurrentSel		= 0 ;
	m_iCurrentLine		= 0 ;
	m_iEntryGID			= 0 ;
	m_dwMaxGuildInfo	= 1000 ;
    	
	m_dwGuildEntryResult	= 0L ;

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
	pImageDIB->MakeFrameRect( 7, 238, 313, 121, 0xffB99E6B, 0xc0000000,  1 ) ;		// 금색 테두리 아래

	pImageDIB->MakeFrameRect( 13, 50,  304,  1, 0xff959595, 0xc0000000,  1 ) ;		// 한줄 라인

	pImageDIB->MakeFrameRect( 199, 4,  106,  19, 0xff959595, 0xc0000000,  1 ) ;		// 정렬방식

	pImageDIB->MakeFrameRect(  62, 250,  96,  19, 0xff959595, 0xc0000000,  1 ) ;	// 길드명
	pImageDIB->MakeFrameRect(  62, 289,  96,  19, 0xff959595, 0xc0000000,  1 ) ;	// 관계
	pImageDIB->MakeFrameRect( 193, 289,  38,  19, 0xff959595, 0xc0000000,  1 ) ;	// 인원
	 
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 328 ) ;
	SetHeight( 375 ) ;

	g_pGuildInfoPanel = this ;
}

CRYLGuildInfoPanel::~CRYLGuildInfoPanel() 
{
	ClearList();
	FinalCtrl();
}

VOID	CRYLGuildInfoPanel::InitCtrl() 
{
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 130, 60, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xff646464 ) ;
	pImageDIB->MakeFrameRect( 3, 3 , 124, 54, 0xff959595, 0xff646464,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 정렬 방식
	m_pSortLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSortLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 123, 8, 197, 19 ) ;
	m_pSortLabel->SetAlignRect( rt ) ;
	m_pSortLabel->SetAlign( RIGHT ) ;
	m_pSortLabel->SetCaption( CRYLStringTable::m_strString[ 2011 ] ) ;
	m_pSortLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSortLabel ) ;

	// 정렬 값
	m_pSortValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSortValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 199, 8, 304, 19 ) ;
	m_pSortValueLabel->SetAlignRect( rt ) ;
	m_pSortValueLabel->SetAlign( CENTER ) ;
	m_pSortValueLabel->SetCaption( CRYLStringTable::m_strString[ 2012 ] ) ;
	m_pSortValueLabel->SetAutoSize( TRUE ) ;
	m_pSortValueLabel->SetEnable( FALSE ) ;
	CGMUICustomPanel::AddChild( m_pSortValueLabel ) ;
	
	// 정렬 방식 버튼( 이전 )
	m_pSortPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pSortPrevButton->SetLeft( 307 ) ;
	m_pSortPrevButton->SetTop( 1 ) ;
	m_pSortPrevButton->OnClickEvent = DlgClick ;
	m_pSortPrevButton->SetInstance( RYLGUILDPANEL::SORTPREV_BUTTON ) ;
	SetRect( &rt, 199, 187, 211, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 187, 225, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 187, 239, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 187, 211, 199 ) ;
	m_pSortPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pSortPrevButton ) ;

	// 정렬 방식 버튼( 다음 )
	m_pSortNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pSortNextButton->SetLeft( 307 ) ;
	m_pSortNextButton->SetTop( 14 ) ;
	m_pSortNextButton->OnClickEvent = DlgClick ;
	m_pSortNextButton->SetInstance( RYLGUILDPANEL::SORTNEXT_BUTTON ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 201, 225, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 201, 239, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pSortNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pSortNextButton ) ;
	
	// 순위
	m_pRankLabel  = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pRankLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 15, 34, 47, 45 ) ;
	m_pRankLabel->SetAlignRect( rt ) ;
	m_pRankLabel->SetAlign( CENTER ) ;
	m_pRankLabel->SetCaption( CRYLStringTable::m_strString[ 2013 ] ) ;
	m_pRankLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pRankLabel ) ;

	// 길드명
	m_pGuildNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 47, 34, 124, 45 ) ;
	m_pGuildNameLabel->SetAlignRect( rt ) ;
	m_pGuildNameLabel->SetAlign( CENTER ) ;
	m_pGuildNameLabel->SetCaption( CRYLStringTable::m_strString[ 2014 ] ) ;
	m_pGuildNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildNameLabel ) ;

	// 길드 마스터
	m_pGuildMasterLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildMasterLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 124, 34, 221, 45 ) ;
	m_pGuildMasterLabel->SetAlignRect( rt ) ;
	m_pGuildMasterLabel->SetAlign( CENTER ) ;
	m_pGuildMasterLabel->SetCaption( CRYLStringTable::m_strString[ 2015 ] ) ;
	m_pGuildMasterLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildMasterLabel ) ;

	// 길드명성
	m_pGuildFameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildFameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 221, 34, 287, 45 ) ;
	m_pGuildFameLabel->SetAlignRect( rt ) ;
	m_pGuildFameLabel->SetAlign( CENTER ) ;
	m_pGuildFameLabel->SetCaption( CRYLStringTable::m_strString[ 2016 ] ) ;
	m_pGuildFameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildFameLabel ) ;
	
	// 마크
	m_pGuildMarkListLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildMarkListLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 287, 34, 317, 45 ) ;
	m_pGuildMarkListLabel->SetAlignRect( rt ) ;
	m_pGuildMarkListLabel->SetAlign( CENTER ) ;
	m_pGuildMarkListLabel->SetCaption( CRYLStringTable::m_strString[ 2017 ] ) ;
	m_pGuildMarkListLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildMarkListLabel ) ;

	
	// 길드 마크
	for ( INT i = 0 ; i < 10 ; i ++ )
	{
		m_pGuildMarkListSprite[ i ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
		m_pGuildMarkListSprite[ i ]->SetLeft( 294 ) ;
		m_pGuildMarkListSprite[ i ]->SetTop( 55 + ( i * 16 ) ) ;
		m_pGuildMarkListSprite[ i ]->SetWidth( 18 ) ;
		m_pGuildMarkListSprite[ i ]->SetHeight( 12 ) ;
		m_pGuildMarkListSprite[ i ]->SetVisible( FALSE ) ;
		CGMUICustomPanel::AddChild( m_pGuildMarkListSprite[ i ] ) ;
	}

	// 길드 그리드
	m_pGuildListStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pGuildListStringGrid->SetLeft( 15 ) ;
	m_pGuildListStringGrid->SetTop( 55 ) ;
	m_pGuildListStringGrid->SetWidth( 278 ) ;
	m_pGuildListStringGrid->SetHeight( 170 ) ;
	m_pGuildListStringGrid->SetColCount( 4 ) ;
	m_pGuildListStringGrid->SetItemHeight( 16 ) ;
	m_pGuildListStringGrid->SetFontColor( 0xffffffff ) ;
	m_pGuildListStringGrid->SetSelectedItemColor( 0xFFC9FFC5 ) ;
	m_pGuildListStringGrid->SetInstance( RYLGUILDPANEL::GUILD_STRINGGRID ) ;
	m_pGuildListStringGrid->OnClickEvent = DlgClick ;
	CGMUICustomPanel::AddChild( m_pGuildListStringGrid ) ;

	// 길드 리스트 이전 버튼
	m_pGuilListPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuilListPrevButton->SetLeft( 292 ) ;
	m_pGuilListPrevButton->SetTop( 220 ) ;
	m_pGuilListPrevButton->OnClickEvent = DlgClick ;
	m_pGuilListPrevButton->SetInstance( RYLGUILDPANEL::LISTPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pGuilListPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pGuilListPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pGuilListPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pGuilListPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuilListPrevButton ) ;
 
	// 길드 리스트 다음 버튼
	m_pGuilListNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuilListNextButton->SetLeft( 307 ) ;
	m_pGuilListNextButton->SetTop( 220 ) ;
	m_pGuilListNextButton->OnClickEvent = DlgClick ;
	m_pGuilListNextButton->SetInstance( RYLGUILDPANEL::LISTNEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pGuilListNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pGuilListNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pGuilListNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pGuilListNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuilListNextButton ) ;

	// 길드명
	m_pGuildNameLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildNameLabel1->SetFontColor (0xffffffff);
	m_pGuildNameLabel1->SetLeft( 23 ) ;
	m_pGuildNameLabel1->SetTop( 254 ) ;
	m_pGuildNameLabel1->SetCaption( CRYLStringTable::m_strString[ 2014 ] ) ;
	m_pGuildNameLabel1->SetAutoSize( TRUE ) ;
	m_pGuildNameLabel1->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1855 ], 1855, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pGuildNameLabel1 ) ;

	// 길드 값
	m_pGuildNameEdit = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildNameEdit->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 62, 254, 157, 265 ) ;
	m_pGuildNameEdit->SetAlignRect( rt ) ;
	m_pGuildNameEdit->SetAlign( CENTER ) ;
	m_pGuildNameEdit->SetInstance( RYLGUILDPANEL::GUILDSEACH_EDIT ) ;
	m_pGuildNameEdit->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildNameEdit ) ;

	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 길드 가입 신청 버튼
	m_pGuildEntryButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildEntryButton->SetLeft( 165 ) ;
	m_pGuildEntryButton->SetTop( 247 ) ;
	m_pGuildEntryButton->SetCaption( CRYLStringTable::m_strString[ 2020 ] ) ;
	m_pGuildEntryButton->SetFontColor( 0xffffffff) ;
	m_pGuildEntryButton->SetColor( 0xffffffff) ;
	m_pGuildEntryButton->OnClickEvent = DlgClick ;
	m_pGuildEntryButton->SetInstance( RYLGUILDPANEL::GUILDENTRY_BUTTON ) ;
	m_pGuildEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildEntryButton ) ;

	// 길드 가입 취소 버튼
	m_pGuildCancelButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildCancelButton->SetLeft( 165 ) ;
	m_pGuildCancelButton->SetTop( 247 ) ;
	m_pGuildCancelButton->SetCaption( CRYLStringTable::m_strString[ 2021 ] ) ;
	m_pGuildCancelButton->SetFontColor( 0xffffffff) ;
	m_pGuildCancelButton->SetColor( 0xffffffff) ;
	m_pGuildCancelButton->OnClickEvent = DlgClick ;
	m_pGuildCancelButton->SetInstance( RYLGUILDPANEL::GUILDCANCEL_BUTTON ) ;
	m_pGuildCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildCancelButton ) ;

	// 길드 용병 가입신청 버튼
	m_pGuildTacticsEntryButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildTacticsEntryButton->SetLeft( 238 ) ;
	m_pGuildTacticsEntryButton->SetTop( 247 ) ;
	m_pGuildTacticsEntryButton->SetCaption( CRYLStringTable::m_strString[3945] ) ;
	m_pGuildTacticsEntryButton->SetFontColor( 0xffffffff) ;
	m_pGuildTacticsEntryButton->SetColor( 0xffffffff) ;
	m_pGuildTacticsEntryButton->OnClickEvent = DlgClick ;
	m_pGuildTacticsEntryButton->SetInstance( RYLGUILDPANEL::GUILDTACTICSENTRY_BUTTON ) ;
	m_pGuildTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildTacticsEntryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildTacticsEntryButton ) ;

	// 길드 용병 가입취소 버튼
	m_pGuildTacticsCancelButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildTacticsCancelButton->SetLeft( 238 ) ;
	m_pGuildTacticsCancelButton->SetTop( 247 ) ;
	m_pGuildTacticsCancelButton->SetCaption( CRYLStringTable::m_strString[3946] ) ;
	m_pGuildTacticsCancelButton->SetFontColor( 0xffffffff) ;
	m_pGuildTacticsCancelButton->SetColor( 0xffffffff) ;
	m_pGuildTacticsCancelButton->OnClickEvent = DlgClick ;
	m_pGuildTacticsCancelButton->SetInstance( RYLGUILDPANEL::GUILDTACTICSCANCEL_BUTTON ) ;
	m_pGuildTacticsCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildTacticsCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildTacticsCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildTacticsCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildTacticsCancelButton ) ;

	// 관계
	m_pGuildRelationLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildRelationLabel->SetFontColor (0xffffffff);
	m_pGuildRelationLabel->SetLeft( 35 ) ;
	m_pGuildRelationLabel->SetTop( 293 ) ;
	m_pGuildRelationLabel->SetCaption( CRYLStringTable::m_strString[3947] ) ;
	m_pGuildRelationLabel->SetAutoSize( TRUE ) ;
	m_pGuildRelationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1862], 1862, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pGuildRelationLabel ) ;

	// 길드 관계 값
	m_pGuildRelationValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildRelationValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 62, 293, 157, 304 ) ;
	m_pGuildRelationValueLabel->SetAlignRect( rt ) ;
	m_pGuildRelationValueLabel->SetAlign( CENTER ) ;
	m_pGuildRelationValueLabel->SetCaption( "" ) ;
	m_pGuildRelationValueLabel->SetAutoSize( TRUE ) ;
	m_pGuildRelationValueLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1862], 1862, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pGuildRelationValueLabel ) ;


	// 길드 인원
	m_pGuildMemberNumLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildMemberNumLabel->SetFontColor (0xffffffff);
	m_pGuildMemberNumLabel->SetLeft( 165 ) ;
	m_pGuildMemberNumLabel->SetTop( 293 ) ;
	m_pGuildMemberNumLabel->SetCaption( CRYLStringTable::m_strString[3948] ) ;
	m_pGuildMemberNumLabel->SetAutoSize( TRUE ) ;
	m_pGuildMemberNumLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1862], 1862, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pGuildMemberNumLabel ) ;


	// 길드 인원 값
	m_pGuildMemberNumValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildMemberNumValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 193, 293, 230, 304 ) ;
	m_pGuildMemberNumValueLabel->SetAlignRect( rt ) ;
	m_pGuildMemberNumValueLabel->SetAlign( CENTER ) ;
	m_pGuildMemberNumValueLabel->SetCaption( "" ) ;
	m_pGuildMemberNumValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildMemberNumValueLabel ) ;

	
	// 적대 현황 버튼
	m_pGuildHostilityInfoButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pGuildHostilityInfoButton->SetLeft( 238 ) ;
	m_pGuildHostilityInfoButton->SetTop( 286 ) ;
	m_pGuildHostilityInfoButton->SetCaption( CRYLStringTable::m_strString[3949] ) ;
	m_pGuildHostilityInfoButton->SetFontColor( 0xffffffff) ;
	m_pGuildHostilityInfoButton->SetColor( 0xffffffff) ;
	m_pGuildHostilityInfoButton->OnClickEvent = DlgClick ;
	m_pGuildHostilityInfoButton->SetInstance( RYLGUILDPANEL::GUILDHOSTILITYINFO_BUTTON ) ;
	m_pGuildHostilityInfoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildHostilityInfoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildHostilityInfoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildHostilityInfoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pGuildHostilityInfoButton ) ;


	// 관계 설정
	m_pSetGuildRelationLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSetGuildRelationLabel->SetFontColor (0xffffffff);
	m_pSetGuildRelationLabel->SetLeft( 35 ) ;
	m_pSetGuildRelationLabel->SetTop( 332 ) ;
	m_pSetGuildRelationLabel->SetCaption( CRYLStringTable::m_strString[3950] ) ;
	m_pSetGuildRelationLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSetGuildRelationLabel ) ;

	// 적대 선언 버튼
	m_pDeclareHostilityButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDeclareHostilityButton->SetLeft( 92 ) ;
	m_pDeclareHostilityButton->SetTop( 325 ) ;
	m_pDeclareHostilityButton->SetCaption( CRYLStringTable::m_strString[3951] ) ;
	m_pDeclareHostilityButton->SetFontColor( 0xffffffff) ;
	m_pDeclareHostilityButton->SetColor( 0xffffffff) ;
	m_pDeclareHostilityButton->OnClickEvent = DlgClick ;
	m_pDeclareHostilityButton->SetInstance( RYLGUILDPANEL::DECLAREHOSTILITY_BUTTON ) ;
	m_pDeclareHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDeclareHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDeclareHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDeclareHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDeclareHostilityButton->SetEnable(FALSE);
	CGMUICustomPanel::AddChild( m_pDeclareHostilityButton ) ;

	// 평화 선언 버튼
	m_pDeclarePeaceButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDeclarePeaceButton->SetLeft( 165 ) ;
	m_pDeclarePeaceButton->SetTop( 325 ) ;
	m_pDeclarePeaceButton->SetCaption( CRYLStringTable::m_strString[3952] ) ;
	m_pDeclarePeaceButton->SetFontColor( 0xffffffff) ;
	m_pDeclarePeaceButton->SetColor( 0xffffffff) ;
	m_pDeclarePeaceButton->OnClickEvent = DlgClick ;
	m_pDeclarePeaceButton->SetInstance( RYLGUILDPANEL::DECLAREPEACE_BUTTON ) ;
	m_pDeclarePeaceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDeclarePeaceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDeclarePeaceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDeclarePeaceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDeclarePeaceButton->SetEnable(FALSE);
	CGMUICustomPanel::AddChild( m_pDeclarePeaceButton ) ;

	// 경계 적대 선언 버튼
	m_pDeclareAlertHostilityButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDeclareAlertHostilityButton->SetLeft( 238 ) ;
	m_pDeclareAlertHostilityButton->SetTop( 325 ) ;
	m_pDeclareAlertHostilityButton->SetCaption( CRYLStringTable::m_strString[3953] ) ;
	m_pDeclareAlertHostilityButton->SetFontColor( 0xffffffff) ;
	m_pDeclareAlertHostilityButton->SetColor( 0xffffffff) ;
	m_pDeclareAlertHostilityButton->OnClickEvent = DlgClick ;
	m_pDeclareAlertHostilityButton->SetInstance( RYLGUILDPANEL::DECLAREALERTHOSTILITY_BUTTON ) ;
	m_pDeclareAlertHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDeclareAlertHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDeclareAlertHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDeclareAlertHostilityButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDeclareAlertHostilityButton->SetEnable(FALSE);
	CGMUICustomPanel::AddChild( m_pDeclareAlertHostilityButton ) ;
}

VOID	CRYLGuildInfoPanel::FinalCtrl() 
{
	GM_DELETE( m_pSortLabel ) ;						// 정렬
	GM_DELETE( m_pSortValueLabel ) ;				// 정렬 값
	GM_DELETE( m_pSortPrevButton ) ;				// 정렬 방식 버튼( 이전 )
	GM_DELETE( m_pSortNextButton ) ;				// 정렬 방식 버튼( 다음 )
	
	GM_DELETE( m_pRankLabel ) ;						// 순위
	GM_DELETE( m_pGuildNameLabel ) ;				// 길드명
	GM_DELETE( m_pGuildMasterLabel ) ;				// 길드 마스터
	GM_DELETE( m_pGuildFameLabel ) ;				// 길드명성
	GM_DELETE( m_pGuildListStringGrid ) ;			// 길드 그리드
	GM_DELETE( m_pGuilListPrevButton ) ;			// 길드 리스트 이전 버튼
	GM_DELETE( m_pGuilListNextButton ) ;			// 길드 리스트 다음 버튼

	GM_DELETE( m_pGuildNameLabel1 ) ;				// 길드명
	GM_DELETE( m_pGuildNameEdit ) ;					// 길드 값
	GM_DELETE( m_pGuildEntryButton ) ;				// 길드 가입신청 버튼
	GM_DELETE( m_pGuildCancelButton ) ;				// 길드 가입취소 버튼
	GM_DELETE( m_pGuildTacticsEntryButton ) ;		// 길드 용병 가입신청 버튼
	GM_DELETE( m_pGuildTacticsCancelButton ) ;		// 길드 용병 가입취소 버튼

	GM_DELETE( m_pGuildRelationLabel ) ;			// 관계
	GM_DELETE( m_pGuildRelationValueLabel ) ;		// 길드 관계 값

	GM_DELETE( m_pGuildMemberNumLabel ) ;			// 길드 인원
	GM_DELETE( m_pGuildMemberNumValueLabel ) ;		// 길드 인원 값

	GM_DELETE( m_pGuildHostilityInfoButton ) ;		// 적대 현황 버튼

	GM_DELETE( m_pSetGuildRelationLabel ) ;			// 관계 설정
	GM_DELETE( m_pDeclareHostilityButton ) ;		// 적대 선언 버튼
	GM_DELETE( m_pDeclarePeaceButton ) ;			// 평화 선언 버튼
	GM_DELETE( m_pDeclareAlertHostilityButton ) ;	// 경계 적대 선언 버튼

	GM_DELETE( m_pGuildMarkListLabel ) ;

	for ( INT i = 0 ; i < 10 ; i ++ ) 
	{
		GM_DELETE( m_pGuildMarkListSprite[ i ] ) ;
	}
}

HRESULT		CRYLGuildInfoPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	m_pSortValueLabel->SetCaption( m_szSortMethodName[ m_iCurrentSort ] ) ;

	if ( 0 == CRYLGameData::Instance()->m_csStatus.m_Info.GID || (CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT) )
	{
		m_pGuildRelationValueLabel->SetCaption( "" ) ;
	}
	else if(m_iCurrentSel != -1)
	{
		if ( (int)m_vecGuildInfo.empty()  || (int)m_vecGuildInfo.size() < m_iCurrentSel)
			return S_OK ;

		LPGUILDLARGEINFOEX lpGuildInfo = m_vecGuildInfo[ m_iCurrentSel ] ;
		if ( lpGuildInfo )
		{
			if ( lpGuildInfo->m_dwGID == CRYLGameData::Instance()->m_csStatus.m_Info.GID )
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3954] ) ;
			}
			else if ( Guild::NEUTRALITY == lpGuildInfo->m_cRelationByMine && 
					  Guild::NEUTRALITY == lpGuildInfo->m_cRelationByTarget )
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3955] ) ;
			}
			else
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3956] ) ;
			}
		}
	}

	UpdateGuildMsgBox() ;

	return S_OK ;
}

HRESULT		CRYLGuildInfoPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLGuildInfoPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLGuildInfoPanel::SortMethodPrev() 
{
	m_iCurrentSel = 0;
	if ( m_iCurrentSort == 0 )
		m_iCurrentSort = GUILDINFOSORT::MAX_SORT_METHOD ;
	else
		-- m_iCurrentSort ;

	m_pSortValueLabel->SetCaption( m_szSortMethodName[ m_iCurrentSort ] ) ;

	m_dwServerGuildPage = 0 ;
	m_dwStartGuildInfo  = 0 ;
	m_dwPageState = GUILDINFOPAGEEX::PAGE_CURRENT ;
	RequestGuildList() ;
}

VOID	CRYLGuildInfoPanel::SortMethodNext() 
{
	m_iCurrentSel = 0;
	if ( m_iCurrentSort == GUILDINFOSORT::MAX_SORT_METHOD )
		m_iCurrentSort = 0 ;
	else
		++ m_iCurrentSort ;

	m_pSortValueLabel->SetCaption( m_szSortMethodName[ m_iCurrentSort ] ) ;

	m_dwServerGuildPage = 0 ;
	m_dwStartGuildInfo  = 0 ;
	m_dwPageState = GUILDINFOPAGEEX::PAGE_CURRENT ;
	RequestGuildList() ;
}

VOID	CRYLGuildInfoPanel::GuildListPrev()
{
	m_iCurrentSel = 0;
	if ( m_dwStartGuildInfo == 0 )
		return ;

	m_dwStartGuildInfo -= 10 ;
	m_dwPageState = GUILDINFOPAGEEX::PAGE_PREV ;
	RequestGuildList() ;
}

VOID	CRYLGuildInfoPanel::GuildListNext()
{
	m_iCurrentSel = 0;
	if  ( GetCurrentPageLine( GUILDINFOPAGEEX::PAGE_CURRENT ) < 10 )
	{
		return ;
	}

	m_dwStartGuildInfo += 10 ;
	m_dwPageState = GUILDINFOPAGEEX::PAGE_NEXT ;
	RequestGuildList() ;
}

LPGUILDLARGEINFOEX		CRYLGuildInfoPanel::GetGuild( DWORD dwGID ) 
{
	std::vector< GUILDLARGEINFOEX* >::iterator	it ;

	for ( it = m_vecGuildInfo.begin() ; it != m_vecGuildInfo.end() ; ++ it )
	{
		if ( ( *it )->m_dwGID == dwGID )
		{
			return ( *it ) ;
		}
	}
	return NULL ;
}

LPGUILDLARGEINFOEX 
CRYLGuildInfoPanel::FindGuild( DWORD dwGID ) 
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
	{
		return iter->second;
	}
	return NULL;
}

UINT		CRYLGuildInfoPanel::GetMyGuildInfoIndex( DWORD dwGID )
{
	UINT uiCount = 0 ;
	std::vector< LPGUILDLARGEINFOEX >::iterator		it ;

	for ( it = m_vecGuildInfo.begin() ; it != m_vecGuildInfo.end() ; ++ it, uiCount ++ )
	{
		if ( ( *it )->m_dwGID == dwGID )
		{
			return uiCount ;
		}
	}
	return -1 ;
}

BOOL	CRYLGuildInfoPanel::IsHostilityGuild( DWORD dwTargetGID )
{
	if ( 0 != dwTargetGID )
	{
		LPGUILDLARGEINFOEX lpGuildInfo = FindGuild( dwTargetGID ) ;
		if ( lpGuildInfo )
		{
			if ( HOSTILITY == lpGuildInfo->m_cRelationByMine ||
                 COUNTER_HOSTILITY == lpGuildInfo->m_cRelationByMine ||
				 ALERT_HOSTILITY == lpGuildInfo->m_cRelationByMine ||
				 TARGET_NEUTRALITY == lpGuildInfo->m_cRelationByMine ||
                 HOSTILITY == lpGuildInfo->m_cRelationByTarget ||
				 COUNTER_HOSTILITY == lpGuildInfo->m_cRelationByTarget ||
				 ALERT_HOSTILITY == lpGuildInfo->m_cRelationByTarget ||
				 TARGET_NEUTRALITY == lpGuildInfo->m_cRelationByTarget )
			{
				return TRUE ;
			}
		}
	}

	return FALSE ;
}

HRESULT		CRYLGuildInfoPanel::UpdateGuildStringGrid() 
{
	if ( m_pGMUIParentDlg->GetVisible() )
	{
		m_pGuildListStringGrid->ClearItem() ;

		for ( INT ispr = 0 ; ispr < 10 ; ++ispr )
		{
			m_pGuildMarkListSprite[ ispr ]->SetVisible( FALSE ) ;
			m_pGuildMarkListSprite[ ispr ]->DetachImageTexture() ;
		}
		
		std::vector< LPGUILDLARGEINFOEX >::iterator it;
		
		if ( false == m_vecGuildInfo.empty() )
		{
			for ( int i = 0; i < GetCurrentPageLine( GUILDINFOPAGEEX::PAGE_CURRENT ) ; i ++ )
			{
				if ( i + m_dwStartGuildInfo >= m_vecGuildInfo.size() )
				{
					return S_OK ;
				}

				it = m_vecGuildInfo.begin() + i + m_dwStartGuildInfo;
				LPGUILDLARGEINFOEX lpInfo = ( *it ) ;
				if ( NULL != lpInfo )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", lpInfo->m_wRank ) ;
					m_pGuildListStringGrid->AddItem( 0, szBuff, 32, 0xffffffff ) ;
					
					unsigned long guildID        = CRYLGameData::Instance()->m_csStatus.m_Info.GID & ~Guild::MEMBER_WAIT_BIT ;
					unsigned char race           = CRYLGameData::Instance()->m_csStatus.m_Info.Race;
                    unsigned char cAccountNation = CRYLGameData::Instance()->m_cAccountNation;

					unsigned long color = 0xFFFFFFFF;

					std::map< INT, LPGUILDLARGEINFOEX >::iterator iter = m_mapGuildInfo.find(guildID) ;

					// 자신이 길드에 가입하지 않은 상태
					if (iter == m_mapGuildInfo.end() || 0 == iter->second->m_cInclination)
					{
						switch ( lpInfo->m_cInclination)
						{
							case Creature::KARTERANT:		color = 0xFF00BFF3;		break;
							case Creature::MERKADIA:		color = 0xFFFFF468;		break;
							case Creature::ALMIGHTY_PIRATE:	color = 0xFF3BB878;		break;
						}
					}
					else
					{
						// 자신의 길드
						if (lpInfo->m_dwGID == guildID)		
						{
							CRYLGuildPropertyPanel*	pProperty = 
								reinterpret_cast<CRYLGuildPropertyPanel *>(
								reinterpret_cast<CRYLGuildFrameDlg *>(m_pGMUIParentDlg)->GetGuildPropertyPanel());
							pProperty->SetInclination( lpInfo->m_cInclination );
							color = 0xFFFFFFC0;			// 금색
							m_pGuildListStringGrid->SelectItemIndex( 0 ) ;
						}
						else
						{
							switch ( lpInfo->m_cInclination)
							{
								case Creature::KARTERANT:		color = 0xFF00BFF3;		break;
								case Creature::MERKADIA:		color = 0xFFFFF468;		break;
								case Creature::ALMIGHTY_PIRATE:	color = 0xFF3BB878;		break;
							}
						}
					}

					m_pGuildListStringGrid->AddItem( 1, lpInfo->m_szName, 76,  color ) ; 
					m_pGuildListStringGrid->AddItem( 2, lpInfo->m_szMasterName, 98, 0xffffffff ) ;
					m_pGuildListStringGrid->AddItem( 3, CRYLStringTable::MakePrintGold( lpInfo->m_dwFame ), 65, 0xffffffff ) ;

					m_pGuildMarkListSprite[ i ]->SetVisible( TRUE ) ;
					m_pGuildMarkListSprite[ i ]->AttachImageTexture( lpInfo->m_pSprite ) ;
				}
			}
		}
	}

	return S_OK ;
}

VOID		CRYLGuildInfoPanel::SelectGuildInfo() 
{
	INT iIndex = m_pGuildListStringGrid->GetItemIndex() + m_dwStartGuildInfo ;

	if ( iIndex == -1 )
		return ;

	UpdateMyGuildInfo( iIndex ) ;
}

VOID		CRYLGuildInfoPanel::UpdateMyGuildInfo( INT iIndex ) 
{
	if ( iIndex == -1 )
		iIndex = GetMyGuildInfoIndex( CRYLGameData::Instance()->m_csStatus.m_Info.GID ) ;

	if ( m_vecGuildInfo.empty() || iIndex == -1 )
		return ;

	LPGUILDLARGEINFOEX lpGuildInfo = m_vecGuildInfo[ iIndex ] ;
	if ( lpGuildInfo )
	{
		strncpy( m_GuildInfo.szGuildName, lpGuildInfo->m_szName, Guild::MAX_GUILD_NAME_LEN ) ;
		strncpy( m_GuildInfo.szGuildMaster, lpGuildInfo->m_szMasterName, Guild::MAX_MEMBER_NAME_LEN ) ;
		strcpy( m_GuildInfo.szGuildFame, CRYLStringTable::MakePrintGold( lpGuildInfo->m_dwFame ) ) ;
		sprintf( m_GuildInfo.szGuildLevel, "%d", Guild::ms_aryMaxMemberNum[ lpGuildInfo->m_cLevel ] ) ;
		sprintf( m_GuildInfo.szGuildMemberNum, "%d", lpGuildInfo->m_cCurrentMemberNum ) ;
	
		switch ( lpGuildInfo->m_cInclination )
		{
			case Creature::KARTERANT:
				sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2254 ]);
				break;

			case Creature::MERKADIA:
				sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2255 ]);
				break;

			case Creature::ALMIGHTY_PIRATE:
				sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2256 ]);
				break;
		}

		m_GuildInfo.pGuildMark = lpGuildInfo->m_pSprite ;

		CRYLGameScene*			pScene	= static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
		CRYLGuildFrameDlg*		pDlg	= (CRYLGuildFrameDlg*) pScene->GetGuildFrameDlg() ;
		CRYLGuildPropertyPanel* pPanel	= (CRYLGuildPropertyPanel*) pDlg->GetGuildPropertyPanel() ;
		pPanel->SetGuildMark( m_GuildInfo.pGuildMark ) ;
		pPanel->SetGuildMemberLevelFame( lpGuildInfo->m_cCurrentMemberNum, lpGuildInfo->m_cLevel, lpGuildInfo->m_dwFame ) ;
		
		m_pGuildNameEdit->SetCaption( m_GuildInfo.szGuildName ) ;							// 길드 값

		CHAR szBuff[ 32 ] ;
		sprintf( szBuff, "%s/%s", m_GuildInfo.szGuildMemberNum, m_GuildInfo.szGuildLevel ) ;
		m_pGuildMemberNumValueLabel->SetCaption( szBuff ) ;									// 길드 인원 값
	
		m_iCurrentGID = lpGuildInfo->m_dwGID ;
		m_iCurrentSel = iIndex ;

		UpdateBtnState() ;
	}
}

VOID	CRYLGuildInfoPanel::GuildEntryButtonEvent()
{
	if ( CRYLGameTimeManager::Instance()->IsGuildWarTime() )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[4056] ) ;
		return ;
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( !pSelfCreature ) return ;

	// 선택한 길드가 없다면...
	if ( 0 == m_iCurrentGID )
	{
		CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[4051] , CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		return ;
	}

	// edith 2008.02.27 길드 가입 제한제외
	if ( Creature::LEVEL_ABLE_WAR > CRYLGameData::Instance()->m_csStatus.m_Info.Level )
	{
		char szErrMsg[ MAX_PATH ] ;
		sprintf( szErrMsg, CRYLStringTable::m_strString[1095], Creature::LEVEL_ABLE_WAR ) ;

		CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( szErrMsg ) ;
		return ;
	}

	DWORD dwSelfGuildID = ( CRYLGameData::Instance()->m_csStatus.m_Info.GID & ~Guild::MEMBER_WAIT_BIT ) ;
	BOOL bWaitBit = FALSE ;
	if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
	{
		bWaitBit = TRUE ;
	}

	LPGUILDLARGEINFOEX pSelfGuildInfo = FindGuild( dwSelfGuildID ) ;
	LPGUILDLARGEINFOEX pTargetGuildInfo = FindGuild( m_iCurrentGID ) ;
	if ( pTargetGuildInfo )
	{
		if ( pSelfGuildInfo )
		{
			if ( bWaitBit )
			{
				if ( pSelfCreature->GetTactics() == Guild::TACTICS_WAIT )
				{
					// 용병 신청 중일때
					char szErrMsg[ MAX_PATH ] ;
					sprintf( szErrMsg, CRYLStringTable::m_strString[4052], pSelfGuildInfo->m_szName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szErrMsg, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				else
				{
					// 가입 신청 중일때
					char szErrMsg[ MAX_PATH ] ;
					sprintf( szErrMsg, CRYLStringTable::m_strString[4053], pSelfGuildInfo->m_szName ) ;
					CRYLChattingDlg::Instance()->AddChatMessage( szErrMsg, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
			else
			{
				if ( pSelfCreature->GetTactics() == Guild::TACTICS )
				{
					// 용병인 경우
					CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[4054], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				else
				{
					// 길드 가입자의 경우
					CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[4055], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}

			return ;
		}

		char cInclination = pTargetGuildInfo->m_cInclination;
		if ( cInclination == CRYLGameData::Instance()->m_cAccountNation )
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			CHAR	szBuff[ 512 ] ;
			sprintf( szBuff, CRYLStringTable::m_strString[ 2980 ], pTargetGuildInfo->m_szName ) ;
			m_iEntryGID = m_iCurrentGID;
			lpMessageBox->Create( szBuff, MB_YES | MB_NO ) ;
			lpMessageBox->SetResult( &m_dwGuildEntryResult ) ;
			m_dwGuildEntryResult = 0 ;
		}
		else
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 2981 ], MB_OK ) ;
		}
	}
}

VOID	CRYLGuildInfoPanel::GuildEntryCancelButtonEvent()
{
	SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
		CRYLNetworkData::Instance()->m_dwMyChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_KICK ) ;
}

VOID	CRYLGuildInfoPanel::GuildTacticsEntryButtonEvent()
{
	CRYLGameData*	pGame			= CRYLGameData::Instance() ;
	RYLCreature*	pSelfCreature	= RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );

	if ( pSelfCreature )
	{		
		CharDataNode* pSelfData	= RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if ( pSelfData )
		{
			DWORD dwSelfGuildID = ( CRYLGameData::Instance()->m_csStatus.m_Info.GID & ~Guild::MEMBER_WAIT_BIT ) ;
			BOOL bWaitBit = FALSE ;
			if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
			{
				bWaitBit = TRUE ;
			}

			LPGUILDLARGEINFOEX pSelfGuildInfo = FindGuild( dwSelfGuildID ) ;
			LPGUILDLARGEINFOEX pTargetGuildInfo = FindGuild( m_iCurrentGID ) ;
			if ( pTargetGuildInfo )
			{								
				// 길드 가입자.
				if ( pGame->m_csStatus.m_Info.GID && !pSelfCreature->GetTactics() )
				{									
					CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[3595], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					return ;
				}

				// 용병 가입.
				if ( pGame->m_csStatus.m_Info.GID && pSelfCreature->GetTactics() == Guild::TACTICS )
				{
					if ( dwSelfGuildID == m_iCurrentGID )
					{
						CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[3597], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					else if ( pSelfGuildInfo )
					{
						char szInfo[ 0xff ] = { 0, } ;
						sprintf( szInfo, CRYLStringTable::m_strString[4046], pSelfGuildInfo->m_szName ) ;
						CRYLChattingDlg::Instance()->AddChatMessage( szInfo, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					else
					{
						CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[4047], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					return ;
				}							

				// 용병 신청중.
				if ( pGame->m_csStatus.m_Info.GID && pSelfCreature->GetTactics() == Guild::TACTICS_WAIT )
				{	
					if ( dwSelfGuildID == m_iCurrentGID )
					{
						CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[3596], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					else if ( pSelfGuildInfo )
					{
						char szInfo[ 0xff ] = { 0, } ;
						sprintf( szInfo, CRYLStringTable::m_strString[4048], pSelfGuildInfo->m_szName ) ;
						CRYLChattingDlg::Instance()->AddChatMessage( szInfo, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					else
					{
						CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[4049], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					return ;
				}

				// 레벨 미만.
				if ( pSelfData->m_cLevel < Guild::TACTICE_JON_LEVEL )
				{
					CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[3598], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					return ;
				}

				// 타국적.
				if ( pTargetGuildInfo->m_cInclination != pSelfData->m_cNation )
				{																											
					CRYLChattingDlg::Instance()->AddChatMessage( CRYLStringTable::m_strString[3599], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					return ;
				}												

				// 조건 만족 서버에 패킷을 보낸다.
				SendPacket::CharGuildCmd( g_GameSession, m_iCurrentGID, 
					CRYLNetworkData::Instance()->m_dwMyChrID, 0, PktGuildCmd::GC_TACTICS ) ;	
			}										
		}
	}	
}

VOID	CRYLGuildInfoPanel::GuildTacticsCancelButtonEvent()
{
	if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID )
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

		if(pSelfCreature)
		{
			if(pSelfCreature->GetTactics()==TACTICS_WAIT)
			{
				SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.GID, 
						CRYLNetworkData::Instance()->m_dwMyChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_TACTICS_KICK ) ;

				CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3890], pSelfCreature->GetCharID(), 128);
			}			
			else
			{
				CRYLChattingDlg::Instance()->AddChatMessage(CRYLStringTable::m_strString[3891], pSelfCreature->GetCharID(), 128);				
			}
		}
	}
}

VOID	CRYLGuildInfoPanel::GuildHostilityInfoButtonEvent()
{
	if ( 0 != CRYLGameData::Instance()->m_csStatus.m_Info.GID )
	{
        CRYLHostilityDlg* lpHostilityDlg = ( CRYLHostilityDlg* )( ( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetHostilityDlg() ) ;
		if ( !lpHostilityDlg->GetVisible() )
		{
			lpHostilityDlg->SetGID( CRYLGameData::Instance()->m_csStatus.m_Info.GID ) ;
			lpHostilityDlg->RequestHostiliyList() ;
			lpHostilityDlg->SetVisible( TRUE ) ;
		}
		else
		{
			lpHostilityDlg->SetVisible( FALSE ) ;
		}
	}
}

VOID	CRYLGuildInfoPanel::GuildDeclareHostilityButtonEvent()
{
	if ( GameTime::GT_NORMAL != CRYLGameTimeManager::Instance()->GetNowGuildTime() )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[3957] ) ;
		return ;
	}

	std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.find( m_iCurrentGID ) ;
	if ( iter != m_mapGuildInfo.end() )
	{
		LPGUILDLARGEINFOEX lpGuildLargeInfoEx = iter->second ;
		if ( lpGuildLargeInfoEx )
		{
			unsigned char cRelationByMine = lpGuildLargeInfoEx->m_cRelationByMine ;
			if ( Guild::HOSTILITY == cRelationByMine )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3958] ) ;
				return ;
			}
			else if ( Guild::MINE == cRelationByMine )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3959] ) ;
				return ;
			}

			SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
										   CRYLGameData::Instance()->m_csStatus.m_Info.GID,
										   lpGuildLargeInfoEx->m_dwGID, 0, PktGuildRelation::GR_HOSTILITY ) ;
		}
	}
}

VOID	CRYLGuildInfoPanel::GuildDeclarePeaceButtonEvent()
{
	if ( GameTime::GT_NORMAL != CRYLGameTimeManager::Instance()->GetNowGuildTime() )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 4045 ] ) ;
		return ;
	}

	std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.find( m_iCurrentGID ) ;
	if ( iter != m_mapGuildInfo.end() )
	{
		LPGUILDLARGEINFOEX lpGuildLargeInfoEx = iter->second ;
		if ( lpGuildLargeInfoEx )
		{
			unsigned char cRelationByTarget = lpGuildLargeInfoEx->m_cRelationByTarget ;
			if ( Guild::TARGET_NEUTRALITY == cRelationByTarget)
			{
				// 만약 내가 피스를 받아줄 길드라면 피스를 받아준다.
				SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											CRYLGameData::Instance()->m_csStatus.m_Info.GID,
											lpGuildLargeInfoEx->m_dwGID, 0, PktGuildRelation::GR_HOSTILITYDEL ) ;
			}
			else
			{
				unsigned char cRelationByMine = lpGuildLargeInfoEx->m_cRelationByMine ;
				if ( Guild::NEUTRALITY == cRelationByMine )
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[3960] ) ;
					return ;
				}
				else if ( Guild::MINE == cRelationByMine )
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[3961] ) ;
					return ;
				}

				SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
											CRYLGameData::Instance()->m_csStatus.m_Info.GID,
											lpGuildLargeInfoEx->m_dwGID, 0, PktGuildRelation::GR_PEACE ) ;
			}
		}
	}
}

VOID	CRYLGuildInfoPanel::GuildDeclareAlertHostilityButtonEvent()
{
	if ( !CRYLGameTimeManager::Instance()->IsGuildWarTime() )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[3962] ) ;
		return ;
	}

	std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.find( m_iCurrentGID ) ;
	if ( iter != m_mapGuildInfo.end() )
	{
		LPGUILDLARGEINFOEX lpGuildLargeInfoEx = iter->second ;
		if ( lpGuildLargeInfoEx )
		{
			unsigned char cRelationByMine = lpGuildLargeInfoEx->m_cRelationByMine ;
			if ( Guild::HOSTILITY == cRelationByMine )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3958] ) ;
				return ;
			}
			else if ( Guild::MINE == cRelationByMine )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3963] ) ;
				return ;
			}

			bool bHasDevelopingCamp = false ;
			bool bHasCompleteCamp = false ;
			CRYLCampManager::Instance()->HasCampByGID( lpGuildLargeInfoEx->m_dwGID, bHasDevelopingCamp, bHasCompleteCamp ) ;

			if ( !bHasDevelopingCamp )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[3964] ) ;
				return ;
			}

			SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
										   CRYLGameData::Instance()->m_csStatus.m_Info.GID,
										   lpGuildLargeInfoEx->m_dwGID, 0, PktGuildRelation::GR_ALERT ) ;
		}
	}
}

VOID	CRYLGuildInfoPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pGuildInfoPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLGUILDPANEL::GUILD_STRINGGRID :
			g_pGuildInfoPanel->SelectGuildInfo() ;
			break ;

		case RYLGUILDPANEL::SORTPREV_BUTTON :
			
			g_pGuildInfoPanel->SortMethodPrev() ;
			break ;

		case RYLGUILDPANEL::SORTNEXT_BUTTON :
			
			g_pGuildInfoPanel->SortMethodNext() ;
			break ;

		// 가입신청
		case RYLGUILDPANEL::GUILDENTRY_BUTTON :
			g_pGuildInfoPanel->GuildEntryButtonEvent() ;
			break ;

		// 가입취소
		case RYLGUILDPANEL::GUILDCANCEL_BUTTON :
			g_pGuildInfoPanel->GuildEntryCancelButtonEvent() ;
			break ;

		// 용병신청
		case RYLGUILDPANEL::GUILDTACTICSENTRY_BUTTON :
			g_pGuildInfoPanel->GuildTacticsEntryButtonEvent() ;
			break ;

		// 용병취소
		case RYLGUILDPANEL::GUILDTACTICSCANCEL_BUTTON :
			g_pGuildInfoPanel->GuildTacticsCancelButtonEvent() ;
			break ;

		case RYLGUILDPANEL::LISTPREV_BUTTON	:
			g_pGuildInfoPanel->GuildListPrev() ;
			break ;

		case RYLGUILDPANEL::LISTNEXT_BUTTON	:
			g_pGuildInfoPanel->GuildListNext() ;
			break ;

		case RYLGUILDPANEL::GUILDHOSTILITYINFO_BUTTON	:
			g_pGuildInfoPanel->GuildHostilityInfoButtonEvent() ;
			break ;

		// 적대 신청
		case RYLGUILDPANEL::DECLAREHOSTILITY_BUTTON :
			g_pGuildInfoPanel->GuildDeclareHostilityButtonEvent() ;
			break ;

		// 평화 신청
		case RYLGUILDPANEL::DECLAREPEACE_BUTTON :
			g_pGuildInfoPanel->GuildDeclarePeaceButtonEvent() ;
			break ;

		// 경계 적대 신청
		case RYLGUILDPANEL::DECLAREALERTHOSTILITY_BUTTON :
			g_pGuildInfoPanel->GuildDeclareAlertHostilityButtonEvent() ;
			break ;
	}	
}

VOID		CRYLGuildInfoPanel::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pGuildInfoPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLGUILDPANEL::GUILDSEACH_EDIT	:	
		break ;
	}
}


HRESULT		CRYLGuildInfoPanel::UpdateGuildScreen() 
{
	CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
	CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;
	UCHAR ucTitle = pASDlg->GetTitle() ;
	
	// 길드에 가입이 안 되었을 경우에는 명성순으로 요청	, 가입 대기자 일 경우에도 그렇게 처리함.
	if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == 0 || CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
	{
		RequestGuildList() ;

		UpdateBtnState() ;
	}
	// 길드에 가입이 되었을 경우에는 자신의 길드순으로 요청
	else
	{
		m_iCurrentSort			= FindSortIndex( Guild::GUILD_MINE ) ;
		m_dwServerGuildPage		= 0 ;
		INT	CurrentPageLine		= 1 ;

		m_vecGuildInfo.clear() ;
		unsigned long ulGuildID = CRYLGameData::Instance()->m_csStatus.m_Info.GID;

		GUILDLARGEINFOEX*	pGuildInfo = FindGuild( ulGuildID ) ;
		if( !pGuildInfo )
		{
			// Rodin : 여기서 생성된 포인터는 관리가 제대로 되지 않습니다. (delete 없이 다른 값이 덮어버리면 낭패!)
			GUILDLARGEINFOEX*	pGuildInfo = new GUILDLARGEINFOEX;

			pGuildInfo->m_dwGID	= ulGuildID ;
			pGuildInfo->m_cLevel = 0 ;

			m_vecGuildInfo.push_back( pGuildInfo );
			InsertGuildInfoMap( *pGuildInfo ) ;

			SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				m_iSortMethod[ m_iCurrentSort ], m_dwServerGuildPage / 10, CurrentPageLine, (GuildSmallInfoNode*)pGuildInfo ) ;
		}
		else
		{
			m_vecGuildInfo.push_back( pGuildInfo );

			SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				m_iSortMethod[ m_iCurrentSort ], m_dwServerGuildPage / 10, CurrentPageLine, (GuildSmallInfoNode*)pGuildInfo ) ;
		}	

		UpdateBtnState() ;
	}

	return S_OK ;
}


std::vector< LPGUILDLARGEINFOEX >::iterator		CRYLGuildInfoPanel::FindGuildIter( LPGUILDLARGEINFOEX pGuildInfo ) 
{
	std::vector< LPGUILDLARGEINFOEX >::iterator	iter = m_vecGuildInfo.begin() ;

	while( iter != m_vecGuildInfo.end() )
	{
		if ( ( *iter ) == pGuildInfo )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecGuildInfo.end() ;
}

std::vector< LPGUILDLARGEINFOEX >::iterator		CRYLGuildInfoPanel::FindGuildIter( INT iIndex ) 
{
	std::vector< LPGUILDLARGEINFOEX >::iterator	iter = m_vecGuildInfo.begin() ;

	while( iter != m_vecGuildInfo.end() )
	{
		if ( ( *iter )->m_cIndex == iIndex )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecGuildInfo.end() ;
}

int		CRYLGuildInfoPanel::GetVectorIndex( unsigned long dwGID ) 
{
	std::vector< LPGUILDLARGEINFOEX >::iterator	iter = m_vecGuildInfo.begin() ;
	int iCnt = 0 ;

	while( iter != m_vecGuildInfo.end() )
	{
		unsigned long dwVecGID =  ( *iter )->m_dwGID ;
		if ( dwVecGID == dwGID )
		{
			return iCnt ;
		}
		++ iter ;
		iCnt ++ ;
	}

	return -1 ;
}

VOID	CRYLGuildInfoPanel::InsertGuildInfo( LPGUILDLARGEINFOEX pGuildInfo ) 
{
	m_vecGuildInfo.push_back( pGuildInfo ) ;
}

VOID		CRYLGuildInfoPanel::ProcessGuildInfo( unsigned char cSortCmd, 
												  unsigned char cPage, 
												  unsigned char cSmallNodeNum, 
												  GuildSmallInfoNode* lpSmallNode, 
												  unsigned char cLargeNodeNum, 
												  GuildLargeInfoNode* lpLargeNode )
{
	if ( cSortCmd != Guild::GUILD_REQUEST )
		m_iCurrentSort = FindSortIndex( cSortCmd ) ;

	m_iCurrentGID = 0 ;
	m_iCurrentSel = -1 ;

	m_dwStartGuildInfo = cPage * PktGuildList::MAX_NUM_PER_PAGE ;
	
	for ( int i = 0 ; i < cSmallNodeNum ; i ++ )
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( lpSmallNode[ i ].m_dwGID ) ;

		if ( iter != m_mapGuildInfo.end() )
		{
			iter->second->m_dwGID				= lpSmallNode[ i ].m_dwGID ;			// 길드 아이디	
			iter->second->m_cIndex				= lpSmallNode[ i ].m_cIndex ;			// 리스트상의 인덱스
			iter->second->m_wRank				= lpSmallNode[ i ].m_wRank ;			// 순위
			iter->second->m_dwFame				= lpSmallNode[ i ].m_dwFame ;			// 명성
			iter->second->m_cLevel				= lpSmallNode[ i ].m_cLevel ;			// 레벨
			iter->second->m_cCurrentMemberNum	= lpSmallNode[ i ].m_cCurrentMemberNum ;		// 인원수
			iter->second->m_cInclination		= lpSmallNode[ i ].m_cInclination ;		// 국적
								
			strncpy( iter->second->m_szName, lpSmallNode[ i ].m_szName, Guild::MAX_GUILD_NAME_LEN ) ;				// 길드 이름
			strncpy( iter->second->m_szMasterName, lpSmallNode[ i ].m_szMasterName, Guild::MAX_MEMBER_NAME_LEN ) ;	// 마스터 이름

			if (m_vecGuildInfo.size() <= lpSmallNode[ i ].m_cIndex + m_dwStartGuildInfo)
			{
				m_vecGuildInfo.push_back(iter->second);
			}
			else
			{
				INT Index = lpSmallNode[ i ].m_cIndex + m_dwStartGuildInfo ;
				m_vecGuildInfo[ Index ] = iter->second;
			}
		}
	}

	for (unsigned char i = 0 ; i < cLargeNodeNum ; i ++ )
	{
		GUILDLARGEINFOEX	Info ;
		memcpy( &Info, &lpLargeNode[ i ], sizeof( GuildLargeInfoNode ) ) ;
		InsertGuildInfoMap( Info ) ;
	}

	for (int nIndex = 0; nIndex < cLargeNodeNum ; ++nIndex )
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( lpLargeNode[ nIndex ].m_dwGID ) ;
		
		if ( iter != m_mapGuildInfo.end() )
		{
			if (m_vecGuildInfo.size() <= lpLargeNode[ nIndex ].m_cIndex + m_dwStartGuildInfo)
			{
				LPGUILDLARGEINFOEX lpGuildInfo = iter->second ;
				m_vecGuildInfo.push_back( lpGuildInfo ) ;
			}
			else
			{
				int iIndex = GetVectorIndex( lpLargeNode[ nIndex ].m_dwGID ) ;
				
				if ( iIndex == -1 )
				{
					m_vecGuildInfo.push_back( iter->second ) ;
				}
				else
				{
					m_vecGuildInfo[ iIndex ] = iter->second;
				}
			}
		}
	}	

	if ( m_pGMUIParentDlg->GetVisible() )
	{
		if ( m_vecGuildInfo.empty() ) 
		{
			m_pGuildListStringGrid->ClearItem() ;
			//return ;
		}

		// 자신의 길드정보 화면에 출력
		LPGUILDLARGEINFOEX lpGuildLargeInfo = GetGuild( CRYLGameData::Instance()->m_csStatus.m_Info.GID & ~Guild::MEMBER_WAIT_BIT ) ;

		if ( NULL != lpGuildLargeInfo && m_iSortMethod[ m_iCurrentSort ] == Guild::GUILD_MINE )
		{
			strncpy( m_GuildInfo.szGuildName, lpGuildLargeInfo->m_szName, Guild::MAX_GUILD_NAME_LEN ) ;
			strncpy( m_GuildInfo.szGuildMaster, lpGuildLargeInfo->m_szMasterName, Guild::MAX_MEMBER_NAME_LEN ) ;		
			sprintf( m_GuildInfo.szGuildFame, "%d", lpGuildLargeInfo->m_dwFame ) ;
			sprintf( m_GuildInfo.szGuildLevel, "%d", Guild::ms_aryMaxMemberNum[ lpGuildLargeInfo->m_cLevel ] ) ;
			sprintf( m_GuildInfo.szGuildMemberNum, "%d", lpGuildLargeInfo->m_cCurrentMemberNum ) ;
			switch ( lpGuildLargeInfo->m_cInclination )
			{
				case Creature::KARTERANT:
					sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2254 ]);
					break;

				case Creature::MERKADIA:
					sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2255 ]);
					break;

				case Creature::ALMIGHTY_PIRATE:
					sprintf( m_GuildInfo.szGuildInclination, CRYLStringTable::m_strString[ 2256 ]);
					break;

			}

			m_GuildInfo.pGuildMark = lpGuildLargeInfo->m_pSprite ;
			
			CRYLGameScene*			pScene	= static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
			CRYLGuildFrameDlg*		pDlg	= (CRYLGuildFrameDlg*) pScene->GetGuildFrameDlg() ;
			CRYLGuildPropertyPanel* pPanel	= (CRYLGuildPropertyPanel*) pDlg->GetGuildPropertyPanel() ;
			pPanel->SetGuildMark( m_GuildInfo.pGuildMark ) ;

			m_iCurrentGID = lpGuildLargeInfo->m_dwGID ;
			m_iCurrentSel = 0 ;
			m_iEntryGID = 0;
			
			UpdateMyGuildInfo( -1 ) ;
		}
	}

	m_dwMaxGuildInfo = m_mapGuildInfo.size() ;

	if(m_iCurrentSort != Guild::GUILD_MINE)
	{
		// 자기자신 길드면 소트를 다시해주면 안된다. 안나온다.
		// 새로 길드리스트를 소트한다.
		// edith 2008.09.25 길드리스트 정렬 문제 수정.
		ReSortGildInfoList() ;
	}

	// 화면에 업데이트한다.
	UpdateGuildStringGrid() ;
}

void	CRYLGuildInfoPanel::ClearList(void)
{
	if ( !m_mapGuildInfo.empty() )
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.begin() ;

		while ( iter != m_mapGuildInfo.end() )
		{
			GM_DELETE( iter->second->m_pTexture );
			if( iter->second->m_pSprite )
			{
				iter->second->m_pSprite->m_lpTexture = NULL;
			}
			GM_DELETE( iter->second->m_pSprite );
			GM_DELETE( iter->second );

			iter = m_mapGuildInfo.erase( iter ) ;
		}

		m_mapGuildInfo.clear() ;
	}

	for ( INT ispr = 0 ; ispr < 10 ; ++ispr )
	{
		m_pGuildMarkListSprite[ ispr ]->SetVisible( FALSE ) ;
		m_pGuildMarkListSprite[ ispr ]->DetachImageTexture() ;
	}

	if ( !m_vecGuildInfo.empty() )
	{
		m_vecGuildInfo.clear() ;
	}
}

VOID	CRYLGuildInfoPanel::UpdateGuildMsgBox() 
{
	if ( m_dwGuildEntryResult & MB_YES )
	{	
		SendPacket::CharGuildCmd( g_GameSession, m_iEntryGID, CRYLNetworkData::Instance()->m_dwMyChrID, 0, PktGuildCmd::GC_ASK_ENTER ) ;
		m_dwGuildEntryResult = 0L ;
	}
}

VOID	CRYLGuildInfoPanel::RequestGuildList()
{
	ReSortGildInfoList() ;
	GuildSmallInfoNode* pSmallNode = NULL ;
	INT CurrentPageLine = GetCurrentPageLine( m_dwPageState ) ;

	if ( !m_vecGuildInfo.empty() && CurrentPageLine != 0 )
	{
		if(m_vecGuildInfo.size() <= m_dwServerGuildPage)
			return;

		pSmallNode = new GuildSmallInfoNode[ CurrentPageLine ] ;
		
		for ( int i = 0 ; i < CurrentPageLine ; i ++ )
		{
			if( i + m_dwServerGuildPage >= m_vecGuildInfo.size() )
				break;

			pSmallNode[ i ].m_dwGID				= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_dwGID ;			// 길드 아이디	
			pSmallNode[ i ].m_cIndex			= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_cIndex ;			// 리스트상의 인덱스
			pSmallNode[ i ].m_wRank				= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_wRank ;			// 순위
			pSmallNode[ i ].m_dwFame			= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_dwFame ;			// 명성
			pSmallNode[ i ].m_cLevel			= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_cLevel ;			// 레벨
			pSmallNode[ i ].m_cCurrentMemberNum	= m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_cCurrentMemberNum ;		// 인원수
																					
			// 마스터 이름
			strncpy( pSmallNode[ i ].m_szMasterName, m_vecGuildInfo[ i + m_dwServerGuildPage ]->m_szMasterName, 
				Guild::MAX_MEMBER_NAME_LEN ) ;															
		}
	}
/*
	if(m_dwPageState == GUILDINFOPAGEEX::PAGE_NEXT)
	{
		if(m_dwServerGuildPage/10 > 0)
		{
			if(m_dwServerGuildPage % 10 == 0)
			{
				return;
			}
		}
	}
*/

	SendPacket::CharGuildList( g_GameSession, 
		CRYLNetworkData::Instance()->m_dwMyChrID, m_iSortMethod[ m_iCurrentSort ], m_dwServerGuildPage / 10, CurrentPageLine, pSmallNode ) ;

	GM_DELETE_ARRAY( pSmallNode ) ;
}

HRESULT		CRYLGuildInfoPanel::InsertGuildInfoMap( const GUILDLARGEINFOEX& guildInfo ) 
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( guildInfo.m_dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
	{
		memcpy( iter->second, &guildInfo, sizeof( GuildLargeInfoNode ) ) ;
		UpdateGuildMark( iter->second->m_dwGID, ( BYTE* )iter->second->m_szMark, -1 ) ;
		return S_OK ;
	}

	LPGUILDLARGEINFOEX pGuild = new GUILDLARGEINFOEX ;
	memcpy( pGuild, &guildInfo, sizeof( GUILDLARGEINFOEX ) ) ;

	m_mapGuildInfo.insert( pair< INT, LPGUILDLARGEINFOEX >( guildInfo.m_dwGID, pGuild ) ) ;
	
	UpdateGuildMark( pGuild->m_dwGID, ( BYTE* )pGuild->m_szMark, -1 ) ;

	return S_OK ;	
}

HRESULT		CRYLGuildInfoPanel::DeleteGuildInfoMap( DWORD dwGID )
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.find( dwGID ) ;
	
	if ( iter != m_mapGuildInfo.end() )
	{
		std::vector< LPGUILDLARGEINFOEX >::iterator		pos = FindGuildIter( iter->second ) ;

		if( dwGID == CRYLGameData::Instance()->m_csStatus.m_Info.GID )
		{
			CRYLGameScene*			pScene	= static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
			CRYLGuildFrameDlg*		pDlg	= (CRYLGuildFrameDlg*) pScene->GetGuildFrameDlg() ;
			CRYLGuildPropertyPanel* pPanel	= (CRYLGuildPropertyPanel*) pDlg->GetGuildPropertyPanel() ;
			pPanel->SetGuildMark( NULL ) ;
		}
		
		GM_DELETE( iter->second->m_pTexture );
		if( iter->second->m_pSprite )
			iter->second->m_pSprite->m_lpTexture = NULL;
		GM_DELETE( iter->second->m_pSprite );
		GM_DELETE( iter->second );
		m_mapGuildInfo.erase( iter ) ;

		if ( pos != m_vecGuildInfo.end() ) m_vecGuildInfo.erase( pos ) ;

		UpdateGuildStringGrid() ;
		return S_OK ;
	}

	return S_FALSE ;
}

VOID
CRYLGuildInfoPanel::DeleteMyGuildInfo( DWORD dwGID )
{
	if( (dwGID & ~Guild::MEMBER_WAIT_BIT) == CRYLGameData::Instance()->m_csStatus.m_Info.GID )
	{
		CRYLGameScene*			pScene	= static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
		CRYLGuildFrameDlg*		pDlg	= (CRYLGuildFrameDlg*) pScene->GetGuildFrameDlg() ;
		CRYLGuildPropertyPanel* pPanel	= (CRYLGuildPropertyPanel*) pDlg->GetGuildPropertyPanel() ;
		pPanel->SetGuildMark( NULL ) ;
	}

	UpdateGuildScreen() ;
}

// 명성순
struct CompareFame
{
	bool operator () (const std::map< INT, LPGUILDLARGEINFOEX >::value_type& lhs, const std::map< INT, LPGUILDLARGEINFOEX >::value_type& rhs) const
	{
		if ( !rhs.second || !lhs.second )
			return false ;

		return lhs.second->m_dwFame > rhs.second->m_dwFame; 
	}
};

// 길드이름순
struct CompareGuildName
{
	bool operator () (const std::map< INT, LPGUILDLARGEINFOEX >::value_type& lhs, const std::map< INT, LPGUILDLARGEINFOEX >::value_type& rhs) const
	{
		if ( !rhs.second || !lhs.second )
			return false ;

		return std::string(lhs.second->m_szName) < std::string(rhs.second->m_szName) ; 
	}
};

struct CompareVectorGuildName
{
	bool operator () (const LPGUILDLARGEINFOEX lhs, const LPGUILDLARGEINFOEX rhs) const
	{
		return std::string(lhs->m_szName) < std::string(rhs->m_szName) ; 
	}
};

HRESULT		CRYLGuildInfoPanel::ReSortGildInfoList() 
{
	switch ( m_iSortMethod[ m_iCurrentSort ] )
	{
		// 명성순
		case Guild::GUILD_FAME :
			SortGuildFame() ;
			break ;

		// 길드이름순
		case Guild::GUILD_NAME : 
			SortGuildName() ;
			break ;

		// 적대길드
		case Guild::GUILD_HOSTILITY :
			SortHostilityGuild() ;
			break ;

		// 비적대길드
		case Guild::GUILD_NEUTRALITY :
			SortNeutralityGuild() ;
			break ;

		// 자신의 길드
		case Guild::GUILD_MINE : 
			SortMyGuild() ;
			break ;
	}

	return S_OK ;
}

VOID		CRYLGuildInfoPanel::SortGuildFame()
{
	// 명성순으로 소트
	typedef std::pair<INT, LPGUILDLARGEINFOEX > GuildPair;
	unsigned short wGuildNum = static_cast<unsigned short>(m_mapGuildInfo.size());

	std::vector<GuildPair> sortVector;
	GuildPair NullPair(0, (LPGUILDLARGEINFOEX)NULL);
	sortVector.reserve(wGuildNum);
	sortVector.assign(wGuildNum, NullPair);

	std::partial_sort_copy(m_mapGuildInfo.begin(), m_mapGuildInfo.end(), 
	sortVector.begin(), sortVector.end(), CompareFame());

	m_vecGuildInfo.clear() ;

	LPGUILDLARGEINFOEX pInfo;

	for (size_t nIndex = 0; nIndex < m_mapGuildInfo.size() ; ++nIndex)
	{
		pInfo = sortVector[nIndex].second;

		m_vecGuildInfo.push_back( sortVector[nIndex].second ) ;
	}
}

VOID		CRYLGuildInfoPanel::SortGuildName()
{
	// 이름순으로 소트
	typedef std::pair<INT, LPGUILDLARGEINFOEX > GuildPair;
	unsigned short wGuildNum = static_cast<unsigned short>(m_mapGuildInfo.size());

	std::vector<GuildPair> sortVector;
	GuildPair NullPair(0, (LPGUILDLARGEINFOEX)NULL);
	sortVector.reserve(wGuildNum);
	sortVector.assign(wGuildNum, NullPair);

	std::partial_sort_copy(m_mapGuildInfo.begin(), m_mapGuildInfo.end(), 
	sortVector.begin(), sortVector.end(), CompareGuildName());

	m_vecGuildInfo.clear() ;

	for (size_t nIndex = 0; nIndex < m_mapGuildInfo.size() ; ++nIndex)
	{
		m_vecGuildInfo.push_back( sortVector[nIndex].second ) ;
	}
}

VOID		CRYLGuildInfoPanel::SortHostilityGuild()
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator iter = m_mapGuildInfo.begin() ;
	m_vecGuildInfo.clear() ;

	INT iCount = 0 ;
	while ( iter != m_mapGuildInfo.end() )
	{
		LPGUILDLARGEINFOEX pInfo = iter->second;

		if ( Guild::HOSTILITY == iter->second->m_cRelationByMine ||				// 대상 길드가 나에게 적대를 걸은 경우
			 Guild::ALERT_HOSTILITY == iter->second->m_cRelationByMine ||
			 Guild::COUNTER_HOSTILITY == iter->second->m_cRelationByMine ||
			 Guild::TARGET_NEUTRALITY == iter->second->m_cRelationByMine ||
			 Guild::HOSTILITY == iter->second->m_cRelationByTarget ||			// 내가 적대를 걸은 경우
			 Guild::ALERT_HOSTILITY == iter->second->m_cRelationByTarget ||
			 Guild::COUNTER_HOSTILITY == iter->second->m_cRelationByTarget ||
			 Guild::TARGET_NEUTRALITY == iter->second->m_cRelationByTarget
			 )
		{
			m_vecGuildInfo.push_back( iter->second ) ;
		}

		++ iter ;
	}

	std::sort( m_vecGuildInfo.begin(), m_vecGuildInfo.end(), CompareVectorGuildName() ) ;
}

VOID		CRYLGuildInfoPanel::SortNeutralityGuild()
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator iter = m_mapGuildInfo.begin() ;
	m_vecGuildInfo.clear() ;

	INT iCount = 0 ;
	while ( iter != m_mapGuildInfo.end() )
	{
		if ( Guild::NEUTRALITY == iter->second->m_cRelationByMine &&
			 Guild::NEUTRALITY == iter->second->m_cRelationByTarget )
		{
			m_vecGuildInfo.push_back( iter->second ) ;
		}

		++ iter ;
	}

	std::sort( m_vecGuildInfo.begin(), m_vecGuildInfo.end(), CompareVectorGuildName() ) ;
}

VOID		CRYLGuildInfoPanel::SortMyGuild()
{
	m_vecGuildInfo.clear() ;
}

INT		CRYLGuildInfoPanel::GetCurrentPageLine( INT bPageState )
{
	if ( m_vecGuildInfo.empty() )
		return 0 ;

	switch ( bPageState )
	{
	case GUILDINFOPAGEEX::PAGE_NEXT :
		m_dwServerGuildPage += 10 ;
		break ;

	case GUILDINFOPAGEEX::PAGE_PREV :
		if ( m_dwServerGuildPage >= 10 )
			m_dwServerGuildPage -= 10 ;
		break ;

	case GUILDINFOPAGEEX::PAGE_CURRENT :
		m_dwServerGuildPage = m_dwStartGuildInfo ;
		break ;
	}

	// 첫 페이지가 10개 미만일때
	if ( m_vecGuildInfo.size() < 10 )
	{
		m_dwServerGuildPage = 0 ;
		m_dwStartGuildInfo  = 0 ;
		return m_vecGuildInfo.size() ;
	}

	INT iResultPage = m_vecGuildInfo.size() - m_dwServerGuildPage ;

	// 마지막 페이지
	if ( iResultPage < 10 )
	{
		// 음수일때
		if ( iResultPage < 0 )
		{
			m_iCurrentLine = m_vecGuildInfo.size() % 10 ;
			m_dwServerGuildPage = m_dwStartGuildInfo ;
		}
		else
		{
			 m_iCurrentLine = iResultPage ;
		}
	}
	else
	{
		m_iCurrentLine = 10 ;
	}
	
	return m_iCurrentLine ;
}

HRESULT		CRYLGuildInfoPanel::UpdateGuildMark( DWORD dwGID, BYTE* wBuff, DWORD dwGold ) 
{
	if ( wBuff[ 0 ] == 0x00 )				return E_FAIL ;

	if ( dwGold != -1 && CRYLGameData::Instance()->m_csStatus.m_Info.GID == dwGID )
	{
		CRYLGameScene*		 	pScene 		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 	pDlg   		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildPropertyPanel* pProPanel	= ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;
		pProPanel->SetCurrentGold( dwGold ) ;	
	}

	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;
	
	if ( iter != m_mapGuildInfo.end() )
	{
		memcpy( iter->second->m_szMark, wBuff, sizeof( BYTE ) * 433 ) ;

		if ( !iter->second->m_pSprite )
		{
			iter->second->m_pSprite		= new CRYLSprite ;
			iter->second->m_pTexture	= new CTexture ;

			iter->second->m_pTexture->CreateEmptyTexture( 32, 16, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED ) ;
			iter->second->m_pTexture->SetBitTexture16( 18, 12, ( WORD * )iter->second->m_szMark ) ;
			iter->second->m_pSprite->Create( 707, 388, 0, 0, 18, 12, iter->second->m_pTexture ) ;
		}
		else
		{
			iter->second->m_pTexture->SetBitTexture16( 18, 12, ( WORD * )iter->second->m_szMark ) ;
			iter->second->m_pSprite->Create( 707, 388, 0, 0, 18, 12, iter->second->m_pTexture ) ;
		}

		return S_OK ;
	}
	else
    {
		GUILDLARGEINFOEX		guildInfo ;

		guildInfo.m_dwGID				= dwGID ;
		guildInfo.m_cIndex				= 0 ;
		guildInfo.m_wRank				= 0 ;	
		guildInfo.m_dwFame				= 0 ;	
		guildInfo.m_cLevel				= 0 ;	
		guildInfo.m_cCurrentMemberNum	= 0 ;
		guildInfo.m_cInclination		= 0 ;

		std::fill_n( guildInfo.m_szName, int( Guild::MAX_GUILD_NAME_LEN ), 0 ) ;
		std::fill_n( guildInfo.m_szMasterName, int( Guild::MAX_MEMBER_NAME_LEN ), 0 ) ;
		memcpy( guildInfo.m_szMark, wBuff, sizeof( WORD ) * 216 ) ;

		guildInfo.m_cRelationByMine		= Guild::NEUTRALITY ;
		guildInfo.m_cRelationByTarget	= Guild::NEUTRALITY ;

		InsertGuildInfoMap( guildInfo ) ;

		std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

		if ( iter != m_mapGuildInfo.end() )
		{
			// Rodin : 이거 이미 있으면 새로 생성할 필요가 없지 않나요? 
			iter->second->m_pSprite		= new CRYLSprite ;
			iter->second->m_pTexture	= new CTexture ;

			iter->second->m_pTexture->CreateEmptyTexture( 32, 16, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED ) ;
			iter->second->m_pTexture->SetBitTexture16( 18, 12, ( WORD* )iter->second->m_szMark ) ;
			iter->second->m_pSprite->Create( 707, 388, 0, 0, 18, 12, iter->second->m_pTexture ) ;
		}
	}

	return E_FAIL ;
}

HRESULT		CRYLGuildInfoPanel::UpdateGuildMark( DWORD dwGID ) 
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
	{
		if ( !iter->second->m_pSprite )
		{
			iter->second->m_pSprite		= new CRYLSprite ;
			iter->second->m_pTexture	= new CTexture ;

			iter->second->m_pTexture->CreateEmptyTexture( 32, 16, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED ) ;
			iter->second->m_pTexture->SetBitTexture16( 18, 12, ( WORD * )iter->second->m_szMark ) ;
			iter->second->m_pSprite->Create( 707, 388, 0, 0, 18, 12, iter->second->m_pTexture ) ;
		}
		else
		{
			iter->second->m_pTexture->SetBitTexture16( 18, 12, ( WORD * )iter->second->m_szMark ) ;
			iter->second->m_pSprite->Create( 707, 388, 0, 0, 18, 12, iter->second->m_pTexture ) ;
		}
	}
	return S_OK ;
}

HRESULT		CRYLGuildInfoPanel::UpdateGuildLevel( DWORD dwGID, BYTE byLevel, DWORD dwGold ) 
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
	{
		iter->second->m_cLevel = byLevel ;
		
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildPropertyPanel* pProPanel = ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;

		pProPanel->SetCurrentGold( dwGold ) ;	

		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		CHAR szBuff[ 128 ] ;
		sprintf( szBuff, CRYLStringTable::m_strString[1093], (INT)byLevel ) ;
		lpMessageBox->Create( szBuff ) ;
	}

	return S_OK ;
}

HRESULT		CRYLGuildInfoPanel::RequestNewGuild( std::set< unsigned long > setGID ) 
{
	if ( true == setGID.empty() ) 
	{
		return E_FAIL ;
	}

	GuildSmallInfoNode*		pSmallNode = NULL ;
	LPGUILDLARGEINFOEX		pGuild = NULL ;
	INT						iCount = 0 ;

	pSmallNode = new GuildSmallInfoNode[ setGID.size() ] ;

	for (std::set< unsigned long >::iterator iter = setGID.begin(); iter != setGID.end(); ++iter)
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator itTmp = m_mapGuildInfo.find( ( *iter ) ) ;

		if ( itTmp == m_mapGuildInfo.end() )
		{
			pSmallNode[ iCount ].m_dwGID = ( *iter );			
			++iCount ;
		}
	}

	if ( 0 == iCount )
	{
		delete [] pSmallNode ;
		return E_FAIL ;
	}

	if ( iCount < 10 )
	{
		SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
			Guild::GUILD_REQUEST, 0, iCount, pSmallNode ) ;
		delete [] pSmallNode ;
		return S_OK ;
	}

	INT iCurrentPt	= 0 ;
	INT iPage		= 0 ;
	for ( INT i = 0 ; i < iCount ; i += 10 )
	{
		iCurrentPt = iCount - i ;
		if ( iCurrentPt < 10 )
		{
			SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				Guild::GUILD_REQUEST, iPage, iCurrentPt, &pSmallNode[ iCurrentPt ] ) ;
			break ;
		}
		else
		{
			SendPacket::CharGuildList( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				Guild::GUILD_REQUEST, iPage, 10, &pSmallNode[ i ] ) ;		
		}

		iPage ++ ;
	}
	

	delete [] pSmallNode ;
	return S_OK ;
}

CRYLSprite*		CRYLGuildInfoPanel::GetGuildSprite( DWORD dwGID ) 
{
	// edith 2009.08.20 길드 아이콘 이미지 (가입대기자이면 스프라이트를 보여주지 않는다)
	if ( dwGID == 0 || dwGID & Guild::MEMBER_WAIT_BIT )
		return NULL ;

	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
		return iter->second->m_pSprite ;

	return NULL ;
}

const char*		CRYLGuildInfoPanel::GetGuildName( DWORD dwGID )
{
	// edith 2009.08.20 길드 아이콘 이미지 (가입대기자이면 스프라이트를 보여주지 않는다)
	if ( dwGID == 0 || dwGID & Guild::MEMBER_WAIT_BIT )
		return NULL ;

	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
		return iter->second->m_szName ;

	return NULL ;
}

BOOL		CRYLGuildInfoPanel::GetUseGID( DWORD dwGID )	
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator		iter = m_mapGuildInfo.find( dwGID ) ;

	if ( iter != m_mapGuildInfo.end() )
		return TRUE ;

	return FALSE ;
}

VOID	
CRYLGuildInfoPanel::SetVisible( BOOL bVisible ) 
{
	UpdateBtnState() ;

	CGMUICustomPanel::SetVisible( bVisible );
	return;
}
	
VOID		
CRYLGuildInfoPanel::Hide( CGMUIObject* Sender )
{
	CGMUICustomPanel::Hide( Sender );

	return;
}

VOID	
CRYLGuildInfoPanel::ProcessGuildInclination( unsigned long dwGID, unsigned char cInclination )
{
	std::map< INT, LPGUILDLARGEINFOEX >::iterator iter = m_mapGuildInfo.find( dwGID ) ;
	if ( iter != m_mapGuildInfo.end() )
	{
		iter->second->m_cInclination = cInclination ;
	}

	LPGUILDLARGEINFOEX pInfo = GetGuild( dwGID & ~Guild::MEMBER_WAIT_BIT );
	if ( NULL != pInfo )
	{
		pInfo->m_cInclination = cInclination ;

		// 자신의 길드 일때
		if ( dwGID == (CRYLGameData::Instance()->m_csStatus.m_Info.GID & ~Guild::MEMBER_WAIT_BIT)	 )
		{
			CRYLGuildPropertyPanel*	pProperty = (CRYLGuildPropertyPanel*) ((CRYLGuildFrameDlg*)m_pGMUIParentDlg)->GetGuildPropertyPanel();
			pProperty->SetInclination( cInclination ) ;

			UpdateMyGuildInfo( -1 );

			CHAR buff[256];
			CHAR inclination[24];

			switch ( pInfo->m_cInclination )
			{
				case Creature::KARTERANT:
					sprintf( inclination, CRYLStringTable::m_strString[ 2254 ]);
					break;

				case Creature::MERKADIA:
					sprintf( inclination, CRYLStringTable::m_strString[ 2255 ]);
					break;

				case Creature::ALMIGHTY_PIRATE:
					sprintf( inclination, CRYLStringTable::m_strString[ 2256 ]);
					break;

			}

			sprintf( buff, CRYLStringTable::m_strString[ 2297 ], pInfo->m_szName, inclination );
			CRYLChattingDlg::Instance()->AddMessage( buff, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
		}
	}
	
	UpdateGuildStringGrid();
	SelectGuildInfo();
}

INT		CRYLGuildInfoPanel::FindSortIndex( unsigned char cSortCmd )
{
	for ( int iIdx=0; iIdx<=GUILDINFOSORT::MAX_SORT_METHOD; ++iIdx )
	{
		if ( m_iSortMethod[ iIdx ] == cSortCmd )
		{
			return iIdx ;
		}
	}

	return 0 ;
}


VOID	CRYLGuildInfoPanel::UpdateBtnState()
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( !pSelfCreature ) return ;

	m_pGuildEntryButton->SetVisible( TRUE ) ;
	m_pGuildCancelButton->SetVisible( FALSE ) ;
	m_pGuildTacticsEntryButton->SetVisible( TRUE ) ;
	m_pGuildTacticsCancelButton->SetVisible( FALSE ) ;
	
	// 가입 대기자라면... (길드 혹은 용병 신청자) 
	if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
	{
		if ( pSelfCreature->GetTactics() == Guild::TACTICS_WAIT )
		{
			// 용병 신청자라면... 
			m_pGuildTacticsEntryButton->SetVisible( FALSE ) ;
			m_pGuildTacticsCancelButton->SetVisible( TRUE ) ;
		}
		else
		{
			m_pGuildEntryButton->SetVisible( FALSE ) ;
			m_pGuildCancelButton->SetVisible( TRUE ) ;
		}
	}

	m_pGuildHostilityInfoButton->SetEnable( FALSE ) ;

	m_pDeclareHostilityButton->SetEnable( FALSE ) ;
	m_pDeclarePeaceButton->SetEnable( FALSE ) ;
	m_pDeclareAlertHostilityButton->SetEnable( FALSE ) ;

	if ( 0 != CRYLGameData::Instance()->m_csStatus.m_Info.GID )
	{
		m_pGuildHostilityInfoButton->SetEnable( TRUE ) ;

		CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

		/* 
		// edith 2010.01.24 적대선언 못하게 비활성화
		UCHAR ucTitle = pASDlg->GetTitle() ;
		if ( ucTitle == Guild::MASTER )
		{
			// GUILD_TODO : 관계에 맞게 활성화 / 비활성화
			m_pDeclareHostilityButton->SetEnable( TRUE ) ;
			m_pDeclarePeaceButton->SetEnable( TRUE ) ;
			m_pDeclareAlertHostilityButton->SetEnable( TRUE ) ;
		}
		*/
	}
}


VOID	CRYLGuildInfoPanel::ClearRelationInfo()
{
	if ( !m_mapGuildInfo.empty() )
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator	iter = m_mapGuildInfo.begin() ;
		while ( iter != m_mapGuildInfo.end() )
		{
			iter->second->m_cRelationByMine = 0 ;
			iter->second->m_cRelationByTarget = 0;

			++iter ;
		}
	}
}

VOID	CRYLGuildInfoPanel::UpdateRelationInfo( unsigned short wNodeNum, GuildRelationInfoNode* lpRelationNode )
{
	if ( 0 == wNodeNum )
		return ;

	for ( unsigned short wIndex = 0; wIndex < wNodeNum; ++wIndex )
	{
		std::map< INT, LPGUILDLARGEINFOEX >::iterator iter = m_mapGuildInfo.find( lpRelationNode[ wIndex ].m_dwGID ) ;
		if ( iter != m_mapGuildInfo.end() )
		{
			LPGUILDLARGEINFOEX lpGuildInfo = iter->second ;
			if ( lpGuildInfo )
			{
				lpGuildInfo->m_cRelationByMine = lpRelationNode[ wIndex ].m_cRelationByMine ;
				lpGuildInfo->m_cRelationByTarget = lpRelationNode[ wIndex ].m_cRelationByTarget ;
			}
		}
	}

	if(m_iCurrentSel!=-1 && m_vecGuildInfo.size() > (unsigned int)m_iCurrentSel)
	{
		LPGUILDLARGEINFOEX lpGuildInfo = m_vecGuildInfo[ m_iCurrentSel ] ;
		if ( lpGuildInfo )
		{
			if ( lpGuildInfo->m_dwGID == CRYLGameData::Instance()->m_csStatus.m_Info.GID )
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3954] ) ;
			}
			else if ( Guild::NEUTRALITY == lpGuildInfo->m_cRelationByMine && 
					Guild::NEUTRALITY == lpGuildInfo->m_cRelationByTarget )
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3955] ) ;
			}
			else
			{
				m_pGuildRelationValueLabel->SetCaption( CRYLStringTable::m_strString[3956] ) ;
			}
		}
	}
}
