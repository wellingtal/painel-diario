#include <winsock2.h>
#include "RYLGuildMarkDlg.h"
#include "RYLButton.h" 
#include "RYLListBox.h"
#include "RYLSpriteEx.h"
#include "RYLLabel.h"
#include "RYLSprite.h"
#include "..\\RYLBitmap.h"
#include "Texture.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "WinInput.h"

#include "RYLMessageBox.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLGuildMarkDlg*	g_pGuildMarkDlg = NULL ;


__MARKINFOEX::__MARKINFOEX()
{
	szFileName[ 0 ] = '\0' ;
	pSprite = NULL ;
	pTex	= NULL ;
	pBMP	= NULL ;
}
__MARKINFOEX::~__MARKINFOEX()
{
	szFileName[ 0 ] = '\0' ;

	GM_DELETE( pBMP ) ;
	GM_DELETE( pSprite ) ;
	GM_DELETE( pTex ) ;
}

CRYLGuildMarkDlg::CRYLGuildMarkDlg() : CRYLDialog()
{
	m_SpriteIndex = -1 ;
	m_iCurrentMarkSize = 0 ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 296, &pImageDIB ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->MakeFrameRect( 2, 0, 252, 21, 0xc0000000, 0xc0000000,  1 ) ;
	int i = 0;
	for (  ; i < 53 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 21, 252, 1, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 24, 42, 144, 180, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 181, 42, 49, 32, 0xffB99E6B, 0xc0000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 265 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pGuildMarkDlg = this ;

	m_dwGID = 0 ;
}

CRYLGuildMarkDlg::~CRYLGuildMarkDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLGuildMarkDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	// 길드마크 변경
	m_pMarkChangeLabel = new CRYLLabel( this ) ;
	m_pMarkChangeLabel->SetLeft( 9 ) ;
	m_pMarkChangeLabel->SetTop( 18 ) ;
	m_pMarkChangeLabel->SetFontColor (0xffffffff);
	m_pMarkChangeLabel->SetAutoSize( TRUE ) ;
	m_pMarkChangeLabel->SetCaption( CRYLStringTable::m_strString[ 2079 ] ) ;
	AttachGMUIControl( m_pMarkChangeLabel ) ;

	// 길드 마크 스프라이트
	m_pGuildMarkSprite = new CRYLSpriteEx( this ) ;
	m_pGuildMarkSprite->SetLeft( 197 ) ;
	m_pGuildMarkSprite->SetTop( 65 ) ;
	m_pGuildMarkSprite->SetWidth( 18 ) ;
	m_pGuildMarkSprite->SetHeight( 12 ) ;
	AttachGMUIControl( m_pGuildMarkSprite ) ;

	// 길드 마크 파일이름
	m_pMarkListBox = new CRYLListBox( this, TRUE ) ;
	m_pMarkListBox->SetLeft( 24 ) ;
	m_pMarkListBox->SetTop( 55 ) ;
	m_pMarkListBox->SetWidth( 144 ) ;
	m_pMarkListBox->SetHeight( 180 ) ;
	
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pMarkListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pMarkListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pMarkListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pMarkListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_pMarkListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pMarkListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pMarkListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pMarkListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pMarkListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pMarkListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pMarkListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pMarkListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
	m_pMarkListBox->OnClickEvent = DlgClick ;
	m_pMarkListBox->SetInstance( RYLGUILDMARKDLG::GUILDMARK_LISTBOX ) ;
 	m_pMarkListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pMarkListBox ) ;

	
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	// 길드 마크 변경 버튼
	m_pChangeButton = new CRYLButton( this ) ;
	m_pChangeButton->SetLeft( 114 ) ;
	m_pChangeButton->SetTop( 270    ) ;
	m_pChangeButton->SetCaption( CRYLStringTable::m_strString[ 125 ] ) ;
	m_pChangeButton->SetFontColor( 0xffffffff) ;
	m_pChangeButton->SetColor( 0xffffffff) ;
	m_pChangeButton->OnClickEvent = DlgClick ;
	m_pChangeButton->SetInstance( RYLGUILDMARKDLG::CHANGEMARK_BUTTON ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pChangeButton ) ;

	// 취소 버튼	
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 182 ) ;
	m_pCancelButton->SetTop( 270    ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 126 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLGUILDMARKDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;

}

VOID	CRYLGuildMarkDlg::FinalCtrl ()
{
	ClearVector()  ;

	GM_DELETE( m_pMarkChangeLabel ) ;		// 길드마크 변경
	GM_DELETE( m_pGuildMarkSprite ) ;		// 길드 마크 스프라이트
	GM_DELETE( m_pMarkListBox ) ;			// 길드 마크 파일이름
	GM_DELETE( m_pChangeButton ) ;			// 길드 마크 변경 버튼
	GM_DELETE( m_pCancelButton ) ;			// 취소 버튼
}

