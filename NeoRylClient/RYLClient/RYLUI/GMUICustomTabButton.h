//================================================================
//
//	Name : GMUICustomTabButton.h 
//	Desc : Gama Soft Custom TabButton Class
//  Date : 2003. 12. 09
//
//================================================================
#ifndef __GMUICUSTOMTABBUTTON_H__
#define __GMUICUSTOMTABBUTTON_H__

#include "GMUICustomPanel.h"
#include "RYLCheckBox.h"
#include "RYLLabel.h"
#include "GMUICustomLabel.h"

#include <vector>

class CGMUICustomDlg ;
class CGMUICustomTabButton ;
class CGMImageDib ;


class CTabButton : public CRYLCheckBox
{
private :
	CGMUICustomTabButton *m_pGMUICustomTabButton ;

public :
	CTabButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CTabButton() ;

	virtual VOID SetParent( LPVOID pGMUIObject ) ;
	virtual VOID GetParent( VOID **ppGMUIObject ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;

	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	VOID	AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
};

class CTabLabel : public CRYLLabel
{
private :
	CGMUICustomTabButton *m_pGMUICustomTabButton ;

public :
	CTabLabel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CTabLabel() ;

	virtual VOID SetParent( LPVOID pGMUIObject ) ;
	virtual VOID GetParent( VOID **ppGMUIObject ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
};

typedef struct __TABBUTTONINFO
{
	CTabButton*		m_pTabButton ;
	CTabLabel*		m_pTabLabel ;
	__TABBUTTONINFO()
	{
		m_pTabButton = NULL ;
		m_pTabLabel  = NULL ;
	}
}TABBUTTONINFO, *LPTABBUTTONINFO ;

class CGMUICustomTabButton : public CGMUICustomPanel
{
protected :
	std::vector< LPTABBUTTONINFO >		m_vecTabButton ;
	OnButtonDraw	OnDrawTabButton ;

	INT				m_iSXMargin ;
	INT				m_iFocusTabButton ;

public :
	CGMUICustomTabButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomTabButton() ; 
	
	VOID		FinalGMUICtrl() ;

	VOID		AttachTabButton( LPCTSTR lpStr, INT iKind ) ;
	VOID		AttachTabButton( INT ISpriteIndex, LPRECT lprtEnable, LPRECT lprtDisable ) ;
	VOID		DetachTabButton( INT iIndex ) ;

	VOID		SetTabVisible(INT iIndex, BOOL boVisible );

	VOID		RestoreTabButton( INT iIndex, LPCTSTR lpStr, INT iKind );

	INT			GetFocusTabButton()	;
	VOID		SetFocusTabButton( INT iIndex ) ;

	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	GMUIPaint( INT xx, INT yy ) ;

	virtual VOID	TabButtonUp() ;

	LPTABBUTTONINFO		GetTabButtonInfo( INT iIndex )		{ return m_vecTabButton[ iIndex ] ; }
} ;

#endif //__GMUICUSTOMTABBUTTON_H__