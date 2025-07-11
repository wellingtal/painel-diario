//================================================================
//
//	Name : RYLSocietyDlg.h 
//	Desc : 파티, 친구, 거부, 파티 찾기 다이얼로그
//  Date : 2003. 12. 15
//
//================================================================
#ifndef __RYL_SOCIETYDLG_H__
#define __RYL_SOCIETYDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CGMUICustomPanel ;

namespace RYLSOCIETYDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
} ;

class CRYLSocietyDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	//CGMUICustomPanel*	m_pStatusPanel ;
	
public :
	CRYLSocietyDlg() ;
	virtual ~CRYLSocietyDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
    virtual HRESULT		Update() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_SOCIETYDLG_H__