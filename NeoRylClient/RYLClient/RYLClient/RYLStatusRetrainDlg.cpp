#include <winsock2.h>
#include "RYLStatusRetrainDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLPicture.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include <Network/ClientSocket/Send/SendEtc.h>
#include <RenderOption.h>
#include "GMMemory.h"

CRYLStatusRetrainDlg*	g_pStatusRetrainDlg = NULL ;

CRYLStatusRetrainDlg::CRYLStatusRetrainDlg() : CRYLDialog()
{
	m_dwStatusInitResult	= 0 ;
	m_lpStatusRetrainItem	= NULL ;
	m_lRetrainIP			= 0 ;
	m_bAcceptMessageBox		= false ;
	m_cDlgState				= DLG_RETRAIN ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 275, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( int i = 0 ; i < 53 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 265, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 7, 4, 242, 239, 0xffB99E6B, 0xc0000000,  1 ) ;

	for ( int i = 0 ; i < 5 ; i ++ )
	{
		pImageDIB->MakeFrameRect( 59, 21 + ( i * 33 ), 37, 18, 0xff959595, 0xc0000000,  1 ) ;
	}

	pImageDIB->MakeFrameRect( 196, 188, 37, 18, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 196, 210, 37, 18, 0xff959595, 0xc0000000,  1 ) ;


	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 265 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pStatusRetrainDlg = this ;
}

