//================================================================
//
//	Name : RYLMemberPanel.h 
//	Desc : ����� ����
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
	CRYLLabel*			m_pGuildMemberListLabel ;		// ���� ����Ʈ
	CRYLLabel*			m_pSortMethodLabel ;			// ���Ĺ��
	CRYLLabel*			m_pSortMethodValueLabel ;		// ���Ĺ�İ�
	CRYLButton*			m_pSortPrevButton ;				// ���Ĺ�� ���� ���
	CRYLButton*			m_pSortNextButton ;				// ���Ĺ�� ���� ���
	CRYLButton*			m_pPagePrevButton ;				// ��� ���� ������
	CRYLButton*			m_pPageNextButton ;				// ��� ���� ������
	
	CRYLLabel*			m_pRankLabel ;					// ����
	CRYLLabel*			m_pNameLabel ;					// �̸�
	CRYLLabel*			m_pTitleLabel ;					// ����
	CRYLLabel*			m_pLevelLabel;					// ����
	CRYLLabel*			m_pClassLabel;					// Ŭ����
	
	CRYLLabel*			m_pCurrentFameLabel ;			// ���� ��
	CRYLLabel*			m_pCurrentFameValueLabel ;		// ���� �� ��
	CRYLLabel*			m_pCurrentGoldLabel ;			// ���� �ݾ�
	CRYLLabel*			m_pCurrentValueGoldLabel ;		// ���� �ݾ� ��
	CRYLLabel*			m_pCurrentPositionLabel ;		// ���� ��ġ
	CRYLLabel*			m_pCurrentPositionValueLabel ;	// ���� ��ġ ��
	CRYLLabel*			m_pCurrentGuildWarFlagLabel ;	// ���� ����
	CRYLLabel*			m_pCurrentGuildWarFlagValueLabel ;	// ���� ���� ��

	CRYLButton*			m_pRightSettingButton ;			// ���� ����
	CRYLButton*			m_pGuildLeaveButton ;			// ��� Ż��	
	CRYLButton*			m_pGuildLeaveCanelButton ;		// ��� Ż�� ���
	CRYLButton*			m_pGuildWarJoin ;				// ��� ���� ����
	CRYLButton*			m_pGuildWarLeave ;				// ��� ���� ����

	CRYLStringGrid*		m_pMemberStringGrid ;			// ��� ��� ����Ʈ

	CRYLListBox*		m_pGeneralGuildPopup ;			// �Ϲݱ��� �˾� �޴�
	CRYLListBox*		m_pProposalGuildPopup ;			// ��û���� �˾� �޴�
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

	// ���õ� ����� ����
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
	INT			m_iTotalMemberNum;						// ���� ���� �ο���.


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

	// ���� ��� ������ ��ư
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// ��� ����Ʈ ������ ��ư
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
