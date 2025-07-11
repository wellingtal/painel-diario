//================================================================
//
//	Name : RYLChattingDlg.h 
//	Desc : 체팅 다이얼로그
//  Date : 2004. 01. 07
//
//================================================================
#ifndef __RYL_CHATTINGDLG_H__
#define __RYL_CHATTINGDLG_H__

#include "RYLDialog.h"
#include "RYLCheckBox.h"
#include <deque>

class CRYLButton ;
class CRYLListBox ;
class CRYLEdit ;
class CRYLImage ;
class CRYLTabButton ;
class CRYLLabel ;

class CRYLStallDlg;

namespace RYLCHATTINGDLG
{	
	const DWORD		 SIZE_BUTTON			= 0x00000000 ;
	const DWORD		 HIDE_BUTTON			= 0x00000001 ;
	const DWORD		 CHAT_EDIT				= 0x00000002 ;
	const DWORD		 WHISPER_CHECKBOX		= 0x00000003 ;
	const DWORD		 SHOUT_CHECKBOX			= 0x00000004 ;
	const DWORD		 TRADE_CHECKBOX			= 0x00000005 ;
	const DWORD		 GUILD_CHECKBOX			= 0x00000006 ;
	const DWORD		 PARTY_CHECKBOX			= 0x00000007 ;
	const DWORD		 CHAT_LISTBOX			= 0x00000008 ;
	const DWORD		 CHAT_ENABLE_CHECKBOX	= 0x00000009 ;
	const DWORD		 SHOUT_ENABLE_CHECKBOX	= 0x00000010 ;
	const DWORD		 DEAL_ENABLE_CHECKBOX	= 0x00000011 ;
	const DWORD		 PARTY_ENABLE_CHECKBOX	= 0x00000012 ;
	const DWORD		 GUILD_ENABLE_CHECKBOX	= 0x00000013 ;
	const DWORD		 ALL_ENABLE_CHECKBOX	= 0x00000014 ;
} ;

namespace RYLCHATMODE
{
	const DWORD		 RYL_EDITMODE_NONE			= 0x00000000 ;
	const DWORD	 	 RYL_EDITMODE_CREATECHAR	= 0x00000001 ;
	const DWORD		 RYL_EDITMODE_CHAT			= 0x00000002 ;
	const DWORD		 RYL_EDITMODE_NUMBER		= 0x00000003 ;
	const DWORD		 RYL_EDITMODE_WHISPER		= 0x00000004 ;
	const DWORD		 RYL_EDITMODE_DEPOSITPASS	= 0x00000005 ;
	const DWORD		 RYL_EDITMODE_CURPASS		= 0x00000006 ;
	const DWORD		 RYL_EDITMODE_NEWPASS		= 0x00000007 ;
	const DWORD		 RYL_EDITMODE_STALL			= 0x00000008 ;
	const DWORD		 RYL_EDITMODE_LOGINID		= 0x00000009 ;
	const DWORD		 RYL_EDITMODE_LOGINPASS		= 0x00000010 ;

	const DWORD		 RYL_CHATMODE_WHOLE			= 0x00000000 ;
	const DWORD		 RYL_CHATMODE_CHAT			= 0x00000001 ;
	const DWORD		 RYL_CHATMODE_SHOUT			= 0x00000002 ;
	const DWORD		 RYL_CHATMODE_DEAL			= 0x00000003 ;
	const DWORD		 RYL_CHATMODE_PARTY			= 0x00000004 ;
	const DWORD		 RYL_CHATMODE_GUILD			= 0x00000005 ;

	const DWORD		 RYL_MAX_DRAWCHAT			= 0x00000005 ;
	const DWORD		 RYL_MAX_CHATMESSAGE		= 0x00000064 ;
	const DWORD		 RYL_MAX_DRAWSTALLCHAT		= 0x00000010 ;
} ;

class CRYLChattingDlg : public CRYLDialog
{
public :
	INT					m_iCurrentSize ;
	BOOL				m_bShout ;
	BOOL				m_bTrade ;
	BOOL				m_bParty ;
	BOOL				m_bGuild ;
	
	DWORD				m_dwChatMode ;

	DWORD				m_dwStartStall ;
	DWORD				m_dwEditMode ;
	DWORD				m_dwOldTabFocus ;

	typedef struct Message
	{
		DWORD		dwCommand ;
		FLOAT		fTimer ;
		CHAR		strMessage[ MAX_PATH ] ;
		DWORD		dwColor ;
	} Message ;

	enum eNoticeMsg
	{
		MAX_NOTICE_MESSAGE = 3	// Maximun line of notice
	};

	std::deque<Message *>	m_deqChatMessage ;
	std::deque<Message *>	m_deqStallMessage ;

	CRYLListBox*		m_pChattingListBox ;		// Chatting ListBox

	CRYLCheckBox*		m_pGeneralChatCheckBox ;	// General Chatting CheckBox
	CRYLCheckBox*		m_pShoutChatCheckBox ;		// Shout Chatting CheckBox
	CRYLCheckBox*		m_pDealChatCheckBox ;		// Deal Chatting CheckBox
	CRYLCheckBox*		m_pPartyChatCheckBox ;		// Party Chatting CheckBox
	CRYLCheckBox*		m_pGuildChatCheckBox ;		// Guild Chatting CheckBox

#ifdef _RYL_LANGUAGE
	CRYLCheckBox*		m_pAllChatCheckBox ;		// AllLanguage Chatting CheckBox
#endif

