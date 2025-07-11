//=================================================================
// Name : RYLRaceSelectScene.h
// Data : 2003. 11. 13
// Desc : 국가 선택 화면
//			- public : CRYLSceneObject
//=================================================================

// WORK_LIST 2.2 NationSelectScene 이름 수정
#ifndef __RYL_RACESELECTSCENE_H__
#define __RYL_RACESELECTSCENE_H__

#include "RYLSceneObject.h"
#include "RYLDialog.h"

class CRYLCheckBox ;
class CRYLButton ;

namespace RYLRACEDLG
{
	const DWORD		HUMAN_CHECKBOX			= 0x00000000 ;
	const DWORD		AKHAN_CHECKBOX			= 0x00000001 ;
	const DWORD		OK_BUTTON				= 0x00000002 ;
} ;

class CRYLRaceDlg : public CRYLDialog
{
public :
	CRYLCheckBox*		m_pHumanCheckBox ;
	CRYLCheckBox*		m_pAkhanCheckBox ;
	CRYLButton*			m_pOkButton ;
	int					m_dwRace ;

public :
	CRYLRaceDlg() ;
	virtual ~CRYLRaceDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
} ;

class CRYLRaceSelectScene : public CRYLSceneObject
{
private :
	CRYLRaceDlg*			m_pRaceDlg ;
public :
	CRYLRaceSelectScene( INT iID ) ;
	virtual ~CRYLRaceSelectScene() ;

	virtual HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT			Update() ;
	
	virtual HRESULT			InitResourceObject() ;
	virtual HRESULT			DeleteResourceObject() ;

	virtual HRESULT			FinalCleanup() ;
	virtual HRESULT			MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) ;

	virtual VOID			SceneShow() ;
	virtual VOID			SceneHide() ;	
} ;

#endif //__RYL_RACESELECTSCENE_H__