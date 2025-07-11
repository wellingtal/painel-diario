//================================================================
//
//	Name : RYLTacticsPanel.h 
//	Desc : 길드용병 관리
//  Date : 2005. 9. 1
//
//================================================================
#ifndef __RYL_GUILDTACTICSPANEL_H__
#define __RYL_GUILDTACTICSPANEL_H__

#include "GMUICustomPanel.h"
#include <d3d8.h>
#include <vector>
#include <map>
#include <Network/Packet/PacketStruct/GuildPacket.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ; 
class CRYLListBox ;

namespace GUILDTACTICSPAGEEX
{
	enum GuildTacticsPage
	{
		PAGE_CURRENT	= 0,
		PAGE_NEXT		= 1,
		PAGE_PREV		= 2
	} ;
} ;

namespace GUILDTACTICSSORT
{
	enum GuildTacticsSort
	{
		MAX_SORT_METHOD = 5
	} ;
} ;

namespace RYLGUILDTACTICSPANEL
{
	const DWORD		SORTPREV_BUTTON					= 0x00000000 ;
	const DWORD		SORTNEXT_BUTTON					= 0x00000001 ;
	const DWORD		PAGEPREV_BUTTON					= 0x00000002 ;
	const DWORD		PAGENEXT_BUTTON					= 0x00000003 ;
	const DWORD		TACTICSENTRY_BUTTON				= 0x00000006 ;
	const DWORD		TACTICSLEAVE_BUTTON				= 0x00000007 ;
	const DWORD		TACTICS_STRINGGRID				= 0x00000010 ;

//	const DWORD		GENERALGUILD_POPUP				= 0x00000012 ;
//	const DWORD		PROPOSALGUILD_POPUP				= 0x00000013 ;
} ;


class CRYLGuildTacticsPanel : public CGMUICustomPanel
{
public :
	CRYLLabel*			m_pGuildTacticsListLabel ;		// 길드 용병 리스트
	CRYLLabel*			m_pSortMethodLabel ;			// 정렬방식
	CRYLLabel*			m_pSortMethodValueLabel ;		// 정렬방식값
	CRYLButton*			m_pSortPrevButton ;				// 정렬방식 이전 방식
	CRYLButton*			m_pSortNextButton ;				// 정렬방식 다음 방식
	CRYLButton*			m_pPagePrevButton ;				// 멤버 이전 페이지
	CRYLButton*			m_pPageNextButton ;				// 멤버 다음 페이지

	CRYLLabel*			m_pRankLabel ;					// 서열
	CRYLLabel*			m_pNameLabel ;					// 이름
	CRYLLabel*			m_pLevelLabel;					// 레벨
	CRYLLabel*			m_pFameLabel ;					// 명성
	CRYLLabel*			m_pClassLabel;					// 클래스

	CRYLLabel*			m_pTacticsNameLabel ;			// 용병 이름
	CRYLLabel*			m_pTacticsNameValueLabel ;		// 용병 이름 값
	CRYLButton*			m_pTacticsEntryButton ;			// 용병 허가 버튼
	CRYLButton*			m_pTacticsLeaveButton ;			// 용병 퇴출/탈퇴 버튼

	CRYLStringGrid*		m_pTacticsStringGrid ;			// 길드 용병 리스트

	DWORD		m_dwStartTacticsInfo ;
	DWORD		m_dwServerPage ;
	DWORD		m_dwMaxTacticsInfo ;
	INT			m_iCurrentLine ;
	INT			m_iPageState ;

	DWORD		m_dwRstEntry ;
	DWORD		m_dwRstLeave ;
	DWORD		m_dwRstLeaveForChina ;
	
	std::vector< GuildMemberInfoNode* >		m_vecTacticsInfo ;
	std::map< INT, GuildMemberInfoNode* >	m_mapTacticsInfo ;

	GuildMemberInfoNode*		m_pTacticsInfo ;

	// 선택된 멤버의 정보
	CHAR		m_szCurrentName[ 32 ] ;

	CHAR		m_szSortMethodName[ 6 ][ 32 ] ;
	INT			m_iSortMethod[ 6 ] ;
	CHAR		m_szTacticsClass[ 24 ][ 16 ] ;
	CHAR		m_szTacticsTitle[ 6 ][ 32 ] ;
	INT			m_iCurrentSort ;

	INT			m_iCurrentSel ;
	INT			m_iTotalTacticsNum;						// 현재 길드원 인원수.


public :
	CRYLGuildTacticsPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGuildTacticsPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	std::vector< GuildMemberInfoNode* >::iterator		FindGuildTacticsIter( GuildMemberInfoNode* pMemberInfo ) ;
	std::vector< GuildMemberInfoNode* >::iterator		FindGuildTacticsIter( DWORD dwCID ) ;

	void	ClearList(void);

	// 정렬 방식 페이지 버튼
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// 멤버 리스트 페이지 버튼
	VOID	TacticsListPrev() ;
	VOID	TacticsListNext() ;

	// ----------------------------------------------------------------------------------------

	VOID	RequestGuildTacticsList() ;
	VOID	ProcessGuildTacticsList( unsigned char cSortCmd, 
									unsigned char cPage, 
									unsigned char cNodeNum, 
									unsigned char cTotalMemberNum,
									GuildMemberInfoNode* lpMemberNode ) ;

	INT		GetCurrentPageLine( INT bPageState ) ;
	HRESULT	UpdateGuildStringGrid() ;

	GuildMemberInfoNode*		GetTacticsInfo()		{ return m_pTacticsInfo ; }

	CHAR*	GetTitleString( UCHAR ucTitle )			{ return m_szTacticsTitle[ ucTitle ] ; }
	INT		GetTitle( DWORD dwCID ) ;

	VOID	SelectTacticsInfo()  ;

	HRESULT	UpdateGuildTacticsPanel() ;

	VOID	SetTactics( DWORD dwCID, unsigned char cTacticsFlag );
	unsigned char GetTactics(DWORD dwCID);

	VOID	TacticsEntryButtonEvent() ;
	VOID	TacticsLeaveButtonEvent() ;
	
	virtual VOID Hide( CGMUIObject* Sender ) ;

	INT		FindSortIndex( unsigned char cSortCmd ) ;

	VOID	UpdateGuildTacticsRight( DWORD dwCID, UCHAR ucTitle ) ;

	unsigned char GetClass( DWORD dwCID ) ;
	unsigned char GetLevel( DWORD dwCID ) ;
} ;

#endif //__RYL_GUILDTACTICSPANEL_H__
