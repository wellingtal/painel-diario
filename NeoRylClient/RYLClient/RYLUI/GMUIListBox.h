//================================================================
//
//	Name : GMUIListBox.h 
//	Desc : Gama Soft ListBox
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __GMUILISTBOX_H__
#define __GMUILISTBOX_H__

#include "GMUICustomListBox.h"
#include "GMUICustomButton.h"
 

#define GMUIListBox_Button_Image_Count	4

class CGMUICustomDlg ;
class CGMTexture ;
class CGMFontTexture ;

typedef struct tagGMUILISTBOXITEM 
{
	CGMFontTexture *pFontTexture ;
	DWORD			TextColor ;
	DWORD			SelectColor ;
	DWORD			BackColor ;
	CGMString		Text ;
}GMUILISTBOXITEM, *PGMUILISTBOXITEM ;

//------------------------------------------------------------------------------
class CGMUIListBox : public CGMUICustomListBox
{
protected :
	CGMTexture* m_paTopButtonTexture[ GMUIListBox_Button_Image_Count ] ;
	CGMTexture* m_paBottomButtonTexture[ GMUIListBox_Button_Image_Count ] ; 
	CGMTexture* m_paTrackButtonTexture[ GMUIListBox_Button_Image_Count ] ;

public :
	CGMUIListBox( CGMUICustomDlg *pGMUIDlg, BOOL bScrollEnable ) ;
	virtual ~CGMUIListBox() ;
public :
    enum EListBoxButtonImageStateType
    {
		ButtonImageStateType_Normal		= 0,
		ButtonImageStateType_Down		= 1,
		ButtonImageStateType_Up			= 2,
		ButtonImageStateType_Disible	= 3
    } ;

public :
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;
	// Top
	virtual VOID	DoTopDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Track
	virtual VOID	DoTrackDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Bottom
	virtual VOID	DoBottomDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID	AddItem( GMUILISTBOXITEM *pData ) ;
	virtual VOID	AddItem( LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) ;
	virtual VOID	GetItem( INT Index, GMUILISTBOXITEM **Data ) ;
	virtual LPSTR	GetItem( INT Index ) ;

	virtual VOID	DeleteItem( INT Index ) ;
	virtual VOID	SetItem( INT Index, GMUILISTBOXITEM *pData ) ;
	virtual VOID	SetItem( INT Index, LPSTR pText, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) ;
	virtual VOID	SetItem( INT Index, LPSTR pText ) ;
	virtual VOID	SetItemColor( INT Index, DWORD dwTextColor, DWORD dwSelectColor, DWORD dwBackColor ) ;
	virtual VOID	ClearItem() ;

	virtual VOID	AttachTopImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachTopImageButtonTexture( EListBoxButtonImageStateType StateType ) ;
	virtual VOID	AttachBottomImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachBottomImageButtonTexture( EListBoxButtonImageStateType StateType ) ;
	virtual VOID	AttachTrackImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachTrackImageButtonTexture( EListBoxButtonImageStateType StateType ) ;
} ;

#endif // __GMUILISTBOX_H__