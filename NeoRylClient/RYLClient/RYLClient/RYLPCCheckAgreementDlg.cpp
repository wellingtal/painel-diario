#include <winsock2.h>
#include "RYLPCCheckAgreementDlg.h"
#include "RYLGameData.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"
#include "RYLNetworkData.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLStringTable.h"
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLPCCheckAgreementDlg* g_pPCCheckAgreementDlg = NULL ;

CRYLPCCheckAgreementDlg::CRYLPCCheckAgreementDlg () : CRYLDialog()
{
	m_pYesButton	= NULL;
	m_pNoButton		= NULL;
	m_pCaption		= NULL;

	RECT			rt;
	CGMImageDib*	pImageDIB	= NULL ;
	CRYLImage*		pImage		= NULL ;	
	CGMImageDib*	pBackImage	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 );

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	SetRect( &rt, 0, 0, 256, 15);
	pImageDIB->DrawRect( &rt, 0, 0, pBackImage);

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, ( 64 * 5 ) + 15 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;		

	int i = 0;
	for (  ; i < 64 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43);
		pImageDIB->DrawRect( &rt, 0, (i * 5), pBackImage ) ;		
	}	

	SetRect( &rt, 0, 42, 256, 52);
	pImageDIB->DrawRect( &rt, 0, (i * 5), pBackImage ) ;	

	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;			

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture((CGMTexture*)pImage);		

	GM_DELETE( pImageDIB ) ;
	
	g_pPCCheckAgreementDlg = this ;	

	SetWidth( 256 ) ;
	SetHeight( 350 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	InitCtrl() ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
}

CRYLPCCheckAgreementDlg::~CRYLPCCheckAgreementDlg ()
{
	FinalCtrl() ;
}

VOID CRYLPCCheckAgreementDlg::InitCtrl()
{
	RECT			rt;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 );
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance();
	
	m_pCaption = new CRYLLabel( this ) ;
	m_pCaption->SetLeft( 12 ) ;
	m_pCaption->SetTop( 17 ) ;
	m_pCaption->SetFontColor (0xffffffff);
	m_pCaption->SetAutoSize( TRUE ) ;
	m_pCaption->SetCaption( CRYLStringTable::m_strString[ 4300 ]);
	AttachGMUIControl( m_pCaption ) ;

	m_pYesButton = new CRYLButton(this) ;
	m_pYesButton->SetLeft(10);
	m_pYesButton->SetTop(300);
	m_pYesButton->SetCaption(CRYLStringTable::m_strString[4033]) ;
	m_pYesButton->SetFontColor( 0xffffffff) ;
	m_pYesButton->SetColor(0xffffffff) ;
	m_pYesButton->OnClickEvent = DlgClick ;
	m_pYesButton->SetInstance(RYLPCCHECKAGREENMENT::YES_BUTTON);
	m_pYesButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pYesButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pYesButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pYesButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE);
	AttachGMUIControl( m_pYesButton );

	m_pNoButton = new CRYLButton( this ) ;
	m_pNoButton->SetLeft(167) ;
	m_pNoButton->SetTop(300) ;
	m_pNoButton->SetCaption(CRYLStringTable::m_strString[4034]);
	m_pNoButton->SetFontColor( 0xffffffff);
	m_pNoButton->SetColor( 0xffffffff) ;
	m_pNoButton->OnClickEvent = DlgClick ;
	m_pNoButton->SetInstance(RYLPCCHECKAGREENMENT::NO_BUTTON);
	m_pNoButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pNoButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pNoButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pNoButton->AttachButtonImage(pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE);
	AttachGMUIControl( m_pNoButton );

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	m_pTextListBox = new CRYLListBox( this, TRUE ) ;
	m_pTextListBox->SetLeft( 12 ) ;
	m_pTextListBox->SetTop( 42 ) ;
	m_pTextListBox->SetWidth( 232 ) ;
	m_pTextListBox->SetHeight( 236 ) ;
	m_pTextListBox->SetInstance( RYLPCCHECKAGREENMENT::PCCHECKAGREE_LISTBOX ) ;

	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pTextListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pTextListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pTextListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pTextListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
	m_pTextListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pTextListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pTextListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pTextListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pTextListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pTextListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pTextListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pTextListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	char *strMessageTemp, *token, seps[] = "\\";
	
	for (int i = 0; i < 13; ++i)
	{
		if (strcmp( CRYLStringTable::m_strString[ 4301 + i ], ""))
		{
			strMessageTemp = CRYLStringTable::MakeWordBreak( CRYLStringTable::m_strString[ 4301 + i ], 220 ) ;
			token = strtok( strMessageTemp, seps ) ;

			while ( token != NULL )
			{
				CHAR strMessage[ MAX_PATH ] ;
				strcpy( strMessage, token ) ;
				token = strtok( NULL, seps ) ;

				m_pTextListBox->AddItem( strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;
			}
		}
	}

	m_pTextListBox->SetColor( 0xffffffff ) ;

	m_pTextListBox->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pTextListBox ) ;
}

VOID CRYLPCCheckAgreementDlg::FinalCtrl ()
{
	GM_DELETE(m_pYesButton);
	GM_DELETE(m_pNoButton);
	GM_DELETE(m_pCaption);	
	GM_DELETE(m_pTextListBox);
}

VOID CRYLPCCheckAgreementDlg::Show( CGMUIObject* Sender )
{
}

VOID CRYLPCCheckAgreementDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLPCCheckAgreementDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLPCCheckAgreementDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{ 
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if(GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
	}

	return S_OK ;
}

VOID CRYLPCCheckAgreementDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if(!g_pPCCheckAgreementDlg)
		return;

	switch(Sender->GetInstance())
	{
	case RYLPCCHECKAGREENMENT::YES_BUTTON:
		{
			// 이관 신청 하기.
			ShellExecute(NULL, "open", "http://www.tidesofwar.com", NULL, NULL, SW_SHOWNORMAL);//was returnofwarrior.com hayzohar

			// 종료 하기.
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
		}
		break;
	case RYLPCCHECKAGREENMENT::NO_BUTTON:
		{				
			// AuthAccount 보내기.

			g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
			
			g_ClientMain.m_wClientLoginFlag |= 0x1000;

			CRYLNetworkData::Instance()->SendLoginID( CRYLNetworkData::Instance()->m_strLoginID);
			
			SendPacket::AuthAccount(*g_pClientSocket, CRYLNetworkData::Instance()->m_strLoginID, 
										CRYLNetworkData::Instance()->m_strLoginPass, g_ClientMain.m_dwClientVersion,
										g_ClientMain.m_dwVersion, g_ClientMain.m_wClientLoginFlag);

			CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
			pScene->SetPCCheckAgreeMode(FALSE);			
		}
		break;		
	}
}
