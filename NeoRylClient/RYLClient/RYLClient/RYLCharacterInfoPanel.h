//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCharacterInfoPanel.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 캐릭터 정보 패널
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __RYL_CHARACTER_INFO_PANEL_H__
#define __RYL_CHARACTER_INFO_PANEL_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "GMUICustomPanel.h"

//--------------------------------------------------------------------------------------------------------------------------
// pre-define
//--------------------------------------------------------------------------------------------------------------------------
class CRYLLabel;
class CRYLButton;
class CRYLProgressBar;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs, Namespaces
//--------------------------------------------------------------------------------------------------------------------------
namespace RYLCHARACTERINFOPANEL
{
    const DWORD             RANKING_BUTTON          = 0x00000000;
    const DWORD             HP_MP_BUTTON            = 0x00000001;
};

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLCharacterInfoPanel
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CRYLCharacterInfoPanel : public CGMUICustomPanel
{
private:
    CRYLLabel*		            m_pPanelNameLabel;					    // 패널 이름	
	CRYLLabel*		            m_pKillStreak;	
    CRYLLabel*		            m_pNameLabel;							// 이름	
	CRYLLabel*		            m_pNameValueLabel;						// 이름 값
	CRYLLabel*		            m_pClassLabel;							// 클레스
	CRYLLabel*		            m_pClassValueLabel;					    // 클레스 값
    CRYLLabel*		            m_pLevelLabel;							// 레벨
	CRYLLabel*		            m_pLevelValueLabel;					    // 레벨 값
    CRYLLabel*		            m_pFameLabel;							// 명성
	CRYLLabel*		            m_pFameValueLabel;						// 명성 값
	CRYLLabel*		            m_pServiceMedalLabel;					// 공헌 메달
	CRYLLabel*		            m_pServiceMedalValueLabel;				// 공헌 메달 값
	CRYLLabel*		            m_pGuildLabel;							// 길드
	CRYLLabel*		            m_pGuildValueLabel;					    // 길드 값     
	CRYLLabel*		            m_pNationalityLabel;					// 국적
	CRYLLabel*		            m_pNationalityValueLabel;				// 국적 값
                                                                                 
    CRYLSpriteEx*		        m_pFameMarkSprite;                      // 계급장
    CRYLSpriteEx*		        m_pGuildMarkSprite;                     // 길드 마크
    CRYLSpriteEx*		        m_pNationMarkSprite;                    // 국가 마크

    CRYLLabel*		            m_pHPLabel;							    // HP
    CRYLLabel*		            m_pHPValueLabel;                        // HP 값
	CRYLProgressBar*		    m_pHPProgressBar;						// HP 확장프로그레스바
	CRYLLabel*		            m_pMPLabel;							    // MP
    CRYLLabel*		            m_pMPValueLabel;						// MP 값
	CRYLProgressBar*		    m_pMPProgressBar;						// MP 확장프로그레스바
    CRYLLabel*		            m_pEXPLabel;							// EXP
    CRYLLabel*		            m_pEXPValueLabel;						// EXP
    CRYLProgressBar*	        m_pEXPProgressBar;                      // EXP 확장프로그레스바

    CRYLButton*		            m_pRankingButton;						// 랭킹 정보
	CRYLButton*		            m_pHPMPButton;							// HP/MP 창

public:
                                CRYLCharacterInfoPanel( CGMUICustomDlg *pParentDlg );
    virtual                     ~CRYLCharacterInfoPanel();

    virtual VOID                Show( CGMUIObject* Sender );
	virtual VOID                Hide( CGMUIObject* Sender );

            VOID	            InitCtrl();
	        VOID	            FinalCtrl();
	
	        HRESULT		        Update();

	static  VOID                DlgClick( CGMUIObject* Sender, INT X, INT Y );
};

#endif //__RYL_CHARACTER_INFO_PANEL_H__