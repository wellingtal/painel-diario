#include <winsock2.h>
#include "RYLChannelDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLPicture.h"

#include "RYLStringTable.h"
#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLSceneManager.h"

#include <wininput.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include "GMMemory.h"

//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLChannelDlg*	g_pChannelDlg = NULL ;

CRYLChannelDlg::CRYLChannelDlg () : CRYLDialog()
{
	m_pChannelSelectLabel1 = NULL ;		// 채널선택
	m_pChannelSelectLabel2 = NULL ;		// 채널을 선택하여 주십시요.
	m_pChannelListBox = NULL ;			// 채널 1
	m_pChannelPicture[ 0 ] = NULL ;			// 채널 1
	m_pChannelPicture[ 1 ] = NULL ;			// 채널 2 
	m_pChannelPicture[ 2 ] = NULL ;			// 채널 3 
	m_pChannelPicture[ 3 ] = NULL ;			// 채널 4 
	m_pChannelPicture[ 4 ] = NULL ;			// 채널 5 
	m_pOKBtn = NULL  ;					// OK Button

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
	pImageDIB->MakeFrameRect( 18, 47, 195, 89, 0xff946b42, 0x80000000,  1 ) ;
		
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 195 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pChannelDlg = this ;	
	InitCtrl() ;
}

CRYLChannelDlg::~CRYLChannelDlg ()
{
	FinalCtrl() ;
}

