//=================================================================
// Name : RYLExchangeDlg.h
// Data : 2003. 01. 13
// Desc : 교환창
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

	CRYLLabel*			m_pExchangeLabel1 ;					// 교환창
	CRYLLabel*			m_pExchangeLabel2 ;					// 교환창
	CRYLLabel*			m_pExchangeOwnerLabel1 ;			// 상대방 이름 값
	CRYLLabel*			m_pExchangeOwnerLabel2 ;			// 자신 이름 값
	CRYLLabel*			m_pGoldLabel1 ;						// 상대방 돈
	CRYLLabel*			m_pGoldLabel2 ;						// 자신 돈

	CRYLCheckBox*		m_pOkButton1 ;						// 상대방 확인
	CRYLButton*			m_pCancelButton1 ;					// 상대방 취소
	CRYLCheckBox*		m_pOkButton2 ;						// 자신 확인
	CRYLButton*			m_pCancelButton2 ;					// 자신 취소
	CRYLButton*			m_pMoneyButton ;					// 돈 버튼
	CRYLCheckBox*		m_pLockCheckBox1 ;					// 상대방 잠금 버튼
	CRYLCheckBox*		m_pLockCheckBox2 ;					// 자신 잠금버튼
	

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
	VOID	SetSingleWindow( BOOL bSingle ) ;		// 자신의 교환창만 띄우는가?
	BOOL	SelectItem() ;							// 물건 집기

	VOID	ExchangeAccept( ULONG dwChrID ) ;		// 확인 버튼 작동
	VOID	ExchangeCancel( ULONG dwChrID ) ;		// 취소 버튼 작동
	VOID	ExchangeLock( ULONG dwChrID ) ;			// 락
	VOID	ExchangeUnLock( ULONG dwChrID ) ;		// 언락
	VOID	ProcessExchange();						// 교환 완료
	VOID	ExchangeQuit();							// 교환 취소

	VOID	ExchangeCancelEvent() ;					// 취소 버튼 이벤트
	VOID	ExchangeMoneyEvent() ;					// 금액 버튼 이벤트
	VOID	ExchangeLockEvent() ;					// 잠금 버튼 이벤트
	VOID	ExchangeOkEvent() ;						// 확인 버튼 이벤트

	BOOL	CheckInvenSpace() ;						// 교환후 아이템이 인벤토리에 들어갈수 있는지 체크
	BOOL	CheckInvenMoney() ;						// 교환후 돈이 인벤토리에 최대로 들어갈 수 있는 값보다 커지는지 체크
} ;

#endif //__RYL_EXCHANGEDLG_H__
