#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLCharacterDeleteDlg.h"
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
#include "RYLCharacterSelectScene.h"
#include "RYLSceneManager.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLStringTable.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendItem.h>

//-- 2004. 1. 29. Zergra From.--//
#include "RYLChattingDlg.h"
//-- Zergra To. --//
//#include <RenderOption.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
CRYLCharacterDeleteDlg*	g_pCharacterDeleteDlg = NULL ;

CRYLCharacterDeleteDlg::CRYLCharacterDeleteDlg() : CRYLDialog()
{
	m_pCertificationLabel = NULL ;			// 인증 이름
	m_pPasswordLabel1	  = NULL ;			// 비밀번호를 입력해주세요.
	m_pPasswordLabel2	  = NULL ;			// 비밀번호를 입력해주세요.
	m_pPasswordLabel3	  = NULL ;			// 비밀번호를 입력해주세요.
	m_pPasswordLabel4	  = NULL ;			// 비밀번호를 입력해주세요.
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 160, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 30 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 98, 98, 75, 19, 0xff959595, 0xc0000000,  1 ) ;
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 20, 252, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 175 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pCharacterDeleteDlg = this ;	
	InitCtrl() ;
}

CRYLCharacterDeleteDlg::~CRYLCharacterDeleteDlg ()
{
	FinalCtrl() ;
}

VOID CRYLCharacterDeleteDlg::InitCtrl ()
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
	m_pPasswordLabel1 = new CRYLLabel( this ) ;
	m_pPasswordLabel1->SetLeft( 30 ) ;
	m_pPasswordLabel1->SetTop( 48 ) ;
	m_pPasswordLabel1->SetFontColor (0xffffffff);
	m_pPasswordLabel1->SetAutoSize( TRUE ) ;
	m_pPasswordLabel1->SetCaption( CRYLStringTable::m_strString[ 2209 ] ) ;
	AttachGMUIControl( m_pPasswordLabel1 ) ;

	// 비밀번호를 입력해주세요.
	m_pPasswordLabel2 = new CRYLLabel( this ) ;
	m_pPasswordLabel2->SetLeft( 30 ) ;
	m_pPasswordLabel2->SetTop( 63 ) ;
	m_pPasswordLabel2->SetFontColor (0xffffffff);
	m_pPasswordLabel2->SetAutoSize( TRUE ) ;
	m_pPasswordLabel2->SetCaption( CRYLStringTable::m_strString[ 2209 ] ) ;
	AttachGMUIControl( m_pPasswordLabel2 ) ;

	// 비밀번호를 입력해주세요.
	m_pPasswordLabel3 = new CRYLLabel( this ) ;
	m_pPasswordLabel3->SetLeft( 30 ) ;
	m_pPasswordLabel3->SetTop( 78 ) ;
	m_pPasswordLabel3->SetFontColor (0xffffffff);
	m_pPasswordLabel3->SetAutoSize( TRUE ) ;
	m_pPasswordLabel3->SetCaption( CRYLStringTable::m_strString[ 2209 ] ) ;
	AttachGMUIControl( m_pPasswordLabel3 ) ;

	// 비밀번호를 입력해주세요.
	m_pPasswordLabel4 = new CRYLLabel( this ) ;
	m_pPasswordLabel4->SetLeft( 30 ) ;
	m_pPasswordLabel4->SetTop( 93 ) ;
	m_pPasswordLabel4->SetFontColor (0xffffffff);
	m_pPasswordLabel4->SetAutoSize( TRUE ) ;
	m_pPasswordLabel4->SetCaption( CRYLStringTable::m_strString[ 2209 ] ) ;
	AttachGMUIControl( m_pPasswordLabel4 ) ;

	// 확인 버튼
	m_pOkButton = new CRYLButton( this ) ;
	m_pOkButton->SetLeft( 48 ) ;
	m_pOkButton->SetTop( 137 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLCHARACTERDELETEDLG::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOkButton ) ;
		
	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 132 ) ;
	m_pCancelButton->SetTop( 137 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCHARACTERDELETEDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;	

	// 현재 비밀번호
	m_pPasswordEdit = new CRYLEdit( this ) ;
	m_pPasswordEdit->SetLeft( 100 ) ;
	m_pPasswordEdit->SetTop( 115 ) ;
	m_pPasswordEdit->SetWidth( 71 ) ;
	m_pPasswordEdit->SetHeight( 15 ) ;
	m_pPasswordEdit->SetColor( 0xff000000) ;
	m_pPasswordEdit->SetFontColor( 0xffffffff ) ;
	m_pPasswordEdit->SetPassWord( '*' ) ;
	m_pPasswordEdit->SetInstance( RYLCHARACTERDELETEDLG::PASSWORD_EDIT ) ;
	m_pPasswordEdit->SetMaxLength( 4 ) ;
	m_pPasswordEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pPasswordEdit ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	if(pGame->m_bUsedDeposit)
		InitCaption( 0 );
	else
		InitCaption( 1 );
}

