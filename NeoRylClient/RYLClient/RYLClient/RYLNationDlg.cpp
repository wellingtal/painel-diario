
// WORK_LIST 2.3 계정 국적 변경 UI 추가

#include <winsock2.h>

#include <Creature/CreatureStructure.h>

#include "GMFontTexture.h"
#include "GMUIScreen.h"
#include "GMUIScreen.h"
#include "GMImageDib.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include "RYLNetWorkData.h"

#include "RYLCharacterSelectScene.h"
#include "RYLSceneManager.h"

#include "RYLNationSelectDlg.h"
#include "RYLNationDlg.h"
#include "GMMemory.h"

CRYLNationDlg*	g_pNationDlg = NULL ;
CRYLNationDlg::CRYLNationDlg() : CRYLDialog()
{
	m_pKarterantBtn = NULL ;
	m_pMerkadiaBtn = NULL ;
	m_pPirateBtn = NULL ;
	m_pDescLabel = NULL ;
	m_pNationLabel = NULL ;
	m_dwCurrentNation = 0L ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Body Image
	CRYLSpriteList::Instance()->CreateGMImage( 200, 200, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 132, 117, 257, 167 ) ;
	pImageDIB->DrawRect( &rt, 75, 22, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_KARTERANT_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachWindowTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 200 ) ;
	SetHeight( 200 ) ;
	SetLeft( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) ;
	SetTop( 26 ) ;
	SetFormType( RYLDLG_TYPE_NONE ) ;
	SetDlgMoving( FALSE ) ;

	InitCtrl() ;
	g_pNationDlg = this ;
}

CRYLNationDlg::~CRYLNationDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLNationDlg::InitCtrl()
{
	RECT rtNormal, rtDown, rtUp, rtDisible;

	SetRect( &rtNormal, 3, 4, 71, 112 ) ;
	SetRect( &rtDown, 3, 4, 71, 112 ) ;
	SetRect( &rtUp, 3, 4, 71, 112 ) ;
	SetRect( &rtDisible, 3, 4, 71, 112 ) ;

	// 카르테란트 국적 표시 버튼
	CGMImageDib* pKarterantSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_KARTERANT_256x256 ) ;
	m_pKarterantBtn = new CRYLButton( this ) ;
	m_pKarterantBtn->SetLeft( 98 ) ;
	m_pKarterantBtn->SetTop( 72 ) ;
	m_pKarterantBtn->OnClickEvent = DlgClick ;
	m_pKarterantBtn->SetInstance( RYLNATIONDLG::KARTERANT_BUTTON ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rtNormal, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rtDown, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rtUp, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rtDisible, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pKarterantBtn ) ;
	m_pKarterantBtn->SetVisible( FALSE ) ;

	// 메르카디아 국적 표시 버튼
	CGMImageDib* pMerkadiaSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_MERKADIA_256x256 ) ;
	m_pMerkadiaBtn = new CRYLButton( this ) ;
	m_pMerkadiaBtn->SetLeft( 98 ) ;
	m_pMerkadiaBtn->SetTop( 72 ) ;
	m_pMerkadiaBtn->OnClickEvent = DlgClick ;
	m_pMerkadiaBtn->SetInstance( RYLNATIONDLG::MERKADIA_BUTTON ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rtNormal, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rtDown, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rtUp, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rtDisible, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pMerkadiaBtn ) ;
	m_pMerkadiaBtn->SetVisible( FALSE ) ;

	// 신의 해적단 국적 표시 버튼
	CGMImageDib* pPirateSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_PIRATE_256x256 ) ;
	m_pPirateBtn = new CRYLButton( this ) ;
	m_pPirateBtn->SetLeft( 98 ) ;
	m_pPirateBtn->SetTop( 72 ) ;
	m_pPirateBtn->OnClickEvent = DlgClick ;
	m_pPirateBtn->SetInstance( RYLNATIONDLG::PIRATE_BUTTON ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rtNormal, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rtDown, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rtUp, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rtDisible, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPirateBtn ) ;
	m_pPirateBtn->SetVisible( FALSE ) ;

	// 설명 글자
	m_pDescLabel = new CRYLLabel( this ) ;
	m_pDescLabel->SetLeft( 5 ) ;
	m_pDescLabel->SetTop( 8 ) ;
	m_pDescLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pDescLabel->SetAutoSize( TRUE ) ;
	m_pDescLabel->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	m_pDescLabel->SetCaption( CRYLStringTable::m_strString[2968] ) ;
	AttachGMUIControl( m_pDescLabel ) ;

	// 국가 이름
	m_pNationLabel = new CRYLLabel( this ) ;
	m_pNationLabel->SetLeft( 99 ) ;
	m_pNationLabel->SetTop( 41 ) ;
	m_pNationLabel->SetFontColor( 0xFFFFFFFF ) ;
	m_pNationLabel->SetAutoSize( TRUE ) ;
	m_pNationLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pNationLabel ) ;
}

