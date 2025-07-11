//================================================================
//
//	Name : RYLNationDlg.h 
//	Desc : ���� ���� ������ ǥ���ϴ� ���̾�α�
//  Date : 2004. 11. 16
//
//================================================================

// WORK_LIST 2.3 ���� ���� ���� UI �߰�

#ifndef __RYL_NATION_DLG_H__
#define __RYL_NATION_DLG_H__

#include "RYLDialog.h"

namespace RYLNATIONDLG
{
	const DWORD		KARTERANT_BUTTON	= 0x00000000 ;
	const DWORD		MERKADIA_BUTTON		= 0x00000001 ;
	const DWORD		PIRATE_BUTTON		= 0x00000002 ;
} ;

class CRYLButton ;
class CRYLLabel ;

//===========================================================
// Nation Dlg
//===========================================================
class CRYLNationDlg : public CRYLDialog
{
private:
	CRYLButton*		m_pKarterantBtn ;	// ���� ���� ���� ��ư (ī���׶�Ʈ ���� ��� ǥ��)
	CRYLButton*		m_pMerkadiaBtn ;	// ���� ���� ���� ��ư (�޸�ī��� ���� ��� ǥ��)
	CRYLButton*		m_pPirateBtn ;		// ���� ���� ���� ��ư (���� ������ ���� ��� ǥ��)

	CRYLLabel*		m_pDescLabel ;		// ����
	CRYLLabel*		m_pNationLabel ;	// ���� �̸�

	DWORD			m_dwCurrentNation ; // ���� ���� ����

public :
	CRYLNationDlg() ;
	virtual ~CRYLNationDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )	;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ;

	VOID	SetCurrentNation( DWORD dwNation ) ;
} ;


#endif //__RYL_NATION_DLG_H__
