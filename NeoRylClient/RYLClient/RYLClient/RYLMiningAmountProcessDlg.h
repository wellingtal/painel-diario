//=================================================================
//
// Name : RYLMiningAmountDlg.h
// Data : 2004. 12. 14
// Desc : Mining Amount Dialog Class, 광물 매립량 확인 다이얼로그
//
//=================================================================
#ifndef __RYL_MININGAMOUNTPROCESSDLG_H__
#define __RYL_MININGAMOUNTPROCESSDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <vector>

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

namespace RYLMININGAMOUNTPROCESSDLG
{
	const DWORD		CANCEL_BUTTON				= 0x00000000 ;
} ;



 

class CRYLMiningAmountProcessDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pCanceltButton ;						// 취소 버튼
	CRYLLabel*			m_pProcessLabel ;						// 프로세스 라벨
	CRYLLabel*			m_pMessageLabel1 ;						// 문구 첫째줄
	CRYLLabel*			m_pMessageLabel2 ;						// 문구 둘째줄
	CRYLLabel*			m_pMessageLabel3 ;						// 문구 셋째줄
	CRYLLabel*			m_pTitleLabel ;							// Title
    DWORD				m_ulTime;
	CItemInstance*		m_lpKitItem ;							// 키트


protected:
	
	BOOL				UpdateProcess();						// 5분이 넘으면 TRUE 리턴
	VOID				CheckMineCount();						// 해당땅에서의 광물 체크

public :
	CRYLMiningAmountProcessDlg() ;
	virtual ~CRYLMiningAmountProcessDlg() ;

	VOID	Cancel();
	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };


	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID		SetKitItem( CItemInstance* pItem ){ m_lpKitItem = pItem; };

} ;

#endif //__RYL_MININGAMOUNTPROCESSDLG_H__