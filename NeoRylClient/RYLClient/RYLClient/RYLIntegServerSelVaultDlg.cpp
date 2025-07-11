#include <winsock2.h>
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLStringTable.h"
#include "RYLNetworkData.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLIntegServerSelVaultDlg*	g_pRYLIntegServerSelVaultDlg = NULL ;

CRYLIntegServerSelVaultDlg::CRYLIntegServerSelVaultDlg () : CRYLDialog()
{
	m_cFirstLogin = 0;
    m_cAgentServerType = 0;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 365, &pImageDIB ) ;
	int i = 0 ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( ; i < 71 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 33, 47, 195, 275, 0xff946b42, 0x80000000,  1 ) ;		
	for ( i = 0 ; i < 7 ; i ++ )
		pImageDIB->MakeFrameRect( 39, 52 + ( i * 38 ), 184, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 336 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pRYLIntegServerSelVaultDlg = this ;	
	InitCtrl() ; 
	m_dwSelectVaultServer = 0 ;
	m_sSelIndex = RYLINTEGSERVERSELVAULT::VAULT_CKBOX ;
	
	m_lpTmpVaultSlot = NULL ;
	m_bVaultEnable = false ; 
}

CRYLIntegServerSelVaultDlg::~CRYLIntegServerSelVaultDlg ()
{
	FinalCtrl() ;
}

VOID CRYLIntegServerSelVaultDlg::InitCtrl ()
{
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	for ( int i = 0 ; i < 7 ; i ++ )
	{
		int iJumpHeight = i * 38 ;
		
		m_VaultSlot[ i ].pSelVaultButton = new CRYLCheckBox( this ) ;
		m_VaultSlot[ i ].pSelVaultButton->SetLeft( 197 ) ;
		m_VaultSlot[ i ].pSelVaultButton->SetTop( 76 + iJumpHeight ) ;
		m_VaultSlot[ i ].pSelVaultButton->SetInstance( RYLINTEGSERVERSELVAULT::VAULT_CKBOX + i ) ;
		m_VaultSlot[ i ].pSelVaultButton->OnClickEvent = DlgClick ;
		m_VaultSlot[ i ].pSelVaultButton->DefaultEvent( m_VaultSlot[ i ].pSelVaultButton ) ;
		m_VaultSlot[ i ].pSelVaultButton->SetVisible( FALSE ) ;
		AttachGMUIControl( m_VaultSlot[ i ].pSelVaultButton ) ;

		m_VaultSlot[ i ].pServerNameLabel = new CRYLLabel( this ) ;
		m_VaultSlot[ i ].pServerNameLabel->SetLeft( 48 ) ;
		m_VaultSlot[ i ].pServerNameLabel->SetTop( 72 + iJumpHeight ) ;
		m_VaultSlot[ i ].pServerNameLabel->SetFontColor (0xffffffff);
		m_VaultSlot[ i ].pServerNameLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_VaultSlot[ i ].pServerNameLabel ) ;

		m_VaultSlot[ i ].pUsedNumLabel = new CRYLLabel( this ) ;
		m_VaultSlot[ i ].pUsedNumLabel->SetLeft( 48 ) ;
		m_VaultSlot[ i ].pUsedNumLabel->SetTop( 87 + iJumpHeight ) ;
		m_VaultSlot[ i ].pUsedNumLabel->SetFontColor (0xffffffb2);
		m_VaultSlot[ i ].pUsedNumLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_VaultSlot[ i ].pUsedNumLabel ) ;

		m_VaultSlot[ i ].pGoldLabel = new CRYLLabel( this ) ;
		m_VaultSlot[ i ].pGoldLabel->SetLeft( 81 ) ;
		m_VaultSlot[ i ].pGoldLabel->SetTop( 87 + iJumpHeight ) ;
		m_VaultSlot[ i ].pGoldLabel->SetFontColor (0xffffffb2);
		m_VaultSlot[ i ].pGoldLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_VaultSlot[ i ].pGoldLabel ) ;  
	}

	// 통합서버 창고선택
	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 7 ) ;
	m_pTitleLabel->SetTop( 19 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 3500 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;  
	
	// 창고를 선택하여주십시요
	m_pSelVaultLabel = new CRYLLabel( this ) ;
	m_pSelVaultLabel->SetLeft( 11 ) ;
	m_pSelVaultLabel->SetTop( 45 ) ;
	m_pSelVaultLabel->SetFontColor (0xffffffff);
	m_pSelVaultLabel->SetAutoSize( TRUE ) ;
	m_pSelVaultLabel->SetCaption( CRYLStringTable::m_strString[ 3501 ] ) ;
	AttachGMUIControl( m_pSelVaultLabel ) ;  

	// 이전단계로
	m_pPrevStepButton = new CRYLButton( this ) ;
	m_pPrevStepButton->SetLeft( 87 ) ;
	m_pPrevStepButton->SetTop( 343 ) ;
	m_pPrevStepButton->SetCaption( CRYLStringTable::m_strString[ 3502 ] ) ;
	m_pPrevStepButton->SetFontColor( 0xffffffff) ;
	m_pPrevStepButton->SetColor( 0xffffffff) ;
	m_pPrevStepButton->OnClickEvent = DlgClick ;
	m_pPrevStepButton->SetInstance( RYLINTEGSERVERSELVAULT::PREV_BUTTON ) ;
	m_pPrevStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPrevStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPrevStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPrevStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPrevStepButton ) ;

	// 선택완료
	m_pArrayButton = new CRYLButton( this ) ;
	m_pArrayButton->SetLeft( 168 ) ;
	m_pArrayButton->SetTop( 343 ) ;
	m_pArrayButton->SetCaption( CRYLStringTable::m_strString[ 3503 ] ) ;
	m_pArrayButton->SetFontColor( 0xffffffff) ;
	m_pArrayButton->SetColor( 0xffffffff) ;
	m_pArrayButton->OnClickEvent = DlgClick ;
	m_pArrayButton->SetInstance( RYLINTEGSERVERSELVAULT::CONFIRM_BUTTON ) ;
	m_pArrayButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pArrayButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pArrayButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pArrayButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pArrayButton ) ;
}

