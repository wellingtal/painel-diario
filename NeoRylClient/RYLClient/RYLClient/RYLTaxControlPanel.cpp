#include <winsock2.h>
#include "RYLTaxControlPanel.h"
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

#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLCreature.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendCastle.h>
#include "GMMemory.h"

CRYLTaxControlPanel*	g_pTaxControlPanel = NULL ;

CRYLTaxControlPanel::CRYLTaxControlPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	m_pShopLabel			= NULL ;	// '상점'
	m_pShopTradeTaxLabel	= NULL ;	// '상점 구매/판매 세율'
	m_pShopTradeTax			= NULL ;	// 구매 세율값
	m_pPercentLabel1		= NULL ;	// '%'
	m_pShopRepairTaxLabel	= NULL ;	// '상점 수리 세율'
	m_pShopRepairTax		= NULL ;	// 수리 세율값
	m_pPercentLabel2		= NULL ;	// '%'

	m_pSmithLabel			= NULL ;	// '대장장이'
	m_pResmeltTaxLabel		= NULL ;	// '재련 세율'
	m_pResmeltTax			= NULL ;	// 재련 세율값
	m_pPercentLabel3		= NULL ;	// '%'

	m_pCampLabel			= NULL ;	// '길드 요새'
	m_pCampGoldTaxLabel		= NULL ;	// 'Gold 수익 세율'
	m_pCampGoldTax			= NULL ;	// 수익 세율값
	m_pPercentLabel4		= NULL ;	// '%'
	m_pCampMineralTaxLabel	= NULL ;	// '채굴된 광물 세율'
	m_pCampMineralTax		= NULL ;	// 광물 세율값
	m_pPercentLabel5		= NULL ;	// '%'


	m_pShopTradeBtn			= NULL ;	// 상점 구매/판매 세율 변경 버튼
	m_pShopRepairBtn		= NULL ;	// 상점 수리 세율 변경 버튼
	m_pResmeltBtn			= NULL ;	// 대장장이 재련 세율 변경 버튼
	m_pCampGoldBtn			= NULL ;	// 길드 요새 수익 세율 변경 버튼
	m_pCampMineralBtn		= NULL ;	// 길드 요새 광물 세율 변경 버튼

	m_cControlIndex			= 0 ;
	m_dwControlResult		= 0 ;
	m_dwResultTax			= 0 ;
	
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	RECT			rt ;
	CRYLSpriteList::Instance()->CreateGMImage( 256, 265, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 48 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect(   7,   22, 243,  60, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(   7,  113, 243,  33, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect(   7,  177, 243,  60, 0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 134,   30,  37,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 134,   57,  37,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 134,  121,  37,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 134,  185,  37,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 134,  212,  37,  19, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 265 ) ;

	g_pTaxControlPanel = this ;
}

CRYLTaxControlPanel::~CRYLTaxControlPanel() 
{
	FinalCtrl() ;
}

VOID CRYLTaxControlPanel::InitCtrl() 
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
	m_pShopTradeTaxLabel->SetCaption( CRYLStringTable::m_strString[4026] ) ;
	m_pShopTradeTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeTaxLabel ) ;

	m_pShopTradeTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopTradeTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 135, 34, 169, 44 ) ;
	m_pShopTradeTax->SetAlignRect( rt ) ;
	m_pShopTradeTax->SetAlign( CENTER ) ;
	m_pShopTradeTax->SetCaption( "000" ) ;
	m_pShopTradeTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeTax ) ;

	m_pPercentLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel1->SetLeft( 176 ) ;
	m_pPercentLabel1->SetTop( 34 ) ;
	m_pPercentLabel1->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel1->SetCaption( "%" ) ;
	m_pPercentLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel1 ) ;

	m_pShopRepairTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopRepairTaxLabel->SetLeft( 20 ) ;
	m_pShopRepairTaxLabel->SetTop( 61 ) ;
	m_pShopRepairTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopRepairTaxLabel->SetCaption( CRYLStringTable::m_strString[4027] ) ;
	m_pShopRepairTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairTaxLabel ) ;

	m_pShopRepairTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShopRepairTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 135, 61, 169, 71 ) ;
	m_pShopRepairTax->SetAlignRect( rt ) ;
	m_pShopRepairTax->SetAlign( CENTER ) ;
	m_pShopRepairTax->SetCaption( "000" ) ;
	m_pShopRepairTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairTax ) ;

	m_pPercentLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel2->SetLeft( 176 ) ;
	m_pPercentLabel2->SetTop( 61 ) ;
	m_pPercentLabel2->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel2->SetCaption( "%" ) ;
	m_pPercentLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel2 ) ;


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
	m_pResmeltTaxLabel->SetCaption( CRYLStringTable::m_strString[4028] ) ;
	m_pResmeltTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pResmeltTaxLabel ) ;

	m_pResmeltTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pResmeltTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 135, 125, 169, 135 ) ;
	m_pResmeltTax->SetAlignRect( rt ) ;
	m_pResmeltTax->SetAlign( CENTER ) ;
	m_pResmeltTax->SetCaption( "000" ) ;
	m_pResmeltTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pResmeltTax ) ;

	m_pPercentLabel3 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel3->SetLeft( 176 ) ;
	m_pPercentLabel3->SetTop( 125 ) ;
	m_pPercentLabel3->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel3->SetCaption( "%" ) ;
	m_pPercentLabel3->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel3 ) ;


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
	m_pCampGoldTaxLabel->SetCaption( CRYLStringTable::m_strString[4029] ) ;
	m_pCampGoldTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldTaxLabel ) ;

	m_pCampGoldTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampGoldTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 135, 189, 169, 199 ) ;
	m_pCampGoldTax->SetAlignRect( rt ) ;
	m_pCampGoldTax->SetAlign( CENTER ) ;
	m_pCampGoldTax->SetCaption( "000" ) ;
	m_pCampGoldTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldTax ) ;

	m_pPercentLabel4 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel4->SetLeft( 176 ) ;
	m_pPercentLabel4->SetTop( 189 ) ;
	m_pPercentLabel4->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel4->SetCaption( "%" ) ;
	m_pPercentLabel4->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel4 ) ;

	m_pCampMineralTaxLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampMineralTaxLabel->SetLeft( 20 ) ;
	m_pCampMineralTaxLabel->SetTop( 216 ) ;
	m_pCampMineralTaxLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampMineralTaxLabel->SetCaption( CRYLStringTable::m_strString[4030] ) ;
	m_pCampMineralTaxLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralTaxLabel ) ;

	m_pCampMineralTax = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCampMineralTax->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 135, 216, 169, 226 ) ;
	m_pCampMineralTax->SetAlignRect( rt ) ;
	m_pCampMineralTax->SetAlign( CENTER ) ;
	m_pCampMineralTax->SetCaption( "000" ) ;
	m_pCampMineralTax->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralTax ) ;

	m_pPercentLabel5 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPercentLabel5->SetLeft( 176 ) ;
	m_pPercentLabel5->SetTop( 216 ) ;
	m_pPercentLabel5->SetFontColor( 0xFFFFFFFF ) ;
	m_pPercentLabel5->SetCaption( "%" ) ;
	m_pPercentLabel5->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPercentLabel5 ) ;


	// -------------------------------------------------------------------
	// 버튼들
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pShopTradeBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pShopTradeBtn->SetLeft( 191 ) ;
	m_pShopTradeBtn->SetTop( 26 ) ;
	m_pShopTradeBtn->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pShopTradeBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopTradeBtn->SetColor( 0xFFFFFFFF ) ;
	m_pShopTradeBtn->OnClickEvent = DlgClick ;
	m_pShopTradeBtn->SetInstance( RYLTAXCONTROLPANEL::SHOP_TRADE_BUTTON ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pShopTradeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShopTradeBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pShopTradeBtn ) ;

	m_pShopRepairBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pShopRepairBtn->SetLeft( 191 ) ;
	m_pShopRepairBtn->SetTop( 53 ) ;
	m_pShopRepairBtn->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pShopRepairBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pShopRepairBtn->SetColor( 0xFFFFFFFF ) ;
	m_pShopRepairBtn->OnClickEvent = DlgClick ;
	m_pShopRepairBtn->SetInstance( RYLTAXCONTROLPANEL::SHOP_REPAIR_BUTTON ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pShopRepairBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShopRepairBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pShopRepairBtn ) ;

	m_pResmeltBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pResmeltBtn->SetLeft( 191 ) ;
	m_pResmeltBtn->SetTop( 117 ) ;
	m_pResmeltBtn->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pResmeltBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pResmeltBtn->SetColor( 0xFFFFFFFF ) ;
	m_pResmeltBtn->OnClickEvent = DlgClick ;
	m_pResmeltBtn->SetInstance( RYLTAXCONTROLPANEL::RESMELT_BUTTON ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pResmeltBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pResmeltBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pResmeltBtn ) ;

	m_pCampGoldBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pCampGoldBtn->SetLeft( 191 ) ;
	m_pCampGoldBtn->SetTop( 181 ) ;
	m_pCampGoldBtn->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pCampGoldBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampGoldBtn->SetColor( 0xFFFFFFFF ) ;
	m_pCampGoldBtn->OnClickEvent = DlgClick ;
	m_pCampGoldBtn->SetInstance( RYLTAXCONTROLPANEL::CAMP_GOLD_BUTTON ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCampGoldBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCampGoldBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCampGoldBtn ) ;

	m_pCampMineralBtn = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pCampMineralBtn->SetLeft( 191 ) ;
	m_pCampMineralBtn->SetTop( 208 ) ;
	m_pCampMineralBtn->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pCampMineralBtn->SetFontColor( 0xFFFFFFFF ) ;
	m_pCampMineralBtn->SetColor( 0xFFFFFFFF ) ;
	m_pCampMineralBtn->OnClickEvent = DlgClick ;
	m_pCampMineralBtn->SetInstance( RYLTAXCONTROLPANEL::CAMP_MINERAL_BUTTON ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCampMineralBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCampMineralBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCampMineralBtn ) ;
}


