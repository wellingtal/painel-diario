//================================================================
//
//	Name : RYLRefusalPanel.h 
//	Desc : �źθ���Ʈ �ǳ�
//  Date : 2003. 12. 16
//
//================================================================
#ifndef __RYL_REFUSALPANEL_H__
#define __RYL_REFUSALPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ;
class CRYLSpriteEx;
struct BanInfo ;

namespace RYLREFUSALPANEL
{
	const DWORD		FRIEND_STRINGGRID				= 0x00000000 ;
	const DWORD		DELREFUSALLIST_BUTTON			= 0x00000001 ;
	const DWORD		ADDFRIENDLIST_BUTTON			= 0x00000002 ;
	const DWORD		PAGEPREV_BUTTON					= 0x00000003 ;
	const DWORD		PAGENEXT_BUTTON					= 0x00000004 ;
} ;

class CRYLRefusalPanel : public CGMUICustomPanel
{
private :
	CRYLLabel*			m_pNameLabel ;					// �̸�
	CRYLLabel*			m_pClassLabel ;					// Ŭ����
	CRYLLabel*			m_pLevelLabel ;					// ����

	CRYLStringGrid*		m_pStringGrid ;					// �׸��� 

	CRYLLabel*			m_pInfoNameLabel ; 
	CRYLLabel*			m_pInfoGuildLabel ;
	CRYLLabel*			m_pInfoClassLabel ;
	CRYLLabel*			m_pInfoLevelLabel ;
	CRYLLabel*			m_pInfoPlaceLabel ;

	CRYLLabel*			m_pNameLabelValue ;				// �̸� ��
	CRYLLabel*			m_pClassLabelValue ;			// Ŭ���� ��
	CRYLLabel*			m_pLevelLabelValue ;			// ���� ��
	CRYLLabel*			m_pPlaceValueLabel ;			// ��� ��
	
	CRYLLabel*			m_pRefusalCmdLabel ;

	CRYLLabel*			m_pZoneLabelValue ;				// ��ġ

	CRYLButton*			m_pDelResusalListButton ;		// �ź� ����Ʈ�� ����
	CRYLButton*			m_pAddFriendListButton ;		// ģ�� ����Ʈ�� �߰�
	CRYLLabel*			m_pPageInfoLabel ;				// ������ ����
	CRYLButton*			m_pPagePrevButton ;				// ���� ������
	CRYLButton*			m_pPageNextButton ;				// ���� ������
    
	DWORD				m_dwRemoveResult ;
	DWORD				m_dwAddResult ;
	DWORD				m_dwCommandType ;
	DWORD				m_dwChrID ;
	CHAR				m_strName[ MAX_PATH ] ;

	DWORD				m_dwStartSlot ;
	CHAR				m_szSelectedMember[32];			// ���õ� ��� �̸�

	CRYLSpriteEx*		m_pGuildMarkSprite;             // ��� ��ũ
	

public :
	CRYLRefusalPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLRefusalPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgDBClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID		DelResusalListEvent() ;
	VOID		AddFriendListEvent() ;
	HRESULT		ResusalListUpdate() ;
	VOID		SetWhisperCharacter() ;

	VOID		ClearWhisperCharacter(char* szName);
	VOID		UpdateWhisperCharacter(char* szName);
	VOID		ClearWhisperCharacter();

	VOID		RefusalListPrev() ;
	VOID		RefusalListNext() ;
	INT			GetCurrentPageLine() ;

	LPSTR		GetSelectedMemberName()			{ return m_szSelectedMember ; }

	std::list< BanInfo* >::iterator		FindIter( INT iIndex ) ;

	virtual VOID                Show( CGMUIObject* Sender );
	virtual VOID                Hide( CGMUIObject* Sender );
} ;

#endif //__RYL_REFUSALPANEL_H__

