#include "RYLClientMain.h"
#include "RYLFirstCreateDlg.h"
#include "GMUIScreen.h"
#include "RYLLabel.h"
#include "FrameTimer.h"
#include "BaseDataDefine.h"

#include "WinInput.h"

#include "RYLSceneManager.h"
#include "RYLCharacterSelectScene.h"
#include "RYLCharacterCreationDlg.h"
#include "RYLNetworkData.h"
#include "RYLGameData.h"

#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImageManager.h"

#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"

#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Creature/Character/CharacterClass.h>
#include <RenderOption.h>
//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLFirstCreateDlg*	g_pFirstCreateDlg = NULL ;

CRYLFirstCreateDlg::CRYLFirstCreateDlg () : CRYLDialog()
{
	m_pTextLabel			= NULL;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;


	CRYLSpriteList::Instance()->CreateGMImage( 93, 41 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 101, 121, 194, 162 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ) ;

	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	g_pFirstCreateDlg = this ;	

	SetWidth( 93 ) ;
	SetHeight( 41 ) ;
	switch ( g_ClientMain.m_iScreenWidth )
	{
		case 800 :
			SetLeft( 178 ) ;
			SetTop( 224 ) ;
			break ;
		case 1024 :
			SetLeft( 241 ) ;
			SetTop( 288 ) ;
			break ;
		case 1280 :
			SetLeft( 314 ) ;
			SetTop( 369 ) ;
			break ;
		case 1600 :
			SetLeft( 402 ) ;
			SetTop( 463 ) ;
			break ;
	}
	
	SetFormType( RYLDLG_TYPE_NONE ) ;
	SetDlgMoving(FALSE);

	InitCtrl() ;
}

CRYLFirstCreateDlg::~CRYLFirstCreateDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLFirstCreateDlg::InitCtrl ()
{
	m_pTextLabel = new CRYLLabel( this ) ;
	m_pTextLabel->SetLeft( 8 ) ;
	m_pTextLabel->SetTop( 18 ) ;
	m_pTextLabel->SetFontColor (0xffffffff);
	m_pTextLabel->SetAutoSize( TRUE ) ;
	m_pTextLabel->SetCaption( CRYLStringTable::m_strString[ 2727 ] ) ;
	AttachGMUIControl( m_pTextLabel ) ;
}

VOID	CRYLFirstCreateDlg::FinalCtrl ()
{
	GM_DELETE( m_pTextLabel ) ;
}

VOID	CRYLFirstCreateDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLFirstCreateDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLFirstCreateDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLFirstCreateDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	return S_OK ;
}

HRESULT	CRYLFirstCreateDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

VOID	CRYLFirstCreateDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

void CRYLFirstCreateDlg::SetVisiblePos(unsigned long dwKind)
{
	switch(dwKind)
	{
		case CClass::RaceType::HUMAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 178 ) ;
					SetTop( 224 ) ;
					break ;
				case 1024 :
					SetLeft( 241 ) ;
					SetTop( 288 ) ;
					break ;
				case 1280 :
					SetLeft( 314 ) ;
					SetTop( 369 ) ;
					break ;
				case 1600 :
					SetLeft( 402 ) ;
					SetTop( 463 ) ;
					break ;
			}
			break;

		case CClass::RaceType::AKHAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 133 ) ;
					SetTop( 363 ) ;
					break ;
				case 1024 :
					SetLeft( 187 ) ;
					SetTop( 463 ) ;
					break ;
				case 1280 :
					SetLeft( 240 ) ;
					SetTop( 588 ) ;
					break ;
				case 1600 :
					SetLeft( 316 ) ;
					SetTop( 747 ) ;
					break ;
			}
			break;
	}
}
