//================================================================
//
//	Name : RYLLabel.h 
//	Desc : RYL Label
//  Date : 2003. 09. 25
//
//================================================================
#ifndef __RYLLABEL_H__
#define __RYLLABEL_H__

#include "GMUILabel.h"

class CGMUICustomDlg ;

class CRYLLabel : public CGMUILabel
{
public :
	CRYLLabel( CGMUICustomDlg *pParentDlg ) ;
	CRYLLabel() ;
	virtual ~CRYLLabel() ;
} ;

#endif //__RYLLABEL_H__