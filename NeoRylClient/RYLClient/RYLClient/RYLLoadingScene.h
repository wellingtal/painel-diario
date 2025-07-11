//=================================================================
// Name : RYLLoadingScene.h
// Data : 2003. 11. 13
// Desc : Loading Scene
//			- public : CRYLSceneObject
//=================================================================

#ifndef __RYL_LOADINGSCENE_H__
#define __RYL_LOADINGSCENE_H__

#include "RYLSceneObject.h"

class CRYLSprite ;
class CTexture ;
class CRYLImage ;

namespace RYLLOADING_SCENE
{
#define FADE_IN				0
#define FADE_OUT			1
#define FADE_NONE			0xFFFFFFFF

#define FADE_START			0
#define FADE_END			200
} ;

class CRYLLoadingScene
{
private :
	CRYLImage*		m_pLoadingImage ;
	UINT			m_uiTitleImageSX ;
	UINT			m_uiTitleImageSY ;

	LONG			m_lScreenFade ;
	LONG			m_lItemFade ;
	DWORD			m_dwFadeMode ;

    BOOL            m_bFadeOutFinish;

public :
	CRYLLoadingScene() ;
	virtual ~CRYLLoadingScene() ;

	HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	HRESULT			FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	HRESULT			Update() ;
	
	HRESULT			InitResourceObject( INT iZone = -1 ) ;
	HRESULT			DeleteResourceObject() ;

	HRESULT			FinalCleanup() ;

	void FadeIn(void);
	
	void FadeOut(void);
	
    //-- 2004. 2. 23. Zergra From.--//
    BOOL            GetFadeOutFinish()                          { return m_bFadeOutFinish; }
    //-- Zergra To. --//

	// Signton
public :
	static CRYLLoadingScene*	s_pInstance ;
	static CRYLLoadingScene*	Instance()			{ return s_pInstance ; } ;
} ;

#endif //__RYL_LOADINGSCENE_H__