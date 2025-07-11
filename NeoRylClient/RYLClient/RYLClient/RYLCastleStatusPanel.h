//================================================================
//
//	Name : RYLCastleStatusPanel.h 
//	Desc : 성의 현황 패널
//  Date : 2005. 4. 18
//
//================================================================

#ifndef __RYL_CASTLESTATUSPANEL_H__
#define __RYL_CASTLESTATUSPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;

class CRYLCastleStatusPanel : public CGMUICustomPanel
{
private :

	CRYLLabel*			m_pCastleTitleLabel ;		// 성의 칭호와 업그레이드 단계
	CRYLLabel*			m_pCastleTitleValue ;		// 성이름 값
	CRYLLabel*			m_pCastleAreaLabel ;		// 성의 영역과 요새 수확량 증가 비율
	CRYLLabel*			m_pMeterLabel ;				// 미터 ("m")
	CRYLLabel*			m_pPercentLabel ;			// 퍼센트 ("%")
	CRYLLabel*			m_pCastleAreaValue ;		// 미터값
	CRYLLabel*			m_pCastleHarvestValue ;		// 퍼센트값
	CRYLLabel*			m_pCastleDefenceLabel ;		// 성을 지켜낸 횟수
	CRYLLabel*			m_pCastleDefenceValue ;		// 횟수값
	CRYLLabel*			m_pCastleIncomeLabel ;		// 지난 세금 회수량
	CRYLLabel*			m_pCastleIncomeValue ;		// 회수량
	CRYLLabel*			m_pGoldLabel ;				// Gold ("Gold")

	unsigned long		m_dwCurrCastleID ;			// 현재 선택된 성 ID

public :

	CRYLCastleStatusPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLCastleStatusPanel() ;

	VOID			InitCtrl() ;
	VOID			FinalCtrl() ;

	HRESULT			Update() ;

	static VOID		DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	virtual VOID	Show( CGMUIObject* Sender ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	void			SetCastleID( unsigned long dwCastleID )		{ m_dwCurrCastleID = dwCastleID ;	}
} ;

#endif //__RYL_CASTLESTATUSPANEL_H__
