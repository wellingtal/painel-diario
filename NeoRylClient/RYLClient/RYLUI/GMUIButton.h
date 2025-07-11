//================================================================
//
//	Name : GMUIButton.h 
//	Desc : 
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __GMUIBUTTON_H__
#define __GMUIBUTTON_H__

#include "GMUICustomButton.h"

#define GMUIButton_Image_Count 4

class CGMUICustomDlg ;
class CGMTexture ;

class CGMUIButton : public CGMUICustomButton
{
public :
    enum EButtonImageStateType
    {
		ButtonImageStateType_Up			= 0,
		ButtonImageStateType_Down		= 1,
		ButtonImageStateType_Disible	= 2,
		ButtonImageStateType_Enter		= 3
    };
protected :
	BOOL			m_bAutoSize ;
	CGMTexture*		m_paButtonTexture[ GMUIButton_Image_Count ] ;

public :
	CGMUIButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUIButton() ;

	VOID	AttachButtonTexture( EButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	VOID	DetachButtonTexture( EButtonImageStateType StateType ) ;
	BOOL	GetIsButtonTexture( EButtonImageStateType StateType ) ;
	VOID	SetAutoSize( BOOL bAutoSize ) ;
	BOOL	GetAutoSize() ;

public:
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	SetVisible( BOOL boVisible ) ;
	virtual BOOL	DoAutoSize() ;
};

#endif