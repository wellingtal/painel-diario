/*
//================================================================
//
//	Name : RYLTradeFrameDlg.h 
//	Desc : 상점관련 다이얼로그
//  Date : 2004. 1. 2
//
//================================================================
#ifndef __RYL_TRADEDLG_H__
#define __RYL_TRADEDLG_H__

#include "RYLDialog.h"
#include <Creature/NPC/NPCList.h>
#include "..\\ItemInstance.h"	// Added by ClassView
#include <vector>


class CRYLTabButton ;
class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;

namespace RYLTRADEDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
	const DWORD		REPAIR_BUTTON			= 0x00000001 ;
	const DWORD		ALLREPAIR_BUTTON		= 0x00000002 ;
	const DWORD		PREV_BUTTON				= 0x00000003 ;
	const DWORD		NEXT_BUTTON				= 0x00000004 ;
} ;

class CRYLTradeDlg : public CRYLDialog
{
private :

	CRYLTabButton*		m_pTabButton ;
	CRYLButton*			m_pRepairButton ;						// 수리 버튼
	CRYLButton*			m_pAllRepairButton ;					// 모두 수리 버튼
	CRYLButton*			m_pPrevButton ;							// 페이즈 이동 버튼
	CRYLButton*			m_pNextButton ;							// 페이즈 이동 버튼
	CRYLLabel*			m_pPhaseLabel ;							// 공헌 매달
	CRYLLabel*			m_pServiceMedalLabel ;					// 공헌 매달
	CRYLLabel*			m_pServiceMedalValueLabel ;				// 공헌 매달 값

	unsigned char		m_cRace ;
	int					m_aryCurPhase[4] ;						// phase 현재 값 창이 열릴때 마다 초기화, 탭별 이동시에는 저장
	int					m_aryMaxPhase[4] ;						// 해당 탭에서의 페이즈 맥스 값.

	std::vector<CItemInstance *> m_lstTradeItem ;
	CItemInstance*		m_lpBuyItem ;

	CRYLImage*			m_pImage1 ;
	CRYLImage*			m_pImage2 ;

	DWORD				m_dwBuyResult ;
	DWORD				m_dwSellResult ;
	DWORD				m_dwKindTitle ;
	DWORD				m_dwTradeTab ;
	DWORD				m_dwItemNumber ;

	BOOL				m_bBlackMarket ;
	BOOL				m_bMedalMarket ;
	BOOL				m_bSkillCouponMarket ;
	BOOL				m_bRandomOptionMarket ;					// 옵션이 랜덤하게 붙는 아이템 (EquipItem의 경우)

	DWORD				m_dwAllRepairResult ;
	DWORD				m_dwNPCID;

	POINT				m_ptStartPos ;

	CHAR				m_TabTitle[ 14 ][ 4 ][ 16 ] ;
	BOOL				m_aryIsEquipItemChanged[ 4 ][ 500 ] ;

	// 해당 탭이 초기화 되었는지 여부 
	// (초기화 되었다면 아이템을 더이상 생성 할 필요 없음. 탭별로 아이템 그레이드가 들어 오기 때문에 이런 방식으로 처리)
	BOOL				m_aryIsEquipItemInitialized[ 8 ] ;		
	

protected:

	// 아이템 그레이드 변경
	VOID	ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, CItemInstance* pItemInstance);		

	// 랜덤 상점에서 아이템을 사면 바꿔줘야 한다.
	VOID	SetBuyFlag( CItemInstance* pItemInstance );											

	VOID	SetPhaseLabel();


public :

	CRYLTradeDlg() ;
	virtual ~CRYLTradeDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton ; } ;
	BOOL				IsBlackMarket()			{ return m_bBlackMarket ; } ;
	BOOL				IsRandomMarket()		{ return m_bRandomOptionMarket; } ;

	VOID	SetTradeTab( INT iTab );

	VOID	NextPhase() ;
	VOID	PrevPhase() ;
	VOID	InitValue() ;

	VOID	InitTradeWindow( NPCNode* lpNpcNode, unsigned char cRace) ;
	VOID	AddTradeEquipItemWindow( NPCNode* lpNpcNode, unsigned char cRace, unsigned char cTab );
	VOID	DeleteTradeItem() ;
	VOID	InitTrade( DWORD dwNPCID, unsigned char cMenuIndex, BOOL bRandomOption = FALSE ) ;
	VOID	ChangeTabTitle( DWORD dwNPCID ) ;

	VOID	AllRepair() ;
	VOID	UpdateAllRepair( CRYLGameData* pGame ) ;

	VOID	SetInitialRandomOpition();							// 아이템 변경 여부를 전부 TRUE 로
} ;

#endif // __RYL_HUMANINVENTORYFRAMEDLG_H__
*/