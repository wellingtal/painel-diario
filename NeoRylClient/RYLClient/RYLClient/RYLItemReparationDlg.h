//================================================================
//
//	Name : RYLItemReparationDlg.h 
//	Desc : ������ ���� �Ǹ� ���̾�α�
//  Date : 2004. 06. 21
//
//================================================================
#ifndef __RYL_ITEMREPARATIONDLG_H__
#define __RYL_ITEMREPARATIONDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	


namespace RYLITEMREPARATIONDLG
{
	const DWORD		OK_BUTTON						= 0x00000000 ;
	const DWORD		CANCEL_BUTTON					= 0x00000001 ;
	
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;

class CRYLItemReparationDlg : public CRYLDialog
{
private :

	CRYLButton*			m_pOKButton ;					// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton ;				// ��� ��ư

	CRYLLabel*			m_pDlgLabel;					// ���̾�α� ��
	CRYLLabel*			m_pSrcItemLabel;				// ���� ������ ��
	CRYLLabel*			m_pDestItemLabel;				// ��ȯ ������ ��

	CRYLLabel*			m_pDescriptionLabel1 ;			// ���� ù°��
	CRYLLabel*			m_pDescriptionLabel2 ;			// ���� ��°��
	CRYLLabel*			m_pDescriptionLabel3 ;			// ���� ��°��

	CRYLLabel*			m_pCostValueLabel ;				// ��� ��
	CRYLLabel*			m_pCostLabel ;					// ���

	long				m_lCompensationGold;			// ȯ�� �ݾ�
	unsigned char		m_cResultErr;					// ������


public :

	CRYLItemReparationDlg() ;
	virtual ~CRYLItemReparationDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	bool OK(void);

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_ITEMREPARATIONDLG_H__