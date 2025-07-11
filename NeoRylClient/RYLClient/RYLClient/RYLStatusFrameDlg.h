//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLStatusFrameDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 새로운 스테이터스 윈도우
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __RYL_STATUS_FRAME_DIALOG_H__
#define __RYL_STATUS_FRAME_DIALOG_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "RYLDialog.h"

//--------------------------------------------------------------------------------------------------------------------------
// pre-define
//--------------------------------------------------------------------------------------------------------------------------
class CRYLTabButton;
class CGMUICustomPanel;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
namespace RYLSTATUSFRAMEDLG
{
    const DWORD     TAB_BUTTON          = 0x00000000;
};

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLStatusFrameDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CRYLStatusFrameDlg : public CRYLDialog
{
private:
    CRYLTabButton*		        m_pTabButton;
	CGMUICustomPanel*	        m_pCharacterInfoPanel;
	CGMUICustomPanel*	        m_pStatusInfoPanel;

public:
                                CRYLStatusFrameDlg();
    virtual                     ~CRYLStatusFrameDlg();

            VOID                InitCtrl();
	        VOID                FinalCtrl();

	virtual VOID                Show( CGMUIObject* Sender );
	virtual VOID                Hide( CGMUIObject* Sender );

	virtual VOID                GMUIPaint( INT xx, INT yy );

	virtual	HRESULT		        Render( LPDIRECT3DDEVICE8 lpD3DDevice )                         { return S_OK; };
	virtual HRESULT		        FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice )                    { return S_OK; };
    virtual HRESULT		        Update( BOOL &bClick, BOOL &bEdge );

	static  VOID	            DlgClick( CGMUIObject* Sender, INT X, INT Y );

	        CRYLTabButton*		GetTabButton()				{ return m_pTabButton; };
	        CGMUICustomPanel*	GetCharacterInfoPanel()     { return m_pCharacterInfoPanel; };
	        CGMUICustomPanel*	GetStatusPanel()	        { return m_pStatusInfoPanel; };

            VOID                ShowCharacterInfoPanel();
            VOID                ShowStatusInfoPanel();
};

#endif //__RYL_STATUS_FRAME_DIALOG_H__