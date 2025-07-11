
#include "RYLClientMain.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLCheckBox.h"
#include "RYLEdit.h"
#include "BaseDataDefine.h"

#include <WinInput.h>
#include <RenderOption.h>

#include "RYLGameScene.h"
#include "RYLCharacterSelectScene.h"
#include "RYLSceneManager.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLStringTable.h"

#include "RYLOptionDlg.h"
#include "RYLChattingDlg.h"
#include "RYLSystemDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "GMMemory.h"


//------------------------------------------------------------------------------

CRYLSystemDlg*	g_pSystemDlg = NULL ;

CRYLSystemDlg::CRYLSystemDlg() : CRYLDialog()
{
	m_pChatacterSelectButton	= NULL ;
	m_pOptionButton				= NULL ;		
	m_pQuitButton				= NULL ;				
	m_pContinueButton			= NULL ;		

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
	
	g_pSystemDlg = this ;	
	InitCtrl() ;
}

CRYLSystemDlg::~CRYLSystemDlg ()
{
	FinalCtrl() ;
}

VOID CRYLSystemDlg::InitCtrl ()
{
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pChatacterSelectButton = new CRYLButton( this ) ;
	m_pChatacterSelectButton->SetLeft( 87 ) ;
	m_pChatacterSelectButton->SetTop( 52 ) ;
	m_pChatacterSelectButton->SetCaption( CRYLStringTable::m_strString[ 2198 ] ) ;
	m_pChatacterSelectButton->SetFontColor( 0xffffffff) ;
	m_pChatacterSelectButton->SetColor( 0xffffffff) ;
	m_pChatacterSelectButton->OnClickEvent = DlgClick ;
	m_pChatacterSelectButton->SetInstance( RYLSYSTEMDLG::CHARACTERSELECT_BUTTON ) ;
	m_pChatacterSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChatacterSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChatacterSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChatacterSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pChatacterSelectButton ) ;

	m_pOptionButton = new CRYLButton( this ) ;
	m_pOptionButton->SetLeft( 87 ) ;
	m_pOptionButton->SetTop( 84 ) ;
	m_pOptionButton->SetCaption( CRYLStringTable::m_strString[ 2199 ] ) ;
	m_pOptionButton->SetFontColor( 0xffffffff) ;
	m_pOptionButton->SetColor( 0xffffffff) ;
	m_pOptionButton->OnClickEvent = DlgClick ;
	m_pOptionButton->SetInstance( RYLSYSTEMDLG::OPTION_BUTTON ) ;
	m_pOptionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOptionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOptionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOptionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOptionButton ) ;

	m_pQuitButton = new CRYLButton( this ) ;
	m_pQuitButton->SetLeft( 87 ) ;
	m_pQuitButton->SetTop( 116 ) ;
	m_pQuitButton->SetCaption( CRYLStringTable::m_strString[ 2200 ] ) ;
	m_pQuitButton->SetFontColor( 0xffffffff) ;
	m_pQuitButton->SetColor( 0xffffffff) ;
	m_pQuitButton->OnClickEvent = DlgClick ;
	m_pQuitButton->SetInstance( RYLSYSTEMDLG::QUIT_BUTTON ) ;
	m_pQuitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pQuitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pQuitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pQuitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pQuitButton ) ;

	m_pContinueButton = new CRYLButton( this ) ;
	m_pContinueButton->SetLeft( 87 ) ;
	m_pContinueButton->SetTop( 148 ) ;
	m_pContinueButton->SetCaption( CRYLStringTable::m_strString[ 2201 ] ) ;
	m_pContinueButton->SetFontColor( 0xffffffff) ;
	m_pContinueButton->SetColor( 0xffffffff) ;
	m_pContinueButton->OnClickEvent = DlgClick ;
	m_pContinueButton->SetInstance( RYLSYSTEMDLG::CONTINUE_BUTTON ) ;
	m_pContinueButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pContinueButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pContinueButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pContinueButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pContinueButton ) ;

	m_pSystemLabel = new CRYLLabel( this ) ;
	m_pSystemLabel->SetLeft( 9 ) ;
	m_pSystemLabel->SetTop( 18 ) ;
	m_pSystemLabel->SetFontColor (0xffffffff);
	m_pSystemLabel->SetAutoSize( TRUE ) ;
	m_pSystemLabel->SetCaption( CRYLStringTable::m_strString[ 2202 ] ) ;
	AttachGMUIControl( m_pSystemLabel ) ;
}

VOID CRYLSystemDlg::FinalCtrl ()
{
	GM_DELETE( m_pChatacterSelectButton ) ;	
	GM_DELETE( m_pOptionButton ) ;			
	GM_DELETE( m_pQuitButton ) ;				
	GM_DELETE( m_pContinueButton ) ;			
	GM_DELETE( m_pSystemLabel ) ;			
}

VOID CRYLSystemDlg::Show( CGMUIObject* Sender )
{
}

VOID CRYLSystemDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID CRYLSystemDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLSystemDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLSystemDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pSystemDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLSYSTEMDLG::CHARACTERSELECT_BUTTON :
			// 강제종료
			CRYLGameData::SetClientExitCounter( 0 ) ;
			CRYLGameData::SetLogoutCounter( 10000 ) ;
			CRYLGameData::Instance()->m_lLogoutTimeResult = 0 ;
			g_pSystemDlg->SetVisible( FALSE ) ;
			break ;

		case RYLSYSTEMDLG::OPTION_BUTTON :	
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetOptionDlg()->SetVisible( TRUE ) ;
			g_pSystemDlg->SetVisible( FALSE ) ;
			break ;

		case RYLSYSTEMDLG::QUIT_BUTTON :		
			// 강제종료
			CRYLGameData::SetClientExitCounter( 10000 ) ;
			CRYLGameData::SetLogoutCounter( 0 ) ;
			CRYLGameData::Instance()->m_lLogoutTimeResult = 0 ;
			g_pSystemDlg->SetVisible( FALSE ) ;
			break ;

		case RYLSYSTEMDLG::CONTINUE_BUTTON :
			g_pSystemDlg->SetVisible( FALSE ) ;
			break ;
	}
}

