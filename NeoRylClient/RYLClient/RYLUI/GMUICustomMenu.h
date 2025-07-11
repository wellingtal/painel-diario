//================================================================
//
//	Name : GMUICustomMenu.h 
//	Desc : Gama Soft Custom Menu Class
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICUSTOMMENU_H__
#define __GMUICUSTOMMENU_H__

#include "GMUIObject.h"

class CGMUICustomDlg ;
class CGMList ;
class CGMTexture ;
 
class CGMUICustomMenu : public CGMUIObject
{
private :
	INT			m_iItemIndex ;
	INT			m_iMenuPosStyle ;
	CGMList*	m_pDataList ;
	CGMTexture*				m_pMenuImage ;

protected:
	INT			m_iItemHeight ;
    INT			m_iItemMerginX ;
    INT			m_iItemMerginY ;
public:
	CGMUICustomMenu( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomMenu() ;

	OnDrawItem	OnDrawItemEvent ;

	VOID	SetVisible( BOOL Value ) ;
	VOID	SetItemHeight( INT Height ) ;
	INT		GetItemHeight() ;

	VOID	SetItemMerginX( INT nItemMerginX ) ;
	INT		GetItemMerginX() ;

	VOID	SetItemMerginY( INT nItemMerginY ) ;
	INT		GetItemMerginY() ;

	INT		GetItemIndex() ; 
	INT		SetItemIndex( INT Index ) ;

	INT		GetMenuPosStyle() ;
	VOID	SetMenuPosStyle( INT PosStyle ) ;

public:
	virtual VOID	ItemClear() ;
	virtual VOID	ItemAdd( LPVOID pValue ) ;
	virtual VOID	ItemGet( INT Index, VOID **ppValue ) ;
	virtual VOID	ItemDelete( INT Index ) ;
	virtual VOID	ItemSet( INT Index, LPVOID pValue ) ;
	virtual INT		ItemCount() ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID	Show( CGMUIObject* Sender ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	virtual VOID	DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;

	virtual VOID	DrawItemBegin( CGMUIObject* Sender ) ;
	virtual VOID	DrawItemEnd( CGMUIObject* Sender ) ;
	virtual VOID	SetItemIndex( CGMUIObject* Sender ) ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	GMUIPaint( INT xx, INT yy ) ;

	VOID	AttachMenuImage( CGMTexture* pTexture ) ;
	void	SelectItemIndex( int iIndex ) ;
};


#endif // __GMUICUSTOMMENU_H__