VOID CRYLIntegServerSelVaultDlg::FinalCtrl ()
{
	GM_DELETE( m_pTitleLabel ) ;	
	GM_DELETE( m_pSelVaultLabel ) ;	
	GM_DELETE( m_pPrevStepButton ) ;		
	GM_DELETE( m_pArrayButton ) ;

	m_lpTmpVaultSlot = NULL ;
}

VOID CRYLIntegServerSelVaultDlg::Show( CGMUIObject* Sender )
{
	CheckButton( m_sSelIndex ) ;
}

VOID CRYLIntegServerSelVaultDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLIntegServerSelVaultDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLIntegServerSelVaultDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID CRYLIntegServerSelVaultDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pRYLIntegServerSelVaultDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLINTEGSERVERSELVAULT::CONFIRM_BUTTON :	
		{
			CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
			pScene->GetSelectedConfirmDlg()->SetVisible( TRUE ) ;
			g_pRYLIntegServerSelVaultDlg->SetVisible( FALSE ) ;
		}
		break ;
	case RYLINTEGSERVERSELVAULT::PREV_BUTTON :	
		{
			CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
			pScene->GetIntegServerSelCharDlg()->SetVisible( TRUE ) ;
			g_pRYLIntegServerSelVaultDlg->SetVisible( FALSE ) ;
		}
		break ;
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 1 :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 2 :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 3 :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 4 :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 5 :	
	case RYLINTEGSERVERSELVAULT::VAULT_CKBOX + 6 :	
		{
			g_pRYLIntegServerSelVaultDlg->CheckButton( Sender->GetInstance() ) ;
		}
		break ;
	}
}

void CRYLIntegServerSelVaultDlg::CheckButton( int iIndex ) 
{
	iIndex -= RYLINTEGSERVERSELVAULT::VAULT_CKBOX ;

	for ( int i = 0 ; i < VAULT_NUM ; i ++ )
	{
		m_VaultSlot[ i ].pSelVaultButton->SetChecked( FALSE ) ;
	}

	m_VaultSlot[ iIndex ].pSelVaultButton->SetChecked( TRUE ) ;
	m_lpTmpVaultSlot = &m_VaultSlot[ iIndex ] ;
	m_dwSelectVaultServer = m_lpTmpVaultSlot->dwSelectVaultServer ;

	m_sSelIndex = iIndex + RYLINTEGSERVERSELVAULT::VAULT_CKBOX ;
}

