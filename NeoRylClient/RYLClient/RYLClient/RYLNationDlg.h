//================================================================
//
//	Name : RYLNationDlg.h 
//	Desc : 현재 계정 국적을 표시하는 다이얼로그
//  Date : 2004. 11. 16
//
//================================================================

// WORK_LIST 2.3 계정 국적 변경 UI 추가

#ifndef __RYL_NATION_DLG_H__
#define __RYL_NATION_DLG_H__

#include "RYLDialog.h"

namespace RYLNATIONDLG
{
	const DWORD		KARTERANT_BUTTON	= 0x00000000 ;
	const DWORD		MERKADIA_BUTTON		= 0x00000001 ;
	const DWORD		PIRATE_BUTTON		= 0x00000002 ;
} ;

class CRYLButton ;
class CRYLLabel ;

//===========================================================
// Nation Dlg
//===========================================================
class CRYLNationDlg : public CRYLDialog
{
private:
	CRYLButton*		m_pKarterantBtn ;	// 계정 국적 변경 버튼 (카르테란트 국적 깃발 표시)
	CRYLButton*		m_pMerkadiaBtn ;	// 계정 국적 변경 버튼 (메르카디아 국적 깃발 표시)
	CRYLButton*		m_pPirateBtn ;		// 계정 국적 변경 버튼 (신의 해적단 국적 깃발 표시)

	CRYLLabel*		m_pDescLabel ;		// 설명문
	CRYLLabel*		m_pNationLabel ;	// 국가 이름

	DWORD			m_dwCurrentNation ; // 현재 계정 국가

public :
	CRYLNationDlg() ;
	virtual ~CRYLNationDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )	;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ;

	VOID	SetCurrentNation( DWORD dwNation ) ;
} ;


#endif //__RYL_NATION_DLG_H__
