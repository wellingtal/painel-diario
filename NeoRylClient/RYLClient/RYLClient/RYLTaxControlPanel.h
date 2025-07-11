//================================================================
//
//	Name : RYLTaxControlPanel.h 
//	Desc : 세율 조정 패널
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
	
	CRYLLabel*		m_pShopLabel ;				// '상점'
	CRYLLabel*		m_pShopTradeTaxLabel ;		// '상점 구매/판매 세율'
	CRYLLabel*		m_pShopTradeTax ;			// 구매 세율값
	CRYLLabel*		m_pPercentLabel1 ;			// '%'
	CRYLLabel*		m_pShopRepairTaxLabel ;		// '상점 수리 세율'
	CRYLLabel*		m_pShopRepairTax ;			// 수리 세율값
	CRYLLabel*		m_pPercentLabel2 ;			// '%'

	CRYLLabel*		m_pSmithLabel ;				// '대장장이'
	CRYLLabel*		m_pResmeltTaxLabel ;		// '재련 세율'
	CRYLLabel*		m_pResmeltTax ;				// 재련 세율값
	CRYLLabel*		m_pPercentLabel3 ;			// '%'

	CRYLLabel*		m_pCampLabel ;				// '길드 요새'
	CRYLLabel*		m_pCampGoldTaxLabel ;		// 'Gold 수익 세율'
	CRYLLabel*		m_pCampGoldTax ;			// 수익 세율값
	CRYLLabel*		m_pPercentLabel4 ;			// '%'
	CRYLLabel*		m_pCampMineralTaxLabel ;	// '채굴된 광물 세율'
	CRYLLabel*		m_pCampMineralTax ;			// 광물 세율값
	CRYLLabel*		m_pPercentLabel5 ;			// '%'


	CRYLButton*		m_pShopTradeBtn ;			// 상점 구매/판매 세율 변경 버튼
	CRYLButton*		m_pShopRepairBtn ;			// 상점 수리 세율 변경 버튼
	CRYLButton*		m_pResmeltBtn ;				// 대장장이 재련 세율 변경 버튼
	CRYLButton*		m_pCampGoldBtn ;			// 길드 요새 수익 세율 변경 버튼
	CRYLButton*		m_pCampMineralBtn ;			// 길드 요새 광물 세율 변경 버튼

	unsigned long	m_dwCurrCastleID ;			// 현재 선택된 성 ID

public :

	unsigned char	m_cControlIndex ;			// 어떠한 종류의 세율을 변경하는가
	unsigned long	m_dwControlResult ;			// 세율 변경 메세지 박스의 이벤트 결과값
	unsigned long	m_dwResultTax ;				// 변경시킬 세율

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
