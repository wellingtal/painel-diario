#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "WinInput.h"
#include "BaseGraphicsLayer.h"
#include "GMImageDib.h"

#include "RYLGMReportDlg.h"
#include "RYLChattingDlg.h"

#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLEdit.h"
#include "RYLLabel.h"
#include "RYLComboBox.h"

#include "RYLStringTable.h"
#include "RYLGameData.h"
#include "RYLMessageBox.h"

#include "GMMemory.h"


//------------------------------------------------------------------------------

CRYLGMReportDlg*	g_pGMReportDlg = NULL ;

CRYLGMReportDlg::CRYLGMReportDlg () : CRYLDialog()
{
	m_pSendBtn			= NULL ;
	m_pCancelBtn		= NULL ;
	m_pReportListCombo	= NULL ;
	m_pReportIDEdit		= NULL ;
	m_pReportDataEdit	= NULL ;

	m_pGMReportLabel		= NULL ;
	m_pGMReportListLabel	= NULL ;
	m_pGMReportIDLabel		= NULL ;
	m_pGMReportDataLabel	= NULL ;


	SetWidth( 256 ) ;
	SetHeight( 227 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	InitCtrl() ;

	g_pGMReportDlg = this ;	
}

CRYLGMReportDlg::~CRYLGMReportDlg ()
{
	FinalCtrl() ;
}

VOID CRYLGMReportDlg::InitCtrl ()
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 210, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 41 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	// 금줄
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 65, 25, 120, 19, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 65, 51, 120, 19, 0xff959595, 0x80000000,  1 ) ;

	// 사각 그리기 8,77			247,165
	pImageDIB->MakeFrameRect( 8, 77, 239, 1,  0xff946b42, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 8, 77, 1,	  88, 0xff946b42, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 8, 165, 239, 1, 0xff946b42, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 247, 77, 1, 88, 0xff946b42, 0xc0000000,  1 ) ;		

	pImageDIB->MakeFrameRect( 13, 96, 230, 1, 0xff959595, 0xc0000000,  1 ) ;		


	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;


	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pSendBtn = new CRYLButton( this ) ;
	m_pSendBtn->SetLeft( 53 ) ;
	m_pSendBtn->SetTop( 187 ) ;
	m_pSendBtn->SetCaption( CRYLStringTable::m_strString[ 1898 ] ) ;
	m_pSendBtn->SetFontColor( 0xffffffff) ;
	m_pSendBtn->SetColor( 0xffffffff) ;
	m_pSendBtn->OnClickEvent = DlgClick ;
	m_pSendBtn->SetInstance( RYLGMREPORTDLG::SEND_BUTTON ) ;
	m_pSendBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pSendBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pSendBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pSendBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pSendBtn ) ;

	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 137 ) ;
	m_pCancelBtn->SetTop( 187 ) ;
	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ]  ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLGMREPORTDLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;

	m_pReportIDEdit = new CRYLEdit( this ) ;
	m_pReportIDEdit->SetLeft( 66 ) ;
	m_pReportIDEdit->SetTop( 68 ) ;
	m_pReportIDEdit->SetWidth( 118 ) ;
	m_pReportIDEdit->SetHeight( 16 ) ;
	m_pReportIDEdit->SetColor( 0x80000000) ;
	m_pReportIDEdit->SetFontColor( 0xffffffff ) ;
	m_pReportIDEdit->SetInstance( RYLGMREPORTDLG::REPORTID_EDITBOX ) ;
	m_pReportIDEdit->SetMaxLength( 20 ) ; 
	m_pReportIDEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pReportIDEdit ) ;

	m_pReportDataEdit = new CRYLEdit( this ) ;
	m_pReportDataEdit->SetLeft( 12 ) ;
	m_pReportDataEdit->SetTop( 113 ) ;
	m_pReportDataEdit->SetWidth( 230 ) ;
	m_pReportDataEdit->SetHeight( 66 ) ;
	m_pReportDataEdit->SetColor( 0x80000000) ;
	m_pReportDataEdit->SetFontColor( 0xffffffff ) ;
	m_pReportDataEdit->SetInstance( RYLGMREPORTDLG::REPORTDATA_EDITBOX ) ;
	m_pReportDataEdit->SetMaxLength( 150 ) ;
	m_pReportDataEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pReportDataEdit ) ;



