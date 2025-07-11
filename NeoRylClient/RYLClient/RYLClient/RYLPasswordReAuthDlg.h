//================================================================
//
//	Name : RYLPasswordReAuthDlg.h 
//	Desc : 비밀번호재인증
//
//================================================================
#ifndef __RYLPASSWORDREAUTHDLG_H__
#define __RYLPASSWORDREAUTHDLG_H__

#include "RYLDialog.h"

namespace RYLPASSWORDREAUTHDLG
{
	const DWORD		CONFIRM_BUTTON			= 0x00000000 ;
	const DWORD		PASSWORD_EDITBOX		= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLEdit ;
class CRYLLabel ;

class CRYLPasswordReAuthDlg : public CRYLDialog
{
public:
	CRYLButton*			m_pConfirmButton ;
	CRYLEdit*			m_pPasswordEdit ;
	CRYLLabel*			m_pTitleLabel ;
	CRYLLabel*			m_pDescLabel ;
	DWORD				m_dwResult ;
	bool				m_bAddCharEnable ;
	
public :
	CRYLPasswordReAuthDlg() ;
	virtual ~CRYLPasswordReAuthDlg() ;

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
	
	void	CheckPassword() ;
	bool	PreCheckPassword() ;
} ;

#endif //__RYLPASSWORDREAUTHDLG_H__