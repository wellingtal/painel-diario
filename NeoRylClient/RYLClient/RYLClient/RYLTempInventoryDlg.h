//================================================================
//
//	Name : RYLTempInventoryDlg.h 
//	Desc : 아이템 보상 판매 다이얼로그
//  Date : 2004. 07. 21
//
//================================================================
#ifndef __RYL_TEMPINVENTORYDLG_H__
#define __RYL_TEMPINVENTORYDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	


namespace RYLTEMPINVENTORYDLG
{
	
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;

class CRYLTempInventoryDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pDlgLabel;								// 다이얼로그 라벨

	CRYLLabel*			m_pDescriptionLabel1 ;						// 설명 첫째줄
	CRYLLabel*			m_pDescriptionLabel2 ;						// 설명 둘째줄
	CRYLLabel*			m_pDescriptionLabel3 ;						// 설명 셋째줄
	CRYLLabel*			m_pDescriptionLabel4 ;						// 설명 넷째줄
	CRYLLabel*			m_pDescriptionLabel5 ;						// 설명 다섯째줄

	CRYLLabel*			m_pRemainingLabel ;							// 잔여 아이템 수 ?? / ??
	CRYLLabel*			m_pLimitTimeLabel ;							// 제한 시간
	CRYLLabel*			m_pLimitTimeValueLabel ;


public :
	CRYLTempInventoryDlg() ;
	virtual ~CRYLTempInventoryDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
    virtual HRESULT		Update() ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID	CheckTime();
} ;

#endif // __RYL_TEMPINVENTORYDLG_H__