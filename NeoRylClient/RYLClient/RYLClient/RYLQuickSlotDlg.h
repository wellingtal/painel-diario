//=================================================================
// Name : RYLDialogControlDlg.h
// Data : 2003. 12. 24
// Desc : ���� ������ �� ���� �ٵ��α�
//=================================================================
#ifndef __RYL_QUICKSLOTDLG_H__
#define __RYL_QUICKSLOTDLG_H__

#include "RYLDialog.h"

namespace RYLQUICKSLOTDLG
{
	const DWORD		ROTATION_BUTTON				= 0x00000009 ;
	const DWORD		HIDE_BUTTON					= 0x00000010 ;
	const DWORD		Z_CHECKBOX					= 0x00000011 ;
	const DWORD		X_CHECKBOX					= 0x00000012 ;
} ;

class CRYLButton ;
class CItemInstance ;
class CRYLImage ;
class CRYLCheckBox ;

class CRYLQuickSlotDlg : public CRYLDialog
{
private:

	enum Const
	{
		MAX_QUICK_SLOT_NUM	= 10
	};

	CRYLButton*			m_pRotationButton ;			// ȸ�� ��ư
	CRYLButton*			m_pHideButton ;				// Hide ��ư

	CRYLImage*			m_pHorzImage ;
	CRYLImage*			m_pVertImage ;			

//	CRYLImage*			m_pHideImage ;			

	BOOL				m_bVertEnable ;

	CRYLCheckBox*		m_pZCheckBox ;				// Z ��ư
	CRYLCheckBox*		m_pXCheckBox ;				// X ��ư


public :

	CRYLQuickSlotDlg( INT nLeft, INT nTop, BOOL bVert = FALSE ) ;
	virtual ~CRYLQuickSlotDlg() ;
    
	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	UseItem(CItemInstance *lpItem) ;
	VOID	RotationDlg() ;

	VOID	ZButton() ;
	VOID	XButton() ;
	VOID	ToggleButton() ; 

	BOOL	GetVerticalEnable()			{  return m_bVertEnable ; }
} ;

#endif //__RYL_QUICKSLOTDLG_H__