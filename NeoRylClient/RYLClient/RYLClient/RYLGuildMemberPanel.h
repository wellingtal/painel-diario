//================================================================
//
//	Name : RYLMemberPanel.h 
//	Desc : 길드멤버 관리
//  Date : 2003. 12. 26
//
//================================================================
#ifndef __RYL_GUILDMEMBERPANEL_H__
#define __RYL_GUILDMEMBERPANEL_H__

#include "GMUICustomPanel.h"
#include <d3d8.h>
#include <vector>
#include <map>
#include <Network/Packet/PacketStruct/GuildPacket.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ; 
class CRYLListBox ;

namespace GUILDMEMBERPAGEEX
{
	enum GuildMemberPage
	{
		PAGE_CURRENT	= 0,
		PAGE_NEXT		= 1,
		PAGE_PREV		= 2
	} ;
} ;

namespace GUILDMEMBERSORT
{
	enum GuildMemberSort
	{
		MAX_SORT_METHOD = 9
	} ;
} ;

namespace RYLGUILDMEMBERPANEL
{
	const DWORD		SORTPREV_BUTTON					= 0x00000000 ;
	const DWORD		SORTNEXT_BUTTON					= 0x00000001 ;
	const DWORD		PAGEPREV_BUTTON					= 0x00000002 ;
	const DWORD		PAGENEXT_BUTTON					= 0x00000003 ;
	const DWORD		RIGHTSETTING_BUTTON				= 0x00000004 ;
	const DWORD		GUILDLEAVE_BUTTON				= 0x00000005 ;
	const DWORD		GUILDLEAVECANCEL_BUTTON			= 0x00000006 ;
	const DWORD		GUILDWARJOIN_BUTTON				= 0x00000007 ;
	const DWORD		GUILDWARLEAVE_BUTTON			= 0x00000008 ;
	const DWORD		MEMBER_STRINGGRID				= 0x00000009 ;
	const DWORD		MEMBERNAME_EDIT					= 0x00000010 ;

	const DWORD		GENERALGUILD_POPUP				= 0x00000011 ;
	const DWORD		PROPOSALGUILD_POPUP				= 0x00000012 ;
} ;


class CRYLGuildMemberPanel : public CGMUICustomPanel
{
public :
	CRYLLabel*			m_pGuildMemberListLabel ;		// 길드원 리스트
	CRYLLabel*			m_pSortMethodLabel ;			// 정렬방식
	CRYLLabel*			m_pSortMethodValueLabel ;		// 정렬방식값
	CRYLButton*			m_pSortPrevButton ;				// 정렬방식 이전 방식
	CRYLButton*			m_pSortNextButton ;				// 정렬방식 다음 방식
	CRYLButton*			m_pPagePrevButton ;				// 멤버 이전 페이지
	CRYLButton*			m_pPageNextButton ;				// 멤버 다음 페이지
	
	CRYLLabel*			m_pRankLabel ;					// 서열
	CRYLLabel*			m_pNameLabel ;					// 이름
	CRYLLabel*			m_pTitleLabel ;					// 직위
	CRYLLabel*			m_pLevelLabel;					// 레벨
	CRYLLabel*			m_pClassLabel;					// 클래스
	
	CRYLLabel*			m_pCurrentFameLabel ;			// 현재 명성
	CRYLLabel*			m_pCurrentFameValueLabel ;		// 현재 명성 값
	CRYLLabel*			m_pCurrentGoldLabel ;			// 소지 금액
	CRYLLabel*			m_pCurrentValueGoldLabel ;		// 소지 금액 값
	CRYLLabel*			m_pCurrentPositionLabel ;		// 현재 위치
	CRYLLabel*			m_pCurrentPositionValueLabel ;	// 현재 위치 값
	CRYLLabel*			m_pCurrentGuildWarFlagLabel ;	// 전쟁 참여
	CRYLLabel*			m_pCurrentGuildWarFlagValueLabel ;	// 전쟁 참여 값

	CRYLButton*			m_pRightSettingButton ;			// 권한 설정
	CRYLButton*			m_pGuildLeaveButton ;			// 길드 탈퇴	
	CRYLButton*			m_pGuildLeaveCanelButton ;		// 길드 탈퇴 취소
	CRYLButton*			m_pGuildWarJoin ;				// 길드 전쟁 참여
	CRYLButton*			m_pGuildWarLeave ;				// 길드 전쟁 불참

	CRYLStringGrid*		m_pMemberStringGrid ;			// 길드 멤버 리스트

