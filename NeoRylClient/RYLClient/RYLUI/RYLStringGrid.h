//================================================================
//
//	Name : RYLStringGrid.h 
//	Desc : RYL StringGrid
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __RYLSTRINGGRID_H__
#define __RYLSTRINGGRID_H__

#include "GMUICustomListBox.h"

#define GMUIListBox_Button_Image_Count	4

class CGMUICustomDlg ;
class CGMTexture ;
class CGMFontTexture ;
class CRYLGridLabel ;
class CGMImageDib ;

//------------------------------------------------------------------------------
//					CNAStringGrid : public CAUXUICustomListBox
//------------------------------------------------------------------------------
class CRYLStringGrid : public CGMUICustomListBox
{
public:
    enum EListBoxButtonImageStateType
    {
		ButtonImageStateType_Up			= 0,
		ButtonImageStateType_Down		= 1,
		ButtonImageStateType_Disible	= 2,
		ButtonImageStateType_Enter		= 3
    } ;

protected :
	INT				m_iColCount;
	CGMTexture*		m_paTopButtonTexture[ GMUIListBox_Button_Image_Count ] ;
	CGMTexture*		m_paBottomButtonTexture[ GMUIListBox_Button_Image_Count ] ;
	CGMTexture*		m_paTrackButtonTexture[ GMUIListBox_Button_Image_Count ] ;

	unsigned long	m_dwSelectedItemColor;

public :
	CRYLStringGrid( CGMUICustomDlg *pGMUIDlg, BOOL bScrollEnable ) ;
	virtual ~CRYLStringGrid() ;

	virtual VOID	DoDrawItem( CGMUIObject* Sender, INT Index, RECT r, INT State ) ;
	// Top
	virtual VOID	DoTopDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Track
	virtual VOID	DoTrackDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Bottom
	virtual VOID	DoBottomDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	virtual VOID	AttachTopImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachTopImageButtonTexture( EListBoxButtonImageStateType StateType ) ;

	virtual VOID	AttachBottomImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachBottomImageButtonTexture( EListBoxButtonImageStateType StateType ) ;

	virtual VOID	AttachTrackImageButtonTexture( EListBoxButtonImageStateType StateType, CGMTexture* pGMTexture);
	virtual VOID	DetachTrackImageButtonTexture( EListBoxButtonImageStateType StateType ) ;


	VOID	SetColCount( INT ColCount ) ;
	VOID	AddItem( INT Col, LPSTR pText, INT Width, DWORD dwColor, int iPosX = -1 ) ;
	LPSTR	GetItem( INT Index, INT Col ) ;
	VOID	DeleteItem( INT Index ) ;
	VOID	SetItem( INT Index, INT Col, LPSTR pText ) ;
	VOID	ClearItem() ;

	VOID	LoadTopImage( LPSTR pFileName ) ;
	VOID	LoadBottomImage( LPSTR pFileName ) ;
	VOID	LoadTrackImage( LPSTR pFileName ) ;

	void	SetSelectedItemColor(unsigned long dwColor)	{ m_dwSelectedItemColor = dwColor;	}

	virtual VOID	AttachTopButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
	virtual VOID	AttachTrackButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
	virtual VOID	AttachBottomButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
};

#endif
  