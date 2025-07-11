//================================================================
//
//	Name : RYLCampRepairDlg.h 
//	Desc : ��� ��� ���� ���̾�α�
//  Date : 2005. 02. 28
//
//================================================================
#ifndef __RYL_CAMPREPAIRDLG_H__
#define __RYL_CAMPREPAIRDLG_H__

#include "RYLDialog.h"

namespace RYLCAMPREPAIRDLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLCampRepairDlg : public CRYLDialog
{
private :
	CRYLLabel*		m_pCampRepairMenuLabel ;			// ��� ��� ���� �޴�
	CRYLLabel*		m_pCampDurability ;					// ��� ��� ������
	CRYLLabel*		m_pCampRepairDescLabel1 ;			// ��� ��� ���� ���� 1
	CRYLLabel*		m_pCampRepairDescLabel2 ;			// ��� ��� ���� ���� 2
	CRYLLabel*		m_pCampRepairDescLabel3 ;			// ��� ��� ���� ���� 3
	CRYLLabel*		m_pCampRepairDescLabel4 ;			// ��� ��� ���� ���� 4

	CRYLButton*		m_pOKButton ;
	CRYLButton*		m_pCancelButton ;
	DWORD			m_dwCampID ;
	DWORD			m_dwCurrHP ;
	DWORD			m_dwMaxHP ;
	BOOL			m_bWorldWeapon ;					// ������� ���̾�α� ��ȯ

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