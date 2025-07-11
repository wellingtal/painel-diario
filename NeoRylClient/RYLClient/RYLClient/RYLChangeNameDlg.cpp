#include <winsock2.h>
#include <WinInput.h>

#include "RYLClientMain.h"
#include "RYLChangeNameDlg.h"
#include "GMUIScreen.h"
#include "RYLButton.h"
#include "RYLEdit.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "BaseDataDefine.h"

#include "..\\StringFilter.h"

#include "RYLChattingDlg.h"
#include "GUITextEdit.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include <Network/ClientSocket/Send/SendEtc.h>

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include <RenderOption.h>
#include "GMMemory.h"

CRYLChangeNameDlg *g_pChangeNameDlg = NULL;

CRYLChangeNameDlg::CRYLChangeNameDlg() : CRYLDialog()
{
	m_pChangeBtn			= NULL ;
	m_pCancelBtn			= NULL ;
	m_pNewNameEdit			= NULL ;

	m_pTitleLabel			= NULL ;
	m_pPrevNameTitleLabel	= NULL ;
	m_pPrevNameLabel		= NULL ;
	m_pNewNameLabel			= NULL ;
//	m_pDescLabel			= NULL ;

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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 197, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 38 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 41, 256, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xFFAD9369, 0x00000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 64, 51, 129, 19, 0xFF959595, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 64, 102, 129, 19, 0xFF959595, 0x80000000,  1 ) ;		
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 212 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pChangeNameDlg = this ;	
	InitCtrl() ;
}

CRYLChangeNameDlg::~CRYLChangeNameDlg()
{
	FinalCtrl();
}

VOID	CRYLChangeNameDlg::InitCtrl()
{
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pChangeBtn = new CRYLButton( this ) ;
	m_pChangeBtn->SetLeft( 33 ) ;
	m_pChangeBtn->SetTop( 177 ) ;
	m_pChangeBtn->SetCaption( CRYLStringTable::m_strString[ 125 ] ) ;
	m_pChangeBtn->SetFontColor( 0xffffffff) ;
	m_pChangeBtn->SetColor( 0xffffffff) ;
	m_pChangeBtn->OnClickEvent = DlgClick ;
	m_pChangeBtn->SetInstance( RYLCHANGENAMEDLG::CHANGE_BUTTON ) ;
	m_pChangeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pChangeBtn ) ;

	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 143 ) ;
	m_pCancelBtn->SetTop( 177 ) ;
	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 126 ]  ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLCHANGENAMEDLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;

	m_pNewNameEdit = new CRYLEdit( this ) ;
	m_pNewNameEdit->SetLeft( 66 ) ;
	m_pNewNameEdit->SetTop( 119 ) ;
	m_pNewNameEdit->SetWidth( 125 ) ;
	m_pNewNameEdit->SetHeight( 15 ) ;
	m_pNewNameEdit->SetColor( 0xff000000) ;
	m_pNewNameEdit->SetFontColor( 0xffffffff ) ;
	m_pNewNameEdit->SetInstance( RYLCHANGENAMEDLG::NEWNAME_EDITBOX ) ;
	m_pNewNameEdit->SetMaxLength( 15 ) ; 
	m_pNewNameEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pNewNameEdit ) ;

	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 8 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 130 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;

	m_pPrevNameTitleLabel = new CRYLLabel( this ) ;
	m_pPrevNameTitleLabel->SetLeft( 79 ) ;
	m_pPrevNameTitleLabel->SetTop( 49 ) ;
	m_pPrevNameTitleLabel->SetFontColor (0xffffffff);
	m_pPrevNameTitleLabel->SetAutoSize( TRUE ) ;
	m_pPrevNameTitleLabel->SetCaption( CRYLStringTable::m_strString[ 131 ] ) ;
	AttachGMUIControl( m_pPrevNameTitleLabel ) ;

	m_pPrevNameLabel = new CRYLLabel( this ) ;
	m_pPrevNameLabel->SetLeft( 66 ) ;
	m_pPrevNameLabel->SetTop( 68 ) ;
	m_pPrevNameLabel->SetFontColor (0xffffffff);
	m_pPrevNameLabel->SetAutoSize( TRUE ) ;
	m_pPrevNameLabel->SetCaption( CRYLStringTable::m_strString[ 132 ] ) ;
	AttachGMUIControl( m_pPrevNameLabel ) ;

	m_pNewNameLabel = new CRYLLabel( this ) ;
	m_pNewNameLabel->SetLeft( 79 ) ;
	m_pNewNameLabel->SetTop( 100 ) ;
	m_pNewNameLabel->SetFontColor (0xffffffff);
	m_pNewNameLabel->SetAutoSize( TRUE ) ;
	m_pNewNameLabel->SetCaption( CRYLStringTable::m_strString[ 133 ] ) ;
	AttachGMUIControl( m_pNewNameLabel ) ;
/*
	// 기존 134번은 캐릭터 이름변경은 1회만 가능합니다. 라는 메시지 로직이 바뀌면서 빠진다.
	m_pDescLabel = new CRYLLabel( this ) ;
	m_pDescLabel->SetLeft( 26 ) ;
	m_pDescLabel->SetTop( 154 ) ;
	m_pDescLabel->SetFontColor (0xffffffff);
	m_pDescLabel->SetAutoSize( TRUE ) ;
	m_pDescLabel->SetCaption( CRYLStringTable::m_strString[ 134 ] ) ;
	AttachGMUIControl( m_pDescLabel ) ;
*/
}

