#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLLargeMapFrameDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "RYLStatusDlg.h"
#include "RYLStatusExtensionSlotTwo.h"

#include "BaseGraphicsLayer.h"
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "GMMemory.h"


CRYLStatusExtensionSlotTwo*	g_pSlotTwo = NULL ;

CRYLStatusExtensionSlotTwo::CRYLStatusExtensionSlotTwo( CGMUICustomDlg *pParentDlg )  : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg		= pParentDlg;
	m_pATRLabel				= NULL ;
	m_pDFRLabel				= NULL ;


	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 238, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 125, 184, 256, 202 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	SetRect( &rt, 125, 223, 231, 241 ) ;
	pImageDIB->DrawRect( &rt, 131, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	AttachPanelImage( ( CGMTexture*)pImage ) ;

	
	SetWidth( rt.right - rt.left + 131 ) ;
	SetHeight( rt.bottom - rt.top ) ;
	
	InitCtrl();
}

CRYLStatusExtensionSlotTwo::~CRYLStatusExtensionSlotTwo() 
{
	FinalCtrl() ;
}

VOID	CRYLStatusExtensionSlotTwo::InitCtrl() 
{
	m_pATRLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pATRLabel->SetLeft( 63 );
	m_pATRLabel->SetTop( 3 );
	m_pATRLabel->SetFontColor (0xffffffff);
	m_pATRLabel->SetAutoSize( TRUE ) ;
	m_pATRLabel->SetCaption("");
	m_pATRLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1937 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pATRLabel ) ;


	m_pDFRLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDFRLabel->SetLeft( 177 );
	m_pDFRLabel->SetTop( 3 );
	m_pDFRLabel->SetFontColor (0xffffffff);
	m_pDFRLabel->SetAutoSize( TRUE ) ;
	m_pDFRLabel->SetCaption("");
	m_pDFRLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1938 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pDFRLabel ) ;

}

HRESULT		CRYLStatusExtensionSlotTwo::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}
	
	static char strString[MAX_PATH];
	CRYLGameData *pGame = CRYLGameData::Instance() ;


	// 명중 값		
	sprintf( strString, "%d", pGame->m_csStatus.GetHitRate() ) ;
	m_pATRLabel->SetCaption( strString ) ;
	if( pGame->m_csStatus.GetHitRate() < pGame->m_csStatus.GetBaseHitRate() )			// 원래 값 보다 낮아 졌을 때
		m_pATRLabel->SetFontColor( 0xffff0000 );
	else if ( pGame->m_csStatus.GetHitRate() > pGame->m_csStatus.GetBaseHitRate() )		// 원래 값 보다 높아 졌을 떄
		m_pATRLabel->SetFontColor( 0xff00ff00 );
	else
		m_pATRLabel->SetFontColor( 0xffffffff );
	
	// 회피 값
	sprintf( strString, "%d", pGame->m_csStatus.GetEvade()) ;
	m_pDFRLabel->SetCaption( strString ) ;
	if( pGame->m_csStatus.GetEvade() < pGame->m_csStatus.GetBaseEvade() )			// 원래 값 보다 낮아 졌을 때
		m_pDFRLabel->SetFontColor( 0xffff0000 );
	else if ( pGame->m_csStatus.GetEvade() > pGame->m_csStatus.GetBaseEvade() )		// 원래 값 보다 높아 졌을 떄
		m_pDFRLabel->SetFontColor( 0xff00ff00 );
	else
		m_pDFRLabel->SetFontColor( 0xffffffff );




	return S_OK ;
}

HRESULT		CRYLStatusExtensionSlotTwo::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{

	return S_OK ;
}

HRESULT		CRYLStatusExtensionSlotTwo::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{

	return S_OK ;
}

VOID	CRYLStatusExtensionSlotTwo::FinalCtrl() 
{
	GM_DELETE( m_pATRLabel );
	GM_DELETE( m_pDFRLabel );
}

VOID	CRYLStatusExtensionSlotTwo::Show( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLStatusExtensionSlotTwo::Hide( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLStatusExtensionSlotTwo::GMUIPaint( INT xx, INT yy ) 
{
	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
}



VOID	CRYLStatusExtensionSlotTwo::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pSlotTwo )
		return ;
}