VOID CRYLChannelDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pOKBtn = new CRYLButton( this ) ;
	m_pOKBtn->SetLeft( 159 ) ;
	m_pOKBtn->SetTop( 156 ) ;
	m_pOKBtn->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKBtn->SetFontColor( 0xffffffff) ;
	m_pOKBtn->SetColor( 0xffffffff) ;
	m_pOKBtn->OnClickEvent = DlgClick ;
	m_pOKBtn->SetInstance( RYLCHANNELDLG::OK_BUTTON ) ;
	m_pOKBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKBtn ) ;

    // 채널 
	m_pChannelListBox = new CRYLListBox( this, FALSE ) ;
	m_pChannelListBox->SetLeft( 29 ) ;
	m_pChannelListBox->SetTop( 69 ) ;
	m_pChannelListBox->SetWidth( 119 ) ;
	m_pChannelListBox->SetHeight( 77 ) ;
	m_pChannelListBox->SetInstance( RYLCHANNELDLG::CHANNEL_LISTBOX ) ;

	m_pChannelListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pChannelListBox ) ;
	
	// 채널선택
	m_pChannelSelectLabel1 = new CRYLLabel( this ) ;
	m_pChannelSelectLabel1->SetLeft( 11 ) ;
	m_pChannelSelectLabel1->SetTop( 18 ) ;
	m_pChannelSelectLabel1->SetFontColor (0xffffffff);
	m_pChannelSelectLabel1->SetAutoSize( TRUE ) ;
	m_pChannelSelectLabel1->SetCaption( CRYLStringTable::m_strString[ 2274 ] ) ;
	AttachGMUIControl( m_pChannelSelectLabel1 ) ;

	// 채널을 선택하여 주십시요.
	m_pChannelSelectLabel2 = new CRYLLabel( this ) ;
	m_pChannelSelectLabel2->SetLeft( 14 ) ;
	m_pChannelSelectLabel2->SetTop( 45 ) ;
	m_pChannelSelectLabel2->SetFontColor (0xffffffff);
	m_pChannelSelectLabel2->SetAutoSize( TRUE ) ;
	m_pChannelSelectLabel2->SetCaption( CRYLStringTable::m_strString[ 2275 ] ) ;
	AttachGMUIControl( m_pChannelSelectLabel2 ) ;
	
	// 채널 1
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHANNEL ) ;
	CGMImageDib*	pImageDIB = NULL ;

	// 1
	m_pChannelPicture[ 0 ] = new CRYLPicture( this ) ;
	m_pChannelPicture[ 0 ]->SetLeft( 158 ) ;
	m_pChannelPicture[ 0 ]->SetTop( 68 ) ;	
	
	CRYLSpriteList::Instance()->CreateGMImage( 13, 13, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 13, 0, 26, 13 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, pSprite ) ;

	m_pChannelGoodImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChannelGoodImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChannelGoodImage ) ;
	
	m_pChannelPicture[ 0 ]->AttachImageTexture( (CGMTexture* )m_pChannelGoodImage ) ;
	m_pChannelPicture[ 0 ]->SetVisible( FALSE ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pChannelPicture[ 0 ] ) ;

	// 2
	m_pChannelPicture[ 1 ] = new CRYLPicture( this ) ;
	m_pChannelPicture[ 1 ]->SetLeft( 158 ) ;
	m_pChannelPicture[ 1 ]->SetTop( 84 ) ;	
	
	CRYLSpriteList::Instance()->CreateGMImage( 13, 13, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 26, 0, 38, 13 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, pSprite ) ;

	m_pChannelNormalImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChannelNormalImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChannelNormalImage ) ;
	
	m_pChannelPicture[ 1 ]->AttachImageTexture( (CGMTexture* )m_pChannelNormalImage ) ;
	m_pChannelPicture[ 1 ]->SetVisible( FALSE ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pChannelPicture[ 1 ] ) ;

	// 3
	m_pChannelPicture[ 2 ] = new CRYLPicture( this ) ;
	m_pChannelPicture[ 2 ]->SetLeft( 158 ) ;
	m_pChannelPicture[ 2 ]->SetTop( 100 ) ;	
	
	CRYLSpriteList::Instance()->CreateGMImage( 13, 13, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 13, 13 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, pSprite ) ;

	m_pChannelConfusionImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChannelConfusionImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChannelConfusionImage ) ;
	
	m_pChannelPicture[ 2 ]->AttachImageTexture( (CGMTexture* )m_pChannelConfusionImage ) ;
	m_pChannelPicture[ 2 ]->SetVisible( FALSE ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pChannelPicture[ 2 ] ) ;

	// 4
	m_pChannelPicture[ 3 ] = new CRYLPicture( this ) ;
	m_pChannelPicture[ 3 ]->SetLeft( 158 ) ;
	m_pChannelPicture[ 3 ]->SetTop( 116 ) ;	
	
	m_pChannelPicture[ 3 ]->AttachImageTexture( (CGMTexture* )m_pChannelConfusionImage ) ;
	m_pChannelPicture[ 3 ]->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pChannelPicture[ 3 ] ) ;

	// 5
	m_pChannelPicture[ 4 ] = new CRYLPicture( this ) ;
	m_pChannelPicture[ 4 ]->SetLeft( 158 ) ;
	m_pChannelPicture[ 4 ]->SetTop( 132 ) ;	
	
	m_pChannelPicture[ 4 ]->AttachImageTexture( (CGMTexture* )m_pChannelConfusionImage ) ;
	m_pChannelPicture[ 4 ]->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pChannelPicture[ 4 ] ) ;
}

VOID CRYLChannelDlg::FinalCtrl ()
{
	GM_DELETE( m_pChannelSelectLabel1 ) ;		// 채널선택
	GM_DELETE( m_pChannelSelectLabel2 ) ;		// 채널을 선택하여 주십시요.
	
	GM_DELETE( m_pChannelListBox ) ;			// 채널 1
	
	GM_DELETE( m_pChannelPicture[ 0 ] ) ;				// 채널 1
	GM_DELETE( m_pChannelPicture[ 1 ] ) ;				// 채널 2 
	GM_DELETE( m_pChannelPicture[ 2 ] ) ;				// 채널 3 
	GM_DELETE( m_pChannelPicture[ 3 ] ) ;				// 채널 4 
	GM_DELETE( m_pChannelPicture[ 4 ] ) ;				// 채널 5 
	
	GM_DELETE( m_pOKBtn ) ;						// OK Button
}

VOID CRYLChannelDlg::Show( CGMUIObject* Sender )
{
	
}

VOID CRYLChannelDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID CRYLChannelDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLChannelDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
	}

	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}
	return S_OK ;
}

