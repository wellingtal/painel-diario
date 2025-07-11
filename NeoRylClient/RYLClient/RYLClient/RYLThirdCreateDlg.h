//================================================================
//
//	Name : RYLThirdCreateDlg.h 
//	Desc : 새로운 캐릭터 판넬
//  Date : 2004. 4. 7
//  By : Belldandy
//
//================================================================
#ifndef __RYL_THIRDCREATEDLG_H__
#define __RYL_THIRDCREATEDLG_H__

#include "RYLDialog.h"

class CRYLLabel ;

class CRYLThirdCreateDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pTextLabel ;

public:
	CRYLThirdCreateDlg(void);
	virtual ~CRYLThirdCreateDlg(void);

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void SetVisiblePos(unsigned long dwKind);
};

#endif //__RYL_THIRDCREATEDLG_H__