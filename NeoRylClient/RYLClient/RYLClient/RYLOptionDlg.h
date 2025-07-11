//================================================================
//
//	Name : RYLOptionDlg.h 
//	Desc : 옵션 다이얼로그
//  Date : 2004. 1. 18
//
//================================================================
#ifndef __RYL_OPTIONDLG_H__
#define __RYL_OPTIONDLG_H__

#include "RYLDialog.h"
#include "RYLGameData.h"

class CRYLTabButton;
class CRYLDialog;
class CGMUICustomPanel;

namespace RYLOPTIONDLG
{
	const DWORD		TAB_BUTTON				= 0x00000000;
	const DWORD		GENERALOPTIN_PANEL		= 0x00000001;
	const DWORD		EFFECTOPTION_PANEL		= 0x00000002;
	const DWORD		SOUNDOPTION_PANEL		= 0x00000003;
};

class CRYLOptionDlg : public CRYLDialog
{
private :
	CRYLTabButton*		m_pTabButton;
	CGMUICustomPanel*	m_pGeneralOptionPanel;
	CGMUICustomPanel*	m_pEffectOptionPanel;
	CGMUICustomPanel*	m_pSoundOptionPanel;

public :
	static RejectOption m_roOption;
	static ClientOption m_coOption;
	static bool m_bIsCancel;
	
public :
	CRYLOptionDlg();
	virtual ~CRYLOptionDlg();

	VOID InitCtrl();
	VOID FinalCtrl();

	virtual VOID Show( CGMUIObject* Sender );
	virtual VOID Hide( CGMUIObject* Sender );

	virtual VOID GMUIPaint( INT xx, INT yy );

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y );

	CRYLTabButton*		GetTabButton()				{ return m_pTabButton;			}
	CGMUICustomPanel*	GetGeneralOptionPanel()		{ return m_pGeneralOptionPanel; }
	CGMUICustomPanel*	GetEffectOptionPanel()		{ return m_pEffectOptionPanel;	}
	CGMUICustomPanel*	GetSoundOptionPanel()		{ return m_pSoundOptionPanel;	} 

	void	ReLoadOption(void);
	void	OK(void);
};

#endif // __RYL_OPTIONDLG_H__
