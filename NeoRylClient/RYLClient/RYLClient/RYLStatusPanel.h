//================================================================
//
//	Name : RYLStatusPanel.h 
//	Desc : �������ͽ� �ǳ�
//  Date : 2003. 12. 10
//
//================================================================
#ifndef __RYL_STATUSPANEL_H__
#define __RYL_STATUSPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;
class CRYLButton ;
class CRYLSpriteEx ;

namespace RYLSTATUSPANEL
{
	const DWORD		RANKING_BUTTON				= 0x00000000 ;
	const DWORD		HP_MP_BUTTON				= 0x00000001 ;
	const DWORD		ENCHANT_BUTTON				= 0x00000002 ;
	const DWORD		STRPOINTINC_BUTTON			= 0x00000007 ;
	const DWORD		DEXPOINTINC_BUTTON			= 0x00000009 ;
	const DWORD		CONPOINTINC_BUTTON			= 0x00000011 ;
	const DWORD		INTPOINTINC_BUTTON			= 0x00000013 ;
	const DWORD		WISPOINTINC_BUTTON			= 0x00000015 ;
} ;

class CRYLStatusPanel : public CGMUICustomPanel
{
private :
	CRYLLabel*		m_pNameLabel ;							// �̸�	
	CRYLLabel*		m_pKillStreak;	
	CRYLLabel*		m_pNameValueLabel ;						// �̸� ��
	CRYLLabel*		m_pGuildLabel ;							// ���
	CRYLLabel*		m_pGuildValueLabel ;					// ��� ��
	CRYLLabel*		m_pClassLabel ;							// Ŭ����
	CRYLLabel*		m_pClassValueLabel ;					// Ŭ���� ��
//	CRYLLabel*		m_pTitleLabel ;							// ����
///	CRYLLabel*		m_pTitleValueLabel ;					// ���� ��
	CRYLLabel*		m_pMarkLabek ;							// ��ũ
	CRYLLabel*		m_pLevelLabel ;							// ����
	CRYLLabel*		m_pLevelValueLabel ;					// ���� ��
	CRYLLabel*		m_pExperienceLabel ;					// ������
	CRYLLabel*		m_pExperienceBalueLabel ;				// ������ ��
	CRYLLabel*		m_pFameLabel ;							// ��
	CRYLLabel*		m_pFameValueLabel ;						// �� ��
	CRYLLabel*		m_pServiceMedalLabel ;					// ���� �޴�
	CRYLLabel*		m_pServiceMedalValueLabel ;				// ���� �޴� ��
	CRYLLabel*		m_pScoreLabel ;							// ���ھ�
	CRYLLabel*		m_pScoreValueLabel ;					// ���ھ� ��
	CRYLLabel*		m_pSTRLabel ;							// STR
	CRYLLabel*		m_pSTRValueLabel ;						// STR ��
	CRYLLabel*		m_pDEXLabel ;							// DEX
	CRYLLabel*		m_pDEXValueLabel ;						// DEX ��
	CRYLLabel*		m_pCONLabel ;							// CON
	CRYLLabel*		m_pCONValueLabel ;						// CON ��
	CRYLLabel*		m_pINTLabel ;							// INT
	CRYLLabel*		m_pINTValueLabel ;						// INT ��
	CRYLLabel*		m_pWISLabel ;							// WIS
	CRYLLabel*		m_pWISValueLabel ;						// WIS ��
	CRYLLabel*		m_pPatchNotesLabel ;						
	CRYLLabel*		m_pPatchNotesAvalansaLabel ;
	CRYLLabel*		m_pPatchNotesLine1 ;
	CRYLLabel*		m_pPatchNotesLine2 ;
	CRYLLabel*		m_pPatchNotesLine3 ;
	CRYLLabel*		m_pPatchNotesLine4 ;
	CRYLLabel*		m_pPatchNotesLine5 ;
	CRYLLabel*		m_pPatchNotesLine6 ;
	CRYLLabel*		m_pPatchNotesLine7 ;
	CRYLLabel*		m_pPatchNotesLine8 ;
	CRYLLabel*		m_pPatchNotesLine9 ;
	CRYLLabel*		m_pPatchNotesLine10 ;
	CRYLLabel*		m_pPatchNotesLine11 ;
	CRYLLabel*		m_pPatchNotesLine12 ;
	CRYLLabel*		m_pPatchNotesLine13 ;
	CRYLLabel*		m_pPatchNotesLine14 ;
	CRYLLabel*		m_pPatchNotesLine15 ;

