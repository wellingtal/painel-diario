//================================================================
//
//	Name : RYLArmsCreatMenuDlg.h 
//	Desc : ���� ���� ���̾�α�
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_ARMSCREATMENUDLG_H__
#define __RYL_ARMSCREATMENUDLG_H__

#include "RYLDialog.h"

namespace RYLARMSCREATMENUDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLArmsCreatMenuDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// ���� ���� �޴�
	CRYLLabel*		m_pArticleLabel ;					// [����]
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// ���� ���� ���� 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// ���� ���� ���� 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// ���� ���� ���� 3
	
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;

public :
	CRYLArmsCreatMenuDlg() ;
	virtual ~CRYLArmsCreatMenuDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 
} ;

#endif // __RYL_ARMSCREATMENUDLG_H__