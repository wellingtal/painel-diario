//================================================================
//
//	Name : RYLCampShowTaxInfoDlg.h 
//	Desc : 요새 상점, 채굴기의 성의 축복 및 세율 정보 확인 창
//  Date : 2005. 05. 18
//
//================================================================
#ifndef __RYL_CAMPSHOWTAXINFODLG_H__
#define __RYL_CAMPSHOWTAXINFODLG_H__

#include "RYLDialog.h"

namespace RYLCAMPSHOWTAXINFODLG
{
	const DWORD		OK_BUTTON				= 0x00000000 ;	// 확인 버튼
};

class CRYLButton ;
class CRYLLabel ;

class CRYLCampShowTaxInfoDlg : public CRYLDialog
{
private :

	enum eCampType
	{
		E_NONE			= 0,
		E_CAMP_SHOP		= 1, 
		E_MINING_CAMP	= 2
	};

	CRYLLabel*		m_pTitleLabel ;				// 성의 축복 / 세율 정보
	CRYLLabel*		m_pCastleLabel ;			// 축복을 주는 성
	CRYLLabel*		m_pCastleValue ;			// 성 이름 (칭호 + 성이름)
	CRYLLabel*		m_pCastleBonusLabel ;		// 축복 보너스
	CRYLLabel*		m_pCastleBonusValue ;		// 축복 보너스 값
	CRYLLabel*		m_pCastleTaxLabel ;			// 요새 상점 수익 세율 (채굴기 광물 세율)
	CRYLLabel*		m_pCastleTaxValue ;			// 세율 값
	CRYLButton*		m_pOKButton ;				// 확인 버튼

	vector3			m_vecPos ;					// 요새 위치
	eCampType		m_eCampType ;				// 요새 타입

public :
	CRYLCampShowTaxInfoDlg() ;
	virtual ~CRYLCampShowTaxInfoDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	VOID	SetCampShop( const vector3& vecPos ) ;
	VOID	SetMiningCamp( const vector3& vecPos ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_CAMPSHOWTAXINFODLG_H__