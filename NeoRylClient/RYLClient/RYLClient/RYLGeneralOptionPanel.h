//================================================================
//
//	Name : RYLGeneralOptionPanel.h 
//	Desc : 일반 옵션 판넬
//  Date : 2004. 04. 13
//
//================================================================
#ifndef __RYL_GENERALOPTIONPANEL_H__
#define __RYL_GENERALOPTIONPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel ;
class CRYLCheckBox ;
class CRYLButton ;

namespace RYLGENERALOPTIONPANEL
{
	const DWORD		DUEL_BUTTON				= 0x00000000 ; // 1 : 1 신청
	const DWORD		EXCHANGE_BUTTON			= 0x00000001 ; // 거래신청거부
	const DWORD		PARTY_BUTTON			= 0x00000002 ; // 파티신청거부
	const DWORD		FRIEND_BUTTON			= 0x00000003 ; // 친구등록거부
	const DWORD		RECALL_BUTTON			= 0x00000004 ; // 리콜거부

	const DWORD		WHISPER_BUTTON			= 0x00000005 ; // 귓속말 보기
	const DWORD		SHOUT_BUTTON			= 0x00000006 ; // 외치기 보기
	const DWORD		TIPBROAD_BUTTON			= 0x00000007 ; // 도움말 보기
	const DWORD		CHAT_BUTTON				= 0x00000008 ; // 일반 대화 보기
	// 케릭터 이름
	const DWORD		CHARNAME01_BUTTON		= 0x00000009 ; // 전체 보기
	const DWORD		CHARNAME02_BUTTON		= 0x00000010 ; // 아군만 보기
	const DWORD		CHARNAME03_BUTTON		= 0x00000011 ; // 파티원만 보기
	const DWORD		CHARNAME04_BUTTON		= 0x00000012 ; // 이름 보지 않음

	const DWORD		OK_BUTTON				= 0x00000013 ;
	const DWORD		JOY_BUTTON				= 0x00000014 ;
} ;

class CRYLGeneralOptionPanel : public CGMUICustomPanel
{
private :
	CRYLButton*		m_pOkButton ;				// 확인
	CRYLButton*		m_pJoyButton ;				// 조이패드 설정.

	CRYLLabel*		m_pProposalRefusalLabel ;	// 신청 거부
	CRYLLabel*		m_pChattingLabel ;			// 대화
	CRYLLabel*		m_pCharacternameLabel ;		// 캐릭터 이름

	CRYLLabel*	m_pDuelLabel ;					// 1 : 1 신청
	CRYLLabel*	m_pExchangeLabel ;				// 거래신청거부
	CRYLLabel*	m_pPartyLabel ;					// 파티신청거부
	CRYLLabel*	m_pFriendLabel ;				// 친구등록거부
	CRYLLabel*	m_pRecallLabel ;				// 리콜

	CRYLLabel*	m_pWhisperLabel ;				// 귓속말 보기
	CRYLLabel*	m_pShoutLabel ;					// 외치기 보기
	CRYLLabel*	m_pTipBroadLabel ;				// 도움말 보기
	CRYLLabel*	m_pChatLabel ;					// 일반 대화 보기
	// 케릭터 이름
	CRYLLabel*	m_pCharName01Label ;			// 전체 보기
	CRYLLabel*	m_pCharName02Label ;			// 아군만 보기
	CRYLLabel*	m_pCharName03Label ;			// 파티원만 보기
	CRYLLabel*	m_pCharName04Label ;			// 이름 보지 않음

	CRYLCheckBox*	m_pDuelButton ;				// 1 : 1 신청
	CRYLCheckBox*	m_pExchangeButton ;			// 거래신청거부
	CRYLCheckBox*	m_pPartyButton ;			// 파티신청거부
	CRYLCheckBox*	m_pFriendButton ;			// 친구등록거부
	CRYLCheckBox*	m_pRecallButton ;			// 리콜응답거부

	CRYLCheckBox*	m_pWhisperButton ;			// 귓속말 보기
	CRYLCheckBox*	m_pShoutButton ;			// 외치기 보기
	CRYLCheckBox*	m_pTipBroadButton ;			// 도움말 보기
	CRYLCheckBox*	m_pChatButton ;				// 일반 대화 보기

	// 케릭터 이름
	CRYLCheckBox*	m_pCharName01Button ;		// 전체 보기
	CRYLCheckBox*	m_pCharName02Button ;		// 아군만 보기
	CRYLCheckBox*	m_pCharName03Button ;		// 파티원만 보기
	CRYLCheckBox*	m_pCharName04Button ;		// 이름 보지 않음
	
public :
	CRYLGeneralOptionPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGeneralOptionPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID GeneralOptionMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID	ReLoadOption() ;
} ;

#endif //__RYL_GENERALOPTIONPANEL_H__

