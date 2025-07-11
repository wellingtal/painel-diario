
#include <winsock2.h>

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include "GMUIScreen.h"
#include "GMImageDib.h"

#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLClientMain.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLMessageBox.h"
#include "RYLStringGrid.h"
#include "RYLSpriteEX.h"

#include "RYLChattingDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildMarkDlg.h"
#include "GMMemory.h"


CRYLGuildPropertyPanel*	g_pGuildPropertyPanel = NULL ;

CRYLGuildPropertyPanel::CRYLGuildPropertyPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	
	m_pStorehouseLabel						= NULL ;		// 길드 금고 / 창고 관리
	m_pGoldManagementLabel					= NULL ;		// 길드금고관리
	m_pCurrentGoldLabel						= NULL ;		// 현재 잔고
	m_pCurrentGoldValueLabel				= NULL ;		// 현재 잔고 값
	m_pPaymentGoldButton					= NULL ;		// 출금
	m_pDepositGoldButton					= NULL ;		// 입금
	m_pDivisionGoldButton					= NULL ;		// 분배
	m_pUseInquiryButton						= NULL ;		// 사용내역 조회

	m_pMarkNMemberNumLabel					= NULL ;		// 마크변경 / 인원수변경
	m_pMarkLabel							= NULL ;		// 길드 마크 변경
	m_pCurrentMarkLabel						= NULL ;		// 마크 현황
	m_pCurrentMarkSprite					= NULL ;		// 길드 마크 스프라이트
	m_pChangeMarkButton						= NULL ;		// 마크 변경 버튼

	m_pMemberNumLabel						= NULL ;		// 길드 최대 인원수 변경
	m_pCurrentMemberNumLabel				= NULL ;		// 길드원 최대 수용 인원
	m_pCurrentMemberNumValueLabel			= NULL ;		// 길드원 최대 수용 인원 값
	m_pChangeMemberNumButton				= NULL ;		// 인원 증대 버튼

	m_dwCurrentMemberNum			= 0L ;
	m_dwGuildLevel					= 0L ;
	m_dwGuildFame					= 0L ;

	m_dwGuildMarkResult				= 0L ;
	m_dwGuildLevelResult			= 0L ;

	// 입금
	m_dwRstInputGold				= 0L ;
	m_dwValInputGold				= 0L ;

	// 출금
	m_dwRstOutputGold				= 0L ;
	m_dwValOutputGold				= 0L ;

	// 방출
	m_dwRstEmissionGold				= 0L ;
	m_dwValEmissionGold				= 0L ;

	m_iCurrentGold					= 0L ;

	m_bMessageBox					= false ;
    
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
	
	// 길드금고 / 창고관리
	pImageDIB->MakeFrameRect( 7,  27, 313, 105, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 14, 53,  299,  70, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 85, 61,  124,  19, 0xff959595, 0xc0000000,  1 ) ;

	// 마크변경 / 인원수변경
	pImageDIB->MakeFrameRect(   7, 160, 313, 134, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  14, 186, 299,  37, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  93, 194,  37,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  14, 248, 299,  37, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 159, 256,  38,  19, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 328 ) ;
	SetHeight( 375 ) ;

	g_pGuildPropertyPanel = this ;
	m_ucInclination = 0;
	m_ucSelectInclination = 0;
}

CRYLGuildPropertyPanel::~CRYLGuildPropertyPanel() 
{
	Clear() ;
	FinalCtrl() ;
}


