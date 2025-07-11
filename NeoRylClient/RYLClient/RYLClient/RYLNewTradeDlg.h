//================================================================
//
//	Name : RYLTradeFrameDlg.h 
//	Desc : �������� ���̾�α�
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
// ����â���� Page �ϳ�..
typedef struct SingleTradePage
{
	vector<Item::CItem*>                m_vecEquipmentItem;
	map<unsigned short, unsigned char>  m_mapEquipmentItemGrade;
	bool                                m_bSpaceStatus[12][8];         // �� ���� �������� ����( ����? ) ����
} SingleTradePage, *LPSingleTradePage;

// ����â���� �� �ϳ�..
typedef struct SingleTradeTab
{
	vector<SingleTradePage*>            m_vecTradePage;
} SingleTradeTab, *LPSingleTradeTab;

// ����â �ϳ�..
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
    CRYLButton*         m_pOptionRefreshButton;                 // ���� �ɼ� ��������
	CRYLButton*			m_pRepairButton;						// ���� ��ư
	CRYLButton*			m_pAllRepairButton;					    // ��� ���� ��ư
	CRYLButton*			m_pPrevButton;							// ������ �̵� ��ư
	CRYLButton*			m_pNextButton;							// ������ �̵� ��ư
	CRYLLabel*			m_pPageLabel;							// ������ ��
	CRYLLabel*			m_pServiceMedalLabel;					// ���� �Ŵ�
	CRYLLabel*			m_pServiceMedalValueLabel;				// ���� �Ŵ� ��

	// ���� �� ��� ���� ���� ǥ��
	CRYLLabel*			m_pTradeTax ;							// �ŷ� ����
	CRYLLabel*			m_pRepairTax ;							// ���� ����
	CRYLLabel*			m_pTradeTaxValue ;						// �ŷ� ���� ��
	CRYLLabel*			m_pRepairTaxValue ;						// ���� ���� ��

	std::vector<CItemInstance *> m_lstTradeItem;
	CItemInstance*		m_lpBuyItem;

	CRYLImage*			m_pImage1;
	CRYLImage*			m_pImage2;

	DWORD				m_dwBuyResult;
	DWORD				m_dwSellResult;
	DWORD				m_dwAutoSellResult;

	DWORD				m_dwKindTitle;
    DWORD               m_dwTradeWindowOrder;                   // NPC�� ���� �ִ� ����â �� �� ��° ����â����..
	DWORD				m_dwTradeTabOrder;                      // ���� ���õ� Tab
	DWORD				m_dwItemNumber;

	BOOL				m_bBlackMarket;
	BOOL				m_bMedalMarket;
	BOOL				m_bSkillCouponMarket;
	BOOL				m_bRandomOptionMarket;					// �ɼ��� �����ϰ� �ٴ� ������ (EquipItem�� ���)

	DWORD				m_dwAllRepairResult;
	DWORD				m_dwNPCID;

	POINT				m_ptStartPos;

#ifdef __NEWNPCSCRIPT
		// ������( ���� ����.. )
		LPSingleTradeWindow m_lpTradeWindow; // ����â ����.
		LPContainerNode m_lpContainer;
#else
	CHAR				m_TabTitle[ 16 ][ 4 ][ 16 ];
#endif

    int                 m_nCurPage[4];						    // Page ���� �� â�� ������ ���� �ʱ�ȭ, �Ǻ� �̵��ÿ��� ����

    unsigned char       m_ucCurTradeDlgIndex;
	

protected:

	// ������ �׷��̵� ����
	VOID	ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, CItemInstance* pItemInstance);		

	// ���� �������� �������� ��� �ٲ���� �Ѵ�.
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