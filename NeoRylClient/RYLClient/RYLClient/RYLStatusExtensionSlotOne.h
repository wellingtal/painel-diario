//================================================================
//
//	Name : RYLStatusExtensionSlotOne.h 
//	Desc : ATT & DEF »Æ¿Â ΩΩ∑‘
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_STATUSEXTENSIONSLOTONE_H__
#define __RYL_STATUSEXTENSIONSLOTONE_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;

namespace RYLSTATUSEXTENSIONSLOTONE
{
} ;

class CRYLStatusExtensionSlotOne : public CGMUICustomPanel
{
private :
	CRYLLabel			*m_pATTLabel;
	CRYLLabel			*m_pDEFLabel;

public :
	CRYLStatusExtensionSlotOne( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLStatusExtensionSlotOne() ;

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