VOID	CRYLGuildMarkDlg::Show( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->AttachImageTexture( NULL ) ;
	m_pMarkListBox->ClearItem() ;
	ClearVector() ;
	GetSystemDirectoryBitmap() ;

	if( m_pMarkListBox->Count() > 0 )
		m_pMarkListBox->SelectItemIndex( 0 );

}
	
VOID	CRYLGuildMarkDlg::Hide( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->AttachImageTexture( NULL ) ;
	ClearVector() ;
}

VOID	CRYLGuildMarkDlg::ClearVector() 
{
	if ( !m_vecMARK.empty() )
	{
		std::vector< LPMARKINFOEX >::iterator		iter = m_vecMARK.begin() ;

		while ( iter != m_vecMARK.end() )
		{
			GM_DELETE( (*iter) ) ;
			++ iter ;
		}
	}

	m_vecMARK.clear() ;
}

VOID	CRYLGuildMarkDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLGuildMarkDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLGuildMarkDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pGuildMarkDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLGUILDMARKDLG::CHANGEMARK_BUTTON :
			g_pGuildMarkDlg->ChangeGuildMarkButton() ;
			break ;

		case RYLGUILDMARKDLG::CANCEL_BUTTON	:
			g_pGuildMarkDlg->SetVisible( FALSE ) ;
			break ;

		case RYLGUILDMARKDLG::GUILDMARK_LISTBOX	:
			g_pGuildMarkDlg->SelectGuildMark() ;
			break ;
	}
}

VOID	CRYLGuildMarkDlg::ChangeGuildMarkButton() 
{
	if ( !m_vecMARK.empty() )
	{	
		std::vector< LPMARKINFOEX >::iterator		iterMark ;
		INT iIndex = m_pMarkListBox->GetItemIndex() ;
		if( iIndex == -1 )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 2791 ]);
			return;
		}
		iterMark = m_vecMARK.begin() + iIndex;
		SendPacket::CharGuildMark( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwGID, 
			( char* )( *iterMark )->pBMP->GetByteBits() ) ;
		SetVisible( FALSE ) ;
	}
}

VOID	CRYLGuildMarkDlg::SelectGuildMark()
{
	INT iIndex = m_pMarkListBox->GetItemIndex() ;

	if ( iIndex == -1 )
		return ;
	
	if ( m_vecMARK[ iIndex ]->pSprite )
		m_pGuildMarkSprite->AttachImageTexture( m_vecMARK[ iIndex ]->pSprite ) ;
}

VOID	CRYLGuildMarkDlg::GetSystemDirectoryBitmap()
{
	HANDLE			hSrch ;
	WIN32_FIND_DATA	wfd ;
	BOOL			bResult = TRUE ;
	CHAR			szFileName[ MAX_PATH ] ;
	CHAR			szDirectory[ MAX_PATH ] ;

	CTexture::SetPath(ROOTPATH);

	strcpy( szDirectory, ROOTPATH ) ;

	strcat( szDirectory, "\\GuildBMP\\*.BMP" ) ;
	hSrch = FindFirstFile( szDirectory, &wfd ) ;

	if ( hSrch == INVALID_HANDLE_VALUE )
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 1130 ] ) ;
		SetVisible( FALSE ) ;
		return ;
	}

	strcpy( szDirectory, ROOTPATH ) ;
	
	while( bResult )
	{
		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			wsprintf( szFileName, "[ %s ]", wfd.cFileName ) ;
			strcat( szDirectory, szFileName ) ;
		}
		else 
		{
			wsprintf( szFileName, "%s\\GuildBMP\\%s", szDirectory, wfd.cFileName ) ;

			LPMARKINFOEX	pMarkInfo ;
			BYTE*			pTempBuff = NULL ;

			pMarkInfo			= new MARKINFOEX ;
			pMarkInfo->pBMP		= new CRYLBitmap ;
			pMarkInfo->pSprite	= new CRYLSprite ;
			pMarkInfo->pTex		= new CTexture ;

			strcpy( pMarkInfo->szFileName, wfd.cFileName ) ;

			if ( pMarkInfo->pBMP->Load( szFileName, &pTempBuff ) == 0 )
			{
				char strText[MAX_PATH];
				sprintf(strText, CRYLStringTable::m_strString[ 1131 ], wfd.cFileName);

				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( strText ) ;	

				delete pMarkInfo ;
				pMarkInfo = NULL ;
			}
			else
			{
				m_pMarkListBox->AddItem( pMarkInfo->szFileName, 0xffffffff, 0xffffffff, 0xffffffff ) ;
				pMarkInfo->pTex->CreateEmptyTexture( 32, 16, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED ) ;
				pMarkInfo->pTex->SetBitTexture16( 18, 12, ( WORD* )pTempBuff ) ;
				pMarkInfo->pSprite->Create( 197, 65, 0, 0, 18, 12, pMarkInfo->pTex ) ;
				
				m_vecMARK.push_back( pMarkInfo ) ;
			}
		}

		bResult = FindNextFile( hSrch, &wfd ) ;
	}

	FindClose( hSrch ) ;

	m_iCurrentMarkSize = m_vecMARK.size() ;
}
