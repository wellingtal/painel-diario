//=================================================================
//
// Name : RYLMiningAmountDlg.h
// Data : 2004. 12. 14
// Desc : Mining Amount Dialog Class, ���� �Ÿ��� Ȯ�� ���̾�α�
//
//=================================================================
#ifndef __RYL_MININGAMOUNTPROCESSDLG_H__
#define __RYL_MININGAMOUNTPROCESSDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <vector>

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

namespace RYLMININGAMOUNTPROCESSDLG
{
	const DWORD		CANCEL_BUTTON				= 0x00000000 ;
} ;



 

class CRYLMiningAmountProcessDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pCanceltButton ;						// ��� ��ư
	CRYLLabel*			m_pProcessLabel ;						// ���μ��� ��
	CRYLLabel*			m_pMessageLabel1 ;						// ���� ù°��
	CRYLLabel*			m_pMessageLabel2 ;						// ���� ��°��
	CRYLLabel*			m_pMessageLabel3 ;						// ���� ��°��
	CRYLLabel*			m_pTitleLabel ;							// Title
    DWORD				m_ulTime;
	CItemInstance*		m_lpKitItem ;							// ŰƮ


protected:
	
	BOOL				UpdateProcess();						// 5���� ������ TRUE ����
	VOID				CheckMineCount();						// �ش綥������ ���� üũ

public :
	CRYLMiningAmountProcessDlg() ;
	virtual ~CRYLMiningAmountProcessDlg() ;

	VOID	Cancel();
	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };


	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID		SetKitItem( CItemInstance* pItem ){ m_lpKitItem = pItem; };

} ;

#endif //__RYL_MININGAMOUNTPROCESSDLG_H__