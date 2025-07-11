#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLVaultPasswordDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLCheckBox.h"

#include "BaseDataDefine.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include <WinInput.h>
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLStringTable.h"
#include "RYLNetworkData.h"
#include "RYLVaultDlg.h"
#include <Network/ClientSocket/Send/SendItem.h>

//-- 2004. 1. 29. Zergra From.--//
#include "RYLChattingDlg.h"
//-- Zergra To. --//
#include <RenderOption.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------
//					
//------------------------------------------------------------------------------
CRYLVaultPasswordDlg*	g_pVaultPasswordDlg = NULL ;

CRYLVaultPasswordDlg::CRYLVaultPasswordDlg() : CRYLDialog()
{
	m_pPasswordLabel			= NULL ;		// 페스워드변경 이름
	m_pCurrentPasswordEdit		= NULL ;		// 현제 비밀번호
	m_pChangePasswordEdit		= NULL ;		// 변경 비밀번호
	m_pOkButton					= NULL ;		// 확인 버튼
	m_pCancelButton				= NULL ;		// 취소 버튼

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
	for (  ; i < 34 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 91, 51, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 91, 102, 75, 19, 0xff959595, 0xc0000000,  1 ) ;	
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 20, 252, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 193 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pVaultPasswordDlg = this ;	
	InitCtrl() ;
}

CRYLVaultPasswordDlg::~CRYLVaultPasswordDlg ()
{
	FinalCtrl() ;
}

VOID CRYLVaultPasswordDlg::InitCtrl ()
{
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 페스워드변경 이름
	m_pPasswordLabel = new CRYLLabel( this ) ;
	m_pPasswordLabel->SetLeft( 9 ) ;
	m_pPasswordLabel->SetTop( 18 ) ;
	m_pPasswordLabel->SetFontColor (0xffffffff);
	m_pPasswordLabel->SetAutoSize( TRUE ) ;
	m_pPasswordLabel->SetCaption( CRYLStringTable::m_strString[ 2205 ] ) ;
	AttachGMUIControl( m_pPasswordLabel ) ;

	// 현제 비밀번호
	m_pCurrentPasswordLabel = new CRYLLabel( this ) ;
	m_pCurrentPasswordLabel->SetLeft( 89) ;
	m_pCurrentPasswordLabel->SetTop( 48 ) ;
	m_pCurrentPasswordLabel->SetFontColor (0xffffffff);
	m_pCurrentPasswordLabel->SetAutoSize( TRUE ) ;
	m_pCurrentPasswordLabel->SetCaption( CRYLStringTable::m_strString[ 2206 ] ) ;
	AttachGMUIControl( m_pCurrentPasswordLabel ) ;

	// 변경 비밀번호
	m_pChangePasswordLabel = new CRYLLabel( this ) ;
	m_pChangePasswordLabel->SetLeft( 89 ) ;
	m_pChangePasswordLabel->SetTop( 100 ) ;
	m_pChangePasswordLabel->SetFontColor (0xffffffff);
	m_pChangePasswordLabel->SetAutoSize( TRUE ) ;
	m_pChangePasswordLabel->SetCaption( CRYLStringTable::m_strString[ 2207 ] ) ;
	AttachGMUIControl( m_pChangePasswordLabel ) ;

	// 확인 버튼
	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 33 ) ;
	m_pOkButton->SetTop( 151 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLVAULTPASSWORDDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;
		
	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 143 ) ;
	m_pCancelButton->SetTop( 151 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLVAULTPASSWORDDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;	

	// 현제 비밀번호
	m_pCurrentPasswordEdit = new CRYLEdit( this ) ;
	m_pCurrentPasswordEdit->SetLeft( 93 ) ;
	m_pCurrentPasswordEdit->SetTop( 68 ) ;
	m_pCurrentPasswordEdit->SetWidth( 71 ) ;
	m_pCurrentPasswordEdit->SetHeight( 15 ) ;
	m_pCurrentPasswordEdit->SetColor( 0xff000000) ;
	m_pCurrentPasswordEdit->SetFontColor( 0xffffffff ) ;
	m_pCurrentPasswordEdit->SetPassWord( '*' ) ;
	m_pCurrentPasswordEdit->SetInstance( RYLVAULTPASSWORDDLG::CURRENTPASSWORD_EDIT ) ;
	m_pCurrentPasswordEdit->SetMaxLength( 4 ) ;
	m_pCurrentPasswordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pCurrentPasswordEdit ) ;

	// 변경 비밀번호
	m_pChangePasswordEdit = new CRYLEdit( this ) ;
	m_pChangePasswordEdit->SetLeft( 93 ) ;
	m_pChangePasswordEdit->SetTop( 119 ) ;
	m_pChangePasswordEdit->SetWidth( 71 ) ;
	m_pChangePasswordEdit->SetHeight( 15 ) ;
	m_pChangePasswordEdit->SetColor( 0xff000000) ;
	m_pChangePasswordEdit->SetFontColor( 0xffffffff ) ;
	m_pChangePasswordEdit->SetPassWord( '*' ) ;
	m_pChangePasswordEdit->SetInstance( RYLVAULTPASSWORDDLG::CHANGEPASSWORD_EDIT ) ;
	m_pChangePasswordEdit->SetMaxLength( 4 ) ;
	m_pChangePasswordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pChangePasswordEdit ) ;
}

