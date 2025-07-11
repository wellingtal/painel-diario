//================================================================
//
//	Name : RYLCastleStatusPanel.h 
//	Desc : ���� ��Ȳ �г�
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

	CRYLLabel*			m_pCastleTitleLabel ;		// ���� Īȣ�� ���׷��̵� �ܰ�
	CRYLLabel*			m_pCastleTitleValue ;		// ���̸� ��
	CRYLLabel*			m_pCastleAreaLabel ;		// ���� ������ ��� ��Ȯ�� ���� ����
	CRYLLabel*			m_pMeterLabel ;				// ���� ("m")
	CRYLLabel*			m_pPercentLabel ;			// �ۼ�Ʈ ("%")
	CRYLLabel*			m_pCastleAreaValue ;		// ���Ͱ�
	CRYLLabel*			m_pCastleHarvestValue ;		// �ۼ�Ʈ��
	CRYLLabel*			m_pCastleDefenceLabel ;		// ���� ���ѳ� Ƚ��
	CRYLLabel*			m_pCastleDefenceValue ;		// Ƚ����
	CRYLLabel*			m_pCastleIncomeLabel ;		// ���� ���� ȸ����
	CRYLLabel*			m_pCastleIncomeValue ;		// ȸ����
	CRYLLabel*			m_pGoldLabel ;				// Gold ("Gold")

	unsigned long		m_dwCurrCastleID ;			// ���� ���õ� �� ID

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
