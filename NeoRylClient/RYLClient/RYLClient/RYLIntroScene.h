//=================================================================
// Name : RYL Intro Scene.h
// Data : 2003. 11. 10
// Desc : Intro Scene 
//			- public : CRYLSceneObject
//=================================================================

#ifndef __RYL_INTROSCENE_H__
#define __RYL_INTROSCENE_H__

#include "RYLSceneObject.h"

class CRYLImage ;

class CRYLIntroScene : public CRYLSceneObject
{
private :
	CRYLImage*				m_pIntroImage ;
	UINT					m_uiTitleImageSX ;
	UINT					m_uiTitleImageSY ;
	UINT					m_dwLogoTimer ;
	
public :
	CRYLIntroScene( INT iID ) ;
	virtual ~CRYLIntroScene() ;

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

#endif //__RYL_INTROSCENE_H__