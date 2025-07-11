//================================================================
//
//	Name : RYLStatusAndQuestFrameDlg.h 
//	Desc : 스테이터스 및 퀘스트 다이얼로그
//  Date : 2003. 12. 10
//
//================================================================
#ifndef __RYL_STATUSANDQUESTFRAMEDLG_H__
#define __RYL_STATUSANDQUESTFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CGMUICustomPanel ;

namespace RYLSTATUSANDQUESTODLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
} ;

class CRYLStatusAndQuestFrameDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pStatusPanel ;
	CGMUICustomPanel*	m_pExecutionQuestPanel ;
	CGMUICustomPanel*	m_pCompleteQuestPanel ;
	
	
public :
	CRYLStatusAndQuestFrameDlg() ;
	virtual ~CRYLStatusAndQuestFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
    virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLTabButton*		GetTabButton()				{ return m_pTabButton ; } ;
	CGMUICustomPanel*	GetStatusPanel()			{ return m_pStatusPanel ; } ;
	CGMUICustomPanel*	GetExecutionQuestPanel()	{ return m_pExecutionQuestPanel ; } ;
	CGMUICustomPanel*	GetCompleteQuestPanel()		{ return m_pCompleteQuestPanel ; } ;

	VOID		ShowStatusPanel() ;
	VOID		ShowExcutionQuestPanel() ;
} ;

#endif // __RYL_STATUSANDQUESTFRAMEDLG_H__