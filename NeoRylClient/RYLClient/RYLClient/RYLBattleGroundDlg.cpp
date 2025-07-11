#include <winsock2.h>
#include "RYLBattleGroundDlg.h"

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
#include "RYLTimer.h"
#include "FrameTimer.h"

#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <RenderOption.h>

#include <Creature/Character/CharacterClass.h>
#include "GMMemory.h"

CRYLBattleGroundDlg*	g_pBattleGroundDlg = NULL ;

CRYLBattleGroundDlg::CRYLBattleGroundDlg() : CRYLDialog()
{
	m_bShow					= TRUE ;
	m_bRespawn				= FALSE ;
	m_bRespawnWait			= FALSE ;
	m_bWarQuit				= FALSE ;
	m_pStoneStatueRespawn	= NULL ;
	m_fProgressTime			= 0.0f ;
	m_lScreenFade			= 0xFFFFFFFF ;
	m_dwFadeMode			= FADE_IN ;

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
	
	pImageDIB->MakeFrameRect( 74, 9 , 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 34, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 59, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 94, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 74, 119, 149, 19, 0xff959595, 0xc0000000,  1 ) ;

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

	g_pBattleGroundDlg = this ;
}

CRYLBattleGroundDlg::~CRYLBattleGroundDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLBattleGroundDlg::InitCtrl ()
{
	// 대기순위
	m_pWaitingRankingLabel = new CRYLLabel( this ) ;
	m_pWaitingRankingLabel->SetLeft( 16 ) ;
	m_pWaitingRankingLabel->SetTop( 28 ) ;
	m_pWaitingRankingLabel->SetFontColor (0xffffffff);
	m_pWaitingRankingLabel->SetAutoSize( TRUE ) ;
	m_pWaitingRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2157 ] ) ;
	AttachGMUIControl( m_pWaitingRankingLabel ) ;

	// 대기순위 값
	m_pWaitingRankingValueLabel = new CRYLLabel( this ) ;
	m_pWaitingRankingValueLabel->SetLeft( 86 ) ;
	m_pWaitingRankingValueLabel->SetTop( 28 ) ;
	m_pWaitingRankingValueLabel->SetFontColor (0xffffffff);
	m_pWaitingRankingValueLabel->SetAutoSize( TRUE ) ;
	m_pWaitingRankingValueLabel->SetCaption( "000 / 000" ) ;
	AttachGMUIControl( m_pWaitingRankingValueLabel ) ;

	// 현재휴먼
	m_pCurrentHumanLabel = new CRYLLabel( this ) ;
	m_pCurrentHumanLabel->SetLeft( 16 ) ;
	m_pCurrentHumanLabel->SetTop( 53 ) ;
	m_pCurrentHumanLabel->SetFontColor (0xffffffff);
	m_pCurrentHumanLabel->SetAutoSize( TRUE ) ;
	m_pCurrentHumanLabel->SetCaption( CRYLStringTable::m_strString[ 2158 ] ) ;
	AttachGMUIControl( m_pCurrentHumanLabel ) ;

	// 현재휴먼 값
	m_pCurrentHumanValueLabel = new CRYLLabel( this ) ;
	m_pCurrentHumanValueLabel->SetLeft( 103 ) ;
	m_pCurrentHumanValueLabel->SetTop( 53 ) ;
	m_pCurrentHumanValueLabel->SetFontColor (0xffffffff);
	m_pCurrentHumanValueLabel->SetAutoSize( TRUE ) ;
	m_pCurrentHumanValueLabel->SetCaption( "000" ) ;
	AttachGMUIControl( m_pCurrentHumanValueLabel ) ;

	// 현재아칸
	m_pCurrentAkhanLabel = new CRYLLabel( this ) ;
	m_pCurrentAkhanLabel->SetLeft( 16 ) ;
	m_pCurrentAkhanLabel->SetTop( 78 ) ;
	m_pCurrentAkhanLabel->SetFontColor (0xffffffff);
	m_pCurrentAkhanLabel->SetAutoSize( TRUE ) ;
	m_pCurrentAkhanLabel->SetCaption( CRYLStringTable::m_strString[ 2159 ] ) ;
	AttachGMUIControl( m_pCurrentAkhanLabel ) ;

	// 현재아칸 값
	m_pCurrentAkhanValueLabel = new CRYLLabel( this ) ;
	m_pCurrentAkhanValueLabel->SetLeft( 103 ) ;
	m_pCurrentAkhanValueLabel->SetTop( 78 ) ;
	m_pCurrentAkhanValueLabel->SetFontColor (0xffffffff);
	m_pCurrentAkhanValueLabel->SetAutoSize( TRUE ) ;
	m_pCurrentAkhanValueLabel->SetCaption( "000" ) ;
	AttachGMUIControl( m_pCurrentAkhanValueLabel ) ;

	// 대기시간
	m_pWaitingTimeLabel = new CRYLLabel( this ) ;
	m_pWaitingTimeLabel->SetLeft( 16 ) ;
	m_pWaitingTimeLabel->SetTop( 113 ) ;
	m_pWaitingTimeLabel->SetFontColor (0xffffffff);
	m_pWaitingTimeLabel->SetAutoSize( TRUE ) ;
	m_pWaitingTimeLabel->SetCaption( CRYLStringTable::m_strString[ 2160 ] ) ;
	AttachGMUIControl( m_pWaitingTimeLabel ) ;

	// 대기시간 값
	m_pWaitingTimeValueLabel = new CRYLLabel( this ) ;
	m_pWaitingTimeValueLabel->SetLeft( 86 ) ;
	m_pWaitingTimeValueLabel->SetTop( 113 ) ;
	m_pWaitingTimeValueLabel->SetFontColor (0xffffffff);
	m_pWaitingTimeValueLabel->SetAutoSize( TRUE ) ;
	m_pWaitingTimeValueLabel->SetCaption( "00:00:00" ) ;
	AttachGMUIControl( m_pWaitingTimeValueLabel ) ;

	// 진행상황
	m_pProgressStateLabel = new CRYLLabel( this ) ;
	m_pProgressStateLabel->SetLeft( 16 ) ;
	m_pProgressStateLabel->SetTop( 138 ) ;
	m_pProgressStateLabel->SetFontColor (0xffffffff);
	m_pProgressStateLabel->SetAutoSize( TRUE ) ;
	m_pProgressStateLabel->SetCaption( CRYLStringTable::m_strString[ 2161 ] ) ;
	AttachGMUIControl( m_pProgressStateLabel ) ;

	// 진행상황 값
	m_pProgressStateValueLabel = new CRYLLabel( this ) ;
	m_pProgressStateValueLabel->SetLeft( 189 ) ;
	m_pProgressStateValueLabel->SetTop( 138 ) ;
	m_pProgressStateValueLabel->SetFontColor (0xffffffff);
	m_pProgressStateValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pProgressStateValueLabel ) ;

	// 진행상황 프로그래스바
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pProgressStateProgrss = new CRYLProgressBar( this ) ;
	m_pProgressStateProgrss->SetLeft( 78 ) ;
	m_pProgressStateProgrss->SetTop( 138 ) ;
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
	m_pChangePositionButton->SetLeft( 178 ) ;
	m_pChangePositionButton->SetTop( 23  ) ;
	m_pChangePositionButton->SetCaption( CRYLStringTable::m_strString[ 2162 ] ) ;
	m_pChangePositionButton->SetFontColor( 0xffffffff ) ;
	m_pChangePositionButton->SetColor( 0xffffffff ) ;
	m_pChangePositionButton->OnClickEvent = DlgClick ;
	m_pChangePositionButton->SetInstance( RYLBATTLEGROUNDDLG::CHANGEPOS_BUTTON ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangePositionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pChangePositionButton ) ;
}

