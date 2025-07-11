#include <winsock2.h>
#include "RYLPasswordReAuthDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLNetworkData.h"
#include "RYLMessageBox.h"
#include "RYLLoginDlg.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLIntegServerSelCharDlg.h"
#include "RYLCharacterSelectScene.h"
#include "RYLNationSelectDlg.h"
#include "RYLStringTable.h"
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
extern "C"
{
#include "MD5/global.h"
#include "MD5/md5.h"
}
#include "GMMemory.h"
//------------------------------------------------------------------------------

CRYLPasswordReAuthDlg*	g_pPasswordReAuthDlg = NULL ;

CRYLPasswordReAuthDlg::CRYLPasswordReAuthDlg () : CRYLDialog()
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 165, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 31 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;		   
	pImageDIB->MakeFrameRect( 66, 69, 125, 19, 0xff959595, 0x80000000,  1 ) ;
		
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 162 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pPasswordReAuthDlg = this ;	
	InitCtrl() ;

	m_dwResult = 0 ;
	m_bAddCharEnable = true ;
}

CRYLPasswordReAuthDlg::~CRYLPasswordReAuthDlg ()
{
	FinalCtrl() ;
}

VOID CRYLPasswordReAuthDlg::InitCtrl ()
{
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pConfirmButton = new CRYLButton( this ) ;
	m_pConfirmButton->SetLeft( 90 ) ;
	m_pConfirmButton->SetTop( 122 ) ;
	m_pConfirmButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pConfirmButton->SetFontColor( 0xffffffff) ;
	m_pConfirmButton->SetColor( 0xffffffff) ;
	m_pConfirmButton->OnClickEvent = DlgClick ;
	m_pConfirmButton->SetInstance( RYLPASSWORDREAUTHDLG::CONFIRM_BUTTON ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConfirmButton ) ;

	m_pPasswordEdit = new CRYLEdit( this ) ;
	m_pPasswordEdit->SetLeft( 67 ) ;
	m_pPasswordEdit->SetTop( 85 ) ;
	m_pPasswordEdit->SetWidth( 123 ) ;
	m_pPasswordEdit->SetHeight( 17 ) ;
	m_pPasswordEdit->SetColor( 0xff000000) ;
	m_pPasswordEdit->SetFontColor( 0xffffffff ) ;
	m_pPasswordEdit->SetPassWord( '*' ) ;
	m_pPasswordEdit->SetInstance( RYLPASSWORDREAUTHDLG::PASSWORD_EDITBOX ) ;
	m_pPasswordEdit->SetMaxLength( 8 ) ;
    if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
    {
		m_pPasswordEdit->SetMaxLength( 12 ) ;
        // 한국에서만 패스워드 창에 영어만 입력 가능
        m_pPasswordEdit->SetEnglishOnly( true );
    }
	else
    {
		m_pPasswordEdit->SetMaxLength( 16 ) ;
    }
	m_pPasswordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pPasswordEdit ) ;

	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 9 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 3527 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;

	m_pDescLabel = new CRYLLabel( this ) ;
	m_pDescLabel->SetLeft( 30 ) ;
	m_pDescLabel->SetTop( 57 ) ;
	m_pDescLabel->SetFontColor (0xffffffff);
	m_pDescLabel->SetAutoSize( TRUE ) ;
	m_pDescLabel->SetCaption( CRYLStringTable::m_strString[ 3528 ] ) ;
	AttachGMUIControl( m_pDescLabel ) ;
}

VOID CRYLPasswordReAuthDlg::FinalCtrl ()
{
	GM_DELETE( m_pConfirmButton ) ;
	GM_DELETE( m_pPasswordEdit ) ;
	GM_DELETE( m_pTitleLabel ) ;
	GM_DELETE( m_pDescLabel ) ;
}

VOID CRYLPasswordReAuthDlg::Show( CGMUIObject* Sender )
{
	if ( m_pPasswordEdit )
	{
		m_pPasswordEdit->SetText( "" ) ;
		m_pPasswordEdit->SetFocus() ;
	}
}

VOID CRYLPasswordReAuthDlg::Hide( CGMUIObject* Sender )
{
	/*
	if ( m_pPasswordEdit )
	{
		m_pPasswordEdit->SetText ("");
	}
	*/
}

VOID CRYLPasswordReAuthDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLPasswordReAuthDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

bool	CRYLPasswordReAuthDlg::PreCheckPassword() 
{
	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	CRYLLoginDlg* pDlg = static_cast< CRYLLoginDlg* >( pScene->GetLoginDlg() ) ;
	
	if ( strcmp( m_pPasswordEdit->GetText(), pDlg->m_pPassWordEdit->GetText() ) )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 3529 ] ) ; 
		m_pPasswordEdit->SetText( "" ) ;
		return false ;
	}

	return true ;
}

void	CRYLPasswordReAuthDlg::CheckPassword() 
{
	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	char* lpszPassword = NULL ;
	if ( m_bAddCharEnable )
	{
		lpszPassword = m_pPasswordEdit->GetText() ;
	}
	else
	{
		CRYLLoginDlg* pDlg = static_cast< CRYLLoginDlg* >( pScene->GetLoginDlg() ) ;
		lpszPassword = pDlg->m_pPassWordEdit->GetText() ;
	}
	

	char szPassword[ 33 ] ;
	if (GameRYL::KOREA == CRYLNetworkData::Instance()->m_eInternationalCode)
	{
		// 패스워드 암호화 (16bytes에서 2배로 길이가 늘어난다.)
		
		MD5_CTX context;
		unsigned char digest[16] ;
		memset( digest, 0, sizeof( char ) * 16 ) ;

		MD5Init(&context);
		MD5Update(&context, reinterpret_cast<unsigned char *>( lpszPassword ), 
			strlen( lpszPassword ) );
		MD5Final(digest, &context);
		for (int i = 0; i < 16; ++i)
		{
			sprintf (szPassword + i * 2, "%02x", digest[i]);
		}
	}
	else
	{
		strncpy( szPassword, lpszPassword, 16 ) ;
	}
	
	
	CRYLIntegServerSelVaultDlg* pVaultDlg = static_cast<CRYLIntegServerSelVaultDlg*>( pScene->GetIntegServerSelVaultDlg() ) ;
	CRYLIntegServerSelCharDlg* pCharDlg = static_cast<CRYLIntegServerSelCharDlg*>( pScene->GetIntegServerSelCharDlg() ) ;
	
	USER_INFO UserInfo = pCharDlg->GetUserInfo() ;
	unsigned long dwCID[ 5 ] = { 0, 0, 0, 0, 0 } ;
	
	for ( int i = 0 ; i < 5 ; i ++ )
	{
		if ( ( UserInfo.CharID[ i ] == 0 ) && pCharDlg->m_SelectChar[ i ].dwCID )
		{
			dwCID[ i ] = pCharDlg->m_SelectChar[ i ].dwCID ;
		}
	}
			
	if ( false == ClientSocket::IsNSFlagChecked(CmdUnifiedCharSelect) )
	{
		CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ) ;
		CRYLNationSelectDlg* pDlg = static_cast<CRYLNationSelectDlg*>( pScene->GetNationSelectDlg() ) ;
		unsigned char cSelectedNation = pDlg->GetSelectNation() ;

		SendPacket::UnifiedCharSelect( g_AuthSession, 
									szPassword, 
									dwCID, 
									USER_INFO::MAX_CHAR_NUM, 
									pVaultDlg->m_dwSelectVaultServer,
									cSelectedNation ) ; 
	}
}

VOID CRYLPasswordReAuthDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pPasswordReAuthDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLPASSWORDREAUTHDLG::CONFIRM_BUTTON : 
			{
				CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
				if ( g_pPasswordReAuthDlg->PreCheckPassword() )
				{
					g_pPasswordReAuthDlg->SetVisible( FALSE ) ;
					g_pPasswordReAuthDlg->CheckPassword() ;
				}			
			}
			break;
	}
}

VOID CRYLPasswordReAuthDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pPasswordReAuthDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLPASSWORDREAUTHDLG::PASSWORD_EDITBOX :
			if (key == 13)
			{
				CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
				if ( g_pPasswordReAuthDlg->PreCheckPassword() )
				{
					g_pPasswordReAuthDlg->SetVisible( FALSE ) ;
					g_pPasswordReAuthDlg->CheckPassword() ;
				}			
			}
			break;
	}
}