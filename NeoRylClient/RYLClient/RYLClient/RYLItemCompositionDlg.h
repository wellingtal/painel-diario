//================================================================
//
//	Name : RYLItemCompositionDlg.h 
//	Desc : 아이템 옵션 이식 시트템 다이얼로그
//  Date : 2004. 06. 04
//
//================================================================
#ifndef __RYL_ITEMCOMPOSITIONDLG_H__
#define __RYL_ITEMCOMPOSITIONDLG_H__

#include "RYLDialog.h"
#include "GMUICustomPanel.h"
#include "..\\ItemInstance.h"	


namespace RYLITEMCOMPOSITIONDLG
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
	const DWORD		MEMBER_BASE_STRINGGRID			= 0x00000002 ;
	const DWORD		MEMBER_OFFERING_STRINGGRID		= 0x00000003 ;
	const DWORD		MEMBER_COMPOSITION_STRINGGRID	= 0x00000004 ;

	enum	ITEMDATA
	{
		ITEMDEFENSIVEPOWER = 0,				// 아이템 방어력
		ITEMMAGICPOWER,						// 아이템 마법력
		ITEMMINDAMAGE,						// 아이템 최소공격력
		ITEMMAXDAMAGE,						// 아이템 최대공격력
		ITEMHITRATIO,						// 아이템 명중
		ITEMMPMAX,							// 아이템 MP MAX
		ITEMMPREGEN,						// 아이템 MP 회복
		ITEMCRITCAL							// 아이템 크리티컬
	};

	enum	ITEMTYPE
	{
		ITEMTYPE_ARMOUR,
	};
} ;

namespace RYLITEMCOMPO1PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
} ;

namespace RYLITEMCOMPO2PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		PREV_BUTTON						= 0x00000001 ;
} ;


namespace RYLITEMCOMPO3PANEL
{
	const DWORD		NEXT_BUTTON						= 0x00000000 ;
	const DWORD		PREV_BUTTON						= 0x00000001 ;
} ;

namespace RYLITEMCOMPO4PANEL
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
} ;


class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;
class CRYLStringGrid ;
class CRYLPicture;

class CRYLItemCompositionDlg : public CRYLDialog
{
private :

	DWORD				m_dwPhaseState ;							// 현재 phase 0~3
	BOOL				m_bIsEnd;									// 결과가 나왔는지..


	CGMUICustomPanel*	m_pPhase1Panel ;							// Phase 1 판넬
	CGMUICustomPanel*	m_pPhase2Panel ;							// Phase 2 판넬
	CGMUICustomPanel*	m_pPhase4Panel ;							// Phase 4 판넬

	CRYLLabel*			m_pDlgLabel ;								// 다이얼로그 라벨
	char				m_cAttribute ;								// 이식할 옵션

	short				m_arrOfferingItemValue[ 8 ] ;				// 제물 아이템 값 ( 방어력, 마력, 최소 공격력, .... )
	VOID				SetPhase();

public :

	VOID		NextPhase();					// 한단계 다음 단계로. 4 단계에서 쓰면 OK()로. 
	VOID		PrevPhase();					// 한단계 전 페이즈로. 1 단계에서 쓰면 Cancel()로
	VOID		Cancel();						// 다이얼로그를 닫고 페이즈를 1 단계로!
	VOID		OK();							// 이식 시도 하고 다이얼로그는 그대로. 페이즈 변함 없음
	char		GetOption(){ return m_cAttribute; };
	VOID		SetOption( int nSelect ){ m_cAttribute = nSelect ; };
	VOID		SetPhaseState2First();
	CRYLItemCompositionDlg() ;
	virtual ~CRYLItemCompositionDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT			FinalRender(  LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK; } ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
    virtual HRESULT		Update() ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual VOID		SetVisible( BOOL bVisible ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;


class CRYLItemCompo1Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pCancelButton ;							// 취소 버튼
	CRYLButton*			m_pNextButton ;								// 다음 버튼

	CRYLLabel*			m_pPhaseLabel ;								// 페이즈 라벨
	CRYLLabel*			m_pDescription1Label ;						// 설명 첫째줄
	CRYLLabel*			m_pDescription2Label ;						// 설명 둘째줄

	CRYLPicture*		m_pItemSlotPicture ;						// 아이템 슬롯

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	CRYLItemCompo1Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo1Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo2Panel : public CGMUICustomPanel
{
private :
	
	CRYLButton*			m_pNextButton ;								// 다음 버튼

	CRYLLabel*			m_pPhaseLabel ;								// 페이즈 라벨

	CRYLLabel*			m_pCostLabel ;								// 옵션 비용 값
	CRYLLabel*			m_pOptionLabel ;							// 옵션 선택
	CRYLLabel*			m_pOptionOptLabel;							// 옵션
	CRYLLabel*			m_pOptionOriginLabel ;						// 원본
	CRYLLabel*			m_pOptionBonusLabel;						// 보너스

	CRYLPicture*		m_pItemSlotPicture ;						// 아이템 슬롯

	CRYLStringGrid*		m_pOfferingStringGrid ;						// 제물 아이템 리스트	
	int					m_nOfferingCount;
	int					m_nOfferingSelected;
	int					m_aryOffering[8];
	bool				m_aryCanComposition[8];						// 이식 가능 여부

	unsigned long		m_dwCheckLimitResult;						// 제한 초과 옵션에 대한 메시지 박스 결과
	
public :

	CRYLButton*			m_pPrevButton ;								// 이전 버튼
	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	VOID	InitStringGrid( CItemInstance * pItem );
	int		SelectStringGrid();

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo2Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo2Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo3Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pPrevButton ;								// 이전 버튼
	CRYLButton*			m_pNextButton ;								// 다음 버튼
	

	CRYLLabel*			m_pPhaseLabel ;								// 페이즈 라벨
	CRYLLabel*			m_pDescription1Label ;						// 설명 첫째줄
	CRYLLabel*			m_pDescription2Label ;						// 설명 둘째줄
	CRYLLabel*			m_pDescription3Label ;						// 설명 셋째줄
	CRYLLabel*			m_pDescription4Label ;						// 설명 넷째줄

	CRYLStringGrid*		m_pOfferingStringGrid ;						// 제물 아이템 리스트	
	int					m_nOfferingCount;
	int					m_nOfferingSelected;
	int					m_aryOffering[8];
	bool				m_aryCanComposition[8];						// 이식 가능 여부

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo3Panel( CGMUICustomDlg *pParentDlg ) ;
	VOID	InitStringGrid();
	int		SelectStringGrid();
	virtual ~CRYLItemCompo3Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

class CRYLItemCompo4Panel : public CGMUICustomPanel
{
private :

	CRYLButton*			m_pOKButton ;						// 확인 버튼
	CRYLButton*			m_pCancelButton ;					// 취소 버튼

	CRYLLabel*			m_pPhaseLabel ;						// 페이즈 라벨
	CRYLLabel*			m_pDescription1Label ;				// 설명 첫째줄
	CRYLLabel*			m_pDescription2Label ;				// 설명 둘째줄
	CRYLLabel*			m_pDescription3Label ;				// 설명 셋째줄
	CRYLLabel*			m_pDescription4Label ;				// 설명 넷째줄
	CRYLLabel*			m_pDescription5Label ;				// 설명 다섯째줄

	
	CRYLPicture*		m_pItemSlotPicture ;				// 아이템 슬롯
	friend				CRYLItemCompositionDlg;

public :

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	VOID ResetText();

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	CRYLItemCompo4Panel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLItemCompo4Panel() ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
};

#endif // __RYL_ITEMCOMPOSITIONDLG_H__