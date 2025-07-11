// GUIWindow.cpp: implementation of the CGUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "WinInput.h"
#include "GUIWindow.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIWindow::CGUIWindow()
{
	m_bShow = TRUE;
	m_bWindowMoving = FALSE;
	m_dwWindowKind = WINDOW_FLOAT;
	m_cAlpha = 0xFF;
}

CGUIWindow::~CGUIWindow()
{
	m_TooltipManager.DestroyAllTooltip();
//	m_ButtonManager.DestroyAllButton();
/*
	{
		vector<CGUIScroll *>::iterator it;
		for(it = m_lstScroll.begin(); it != m_lstScroll.end(); it++)
		{
			delete (*it);
		}
		m_lstScroll.clear();
	}
*/
	{
		vector<CRYLSprite *>::iterator it;
		for(it = m_lstSprite.begin(); it != m_lstSprite.end(); it++)
		{
			delete (*it);
		}
		m_lstSprite.clear();
	}
}

BOOL CGUIWindow::WindowMoving(POINT &ptMousePos)
{
	if(g_DeviceInput.InRect(m_rcMoveEdge) && g_DeviceInput.GetIsLeftMouseDown())
	{
		m_ptMove.x = ptMousePos.x - m_rcMoveEdge.left;
		m_ptMove.y = ptMousePos.y - m_rcMoveEdge.top;

		m_bWindowMoving = TRUE;

		m_cAlpha = 100;
	}

	if((g_DeviceInput.InRect(m_rcMoveEdge) && g_DeviceInput.GetIsLeftMouseUp()) || !g_DeviceInput.GetIsFocus())
	{
		m_bWindowMoving = FALSE;

		m_cAlpha = 0xFF;
	}

	if(m_bWindowMoving)
	{
		m_ptWindowMovePos.x = ptMousePos.x - m_ptMove.x;
		m_ptWindowMovePos.y = ptMousePos.y - m_ptMove.y;

		m_rcWindowSize.right = m_ptWindowMovePos.x + (m_rcWindowSize.right - m_rcWindowSize.left);
		m_rcWindowSize.bottom = m_ptWindowMovePos.y + (m_rcWindowSize.bottom - m_rcWindowSize.top);
		m_rcWindowSize.left = m_ptWindowMovePos.x;
		m_rcWindowSize.top = m_ptWindowMovePos.y;

		m_rcMoveEdge.right = m_ptWindowMovePos.x + (m_rcMoveEdge.right - m_rcMoveEdge.left);
		m_rcMoveEdge.bottom = m_ptWindowMovePos.y + (m_rcMoveEdge.bottom - m_rcMoveEdge.top);
		m_rcMoveEdge.left = m_ptWindowMovePos.x;
		m_rcMoveEdge.top = m_ptWindowMovePos.y;

		return TRUE;
	}

	return FALSE;
}
