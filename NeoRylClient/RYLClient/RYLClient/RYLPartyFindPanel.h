//================================================================
//
//	Name : RYLPartyFindPanel.h 
//	Desc : ��Ƽã�� �ǳ�
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
	CRYLLabel*			m_pPartyFindPersonalLabel ;		// ��Ƽ�� ã�� �ִ� ����
	CRYLLabel*			m_pPartyMemberFindPartyLabel ;	// ��Ƽ���� ã�� �ִ� ��Ƽ
	CRYLLabel*			m_pEntryLabel1 ;				// ���
	CRYLLabel*			m_pEntryLabel2 ;				// ���
	CRYLButton*			m_pRefreshButton ;				// ���ΰ�ħ
	
	CRYLStringGrid*		m_pPartyForMemberPartyGrid ;	// ��Ƽ���� ã�� �ִ� ��Ƽ
	CRYLStringGrid*		m_MemberForPartyMemberGrid ;	// ��Ƽ�� ã�� �ִ� ���� ����Ʈ �ڽ�

	CRYLLabel*			m_pNameLabel ;					// �̸�
	CRYLLabel*			m_pClassLabel ;					// Ŭ����
	CRYLLabel*			m_pLevelLabel ;					// ����
	CRYLLabel*			m_pLeaderNameLabel ;			// �����̸�
	CRYLLabel*			m_pMemberNumLabel ;				// �����
	CRYLLabel*			m_pAverageLevelLabel ;			// ��շ���
public :
	CRYLCheckBox*		m_pEntryCheckBox1 ;				// ��� üũ
	CRYLCheckBox*		m_pEntryCheckBox2 ;				// ��� üũ
	
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