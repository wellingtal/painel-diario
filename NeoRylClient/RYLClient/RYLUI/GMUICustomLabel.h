//================================================================
//
//	Name : GMUICustomLabel.h 
//	Desc : Gama Soft Custom Label Class
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __GMUICUSTOMLABEL_H__
#define __GMUICUSTOMLABEL_H__

#include "GMUIObject.h"

class CGMUICustomLabel : public CGMUIObject
{
private :
	CGMString m_Caption ;

public :
	OnNotifyEvent	OnChangeEvent ;
	OnButtonDraw	OnDrawEvent ;

public :
	CGMUICustomLabel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomLabel() ;

	VOID		SetCaption( LPSTR pText ) ;
	LPSTR		GetCaption() ;

	virtual VOID	Change( CGMUIObject* Sender ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	GMUIPaint( INT xx, INT yy ) ;
} ;

#endif
