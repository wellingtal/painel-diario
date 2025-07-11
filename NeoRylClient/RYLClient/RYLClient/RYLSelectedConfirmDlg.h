//================================================================
//
//	Name : RYLSelectedConfirmDlg.h 
//	Desc : 선택완료
//
//================================================================
#ifndef __RYLSELECTEDCONFIRMDLG_H__
#define __RYLSELECTEDCONFIRMDLG_H__

#include "RYLDialog.h"

namespace RYLSELECTEDCONFIRMDLG
{
	const DWORD		CHAREDIT_BUTTON				= 0x00000000 ;
	const DWORD		VAULTEDIT_BUTTON			= 0x00000001 ;
	const DWORD		CONFIRM_BUTTON				= 0x00000002 ;
	
} ;

class CRYLButton ;
class CRYLLabel ;

typedef struct __IS_SELCONFIRMINFO
{
	CRYLLabel*		pCharNameLabel ;
	CRYLLabel*		pServerAndClassLabel ;
	CRYLLabel*		pLevelLabel ;
}IS_SELCONFIRMINFO, *LPIS_SELCONFIRMINFO ;

class CRYLSelectedConfirmDlg : public CRYLDialog
{
public:
	IS_SELCONFIRMINFO	m_ConfirmSlot[ 5 ] ;	// 결과 슬롯
	
	CRYLLabel*		m_pTitleLabel ;				// 선택확인
	CRYLLabel*		m_pSelectResultLabel ;		// 캐릭터 / 창고 선택결과
	
	CRYLLabel*		m_pCharLabel ;				// 캐릭터
	CRYLLabel*		m_pVaultLabel ;				// 창고
	CRYLLabel*		m_pVaultNameLabel ;			// 창고 이름
	CRYLLabel*		m_pUsedNumLabel ;			// 창고탭 개수
	CRYLLabel*		m_pGoldLabel ;				// 금고

	CRYLButton*		m_pEditCharButton ;			// 캐릭터 수정
	CRYLButton*		m_pEditVaultButton ;		// 창고수정
	CRYLButton*		m_pConfirmButton ;			// 선택완료
			
public :
	CRYLSelectedConfirmDlg() ;
	virtual ~CRYLSelectedConfirmDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	VOID UpdateDlg() ;
} ;

#endif //__RYLSELECTEDCONFIRMDLG_H__