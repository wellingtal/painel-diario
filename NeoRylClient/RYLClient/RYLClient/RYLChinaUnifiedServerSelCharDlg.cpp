#include <winsock2.h>
#include "RYLChinaUnifiedServerSelCharDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLComboBox.h"
#include "RYLEdit.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLLoginDlg.h"
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include "GMMemory.h"
//------------------------------------------------------------------------------

CRYLChinaUnifiedServerSelCharDlg*	g_pChinaServerSelCharDlg = NULL ;

CRYLChinaUnifiedServerSelCharDlg::CRYLChinaUnifiedServerSelCharDlg () : CRYLDialog()
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 300, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 58 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 4, 49, 248, 194, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 7, 52, 83, 187, 0xff959595, 0xc0000000,  1 ) ;
	for ( i = 0 ; i < 5 ; i ++ )
		pImageDIB->MakeFrameRect( 92, 52 + ( i * 38 ), 157, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 7, 248, 157, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 305 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	g_pChinaServerSelCharDlg = this ;	
	InitCtrl() ;

	m_cAgentServerType = 0 ;
	m_cFirstLogin      = 0 ;
	m_dwFastLoginSlot  = -1 ;
}

CRYLChinaUnifiedServerSelCharDlg::~CRYLChinaUnifiedServerSelCharDlg ()
{
	FinalCtrl() ;
}

VOID CRYLChinaUnifiedServerSelCharDlg::InitCtrl ()
{
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	for ( int i = 0 ; i < 5 ; i ++ )
	{
		int iJumpHeight = i * 38 ;

		m_OldCharSlot[ i ].pCharNameLabel = new CRYLLabel( this ) ;
		m_OldCharSlot[ i ].pCharNameLabel->SetLeft( 100 ) ;
		m_OldCharSlot[ i ].pCharNameLabel->SetTop( 72 + iJumpHeight ) ;
		m_OldCharSlot[ i ].pCharNameLabel->SetFontColor (0xffffffff);
		m_OldCharSlot[ i ].pCharNameLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_OldCharSlot[ i ].pCharNameLabel ) ;

		m_OldCharSlot[ i ].pCharClassLabel = new CRYLLabel( this ) ;
		m_OldCharSlot[ i ].pCharClassLabel->SetLeft( 100 ) ;
		m_OldCharSlot[ i ].pCharClassLabel->SetTop( 87 + iJumpHeight ) ;
		m_OldCharSlot[ i ].pCharClassLabel->SetFontColor (0xffffffb2);
		m_OldCharSlot[ i ].pCharClassLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_OldCharSlot[ i ].pCharClassLabel ) ;  

		m_OldCharSlot[ i ].pCharLevelLabel = new CRYLLabel( this ) ;
		m_OldCharSlot[ i ].pCharLevelLabel->SetLeft( 168 ) ;
		m_OldCharSlot[ i ].pCharLevelLabel->SetTop( 87 + iJumpHeight ) ;
		m_OldCharSlot[ i ].pCharLevelLabel->SetFontColor (0xffffffb2);
		m_OldCharSlot[ i ].pCharLevelLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_OldCharSlot[ i ].pCharLevelLabel ) ;  
	}

	m_pSelectButton = new CRYLButton( this ) ;
	m_pSelectButton->SetLeft( 170 ) ;
	m_pSelectButton->SetTop( 274 ) ;
	m_pSelectButton->SetCaption( CRYLStringTable::m_strString[ 3389 ] ) ;
	m_pSelectButton->SetFontColor( 0xffffffff) ;
	m_pSelectButton->SetColor( 0xffffffff) ;
	m_pSelectButton->OnClickEvent = DlgClick ;
	m_pSelectButton->SetInstance( RYLCHINASELCHARDLG::SELECT_BUTTON ) ;
	m_pSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pSelectButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pSelectButton ) ;

	// 통합서버 캐릭터선택
	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 7 ) ;
	m_pTitleLabel->SetTop( 19 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 3390 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;  

	// 기존 캐릭터선택
	m_pOldCharLabel = new CRYLLabel( this ) ;
	m_pOldCharLabel->SetLeft( 8 ) ;
	m_pOldCharLabel->SetTop( 47 ) ;
	m_pOldCharLabel->SetFontColor (0xffffffff);
	m_pOldCharLabel->SetAutoSize( TRUE ) ;
	m_pOldCharLabel->SetCaption( CRYLStringTable::m_strString[ 3391 ] ) ;
	AttachGMUIControl( m_pOldCharLabel ) ;  

	// 창고 금액
	m_pVaultLabel = new CRYLLabel( this ) ;
	m_pVaultLabel->SetLeft( 15 ) ;
	m_pVaultLabel->SetTop( 268 ) ;
	m_pVaultLabel->SetFontColor (0xffffffb2);
	m_pVaultLabel->SetAutoSize( TRUE ) ;
	m_pVaultLabel->SetCaption( CRYLStringTable::m_strString[ 3392 ]) ;
	AttachGMUIControl( m_pVaultLabel ) ; 

	// 금액
	m_pVaultGoldLabel = new CRYLLabel( this ) ;
	m_pVaultGoldLabel->SetLeft( 70 ) ;
	m_pVaultGoldLabel->SetTop( 268 ) ;
	m_pVaultGoldLabel->SetFontColor (0xffffffb2);
	m_pVaultGoldLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pVaultGoldLabel ) ; 

	// 창고탭 
	m_pVaultTebLabel = new CRYLLabel( this ) ;
	m_pVaultTebLabel->SetLeft( 15 ) ;
	m_pVaultTebLabel->SetTop( 283 ) ;
	m_pVaultTebLabel->SetFontColor (0xffffffb2);
	m_pVaultTebLabel->SetAutoSize( TRUE ) ;
	m_pVaultTebLabel->SetCaption( CRYLStringTable::m_strString[ 3393 ] ) ;
	AttachGMUIControl( m_pVaultTebLabel ) ; 

	// 창고탭 개수
	m_pVaultUsedLabel = new CRYLLabel( this ) ;
	m_pVaultUsedLabel->SetLeft( 70 ) ;
	m_pVaultUsedLabel->SetTop( 283 ) ;
	m_pVaultUsedLabel->SetFontColor (0xffffffb2);
	m_pVaultUsedLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pVaultUsedLabel ) ; 

	m_pServerList = new CRYLListBox( this, FALSE ) ;
	m_pServerList->SetLeft( 14 ) ;
	m_pServerList->SetTop( 74 ) ;
	m_pServerList->SetWidth( 75 ) ;
	m_pServerList->SetHeight( 179 ) ;
	m_pServerList->SetItemHeight( 16 ) ;
	m_pServerList->SetFontColor( 0xffffffff ) ;
	m_pServerList->SetInstance( RYLINTEGSERVERSELCHAR::SEVER_LIST ) ;
	m_pServerList->OnClickEvent = DlgClick ;
	m_pServerList->SelectItemIndex( 0 ) ;
	m_pServerList->SetItemIndex( 0 ) ;
	AttachGMUIControl( m_pServerList ) ;
}

