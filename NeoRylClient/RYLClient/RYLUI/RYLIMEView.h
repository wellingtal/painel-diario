//================================================================
//
//	Name : RYLIMEView.h 
//	Desc : 
//  Date : 2003. 09. 22
//
//================================================================
#ifndef __RYLIMEVIEW_H__
#define __RYLIMEVIEW_H__

#include "GMUICustomImeView.h"


#define IMEVIEW_VIEWFONT_COUNT 9

class CRYLImage ;
class CRYLFontTexture ;
class CGMTexture ;

//------------------------------------------------------------------------------
//					Up 0,  Down 1,  Disible 2,  Enter 3 
//------------------------------------------------------------------------------
#define IMEVIEW_BUTTON_COUNT 4

//------------------------------------------------------------------------------
//					차후에 변경될부분 Texture로 Draw하는 부분인데 변경가능
//------------------------------------------------------------------------------
typedef struct tagSIMEVIEWFONTIMAGE 
{
	CRYLFontTexture *m_pArrCaption ;
} SIMEVIEWFONTIMAGE;

class CRYLIMEView : public CGMUICustomIMEView
{
protected:
	CRYLFontTexture*		m_paFontTexture[ IMEVIEW_VIEWFONT_COUNT ] ;
	CGMTexture*				m_pBackImage ;
	CGMTexture*				m_paLeftButtonImage[ IMEVIEW_BUTTON_COUNT ] ;
	CGMTexture*				m_paRightButtonImage[ IMEVIEW_BUTTON_COUNT ] ;
	SIMEVIEWFONTIMAGE*   m_pIMEVIEWFONTIMAGE ;

public:
	CRYLIMEView() ;
	virtual ~CRYLIMEView() ;

public:
    enum EImeViewButtonImageStateType
    {
		ImeViewButtonImageStateType_Up = 0,
		ImeViewButtonImageStateType_Down = 1,
		ImeViewButtonImageStateType_Disible = 2,
		ImeViewButtonImageStateType_Enter = 3
    } ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Left
	virtual VOID	DoLeftButtonDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// Right
	virtual VOID	DoRightButtonDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	// char
	virtual VOID	DoDrawImeChar( CGMUIObject* Sender, INT Index, RECT r, LPSTR pText, INT State ) ;

	virtual VOID	DoLeftButtonClick( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	DoRightButtonClick( CGMUIObject* Sender, INT X, INT Y ) ;
	virtual VOID	DoCharClick( CGMUIObject* Sender, LPSTR aText, INT Index, INT X, INT Y ) ;

public:
	virtual VOID	AttachLeftButtonTexture( EImeViewButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachLeftButtonTexture( EImeViewButtonImageStateType StateType);

	virtual VOID	AttachRightButtonTexture( EImeViewButtonImageStateType StateType, CGMTexture* pGMTexture ) ;
	virtual VOID	DetachRightButtonTexture( EImeViewButtonImageStateType StateType);

	virtual VOID	AttachBackTexture( CGMTexture* pGMTexture ) ;
	virtual VOID	DetachBackTexture() ;
};



#endif