#include "RYLClientMain.h"
#include "RYLSecondCreateDlg.h"
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
CRYLSecondCreateDlg*	g_pSecondCreateDlg = NULL ;

CRYLSecondCreateDlg::CRYLSecondCreateDlg () : CRYLDialog()
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

	g_pSecondCreateDlg = this ;	

	SetWidth( 93 ) ;
	SetHeight( 41 ) ;
	switch ( g_ClientMain.m_iScreenWidth )
	{
		case 800 :
			SetLeft( 461 ) ;
			SetTop( 434 ) ;
			break ;
		case 1024 :
			SetLeft( 603 ) ;
			SetTop( 555 ) ;
			break ;
		case 1280 :
			SetLeft( 764 ) ;
			SetTop( 707 ) ;
			break ;
		case 1600 :
			SetLeft( 968 ) ;
			SetTop( 878 ) ;
			break ;
	}
	
	SetFormType( RYLDLG_TYPE_NONE ) ;
	SetDlgMoving(FALSE);

	InitCtrl() ;
}

CRYLSecondCreateDlg::~CRYLSecondCreateDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLSecondCreateDlg::InitCtrl ()
{
	m_pTextLabel = new CRYLLabel( this ) ;
	m_pTextLabel->SetLeft( 8 ) ;
	m_pTextLabel->SetTop( 18 ) ;
	m_pTextLabel->SetFontColor (0xffffffff);
	m_pTextLabel->SetAutoSize( TRUE ) ;
	m_pTextLabel->SetCaption( CRYLStringTable::m_strString[ 2727 ] ) ;
	AttachGMUIControl( m_pTextLabel ) ;
}

VOID	CRYLSecondCreateDlg::FinalCtrl ()
{
	GM_DELETE( m_pTextLabel ) ;
}

VOID	CRYLSecondCreateDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLSecondCreateDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLSecondCreateDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLSecondCreateDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	return S_OK ;
}

HRESULT	CRYLSecondCreateDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

VOID	CRYLSecondCreateDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

void CRYLSecondCreateDlg::SetVisiblePos(unsigned long dwKind)
{
	switch(dwKind)
	{
		case CClass::RaceType::HUMAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 461 ) ;
					SetTop( 434 ) ;
					break ;
				case 1024 :
					SetLeft( 603 ) ;
					SetTop( 555 ) ;
					break ;
				case 1280 :
					SetLeft( 764 ) ;
					SetTop( 707 ) ;
					break ;
				case 1600 :
					SetLeft( 968 ) ;
					SetTop( 878 ) ;
					break ;
			}
			break;

		case CClass::RaceType::AKHAN:
			switch ( g_ClientMain.m_iScreenWidth )
			{
				case 800 :
					SetLeft( 453 ) ;
					SetTop( 424 ) ;
					break ;
				case 1024 :
					SetLeft( 593 ) ;
					SetTop( 543 ) ;
					break ;
				case 1280 :
					SetLeft( 755 ) ;
					SetTop( 681 ) ;
					break ;
				case 1600 :
					SetLeft( 956 ) ;
					SetTop( 853 ) ;
					break ;
			}
			break;
	}
}
