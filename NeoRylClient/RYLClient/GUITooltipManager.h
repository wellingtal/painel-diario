// GUITooltipManager.h: interface for the CGUITooltipManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUITOOLTIPMANAGER_H__3806D783_4DEC_4B10_8E79_64E6133EBC3B__INCLUDED_)
#define AFX_GUITOOLTIPMANAGER_H__3806D783_4DEC_4B10_8E79_64E6133EBC3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUITooltip.h"
#include <vector>

using namespace std;

class CGUITooltipManager  
{
	public:
		vector<CGUITooltip *> m_lstTooltip;
		static CGUITooltip *m_ttRenderTarget;

		void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
		void Update(long lMoveX = 0, long lMoveY = 0);
		void DestroyAllTooltip(void);
		BOOL AddTooltip(CGUITooltip *pTooltip);
		CGUITooltipManager();
		~CGUITooltipManager();
};

extern CGUITooltipManager g_TooltipManager;

#endif // !defined(AFX_GUITOOLTIPMANAGER_H__3806D783_4DEC_4B10_8E79_64E6133EBC3B__INCLUDED_)
