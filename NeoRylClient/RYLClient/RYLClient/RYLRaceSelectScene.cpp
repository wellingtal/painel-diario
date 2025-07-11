
#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"

#include "GMImageDib.h"
#include "GMUIScreen.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLSpriteList.h"
#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLStringTable.h"

#include "RYLSceneManager.h"
// WORK_LIST 2.2 NationSelectScene 이름 수정
#include "RYLRaceSelectScene.h"
#include "RYLCharacterSelectScene.h"

#include <RenderOption.h>
#include "GMMemory.h"

//--------------------------------------------------------------------------------------
// Race Dlg

CRYLRaceDlg*	g_pRaceDlg = NULL ;
CRYLRaceDlg::CRYLRaceDlg() : CRYLDialog()
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;	
	m_dwRace = -1;

	CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 800, 600 ) ;
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE01 ) ) ;
	/*pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE02 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE03 ) ) ;
	SetRect( &rt, 0, 0, 32,256 ) ;
	pImageDIB->DrawRect( &rt, 768, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE04_08_12 ) ) ;

	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE05 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE06 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE07 ) ) ;
	SetRect( &rt, 32, 0, 64, 256 ) ;
	pImageDIB->DrawRect( &rt, 768, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE04_08_12 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 0, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE09_10) ) ;
	SetRect( &rt, 0, 88, 256, 176 ) ;
	pImageDIB->DrawRect( &rt, 256, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE09_10) ) ;
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 512, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE11) ) ;
	SetRect( &rt, 64, 0, 96, 88 ) ;
	pImageDIB->DrawRect( &rt, 768, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RACE04_08_12) ) ;*/

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 800 ) ;
	SetHeight( 600 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_NONE ) ;
	SetDlgMoving( FALSE ) ;

	InitCtrl() ;
	g_pRaceDlg = this ;
}

CRYLRaceDlg::~CRYLRaceDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLRaceDlg::InitCtrl()
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;	

	m_pHumanCheckBox = new CRYLCheckBox( this ) ;
	m_pHumanCheckBox->SetLeft( 40 ) ;
	m_pHumanCheckBox->SetTop( 52 ) ;
	m_pHumanCheckBox->SetInstance( RYLRACEDLG::HUMAN_CHECKBOX ) ;
	m_pHumanCheckBox->OnClickEvent = DlgClick ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_UP ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

	// edith
	m_pHumanCheckBox->SetChecked( FALSE ) ;
//	m_pHumanCheckBox->SetChecked( TRUE ) ;
	m_pHumanCheckBox->SetWidth( 255 ) ;
	m_pHumanCheckBox->SetHeight( 382 ) ;
	AttachGMUIControl( m_pHumanCheckBox ) ;



	// Akhan
	m_pAkhanCheckBox = new CRYLCheckBox( this ) ;
	m_pAkhanCheckBox->SetLeft( 522 ) ;
	m_pAkhanCheckBox->SetTop( 52 ) ;
	m_pAkhanCheckBox->SetInstance( RYLRACEDLG::AKHAN_CHECKBOX ) ;
	m_pAkhanCheckBox->OnClickEvent = DlgClick ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_UP ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 255, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 255, 382 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	
	m_pAkhanCheckBox->SetChecked( FALSE ) ;
	m_pAkhanCheckBox->SetWidth( 255 ) ;
	m_pAkhanCheckBox->SetHeight( 382 ) ;
	AttachGMUIControl( m_pAkhanCheckBox ) ;

	/*m_pHumanCheckBox = new CRYLCheckBox( this ) ;
	m_pHumanCheckBox->SetLeft( 115 ) ;
	m_pHumanCheckBox->SetTop( 74 ) ;
	m_pHumanCheckBox->SetInstance( RYLRACEDLG::HUMAN_CHECKBOX ) ;
	m_pHumanCheckBox->OnClickEvent = DlgClick ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE02_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE02_03 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE02_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_UP ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_HUMAN_RACE02_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pHumanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

	// edith
	m_pHumanCheckBox->SetChecked( FALSE ) ;
//	m_pHumanCheckBox->SetChecked( TRUE ) ;
	m_pHumanCheckBox->SetWidth( 256 ) ;
	m_pHumanCheckBox->SetHeight( 512 ) ;
	AttachGMUIControl( m_pHumanCheckBox ) ;



	// Akhan
	m_pAkhanCheckBox = new CRYLCheckBox( this ) ;
	m_pAkhanCheckBox->SetLeft( 444 ) ;
	m_pAkhanCheckBox->SetTop( 74 ) ;
	m_pAkhanCheckBox->SetInstance( RYLRACEDLG::AKHAN_CHECKBOX ) ;
	m_pAkhanCheckBox->OnClickEvent = DlgClick ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE02_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE02_03 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE02_02 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_UP ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) ;
	SetRect( &rt, 0, 0, 256,256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_AKHAN_RACE02_01 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pAkhanCheckBox->AttachButtonImage( ( CGMTexture*)pImage, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	
	m_pAkhanCheckBox->SetChecked( FALSE ) ;
	m_pAkhanCheckBox->SetWidth( 256 ) ;
	m_pAkhanCheckBox->SetHeight( 512 ) ;
	AttachGMUIControl( m_pAkhanCheckBox ) ;*/

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 369 ) ;
	m_pOkButton->SetTop( 540 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff ) ;
	m_pOkButton->SetColor( 0xffffffff ) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLRACEDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;
}

