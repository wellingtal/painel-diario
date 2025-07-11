//================================================================
//
//	Name : RYLArmsHPDlg.h 
//	Desc : 2005.12.7
//  Date : 병기 탑승시에 병기 HP 출력창
//
//================================================================
#ifndef __RYL_ARMS_HP_DLG_H__
#define __RYL_ARMS_HP_DLG_H__

#include "RYLDialog.h"

class CRYLLabel ;
class CRYLProgressBar ;


namespace RYLARMSHPDLG
{
} ;

class CRYLArmsHPDlg : public CRYLDialog
{
private:

	CRYLLabel*			m_pCaption ;
	CRYLProgressBar*	m_pHPProgressBar ;
	CRYLLabel*			m_pHPLabel ;

public:

	CRYLArmsHPDlg() ;
	virtual ~CRYLArmsHPDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

#endif // __RYL_ARMS_HP_DLG_H__