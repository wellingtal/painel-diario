//================================================================
//
//	Name : RYLTaxrateDlg.h 
//	Desc : �������� ���̾�α�
//  Date : 2004. 05. 28
//
//================================================================
#ifndef __RYL_TAXRATEDLG_H__
#define __RYL_TAXRATEDLG_H__

#include "RYLDialog.h"

namespace RYLTAXRATEDLG
{
	const DWORD		TAX_EDIT			= 0x00000000 ;// ���� ����Ʈ �ڽ�
	const DWORD		OK_BUTTON			= 0x00000001 ;// Ȯ�� ��ư
	const DWORD		CANCEL_BUTTON		= 0x00000002 ;// ��� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLTaxrateDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pTaxrateLabel ;			// ��������
	CRYLLabel*			m_pCurrentTaxLabel ;		// ���� ������ 000%�Դϴ�.
	CRYLLabel*			m_pChangeTaxLabel ;			// ������ ���� �ϰڽ��ϱ�? ( 0 - 100% )
	CRYLLabel*			m_pTaxpercentLabel ;		// %
	CRYLEdit*			m_pTaxEdit ;				// ���� ����Ʈ �ڽ�
	CRYLButton*			m_pOKButton ;				// Ȯ��
	CRYLButton*			m_pCancelButton ;			// ���
	
public :
	CRYLTaxrateDlg() ;
	virtual ~CRYLTaxrateDlg() ;

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
} ;

#endif // __RYL_TAXRATEDLG_H__