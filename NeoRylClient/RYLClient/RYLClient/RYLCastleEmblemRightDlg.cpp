
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"
#include "RYLStringTable.h"

#include "RYLNetworkData.h"
#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLChattingDlg.h"
#include "RYLCastleEmblemRightDlg.h"
#include "GMMemory.h"

CRYLCastleEmblemRightDlg*	g_pRYLCastleEmblemRightDlg = NULL ;

CRYLCastleEmblemRightDlg::CRYLCastleEmblemRightDlg() 
{
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 375, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 73 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->MakeFrameRect( 11, 46, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 98, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 150, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 202, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 254, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 305, 121, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 389 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
	
	g_pRYLCastleEmblemRightDlg = this ;

	m_CastleRight = CastleRight() ;
	m_dwCastleID = 0 ;
}

CRYLCastleEmblemRightDlg::~CRYLCastleEmblemRightDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCastleEmblemRightDlg::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 일반/관리자 권한
	m_pGeneralAdminRightTitleLable = new CRYLLabel( this ) ;
	m_pGeneralAdminRightTitleLable->SetLeft( 11 ) ;
	m_pGeneralAdminRightTitleLable->SetTop( 18 ) ;
	m_pGeneralAdminRightTitleLable->SetFontColor (0xffffffff);
	m_pGeneralAdminRightTitleLable->SetAutoSize( TRUE ) ;
	m_pGeneralAdminRightTitleLable->SetCaption( CRYLStringTable::m_strString[ 2682 ] ) ;
	AttachGMUIControl( m_pGeneralAdminRightTitleLable ) ;

	// 성 소유 상징물 이용
	m_pEmbleUsedLable = new CRYLLabel( this ) ;
	m_pEmbleUsedLable->SetLeft( 15 ) ;
	m_pEmbleUsedLable->SetTop( 45 ) ;
	m_pEmbleUsedLable->SetFontColor (0xffffffff);
	m_pEmbleUsedLable->SetAutoSize( TRUE ) ;
	m_pEmbleUsedLable->SetCaption( CRYLStringTable::m_strString[ 2683 ] ) ;
	AttachGMUIControl( m_pEmbleUsedLable ) ;

	// 일반 길드원
	m_pGeneralLabel1 = new CRYLLabel( this ) ;
	m_pGeneralLabel1->SetLeft( 21 ) ;
	m_pGeneralLabel1->SetTop( 69 ) ;
	m_pGeneralLabel1->SetFontColor (0xffffffff);
	m_pGeneralLabel1->SetAutoSize( TRUE ) ;
	m_pGeneralLabel1->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel1 ) ;

	// 중간 관리자
	m_pMiddleAdminLable1 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable1->SetLeft( 132 ) ;
	m_pMiddleAdminLable1->SetTop( 69 ) ;
	m_pMiddleAdminLable1->SetFontColor (0xffffffff);
	m_pMiddleAdminLable1->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable1->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable1 ) ;

	// [일반길드원]성 소유 상징물 이용
	m_pEmbleUsedGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pEmbleUsedGeneralChkBox->SetLeft( 94 ) ;
	m_pEmbleUsedGeneralChkBox->SetTop( 65 ) ;
	m_pEmbleUsedGeneralChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::EMBLEMUSEDGENERAL_CHECKBUTTON ) ;
	m_pEmbleUsedGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pEmbleUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pEmbleUsedGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pEmbleUsedGeneralChkBox ) ;

	// [중간관리자]성 소유 상징물 이용
	m_pEmbleUsedMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pEmbleUsedMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pEmbleUsedMiddleAdminChkBox->SetTop( 65 ) ;
	m_pEmbleUsedMiddleAdminChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::EMBLEMUSEDADMIN_CHECKBUTTON ) ;
	m_pEmbleUsedMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pEmbleUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pEmbleUsedMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pEmbleUsedMiddleAdminChkBox ) ;

	// 성문 관리
	m_pCastleDoorMgrLable = new CRYLLabel( this ) ;
	m_pCastleDoorMgrLable->SetLeft( 15 ) ;
	m_pCastleDoorMgrLable->SetTop( 97 ) ;
	m_pCastleDoorMgrLable->SetFontColor (0xffffffff);
	m_pCastleDoorMgrLable->SetAutoSize( TRUE ) ;
	m_pCastleDoorMgrLable->SetCaption( CRYLStringTable::m_strString[ 2684 ] ) ;
	AttachGMUIControl( m_pCastleDoorMgrLable ) ;

	// 일반 길드원
	m_pGeneralLabel2 = new CRYLLabel( this ) ;
	m_pGeneralLabel2->SetLeft( 21 ) ;
	m_pGeneralLabel2->SetTop( 121 ) ;
	m_pGeneralLabel2->SetFontColor (0xffffffff);
	m_pGeneralLabel2->SetAutoSize( TRUE ) ;
	m_pGeneralLabel2->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel2 ) ;

	// 중간 관리자
	m_pMiddleAdminLable2 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable2->SetLeft( 132 ) ;
	m_pMiddleAdminLable2->SetTop( 121 ) ;
	m_pMiddleAdminLable2->SetFontColor (0xffffffff);
	m_pMiddleAdminLable2->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable2->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable2 ) ;

	// [일반길드원]성문 관리
	m_pCastleDoorMgrGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pCastleDoorMgrGeneralChkBox->SetLeft( 94 ) ;
	m_pCastleDoorMgrGeneralChkBox->SetTop( 117 ) ;
	m_pCastleDoorMgrGeneralChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::CASTLEDOORMGRGENERAL_CHECKBUTTON ) ;
	m_pCastleDoorMgrGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCastleDoorMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCastleDoorMgrGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCastleDoorMgrGeneralChkBox ) ;

	// [중간관리자]성문 관리
	m_pCastleDoorMgrMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->SetTop( 117 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::CASTLEDOORMGRADMIN_CHECKBUTTON ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCastleDoorMgrMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCastleDoorMgrMiddleAdminChkBox ) ;

	// 성문 이용
	m_pCastleDoorUsedLable = new CRYLLabel( this ) ;
	m_pCastleDoorUsedLable->SetLeft( 15 ) ;
	m_pCastleDoorUsedLable->SetTop( 149 ) ;
	m_pCastleDoorUsedLable->SetFontColor (0xffffffff);
	m_pCastleDoorUsedLable->SetAutoSize( TRUE ) ;
	m_pCastleDoorUsedLable->SetCaption( CRYLStringTable::m_strString[ 2685 ] ) ;
	AttachGMUIControl( m_pCastleDoorUsedLable ) ;

	// 일반 길드원
	m_pGeneralLabel3 = new CRYLLabel( this ) ;
	m_pGeneralLabel3->SetLeft( 21 ) ;
	m_pGeneralLabel3->SetTop( 173 ) ;
	m_pGeneralLabel3->SetFontColor (0xffffffff);
	m_pGeneralLabel3->SetAutoSize( TRUE ) ;
	m_pGeneralLabel3->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel3 ) ;

	// 중간 관리자
	m_pMiddleAdminLable3 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable3->SetLeft( 132 ) ;
	m_pMiddleAdminLable3->SetTop( 173 ) ;
	m_pMiddleAdminLable3->SetFontColor (0xffffffff);
	m_pMiddleAdminLable3->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable3->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable3 ) ;

	// [일반길드원]성문 이용
	m_pCastleDoorUsedGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pCastleDoorUsedGeneralChkBox->SetLeft( 94 ) ;
	m_pCastleDoorUsedGeneralChkBox->SetTop( 169 ) ;
	m_pCastleDoorUsedGeneralChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::CASTLEDOORUSEDGENERAL_CHECKBUTTON ) ;
	m_pCastleDoorUsedGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCastleDoorUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCastleDoorUsedGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCastleDoorUsedGeneralChkBox ) ;

	// [중간관리자]성문 이용
	m_pCastleDoorUsedMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->SetTop( 169 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::CASTLEDOORUSEDADMIN_CHECKBUTTON ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCastleDoorUsedMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCastleDoorUsedMiddleAdminChkBox ) ;

	// 병기 관리
	m_pArmsMgrLable = new CRYLLabel( this ) ;
	m_pArmsMgrLable->SetLeft( 15 ) ;
	m_pArmsMgrLable->SetTop( 201 ) ;
	m_pArmsMgrLable->SetFontColor (0xffffffff);
	m_pArmsMgrLable->SetAutoSize( TRUE ) ;
	m_pArmsMgrLable->SetCaption( CRYLStringTable::m_strString[ 2686 ] ) ;
	AttachGMUIControl( m_pArmsMgrLable ) ;

	// 일반 길드원
	m_pGeneralLabel4 = new CRYLLabel( this ) ;
	m_pGeneralLabel4->SetLeft( 21 ) ;
	m_pGeneralLabel4->SetTop( 225 ) ;
	m_pGeneralLabel4->SetFontColor (0xffffffff);
	m_pGeneralLabel4->SetAutoSize( TRUE ) ;
	m_pGeneralLabel4->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel4 ) ;

	// 중간 관리자
	m_pMiddleAdminLable4 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable4->SetLeft( 132 ) ;
	m_pMiddleAdminLable4->SetTop( 225 ) ;
	m_pMiddleAdminLable4->SetFontColor (0xffffffff);
	m_pMiddleAdminLable4->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable4->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable4 ) ;

	// [일반길드원]병기 관리
	m_pArmsMgrGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pArmsMgrGeneralChkBox->SetLeft( 94 ) ;
	m_pArmsMgrGeneralChkBox->SetTop( 221 ) ;
	m_pArmsMgrGeneralChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::ARMSMGRGENERAL_CHECKBUTTON ) ;
	m_pArmsMgrGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pArmsMgrGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pArmsMgrGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pArmsMgrGeneralChkBox ) ;

	// [중간관리자]병기 관리
	m_pArmsMgrMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pArmsMgrMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pArmsMgrMiddleAdminChkBox->SetTop( 221 ) ;
	m_pArmsMgrMiddleAdminChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::ARMSMGRADMIN_CHECKBUTTON ) ;
	m_pArmsMgrMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pArmsMgrMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pArmsMgrMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pArmsMgrMiddleAdminChkBox ) ;

	// 병기 이용
	m_pArmsUsedLable = new CRYLLabel( this ) ;
	m_pArmsUsedLable->SetLeft( 15 ) ;
	m_pArmsUsedLable->SetTop( 253 ) ;
	m_pArmsUsedLable->SetFontColor (0xffffffff);
	m_pArmsUsedLable->SetAutoSize( TRUE ) ;
	m_pArmsUsedLable->SetCaption( CRYLStringTable::m_strString[ 2687 ] ) ;
	AttachGMUIControl( m_pArmsUsedLable ) ;

	// 일반 길드원
	m_pGeneralLabel5 = new CRYLLabel( this ) ;
	m_pGeneralLabel5->SetLeft( 21 ) ;
	m_pGeneralLabel5->SetTop( 277 ) ;
	m_pGeneralLabel5->SetFontColor (0xffffffff);
	m_pGeneralLabel5->SetAutoSize( TRUE ) ;
	m_pGeneralLabel5->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel5) ;

	// 중간 관리자
	m_pMiddleAdminLable5 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable5->SetLeft( 132 ) ;
	m_pMiddleAdminLable5->SetTop( 277 ) ;
	m_pMiddleAdminLable5->SetFontColor (0xffffffff);
	m_pMiddleAdminLable5->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable5->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable5 ) ;

	// [일반길드원]병기 이용
	m_pArmsUsedGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pArmsUsedGeneralChkBox->SetLeft( 94 ) ;
	m_pArmsUsedGeneralChkBox->SetTop( 273 ) ;
	m_pArmsUsedGeneralChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::ARMSMGRUSEDGENERAL_CHECKBUTTON ) ;
	m_pArmsUsedGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pArmsUsedGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pArmsUsedGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pArmsUsedGeneralChkBox ) ;

	// [중간관리자]병기 이용
	m_pArmsUsedMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pArmsUsedMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pArmsUsedMiddleAdminChkBox->SetTop( 273 ) ;
	m_pArmsUsedMiddleAdminChkBox->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::ARMSMGRUSEDADMIN_CHECKBUTTON ) ;
	m_pArmsUsedMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pArmsUsedMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pArmsUsedMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pArmsUsedMiddleAdminChkBox ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 확인 버튼
	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 109 ) ;
	m_pOkButton->SetTop( 355 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 179 ) ;
	m_pCancelButton->SetTop( 355 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCASTLEEMBLEMRIGHTDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLCastleEmblemRightDlg::FinalCtrl() 
{
	GM_DELETE( m_pGeneralAdminRightTitleLable ) ;			// 성의 관리 권한설정
	
	GM_DELETE( m_pEmbleUsedLable ) ;						// 성 소유 상징물 이용
	GM_DELETE( m_pGeneralLabel1 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable1 ) ;						// 중간 관리자
	GM_DELETE( m_pEmbleUsedGeneralChkBox ) ;				// [일반길드원]성 소유 상징물 이용
	GM_DELETE( m_pEmbleUsedMiddleAdminChkBox ) ;			// [중간관리자]성 소유 상징물 이용
			   
	GM_DELETE( m_pCastleDoorMgrLable ) ;					// 성문 관리
	GM_DELETE( m_pGeneralLabel2 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable2 ) ;						// 중간 관리자
	GM_DELETE( m_pCastleDoorMgrGeneralChkBox ) ;			// [일반길드원]성문 관리
	GM_DELETE( m_pCastleDoorMgrMiddleAdminChkBox ) ;		// [중간관리자]성문 관리
	
	GM_DELETE( m_pCastleDoorUsedLable ) ;					// 성문 이용
	GM_DELETE( m_pGeneralLabel3 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable3 ) ;						// 중간 관리자
	GM_DELETE( m_pCastleDoorUsedGeneralChkBox ) ;			// [일반길드원]성문 이용
	GM_DELETE( m_pCastleDoorUsedMiddleAdminChkBox ) ;		// [중간관리자]성문 이용
			   
	GM_DELETE( m_pArmsMgrLable ) ;							// 병기 관리
	GM_DELETE( m_pGeneralLabel4 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable4 ) ;						// 중간 관리자
	GM_DELETE( m_pArmsMgrGeneralChkBox ) ;					// [일반길드원]병기 관리
	GM_DELETE( m_pArmsMgrMiddleAdminChkBox ) ;				// [중간관리자]병기 관리
			   
	GM_DELETE( m_pArmsUsedLable ) ;							// 병기 이용
	GM_DELETE( m_pGeneralLabel5 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable5 ) ;						// 중간 관리자
	GM_DELETE( m_pArmsUsedGeneralChkBox ) ;					// [일반길드원]병기 이용
	GM_DELETE( m_pArmsUsedMiddleAdminChkBox ) ;				// [중간관리자]병기 이용
			   
	GM_DELETE( m_pOkButton ) ;								// 확인 버튼
	GM_DELETE( m_pCancelButton ) ;							// 취소 버튼
}

VOID	CRYLCastleEmblemRightDlg::InitValue() 
{
}

VOID	CRYLCastleEmblemRightDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleEmblemRightDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleEmblemRightDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCastleEmblemRightDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCastleEmblemRightDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCastleEmblemRightDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	if ( m_pEmbleUsedGeneralChkBox->GetChecked() ) 					// [일반길드원]성 소유 상징물 이용
		m_pEmbleUsedMiddleAdminChkBox->SetChecked( TRUE ) ;			// [중간관리자]성 소유 상징물 이용

	if ( m_pCastleDoorMgrGeneralChkBox->GetChecked() )				// [일반길드원]성문 관리
		m_pCastleDoorMgrMiddleAdminChkBox->SetChecked( TRUE ) ;		// [중간관리자]성문 관리

	if ( m_pCastleDoorUsedGeneralChkBox->GetChecked() )				// [일반길드원]성문 이용
		m_pCastleDoorUsedMiddleAdminChkBox->SetChecked( TRUE ) ;	// [중간관리자]성문 이용

	if ( m_pArmsMgrGeneralChkBox->GetChecked() )					// [일반길드원]병기 관리
		m_pArmsMgrMiddleAdminChkBox->SetChecked( TRUE ) ;			// [중간관리자]병기 관리
	
	if ( m_pArmsUsedGeneralChkBox->GetChecked() )					// [일반길드원]병기 이용
		m_pArmsUsedMiddleAdminChkBox->SetChecked( TRUE ) ;			// [중간관리자]병기 이용

	return S_OK ;
}