/*
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pReportListCombo = new CRYLCheckBox( this ) ;
	m_pReportListCombo->SetLeft( 195 ) ;
	m_pReportListCombo->SetTop( 115 ) ;
	m_pReportListCombo->SetInstance( RYLGMREPORTDLG::IDSAVE_CHECKBOX ) ;
	m_pReportListCombo->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pReportListCombo->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pReportListCombo->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pReportListCombo->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pReportListCombo->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pReportListCombo->SetChecked( FALSE ) ;
	AttachGMUIControl( m_pReportListCombo ) ;

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

	m_pGMReportIDLabel = new CRYLLabel( this ) ;
	m_pGMReportIDLabel->SetLeft( 43 ) ;
	m_pGMReportIDLabel->SetTop( 57 ) ;
	m_pGMReportIDLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 57, 78, 68 ) ;
	m_pGMReportIDLabel->SetAlignRect( rt ) ;
	m_pGMReportIDLabel->SetAlign( RIGHT ) ;
	m_pGMReportIDLabel->SetCaption( CRYLStringTable::m_strString[ 1900 ] ) ;
	m_pGMReportIDLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGMReportIDLabel ) ;

	m_pGMReportListLabel = new CRYLLabel( this ) ;
	m_pGMReportListLabel->SetLeft( 31 ) ;
	m_pGMReportListLabel->SetTop( 89 ) ;
	m_pGMReportListLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 89, 78, 100 ) ;
	m_pGMReportListLabel->SetAlignRect( rt ) ;
	m_pGMReportListLabel->SetAlign( RIGHT ) ;
	m_pGMReportListLabel->SetCaption( CRYLStringTable::m_strString[ 1901 ] ) ;
	m_pGMReportListLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGMReportListLabel ) ;

	m_pGMReportIDLabel = new CRYLLabel( this ) ;
	m_pGMReportIDLabel->SetLeft( 128 ) ;
	m_pGMReportIDLabel->SetTop( 118 ) ;
	m_pGMReportIDLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 11, 118, 188, 129 ) ;
	m_pGMReportIDLabel->SetAlignRect( rt ) ;
	m_pGMReportIDLabel->SetAlign( RIGHT ) ;
	m_pGMReportIDLabel->SetCaption( CRYLStringTable::m_strString[ 1902 ] ) ;
	m_pGMReportIDLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGMReportIDLabel ) ;
*/

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;	
}

VOID CRYLGMReportDlg::FinalCtrl ()
{
/*
	GM_DELETE( m_pSendBtn ) ;
	GM_DELETE( m_pCancelBtn ) ;
	GM_DELETE( m_pReportIDEdit ) ;
	GM_DELETE( m_pReportDataEdit ) ;
	GM_DELETE( m_pReportListCombo ) ;
	GM_DELETE( m_pGMReportIDLabel ) ;
	GM_DELETE( m_pGMReportListLabel ) ;
	GM_DELETE( m_pGMReportIDLabel ) ;
	GM_DELETE( m_pLoginLabel ) ;
*/
}

VOID CRYLGMReportDlg::Show( CGMUIObject* Sender )
{
	if (m_pReportIDEdit)
	{
		m_pReportIDEdit->SetFocus() ;
	}
	if ( m_pReportDataEdit )
	{
		m_pReportDataEdit->SetText ("");
	}
}

VOID CRYLGMReportDlg::Hide( CGMUIObject* Sender )
{
	if ( ( m_pReportIDEdit ) && ( m_pReportDataEdit ) )
	{
		m_pReportIDEdit->SetText ("");
		m_pReportDataEdit->SetText ("");
	}

	CRYLChattingDlg::Instance()->SetFocusChattingEdit();
}

VOID CRYLGMReportDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLGMReportDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible() )
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

	CRYLGameData* pGame = CRYLGameData::Instance();


	// 다음 처리



	return S_OK ;
}

