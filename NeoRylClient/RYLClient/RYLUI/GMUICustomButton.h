//================================================================
//
//	Name : GMUICustomButton.h 
//	Desc : Gama Soft Custom Button Class
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __GMUICUSTOMBUTTON_H__
#define __GMUICUSTOMBUTTON_H__

#include "GMUIObject.h"

class CGMUICustomForm;

class CGMUICustomButton : public CGMUIObject
{
public :
	OnButtonDraw	OnDrawEvent ;

	CGMUICustomButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomButton() ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID GMUIPaint( INT xx, INT yy ) ;
};

#endif // __GMUICUSTOMBUTTON_H__