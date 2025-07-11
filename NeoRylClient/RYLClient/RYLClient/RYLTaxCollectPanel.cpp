#include <winsock2.h>
#include "RYLTaxCollectPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMUICustomDlg.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "RYLDialog.h"
#include "ItemInstance.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLCastleMineralDlg.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLCreature.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendCastle.h>
#include "GMMemory.h"


CRYLTaxCollectPanel*	g_pTaxCollectPanel = NULL ;

CRYLTaxCollectPanel::CRYLTaxCollectPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pShopLabel			= NULL ;		// 상점	
	m_pShopTradeTaxLabel	= NULL ;		// 구매 / 판매
	m_pShopTradeTax			= NULL ;		// 수익값
	m_pGoldLabel1			= NULL ;		// Gold
	m_pShopRepairTaxLabel	= NULL ;		// 수리
	m_pShopRepairTax		= NULL ;		// 수익값
	m_pGoldLabel2			= NULL ;		// Gold
											
	m_pSmithLabel			= NULL ;		// 대장장이
	m_pResmeltTaxLabel		= NULL ;		// 수익
	m_pResmeltTax			= NULL ;		// 수익값
	m_pGoldLabel3			= NULL ;		// Gold
											
	m_pCampLabel			= NULL ;		// 길드 요새
	m_pCampGoldTaxLabel		= NULL ;		// Gold
	m_pCampGoldTax			= NULL ;		// 수익값
	m_pGoldLabel4			= NULL ;		// Gold
	m_pCampMineralTaxLabel	= NULL ;		// 광물
	m_pCampMineralTax		= NULL ;		// 별도 창 이용
											
											
	m_pShopTradeBtn			= NULL ;		// 상점 구매/판매 세금 회수 버튼
	m_pShopRepairBtn		= NULL ;		// 상점 수리 세금 회수 버튼
	m_pResmeltBtn			= NULL ;		// 대장장이 재련 세금 회수 버튼
	m_pCampGoldBtn			= NULL ;		// 길드 요새 수익 세금 회수 버튼
	m_pCampMineralBtn		= NULL ;		// 길드 요새 광물 세금 회수 버튼

	m_cControlIndex			= 0 ;
	m_dwControlResult		= 0 ;
	m_dwResultGold			= 0 ;


	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	RECT			rt ;
	CRYLSpriteList::Instance()->CreateGMImage( 256, 265, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	 int i = 0;
	for ( ; i < 48 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect(   7,   22, 243,  60, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(   7,  113, 243,  33, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(   7,  177, 243,  60, 0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect(  85,   30,  73,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  85,   57,  73,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  85,  121,  73,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  85,  185,  73,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(  85,  212,  73,  19, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 265 ) ;

	g_pTaxCollectPanel = this ;
}

CRYLTaxCollectPanel::~CRYLTaxCollectPanel() 
{
	FinalCtrl() ;
}

VOID CRYLTaxCollectPanel::InitCtrl() 
{
	RECT rt ;

	// -------------------------------------------------------------------
	// 상점
	m_pShopLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopLabel->SetLeft( 7 ) ;
	m_pShopLabel->SetTop( 7 ) ;
	m_pShopLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopLabel->SetCaption( CRYLStringTable::m_strString[4012] ) ;
	m_pShopLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopLabel ) ;

	m_pShopTradeTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopTradeTaxLabel->SetLeft( 20 ) ;
	m_pShopTradeTaxLabel->SetTop( 34 ) ;
	m_pShopTradeTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopTradeTaxLabel->SetCaption( CRYLStringTable::m_strString[4013] ) ;
	m_pShopTradeTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeTaxLabel ) ;

	m_pShopTradeTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopTradeTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 86, 34, 156, 44 ) ;
	m_pShopTradeTax->SetAlignRect( rt ) ;
	m_pShopTradeTax->SetAlign( CENTER ) ;
	m_pShopTradeTax->SetCaption( "0000000000" ) ;
	m_pShopTradeTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeTax ) ;

	m_pGoldLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldLabel1->SetLeft( 163 ) ;
	m_pGoldLabel1->SetTop( 34 ) ;
	m_pGoldLabel1->SetFontColor( 0xFFFFFFFF ) ;
	m_pGoldLabel1->SetCaption( "Gold" ) ;
	m_pGoldLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldLabel1 ) ;

	m_pShopRepairTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopRepairTaxLabel->SetLeft( 20 ) ;
	m_pShopRepairTaxLabel->SetTop( 61 ) ;
	m_pShopRepairTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopRepairTaxLabel->SetCaption( CRYLStringTable::m_strString[2522] ) ;
	m_pShopRepairTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairTaxLabel ) ;

	m_pShopRepairTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopRepairTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 86, 61, 156, 71 ) ;
	m_pShopRepairTax->SetAlignRect( rt ) ;
	m_pShopRepairTax->SetAlign( CENTER ) ;
	m_pShopRepairTax->SetCaption( "0000000000" ) ;
	m_pShopRepairTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairTax ) ;

	m_pGoldLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldLabel2->SetLeft( 163 ) ;
	m_pGoldLabel2->SetTop( 61 ) ;
	m_pGoldLabel2->SetFontColor( 0xFFFFFFFF ) ;
	m_pGoldLabel2->SetCaption( "Gold" ) ;
	m_pGoldLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldLabel2 ) ;


	// -------------------------------------------------------------------
	// 대장장이
	m_pSmithLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSmithLabel->SetLeft( 7 ) ;
	m_pSmithLabel->SetTop( 97 ) ;
	m_pSmithLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pSmithLabel->SetCaption( CRYLStringTable::m_strString[4014] ) ;
	m_pSmithLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSmithLabel ) ;

	m_pResmeltTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pResmeltTaxLabel->SetLeft( 20 ) ;
	m_pResmeltTaxLabel->SetTop( 125 ) ;
	m_pResmeltTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pResmeltTaxLabel->SetCaption( CRYLStringTable::m_strString[4015] ) ;
	m_pResmeltTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pResmeltTaxLabel ) ;

	m_pResmeltTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pResmeltTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 86, 125, 156, 135 ) ;
	m_pResmeltTax->SetAlignRect( rt ) ;
	m_pResmeltTax->SetAlign( CENTER ) ;
	m_pResmeltTax->SetCaption( "0000000000" ) ;
	m_pResmeltTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pResmeltTax ) ;

	m_pGoldLabel3 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldLabel3->SetLeft( 163 ) ;
	m_pGoldLabel3->SetTop( 125 ) ;
	m_pGoldLabel3->SetFontColor( 0xFFFFFFFF ) ;
	m_pGoldLabel3->SetCaption( "Gold" ) ;
	m_pGoldLabel3->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldLabel3 ) ;


	// -------------------------------------------------------------------
	// 길드 요새
	m_pCampLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampLabel->SetLeft( 7 ) ;
	m_pCampLabel->SetTop( 161 ) ;
	m_pCampLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampLabel->SetCaption( CRYLStringTable::m_strString[4016] ) ;
	m_pCampLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampLabel ) ;

	m_pCampGoldTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampGoldTaxLabel->SetLeft( 20 ) ;
	m_pCampGoldTaxLabel->SetTop( 189 ) ;
	m_pCampGoldTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampGoldTaxLabel->SetCaption( "Gold" ) ;
	m_pCampGoldTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldTaxLabel ) ;

	m_pCampGoldTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampGoldTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 86, 189, 156, 199 ) ;
	m_pCampGoldTax->SetAlignRect( rt ) ;
	m_pCampGoldTax->SetAlign( CENTER ) ;
	m_pCampGoldTax->SetCaption( "0000000000" ) ;
	m_pCampGoldTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldTax ) ;

	m_pGoldLabel4 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGoldLabel4->SetLeft( 163 ) ;
	m_pGoldLabel4->SetTop( 189 ) ;
	m_pGoldLabel4->SetFontColor( 0xFFFFFFFF ) ;
	m_pGoldLabel4->SetCaption( "Gold" ) ;
	m_pGoldLabel4->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGoldLabel4 ) ;

	m_pCampMineralTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampMineralTaxLabel->SetLeft( 20 ) ;
	m_pCampMineralTaxLabel->SetTop( 216 ) ;
	m_pCampMineralTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampMineralTaxLabel->SetCaption( CRYLStringTable::m_strString[4017] ) ;
	m_pCampMineralTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralTaxLabel ) ;

	m_pCampMineralTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampMineralTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 86, 216, 156, 226 ) ;
	m_pCampMineralTax->SetAlignRect( rt ) ;
	m_pCampMineralTax->SetAlign( CENTER ) ;
	m_pCampMineralTax->SetCaption( CRYLStringTable::m_strString[4018] ) ;
	m_pCampMineralTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralTax ) ;



	// -------------------------------------------------------------------
	// 버튼들
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pShopTradeBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pShopTradeBtn->SetLeft( 191 ) ;
	m_pShopTradeBtn->SetTop( 26 ) ;
	m_pShopTradeBtn->SetCaption( CRYLStringTable::m_strString[4019] ) ;
	m_pShopTradeBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopTradeBtn->SetColor( 0xFFFFFFFF ) ;
	m_pShopTradeBtn->OnClickEvent = DlgClick ;
	m_pShopTradeBtn->SetInstance( RYLTAXCOLLECTPANEL::SHOP_TRADE_BUTTON ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShopTradeBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeBtn ) ;

	m_pShopRepairBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pShopRepairBtn->SetLeft( 191 ) ;
	m_pShopRepairBtn->SetTop( 53 ) ;
	m_pShopRepairBtn->SetCaption( CRYLStringTable::m_strString[4019] ) ;
	m_pShopRepairBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopRepairBtn->SetColor( 0xFFFFFFFF ) ;
	m_pShopRepairBtn->OnClickEvent = DlgClick ;
	m_pShopRepairBtn->SetInstance( RYLTAXCOLLECTPANEL::SHOP_REPAIR_BUTTON ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShopRepairBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairBtn ) ;

	m_pResmeltBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pResmeltBtn->SetLeft( 191 ) ;
	m_pResmeltBtn->SetTop( 117 ) ;
	m_pResmeltBtn->SetCaption( CRYLStringTable::m_strString[4019] ) ;
	m_pResmeltBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pResmeltBtn->SetColor( 0xFFFFFFFF ) ;
	m_pResmeltBtn->OnClickEvent = DlgClick ;
	m_pResmeltBtn->SetInstance( RYLTAXCOLLECTPANEL::RESMELT_BUTTON ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pResmeltBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pResmeltBtn ) ;

	m_pCampGoldBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pCampGoldBtn->SetLeft( 191 ) ;
	m_pCampGoldBtn->SetTop( 181 ) ;
	m_pCampGoldBtn->SetCaption( CRYLStringTable::m_strString[4019] ) ;
	m_pCampGoldBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampGoldBtn->SetColor( 0xFFFFFFFF ) ;
	m_pCampGoldBtn->OnClickEvent = DlgClick ;
	m_pCampGoldBtn->SetInstance( RYLTAXCOLLECTPANEL::CAMP_GOLD_BUTTON ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCampGoldBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldBtn ) ;

	m_pCampMineralBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pCampMineralBtn->SetLeft( 191 ) ;
	m_pCampMineralBtn->SetTop( 208 ) ;
	m_pCampMineralBtn->SetCaption( CRYLStringTable::m_strString[4019] ) ;
	m_pCampMineralBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampMineralBtn->SetColor( 0xFFFFFFFF ) ;
	m_pCampMineralBtn->OnClickEvent = DlgClick ;
	m_pCampMineralBtn->SetInstance( RYLTAXCOLLECTPANEL::CAMP_MINERAL_BUTTON ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCampMineralBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralBtn ) ;
}