VOID	CRYLNationDlg::FinalCtrl() 
{
	GM_DELETE( m_pKarterantBtn ) ;
	GM_DELETE( m_pMerkadiaBtn ) ;
	GM_DELETE( m_pPirateBtn ) ;
	GM_DELETE( m_pDescLabel ) ;
	GM_DELETE( m_pNationLabel ) ;
}

VOID	CRYLNationDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLNationDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLNationDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLNationDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{ 
	return S_OK ; 
} 

HRESULT		CRYLNationDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{ 
	return S_OK ; 
}

HRESULT		CRYLNationDlg::Update( BOOL &bClick, BOOL &bEdge )  
{
	return S_OK ;
}

VOID	CRYLNationDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pNationDlg )
		return  ;

	switch( Sender->GetInstance() )
	{
		case RYLNATIONDLG::KARTERANT_BUTTON :
		case RYLNATIONDLG::MERKADIA_BUTTON :
		case RYLNATIONDLG::PIRATE_BUTTON :
		{
			// DISABLE BUTTON CHAR SELECT 3 ND RACE BY ALEX
///--------------
/*
			// 3국가 체제에서 다시 2국 체제로 돌아가게 되어 더이상 국적 변경은 불가능합니다. (2005-07-06 by 로딘)

			CRYLGameData* pGame = CRYLGameData::Instance() ;
			CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* ) CRYLSceneManager::Instance()->GetCurrentScene() ;
			if ( !pScene->GetNationSelectDlg()->GetVisible() )
			{
				( ( CRYLNationSelectDlg* )( pScene->GetNationSelectDlg() ) )->InitValue( pGame->m_dwClientRace, pGame->m_cAccountNation ) ;
				pScene->GetNationSelectDlg()->SetVisible( TRUE ) ;
			}
*/
//----------------
			break ;
		}
	}
}

VOID	CRYLNationDlg::SetCurrentNation( DWORD dwNation )
{
	switch ( dwNation )
	{
	case Creature::KARTERANT :
		m_pKarterantBtn->SetVisible( TRUE ) ;
		m_pKarterantBtn->SetEnable( TRUE ) ;
		m_pMerkadiaBtn->SetVisible( FALSE ) ;
		m_pMerkadiaBtn->SetEnable( FALSE ) ;
		m_pPirateBtn->SetVisible( FALSE ) ;
		m_pPirateBtn->SetEnable( FALSE ) ;
		m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2233 ] ) ;
		m_dwCurrentNation = dwNation ;
		break ;

	case Creature::MERKADIA :
		m_pKarterantBtn->SetVisible( FALSE ) ;
		m_pKarterantBtn->SetEnable( FALSE ) ;
		m_pMerkadiaBtn->SetVisible( TRUE ) ;
		m_pMerkadiaBtn->SetEnable( TRUE ) ;
		m_pPirateBtn->SetVisible( FALSE ) ;
		m_pPirateBtn->SetEnable( FALSE ) ;
		m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2264 ] ) ;
		m_dwCurrentNation = dwNation ;
		break ;

	case Creature::ALMIGHTY_PIRATE :
		m_pKarterantBtn->SetVisible( FALSE ) ;
		m_pKarterantBtn->SetEnable( FALSE ) ;
		m_pMerkadiaBtn->SetVisible( FALSE ) ;
		m_pMerkadiaBtn->SetEnable( FALSE ) ;
		m_pPirateBtn->SetVisible( TRUE ) ;
		m_pPirateBtn->SetEnable( TRUE ) ;
		m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2268 ] ) ;
		m_dwCurrentNation = dwNation ;
		break ;

	default :
		m_pKarterantBtn->SetVisible( FALSE ) ;
		m_pKarterantBtn->SetEnable( FALSE ) ;
		m_pMerkadiaBtn->SetVisible( FALSE ) ;
		m_pMerkadiaBtn->SetEnable( FALSE ) ;
		m_pPirateBtn->SetVisible( FALSE ) ;
		m_pPirateBtn->SetEnable( FALSE ) ;
		m_pNationLabel->SetCaption( "" ) ;
		m_dwCurrentNation = 0 ;
		break ;
	}
}