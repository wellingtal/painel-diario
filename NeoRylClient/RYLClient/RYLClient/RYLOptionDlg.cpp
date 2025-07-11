
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendEtc.h>

#include <RenderOption.h>
#include <WinInput.h>

#include "BaseGraphicsLayer.h"
#include "BaseDataDefine.h"
#include "GMUIObject.h"
#include "GMImageDib.h"

#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLListBox.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLClientMain.h"
#include "RYLNetworkData.h"

#include "RYLOptionDlg.h"
#include "RYLChattingDlg.h"

#include "RYLGeneralOptionPanel.h"
#include "RYLEffectOptionPanel.h"
#include "RYLSoundOptionPanel.h"
#include "GMMemory.h"


CRYLOptionDlg*	g_pOptionDlg = NULL ;

RejectOption	CRYLOptionDlg::m_roOption ;
ClientOption	CRYLOptionDlg::m_coOption ;
bool			CRYLOptionDlg::m_bIsCancel ;

CRYLOptionDlg::CRYLOptionDlg() 
{
	m_pTabButton				= NULL ;
	m_pGeneralOptionPanel		= NULL ;
	m_pEffectOptionPanel		= NULL ;
	m_pSoundOptionPanel			= NULL ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 461, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 110, 0, 143, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 110, 20, 143, 1, 0xffa88a60 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 412 ) ;		
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;
	
	g_pOptionDlg = this ;
	InitCtrl() ;

	m_bIsCancel = false ;

	m_roOption = CRYLGameData::Instance()->m_roOption ;
	m_coOption = CRYLGameData::Instance()->m_coOption ;
	
	ReLoadOption() ;
}

CRYLOptionDlg::~CRYLOptionDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLOptionDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLOPTIONDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2167 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2246 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2247 ], 3 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	m_pGeneralOptionPanel = new CRYLGeneralOptionPanel( this ) ;
	m_pGeneralOptionPanel->SetLeft( 0 ) ;
	m_pGeneralOptionPanel->SetTop( 36 ) ;
	m_pGeneralOptionPanel->SetWidth( 256 ) ;
	m_pGeneralOptionPanel->SetHeight( 440 ) ;
	( ( CRYLGeneralOptionPanel* )m_pGeneralOptionPanel )->InitCtrl() ;
	m_pGeneralOptionPanel->OnClickEvent = DlgClick ;
	m_pGeneralOptionPanel->SetInstance( RYLOPTIONDLG::GENERALOPTIN_PANEL ) ;
	m_pGeneralOptionPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pGeneralOptionPanel ) ;

	m_pEffectOptionPanel = new CRYLEffectOptionPanel( this ) ;
	m_pEffectOptionPanel->SetLeft( 0 ) ;
	m_pEffectOptionPanel->SetTop( 36 ) ;
	m_pEffectOptionPanel->SetWidth( 256 ) ;
	m_pEffectOptionPanel->SetHeight( 440 ) ;
	( ( CRYLEffectOptionPanel* )m_pEffectOptionPanel )->InitCtrl() ;
	m_pEffectOptionPanel->OnClickEvent = DlgClick ;
	m_pEffectOptionPanel->SetInstance( RYLOPTIONDLG::EFFECTOPTION_PANEL ) ;
	m_pEffectOptionPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pEffectOptionPanel ) ;

	m_pSoundOptionPanel = new CRYLSoundOptionPanel( this ) ;
	m_pSoundOptionPanel->SetLeft( 0 ) ;
	m_pSoundOptionPanel->SetTop( 36 ) ;
	m_pSoundOptionPanel->SetWidth( 256 ) ;
	m_pSoundOptionPanel->SetHeight( 440 ) ;
	
	((CRYLSoundOptionPanel*)m_pSoundOptionPanel)->SetBgmVolume(CRYLGameData::Instance()->m_coOption.m_fBgmVolume); 
	((CRYLSoundOptionPanel*)m_pSoundOptionPanel)->SetAmbVolume(CRYLGameData::Instance()->m_coOption.m_fAmbVolume); 	

	( ( CRYLSoundOptionPanel* )m_pSoundOptionPanel )->InitCtrl() ;
	m_pSoundOptionPanel->OnClickEvent = DlgClick ;
	m_pSoundOptionPanel->SetInstance( RYLOPTIONDLG::SOUNDOPTION_PANEL ) ;
	m_pSoundOptionPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSoundOptionPanel ) ;	
}

