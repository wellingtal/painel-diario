//================================================================
//
//	Name : RYLFritnePanel.h 
//	Desc : 친구리스트 판넬
//  Date : 2003. 12. 16
//
//================================================================
#ifndef __RYL_FRIENDPANEL_H__
#define __RYL_FRIENDPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ;
class CRYLSpriteEx;
struct FriendInfo ;

namespace RYLFRIENDPANEL
{
	const DWORD		FRIEND_STRINGGRID				= 0x00000000 ;
	const DWORD		DELFRIENDLIST_BUTTON			= 0x00000001 ;
	const DWORD		ADDREFUSALLIST_BUTTON			= 0x00000002 ;
	const DWORD		PAGEPREV_BUTTON					= 0x00000003 ;
	const DWORD		PAGENEXT_BUTTON					= 0x00000004 ;
} ;

class CRYLFriendPanel : public CGMUICustomPanel
{
private :
	CRYLLabel*			m_pNameLabel ;					// 이름
	CRYLLabel*			m_pClassLabel ;					// 클레스
	CRYLLabel*			m_pLevelLabel ;					// 레벨

	CRYLStringGrid*		m_pStringGrid ;					// 그리드 

	CRYLLabel*			m_pInfoNameLabel ;
	CRYLLabel*			m_pInfoGuildLabel ;
	CRYLLabel*			m_pInfoClassLabel ;
	CRYLLabel*			m_pInfoLevelLabel ;
	CRYLLabel*			m_pInfoPlaceLabel ;

	CRYLLabel*			m_pNameLabelValue ;				// 이름 값
	CRYLLabel*			m_pClassLabelValue ;			// 클래스 값
	CRYLLabel*			m_pLevelLabelValue ;			// 레벨 값
	CRYLLabel*			m_pPlaceValueLabel ;			// 장소 값
	CRYLLabel*			m_pFriendCmdLabel ;				// 친구등록 설명
	CRYLLabel*			m_pZoneLabelValue ;				// 위치

	CRYLButton*			m_pDelFriendListButton ;		// 친구 리스트에 삭제
	CRYLButton*			m_pAddResusalListButton ;		// 거부 리스트에 추가
	CRYLLabel*			m_pPageInfoLabel ;				// 페이지 정보
	CRYLButton*			m_pPagePrevButton ;				// 이전 페이지
	CRYLButton*			m_pPageNextButton ;				// 다음 페이지

	DWORD				m_dwRemoveResult ;
	DWORD				m_dwAddResult ;
	DWORD				m_dwCommandType ;
	DWORD				m_dwChrID ;
	CHAR				m_strName[ MAX_PATH ] ;

	DWORD				m_dwStartSlot ;
	CHAR				m_szSelectedMember[32];			// 선택된 멤버 이름	

	CRYLSpriteEx*		m_pGuildMarkSprite;             // 길드 마크

public :
	CRYLFriendPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLFriendPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ; 
	static VOID DlgDBClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID		DelFriendListEvent() ;
	VOID		AddResusalListEvent() ;
	HRESULT		FriendListUpdate() ;
	VOID		SetWhisperCharacter() ;
	VOID		ClearWhisperCharacter(char* szName);
	VOID		UpdateWhisperCharacter(char* szName);
	VOID		ClearWhisperCharacter();

	VOID		FriendListPrev() ;
	VOID		FriendListNext() ;
	INT			GetCurrentPageLine() ;

	LPSTR		GetSelectedMemberName()			{ return m_szSelectedMember ; }

	std::list< FriendInfo* >::iterator		FindIter( INT iIndex ) ;

	virtual VOID                Show( CGMUIObject* Sender );
	virtual VOID                Hide( CGMUIObject* Sender );
} ;

#endif //__RYL_FRIENDPANEL_H__

