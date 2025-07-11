//================================================================
//
//	Name : RYLBlacksmithDlg.h 
//	Desc : �������� ���̾�α�
//  Date : 2004. 01. 07
//
//================================================================
#ifndef __RYL_BLACKSMITHDLG_H__
#define __RYL_BLACKSMITHDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	// Added by ClassView

class CRYLButton ;
class CRYLLabel ;

namespace RYLBLACKSMITHDLG
{
	const DWORD		REINFORCEMENT_BUTTON	= 0x00000000 ;
	const DWORD		REPAIR_BUTTON			= 0x00000001 ;
	const DWORD		ALLREPAIR_BUTTON		= 0x00000002 ;
} ;

class CRYLBlacksmithDlg : public CRYLDialog
{
private :

	CRYLButton*			m_pReinforcementButton ;			// ��ȭ
	CRYLButton*			m_pRepairButton ;					// �����ϱ�
	CRYLButton*			m_pAllRepairButton ;				// ��� �����ϱ�
	CRYLLabel*			m_pRefineryLabel ;					// ����
	CRYLLabel*			m_pReinforcementWeaponLabel ;		// ��ȭ��ų ���
	CRYLLabel*			m_pMetalLabel ;						// �ݼ�
	CRYLLabel*			m_pGoldLabel ;						// ���

	CRYLLabel*			m_pResmeltTax ;						// ���� ����
	CRYLLabel*			m_pRepairTax ;						// ���� ����
	CRYLLabel*			m_pResmeltTaxValue ;				// ���� ���� ��
	CRYLLabel*			m_pRepairTaxValue ;					// ���� ���� ��

	DWORD				m_dwUpgradeGold ;
	DWORD				m_dwUpgradeResult ;
	

public :

	enum Const
	{
		// edith 2008.02.27 ����,�ɼ��̽� ���� ������ ����
		UPGRADE_LIMIT_LEVEL		= 1,	// ���� ���� ���� 40
		GRAFT_LIMIT_LEVEL		= 1		// �ɼ� �̽� ���� ���� 20
	};

	CRYLBlacksmithDlg() ;
	virtual ~CRYLBlacksmithDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	ReinforcementButton() ;
	VOID	RepairButton() ;		
	VOID	AllRepairButton() ;	
} ;

#endif // __RYL_BLACKSMITHDLG_H__