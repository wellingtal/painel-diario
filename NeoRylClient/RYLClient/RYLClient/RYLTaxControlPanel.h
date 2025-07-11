//================================================================
//
//	Name : RYLTaxControlPanel.h 
//	Desc : ���� ���� �г�
//  Date : 2005. 4. 18
//
//================================================================

#ifndef __RYL_TAXCONTROLPANEL_H__
#define __RYL_TAXCONTROLPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;
class CRYLButton ;

namespace RYLTAXCONTROLPANEL
{
	const DWORD		SHOP_TRADE_BUTTON			= 0x00000000 ;
	const DWORD		SHOP_REPAIR_BUTTON			= 0x00000001 ;
	const DWORD		RESMELT_BUTTON				= 0x00000002 ;
	const DWORD		CAMP_GOLD_BUTTON			= 0x00000003 ;
	const DWORD		CAMP_MINERAL_BUTTON			= 0x00000004 ;
} ;

class CRYLTaxControlPanel : public CGMUICustomPanel
{
private :
	
	CRYLLabel*		m_pShopLabel ;				// '����'
	CRYLLabel*		m_pShopTradeTaxLabel ;		// '���� ����/�Ǹ� ����'
	CRYLLabel*		m_pShopTradeTax ;			// ���� ������
	CRYLLabel*		m_pPercentLabel1 ;			// '%'
	CRYLLabel*		m_pShopRepairTaxLabel ;		// '���� ���� ����'
	CRYLLabel*		m_pShopRepairTax ;			// ���� ������
	CRYLLabel*		m_pPercentLabel2 ;			// '%'

	CRYLLabel*		m_pSmithLabel ;				// '��������'
	CRYLLabel*		m_pResmeltTaxLabel ;		// '��� ����'
	CRYLLabel*		m_pResmeltTax ;				// ��� ������
	CRYLLabel*		m_pPercentLabel3 ;			// '%'

	CRYLLabel*		m_pCampLabel ;				// '��� ���'
	CRYLLabel*		m_pCampGoldTaxLabel ;		// 'Gold ���� ����'
	CRYLLabel*		m_pCampGoldTax ;			// ���� ������
	CRYLLabel*		m_pPercentLabel4 ;			// '%'
	CRYLLabel*		m_pCampMineralTaxLabel ;	// 'ä���� ���� ����'
	CRYLLabel*		m_pCampMineralTax ;			// ���� ������
	CRYLLabel*		m_pPercentLabel5 ;			// '%'


	CRYLButton*		m_pShopTradeBtn ;			// ���� ����/�Ǹ� ���� ���� ��ư
	CRYLButton*		m_pShopRepairBtn ;			// ���� ���� ���� ���� ��ư
	CRYLButton*		m_pResmeltBtn ;				// �������� ��� ���� ���� ��ư
	CRYLButton*		m_pCampGoldBtn ;			// ��� ��� ���� ���� ���� ��ư
	CRYLButton*		m_pCampMineralBtn ;			// ��� ��� ���� ���� ���� ��ư

	unsigned long	m_dwCurrCastleID ;			// ���� ���õ� �� ID

public :

	unsigned char	m_cControlIndex ;			// ��� ������ ������ �����ϴ°�
	unsigned long	m_dwControlResult ;			// ���� ���� �޼��� �ڽ��� �̺�Ʈ �����
	unsigned long	m_dwResultTax ;				// �����ų ����

	CRYLTaxControlPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLTaxControlPanel() ;

	VOID			InitCtrl() ;
	VOID			FinalCtrl() ;

	HRESULT			Update() ;

	static VOID		DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	virtual VOID	Show( CGMUIObject* Sender ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	bool			ProcessMessageBoxResult(void) ;

	void			SetCastleID( unsigned long dwCastleID )		{ m_dwCurrCastleID = dwCastleID ;	}
} ;

#endif //__RYL_TAXCONTROLPANEL_H__
