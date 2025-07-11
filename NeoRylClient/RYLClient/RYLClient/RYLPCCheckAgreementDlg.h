//================================================================
//
//	Name : RYLPCCheckAgreementDlg.h 
//	Desc : 이관 체크.
//
//================================================================
#ifndef __RYL_RYLPCCHECKAGREEMENT_H__
#define __RYL_RYLPCCHECKAGREEMENT_H__

#include "RYLDialog.h"

namespace RYLPCCHECKAGREENMENT
{
	const DWORD		YES_BUTTON				= 0x00000000;
	const DWORD		NO_BUTTON				= 0x00000001;	
	const DWORD		PCCHECKAGREE_LISTBOX	= 0x00000002;	
} ;

class CRYLButton;
class CRYLLabel;
class CRYLListBox;

class CRYLPCCheckAgreementDlg : public CRYLDialog
{
private:

	CRYLButton*			m_pYesButton;
	CRYLButton*			m_pNoButton;

	CRYLLabel*			m_pCaption;
	
	CRYLListBox*		m_pTextListBox;

public :
	CRYLPCCheckAgreementDlg() ;
	virtual ~CRYLPCCheckAgreementDlg() ;

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

#endif //__RYL_RYLPCCHECKAGREEMENT_H__