//================================================================
//
//	Name : RYLVaultPasswordDlg.h 
//	Desc : ��й�ȣ ����� ���� ���̾�α�
//  Date : 2004. 01. 26
//
//================================================================
#ifndef __RYL_VAULTPASSWORDDLG_H__
#define __RYL_VAULTPASSWORDDLG_H__

#include "RYLDialog.h"

namespace RYLVAULTPASSWORDDLG
{
	const DWORD		CURRENTPASSWORD_EDIT		= 0x00000000 ;
	const DWORD		CHANGEPASSWORD_EDIT			= 0x00000001 ;
	const DWORD		OK_BUTTON					= 0x00000002 ;
	const DWORD		CANCEL_BUTTON				= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

//-----------------------------------------------------------------
// ��й�ȣ ����
class CRYLVaultPasswordDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pPasswordLabel ;				// �佺���庯�� �̸�
	CRYLLabel*			m_pCurrentPasswordLabel ;		// ���� ��й�ȣ
	CRYLLabel*			m_pChangePasswordLabel ;		// ���� ��й�ȣ
	CRYLButton*			m_pOkButton ;					// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton ;				// ��� ��ư

public :
	CRYLEdit*			m_pCurrentPasswordEdit ;		// ���� ��й�ȣ
	CRYLEdit*			m_pChangePasswordEdit ;			// ���� ��й�ȣ
	
public :
	CRYLVaultPasswordDlg() ;
	virtual ~CRYLVaultPasswordDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID	OKButtonEvent() ;
	VOID	CancelButtonEvent() ;
} ;


//-----------------------------------------------------------------
// ���� ����
namespace RYLVAULTCERTIFICATIONDDLG
{
	const DWORD		PASSWORD_EDIT		= 0x00000000 ;
	const DWORD		OK_BUTTON			= 0x00000001 ;
	const DWORD		CANCEL_BUTTON		= 0x00000002 ;
} ;
class CRYLVaultCertificationdDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pCertificationLabel ;			// ���� �̸�
	CRYLLabel*			m_pPasswordLabel ;				// ��й�ȣ�� �Է����ּ���.
	CRYLButton*			m_pOkButton ;					// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton ;				// ��� ��ư

public :
	CRYLEdit*			m_pPasswordEdit ;		// ���� ��й�ȣ
	CHAR				m_cSavePass ;
			
public :
	CRYLVaultCertificationdDlg() ;
	virtual ~CRYLVaultCertificationdDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID	OKButtonEvent() ;
	VOID	CancelButtonEvent() ;
} ;

#endif //__RYL_VAULTPASSWORDDLG_H__