VOID CRYLTaxCollectPanel::FinalCtrl() 
{
	GM_DELETE( m_pShopLabel ) ;				// '상점'
	GM_DELETE( m_pShopTradeTaxLabel ) ;		// '구매 / 판매'
	GM_DELETE( m_pShopTradeTax ) ;			// 수익값
	GM_DELETE( m_pGoldLabel1 ) ;			// 'Gold'
	GM_DELETE( m_pShopRepairTaxLabel ) ;	// '수리'
	GM_DELETE( m_pShopRepairTax ) ;			// 수익값
	GM_DELETE( m_pGoldLabel2 ) ;			// 'Gold'
											
	GM_DELETE( m_pSmithLabel ) ;			// '대장장이'
	GM_DELETE( m_pResmeltTaxLabel ) ;		// '수익'
	GM_DELETE( m_pResmeltTax ) ;			// 수익값
	GM_DELETE( m_pGoldLabel3 ) ;			// 'Gold'
											
	GM_DELETE( m_pCampLabel ) ;				// '길드 요새'
	GM_DELETE( m_pCampGoldTaxLabel ) ;		// 'Gold'
	GM_DELETE( m_pCampGoldTax ) ;			// 수익값
	GM_DELETE( m_pGoldLabel4 ) ;			// 'Gold'
	GM_DELETE( m_pCampMineralTaxLabel ) ;	// '광물'
	GM_DELETE( m_pCampMineralTax ) ;		// '별도 창 이용'
											
											
	GM_DELETE( m_pShopTradeBtn ) ;			// 상점 구매/판매 세금 회수 버튼
	GM_DELETE( m_pShopRepairBtn ) ;			// 상점 수리 세금 회수 버튼
	GM_DELETE( m_pResmeltBtn ) ;			// 대장장이 재련 세금 회수 버튼
	GM_DELETE( m_pCampGoldBtn ) ;			// 길드 요새 수익 세금 회수 버튼
	GM_DELETE( m_pCampMineralBtn ) ;		// 길드 요새 광물 세금 회수 버튼
}


