//================================================================
//
//	Name : RYLVaultPasswordDlg.h 
//	Desc : 비밀번호 변경및 인증 다이얼로그
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
// 비밀번호 변경
class CRYLVaultPasswordDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pPasswordLabel ;				// 페스워드변경 이름
	CRYLLabel*			m_pCurrentPasswordLabel ;		// 현제 비밀번호
	CRYLLabel*			m_pChangePasswordLabel ;		// 변경 비밀번호
	CRYLButton*			m_pOkButton ;					// 확인 버튼
	CRYLButton*			m_pCancelButton ;				// 취소 버튼

public :
	CRYLEdit*			m_pCurrentPasswordEdit ;		// 현제 비밀번호
	CRYLEdit*			m_pChangePasswordEdit ;			// 변경 비밀번호
	
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
// 인증 변경
namespace RYLVAULTCERTIFICATIONDDLG
{
	const DWORD		PASSWORD_EDIT		= 0x00000000 ;
	const DWORD		OK_BUTTON			= 0x00000001 ;
	const DWORD		CANCEL_BUTTON		= 0x00000002 ;
} ;
class CRYLVaultCertificationdDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pCertificationLabel ;			// 인증 이름
	CRYLLabel*			m_pPasswordLabel ;				// 비밀번호를 입력해주세요.
	CRYLButton*			m_pOkButton ;					// 확인 버튼
	CRYLButton*			m_pCancelButton ;				// 취소 버튼

public :
	CRYLEdit*			m_pPasswordEdit ;		// 현제 비밀번호
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

