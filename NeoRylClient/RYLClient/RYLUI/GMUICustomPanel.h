//================================================================
//
//	Name : GMUICUSTOMPANEL.h 
//	Desc : Gama Soft Custom Menu Class
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICUSTOMPANEL_H__
#define __GMUICUSTOMPANEL_H__

#include "GMUIObject.h"

#include <vector>
using namespace std ;
 
class CGMUICustomDlg ;
class CGMTexture ;

class CGMUICustomPanel : public CGMUIObject
{
private:
	vector< CGMUIObject* >	m_ChildVector ;
	CGMTexture*				m_pPanelImage ;

protected:
	VOID GetChildPosition( CGMUIObject *pGMUIObject, INT *X, INT *Y ) ;

public:
	CGMUICustomPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomPanel() ; 

	virtual VOID Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID DblClick (CGMUIObject* Sender, int Button, int Shift, int X, int Y) ;

	virtual VOID AddChild( CGMUIObject *pGMUICtrl ) ;
	virtual VOID DeleteChild( CGMUIObject *pGMUICtrl ) ;

	virtual VOID MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	VOID	AttachPanelImage( CGMTexture* pTexture ) ;
	VOID	AddChildEx( CGMUIObject *pGMUICtrl ) ;
} ;

#endif // __GMUICUSTOMPANEL_H__
