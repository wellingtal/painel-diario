//================================================================
//
//	Name : RYLPartChoiceDlg.h 
//	Desc : PartII다운로드 및 PartI 접속
//
//================================================================
#ifndef __RYL_RYLPARTCHOICE_H__
#define __RYL_RYLPARTCHOICE_H__

#include "RYLDialog.h"

namespace RYLPARTCHOICE
{
	const DWORD		DOWN_BUTTON				= 0x00000000 ;
	const DWORD		PART1_BUTTON			= 0x00000001 ;
	const DWORD		DESC_LISTBOX			= 0x00000002 ;
	const DWORD		TODAYSHOW_CBBOX			= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLListBox ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLPartChoiceDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pRYLDownButton ;
	CRYLButton*			m_pPart1Button ;
	CRYLListBox*		m_pDescListBox ;
	CRYLCheckBox*		m_pTodayCheckBox ;
	CRYLLabel*			m_pDescLabel ;

public :
	CRYLPartChoiceDlg() ;
	virtual ~CRYLPartChoiceDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif //__RYL_RYLPARTCHOICE_H__