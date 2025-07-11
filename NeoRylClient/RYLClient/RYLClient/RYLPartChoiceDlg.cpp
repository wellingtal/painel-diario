#include <winsock2.h>
#include "RYLPartChoiceDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLCheckBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLNetworkData.h"
#include "RYLMessageBox.h"
#include "RYLClientMain.h"
#include "RYLNetworkData.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLPasswordReAuthDlg.h"
#include "RYLStringTable.h"
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLPartChoiceDlg*	g_pPartChoiceDlg = NULL ;

CRYLPartChoiceDlg::CRYLPartChoiceDlg () : CRYLDialog()
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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 330, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	int i = 0 ;
	for ( ; i < 64 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	//pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff6e6857, 0xc0000000,  1 ) ;		
	//pImageDIB->MakeFrameRect( 7, 24, 242, 246, 0xff6e6857, 0xc0000000,  1 ) ;		
	
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
	
	g_pPartChoiceDlg = this ;	
	InitCtrl() ;
}

CRYLPartChoiceDlg::~CRYLPartChoiceDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLPartChoiceDlg::InitCtrl()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	
	m_pDescLabel = new CRYLLabel( this ) ;
	m_pDescLabel->SetLeft( 30 ) ;
	m_pDescLabel->SetTop( 314 ) ;
	m_pDescLabel->SetFontColor (0xffffffff);
	m_pDescLabel->SetAutoSize( TRUE ) ;
	m_pDescLabel->SetCaption( CRYLStringTable::m_strString[ 3508 ] ) ;
	AttachGMUIControl( m_pDescLabel ) ;

	m_pRYLDownButton = new CRYLButton( this ) ;
	m_pRYLDownButton->SetLeft( 84 ) ;
	m_pRYLDownButton->SetTop( 281 ) ;
	m_pRYLDownButton->SetCaption( CRYLStringTable::m_strString[ 3509 ] ) ;
	m_pRYLDownButton->SetFontColor( 0xffffffff) ;
	m_pRYLDownButton->SetColor( 0xffffffff) ;
	m_pRYLDownButton->OnClickEvent = DlgClick ;
	m_pRYLDownButton->SetInstance( RYLPARTCHOICE::DOWN_BUTTON ) ;
	m_pRYLDownButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRYLDownButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRYLDownButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRYLDownButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRYLDownButton ) ;

	m_pPart1Button = new CRYLButton( this ) ;
	m_pPart1Button->SetLeft( 165 ) ;
	m_pPart1Button->SetTop( 281 ) ;
	m_pPart1Button->SetCaption( CRYLStringTable::m_strString[ 3510 ] ) ;
	m_pPart1Button->SetFontColor( 0xffffffff) ;
	m_pPart1Button->SetColor( 0xffffffff) ;
	m_pPart1Button->OnClickEvent = DlgClick ;
	m_pPart1Button->SetInstance( RYLPARTCHOICE::PART1_BUTTON ) ;
	m_pPart1Button->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPart1Button->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPart1Button->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPart1Button->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPart1Button ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pDescListBox = new CRYLListBox( this, FALSE ) ;
	m_pDescListBox->SetLeft( 10 ) ;
	m_pDescListBox->SetTop( 23 ) ;
	m_pDescListBox->SetWidth( 236 ) ;
	m_pDescListBox->SetHeight( 253 ) ;
	m_pDescListBox->SetInstance( RYLPARTCHOICE::DESC_LISTBOX ) ;

	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pDescListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pDescListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pDescListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pDescListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_pDescListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pDescListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pDescListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pDescListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pDescListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pDescListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pDescListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pDescListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
	
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3511 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3512 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3513 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3514 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3515 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3516 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3517 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3518 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3519 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3520 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3521 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3522 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3523 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( " ", 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3524 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3525 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	m_pDescListBox->AddItem( CRYLStringTable::m_strString[ 3526 ], 0xffffffff, 0xffffffff, 0xffffffff ) ;

	m_pDescListBox->SetColor( 0xffffffff ) ;
	
	m_pDescListBox->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pDescListBox ) ;

	m_pTodayCheckBox = new CRYLCheckBox( this ) ;
	m_pTodayCheckBox->SetLeft( 7 ) ;
	m_pTodayCheckBox->SetTop( 311 ) ;
	m_pTodayCheckBox->SetInstance( RYLPARTCHOICE::TODAYSHOW_CBBOX ) ;
	m_pTodayCheckBox->OnClickEvent = DlgClick ;
	m_pTodayCheckBox->DefaultEvent( m_pTodayCheckBox ) ;
	m_pTodayCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pTodayCheckBox ) ;
}

VOID	CRYLPartChoiceDlg::FinalCtrl ()
{
	GM_DELETE( m_pRYLDownButton ) ;
	GM_DELETE( m_pPart1Button ) ;
	GM_DELETE( m_pDescListBox ) ;
	GM_DELETE( m_pTodayCheckBox ) ;
	GM_DELETE( m_pDescLabel ) ;
}

VOID	CRYLPartChoiceDlg::Show( CGMUIObject* Sender )
{
}

VOID	CRYLPartChoiceDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLPartChoiceDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLPartChoiceDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{ 
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID	CRYLPartChoiceDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pPartChoiceDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLPARTCHOICE::DOWN_BUTTON : 
			break;

		case RYLPARTCHOICE::PART1_BUTTON :
			{
				/*
				g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
				g_ClientMain.m_wClientLoginFlag |= 0x8000;
				CRYLNetworkData::Instance()->SendLoginID( CRYLNetworkData::Instance()->m_strLoginID);
				SendPacket::AuthAccount(*g_pClientSocket, CRYLNetworkData::Instance()->m_strLoginID, 
										CRYLNetworkData::Instance()->m_strLoginPass, 
										g_ClientMain.m_dwClientVersion, g_ClientMain.m_dwVersion, 
										g_ClientMain.m_wClientLoginFlag);
				*/
				CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
				CRYLPasswordReAuthDlg* pDlg = static_cast< CRYLPasswordReAuthDlg* >( pScene->GetPasswordReAuthDlg() ) ;
				pDlg->CheckPassword() ;
				g_pPartChoiceDlg->SetVisible( FALSE ) ;
			}
			break ;

		case RYLPARTCHOICE::DESC_LISTBOX :
			break ;

		case RYLPARTCHOICE::TODAYSHOW_CBBOX :
			break ;
	}
}