VOID	CRYLRaceDlg::FinalCtrl() 
{
	GM_DELETE( m_pHumanCheckBox ) ;
	GM_DELETE( m_pAkhanCheckBox ) ;
	GM_DELETE( m_pOkButton ) ;
}

VOID	CRYLRaceDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLRaceDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLRaceDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLRaceDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{ 
	return S_OK ; 
} 

HRESULT		CRYLRaceDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{ 
	return S_OK ; 
}

HRESULT		CRYLRaceDlg::Update( BOOL &bClick, BOOL &bEdge )  
{
	return S_OK ;
}

VOID	CRYLRaceDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRaceDlg )
		return  ;

	switch( Sender->GetInstance() )
	{
	case RYLRACEDLG::HUMAN_CHECKBOX :
		g_pRaceDlg->m_pHumanCheckBox->SetChecked( TRUE ) ;
		g_pRaceDlg->m_pAkhanCheckBox->SetChecked( FALSE ) ;
		g_pRaceDlg->m_dwRace = 0 ;
		break ;
	case RYLRACEDLG::AKHAN_CHECKBOX :
		g_pRaceDlg->m_pHumanCheckBox->SetChecked( FALSE ) ;
		g_pRaceDlg->m_pAkhanCheckBox->SetChecked( TRUE ) ;
		g_pRaceDlg->m_dwRace = 1 ;
		break ;
	case RYLRACEDLG::OK_BUTTON :
		{
//	WORK_LIST 1.1 종족 선택 화면 다시 추가
			if(g_pRaceDlg->m_dwRace == -1)
				return;
			if(g_pRaceDlg->m_dwRace == 0){
				CRYLGameData::Instance()->CreateRaceInterface( g_pRaceDlg->m_dwRace, 1 ) ;	
			}else{
				CRYLGameData::Instance()->CreateRaceInterface( g_pRaceDlg->m_dwRace, 3 ) ;
			}
			
			CRYLGameData::Instance()->m_lpInterface->InitChrSelect();

			CRYLSceneManager::Instance()->SetScene( CHARACTERSELECT_SCENE ) ;
			( ( CRYLCharacterSelectScene* ) CRYLSceneManager::GetCurrentScene() )->CreateCharacterSelectDlg( g_pRaceDlg->m_dwRace ) ;
			if ( FAILED( CRYLSceneManager::Instance()->GetCurrentSceneInitResource() ) )
			{
				GMASSERT( FALSE, "IntroScene Resource Load Error!" ) ;
				return ;
			}
			g_ClientMain.Update() ;
		}
		break ;
	}
}

//--------------------------------------------------------------------------------------
// RaceSelectScene
CRYLRaceSelectScene::CRYLRaceSelectScene( INT iID ) : CRYLSceneObject( iID )  
{
	m_pRaceDlg = NULL ;
}

CRYLRaceSelectScene::~CRYLRaceSelectScene() 
{
	DeleteResourceObject() ;
	FinalCleanup() ;
}

HRESULT			CRYLRaceSelectScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::Update() 
{
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::InitResourceObject() 
{
	m_pRaceDlg = new CRYLRaceDlg ;
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::DeleteResourceObject() 
{
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::FinalCleanup() 
{
	GM_DELETE( m_pRaceDlg ) ;
	return S_OK ;
}

HRESULT			CRYLRaceSelectScene::MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) 
{
	return S_OK ;
}

VOID			CRYLRaceSelectScene::SceneShow() 
{
	if ( m_pRaceDlg ) 
		m_pRaceDlg->SetVisible( TRUE ) ;
}

VOID			CRYLRaceSelectScene::SceneHide() 
{
	if ( m_pRaceDlg ) 
		m_pRaceDlg->SetVisible( FALSE ) ;
}
