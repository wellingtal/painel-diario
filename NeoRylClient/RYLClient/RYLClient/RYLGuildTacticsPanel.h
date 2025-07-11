//================================================================
//
//	Name : RYLTacticsPanel.h 
//	Desc : ���뺴 ����
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
	CRYLLabel*			m_pGuildTacticsListLabel ;		// ��� �뺴 ����Ʈ
	CRYLLabel*			m_pSortMethodLabel ;			// ���Ĺ��
	CRYLLabel*			m_pSortMethodValueLabel ;		// ���Ĺ�İ�
	CRYLButton*			m_pSortPrevButton ;				// ���Ĺ�� ���� ���
	CRYLButton*			m_pSortNextButton ;				// ���Ĺ�� ���� ���
	CRYLButton*			m_pPagePrevButton ;				// ��� ���� ������
	CRYLButton*			m_pPageNextButton ;				// ��� ���� ������

	CRYLLabel*			m_pRankLabel ;					// ����
	CRYLLabel*			m_pNameLabel ;					// �̸�
	CRYLLabel*			m_pLevelLabel;					// ����
	CRYLLabel*			m_pFameLabel ;					// ��
	CRYLLabel*			m_pClassLabel;					// Ŭ����

	CRYLLabel*			m_pTacticsNameLabel ;			// �뺴 �̸�
	CRYLLabel*			m_pTacticsNameValueLabel ;		// �뺴 �̸� ��
	CRYLButton*			m_pTacticsEntryButton ;			// �뺴 �㰡 ��ư
	CRYLButton*			m_pTacticsLeaveButton ;			// �뺴 ����/Ż�� ��ư

	CRYLStringGrid*		m_pTacticsStringGrid ;			// ��� �뺴 ����Ʈ

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

	// ���õ� ����� ����
	CHAR		m_szCurrentName[ 32 ] ;

	CHAR		m_szSortMethodName[ 6 ][ 32 ] ;
	INT			m_iSortMethod[ 6 ] ;
	CHAR		m_szTacticsClass[ 24 ][ 16 ] ;
	CHAR		m_szTacticsTitle[ 6 ][ 32 ] ;
	INT			m_iCurrentSort ;

	INT			m_iCurrentSel ;
	INT			m_iTotalTacticsNum;						// ���� ���� �ο���.


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

	// ���� ��� ������ ��ư
	VOID	SortMethodPrev() ;
	VOID	SortMethodNext() ;

	// ��� ����Ʈ ������ ��ư
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
