// GUITooltip.h: interface for the CGUITooltip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUITOOLTIP_H__269F8F44_BF9E_401E_BFC8_8237A2BFE60B__INCLUDED_)
#define AFX_GUITOOLTIP_H__269F8F44_BF9E_401E_BFC8_8237A2BFE60B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <d3d8.h>

class CGUITooltip  
{
	protected:
		unsigned long m_dwMaxLength;

	public:
		BOOL m_bShow;
		RECT m_rcEdge;
		unsigned long m_dwAlign;

		CGUITooltip();
		virtual ~CGUITooltip();

		void SetRect(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
		{
			m_rcEdge.left = x1; m_rcEdge.top = y1; m_rcEdge.right = x2; m_rcEdge.bottom = y2;
		}
		void SetAlign(unsigned long dwAlign)
		{
			m_dwAlign = dwAlign;
		}

		virtual BOOL GetIsTooltip(void) = 0;
		virtual void DestroyTooltip(void) = 0;
		virtual void Render(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
};

#endif // !defined(AFX_GUITOOLTIP_H__269F8F44_BF9E_401E_BFC8_8237A2BFE60B__INCLUDED_)
