//================================================================
//
//	Name : RYLCheckBox.h 
//	Desc : RYL Check Box
//  Date : 2003. 09. 23
//
//================================================================
#ifndef __RYLCHECKBOX_H__
#define __RYLCHECKBOX_H__

#include "GMUICheckBox.h"

class CGMUICustomDlg ;
class CRYLFontTexture ;
class CGMImageDib ;

class CRYLCheckBox : public CGMUICheckBox
{
private :
	INT					m_iOffsetX ;
	INT					m_iOffsetY ;
	CRYLFontTexture*	m_pCaptionTexture ;
	LPSTR				m_pCaption ;

public:
	CRYLCheckBox( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLCheckBox() ;

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID SetVisible( BOOL bVisible ) ;
	
	VOID	SetCaption( LPSTR pCaption, BOOL bOffsetX = FALSE, BOOL bOffsetY = FALSE, DWORD dwFontColor = 0xffffffff) ;
	LPSTR	GetCaption() ;

	VOID	LoadFromFile( LPSTR pFileName, BOOL boDefault = FALSE) ;
	VOID	AttachButtonImage( CGMImageDib* pDib, LPRECT lpRect, INT iInstance ) ;
	VOID	AttachButtonImage( CGMTexture* pTexture, INT iInstance ) ;
	VOID	DefaultEvent( CRYLCheckBox* pCheckBox ) ;
} ;

#endif //__RYLCHECKBOX_H__