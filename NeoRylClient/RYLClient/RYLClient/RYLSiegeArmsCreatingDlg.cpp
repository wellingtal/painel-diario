
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

#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLSiegeArmsCreatingDlg.h"
#include "GMMemory.h"

CRYLSiegeArmsCreatingDlg*	g_pRYLSiegeArmsCreatingDlg = NULL ;

CRYLSiegeArmsCreatingDlg::CRYLSiegeArmsCreatingDlg() 
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
	int i = 0;
	for (  ; i < 35 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	/*
	pImageDIB->MakeFrameRect( 12, 73, 80, 80, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->DrawFrameRect( 38, 74, 1, 79, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 64, 74, 1, 79, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 13, 100, 79, 1, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 13, 126, 79, 1, 0xff675e46 ) ;
	*/
	
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
	
	g_pRYLSiegeArmsCreatingDlg = this ;
}

CRYLSiegeArmsCreatingDlg::~CRYLSiegeArmsCreatingDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLSiegeArmsCreatingDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// ���� ���� �޴�
	m_pArmsCreatMenuLabel = new CRYLLabel( this ) ;
	m_pArmsCreatMenuLabel->SetLeft( 17 ) ;
	m_pArmsCreatMenuLabel->SetTop( 18 ) ;
	m_pArmsCreatMenuLabel->SetFontColor (0xffffffff);
	m_pArmsCreatMenuLabel->SetAutoSize( TRUE ) ;
	m_pArmsCreatMenuLabel->SetCaption( CRYLStringTable::m_strString[ 2810 ] ) ;
	AttachGMUIControl( m_pArmsCreatMenuLabel ) ;

	// ���� ���� ���� 1
	m_pArmsCreatExpla1Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla1Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla1Label->SetTop( 44 ) ;
	m_pArmsCreatExpla1Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla1Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla1Label->SetCaption( CRYLStringTable::m_strString[ 2811 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla1Label ) ;

	// ���� ���� ���� 2
	m_pArmsCreatExpla2Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla2Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla2Label->SetTop( 59 ) ;
	m_pArmsCreatExpla2Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla2Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla2Label->SetCaption( CRYLStringTable::m_strString[ 2812 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla2Label ) ;

	// ���� ���� ���� 3
	m_pArmsCreatExpla3Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla3Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla3Label->SetTop( 76 ) ;
	m_pArmsCreatExpla3Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla3Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla3Label->SetCaption( CRYLStringTable::m_strString[ 2813 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla3Label ) ;

	// ���� ���� ���� 4
	m_pArmsCreatExpla4Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla4Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla4Label->SetTop( 92 ) ;
	m_pArmsCreatExpla4Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla4Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla4Label->SetCaption( CRYLStringTable::m_strString[ 2814 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla4Label ) ;

	// ���� ���� ���� 5
	m_pArmsCreatExpla5Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla5Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla5Label->SetTop( 108 ) ;
	m_pArmsCreatExpla5Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla5Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla5Label->SetCaption( CRYLStringTable::m_strString[ 2815 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla5Label ) ;

	// ���� ���� ���� 6
	m_pArmsCreatExpla6Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla6Label->SetLeft( 17 ) ;
	m_pArmsCreatExpla6Label->SetTop( 124 ) ;
	m_pArmsCreatExpla6Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla6Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla6Label->SetCaption( CRYLStringTable::m_strString[ 2816 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla6Label ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// ��� ��ư
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 173 ) ;
	m_pCancelButton->SetTop( 159 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 2817 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLSIEGEARMSCREATINGDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;

	// â �ݱ�
	m_pCloseButton = new CRYLButton( this ) ;
	m_pCloseButton->SetLeft( 103 ) ;
	m_pCloseButton->SetTop( 159 ) ;
	m_pCloseButton->SetCaption( CRYLStringTable::m_strString[ 160 ] ) ;
	m_pCloseButton->SetFontColor( 0xffffffff) ;
	m_pCloseButton->SetColor( 0xffffffff) ;
	m_pCloseButton->OnClickEvent = DlgClick ;
	m_pCloseButton->SetInstance( RYLSIEGEARMSCREATINGDLG::CLOSE_BUTTON ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCloseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCloseButton ) ;
	
}

VOID	CRYLSiegeArmsCreatingDlg::FinalCtrl() 
{
	GM_DELETE( m_pArmsCreatMenuLabel ) ;			// ���� ���� �޴�
	GM_DELETE( m_pArmsCreatExpla1Label ) ;			// ���� ���� ���� 1
	GM_DELETE( m_pArmsCreatExpla2Label ) ;			// ���� ���� ���� 2
	GM_DELETE( m_pArmsCreatExpla3Label ) ;			// ���� ���� ���� 3
	GM_DELETE( m_pArmsCreatExpla4Label ) ;			// ���� ���� ���� 4
	GM_DELETE( m_pArmsCreatExpla5Label ) ;			// ���� ���� ���� 5
	GM_DELETE( m_pArmsCreatExpla6Label ) ;			// ���� ���� ���� 6
	GM_DELETE( m_pCancelButton ) ;
	GM_DELETE( m_pCloseButton ) ;
}

VOID	CRYLSiegeArmsCreatingDlg::InitValue() 
{
}

VOID	CRYLSiegeArmsCreatingDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLSiegeArmsCreatingDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLSiegeArmsCreatingDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLSiegeArmsCreatingDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLSiegeArmsCreatingDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLSiegeArmsCreatingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLSiegeArmsCreatingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLSiegeArmsCreatingDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLSIEGEARMSCREATINGDLG::CANCEL_BUTTON :
			{
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if( !pSelfCreature )
                    return;

				SendPacket::CharSiegeArmsCmd( g_GameSession, 
                                              pSelfCreature->GetCharID(), 
											  CRYLGameData::Instance()->m_lpInterface->m_dwSiegeArmsSelectID, 
											  PktSiegeArmsCmd::SIEGE_CANCEL ) ;
				
				g_pRYLSiegeArmsCreatingDlg->SetVisible( FALSE ) ;
			}
			break ;
		case RYLSIEGEARMSCREATINGDLG::CLOSE_BUTTON:
			{
				g_pRYLSiegeArmsCreatingDlg->SetVisible( FALSE ) ;
			}
			break;
	}
}
