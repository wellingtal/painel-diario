#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLSummonDlg.h"
#include "RYLButton.h"
#include "RYLProgressBar.h"
#include <WinInput.h>
#include "RYLGameScene.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include <RenderOption.h>
#include "GMMemory.h"
CRYLSummonDlg*	g_pSummonDlg = NULL ;

CRYLSummonDlg::CRYLSummonDlg() : CRYLDialog()
{
	m_pAttackButton			= NULL ;	
	m_pReturnButton			= NULL ;		
	m_pDefenseButton		= NULL ;	
	m_pExitButton			= NULL ;		
	m_pHPProgressBar		= NULL ;	

	RECT rt ;
	SetRect( &rt, 0, 97, 155, 138 ) ;
	AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	
	SetWidth( 151 ) ;
	SetHeight( 41 ) ;

	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_NONE ) ;
	
	g_pSummonDlg = this ;	
	InitCtrl() ;
}

CRYLSummonDlg::~CRYLSummonDlg()
{
	FinalCtrl() ;
}

VOID CRYLSummonDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ;
	
	// 공격 버튼
	m_pAttackButton = new CRYLButton( this ) ;
	m_pAttackButton->SetLeft( 2 ) ;
	m_pAttackButton->SetTop( 14 ) ;
	m_pAttackButton->SetWidth( 25 ) ;
	m_pAttackButton->SetHeight( 25 ) ;
	m_pAttackButton->OnClickEvent = DlgClick ;
	m_pAttackButton->SetInstance( RYLSUMMONDLG::ATTACK_BUTTON ) ;
	SetRect( &rt, 0, 138, 27, 163 ) ;
	m_pAttackButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 188, 27, 213 ) ;
	m_pAttackButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 163, 27, 188 ) ;
	m_pAttackButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 0, 138, 27, 163 ) ;
	m_pAttackButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pAttackButton ) ;

	// 돌아오기 버튼
	m_pReturnButton = new CRYLButton( this ) ;
	m_pReturnButton->SetLeft( 29 ) ;
	m_pReturnButton->SetTop( 14 ) ;
	m_pReturnButton->SetWidth( 25 ) ;
	m_pReturnButton->SetHeight( 25 ) ;
	m_pReturnButton->OnClickEvent = DlgClick ;
	m_pReturnButton->SetInstance( RYLSUMMONDLG::RETURN_BUTTON ) ;
	SetRect( &rt, 27, 138, 54, 163 ) ;
	m_pReturnButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 27, 188, 54, 213 ) ;
	m_pReturnButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 27, 163, 54, 188 ) ;
	m_pReturnButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 27, 138, 54, 163 ) ;
	m_pReturnButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pReturnButton ) ;

	// 방어 버튼
	m_pDefenseButton = new CRYLButton( this ) ;
	m_pDefenseButton->SetLeft( 56 ) ;
	m_pDefenseButton->SetTop( 14 ) ;
	m_pDefenseButton->SetWidth( 25 ) ;
	m_pDefenseButton->SetHeight( 25 ) ;
	m_pDefenseButton->OnClickEvent = DlgClick ;
	m_pDefenseButton->SetInstance( RYLSUMMONDLG::DEFENSE_BUTTON ) ;
	SetRect( &rt, 54, 138, 81, 163 ) ;
	m_pDefenseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 54, 188, 81, 213 ) ;
	m_pDefenseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 54, 163, 81, 188 ) ;
	m_pDefenseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 45, 138, 81, 163 ) ;
	m_pDefenseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pDefenseButton ) ;

	// 소환종료 버튼
	m_pExitButton = new CRYLButton( this ) ;
	m_pExitButton->SetLeft( 83 ) ;
	m_pExitButton->SetTop( 14 ) ;
	m_pExitButton->SetWidth( 25 ) ;
	m_pExitButton->SetHeight( 25 ) ;
	m_pExitButton->OnClickEvent = DlgClick ;
	m_pExitButton->SetInstance( RYLSUMMONDLG::EXIT_BUTTON ) ;
	SetRect( &rt, 81, 138, 108, 163 ) ;
	m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 81, 188, 108, 213 ) ;
	m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 81, 163, 108, 188 ) ;
	m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 81, 138, 108, 163 ) ;
	m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pExitButton ) ;

	//CRYLProgressBar*	m_pHPProgressBar ;				// HP
	//----------------------------------------------------------------
	m_pHPProgressBar = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pHPProgressBar->SetLeft( 114 ) ;
	m_pHPProgressBar->SetTop( 27 ) ;
	m_pHPProgressBar->SetWidth( 33 ) ;
	m_pHPProgressBar->SetHeight( 7 ) ;
	m_pHPProgressBar->SetMax( 10 ) ;
	SetRect( &rt, 185, 0, 218, 7 ) ;
	m_pHPProgressBar->AttachBarImageTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	AttachGMUIControl( m_pHPProgressBar ) ;		
	m_pHPProgressBar->SetPosition( 10 ) ;
}

