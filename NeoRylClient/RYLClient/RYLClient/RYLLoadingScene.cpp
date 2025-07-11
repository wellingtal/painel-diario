#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLLoadingScene.h"
#include "RYLSprite.h"
#include "Texture.h"
#include "RYLGameData.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "BaseGraphicsLayer.h"
#include "BaseDataDefine.h"
#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "GMMemory.h"
#include "RYLGameScene.h"
#include "RylGuildWarPenaltyDlg.h"
#include "RYLSceneManager.h"
#include "RYLGameTimeManager.h"

CRYLLoadingScene*	CRYLLoadingScene::s_pInstance = NULL ;

CRYLLoadingScene::CRYLLoadingScene() 
{
	s_pInstance = this ;

	m_uiTitleImageSX = g_ClientMain.m_iScreenWidth/2-400;
	m_uiTitleImageSY = g_ClientMain.m_iScreenHeight/2-300;

	m_pLoadingImage = NULL ;

    //-- 2004. 2. 23. Zergra From.--//
    m_bFadeOutFinish = FALSE ;
    //-- Zergra To. --//

	CGMImageDib*	pImageDIB = NULL ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	CTexture::SetPath(NEOINTERFACETEXTUREPATH);
	
	CHAR buff[ 9 ][ MAX_PATH ] ;
 	sprintf( buff[ 0 ], "\\Loading\\zone%d\\Lz01.gmi", 1 ) ;
	sprintf( buff[ 1 ], "\\Loading\\zone%d\\Lz02.gmi", 1 ) ;
	sprintf( buff[ 2 ], "\\Loading\\zone%d\\Lz03.gmi", 1 ) ;
	sprintf( buff[ 3 ], "\\Loading\\zone%d\\Lz04_08_12.gmi", 1 ) ;
	sprintf( buff[ 4 ], "\\Loading\\zone%d\\Lz05.gmi", 1 ) ;
	sprintf( buff[ 5 ], "\\Loading\\zone%d\\Lz06.gmi", 1 ) ;
	sprintf( buff[ 6 ], "\\Loading\\zone%d\\Lz07.gmi", 1 ) ;
	sprintf( buff[ 7 ], "\\Loading\\zone%d\\Lz09_10.gmi", 1 ) ;
	sprintf( buff[ 8 ], "\\Loading\\zone%d\\Lz11.gmi", 1 ) ;

	CHAR szPath[ MAX_PATH ] ;
	for ( INT i = 0 ; i < 9 ; i ++ )
	{
		strcpy( szPath, NEOINTERFACETEXTUREPATH ) ;
		strcat( szPath, buff[ i ] ) ;
		CRYLSpriteList::Instance()->AddSprite( szPath, ERYLSPRITENAME::RYL_ZONE_LOADING01 + i) ;
	}	
	
	
	GM_DELETE( pImageDIB ) ;
	
}

CRYLLoadingScene::~CRYLLoadingScene() 
{
	DeleteResourceObject() ;
}

HRESULT			CRYLLoadingScene::SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) 
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

HRESULT			CRYLLoadingScene::Render( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	if (m_lScreenFade < FADE_START) 
	{
		return E_FAIL ;
	}

	lpD3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0);
	SetRYLRenderState( lpD3DDevice ) ;
	
	CRYLGameData* pGame = CRYLGameData::Instance();

	int nFadeSpeed	= 18;		

	if (m_dwFadeMode == FADE_OUT)
	{ 
		m_lScreenFade -= nFadeSpeed;
		if ( m_lScreenFade > FADE_START )
		{
			if ( m_pLoadingImage )
				m_pLoadingImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY, (unsigned char)(((float)m_lScreenFade / FADE_END) * 255.0f), 0xffffffff ) ;

            m_bFadeOutFinish = TRUE ;
        }
        else
        {
            m_bFadeOutFinish = FALSE ;
        }
	} 
	else if (m_dwFadeMode == FADE_IN)
	{
		m_lScreenFade += nFadeSpeed;
		if ( m_lScreenFade > FADE_END )
		{
			if ( m_pLoadingImage )
				m_pLoadingImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY ) ;

            m_bFadeOutFinish = FALSE ;
		}
		else
		{
			if ( m_pLoadingImage )
				m_pLoadingImage->Draw( m_uiTitleImageSX, m_uiTitleImageSY, (unsigned char)(((float)m_lScreenFade / FADE_END) * 255.0f), 0xffffffff ) ;

            m_bFadeOutFinish = FALSE ;
		}
	}
	return S_OK ;
}

