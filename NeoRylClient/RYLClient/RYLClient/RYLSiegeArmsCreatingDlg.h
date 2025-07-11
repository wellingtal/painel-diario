//================================================================
//
//	Name : RYLSiegeArmsCreatingDlg.h 
//	Desc : ���� ���� ��������
//  Date : 2004. 07. 07
//
//================================================================
#ifndef __RYL_RYLSIEGEARMSCREATINGDLG_H__
#define __RYL_RYLSIEGEARMSCREATINGDLG_H__

#include "RYLDialog.h"

namespace RYLSIEGEARMSCREATINGDLG
{
	const DWORD		CANCEL_BUTTON			= 0x00000000 ;// ���� ����
	const DWORD		CLOSE_BUTTON			= 0x00000001 ;// â �ݱ�

};

class CRYLButton ;
class CRYLLabel ;

class CRYLSiegeArmsCreatingDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// ���� ���� �޴�
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// ���� ���� �� ���� 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// ���� ���� �� ���� 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// ���� ���� �� ���� 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// ���� ���� �� ���� 4
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// ���� ���� �� ���� 5
	CRYLLabel*		m_pArmsCreatExpla6Label ;			// ���� ���� �� ���� 6
	
	CRYLButton*		m_pCancelButton ;
	CRYLButton*		m_pCloseButton ;


public :
	CRYLSiegeArmsCreatingDlg() ;
	virtual ~CRYLSiegeArmsCreatingDlg() ;

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
} ;

#endif // __RYL_RYLSIEGEARMSCREATINGDLG_H__