CRYLStatusRetrainDlg::~CRYLStatusRetrainDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLStatusRetrainDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// STR 포인트 증가,감소 버튼
	m_pStrLabel = new CRYLLabel( this ) ;
	m_pStrLabel->SetLeft( 26 ) ;
	m_pStrLabel->SetTop( 40 ) ;
	m_pStrLabel->SetFontColor (0xffffffff);
	m_pStrLabel->SetAutoSize( TRUE ) ;
	m_pStrLabel->SetCaption( "STR" ) ;
	AttachGMUIControl( m_pStrLabel ) ;

	m_pStrValueLabel = new CRYLLabel( this ) ;
	m_pStrValueLabel->SetLeft( 68 ) ;
	m_pStrValueLabel->SetTop( 40 ) ;
	m_pStrValueLabel->SetFontColor (0xffffffff);
	m_pStrValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pStrValueLabel ) ;

	m_pStrPointIncBtn = new CRYLButton( this ) ;
	m_pStrPointIncBtn->SetLeft( 110 ) ;
	m_pStrPointIncBtn->SetTop( 32 ) ;
	m_pStrPointIncBtn->SetFontColor( 0xffffffff) ;
	m_pStrPointIncBtn->SetColor( 0xffffffff) ;
	m_pStrPointIncBtn->OnClickEvent = DlgClick ;
	m_pStrPointIncBtn->SetInstance( RYLSTATUSRETRAINDLG::STRPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pStrPointIncBtn ) ;

	m_pStrPointDecBtn = new CRYLButton( this ) ;
	m_pStrPointDecBtn->SetLeft( 144 ) ;
	m_pStrPointDecBtn->SetTop( 32 ) ;
	m_pStrPointDecBtn->SetFontColor( 0xffffffff) ;
	m_pStrPointDecBtn->SetColor( 0xffffffff) ;
	m_pStrPointDecBtn->OnClickEvent = DlgClick ;
	m_pStrPointDecBtn->SetInstance( RYLSTATUSRETRAINDLG::STRPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pStrPointDecBtn ) ;

	
	
	// DEX 포인트 증가,감소 버튼
	m_pDexLabel = new CRYLLabel( this ) ;
	m_pDexLabel->SetLeft( 26 ) ;
	m_pDexLabel->SetTop( 73 ) ;
	m_pDexLabel->SetFontColor (0xffffffff);
	m_pDexLabel->SetAutoSize( TRUE ) ;
	m_pDexLabel->SetCaption( "DEX" ) ;
	AttachGMUIControl( m_pDexLabel ) ;

	m_pDexValueLabel = new CRYLLabel( this ) ;
	m_pDexValueLabel->SetLeft( 68 ) ;
	m_pDexValueLabel->SetTop( 73 ) ;
	m_pDexValueLabel->SetFontColor (0xffffffff);
	m_pDexValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDexValueLabel ) ; 

	m_pDexPointIncBtn = new CRYLButton( this ) ;
	m_pDexPointIncBtn->SetLeft( 110 ) ;
	m_pDexPointIncBtn->SetTop( 65 ) ;
	m_pDexPointIncBtn->SetFontColor( 0xffffffff) ;
	m_pDexPointIncBtn->SetColor( 0xffffffff) ;
	m_pDexPointIncBtn->OnClickEvent = DlgClick ;
	m_pDexPointIncBtn->SetInstance( RYLSTATUSRETRAINDLG::DEXPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pDexPointIncBtn ) ;

	m_pDexPointDecBtn = new CRYLButton( this ) ;
	m_pDexPointDecBtn->SetLeft( 144 ) ;
	m_pDexPointDecBtn->SetTop( 65 ) ;
	m_pDexPointDecBtn->SetFontColor( 0xffffffff) ;
	m_pDexPointDecBtn->SetColor( 0xffffffff) ;
	m_pDexPointDecBtn->OnClickEvent = DlgClick ;
	m_pDexPointDecBtn->SetInstance( RYLSTATUSRETRAINDLG::DEXPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pDexPointDecBtn ) ;
	
	// CON 포인트 증가,감소 버튼
	m_pConLabel = new CRYLLabel( this ) ;
	m_pConLabel->SetLeft( 26 ) ;
	m_pConLabel->SetTop( 106 ) ;
	m_pConLabel->SetFontColor (0xffffffff);
	m_pConLabel->SetAutoSize( TRUE ) ;
	m_pConLabel->SetCaption( "CON" ) ;
	AttachGMUIControl( m_pConLabel ) ;

	m_pConValueLabel = new CRYLLabel( this ) ;
	m_pConValueLabel->SetLeft( 68 ) ;
	m_pConValueLabel->SetTop( 106 ) ;
	m_pConValueLabel->SetFontColor (0xffffffff);
	m_pConValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pConValueLabel ) ;

	m_pConPointIncBtn = new CRYLButton( this ) ;
	m_pConPointIncBtn->SetLeft( 110 ) ;
	m_pConPointIncBtn->SetTop( 98 ) ;
	m_pConPointIncBtn->SetFontColor( 0xffffffff) ;
	m_pConPointIncBtn->SetColor( 0xffffffff) ;
	m_pConPointIncBtn->OnClickEvent = DlgClick ;
	m_pConPointIncBtn->SetInstance( RYLSTATUSRETRAINDLG::CONPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConPointIncBtn ) ;

	m_pConPointDecBtn = new CRYLButton( this ) ;
	m_pConPointDecBtn->SetLeft( 144 ) ;
	m_pConPointDecBtn->SetTop( 98 ) ;
	m_pConPointDecBtn->SetFontColor( 0xffffffff) ;
	m_pConPointDecBtn->SetColor( 0xffffffff) ;
	m_pConPointDecBtn->OnClickEvent = DlgClick ;
	m_pConPointDecBtn->SetInstance( RYLSTATUSRETRAINDLG::CONPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConPointDecBtn ) ;

	// INT 포인트 증가,감소 버튼
	m_pIntLabel = new CRYLLabel( this ) ;
	m_pIntLabel->SetLeft( 26 ) ;
	m_pIntLabel->SetTop( 139 ) ;
	m_pIntLabel->SetFontColor (0xffffffff);
	m_pIntLabel->SetAutoSize( TRUE ) ;
	m_pIntLabel->SetCaption( "INT" ) ;
	AttachGMUIControl( m_pIntLabel ) ;

	m_pIntValueLabel = new CRYLLabel( this ) ;
	m_pIntValueLabel->SetLeft( 68 ) ;
	m_pIntValueLabel->SetTop( 139 ) ;
	m_pIntValueLabel->SetFontColor (0xffffffff);
	m_pIntValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pIntValueLabel ) ;

	m_pIntPointIncBtn = new CRYLButton( this ) ;
	m_pIntPointIncBtn->SetLeft( 110 ) ;
	m_pIntPointIncBtn->SetTop( 131 ) ;
	m_pIntPointIncBtn->SetFontColor( 0xffffffff) ;
	m_pIntPointIncBtn->SetColor( 0xffffffff) ;
	m_pIntPointIncBtn->OnClickEvent = DlgClick ;
	m_pIntPointIncBtn->SetInstance( RYLSTATUSRETRAINDLG::INTPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pIntPointIncBtn ) ;

	m_pIntPointDecBtn = new CRYLButton( this ) ;
	m_pIntPointDecBtn->SetLeft( 144 ) ;
	m_pIntPointDecBtn->SetTop( 131 ) ;
	m_pIntPointDecBtn->SetFontColor( 0xffffffff) ;
	m_pIntPointDecBtn->SetColor( 0xffffffff) ;
	m_pIntPointDecBtn->OnClickEvent = DlgClick ;
	m_pIntPointDecBtn->SetInstance( RYLSTATUSRETRAINDLG::INTPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pIntPointDecBtn ) ;

	// WIS 포인트 증가,감소 버튼
	m_pWisLabel = new CRYLLabel( this ) ;
	m_pWisLabel->SetLeft( 26 ) ;
	m_pWisLabel->SetTop( 172 ) ;
	m_pWisLabel->SetFontColor (0xffffffff);
	m_pWisLabel->SetAutoSize( TRUE ) ;
	m_pWisLabel->SetCaption( "WIS" ) ;
	AttachGMUIControl( m_pWisLabel ) ;

	m_pWisValueLabel = new CRYLLabel( this ) ;
	m_pWisValueLabel->SetLeft( 68 ) ;
	m_pWisValueLabel->SetTop( 172 ) ;
	m_pWisValueLabel->SetFontColor (0xffffffff);
	m_pWisValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pWisValueLabel ) ;

	m_pWisPointIncBtn = new CRYLButton( this ) ;
	m_pWisPointIncBtn->SetLeft( 110 ) ;
	m_pWisPointIncBtn->SetTop( 164 ) ;
	m_pWisPointIncBtn->SetFontColor( 0xffffffff) ;
	m_pWisPointIncBtn->SetColor( 0xffffffff) ;
	m_pWisPointIncBtn->OnClickEvent = DlgClick ;
	m_pWisPointIncBtn->SetInstance( RYLSTATUSRETRAINDLG::WISPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pWisPointIncBtn ) ;

	m_pWisPointDecBtn = new CRYLButton( this ) ;
	m_pWisPointDecBtn->SetLeft( 144 ) ;
	m_pWisPointDecBtn->SetTop( 164 ) ;
	m_pWisPointDecBtn->SetFontColor( 0xffffffff) ;
	m_pWisPointDecBtn->SetColor( 0xffffffff) ;
	m_pWisPointDecBtn->OnClickEvent = DlgClick ;
	m_pWisPointDecBtn->SetInstance( RYLSTATUSRETRAINDLG::WISPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pWisPointDecBtn ) ;

	// 재훈련 포인트
	m_pRetrainLabel = new CRYLLabel( this ) ;
	m_pRetrainLabel->SetLeft( 120 ) ;
	m_pRetrainLabel->SetTop( 206 ) ;
	m_pRetrainLabel->SetFontColor(0xffffffff);
	m_pRetrainLabel->SetAutoSize( TRUE ) ;
	m_pRetrainLabel->SetCaption( CRYLStringTable::m_strString[ 1958 ] ) ;
	AttachGMUIControl( m_pRetrainLabel ) ;

	m_pRetrainValueLabel = new CRYLLabel( this ) ;
	m_pRetrainValueLabel->SetLeft( 204 ) ;
	m_pRetrainValueLabel->SetTop( 207 ) ;
	m_pRetrainValueLabel->SetFontColor(0xffffffff);
	m_pRetrainValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pRetrainValueLabel ) ;

	// 보너스 포인트
	m_pBonusPointLabel = new CRYLLabel( this ) ;
	m_pBonusPointLabel->SetLeft( 108 ) ;
	m_pBonusPointLabel->SetTop( 228 ) ;
	m_pBonusPointLabel->SetFontColor(0xffffffff);
	m_pBonusPointLabel->SetAutoSize( TRUE ) ;
	m_pBonusPointLabel->SetCaption( CRYLStringTable::m_strString[ 1959 ] ) ;
	AttachGMUIControl( m_pBonusPointLabel ) ;

	m_pBonusPointValueLabel = new CRYLLabel( this ) ;
	m_pBonusPointValueLabel->SetLeft( 204 ) ;
	m_pBonusPointValueLabel->SetTop( 229 ) ;
	m_pBonusPointValueLabel->SetFontColor(0xffffffff);
	m_pBonusPointValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pBonusPointValueLabel ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// ok 버튼
	m_pOkBtn = new CRYLButton( this ) ;
	m_pOkBtn->SetLeft( 154 ) ;
	m_pOkBtn->SetTop( 260 ) ;
	m_pOkBtn->SetCaption( CRYLStringTable::m_strString[ 125 ] ) ;
	m_pOkBtn->SetFontColor( 0xffffffff) ;
	m_pOkBtn->SetColor( 0xffffffff) ;
	m_pOkBtn->OnClickEvent = DlgClick ;
	m_pOkBtn->SetInstance( RYLSTATUSRETRAINDLG::OK_BUTTON ) ;
	m_pOkBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkBtn ) ;

	// 취소 버튼
	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 205 ) ;
	m_pCancelBtn->SetTop( 260 ) ;
	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 126 ] ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLSTATUSRETRAINDLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	
	m_pBonusSTRPicture = new CRYLPicture( this ) ;
	m_pBonusSTRPicture->SetLeft( 179 ) ;
	m_pBonusSTRPicture->SetTop( 36 ) ;
	// Bounus Str Image
	CRYLSpriteList::Instance()->CreateGMImage( 60, 18, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 80, 53, 140, 71 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusSTRPicture->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusSTRPicture ) ;
	


	m_pBonusDEXPicture = new CRYLPicture( this ) ;
	m_pBonusDEXPicture->SetLeft( 179 ) ;
	m_pBonusDEXPicture->SetTop( 69 ) ;	
	// Bounus Dex Image
	CRYLSpriteList::Instance()->CreateGMImage( 60, 18, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 80, 53, 140, 71 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusDEXPicture->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusDEXPicture ) ;
	

	m_pBonusCONPicture = new CRYLPicture( this ) ;
	m_pBonusCONPicture->SetLeft( 179 ) ;
	m_pBonusCONPicture->SetTop( 102 ) ;
	// Bounus Con Image
	CRYLSpriteList::Instance()->CreateGMImage( 60, 18, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 80, 53, 140, 71 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusCONPicture->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusCONPicture ) ;
		

	m_pBonusINTPicture = new CRYLPicture( this ) ;
	m_pBonusINTPicture->SetLeft( 179 ) ;
	m_pBonusINTPicture->SetTop( 135 ) ;
	// Bounus Int Image
	CRYLSpriteList::Instance()->CreateGMImage( 60, 18, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 80, 53, 140, 71 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusINTPicture->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusINTPicture ) ;
	
	m_pBonusWISPicture =  new CRYLPicture( this ) ;
	m_pBonusWISPicture->SetLeft( 179 ) ;
	m_pBonusWISPicture->SetTop( 168 ) ;
	// Bounus Wis Image
	CRYLSpriteList::Instance()->CreateGMImage( 60, 18, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 80, 53, 140, 71 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusWISPicture->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusWISPicture ) ;
}

