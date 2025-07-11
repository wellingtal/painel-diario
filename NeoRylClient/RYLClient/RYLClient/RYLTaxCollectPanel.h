//================================================================
//
//	Name : RYLTaxCollectPanel.h 
//	Desc : 세금 회수 패널
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

	CRYLLabel*		m_pShopLabel ;				// '상점'
	CRYLLabel*		m_pShopTradeTaxLabel ;		// '구매 / 판매'
	CRYLLabel*		m_pShopTradeTax ;			// 수익값
	CRYLLabel*		m_pGoldLabel1 ;				// 'Gold'
	CRYLLabel*		m_pShopRepairTaxLabel ;		// '수리'
	CRYLLabel*		m_pShopRepairTax ;			// 수익값
	CRYLLabel*		m_pGoldLabel2 ;				// 'Gold'

	CRYLLabel*		m_pSmithLabel ;				// '대장장이'
	CRYLLabel*		m_pResmeltTaxLabel ;		// '수익'
	CRYLLabel*		m_pResmeltTax ;				// 수익값
	CRYLLabel*		m_pGoldLabel3 ;				// 'Gold'

	CRYLLabel*		m_pCampLabel ;				// '길드 요새'
	CRYLLabel*		m_pCampGoldTaxLabel ;		// 'Gold'
	CRYLLabel*		m_pCampGoldTax ;			// 수익값
	CRYLLabel*		m_pGoldLabel4 ;				// 'Gold'
	CRYLLabel*		m_pCampMineralTaxLabel ;	// '광물'
	CRYLLabel*		m_pCampMineralTax ;			// '별도 창 이용'


	CRYLButton*		m_pShopTradeBtn ;			// 상점 구매/판매 세금 회수 버튼
	CRYLButton*		m_pShopRepairBtn ;			// 상점 수리 세금 회수 버튼
	CRYLButton*		m_pResmeltBtn ;				// 대장장이 재련 세금 회수 버튼
	CRYLButton*		m_pCampGoldBtn ;			// 길드 요새 수익 세금 회수 버튼
	CRYLButton*		m_pCampMineralBtn ;			// 길드 요새 광물 세금 회수 버튼

	unsigned long	m_dwCurrCastleID ;			// 현재 선택된 성 ID

public :

	unsigned char	m_cControlIndex ;			// 어떠한 종류의 세금을 회수하는가
	unsigned long	m_dwControlResult ;			// 세금 회수 메세지 박스의 이벤트 결과값
	unsigned long	m_dwResultGold ;			// 회수할 세금

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
