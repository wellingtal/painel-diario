//================================================================
//
//	Name : RYLStatusExtensionSlotTwo.h 
//	Desc : ATR & DFR »Æ¿Â ΩΩ∑‘
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_STATUSEXTENSIONSLOTTWO_H__
#define __RYL_STATUSEXTENSIONSLOTTWO_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;

namespace RYLSTATUSEXTENSIONSLOTTWO
{
} ;

class CRYLStatusExtensionSlotTwo : public CGMUICustomPanel
{
private :
	CRYLLabel			*m_pATRLabel;
	CRYLLabel			*m_pDFRLabel;

public :
	CRYLStatusExtensionSlotTwo( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLStatusExtensionSlotTwo() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;


	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;







#endif