VOID	CRYLStatusRetrainDlg::FinalCtrl ()
{
	// STR 포인트 증가,감소 버튼
	GM_DELETE( m_pStrLabel ) ;
	GM_DELETE( m_pStrValueLabel ) ;
	GM_DELETE( m_pStrPointIncBtn ) ;
	GM_DELETE( m_pStrPointDecBtn ) ;
	// DEX 포인트 증가,감소 버튼
	GM_DELETE( m_pDexLabel ) ;
	GM_DELETE( m_pDexValueLabel ) ;
	GM_DELETE( m_pDexPointIncBtn ) ;
	GM_DELETE( m_pDexPointDecBtn ) ;
	// CON 포인트 증가,감소 버튼
	GM_DELETE( m_pConLabel ) ;
	GM_DELETE( m_pConValueLabel ) ;
	GM_DELETE( m_pConPointIncBtn ) ;
	GM_DELETE( m_pConPointDecBtn ) ;
	// INT 포인트 증가,감소 버튼
	GM_DELETE( m_pIntLabel ) ;
	GM_DELETE( m_pIntValueLabel ) ;
	GM_DELETE( m_pIntPointIncBtn ) ;
	GM_DELETE( m_pIntPointDecBtn ) ;
	// WIS 포인트 증가,감소 버튼
	GM_DELETE( m_pWisLabel ) ;
	GM_DELETE( m_pWisValueLabel ) ;
	GM_DELETE( m_pWisPointIncBtn ) ;
	GM_DELETE( m_pWisPointDecBtn ) ;

	// 재훈련 포인트
	GM_DELETE( m_pRetrainLabel ) ;
	GM_DELETE( m_pRetrainValueLabel ) ;

	// 보너스 포인트
	GM_DELETE( m_pBonusPointLabel ) ;
	GM_DELETE( m_pBonusPointValueLabel ) ;

	// Ok, Cancel
	GM_DELETE( m_pOkBtn ) ;
	GM_DELETE( m_pCancelBtn ) ;

	GM_DELETE( m_pBonusSTRPicture ) ;
	GM_DELETE( m_pBonusDEXPicture ) ;
	GM_DELETE( m_pBonusCONPicture ) ;
	GM_DELETE( m_pBonusINTPicture ) ;
	GM_DELETE( m_pBonusWISPicture ) ;
}

