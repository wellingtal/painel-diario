#include <winsock2.h>
#include "RYLBattleWaitDlg.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLPicture.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"
#include "RYLTimer.h"
#include "FrameTimer.h"

#include "RYLLargeMapFrameDlg.h"

#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLBattleWaitDlg*	g_pBattleWaitDlg = NULL ;

CRYLBattleWaitDlg::CRYLBattleWaitDlg() : CRYLDialog()
{
	m_bShow					= TRUE ;
	m_bRespawn				= FALSE ;
	m_bRespawnWait			= FALSE ;
	m_bWarQuit				= FALSE ;
	m_bPortal				= FALSE;
	m_bClearPortal			= FALSE;
//	m_pStoneStatueRespawn	= NULL ;
	m_fProgressTime			= 0.0f ;
	m_lScreenFade			= 0xFFFFFFFF ;
	m_dwFadeMode			= FADE_IN ;
	m_dwTownID				= 0L ;

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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 155, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( int i = 0 ; i < 29 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 145, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->MakeFrameRect( 74, 32 , 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 57, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 82, 149, 19, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 170 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pBattleWaitDlg = this ;
}

CRYLBattleWaitDlg::~CRYLBattleWaitDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLBattleWaitDlg::InitCtrl ()
{
	// 대기순위
	m_pTitle = new CRYLLabel( this ) ;
	m_pTitle->SetLeft( 12 ) ;
	m_pTitle->SetTop( 18 ) ;
	m_pTitle->SetFontColor (0xffffffff);
	m_pTitle->SetAutoSize( TRUE ) ;
	m_pTitle->SetCaption( CRYLStringTable::m_strString[ 2566 ] ) ;
	AttachGMUIControl( m_pTitle ) ;

	// 대기순위
	m_pWaitingRankingLabel = new CRYLLabel( this ) ;
	m_pWaitingRankingLabel->SetLeft( 19 ) ;
	m_pWaitingRankingLabel->SetTop( 51 ) ;
	m_pWaitingRankingLabel->SetFontColor (0xffffffff);
	m_pWaitingRankingLabel->SetAutoSize( TRUE ) ;
	m_pWaitingRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2157 ] ) ;
	AttachGMUIControl( m_pWaitingRankingLabel ) ;

	// 대기순위 값
	m_pWaitingRankingValueLabel = new CRYLLabel( this ) ;
	m_pWaitingRankingValueLabel->SetLeft( 88 ) ;
	m_pWaitingRankingValueLabel->SetTop( 51 ) ;
	m_pWaitingRankingValueLabel->SetFontColor (0xffffffff);
	m_pWaitingRankingValueLabel->SetAutoSize( TRUE ) ;
	m_pWaitingRankingValueLabel->SetCaption( "000 / 000" ) ;
	AttachGMUIControl( m_pWaitingRankingValueLabel ) ;

	// 대기시간
	m_pWaitingTimeLabel = new CRYLLabel( this ) ;
	m_pWaitingTimeLabel->SetLeft( 19 ) ;
	m_pWaitingTimeLabel->SetTop( 76 ) ;
	m_pWaitingTimeLabel->SetFontColor (0xffffffff);
	m_pWaitingTimeLabel->SetAutoSize( TRUE ) ;
	m_pWaitingTimeLabel->SetCaption( CRYLStringTable::m_strString[ 2160 ] ) ;
	AttachGMUIControl( m_pWaitingTimeLabel ) ;

	// 대기시간 값
	m_pWaitingTimeValueLabel = new CRYLLabel( this ) ;
	m_pWaitingTimeValueLabel->SetLeft( 87 ) ;
	m_pWaitingTimeValueLabel->SetTop( 76 ) ;
	m_pWaitingTimeValueLabel->SetFontColor (0xffffffff);
	m_pWaitingTimeValueLabel->SetAutoSize( TRUE ) ;
	m_pWaitingTimeValueLabel->SetCaption( "00:00:00" ) ;
	AttachGMUIControl( m_pWaitingTimeValueLabel ) ;

	// 진행상황
	m_pProgressStateLabel = new CRYLLabel( this ) ;
	m_pProgressStateLabel->SetLeft( 19 ) ;
	m_pProgressStateLabel->SetTop( 101 ) ;
	m_pProgressStateLabel->SetFontColor (0xffffffff);
	m_pProgressStateLabel->SetAutoSize( TRUE ) ;
	m_pProgressStateLabel->SetCaption( CRYLStringTable::m_strString[ 2161 ] ) ;
	AttachGMUIControl( m_pProgressStateLabel ) ;

	// 진행상황 값
	m_pProgressStateValueLabel = new CRYLLabel( this ) ;
	m_pProgressStateValueLabel->SetLeft( 189 ) ;
	m_pProgressStateValueLabel->SetTop( 101 ) ;
	m_pProgressStateValueLabel->SetFontColor (0xffffffff);
	m_pProgressStateValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pProgressStateValueLabel ) ;

	// 진행상황 프로그래스바
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pProgressStateProgrss = new CRYLProgressBar( this ) ;
	m_pProgressStateProgrss->SetLeft( 79 ) ;
	m_pProgressStateProgrss->SetTop( 101 ) ;
	m_pProgressStateProgrss->SetWidth( 110 ) ;
	m_pProgressStateProgrss->SetHeight( 11 ) ;
	m_pProgressStateProgrss->SetMax( 10 ) ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 110, 11, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,140, 53, 250, 64 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pProgressStateProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pProgressStateProgrss->SetPosition( 0 ) ;
	AttachGMUIControl( m_pProgressStateProgrss ) ;
	GM_DELETE( pImageDIB ) ;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 위치 변경
	m_pChangePositionButton = new CRYLButton( this ) ;
	m_pChangePositionButton->SetLeft( 84 ) ;
	m_pChangePositionButton->SetTop( 134  ) ;
	m_pChangePositionButton->SetCaption( CRYLStringTable::m_strString[ 2162 ] ) ;
	m_pChangePositionButton->SetFontColor( 0xffffffff ) ;
	m_pChangePositionButton->SetColor( 0xffffffff ) ;
	m_pChangePositionButton->OnClickEvent = DlgClick ;
	m_pChangePositionButton->SetInstance( RYLBATTLEWAITDLG::CHANGEPOS_BUTTON ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pChangePositionButton ) ;

	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 158 ) ;
	m_pOKButton->SetTop( 134  ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff ) ;
	m_pOKButton->SetColor( 0xffffffff ) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLBATTLEWAITDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

}

