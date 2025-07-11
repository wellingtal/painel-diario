#include "RYLClientMain.h"
#include "RYLThirdCreateDlg.h"
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
CRYLThirdCreateDlg*	g_pThirdCreateDlg = NULL ;

CRYLThirdCreateDlg::CRYLThirdCreateDlg () : CRYLDialog()
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

	g_pThirdCreateDlg = this ;	

	SetWidth( 93 ) ;
	SetHeight( 41 ) ;
	switch ( g_ClientMain.m_iScreenWidth )
	{
		case 800 :
			SetLeft( 542 ) ;
			SetTop( 170 ) ;
			break ;
		case 1024 :
			SetLeft( 705 ) ;
			SetTop( 231 ) ;
			break ;
		case 1280 :
			SetLeft( 897 ) ;
			SetTop( 289 ) ;
			break ;
		case 1600 :
			SetLeft( 1129 ) ;
			SetTop( 364 ) ;
			break ;
	}
	
	SetFormType( RYLDLG_TYPE_NONE ) ;
	SetDlgMoving(FALSE);

	InitCtrl() ;
}

CRYLThirdCreateDlg::~CRYLThirdCreateDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLThirdCreateDlg::InitCtrl ()
{
	m_pTextLabel = new CRYLLabel( this ) ;
	m_pTextLabel->SetLeft( 8 ) ;
	m_pTextLabel->SetTop( 18 ) ;
	m_pTextLabel->SetFontColor (0xffffffff);
	m_pTextLabel->SetAutoSize( TRUE ) ;
	m_pTextLabel->SetCaption( CRYLStringTable::m_strString[ 2727 ] ) ;
	AttachGMUIControl( m_pTextLabel ) ;
}

VOID	CRYLThirdCreateDlg::FinalCtrl ()
{
	GM_DELETE( m_pTextLabel ) ;
}

VOID	CRYLThirdCreateDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLThirdCreateDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLThirdCreateDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLThirdCreateDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	return S_OK ;
}

HRESULT	CRYLThirdCreateDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

VOID	CRYLThirdCreateDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

void CRYLThirdCreateDlg::SetVisiblePos(unsigned long dwKind)
{
	switch(dwKind)
	{
		case CClass::RaceType::HUMAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 542 ) ;
					SetTop( 170 ) ;
					break ;
				case 1024 :
					SetLeft( 705 ) ;
					SetTop( 231 ) ;
					break ;
				case 1280 :
					SetLeft( 897 ) ;
					SetTop( 289 ) ;
					break ;
				case 1600 :
					SetLeft( 1129 ) ;
					SetTop( 364 ) ;
					break ;
			}
			break;

		case CClass::RaceType::AKHAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 565 ) ;
					SetTop( 110 ) ;
					break ;
				case 1024 :
					SetLeft( 718 ) ;
					SetTop( 151 ) ;
					break ;
				case 1280 :
					SetLeft( 917 ) ;
					SetTop( 184 ) ;
					break ;
				case 1600 :
					SetLeft( 1151 ) ;
					SetTop( 237 ) ;
					break ;
			}
			break;
	}
}