VOID CRYLTaxControlPanel::FinalCtrl() 
{
	GM_DELETE( m_pShopLabel ) ;				// 상점
	GM_DELETE( m_pShopTradeTaxLabel ) ;		// 상점 구매/판매 세율
	GM_DELETE( m_pShopTradeTax ) ;			// 구매 세율값
	GM_DELETE( m_pPercentLabel1 ) ;			// %
	GM_DELETE( m_pShopRepairTaxLabel ) ;	// 상점 수리 세율
	GM_DELETE( m_pShopRepairTax ) ;			// 수리 세율값
	GM_DELETE( m_pPercentLabel2 ) ;			// %

	GM_DELETE( m_pSmithLabel ) ;			// 대장장이
	GM_DELETE( m_pResmeltTaxLabel ) ;		// 재련 세율
	GM_DELETE( m_pResmeltTax ) ;			// 재련 세율값
	GM_DELETE( m_pPercentLabel3 ) ;			// %

	GM_DELETE( m_pCampLabel ) ;				// 길드 요새
	GM_DELETE( m_pCampGoldTaxLabel ) ;		// Gold 수익 세율
	GM_DELETE( m_pCampGoldTax ) ;			// 수익 세율값
	GM_DELETE( m_pPercentLabel4 ) ;			// %"
	GM_DELETE( m_pCampMineralTaxLabel ) ;	// 채굴된 광물 세율
	GM_DELETE( m_pCampMineralTax ) ;		// 광물 세율값
	GM_DELETE( m_pPercentLabel5 ) ;			// %


	GM_DELETE( m_pShopTradeBtn ) ;			// 상점 구매/판매 세율 변경 버튼
	GM_DELETE( m_pShopRepairBtn ) ;			// 상점 수리 세율 변경 버튼
	GM_DELETE( m_pResmeltBtn ) ;			// 대장장이 재련 세율 변경 버튼
	GM_DELETE( m_pCampGoldBtn ) ;			// 길드 요새 수익 세율 변경 버튼
	GM_DELETE( m_pCampMineralBtn ) ;		// 길드 요새 광물 세율 변경 버튼	
}