VOID CRYLTaxCollectPanel::Show( CGMUIObject* Sender )
{
}


VOID CRYLTaxCollectPanel::Hide( CGMUIObject* Sender )
{
}


HRESULT CRYLTaxCollectPanel::Update()
{
	ProcessMessageBoxResult() ;

	CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCurrCastleID ) ;
	if ( pCastle )
	{
		m_pShopTradeTax->SetCaption( CRYLStringTable::MakePrintGold( pCastle->GetTaxMoney(Castle::TRADE_TAX) ) ) ;
		m_pShopRepairTax->SetCaption( CRYLStringTable::MakePrintGold( pCastle->GetTaxMoney(Castle::REPAIR_TAX) ) ) ;
		m_pResmeltTax->SetCaption( CRYLStringTable::MakePrintGold( pCastle->GetTaxMoney(Castle::RESMELT_TAX) ) ) ;
		m_pCampGoldTax->SetCaption( CRYLStringTable::MakePrintGold( pCastle->GetTaxMoney(Castle::CAMP_GOLD_TAX) ) ) ;

		if ( 0 == pCastle->GetTaxMoney( Castle::TRADE_TAX ) ) m_pShopTradeBtn->SetEnable( FALSE ) ;
		else m_pShopTradeBtn->SetEnable( TRUE ) ;

		if ( 0 == pCastle->GetTaxMoney( Castle::REPAIR_TAX ) ) m_pShopRepairBtn->SetEnable( FALSE ) ;
		else m_pShopRepairBtn->SetEnable( TRUE ) ;

		if ( 0 == pCastle->GetTaxMoney( Castle::RESMELT_TAX ) ) m_pResmeltBtn->SetEnable( FALSE ) ;
		else m_pResmeltBtn->SetEnable( TRUE ) ;

		if ( 0 == pCastle->GetTaxMoney( Castle::CAMP_GOLD_TAX ) ) m_pCampGoldBtn->SetEnable( FALSE ) ;
		else m_pCampGoldBtn->SetEnable( TRUE ) ;
	}

	return S_OK ;
}

