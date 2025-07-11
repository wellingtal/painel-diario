//================================================================
//
//	Name : GMUICheckBox.h 
//	Desc : Gama Soft ComboBox
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __GMUICOMBOBOX_H__
#define __GMUICOMBOBOX_H__

#include "GMUICustomComboBox.h"

#define		GMUIComboButton_Image_Count		4

class CGMUICustomDlg ;
class CGMTexture ;
class CGMFontTexture ;

typedef struct tagSCOMBOBOXDATA
{
	CGMFontTexture* pFontTexture ;
	DWORD			TextColor ;
	DWORD			SelectColor ;
	DWORD			BackColor ;
	CGMString		Text ;
} SCOMBOBOXDATA ;


class CGMUIComboBox : public CGMUICustomComboBox
{
protected :
    enum EComboButtonImageStateType
    {
		ComboButtonImageStateType_Up		= 0,
		ComboButtonImageStateType_Down		= 1,
		ComboButtonImageStateType_Disible	= 2,
		ComboButtonImageStateType_Enter		= 3
    };

	CGMFontTexture*	m_pCaptionTexture;
	CGMTexture*		m_paButtonTexture[ GMUIComboButton_Image_Count ] ;

public:
	CGMUIComboBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUIComboBox() ;

	virtual VOID	AddItem( SCOMBOBOXDATA* pData ) ;
	virtual VOID	AddItem( LPSTR	pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) ;
	virtual VOID	GetItem( INT Index, SCOMBOBOXDATA **Data ) ;
	virtual VOID	DeleteItem( INT Index ) ;
	virtual VOID	SetItem( INT Index, SCOMBOBOXDATA *pData ) ;
	virtual VOID	SetItem( INT Index, LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) ;
	virtual VOID	ClearItem() ;
	virtual VOID	SelectMenuData( INT nIndex ) ;


	virtual VOID	Change( CGMUIObject* Sender ) ;
	virtual VOID	DoDraw(	CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	DrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;
	virtual VOID	DoBtnDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID	SetVisible( BOOL bVisible ) ;
	virtual VOID	SetSize( CGMUIObject* Sender ) ;

	virtual VOID	DrawItemBegin( CGMUIObject* Sender ) ;
	virtual VOID	DrawItemEnd( CGMUIObject* Sender ) ;

	virtual VOID	SetItemIndex( CGMUIObject* Sender ) ;

	virtual VOID	ShowMenu( CGMUIObject* Sender ) ;
	virtual VOID	HideMenu( CGMUIObject* Sender ) ;

	virtual VOID	SetFontColor( DWORD dwColor ) ;

	VOID AttachComboButtonTexture( EComboButtonImageStateType StateType, CGMTexture* pGMTexture ) ;	
	VOID DetachComboButtonTexture( EComboButtonImageStateType StateType ) ;
};


#endif // __GMUICOMBOBOX_H__