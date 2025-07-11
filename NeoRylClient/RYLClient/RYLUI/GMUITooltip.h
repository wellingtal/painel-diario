//================================================================
//
//	Name : GMUITooltip.h 
//	Desc : Gama Soft Tooltip Base class
//  Date : 2004. 02. 11
//
//================================================================
#ifndef __GMUI_TOOLTIP__
#define __GMUI_TOOLTIP__

#include <windows.h>
#include <d3d8.h>

enum TOOLTIP_CTRL
{
	TOOLTIP_BUTTON = 0,
	TOOLTIP_DLG,
	TOOLTIP_TAB,
	TOOLTIP_PANEL
} ;

class CGMUITooltip  
{
protected :
	ULONG	m_dwMaxLength ;

public:
	BOOL		m_bShow ;
	RECT		m_rcEdge ;
	ULONG		m_dwAlign ;
	BOOL		m_dwCtrlType ;
	DWORD		m_dwKey ;
    
	CGMUITooltip() ;
	virtual ~CGMUITooltip() ;

	VOID	SetRect( USHORT x1, USHORT y1, USHORT x2, USHORT y2)
	{
		m_rcEdge.left = x1; m_rcEdge.top = y1; m_rcEdge.right = x2; m_rcEdge.bottom = y2;
	}

	VOID	SetAlign( ULONG dwAlign )
	{
		m_dwAlign = dwAlign;
	}

	virtual BOOL	GetIsTooltip() = 0 ;
	virtual VOID	DestroyTooltip() = 0 ;
	virtual VOID	Render( LPDIRECT3DDEVICE8 lpD3DDevice ) = 0 ;
};

#endif //__GMUI_TOOLTIP__
