#include "RYLClientMain.h"
#include "RYLSocietyFrameDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLPartyPanel.h"
#include "RYLFriendPanel.h"
#include "RYLRefusalPanel.h"
#include "RYLPartyFindPanel.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "RYLStringTable.h"
#include <RenderOption.h>
#include "RYLChattingDlg.h"
#include "RYLCommunityData.h"
#include "RYLEdit.h"
#include "GMMemory.h"

CRYLSocietyFrameDlg*	g_pSocietyDlg = NULL ;

CRYLSocietyFrameDlg::CRYLSocietyFrameDlg() 
{
	m_pTabButton	= NULL ;
	m_pPartyPanel	= NULL ;
	m_pFriendPanel	= NULL ;
	m_pRefusalPanel = NULL ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 206, 0, 48, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 206, 20, 48, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 412 ) ;

	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;
	
	g_pSocietyDlg = this ;
	InitCtrl() ;
}

CRYLSocietyFrameDlg::~CRYLSocietyFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLSocietyFrameDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 22 ) ;
	m_pTabButton->SetInstance( RYLSOCIETYDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1962 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1963 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1964 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1965 ], 5 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1821 ], 1821, TOOLTIP_TAB ) ;

	m_pPartyPanel = new CRYLPartyPanel( this ) ;
	m_pPartyPanel->SetLeft( 0 ) ;
	m_pPartyPanel->SetTop( 36 ) ;
	m_pPartyPanel->SetWidth( 256 ) ;
	m_pPartyPanel->SetHeight( 395 ) ;
	( ( CRYLPartyPanel* )m_pPartyPanel )->InitCtrl() ;
	m_pPartyPanel->OnClickEvent = DlgClick ;
	m_pPartyPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pPartyPanel ) ;

	m_pFriendPanel = new CRYLFriendPanel( this ) ;
	m_pFriendPanel->SetLeft( 0 ) ;
	m_pFriendPanel->SetTop( 36 ) ;
	m_pFriendPanel->SetWidth( 256 ) ;
	m_pFriendPanel->SetHeight( 395 ) ;
	( ( CRYLFriendPanel* )m_pFriendPanel )->InitCtrl() ;
	m_pFriendPanel->OnClickEvent = DlgClick ;
	m_pFriendPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pFriendPanel ) ;

	m_pRefusalPanel = new CRYLRefusalPanel( this ) ;
	m_pRefusalPanel->SetLeft( 0 ) ;
	m_pRefusalPanel->SetTop( 36 ) ;
	m_pRefusalPanel->SetWidth( 256 ) ;
	m_pRefusalPanel->SetHeight( 395 ) ;
	( ( CRYLRefusalPanel* )m_pRefusalPanel )->InitCtrl() ;
	m_pRefusalPanel->OnClickEvent = DlgClick ;
	m_pRefusalPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pRefusalPanel ) ;

	
	m_pPartyFindPanel = new CRYLPartyFindPanel( this ) ;
	m_pPartyFindPanel->SetLeft( 0 ) ;
	m_pPartyFindPanel->SetTop( 36 ) ;
	m_pPartyFindPanel->SetWidth( 255 ) ;
	m_pPartyFindPanel->SetHeight( 395 ) ;
	( ( CRYLPartyFindPanel* )m_pPartyFindPanel )->InitCtrl() ;
	m_pPartyFindPanel->OnClickEvent = DlgClick ;
	m_pPartyFindPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pPartyFindPanel ) ;
}

HRESULT		CRYLSocietyFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	switch ( m_pTabButton->GetFocusTabButton() )
	{
	case 0 : // 파티
		( ( CRYLPartyPanel* )m_pPartyPanel )->Update() ;
		break ;
	case 1 : // 친구
		( ( CRYLFriendPanel* )m_pFriendPanel )->Update() ;
		break ;
	case 2 : //거부
		( ( CRYLRefusalPanel* )m_pRefusalPanel )->Update() ;
		break ;
	case 3 : // 파티 찾기
		( ( CRYLPartyFindPanel* )m_pPartyFindPanel )->Update() ;
		break ;
	}

	return S_OK ;
}