VOID	CRYLStatusRetrainDlg::Show( CGMUIObject* Sender )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (DLG_RETRAIN == m_cDlgState)
	{
		// edith 2008.02.14  부활의돌 검색
		CItemInstance* lpItem = pGame->m_csStatus.GetInventoryItemfromID(Item::EtcItemID::REBIRTH_STONE);	

		// 돈검색 제거.
		//if (!(lpItem && (unsigned long)( pGame->m_csStatus.m_Info.Level * 10000 ) <= pGame->m_csStatus.m_Info.Gold ) )

		if (!lpItem )
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[1409]);

			SetVisible( FALSE );
			return ;
		}
	}

	pGame->m_csStatus.SetStatusInit( pGame->m_csStatus.m_Info.Class, m_csBasicState ) ;
	m_csInitState.m_wSTR = m_csBaseState.m_wSTR = pGame->m_csStatus.m_Info.STR ;
	m_csInitState.m_wDEX = m_csBaseState.m_wDEX = pGame->m_csStatus.m_Info.DEX ;
	m_csInitState.m_wCON = m_csBaseState.m_wCON = pGame->m_csStatus.m_Info.CON ;
	m_csInitState.m_wINT = m_csBaseState.m_wINT = pGame->m_csStatus.m_Info.INT ;
	m_csInitState.m_wWIS = m_csBaseState.m_wWIS = pGame->m_csStatus.m_Info.WIS ;
	m_csInitState.m_wIP  = m_csBaseState.m_wIP  = pGame->m_csStatus.m_Info.IP ;

	m_dwStatusInitResult = 0 ;
	m_lRetrainIP = 0 ;

	m_pBonusSTRPicture->SetVisible( FALSE ) ;
	m_pBonusDEXPicture->SetVisible( FALSE ) ;
	m_pBonusCONPicture->SetVisible( FALSE ) ;
	m_pBonusINTPicture->SetVisible( FALSE ) ;
	m_pBonusWISPicture->SetVisible( FALSE ) ;

	//hayzohar stone of rebirth(REBIRTH_STONE)
	if ( pGame->m_csStatus.GetUseIP(STATUS_STR) == 2)	m_pBonusSTRPicture->SetVisible( TRUE ) ;
	if ( pGame->m_csStatus.GetUseIP(STATUS_DEX) == 2)	m_pBonusDEXPicture->SetVisible( TRUE ) ;
	if ( pGame->m_csStatus.GetUseIP(STATUS_CON) == 2)	m_pBonusCONPicture->SetVisible( TRUE ) ;
	if ( pGame->m_csStatus.GetUseIP(STATUS_INT) == 2)	m_pBonusINTPicture->SetVisible( TRUE ) ;
	if ( pGame->m_csStatus.GetUseIP(STATUS_WIS) == 2)	m_pBonusWISPicture->SetVisible( TRUE ) ;
}
	
