//================================================================
//
//	Name : RYLPartyPanel.h 
//	Desc : ��Ƽ �ǳ�
//  Date : 2003. 12. 10
//
//================================================================
#ifndef __RYL_PARTYPANEL_H__
#define __RYL_PARTYPANEL_H__

#include "GMUICustomPanel.h"
#include "RYLCheckBox.h"
#include <vector>

class CRYLLabel ;
class CRYLButton ;
class CRYLStringGrid ;
class CRYLSprite ;
class CRYLSpriteEx;

namespace RYLPARTYPANEL
{
	const DWORD		AUTOROUTING_CHECKBOX1			= 0x00000000 ;
	const DWORD		AUTOROUTING_CHECKBOX2			= 0x00000001 ;
	const DWORD		AUTOROUTING_CHECKBOX3			= 0x00000002 ;
	const DWORD		AUTOROUTING_CHECKBOX4			= 0x00000003 ;
	const DWORD		AUTOROUTING_CHECKBOX5			= 0x00000004 ;
	const DWORD		AUTOROUTING_CHECKBOX6			= 0x00000005 ;
	const DWORD		AUTOROUTING_CHECKBOX7			= 0x00000006 ;
	const DWORD		AUTOROUTING_CHECKBOX8			= 0x00000007 ;
	const DWORD		AUTOROUTING_CHECKBOX9			= 0x00000008 ;
	const DWORD		AUTOROUTING_CHECKBOX10			= 0x00000009 ;
	
	const DWORD		AUTOROUTING_CHECKBOX			= 0x00000010 ;
	const DWORD		PARTY_STRINGGRID				= 0x00000011 ;
	const DWORD		BANISH_BUTTON					= 0x00000012 ;
	const DWORD		TRANSFER_BUTTON					= 0x00000013 ;
	const DWORD		PARTYLEAVE_BUTTON				= 0x00000014 ;
	const DWORD		MINIPARTY_BUTTON				= 0x00000015 ;
	const DWORD		ALLAUTOROUTING_BUTTON			= 0x00000016 ;
} ;

typedef struct __PARTYMEMBERINFO
{
	INT				iIndex ;
	CRYLCheckBox*	pAutoRoutingCBox ;
	CRYLSprite*		pGuildMark ;
	
	__PARTYMEMBERINFO();
	~__PARTYMEMBERINFO();

} PARTYMEMBERINFO, *LPPARTYMEMBERINFO ;

class CRYLPartyPanel : public CGMUICustomPanel
{
private :

	CRYLLabel*			m_pNameLabel1 ;
	CRYLLabel*			m_pClassLabel1 ;
	CRYLLabel*			m_pLevelLabel1 ;
	CRYLLabel*			m_pRoutingLabel1 ;

	CRYLLabel*			m_pNameLabel2 ;
	CRYLLabel*			m_pClassLabel2 ;
	CRYLLabel*			m_pLevelLabel2 ;
	CRYLLabel*			m_pGuildLabel2 ;
	CRYLLabel*			m_pPlaceLabel2 ;

	CRYLLabel*			m_pNameLabelValue ;				// �̸� ��
	CRYLLabel*			m_pClassLabelValue ;			// Ŭ���� ��
	CRYLLabel*			m_pLevelLabelValue ;			// ���� ��
	CRYLLabel*			m_pPlaceValueLabel ;
	CRYLLabel*			m_pZoneLabelValue ;				// ��ġ

	CRYLSpriteEx*		m_pGuildMarkSprite;             // ��� ��ũ
	
    CRYLStringGrid*		m_pStringGrid ;					// �׸��� 

	CRYLButton*			m_pExpulsionButton ;			// ��Ƽ���� ����
	CRYLButton*			m_pTransferButton ;				// ������ �絵
	CRYLButton*			m_pPartyLeave ;					// ��Ƽ Ż��
	CRYLButton*			m_pMiniParty ;					// �̴���Ƽ
	CRYLButton*			m_pAllAutoroutingButton ;		// ��ü �������

	CHAR				m_szSelectedMember[32];			// ���õ� ��� �̸�


public :

	LPPARTYMEMBERINFO	m_pPartyControl ;
	
	BOOL				m_bAutoRouting ;				// ������� �÷���


public :

	CRYLPartyPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLPartyPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	VOID		UpdateSelectPartyMember() ;
	VOID		UpdateAutoRouting() ;

	INT			GetItemIndexByName( LPSTR szName ) ;
	LPSTR		GetSelectedMemberName()			{ return m_szSelectedMember ; }

	VOID		DeletePartyList() ;
	VOID		UpdateControl() ;
	VOID		UpdatePartyList() ;
	VOID		CaptionClear() ;
	
	CRYLStringGrid*	GetStringGrid()				{ return m_pStringGrid ; } ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgDBClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	virtual VOID                Show( CGMUIObject* Sender );
	virtual VOID                Hide( CGMUIObject* Sender );
} ;

#endif //__RYL_STATUSPANEL_H__

