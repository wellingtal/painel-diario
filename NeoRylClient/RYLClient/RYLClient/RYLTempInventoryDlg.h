//================================================================
//
//	Name : RYLTempInventoryDlg.h 
//	Desc : ������ ���� �Ǹ� ���̾�α�
//  Date : 2004. 07. 21
//
//================================================================
#ifndef __RYL_TEMPINVENTORYDLG_H__
#define __RYL_TEMPINVENTORYDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	


namespace RYLTEMPINVENTORYDLG
{
	
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLImage ;

class CRYLTempInventoryDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pDlgLabel;								// ���̾�α� ��

	CRYLLabel*			m_pDescriptionLabel1 ;						// ���� ù°��
	CRYLLabel*			m_pDescriptionLabel2 ;						// ���� ��°��
	CRYLLabel*			m_pDescriptionLabel3 ;						// ���� ��°��
	CRYLLabel*			m_pDescriptionLabel4 ;						// ���� ��°��
	CRYLLabel*			m_pDescriptionLabel5 ;						// ���� �ټ�°��

	CRYLLabel*			m_pRemainingLabel ;							// �ܿ� ������ �� ?? / ??
	CRYLLabel*			m_pLimitTimeLabel ;							// ���� �ð�
	CRYLLabel*			m_pLimitTimeValueLabel ;


public :
	CRYLTempInventoryDlg() ;
	virtual ~CRYLTempInventoryDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
    virtual HRESULT		Update() ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID	CheckTime();
} ;

#endif // __RYL_TEMPINVENTORYDLG_H__