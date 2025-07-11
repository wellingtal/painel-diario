//================================================================
//
//	Name : RYLGuildInfoPanel.h 
//	Desc : 길드정보 판넬
//  Date : 2003. 12. 21
//
//================================================================
#ifndef __RYL_GUILDINFOPANEL_H__
#define __RYL_GUILDINFOPANEL_H__

#include "GMUICustomPanel.h"
#include <d3d8.h>

#include <vector>
#include <map> 
#include <set>

#include <Network/Packet/PacketStruct/GuildPacket.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ;
class CRYLEdit ;
class CRYLSpriteEx ;
class CRYLSprite ;
class CTexture ;
class CGMImageDib ;
class CRYLListBox ;

namespace GUILDINFOPAGEEX
{
	enum GuildInfoPage
	{
		PAGE_CURRENT	= 0,
		PAGE_NEXT		= 1,
		PAGE_PREV		= 2
	} ;
} ;

namespace GUILDINFOSORT
{
	enum GuildInfoSort
	{
		MAX_SORT_METHOD = 4
	} ;
} ;

namespace RYLGUILDPANEL
{
	const DWORD		GUILD_STRINGGRID				= 0x00000000 ;
	const DWORD		SORTPREV_BUTTON					= 0x00000001 ;
	const DWORD		SORTNEXT_BUTTON					= 0x00000002 ;
	const DWORD		GUILDSEACH_EDIT					= 0x00000003 ;
	const DWORD		GUILDENTRY_BUTTON				= 0x00000004 ;
	const DWORD		GUILDCANCEL_BUTTON				= 0x00000005 ;
	const DWORD		GUILDTACTICSENTRY_BUTTON		= 0x00000006 ;
	const DWORD		GUILDTACTICSCANCEL_BUTTON		= 0x00000007 ;
	const DWORD		GUILDHOSTILITYINFO_BUTTON		= 0x00000008 ;
	const DWORD		DECLAREHOSTILITY_BUTTON			= 0x00000009 ;
	const DWORD		DECLAREPEACE_BUTTON				= 0x00000010 ;
	const DWORD		DECLAREALERTHOSTILITY_BUTTON	= 0x00000011 ;

	const DWORD		LISTPREV_BUTTON					= 0x00000012 ;
	const DWORD		LISTNEXT_BUTTON					= 0x00000013 ;

	const DWORD		GUILDCOUSE_COMBOBOX				= 0x00000014 ;
	const DWORD		GUILDCOUSE_MENU					= 0x00000015 ;

	const DWORD		RELATION_POPUP					= 0x00000017 ;
} ;

typedef struct __SGUILDINFOEX
{
	TCHAR		szGuildName[ MAX_GUILD_NAME_LEN ] ;
	TCHAR		szGuildMaster[ MAX_MEMBER_NAME_LEN ] ;
	TCHAR		szGuildFame[ 16 ] ;
	CRYLSprite*	pGuildMark ;
	TCHAR		szGuildLevel[ 16 ] ;
	TCHAR		szGuildMemberNum[ 16 ] ;
	TCHAR		szGuildInclination[ 16 ] ;

	__SGUILDINFOEX()
	{
		pGuildMark				= NULL ;
		szGuildName[ 0 ]		='\0' ;
		szGuildMaster[ 0 ]		='\0' ;
		szGuildFame[ 0 ]		='\0' ;
		szGuildLevel[ 0 ]		='\0' ;
		szGuildMemberNum[ 0 ]	='\0' ;
		szGuildInclination[ 0 ]	='\0' ;

	}
	~__SGUILDINFOEX()
	{
		if ( pGuildMark )
		{
			pGuildMark = NULL ;
		}
	}
} SGUILDINFOEX, *LPSGUILDINFOEX ;

typedef struct GUILDLARGEINFOEX* LPGUILDLARGEINFOEX;
struct GUILDLARGEINFOEX : public GuildLargeInfoNode
{
	CTexture*		m_pTexture ;
	CRYLSprite*		m_pSprite ;

