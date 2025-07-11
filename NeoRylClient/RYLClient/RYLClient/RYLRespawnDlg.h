//=================================================================
// Name : RYLRespawnDlg.h
// Data : 2004. 07. 27
// Desc : ��Ȱ �޴� ���̾�α�
//=================================================================
#ifndef __RYL_RYLRESPAWNDLG_H__
#define __RYL_RYLRESPAWNDLG_H__

#include "RYLDialog.h"

namespace RYLRESPAWNDLG
{
	const DWORD		RESPAWNNOW_BUTTON				= 0x00000000 ;
	const DWORD		RESPAWNCHOICE_BUTTON			= 0x00000001 ;
} ;

class CRYLButton ;
class CRYLLabel ;

class CRYLRespawnDlg : public CRYLDialog
{
public :
	CRYLButton*			m_pRespawnButton ;			// �ٷ� ��Ȱ
	CRYLButton*			m_pRespawnChoiceButton ;	// ��Ȱ��� ����
			
public :
	CRYLRespawnDlg() ;
	virtual ~CRYLRespawnDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual VOID SetVisible( BOOL bVisible ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif //__RYL_RYLRESPAWNDLG_H__
