#include "RYLClientMain.h"
#include "RYLLoginDlg.h"
#include "GMUIScreen.h"
#include "RYLButton.h"
#include "RYLEdit.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLCheckBox.h"
#include "BaseDataDefine.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLStringTable.h"

#include "RYLLoginScene.h"
#include "RYLSceneManager.h"

#include <Network/ClientSocket/Send/SendAuthServer.h>
 
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include <RenderOption.h>

extern "C"
{
#include <MD5/global.h>
#include <MD5/md5.h>
}

#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLLoginDlg*	g_pLoginDlg = NULL ;

CRYLLoginDlg::CRYLLoginDlg () : CRYLDialog()
{
	//SetColor (0xC4FFFFFF);

	m_pLoginBtn			= NULL ;
	m_pCancelBtn		= NULL ;
	m_pIDEdit			= NULL ;
	m_pPassWordEdit		= NULL ;
	m_pIDSaveCheckBox	= NULL ;

	m_pIDLabel			= NULL ;
	m_pPassWordLabel	= NULL ;
	m_pIDSaveLabel		= NULL ;
	
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

// remove by shadow
/*
//--
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
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 86, 38, 125, 19, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 86, 70, 125, 19, 0xff959595, 0x80000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 195 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
//--
*/


// add by shadow
//--
	
	CRYLSpriteList::Instance()->CreateGMImage( 800, 15, &pImageDIB ) ; // was 200 15
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 800, 15 ) ; // was 200, 15
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOGIN_MAIN ) ) ;
	


	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 800, 600, &pImageDIB ) ; // was 200, 250
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 800, 600 ) ; // was 0, 15, 200, 250
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NEWLOGIN_256x256 ) ) ;

	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;



	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 800 ) ; // was 200
	SetHeight( 600 ) ; // was 250
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	
//--

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pLoginDlg = this ;	
	InitCtrl() ;

	HKEY hMPClientReg;

	char strReg[MAX_PATH];
	sprintf(strReg, "Software\\ROW\\MP-Client");

	if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, strReg, 0, KEY_READ, &hMPClientReg ) )
	{
		char strUserID[MAX_PATH];
		unsigned long dwReadType = 0;
		unsigned long dwReadIDLens = 23;
		unsigned long dwReadPassLens = 35;	

		if ( ERROR_SUCCESS == RegQueryValueEx( hMPClientReg, "UserID", 0, &dwReadType, (LPBYTE)strUserID, &dwReadIDLens ) )
		{
			CRYLGameData::Instance()->m_bSaveID = TRUE;
			strncpy( CRYLNetworkData::Instance()->m_strLoginID, strUserID, dwReadIDLens );
			std::fill_n( CRYLNetworkData::Instance()->m_strLoginPass, dwReadPassLens, 0 );
		} 
		else
		{
			CRYLGameData::Instance()->m_bSaveID = FALSE;
			std::fill_n( CRYLNetworkData::Instance()->m_strLoginID, dwReadIDLens, 0 );
			std::fill_n( CRYLNetworkData::Instance()->m_strLoginPass, dwReadPassLens, 0 );
		}

		RegCloseKey(hMPClientReg);
	}
}

CRYLLoginDlg::~CRYLLoginDlg ()
{
	FinalCtrl() ;
}

VOID CRYLLoginDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pLoginBtn = new CRYLButton( this ) ;
	m_pLoginBtn->SetLeft( 22 ) ; // was 53
	m_pLoginBtn->SetTop( 378 ) ; // was 147 // 187
//	m_pLoginBtn->SetCaption( CRYLStringTable::m_strString[ 1898 ] ) ;
	m_pLoginBtn->SetFontColor( 0xffffffff) ;
	m_pLoginBtn->SetColor( 0xffffffff) ;
	m_pLoginBtn->OnClickEvent = DlgClick ;
	m_pLoginBtn->SetInstance( RYLLOGINDLG::LOGIN_BUTTON ) ;
	m_pLoginBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 13 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pLoginBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 13 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pLoginBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 13 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
//	m_pLoginBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pLoginBtn ) ;

	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 22 ) ; // was 137
	m_pCancelBtn->SetTop( 418 ) ; // was 147 // 147
//	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ]  ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLLOGINDLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 12 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 12 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 12 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
//	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;


	// new button register by shadow

	//--
	
	m_pRegisterBtn = new CRYLButton( this ) ;
	m_pRegisterBtn->SetLeft( 30 ) ; // was 137
	m_pRegisterBtn->SetTop( 332 ) ; // was 147
