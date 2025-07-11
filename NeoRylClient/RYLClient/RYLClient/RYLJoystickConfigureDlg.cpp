#include <winsock2.h>
#include "RYLJoystickConfigureDlg.h"
#include "RYLJoystickConfigurePanel.h"

#include "GMImageDib.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLClientMain.h"
#include "WinInput.h"
#include "RYLTabButton.h"
#include "RYLStringTable.h"
#include "RYLNetworkData.h"

CRYLJoystickConfigureDlg* g_JoystickConfigureDlg = NULL;

CRYLJoystickConfigureDlg::CRYLJoystickConfigureDlg() : CRYLDialog()
{
	m_pTabButton	= NULL ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 );

	CRYLSpriteList::Instance()->CreateGMImage( 448, 15, &pImageDIB ) ;

	pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	SetRect( &rt, 0, 0, 50, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, pSprite) ;
	
	SetRect( &rt, 50, 0, 150, 15 ) ;
	pImageDIB->DrawRect( &rt, 50,  0, pSprite ) ;			
	pImageDIB->DrawRect( &rt, 150, 0, pSprite ) ;
	pImageDIB->DrawRect( &rt, 250, 0, pSprite ) ;

	SetRect( &rt, 50, 0, 86, 15 ) ;
	pImageDIB->DrawRect( &rt, 350, 0, pSprite ) ;	
	
	SetRect( &rt, 194, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 386, 0, pSprite ) ;	

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 448, 400, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	SetRect( &rt, 0, 15, 50, 37) ;
	pImageDIB->DrawRect( &rt, 0, 0, pSprite) ;
	
	SetRect( &rt, 50, 15, 150, 37 ) ;
	pImageDIB->DrawRect( &rt, 50,  0, pSprite ) ;			
	pImageDIB->DrawRect( &rt, 150, 0, pSprite ) ;
	pImageDIB->DrawRect( &rt, 250, 0, pSprite ) ;
	
	SetRect( &rt, 50, 15, 86, 37 ) ;
	pImageDIB->DrawRect( &rt, 350, 0, pSprite ) ;
	
	SetRect( &rt, 194, 15, 256, 37 ) ;	
	pImageDIB->DrawRect( &rt, 386, 0, pSprite ) ;

	pImageDIB->DrawFrameRect( 67, 0, 378, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 67, 20, 378, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 448 ) ;	
	SetHeight( 400 ) ;

	SetLeft( g_ClientMain.m_iScreenWidth/4 ) ;
	SetTop( 30 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	InitCtrl();
}

CRYLJoystickConfigureDlg::~CRYLJoystickConfigureDlg()
{
	FinalCtrl() ;
}

VOID CRYLJoystickConfigureDlg::InitCtrl()
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLJOYSTICKCONFIGUREDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[4122], 4 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	m_pJoystickPanel = new CRYLJoystickConfigurePanel(this);
	m_pJoystickPanel->SetLeft( 0 ) ;
	m_pJoystickPanel->SetTop( 36 ) ;
	m_pJoystickPanel->SetWidth( 448 ) ;
	m_pJoystickPanel->SetHeight( 370 ) ;
	( ( CRYLJoystickConfigurePanel* )m_pJoystickPanel )->InitCtrl() ;
	m_pJoystickPanel->OnClickEvent = DlgClick ;
	m_pJoystickPanel->SetInstance( RYLJOYSTICKCONFIGUREDLG::JOYSTICK_PANEL ) ;
	m_pJoystickPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pJoystickPanel ) ;
}

VOID CRYLJoystickConfigureDlg::FinalCtrl ()
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pJoystickPanel ) ;
}

VOID CRYLJoystickConfigureDlg::Show( CGMUIObject* Sender )
{	
}
	
VOID CRYLJoystickConfigureDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLJoystickConfigureDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;	
}

HRESULT CRYLJoystickConfigureDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible() )
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

	if ( m_pJoystickPanel->GetVisible() )
	{
		( ( CRYLJoystickConfigurePanel * )m_pJoystickPanel )->Update( bClick, bEdge ) ;
	}	

	return S_OK ;
}

VOID CRYLJoystickConfigureDlg::SetVisible(BOOL bVisible )
{
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	if (!bVisible)
	{
		( ( CRYLJoystickConfigurePanel * )m_pJoystickPanel )->SetInitialize();
	}

	// 한국에서는 일단 사용하지 않기 때문에 막아둡니다. (2006-05-02 by hackermz)
	// 테스트 목적으로 레지스트리의 UseJoystick 항목을 참고합니다. (2006-05-15 by hackermz)
//	if (GameRYL::KOREA != pNetwork->m_eInternationalCode)
	if (1 == g_ClientMain.GetUseJoystick())
	{
		CRYLDialog::SetVisible(bVisible);
	}
	else
	{
		CRYLDialog::SetVisible( FALSE );
	}
}

VOID CRYLJoystickConfigureDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_JoystickConfigureDlg )
		return ;
}

HRESULT	CRYLJoystickConfigureDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{ 
	return S_OK;
}

HRESULT	CRYLJoystickConfigureDlg::FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice) 
{
	return S_OK ;
}