//================================================================
//
//	Name : RYLTaxpresentDlg.h 
//	Desc : ���� ��Ȳ ���̾�α�
//  Date : 2004. 05. 31
//
//================================================================
#ifndef __RYL_TAXPRESENTDLG_H__
#define __RYL_TAXPRESENTDLG_H__

#include "RYLDialog.h"

namespace RYLTAXPRESENTDLG
{
	const DWORD		OK_BUTTON				= 0x00000003 ;// Ȯ�� ��ư
};

class CRYLButton ;
class CRYLLabel ;
class CRYLEdit ;

class CRYLTaxpresentDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pTaxpresentLabel ;			// ���� ��Ȳ
	CRYLLabel*			m_pTaxLabel ;					// ����
	CRYLLabel*			m_pTaxValueLabel ;				// ����
	CRYLLabel*			m_pTaxLevyPresentLabel ;		// ���� ¡�� ��Ȳ
	CRYLLabel*			m_pTaxLevyPresentValueLabel ;	// ���� ¡�� ��Ȳ
	CRYLLabel*			m_pTaxLevyExpirationLabel ;		// ���� ¡�� ����
	CRYLLabel*			m_pTaxLevyExpirationValueLabel ;// ���� ¡�� ����
	CRYLButton*			m_pOKButton ;					// Ȯ�� ��ư	
	
public :
	CRYLTaxpresentDlg() ;
	virtual ~CRYLTaxpresentDlg() ;

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
	
	VOID	SetData( unsigned long dwCurrentTax,
		             unsigned long dwTaxLevyPresent,
					 unsigned long dwTaxLevyExpiration ) ;
} ;

#endif // __RYL_TAXPRESENTDLG_H__