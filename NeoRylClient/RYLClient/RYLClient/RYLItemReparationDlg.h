//================================================================
//
//	Name : RYLItemReparationDlg.h 
//	Desc : 아이템 보상 판매 다이얼로그
//  Date : 2004. 06. 21
//
//================================================================
#ifndef __RYL_ITEMREPARATIONDLG_H__
#define __RYL_ITEMREPARATIONDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	


namespace RYLITEMREPARATIONDLG
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
	
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;

class CRYLItemReparationDlg : public CRYLDialog
{
private :

	CRYLButton*			m_pOKButton ;					// 확인 버튼
	CRYLButton*			m_pCancelButton ;				// 취소 버튼

	CRYLLabel*			m_pDlgLabel;					// 다이얼로그 라벨
	CRYLLabel*			m_pSrcItemLabel;				// 원본 아이템 라벨
	CRYLLabel*			m_pDestItemLabel;				// 변환 아이템 라벨

	CRYLLabel*			m_pDescriptionLabel1 ;			// 설명 첫째줄
	CRYLLabel*			m_pDescriptionLabel2 ;			// 설명 둘째줄
	CRYLLabel*			m_pDescriptionLabel3 ;			// 설명 셋째줄

	CRYLLabel*			m_pCostValueLabel ;				// 비용 값
	CRYLLabel*			m_pCostLabel ;					// 비용

	long				m_lCompensationGold;			// 환불 금액
	unsigned char		m_cResultErr;					// 에러값


public :

	CRYLItemReparationDlg() ;
	virtual ~CRYLItemReparationDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	bool OK(void);

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_ITEMREPARATIONDLG_H__