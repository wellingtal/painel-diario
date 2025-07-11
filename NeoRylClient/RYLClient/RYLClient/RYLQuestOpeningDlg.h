//================================================================
//
//	Name : RYLQuestOpeningDlg.h 
//	Desc : ����Ʈ ������ ���̾�α�
//  Date : 2004. 01. 05
//
//================================================================
#ifndef __RYL_QUESTOPENINGDLG_H__
#define __RYL_QUESTOPENINGDLG_H__

#include "RYLDialog.h"
#include <vector>
#include <Item/ItemStructure.h>

class CRYLButton ;
class CRYLListBox ;
class CRYLLabel ;

namespace RYLQUESTOPENINGDLG
{
	const DWORD		ACCEPT_BUTTON		= 0x00000000 ;
	const DWORD		REFUSE_BUTTON		= 0x00000001 ;
} ;

typedef struct Message
{
	CHAR	strMessage[ MAX_PATH ] ;
}Message, *LPMessage ;


class CRYLQuestOpeningDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pQuestOpeningLabel ;		// ����Ʈ ������
	CRYLLabel*			m_pQuestNPCTitleLabel ;		// ����Ʈ
	CRYLLabel*			m_pQuestLevelLabel ;		// ����Ʈ ������
	CRYLLabel*			m_pQuestLevelValueLabel ;	// ����Ʈ ������ ��
	CRYLLabel*			m_pQuestCompensationLabel ;	// ����Ʈ ����
	CRYLLabel*			m_pQuestTitleLabel ;		// ����Ʈ ����
	CRYLButton*			m_pAcceptButton ;			// ������ư
	CRYLButton*			m_pRefuseButton ;			// ������ư
	CRYLListBox*		m_pQuestPurposeButton ;		// ����Ʈ���� ����Ʈ
	CRYLListBox*		m_pQuestCompensationButton ;// ����Ʈ���� ����Ʈ

	std::vector< LPMessage >	m_lstMessage ;
	DWORD						m_dwChrID;
	DWORD						m_dwQuestID ;
	LONG						m_lStartDesc ;
	CHAR						m_strQuestTitle[ MAX_PATH ] ;
	CHAR						m_strQuestLevel[ MAX_PATH ] ;
	std::vector< LPMessage >	m_lstQuestAward ;
	Item::ItemPos				m_QuestItemPos ;

public :
	CRYLQuestOpeningDlg() ;
	virtual ~CRYLQuestOpeningDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	SetText( DWORD dwChrID, DWORD dwQuestID, LPCTSTR strQuestTitle, LPCSTR strQuestLevel,
					 LPCTSTR strMessage, LPCSTR strQuestAward ) ;

	VOID	SetQuestItemPos(Item::ItemPos itemPos)		{ m_QuestItemPos = itemPos;	}

	DWORD	GetCharID()					{ return m_dwChrID ; } 
	DWORD	GetQuestID()				{ return m_dwQuestID ; }
	Item::ItemPos	GetQuestItemPos()	{ return m_QuestItemPos; }
} ;

#endif // __RYL_QUESTOPENINGDLG_H__