VOID CRYLTaxCollectPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pTaxCollectPanel )
		return;

	switch ( Sender->GetInstance() )
	{
		case RYLTAXCOLLECTPANEL::SHOP_TRADE_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4020], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxCollectPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxCollectPanel->m_dwResultGold ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxCollectPanel->m_cControlIndex = Castle::TRADE_TAX ;
			g_pTaxCollectPanel->m_dwControlResult = 0 ;
			g_pTaxCollectPanel->m_dwResultGold = 0 ;
		}
		break;

		case RYLTAXCOLLECTPANEL::SHOP_REPAIR_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4021], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxCollectPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxCollectPanel->m_dwResultGold ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxCollectPanel->m_cControlIndex = Castle::REPAIR_TAX ;
			g_pTaxCollectPanel->m_dwControlResult = 0 ;
			g_pTaxCollectPanel->m_dwResultGold = 0 ;
		}
		break;

		case RYLTAXCOLLECTPANEL::RESMELT_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4022], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxCollectPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxCollectPanel->m_dwResultGold ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxCollectPanel->m_cControlIndex = Castle::RESMELT_TAX ;
			g_pTaxCollectPanel->m_dwControlResult = 0 ;
			g_pTaxCollectPanel->m_dwResultGold = 0 ;
		}
		break;

		case RYLTAXCOLLECTPANEL::CAMP_GOLD_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4023], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxCollectPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxCollectPanel->m_dwResultGold ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxCollectPanel->m_cControlIndex = Castle::CAMP_GOLD_TAX ;
			g_pTaxCollectPanel->m_dwControlResult = 0 ;
			g_pTaxCollectPanel->m_dwResultGold = 0 ;
		}
		break;

		case RYLTAXCOLLECTPANEL::CAMP_MINERAL_BUTTON :
		{
			// 별도의 창을 띄운다.
			CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLCastleMineralDlg* pCastleMineralDlg = (	CRYLCastleMineralDlg* )pScene->GetCastleMineralDlg() ;
			pCastleMineralDlg->SetCastle( g_pTaxCollectPanel->m_dwCurrCastleID ) ;
			pCastleMineralDlg->SetVisible( TRUE ) ;

			g_pTaxCollectPanel->GetParentDlg()->SetVisible( FALSE ) ;
		}
		break;
	}
}