	GUILDLARGEINFOEX();
	~GUILDLARGEINFOEX();

};

class CRYLGuildInfoPanel : public CGMUICustomPanel
{
public :

	CRYLLabel*			m_pSortLabel ;						// 정렬
	CRYLLabel*			m_pSortValueLabel ;					// 정렬 값
	CRYLButton*			m_pSortPrevButton ;					// 정렬 방식 버튼( 이전 )
	CRYLButton*			m_pSortNextButton ;					// 정렬 방식 버튼( 다음 )
	
	CRYLLabel*			m_pRankLabel ;						// 순위
	CRYLLabel*			m_pGuildNameLabel ;					// 길드명
	CRYLLabel*			m_pGuildMasterLabel ;				// 길드 마스터
	CRYLLabel*			m_pGuildFameLabel ;					// 길드명성
	CRYLLabel*			m_pGuildMarkListLabel ;				// 마크
	CRYLSpriteEx*		m_pGuildMarkListSprite[ 10 ] ;		// 길드 마크 스프라이트
	CRYLStringGrid*		m_pGuildListStringGrid ;			// 길드 그리드
	CRYLButton*			m_pGuilListPrevButton ;				// 길드 리스트 이전 버튼
	CRYLButton*			m_pGuilListNextButton ;				// 길드 리스트 다음 버튼

	CRYLLabel*			m_pGuildNameLabel1 ;				// 길드명
	CRYLLabel*			m_pGuildNameEdit ;					// 길드 값

	CRYLButton*			m_pGuildEntryButton ;				// 길드 가입신청 버튼
	CRYLButton*			m_pGuildCancelButton ;				// 길드 가입취소 버튼
	CRYLButton*			m_pGuildTacticsEntryButton ;		// 길드 용병 가입신청 버튼
	CRYLButton*			m_pGuildTacticsCancelButton ;		// 길드 용병 가입취소 버튼

	CRYLLabel*			m_pGuildRelationLabel ;				// 관계
	CRYLLabel*			m_pGuildRelationValueLabel ;		// 길드 관계 값

	CRYLLabel*			m_pGuildMemberNumLabel ;			// 길드 인원
	CRYLLabel*			m_pGuildMemberNumValueLabel ;		// 길드 인원 값

	CRYLButton*			m_pGuildHostilityInfoButton ;		// 적대 현황 버튼

	CRYLLabel*			m_pSetGuildRelationLabel ;			// 관계 설정
	CRYLButton*			m_pDeclareHostilityButton ;			// 적대 선언 버튼
	CRYLButton*			m_pDeclarePeaceButton ;				// 평화 선언 버튼
	CRYLButton*			m_pDeclareAlertHostilityButton ;	// 경계 적대 선언 버튼

	INT					m_iCurrentSort  ;					// 현재 소스 방식
	INT					m_iCurrentLine ;
	CHAR				m_szSortMethodName[ 5 ][ 64 ] ;		
	INT					m_iSortMethod[ 5 ] ;
	DWORD				m_dwServerGuildPage ;				// 서버에서 요구하는 페이지
	DWORD				m_dwStartGuildInfo  ;				// 길드 리스트의 시작 인덱스
	DWORD				m_dwPageState ;						// 페이지 요구 상태

	DWORD				m_dwGuildEntryResult ;				// 길드 가입 인증

	std::map< INT, GUILDLARGEINFOEX* >		m_mapGuildInfo ;
	std::vector< GUILDLARGEINFOEX* >		m_vecGuildInfo ;

	SGUILDINFOEX		m_GuildInfo ;

	// 현재 내 길드 레벨
	INT			m_iCurrentLevel ;
	INT			m_iCurrentGID ;
	INT			m_iEntryGID ;
	INT			m_iCurrentSel ;
	DWORD		m_dwMaxGuildInfo ;
	
	
protected:

	LPGUILDLARGEINFOEX GetGuild( DWORD dwGID ) ;			// visual 시에만 쓰임


public :

	CRYLGuildInfoPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGuildInfoPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	VOID		GuildEntryButtonEvent() ;
	VOID		GuildEntryCancelButtonEvent() ;
	VOID		GuildTacticsEntryButtonEvent() ;
	VOID		GuildTacticsCancelButtonEvent() ;
	VOID		GuildHostilityInfoButtonEvent() ;
	VOID		GuildDeclareHostilityButtonEvent() ;
	VOID		GuildDeclarePeaceButtonEvent() ;
	VOID		GuildDeclareAlertHostilityButtonEvent() ;


	CRYLStringGrid*	GetStringGrid()				{ return m_pGuildListStringGrid ; } ;

	void	ClearList(void);

	// 정렬 방식 페이지 버튼
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// 길드 리스트 페이지 버튼
	VOID	GuildListPrev() ;
	VOID	GuildListNext() ;
	
	LPGUILDLARGEINFOEX FindGuild( DWORD dwGID ) ;	

	VOID	ProcessGuildInfo( unsigned char cSortCmd, 
							  unsigned char cPage, 
							  unsigned char cSmallNodeNum, 
							  GuildSmallInfoNode* lpSmallNode, 
							  unsigned char cLargeNodeNum, 
							  GuildLargeInfoNode* lpLargeNode ) ;

	HRESULT		InsertGuildInfoMap( const GUILDLARGEINFOEX& guildInfo ) ;
	HRESULT		DeleteGuildInfoMap( DWORD dwGID ) ;
	VOID		DeleteMyGuildInfo( DWORD dwGID ) ;
	HRESULT		ReSortGildInfoList()  ;
	HRESULT		RequestNewGuild( std::set< unsigned long > setGID ) ;
	HRESULT		UpdateGuildMark( DWORD dwGID ) ;
	HRESULT		UpdateGuildMark( DWORD dwGID, BYTE* wBuff, DWORD dwGold ) ;
	HRESULT		UpdateGuildScreen()  ;
	HRESULT		UpdateGuildLevel( DWORD dwGID, BYTE byLevel, DWORD dwGold ) ;
	HRESULT		UpdateGuildStringGrid() ;
	VOID		UpdateMyGuildInfo( INT iIndex ) ;
	VOID		RequestGuildList() ;
	
	VOID		SelectGuildInfo() ;

	VOID		SortGuildFame() ;
	VOID		SortGuildName() ; 
	VOID		SortHostilityGuild() ;
	VOID		SortNeutralityGuild() ;
	VOID		SortMyGuild() ;
	INT			GetCurrentPageLine( INT bPageState ) ;
	VOID		UpdateGuildMsgBox() ;
	CRYLSprite*	GetGuildSprite( DWORD dwGID ) ;
	const char*	GetGuildName( DWORD dwGID ) ;
	BOOL		GetUseGID( DWORD dwGID )	;
	VOID		InsertGuildInfo( LPGUILDLARGEINFOEX pGuildInfo )  ;

	std::vector< LPGUILDLARGEINFOEX >::iterator		FindGuildIter( LPGUILDLARGEINFOEX pGuildInfo ) ;
	std::vector< LPGUILDLARGEINFOEX >::iterator		FindGuildIter( INT iIndex ) ;
	int			GetVectorIndex( unsigned long dwGID ) ;

	UINT	GetMyGuildInfoIndex( DWORD dwGID ) ;
	BOOL	IsHostilityGuild( DWORD dwTargetGID ) ;

	VOID	UpdateBtnState() ;

	VOID	ClearRelationInfo() ;
	VOID	UpdateRelationInfo( unsigned short wNodeNum, GuildRelationInfoNode* lpRelationNode ) ;

	VOID	ProcessGuildInclination( unsigned long dwGID, unsigned char cInclination );		// 길드 국적 변경

	virtual VOID	SetVisible( BOOL bVisible ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift) ;

	INT		FindSortIndex( unsigned char cSortCmd ) ;
} ;

#endif //__RYL_GUILDINFOPANEL_H__

