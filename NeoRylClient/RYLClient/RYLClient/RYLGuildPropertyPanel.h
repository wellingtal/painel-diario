//================================================================
//
//	Name : RYLPropertyPanel.h 
//	Desc : 길드금고 판넬
//  Date : 2003. 12. 23
//
//================================================================
#ifndef __RYL_GUILDPROPERTYPANEL_H__
#define __RYL_GUILDPROPERTYPANEL_H__

#include "GMUICustomPanel.h"
#include <Community/Guild/GuildConstants.h>
#include <d3d8.h>

class CRYLLabel ;
class CRYLButton ;
class CRYLEdit ;

class CRYLListBox ;
class CRYLSpriteEx ;
class CRYLLabel ;
class CRYLBitmap ;
class CRYLStringGrid ;
class CRYLGuildInfoPanel;

namespace RYLGUILDPROPERTYPANEL
{
	const DWORD		PAYMENTGOLD_BUTTON				= 0x00000000 ;
	const DWORD		DEPOSITGOLD_BUTTON				= 0x00000001 ;
	const DWORD		DIVISIONGOLD_BUTTON				= 0x00000002 ;
	const DWORD		USEINQUIRY_BUTTON				= 0x00000003 ;

	const DWORD		GUILDMARK_BUTTON				= 0x00000004 ;
	const DWORD		GUILDLEVEL_BUTTON				= 0x00000005 ;

	/*
	enum	INCLINATION
	{
		HUMAN_KARTERANT_WAR			= Guild::RACE_HUMAN | Guild::NATION_KARTERANT 	| Guild::MODE_WAR ,
		HUMAN_MERKADIA_WAR			= Guild::RACE_HUMAN | Guild::NATION_MERKADIA  	| Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_WAR		= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_PEACE	= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		AKHAN_KARTERANT_WAR			= Guild::RACE_AKHAN | Guild::NATION_KARTERANT 	| Guild::MODE_WAR ,
		AKHAN_MERKADIA_WAR			= Guild::RACE_AKHAN | Guild::NATION_MERKADIA  	| Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_WAR		= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_PEACE	= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		BOTH_KARTERANT_WAR			= Guild::RACE_BOTH  | Guild::NATION_KARTERANT	| Guild::MODE_WAR ,
		BOTH_MERKADIA_WAR			= Guild::RACE_BOTH  | Guild::NATION_MERKADIA	| Guild::MODE_WAR ,
		BOTH_NATION_THIRD_WAR		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		BOTH_NATION_THIRD_PEACE		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_PEACE 
	};
	*/
} ;


class CRYLGuildPropertyPanel : public CGMUICustomPanel
{
public :

	// -----------------------------------------------------------------------------------
	// 길드금고 / 창고관리

	CRYLLabel*			m_pStorehouseLabel ;					// 길드금고 / 창고관리
	CRYLLabel*			m_pGoldManagementLabel ;				// 길드금고관리
	CRYLLabel*			m_pCurrentGoldLabel	;					// 현재 잔고
	CRYLLabel*			m_pCurrentGoldValueLabel ;				// 현재 잔고 값
	CRYLButton*			m_pPaymentGoldButton ;					// 출금
	CRYLButton*			m_pDepositGoldButton ;					// 입금
	CRYLButton*			m_pDivisionGoldButton ;					// 분배
	CRYLButton*			m_pUseInquiryButton ;					// 사용내역 조회

	// -----------------------------------------------------------------------------------
	// 마크변경 / 인원수변경

	CRYLLabel*			m_pMarkNMemberNumLabel ;				// 마크변경 / 인원수변경
	CRYLLabel*			m_pMarkLabel ;							// 길드 마크 변경
	CRYLLabel*			m_pCurrentMarkLabel ;					// 마크 현황
	CRYLSpriteEx*		m_pCurrentMarkSprite ;					// 길드 마크 스프라이트
	CRYLButton*			m_pChangeMarkButton ;					// 마크 변경 버튼

	CRYLLabel*			m_pMemberNumLabel ;						// 길드 최대 인원수 변경
	CRYLLabel*			m_pCurrentMemberNumLabel ;				// 길드원 최대 수용 인원
	CRYLLabel*			m_pCurrentMemberNumValueLabel ;			// 길드원 최대 수용 인원 값
	CRYLButton*			m_pChangeMemberNumButton ;				// 인원 증대 버튼

	DWORD				m_dwCurrentMemberNum ;					// 현재 길드 멤버 수
	DWORD				m_dwGuildLevel ;						// 현재 길드 레벨
	DWORD				m_dwGuildFame ;							// 현재 길드 명성

	DWORD				m_dwGuildMarkResult ;					// 길드 마크 변경 결과
	DWORD				m_dwGuildLevelResult ;					// 길드 레벨(인원 증대) 변경 결과

	// -----------------------------------------------------------------------------------
	// 국적

	unsigned char		m_ucInclination;						// 자신의 길드 국적
	unsigned char		m_ucSelectInclination;					// 선택된 길드 국적


	DWORD		m_dwRstInputGold ;
	DWORD		m_dwValInputGold ;

	DWORD		m_dwRstOutputGold ;
	DWORD		m_dwValOutputGold ;

	DWORD		m_dwRstEmissionGold ;
	DWORD		m_dwValEmissionGold ;

	ULONG		m_iCurrentGold ;

	bool		m_bMessageBox ;


public :

	CRYLGuildPropertyPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGuildPropertyPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	Hide( CGMUIObject* Sender ) ;

	VOID	CheckRightButton() ;
	VOID	SetCurrentGold( INT iGold )		{ m_iCurrentGold = iGold ; }
	ULONG	GetCurrentGold()				{ return m_iCurrentGold ; }
	ULONG	GetGuildGold()					{ return m_iCurrentGold ; }

	VOID	PaymentGold() ;						// 출금
	VOID	DepositGold() ;						// 입금
	VOID	DivisionGold() ;					// 분배
	VOID	SetSafeAndMyGold( DWORD dwCID, char* szName, DWORD dwSafeGold, DWORD dwCharGold, DWORD dwCmd )  ;

	VOID	UpdateCheckPropertyMsgBox() ;
	
	VOID			SetInclination( unsigned char type ) ;			// 자신의 길드 국적 셋팅
	unsigned char	GetInclination();
	unsigned char	GetSelectInclination();

	
	VOID	Clear() ;

	VOID	SetGuildMark( CRYLSprite* pMarkSprite ) ;
	VOID	SetGuildMemberLevelFame( DWORD dwCurrentMemberNum, DWORD dwLevel, DWORD dwFame ) ;

	VOID	ChangeGuildMarkButtonEvent() ;			// 길드 마크 변경
	VOID	ChangeGuildLevelButtonEvent() ;			// 길드 인원 변경

	static VOID	SetNation( unsigned char cNation );
} ;

#endif //__RYL_GUILDPROPERTYPANEL_H__
