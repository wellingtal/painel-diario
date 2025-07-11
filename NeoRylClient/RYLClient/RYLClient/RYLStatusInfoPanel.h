//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLStatusInfoPanel.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 상태 정보 패널
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __RYL_STATUS_INFO_PANEL_H__
#define __RYL_STATUS_INFO_PANEL_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "GMUICustomPanel.h"

//--------------------------------------------------------------------------------------------------------------------------
// pre-defines
//--------------------------------------------------------------------------------------------------------------------------
class CRYLLabel;
class CRYLButton;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs, Namespaces, Etc
//--------------------------------------------------------------------------------------------------------------------------
namespace RYLSTATUSINFOPANEL
{
    const DWORD		        STRPOINTINC_BUTTON			= 0x00000001;
	const DWORD		        DEXPOINTINC_BUTTON			= 0x00000002;
	const DWORD		        CONPOINTINC_BUTTON			= 0x00000003;
	const DWORD		        INTPOINTINC_BUTTON			= 0x00000004;
	const DWORD		        WISPOINTINC_BUTTON			= 0x00000005;
};

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLStatusInfoPanel
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CRYLStatusInfoPanel : public CGMUICustomPanel
{
private:
    CRYLLabel*		            m_pSTRLabel;							// STR
	CRYLLabel*		            m_pSTRValueLabel;						// STR 값
	CRYLLabel*		            m_pDEXLabel;							// DEX
	CRYLLabel*		            m_pDEXValueLabel;						// DEX 값
	CRYLLabel*		            m_pCONLabel;							// CON
	CRYLLabel*		            m_pCONValueLabel;						// CON 값
	CRYLLabel*		            m_pINTLabel;							// INT
	CRYLLabel*		            m_pINTValueLabel;						// INT 값
	CRYLLabel*		            m_pWISLabel;							// WIS
	CRYLLabel*		            m_pWISValueLabel;						// WIS 값

    CRYLLabel*		            m_pTipString1Label;						// '다음 레벨에'
    CRYLLabel*		            m_pTipString2Label;                     // 보너스 1
    CRYLLabel*		            m_pTipString3Label;                     // 보너스 2
    CRYLLabel*		            m_pTipString4Label;                     // '보너스가 있습니다.'

    CRYLLabel*		            m_pBonusPointLabel;					    // 보너스 포인트
	CRYLLabel*		            m_pBonusPointValueLabel;				// 보너스 포인트 값

    CRYLLabel*                  m_pRightHandLabel;                      // 오른손
    CRYLLabel*                  m_pLeftHandLabel;                       // 왼손

    CRYLLabel*                  m_pDamageLabel;                         // 데미지
    CRYLLabel*                  m_pRightDamageValueLabel;               // 오른손 데미지 값
    CRYLLabel*                  m_pLeftDamageValueLabel;                // 왼손 데미지 값

    CRYLLabel*                  m_pHitRationLabel;                      // 명중률
    CRYLLabel*                  m_pRightHitRationValueLabel;            // 오른손 명중률 값
    CRYLLabel*                  m_pLeftHitRationValueLabel;             // 왼손 명중률 값

    CRYLLabel*                  m_pCriticalLabel;                       // 크리티컬
    CRYLLabel*                  m_pRightCriticalValueLabel;             // 오른손 크리티컬 값
    CRYLLabel*                  m_pLeftCriticalValueLabel;              // 왼손 크리티컬 값

    CRYLLabel*                  m_pDefensivePowerLabel;                 // 방어력
    CRYLLabel*                  m_pDefensivePowerValueLabel;            // 방어 값
    CRYLLabel*                  m_pDefensiveRateValueLabel;             // 방어률 값

    CRYLLabel*                  m_pEvasionLabel;                        // 회피
    CRYLLabel*                  m_pEvasionValueLabel;                   // 회피 값

    CRYLLabel*                  m_pBlockPowerLabel;                     // 블록
    CRYLLabel*                  m_pBlockPowerValueLabel;                // 블록 값
    CRYLLabel*                  m_pBlockRateValueLabel;                 // 블록률 값

    CRYLLabel*                  m_pMagicPowerLabel;                     // 마법력
    CRYLLabel*                  m_pMagicPowerValueLabel;                // 마법력 값
    CRYLLabel*                  m_pMagicResistLabel;                    // 마법저항
    CRYLLabel*                  m_pMagicResistValueLabel;               // 마법저항 값

	CRYLButton*			        m_pStrPointIncBtn;                      // STR 포인트 증가,감소 버튼
	CRYLButton*			        m_pDexPointIncBtn;                      // DEX 포인트 증가,감소 버튼
	CRYLButton*			        m_pConPointIncBtn;                      // CON 포인트 증가,감소 버튼
	CRYLButton*			        m_pIntPointIncBtn;                      // INT 포인트 증가,감소 버튼
	CRYLButton*			        m_pWisPointIncBtn;                      // WIS 포인트 증가,감소 버튼

public:
                                CRYLStatusInfoPanel( CGMUICustomDlg *pParentDlg );
    virtual                     ~CRYLStatusInfoPanel();

            VOID	            InitCtrl();
	        VOID	            FinalCtrl();
	
	        HRESULT		        Update();

	static  VOID                DlgClick( CGMUIObject* Sender, INT X, INT Y );

private:
            VOID                SetTipString();
};

#endif //__RYL_STATUS_INFO_PANEL_H__