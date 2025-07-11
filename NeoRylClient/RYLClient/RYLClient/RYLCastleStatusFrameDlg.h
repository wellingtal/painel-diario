//================================================================
//
//	Name : RYLCastleStatusFrameDlg.h 
//	Desc : ���� ��Ȳ ������ ���̾�α�
//  Date : 2003. 12. 15
//
//================================================================

#ifndef __RYL_CASTLESTATUSFRAMEDLG_H__
#define __RYL_CASTLESTATUSFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton ;
class CGMUICustomPanel ;

namespace RYLCASTLEDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000 ;
} ;

class CRYLCastleStatusFrameDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pCastleStatusPanel ;	// ���� ��Ȳ �г�
	CGMUICustomPanel*	m_pTaxControlPanel ;	// ���� ���� �г�
	CGMUICustomPanel*	m_pTaxCollectPanel ;	// ���� ȸ�� �г�

	unsigned long		m_dwCurrCastleID ;		// ���� ���õ� �� ID

public :
	CRYLCastleStatusFrameDlg() ;
	virtual ~CRYLCastleStatusFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLTabButton*		GetTabButton()				{ return m_pTabButton ;			}
	CGMUICustomPanel*	GetCastleStatusPanel()		{ return m_pCastleStatusPanel ;	}
	CGMUICustomPanel*	GetTaxControlPanel()		{ return m_pTaxControlPanel ;	}
	CGMUICustomPanel*	GetTaxCollectPanel()		{ return m_pTaxCollectPanel ;	}

	void	SetCastleID( unsigned long dwCastleID ) ;
} ;

#endif // __RYL_CASTLESTATUSFRAMEDLG_H__