//================================================================
//
//	Name : GMUITooltip.h 
//	Desc : Gama Soft Tooltip Base class
//  Date : 2004. 02. 11
//
//================================================================
#ifndef __GMUI_TOOLTIP_H__
#define __GMUI_TOOLTIP_H__

#include "GMUITooltip.h"
#include <vector>

class CGMUITooltipManager
{
public:
	std::vector<CGMUITooltip *>		m_lstTooltip ;
	static CGMUITooltip*			m_ttRenderTarget ;
	
public :
	CGMUITooltipManager() ;
	virtual ~CGMUITooltipManager() ;

	VOID	Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	VOID	Update( LONG lMoveX = 0, LONG lMoveY = 0 ) ;
	VOID	DestroyAllTooltip() ;
	BOOL	AddTooltip( CGMUITooltip *pTooltip ) ;
	CGMUITooltip*		FindTooltip( DWORD dwKey ) ;
};

extern CGMUITooltipManager GUITooltipMgr ;

#endif //__GMUI_TOOLTIP_H__
