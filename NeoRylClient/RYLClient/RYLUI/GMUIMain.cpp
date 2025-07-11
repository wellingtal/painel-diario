#include "GMUIMain.h"
#include "RYLSpriteList.h"
#include "GMMemory.h"

HRESULT GMUIInitialize( HWND hWnd, INT ScreenWidth, INT ScreenHeight )
{
	CRYLRectangle::CreateInstance() ;
	CRYLImageManager::CreateInstance() ;

	CGMUIScreen::CreateInstance() ;
	CGMUIScreen::GetInstance()->SetParentHandle( hWnd ) ;
	CGMUIScreen::GetInstance()->SetWidth( ScreenWidth ) ;
	CGMUIScreen::GetInstance()->SetHeight( ScreenHeight ) ;

	CRYLTextureManager::CreateInstance() ; 

	CRYLSpriteList::CreateInstance() ;
	if ( FAILED( CRYLSpriteList::Instance()->InitSprite() ) )
		return E_FAIL ;

	return S_OK ;
}
 
HRESULT GMUIFinalize() 
{
	CRYLSpriteList::Instance()->ClearVector() ;
	CRYLSpriteList::DeleteInstance() ;
	CGMUIScreen::DeleteInstance() ;
	CRYLImageManager::DeleteInstance() ;
	CRYLTextureManager::DeleteInstance() ;
	CRYLRectangle::DeleteInstance() ;

	return S_OK ;
}
