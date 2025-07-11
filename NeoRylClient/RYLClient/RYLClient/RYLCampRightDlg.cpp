
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"
#include "RYLStringTable.h"

#include "RYLNetworkData.h"
#include "RYLCamp.h"
#include "RYLCampManager.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLChattingDlg.h"

#include "RYLCampRightDlg.h"
#include "GMMemory.h"

CRYLCampRightDlg*	g_pRYLCampRightDlg = NULL ;

CRYLCampRightDlg::CRYLCampRightDlg() 
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 180, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 33 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}

	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->MakeFrameRect( 11, 46, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
//	pImageDIB->MakeFrameRect( 11, 99, 233, 27, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 11, 99, 121, 27, 0xff946b42, 0xc0000000,  1 ) ;	
		
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 191 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	

	
	g_pRYLCampRightDlg = this ;
	m_dwCampID  = 0 ;
	m_bWorldWeapon = FALSE;
	InitCtrl() ;
	m_CampRight = CampRight() ;
}

CRYLCampRightDlg::~CRYLCampRightDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCampRightDlg::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 진지 권한 설정
	m_pCampRightTitleLable = new CRYLLabel( this ) ;
	m_pCampRightTitleLable->SetLeft( 11 ) ;
	m_pCampRightTitleLable->SetTop( 18 ) ;
	m_pCampRightTitleLable->SetFontColor (0xffffffff);
	m_pCampRightTitleLable->SetAutoSize( TRUE ) ;
	m_pCampRightTitleLable->SetCaption( CRYLStringTable::m_strString[ 2562 ] ) ;
	AttachGMUIControl( m_pCampRightTitleLable ) ;
	
	// 진지 관리
	m_pCampControlLable = new CRYLLabel( this ) ;
	m_pCampControlLable->SetLeft( 15 ) ;
	m_pCampControlLable->SetTop( 45 ) ;
	m_pCampControlLable->SetFontColor (0xffffffff);
	m_pCampControlLable->SetAutoSize( TRUE ) ;
	m_pCampControlLable->SetCaption( CRYLStringTable::m_strString[ 2563 ] ) ;
	AttachGMUIControl( m_pCampControlLable ) ;

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

	// [일반길드원]진지 관리
	m_pCampControlGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pCampControlGeneralChkBox->SetLeft( 94 ) ;
	m_pCampControlGeneralChkBox->SetTop( 65 ) ;
	m_pCampControlGeneralChkBox->SetInstance( RYLCAMPRIGHTDLG::CAMPCONTROLGENERAL_CHECKBUTTON ) ;
	m_pCampControlGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCampControlGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCampControlGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCampControlGeneralChkBox ) ;

	// [중간관리자]진지 업그레이드
	m_pCampControlMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pCampControlMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pCampControlMiddleAdminChkBox->SetTop( 65 ) ;
	m_pCampControlMiddleAdminChkBox->SetInstance( RYLCAMPRIGHTDLG::CAMPCONTROLADMIN_CHECKBUTTON ) ;
	m_pCampControlMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCampControlMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCampControlMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCampControlMiddleAdminChkBox ) ;

