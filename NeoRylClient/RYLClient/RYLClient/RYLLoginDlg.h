//================================================================
//
//	Name : RYLLoginDlg.h 
//	Desc : 로그인 다이얼로그 박스
//  Date : - Relese 2003. 11. 14
//		   - Relese 2004. 01. 18
//
//================================================================
#ifndef __RYLLOGINDLG_H__
#define __RYLLOGINDLG_H__

#include "RYLDialog.h"

namespace RYLLOGINDLG
{
	const DWORD		LOGIN_BUTTON			= 0x00000000 ;
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;

	const DWORD		FACE_BUTTON			    = 0x00000002 ;
	const DWORD		ID_EDITBOX				= 0x00000003 ;
	const DWORD		PASSWORD_EDITBOX		= 0x00000004 ;
	const DWORD		IDSAVE_CHECKBOX			= 0x00000005 ;
	const DWORD		ID_LABEL				= 0x00000006 ;
	const DWORD		PASSWORD_LABEL			= 0x00000007 ;
	const DWORD		IDSAVE_LABEL			= 0x00000008 ;
	const DWORD		REGISTER_BUTTON			= 0x00000009 ;
} ;

class CRYLButton ;
class CRYLEdit ;
class CRYLCheckBox ;
class CRYLLabel ;

class CRYLLoginDlg : public CRYLDialog
{
public:
	CRYLButton*			m_pLoginBtn ;
	CRYLButton*			m_pCancelBtn ;
	CRYLButton*			m_pFaceBtn ;
	CRYLButton*			m_pRegisterBtn ;
	CRYLEdit*			m_pIDEdit ;
	CRYLEdit*			m_pPassWordEdit ;
	CRYLCheckBox*		m_pIDSaveCheckBox ;

	CRYLLabel*			m_pIDLabel ;
	CRYLLabel*			m_pPassWordLabel ;
	CRYLLabel*			m_pIDSaveLabel ;
	CRYLLabel*			m_pLoginLabel ;

public :
	CRYLLoginDlg() ;
	virtual ~CRYLLoginDlg() ;

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

public :
	VOID	LoginButtonDown() ;
} ;

#endif //__RYLLOGINDLG_H__

