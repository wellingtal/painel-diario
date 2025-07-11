//================================================================
//
//	Name : RYLCampUpgradeDlg.h 
//	Desc : ���� ���׷��̵� �޴�
//  Date : 2004. 07. 14
//
//================================================================
#ifndef __RYL_CAMPUPGRADEDLG_H__
#define __RYL_CAMPUPGRADEDLG_H__

#include "RYLDialog.h"

namespace RYLCAMPUPGRADEDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLCampUpgradeDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pArmsCreatMenuLabel ;				// ���� ���׷��̵� �޴�
	CRYLLabel*		m_pArmsRemainTimeLabel ;			// ���׷��̵� �ð� ����
	CRYLLabel*		m_pArmsCreatExpla1Label ;			// ���� ���� ���� 1
	CRYLLabel*		m_pArmsCreatExpla2Label ;			// ���� ���� ���� 2
	CRYLLabel*		m_pArmsCreatExpla3Label ;			// ���� ���� ���� 3
	CRYLLabel*		m_pArmsCreatExpla4Label ;			// ���� ���� ���� 4
	CRYLLabel*		m_pArmsCreatExpla5Label ;			// ���� ���� ���� 5
	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	int				m_nMaterialNeed ;					//�ʿ� ���緮
	BOOL			m_bWorldWeapon ;							// ������� ���̾�α� ��ȯ

protected:
	VOID				ChangeDlg();

public :
	CRYLCampUpgradeDlg() ;
	virtual ~CRYLCampUpgradeDlg() ;

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
	VOID	SetCampID( DWORD dwCampID, int nMtrlCnt )								{ m_dwCampID = dwCampID ; m_nMaterialNeed = nMtrlCnt; } ;
	VOID	SetWorldWeaponDlg( BOOL bWeapon ){ m_bWorldWeapon = bWeapon ; };
} ;

#endif // __RYL_ARMSCREATMENUDLG_H__