#include <winsock2.h>
#include "RYLAgreeDlg.h"
#include "GMUIScreen.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLStringTable.h"
#include "BaseDataDefine.h"

#include "RYLClientMain.h"
#include "RYLNetworkData.h"
#include "RYLGameData.h"

#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"

#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLLoginDlg.h"
#include <RenderOption.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLAgreeDlg*	g_pAgreeDlg = NULL ;

CRYLAgreeDlg::CRYLAgreeDlg () : CRYLDialog()
{
	m_pAgreeBtn			= NULL ;
	m_pCancelBtn		= NULL ;
	m_pAgreeListBox		= NULL ;
	
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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 320, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	int i = 0 ;
	for ( ; i < 57 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 21 + ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 7, 24, 242, 246, 0xff6e6857, 0xc0000000,  1 ) ;		
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 315 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pAgreeDlg = this ;	
	InitCtrl() ;
}

CRYLAgreeDlg::~CRYLAgreeDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLAgreeDlg::InitCtrl()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	
	m_pAgreeLabel = new CRYLLabel( this ) ;
	m_pAgreeLabel->SetLeft( 9 ) ;
	m_pAgreeLabel->SetTop( 18 ) ;
	m_pAgreeLabel->SetFontColor (0xffffffff);
	m_pAgreeLabel->SetAutoSize( TRUE ) ;
	m_pAgreeLabel->SetCaption( CRYLStringTable::m_strString[ 1895 ] ) ;
	AttachGMUIControl( m_pAgreeLabel ) ;

	m_pAgreeBtn = new CRYLButton( this ) ;
	m_pAgreeBtn->SetLeft( 99 ) ;
	m_pAgreeBtn->SetTop( 291    ) ;
	m_pAgreeBtn->SetCaption( CRYLStringTable::m_strString[ 1896 ] ) ;
	m_pAgreeBtn->SetFontColor( 0xffffffff) ;
	m_pAgreeBtn->SetColor( 0xffffffff) ;
	m_pAgreeBtn->OnClickEvent = DlgClick ;
	m_pAgreeBtn->SetInstance( RYLAGREEDLG::AGREE_BUTTON ) ;
	m_pAgreeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAgreeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAgreeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAgreeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pAgreeBtn ) ;

	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 183 ) ;
	m_pCancelBtn->SetTop( 291 ) ;
	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLAGREEDLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;


	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pAgreeListBox = new CRYLListBox( this, TRUE ) ;
	m_pAgreeListBox->SetLeft( 12 ) ;
	m_pAgreeListBox->SetTop( 42 ) ;
	m_pAgreeListBox->SetWidth( 232 ) ;
	m_pAgreeListBox->SetHeight( 236 ) ;
	m_pAgreeListBox->SetInstance( RYLAGREEDLG::AGREE_LISTBOX ) ;

	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pAgreeListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pAgreeListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pAgreeListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pAgreeListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_pAgreeListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pAgreeListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pAgreeListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pAgreeListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pAgreeListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pAgreeListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pAgreeListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pAgreeListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
	
	char *strMessageTemp, *token, seps[] = "\\";

	// 배틀 로한용 약관
	if (GameRYL::SERVER_BATTLE_LOHAN == CRYLNetworkData::Instance()->m_eServerType)
	{
		for (int i = 0; i < 126; ++i)
		{
			if (strcmp( CRYLStringTable::m_strString[ 4200 + i ], ""))
			{
				strMessageTemp = CRYLStringTable::MakeWordBreak( CRYLStringTable::m_strString[ 4200 + i ], 220 ) ;
				token = strtok( strMessageTemp, seps ) ;

				while ( token != NULL )
				{
					CHAR strMessage[ MAX_PATH ] ;
					strcpy( strMessage, token ) ;
					token = strtok( NULL, seps ) ;

					m_pAgreeListBox->AddItem( strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;
				}
			}
		}
	}
	// R.Y.L Online 약관
	else
	{
		for (int i = 0; i < 160; ++i)
		{
			if (strcmp( CRYLStringTable::m_strString[ 4000 + i ], ""))
			{
				strMessageTemp = CRYLStringTable::MakeWordBreak( CRYLStringTable::m_strString[ 4000 + i ], 220 ) ;
				token = strtok( strMessageTemp, seps ) ;

				while ( token != NULL )
				{
					CHAR strMessage[ MAX_PATH ] ;
					strcpy( strMessage, token ) ;
					token = strtok( NULL, seps ) ;

					m_pAgreeListBox->AddItem( strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;
				}
			}
		}
	}

	m_pAgreeListBox->SetColor( 0xffffffff ) ;
	
	m_pAgreeListBox->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pAgreeListBox ) ;
}

VOID	CRYLAgreeDlg::FinalCtrl ()
{
	GM_DELETE( m_pAgreeBtn ) ;
	GM_DELETE( m_pCancelBtn ) ;
	GM_DELETE( m_pAgreeListBox ) ;
	GM_DELETE( m_pAgreeLabel ) ;
}

VOID	CRYLAgreeDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLAgreeDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLAgreeDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}
 
HRESULT	CRYLAgreeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID	CRYLAgreeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pAgreeDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLAGREEDLG::AGREE_BUTTON : 
			{
				g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
				g_ClientMain.m_wClientLoginFlag |= 0x8000;
				CRYLNetworkData::Instance()->SendLoginID( CRYLNetworkData::Instance()->m_strLoginID);
				SendPacket::AuthAccount(*g_pClientSocket, CRYLNetworkData::Instance()->m_strLoginID, 
										CRYLNetworkData::Instance()->m_strLoginPass, 
										g_ClientMain.m_dwClientVersion, g_ClientMain.m_dwVersion, 
										g_ClientMain.m_wClientLoginFlag);

				CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
				pScene->SetAgreeMode( FALSE ) ;
				g_pAgreeDlg->SetVisible( FALSE ) ;
			}
			break;

		case RYLAGREEDLG::CANCEL_BUTTON :
			{
				SendMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
				CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
			}
			break ;
	}
}

VOID		CRYLAgreeDlg::InsertStringAgreeListBox() 
{
}