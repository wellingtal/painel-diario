// CommonOption.h: interface for the CCommonOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONOPTION_H__AE9ED6FC_651F_41A2_933A_AB88CF554CCE__INCLUDED_)
#define AFX_COMMONOPTION_H__AE9ED6FC_651F_41A2_933A_AB88CF554CCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUIWindow.h"

class CCommonOption : public CGUIWindow  
{
	public:
		unsigned long m_dwShowOption;
		unsigned long m_dwStartMusic;
		unsigned long m_dwStartSound;

		void ShowOptionWindow(unsigned long dwKind);
		void SelectOption(void);

		CCommonOption();
		~CCommonOption();

		BOOL Init(unsigned short x, unsigned short y);
		void InitValue(void);
		BOOL Update(BOOL &bClick, BOOL &bEdge);
		void ShowWindow(BOOL bShow);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
};

#endif // !defined(AFX_COMMONOPTION_H__AE9ED6FC_651F_41A2_933A_AB88CF554CCE__INCLUDED_)
