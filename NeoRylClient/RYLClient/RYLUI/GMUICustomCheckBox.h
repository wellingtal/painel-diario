//================================================================
//
//	Name : GMUICustomCheckBox.h 
//	Desc : Gama Soft Custom Check Box
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICUSTOMCHECKBOX_H__
#define __GMUICUSTOMCHECKBOX_H__
 
#include "GMUIObject.h"

//------------------------------------------------------------------------------
//					CAUXUICustomCheckBox : public CAUXUIObject					
//------------------------------------------------------------------------------
class CGMUICustomCheckBox : public CGMUIObject
{
protected:
	BOOL m_bChecked ;

public:
	OnNotifyEvent	OnChangeEvent ;
	OnButtonDraw	OnDrawEvent ;

	CGMUICustomCheckBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomCheckBox() ;

	BOOL GetChecked() ;
	VOID SetChecked( BOOL bChecked ) ;

	virtual VOID Change( CGMUIObject* Sender ) ;
	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID GMUIPaint( INT xx, INT yy ) ;
} ;

#endif // __GMUICUSTOMCHECKBOX_H__