VOID	CRYLGuildPropertyPanel::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	RECT rt ;

	// 길드 금고 / 창고 관리
	m_pStorehouseLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pStorehouseLabel->SetLeft( 10 ) ;
	m_pStorehouseLabel->SetTop( 11 ) ;
	m_pStorehouseLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 11, 11, 200, 22 ) ;
	m_pStorehouseLabel->SetAlignRect( rt ) ;
	m_pStorehouseLabel->SetAlign( LEFT ) ;
	m_pStorehouseLabel->SetCaption( CRYLStringTable::m_strString[ 2060 ] ) ;
	m_pStorehouseLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pStorehouseLabel ) ;

	// 길드금고관리
	m_pGoldManagementLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldManagementLabel->SetLeft( 22 ) ;
	m_pGoldManagementLabel->SetTop( 37 ) ;
	m_pGoldManagementLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 23, 37, 134, 48 ) ;
	m_pGoldManagementLabel->SetAlignRect( rt ) ;
	m_pGoldManagementLabel->SetAlign( LEFT ) ;
	m_pGoldManagementLabel->SetCaption( CRYLStringTable::m_strString[ 2064 ] ) ;
	m_pGoldManagementLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldManagementLabel ) ;

	// 현재 잔고
	m_pCurrentGoldLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGoldLabel->SetLeft( 31 ) ;
	m_pCurrentGoldLabel->SetTop( 65 ) ;
	m_pCurrentGoldLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 32, 66, 82, 76 ) ;
	m_pCurrentGoldLabel->SetAlignRect( rt ) ;
	m_pCurrentGoldLabel->SetAlign( LEFT ) ;
	m_pCurrentGoldLabel->SetCaption( CRYLStringTable::m_strString[ 2065 ] ) ;
	m_pCurrentGoldLabel->SetAutoSize( TRUE ) ;
	m_pCurrentGoldLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1864], 1864, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGoldLabel ) ;

	// 현재 잔고 값
	m_pCurrentGoldValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGoldValueLabel->SetLeft( 92 ) ;
	m_pCurrentGoldValueLabel->SetTop( 66 ) ;
	m_pCurrentGoldValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 85, 66, 208, 76 ) ;
	m_pCurrentGoldValueLabel->SetAlignRect( rt ) ;
	m_pCurrentGoldValueLabel->SetAlign( CENTER ) ;
	m_pCurrentGoldValueLabel->SetCaption( "" ) ;
	m_pCurrentGoldValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGoldValueLabel ) ;

	// 출금
	m_pPaymentGoldButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPaymentGoldButton->SetLeft( 25 ) ;
	m_pPaymentGoldButton->SetTop( 90 ) ;
	m_pPaymentGoldButton->SetCaption( CRYLStringTable::m_strString[ 2066 ] ) ;
	m_pPaymentGoldButton->SetFontColor( 0xffffffff) ;
	m_pPaymentGoldButton->SetColor( 0xffffffff) ;
	m_pPaymentGoldButton->OnClickEvent = DlgClick ;
	m_pPaymentGoldButton->SetInstance( RYLGUILDPROPERTYPANEL::PAYMENTGOLD_BUTTON ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPaymentGoldButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1865], 1865, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pPaymentGoldButton ) ;
	
	// 입금
	m_pDepositGoldButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDepositGoldButton->SetLeft( 95 ) ;
	m_pDepositGoldButton->SetTop( 90 ) ;
	m_pDepositGoldButton->SetCaption( CRYLStringTable::m_strString[ 2067 ] ) ;
	m_pDepositGoldButton->SetFontColor( 0xffffffff) ;
	m_pDepositGoldButton->SetColor( 0xffffffff) ;
	m_pDepositGoldButton->OnClickEvent = DlgClick ;
	m_pDepositGoldButton->SetInstance( RYLGUILDPROPERTYPANEL::DEPOSITGOLD_BUTTON ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDepositGoldButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1866], 1866, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pDepositGoldButton ) ;

	// 사용내역 조회
	m_pUseInquiryButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pUseInquiryButton->SetLeft( 165 ) ;
	m_pUseInquiryButton->SetTop( 90 ) ;
	m_pUseInquiryButton->SetCaption( CRYLStringTable::m_strString[ 2068 ] ) ;
	m_pUseInquiryButton->SetFontColor( 0xffffffff) ;
	m_pUseInquiryButton->SetColor( 0xffffffff) ;
	m_pUseInquiryButton->SetEnable( FALSE ) ;
	m_pUseInquiryButton->OnClickEvent = DlgClick ;
	m_pUseInquiryButton->SetInstance( RYLGUILDPROPERTYPANEL::USEINQUIRY_BUTTON ) ;
	m_pUseInquiryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pUseInquiryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pUseInquiryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pUseInquiryButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pUseInquiryButton ) ;

	// 방출
	m_pDivisionGoldButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDivisionGoldButton->SetLeft( 235 ) ;
	m_pDivisionGoldButton->SetTop( 90 ) ;
	m_pDivisionGoldButton->SetCaption( CRYLStringTable::m_strString[ 2069 ] ) ;
	m_pDivisionGoldButton->SetFontColor( 0xffffffff) ;
	m_pDivisionGoldButton->SetColor( 0xffffffff) ;
	m_pDivisionGoldButton->OnClickEvent = DlgClick ;
	m_pDivisionGoldButton->SetInstance( RYLGUILDPROPERTYPANEL::DIVISIONGOLD_BUTTON ) ;
	m_pDivisionGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDivisionGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDivisionGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDivisionGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDivisionGoldButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1867], 1867, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pDivisionGoldButton ) ;

	// 마크변경 / 인원수변경
	m_pMarkNMemberNumLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMarkNMemberNumLabel->SetLeft( 12 ) ;
	m_pMarkNMemberNumLabel->SetTop( 144 ) ;
	m_pMarkNMemberNumLabel->SetFontColor( 0xffffffff ) ;
	m_pMarkNMemberNumLabel->SetCaption( CRYLStringTable::m_strString[3974] ) ;
	m_pMarkNMemberNumLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pMarkNMemberNumLabel ) ;

	// 길드 마크 변경
	m_pMarkLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMarkLabel->SetLeft( 24 ) ;
	m_pMarkLabel->SetTop( 170 ) ;
	m_pMarkLabel->SetFontColor( 0xffffffff ) ;
	m_pMarkLabel->SetCaption( CRYLStringTable::m_strString[3975] ) ;
	m_pMarkLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pMarkLabel ) ;

	// 마크 현황
	m_pCurrentMarkLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentMarkLabel->SetLeft( 32 ) ;
	m_pCurrentMarkLabel->SetTop( 198 ) ;
	m_pCurrentMarkLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentMarkLabel->SetCaption( CRYLStringTable::m_strString[3976] ) ;
	m_pCurrentMarkLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentMarkLabel ) ;

	// 길드 마크 스프라이트
	m_pCurrentMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCurrentMarkSprite->SetLeft( 100 ) ;
	m_pCurrentMarkSprite->SetTop( 198 ) ;
	m_pCurrentMarkSprite->SetWidth( 18 ) ;
	m_pCurrentMarkSprite->SetHeight( 12 ) ;
	CGMUICustomPanel::AddChild( m_pCurrentMarkSprite ) ;

	// 마크 변경 버튼
	m_pChangeMarkButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pChangeMarkButton->SetLeft( 138 ) ;
	m_pChangeMarkButton->SetTop( 192 ) ;
	m_pChangeMarkButton->SetCaption( CRYLStringTable::m_strString[3977] ) ;
	m_pChangeMarkButton->SetFontColor( 0xffffffff ) ;
	m_pChangeMarkButton->SetColor( 0xffffffff ) ;
	m_pChangeMarkButton->OnClickEvent = DlgClick ;
	m_pChangeMarkButton->SetInstance( RYLGUILDPROPERTYPANEL::GUILDMARK_BUTTON ) ;
	m_pChangeMarkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangeMarkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangeMarkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangeMarkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pChangeMarkButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1858 ], 1858, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pChangeMarkButton ) ;

	// 길드 최대 인원수 변경
	m_pMemberNumLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMemberNumLabel->SetLeft( 24 ) ;
	m_pMemberNumLabel->SetTop( 232 ) ;
	m_pMemberNumLabel->SetFontColor( 0xffffffff ) ;
	m_pMemberNumLabel->SetCaption( CRYLStringTable::m_strString[3978] ) ;
	m_pMemberNumLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pMemberNumLabel ) ;

	// 길드원 최대 수용 인원
	m_pCurrentMemberNumLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentMemberNumLabel->SetLeft( 32 ) ;
	m_pCurrentMemberNumLabel->SetTop( 260 ) ;
	m_pCurrentMemberNumLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentMemberNumLabel->SetCaption( CRYLStringTable::m_strString[3979] ) ;
	m_pCurrentMemberNumLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentMemberNumLabel ) ;

	// 길드원 최대 수용 인원 값
	m_pCurrentMemberNumValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentMemberNumValueLabel->SetLeft( 0 ) ;
	m_pCurrentMemberNumValueLabel->SetTop( 0 ) ;
	m_pCurrentMemberNumValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 159, 260, 196, 271 ) ;
	m_pCurrentMemberNumValueLabel->SetAlignRect( rt ) ;
	m_pCurrentMemberNumValueLabel->SetAlign( CENTER ) ;
	m_pCurrentMemberNumValueLabel->SetCaption( "" ) ;
	m_pCurrentMemberNumValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCurrentMemberNumValueLabel ) ;

	// 인원 증대 버튼
	m_pChangeMemberNumButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pChangeMemberNumButton->SetLeft( 205 ) ;
	m_pChangeMemberNumButton->SetTop( 254 ) ;
	m_pChangeMemberNumButton->SetCaption( CRYLStringTable::m_strString[3980] ) ;
	m_pChangeMemberNumButton->SetFontColor( 0xffffffff ) ;
	m_pChangeMemberNumButton->SetColor( 0xffffffff ) ;
	m_pChangeMemberNumButton->OnClickEvent = DlgClick ;
	m_pChangeMemberNumButton->SetInstance( RYLGUILDPROPERTYPANEL::GUILDLEVEL_BUTTON ) ;
	m_pChangeMemberNumButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangeMemberNumButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangeMemberNumButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangeMemberNumButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pChangeMemberNumButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1863], 1863, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pChangeMemberNumButton ) ;
}

