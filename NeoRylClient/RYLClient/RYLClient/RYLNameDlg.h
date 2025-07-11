//=================================================================
// Name : RYLnameDlg.h
// Data : 2003. 12. 24
// Desc : ���� ������ �̸� ���̾�α� ?
//=================================================================
#ifndef __RYL_NAMEDLG_H__
#define __RYL_NAMEDLG_H__

#include "RYLDialog.h"

namespace RYLNAMEDLG
{
	const DWORD		HIDE_BUTTON					= 0x00000000 ;
} ;

class CRYLButton ;
class CRYLLabel ;

class CRYLNameDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pNameLabel ;				// �̸�
	CRYLButton*			m_pHideButton ;				// Hide ��ư
		
public :
	CRYLNameDlg() ;
	virtual ~CRYLNameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif //__RYL_NAMEDLG_H__