VOID	CRYLBattleWaitDlg::FinalCtrl ()
{
	GM_DELETE( m_pTitle ) ;
	GM_DELETE( m_pWaitingRankingLabel ) ;			// 대기순위
	GM_DELETE( m_pWaitingRankingValueLabel ) ;		// 대기순위 값
	GM_DELETE( m_pWaitingTimeLabel ) ;				// 대기시간
	GM_DELETE( m_pWaitingTimeValueLabel ) ;			// 대기시간 값
	GM_DELETE( m_pProgressStateLabel ) ;			// 진행상황
	GM_DELETE( m_pProgressStateValueLabel ) ;		// 진행상황 값
	GM_DELETE( m_pProgressStateProgrss ) ;			// 진행상황 프로그래스바
	GM_DELETE( m_pChangePositionButton ) ;			// 위치 변경
	GM_DELETE( m_pOKButton ) ;						// 확인
}

VOID	CRYLBattleWaitDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLBattleWaitDlg::Hide( CGMUIObject* Sender )
{
	if(GetPortal())
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		if ( pGame->GetAutoRun() )
			pGame->SetAutoRun( TRUE ) ;

		CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
		CRYLLargeMapFrameDlg* pDlg = static_cast<CRYLLargeMapFrameDlg*>(pScene->GetLargeMapDlg());

		if(!GetClearPortal())
		{
			pDlg->ClearRespawnButton() ;			
		}
		else			
		{
			pDlg->SetClearPortal(FALSE);
		}

		pGame->SetEnableMove( TRUE ) ;	
		SetPortal(FALSE);
	}	
}