void CRYLIntegServerSelVaultDlg::SetVaultInfo( UnifiedStoreInfo* StoreInfo, unsigned long dwStoreInfoNum_Out ) 
{
	memcpy(m_UnifiedStoreInfo, StoreInfo, 
        sizeof(UnifiedStoreInfo) * min(unsigned long(PktUnifiedCharInfo::MAX_STORE_INFO), dwStoreInfoNum_Out)) ;

	m_dwStoreInfoNum_Out = dwStoreInfoNum_Out ;
}

void CRYLIntegServerSelVaultDlg::SetUnifiedStatus ( unsigned char cAgentServerType, unsigned char cFirstLogin )
{
    m_cAgentServerType = cAgentServerType;
    m_cFirstLogin = cFirstLogin;
}

void CRYLIntegServerSelVaultDlg::UpdateDlg() 
{
	char szBuff[ 64 ] ;
	
    UnifiedStoreInfo* lpPos = m_UnifiedStoreInfo;
    UnifiedStoreInfo* lpEnd = m_UnifiedStoreInfo + m_dwStoreInfoNum_Out;

    for ( ; lpPos != lpEnd; ++lpPos )
	{
        if ( lpPos->cOldServerGroupID != UnifiedConst::Part1Unified &&
             m_cAgentServerType < lpPos->cOldServerGroupID)
        {
            continue;
        }

        unsigned char cOldServerGroupID = lpPos->cOldServerGroupID;
        unsigned char cIndex = 
            (lpPos->cOldServerGroupID != UnifiedConst::Part1Unified) 
            ? lpPos->cOldServerGroupID - 1 : VAULT_NUM - 1;

        const char* lpszText = (lpPos->cOldServerGroupID == UnifiedConst::Part1Unified) 
			? CRYLStringTable::m_strString[ 3504 ] : CRYLNetworkData::Instance()->GetOldServerName( cOldServerGroupID ) ;

		DWORD dwMaxTab = 0L ;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB1) dwMaxTab = 1;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB2) dwMaxTab = 2;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB3) dwMaxTab = 3;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB4) dwMaxTab = 4;

		if ( dwMaxTab )
		{
			m_VaultSlot[ cIndex ].pSelVaultButton->SetVisible( TRUE ) ;
			sprintf( szBuff, CRYLStringTable::m_strString[ 3505 ], dwMaxTab ) ;
			m_VaultSlot[ cIndex ].pUsedNumLabel->SetCaption( szBuff ) ;
			sprintf( szBuff, ": %s Gold", CRYLStringTable::MakePrintGold( lpPos->Gold ) ) ;
			m_VaultSlot[ cIndex ].pGoldLabel->SetCaption( szBuff ) ;
		}
		else
		{
			m_VaultSlot[ cIndex ].pSelVaultButton->SetVisible( FALSE ) ;
			sprintf( szBuff, CRYLStringTable::m_strString[ 3506 ] ) ;
			m_VaultSlot[ cIndex ].pUsedNumLabel->SetCaption( szBuff ) ;
			m_VaultSlot[ cIndex ].pSelVaultButton->SetVisible( FALSE ) ;
		}

		m_VaultSlot[ cIndex ].dwSelectVaultServer = cOldServerGroupID;
		sprintf( szBuff, CRYLStringTable::m_strString[ 3507 ], lpszText ) ;
		m_VaultSlot[ cIndex ].pServerNameLabel->SetCaption( szBuff ) ;
	}

	for (int i = 0 ; i < VAULT_NUM ; i ++ )
	{
		if ( m_VaultSlot[ i ].pSelVaultButton->GetVisible() )
		{
			m_sSelIndex = i + RYLINTEGSERVERSELVAULT::VAULT_CKBOX ;
			break ;
		}
	}
}