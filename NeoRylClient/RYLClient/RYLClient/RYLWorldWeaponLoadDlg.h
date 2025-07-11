//================================================================
//
//	Name : RYLWorldWeaponLoadDlg.h 
//	Desc : 월드 웨폰 장전 메뉴
//  Date : 2004. 12. 30
//
//================================================================
#ifndef __RYL_WORLDWEAPONLOADDLG_H__
#define __RYL_WORLDWEAPONLOADDLG_H__

#include "RYLDialog.h"

namespace RYLWORLDWEAPONLOADDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// 확인 버튼
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// 취소 버튼
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLWorldWeaponLoadDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// 진지 업그레이드 메뉴
	CRYLLabel*		m_pArticleLabel ;					// [자제]
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// 병기 생성 설명 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// 병기 생성 설명 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// 병기 생성 설명 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// 병기 생성 설명 4
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// 병기 생성 설명 5
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	DWORD			m_dwChangeCampType ;				//변경될 요새 타입
	int				m_nMaterialNeed ;					//필요 자재량

public :
	CRYLWorldWeaponLoadDlg() ;
	virtual ~CRYLWorldWeaponLoadDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;
	VOID	SetLoad( BOOL bLoad ) ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID	SetWeaponLoad( DWORD dwCampID, int nMtrlCnt = 50 )								{ m_dwCampID = dwCampID ; m_nMaterialNeed = nMtrlCnt ;} ;
} ;

#endif // __RYL_WORLDWEAPONLOADDLG_H__