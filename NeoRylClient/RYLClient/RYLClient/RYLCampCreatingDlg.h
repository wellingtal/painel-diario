//================================================================
//
//	Name : RYLCampCreatingDlg.h 
//	Desc : 진지 생성 구축해제
//  Date : 2004. 07. 14
//
//================================================================
#ifndef __RYL_RYLCAMPCREATINGDLG_H__
#define __RYL_RYLCAMPCREATINGDLG_H__

#include "RYLDialog.h"

namespace RYLCAMPCREATINGDLG
{
	const DWORD		CANCEL_BUTTON			= 0x00000000 ;	// 구축 해제
	const DWORD		CLOSE_BUTTON			= 0x00000001 ;	// 닫기 버튼
};

class CRYLButton ;
class CRYLLabel ;

class CRYLCampCreatingDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// 병기 생성 메뉴
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// 병기 생성 중 설명 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// 병기 생성 중 설명 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// 병기 생성 중 설명 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// 병기 생성 중 설명 4
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// 병기 생성 중 설명 5
	CRYLLabel*		m_pArmsCreatExpla6Label ;			// 병기 생성 중 설명 6
	
	CRYLButton*		m_pCancelButton ;
	CRYLButton*		m_pCloseButton ;
	DWORD			m_dwCampID ;

public :
	CRYLCampCreatingDlg() ;
	virtual ~CRYLCampCreatingDlg() ;

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
	VOID	SetCampID( DWORD dwCampID )								{ m_dwCampID = dwCampID ; } ;
} ;

#endif // __RYL_RYLCAMPCREATINGDLG_H__