VOID CRYLTaxControlPanel::Show( CGMUIObject* Sender )
{
}


VOID CRYLTaxControlPanel::Hide( CGMUIObject* Sender )
{
}


HRESULT	CRYLTaxControlPanel::Update()
{
	ProcessMessageBoxResult() ;

	CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCurrCastleID ) ;
	if ( pCastle )
	{
		char szBuffer[ MAX_PATH ] ;

		sprintf( szBuffer, "%3d", pCastle->GetTax( Castle::TRADE_TAX ) ) ;
		m_pShopTradeTax->SetCaption( szBuffer ) ;
		sprintf( szBuffer, "%3d", pCastle->GetTax( Castle::REPAIR_TAX ) ) ;
		m_pShopRepairTax->SetCaption( szBuffer ) ;
		sprintf( szBuffer, "%3d", pCastle->GetTax( Castle::RESMELT_TAX ) ) ;
		m_pResmeltTax->SetCaption( szBuffer ) ;
		sprintf( szBuffer, "%3d", pCastle->GetTax( Castle::CAMP_GOLD_TAX ) ) ;
		m_pCampGoldTax->SetCaption( szBuffer ) ;
		sprintf( szBuffer, "%3d", pCastle->GetTax( Castle::CAMP_MINERAL_TAX ) ) ;
		m_pCampMineralTax->SetCaption( szBuffer ) ;

		if ( pCastle->GetEnableTaxChange( Castle::TRADE_TAX ) )	m_pShopTradeBtn->SetEnable( TRUE ) ;
		else m_pShopTradeBtn->SetEnable( FALSE ) ;

		if ( pCastle->GetEnableTaxChange( Castle::REPAIR_TAX ) ) m_pShopRepairBtn->SetEnable( TRUE ) ;
		else m_pShopRepairBtn->SetEnable( FALSE ) ;

		if ( pCastle->GetEnableTaxChange( Castle::RESMELT_TAX ) ) m_pResmeltBtn->SetEnable( TRUE ) ;
		else m_pResmeltBtn->SetEnable( FALSE ) ;

		if ( pCastle->GetEnableTaxChange( Castle::CAMP_GOLD_TAX ) )	m_pCampGoldBtn->SetEnable( TRUE ) ;
		else m_pCampGoldBtn->SetEnable( FALSE ) ;

		if ( pCastle->GetEnableTaxChange( Castle::CAMP_MINERAL_TAX ) ) m_pCampMineralBtn->SetEnable( TRUE ) ;
		else m_pCampMineralBtn->SetEnable( FALSE ) ;
	}

	return S_OK ;
}