VOID CRYLCharacterDeleteDlg::FinalCtrl ()
{
	GM_DELETE( m_pPasswordLabel1 ) ;		
	GM_DELETE( m_pPasswordLabel2 ) ;		
	GM_DELETE( m_pPasswordLabel3 ) ;		
	GM_DELETE( m_pPasswordLabel4 ) ;		
	GM_DELETE( m_pCertificationLabel ) ;		// 인증 이름
	GM_DELETE( m_pOkButton ) ;					// 확인 버튼
	GM_DELETE( m_pCancelButton ) ;				// 취소 버튼
	GM_DELETE( m_pPasswordEdit ) ;				// 현제 비밀번호
}

VOID CRYLCharacterDeleteDlg::Show( CGMUIObject* Sender )
{
	m_pPasswordEdit->SetText( "" ) ;
	m_pPasswordEdit->SetFocus() ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	if(pGame->m_bUsedDeposit)
		InitCaption( 0 );
	else
		InitCaption( 1 );
}

VOID CRYLCharacterDeleteDlg::Hide( CGMUIObject* Sender )
{
	m_pPasswordEdit->SetText( "" ) ;

    if ( CRYLChattingDlg::Instance() )
    {
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE ) ;
    }
}

VOID CRYLCharacterDeleteDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCharacterDeleteDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLCharacterDeleteDlg::OKButtonEvent() 
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
		CRYLGameData*	 pGame		= CRYLGameData::Instance() ;
		CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;

		pGame->m_bDeleteChar = TRUE;
		pNetwork->SendDeleteChar( pGame->m_dwSelectChr, pGame->m_scLoginChar[ pGame->m_dwSelectChr ].CID, pszCurPass ) ;

		CRYLMessageBox *lpMessage ;
		lpMessage = new CRYLMessageBox ;
		lpMessage->Create( CRYLStringTable::m_strString[ 120 ] ) ;
		lpMessage->SetTimer( 100000 ) ;
		m_pPasswordEdit->SetText( "" ) ;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 617 ] ) ;
		m_pPasswordEdit->SetText( "" ) ;
	}
}

VOID	CRYLCharacterDeleteDlg::CancelButtonEvent() 
{
	m_pPasswordEdit->SetText( "" ) ;
	SetVisible( FALSE ) ;
}

VOID	CRYLCharacterDeleteDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pCharacterDeleteDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCHARACTERDELETEDLG::OK_BUTTON :
			g_pCharacterDeleteDlg->OKButtonEvent() ;
			break ;
		case RYLCHARACTERDELETEDLG::CANCEL_BUTTON :	
			g_pCharacterDeleteDlg->CancelButtonEvent() ;
			break ;
	}
}


VOID	CRYLCharacterDeleteDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pCharacterDeleteDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCHARACTERDELETEDLG::PASSWORD_EDIT :
			if (key == 13)
				g_pCharacterDeleteDlg->OKButtonEvent() ;
			break;
	}
}

VOID	CRYLCharacterDeleteDlg::InitCaption( unsigned long dwCaption )
{
	switch(dwCaption)
	{
		case 0:
			m_pCertificationLabel->SetCaption( CRYLStringTable::m_strString[ 2208 ] );
			m_pPasswordLabel1->SetCaption(CRYLStringTable::m_strString[ 230 ]);
			m_pPasswordLabel1->SetLeft(30);
			m_pPasswordLabel2->SetCaption(CRYLStringTable::m_strString[ 231 ]);
			m_pPasswordLabel2->SetLeft(30);
			m_pPasswordLabel3->SetCaption("");
			m_pPasswordLabel3->SetLeft(30);
			m_pPasswordLabel4->SetCaption("");
			m_pPasswordLabel4->SetLeft(30);
			break;

		case 1:
			m_pCertificationLabel->SetCaption( CRYLStringTable::m_strString[ 2208 ] );
			m_pPasswordLabel1->SetCaption(CRYLStringTable::m_strString[ 230 ]);
			m_pPasswordLabel1->SetLeft(12);
			m_pPasswordLabel2->SetCaption(CRYLStringTable::m_strString[ 232 ]);
			m_pPasswordLabel2->SetLeft(12);
			m_pPasswordLabel3->SetCaption(CRYLStringTable::m_strString[ 233 ]);
			m_pPasswordLabel3->SetLeft(12);
			m_pPasswordLabel4->SetCaption(CRYLStringTable::m_strString[ 234 ]);
			m_pPasswordLabel4->SetLeft(12);
			break;

		case 2:
			m_pCertificationLabel->SetCaption( CRYLStringTable::m_strString[ 235 ] );
			m_pPasswordLabel1->SetCaption(CRYLStringTable::m_strString[ 475 ]);
			m_pPasswordLabel1->SetLeft(30);
			m_pPasswordLabel2->SetCaption(CRYLStringTable::m_strString[ 236 ]);
			m_pPasswordLabel2->SetLeft(30);
			m_pPasswordLabel3->SetCaption("");
			m_pPasswordLabel3->SetLeft(30);
			m_pPasswordLabel4->SetCaption("");
			m_pPasswordLabel4->SetLeft(30);
			break;
	}
}