//================================================================
//
//	Name : RYLTabButton.h 
//	Desc : RYL Tab Button
//  Date : 2003. 12. 8
//
//================================================================
#ifndef __RYL_TABBUTTON_H__
#define __RYL_TABBUTTON_H__

#include "GMUITabButton.h"

class CGMUICustomDlg ;

class CRYLTabButton : public CGMUITabButton
{
public:
	CRYLTabButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLTabButton() ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
} ;

#endif //__RYLTABBUTTON_H__