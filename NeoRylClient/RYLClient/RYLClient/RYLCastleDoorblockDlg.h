//================================================================
//
//	Name : RYLCastleDoorblockDlg.h 
//	Desc : �������� ���̾�α�
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_CASTLEDOORBLOCKDLG_H__
#define __RYL_CASTLEDOORBLOCKDLG_H__

#include "RYLDialog.h"

namespace RYLCASTLEDOORBLOCKDLG
{
	const DWORD		DAMAGEDEC_EDIT			= 0x00000000 ;// ������ ������
	const DWORD		REMAINHP_EDIT			= 0x00000001 ;// ������ ���� HP
	const DWORD		CANCEL_BUTTON			= 0x00000002 ;// ���� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLCastleDoorblockDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pCastleDoorLabel ;		// ��������
	CRYLLabel*			m_pDamageDecLabel ;			// ������ ������(%)
	CRYLLabel*			m_pRemainHPLabel ;			// ������ ���� HP
	CRYLLabel*			m_pDamageDecLabelValue ;			// ������ ������(%)
	CRYLLabel*			m_pRemainHPLabelValue ;			// ������ ������(%)
	CRYLButton*			m_pCancelButton ;			// ��� ��ư
	
public :
	CRYLCastleDoorblockDlg() ;
	virtual ~CRYLCastleDoorblockDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	void	SetData( unsigned long ulDec, unsigned long ulRemainHP ) ;
} ;

#endif // __RYL_CASTLEDOORBLOCKDLG_H__