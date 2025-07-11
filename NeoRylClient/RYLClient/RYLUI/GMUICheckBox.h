//================================================================
//
//	Name : GMUICheckBox.h 
//	Desc : Gama Soft Check Box
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICHECKBOX_H__
#define __GMUICHECKBOX_H__

#include "GMUICustomCheckBox.h"

#define GMUICHECKBOX_IMAGE_COUNT	6
 
class CGMUICustomDlg ;
class CGMTexture ;

class CGMUICheckBox : public CGMUICustomCheckBox
{
public:
    enum ECheckBoxImageStateType
    {
		CheckBoxImageStateType_Up		= 0,
		CheckBoxImageStateType_Down		= 1,
		CheckBoxImageStateType_Disible	= 2,
		CheckBoxImageStateType_ENTER	= 3,

		CheckBoxImageStateType_CHECKED_DISABLE	= 4,
		CheckBoxImageStateType_NCHECKED_DISABLE	= 5,
    } ;
	
public :
	CGMTexture *m_paCheckBoxTexture[ GMUICHECKBOX_IMAGE_COUNT ] ;

public:
	CGMUICheckBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICheckBox() ;

	VOID AttachCheckBoxTexture( ECheckBoxImageStateType StateType, CGMTexture* pGMTexture ) ;
	VOID DetachCheckBoxTexture( ECheckBoxImageStateType StateType ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID SetVisible( BOOL bVisible ) ;
};


#endif