bool CRYLTaxCollectPanel::ProcessMessageBoxResult(void)
{
	if ( m_dwControlResult & MB_YES )
	{
		// 현재 보유한 세금 수익보다 더 큰 금액을 회수하려 할때
		CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCurrCastleID ) ;
		if ( pCastle )
		{
			unsigned long dwCurrentTaxMoney = pCastle->GetTaxMoney( m_cControlIndex ) ;
			if ( m_dwResultGold > dwCurrentTaxMoney )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[4024], MB_OK ) ;
			}

			// 돈을 회수할 경우 길드 금고의 한계치를 넘는 경우
			CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
			CRYLGuildPropertyPanel*		pPanel = ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;

			if ( pPanel->GetCurrentGold() > ULONG_MAX - m_dwResultGold )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[4025], MB_OK ) ;
			}
			else
			{
				RYLCreature* pEmblem = pCastle->GetEmblem() ;
				if ( pEmblem )
				{
					SendPacket::CharCastleCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
						m_dwCurrCastleID, pEmblem->GetCharID(), m_cControlIndex, m_dwResultGold, PktCastleCmd::CASTLE_TAKE_TAXMONEY ) ;
				}
			}
		}

		m_cControlIndex = 0 ;
		m_dwControlResult = 0 ;
		m_dwResultGold = 0 ;
		return true ;
	}
	else if ( m_dwControlResult & MB_NO || m_dwControlResult & MB_EXIT )
	{
		m_cControlIndex = 0 ;
		m_dwControlResult = 0 ;
		m_dwResultGold = 0 ;
		return true ;
	}

	return false ;
}