VOID	CRYLGMReportDlg::SendButtonDown(void) 
{
/*
	CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance();

	if ( !strcmp( g_pGMReportDlg->m_pReportIDEdit->GetText(), "" ) )
	{	
		// 아이디가 없다.
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ; 
		lpMessageBox->Create( CRYLStringTable::m_strString[ 33 ] ) ;
		lpMessageBox->SetTimer( 1000 ) ;
	} 
	else if ( !strcmp( g_pGMReportDlg->m_pReportDataEdit->GetText(), "" ) )
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
			int nPassword = strlen(g_pGMReportDlg->m_pReportDataEdit->GetText());

			if(nPassword<6 || nPassword>12)
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox  ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3875] ) ;
				lpMessageBox->SetTimer( 1000 ) ;				
				return;
			}

			int nID = strlen(g_pGMReportDlg->m_pReportIDEdit->GetText());

			if(nID<6 || nID>12)
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox  ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3876] ) ;
				lpMessageBox->SetTimer( 1000 ) ;
				return;
			}
		}


		strcpy( lpNetwork->m_strLoginID, strlwr( g_pGMReportDlg->m_pReportIDEdit->GetText() ) ) ;

		char strReg[MAX_PATH];
		sprintf(strReg, "Software\\ROW\\MP-Client");

		if ( g_pGMReportDlg->m_pReportListCombo->GetChecked() )
		{
			HKEY hMPClientReg;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_WRITE, &hMPClientReg) == ERROR_SUCCESS)
			{
				RegSetValueEx(hMPClientReg, "UserID", 0, REG_SZ, (LPBYTE)g_pGMReportDlg->m_pReportIDEdit->GetText(), strlen( g_pGMReportDlg->m_pReportIDEdit->GetText() ) + 1);
				RegCloseKey(hMPClientReg);
			}
		} 


		CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
		if ( FALSE == pScene->GetAuthSuccess() )
		{
			strcpy( lpNetwork->m_strLoginID, g_pGMReportDlg->m_pReportIDEdit->GetText() ) ;

			if (GameRYL::KOREA == lpNetwork->m_eInternationalCode || 
				GameRYL::GLOBAL == lpNetwork->m_eInternationalCode)
			{
				// 패스워드 암호화 (16bytes에서 2배로 길이가 늘어난다.)
				MD5_CTX context;
				unsigned char digest[16] = { 0, };

				MD5Init(&context);
				MD5Update(&context, reinterpret_cast<unsigned char *>(g_pGMReportDlg->m_pReportDataEdit->GetText()), 
					strlen(g_pGMReportDlg->m_pReportDataEdit->GetText()));
				MD5Final(digest, &context);
				for (int i = 0; i < 16; ++i)
				{
					sprintf (lpNetwork->m_strLoginPass + i * 2, "%02x", digest[i]);
				}
			}
			else
			{
				strcpy( lpNetwork->m_strLoginPass, g_pGMReportDlg->m_pReportDataEdit->GetText() ) ;
	//			strncpy( lpNetwork->m_strLoginPass, g_pGMReportDlg->m_pReportDataEdit->GetText(), 16 ) ;
			}
	

			g_pClientSocket->SetStatusFlag( NS_AUTHACCOUNT ) ;
			lpNetwork->SendLoginID( g_pGMReportDlg->m_pReportIDEdit->GetText() ) ;
			SendPacket::AuthAccount( *g_pClientSocket, 
										lpNetwork->m_strLoginID, 
										lpNetwork->m_strLoginPass, 
										g_ClientMain.m_dwClientVersion, 
										g_ClientMain.m_dwVersion, 
										g_ClientMain.m_wClientLoginFlag ) ;
		}
		
		const GMIMEPACK* pImePack = g_pGMReportDlg->m_pReportDataEdit->GetImePack();
	}
*/
}

VOID CRYLGMReportDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pGMReportDlg )
		return ;
/*
	switch ( Sender->GetInstance() )
	{
		case RYLGMREPORTDLG::SEND_BUTTON : 
			g_pGMReportDlg->LoginButtonDown() ;			
			break;

		case RYLGMREPORTDLG::CANCEL_BUTTON :
			CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			break ;

		case RYLGMREPORTDLG::IDSAVE_CHECKBOX :

			if ( !g_pGMReportDlg->m_pReportListCombo->GetChecked() )
			{
				CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance();

				strcpy( lpNetwork->m_strLoginID, strlwr( g_pGMReportDlg->m_pReportIDEdit->GetText() ) ) ;

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
	}
*/
}


VOID CRYLGMReportDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pGMReportDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLGMREPORTDLG::REPORTID_EDITBOX :
		if (key == 13)
			g_pGMReportDlg->m_pReportDataEdit->SetFocus ();
		break;
	case RYLGMREPORTDLG::REPORTDATA_EDITBOX : 
		if (key == 13)
			g_pGMReportDlg->SendButtonDown() ;
		break;
	}
}
