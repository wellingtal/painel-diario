
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLCampCreatingDlg.h"
#include "GMMemory.h"

CRYLCampCreatingDlg*	g_pRYLCampCreatingDlg = NULL ;

CRYLCampCreatingDlg::CRYLCampCreatingDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 180, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 35 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 192 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
	
	g_pRYLCampCreatingDlg = this ;
	m_dwCampID = 0L ;
}

CRYLCampCreatingDlg::~CRYLCampCreatingDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCampCreatingDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 병기 생성 메뉴
	m_pArmsCreatMenuLabel = new CRYLLabel( this ) ;
	m_pArmsCreatMenuLabel->SetLeft( 17 ) ;
	m_pArmsCreatMenuLabel->SetTop( 18 ) ;
	m_pArmsCreatMenuLabel->SetFontColor (0xffffffff);
	m_pArmsCreatMenuLabel->SetAutoSize( TRUE ) ;
	m_pArmsCreatMenuLabel->SetCaption( CRYLStringTable::m_strString[ 2818 ] ) ;
	AttachGMUIControl( m_pArmsCreatMenuLabel ) ;

	// 병기 생성 설명 1
	m_pArmsCreatExpla1Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla1Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla1Label->SetTop( 44 ) ;
	m_pArmsCreatExpla1Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla1Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla1Label->SetCaption( CRYLStringTable::m_strString[ 2819 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla1Label ) ;

	// 병기 생성 설명 2
	m_pArmsCreatExpla2Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla2Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla2Label->SetTop( 59 ) ;
	m_pArmsCreatExpla2Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla2Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla2Label->SetCaption( CRYLStringTable::m_strString[ 2820 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla2Label ) ;

	// 병기 생성 설명 3
	m_pArmsCreatExpla3Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla3Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla3Label->SetTop( 76 ) ;
	m_pArmsCreatExpla3Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla3Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla3Label->SetCaption( CRYLStringTable::m_strString[ 2821 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla3Label ) ;

	// 병기 생성 설명 4
	m_pArmsCreatExpla4Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla4Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla4Label->SetTop( 92 ) ;
	m_pArmsCreatExpla4Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla4Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla4Label->SetCaption( CRYLStringTable::m_strString[ 2822 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla4Label ) ;

	// 병기 생성 설명 5
	m_pArmsCreatExpla5Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla5Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla5Label->SetTop( 108 ) ;
	m_pArmsCreatExpla5Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla5Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla5Label->SetCaption( CRYLStringTable::m_strString[ 2823 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla5Label ) ;

	// 병기 생성 설명 6
	char strMessage[ MAX_PATH ] = "" ;
	sprintf( strMessage, CRYLStringTable::m_strString[ 2824 ], Siege::CAMP_CANCELING_TIME ) ;
	m_pArmsCreatExpla6Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla6Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla6Label->SetTop( 124 ) ;
	m_pArmsCreatExpla6Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla6Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla6Label->SetCaption( strMessage ) ;
	AttachGMUIControl( m_pArmsCreatExpla6Label ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 102 ) ;
	m_pCancelButton->SetTop( 159 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 2825 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCAMPCREATINGDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;

	// 닫기 버튼
	m_pCloseButton = new CRYLButton( this ) ;
	m_pCloseButton->SetLeft( 178 ) ;
	m_pCloseButton->SetTop( 159 ) ;
	m_pCloseButton->SetCaption( CRYLStringTable::m_strString[ 2900 ] ) ;
	m_pCloseButton->SetFontColor( 0xffffffff) ;
	m_pCloseButton->SetColor( 0xffffffff) ;
	m_pCloseButton->OnClickEvent = DlgClick ;
	m_pCloseButton->SetInstance( RYLCAMPCREATINGDLG::CLOSE_BUTTON ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCloseButton ) ;
}

VOID	CRYLCampCreatingDlg::FinalCtrl() 
{
	GM_DELETE( m_pArmsCreatMenuLabel ) ;			// 병기 생성 메뉴
	GM_DELETE( m_pArmsCreatExpla1Label ) ;			// 병기 생성 설명 1
	GM_DELETE( m_pArmsCreatExpla2Label ) ;			// 병기 생성 설명 2
	GM_DELETE( m_pArmsCreatExpla3Label ) ;			// 병기 생성 설명 3
	GM_DELETE( m_pArmsCreatExpla4Label ) ;			// 병기 생성 설명 4
	GM_DELETE( m_pArmsCreatExpla5Label ) ;			// 병기 생성 설명 5
	GM_DELETE( m_pArmsCreatExpla6Label ) ;			// 병기 생성 설명 6
	GM_DELETE( m_pCancelButton ) ;
	GM_DELETE( m_pCloseButton ) ;
}

VOID	CRYLCampCreatingDlg::InitValue() 
{
}

VOID	CRYLCampCreatingDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLCampCreatingDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCampCreatingDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCampCreatingDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCampCreatingDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCampCreatingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	return S_OK ;
}

VOID	CRYLCampCreatingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCampCreatingDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCAMPCREATINGDLG::CANCEL_BUTTON :
			{
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if( !pSelfCreature )
                    return;

				SendPacket::CharCampCmd( g_GameSession, 
                                         pSelfCreature->GetCharID(),
										 g_pRYLCampCreatingDlg->m_dwCampID, 
										 0,
										 0,
										 PktCampCmd::CAMP_CANCEL ) ;
			    				
				g_pRYLCampCreatingDlg->SetVisible( FALSE ) ;
			}
			break ;

		case RYLCAMPCREATINGDLG::CLOSE_BUTTON :
			{
				g_pRYLCampCreatingDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}
