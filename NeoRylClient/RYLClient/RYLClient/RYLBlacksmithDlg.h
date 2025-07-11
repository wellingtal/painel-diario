//================================================================
//
//	Name : RYLBlacksmithDlg.h 
//	Desc : 대장장이 다이얼로그
//  Date : 2004. 01. 07
//
//================================================================
#ifndef __RYL_BLACKSMITHDLG_H__
#define __RYL_BLACKSMITHDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	// Added by ClassView

class CRYLButton ;
class CRYLLabel ;

namespace RYLBLACKSMITHDLG
{
	const DWORD		REINFORCEMENT_BUTTON	= 0x00000000 ;
	const DWORD		REPAIR_BUTTON			= 0x00000001 ;
	const DWORD		ALLREPAIR_BUTTON		= 0x00000002 ;
} ;

class CRYLBlacksmithDlg : public CRYLDialog
{
private :

	CRYLButton*			m_pReinforcementButton ;			// 강화
	CRYLButton*			m_pRepairButton ;					// 수리하기
	CRYLButton*			m_pAllRepairButton ;				// 모두 수리하기
	CRYLLabel*			m_pRefineryLabel ;					// 제련
	CRYLLabel*			m_pReinforcementWeaponLabel ;		// 강화시킬 장비
	CRYLLabel*			m_pMetalLabel ;						// 금속
	CRYLLabel*			m_pGoldLabel ;						// 골드

	CRYLLabel*			m_pResmeltTax ;						// 제련 세율
	CRYLLabel*			m_pRepairTax ;						// 수리 세율
	CRYLLabel*			m_pResmeltTaxValue ;				// 제련 세율 값
	CRYLLabel*			m_pRepairTaxValue ;					// 수리 세율 값

	DWORD				m_dwUpgradeGold ;
	DWORD				m_dwUpgradeResult ;
	

public :

	enum Const
	{
		// edith 2008.02.27 제련,옵션이식 레벨 제한을 제거
		UPGRADE_LIMIT_LEVEL		= 1,	// 제련 제한 레벨 40
		GRAFT_LIMIT_LEVEL		= 1		// 옵션 이식 제한 레벨 20
	};

	CRYLBlacksmithDlg() ;
	virtual ~CRYLBlacksmithDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	ReinforcementButton() ;
	VOID	RepairButton() ;		
	VOID	AllRepairButton() ;	
} ;

#endif // __RYL_BLACKSMITHDLG_H__