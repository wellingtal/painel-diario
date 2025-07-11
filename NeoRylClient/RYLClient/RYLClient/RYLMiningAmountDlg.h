//=================================================================
//
// Name : RYLMiningAmountDlg.h
// Data : 2004. 12. 14
// Desc : Mining Amount Dialog Class, 광물 매립량 확인 다이얼로그
//
//=================================================================
#ifndef __RYL_MININGAMOUNTDLG_H__
#define __RYL_MININGAMOUNTDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <vector>

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

namespace RYLMININGAMOUNTDLG
{
	const DWORD		PREV_BUTTON				= 0x00000000 ;
	const DWORD		NEXT_BUTTON				= 0x00000001 ;
} ;


struct MineralVeinInfo;

 

class CRYLMiningAmountDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pPrevButton ;							// 페이즈 이동 버튼
	CRYLButton*			m_pNextButton ;							// 페이즈 이동 버튼
	CRYLLabel*			m_pPhaseLabel ;							// 페이즈 라벨
	CRYLLabel*			m_pMineNameLabel[8] ;					// 광물 이름
	CRYLLabel*			m_pMineCountLabel[8] ;					// 광물 수량
	CRYLLabel*			m_pTitleLabel ;							// 광물의 매입량 확인 ( 타이틀 )
	CRYLLabel*			m_pVeinNameLabel ;						// 광맥 이름
	CRYLLabel*			m_pDescription1 ;						// 설명 1
	CRYLLabel*			m_pDescription2 ;						// 설명 2
	CRYLLabel*			m_pDescription3 ;						// 설명 3
	CRYLLabel*			m_pDescription4 ;						// 설명 4
	std::vector< CItemInstance* >	m_vecMineIcon ;				// 광물 아이콘
	std::vector< int >	m_vecMineCnt;							// 광물 갯수


	int					m_nCurPhase ;							// phase 
	int					m_nMaxPhase ;							// 페이즈 맥스 값.

protected:
	VOID				NextPhase();
	VOID				PrevPhase();
	VOID				SetPhaseLabel();
	VOID				ClearItem();

public :
	CRYLMiningAmountDlg() ;
	virtual ~CRYLMiningAmountDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	SetItemIcon( );				//임시

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	VOID	UpdateMineral();

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };


	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

} ;

#endif //__RYL_MININGAMOUNTDLG_H__