//================================================================
//
//	Name : RYLQuestDetailDataDlg.h 
//	Desc : 퀘스트 상세내역 다이얼로그
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
	CRYLLabel*			m_pQuestTitle ;					// 퀘스트 제목값
	CRYLLabel*			m_pQuestLevel ;					// 퀘스트 레벨
	CRYLLabel*			m_pQuestPurposeLabel ;			// 퀘스트 목적
	CRYLLabel*			m_pQuestPhase ;					// 퀘스트 단계
	CRYLLabel*			m_pExecutionReportLabel ;		// 퀘스트 수행과제
	CRYLLabel*			m_pQuestCompensationLabel ;		// 퀘스트 보상
	CRYLLabel*			m_pTargetObjectLabel ;			// 대상물
	CRYLLabel*			m_pTargetObjectValueLabel ;		// 대상물값
	CRYLLabel*			m_pProgressStateLabel ;			// 진행상황
	CRYLLabel*			m_pProgressStateValueLabel ;	// 진행상황 값

	CRYLListBox*		m_pQuestPurposeListBox ;		// 퀘스트목적 리스트
	CRYLListBox*		m_pExecutionReportListBox ;		// 수행과제 리스트
	CRYLListBox*		m_pQuestCompensationListBox ;	// 퀘스트 보상 리스트
	
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