VOID	CRYLChangeNameDlg::FinalCtrl ()
{
	GM_DELETE( m_pChangeBtn );
	GM_DELETE( m_pCancelBtn );
	GM_DELETE( m_pNewNameEdit );
	GM_DELETE( m_pTitleLabel );
	GM_DELETE( m_pPrevNameTitleLabel );
	GM_DELETE( m_pPrevNameLabel );
	GM_DELETE( m_pNewNameLabel );
//	GM_DELETE( m_pDescLabel );
}

VOID	CRYLChangeNameDlg::Show( CGMUIObject* Sender )
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	m_pPrevNameLabel->SetCaption( pGame->m_csStatus.m_Info.Name ) ;
	m_pPrevNameLabel->SetLeft( 66 + ( 125 - m_pPrevNameLabel->GetWidth() ) / 2 );
	m_pPrevNameLabel->SetTop( 68 + ( 15 - m_pPrevNameLabel->GetHeight() ) / 2 );
	m_pNewNameEdit->SetText("");
	m_pNewNameEdit->SetFocus();
}

VOID	CRYLChangeNameDlg::Hide( CGMUIObject* Sender )
{
	m_pNewNameEdit->SetText( "" );
	m_pNewNameEdit->SetFocusState( FALSE );

    if ( CRYLChattingDlg::Instance() )
    {
		CRYLChattingDlg::Instance()->SetFocusChattingEdit();
    }
}

VOID	CRYLChangeNameDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLChangeNameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
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

VOID CRYLChangeNameDlg::ChangeButtonDown()
{
	int		len = strlen( m_pNewNameEdit->GetText() ), i ;
	LPSTR	pszName = m_pNewNameEdit->GetText() ;

	if ( len == 0 )// 캐릭터 이름이 없습니다.
	{
		CRYLMessageBox *lpMessage;
		lpMessage = new CRYLMessageBox;
		lpMessage->Create( CRYLStringTable::m_strString[110]);
		lpMessage->SetTimer(1000);
		m_pNewNameEdit->SetText( "" );
		return ;
	} 
    else if( len < 4 )// 캐릭터 이름이 짧습니다.
	{
		CRYLMessageBox *lpMessage;
		lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[111]);
		lpMessage->SetTimer(1000);
		m_pNewNameEdit->SetText( "" );
		return ;
	}

	for ( i = 0; i < len; i ++ )
	{
		if ( pszName[ i ] == ' ' )// 빈칸이 있습니다.
		{
			CRYLMessageBox *lpMessage;
			lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[113]);
			lpMessage->SetTimer(1000);
			m_pNewNameEdit->SetText( "" );
			return ;
		}

		if ( pszName[ i ] == 34 || pszName[ i ] == 39 )// 이름이 따옴표가 있습니다.
		{
			CRYLMessageBox *lpMessage;
			lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[114]);
			lpMessage->SetTimer(1000);
			m_pNewNameEdit->SetText( "" );
			return ;
		}
	}
/*
	if(g_ClientMain.m_wAdminMode != ADMIN_L3) 
	{
		// 이름 제제
		if (!strcmp(pszName, "edith"))
		{
			CRYLMessageBox* lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[115]);
			lpMessage->SetTimer(1000);
			m_pNewNameEdit->SetText( "" );
			return ;
		}
	}
*/
	if ( false == g_StringFilter.Filter( pszName ) )
	{
		CRYLMessageBox* lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[2784]);
		lpMessage->SetTimer(1000);
		m_pNewNameEdit->SetText( "" );
		return ;
	}

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA ) // 포인트가 남았다.
	{
		if ( !CheckCharCreateName( m_pNewNameEdit->GetText() ) )
		{
			CRYLMessageBox* lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[116]);
			lpMessage->SetTimer(1000);
			m_pNewNameEdit->SetText( "" );
			return ;
		}
	}


	// edith 2008.11.11 계명허가서 추가작업
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(Item::EtcItemID::RENAME_WARRANT);		
	if(!lpItem)
	{
		CRYLMessageBox* lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[134]);
		lpMessage->SetTimer(2000);
		return;
	}

	pGame->m_lpRenameWarrantItem = lpItem;

	if(!SendPacket::CharNameChange(g_GameSession, CRYLNetworkData::Instance()->m_dwUserID, CRYLNetworkData::Instance()->m_dwMyChrID, pszName, &lpItem->m_lpItemBase->GetPos()))
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[3340]);
		m_pNewNameEdit->SetText( "" );
	}
}

VOID CRYLChangeNameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pChangeNameDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCHANGENAMEDLG::CHANGE_BUTTON : 
			g_pChangeNameDlg->ChangeButtonDown() ;			
			break;

		case RYLCHANGENAMEDLG::CANCEL_BUTTON :
			g_pChangeNameDlg->SetVisible( FALSE );
			break ;
	}
}

VOID CRYLChangeNameDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pChangeNameDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCHANGENAMEDLG::NEWNAME_EDITBOX :
			if (key == 13)
				g_pChangeNameDlg->ChangeButtonDown() ;
			break;
	}
}