	CRYLLabel*			m_pGeneralChatLabel ;		// General Chatting CheckBox
	CRYLLabel*			m_pShoutChatLabel ;			// Shout Chatting CheckBox
	CRYLLabel*			m_pDealChatLabel ;			// Deal Chatting CheckBox
	CRYLLabel*			m_pPartyChatLabel ;			// Party Chatting CheckBox
	CRYLLabel*			m_pGuildChatLabel ;			// Guild Chatting CheckBox

#ifdef _RYL_LANGUAGE
	CRYLLabel*			m_pAllChatLabel ;			// AllLanguage Chatting CheckBox
#endif

	CRYLButton*			m_pHideButton ;			// Hide
	CRYLButton*			m_pSizeButton ;			// Size
	CRYLEdit*			m_pChatEdit ;			// Chatting Edit
	

	CRYLImage*			m_pChatType1Image ;
	CRYLImage*			m_pChatType2Image ;
	CRYLImage*			m_pChatType3Image ;
	CRYLImage*			m_pChatType4Image ;

	CRYLImage*			m_pChatType1EmptyImage ;
	CRYLImage*			m_pChatType2EmptyImage ;
	CRYLImage*			m_pChatType3EmptyImage ;
	CRYLImage*			m_pChatType4EmptyImage ;

	CRYLCheckBox*		m_pWhisperCheckBox ;		// 귓속말
	CRYLCheckBox*		m_pShoutCheckBox ;			// 외치기
	CRYLCheckBox*		m_pTradeCheckBox ;			// 거래
	CRYLCheckBox*		m_pPartyCheckBox ;			// 파티
	CRYLCheckBox*		m_pGuildCheckBox ;			// 길드
	
	BOOL				m_bHideEnable ;

	unsigned short		m_usLocalLang;		// 컴퓨터의 기본 로케일을 확인한다.

	BOOL				m_bShowTooltip;
#ifdef _RYL_LANGUAGE
	CGMUITooltipBox*	m_lpTooltip;		// 채팅창에서 강제로 사용할 툴팁창
#endif

public :
	CRYLChattingDlg( INT nLeft, INT nTop, INT nDlgType ) ;
	virtual ~CRYLChattingDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	void    namecheckfunction(int check);
	VOID	ProcessChatDlgSize() ;
	VOID	AddChatText( LPSTR pText, DWORD dwColor, DWORD dwChatKind ) ;

	VOID	DeleteStallMessage();

	VOID	AddChatMessage( LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) ;
	
	VOID	AddStallChatMessage( CRYLStallDlg* pDlg, LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) ;
	VOID	AddMessage( LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) ;

	VOID	AttachShoutCmd() ;
	VOID	SetWhisperCharacter( int instance ) ;

	VOID	AdminCommand() ;
	
	VOID	ChatWhisper() ;		// 귓속말
	VOID	ChatShout() ;		// 외치기
	VOID	ChatTrade() ;		// 거래
	VOID	ChatParty() ;		// 파티
	VOID	ChatGuild() ;		// 길드
	VOID	SetWhisperMode() ;

	VOID	ShowButton() ;
	VOID	HideButton() ;
	BOOL	GetHideEnable()							{ return m_bHideEnable ; }
	void	SetHideEnable( BOOL bHideEnable )		{ m_bHideEnable = bHideEnable ; }

	INT		GetChatDlgType()	{ return m_iCurrentSize; }
	void	SaveText( LPSTR strMessage, DWORD dwCommand ) ;

	BOOL	GetGeneralChatChecked()			{ return m_pGeneralChatCheckBox->GetChecked() ; }	// General Chatting CheckBox
	BOOL	GetShoutChatChecked()			{ return m_pShoutChatCheckBox->GetChecked() ; }		// Shout Chatting CheckBox
	BOOL	GetDealChatCheckBox()			{ return m_pDealChatCheckBox->GetChecked() ; }		// Deal Chatting CheckBox
	BOOL	GetPartyChatCheckBox()			{ return m_pPartyChatCheckBox->GetChecked() ; }		// Party Chatting CheckBox
	BOOL	GetGuildChatCheckBox()			{ return m_pGuildChatCheckBox->GetChecked() ; }		// Guild Chatting CheckBox

#ifdef _RYL_LANGUAGE
	BOOL	GetAllChatCheckBox()			{ return m_pAllChatCheckBox->GetChecked() ; }		// MultiByte Chatting CheckBox
#endif
	
	BOOL	CheckCharSet(CHAR* str, int Len);
	BOOL	CheckAisaLang(unsigned short usLang);

	//-- 2004. . . Zergra From.--//
public:
	VOID    SetFocusChattingEdit();	  
	//-- Zergra To. --//
	
	// Signton
public :
	static CRYLChattingDlg*	s_pInstance ;
	static CRYLChattingDlg*	Instance()			{ return s_pInstance ; } ;
} ;

#endif // __RYL_CHATTINGDLG_H__