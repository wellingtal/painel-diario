//================================================================
//
//	Name : RYLTaxpresentDlg.h 
//	Desc : 세금 현황 다이얼로그
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_TAXPRESENTDLG_H__
#define __RYL_TAXPRESENTDLG_H__

#include "RYLDialog.h"

namespace RYLTAXPRESENTDLG
{
	const DWORD		OK_BUTTON				= 0x00000003 ;// 확인 버튼
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLTaxpresentDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pTaxpresentLabel ;			// 세금 현황
	CRYLLabel*			m_pTaxLabel ;					// 세율
	CRYLLabel*			m_pTaxValueLabel ;				// 세율
	CRYLLabel*			m_pTaxLevyPresentLabel ;		// 세금 징수 현황
	CRYLLabel*			m_pTaxLevyPresentValueLabel ;	// 세금 징수 현황
	CRYLLabel*			m_pTaxLevyExpirationLabel ;		// 세금 징구 만기
	CRYLLabel*			m_pTaxLevyExpirationValueLabel ;// 세금 징구 만기
	CRYLButton*			m_pOKButton ;					// 확인 버튼	
	
public :
	CRYLTaxpresentDlg() ;
	virtual ~CRYLTaxpresentDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	
	VOID	SetData( unsigned long dwCurrentTax,
		             unsigned long dwTaxLevyPresent,
					 unsigned long dwTaxLevyExpiration ) ;
} ;

#endif // __RYL_TAXPRESENTDLG_H__