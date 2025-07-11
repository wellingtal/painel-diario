//================================================================
//
//	Name : RYLHelpFrameDlg.h 
//	Desc : 도움말 창 프레임 
//  Date : 2005. 07. 14
//
//================================================================
#ifndef __RYL_HELPFRAMEDLG_H__
#define __RYL_HELPFRAMEDLG_H__

#include "RYLDialog.h"

class CRYLTabButton;
class CGMUICustomPanel;

class CRYLHelpFrameDlg : public CRYLDialog
{
private:

	enum ControlIndex
	{
		TAB_BUTTON			= 0,
		HELP_CONTROL_PANEL	= 1
	};

	CRYLTabButton*		m_pTabButton;
	CGMUICustomPanel*	m_pHelpControlPanel;


public :

	CRYLHelpFrameDlg();
	virtual ~CRYLHelpFrameDlg();

	void InitCtrl(void);
	void FinalCtrl(void);

	virtual HRESULT Update(BOOL &bClick, BOOL &bEdge);
	virtual	HRESULT Render(LPDIRECT3DDEVICE8 lpD3DDevice)			{ return S_OK;		}
	virtual HRESULT FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice)		{ return S_OK;		}

	virtual VOID Show(CGMUIObject* Sender);
	virtual VOID Hide(CGMUIObject* Sender);

	virtual VOID GMUIPaint(INT xx, INT yy);

	static VOID	DlgClick(CGMUIObject* Sender, INT X, INT Y);

	CRYLTabButton*		GetTabButton(void)				{ return m_pTabButton;			}
	CGMUICustomPanel*	GetLargeMapDlg(void)			{ return m_pHelpControlPanel;	}

	void				SetFocusPanel(int iFocus);
	void				LoadHelpTexture(void);
};

#endif