/*
//================================================================
//
//	Name : RYLTradeFrameDlg.h 
//	Desc : �������� ���̾�α�
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
	CRYLButton*			m_pRepairButton ;						// ���� ��ư
	CRYLButton*			m_pAllRepairButton ;					// ��� ���� ��ư
	CRYLButton*			m_pPrevButton ;							// ������ �̵� ��ư
	CRYLButton*			m_pNextButton ;							// ������ �̵� ��ư
	CRYLLabel*			m_pPhaseLabel ;							// ���� �Ŵ�
	CRYLLabel*			m_pServiceMedalLabel ;					// ���� �Ŵ�
	CRYLLabel*			m_pServiceMedalValueLabel ;				// ���� �Ŵ� ��

	unsigned char		m_cRace ;
	int					m_aryCurPhase[4] ;						// phase ���� �� â�� ������ ���� �ʱ�ȭ, �Ǻ� �̵��ÿ��� ����
	int					m_aryMaxPhase[4] ;						// �ش� �ǿ����� ������ �ƽ� ��.

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
	BOOL				m_bRandomOptionMarket ;					// �ɼ��� �����ϰ� �ٴ� ������ (EquipItem�� ���)

	DWORD				m_dwAllRepairResult ;
	DWORD				m_dwNPCID;

	POINT				m_ptStartPos ;

	CHAR				m_TabTitle[ 14 ][ 4 ][ 16 ] ;
	BOOL				m_aryIsEquipItemChanged[ 4 ][ 500 ] ;

	// �ش� ���� �ʱ�ȭ �Ǿ����� ���� 
	// (�ʱ�ȭ �Ǿ��ٸ� �������� ���̻� ���� �� �ʿ� ����. �Ǻ��� ������ �׷��̵尡 ��� ���� ������ �̷� ������� ó��)
	BOOL				m_aryIsEquipItemInitialized[ 8 ] ;		
	

protected:

	// ������ �׷��̵� ����
	VOID	ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, CItemInstance* pItemInstance);		

	// ���� �������� �������� ��� �ٲ���� �Ѵ�.
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

	VOID	SetInitialRandomOpition();							// ������ ���� ���θ� ���� TRUE ��
} ;

#endif // __RYL_HUMANINVENTORYFRAMEDLG_H__
*/