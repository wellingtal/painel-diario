//================================================================
//
//	Name : RYLWorldWeaponLoadDlg.h 
//	Desc : ���� ���� ���� �޴�
//  Date : 2004. 12. 30
//
//================================================================
#ifndef __RYL_WORLDWEAPONLOADDLG_H__
#define __RYL_WORLDWEAPONLOADDLG_H__

#include "RYLDialog.h"

namespace RYLWORLDWEAPONLOADDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLWorldWeaponLoadDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// ���� ���׷��̵� �޴�
	CRYLLabel*		m_pArticleLabel ;					// [����]
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// ���� ���� ���� 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// ���� ���� ���� 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// ���� ���� ���� 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// ���� ���� ���� 4
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// ���� ���� ���� 5
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	DWORD			m_dwChangeCampType ;				//����� ��� Ÿ��
	int				m_nMaterialNeed ;					//�ʿ� ���緮

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