/*
	// 진지 파괴
	m_pCampDestoryLable = new CRYLLabel( this ) ;
	m_pCampDestoryLable->SetLeft( 15 ) ;
	m_pCampDestoryLable->SetTop( 98 ) ;
	m_pCampDestoryLable->SetFontColor (0xffffffff);
	m_pCampDestoryLable->SetAutoSize( TRUE ) ;
	m_pCampDestoryLable->SetCaption( CRYLStringTable::m_strString[ 2516 ] ) ;
	AttachGMUIControl( m_pCampDestoryLable ) ;


	// 일반 길드원
	m_pGeneralLabel2 = new CRYLLabel( this ) ;
	m_pGeneralLabel2->SetLeft( 21 ) ;
	m_pGeneralLabel2->SetTop( 122 ) ;
	m_pGeneralLabel2->SetFontColor (0xffffffff);
	m_pGeneralLabel2->SetAutoSize( TRUE ) ;
	m_pGeneralLabel2->SetCaption( CRYLStringTable::m_strString[ 1150 ] ) ;
	AttachGMUIControl( m_pGeneralLabel2 ) ;

	// 중간 관리자
	m_pMiddleAdminLable2 = new CRYLLabel( this ) ;
	m_pMiddleAdminLable2->SetLeft( 132) ;
	m_pMiddleAdminLable2->SetTop( 122 ) ;
	m_pMiddleAdminLable2->SetFontColor (0xffffffff);
	m_pMiddleAdminLable2->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLable2->SetCaption( CRYLStringTable::m_strString[ 1149 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLable2 ) ;

	// [일반길드원]진지 파괴
	m_pCampDestoryGeneralChkBox = new CRYLCheckBox( this ) ;
	m_pCampDestoryGeneralChkBox->SetLeft( 94 ) ;
	m_pCampDestoryGeneralChkBox->SetTop( 118 ) ;
	m_pCampDestoryGeneralChkBox->SetInstance( RYLCAMPRIGHTDLG::CAMPDESTORYGRGENERAL_CHECKBUTTON ) ;
	m_pCampDestoryGeneralChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCampDestoryGeneralChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCampDestoryGeneralChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCampDestoryGeneralChkBox ) ;

	// [중간관리자]진지 파괴
	m_pCampDestoryMiddleAdminChkBox = new CRYLCheckBox( this ) ;
	m_pCampDestoryMiddleAdminChkBox->SetLeft( 203 ) ;
	m_pCampDestoryMiddleAdminChkBox->SetTop( 118 ) ;
	m_pCampDestoryMiddleAdminChkBox->SetInstance( RYLCAMPRIGHTDLG::CAMPDESTORYADMIN_CHECKBUTTON ) ;
	m_pCampDestoryMiddleAdminChkBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pCampDestoryMiddleAdminChkBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pCampDestoryMiddleAdminChkBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pCampDestoryMiddleAdminChkBox ) ;
*/

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 확인 버튼
	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 109 ) ;
	m_pOkButton->SetTop( 154 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLCAMPRIGHTDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 179 ) ;
	m_pCancelButton->SetTop( 154 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCAMPRIGHTDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLCampRightDlg::FinalCtrl() 
{
	GM_DELETE( m_pCampRightTitleLable ) ;					// 진지 권한 설정
			   
	GM_DELETE( m_pCampControlLable ) ;						// 진지 관리
	GM_DELETE( m_pGeneralLabel1 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable1 ) ;						// 중간 관리자
	GM_DELETE( m_pCampControlGeneralChkBox ) ;				// [일반길드원]진지 관리
	GM_DELETE( m_pCampControlMiddleAdminChkBox ) ;			// [중간관리자]진지 관리

/*			   
	GM_DELETE( m_pCampDestoryLable ) ;						// 진지 파괴
	GM_DELETE( m_pGeneralLabel2 ) ;							// 일반 길드원
	GM_DELETE( m_pMiddleAdminLable2 ) ;						// 중간 관리자
	GM_DELETE( m_pCampDestoryGeneralChkBox ) ;				// [일반길드원]진지 파괴
	GM_DELETE( m_pCampDestoryMiddleAdminChkBox ) ;			// [중간관리자]진지 파괴
*/
			   
	GM_DELETE( m_pOkButton ) ;								// 확인 버튼
	GM_DELETE( m_pCancelButton ) ;							// 취소 버튼
}

VOID	CRYLCampRightDlg::InitValue() 
{
}

VOID	CRYLCampRightDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
	CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;
	UCHAR						uTitle = pASDlg->GetTitle() ;

	if ( uTitle >= Guild::MIDDLE_ADMIN ) 
	{
		m_pOkButton->SetVisible( FALSE ) ;
		m_pCampControlGeneralChkBox->SetDisableEmptyCheck( m_pCampControlGeneralChkBox->GetChecked() ) ; 
		m_pCampControlMiddleAdminChkBox->SetDisableEmptyCheck( m_pCampControlMiddleAdminChkBox->GetChecked() ) ;
	}
	else
	{
		m_pOkButton->SetVisible( TRUE ) ;
		m_pCampControlGeneralChkBox->SetEnable( TRUE ) ;
		m_pCampControlMiddleAdminChkBox->SetEnable( TRUE ) ;

		m_pCampControlGeneralChkBox->SetChecked( m_pCampControlGeneralChkBox->GetChecked() ) ; 
		m_pCampControlMiddleAdminChkBox->SetChecked( m_pCampControlMiddleAdminChkBox->GetChecked() ) ;
	}

	ChangeDlg();
}

