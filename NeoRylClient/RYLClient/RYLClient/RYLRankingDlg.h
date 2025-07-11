//================================================================
//
//	Name : RYLQuestRankingDlg.h 
//	Desc : 퀘스트 오프닝 다이얼로그
//  Date : 2004. 01. 05
//
//================================================================
#ifndef __RYL_RANKINGDLG_H__
#define __RYL_RANKINGDLG_H__

#include "RYLDialog.h"
#include <vector>

class CRYLButton ;
class CRYLStringGrid ;
class CRYLLabel ;
class CRYLComboBox ;
struct RankingNode ;


namespace RYLRANKINGDLG
{
	const DWORD		PREVRANK_BUTTON		= 0x00000000 ;
	const DWORD		NEXTRANK_BUTTON		= 0x00000001 ;
	const DWORD		RANKING_COMBO		= 0x00000002 ;
	const DWORD		RANKING_MENU		= 0x00000003 ;
	const DWORD		RANKING_LISTBOX		= 0x00000004 ;
} ;

class CRYLRankingDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pRankingDlgLabel ;				// 랭킹창
	CRYLLabel*			m_pServerNameLabel ;				// 서버이름
	CRYLLabel*			m_pServerNameValueLabel ;			// 서버이름 값
	CRYLLabel*			m_pCurrentRankingLabel ;			// 현재 랭킹
	CRYLLabel*			m_pCurrentRankingValueLabel ;		// 현재 랭킹값
	CRYLLabel*			m_pSortLabel ;						// 정렬 방식
	
	CRYLLabel*			m_pRankingLabel ;					// 순위
	CRYLLabel*			m_pClassLabel ;						// 클래스
	CRYLLabel*			m_pNameLabel ;						// 이름
	CRYLLabel*			m_pPointLabel ;						// 포인트
	CRYLLabel*			m_pLebelLabel ;						// 레벨

	CRYLButton*			m_pPrevButton ;						// 이전 페이지
	CRYLButton*			m_pNextButton ;						// 다음 페이지
	CRYLStringGrid*		m_pRankingStrGrid ;					// 랭킹 리스트
	CRYLComboBox*		m_pClassCBBox ;						// 클래스 콤보박스

	unsigned char		m_cCurrentPage ;					// 현재 페이지
	INT					m_iClass ;							// 선택중인 클래스

public :
	// TODO : 하나의 벡터로 관리해도 될 듯 합니다. 
	//	현재 뿌리는 페이지만 가지는 인스턴스라는 개념으로 말이죠. (2004-02-09 로딘)

	std::vector<RankingNode>			m_vecAll ;			// 전체 리스트
	std::vector<RankingNode>			m_vecDefender ;		// 디펜더 리스트
	std::vector<RankingNode>			m_vecWarrior ;		// 워리어 리스트
	std::vector<RankingNode>			m_vecAssassin ;		// 어쎄신 리스트
	std::vector<RankingNode>			m_vecArcher ;		// 아처 리스트
	std::vector<RankingNode>			m_vecSorcerer ;		// 소서러 리스트
	std::vector<RankingNode>			m_vecEnchanter ;	// 인첸터 리스트
	std::vector<RankingNode>			m_vecPriest ;		// 프리스트 리스트
	std::vector<RankingNode>			m_vecCleric ;		// 클래릭 리스트

	std::vector<RankingNode>			m_vecTemplar ;		// 템플러 리스트
	std::vector<RankingNode>			m_vecAttacker ;		// 어태커 리스트
	std::vector<RankingNode>			m_vecGunner ;		// 거너	리스트
	std::vector<RankingNode>			m_vecRuneOff ;		// 룬오프 리스트
	std::vector<RankingNode>			m_vecLifeOff ;		// 라이프오프 리스트
	std::vector<RankingNode>			m_vecShadowOff ;	// 쉐도우오프 리스트
	
public :
	CRYLRankingDlg() ;
	virtual ~CRYLRankingDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } 
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	InsertRanking( INT iClass, INT iPage, INT iNum, RankingNode* lpRankingNode ) ;
	VOID	InsertDataListbox( INT iClass ) ;

	unsigned char	GetCurrentPage( VOID ) { return m_cCurrentPage ; }
	VOID			SetCurrentPage( unsigned char cPage ) { m_cCurrentPage = cPage ; }
} ;

#endif // __RYL_RANKINGDLG_H__