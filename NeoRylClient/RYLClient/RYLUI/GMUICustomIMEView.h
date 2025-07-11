//================================================================
//
//	Name : GMUICustomIMEView.h 
//	Desc : Gama Soft Custom IME View Class
//  Date : 2003. 09. 17
//
//================================================================
#ifndef __GMUICUSTOMIMEVIEW_H__
#define __GMUICUSTOMIMEVIEW_H__


#include "GMUIObject.h"
#include "RYLButton.h"

class CGMUICustomDlg;
class CGMUICustomEdit;
class CGMUICustomImeViewRightButton;
class CGMUICustomImeViewLeftButton;
class CRYLImage ;


class CGMUICustomIMEView : public CGMUIObject
{
private:
	LPSTR									m_strImeBuffer;
	INT										m_iImeCharViewCount;
	INT										m_iImeBufferViewIndex;
	INT										m_iImeBufferIndex;
	CGMUICustomEdit*						m_pGMUIEdit;
	CGMUICustomImeViewRightButton*			m_pImeViewRightButton;
	CGMUICustomImeViewLeftButton*			m_pImeViewLeftButton;

	CRYLImage*								m_pIMEBackGroundImage ;

	VOID	DrawCandidateString( INT xx, INT yy );

	VOID	InitGMUICtrl();
	VOID	FinalGMUICtrl();

	BOOL	GMUICtrlMove( CGMUIObject* Sender, INT Shift, INT X, INT Y);
	BOOL	GMUICtrlDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y);
	BOOL	GMUICtrlUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y);
	VOID	GMUICtrlSetSize();
protected:
	CGMString	m_pFontName;

public:
	CGMUICustomIMEView();
	virtual ~CGMUICustomIMEView();

public:
	VOID	SetGMUIEdit( CGMUICustomEdit *pGMUIEdit );

	INT GetImeCharViewCount();

   //-- Zergra From.--//
            void  SetLeftButtonPos( INT nLeft, INT nTop );
            void  SetRightButtonPos( INT nLeft, INT nTop );
   //-- Zergra To. --//

	virtual  VOID  SetVisible( BOOL boVisible );
	
	virtual  VOID  DoLeftButtonClick( CGMUIObject* Sender, INT X, INT Y );
	virtual  VOID  DoRightButtonClick(CGMUIObject* Sender, INT X, INT Y );
	virtual  VOID  DoCharClick( CGMUIObject* Sender, LPSTR aText, INT Index, INT X, INT Y );

	virtual  VOID  MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	virtual  VOID  MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y );
	virtual  VOID  MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	virtual  VOID  GMUIPaint( INT xx, INT yy );

	virtual  VOID  KeyDown( CGMUIObject* Sender, WORD key, INT Shift );
	virtual  VOID  DoDraw( CGMUIObject* Sender, RECT r, INT State );
	
	// Left
	virtual  VOID  DoLeftButtonDraw( CGMUIObject* Sender, RECT r, INT State );
	// Right 
	virtual  VOID  DoRightButtonDraw( CGMUIObject* Sender, RECT r, INT State );
	// char
	virtual  VOID  DoDrawImeChar( CGMUIObject* Sender, INT Index, RECT r, LPSTR pText, INT State );

	virtual  VOID  DoDrawImeCharBegin( CGMUIObject* Sender );
	virtual  VOID  DoDrawImeCharEnd( CGMUIObject* Sender );
};

 

class CGMUICustomImeViewLeftButton : public CRYLButton
{
private:
	CGMUICustomIMEView*		m_pGMUICustomImeView;

public:
	CGMUICustomImeViewLeftButton( CGMUICustomDlg *pGMUIDlg );
	virtual ~CGMUICustomImeViewLeftButton();

	virtual VOID SetParent( CGMUIObject* pGMUIObject );
	virtual VOID GetParent( CGMUIObject **GMUIObject );
	virtual VOID SetSize( CGMUIObject* Sender );

	virtual VOID Click( CGMUIObject* Sender, INT X, INT Y );

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State );
	virtual VOID GMUIPaint( INT xx, INT yy );
};


class CGMUICustomImeViewRightButton : public CRYLButton
{
private:
	CGMUICustomIMEView*		m_pGMUICustomImeView;

public:
	CGMUICustomImeViewRightButton( CGMUICustomDlg *aGMUIDlg );
	virtual ~CGMUICustomImeViewRightButton();

	virtual VOID SetParent( CGMUIObject* pGMUIObject );
	virtual VOID GetParent( CGMUIObject **GMUIObject );
	virtual VOID SetSize( CGMUIObject* Sender );

	virtual VOID Click( CGMUIObject* Sender, INT X, INT Y );

	virtual VOID DoDraw( CGMUIObject* Sender, RECT r, INT State );
	virtual VOID GMUIPaint( INT xx, INT yy );
};

#endif