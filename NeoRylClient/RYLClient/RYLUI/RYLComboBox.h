//================================================================
//
//	Name : RYLComboBox.h 
//	Desc : RYL ComboBox
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __RYLCOMBOBOX_H__
#define __RYLCOMBOBOX_H__

#include "GMUIComboBox.h"

class CGMCustomDlg ;
class CRYLImage ;
class CGMImageDib ;
//------------------------------------------------------------------------------
//					CNAComboBox : public CAUXUIComboBox 
//------------------------------------------------------------------------------
class CRYLComboBox : public CGMUIComboBox 
{
private :
	CRYLImage* m_pImage ;

public :
	CRYLComboBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLComboBox() ;

	VOID DefaultDraw() ;

	virtual VOID	DoBtnDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	VOID	AttachButtonImageEx( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
};

#endif