VOID	CRYLBattleGroundDlg::FinalCtrl ()
{
	GM_DELETE( m_pWaitingRankingLabel ) ;			// 대기순위
	GM_DELETE( m_pWaitingRankingValueLabel ) ;		// 대기순위 값
	GM_DELETE( m_pCurrentHumanLabel ) ;				// 현재휴먼
	GM_DELETE( m_pCurrentHumanValueLabel ) ;		// 현재휴먼 값
	GM_DELETE( m_pCurrentAkhanLabel ) ;				// 현재아칸
	GM_DELETE( m_pCurrentAkhanValueLabel ) ;		// 현재아칸 값
	GM_DELETE( m_pWaitingTimeLabel ) ;				// 대기시간
	GM_DELETE( m_pWaitingTimeValueLabel ) ;			// 대기시간 값
	GM_DELETE( m_pProgressStateLabel ) ;			// 진행상황
	GM_DELETE( m_pProgressStateValueLabel ) ;		// 진행상황 값
	GM_DELETE( m_pProgressStateProgrss ) ;			// 진행상황 프로그래스바
	GM_DELETE( m_pChangePositionButton ) ;			// 위치 변경
}

VOID	CRYLBattleGroundDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLBattleGroundDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLBattleGroundDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLBattleGroundDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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
	
	float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID() ) ;

	if ( m_dwFadeMode == FADE_IN && m_bRespawnWait == TRUE )
	{
		m_lScreenFade += fUpdate ;

		m_pProgressStateProgrss->SetPosition( ( int )( ( ( float )m_lScreenFade / m_fProgressTime ) * 100 ) ) ;
		char Buff[ 8 ] ;
		sprintf( Buff, "%d%%", ( int )( ( ( float )m_lScreenFade / m_fProgressTime ) * 100 ) ) ;
		m_pProgressStateValueLabel->SetCaption( Buff ) ;
	}

	return S_OK ;	
}

