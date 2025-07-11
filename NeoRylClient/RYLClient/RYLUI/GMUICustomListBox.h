//================================================================
//
//	Name : GMUICustomListBox.h 
//	Desc : Gama Soft Custom ListBox Class
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __GMUICUSTOMLISTBOX_H__
#define __GMUICUSTOMLISTBOX_H__

#include "GMUICustomPanel.h"
#include "GMUICustomButton.h"
#include "GMList.h" 

class CGMUICustomDlg ;

class CGMUICustomListBox ;

class CListBoxTopButton : public CGMUICustomButton
{
private :
	CGMUICustomListBox *m_pGMUICustomListBox ;

public :
	CListBoxTopButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CListBoxTopButton() ;

	virtual VOID SetParent( LPVOID pGMUIObject ) ;
	virtual VOID GetParent( VOID **ppGMUIObject ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;

	virtual VOID MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
};

//---------------------------------------------------------------------------------------
class CListBoxTrackButton : public CGMUICustomButton
{
private:
	BOOL				m_boTracking ;  
	CGMUICustomListBox *m_pGMUICustomListBox ;
	
public: 
	CListBoxTrackButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CListBoxTrackButton() ;

	virtual VOID SetParent( LPVOID pGMUIObject ) ;
	virtual VOID GetParent( VOID **ppGMUIObject ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;

	virtual VOID MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
};

//---------------------------------------------------------------------------------------
class CListBoxBottomButton : public CGMUICustomButton
{
private:
	CGMUICustomListBox *m_pGMUICustomListBox;
public:
	CListBoxBottomButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CListBoxBottomButton() ;

	virtual VOID SetParent( LPVOID pGMUIObject ) ;
	virtual VOID GetParent( VOID **ppGMUIObject ) ;
	virtual VOID SetSize( CGMUIObject* Sender ) ;

	virtual VOID MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
};

//---------------------------------------------------------------------------------------
class CGMUICustomListBox : public CGMUICustomPanel
{
private:
	INT			m_iMouseX ;
	INT			m_iMouseY ;

	INT			m_iOrY ;
	INT			m_iItemHeight ;

	INT			m_iViewIndex ;
	INT			m_iOverIndex ;

    INT			m_iScrollTrackTopPos ;
    INT			m_iScrollTrackingPos ;

	INT			m_iItemIndex ;

protected:
	INT			m_iItemMerginX ;
	INT			m_iItemMerginY ;
	INT			m_iViewItemCount ;

	BOOL		m_bScrollEnable ;

private:
	CGMList*	m_pDataList;

	CListBoxTopButton*		m_pListBoxTopButton ;
	CListBoxTrackButton*	m_pListBoxTrackButton ;
	CListBoxBottomButton*	m_pListBoxBottomButton ;


	
public:
	CGMUICustomListBox( CGMUICustomDlg *pGMUIDlg, BOOL bScrollEnable ) ;
	virtual ~CGMUICustomListBox() ;

	INT		GetItemIndex() ; 
	VOID	SelectItemIndex( INT Index );			//아이템 선택 부분
	INT		SetItemIndex( INT Index ) ;

	virtual INT		Count() ;
	virtual INT		GetItemWidth() ;
	virtual VOID	SetItemHeight( INT Value ) ;
	virtual INT		GetItemHeight() ;

	VOID	SetItemMerginX( INT nX ) ;
    INT		GetItemMerginX() ;
	VOID	SetItemMerginY( INT nY ) ;
    INT		GetItemMerginY() ;
	INT		GetListSize()	
	{ 
		return m_pDataList->Count() ; 
	}

	VOID	TopButtonDown() ;
	VOID	TrackButtonDown() ; 
	VOID	BottomButtonDown() ;
	VOID	TrackButtonMove( INT yy ) ;

private :
	VOID	InitGMUICtrl( BOOL bScrollEnable ) ;
	VOID	FinalGMUICtrl() ;

public :
	BOOL	GetScrollBarInPos( INT X, INT Y ) ;
	INT		GetScrollTrackingPos() ;
	INT		GetViewIndex() ;
	INT		GetViewCount()				{ return m_iViewItemCount ; }
	VOID	SetViewIndex( INT iIndex ) ;
	VOID	DrawItems( INT xx, INT yy ) ;
	
	VOID	DecItem( INT Value ) ;
	VOID	IncItem( INT Value ) ;
	VOID	SetTrackPos( INT Y ) ;

protected:
	VOID	SetGMUICtrlSize() ;
	virtual VOID	ClearData();
	virtual VOID	AddData( LPVOID Value ) ;
	virtual VOID	GetData( INT Index, VOID **ppValue ) ;
	virtual VOID	DeleteData( INT Index ) ;
	virtual VOID	SetData( INT Index, VOID *pValue ) ;

	VOID	SetScrollBarEnable( BOOL bEnable )			{ m_bScrollEnable = bEnable ; }
	BOOL	GetScrollBarEnable()						{ return m_bScrollEnable ; } 

public:
	OnDrawItem		OnDrawItemEvent ;
	OnButtonDraw	OnDrawEvent ;
	OnButtonDraw	OnTopDrawEvent ;
	OnButtonDraw	OnTrackDrawEvent ;
	OnButtonDraw	OnBottomDrawEvent ;

	CListBoxTopButton*		GetTopButton() ;
	CListBoxTrackButton*	GetTrackButton() ;
	CListBoxBottomButton*	GetBottomButton() ;

	virtual VOID	SetTopButtonSize( INT Width, INT Height ) ;
	virtual VOID	SetTrackButtonSize( INT Width, INT Height ) ;
	virtual VOID	SetBottomButtonSize( INT Width, INT Height ) ;

	virtual VOID	MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y ) ;
	virtual VOID	MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID	SetSize( CGMUIObject* Sender ) ;

	virtual VOID	DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Top
	virtual VOID	DoTopDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Track
	virtual VOID	DoTrackDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Bottom
	virtual VOID	DoBottomDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID	DrawItemBegin( CGMUIObject* Sender ) ;
	virtual VOID	DrawItemEnd( CGMUIObject* Sender ) ;

	virtual VOID	GMUIPaint( INT xx, INT yy ) ;

	virtual VOID	SetCalcViewItemCount() ;
};

#endif //__GMUICUSTOMLISTBOX_H__