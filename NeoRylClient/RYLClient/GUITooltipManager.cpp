// GUITooltipManager.cpp: implementation of the CGUITooltipManager class.
//
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "WinInput.h"
#include "GUITooltipManager.h"

#include "RYLSkillDescDlg.h"
#include "RYLAbilityDescDlg.h"
#include "GMMemory.h"

CGUITooltipManager g_TooltipManager;
CGUITooltip *CGUITooltipManager::m_ttRenderTarget = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUITooltipManager::CGUITooltipManager()
{
	m_ttRenderTarget = NULL;
}

CGUITooltipManager::~CGUITooltipManager()
{
	DestroyAllTooltip();
}

BOOL CGUITooltipManager::AddTooltip(CGUITooltip *pTooltip)
{
	m_lstTooltip.push_back(pTooltip);

	return TRUE;
}

void CGUITooltipManager::DestroyAllTooltip()
{
	vector<CGUITooltip *>::iterator it;
	CGUITooltip *lpTooltip;
	for(it = m_lstTooltip.begin(); it != m_lstTooltip.end();)
	{
		lpTooltip = (*it);
		it = m_lstTooltip.erase(it);
		delete lpTooltip;
		lpTooltip = NULL;
	}

	m_lstTooltip.clear();
}

void CGUITooltipManager::Update(long lMoveX, long lMoveY)
{
	vector<CGUITooltip *>::iterator it;
	CGUITooltip *pTooltip;

	for(it = m_lstTooltip.begin(); it != m_lstTooltip.end(); it++)
	{
		pTooltip = *it;

		if(pTooltip->m_bShow)
		{
			if(g_DeviceInput.InRect(pTooltip->m_rcEdge.left + lMoveX, pTooltip->m_rcEdge.top + lMoveY,
				pTooltip->m_rcEdge.right + lMoveX - 1, pTooltip->m_rcEdge.bottom + lMoveY - 1))
			{
				m_ttRenderTarget = pTooltip;
				break;
			}
		}
	}
}

void CGUITooltipManager::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	if(m_ttRenderTarget && m_ttRenderTarget->m_bShow)
	{
//		g_DeviceInput.ShowCursor(FALSE);
		m_ttRenderTarget->Render(lpD3DDevice);
	} else
	{
//		g_DeviceInput.ShowCursor(TRUE);
		if ( g_pSkillDescDlg )
			g_pSkillDescDlg->SetVisible( FALSE ) ;

		if ( g_pAbilityDescDlg )
			g_pAbilityDescDlg->SetVisible( FALSE ) ;
	}
}
