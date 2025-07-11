//================================================================
//
//	Name : RYLSummonDlg.h 
//	Desc : 소환 다이얼로그
//  Date : 2004. 01. 29
//
//================================================================
#ifndef __RYL_SUMMONDLG_H__
#define __RYL_SUMMONDLG_H__

#include "RYLDialog.h"

namespace RYLSUMMONDLG
{
	const DWORD		ATTACK_BUTTON				= 0x00000000 ;
	const DWORD		RETURN_BUTTON				= 0x00000001 ;
	const DWORD		DEFENSE_BUTTON				= 0x00000002 ;
	const DWORD		EXIT_BUTTON					= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLProgressBar ;

class CRYLSummonDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pAttackButton ;				// 공격 버튼
	CRYLButton*			m_pReturnButton ;				// 돌아오기 버튼
	CRYLButton*			m_pDefenseButton ;				// 방어 버튼
	CRYLButton*			m_pExitButton ;					// 소환종료 버튼
	CRYLProgressBar*	m_pHPProgressBar ;				// HP

public :
	CRYLSummonDlg() ;
	virtual ~CRYLSummonDlg() ;

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

	VOID		SetSummonHPProgress( UINT uiCurrentHP, UINT uiMaxHP ) ;
	VOID		AttackButtonEvent() ;
	VOID		ReturnButtonEvent() ;
	VOID		DefenseButtonEvent() ;
	VOID		ExitButtonEvent() ;

} ;

#endif //__RYL_SUMMONDLG_H__