VOID	CRYLStatusRetrainDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLStatusRetrainDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLStatusRetrainDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	if ( m_dwStatusInitResult & MB_YES )
	{
		// 테스트용.
//		m_csInitState.m_wINT = 45;
//		m_csInitState.m_wWIS = 65530;
		// 패킷 보내기
		if (DLG_RETRAIN == m_cDlgState)
		{
			SendPacket::CharStatusRetrain(g_GameSession, &m_csInitState, &m_lpStatusRetrainItem->m_lpItemBase->GetPos());
		}
		else if (DLG_REDSTRIBUTION == m_cDlgState)
		{
			// edith 2008.02.14 스텟초기화?
			SendPacket::CharStateRedistribution(g_GameSession, &m_csInitState);
		}

		m_dwStatusInitResult = 0 ;
		m_bAcceptMessageBox = false ;

		SetVisible( FALSE ) ;
	}
	else if ( m_dwStatusInitResult & MB_NO || m_dwStatusInitResult & MB_EXIT )
	{
		m_dwStatusInitResult = 0 ;
		m_bAcceptMessageBox = false ;
	}

	CHAR	strText[ 16 ] ;

	// STR
	itoa(m_csInitState.m_wSTR, strText, 10);
	m_pStrValueLabel->SetCaption( strText ) ;

	// DEX
	itoa(m_csInitState.m_wDEX, strText, 10);
	m_pDexValueLabel->SetCaption( strText ) ;
	
	// CON
	itoa(m_csInitState.m_wCON, strText, 10);
	m_pConValueLabel->SetCaption( strText ) ;
	
	// INT
	itoa(m_csInitState.m_wINT, strText, 10);
	m_pIntValueLabel->SetCaption( strText ) ;
	
	// WIS
	itoa(m_csInitState.m_wWIS, strText, 10);
	m_pWisValueLabel->SetCaption( strText ) ;
	
	// 재훈련 IP
	itoa(m_lRetrainIP, strText, 10);
	m_pRetrainValueLabel->SetCaption( strText ) ;

	// 총 IP
	itoa(m_csInitState.m_wIP, strText, 10);
	m_pBonusPointValueLabel->SetCaption( strText ) ;

	return S_OK ;
}

