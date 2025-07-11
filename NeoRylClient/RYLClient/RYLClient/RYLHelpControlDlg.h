//=================================================================
//
//	Name : RYLHelpControlDlg.h
//	Desc : 도움말(조작키) 다이얼로그
//	Data : 2005. 07. 14
//
//=================================================================
#ifndef __RYL_HELPCONTROLDLG_H__
#define __RYL_HELPCONTROLDLG_H__

#include "RYLDialog.h"

class CRYLSpriteEx;

class CRYLHelpControlDlg : public CGMUICustomPanel
{
private:

	CRYLSpriteEx*		m_pHelpControlSprite[4];


public :

	CRYLHelpControlDlg(CGMUICustomDlg *pParentDlg);
	virtual ~CRYLHelpControlDlg();

	void	InitCtrl(void);
	void	FinalCtrl(void);

	virtual VOID Show(CGMUIObject* Sender);
	virtual VOID Hide(CGMUIObject* Sender);

	virtual VOID GMUIPaint(INT xx, INT yy);

	virtual	HRESULT		Render(LPDIRECT3DDEVICE8 lpD3DDevice)		{ return S_OK;	}
	virtual HRESULT		FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice)	{ return S_OK;	}
	virtual HRESULT		Update(BOOL &bClick, BOOL &bEdge)			{ return S_OK;	}

	static VOID DlgClick(CGMUIObject* Sender, INT X, INT Y);

	void	LoadHelpControlTexture(void);
};

#endif 