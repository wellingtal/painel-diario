//================================================================
//
//	Name : RYLTradeFrameDlg.h 
//	Desc : 상점관련 다이얼로그
//  Date : 2004. 1. 2
//
//================================================================
#ifndef __RYL_NEW_TRADEDLG_H__
#define __RYL_NEW_TRADEDLG_H__

#include "RYLDialog.h"
#include "NPC/NPCList.h"
#include "..\\ItemInstance.h"	// Added by ClassView
#include <vector>


class CRYLTabButton;
class CRYLButton;
class CRYLLabel;
class CRYLImage;

namespace RYLNEWTRADEDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000;
	const DWORD		REPAIR_BUTTON			= 0x00000001;
	const DWORD		ALLREPAIR_BUTTON		= 0x00000002;
    const DWORD     OPTION_REFRESH          = 0x00000003;
	const DWORD		PREV_BUTTON				= 0x00000004;
	const DWORD		NEXT_BUTTON				= 0x00000005;
};

#ifdef __NEWNPCSCRIPT
// 상점창에서 Page 하나..
typedef struct SingleTradePage
{
	vector<Item::CItem*>                m_vecEquipmentItem;
	map<unsigned short, unsigned char>  m_mapEquipmentItemGrade;
	bool                                m_bSpaceStatus[12][8];         // 각 상점 페이지의 슬롯( 공간? ) 정보
} SingleTradePage, *LPSingleTradePage;

// 상점창에서 탭 하나..
typedef struct SingleTradeTab
{
	vector<SingleTradePage*>            m_vecTradePage;
} SingleTradeTab, *LPSingleTradeTab;

// 상점창 하나..
typedef struct SingleTradeWindow
{
	vector<SingleTradeTab*>              m_vecTradeTab;
} SingleTradeWindow, *LPSingleTradeWindow;
#endif

class CRYLNewTradeDlg : public CRYLDialog
{
private :

	DWORD				m_dwTickCount;

	CRYLTabButton*		m_pTabButton;
    CRYLButton*         m_pOptionRefreshButton;                 // 랜덤 옵션 리프레쉬
	CRYLButton*			m_pRepairButton;						// 수리 버튼
	CRYLButton*			m_pAllRepairButton;					    // 모두 수리 버튼
	CRYLButton*			m_pPrevButton;							// 페이즈 이동 버튼
	CRYLButton*			m_pNextButton;							// 페이즈 이동 버튼
	CRYLLabel*			m_pPageLabel;							// 페이지 값
	CRYLLabel*			m_pServiceMedalLabel;					// 공헌 매달
	CRYLLabel*			m_pServiceMedalValueLabel;				// 공헌 매달 값

	// 성이 들어갈 경우 성의 세율 표시
	CRYLLabel*			m_pTradeTax ;							// 거래 세율
	CRYLLabel*			m_pRepairTax ;							// 수리 세율
	CRYLLabel*			m_pTradeTaxValue ;						// 거래 세율 값
	CRYLLabel*			m_pRepairTaxValue ;						// 수리 세율 값

	std::vector<CItemInstance *> m_lstTradeItem;
	CItemInstance*		m_lpBuyItem;

	CRYLImage*			m_pImage1;
	CRYLImage*			m_pImage2;

	DWORD				m_dwBuyResult;
	DWORD				m_dwSellResult;
	DWORD				m_dwAutoSellResult;

	DWORD				m_dwKindTitle;
    DWORD               m_dwTradeWindowOrder;                   // NPC가 갖고 있는 상점창 중 몇 번째 상점창인지..
	DWORD				m_dwTradeTabOrder;                      // 현재 선택된 Tab
	DWORD				m_dwItemNumber;

	BOOL				m_bBlackMarket;
	BOOL				m_bMedalMarket;
	BOOL				m_bSkillCouponMarket;
	BOOL				m_bRandomOptionMarket;					// 옵션이 랜덤하게 붙는 아이템 (EquipItem의 경우)

	DWORD				m_dwAllRepairResult;
	DWORD				m_dwNPCID;

	POINT				m_ptStartPos;

#ifdef __NEWNPCSCRIPT
		// 아이템( 상점 관련.. )
		LPSingleTradeWindow m_lpTradeWindow; // 상점창 벡터.
		LPContainerNode m_lpContainer;
#else
	CHAR				m_TabTitle[ 16 ][ 4 ][ 16 ];
#endif

    int                 m_nCurPage[4];						    // Page 현재 값 창이 열릴때 마다 초기화, 탭별 이동시에는 저장

    unsigned char       m_ucCurTradeDlgIndex;
	

protected:

	// 아이템 그레이드 변경
	VOID	ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, CItemInstance* pItemInstance);		

	// 랜덤 상점에서 아이템을 사면 바꿔줘야 한다.
	VOID	SetBuyFlag( CItemInstance* pItemInstance );											

	VOID	SetPageLabel();


public :

	CRYLNewTradeDlg();
	virtual ~CRYLNewTradeDlg();

	VOID InitCtrl();
	VOID FinalCtrl();

	virtual VOID Show( CGMUIObject* Sender );
	virtual VOID Hide( CGMUIObject* Sender );

	virtual VOID GMUIPaint( INT xx, INT yy );

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y );

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton; };
	BOOL				IsBlackMarket()			{ return m_bBlackMarket; };
	BOOL				IsMedalMarket()			{ return m_bMedalMarket; };
	BOOL				IsRandomMarket()		{ return m_bRandomOptionMarket; };

	VOID	SetTradeTab( INT iTab );

	VOID	NextPage();
	VOID	PrevPage();
	VOID	InitValue();

#ifdef __NEWNPCSCRIPT
	VOID	AddTradeEquipItemWindow();
#else
	VOID	AddTradeEquipItemWindow( NPCNode* lpNpcNode, unsigned char ucTradeDlgIndex );
#endif
	VOID	DeleteTradeItem();
#ifdef __NEWNPCSCRIPT
	VOID	InitTrade( DWORD dwNPCID, unsigned long dwContainerID );
	VOID	InitTradeWindow(void);
	VOID	DestroyTradeWindow(void);
	VOID    SetItemToTradeWindow( NPCNode *lpNPCNode );
	VOID	ClearTradeWindow(void);
	VOID	ChangeEquipItemGradeOption(NPCNode *lpNPCNode);
#else
	VOID	InitTrade( DWORD dwNPCID, unsigned char ucTradeDlgIndex, unsigned short usPopupCapacity );
	VOID	ChangeTabTitle( unsigned short usPopupCapacity );
#endif
    VOID    RefreshRandomOption();

	VOID	AllRepair();
	VOID	UpdateAllRepair( CRYLGameData* pGame );	
};

#endif // __RYL_HUMANINVENTORYFRAMEDLG_H__