//================================================================
//
//	Name : RYLGMReportDlg.h 
//	Desc : GM신고 다이얼로그 박스
//  Date : - Relese 2010. 01. 23
//
//================================================================
#ifndef __RYLGMREPORTDLG_H__
#define __RYLGMREPORTDLG_H__

#include "RYLDialog.h"

namespace RYLGMREPORTDLG
{
	const DWORD		SEND_BUTTON				= 0x00000000 ;
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;
	const DWORD		REPORTLIST_COMBO		= 0x00000002 ;
	const DWORD		REPORTID_EDITBOX		= 0x00000003 ;
	const DWORD		REPORTDATA_EDITBOX		= 0x00000004 ;
	const DWORD		GMREPORT_LABLE			= 0x00000005 ;
	const DWORD		GMREPORTLIST_LABLE		= 0x00000006 ;
	const DWORD		GMREPORTID_LABLE		= 0x00000007 ;
	const DWORD		GMREPORTDATA_LABLE		= 0x00000008 ;

} ;

class CRYLButton ;
class CRYLEdit ;
class CRYLComboBox ;
class CRYLLabel ;

class CRYLGMReportDlg : public CRYLDialog
{
public:
	CRYLButton*			m_pSendBtn ;
	CRYLButton*			m_pCancelBtn ;
	CRYLComboBox*		m_pReportListCombo ;
	CRYLEdit*			m_pReportIDEdit ;
	CRYLEdit*			m_pReportDataEdit ;

	CRYLLabel*			m_pGMReportLabel ;
	CRYLLabel*			m_pGMReportListLabel ;
	CRYLLabel*			m_pGMReportIDLabel ;
	CRYLLabel*			m_pGMReportDataLabel ;

public :
	CRYLGMReportDlg() ;
	virtual ~CRYLGMReportDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

public :
	VOID	SendButtonDown() ;

public :

	// Signton
//	static CRYLGMReportDlg*		s_pInstance ;
//	static CRYLGMReportDlg*		Instance()				{ return s_pInstance; }

} ;

#endif //__RYLGMREPORTDLG_H__