void CRYLBattleWaitDlg::SetPortal(BOOL bPortal)
{
	m_bPortal = bPortal;
}

BOOL CRYLBattleWaitDlg::GetPortal()
{
	return m_bPortal;
}

void CRYLBattleWaitDlg::SetClearPortal(BOOL bClearPortal)
{
	m_bClearPortal = bClearPortal;
}

BOOL CRYLBattleWaitDlg::GetClearPortal()
{
	return m_bClearPortal;
}

VOID	CRYLBattleWaitDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLBattleWaitDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	if ( m_lScreenFade >= m_fProgressTime && m_bRespawnWait == FALSE )
	{
		m_dwFadeMode = FADE_IN ;
		m_lScreenFade = FADE_START ;
	}
	
    float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetWeatherTimerID() ) ;

	if ( m_dwFadeMode == FADE_IN && m_bRespawnWait == TRUE )
	{
		m_lScreenFade += fUpdate ;

		m_pProgressStateProgrss->SetPosition( ( int )( ( ( float )m_lScreenFade / ( m_fProgressTime * 5 )) * 100 ) ) ;
		char Buff[ 8 ] ;
		sprintf( Buff, "%d%%", ( int )( ( ( float )m_lScreenFade / ( m_fProgressTime * 5 )) * 100 ) ) ;
		m_pProgressStateValueLabel->SetCaption( Buff ) ;
	}

	return S_OK ;	
}

void	CRYLBattleWaitDlg::SetData( unsigned long dwTownID, unsigned short wWaitOrder, 
								   unsigned char cHour, unsigned char cMin, unsigned char cSec, bool bCount ) 
{
	char szBuff[ 32 ] ;
	sprintf( szBuff, "%d", wWaitOrder ) ;
	m_pWaitingRankingValueLabel->SetCaption( szBuff ) ;
	sprintf( szBuff, "%d : %d : %d", cHour, cMin, cSec ) ;
	m_pWaitingTimeValueLabel->SetCaption( szBuff ) ;

	m_dwTownID = dwTownID ;

	// 초 : ( 시간 * 분 * 초 ) + ( 분 * 초 ) + ( 초 )
	m_pProgressStateProgrss->SetMax( 100 ) ;
	m_fProgressTime = ( ( (cHour * 60 * 60 ) + ( cMin * 60 ) + ( cSec ) ) ) ;
	m_bRespawnWait  = bCount ;

	if ( m_bRespawnWait )
	{
		m_lScreenFade	= FADE_START ;
		m_dwFadeMode	= FADE_IN ;
	}
}

VOID	CRYLBattleWaitDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pBattleWaitDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLBATTLEWAITDLG::CHANGEPOS_BUTTON :
		{
			g_pBattleWaitDlg->SetClearPortal(TRUE);
			g_pBattleWaitDlg->SetVisible( FALSE ) ;

			SendPacket::CharRespawnInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID ) ;
			break ;
		}

		case RYLBATTLEWAITDLG::OK_BUTTON :
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			POS pPos = { 0.0f, 0.0f, 0.0f };

			SendPacket::CharRespawn( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				ClientConstants::RespawnCmd_Select, pPos, g_pBattleWaitDlg->m_dwTownID ) ;

			g_pBattleWaitDlg->m_pChangePositionButton->SetEnable( FALSE ) ;
			g_pBattleWaitDlg->m_pOKButton->SetEnable( FALSE ) ;
			break ;
		}
	}
}

void	CRYLBattleWaitDlg::InitDlg( BOOL bCounting ) 
{
	if ( !bCounting )
	{
		m_pChangePositionButton->SetEnable( TRUE ) ;
		m_pOKButton->SetEnable( TRUE ) ;
	}
	
	m_pProgressStateProgrss->SetPosition( 0 ) ;
	m_pProgressStateValueLabel->SetCaption( "0%" ) ;
}