VOID CRYLSummonDlg::FinalCtrl ()
{
	GM_DELETE( m_pAttackButton ) ;
	GM_DELETE( m_pReturnButton ) ;
	GM_DELETE( m_pDefenseButton ) ;
	GM_DELETE( m_pExitButton ) ;	
	GM_DELETE( m_pHPProgressBar ) ; 
}

VOID		CRYLSummonDlg::SetSummonHPProgress( UINT uiCurrentHP, UINT uiMaxHP ) 
{
	m_pHPProgressBar->SetMax( uiMaxHP ) ;
	m_pHPProgressBar->SetPosition( uiCurrentHP ) ;
}

VOID CRYLSummonDlg::Show( CGMUIObject* Sender )
{
}

VOID CRYLSummonDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID CRYLSummonDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLSummonDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

    RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( CRYLGameData::Instance()->m_csStatus.m_dwSummonID );
    if ( pTempCreature )
	{
        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
        if ( pTempData )
        {
		    m_pHPProgressBar->SetMax( pTempData->m_ulMaxHP );
		    m_pHPProgressBar->SetPosition( pTempData->m_lCurrHP );
        }
	}

	if ( ( g_DeviceInput.KeyHold( VK_SHIFT ) && g_DeviceInput.KeyDown( '1' ) ) )
		AttackButtonEvent() ;
	else if ( ( g_DeviceInput.KeyHold( VK_SHIFT ) && g_DeviceInput.KeyDown( '2' ) ) )
		ReturnButtonEvent() ;
	else if ( ( g_DeviceInput.KeyHold( VK_SHIFT ) && g_DeviceInput.KeyDown( '3' ) ) )
		DefenseButtonEvent() ;
	else if ( ( g_DeviceInput.KeyHold( VK_SHIFT ) && g_DeviceInput.KeyDown( '4' ) ) )
		ExitButtonEvent() ;

	return S_OK ;
}

VOID	CRYLSummonDlg::AttackButtonEvent() 
{
	if ( g_CharacterData.m_bSummon )
	{
		if ( g_CharacterData.m_bSummonCmd )
			g_CharacterData.m_bSummonCmd = FALSE ;
		else
			g_CharacterData.m_bSummonCmd = TRUE ;
	}
}

VOID	CRYLSummonDlg::ReturnButtonEvent() 
{
	if ( g_CharacterData.m_bSummon )
	{
		if ( g_CharacterData.m_bSummonCmd ) 
			g_CharacterData.m_bSummonCmd = FALSE ;

		CRYLNetworkData::Instance()->SendSummonCmd( ClientConstants::SummonCmd_Return ) ;
	}
}

VOID	CRYLSummonDlg::DefenseButtonEvent() 
{
	if ( g_CharacterData.m_bSummon )
	{
		if ( g_CharacterData.m_bSummonCmd ) 
			g_CharacterData.m_bSummonCmd = FALSE ;

		if ( CRYLGameData::Instance()->m_csStatus.m_bSummonGuard )
		{
			CRYLNetworkData::Instance()->SendSummonCmd( ClientConstants::SummonCmd_GuardOff ) ;
		} 
		else
		{
			CRYLNetworkData::Instance()->SendSummonCmd( ClientConstants::SummonCmd_GuardOn ) ;
		}
	}
}

VOID	CRYLSummonDlg::ExitButtonEvent() 
{
	if ( g_CharacterData.m_bSummon )
	{
		if ( g_CharacterData.m_bSummonCmd ) 
			g_CharacterData.m_bSummonCmd = FALSE ;

		CRYLNetworkData::Instance()->SendSummonCmd( ClientConstants::SummonCmd_Die ) ;
	}
}

VOID	CRYLSummonDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pSummonDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLSUMMONDLG::ATTACK_BUTTON :
		g_pSummonDlg->AttackButtonEvent() ;
		break ;
	case RYLSUMMONDLG::RETURN_BUTTON :	
		g_pSummonDlg->ReturnButtonEvent() ;
		break ;
	case RYLSUMMONDLG::DEFENSE_BUTTON :	
		g_pSummonDlg->DefenseButtonEvent() ;
		break ;
	case RYLSUMMONDLG::EXIT_BUTTON :	
		g_pSummonDlg->ExitButtonEvent() ;
		break ;		
	}
}
