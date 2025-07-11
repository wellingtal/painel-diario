//================================================================
//
//	Name : RYLSystemDlg.h 
//	Desc : �ý��� ���̾�α� �ڽ�
//  Date : 2004. 01. 18
//
//================================================================
#ifndef __RYL_SYSTEMDLG_H__
#define __RYL_SYSTEMDLG_H__

#include "RYLDialog.h"

namespace RYLSYSTEMDLG
{
	const DWORD		CHARACTERSELECT_BUTTON		= 0x00000000 ;
	const DWORD		OPTION_BUTTON				= 0x00000001 ;
	const DWORD		QUIT_BUTTON					= 0x00000002 ;
	const DWORD		CONTINUE_BUTTON				= 0x00000003 ;
} ;

class CRYLButton ;
class CRYLLabel ;

class CRYLSystemDlg : public CRYLDialog
{
private:
	CRYLButton*			m_pChatacterSelectButton ;		// ĳ���ͼ���
	CRYLButton*			m_pOptionButton ;				// �ɼ� 
	CRYLButton*			m_pQuitButton ;					// ����
	CRYLButton*			m_pContinueButton ;				// ���
	CRYLLabel*			m_pSystemLabel ;				// �ý��� �̸�
	
public :
	CRYLSystemDlg() ;
	virtual ~CRYLSystemDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif //__RYL_SYSTEMDLG_H__