VOID	CRYLGuildPropertyPanel::FinalCtrl() 
{
	GM_DELETE( m_pStorehouseLabel ) ;					
	GM_DELETE( m_pGoldManagementLabel ) ;				
	GM_DELETE( m_pCurrentGoldLabel	) ;					
	GM_DELETE( m_pCurrentGoldValueLabel ) ;				
	GM_DELETE( m_pPaymentGoldButton ) ;					
	GM_DELETE( m_pDepositGoldButton ) ;					
	GM_DELETE( m_pDivisionGoldButton ) ;					
	GM_DELETE( m_pUseInquiryButton ) ;		

	GM_DELETE( m_pMarkNMemberNumLabel ) ;
	GM_DELETE( m_pMarkLabel ) ;
	GM_DELETE( m_pCurrentMarkLabel ) ;
	GM_DELETE( m_pCurrentMarkSprite ) ;
	GM_DELETE( m_pChangeMarkButton ) ;

	GM_DELETE( m_pMemberNumLabel ) ;
	GM_DELETE( m_pCurrentMemberNumLabel ) ;
	GM_DELETE( m_pCurrentMemberNumValueLabel ) ;
	GM_DELETE( m_pChangeMemberNumButton ) ;
}

VOID	CRYLGuildPropertyPanel::Hide( CGMUIObject* Sender ) 
{
	// edith 2009.08.04 길드창에서 입,출금시 ESC를 눌러서 메시지 박스를 닫은후 메시지 박스 안뜨는 버그수정.
	m_bMessageBox = false;
}	

