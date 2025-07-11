#include <winsock2.h>
#include "RYLRespawnDlg.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLPicture.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include <Network/ClientSocket/Send/SendAttack.h>
#include "GMMemory.h"

CRYLRespawnDlg*	g_pRYLRespawnDlg = NULL ;

CRYLRespawnDlg::CRYLRespawnDlg() : CRYLDialog()
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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 86, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 16 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 95 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pRYLRespawnDlg = this ;
}

CRYLRespawnDlg::~CRYLRespawnDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLRespawnDlg::InitCtrl ()
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	// 위치 변경
	m_pRespawnButton = new CRYLButton( this ) ;
	m_pRespawnButton->SetLeft( 87 ) ;
	m_pRespawnButton->SetTop( 24  ) ;
	m_pRespawnButton->SetCaption( CRYLStringTable::m_strString[ 2567 ] ) ;
	m_pRespawnButton->SetFontColor( 0xffffffff ) ;
	m_pRespawnButton->SetColor( 0xffffffff ) ;
	m_pRespawnButton->OnClickEvent = DlgClick ;
	m_pRespawnButton->SetInstance( RYLRESPAWNDLG::RESPAWNNOW_BUTTON ) ;
	m_pRespawnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRespawnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRespawnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRespawnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRespawnButton ) ;

	m_pRespawnChoiceButton = new CRYLButton( this ) ;
	m_pRespawnChoiceButton->SetLeft( 87 ) ;
	m_pRespawnChoiceButton->SetTop( 55  ) ;
	m_pRespawnChoiceButton->SetCaption( CRYLStringTable::m_strString[ 2568 ] ) ;
	m_pRespawnChoiceButton->SetFontColor( 0xffffffff ) ;
	m_pRespawnChoiceButton->SetColor( 0xffffffff ) ;
	m_pRespawnChoiceButton->OnClickEvent = DlgClick ;
	m_pRespawnChoiceButton->SetInstance( RYLRESPAWNDLG::RESPAWNCHOICE_BUTTON ) ;
	m_pRespawnChoiceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRespawnChoiceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRespawnChoiceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRespawnChoiceButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRespawnChoiceButton ) ;
}

VOID	CRYLRespawnDlg::FinalCtrl ()
{
	GM_DELETE( m_pRespawnButton ) ;			// 바로 부활
	GM_DELETE( m_pRespawnChoiceButton ) ;	// 부활장소 선택
}

VOID	CRYLRespawnDlg::Show( CGMUIObject* Sender )
{
	CRYLGameData::Instance()->m_bSimpleMouse = TRUE ;
}
	
VOID	CRYLRespawnDlg::Hide( CGMUIObject* Sender )
{
	CRYLGameData::Instance()->m_bSimpleMouse = FALSE ;
}

VOID	CRYLRespawnDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLRespawnDlg::SetVisible( BOOL bVisible )
{
	if ( bVisible )
	{
		// 레져렉션 인증창이 떠 있거나, 라지맵, 리스폰 정보창이 떠 있으면 뜨지 않아야 한다!!
		CRYLGameScene* pScene = (CRYLGameScene*) CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		if ( !pScene ||
			 pScene->GetLargeMapDlg()->GetVisible() ||
			 pScene->GetBattleWaitDlg()->GetVisible() ||
			 0 != CRYLGameData::Instance()->m_cAuthorizeCmd )
		{
			return;
		}
	}

	CRYLDialog::SetVisible( bVisible ) ;
}

HRESULT		CRYLRespawnDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	return S_OK ;	
}

VOID	CRYLRespawnDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pRYLRespawnDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLRESPAWNDLG::RESPAWNNOW_BUTTON :
		g_CharacterData.CharRespawn() ;
		break ;
	case RYLRESPAWNDLG::RESPAWNCHOICE_BUTTON :
		{
			SendPacket::CharRespawnInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID ) ;
		}
		break ;
	}
}