VOID CRYLVaultPasswordDlg::FinalCtrl ()
{
	GM_DELETE( m_pPasswordLabel ) ;		
	GM_DELETE( m_pCurrentPasswordLabel ) ; 
	GM_DELETE( m_pChangePasswordLabel ) ;
	GM_DELETE( m_pOkButton ) ;			
	GM_DELETE( m_pCancelButton ) ;		
	GM_DELETE( m_pCurrentPasswordEdit ) ;
	GM_DELETE( m_pChangePasswordEdit ) ;	
}

VOID CRYLVaultPasswordDlg::Show( CGMUIObject* Sender )
{
	m_pCurrentPasswordEdit->SetFocus() ;
}

VOID CRYLVaultPasswordDlg::Hide( CGMUIObject* Sender )
{
    if ( CRYLChattingDlg::Instance() )
    {
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE );
    }
}

VOID CRYLVaultPasswordDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLVaultPasswordDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLVaultPasswordDlg::OKButtonEvent() 
{
	BOOL	bPass = TRUE;
	DWORD	i ;
	CHAR*   pszCurPass = m_pCurrentPasswordEdit->GetText() ;
	CHAR*	pszNewPass = m_pChangePasswordEdit->GetText() ;
	if ( strlen( pszCurPass ) < 4 ) 
		bPass = FALSE ;
	if ( strlen( pszNewPass ) < 4 ) 
		bPass = FALSE ;

	for ( i = 0 ; i < 4 ; i ++ )
	{
		if ( !( ( 48 <= pszCurPass[ i ] && pszCurPass[ i ] <= 57 ) || 
			    ( 65 <= pszCurPass[ i ] && pszCurPass[ i ] <= 90 ) || 
				( 97 <= pszCurPass[ i ] && pszCurPass[ i ] <= 122 ) ) )  
				bPass = FALSE ;

		if ( !( ( 48 <= pszNewPass[ i ] && pszNewPass[ i ] <= 57 ) || 
			    ( 65 <= pszNewPass[ i ] && pszNewPass[ i ] <= 90 ) || 
				( 97 <= pszNewPass[ i ] && pszNewPass[ i ] <= 122 ) ) ) 
				bPass = FALSE ;
	}

	if ( bPass )
	{
		SendPacket::StoreChangePass( g_GameSession, pszCurPass, pszNewPass ) ;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 617 ] ) ;
		m_pCurrentPasswordEdit->SetText( "" ) ;
		m_pChangePasswordEdit->SetText( "" ) ;
		//g_TextEdit.m_dwEditMode = EDITMODE_CURPASS ;
		//g_TextEdit.InitMessage() ;
	}
}

VOID	CRYLVaultPasswordDlg::CancelButtonEvent() 
{
	m_pCurrentPasswordEdit->SetText( "" ) ;
	m_pChangePasswordEdit->SetText( "" ) ;

	if ( CRYLGameData::Instance()->m_csStatus.m_cUsedDeposit )
	{
		CRYLGameScene*   	 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLVaultDlg*     pVaultDlg = ( CRYLVaultDlg* )pScene->GetVaultDlg() ;
		pVaultDlg->ShowNormal() ;
	} 

	SetVisible( FALSE ) ;
}

VOID	CRYLVaultPasswordDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pVaultPasswordDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLVAULTPASSWORDDLG::OK_BUTTON :
		g_pVaultPasswordDlg->OKButtonEvent() ;
		break ;
	case RYLVAULTPASSWORDDLG::CANCEL_BUTTON :	
		g_pVaultPasswordDlg->CancelButtonEvent() ;
		break ;
	}
}


VOID	CRYLVaultPasswordDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pVaultPasswordDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLVAULTPASSWORDDLG::CURRENTPASSWORD_EDIT :
			if (key == 13)
				g_pVaultPasswordDlg->m_pChangePasswordEdit->SetFocus() ;
			break;
		case RYLVAULTPASSWORDDLG::CHANGEPASSWORD_EDIT : 
			if (key == 13)
				g_pVaultPasswordDlg->OKButtonEvent() ;
			break;
	}
}

//------------------------------------------------------------------------------------
// 인증 변경
CRYLVaultCertificationdDlg*	g_pVaultCertificationDlg = NULL ;

CRYLVaultCertificationdDlg::CRYLVaultCertificationdDlg() : CRYLDialog()
{
	m_pCertificationLabel = NULL ;			// 인증 이름
	m_pPasswordLabel	  = NULL ;			// 비밀번호를 입력해주세요.
	m_pOkButton			  = NULL ;			// 확인 버튼
	m_pCancelButton       = NULL ;			// 취소 버튼

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 150, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 28 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 91, 62, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 20, 252, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 163 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pVaultCertificationDlg = this ;	
	InitCtrl() ;
}

CRYLVaultCertificationdDlg::~CRYLVaultCertificationdDlg ()
{
	FinalCtrl() ;
}