	CRYLListBox*		m_pGeneralGuildPopup ;			// 일반길드원 팝업 메뉴
	CRYLListBox*		m_pProposalGuildPopup ;			// 신청길드원 팝업 메뉴
	BOOL				m_bGeneralGuild[ 6 ] ;
	BOOL				m_bProposalGuild[ 2 ] ;

	DWORD				m_dwResultMessageBox[ 7 ] ;
	DWORD				m_dwResultProposalMessageBox[ 2 ] ;

	DWORD		m_dwStartMemberInfo ;
	DWORD		m_dwServerPage ;
	DWORD		m_dwMaxMemberInfo ;
	INT			m_iCurrentLine ;
	INT			m_iPageState ;

	DWORD		m_dwRstLeave ;
	DWORD		m_dwRstLeaveForChina ;
	DWORD		m_dwRstLeaveCancel ;

	std::vector< GuildMemberInfoNode* >		m_vecMemberInfo ;
	std::map< INT, GuildMemberInfoNode* >	m_mapMemberInfo ;

	GuildMemberInfoNode*		m_pMemberInfo ;

	// 선택된 멤버의 정보
	CHAR		m_szCurrentName[ 32 ] ;
	CHAR		m_szCurrentFarm[ 32 ] ;
	CHAR		m_szCurrentGold[ 32 ] ;
	CHAR		m_szCurrentPos[ 20 ][ 32 ] ;
	CHAR		m_szGeneralGuild[ 8 ][ 32 ] ;
	INT			m_iCurrentPos ;
	
	CHAR		m_szSortMethodName[ 10 ][ 32 ] ;
	INT			m_iSortMethod[ 10 ] ;
	CHAR		m_szMemberClass[ 24 ][ 16 ] ;
	CHAR		m_szMembertitle[ 6 ][ 32 ] ;
	INT			m_iCurrentSort ;

	INT			m_iCurrentSel ;
	INT			m_iTotalMemberNum;						// 현재 길드원 인원수.


public :
	CRYLGuildMemberPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGuildMemberPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	std::vector< GuildMemberInfoNode* >::iterator		FindGuildMemberIter( GuildMemberInfoNode* pMemberInfo ) ;
	std::vector< GuildMemberInfoNode* >::iterator		FindGuildMemberIter( DWORD dwCID ) ;

	void	ClearList(void);

	// 정렬 방식 페이지 버튼
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// 멤버 리스트 페이지 버튼
	VOID	MemberListPrev() ;
	VOID	MemberListNext() ;

	VOID	RightSettingButton() ;

	// ----------------------------------------------------------------------------------------

	VOID	RequestGuildMemberList() ;
	VOID	ProcessGuildMemberList( unsigned char cSortCmd, 
								    unsigned char cPage, 
								    unsigned char cNodeNum, 
									unsigned char cTotalMemberNum,
								    GuildMemberInfoNode* lpMemberNode ) ;

	INT		GetCurrentPageLine( INT bPageState ) ;
	HRESULT	UpdateGuildStringGrid() ;

	GuildMemberInfoNode*		GetMemberInfo()		{ return m_pMemberInfo ; }

	CHAR*	GetTitleString( UCHAR ucTitle )			{ return m_szMembertitle[ ucTitle ] ; }
	INT		GetTitle( DWORD dwCID ) ;
	
	unsigned char GetClass( DWORD dwCID ) ;
	unsigned char GetLevel( DWORD dwCID ) ;

	VOID	UpdateGuildMemberRight( DWORD dwCID, UCHAR ucTitle ) ;
	VOID	UpdateGuildWarJoinFlag( DWORD dwCID, UCHAR ucFlag ) ;
    
	VOID	SelectMemberInfo()  ;
	VOID	ShowGuildMenuPopup(INT iX, INT iY ) ;
	
	VOID	GeneralGuildEnableEvent( UCHAR ucTitleVal ) ; 
	VOID	ProposalGuildEnableEvent() ;

	HRESULT	UpdateGuildMemberPanel() ;

	VOID	ProcessGeneralGuildPopup() ;
	VOID	ResultGeneralGuildPopup() ;
	VOID	PorcessProposalGuildPopup() ;
	VOID	ResultProposalGuildPopup() ;

	VOID	GuildLeaveButtonEvent() ;
	VOID	GuildLeaveCancelButtonEvent() ;
	VOID	GuildWarJoinButtonEvent() ;
	VOID	GuildWarLeaveButtonEvent() ;

	virtual VOID Hide( CGMUIObject* Sender ) ;

	INT		FindSortIndex( unsigned char cSortCmd ) ;
} ;

#endif //__RYL_GUILDMEMBERPANEL_H__
