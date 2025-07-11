// GUIWindow.h: interface for the CGUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIWINDOW_H__FBEDCE5C_5D7D_438F_B55B_112096006F1A__INCLUDED_)
#define AFX_GUIWINDOW_H__FBEDCE5C_5D7D_438F_B55B_112096006F1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include "RYLSprite.h"
#include "GUITooltipManager.h"
//#include "GUIButtonManager.h"
//#include "GUIScroll.h"

using namespace std;

#define WINDOW_FIX			0
#define WINDOW_FLOAT		1

class CGUIWindow  
{
	protected:
		vector<CRYLSprite *> m_lstSprite;
//		vector<CGUIScroll *> m_lstScroll;
		CGUITooltipManager m_TooltipManager;
		//CGUIButtonManager m_ButtonManager;

	public:
		unsigned long m_dwWindowKind;
		RECT m_rcMoveEdge;
		RECT m_rcWindowSize;
		unsigned char m_cAlpha;
		BOOL m_bShow;
		POINT m_ptMove;
		POINT m_ptWindowMovePos;
		BOOL m_bWindowMoving;

		CGUIWindow();
		virtual ~CGUIWindow();

		void SetAlpha(unsigned char cAlpha) { m_cAlpha = cAlpha; }
		void SetWindowPos(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
		{
			m_rcWindowSize.left = x1; m_rcWindowSize.top = y1; m_rcWindowSize.right = x2; m_rcWindowSize.bottom = y2;
		}
		void SetMoveEdge(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
		{
			m_rcMoveEdge.left = x1; m_rcMoveEdge.top = y1; m_rcMoveEdge.right = x2; m_rcMoveEdge.bottom = y2;
		}
		BOOL WindowMoving(POINT &ptMousePos);

		/*
		CGUIButton *GetButton(unsigned long dwIndex)
		{
			return m_ButtonManager.m_lstButton[dwIndex];
		}
		*/

		virtual BOOL Init(unsigned short x, unsigned short y) = 0;
		virtual void InitValue(void) = 0;
		virtual BOOL Update(BOOL &bClick, BOOL &bEdge) = 0;
		virtual void ShowWindow(BOOL bShow) = 0;
		virtual void Render(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
};

#endif // !defined(AFX_GUIWINDOW_H__FBEDCE5C_5D7D_438F_B55B_112096006F1A__INCLUDED_)
