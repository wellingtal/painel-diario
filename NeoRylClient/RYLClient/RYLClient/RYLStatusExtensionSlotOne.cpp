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
#include "RYLStatusExtensionSlotOne.h"

#include "BaseGraphicsLayer.h"
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "GMMemory.h"


CRYLStatusExtensionSlotOne*	g_pSlotOne = NULL ;

CRYLStatusExtensionSlotOne::CRYLStatusExtensionSlotOne( CGMUICustomDlg *pParentDlg  )  : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg		= pParentDlg ;
	m_pATTLabel				= NULL ;
	m_pDEFLabel				= NULL ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 238, 22, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 125, 163, 256, 184 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	SetRect( &rt, 125, 202, 231, 223 ) ;
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

CRYLStatusExtensionSlotOne::~CRYLStatusExtensionSlotOne() 
{
	FinalCtrl() ;
}

VOID	CRYLStatusExtensionSlotOne::InitCtrl() 
{

	m_pATTLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pATTLabel->SetLeft( 46 );
	m_pATTLabel->SetTop( 5 );
	m_pATTLabel->SetFontColor (0xffffffff);
	m_pATTLabel->SetAutoSize( TRUE ) ;
	m_pATTLabel->SetCaption("");
	m_pATTLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1935 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pATTLabel ) ;


	m_pDEFLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDEFLabel->SetLeft( 177 );
	m_pDEFLabel->SetTop( 5 );
	m_pDEFLabel->SetFontColor (0xffffffff);
	m_pDEFLabel->SetAutoSize( TRUE ) ;
	m_pDEFLabel->SetCaption("");
	m_pDEFLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1936 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pDEFLabel ) ;


}

HRESULT		CRYLStatusExtensionSlotOne::Update( BOOL &bClick, BOOL &bEdge ) 
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


	sprintf(strString, "%d ~ %d", pGame->m_csStatus.GetMinDamage(), pGame->m_csStatus.GetMaxDamage());
	long lDamage = pGame->m_csStatus.GetMinDamage() + ( pGame->m_csStatus.GetMaxDamage() - pGame->m_csStatus.GetMinDamage()) / 2;
	long lRealDamage = pGame->m_csStatus.GetBaseMinDamage() + ( pGame->m_csStatus.GetBaseMaxDamage() - pGame->m_csStatus.GetBaseMinDamage()) / 2;
	m_pATTLabel->SetCaption( strString ) ;

	if(lDamage < lRealDamage)
		m_pATTLabel->SetFontColor( 0xffff0000 ) ;
	else if(lDamage > lRealDamage)
		m_pATTLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pATTLabel->SetFontColor( 0xffffffff ) ;

	// 방어력 값
	sprintf(strString, "%d", pGame->m_csStatus.GetArmor());
	m_pDEFLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetArmor() < pGame->m_csStatus.GetBaseArmor() )
		m_pDEFLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetArmor() > pGame->m_csStatus.GetBaseArmor() )
		m_pDEFLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pDEFLabel->SetFontColor( 0xffffffff ) ;

	return S_OK ;
}

HRESULT		CRYLStatusExtensionSlotOne::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{

	return S_OK ;
}

HRESULT		CRYLStatusExtensionSlotOne::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{

	
	return S_OK ;
}

VOID	CRYLStatusExtensionSlotOne::FinalCtrl() 
{
	GM_DELETE( m_pATTLabel );
	GM_DELETE( m_pDEFLabel );
}

VOID	CRYLStatusExtensionSlotOne::Show( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLStatusExtensionSlotOne::Hide( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLStatusExtensionSlotOne::GMUIPaint( INT xx, INT yy ) 
{
	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
}



VOID	CRYLStatusExtensionSlotOne::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pSlotOne )
		return ;
}