VOID	CRYLCastleEmblemRightDlg::SetCastleRight()
{
	if ( m_pEmbleUsedMiddleAdminChkBox->GetChecked() ) 			// [중간관리자]성 소유 상징물 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] = Guild::MIDDLE_ADMIN ;
	else
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] = Guild::MASTER ;

	if ( m_pCastleDoorMgrMiddleAdminChkBox->GetChecked() ) 			// [중간관리자]성문 관리
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_GATE ] = Guild::MIDDLE_ADMIN ;
	else
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_GATE ] = Guild::MASTER ;

	if ( m_pCastleDoorUsedMiddleAdminChkBox->GetChecked() )			// [중간관리자]성문 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_GATE ] = Guild::MIDDLE_ADMIN ;
	else
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_GATE ] = Guild::MASTER ;

	if ( m_pArmsMgrMiddleAdminChkBox->GetChecked() )				// [중간관리자]병기 관리
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_ARMS ] = Guild::MIDDLE_ADMIN ;
	else
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_ARMS ] = Guild::MASTER ;

	if ( m_pArmsUsedMiddleAdminChkBox->GetChecked() )				// [중간관리자]병기 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_ARMS ] = Guild::MIDDLE_ADMIN ;
	else
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_ARMS ] = Guild::MASTER ;

	if ( m_pEmbleUsedGeneralChkBox->GetChecked() ) 					// [일반길드원]성 소유 상징물 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] = Guild::COMMON ;
	if ( m_pCastleDoorMgrGeneralChkBox->GetChecked() )				// [일반길드원]성문 관리
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_GATE ] = Guild::COMMON ;
	if ( m_pCastleDoorUsedGeneralChkBox->GetChecked() )				// [일반길드원]성문 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_GATE ] = Guild::COMMON ;
	if ( m_pArmsMgrGeneralChkBox->GetChecked() )					// [일반길드원]병기 관리
		m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_ARMS ] = Guild::COMMON ;
	if ( m_pArmsUsedGeneralChkBox->GetChecked() )					// [일반길드원]병기 이용
		m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_ARMS ] = Guild::COMMON ;
	
	SendPacket::CharCastleRight( g_GameSession, 
								 CRYLNetworkData::Instance()->m_dwMyChrID, 
								 m_dwCastleID, 
								 m_CastleRight ) ;
}

