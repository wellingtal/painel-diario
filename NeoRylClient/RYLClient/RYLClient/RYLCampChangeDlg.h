//================================================================
//
//	Name : RYLCampChangeDlg.h 
//	Desc : 진지 변형 메뉴
//  Date : 2004. 07. 14
//
//================================================================
#ifndef __RYL_CAMPCHANGEDLG_H__
#define __RYL_CAMPCHANGEDLG_H__

#include "RYLDialog.h"

namespace RYLCAMPCHANGEDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// 확인 버튼
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// 취소 버튼
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLCampChangeDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// 진지 업그레이드 메뉴
	CRYLLabel*		m_pArmsRemainTimeLabel ;			// 타입 변형 시간 설명
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// 병기 생성 설명 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// 병기 생성 설명 2
/*
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// 병기 생성 설명 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// 병기 생성 설명 4
*/
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// 병기 생성 설명 5
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	DWORD			m_dwChangeCampType ;				//변경될 요새 타입
	int				m_nMaterialNeed ;					//필요 자재량

public :
	CRYLCampChangeDlg() ;
	virtual ~CRYLCampChangeDlg() ;

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
	VOID		SetCampChange( DWORD dwCampID, DWORD dwChangeCampType, int nMtrlCnt )	{ m_dwCampID = dwCampID ; m_dwChangeCampType = dwChangeCampType; m_nMaterialNeed = nMtrlCnt ;} ;
} ;

#endif // __RYL_CAMPCHANGEDLG_H__