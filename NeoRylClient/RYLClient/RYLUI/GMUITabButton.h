//================================================================
//
//	Name : GMUITabButton.h 
//	Desc : Gama Soft TabButton
//  Date : 2003. 12. 09
//
//================================================================
#ifndef __GMUITABBUTTON_H__
#define __GMUITABBUTTON_H__

#include "GMUICustomTabButton.h"

class CGMTexture ;

class CGMUITabButton : public CGMUICustomTabButton
{
public:
    enum ETabButtonImageStateType
    {
		TabButtonImageStateType_Up		= 0,
		TabButtonImageStateType_Down	= 1,
		TabButtonImageStateType_Disible	= 2,
		TabButtonImageStateType_ENTER	= 3,
    } ;

public :
	CGMUITabButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUITabButton() ;
	
	VOID AttachTabButtonTexture( ETabButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	VOID DetachTabButtonTexture( ETabButtonImageStateType StateType ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID SetVisible( BOOL bVisible ) ;
} ;

#endif //__GMUITABBUTTON_H__