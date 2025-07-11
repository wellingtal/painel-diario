//================================================================
//
//	Name : RYLQuestDetailDataDlg.h 
//	Desc : ����Ʈ �󼼳��� ���̾�α�
//  Date : 2004. 01. 07
//
//================================================================
#ifndef __RYL_QUESTDETAILDATADLG_H__
#define __RYL_QUESTDETAILDATADLG_H__

#include "RYLDialog.h"
#include <vector>

class CRYLListBox ;
class CRYLLabel ;
struct QuestInstance ;

class CRYLQuestDetailDataDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pQuestTitle ;					// ����Ʈ ����
	CRYLLabel*			m_pQuestLevel ;					// ����Ʈ ����
	CRYLLabel*			m_pQuestPurposeLabel ;			// ����Ʈ ����
	CRYLLabel*			m_pQuestPhase ;					// ����Ʈ �ܰ�
	CRYLLabel*			m_pExecutionReportLabel ;		// ����Ʈ �������
	CRYLLabel*			m_pQuestCompensationLabel ;		// ����Ʈ ����
	CRYLLabel*			m_pTargetObjectLabel ;			// ���
	CRYLLabel*			m_pTargetObjectValueLabel ;		// ��󹰰�
	CRYLLabel*			m_pProgressStateLabel ;			// �����Ȳ
	CRYLLabel*			m_pProgressStateValueLabel ;	// �����Ȳ ��

	CRYLListBox*		m_pQuestPurposeListBox ;		// ����Ʈ���� ����Ʈ
	CRYLListBox*		m_pExecutionReportListBox ;		// ������� ����Ʈ
	CRYLListBox*		m_pQuestCompensationListBox ;	// ����Ʈ ���� ����Ʈ
	
public :
	CRYLQuestDetailDataDlg() ;
	virtual ~CRYLQuestDetailDataDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	
	VOID	SetDetailDataDlg( QuestInstance* lpQuestInstance ) ;
	VOID	UpdateQuestMsg( QuestInstance* lpQuestInstance );

	LPSTR	GetQuestTitle() ;
	VOID	SetQuestTitle( LPSTR strTitle ) ;
} ;

#endif // __RYL_QUESTDETAILDATADLG_H__