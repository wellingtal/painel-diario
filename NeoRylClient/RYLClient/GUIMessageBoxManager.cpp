// GUIMessageBoxManager.cpp: implementation of the CGUIMessageBoxManager class.
//
//////////////////////////////////////////////////////////////////////

#include "GUIMessageBoxManager.h"
#include "BaseDataDefine.h"
#include "FrameTimer.h"
#include "GMUIScreen.h"


CGUIMessageBoxManager g_MessageBoxManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIMessageBoxManager::CGUIMessageBoxManager()
{
	m_lpTexture =  NULL;
}

CGUIMessageBoxManager::~CGUIMessageBoxManager()
{
	if(m_lpTexture) { delete m_lpTexture; m_lpTexture = NULL; }
}

void CGUIMessageBoxManager::AddMessageBox(CGUIMessageBox *lpMessageBox)
{
	m_lstMessageBox.push_back(lpMessageBox);
}

void CGUIMessageBoxManager::Create()
{
	CTexture::SetPath(INTERFACETEXTUREPATH);
	m_lpTexture = new CTexture;
	m_lpTexture->Load("GUIMsgBox.dds");

	m_dwMessageBoxTick = CFrameTimer::Regist(1000.0f);
}

void CGUIMessageBoxManager::DeleteMessageBox(CGUIMessageBox *lpMessageBox)
{
	vector<CGUIMessageBox *>::iterator it;
	CGUIMessageBox *lpDelete;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		if((*it) == lpMessageBox)
		{
			 m_lstMessageBox.erase(it);
			CGMUIScreen::GetInstance()->DeleteDlg( lpDelete ) ;
			break;
		}
	}
}

BOOL CGUIMessageBoxManager::Update()
{
	float fTick = CFrameTimer::GetUpdateTimer(m_dwMessageBoxTick);

	vector<CGUIMessageBox *>::iterator it;
	BOOL bClick = FALSE;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		if(!(*it)->UpdateEx(fTick, bClick, !bClick))
		{
			CGUIMessageBox *lpDelete = (*it);
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

void CGUIMessageBoxManager::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	vector<CGUIMessageBox *>::iterator it;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end(); it++)
	{
		(*it)->Render(lpD3DDevice);
	}
}

void CGUIMessageBoxManager::DestroyList()
{
	vector<CGUIMessageBox *>::iterator it;
	CGUIMessageBox *lpDelete;
	for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end();)
	{
		lpDelete = (*it);
		it = m_lstMessageBox.erase(it);
		CGMUIScreen::GetInstance()->DeleteDlg( lpDelete ) ;
		delete lpDelete;
	}
	m_lstMessageBox.clear();
}
