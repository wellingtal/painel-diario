//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCharacterInfoPanel.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: ĳ���� ���� �г�
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
    CRYLLabel*		            m_pPanelNameLabel;					    // �г� �̸�	
	CRYLLabel*		            m_pKillStreak;	
    CRYLLabel*		            m_pNameLabel;							// �̸�	
	CRYLLabel*		            m_pNameValueLabel;						// �̸� ��
	CRYLLabel*		            m_pClassLabel;							// Ŭ����
	CRYLLabel*		            m_pClassValueLabel;					    // Ŭ���� ��
    CRYLLabel*		            m_pLevelLabel;							// ����
	CRYLLabel*		            m_pLevelValueLabel;					    // ���� ��
    CRYLLabel*		            m_pFameLabel;							// ��
	CRYLLabel*		            m_pFameValueLabel;						// �� ��
	CRYLLabel*		            m_pServiceMedalLabel;					// ���� �޴�
	CRYLLabel*		            m_pServiceMedalValueLabel;				// ���� �޴� ��
	CRYLLabel*		            m_pGuildLabel;							// ���
	CRYLLabel*		            m_pGuildValueLabel;					    // ��� ��     
	CRYLLabel*		            m_pNationalityLabel;					// ����
	CRYLLabel*		            m_pNationalityValueLabel;				// ���� ��
                                                                                 
    CRYLSpriteEx*		        m_pFameMarkSprite;                      // �����
    CRYLSpriteEx*		        m_pGuildMarkSprite;                     // ��� ��ũ
    CRYLSpriteEx*		        m_pNationMarkSprite;                    // ���� ��ũ

    CRYLLabel*		            m_pHPLabel;							    // HP
    CRYLLabel*		            m_pHPValueLabel;                        // HP ��
	CRYLProgressBar*		    m_pHPProgressBar;						// HP Ȯ�����α׷�����
	CRYLLabel*		            m_pMPLabel;							    // MP
    CRYLLabel*		            m_pMPValueLabel;						// MP ��
	CRYLProgressBar*		    m_pMPProgressBar;						// MP Ȯ�����α׷�����
    CRYLLabel*		            m_pEXPLabel;							// EXP
    CRYLLabel*		            m_pEXPValueLabel;						// EXP
    CRYLProgressBar*	        m_pEXPProgressBar;                      // EXP Ȯ�����α׷�����

    CRYLButton*		            m_pRankingButton;						// ��ŷ ����
	CRYLButton*		            m_pHPMPButton;							// HP/MP â

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