
#include <winsock2.h>

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"

#include "SceneManager.h"
#include "FullSceneEffect.h"

#include "RYLSceneManager.h"
#include "RYLLoadingScene.h"
#include "RYLGameScene.h"
#include "RYLCharacterSelectScene.h"

#include "RYLMessageBoxManager.h"

#include "StreamingSound.h"
#include "SoundMgr.h"

#include "RYLOptionDlg.h"
#include "RYLCharacterCreationDlg.h"
#include "RYLCharacterInfoDlg.h"
#include "RYLCharacterDeleteDlg.h"
#include "RYLChannelDlg.h"
// WORK_LIST 2.3 계정 국적 변경 UI 추가
#include "RYLNationDlg.h"
#include "RYLNationSelectDlg.h"
#include "RYLLabel.h"
#include "GMFontTexture.h"

#include "RYLStringTable.h"

#include <Creature/Character/CharacterClass.h>
#include "GMMemory.h"

CRYLCharacterSelectScene::CRYLCharacterSelectScene( INT iID ) : CRYLSceneObject( iID )  
{
	m_pCharacterCreation = NULL ;	
	// 캐릭터 정보창
	m_pCharacterInfoDlg		= new CRYLCharacterInfoDlg ;
	m_pCharacterDeleteDlg	= new CRYLCharacterDeleteDlg ;

	m_pChannelDlg			= new CRYLChannelDlg ;

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	m_pNationDlg			= new CRYLNationDlg ;
	m_pNationSelectDlg		= new CRYLNationSelectDlg ;

	m_pDescription = new CRYLLabel() ;
	m_pDescription->SetLeft( 30 ) ;
	m_pDescription->SetTop( g_ClientMain.m_iScreenHeight - 30 ) ;
	m_pDescription->SetFontColor( 0xFFFFFFFF ) ;
	m_pDescription->SetAutoSize( TRUE ) ;
	m_pDescription->GetFontTexture()->SetFontDrawType( CGMFontTexture::FontDrawType_Border ) ;
	m_pDescription->SetCaption( CRYLStringTable::m_strString[ 3574 ] ) ;
}

CRYLCharacterSelectScene::~CRYLCharacterSelectScene() 
{
	GM_DELETE( m_pCharacterCreation ) ;
	GM_DELETE( m_pCharacterInfoDlg ) ;
	GM_DELETE( m_pCharacterDeleteDlg ) ;

	GM_DELETE( m_pChannelDlg ) ;

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	GM_DELETE( m_pNationDlg ) ;
	GM_DELETE( m_pNationSelectDlg ) ;

	GM_DELETE( m_pDescription ) ;

	DeleteResourceObject() ;
}

void			CRYLCharacterSelectScene::DeleteCharacterCreation()	
{ 
    GM_DELETE( m_pCharacterCreation ) ; 
}

