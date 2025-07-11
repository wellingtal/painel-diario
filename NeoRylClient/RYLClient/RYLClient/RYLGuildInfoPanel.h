//================================================================
//
//	Name : RYLGuildInfoPanel.h 
//	Desc : ������� �ǳ�
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

	CRYLLabel*			m_pSortLabel ;						// ����
	CRYLLabel*			m_pSortValueLabel ;					// ���� ��
	CRYLButton*			m_pSortPrevButton ;					// ���� ��� ��ư( ���� )
	CRYLButton*			m_pSortNextButton ;					// ���� ��� ��ư( ���� )
	
	CRYLLabel*			m_pRankLabel ;						// ����
	CRYLLabel*			m_pGuildNameLabel ;					// ����
	CRYLLabel*			m_pGuildMasterLabel ;				// ��� ������
	CRYLLabel*			m_pGuildFameLabel ;					// ����
	CRYLLabel*			m_pGuildMarkListLabel ;				// ��ũ
	CRYLSpriteEx*		m_pGuildMarkListSprite[ 10 ] ;		// ��� ��ũ ��������Ʈ
	CRYLStringGrid*		m_pGuildListStringGrid ;			// ��� �׸���
	CRYLButton*			m_pGuilListPrevButton ;				// ��� ����Ʈ ���� ��ư
	CRYLButton*			m_pGuilListNextButton ;				// ��� ����Ʈ ���� ��ư

	CRYLLabel*			m_pGuildNameLabel1 ;				// ����
	CRYLLabel*			m_pGuildNameEdit ;					// ��� ��

	CRYLButton*			m_pGuildEntryButton ;				// ��� ���Խ�û ��ư
	CRYLButton*			m_pGuildCancelButton ;				// ��� ������� ��ư
	CRYLButton*			m_pGuildTacticsEntryButton ;		// ��� �뺴 ���Խ�û ��ư
	CRYLButton*			m_pGuildTacticsCancelButton ;		// ��� �뺴 ������� ��ư

	CRYLLabel*			m_pGuildRelationLabel ;				// ����
	CRYLLabel*			m_pGuildRelationValueLabel ;		// ��� ���� ��

	CRYLLabel*			m_pGuildMemberNumLabel ;			// ��� �ο�
	CRYLLabel*			m_pGuildMemberNumValueLabel ;		// ��� �ο� ��

	CRYLButton*			m_pGuildHostilityInfoButton ;		// ���� ��Ȳ ��ư

	CRYLLabel*			m_pSetGuildRelationLabel ;			// ���� ����
	CRYLButton*			m_pDeclareHostilityButton ;			// ���� ���� ��ư
	CRYLButton*			m_pDeclarePeaceButton ;				// ��ȭ ���� ��ư
	CRYLButton*			m_pDeclareAlertHostilityButton ;	// ��� ���� ���� ��ư

	INT					m_iCurrentSort  ;					// ���� �ҽ� ���
	INT					m_iCurrentLine ;
	CHAR				m_szSortMethodName[ 5 ][ 64 ] ;		
	INT					m_iSortMethod[ 5 ] ;
	DWORD				m_dwServerGuildPage ;				// �������� �䱸�ϴ� ������
	DWORD				m_dwStartGuildInfo  ;				// ��� ����Ʈ�� ���� �ε���
	DWORD				m_dwPageState ;						// ������ �䱸 ����

	DWORD				m_dwGuildEntryResult ;				// ��� ���� ����

	std::map< INT, GUILDLARGEINFOEX* >		m_mapGuildInfo ;
	std::vector< GUILDLARGEINFOEX* >		m_vecGuildInfo ;

	SGUILDINFOEX		m_GuildInfo ;

	// ���� �� ��� ����
	INT			m_iCurrentLevel ;
	INT			m_iCurrentGID ;
	INT			m_iEntryGID ;
	INT			m_iCurrentSel ;
	DWORD		m_dwMaxGuildInfo ;
	
	
protected:

	LPGUILDLARGEINFOEX GetGuild( DWORD dwGID ) ;			// visual �ÿ��� ����


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

	// ���� ��� ������ ��ư
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// ��� ����Ʈ ������ ��ư
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

	VOID	ProcessGuildInclination( unsigned long dwGID, unsigned char cInclination );		// ��� ���� ����

	virtual VOID	SetVisible( BOOL bVisible ) ;
	virtual VOID	Hide( CGMUIObject* Sender ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift) ;

	INT		FindSortIndex( unsigned char cSortCmd ) ;
} ;

#endif //__RYL_GUILDINFOPANEL_H__