VOID	CRYLCampRightDlg::Hide( CGMUIObject* Sender ) 
{
	m_bWorldWeapon = FALSE;
}

VOID	CRYLCampRightDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCampRightDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCampRightDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCampRightDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	if ( m_pCampControlGeneralChkBox->GetChecked() ) 					// [일반길드원]진지 관리
		m_pCampControlMiddleAdminChkBox->SetChecked( TRUE ) ;			// [중간관리자]진지 관리

	return S_OK ;
}

VOID	CRYLCampRightDlg::SetCampRight() 
{
	if ( m_pCampControlMiddleAdminChkBox->GetChecked() ) 			// [중간관리자]진지 관리
		m_CampRight.m_aryCampRight[ CampRight::CAMP_CONTROL ] = Guild::MIDDLE_ADMIN ;
	else
		m_CampRight.m_aryCampRight[ CampRight::CAMP_CONTROL ] = Guild::MASTER ;

	if ( m_pCampControlGeneralChkBox->GetChecked() ) 				// [일반길드원]진지 관리
		m_CampRight.m_aryCampRight[ CampRight::CAMP_CONTROL ] = Guild::COMMON ;

		
	SendPacket::CharCampRight( g_GameSession, 
							   CRYLNetworkData::Instance()->m_dwMyChrID, 
							   m_dwCampID, 
							   m_CampRight ) ;
}

VOID	CRYLCampRightDlg::SetCampID( DWORD dwCampID )
{
	m_dwCampID = dwCampID ;

	CRYLCamp* lpCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;

	if ( lpCamp )
	{
		m_CampRight = *lpCamp->GetCampRight() ;

		m_pCampControlGeneralChkBox->SetChecked( TRUE ) ;
		m_pCampControlMiddleAdminChkBox->SetChecked( TRUE ) ;
		
		// [일반길드원] [중간관리자] 진지 관리
		if ( m_CampRight.m_aryCampRight[ CampRight::CAMP_CONTROL ] == Guild::MIDDLE_ADMIN )
		{
			m_pCampControlGeneralChkBox->SetChecked( FALSE ) ;
		}
		else if ( m_CampRight.m_aryCampRight[ CampRight::CAMP_CONTROL ] == Guild::MASTER )
		{
			m_pCampControlMiddleAdminChkBox->SetChecked( FALSE ) ;
			m_pCampControlGeneralChkBox->SetChecked( FALSE ) ;
		}
	}
}

VOID	CRYLCampRightDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCampRightDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCAMPRIGHTDLG::OK_BUTTON :
			{
				CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;
				CRYLChattingDlg*			pChat  = CRYLChattingDlg::Instance() ;

				if ( Guild::MASTER == pASDlg->GetTitle() )
				{
                    g_pRYLCampRightDlg->SetCampRight() ;
					g_pRYLCampRightDlg->SetVisible( FALSE ) ;
				}
				else
				{
					pChat->AddMessage( CRYLStringTable::m_strString[ 2565 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
			break ;

		case RYLCAMPRIGHTDLG::CANCEL_BUTTON :
			{
				g_pRYLCampRightDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}

VOID	
CRYLCampRightDlg::ChangeDlg()
{
	if( m_bWorldWeapon )
		m_pCampRightTitleLable->SetCaption( CRYLStringTable::m_strString[ 3419 ] ) ;	
	else
		m_pCampRightTitleLable->SetCaption( CRYLStringTable::m_strString[ 2562 ] ) ;
}