HRESULT		CRYLGuildPropertyPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
	CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

	UCHAR ucTitle = pASDlg->GetTitle() ;
	if ( ucTitle == Guild::MASTER )
	{
/*
		// 길드 마크 변경 버튼 (중국은 GM(운영자)만 길드 마크 변경이 가능하다.)
		if (CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::CHINA)
		{
			m_pChangeMarkButton->SetEnable( FALSE ) ;
		}
		else
		{
*/
			m_pChangeMarkButton->SetEnable( TRUE ) ;
//		}

		m_pChangeMemberNumButton->SetEnable( TRUE ) ;
	}
	else
	{
		m_pChangeMarkButton->SetEnable( FALSE ) ;
		m_pChangeMemberNumButton->SetEnable( FALSE ) ;
	}
/*
	if (CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::CHINA)
	{
		if ( g_ClientMain.m_wAdminMode != 0 )
		{
			// 운영자는 모든 길드의 마크를 변경할 수 있다.
			m_pChangeMarkButton->SetEnable( TRUE ) ;				
		}
	}
*/
	CHAR szBuff[ 32 ] ;
	sprintf( szBuff, "%d/%d", m_dwCurrentMemberNum, Guild::ms_aryMaxMemberNum[ m_dwGuildLevel ] ) ;
	m_pCurrentMemberNumValueLabel->SetCaption( szBuff ) ;
	m_pCurrentGoldValueLabel->SetCaption( CRYLStringTable::MakePrintGold( m_iCurrentGold ) ) ;

	UpdateCheckPropertyMsgBox() ;
	
	return S_OK ;
}

