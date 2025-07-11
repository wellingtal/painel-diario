//================================================================
//
//	Name : RYLPartyFindPanel.h 
//	Desc : 파티찾기 판넬
//  Date : 2004. 01. 15
//
//================================================================
#ifndef __RYL_PARTYFINDPANEL_H__
#define __RYL_PARTYFINDPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel ;
class CRYLButton ;
class CRYLCheckBox ;
class CRYLListBox ;
class CRYLStringGrid ;

namespace RYLPARTYFINDPANEL
{
	const DWORD		FINDPERSONAL_CHECKBOX			= 0x00000000 ;
	const DWORD		MEMBERFINDPARTY_CHECKBOX		= 0x00000001 ;
	const DWORD		REFRESH_BUTTON					= 0x00000002 ;
	const DWORD		PFMP_STRINGGRID					= 0x00000003 ;
	const DWORD		MFPM_STRINGGRID					= 0x00000004 ;
} ;

class CRYLPartyFindPanel : public CGMUICustomPanel
{
private :
	CRYLLabel*			m_pPartyFindPersonalLabel ;		// 파티를 찾고 있는 개인
	CRYLLabel*			m_pPartyMemberFindPartyLabel ;	// 파티원을 찾고 있는 파티
	CRYLLabel*			m_pEntryLabel1 ;				// 등록
	CRYLLabel*			m_pEntryLabel2 ;				// 등록
	CRYLButton*			m_pRefreshButton ;				// 새로고침
	
	CRYLStringGrid*		m_pPartyForMemberPartyGrid ;	// 파티원을 찾고 있는 파티
	CRYLStringGrid*		m_MemberForPartyMemberGrid ;	// 파티를 찾고 있는 개인 리스트 박스

	CRYLLabel*			m_pNameLabel ;					// 이름
	CRYLLabel*			m_pClassLabel ;					// 클레스
	CRYLLabel*			m_pLevelLabel ;					// 레벨
	CRYLLabel*			m_pLeaderNameLabel ;			// 리더이름
	CRYLLabel*			m_pMemberNumLabel ;				// 멤버수
	CRYLLabel*			m_pAverageLevelLabel ;			// 평균레벨
public :
	CRYLCheckBox*		m_pEntryCheckBox1 ;				// 등록 체크
	CRYLCheckBox*		m_pEntryCheckBox2 ;				// 등록 체크
	
public :
	CRYLPartyFindPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLPartyFindPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;
	HRESULT		UpdateMemberForPartyMember() ;
	HRESULT		UpdatePartyForMemberParty() ;

	VOID		UpdateCheckBox(BOOL bFindChar, BOOL bFindParty);

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID PartyFindMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID		PartyFindEvent() ;
	VOID		PartyPlayerFind() ;
	VOID		RefreshEvent() ;
	VOID		PartyFindWhisperEvent() ;
	VOID		PartyPlayerWhisperEvent() ;
} ;

#endif //__RYL_PARTYFINDPANEL_H__