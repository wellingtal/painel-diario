//================================================================
//
//	Name : RYLTaxrateDlg.h 
//	Desc : 세율변경 다이얼로그
//  Date : 2004. 05. 28
//
//================================================================
#ifndef __RYL_TAXRATEDLG_H__
#define __RYL_TAXRATEDLG_H__

#include "RYLDialog.h"

namespace RYLTAXRATEDLG
{
	const DWORD		TAX_EDIT			= 0x00000000 ;// 세율 에디트 박스
	const DWORD		OK_BUTTON			= 0x00000001 ;// 확인 버튼
	const DWORD		CANCEL_BUTTON		= 0x00000002 ;// 취소 버튼
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLTaxrateDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pTaxrateLabel ;			// 세율변경
	CRYLLabel*			m_pCurrentTaxLabel ;		// 현제 세율은 000%입니다.
	CRYLLabel*			m_pChangeTaxLabel ;			// 세율을 변경 하겠습니까? ( 0 - 100% )
	CRYLLabel*			m_pTaxpercentLabel ;		// %
	CRYLEdit*			m_pTaxEdit ;				// 세율 에디트 박스
	CRYLButton*			m_pOKButton ;				// 확인
	CRYLButton*			m_pCancelButton ;			// 취소
	
public :
	CRYLTaxrateDlg() ;
	virtual ~CRYLTaxrateDlg() ;

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
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 
} ;

#endif // __RYL_TAXRATEDLG_H__