VOID		CRYLStatusRetrainDlg::StrPointInc() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_STR);
	
	if (lPoint <= m_csInitState.m_wIP)
	{
		m_csInitState.m_wSTR++;
		m_csInitState.m_wIP -= lPoint;
		
		if (m_csInitState.m_wSTR - m_csBaseState.m_wSTR <= 0) 
			m_lRetrainIP -= lPoint;
	}
}

VOID		CRYLStatusRetrainDlg::StrPointDec() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_STR);
		
	if (m_csBasicState.m_wSTR < m_csInitState.m_wSTR)
	{
		if (m_csInitState.m_wSTR - 1 < m_csBaseState.m_wSTR)
		{
			if (DLG_RETRAIN == m_cDlgState && m_lRetrainIP + lPoint > MAX_RETRAIN_STATE)
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[1400], MAX_RETRAIN_STATE);
				lpMessageBox->Create(strText);
				lpMessageBox->SetTimer(1000);
				return ;
			}
			
			m_lRetrainIP += lPoint;
		}
		m_csInitState.m_wSTR--;
		m_csInitState.m_wIP += lPoint;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[1401], pGame->m_csStatus.m_Info.Level,
		pGame->GetClassName(pGame->m_csStatus.m_Info.Class), m_csBasicState.m_wSTR);
		lpMessageBox->Create(strText);
	}
}

VOID		CRYLStatusRetrainDlg::DexPointInc() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_DEX);
		
	if (lPoint <= m_csInitState.m_wIP)
	{
		m_csInitState.m_wDEX++;
		m_csInitState.m_wIP -= lPoint;
		
		if (m_csInitState.m_wDEX - m_csBaseState.m_wDEX <= 0) m_lRetrainIP -= lPoint;
	}
}

VOID		CRYLStatusRetrainDlg::DexPointDec() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_DEX);
		
	if (m_csBasicState.m_wDEX < m_csInitState.m_wDEX)
	{	
		if (m_csInitState.m_wDEX - 1 < m_csBaseState.m_wDEX)
		{
			if (DLG_RETRAIN == m_cDlgState && m_lRetrainIP + lPoint > MAX_RETRAIN_STATE)
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[1400], MAX_RETRAIN_STATE);
				lpMessageBox->Create(strText);
				lpMessageBox->SetTimer(1000);
			
				return ;
			}
			
			m_lRetrainIP += lPoint;
		}	
		
		m_csInitState.m_wDEX--;
		m_csInitState.m_wIP += lPoint;
	}	 
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[1402], pGame->m_csStatus.m_Info.Level,
		pGame->GetClassName(pGame->m_csStatus.m_Info.Class), m_csBasicState.m_wDEX);
		lpMessageBox->Create(strText);
	}
}