HRESULT		CRYLGuildPropertyPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLGuildPropertyPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID		CRYLGuildPropertyPanel::PaymentGold() 					// 출금
{
	m_dwRstOutputGold = 0L ;
	m_dwValOutputGold = 0L ;

	if(CRYLGameData::Instance()->m_bGuildSafeResult)
		return;

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ;
	lpMessageBox->Create( CRYLStringTable::m_strString[ 1180 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwRstOutputGold ) ;
	lpMessageBox->SetValue( &m_dwValOutputGold  ) ;
	m_bMessageBox = true;
}

VOID		CRYLGuildPropertyPanel::DepositGold() 					// 입금
{
	m_dwRstInputGold = 0L;
	m_dwValInputGold = 0L;

	if(CRYLGameData::Instance()->m_bGuildSafeResult)
		return;

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ;
	lpMessageBox->Create( CRYLStringTable::m_strString[ 1181 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwRstInputGold ) ;
	lpMessageBox->SetValue( &m_dwValInputGold  ) ;
	m_bMessageBox = true;
}

VOID		CRYLGuildPropertyPanel::DivisionGold() 					// 분배
{
	m_dwRstEmissionGold = 0L;
	m_dwValEmissionGold = 0L;

	if(CRYLGameData::Instance()->m_bGuildSafeResult)
		return;

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ;
	char buff[ 512 ] ;
	sprintf( buff, CRYLStringTable::m_strString[ 1182 ], CRYLStringTable::MakePrintGold( m_iCurrentGold ) ) ;
	lpMessageBox->Create( buff, MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwRstEmissionGold ) ;
	lpMessageBox->SetValue( &m_dwValEmissionGold  ) ;
	m_bMessageBox = true;
}

VOID		CRYLGuildPropertyPanel::UpdateCheckPropertyMsgBox() 
{
	// edith 2008.03.15 금고 입출금서비스 추가
	if(m_bMessageBox && !CRYLGameData::Instance()->m_bGuildSafeResult)
	{
		if ( m_dwRstOutputGold & MB_YES )
		{
			if(m_dwValOutputGold==0)
			{
				m_bMessageBox = false;
				return;
			}

			if ( m_iCurrentGold < m_dwValOutputGold )
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 1183 ] ) ;
				m_dwRstOutputGold = 0L ;
				m_bMessageBox = false;
				return ;
			}

			// 길드 결과패킷을 받기전에 사용못하게 제어
			CRYLGameData::Instance()->m_bGuildSafeResult = TRUE;

			// edith 2008.03.07 금고 입출금서비스
			SendPacket::CharGuildSafe( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwValOutputGold, PktGuildSafe::SC_WITHDRAWAL ) ;

			m_dwRstOutputGold = 0L ;
			m_dwValOutputGold = 0L ;
			m_bMessageBox = false;
			return ;
		}
		else if ( m_dwRstOutputGold & MB_NO || m_dwRstOutputGold & MB_EXIT )
		{
			m_dwRstOutputGold = 0L ;
			m_bMessageBox = false;
			return ;
		}

		if ( m_dwRstInputGold & MB_YES )
		{
			if(m_dwValInputGold==0)
			{
				m_bMessageBox = false;
				return;
			}

			if ( CRYLGameData::Instance()->m_csStatus.m_Info.Gold < m_dwValInputGold )
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 1184 ] ) ;
				m_dwRstInputGold = 0L ;
				m_bMessageBox = false;
				return ;
			}

			CRYLGameData::Instance()->m_bGuildSafeResult = TRUE;

			// 입금
			SendPacket::CharGuildSafe( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwValInputGold, PktGuildSafe::SC_DEPOSIT ) ;


			m_dwValInputGold = 0L ; 
			m_dwRstInputGold = 0L ;
			m_bMessageBox = false;
			return ;
		}
		else if ( m_dwRstInputGold & MB_NO || m_dwRstInputGold & MB_EXIT )
		{
			m_dwRstInputGold = 0L ;
			m_bMessageBox = false;
			return ;
		}

		if ( m_dwRstEmissionGold & MB_YES ) 
		{
			if(m_dwValEmissionGold==0)
			{
				m_bMessageBox = false;
				return;
			}

			if ( m_iCurrentGold < m_dwValEmissionGold )
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 1185 ] ) ;
				m_dwRstEmissionGold = 0L ;
				m_bMessageBox = false;
				return ;
			}

			CRYLGameData::Instance()->m_bGuildSafeResult = TRUE;

			// 분배
			SendPacket::CharGuildSafe( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwValEmissionGold, PktGuildSafe::SC_RELEASE ) ;
			m_dwValEmissionGold = 0L ;
			m_dwRstEmissionGold = 0L ;
			m_bMessageBox = false;
			return ;
		}
		else if ( m_dwRstEmissionGold & MB_NO || m_dwRstEmissionGold & MB_EXIT )
		{
			m_dwRstEmissionGold = 0L ;
			m_bMessageBox = false;
			return ;
		}
	}

	if ( m_dwGuildMarkResult & MB_YES )
	{
		if ( 0 != CRYLGameData::Instance()->m_csStatus.m_Info.GID )
		{
			CRYLGuildMarkDlg* lpGuildMarkDlg = ( CRYLGuildMarkDlg* )( ( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetGuildMarkDlg() ) ;
			lpGuildMarkDlg->SetGID( CRYLGameData::Instance()->m_csStatus.m_Info.GID ) ;
			lpGuildMarkDlg->SetVisible( TRUE ) ;
		}

		m_dwGuildMarkResult = 0L ;
	}

	if ( m_dwGuildLevelResult & MB_YES )
	{
		// 길드 레벨 변경
		SendPacket::CharGuildLevel( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwGuildLevel + 1 ) ;

		m_dwGuildLevelResult = 0L ;
	}
}

