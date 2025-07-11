#include <winsock2.h>
#include "RYLIntroScene.h"
#include "RYLClientMain.h"
#include "RYLSprite.h"
#include "Texture.h"
#include "WinInput.h"
#include "RYLTimer.h"
#include "FrameTimer.h"
#include "BaseDataDefine.h"
#include "RYLTimer.h"
#include "RYLSceneManager.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "GMMemory.h"

CRYLIntroScene::CRYLIntroScene( INT iID ) : CRYLSceneObject( iID )  
{
	m_dwLogoTimer	= 0L ;

	m_uiTitleImageSX = g_ClientMain.m_iScreenWidth/2-400;
	m_uiTitleImageSY = g_ClientMain.m_iScreenHeight/2-300;

}

CRYLIntroScene::~CRYLIntroScene() 
{
	DeleteResourceObject() ;
}

HRESULT		CRYLIntroScene::InitResourceObject() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	

	CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO01 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO02 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO03 ) ) ;
	SetRect( &rt, 0, 0, 32,256 ) ;
	pImageDIB->DrawRect( &rt, 768, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO04_08_12 ) ) ;

	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO05 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO06 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO07 ) ) ;
	SetRect( &rt, 32, 0, 64, 256 ) ;
	pImageDIB->DrawRect( &rt, 768, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO04_08_12 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 0, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO09_10) ) ;
	SetRect( &rt, 0, 88, 256, 176 ) ;
	pImageDIB->DrawRect( &rt, 256, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO09_10) ) ;
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 512, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO11) ) ;
	SetRect( &rt, 64, 0, 96, 88 ) ;
	pImageDIB->DrawRect( &rt, 768, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGO04_08_12) ) ;

	m_pIntroImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pIntroImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pIntroImage ) ;
	
	GM_DELETE( pImageDIB ) ;
	
	m_dwLogoTimer = CFrameTimer::Regist(0.3f);
	
	FadeIn() ;

	return S_OK ;
}

HRESULT		CRYLIntroScene::DeleteResourceObject() 
{
	return S_OK ;
}

HRESULT			CRYLIntroScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK ;
}

HRESULT		CRYLIntroScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID() ) ;
	
	SetRYLRenderState( lpD3DDevice ) ;
	
	if ( m_dwFadeMode == FADE_IN )
	{
		m_lScreenFade += fUpdate ;

		if(m_lScreenFade > FADE_END)
		{
			m_pIntroImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY ) ;
		}
		else
		{
			m_pIntroImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY, (unsigned char)(((float)m_lScreenFade / FADE_END) * 255.0f), 0xffffffff ) ;
		}
	} 
	else if(m_dwFadeMode == FADE_OUT)
	{
		m_lScreenFade -= fUpdate;

		if(m_lScreenFade > FADE_START)
		{
			m_pIntroImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY, (unsigned char)(((float)m_lScreenFade / FADE_END) * 255.0f), 0xffffffff ) ;
		}
	}
	return S_OK ;
}

HRESULT		CRYLIntroScene::Update() 
{
	if ( m_dwFadeMode == FADE_IN && m_lScreenFade <= FADE_END ) 
		return E_FAIL ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLSceneManager::Instance()->SetScene( LOGIN_SCENE ) ;
	if ( FAILED( CRYLSceneManager::Instance()->GetCurrentSceneInitResource() ) )
	{
		GMASSERT( FALSE, "IntroScene Resource Load Error!" ) ;
		return E_FAIL ;
	}
	
	return S_OK ;
}



HRESULT		CRYLIntroScene::FinalCleanup() 
{
	return S_OK ;
}

HRESULT		CRYLIntroScene::MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) 
{
	return S_OK ;
}

VOID		CRYLIntroScene::SceneShow() 
{
}

VOID		CRYLIntroScene::SceneHide() 
{
}