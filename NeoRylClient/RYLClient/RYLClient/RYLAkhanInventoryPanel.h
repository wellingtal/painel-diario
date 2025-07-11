//================================================================
//
//	Name : RYLAkhanInventoryPanel.h 
//	Desc : ��ĭ �κ��丮 ���
//  Date : 2003. 01. 11
//
//================================================================
#ifndef __RYL_AKHANINVENTORYPANEL_H__
#define __RYL_AKHANINVENTORYPANEL_H__

#include "RYLInventoryPanel.h"
#include <d3d8.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLTabButton ;
class CRYLCheckBox ;

class CItemInstance ;

namespace RYLAKHANINVENTORYPANEL
{
	const DWORD		WEAPON_CHECKBOX1			= 0x00000000 ;
	const DWORD		WEAPON_CHECKBOX2			= 0x00000001 ;

	const DWORD		INVEN_CHECKBOX1				= 0x00000007 ;
	const DWORD		INVEN_CHECKBOX2				= 0x00000008 ;
	const DWORD		INVEN_CHECKBOX3				= 0x00000009 ;
	const DWORD		INVEN_CHECKBOX4				= 0x00000010 ;
	const DWORD		STREETSTALL_BUTTON			= 0x00000011 ;

	const DWORD		STRPOINTINC_BUTTON			= 0x00000012 ;
	const DWORD		DEXPOINTINC_BUTTON			= 0x00000013 ;
	const DWORD		CONPOINTINC_BUTTON			= 0x00000014 ;
	const DWORD		INTPOINTINC_BUTTON			= 0x00000015 ;
	const DWORD		WISPOINTINC_BUTTON			= 0x00000016 ;
} ;

class CRYLAkhanInventoryPanel : public CRYLInventoryPanel
{
private :
	CRYLTabButton*			m_pTabButton ;
	CRYLCheckBox*			m_pWeaponButton[ 2 ] ;
	CRYLCheckBox*			m_pInvenButton[ 4 ] ;

	CRYLButton*				m_pStreetStallButton ;			// ������ ��ư
	CRYLButton*			    m_pStrPointIncBtn;                      // STR ����Ʈ ����,���� ��ư
	CRYLButton*			    m_pDexPointIncBtn;                      // DEX ����Ʈ ����,���� ��ư
	CRYLButton*			    m_pConPointIncBtn;                      // CON ����Ʈ ����,���� ��ư
	CRYLButton*			    m_pIntPointIncBtn;                      // INT ����Ʈ ����,���� ��ư
	CRYLButton*			    m_pWisPointIncBtn;
	CRYLLabel*				m_pCurrentGoldLabel ;

	// by avalansa
	CRYLLabel*			    m_pSTRLabel;
	CRYLLabel*			    m_pDEXLabel;
	CRYLLabel*			    m_pCONLabel;
	CRYLLabel*			    m_pINTLabel;
	CRYLLabel*			    m_pWISLabel;

	CRYLLabel*		        m_pTipString1Label;						// '���� ������'
    CRYLLabel*		        m_pTipString2Label;                     // ���ʽ� 1
    CRYLLabel*		        m_pTipString3Label;                     // ���ʽ� 2
    CRYLLabel*		        m_pTipString4Label;                     // '���ʽ��
	CRYLLabel*			    m_pHPRECLabel;
	CRYLLabel*			    m_pMPRECLabel;
	CRYLLabel*			    m_pSPEEDLabel;
	CRYLLabel*			    m_pCOOLDOWNLabel;
	CRYLLabel*			    m_pFAMELabel;
	CRYLLabel*			    m_pMEDALSLabel;

	// LAST STATS

	CRYLLabel*              m_pATTLabel;
	CRYLLabel*              m_pDEFLabel;
	CRYLLabel*              m_pMagicPowerLabel;
	CRYLLabel*              m_pMagicResistLabel;

	CRYLLabel*              m_pEvasionLabel;
	CRYLLabel*              m_pHitRationLabel;
	CRYLLabel*              m_pCriticalLabel;
	CRYLLabel*              m_pBlockPowerLabel;

	CRYLLabel*		        m_pBonusPointLabel;					    
//	CRYLLabel*		        m_pBonusPointValueLabel;

	CRYLLabel*		        m_pNameLabel;

//	CRYLLabel*		        m_pExperienceLabel ;
//	CRYLLabel*		        m_pExperienceBalueLabel ;

//	CRYLLabel*		        m_pGuildLabel ;
//	CRYLLabel*		        m_pClassLabel ;
//	CRYLLabel*		        m_pLevelLabel ;
//	CRYLLabel*		        m_pNationLabel;
	CRYLLabel*		        m_pKillStreak;

	DWORD					m_dwBodyInven ;

	DWORD					m_dwRideFlag ;

public :
	CRYLAkhanInventoryPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLAkhanInventoryPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLCheckBox*		GetWeaponCheckButton( INT iIndex )		{ return m_pWeaponButton[ iIndex ] ; }
	CRYLCheckBox*		GetInvenCheckButton( INT iIndex )		{ return m_pInvenButton[ iIndex ] ; }

	VOID	UpdateRideFlag( INT iIndex );

	// üũ ��ư ������Ʈ
	VOID	UpdateWeaponCheckButton( INT iIndex ) ;
	VOID	UpdateInvenCheckButton( INT iIndex ) ;

	BOOL	SelectItem() ;

	// ��� ���� ����
	VOID	InventorySelectWeapon( INT iIndex ) ;

	VOID	UpdateInvenItemPos(VOID);

	ULONG	GetAllRepairMoney() ;
		private:
    VOID                SetTipString();
} ;

#endif //__RYL_AKHANINVENTORYPANEL_H__