VOID	CRYLGuildPropertyPanel::SetSafeAndMyGold( DWORD dwCID, char* szName, DWORD dwSafeGold, DWORD dwCharGold, DWORD dwCmd ) 
{
	CHAR	szBuff[ MAX_PATH ] ;
	ULONG	iTempGold = 0 ;

	char	strTempGold[ 16 ] ;
	char	strSafeGold[ 16 ] ;

	switch ( dwCmd )
	{
		case PktGuildSafe::SC_WITHDRAWAL:				// 출금
		{
			iTempGold = m_iCurrentGold - dwSafeGold ;

			memcpy( strTempGold, CRYLStringTable::MakePrintGold( iTempGold ), sizeof( char ) * 16 ) ;
			memcpy( strSafeGold, CRYLStringTable::MakePrintGold( dwSafeGold ), sizeof( char ) * 16 ) ;

			sprintf( szBuff, CRYLStringTable::m_strString[ 1186 ], szName, strTempGold, strSafeGold ) ;

			CRYLChattingDlg::Instance()->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202 ) ;
			break ;
		}

		case PktGuildSafe::SC_BONUS:					// 보너스입금
		{
			iTempGold = dwSafeGold ;

			memcpy( strTempGold, CRYLStringTable::MakePrintGold( iTempGold ), sizeof( char ) * 16 ) ;
			memcpy( strSafeGold, CRYLStringTable::MakePrintGold( dwSafeGold ), sizeof( char ) * 16 ) ;

			sprintf( szBuff, CRYLStringTable::m_strString[ 1187 ], szName, strTempGold, strSafeGold ) ;		

			CRYLChattingDlg::Instance()->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202);
			break ;
		}

		case PktGuildSafe::SC_DEPOSIT:					// 입금
		{
			iTempGold = dwSafeGold - m_iCurrentGold ;

			memcpy( strTempGold, CRYLStringTable::MakePrintGold( iTempGold ), sizeof( char ) * 16 ) ;
			memcpy( strSafeGold, CRYLStringTable::MakePrintGold( dwSafeGold ), sizeof( char ) * 16 ) ;

			sprintf( szBuff, CRYLStringTable::m_strString[ 1187 ], szName, strTempGold, strSafeGold ) ;		

			CRYLChattingDlg::Instance()->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202);
			break ;
		}

		case PktGuildSafe::SC_RELEASE:					// 길드원에게 방출
		{
			iTempGold = m_iCurrentGold - dwSafeGold ;

			memcpy( strTempGold, CRYLStringTable::MakePrintGold( iTempGold ), sizeof( char ) * 16 ) ;
			memcpy( strSafeGold, CRYLStringTable::MakePrintGold( dwSafeGold ), sizeof( char ) * 16 ) ;

			sprintf( szBuff, CRYLStringTable::m_strString[ 1188 ], szName, strTempGold, strSafeGold ) ;
			CRYLChattingDlg::Instance()->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 202);

			CRYLGameData::Instance()->m_csStatus.m_Info.Gold = dwCharGold ;
			break ;
		}

		case PktGuildSafe::SC_CAMPSHOP:					// 길드 요새 상점의 수익금
		default:
			break;
	}

	if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
	{
		CRYLGameData::Instance()->m_csStatus.m_Info.Gold = dwCharGold ;
	}

	m_iCurrentGold = dwSafeGold ;
}

