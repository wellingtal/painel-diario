#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"

#include "RYLGameData.h"
#include "RYLHumanInventoryPanel.h"
#include "RYLAkhanInventoryPanel.h"
#include "RYLSkillPanel.h"
#include "RYLAbilityPanel.h"
#include "RYLSocialActionPanel.h"

#include "WinInput.h"
#include "RYLStringTable.h"
#include "BaseGraphicsLayer.h"
#include <RenderOption.h>

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include <Creature/Character/CharacterClass.h>
#include "GMMemory.h"

CRYLHumanInventoryFrameDlg*	g_pHumanInventoryFrameDlg = NULL ;

CRYLHumanInventoryFrameDlg::CRYLHumanInventoryFrameDlg() 
{
	m_lpD3DDevice	= NULL ;
	m_pTabButton	= NULL ;
	m_pHumanInvenPanel = NULL ;
	m_pAkhanInvenPanel = NULL ;
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 375, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 375, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 375, 461, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 375, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ) ;
	
//	pImageDIB->DrawFrameRect( 110, 0, 143, 21, 0xc0000000 ) ; 
//	pImageDIB->DrawFrameRect( 110, 20, 143, 1, 0xffa88a60 ) ;
	pImageDIB->DrawFrameRect( 110, 0, 263, 21, 0xc0000000 ) ; 
	pImageDIB->DrawFrameRect( 110, 20, 263, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 375 ) ;	
	SetHeight( 440 ) ;

	SetTop( 135 ) ;
	SetLeft( g_ClientMain.m_iScreenWidth-GetWidth() );

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;
	
	g_pHumanInventoryFrameDlg = this ;
	InitCtrl() ;
}

CRYLHumanInventoryFrameDlg::~CRYLHumanInventoryFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLHumanInventoryFrameDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLHUMANINVENTORYFRAMEDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2082 ], 4 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2083 ], 2 ) ;

	// edith 2009.09.06 잡 탭 추가하기
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2080 ], 5 ) ;
	// 소셜모션

	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 3481 ], 3 ) ;
	AttachGMUIControl( m_pTabButton ) ;
	
	switch ( CRYLGameData::Instance()->m_dwClientRace )
	{
		case CClass::RaceType::HUMAN:
		{
			m_pHumanInvenPanel = new CRYLHumanInventoryPanel( this ) ;
			m_pHumanInvenPanel->SetLeft( 0 ) ;
			m_pHumanInvenPanel->SetTop( 36 ) ;
			m_pHumanInvenPanel->SetWidth( 375 ) ;
			m_pHumanInvenPanel->SetHeight( 440 ) ;
			( ( CRYLHumanInventoryPanel* )m_pHumanInvenPanel )->InitCtrl() ;
			m_pHumanInvenPanel->OnClickEvent = DlgClick ;
			m_pHumanInvenPanel->SetInstance( RYLHUMANINVENTORYFRAMEDLG::HUMANINVEN_PANEL ) ;
			m_pHumanInvenPanel->SetVisible( TRUE ) ;
			AttachGMUIControl( m_pHumanInvenPanel ) ;
			break ;
		}

		case CClass::RaceType::AKHAN:
		{
			m_pAkhanInvenPanel = new CRYLAkhanInventoryPanel( this ) ;
			m_pAkhanInvenPanel->SetLeft( 0 ) ;
			m_pAkhanInvenPanel->SetTop( 36 ) ;
			m_pAkhanInvenPanel->SetWidth( 375 ) ;// was 320
			m_pAkhanInvenPanel->SetHeight( 440 ) ;
			( ( CRYLAkhanInventoryPanel* )m_pAkhanInvenPanel )->InitCtrl() ;
			m_pAkhanInvenPanel->OnClickEvent = DlgClick ;
			m_pAkhanInvenPanel->SetInstance( RYLHUMANINVENTORYFRAMEDLG::HUMANINVEN_PANEL ) ;
			m_pAkhanInvenPanel->SetVisible( TRUE ) ;
			AttachGMUIControl( m_pAkhanInvenPanel ) ;
			break ;
		}
	}
	
	m_pSkillPanel = new CRYLSkillPanel( this ) ;
	m_pSkillPanel->SetLeft( 0 ) ;
	m_pSkillPanel->SetTop( 36 ) ;
	m_pSkillPanel->SetWidth( 375 ) ;
	m_pSkillPanel->SetHeight( 440 ) ;
	( ( CRYLSkillPanel* )m_pSkillPanel )->InitCtrl() ;
	m_pSkillPanel->OnClickEvent = DlgClick ;
	m_pSkillPanel->SetInstance( RYLHUMANINVENTORYFRAMEDLG::SKILL_PANEL ) ;
	m_pSkillPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSkillPanel ) ;

	// edith 2009.09.06 잡 탭 추가하기
	m_pAbilityPanel = new CRYLAbilityPanel( this ) ;
	m_pAbilityPanel->SetLeft( 0 ) ;
	m_pAbilityPanel->SetTop( 36 ) ;
	m_pAbilityPanel->SetWidth( 375 ) ;
	m_pAbilityPanel->SetHeight( 440 ) ;
	( ( CRYLAbilityPanel* )m_pAbilityPanel )->InitCtrl() ;
	m_pAbilityPanel->OnClickEvent = DlgClick ;
	m_pAbilityPanel->SetInstance( RYLHUMANINVENTORYFRAMEDLG::ABILITY_PANEL ) ;
	m_pAbilityPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pAbilityPanel ) ;

	m_pSocialActionPanel = new CRYLSocialActionPanel( this ) ;
	m_pSocialActionPanel->SetLeft( 0 ) ;
	m_pSocialActionPanel->SetTop( 36 ) ;
	m_pSocialActionPanel->SetWidth( 375 ) ;
	m_pSocialActionPanel->SetHeight( 440 ) ;
	( ( CRYLSocialActionPanel* )m_pSocialActionPanel )->InitCtrl() ;
	m_pSocialActionPanel->OnClickEvent = DlgClick ;
	m_pSocialActionPanel->SetInstance( RYLHUMANINVENTORYFRAMEDLG::SOCIALACTION_PANEL ) ;
	m_pSocialActionPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSocialActionPanel ) ;
}