VOID	CRYLBattleGroundDlg::SetMyRanking( LPSTR szMyRanking, LPSTR szTotalRanking ) 	
{
	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%s / %s", szMyRanking, szTotalRanking ) ;
	m_pWaitingRankingValueLabel->SetCaption( szBuff ) ;
}

VOID	CRYLBattleGroundDlg::SetLeftoverTime( LPSTR szLeftoverTime )		
{	
	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%s", szLeftoverTime ) ;
	m_pWaitingTimeValueLabel->SetCaption( szBuff ) ;
	INT iTime = atoi( szBuff ) ;
	m_pProgressStateProgrss->SetMax( iTime ) ;
}

VOID	CRYLBattleGroundDlg::SetCurrentHumanNum( LPSTR szCurrentHumanNum ) 
{
	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%s", szCurrentHumanNum ) ;
	m_pCurrentHumanValueLabel->SetCaption( szBuff ) ;
}

VOID	CRYLBattleGroundDlg::SetCurrentAkhanNum( LPSTR szCurrentAkhanNum ) 
{
	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%s", szCurrentAkhanNum ) ;
	m_pCurrentAkhanValueLabel->SetCaption( szBuff ) ;
}


VOID	CRYLBattleGroundDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pBattleGroundDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLBATTLEGROUNDDLG::CHANGEPOS_BUTTON :
		{
			SendPacket::CharRespawnWaitQueue(g_GameSession,  CRYLNetworkData::Instance()->m_dwMyChrID ) ;
			CRYLCommunityData::Instance()->SetStoneStatueRespawn( FALSE ) ;
			g_pBattleGroundDlg->SetWarQuit( TRUE ) ;
			g_pBattleGroundDlg->InitRespawnPoint() ;
		}
		break ;
	}
}

VOID	CRYLBattleGroundDlg::SetStoneStatueRespawnInfo( LPSTONESTATUERESPAWNINFOEX	pStoneStatueRespawn ) 
{
	m_pStoneStatueRespawn = pStoneStatueRespawn ;

	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%d / %d", m_pStoneStatueRespawn->usTurn_Out, m_pStoneStatueRespawn->usWaitNum_Out ) ;
	m_pWaitingRankingValueLabel->SetCaption( szBuff ) ;
	
	sprintf( szBuff, "%d", m_pStoneStatueRespawn->usLeftTime_Out ) ;
	m_pWaitingTimeValueLabel->SetCaption( szBuff ) ;
	INT iTime = atoi( szBuff ) ;
	m_pProgressStateProgrss->SetMax( iTime ) ;

	sprintf( szBuff, "%d", m_pStoneStatueRespawn->usHumanNum_Out ) ;
	m_pCurrentHumanValueLabel->SetCaption( szBuff ) ;
	sprintf( szBuff, "%d", m_pStoneStatueRespawn->usAkhanNum_Out ) ;
	m_pCurrentAkhanValueLabel->SetCaption( szBuff ) ;


	if ( !m_bWarQuit )
	{
		CRYLCommunityData::Instance()->SetStoneStatueRespawn( TRUE ) ;
	}

	
	if ( m_pStoneStatueRespawn->usHumanNum_Out == 0 )
		m_pStoneStatueRespawn->usHumanNum_Out = 1 ;
	if ( m_pStoneStatueRespawn->usAkhanNum_Out == 0 )
		m_pStoneStatueRespawn->usAkhanNum_Out = 1 ;

	switch ( CRYLGameData::Instance()->m_dwClientRace )
	{
	case CClass::RaceType::HUMAN :
		{
			m_fProgressTime = m_pStoneStatueRespawn->usHumanNum_Out / m_pStoneStatueRespawn->usAkhanNum_Out * 2500.0f ;
		}
		break ;

	case CClass::RaceType::AKHAN :
		{
			m_fProgressTime = m_pStoneStatueRespawn->usAkhanNum_Out / m_pStoneStatueRespawn->usHumanNum_Out * 2500.0f ;
		}
		break ;
	}
	if ( m_pStoneStatueRespawn->usLeftTime_Out == 0 )
	{
		m_lScreenFade	= FADE_START ;
		m_dwFadeMode	= FADE_IN ;
		m_bRespawnWait	= FALSE ;
	}
}

VOID	CRYLBattleGroundDlg::InitRespawnPoint()
{
	m_lScreenFade	= FADE_START ;
	m_dwFadeMode	= FADE_IN ;
	m_bRespawnWait	= FALSE ;
}