VOID	CRYLSocietyFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pPartyPanel ) ;
	GM_DELETE( m_pFriendPanel ) ;
	GM_DELETE( m_pRefusalPanel ) ;
	GM_DELETE( m_pPartyFindPanel ) ;
}

VOID	CRYLSocietyFrameDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLSocietyFrameDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLSocietyFrameDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLSocietyFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pSocietyDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLSOCIETYDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pSocietyDlg->GetTabButton()->GetFocusTabButton() ;
			switch( iFocus )
			{
			case 0 : 
				{
					( ( CRYLPartyPanel* )g_pSocietyDlg->GetPartyPanel() )->SetVisible( TRUE ) ;
					( ( CRYLFriendPanel* )g_pSocietyDlg->GetFriendPanel() )->SetVisible( FALSE ) ;					
					( ( CRYLRefusalPanel* )g_pSocietyDlg->GetRefusalPanel() )->SetVisible( FALSE ) ;
					( ( CRYLPartyFindPanel* )g_pSocietyDlg->GetPartyFindPanel() )->SetVisible( FALSE ) ;
				}
				break ;
			case 1 : 
				{
					( ( CRYLPartyPanel* )g_pSocietyDlg->GetPartyPanel() )->SetVisible( FALSE ) ;
					( ( CRYLFriendPanel* )g_pSocietyDlg->GetFriendPanel() )->SetVisible( TRUE ) ;
					( ( CRYLRefusalPanel* )g_pSocietyDlg->GetRefusalPanel() )->SetVisible( FALSE ) ;
					( ( CRYLPartyFindPanel* )g_pSocietyDlg->GetPartyFindPanel() )->SetVisible( FALSE ) ;
				}
				break ;
			case 2 :
				{
					( ( CRYLPartyPanel* )g_pSocietyDlg->GetPartyPanel() )->SetVisible( FALSE ) ;
					( ( CRYLFriendPanel* )g_pSocietyDlg->GetFriendPanel() )->SetVisible( FALSE ) ;
					( ( CRYLRefusalPanel* )g_pSocietyDlg->GetRefusalPanel() )->SetVisible( TRUE ) ;
					( ( CRYLPartyFindPanel* )g_pSocietyDlg->GetPartyFindPanel() )->SetVisible( FALSE ) ;
				}
				break ;
			case 3 :
				{
					( ( CRYLPartyPanel* )g_pSocietyDlg->GetPartyPanel() )->SetVisible( FALSE ) ;
					( ( CRYLFriendPanel* )g_pSocietyDlg->GetFriendPanel() )->SetVisible( FALSE ) ;
					( ( CRYLRefusalPanel* )g_pSocietyDlg->GetRefusalPanel() )->SetVisible( FALSE ) ;
					( ( CRYLPartyFindPanel* )g_pSocietyDlg->GetPartyFindPanel() )->SetVisible( TRUE ) ;
				}
				break ;
			}
		}
		break ;
	}
}

VOID	CRYLSocietyFrameDlg::SetWhisperCharacter( char* lpszName ) 
{
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	char szBuff[ 64 ] ;
	memset( szBuff, 0, 64 ) ;
	
	sprintf( szBuff, "/s %s ", lpszName ) ;
	strcpy( CRYLCommunityData::Instance()->m_strRecentChatChrID, lpszName ) ;
	strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_strRecentChatChrID ) ;

    CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
	pChat->m_pChatEdit->SetText( szBuff ) ;
	
	pChat->m_pChatEdit->SetFocus() ;
	pChat->m_pChatEdit->SetFocusState( TRUE ) ;

	HIMC hImc;
	hImc = ImmGetContext(g_ClientMain.m_hWnd);
	ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
	ImmReleaseContext(g_ClientMain.m_hWnd, hImc);
}