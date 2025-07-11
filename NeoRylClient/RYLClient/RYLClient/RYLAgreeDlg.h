//================================================================
//
//	Name : RYLAgreeDlg.h 
//	Desc : 최초 접속시 동의창
//  Date : 2003. 11. 14
//
//================================================================
#ifndef __RYL_AGREEDLG_H__
#define __RYL_AGREEDLG_H__

#include "RYLDialog.h"

namespace RYLAGREEDLG
{
	const DWORD		AGREE_BUTTON			= 0x00000000 ;
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;
	const DWORD		AGREE_LISTBOX			= 0x00000002 ;
} ;

class CRYLButton ;
class CRYLListBox ;
class CRYLLabel ;

class CRYLAgreeDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pAgreeBtn ;
	CRYLButton*			m_pCancelBtn ;
	CRYLListBox*		m_pAgreeListBox ;
	CRYLLabel*			m_pAgreeLabel ;

public :
	CRYLAgreeDlg() ;
	virtual ~CRYLAgreeDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	
private :
	VOID		InsertStringAgreeListBox() ;
	VOID		ClearAgreeVector() ;	
} ;

#endif //__RYL_AGREEDLG_H__