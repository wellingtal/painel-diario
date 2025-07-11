//================================================================
//
//	Name : RYLTaxCollectPanel.h 
//	Desc : ���� ȸ�� �г�
//  Date : 2005. 4. 18
//
//================================================================

#ifndef __RYL_TAXCOLLECTPANEL_H__
#define __RYL_TAXCOLLECTPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;
class CRYLButton ;

namespace RYLTAXCOLLECTPANEL
{
	const DWORD		SHOP_TRADE_BUTTON			= 0x00000000 ;
	const DWORD		SHOP_REPAIR_BUTTON			= 0x00000001 ;
	const DWORD		RESMELT_BUTTON				= 0x00000002 ;
	const DWORD		CAMP_GOLD_BUTTON			= 0x00000003 ;
	const DWORD		CAMP_MINERAL_BUTTON			= 0x00000004 ;
} ;

class CRYLTaxCollectPanel : public CGMUICustomPanel
{
private :

	CRYLLabel*		m_pShopLabel ;				// '����'
	CRYLLabel*		m_pShopTradeTaxLabel ;		// '���� / �Ǹ�'
	CRYLLabel*		m_pShopTradeTax ;			// ���Ͱ�
	CRYLLabel*		m_pGoldLabel1 ;				// 'Gold'
	CRYLLabel*		m_pShopRepairTaxLabel ;		// '����'
	CRYLLabel*		m_pShopRepairTax ;			// ���Ͱ�
	CRYLLabel*		m_pGoldLabel2 ;				// 'Gold'

	CRYLLabel*		m_pSmithLabel ;				// '��������'
	CRYLLabel*		m_pResmeltTaxLabel ;		// '����'
	CRYLLabel*		m_pResmeltTax ;				// ���Ͱ�
	CRYLLabel*		m_pGoldLabel3 ;				// 'Gold'

	CRYLLabel*		m_pCampLabel ;				// '��� ���'
	CRYLLabel*		m_pCampGoldTaxLabel ;		// 'Gold'
	CRYLLabel*		m_pCampGoldTax ;			// ���Ͱ�
	CRYLLabel*		m_pGoldLabel4 ;				// 'Gold'
	CRYLLabel*		m_pCampMineralTaxLabel ;	// '����'
	CRYLLabel*		m_pCampMineralTax ;			// '���� â �̿�'


	CRYLButton*		m_pShopTradeBtn ;			// ���� ����/�Ǹ� ���� ȸ�� ��ư
	CRYLButton*		m_pShopRepairBtn ;			// ���� ���� ���� ȸ�� ��ư
	CRYLButton*		m_pResmeltBtn ;				// �������� ��� ���� ȸ�� ��ư
	CRYLButton*		m_pCampGoldBtn ;			// ��� ��� ���� ���� ȸ�� ��ư
	CRYLButton*		m_pCampMineralBtn ;			// ��� ��� ���� ���� ȸ�� ��ư

	unsigned long	m_dwCurrCastleID ;			// ���� ���õ� �� ID

public :

	unsigned char	m_cControlIndex ;			// ��� ������ ������ ȸ���ϴ°�
	unsigned long	m_dwControlResult ;			// ���� ȸ�� �޼��� �ڽ��� �̺�Ʈ �����
	unsigned long	m_dwResultGold ;			// ȸ���� ����

	CRYLTaxCollectPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLTaxCollectPanel() ;

	VOID			InitCtrl() ;
	VOID			FinalCtrl() ;

	HRESULT			Update() ;

	static VOID		DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	virtual VOID	Show( CGMUIObject* Sender ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	bool			ProcessMessageBoxResult(void) ;

	void			SetCastleID( unsigned long dwCastleID )		{ m_dwCurrCastleID = dwCastleID ;	}
} ;

#endif //__RYL_TAXCOLLECTPANEL_H__
