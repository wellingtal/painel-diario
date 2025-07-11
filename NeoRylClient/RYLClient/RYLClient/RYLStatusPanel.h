//================================================================
//
//	Name : RYLStatusPanel.h 
//	Desc : 스테이터스 판넬
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
	CRYLLabel*		m_pNameLabel ;							// 이름	
	CRYLLabel*		m_pKillStreak;	
	CRYLLabel*		m_pNameValueLabel ;						// 이름 값
	CRYLLabel*		m_pGuildLabel ;							// 길드
	CRYLLabel*		m_pGuildValueLabel ;					// 길드 값
	CRYLLabel*		m_pClassLabel ;							// 클레스
	CRYLLabel*		m_pClassValueLabel ;					// 클레스 값
//	CRYLLabel*		m_pTitleLabel ;							// 작위
///	CRYLLabel*		m_pTitleValueLabel ;					// 작위 값
	CRYLLabel*		m_pMarkLabek ;							// 마크
	CRYLLabel*		m_pLevelLabel ;							// 레벨
	CRYLLabel*		m_pLevelValueLabel ;					// 레벨 값
	CRYLLabel*		m_pExperienceLabel ;					// 경험점
	CRYLLabel*		m_pExperienceBalueLabel ;				// 경험점 값
	CRYLLabel*		m_pFameLabel ;							// 명성
	CRYLLabel*		m_pFameValueLabel ;						// 명성 값
	CRYLLabel*		m_pServiceMedalLabel ;					// 공헌 메달
	CRYLLabel*		m_pServiceMedalValueLabel ;				// 공헌 메달 값
	CRYLLabel*		m_pScoreLabel ;							// 스코어
	CRYLLabel*		m_pScoreValueLabel ;					// 스코어 값
	CRYLLabel*		m_pSTRLabel ;							// STR
	CRYLLabel*		m_pSTRValueLabel ;						// STR 값
	CRYLLabel*		m_pDEXLabel ;							// DEX
	CRYLLabel*		m_pDEXValueLabel ;						// DEX 값
	CRYLLabel*		m_pCONLabel ;							// CON
	CRYLLabel*		m_pCONValueLabel ;						// CON 값
	CRYLLabel*		m_pINTLabel ;							// INT
	CRYLLabel*		m_pINTValueLabel ;						// INT 값
	CRYLLabel*		m_pWISLabel ;							// WIS
	CRYLLabel*		m_pWISValueLabel ;						// WIS 값
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

	CRYLLabel*		m_pStrikingPowerLabel ;					// 공격력		
	CRYLLabel*		m_pStrikingPowerValueLabel ;			// 공력력 값
	CRYLLabel*		m_pDefensivePowerLabel ;				// 방어력
	CRYLLabel*		m_pDefensivePowerValueLabel ;			// 방어력 값
	CRYLLabel*		m_pArmorRateValueLabel ;				// 방어율 값

	CRYLLabel*		m_pHPLabel ;							// HP
	CRYLLabel*		m_pNowHPValueLabel ;					// 현재 HP 값
	CRYLLabel*		m_pMaxHPValueLabel ;					// 최대 HP 값

	CRYLLabel*		m_pMPLabel ;							// MP
	CRYLLabel*		m_pNowMPValueLabel ;					// 현재 MP 값
	CRYLLabel*		m_pMaxMPValueLabel ;					// 최대 MP 값

	CRYLLabel*		m_pBonusPointLabel ;					// 보너스 포인트
	CRYLLabel*		m_pBonusPointValueLabel ;				// 보너스 포인트 값

	CRYLLabel*		m_pHitRatioLabel ;						// 명중률
	CRYLLabel*		m_pHitRatioValueLabel ;					// 명중률 값		
	CRYLLabel*		m_pEvasionLabel ;						// 방어보정
	CRYLLabel*		m_pEvasionValueLabel ;					// 방어보정 값

	CRYLLabel*		m_pMagicPowerLabel ;					// 마법력
	CRYLLabel*		m_pMagicPowerValueLabel ;				// 마법력 값

	CRYLLabel*		m_pBlockPowerLabel ;					// 블록률
	CRYLLabel*		m_pBlockPowerValueLabel ;				// 블록률 값

	CRYLLabel*		m_pCriticalLabel ;						// 크리티컬
	CRYLLabel*		m_pCriticalValueLabel ;					// 크리티컬 값

	CRYLLabel*		m_pMagicResistLabel ;					// 마법저항
	CRYLLabel*		m_pMagicResistValueLabel ;				// 마법저항 값
	
	CRYLButton*		m_pRankingButton ;						// 랭킹 정보
	CRYLButton*		m_pHPMPButton ;							// HP/MP 창
	CRYLButton*		m_pEnchantButton ;						// 인첸트 창

	CRYLLabel*		m_pNationLabel;							// 국적
	CRYLLabel*		m_pNationValueLabel;					// 국적 값

	CRYLSpriteEx*	m_pGuildMarkSprite;                     // 길드 마크


	// STR 포인트 증가,감소 버튼
	CRYLButton*			m_pStrPointIncBtn ;
	// DEX 포인트 증가,감소 버튼
	CRYLButton*			m_pDexPointIncBtn ;
	// CON 포인트 증가,감소 버튼
	CRYLButton*			m_pConPointIncBtn ;
	// INT 포인트 증가,감소 버튼
	CRYLButton*			m_pIntPointIncBtn ;
	// WIS 포인트 증가,감소 버튼
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
