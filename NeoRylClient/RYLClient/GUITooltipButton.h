// GUITooltipButton.h: interface for the CGUITooltipButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUITOOLTIPBUTTON_H__F5EDCEA4_DBCE_459E_BD5D_09BDF373993F__INCLUDED_)
#define AFX_GUITOOLTIPBUTTON_H__F5EDCEA4_DBCE_459E_BD5D_09BDF373993F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUITooltip.h"
#include <vector>

using namespace std;

class CGUITooltipButton : public CGUITooltip  
{
	protected:
		vector<char *> m_lstTooltip;

	public:
		CGUITooltipButton();
		~CGUITooltipButton();

		void InitTooltip(const char *strTooltip);
		BOOL GetIsTooltip(void);
		void DestroyTooltip(void);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
};

#endif // !defined(AFX_GUITOOLTIPBUTTON_H__F5EDCEA4_DBCE_459E_BD5D_09BDF373993F__INCLUDED_)