//	m_pFaceBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ]  ) ;
	m_pRegisterBtn->SetFontColor( 0xffffffff) ;
	m_pRegisterBtn->SetColor( 0xffffffff) ;
	m_pRegisterBtn->OnClickEvent = DlgClick ;
	m_pRegisterBtn->SetInstance( RYLLOGINDLG::REGISTER_BUTTON ) ;
	m_pRegisterBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 11 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRegisterBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 11 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRegisterBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 11 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
//	m_pRegisterBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 11 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRegisterBtn ) ;
	
	//--

	// new button facebook by shadow

	//--
	
	m_pFaceBtn = new CRYLButton( this ) ;
	m_pFaceBtn->SetLeft( 39 ) ; // was 137
	m_pFaceBtn->SetTop( 461 ) ; // was 147
//	m_pFaceBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ]  ) ;
	m_pFaceBtn->SetFontColor( 0xffffffff) ;
	m_pFaceBtn->SetColor( 0xffffffff) ;
	m_pFaceBtn->OnClickEvent = DlgClick ;
	m_pFaceBtn->SetInstance( RYLLOGINDLG::FACE_BUTTON ) ;
	m_pFaceBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 10 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pFaceBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 10 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pFaceBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 10 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
//	m_pFaceBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 7 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pFaceBtn ) ;
	
	//--


	m_pIDEdit = new CRYLEdit( this ) ;
	m_pIDEdit->SetLeft( 25 ) ; // was 88 // 22
	m_pIDEdit->SetTop( 175 ) ; // was 55 // 81 // 69
	m_pIDEdit->SetWidth( 155 ) ; // was 121
	m_pIDEdit->SetHeight( 16 ) ; // was 14
//	m_pIDEdit->SetColor( 0xff000000) ; // was 0xff000000
	m_pIDEdit->SetFontColor( 0xffffffff ) ; // was 0xffffffff
	m_pIDEdit->SetInstance( RYLLOGINDLG::ID_EDITBOX ) ;
	m_pIDEdit->SetMaxLength( 100 ) ; 
	m_pIDEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pIDEdit ) ;
/*
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
	{
		// 한국에서만 아이디 창에 영어만 입력 가능
		m_pIDEdit->SetEnglishOnly( true );
	}
*/
	m_pPassWordEdit = new CRYLEdit( this ) ;
	m_pPassWordEdit->SetLeft( 25 ) ; // was 88 // 22
	m_pPassWordEdit->SetTop( 233 ) ; // was 88 // 124
	m_pPassWordEdit->SetWidth( 155 ) ; // was 121
	m_pPassWordEdit->SetHeight( 16 ) ; // was 14
//	m_pPassWordEdit->SetColor( 0xff000000) ; // was 0xff000000
	m_pPassWordEdit->SetFontColor( 0xffffffff ) ; // was 0xffffffff
	m_pPassWordEdit->SetPassWord( '*' ) ;
	m_pPassWordEdit->SetInstance( RYLLOGINDLG::PASSWORD_EDITBOX ) ;
	m_pPassWordEdit->SetMaxLength( 8 ) ;
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
    {
		m_pPassWordEdit->SetMaxLength( 12 ) ;
        // 한국에서만 패스워드 창에 영어만 입력 가능
        m_pPassWordEdit->SetEnglishOnly( true );
    }
	else
    {
		m_pPassWordEdit->SetMaxLength( 20 ) ;
    }
	m_pPassWordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pPassWordEdit ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pIDSaveCheckBox = new CRYLCheckBox( this ) ;
	m_pIDSaveCheckBox->SetLeft( 16 ) ; // was 195 // 167
	m_pIDSaveCheckBox->SetTop( 271 ) ; // was 165
	m_pIDSaveCheckBox->SetInstance( RYLLOGINDLG::IDSAVE_CHECKBOX ) ;
	m_pIDSaveCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pIDSaveCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pIDSaveCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pIDSaveCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pIDSaveCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pIDSaveCheckBox->SetChecked( FALSE ) ;
	AttachGMUIControl( m_pIDSaveCheckBox ) ;

// disable by shadow

