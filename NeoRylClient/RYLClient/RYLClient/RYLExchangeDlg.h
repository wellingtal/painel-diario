//=================================================================
// Name : RYLExchangeDlg.h
// Data : 2003. 01. 13
// Desc : ��ȯâ
//=================================================================
#ifndef __RYL_EXCHANGEDLG_H__
#define __RYL_EXCHANGEDLG_H__

#include "RYLDialog.h"

#include "../CharStatus.h"
#include "../ItemInstance.h"

namespace RYLEXCHANGEDDLG
{
	const DWORD			LOCK1_CHECKBOX			= 0x00000000 ;
	const DWORD			LOCK2_CHECKBOX			= 0x00000001 ;
	const DWORD			OK1_BUTTON				= 0x00000002 ;
	const DWORD			OK2_BUTTON				= 0x00000003 ;
	const DWORD			CANCEL1_BUTTON			= 0x00000004 ;
	const DWORD			CANCEL2_BUTTON			= 0x00000005 ;
	const DWORD			MONEY_BUTTON			= 0x00000006 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;
class CGMImageDib ;
class CRYLImage ;

class CRYLExchangeDlg : public CRYLDialog
{
public :

	CRYLLabel*			m_pExchangeLabel1 ;					// ��ȯâ
	CRYLLabel*			m_pExchangeLabel2 ;					// ��ȯâ
	CRYLLabel*			m_pExchangeOwnerLabel1 ;			// ���� �̸� ��
	CRYLLabel*			m_pExchangeOwnerLabel2 ;			// �ڽ� �̸� ��
	CRYLLabel*			m_pGoldLabel1 ;						// ���� ��
	CRYLLabel*			m_pGoldLabel2 ;						// �ڽ� ��

	CRYLCheckBox*		m_pOkButton1 ;						// ���� Ȯ��
	CRYLButton*			m_pCancelButton1 ;					// ���� ���
	CRYLCheckBox*		m_pOkButton2 ;						// �ڽ� Ȯ��
	CRYLButton*			m_pCancelButton2 ;					// �ڽ� ���
	CRYLButton*			m_pMoneyButton ;					// �� ��ư
	CRYLCheckBox*		m_pLockCheckBox1 ;					// ���� ��� ��ư
	CRYLCheckBox*		m_pLockCheckBox2 ;					// �ڽ� ��ݹ�ư
	

	CItemInstance*		m_lpSplitItem ;
	DWORD				m_dwSplitResult ;
	DWORD				m_dwSplitValue ;
	DWORD				m_dwExchangeGoldResult ;
	DWORD				m_dwExchangeGoldValue ;
	BOOL				m_bSingleWindow ;

	CGMImageDib*		m_pImageDIB ;
	CRYLImage*			m_pImage ;
	POINT				m_ptStartPos ;

private:
		

public :

	CRYLExchangeDlg() ;
	virtual ~CRYLExchangeDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	InitPos( CItemInstance *lpItem ) ;
	VOID	SetSingleWindow( BOOL bSingle ) ;		// �ڽ��� ��ȯâ�� ���°�?
	BOOL	SelectItem() ;							// ���� ����

	VOID	ExchangeAccept( ULONG dwChrID ) ;		// Ȯ�� ��ư �۵�
	VOID	ExchangeCancel( ULONG dwChrID ) ;		// ��� ��ư �۵�
	VOID	ExchangeLock( ULONG dwChrID ) ;			// ��
	VOID	ExchangeUnLock( ULONG dwChrID ) ;		// ���
	VOID	ProcessExchange();						// ��ȯ �Ϸ�
	VOID	ExchangeQuit();							// ��ȯ ���

	VOID	ExchangeCancelEvent() ;					// ��� ��ư �̺�Ʈ
	VOID	ExchangeMoneyEvent() ;					// �ݾ� ��ư �̺�Ʈ
	VOID	ExchangeLockEvent() ;					// ��� ��ư �̺�Ʈ
	VOID	ExchangeOkEvent() ;						// Ȯ�� ��ư �̺�Ʈ

	BOOL	CheckInvenSpace() ;						// ��ȯ�� �������� �κ��丮�� ���� �ִ��� üũ
	BOOL	CheckInvenMoney() ;						// ��ȯ�� ���� �κ��丮�� �ִ�� �� �� �ִ� ������ Ŀ������ üũ
} ;

#endif //__RYL_EXCHANGEDLG_H__