	CRYLLabel*		m_pStrikingPowerLabel ;					// ���ݷ�		
	CRYLLabel*		m_pStrikingPowerValueLabel ;			// ���·� ��
	CRYLLabel*		m_pDefensivePowerLabel ;				// ����
	CRYLLabel*		m_pDefensivePowerValueLabel ;			// ���� ��
	CRYLLabel*		m_pArmorRateValueLabel ;				// ����� ��

	CRYLLabel*		m_pHPLabel ;							// HP
	CRYLLabel*		m_pNowHPValueLabel ;					// ���� HP ��
	CRYLLabel*		m_pMaxHPValueLabel ;					// �ִ� HP ��

	CRYLLabel*		m_pMPLabel ;							// MP
	CRYLLabel*		m_pNowMPValueLabel ;					// ���� MP ��
	CRYLLabel*		m_pMaxMPValueLabel ;					// �ִ� MP ��

	CRYLLabel*		m_pBonusPointLabel ;					// ���ʽ� ����Ʈ
	CRYLLabel*		m_pBonusPointValueLabel ;				// ���ʽ� ����Ʈ ��

	CRYLLabel*		m_pHitRatioLabel ;						// ���߷�
	CRYLLabel*		m_pHitRatioValueLabel ;					// ���߷� ��		
	CRYLLabel*		m_pEvasionLabel ;						// ����
	CRYLLabel*		m_pEvasionValueLabel ;					// ���� ��

	CRYLLabel*		m_pMagicPowerLabel ;					// ������
	CRYLLabel*		m_pMagicPowerValueLabel ;				// ������ ��

	CRYLLabel*		m_pBlockPowerLabel ;					// ��Ϸ�
	CRYLLabel*		m_pBlockPowerValueLabel ;				// ��Ϸ� ��

	CRYLLabel*		m_pCriticalLabel ;						// ũ��Ƽ��
	CRYLLabel*		m_pCriticalValueLabel ;					// ũ��Ƽ�� ��

	CRYLLabel*		m_pMagicResistLabel ;					// ��������
	CRYLLabel*		m_pMagicResistValueLabel ;				// �������� ��
	
	CRYLButton*		m_pRankingButton ;						// ��ŷ ����
	CRYLButton*		m_pHPMPButton ;							// HP/MP â
	CRYLButton*		m_pEnchantButton ;						// ��þƮ â

	CRYLLabel*		m_pNationLabel;							// ����
	CRYLLabel*		m_pNationValueLabel;					// ���� ��

	CRYLSpriteEx*	m_pGuildMarkSprite;                     // ��� ��ũ


	// STR ����Ʈ ����,���� ��ư
	CRYLButton*			m_pStrPointIncBtn ;
	// DEX ����Ʈ ����,���� ��ư
	CRYLButton*			m_pDexPointIncBtn ;
	// CON ����Ʈ ����,���� ��ư
	CRYLButton*			m_pConPointIncBtn ;
	// INT ����Ʈ ����,���� ��ư
	CRYLButton*			m_pIntPointIncBtn ;
	// WIS ����Ʈ ����,���� ��ư
	CRYLButton*			m_pWisPointIncBtn ;

public :
	CRYLStatusPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLStatusPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

} ;

#endif //__RYL_STATUSPANEL_H__
