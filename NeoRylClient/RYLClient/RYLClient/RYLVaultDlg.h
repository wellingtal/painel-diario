//================================================================
//
//	Name : RYLVaultDlg.h 
//	Desc : â�� ���̾�α�
//  Date : 2004. 1. 26
//
//================================================================
#ifndef __RYL_VAULTDLG_H__
#define __RYL_VAULTDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"	// Added by ClassView

class CRYLTabButton ;
class CRYLButton ;
class CRYLLabel ;

namespace RYLVAULTDLG
{
	const DWORD		TAB_BUTTON						= 0x00000000 ;
	const DWORD		PAYMENTGOLD_BUTTON				= 0x00000001 ;	// ���
	const DWORD		DEPOSITGOLD_BUTTON				= 0x00000002 ;  // �Ա�
	const DWORD		PASSWORD_BUTTON					= 0x00000003 ;	// �佺���� ����
} ;

class CRYLVaultDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton ;
	CRYLLabel*			m_pGoldLabel ;
	CRYLButton*			m_pPaymentGoldButton ;						// ���
	CRYLButton*			m_pDepositGoldButton ;						// �Ա�
	CRYLButton*			m_pPasswordChangeButton	;					// �佺���� ����

public :
	DWORD				m_dwVaultTab ;
	DWORD				m_dwShowVault ;
	DWORD				m_dwBuyTabResult ;
	CItemInstance*		m_lpSplitItem;
	DWORD				m_dwSplitResult ;
	DWORD				m_dwSplitValue ;
	DWORD				m_dwVaultGoldResult ;
	DWORD				m_dwInvenGoldResult ;
	DWORD				m_dwGoldValue ;

	CHAR				m_strCurPass[ 5 ], m_strNewPass[ 5 ] ;
	CHAR				m_cSavePass ;
	BOOL				m_bLogined ;
	CHAR				m_TabTitle[ 4 ][ 32 ] ;
	
public :
	CRYLVaultDlg() ;
	virtual ~CRYLVaultDlg() ;

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

	CRYLTabButton*		GetTabButton()			{ return m_pTabButton ; } ;

	VOID		InitVault() ;
	VOID		ShowNormal() ;
	BOOL		SelectItem() ;

	VOID		PaymentGoldButtonEvent() ;						// ���
	VOID		DepositGoldButtonEvent() ;						// �Ա�
	VOID		PasswordChangeButtonEvent()	;					// �佺���� 

	VOID		VaultTabI() ;
	VOID		VaultTabII() ;
	VOID		VaultTabIII() ;
	VOID		VaultTabIV() ;

	VOID		EnableTabButton() ;
} ;

#endif // __RYL_VAULTDLG_H__