VOID		CRYLStatusRetrainDlg::ConPointInc() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_CON);
		
	if (lPoint <= m_csInitState.m_wIP)
	{
		m_csInitState.m_wCON++;
		m_csInitState.m_wIP -= lPoint;
		
		if (m_csInitState.m_wCON - m_csBaseState.m_wCON <= 0) 
			m_lRetrainIP -= lPoint;
	}
}

VOID		CRYLStatusRetrainDlg::ConPointDec() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_CON);

	if (m_csBasicState.m_wCON < m_csInitState.m_wCON)
	{
		if (m_csInitState.m_wCON - 1 < m_csBaseState.m_wCON)
		{
			if (DLG_RETRAIN == m_cDlgState && m_lRetrainIP + lPoint > MAX_RETRAIN_STATE)
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[1400], MAX_RETRAIN_STATE);
				lpMessageBox->Create(strText);
				lpMessageBox->SetTimer(1000);
				
				return ;
			}	
			
			m_lRetrainIP += lPoint;
		}

		m_csInitState.m_wCON--;
		m_csInitState.m_wIP += lPoint;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[1403], pGame->m_csStatus.m_Info.Level,
		pGame->GetClassName(pGame->m_csStatus.m_Info.Class), m_csBasicState.m_wCON);
		lpMessageBox->Create(strText);
	}
}

VOID		CRYLStatusRetrainDlg::IntPointInc() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_INT);
		
	if (lPoint <= m_csInitState.m_wIP)
	{
		m_csInitState.m_wINT++;
		m_csInitState.m_wIP -= lPoint;
		
		if (m_csInitState.m_wINT - m_csBaseState.m_wINT <= 0) 
			m_lRetrainIP -= lPoint;
	}
}

VOID		CRYLStatusRetrainDlg::IntPointDec() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_INT);
		
	if (m_csBasicState.m_wINT < m_csInitState.m_wINT)
	{
		if (m_csInitState.m_wINT - 1 < m_csBaseState.m_wINT)
		{
			if (DLG_RETRAIN == m_cDlgState && m_lRetrainIP + lPoint > MAX_RETRAIN_STATE)
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[1400], MAX_RETRAIN_STATE);
				lpMessageBox->Create(strText);
				lpMessageBox->SetTimer(1000);
				return ;
			}
			
			m_lRetrainIP += lPoint;
		}
		
		m_csInitState.m_wINT--;
		m_csInitState.m_wIP += lPoint;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[1404], pGame->m_csStatus.m_Info.Level,
		pGame->GetClassName(pGame->m_csStatus.m_Info.Class), m_csBasicState.m_wINT);
		lpMessageBox->Create(strText);
	}
}

VOID		CRYLStatusRetrainDlg::WisPointInc() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_WIS);
		
	if (lPoint <= m_csInitState.m_wIP)
	{
		m_csInitState.m_wWIS++;
		m_csInitState.m_wIP -= lPoint;

		if (m_csInitState.m_wWIS - m_csBaseState.m_wWIS <= 0) 
			m_lRetrainIP -= lPoint;
	}
}

VOID		CRYLStatusRetrainDlg::WisPointDec() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	long lPoint = pGame->m_csStatus.GetUseIP(STATUS_WIS);
		
	if (m_csBasicState.m_wWIS < m_csInitState.m_wWIS)
	{
		if (m_csInitState.m_wWIS - 1 < m_csBaseState.m_wWIS)
		{
			if (DLG_RETRAIN == m_cDlgState && m_lRetrainIP + lPoint > MAX_RETRAIN_STATE)
			{
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[1400], MAX_RETRAIN_STATE);
				lpMessageBox->Create(strText);
				lpMessageBox->SetTimer(1000);
				return ;
			}
	
			m_lRetrainIP += lPoint;
		}
		
		m_csInitState.m_wWIS--;
		m_csInitState.m_wIP += lPoint;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[1405], pGame->m_csStatus.m_Info.Level,
		pGame->GetClassName(pGame->m_csStatus.m_Info.Class), m_csBasicState.m_wWIS);
		lpMessageBox->Create(strText);
	}
}