/*
//--
	m_pLoginLabel = new CRYLLabel( this ) ;
	m_pLoginLabel->SetLeft( 11 ) ;
	m_pLoginLabel->SetTop( 18 ) ;
	m_pLoginLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 18, 79, 29 ) ;
	m_pLoginLabel->SetAlignRect( rt ) ;
	m_pLoginLabel->SetAlign( LEFT ) ;
	m_pLoginLabel->SetCaption( CRYLStringTable::m_strString[ 1899 ] ) ;
	m_pLoginLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pLoginLabel ) ;

	m_pIDLabel = new CRYLLabel( this ) ;
	m_pIDLabel->SetLeft( 43 ) ;
	m_pIDLabel->SetTop( 57 ) ;
	m_pIDLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 57, 78, 68 ) ;
	m_pIDLabel->SetAlignRect( rt ) ;
	m_pIDLabel->SetAlign( RIGHT ) ;
	m_pIDLabel->SetCaption( CRYLStringTable::m_strString[ 1900 ] ) ;
	m_pIDLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pIDLabel ) ;

	m_pPassWordLabel = new CRYLLabel( this ) ;
	m_pPassWordLabel->SetLeft( 31 ) ;
	m_pPassWordLabel->SetTop( 89 ) ;
	m_pPassWordLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 89, 78, 100 ) ;
	m_pPassWordLabel->SetAlignRect( rt ) ;
	m_pPassWordLabel->SetAlign( RIGHT ) ;
	m_pPassWordLabel->SetCaption( CRYLStringTable::m_strString[ 1901 ] ) ;
	m_pPassWordLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pPassWordLabel ) ;

	m_pIDSaveLabel = new CRYLLabel( this ) ;
	m_pIDSaveLabel->SetLeft( 128 ) ;
	m_pIDSaveLabel->SetTop( 118 ) ;
	m_pIDSaveLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 118, 188, 129 ) ;
	m_pIDSaveLabel->SetAlignRect( rt ) ;
	m_pIDSaveLabel->SetAlign( RIGHT ) ;
	m_pIDSaveLabel->SetCaption( CRYLStringTable::m_strString[ 1902 ] ) ;
	m_pIDSaveLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pIDSaveLabel ) ;
//--
*/
}

VOID CRYLLoginDlg::FinalCtrl ()
{
	GM_DELETE( m_pLoginBtn ) ;
	GM_DELETE( m_pCancelBtn ) ;
	GM_DELETE( m_pIDEdit ) ;
	GM_DELETE( m_pPassWordEdit ) ;
	GM_DELETE( m_pIDSaveCheckBox ) ;
	GM_DELETE( m_pIDLabel ) ;
	GM_DELETE( m_pPassWordLabel ) ;
	GM_DELETE( m_pIDSaveLabel ) ;
	GM_DELETE( m_pLoginLabel ) ;
	GM_DELETE( m_pFaceBtn ) ;
	GM_DELETE( m_pRegisterBtn ) ;
}

VOID CRYLLoginDlg::Show( CGMUIObject* Sender )
{
	if (m_pIDEdit)
	{
		//m_pIDEdit->SetText( "" ) ;
		m_pIDEdit->SetFocus() ;
	}
	if ( m_pPassWordEdit )
	{
		m_pPassWordEdit->SetText( "" ) ;
	}
}

VOID CRYLLoginDlg::Hide( CGMUIObject* Sender )
{
	/*
	if ( ( m_pIDEdit ) && ( m_pPassWordEdit ) )
	{
		m_pIDEdit->SetText ("");
		m_pPassWordEdit->SetText ("");
	}
	*/
}

VOID CRYLLoginDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
	g_MessageBoxManager.Update();
}