VOID CRYLChinaUnifiedServerSelCharDlg::FinalCtrl ()
{
	for ( int i = 0 ; i < 5 ; i ++ )
	{
		GM_DELETE( m_OldCharSlot[ i ].pCharLevelLabel ) ;
		GM_DELETE( m_OldCharSlot[ i ].pCharClassLabel ) ;
		GM_DELETE( m_OldCharSlot[ i ].pCharNameLabel ) ;
	}
	GM_DELETE( m_pSelectButton ) ;
	GM_DELETE( m_pTitleLabel ) ;	
	GM_DELETE( m_pOldCharLabel ) ;	
	GM_DELETE( m_pServerList ) ;
	GM_DELETE( m_pVaultLabel ) ;
	GM_DELETE( m_pVaultGoldLabel ) ;
	GM_DELETE( m_pVaultUsedLabel ) ;
	GM_DELETE( m_pVaultTebLabel ) ;
}

VOID CRYLChinaUnifiedServerSelCharDlg::Show( CGMUIObject* Sender )
{
	SelectServer() ;
}

VOID CRYLChinaUnifiedServerSelCharDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLChinaUnifiedServerSelCharDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLChinaUnifiedServerSelCharDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID CRYLChinaUnifiedServerSelCharDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pChinaServerSelCharDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLCHINASELCHARDLG::SEVER_LIST :	
		g_pChinaServerSelCharDlg->SelectServer() ;
		break ;
	case RYLCHINASELCHARDLG::SELECT_BUTTON :	
		{
			CRYLLoginScene* pScene = static_cast< CRYLLoginScene* >(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
			pScene->ChinaSelectSlot() ;
			g_pChinaServerSelCharDlg->SetVisible( FALSE ) ;
		}
		break ;
	}
}

const char* CRYLChinaUnifiedServerSelCharDlg::GetSlotName()
{
	int iIdx = m_pServerList->GetItemIndex() ;
	if ( iIdx == -1 )		return NULL ;

	return m_pServerList->GetItem( iIdx ) ;
}

