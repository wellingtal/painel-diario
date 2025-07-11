//================================================================
//
//	Name : RYLCampRepairDlg.h 
//	Desc : 길드 요새 수리 다이얼로그
//  Date : 2005. 02. 28
//
//================================================================
#ifndef __RYL_CAMPREPAIRDLG_H__
#define __RYL_CAMPREPAIRDLG_H__

#include "RYLDialog.h"

namespace RYLCAMPREPAIRDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// 확인 버튼
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// 취소 버튼
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLCampRepairDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pCampRepairMenuLabel ;			// 길드 요새 수리 메뉴
	CRYLLabel*		m_pCampDurability ;					// 길드 요새 내구도
	CRYLLabel*		m_pCampRepairDescLabel1 ;			// 길드 요새 수리 설명 1
	CRYLLabel*		m_pCampRepairDescLabel2 ;			// 길드 요새 수리 설명 2
	CRYLLabel*		m_pCampRepairDescLabel3 ;			// 길드 요새 수리 설명 3
	CRYLLabel*		m_pCampRepairDescLabel4 ;			// 길드 요새 수리 설명 4

	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	DWORD			m_dwCurrHP ;
	DWORD			m_dwMaxHP ;
	BOOL			m_bWorldWeapon ;					// 월드웨폰 다이얼로그 변환

protected:
	VOID				ChangeDlg();

public :
	CRYLCampRepairDlg() ;
	virtual ~CRYLCampRepairDlg() ;

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
	VOID	SetCampID( DWORD dwCampID )								{ m_dwCampID = dwCampID ; }
	VOID	SetWorldWeaponDlg( BOOL bWeapon )						{ m_bWorldWeapon = bWeapon ; }
} ;

#endif // __RYL_CAMPREPAIRDLG_H__