HRESULT	CRYLLoginDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID	CRYLLoginDlg::LoginButtonDown(void) 
{
	CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance();

	if ( !strcmp( g_pLoginDlg->m_pIDEdit->GetText(), "" ) )
	{	
		// 아이디가 없다.
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( CRYLStringTable::m_strString[ 33 ] ) ;
		lpMessageBox->SetTimer( 1000 ) ;
	} 
	else if ( !strcmp( g_pLoginDlg->m_pPassWordEdit->GetText(), "" ) )
	{	
		// 패스워드가 없다.
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox  ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 34 ] ) ;
		lpMessageBox->SetTimer( 1000 ) ;
	} 
	else
	{
		if (GameRYL::KOREA == lpNetwork->m_eInternationalCode)
		{
			int nPassword = strlen(g_pLoginDlg->m_pPassWordEdit->GetText());

			if(nPassword<6 || nPassword>12)
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox  ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3875] ) ;
				lpMessageBox->SetTimer( 1000 ) ;				
				return;
			}

			int nID = strlen(g_pLoginDlg->m_pIDEdit->GetText());

			if(nID<6 || nID>12)
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox  ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3876] ) ;
				lpMessageBox->SetTimer( 1000 ) ;
				return;
			}
		}


		strcpy( lpNetwork->m_strLoginID, strlwr( g_pLoginDlg->m_pIDEdit->GetText() ) ) ;

		char strReg[MAX_PATH];
		sprintf(strReg, "Software\\ROW\\MP-Client");//was ROW hayzohar

		if ( g_pLoginDlg->m_pIDSaveCheckBox->GetChecked() )
		{
			HKEY hMPClientReg;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_WRITE, &hMPClientReg) == ERROR_SUCCESS)
			{
				RegSetValueEx(hMPClientReg, "UserID", 0, REG_SZ, (LPBYTE)g_pLoginDlg->m_pIDEdit->GetText(), strlen( g_pLoginDlg->m_pIDEdit->GetText() ) + 1);
				RegCloseKey(hMPClientReg);
			}
		} 
		/*else
		{
			HKEY hMPClientReg;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_WRITE, &hMPClientReg) == ERROR_SUCCESS)
			{
				RegDeleteValue(hMPClientReg, "UserID");
				RegCloseKey(hMPClientReg);
			}
		}*/

		CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
		if ( FALSE == pScene->GetAuthSuccess() )
		{
			strcpy( lpNetwork->m_strLoginID, g_pLoginDlg->m_pIDEdit->GetText() ) ;

			if (GameRYL::KOREA == lpNetwork->m_eInternationalCode || 
				GameRYL::GLOBAL == lpNetwork->m_eInternationalCode)
			{
				/* 패스워드 암호화 (16bytes에서 2배로 길이가 늘어난다.)
				MD5_CTX context;
				unsigned char digest[16] = { 0, };

				MD5Init(&context);
				MD5Update(&context, reinterpret_cast<unsigned char *>(g_pLoginDlg->m_pPassWordEdit->GetText()), 
					strlen(g_pLoginDlg->m_pPassWordEdit->GetText()));
				MD5Final(digest, &context);
				for (int i = 0; i < 16; ++i)
				{
					sprintf (lpNetwork->m_strLoginPass + i * 2, "%02x", digest[i]);
				}*/
				strcpy( lpNetwork->m_strLoginPass, g_pLoginDlg->m_pPassWordEdit->GetText() ) ;
			}
			else
			{
				strcpy( lpNetwork->m_strLoginPass, g_pLoginDlg->m_pPassWordEdit->GetText() ) ;
	//			strncpy( lpNetwork->m_strLoginPass, g_pLoginDlg->m_pPassWordEdit->GetText(), 16 ) ;
			}
	

			g_pClientSocket->SetStatusFlag( NS_AUTHACCOUNT ) ;
			lpNetwork->SendLoginID( g_pLoginDlg->m_pIDEdit->GetText() ) ;
			SendPacket::AuthAccount( *g_pClientSocket, 
										lpNetwork->m_strLoginID, 
										lpNetwork->m_strLoginPass, 
										g_ClientMain.m_dwClientVersion, 
										g_ClientMain.m_dwVersion, 
										g_ClientMain.m_wClientLoginFlag ) ;
		}
		
		const GMIMEPACK* pImePack = g_pLoginDlg->m_pPassWordEdit->GetImePack();
	}
}

VOID CRYLLoginDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pLoginDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLLOGINDLG::LOGIN_BUTTON : 
			g_pLoginDlg->LoginButtonDown() ;			
			break;

		case RYLLOGINDLG::CANCEL_BUTTON :
			CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			break ;

		case RYLLOGINDLG::IDSAVE_CHECKBOX :

			if ( !g_pLoginDlg->m_pIDSaveCheckBox->GetChecked() )
			{
				CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance();

				strcpy( lpNetwork->m_strLoginID, strlwr( g_pLoginDlg->m_pIDEdit->GetText() ) ) ;

				char strReg[MAX_PATH];
				sprintf(strReg, "Software\\ROW\\MP-Client");

				HKEY hMPClientReg;

				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_WRITE, &hMPClientReg) == ERROR_SUCCESS)
				{
					RegDeleteValue(hMPClientReg, "UserID");
					RegCloseKey(hMPClientReg);
				}
			}

			break ;
		
// facebook buton function by shadow
//--
		case RYLLOGINDLG::FACE_BUTTON:
		{
			// 픢xecute button

			ShellExecute(NULL, "open", "https://www.facebook.com/RYLTOW/", NULL, NULL, SW_SHOWNORMAL);

			// kill client when you press buttom

			//PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			//CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
		}
		break;
//--


		// facebook buton function by shadow
//--
		case RYLLOGINDLG::REGISTER_BUTTON:
		{
			// 픢xecute button

			ShellExecute(NULL, "open", "http://www.tidesofwar.com/Modules/Register.php", NULL, NULL, SW_SHOWNORMAL);

			// kill client when you press buttom

			//PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			//CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
		}
		break;
//--

	}
}



VOID CRYLLoginDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pLoginDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLLOGINDLG::ID_EDITBOX :
			if (key == 13)
				g_pLoginDlg->m_pIDEdit->SetFocus ();
			break;
		case RYLLOGINDLG::PASSWORD_EDITBOX : 
			if (key == 13)
				g_pLoginDlg->LoginButtonDown() ;
			break;
	}
}