VOID	CRYLCastleEmblemRightDlg::SetCastleID( unsigned long dwCastleID )
{
	m_dwCastleID = dwCastleID ;

	CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
	if ( lpCastle )
	{
		m_CastleRight = *lpCastle->GetCastleRight() ;

		// [일반길드원] [중간관리자] 성 소유 상징물 이용
		if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] == Guild::MIDDLE_ADMIN )
		{
			m_pEmbleUsedGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_EMBLEM ] == Guild::MASTER )
		{
			m_pEmbleUsedMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pEmbleUsedGeneralChkBox->SetChecked( FALSE ) ;
		}

		// [일반길드원] [중간관리자] 성문 관리
		if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_GATE ] == Guild::MIDDLE_ADMIN )
		{
			m_pCastleDoorMgrGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_GATE ] == Guild::MASTER )
		{
			m_pCastleDoorMgrMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pCastleDoorMgrGeneralChkBox->SetChecked( FALSE ) ;
		}

		// [일반길드원] [중간관리자] 성문 이용
		if ( m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_GATE ] == Guild::MIDDLE_ADMIN )
		{
			m_pCastleDoorUsedGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_GATE ] == Guild::MASTER )
		{
			m_pCastleDoorUsedMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pCastleDoorUsedGeneralChkBox->SetChecked( FALSE ) ;
		}

		// [일반길드원] [중간관리자] 병기 관리
		if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_ARMS ] == Guild::MIDDLE_ADMIN )
		{
			m_pArmsMgrGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CastleRight.m_aryCastleRight[ CastleRight::CONTROL_CASTLE_ARMS ] == Guild::MASTER )
		{
			m_pArmsMgrMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pArmsMgrGeneralChkBox->SetChecked( FALSE ) ;
		}

		// [일반길드원] [중간관리자] 병기 이용
		if ( m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_ARMS ] == Guild::MIDDLE_ADMIN )
		{
			m_pArmsUsedGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CastleRight.m_aryCastleRight[ CastleRight::USE_CASTLE_ARMS ] == Guild::MASTER )
		{
			m_pArmsUsedMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pArmsUsedGeneralChkBox->SetChecked( FALSE ) ;
		}
	}
}

VOID	CRYLCastleEmblemRightDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCastleEmblemRightDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCASTLEEMBLEMRIGHTDLG::OK_BUTTON :
			{
				CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;
				CRYLChattingDlg*			pChat  = CRYLChattingDlg::Instance() ;

				if ( Guild::MASTER == pASDlg->GetTitle() )
				{
					g_pRYLCastleEmblemRightDlg->SetCastleRight() ;
					g_pRYLCastleEmblemRightDlg->SetVisible( FALSE ) ;
				}
				else
				{
					pChat->AddMessage( CRYLStringTable::m_strString[ 2565 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
			break ;

		case RYLCASTLEEMBLEMRIGHTDLG::CANCEL_BUTTON :
			{
				g_pRYLCastleEmblemRightDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}