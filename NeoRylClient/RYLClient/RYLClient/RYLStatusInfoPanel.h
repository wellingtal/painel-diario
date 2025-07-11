//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLStatusInfoPanel.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: ���� ���� �г�
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
	CRYLLabel*		            m_pSTRValueLabel;						// STR ��
	CRYLLabel*		            m_pDEXLabel;							// DEX
	CRYLLabel*		            m_pDEXValueLabel;						// DEX ��
	CRYLLabel*		            m_pCONLabel;							// CON
	CRYLLabel*		            m_pCONValueLabel;						// CON ��
	CRYLLabel*		            m_pINTLabel;							// INT
	CRYLLabel*		            m_pINTValueLabel;						// INT ��
	CRYLLabel*		            m_pWISLabel;							// WIS
	CRYLLabel*		            m_pWISValueLabel;						// WIS ��

    CRYLLabel*		            m_pTipString1Label;						// '���� ������'
    CRYLLabel*		            m_pTipString2Label;                     // ���ʽ� 1
    CRYLLabel*		            m_pTipString3Label;                     // ���ʽ� 2
    CRYLLabel*		            m_pTipString4Label;                     // '���ʽ��� �ֽ��ϴ�.'

    CRYLLabel*		            m_pBonusPointLabel;					    // ���ʽ� ����Ʈ
	CRYLLabel*		            m_pBonusPointValueLabel;				// ���ʽ� ����Ʈ ��

    CRYLLabel*                  m_pRightHandLabel;                      // ������
    CRYLLabel*                  m_pLeftHandLabel;                       // �޼�

    CRYLLabel*                  m_pDamageLabel;                         // ������
    CRYLLabel*                  m_pRightDamageValueLabel;               // ������ ������ ��
    CRYLLabel*                  m_pLeftDamageValueLabel;                // �޼� ������ ��

    CRYLLabel*                  m_pHitRationLabel;                      // ���߷�
    CRYLLabel*                  m_pRightHitRationValueLabel;            // ������ ���߷� ��
    CRYLLabel*                  m_pLeftHitRationValueLabel;             // �޼� ���߷� ��

    CRYLLabel*                  m_pCriticalLabel;                       // ũ��Ƽ��
    CRYLLabel*                  m_pRightCriticalValueLabel;             // ������ ũ��Ƽ�� ��
    CRYLLabel*                  m_pLeftCriticalValueLabel;              // �޼� ũ��Ƽ�� ��

    CRYLLabel*                  m_pDefensivePowerLabel;                 // ����
    CRYLLabel*                  m_pDefensivePowerValueLabel;            // ��� ��
    CRYLLabel*                  m_pDefensiveRateValueLabel;             // ���� ��

    CRYLLabel*                  m_pEvasionLabel;                        // ȸ��
    CRYLLabel*                  m_pEvasionValueLabel;                   // ȸ�� ��

    CRYLLabel*                  m_pBlockPowerLabel;                     // ���
    CRYLLabel*                  m_pBlockPowerValueLabel;                // ��� ��
    CRYLLabel*                  m_pBlockRateValueLabel;                 // ��Ϸ� ��

    CRYLLabel*                  m_pMagicPowerLabel;                     // ������
    CRYLLabel*                  m_pMagicPowerValueLabel;                // ������ ��
    CRYLLabel*                  m_pMagicResistLabel;                    // ��������
    CRYLLabel*                  m_pMagicResistValueLabel;               // �������� ��

	CRYLButton*			        m_pStrPointIncBtn;                      // STR ����Ʈ ����,���� ��ư
	CRYLButton*			        m_pDexPointIncBtn;                      // DEX ����Ʈ ����,���� ��ư
	CRYLButton*			        m_pConPointIncBtn;                      // CON ����Ʈ ����,���� ��ư
	CRYLButton*			        m_pIntPointIncBtn;                      // INT ����Ʈ ����,���� ��ư
	CRYLButton*			        m_pWisPointIncBtn;                      // WIS ����Ʈ ����,���� ��ư

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