void CRYLChinaUnifiedServerSelCharDlg::SetUnifiedStatus ( unsigned char cAgentServerType, unsigned char cFirstLogin )
{
	m_cAgentServerType = cAgentServerType;
	m_cFirstLogin = cFirstLogin; 

	m_pServerList->ClearItem() ;

	for ( int i = 0 ; i < m_cFirstLogin ; i ++ )
	{
		m_pServerList->AddItem( CRYLStringTable::m_strString[ 3382 + i ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	}
}

void CRYLChinaUnifiedServerSelCharDlg::SetCharInfo( USER_INFO* UserInfo, UnifiedCharData* CharData, 
												   unsigned long dwCharDataNum_Out ) 
{
	m_UserInfo = *UserInfo ;

	memcpy(m_UnifiedCharData, CharData, sizeof(UnifiedCharData) * min(int(PktUnifiedCharInfo::MAX_CHAR_DATA), int(dwCharDataNum_Out)));

	m_dwCharDataNum_Out = dwCharDataNum_Out ;	
}

void CRYLChinaUnifiedServerSelCharDlg::SetVaultInfo( UnifiedStoreInfo* StoreInfo, unsigned long dwStoreInfoNum_Out ) 
{
	memcpy(m_UnifiedStoreInfo, StoreInfo, sizeof(UnifiedStoreInfo) * min(unsigned long(PktUnifiedCharInfo::MAX_STORE_INFO), dwStoreInfoNum_Out)) ;

	m_dwStoreInfoNum_Out = dwStoreInfoNum_Out ;
}

void CRYLChinaUnifiedServerSelCharDlg::UpdateCharDlg() 
{
	UnifiedCharData* lpPos = m_UnifiedCharData;
	UnifiedCharData* lpEnd = m_UnifiedCharData + m_dwCharDataNum_Out;

	char szBuff[ 64 ] ;

	lpPos = m_UnifiedCharData;
	lpEnd = m_UnifiedCharData + m_dwCharDataNum_Out;

	int iSlot[ 10 ] ;
	memset( iSlot, 0, sizeof( int ) * 10 ) ;

	for (; lpPos != lpEnd; ++lpPos)
	{
		if ( lpPos->cOldServerGroupID == UnifiedConst::Part1Unified )
			lpPos->cOldServerGroupID = 7 ;

		unsigned char iIdx = lpPos->cOldServerGroupID - 1 ;

		m_SortData[ iIdx ][ iSlot[ iIdx ] ].bEnable = true ;
		strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szName, lpPos->szName ) ;
		strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szClass, CRYLGameData::Instance()->GetClassName( lpPos->sClass ) ) ;
		sprintf( szBuff, "Lv %d", lpPos->cLevel ) ;
		strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szLevel, szBuff ) ;
		m_SortData[ iIdx ][ iSlot[ iIdx ] ].dwCID = lpPos->dwNewCID ;
		m_SortData[ iIdx ][ iSlot[ iIdx ] ].sClass = lpPos->sClass ;
		m_SortData[ iIdx ][ iSlot[ iIdx ] ].cLevel = lpPos->cLevel ;
		++ iSlot[ iIdx ] ;
	}	
}

void CRYLChinaUnifiedServerSelCharDlg::UpdateVaultDlg() 
{
	char szBuff[ 64 ] ;

	UnifiedStoreInfo* lpPos = m_UnifiedStoreInfo;
	UnifiedStoreInfo* lpEnd = m_UnifiedStoreInfo + m_dwStoreInfoNum_Out;

	for ( ; lpPos != lpEnd; ++lpPos )
	{
		unsigned char cOldServerGroupID = lpPos->cOldServerGroupID;
		unsigned char cIndex = 
			(lpPos->cOldServerGroupID != UnifiedConst::Part1Unified) 
			? lpPos->cOldServerGroupID - 1 : VAULT_NUM - 1;

		const char* lpszText = (lpPos->cOldServerGroupID == UnifiedConst::Part1Unified) 
			? CRYLStringTable::m_strString[ 3394 ] : CRYLNetworkData::Instance()->GetOldServerName( cOldServerGroupID ) ;

		DWORD dwMaxTab = 0L ;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB1) dwMaxTab = 1;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB2) dwMaxTab = 2;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB3) dwMaxTab = 3;
		if (lpPos->Flag & PktDepositUpdateDB::ITEM_TAB4) dwMaxTab = 4;

		if ( dwMaxTab )
		{
			m_VaultSlot[ cIndex ].cUsedTab = dwMaxTab ;
			sprintf( szBuff, "%s Gold", CRYLStringTable::MakePrintGold( lpPos->Gold ) ) ;
			strcpy( m_VaultSlot[ cIndex ].szGold, szBuff ) ;
		}
		else
		{
			m_VaultSlot[ cIndex ].cUsedTab = 0 ;
			sprintf( szBuff, CRYLStringTable::m_strString[ 3395 ] ) ;
			strcpy( m_VaultSlot[ cIndex ].szGold, szBuff ) ;
		}
	}
}

