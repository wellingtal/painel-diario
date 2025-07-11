//================================================================
//
//	Name : GMUILabel.h 
//	Desc : Gama Soft Label
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __GMUILABEL_H__
#define __GMUILABEL_H__

#include "GMUICustomLabel.h"
 
class CGMUICustomDlg ;
class CGMFontTexture ;

enum RYL_ALIGN { LEFT = 0, RIGHT, CENTER, NONE };

class CGMUILabel : public CGMUICustomLabel
{
private :
	INT			m_iOffsetX, m_iOffsetY ;
	BOOL		m_bAutoSize ;
	RECT		m_rtAline ;
	RYL_ALIGN	m_eAlign ;

	CGMFontTexture* m_pFontTexture;
	
public:
	CGMUILabel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CGMUILabel() ;

	virtual VOID	Change( CGMUIObject* Sender ) ;
	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	SetFontColor( DWORD dwColor ) ;
	virtual VOID	SetBorderFontColor( DWORD dwColor ) ;
	virtual VOID	SetVisible( BOOL bVisible ) ;

	VOID	SetAutoSize( BOOL bAutoSize ) ;
	BOOL	GetAutoSize() ;

	RECT	SetAlign( RYL_ALIGN eAlign ) ;
	void	SetAlignRect( RECT rt ) ;
	CGMFontTexture*	GetFontTexture()		{ return m_pFontTexture ; }

private :
	virtual BOOL	DoAutoSize() ;
} ;

#endif //__GMUILABEL_H__