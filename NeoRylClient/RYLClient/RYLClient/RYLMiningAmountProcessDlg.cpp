#include <winsock2.h>

#include <Network/ClientSocket/Send/SendItem.h>

#include "WinInput.h"
#include "GMImageDib.h"


#include "RYLClientMain.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLMiningAmountProcessDlg.h"
#include "RYLMiningAmountDlg.h"
#include <Map/FieldMap/MineralVeinMgr.h>
#include "GMMemory.h"

CRYLMiningAmountProcessDlg*		g_pMiningAmountProcessDlg = NULL ;

CRYLMiningAmountProcessDlg::CRYLMiningAmountProcessDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 145, &pImageDIB ) ;

	int i;
	CGMImageDib*	pTempImage;
	pTempImage = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 );
	for ( i = 0 ; i < 27 ; i ++ )
	{
		SetRect( &rt, 0, 38, 184, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage ) ;
	}

	SetRect( &rt, 0, 43, 184, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage  ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 10, 108, 158, 17, 0xff959595, 0x80000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 145 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	m_pCanceltButton = NULL ;						// 취소 버튼
	m_pProcessLabel = NULL ;						// 프로세스 라벨
	m_pMessageLabel1 = NULL ;						// 문구 첫째줄
	m_pMessageLabel2 = NULL ;						// 문구 둘째줄
	m_pMessageLabel3 = NULL ;						// 문구 셋째줄
	m_pTitleLabel = NULL ;							// Title
	m_ulTime = 0;
	m_lpKitItem = NULL;

	g_pMiningAmountProcessDlg = this ;
	InitCtrl() ;
}


CRYLMiningAmountProcessDlg::~CRYLMiningAmountProcessDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLMiningAmountProcessDlg::InitCtrl() 
{
	CGMImageDib* pSprite			;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;


	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 취소 버튼
	m_pCanceltButton = new CRYLButton( this ) ;
	m_pCanceltButton->SetLeft( 175 ) ;
	m_pCanceltButton->SetTop( 119 ) ;
	m_pCanceltButton->SetCaption( CRYLStringTable::m_strString[1897] ) ;  // 취 소
	m_pCanceltButton->SetFontColor( 0xffffffff) ;
	m_pCanceltButton->SetColor( 0xffffffff) ;
	m_pCanceltButton->OnClickEvent = DlgClick ;
	m_pCanceltButton->SetInstance( RYLMININGAMOUNTPROCESSDLG::CANCEL_BUTTON ) ;
	m_pCanceltButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCanceltButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCanceltButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCanceltButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCanceltButton ) ;

	// 프로세스 라벨
	m_pProcessLabel = new CRYLLabel( this ) ;		
	m_pProcessLabel->SetLeft( 15 ) ;
	m_pProcessLabel->SetTop( 126 ) ;
	m_pProcessLabel->SetFontColor (0xffffffff);
	m_pProcessLabel->SetAutoSize( TRUE ) ;
	m_pProcessLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pProcessLabel ) ;


	// 문구 첫째줄
	m_pMessageLabel1 = new CRYLLabel( this ) ;		
	m_pMessageLabel1->SetLeft( 11 ) ;
	m_pMessageLabel1->SetTop( 43 ) ;
	m_pMessageLabel1->SetFontColor (0xffffffff);
	m_pMessageLabel1->SetAutoSize( TRUE ) ;
	m_pMessageLabel1->SetCaption( CRYLStringTable::m_strString[3171] ) ;
	AttachGMUIControl( m_pMessageLabel1 ) ;

	// 문구 둘째줄
	m_pMessageLabel2 = new CRYLLabel( this ) ;		
	m_pMessageLabel2->SetLeft( 11 ) ;
	m_pMessageLabel2->SetTop( 64 ) ;
	m_pMessageLabel2->SetFontColor (0xffffffff);
	m_pMessageLabel2->SetAutoSize( TRUE ) ;
	m_pMessageLabel2->SetCaption( CRYLStringTable::m_strString[3172]) ;
	AttachGMUIControl( m_pMessageLabel2 ) ;

	// 문구 셋째줄
	m_pMessageLabel3 = new CRYLLabel( this ) ;		
	m_pMessageLabel3->SetLeft( 11 ) ;
	m_pMessageLabel3->SetTop( 85 ) ;
	m_pMessageLabel3->SetFontColor (0xffffffff);
	m_pMessageLabel3->SetAutoSize( TRUE ) ;
	m_pMessageLabel3->SetCaption( CRYLStringTable::m_strString[3173] ) ;
	AttachGMUIControl( m_pMessageLabel3 ) ;


	// 타이틀
	m_pTitleLabel = new CRYLLabel( this ) ;		
	m_pTitleLabel->SetLeft( 11 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[3170] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;

}

VOID	CRYLMiningAmountProcessDlg::FinalCtrl() 
{
	GM_DELETE( m_pCanceltButton ) ;						// 취소 버튼
	GM_DELETE( m_pProcessLabel );						// 프로세스 라벨
	GM_DELETE( m_pMessageLabel1 );						// 문구 첫째줄
	GM_DELETE( m_pMessageLabel2 );						// 문구 둘째줄
	GM_DELETE( m_pMessageLabel3 );						// 문구 셋째줄
	GM_DELETE( m_pTitleLabel );							// Title
}

VOID	CRYLMiningAmountProcessDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->SetAutoRun( FALSE ) ;
	m_ulTime = timeGetTime();
	m_pProcessLabel->SetCaption( "" );
}

VOID	CRYLMiningAmountProcessDlg::Hide( CGMUIObject* Sender ) 
{
	m_ulTime = 0;
}

VOID	CRYLMiningAmountProcessDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;
	if( UpdateProcess() )
	{
		CheckMineCount();
	}

}

VOID
CRYLMiningAmountProcessDlg::CheckMineCount()
{
	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLMiningAmountDlg * pDlg = (CRYLMiningAmountDlg * )pScene->GetMiningAmountDlg();
	if( pDlg )
	{
		if( m_lpKitItem )
		{
			CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
			Item::CUseItem *lpKit = Item::CUseItem::DowncastToUseItem(m_lpKitItem->m_lpItemBase);
			if( lpKit )
			{
				g_pClientSocket->SetStatusFlag(NS_USEITEM);
				SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpKit->GetPos());
				pDlg->UpdateMineral();
			}
		}
	}
	SetVisible( FALSE );
}

BOOL
CRYLMiningAmountProcessDlg::UpdateProcess()
{
	CHAR	szBuff[24];

	unsigned long ulTime = timeGetTime();
	int nProcess = ( (ulTime - m_ulTime) / 3000 );

	if( nProcess > 20 )
		nProcess = 20;

	memset( szBuff, 0, sizeof(CHAR)*20 );
	for( int i = 0; i < nProcess; ++i )
	{
		strcat( szBuff,">" );
	}
	m_pProcessLabel->SetCaption( szBuff );

	return ( nProcess == 20 ) ? TRUE : FALSE;
}

HRESULT		CRYLMiningAmountProcessDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;

	bEdge = TRUE;
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

	return S_OK;
}

VOID	CRYLMiningAmountProcessDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pMiningAmountProcessDlg )
		return ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch( Sender->GetInstance() )
	{
	case RYLMININGAMOUNTPROCESSDLG::CANCEL_BUTTON :
		{
			g_pMiningAmountProcessDlg->Cancel() ;
		}
		break;
	}
}

VOID	CRYLMiningAmountProcessDlg::Cancel()
{
	SetVisible( FALSE );
}