VOID CRYLTaxControlPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pTaxControlPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLTAXCONTROLPANEL::SHOP_TRADE_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4031], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxControlPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxControlPanel->m_dwResultTax ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxControlPanel->m_cControlIndex = Castle::TRADE_TAX ;
			g_pTaxControlPanel->m_dwControlResult = 0 ;
			g_pTaxControlPanel->m_dwResultTax = 0 ;
		}
		break;

		case RYLTAXCONTROLPANEL::SHOP_REPAIR_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4031], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxControlPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxControlPanel->m_dwResultTax ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxControlPanel->m_cControlIndex = Castle::REPAIR_TAX ;
			g_pTaxControlPanel->m_dwControlResult = 0 ;
			g_pTaxControlPanel->m_dwResultTax = 0 ;
		}
		break;

		case RYLTAXCONTROLPANEL::RESMELT_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4031], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxControlPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxControlPanel->m_dwResultTax ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxControlPanel->m_cControlIndex = Castle::RESMELT_TAX ;
			g_pTaxControlPanel->m_dwControlResult = 0 ;
			g_pTaxControlPanel->m_dwResultTax = 0 ;
		}
		break;

		case RYLTAXCONTROLPANEL::CAMP_GOLD_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4031], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxControlPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxControlPanel->m_dwResultTax ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxControlPanel->m_cControlIndex = Castle::CAMP_GOLD_TAX ;
			g_pTaxControlPanel->m_dwControlResult = 0 ;
			g_pTaxControlPanel->m_dwResultTax = 0 ;
		}
		break;

		case RYLTAXCONTROLPANEL::CAMP_MINERAL_BUTTON :
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4031], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &g_pTaxControlPanel->m_dwControlResult ) ;
			lpMessageBox->SetValue( &g_pTaxControlPanel->m_dwResultTax ) ;
			lpMessageBox->SetEnableEnter( TRUE ) ;

			g_pTaxControlPanel->m_cControlIndex = Castle::CAMP_MINERAL_TAX ;
			g_pTaxControlPanel->m_dwControlResult = 0 ;
			g_pTaxControlPanel->m_dwResultTax = 0 ;
		}
		break;
	}
}


bool CRYLTaxControlPanel::ProcessMessageBoxResult(void)
{
	if ( m_dwControlResult & MB_YES )
	{
		if ( m_dwResultTax > 50 )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[4032], MB_OK ) ;
		}
		else
		{
			CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCurrCastleID ) ;
			if ( pCastle )
			{
				RYLCreature* pEmblem = pCastle->GetEmblem() ;
				if ( pEmblem )
				{
					SendPacket::CharCastleCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
						m_dwCurrCastleID, pEmblem->GetCharID(), m_cControlIndex, m_dwResultTax, PktCastleCmd::CASTLE_SET_TAX ) ;
				}
			}
		}

		m_cControlIndex = 0 ;
        m_dwControlResult = 0 ;
		m_dwResultTax = 0 ;
		return true ;
	}
	else if ( m_dwControlResult & MB_NO || m_dwControlResult & MB_EXIT )
	{
		m_cControlIndex = 0 ;
		m_dwControlResult = 0 ;
		m_dwResultTax = 0 ;
		return true ;
	}

	return false ;
}
