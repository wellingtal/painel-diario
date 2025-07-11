//================================================================
//
//	Name : RYLSelectedConfirmDlg.h 
//	Desc : ���ÿϷ�
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
	IS_SELCONFIRMINFO	m_ConfirmSlot[ 5 ] ;	// ��� ����
	
	CRYLLabel*		m_pTitleLabel ;				// ����Ȯ��
	CRYLLabel*		m_pSelectResultLabel ;		// ĳ���� / â�� ���ð��
	
	CRYLLabel*		m_pCharLabel ;				// ĳ����
	CRYLLabel*		m_pVaultLabel ;				// â��
	CRYLLabel*		m_pVaultNameLabel ;			// â�� �̸�
	CRYLLabel*		m_pUsedNumLabel ;			// â���� ����
	CRYLLabel*		m_pGoldLabel ;				// �ݰ�

	CRYLButton*		m_pEditCharButton ;			// ĳ���� ����
	CRYLButton*		m_pEditVaultButton ;		// â�����
	CRYLButton*		m_pConfirmButton ;			// ���ÿϷ�
			
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