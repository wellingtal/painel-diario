//================================================================
//
//	Name : GMUICaptionButton.h 
//	Desc : 
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __GMUICAPTIONBUTTON_H__
#define __GMUICAPTIONBUTTON_H__

#include "GMUIButton.h"

class CGMUICustomDlg;
class CGMFontTexture;

class CGMUICaptionButton : public CGMUIButton
{
protected :
	INT					m_iOffsetX, m_iOffsetY ;
	BOOL				m_bCaptionFlag ;
	CGMString			m_Caption ;
	CGMFontTexture*		m_pFontTexture ;
	
public :
	CGMUICaptionButton( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUICaptionButton() ;

	VOID		SetAutoSize( BOOL bAutoSize ) ;
	BOOL		GetAutoSize();

	VOID		SetCaption( LPSTR pCaption ) ;
	LPSTR		GetCaption() ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	SetVisible( BOOL bVisible ) ;
	virtual VOID	Change( CGMUIObject* Sender ) ;

	virtual VOID	SetFontColor( DWORD dwColor ) ;
	virtual VOID	SetBorderFontColor( DWORD dwColor ) ;

protected :
	virtual BOOL	DoAutoSize() ;
	virtual VOID	SetCaptionChange() ;
};

#endif //__GMUICAPTIONBUTTON_H__