HRESULT		CRYLHumanInventoryFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if ( m_pHumanInvenPanel && m_pHumanInvenPanel->GetVisible() )
	{
		( ( CRYLHumanInventoryPanel * )m_pHumanInvenPanel )->UpdateInvenItemPos() ;
	}

	if ( m_pAkhanInvenPanel && m_pAkhanInvenPanel->GetVisible() )
	{
		( ( CRYLAkhanInventoryPanel * )m_pAkhanInvenPanel )->UpdateInvenItemPos() ;
	}

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_bRYLHumanInventoryFrameDlgClick = FALSE ;

	if ( pGame->m_bRYLStreetStallDlgClick ||
		 pGame->m_bRYLExchangeDlgClick ||
		 pGame->m_lpInterface->m_bPopupShow)
	{
		return S_OK ;
	}

	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
			pGame->m_bRYLHumanInventoryFrameDlgClick = TRUE ;
		}
	}

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return S_FALSE;

    if( pSelfCreature->GetIsDead() )
	{
		// 아이템창에 인벤토리에 있는 위치가 이상할 경우가 있어서 //
		if(GetHumanInvenPanel())
		{
			CRYLHumanInventoryPanel* pHPanel = ( CRYLHumanInventoryPanel* )GetHumanInvenPanel();
			pHPanel->UpdateInvenItemPos();
		}
		else if(GetAkhanInvenPanel())
		{
			CRYLAkhanInventoryPanel* pAPanel = ( CRYLAkhanInventoryPanel* )GetAkhanInvenPanel();
			pAPanel->UpdateInvenItemPos();
		}
		return S_OK ;
	}

	if ( m_pHumanInvenPanel && m_pHumanInvenPanel->GetVisible() )
		( ( CRYLHumanInventoryPanel * )m_pHumanInvenPanel )->Update( bClick, bEdge ) ;

	if ( m_pAkhanInvenPanel && m_pAkhanInvenPanel->GetVisible() )
		( ( CRYLAkhanInventoryPanel * )m_pAkhanInvenPanel )->Update( bClick, bEdge ) ;

	if ( m_pSkillPanel->GetVisible() )
		( ( CRYLSkillPanel * )m_pSkillPanel )->Update( bClick, bEdge ) ;

	if ( m_pAbilityPanel->GetVisible() )
		( ( CRYLAbilityPanel * )m_pAbilityPanel )->Update( bClick, bEdge ) ;

	if ( m_pSocialActionPanel->GetVisible() )
		( ( CRYLSocialActionPanel* )m_pSocialActionPanel )->Update(bClick, bEdge) ;

	return S_OK ;
}
 
HRESULT		CRYLHumanInventoryFrameDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{

	return S_OK ;
}

HRESULT		CRYLHumanInventoryFrameDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLHumanInventoryFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pHumanInvenPanel ) ;
	GM_DELETE( m_pAkhanInvenPanel ) ;
	GM_DELETE( m_pSkillPanel ) ; 
	GM_DELETE( m_pAbilityPanel ) ; 
	GM_DELETE( m_pSocialActionPanel ) ;
}

VOID	CRYLHumanInventoryFrameDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;		

	pGame->m_bSimpleMouse = TRUE ;		
}

VOID	CRYLHumanInventoryFrameDlg::Hide( CGMUIObject* Sender ) 
{
	// Simple Mouse Mode 로 변경
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLGameScene*	pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;

	pGame->m_bRYLHumanInventoryFrameDlgClick = FALSE ;

	if ( pScene && pGame )
	{
		if ( pScene->GetExchangeDlg()->GetVisible() )
		{
			SetVisible( TRUE ) ;
			return ;
		}

		if ( pScene->GetBlacksmithDlg()->GetVisible() )		pScene->GetBlacksmithDlg()->SetVisible(FALSE);	
		if ( pScene->GetNewTradeDlg()->GetVisible() )		pScene->GetNewTradeDlg()->SetVisible(FALSE);	
		if ( pScene->GetVaultDlg()->GetVisible() )			pScene->GetVaultDlg()->SetVisible(FALSE);		

		if ( !pGame->m_bMouseMode )
		{
			if ( !pScene->GetStatusAndQuestFrameDlg()->GetVisible() && !pScene->GetSocietyFrameDlg()->GetVisible() &&
				 !pScene->GetGuildFrameDlg()->GetVisible() && !pScene->GetLargeMapDlg()->GetVisible() &&
				 !pScene->GetHumanInventoryFrameDlg()->GetVisible() && !pScene->GetSystemDlg()->GetVisible() &&
				 !pScene->GetBlacksmithDlg()->GetVisible() && !pScene->GetExchangeDlg()->GetVisible() &&
				 !pScene->GetJobChangeDlg()->GetVisible() && !pScene->GetOptionDlg()->GetVisible() &&
				 !pScene->GetQuestDetailDataDlg()->GetVisible() && !pScene->GetQuestOpeningDlg()->GetVisible() &&
				 !pScene->GetRankingDlg()->GetVisible() && !pScene->GetStatusRetrainDlg()->GetVisible() &&
				 !pScene->GetStreetStallDlg()->GetVisible() && !pScene->GetNewTradeDlg()->GetVisible() &&
				 !pScene->GetVaultCertificationdDlg()->GetVisible() && !pScene->GetVaultDlg()->GetVisible() &&
				 !pScene->GetVaultPasswordDlg()->GetVisible() )
			{
				pGame->m_bSimpleMouse = FALSE ;
			}
			else 
			{
				pGame->m_bSimpleMouse = TRUE ;
			}
		}
	}
}

VOID	CRYLHumanInventoryFrameDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	if ( m_pHumanInvenPanel && m_pHumanInvenPanel->GetVisible() )
		( ( CRYLHumanInventoryPanel * )m_pHumanInvenPanel )->FinalRender( lpD3DDevice ) ;

	if ( m_pAkhanInvenPanel && m_pAkhanInvenPanel->GetVisible() )
		( ( CRYLAkhanInventoryPanel * )m_pAkhanInvenPanel )->FinalRender( lpD3DDevice ) ;

	if ( m_pSkillPanel->GetVisible() )
		( ( CRYLSkillPanel * )m_pSkillPanel )->FinalRender( lpD3DDevice ) ;

	if ( m_pAbilityPanel->GetVisible() )
		( ( CRYLAbilityPanel * )m_pAbilityPanel )->FinalRender( lpD3DDevice ) ;

	if ( m_pSocialActionPanel->GetVisible() )
		( ( CRYLSocialActionPanel* )m_pSocialActionPanel )->FinalRender( lpD3DDevice ) ;
}

VOID	CRYLHumanInventoryFrameDlg::ShowInventory()
{
	if ( m_pHumanInvenPanel )
		m_pHumanInvenPanel->SetVisible( TRUE ) ;

	if ( m_pAkhanInvenPanel )
		m_pAkhanInvenPanel->SetVisible( TRUE ) ;

	m_pSkillPanel->SetVisible( FALSE ) ;
	m_pAbilityPanel->SetVisible( FALSE ) ;
	m_pSocialActionPanel->SetVisible( FALSE ) ;

	GetTabButton()->SetFocusTabButton( 0 ) ;
}

VOID	CRYLHumanInventoryFrameDlg::ShowSkill()
{
	if ( m_pHumanInvenPanel )
		m_pHumanInvenPanel->SetVisible( FALSE ) ;

	if ( m_pAkhanInvenPanel )
		m_pAkhanInvenPanel->SetVisible( FALSE ) ;

	m_pSkillPanel->SetVisible( TRUE ) ;
	m_pAbilityPanel->SetVisible( FALSE ) ;
	m_pSocialActionPanel->SetVisible( FALSE ) ;

	GetTabButton()->SetFocusTabButton( 1 ) ;
}

VOID	CRYLHumanInventoryFrameDlg::ShowAbility()
{
	if ( m_pHumanInvenPanel )
		m_pHumanInvenPanel->SetVisible( FALSE ) ;

	if ( m_pAkhanInvenPanel )
		m_pAkhanInvenPanel->SetVisible( FALSE ) ;

	m_pSkillPanel->SetVisible( FALSE ) ;
	m_pAbilityPanel->SetVisible( TRUE ) ;
	m_pSocialActionPanel->SetVisible( FALSE ) ;

	GetTabButton()->SetFocusTabButton( 2 ) ;
}

VOID	CRYLHumanInventoryFrameDlg::ShowSocialAction()
{
	if ( m_pHumanInvenPanel )
		m_pHumanInvenPanel->SetVisible( FALSE ) ;

	if ( m_pAkhanInvenPanel )
		m_pAkhanInvenPanel->SetVisible( FALSE ) ;

	m_pSkillPanel->SetVisible( FALSE ) ;
	m_pAbilityPanel->SetVisible( FALSE ) ;
	m_pSocialActionPanel->SetVisible( TRUE ) ;

	GetTabButton()->SetFocusTabButton( 3 ) ;
}

VOID	CRYLHumanInventoryFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pHumanInventoryFrameDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLHUMANINVENTORYFRAMEDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pHumanInventoryFrameDlg->GetTabButton()->GetFocusTabButton() ;
			switch( iFocus )
			{
				case 0 : 
					g_pHumanInventoryFrameDlg->ShowInventory() ;
					break ;
	
				case 1 : 
					g_pHumanInventoryFrameDlg->ShowSkill() ;
					break ;

				case 2 : 
					g_pHumanInventoryFrameDlg->ShowAbility() ;
					break ;

				case 3 : 
					g_pHumanInventoryFrameDlg->ShowSocialAction() ;
					break ;
			}
			break ;
		}
	}
}