HRESULT			CRYLLoadingScene::Update() 
{

	return S_OK ;
}

HRESULT			CRYLLoadingScene::InitResourceObject( INT iZone ) 
{
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

	if ( iZone != -1 )
		pNetwork->m_dwPresentZone = iZone ; 

	INT i ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;	

	CTexture::SetPath(NEOINTERFACETEXTUREPATH);
	
	CHAR buff[ 9 ][ MAX_PATH ] ;
	CHAR szPath[ MAX_PATH ] ;

 	sprintf( buff[ 0 ], "\\Loading\\zone%d\\Lz01.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 1 ], "\\Loading\\zone%d\\Lz02.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 2 ], "\\Loading\\zone%d\\Lz03.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 3 ], "\\Loading\\zone%d\\Lz04_08_12.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 4 ], "\\Loading\\zone%d\\Lz05.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 5 ], "\\Loading\\zone%d\\Lz06.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 6 ], "\\Loading\\zone%d\\Lz07.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 7 ], "\\Loading\\zone%d\\Lz09_10.gmi", pNetwork->m_dwPresentZone ) ;
	sprintf( buff[ 8 ], "\\Loading\\zone%d\\Lz11.gmi", pNetwork->m_dwPresentZone ) ;
	
	for ( i = 0 ; i < 9 ; i ++ )
	{
		strcpy( szPath, NEOINTERFACETEXTUREPATH ) ;
		strcat( szPath, buff[ i ] ) ;
		pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING01 + i ) ;
		pImageDIB->LoadFromFile( szPath ) ;
	}	

	CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x000000ff ) ;
	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING01 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING02 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING03 ) ) ;
	SetRect( &rt, 0, 0, 32,256 ) ;
	pImageDIB->DrawRect( &rt, 768, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING04_08_12 ) ) ;

	SetRect( &rt, 0, 0, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING05 ) ) ;
	pImageDIB->DrawRect( &rt, 256, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING06 ) ) ;
	pImageDIB->DrawRect( &rt, 512, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING07 ) ) ;
	SetRect( &rt, 32, 0, 64, 256 ) ;
	pImageDIB->DrawRect( &rt, 768, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING04_08_12 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 0, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING09_10) ) ;
	SetRect( &rt, 0, 88, 256, 176 ) ;
	pImageDIB->DrawRect( &rt, 256, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING09_10) ) ;
	SetRect( &rt, 0, 0, 256, 88 ) ;
	pImageDIB->DrawRect( &rt, 512, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING11) ) ;
	SetRect( &rt, 64, 0, 96, 88 ) ;
	pImageDIB->DrawRect( &rt, 768, 512, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ZONE_LOADING04_08_12) ) ;

	if ( !CRYLGameData::Instance()->m_cReLogin )
	{
		m_pLoadingImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	}
	
	m_pLoadingImage->LoadFromImage( pImageDIB ) ;
	
	if ( !CRYLGameData::Instance()->m_cReLogin )
	{
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLoadingImage ) ;
	}
	
	GM_DELETE( pImageDIB ) ;	
	FadeOut() ;

	return S_OK ;
}

HRESULT			CRYLLoadingScene::DeleteResourceObject() 
{
	return S_OK ;
}

HRESULT			CRYLLoadingScene::FinalCleanup() 
{
	return S_OK ;
}

void CRYLLoadingScene::FadeIn(void)
{	
	m_dwFadeMode	= FADE_IN;
	m_lScreenFade	= FADE_START;
	m_lItemFade		= FADE_START;
}
void CRYLLoadingScene::FadeOut(void)
{	
	m_dwFadeMode	= FADE_OUT;
	m_lScreenFade	= FADE_END;
	m_lItemFade		= FADE_END;
}
