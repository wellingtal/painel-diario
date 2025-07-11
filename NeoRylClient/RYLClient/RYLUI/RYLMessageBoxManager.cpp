// GUIMessageBoxManager.cpp: implementation of the CRYLMessageBoxManager class.
//
//////////////////////////////////////////////////////////////////////
#include "RYLGameData.h"

#include "RYLMessageBoxManager.h"
#include "BaseDataDefine.h"
#include "FrameTimer.h"
#include "GMUIScreen.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "GMMemory.h"

CRYLMessageBoxManager g_MessageBoxManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRYLMessageBoxManager::CRYLMessageBoxManager()
{
	m_lpTexture =  NULL;
}

CRYLMessageBoxManager::~CRYLMessageBoxManager()
{
	if(m_lpTexture) 
	{ 
		delete m_lpTexture; 
		m_lpTexture = NULL; 
	}
}

void CRYLMessageBoxManager::AddMessageBox(CRYLMessageBox *lpMessageBox)
{
	m_lstMessageBox.push_back(lpMessageBox);
	
	// Simple Mouse Mode 로 변경
	if ( lpMessageBox && (lpMessageBox->m_dwFlag & GUIMESSAGEBOX::YES) && (lpMessageBox->m_dwFlag & GUIMESSAGEBOX::CONFIRM) )
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;
		if ( pGame && !pGame->m_bMouseMode ) pGame->m_bSimpleMouse = TRUE ;
	}
}

void CRYLMessageBoxManager::Create()
{
	CTexture::SetPath(INTERFACETEXTUREPATH);
	m_lpTexture = new CTexture;
	m_lpTexture->Load("GUIMsgBox.dds");

	m_dwMessageBoxTick = CFrameTimer::Regist(1000.0f);
}

void CRYLMessageBoxManager::DeleteMessageBox(CRYLMessageBox *lpMessageBox)
{
	vector<CRYLMessageBox *>::iterator it;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		if((*it) == lpMessageBox)
		{
			m_lstMessageBox.erase(it);
			CGMUIScreen::GetInstance()->DeleteDlg( ( *it ) ) ;
			break;
		}
	}
}

BOOL CRYLMessageBoxManager::Update()
{
	float fTick = CFrameTimer::GetUpdateTimer(m_dwMessageBoxTick);

	vector<CRYLMessageBox *>::iterator it;
	BOOL bClick = FALSE;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		if(!(*it)->UpdateEx(fTick, bClick, !bClick))
		{
			CRYLMessageBox *lpDelete = (*it);
			it = m_lstMessageBox.erase(it);
			lpDelete->SetVisible( FALSE ) ;

			CGMUIScreen::GetInstance()->DeleteDlg( lpDelete ) ;

			delete lpDelete;
		} else
		{
			it++;
		}
	}

	return bClick;
/*	if(m_lstMessageBox.empty())
		return FALSE;
	else
		return TRUE;*/
}

void CRYLMessageBoxManager::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	vector<CRYLMessageBox *>::iterator it;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end(); it++)
	{
		(*it)->Render(lpD3DDevice);
	}
}

void CRYLMessageBoxManager::DestroyList(bool bChk)
{
	vector<CRYLMessageBox *>::iterator it;
	CRYLMessageBox *lpDelete;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		lpDelete = (*it);

		if(lpDelete->m_bTimerDisable && bChk)
		{
			it++;
			continue;
		}

		it = m_lstMessageBox.erase(it);
		CGMUIScreen::GetInstance()->DeleteDlg( lpDelete ) ;
		delete lpDelete;
	}

	if(!bChk)
	{
		m_lstMessageBox.clear();
	}

	// 무비 모드 해제
	if ( CRYLGameData::Instance()->m_bMovieVersion )
	{
		CRYLGameData* pGame = CRYLGameData::Instance();
		pGame->m_bMovieVersion = false;
		CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		pScene->MovieMode( TRUE ) ;


		if ( pGame->m_bQuestComplete )
		{
			pGame->RenderQuestCompleteEffect() ;
		}
	}

	// 스타터킷 사용 해제
	if ( CRYLGameData::Instance()->m_lpCampStartKit )
	{
		CRYLGameData::Instance()->m_lpCampStartKit = NULL ;
	}

	if ( CRYLGameData::Instance()->m_lpWorldWeaponKit )
	{
		CRYLGameData::Instance()->m_lpWorldWeaponKit = NULL ;
	}

	if ( CRYLGameData::Instance()->m_lpStartKit )
	{
		CRYLGameData::Instance()->m_lpStartKit = NULL ;
	}
}