void CRYLChinaUnifiedServerSelCharDlg::SelectServer() 
{
	int iIdx = m_pServerList->GetItemIndex() ;

	if ( iIdx == -1 )
		return ;

	ResetServerCharInfo() ;

	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i++ )
	{
		if ( strcmp( m_SortData[ iIdx ][ i ].szName, "" ) )
		{
			m_OldCharSlot[ i ].pCharNameLabel->SetCaption( m_SortData[ iIdx ][ i ].szName ) ;
			m_OldCharSlot[ i ].pCharClassLabel->SetCaption(m_SortData[ iIdx ][ i ].szClass ) ;
			m_OldCharSlot[ i ].pCharLevelLabel->SetCaption(m_SortData[ iIdx ][ i ].szLevel ) ;
		}
	}

	char szBuff[ 16 ] ;
	sprintf( szBuff, CRYLStringTable::m_strString[ 3396 ], m_VaultSlot[ iIdx ].cUsedTab ) ;
	m_pVaultUsedLabel->SetCaption( szBuff ) ;
	m_pVaultGoldLabel->SetCaption( m_VaultSlot[ iIdx ].szGold ) ;
}

void CRYLChinaUnifiedServerSelCharDlg::ResetServerCharInfo() 
{
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		m_OldCharSlot[ i ].pCharNameLabel->SetCaption( "" ) ;
		m_OldCharSlot[ i ].pCharClassLabel->SetCaption( "" ) ;
		m_OldCharSlot[ i ].pCharLevelLabel->SetCaption( "" ) ;
	}
}

void CRYLChinaUnifiedServerSelCharDlg::SendUnifiedCharSelect() 
{
	unsigned long dwCID[ 5 ] = { 0, 0, 0, 0, 0 } ;

	int iIdx = 0 ;

	if ( m_dwFastLoginSlot ==  -1 )
	{
		iIdx = m_pServerList->GetItemIndex() ;
		m_dwFastLoginSlot = iIdx + 1 ;
	}
	else
	{
		iIdx = m_dwFastLoginSlot - 1 ;
	}

	for ( int i = 0 ; i < 5 ; i ++ )
	{
		if ( ( m_UserInfo.CharID[ i ] == 0 ) && m_SortData[ iIdx ][ i ].dwCID )
		{
			dwCID[ i ] = m_SortData[ iIdx ][ i ].dwCID ;
		}
	}

	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	CRYLLoginDlg* pDlg = static_cast< CRYLLoginDlg* >( pScene->GetLoginDlg() ) ;
	char* lpszPassword = pDlg->m_pPassWordEdit->GetText() ;

	if ( false == ClientSocket::IsNSFlagChecked(CmdUnifiedCharSelect) )
	{
		SendPacket::UnifiedCharSelect( g_AuthSession, 
			lpszPassword, 
			dwCID, 
			USER_INFO::MAX_CHAR_NUM, 
			m_dwFastLoginSlot,
			0 ) ; 

		SaveFastLoginSlot() ;
	}
}

bool CRYLChinaUnifiedServerSelCharDlg::LoadFastLoginSlot() 
{
	CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance() ;
	HKEY hMPClientReg;
	char strReg[ MAX_PATH ] ;
	unsigned long dwSize = sizeof( unsigned long ) ;

	sprintf( strReg, "Software\\ROW\\MP-Client" ) ;//was ROW hayzohar

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_READ, &hMPClientReg))
	{
		if ( ERROR_SUCCESS == RegQueryValueEx(hMPClientReg, "FastLoginSlot(Ch)", 0, NULL, (LPBYTE)&m_dwFastLoginSlot, &dwSize ) )
		{
			RegCloseKey(hMPClientReg);	
		}
		else
		{
			RegCloseKey(hMPClientReg);	

			return false ;
		}
	}

	return true ;
}

void CRYLChinaUnifiedServerSelCharDlg::SaveFastLoginSlot() 
{
	CRYLNetworkData* lpNetwork = CRYLNetworkData::Instance() ;
	HKEY hMPClientReg;
	char strReg[ MAX_PATH ] ;

	sprintf( strReg, "Software\\ROW\\MP-Client" ) ;//was ROW hayzohar

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, strReg, 0, KEY_WRITE, &hMPClientReg ) == ERROR_SUCCESS )
	{
		RegSetValueEx( hMPClientReg, "FastLoginSlot(Ch)", 0, REG_DWORD, (LPBYTE)&m_dwFastLoginSlot, sizeof( unsigned long ) );
		RegCloseKey( hMPClientReg ) ;
	}
}