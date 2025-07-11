//================================================================
//
//	Name : RYLButton.h 
//	Desc : RYL Button
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLBUTTON_H__
#define __RYLBUTTON_H__

#include "GMUICaptionButton.h"


class CGMUICustomDlg ;
class CGMImageDib ;

class CRYLButton : public CGMUICaptionButton
{
public:
	CRYLButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLButton() ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	VOID LoadFromFile( LPSTR pFileName, BOOL boDefault = FALSE ) ;
	VOID AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
} ;

#endif //__RYLBUTTON_H__