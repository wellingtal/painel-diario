#include "RYLClientMain.h"
#include "RYLCastleStatusFrameDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLStringTable.h"
#include "WinInput.h"

#include "RYLCastleStatusPanel.h"
#include "RYLTaxControlPanel.h"
#include "RYLTaxCollectPanel.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "Community/Guild/GuildConstants.h"

#include "RYLMessageBox.h"
#include "GMMemory.h"

CRYLCastleStatusFrameDlg*	g_pCastleDlg = NULL ;

CRYLCastleStatusFrameDlg::CRYLCastleStatusFrameDlg()
{
	m_pTabButton			= NULL ;
	m_pCastleStatusPanel	= NULL ;
	m_pTaxControlPanel		= NULL ;
	m_pTaxCollectPanel		= NULL ;

	m_dwCurrCastleID		= 0 ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 266, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 206, 0, 48, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 206, 20, 48, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 281 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	g_pCastleDlg = this ;
	InitCtrl() ;
}

CRYLCastleStatusFrameDlg::~CRYLCastleStatusFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCastleStatusFrameDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 22 ) ;
	m_pTabButton->SetInstance( RYLCASTLEDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[4003], 5 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[4004], 5 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[4005], 5 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1821 ], 1821, TOOLTIP_TAB ) ;

	m_pCastleStatusPanel = new CRYLCastleStatusPanel( this ) ;
	m_pCastleStatusPanel->SetLeft( 0 ) ;
	m_pCastleStatusPanel->SetTop( 36 ) ;
	m_pCastleStatusPanel->SetWidth( 256 ) ;
	m_pCastleStatusPanel->SetHeight( 395 ) ;
	( ( CRYLCastleStatusPanel* )m_pCastleStatusPanel )->InitCtrl() ;
	m_pCastleStatusPanel->OnClickEvent = DlgClick ;
	m_pCastleStatusPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pCastleStatusPanel ) ;

	m_pTaxControlPanel = new CRYLTaxControlPanel( this ) ;
	m_pTaxControlPanel->SetLeft( 0 ) ;
	m_pTaxControlPanel->SetTop( 36 ) ;
	m_pTaxControlPanel->SetWidth( 256 ) ;
	m_pTaxControlPanel->SetHeight( 395 ) ;
	( ( CRYLTaxControlPanel* )m_pTaxControlPanel )->InitCtrl() ;
	m_pTaxControlPanel->OnClickEvent = DlgClick ;
	m_pTaxControlPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pTaxControlPanel ) ;

	m_pTaxCollectPanel = new CRYLTaxCollectPanel( this ) ;
	m_pTaxCollectPanel->SetLeft( 0 ) ;
	m_pTaxCollectPanel->SetTop( 36 ) ;
	m_pTaxCollectPanel->SetWidth( 256 ) ;
	m_pTaxCollectPanel->SetHeight( 395 ) ;
	( ( CRYLTaxCollectPanel* )m_pTaxCollectPanel )->InitCtrl() ;
	m_pTaxCollectPanel->OnClickEvent = DlgClick ;
	m_pTaxCollectPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pTaxCollectPanel ) ;
}

HRESULT		CRYLCastleStatusFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	switch ( m_pTabButton->GetFocusTabButton() )
	{
		case 0 : // 성의 현황
			( ( CRYLCastleStatusPanel* )m_pCastleStatusPanel )->Update() ;
			break ;

		case 1 : // 세율 조정
			( ( CRYLTaxControlPanel* )m_pTaxControlPanel )->Update() ;
			break ;

		case 2 : // 세금 회수
			( ( CRYLTaxCollectPanel* )m_pTaxCollectPanel )->Update() ;
			break ;
	}

	return S_OK ;
}

VOID	CRYLCastleStatusFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pCastleStatusPanel ) ;
	GM_DELETE( m_pTaxControlPanel ) ;
	GM_DELETE( m_pTaxCollectPanel ) ;
}

VOID	CRYLCastleStatusFrameDlg::Show( CGMUIObject* Sender ) 
{
	if ( m_pTabButton )
	{
		m_pTabButton->SetFocusTabButton( 0 ) ;

		if ( m_pCastleStatusPanel ) m_pCastleStatusPanel->SetVisible( TRUE ) ;
		if ( m_pTaxControlPanel ) m_pTaxControlPanel->SetVisible( FALSE ) ;
		if ( m_pTaxCollectPanel ) m_pTaxCollectPanel->SetVisible( FALSE ) ;
	}
}

VOID	CRYLCastleStatusFrameDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleStatusFrameDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLCastleStatusFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pCastleDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCASTLEDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pCastleDlg->GetTabButton()->GetFocusTabButton() ;
			switch ( iFocus )
			{
				case 0 :
				{
					( ( CRYLCastleStatusPanel* )g_pCastleDlg->GetCastleStatusPanel() )->SetVisible( TRUE ) ;
					( ( CRYLTaxControlPanel* )g_pCastleDlg->GetTaxControlPanel() )->SetVisible( FALSE ) ;
					( ( CRYLTaxCollectPanel* )g_pCastleDlg->GetTaxCollectPanel() )->SetVisible( FALSE ) ;
				}
				break ;
				
				case 1 :
				{
					// 길드 마스터 체크
					CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
					CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

					if ( pASDlg->GetTitle() == Guild::MASTER )
					{
						( ( CRYLCastleStatusPanel* )g_pCastleDlg->GetCastleStatusPanel() )->SetVisible( FALSE ) ;
						( ( CRYLTaxControlPanel* )g_pCastleDlg->GetTaxControlPanel() )->SetVisible( TRUE ) ;
						( ( CRYLTaxCollectPanel* )g_pCastleDlg->GetTaxCollectPanel() )->SetVisible( FALSE ) ;
					}
					else
					{
						g_pCastleDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
					}
				}
				break ;
				
				case 2 :
				{
					// 길드 마스터 체크
					CRYLGameScene*		 		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLGuildFrameDlg*	 		pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
					CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

					if ( pASDlg->GetTitle() == Guild::MASTER )
					{
						( ( CRYLCastleStatusPanel* )g_pCastleDlg->GetCastleStatusPanel() )->SetVisible( FALSE ) ;
						( ( CRYLTaxControlPanel* )g_pCastleDlg->GetTaxControlPanel() )->SetVisible( FALSE ) ;
						( ( CRYLTaxCollectPanel* )g_pCastleDlg->GetTaxCollectPanel() )->SetVisible( TRUE ) ;
					}
					else
					{
						g_pCastleDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] ) ;
					}
				}
				break ;
			}
		}
		break ;
	}
}

void	CRYLCastleStatusFrameDlg::SetCastleID( unsigned long dwCastleID )
{
	m_dwCurrCastleID = dwCastleID ;

	if ( m_pCastleStatusPanel ) ( ( CRYLCastleStatusPanel* )m_pCastleStatusPanel )->SetCastleID( dwCastleID ) ;
	if ( m_pTaxControlPanel ) ( ( CRYLTaxControlPanel* )m_pTaxControlPanel )->SetCastleID( dwCastleID ) ;
	if ( m_pTaxCollectPanel ) ( ( CRYLTaxCollectPanel* )m_pTaxCollectPanel )->SetCastleID( dwCastleID ) ;
}