VOID CRYLChannelDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pChannelDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCHANNELDLG::CHANNEL_LISTBOX : 
			break;

		case RYLCHANNELDLG::OK_BUTTON :
			{
				CRYLGameData::Instance()->m_dwClientMode = CLIENT_ZONEMOVE;
				g_pClientSocket->SetStatusFlag(NS_ZONEMOVE);
				INT iIndex = g_pChannelDlg->m_pChannelListBox->GetItemIndex() ;
 
				if ( iIndex == -1 )
				{
					CRYLMessageBox *lpMessageBox;
					lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[ 2276 ],  MB_CONFIRM | MB_EXIT );
					return ;
				}

				if ( CRYLSceneManager::Instance()->GetSceneID() == GAME_SCENE )
					SendPacket::ServerZone( g_GameSession, CRYLNetworkData::Instance()->m_dwNextZone, iIndex);
				else
					SendPacket::ServerZone( g_AuthSession, CRYLNetworkData::Instance()->m_dwNextZone, iIndex);

				CRYLGameData::Instance()->DeleteAllFieldItem() ;
			}
			break ;
	}
}

VOID	CRYLChannelDlg::InitChannel() 
{
	m_pChannelListBox->ClearItem() ;
	char	szBuff[ 32 ] ;
	char*	pZoneName = NULL ;

	switch( CRYLNetworkData::Instance()->m_dwNextZone )
	{
		case 1 : 	pZoneName = CRYLStringTable::m_strString[ 1154 ] ; 	break ;
		case 2 : 	pZoneName = CRYLStringTable::m_strString[ 1155 ] ; 	break ;
		case 3 : 	pZoneName = CRYLStringTable::m_strString[ 1156 ] ; 	break ;
		case 4 : 	pZoneName = CRYLStringTable::m_strString[ 1157 ] ; 	break ;
		case 5 : 	pZoneName = CRYLStringTable::m_strString[ 1158 ] ; 	break ;
		case 8 : 	pZoneName = CRYLStringTable::m_strString[ 1160 ] ; 	break ;
		case 9 : 	pZoneName = CRYLStringTable::m_strString[ 1166 ] ; 	break ;
		case 11 :	pZoneName = CRYLStringTable::m_strString[ 1163 ] ; 	break ;
        case 12 :   pZoneName = CRYLStringTable::m_strString[ 2734 ] ; 	break ;

        case 16 :   pZoneName = CRYLStringTable::m_strString[ 1156 ] ; 	break ;
        case 17 :   pZoneName = CRYLStringTable::m_strString[ 1156 ] ; 	break ;
        case 18 :   pZoneName = CRYLStringTable::m_strString[ 1156 ] ; 	break ;
	}	

	for ( INT i = 0 ; i < 5 ; i ++ )
	{
		m_pChannelPicture[ i ]->SetVisible( FALSE ) ;
	}

	for ( INT i = 0 ; i < CRYLNetworkData::Instance()->m_aryChannelNum ; i ++ )
	{
		sprintf( szBuff, "%s #%d", pZoneName, i + 1 ) ;
		m_pChannelListBox->AddItem( szBuff, 0xffffffff, 0xff00ffff, 0xffffffff ) ;		
		m_pChannelPicture[ i ]->SetVisible( TRUE ) ;
/*
		// 중국 : 1800명
		if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::CHINA )
		{
			int iResult = ( int )( ( ( float )CRYLNetworkData::Instance()->m_aryChannel[ i ] / 1800 ) * 100 ) ;

			if ( iResult <= 50 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelGoodImage ) ;
			else if ( iResult > 50 && iResult <= 70 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelNormalImage ) ;
			else if ( iResult > 70 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelConfusionImage ) ;
		}
		else
		{
*/
			INT iResult = ( int )( ( ( float )CRYLNetworkData::Instance()->m_aryChannel[ i ] / 1000 ) * 100 ) ;

			if ( iResult <= 50 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelGoodImage ) ;
			else if ( iResult > 50 && iResult <= 70 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelNormalImage ) ;
			else if ( iResult > 70 )
				m_pChannelPicture[ i ]->AttachImageTexture( (CGMTexture* )m_pChannelConfusionImage ) ;
//		}
	}
}