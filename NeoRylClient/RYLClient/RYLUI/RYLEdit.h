//================================================================
//
//	Name : RYLEdit.h 
//	Desc : RYL Edit Box
//  Date : 2003. 09. 22
//
//================================================================
#ifndef __RYLEDIT_H__
#define __RYLEDIT_H__

#include "GMUICustomEdit.h"
 
class CGMUICustomDlg ;

class CRYLFontTexture ;
class CGMTexture ;

//-- Zergra From.--//
class CRYLIMEView;
//-- Zergra To. --//

class CRYLEdit : public CGMUICustomEdit
{
protected:
	DWORD				   m_dwLineColor ;
	CRYLFontTexture*	m_pFontTexture ;
	CGMTexture*			m_pBackImage ;

   //-- Zergra From.--//
protected:
   CRYLIMEView*      m_pImeView;
   //-- Zergra To. --//

public:
	CRYLEdit( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLEdit() ;
	virtual VOID DoDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID DoCharDraw( CGMUIObject* Sender, RECT r, LPSTR str ) ;
	virtual VOID DoUnderLineDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID DoSelectLineDraw( CGMUIObject* Sender, RECT r ) ;
	virtual VOID SetFontColor( DWORD dwColor ) ;

	virtual VOID SetSize( CGMUIObject* Sender ) ;

	VOID SetLineColor( DWORD dwColor ) ;
	DWORD GetLineColor() ;

	VOID DefaultDraw( RECT r ) ;

	VOID AttachBackTexture( CGMTexture* pGMTexture ) ;
	VOID DetachBackTexture() ;
};

#endif