VOID	CRYLOptionDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pGeneralOptionPanel ) ;
	GM_DELETE( m_pEffectOptionPanel	) ;
	GM_DELETE( m_pSoundOptionPanel ) ;
}

VOID	CRYLOptionDlg::Show( CGMUIObject* Sender ) 
{
	m_roOption = CRYLGameData::Instance()->m_roOption ;
	m_coOption = CRYLGameData::Instance()->m_coOption ;
	CRYLOptionDlg::m_bIsCancel = false ;

	ReLoadOption() ;
}

VOID	CRYLOptionDlg::Hide( CGMUIObject* Sender ) 
{
}

void	CRYLOptionDlg::ReLoadOption(void) 
{
	( ( CRYLGeneralOptionPanel* )m_pGeneralOptionPanel )->ReLoadOption() ;
	( ( CRYLEffectOptionPanel* )m_pEffectOptionPanel )->ReLoadOption() ;
	( ( CRYLSoundOptionPanel* )m_pSoundOptionPanel )->ReLoadOption() ;
}

void	CRYLOptionDlg::OK(void)
{
	// 장비 그레이드별 효과 옵션이 변경되면 근처 모든 캐릭터의 장비를 다시 입혀준다.
	bool bChangeEquipEffectOption = false;
	if ( (m_coOption.m_dwEquipEffect & 1) != (CRYLGameData::Instance()->m_coOption.m_dwEquipEffect & 1) )
	{
		// SetShape() 내부에선 변경된 값을 사용하므로, 변경이 되었는지를 미리 체크한다.
		bChangeEquipEffectOption = true;
	}

	CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	pScene->GetOptionDlg()->SetVisible( FALSE );
	
	m_bIsCancel = false;

	char strFileName[MAX_PATH] = { 0, };
	sprintf(strFileName, "%s\\ClientOption.dat", g_ClientMain.m_strClientPath);
	CRYLGameData::Instance()->SaveClientOption( strFileName, &CRYLOptionDlg::m_coOption );
	SendPacket::CharControlOption( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, CRYLOptionDlg::m_roOption );

	// 장비 그레이드별 효과 옵션의 실제 적용부
	if (true == bChangeEquipEffectOption)
	{
		RYLCreature* lpCreature = RYLCreatureManager::Instance()->GetCreatureListHead();
		while (NULL != lpCreature)
		{
			lpCreature->SetShape(true);
			lpCreature = RYLCreatureManager::Instance()->GetCreatureListNext();
		}
	}
}

VOID	CRYLOptionDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLOptionDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLOptionDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLOptionDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	switch ( m_pTabButton->GetFocusTabButton() )
	{
		case 0 : ( ( CRYLGeneralOptionPanel* )m_pGeneralOptionPanel )->Update() ;	break ;		// 일반 옵션
		case 1 : ( ( CRYLEffectOptionPanel* )m_pEffectOptionPanel )->Update() ;		break ;		// 이펙트 옵션
		case 2 : ( ( CRYLSoundOptionPanel* )m_pSoundOptionPanel )->Update() ;		break ;		// 사운드 옵션
	}

	return S_OK ;
}

VOID	CRYLOptionDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pOptionDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLOPTIONDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pOptionDlg->GetTabButton()->GetFocusTabButton() ;
			switch( iFocus )
			{
				case 0 : 
				{
					g_pOptionDlg->GetGeneralOptionPanel()->SetVisible( TRUE ) ;
					g_pOptionDlg->GetEffectOptionPanel()->SetVisible( FALSE ) ;
					g_pOptionDlg->GetSoundOptionPanel()->SetVisible( FALSE ) ;
					break ;
				}

				case 1 : 
				{
					g_pOptionDlg->GetGeneralOptionPanel()->SetVisible( FALSE ) ;
					g_pOptionDlg->GetEffectOptionPanel()->SetVisible( TRUE ) ;
					g_pOptionDlg->GetSoundOptionPanel()->SetVisible( FALSE ) ;
					break ;
				}

				case 2 :
				{
					g_pOptionDlg->GetGeneralOptionPanel()->SetVisible( FALSE ) ;
					g_pOptionDlg->GetEffectOptionPanel()->SetVisible( FALSE ) ;
					g_pOptionDlg->GetSoundOptionPanel()->SetVisible( TRUE) ;
					break ;
				}
			}
		}
		break ;
	}
}
