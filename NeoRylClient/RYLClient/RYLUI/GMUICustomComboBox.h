//================================================================
//
//	Name : GMUICustomCheckBox.h 
//	Desc : Gama Soft Custom ComboBox
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICUSTOMCOMBOBOX_H__
#define __GMUICUSTOMCOMBOBOX_H__

#include "GMUICustomPanel.h"
#include "GMUICustomButton.h"
#include "GMUICustomMenu.h"
 
class CGMUICustomDlg ;
class CGMUICustomComboBox ;

class CComboMenu : public CGMUICustomMenu
{
private :
	CGMUICustomComboBox *m_pGMUICustomComboBox ;

public:
	CComboMenu( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CComboMenu() ;

	virtual VOID	SetParent( LPVOID pGMUIObject ) ;
	virtual VOID	GetParent( VOID **ppGMUIObject ) ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;

	virtual VOID	Show( CGMUIObject* Sender ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ; 

	virtual VOID	DrawItemBegin( CGMUIObject* Sender ) ;
	virtual VOID	DrawItemEnd( CGMUIObject* Sender ) ;

	virtual VOID	DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;
	virtual VOID	SetVisible( BOOL bVisible ) ;
	virtual VOID	SetItemIndex( CGMUIObject* Sender ) ;
};

class CGMUIComboButton : public CGMUICustomButton
{
private :
	CGMUICustomComboBox* m_pGMUICustomComboBox ;

public:
	CGMUIComboButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUIComboButton() ;

	virtual VOID	SetParent( LPVOID pGMUIObject ) ;
	virtual VOID	GetParent( VOID **ppGMUIObject ) ;

	virtual VOID	Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
};

class CGMUICustomComboBox : public CGMUICustomPanel
{
protected:
	CGMString			m_strText ;
	CGMUIComboButton*	m_pComboButton ;
	CComboMenu*			m_pComboMenu ;
	

public:
	OnNotifyEvent		OnChanageEvent ;
	OnDrawItem			OnDrawItemEvent ;

public:
	CGMUICustomComboBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICustomComboBox() ;

	VOID	SetMenuPos() ;
	VOID	SetButtonPos() ;
	VOID	ViewMenu() ;
	
	VOID	SetText( LPSTR pText ) ;
	LPSTR	GetText() ;

	VOID	SetItemHeight( INT Value ) ;
	INT		GetItemHeight() ;

	VOID	SetItemMerginX( INT Value ) ;
	INT		GetItemMerginX() ;

	VOID	SetItemMerginY( INT Value ) ;
	INT		GetItemMerginY() ;

	CComboMenu*			GetMenu() ;
	CGMUIComboButton*	GetButton() ;

	virtual VOID SelectMenuData( INT nIndex ) ;

	virtual VOID ItemClear() ;
	virtual VOID ItemAdd( LPVOID Value ) ;
	virtual VOID ItemGet( INT Index, VOID **ppValue ) ;
	virtual VOID ItemDelete( INT Index ) ;
	virtual VOID ItemSet( INT Index, LPVOID pValue ) ;
	virtual INT ItemCount() ;

	virtual VOID SetLeft( INT X ) ;
	virtual VOID SetTop( INT Y ) ;
	virtual VOID SetWidth( INT Width ) ;
	virtual VOID SetHeight( INT Height ) ;
	virtual VOID SetVisible( BOOL bVisible ) ;
	virtual VOID SetEnable( BOOL bEnable ) ;

	virtual VOID Click( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;
	virtual VOID ShowMenu( CGMUIObject* Sender ) ;
	virtual VOID HideMenu( CGMUIObject* Sender ) ;
	virtual VOID Change( CGMUIObject* Sender ) ;

	// Combo Draw
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Menu Draw
	virtual VOID DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;
	// Button Draw
	virtual VOID DoBtnDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID DrawItemBegin( CGMUIObject* Sender ) ;
	virtual VOID DrawItemEnd( CGMUIObject* Sender ) ;
	
	virtual VOID SetItemIndex( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

private :
	VOID InitGMUICtrl() ;
	VOID FinalGMUICtrl() ;
};


#endif