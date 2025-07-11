//=================================================================
// Name : RYLSceneObject.h
// Data : 2003. 11. 10
// Desc : Scene Base Class
//=================================================================
#ifndef __RYL_SCENEOBJECT_H__
#define __RYL_SCENEOBJECT_H__

#include <windows.h>
#include <d3d8.h>
#include "GMBase.h"



class CRYLSceneObject
{
protected :
	// Scene ID
	INT				m_iSceneID ;

	long			m_lScreenFade ;
	long			m_lItemFade ;
	unsigned long	m_dwFadeMode ;

public :
	CRYLSceneObject( INT iID ) ;
	virtual ~CRYLSceneObject() ;

public :
	virtual HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) PURE ;
	virtual	HRESULT			Render(  LPDIRECT3DDEVICE8 lpD3DDevice ) PURE ;
	virtual HRESULT			FinalRender(  LPDIRECT3DDEVICE8 lpD3DDevice ) PURE ;
	virtual HRESULT			Update() PURE ;
	
	virtual HRESULT			InitResourceObject() PURE ;
	virtual HRESULT			DeleteResourceObject() PURE ;

	virtual HRESULT			FinalCleanup() PURE ;
	virtual HRESULT			MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) PURE ;

	virtual VOID			SceneShow() PURE ;
	virtual VOID			SceneHide() PURE ;

    INT		GetSceneID()	{ return m_iSceneID ; }

	void FadeIn(void);

	void FadeOut(void);	
} ;

#endif // __RYL_SCENEOBJECT_H__