VOID CRYLVaultCertificationdDlg::InitCtrl ()
{
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 인증 이름
	m_pCertificationLabel = new CRYLLabel( this ) ;
	m_pCertificationLabel->SetLeft( 9 ) ;
	m_pCertificationLabel->SetTop( 18 ) ;
	m_pCertificationLabel->SetFontColor (0xffffffff);
	m_pCertificationLabel->SetAutoSize( TRUE ) ;
	m_pCertificationLabel->SetCaption( CRYLStringTable::m_strString[ 2208 ] ) ;
	AttachGMUIControl( m_pCertificationLabel ) ;

	// 비밀번호를 입력해주세요.
	m_pPasswordLabel = new CRYLLabel( this ) ;
	m_pPasswordLabel->SetLeft( 89) ;
	m_pPasswordLabel->SetTop( 48 ) ;
	m_pPasswordLabel->SetFontColor (0xffffffff);
	m_pPasswordLabel->SetAutoSize( TRUE ) ;
	m_pPasswordLabel->SetCaption( CRYLStringTable::m_strString[ 2209 ] ) ;
	AttachGMUIControl( m_pPasswordLabel ) ;

	// 확인 버튼
	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 33 ) ;
	m_pOkButton->SetTop( 121 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLVAULTCERTIFICATIONDDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;
		
	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 143 ) ;
	m_pCancelButton->SetTop( 121 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLVAULTCERTIFICATIONDDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;	

	// 현제 비밀번호
	m_pPasswordEdit = new CRYLEdit( this ) ;
	m_pPasswordEdit->SetLeft( 93 ) ;
	m_pPasswordEdit->SetTop( 79 ) ;
	m_pPasswordEdit->SetWidth( 71 ) ;
	m_pPasswordEdit->SetHeight( 15 ) ;
	m_pPasswordEdit->SetColor( 0xff000000) ;
	m_pPasswordEdit->SetFontColor( 0xffffffff ) ;
	m_pPasswordEdit->SetPassWord( '*' ) ;
	m_pPasswordEdit->SetInstance( RYLVAULTCERTIFICATIONDDLG::PASSWORD_EDIT ) ;
	m_pPasswordEdit->SetMaxLength( 4 ) ;
	m_pPasswordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pPasswordEdit ) ;
}

VOID CRYLVaultCertificationdDlg::FinalCtrl ()
{
	GM_DELETE( m_pPasswordLabel ) ;		
	GM_DELETE( m_pCertificationLabel ) ;		// 인증 이름
	GM_DELETE( m_pPasswordLabel ) ;				// 비밀번호를 입력해주세요.
	GM_DELETE( m_pOkButton ) ;					// 확인 버튼
	GM_DELETE( m_pCancelButton ) ;				// 취소 버튼
	GM_DELETE( m_pPasswordEdit ) ;				// 현제 비밀번호
}

VOID CRYLVaultCertificationdDlg::Show( CGMUIObject* Sender )
{
	m_pPasswordEdit->SetFocus() ;
}

VOID CRYLVaultCertificationdDlg::Hide( CGMUIObject* Sender )
{
    if ( CRYLChattingDlg::Instance() )
    {
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
    }
}

VOID CRYLVaultCertificationdDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLVaultCertificationdDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLVaultCertificationdDlg::OKButtonEvent() 
{
	// 확인
	BOOL	bPass = TRUE;
	DWORD	i ;
	CHAR*	pszCurPass = m_pPasswordEdit->GetText() ;
	if ( strlen( pszCurPass ) < 4 ) 
		bPass = FALSE ;

	for ( i = 0 ; i < 4 ; i ++ )
	{
		if ( !( ( 48 <= pszCurPass[ i ] && pszCurPass[ i ] <= 57 ) || 
			    ( 65 <= pszCurPass[ i ] && pszCurPass[ i ] <= 90 ) || 
				( 97 <= pszCurPass[ i ] && pszCurPass[ i ] <= 122 ) ) )  
				bPass = FALSE ;
	}

	if(bPass)
	{
		SendPacket::StoreLogin(g_GameSession, pszCurPass, m_cSavePass);
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 617 ] ) ;
		m_pPasswordEdit->SetText( "" ) ;
	}
}

VOID	CRYLVaultCertificationdDlg::CancelButtonEvent() 
{
	m_pPasswordEdit->SetText( "" ) ;
	SetVisible( FALSE ) ;
}

VOID	CRYLVaultCertificationdDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pVaultCertificationDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLVAULTCERTIFICATIONDDLG::OK_BUTTON :
		g_pVaultCertificationDlg->OKButtonEvent() ;
		break ;
	case RYLVAULTCERTIFICATIONDDLG::CANCEL_BUTTON :	
		g_pVaultCertificationDlg->CancelButtonEvent() ;
		break ;
	}
}


VOID	CRYLVaultCertificationdDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pVaultCertificationDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLVAULTCERTIFICATIONDDLG::PASSWORD_EDIT :
			if (key == 13)
				g_pVaultCertificationDlg->OKButtonEvent() ;
			break;
	}
}