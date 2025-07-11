//================================================================
//
//	Name : RYLCampShowTaxInfoDlg.h 
//	Desc : ��� ����, ä������ ���� �ູ �� ���� ���� Ȯ�� â
//  Date : 2005. 05. 18
//
//================================================================
#ifndef __RYL_CAMPSHOWTAXINFODLG_H__
#define __RYL_CAMPSHOWTAXINFODLG_H__

#include "RYLDialog.h"

namespace RYLCAMPSHOWTAXINFODLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;	// Ȯ�� ��ư
};

class CRYLButton ;
class CRYLLabel ;

class CRYLCampShowTaxInfoDlg : public CRYLDialog
{
private :

	enum eCampType
	{
		E_NONE			= 0,
		E_CAMP_SHOP		= 1, 
		E_MINING_CAMP	= 2
	};

	CRYLLabel*		m_pTitleLabel ;				// ���� �ູ / ���� ����
	CRYLLabel*		m_pCastleLabel ;			// �ູ�� �ִ� ��
	CRYLLabel*		m_pCastleValue ;			// �� �̸� (Īȣ + ���̸�)
	CRYLLabel*		m_pCastleBonusLabel ;		// �ູ ���ʽ�
	CRYLLabel*		m_pCastleBonusValue ;		// �ູ ���ʽ� ��
	CRYLLabel*		m_pCastleTaxLabel ;			// ��� ���� ���� ���� (ä���� ���� ����)
	CRYLLabel*		m_pCastleTaxValue ;			// ���� ��
	CRYLButton*		m_pOKButton ;				// Ȯ�� ��ư

	vector3			m_vecPos ;					// ��� ��ġ
	eCampType		m_eCampType ;				// ��� Ÿ��

public :
	CRYLCampShowTaxInfoDlg() ;
	virtual ~CRYLCampShowTaxInfoDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	VOID	SetCampShop( const vector3& vecPos ) ;
	VOID	SetMiningCamp( const vector3& vecPos ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_CAMPSHOWTAXINFODLG_H__