HRESULT			CRYLCharacterSelectScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLGameData::Instance()->m_lpInterface->RenderChrSelect( lpD3DDevice ) ;

	if ( m_pDescription && m_pDescription->GetVisible() )
	{
		RECT rt ;
		int iWidth = CRYLStringTable::GetStringWidth( CRYLStringTable::m_strString[ 3574 ] ) ;
		::SetRect( &rt, m_pDescription->GetLeft(), m_pDescription->GetTop(), m_pDescription->GetLeft() + iWidth, m_pDescription->GetTop() + 16 ) ;
		m_pDescription->DoDraw( NULL, rt, 0 ) ;
	}
	
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLLoadingScene::Instance()->Render( lpD3DDevice ) ;
	
	if ( GUITooltipMgr.m_ttRenderTarget )
			GUITooltipMgr.m_ttRenderTarget->Render( lpD3DDevice ) ;

	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::Update() 
{
	if ( CRYLGameData::Instance()->m_lpInterface )
		CRYLGameData::Instance()->m_lpInterface->UpdateChrSelect();

    //-- 2004. 2. 24. Zergra From.--//
    // for message box
    if (g_MessageBoxManager.m_lstMessageBox.size())
	{
		g_MessageBoxManager.Update();
	}
    //-- Zergra To. --//
	
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::InitResourceObject() 
{
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::DeleteResourceObject() 
{
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::FinalCleanup() 
{
	return S_OK ;
}

HRESULT			CRYLCharacterSelectScene::MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) 
{
	return S_OK ;
}

VOID			CRYLCharacterSelectScene::SceneShow() 
{
	char szBuff[ MAX_PATH ] ;
	CStreamingSound* pPrevSound; 
    sprintf( szBuff, "%s\\sound\\Bgm\\CharSel_815.ogg", g_ClientMain.m_strClientPath );
    CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, szBuff ); 
	pPrevSound->SetLooping( true ) ;
	pPrevSound->Play();

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	
	if ( CRYLOptionDlg::m_coOption.m_dwBGMSound )
	{
		pPrevSound->Stop() ;
	}
	/*
	CRYLGameScene*	pScene = static_cast<CRYLGameScene *>( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
	CRYLOptionDlg*  pDlg = static_cast<CRYLOptionDlg *>( pScene->GetOptionDlg() ) ;
	pDlg->ReLoadOption() ;
	*/

	m_pCharacterInfoDlg->SetVisible( TRUE ) ;
	m_pChannelDlg->SetVisible( FALSE ) ;
	m_pCharacterDeleteDlg->SetVisible( FALSE );

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	m_pNationDlg->SetVisible( TRUE ) ;
	m_pNationSelectDlg->SetVisible( FALSE ) ;

	m_pDescription->SetVisible( TRUE ) ;

	if ( CRYLGameData::Instance()->m_cReLogin)
	{
		CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR, false );
		CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR, false ) ;
		CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_NEGATIVE, false );
		CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_NEGATIVE, false ) ;
		CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_SATURATION2, false );
		CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_SATURATION2, false ) ;

		if ( static_cast<unsigned short>(CRYLGameData::Instance()->m_csStatus.m_Info.HP) == 0)
		{
			CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR, true ) ;
			CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_SATURATION2, true );
		}
	}
}

VOID			CRYLCharacterSelectScene::SceneHide() 
{
	char szBuff[ MAX_PATH ] ;
	CStreamingSound* pPrevSound; 
    sprintf( szBuff, "%s\\sound\\Bgm\\CharSel_815.ogg", g_ClientMain.m_strClientPath );
    CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, szBuff ); 
	pPrevSound->Stop();

	if ( m_pCharacterCreation )
		m_pCharacterCreation->SetVisible( FALSE ) ;

	if ( m_pCharacterInfoDlg )
		m_pCharacterInfoDlg->SetVisible( FALSE ) ;

	if ( m_pChannelDlg )
		m_pChannelDlg->SetVisible( FALSE ) ;

	if ( m_pCharacterDeleteDlg )
		m_pCharacterDeleteDlg->SetVisible( FALSE );

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	if ( m_pNationDlg )
		m_pNationDlg->SetVisible( FALSE ) ;

	if ( m_pNationSelectDlg )
		m_pNationSelectDlg->SetVisible( FALSE );

	if ( m_pDescription )
		m_pDescription->SetVisible( FALSE ) ;
}

//	WORK_LIST 1.2 캐릭터 선택창에서 종족 라벨 빼내기
//VOID			CRYLCharacterSelectScene::CreateCharacterSelectDlg() 
VOID			CRYLCharacterSelectScene::CreateCharacterSelectDlg( unsigned long dwRace ) 
{
	if ( !m_pCharacterCreation )
	{
		//	WORK_LIST 1.2 캐릭터 선택창에서 종족 라벨 빼내기
		//m_pCharacterCreation = new CRYLCharacterCreationDlg ; 
		m_pCharacterCreation = new CRYLCharacterCreationDlg( dwRace ) ; 
	}
}

// WORK_LIST 2.3 계정 국적 변경 UI 추가
VOID			CRYLCharacterSelectScene::SetCurrentNation( unsigned long dwNation )
{
	if ( m_pNationDlg )
	{
		( ( CRYLNationDlg* )( m_pNationDlg ) )->SetCurrentNation( dwNation ) ;
	}

	if ( m_pNationSelectDlg )
	{
		m_pNationSelectDlg->SetVisible( FALSE ) ;
	}
}

VOID			CRYLCharacterSelectScene::ShowNationDlg()
{
	if ( m_pNationDlg )
	{
		m_pNationDlg->SetVisible( TRUE ) ;
	}

	if ( m_pDescription )
	{
		m_pDescription->SetVisible( TRUE ) ;
	}
}

VOID			CRYLCharacterSelectScene::HideNationDlg()
{
	if ( m_pNationDlg )
	{
		m_pNationDlg->SetVisible( FALSE ) ;
	}

	if ( m_pDescription )
	{
		m_pDescription->SetVisible( FALSE ) ;
	}
}