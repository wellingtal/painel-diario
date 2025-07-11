//================================================================
//
//	Name : GMUITooltipBox.h 
//	Desc : Gama Soft TooltipBox class
//  Date : 2004. 02. 11
//
//================================================================
#ifndef __GMUI_TOOLTIPBOX__
#define __GMUI_TOOLTIPBOX__

#include "GMUITooltip.h"
#include <vector>

using namespace std;

class CGMUITooltipBox : public CGMUITooltip  
{
protected :
	vector< CHAR * >	m_lstTooltip ;

public:
	CGMUITooltipBox() ;
	~CGMUITooltipBox() ;

	VOID		InitTooltip( LPCTSTR strTooltip, DWORD dwKey, DWORD dwCtrlType = FALSE ) ;
	BOOL		GetIsTooltip() ;
	VOID		DestroyTooltip() ;
	VOID		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
} ;

#endif //__GMUI_TOOLTIPBOX__