VOID		CRYLStatusRetrainDlg::Ok() 
{
	CHAR strTemp[ 512 ] ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (DLG_RETRAIN == m_cDlgState)
	{
		CHAR strText[MAX_PATH], strText1[MAX_PATH];

		// edith 2008.02.14  부활의돌 검색
		CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(Item::EtcItemID::REBIRTH_STONE);		
		if (lpItem)// && (unsigned long)(pGame->m_csStatus.m_Info.Level * 10000) <= pGame->m_csStatus.m_Info.Gold)
		{
			if (m_lRetrainIP == MAX_RETRAIN_STATE)
			{
				sprintf(strText, CRYLStringTable::m_strString[1406], m_lRetrainIP);
			} 
			else
			{
				sprintf(strText, CRYLStringTable::m_strString[1407], m_lRetrainIP, MAX_RETRAIN_STATE, m_lRetrainIP);
			}

			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			sprintf(strText1, CRYLStringTable::m_strString[1408]);//, pGame->m_csStatus.m_Info.Level * 10000);
			sprintf( strTemp, strText, strText1 ) ;
			
			lpMessageBox->Create( strTemp, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwStatusInitResult);
			
			m_bAcceptMessageBox = true;
			m_lpStatusRetrainItem = lpItem;
		} 
		else
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[1409]);
		}
	}
	else if (DLG_REDSTRIBUTION == m_cDlgState)
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		sprintf( strTemp, CRYLStringTable::m_strString[2309], 
			pGame->m_csStatus.m_Info.Chance, pGame->m_csStatus.m_Info.Chance - 1 ) ;

		lpMessageBox->Create( strTemp, MB_YES | MB_NO | MB_EXIT);
		lpMessageBox->SetResult(&m_dwStatusInitResult);

		m_bAcceptMessageBox = true;
	}
}
VOID	CRYLStatusRetrainDlg::Cancel() 
{
	SetVisible( FALSE ) ;
}

void	CRYLStatusRetrainDlg::SetState( unsigned char cDlgState ) 
{
	m_cDlgState = cDlgState;

	// Rodin : 상태에 따른 처리를 해준다.
}

VOID	CRYLStatusRetrainDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pStatusRetrainDlg )
		return ;

	if ( true == g_pStatusRetrainDlg->m_bAcceptMessageBox )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLSTATUSRETRAINDLG::STRPOINTINC_BUTTON :	g_pStatusRetrainDlg->StrPointInc() ;	break ;
		case RYLSTATUSRETRAINDLG::STRPOINTDEC_BUTTON :	g_pStatusRetrainDlg->StrPointDec() ;	break ;
		
		case RYLSTATUSRETRAINDLG::DEXPOINTINC_BUTTON :	g_pStatusRetrainDlg->DexPointInc() ;	break ;
		case RYLSTATUSRETRAINDLG::DEXPOINTDEC_BUTTON :	g_pStatusRetrainDlg->DexPointDec() ;	break ;
		
		case RYLSTATUSRETRAINDLG::CONPOINTINC_BUTTON :	g_pStatusRetrainDlg->ConPointInc() ;	break ;
		case RYLSTATUSRETRAINDLG::CONPOINTDEC_BUTTON :	g_pStatusRetrainDlg->ConPointDec() ;	break ;
		
		case RYLSTATUSRETRAINDLG::INTPOINTINC_BUTTON :	g_pStatusRetrainDlg->IntPointInc() ;	break ;
		case RYLSTATUSRETRAINDLG::INTPOINTDEC_BUTTON :	g_pStatusRetrainDlg->IntPointDec() ;	break ;
	
		case RYLSTATUSRETRAINDLG::WISPOINTINC_BUTTON :	g_pStatusRetrainDlg->WisPointInc() ;	break ;
		case RYLSTATUSRETRAINDLG::WISPOINTDEC_BUTTON :	g_pStatusRetrainDlg->WisPointDec() ;	break ;
	
		case RYLSTATUSRETRAINDLG::OK_BUTTON	:			g_pStatusRetrainDlg->Ok() ;				break ;
		case RYLSTATUSRETRAINDLG::CANCEL_BUTTON :		g_pStatusRetrainDlg->Cancel() ;			break ;
	}
}