VOID	CRYLGuildPropertyPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pGuildPropertyPanel )
		return ;

	if ( true == g_pGuildPropertyPanel->m_bMessageBox )
		return ;

	switch ( Sender->GetInstance() )
	{
/*
		case RYLGUILDPROPERTYPANEL::PAYMENTGOLD_BUTTON :
		case RYLGUILDPROPERTYPANEL::DEPOSITGOLD_BUTTON :
		case RYLGUILDPROPERTYPANEL::DIVISIONGOLD_BUTTON :
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1096] ) ;
			break;
*/
		case RYLGUILDPROPERTYPANEL::PAYMENTGOLD_BUTTON :
			g_pGuildPropertyPanel->PaymentGold() ;						// 출금
			break ;

		case RYLGUILDPROPERTYPANEL::DEPOSITGOLD_BUTTON :
			g_pGuildPropertyPanel->DepositGold() ;						// 입금
			break ;

		case RYLGUILDPROPERTYPANEL::DIVISIONGOLD_BUTTON :
			g_pGuildPropertyPanel->DivisionGold() ;						// 분배
			break ;

		case RYLGUILDPROPERTYPANEL::USEINQUIRY_BUTTON :
			break ;

		case RYLGUILDPROPERTYPANEL::GUILDMARK_BUTTON :					// 길드 마크 변경
			g_pGuildPropertyPanel->ChangeGuildMarkButtonEvent() ;
			break ;

		case RYLGUILDPROPERTYPANEL::GUILDLEVEL_BUTTON :
			g_pGuildPropertyPanel->ChangeGuildLevelButtonEvent() ;		// 길드 인원 변경
			break ;
	}	
}

VOID	CRYLGuildPropertyPanel::CheckRightButton() 
{
	CRYLAuthoritySettingDlg* pDlg = ( CRYLAuthoritySettingDlg* )( ( CRYLGuildFrameDlg* )m_pGMUIParentDlg )->GetAuthoritySettingDlg() ;
	UCHAR ucTitle = pDlg->GetTitle() ;
	GuildRight* pRight = pDlg->GetGuildRight() ;

	m_pPaymentGoldButton->SetEnable( TRUE )  ;							// 출금
	m_pDepositGoldButton->SetEnable( TRUE )  ; 							// 입금
	m_pDivisionGoldButton->SetEnable( TRUE )  ;							// 분배

	if ( ucTitle == Guild::LEAVE_WAIT || ucTitle == Guild::JOIN_WAIT )
	{
		m_pPaymentGoldButton->SetEnable( FALSE )  ;						// 출금
		m_pDepositGoldButton->SetEnable( TRUE )  ;						// 입금
		m_pDivisionGoldButton->SetEnable( FALSE )  ;					// 분배
	}

	if ( ucTitle == Guild::COMMON )
	{
		if ( ( pRight->m_aryRight[ GuildRight::USE_SAFE ] == Guild::MASTER ) ||
		     ( pRight->m_aryRight[ GuildRight::USE_SAFE ] == Guild::MIDDLE_ADMIN ) )
		{		
			m_pPaymentGoldButton->SetEnable( FALSE )  ;					// 출금
			m_pDivisionGoldButton->SetEnable( FALSE )  ;				// 분배
		}
	}
	
	if ( ucTitle == Guild::MIDDLE_ADMIN )
	{
		if ( pRight->m_aryRight[ GuildRight::CHANGE_PASSWORD ] == Guild::MASTER )
		{
			m_pPaymentGoldButton->SetEnable( FALSE )  ;					// 출금
			m_pDivisionGoldButton->SetEnable( FALSE )  ;				// 분배
		}
		if ( pRight->m_aryRight[ GuildRight::USE_SAFE ] == Guild::MASTER )
		{
			m_pPaymentGoldButton->SetEnable( FALSE )  ;					// 출금
			m_pDivisionGoldButton->SetEnable( FALSE )  ;				// 분배
		}		
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();	

	if(pSelfCreature && pSelfCreature->GetTactics()==Guild::TACTICS)
	{
		m_pDepositGoldButton->SetEnable( FALSE )  ;
	}
}

VOID
CRYLGuildPropertyPanel::SetInclination( unsigned char cInclination )
{
	m_ucInclination = cInclination ;
}

unsigned char
CRYLGuildPropertyPanel::GetInclination()
{
	return m_ucInclination ;
}

unsigned char
CRYLGuildPropertyPanel::GetSelectInclination()
{
	return m_ucSelectInclination ;
}

VOID	CRYLGuildPropertyPanel::Clear()
{
	m_pCurrentMarkSprite->DetachImageTexture() ;
}

VOID	CRYLGuildPropertyPanel::SetGuildMark( CRYLSprite* pMarkSprite )
{
	if ( NULL == pMarkSprite )
	{
		m_pCurrentMarkSprite->DetachImageTexture() ;
	}
	else
	{
		m_pCurrentMarkSprite->AttachImageTexture( pMarkSprite ) ;
	}
}

VOID	CRYLGuildPropertyPanel::SetGuildMemberLevelFame( DWORD dwCurrentMemberNum, DWORD dwLevel, DWORD dwFame )
{
	m_dwCurrentMemberNum = dwCurrentMemberNum ;
	m_dwGuildLevel = dwLevel ;
	m_dwGuildFame = dwFame ;
}

VOID	CRYLGuildPropertyPanel::ChangeGuildMarkButtonEvent()
{
	if ( g_ClientMain.m_wAdminMode > SUPPORTER && m_iCurrentGold < 5000000 ) 
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[1082] ) ;
		return ;
	}	

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ;
	lpMessageBox->CreateEx( CRYLStringTable::m_strString[1083], 
		CRYLStringTable::m_strString[1084], CRYLStringTable::MakePrintGold( m_iCurrentGold ), 
		NULL, NULL, MB_SUPPLEMENTDATA | MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwGuildMarkResult ) ;
}

