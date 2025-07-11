//================================================================
//
//	Name : RYLlistBox.h 
//	Desc : RYL ListBox
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __RYLLISTBOX_H__
#define __RYLLISTBOX_H__

#include "GMUIListBox.h"
class CGMUICustomDlg ;
class CGMImageDib ;
class CGMTexture ;

class CRYLListBox : public CGMUIListBox
{
private :
	CGMTexture*				m_pListBoxImage ;
public :
	CRYLListBox( CGMUICustomDlg *pParentDlg, BOOL bScrollEnable ) ;
	virtual ~CRYLListBox() ;

	virtual VOID	DoTopDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	DoTrackDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	DoBottomDraw( CGMUIObject* Sender, RECT r, INT State ) ;

	VOID	LoadTopImage( LPSTR pFileName ) ;
	VOID	LoadBottomImage( LPSTR pFileName ) ;
	VOID	LoadTrackImage( LPSTR pFileName ) ;

	VOID	AttachTopButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
	VOID	AttachTrackButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
	VOID	AttachBottomButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	
	VOID	AttachListBoxImage( CGMTexture* pTexture ) { m_pListBoxImage = pTexture ; }
};

#endif