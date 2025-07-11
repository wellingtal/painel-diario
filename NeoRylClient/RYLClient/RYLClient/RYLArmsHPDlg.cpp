#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLArmsHPDlg.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "WinInput.h"
#include "GMUIObject.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLStringTable.h"

#include "RYLCreature.h"
#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "GMMemory.h"


CRYLArmsHPDlg*	g_pArmsHPDlg = NULL ;

CRYLArmsHPDlg::CRYLArmsHPDlg()
{
	m_pCaption			= NULL ;
	m_pHPProgressBar	= NULL ;
	m_pHPLabel			= NULL ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 150, 50, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 0, 100, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 206, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	int i = 0 ;
	for ( ; i < 5; ++i )
	{
		SetRect( &rt, 0, 38, 100, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5 + 15, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 206, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 100, i * 5 + 15, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}

	SetRect( &rt, 0, 43, 100, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5 + 15, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 206, 43, 256, 53 ) ;
	pImageDIB->DrawRect( &rt, 100, i * 5 + 15, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 14, 34, 122, 12, 0xffB99E6B, 0xc0000000,  1 ) ;		// 금색 테두리 위

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	SetWidth( 150 ) ;
	SetHeight( 50 ) ;

	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( 20 ) ;

	SetFormType( RYLDLG_TYPE_NONE ) ;

	g_pArmsHPDlg = this ;
	InitCtrl() ;
}

CRYLArmsHPDlg::~CRYLArmsHPDlg()
{
	FinalCtrl() ;
}


VOID	CRYLArmsHPDlg::InitCtrl()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	// 캡션
	m_pCaption = new CRYLLabel( this ) ;
	m_pCaption->SetLeft( 15 ) ;
	m_pCaption->SetTop( 15 ) ;
	m_pCaption->SetFontColor( 0xffffffff ) ;
	m_pCaption->SetAutoSize( TRUE ) ;
	m_pCaption->SetCaption( CRYLStringTable::m_strString[ 4095 ] ) ;
	AttachGMUIControl( m_pCaption ) ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// HP 프로그레스바
	m_pHPProgressBar = new CRYLProgressBar( this ) ;
	m_pHPProgressBar->SetLeft( 15 ) ;
	m_pHPProgressBar->SetTop( 35 ) ;
	m_pHPProgressBar->SetWidth( 120 ) ;
	m_pHPProgressBar->SetHeight( 10 ) ;
	m_pHPProgressBar->SetMax( 10 ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 120, 10, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0xFFCE000A ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pHPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pHPProgressBar->SetPosition( 10 ) ;
	AttachGMUIControl( m_pHPProgressBar ) ;
	GM_DELETE( pImageDIB ) ;

	// HP 라벨
	m_pHPLabel = new CRYLLabel( this ) ;
	SetRect( &rt, 15, 35, 135, 46 ) ;
	m_pHPLabel->SetAlignRect( rt ) ;
	m_pHPLabel->SetAlign( CENTER ) ;
	m_pHPLabel->SetFontColor( 0xffffffff ) ;
	m_pHPLabel->SetAutoSize( TRUE ) ;
	m_pHPLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pHPLabel ) ;
}

VOID	CRYLArmsHPDlg::FinalCtrl()
{
	GM_DELETE( m_pCaption ) ;
	GM_DELETE( m_pHPProgressBar ) ;
	GM_DELETE( m_pHPLabel ) ;
}


VOID	CRYLArmsHPDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLArmsHPDlg::Hide( CGMUIObject* Sender )
{
}


VOID	CRYLArmsHPDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}


HRESULT	CRYLArmsHPDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	return S_OK ;
}

HRESULT	CRYLArmsHPDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	return S_OK ;
}

HRESULT	CRYLArmsHPDlg::Update( BOOL &bClick, BOOL &bEdge )
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

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( !pSelfCreature )
	{
		SetVisible( FALSE ) ;
		return S_OK ;
	}

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() ) ;
	if ( !pSelfData )
	{
		SetVisible( FALSE ) ;
		return S_OK ;
	}

	m_pHPProgressBar->SetMax( pSelfData->m_ulMaxHP ) ;
	m_pHPProgressBar->SetPosition( pSelfData->m_lCurrHP ) ;

	char szString[ MAX_PATH ] ;
	sprintf( szString, "%d / %d", pSelfData->m_lCurrHP, pSelfData->m_ulMaxHP ) ;
	m_pHPLabel->SetCaption( szString ) ;

	return S_OK ;
}


VOID	CRYLArmsHPDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}