VOID	CRYLGuildPropertyPanel::ChangeGuildLevelButtonEvent()
{
	if ( (Guild::MAX_LEVEL - 1) == m_dwGuildLevel )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[1094] ) ;
		return ;
	}

	CHAR	szBuff[ 512 ] ;
	if ( m_dwGuildFame < Guild::ms_arySetLevelFame[ m_dwGuildLevel + 1 ] )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;

		sprintf( szBuff, CRYLStringTable::m_strString[1085], 
			CRYLStringTable::MakePrintGold( Guild::ms_arySetLevelFame[ m_dwGuildLevel + 1 ] - m_dwGuildFame ) ) ;
		lpMessageBox->Create( szBuff ) ;
		return ;
	}

	if ( static_cast<unsigned long>(m_iCurrentGold) < Guild::ms_arySetLevelGold[ m_dwGuildLevel + 1 ] )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		sprintf( szBuff, CRYLStringTable::m_strString[1086], 
			CRYLStringTable::MakePrintGold( Guild::ms_arySetLevelGold[ m_dwGuildLevel + 1 ] - m_iCurrentGold ) ) ;
		lpMessageBox->Create( szBuff ) ;
		return ;
	}

	CRYLMessageBox *lpMessageBox ;
	lpMessageBox = new CRYLMessageBox ;

	CHAR szTemp[ 2 ][ 128 ] ;
	char	LevelFame[ 16 ];
	char	LevelGold[ 16 ] ;
	memcpy( LevelFame, CRYLStringTable::MakePrintGold( Guild::ms_arySetLevelFame[ m_dwGuildLevel + 1 ] ), sizeof( char ) * 16 ) ;
	memcpy( LevelGold, CRYLStringTable::MakePrintGold( Guild::ms_arySetLevelGold[ m_dwGuildLevel + 1 ] ), sizeof( char ) * 16 ) ;

	sprintf( szBuff, CRYLStringTable::m_strString[1087], 
		Guild::ms_aryMaxMemberNum[ m_dwGuildLevel ],
		Guild::ms_aryMaxMemberNum[ m_dwGuildLevel + 1 ],
		LevelFame,
		LevelGold ) ;

	sprintf( szTemp[ 0 ], "%s", CRYLStringTable::MakePrintGold( m_dwGuildFame ) ) ;
	sprintf( szTemp[ 1 ], "%s", CRYLStringTable::MakePrintGold( m_iCurrentGold ) ) ;
	lpMessageBox->CreateEx( szBuff, CRYLStringTable::m_strString[1088], szTemp[ 0 ],CRYLStringTable::m_strString[1089], szTemp[ 1 ], MB_SUPPLEMENTDATA | MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwGuildLevelResult ) ;
}
