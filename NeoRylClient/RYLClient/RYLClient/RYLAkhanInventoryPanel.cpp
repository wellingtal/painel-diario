
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "..\\GUITooltipItem.h"
#include "..\\ItemInstance.h"

#include "WinInput.h"
#include "GMUITooltipBox.h"
#include "GMUICustomDlg.h"
#include "GMImageDib.h"

#include "SoundMgr.h"

#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"
#include "RYLRaceBase.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLCampShopDlg.h"
#include "RYLQuestOpeningDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLMiningAmountProcessDlg.h"
#include "RYLAkhanInventoryPanel.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "GMMemory.h"

////------

#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendEtc.h>

#include "..\\GUITooltipItem.h"
#include "..\\ItemInstance.h"

#include "WinInput.h"
#include "GMUITooltipBox.h"
#include "GMUICustomDlg.h"
#include "GMImageDib.h"

#include "SoundMgr.h"

#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"
#include "RYLRaceBase.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLCampShopDlg.h"
#include "RYLQuestOpeningDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLMiningAmountProcessDlg.h"
#include "RYLHumanInventoryPanel.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "GMMemory.h"

CRYLAkhanInventoryPanel*	g_pAkhanInventoryPanel = NULL ;

CRYLAkhanInventoryPanel::CRYLAkhanInventoryPanel( CGMUICustomDlg *pParentDlg ) 
:	CRYLInventoryPanel( pParentDlg )
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 375, /*442*/632, &pImageDIB ) ; // inventory mod
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 375, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_INVEN_AKHAN01_256x256 ) ) ;
	SetRect( &rt, 0, 0, 375, /*185*/375 ) ; // inventory mod
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_INVEN_HUMAN02_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
    AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;
	SetWidth( 375 ) ;
	SetHeight( 630 ) ; // inventory mod


	m_pTabButton		    = NULL ;
	m_dwBodyInven			= 0L ;

	m_dwRideFlag			= 0L ;
	
	m_pStreetStallButton	= NULL ;

			
	m_pSTRLabel				= NULL ;
	m_pDEXLabel				= NULL ;
	m_pCONLabel				= NULL ;
	m_pINTLabel				= NULL ;
	m_pWISLabel				= NULL ;
	m_pHPRECLabel			= NULL ;
	m_pMPRECLabel			= NULL ;
	m_pSPEEDLabel			= NULL ;
	m_pCOOLDOWNLabel		= NULL ;
	m_pFAMELabel			= NULL ;
	m_pMEDALSLabel			= NULL ;


	m_pGMUIParentDlg = pParentDlg ;
	g_pAkhanInventoryPanel = this ;
}

CRYLAkhanInventoryPanel::~CRYLAkhanInventoryPanel() 
{
	FinalCtrl() ;	
}


VOID	CRYLAkhanInventoryPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_INVEN_HUMAN03_256x256 ) ;
/*
	// 무기 I
	m_pWeaponButton[ 0 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pWeaponButton[ 0 ]->SetLeft( 18 ) ;
	m_pWeaponButton[ 0 ]->SetTop( 119 ) ;
	m_pWeaponButton[ 0 ]->SetInstance( RYLAKHANINVENTORYPANEL::WEAPON_CHECKBOX1 ) ;
	m_pWeaponButton[ 0 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 27, 0, 54, 21 ) ;
	m_pWeaponButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 0, 27, 21 ) ;
	m_pWeaponButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 0, 27, 21 ) ;
	m_pWeaponButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 27, 0, 54, 21 ) ;
	m_pWeaponButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pWeaponButton[ 0 ]->SetChecked( TRUE ) ;
	m_pWeaponButton[ 0 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1837 ], 1837, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pWeaponButton[ 0 ] ) ;

	// 무기 II
	m_pWeaponButton[ 1 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pWeaponButton[ 1 ]->SetLeft( 45 ) ;
	m_pWeaponButton[ 1 ]->SetTop( 119 ) ;
	m_pWeaponButton[ 1 ]->SetInstance( RYLAKHANINVENTORYPANEL::WEAPON_CHECKBOX2 ) ;
	m_pWeaponButton[ 1 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 26, 21, 52, 42 ) ;
	m_pWeaponButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 21, 26, 42 ) ;
	m_pWeaponButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 21, 26, 42 ) ;
	m_pWeaponButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 21, 52, 42 ) ;
	m_pWeaponButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pWeaponButton[ 1 ]->SetChecked( FALSE ) ;
	m_pWeaponButton[ 1 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1838 ], 1838, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pWeaponButton[ 1 ] ) ;
*/
	// 인벤 1
	m_pInvenButton[ 0 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pInvenButton[ 0 ]->SetLeft( 6 ) ;
	m_pInvenButton[ 0 ]->SetTop( 251 ) ;
	m_pInvenButton[ 0 ]->SetInstance( RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX1 ) ;
	m_pInvenButton[ 0 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 31, 63, 62, 102 ) ;
	m_pInvenButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 63, 31, 102 ) ;
	m_pInvenButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 63, 31, 102 ) ;
	m_pInvenButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 31, 63, 62, 102 ) ;
	m_pInvenButton[ 0 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pInvenButton[ 0 ]->SetChecked( TRUE ) ;
	m_pInvenButton[ 0 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1842 ], 1842, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pInvenButton[ 0 ] ) ;

	// 인벤 2
	m_pInvenButton[ 1 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pInvenButton[ 1 ]->SetLeft( 6 ) ;
	m_pInvenButton[ 1 ]->SetTop( 290 ) ;
	m_pInvenButton[ 1 ]->SetInstance( RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX2 ) ;
	m_pInvenButton[ 1 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 31, 102, 62, 141 ) ;
	m_pInvenButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 102, 31, 141 ) ;
	m_pInvenButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 102, 31, 141 ) ;
	m_pInvenButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 31, 102, 62, 141 ) ;
	m_pInvenButton[ 1 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pInvenButton[ 1 ]->SetChecked( FALSE ) ;
	m_pInvenButton[ 1 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1843 ], 1843, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pInvenButton[ 1 ] ) ;

	// 인벤 3
	m_pInvenButton[ 2 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pInvenButton[ 2 ]->SetLeft( 6 ) ;
	m_pInvenButton[ 2 ]->SetTop( 329 ) ;
	m_pInvenButton[ 2 ]->SetInstance( RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX3 ) ;
	m_pInvenButton[ 2 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 31, 141, 62, 180 ) ;
	m_pInvenButton[ 2 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 141, 31, 180 ) ;
	m_pInvenButton[ 2 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 141, 31, 180 ) ;
	m_pInvenButton[ 2 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 31, 141, 62, 180 ) ;
	m_pInvenButton[ 2 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pInvenButton[ 2 ]->SetChecked( FALSE ) ;
	m_pInvenButton[ 2 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1844 ], 1844, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pInvenButton[ 2 ] ) ;

	// 인벤 퀘스트
	m_pInvenButton[ 3 ] = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pInvenButton[ 3 ]->SetLeft( 6 ) ;
	m_pInvenButton[ 3 ]->SetTop( 368 ) ;
	m_pInvenButton[ 3 ]->SetInstance( RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX4 ) ;
	m_pInvenButton[ 3 ]->OnClickEvent = DlgClick ;
	SetRect( &rt, 31, 180, 62, 219 ) ;
	m_pInvenButton[ 3 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 180, 31, 219 ) ;
	m_pInvenButton[ 3 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 180, 31, 219 ) ;
	m_pInvenButton[ 3 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 31, 180, 62, 219 ) ;
	m_pInvenButton[ 3 ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pInvenButton[ 3 ]->SetChecked( FALSE ) ;
	m_pInvenButton[ 3 ]->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1845 ], 1845, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pInvenButton[ 3 ] ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	// 노점상 버튼
	m_pStreetStallButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pStreetStallButton->SetLeft( 159 ) ;
	m_pStreetStallButton->SetTop( 12 ) ;
	m_pStreetStallButton->SetCaption( CRYLStringTable::m_strString[ 2084 ] ) ;     
	m_pStreetStallButton->SetFontColor( 0xffffffff) ;
	m_pStreetStallButton->SetColor( 0xffffffff) ;
	m_pStreetStallButton->OnClickEvent = DlgClick ;
	m_pStreetStallButton->SetInstance( RYLAKHANINVENTORYPANEL::STREETSTALL_BUTTON ) ;
	m_pStreetStallButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pStreetStallButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pStreetStallButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pStreetStallButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pStreetStallButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1836 ], 1836, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pStreetStallButton ) ;

			// -------- BONUS POINTS BUTTON ----------

	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	// STR 포인트 증가,감소 버튼
	m_pStrPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pStrPointIncBtn->SetLeft( 337 );
	m_pStrPointIncBtn->SetTop( 36 );
	m_pStrPointIncBtn->OnClickEvent = DlgClick ;
	m_pStrPointIncBtn->SetInstance( RYLHUMANINVENTORYPANEL::STRPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	//m_pStrPointIncBtn->SetVisible( FALSE );
	m_pStrPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pStrPointIncBtn );

	// DEX 포인트 증가,감소 버튼
	m_pDexPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pDexPointIncBtn->SetLeft( 337 );
	m_pDexPointIncBtn->SetTop( 56 );
	m_pDexPointIncBtn->OnClickEvent = DlgClick ;
	m_pDexPointIncBtn->SetInstance( RYLHUMANINVENTORYPANEL::DEXPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	//m_pDexPointIncBtn->SetVisible( FALSE );
	m_pDexPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pDexPointIncBtn );

	// CON 포인트 증가,감소 버튼
	m_pConPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pConPointIncBtn->SetLeft( 337 );
	m_pConPointIncBtn->SetTop( 76 );
	m_pConPointIncBtn->OnClickEvent = DlgClick ;
	m_pConPointIncBtn->SetInstance( RYLHUMANINVENTORYPANEL::CONPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	//m_pConPointIncBtn->SetVisible( FALSE );
	m_pConPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pConPointIncBtn );

	// INT 포인트 증가,감소 버튼
	m_pIntPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pIntPointIncBtn->SetLeft( 337 );
	m_pIntPointIncBtn->SetTop( 96 );
	m_pIntPointIncBtn->OnClickEvent = DlgClick ;
	m_pIntPointIncBtn->SetInstance( RYLHUMANINVENTORYPANEL::INTPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	//m_pIntPointIncBtn->SetVisible( FALSE );
	m_pIntPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pIntPointIncBtn );

	// WIS 포인트 증가,감소 버튼
	m_pWisPointIncBtn = new CRYLButton( m_pGMUIParentDlg );
	m_pWisPointIncBtn->SetLeft( 337 );
	m_pWisPointIncBtn->SetTop( 116 );
	m_pWisPointIncBtn->OnClickEvent = DlgClick ;
	m_pWisPointIncBtn->SetInstance( RYLHUMANINVENTORYPANEL::WISPOINTINC_BUTTON );
	SetRect( &rt, 29, 81, 49, 101 );
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 29, 130, 49, 150);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 29, 106, 49, 126);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 29, 56, 49, 76);
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	//m_pWisPointIncBtn->SetVisible( FALSE );
	m_pWisPointIncBtn->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1806 ], 1806, TOOLTIP_PANEL );
	CGMUICustomPanel::AddChild( m_pWisPointIncBtn );

	// 현재 가진 돈
	m_pCurrentGoldLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCurrentGoldLabel->SetLeft( 90 ) ;
	m_pCurrentGoldLabel->SetTop( 600 ) ;
	m_pCurrentGoldLabel->SetFontColor( 0xffffffff ) ;
	m_pCurrentGoldLabel->SetAutoSize( TRUE ) ;
	SetRect( &rt, 65, /*419*/599, 193, /*430*/610 ) ;
	m_pCurrentGoldLabel->SetAlignRect( rt ) ;
	m_pCurrentGoldLabel->SetAlign( CENTER ) ;
	m_pCurrentGoldLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1846 ], 1846, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCurrentGoldLabel ) ;


	m_pSTRLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSTRLabel->SetLeft( 300 ); 
	m_pSTRLabel->SetTop( 39 );
	m_pSTRLabel->SetFontColor (0xff2ECCFA);
	m_pSTRLabel->SetAutoSize( TRUE ) ;	
	m_pSTRLabel->SetCaption( "STR" );
	m_pSTRLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1801 ], 1801, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pSTRLabel ) ;


	m_pDEXLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDEXLabel->SetLeft( 300 );
	m_pDEXLabel->SetTop( 59 );
	m_pDEXLabel->SetFontColor (0xff2ECCFA);
	m_pDEXLabel->SetAutoSize( TRUE ) ;
	m_pDEXLabel->SetCaption( "DEX" );
	m_pDEXLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1802 ], 1802, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pDEXLabel ) ;


	m_pCONLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCONLabel->SetLeft( 300 ); // was 136
	m_pCONLabel->SetTop( 79 );
	m_pCONLabel->SetFontColor (0xff2ECCFA);
	m_pCONLabel->SetAutoSize( TRUE ) ;	
	m_pCONLabel->SetCaption( "CON" );
	m_pCONLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1803 ], 1803, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pCONLabel ) ;


	m_pINTLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pINTLabel->SetLeft( 300 ); // was 136
	m_pINTLabel->SetTop( 99 );
	m_pINTLabel->SetFontColor (0xff2ECCFA);
	m_pINTLabel->SetAutoSize( TRUE ) ;
	m_pINTLabel->SetCaption( "INT" );
	m_pINTLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1804 ], 1804, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pINTLabel ) ;

	// Avalansa new stats add WIS
	m_pWISLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pWISLabel->SetLeft( 300 ); // was 136
	m_pWISLabel->SetTop( 119 );
	m_pWISLabel->SetFontColor (0xff2ECCFA);
	m_pWISLabel->SetAutoSize( TRUE ) ;
	m_pWISLabel->SetCaption( "WIS" );
	m_pWISLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1805 ], 1805, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pWISLabel ) ;

	// Avalansa new stats add KILLSTREAK
	m_pKillStreak = new CRYLLabel( m_pGMUIParentDlg );
	m_pKillStreak->SetLeft( 305 );
	m_pKillStreak->SetTop( 160 );
	m_pKillStreak->SetFontColor(0xffffffff);
	m_pKillStreak->SetAutoSize( TRUE );
	m_pKillStreak->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1861 ], 1861, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pKillStreak );

	// Avalansa new stats add FAME
	m_pFAMELabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pFAMELabel->SetLeft( 305 );
	m_pFAMELabel->SetTop( 180 );
	m_pFAMELabel->SetFontColor (0xffffffff);
	m_pFAMELabel->SetAutoSize( TRUE ) ;
	m_pFAMELabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1798 ], 1798, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pFAMELabel ) ;
/*
	// MAGIC POWER LABEL
	m_pMagicPowerLabel = new CRYLLabel( this ) ;
	m_pMagicPowerLabel->SetLeft( 254) ; // was 212
	m_pMagicPowerLabel->SetTop( 75 ) ; // was 26
	m_pMagicPowerLabel->SetFontColor (0xffffffff);
	m_pMagicPowerLabel->SetCaption( "" ) ;
	SetRect( &rt, 220, 75, 291, 89 ) ;
	m_pMagicPowerLabel->SetAlignRect( rt ) ;
	m_pMagicPowerLabel->SetAlign( CENTER ) ;
	m_pMagicPowerLabel->SetAutoSize( TRUE ) ;
	m_pMagicPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1816 ], 1816, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pMagicPowerLabel ) ;
*/
	// Avalansa new stats add MEDALS
	m_pMEDALSLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMEDALSLabel->SetLeft( 305 );
	m_pMEDALSLabel->SetTop( 200 );
	m_pMEDALSLabel->SetFontColor (0xffffffff);
	m_pMEDALSLabel->SetAutoSize( TRUE ) ;
	m_pMEDALSLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1799 ], 1799, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pMEDALSLabel ) ;

	// Avalansa new stats add DMG
	m_pATTLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pATTLabel->SetLeft( 305 );
	m_pATTLabel->SetTop( 220 );
	m_pATTLabel->SetFontColor (0xffffffff);
	m_pATTLabel->SetAutoSize( TRUE ) ;
	m_pATTLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3472 ], 3472, TOOLTIP_PANEL ) ;
	//m_pATTLabel->SetCaption( CRYLStringTable::m_strString[ 3472 ] );
	CGMUICustomPanel::AddChild( m_pATTLabel ) ;

	// Avalansa new stats add Defense
	m_pDEFLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDEFLabel->SetLeft( 305 );
	m_pDEFLabel->SetTop( 240 );
	m_pDEFLabel->SetFontColor (0xffffffff);
	m_pDEFLabel->SetAutoSize( TRUE ) ;
	m_pDEFLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3475 ], 3475, TOOLTIP_PANEL ) ;
	//m_pDEFLabel->SetCaption( CRYLStringTable::m_strString[ 3475 ] );
	CGMUICustomPanel::AddChild( m_pDEFLabel ) ;

	// Avalansa new stats add MAGIC POWER
	m_pMagicPowerLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMagicPowerLabel->SetLeft( 305 );
	m_pMagicPowerLabel->SetTop( 260 );
	m_pMagicPowerLabel->SetFontColor (0xff2ECCFA);
	m_pMagicPowerLabel->SetAutoSize( TRUE ) ;
	m_pMagicPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3478 ], 3478, TOOLTIP_PANEL ) ;
	//m_pMagicPowerLabel->SetCaption( CRYLStringTable::m_strString[ 3478 ] );
	CGMUICustomPanel::AddChild( m_pMagicPowerLabel ) ;

	// Avalansa new stats add MAGIC REZIST
	m_pMagicResistLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMagicResistLabel->SetLeft( 305 );
	m_pMagicResistLabel->SetTop( 280 );
	m_pMagicResistLabel->SetFontColor (0xff2ECCFA);
	m_pMagicResistLabel->SetAutoSize( TRUE ) ;
	m_pMagicResistLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3479 ], 3479, TOOLTIP_PANEL ) ;
	//m_pMagicResistLabel->SetCaption( CRYLStringTable::m_strString[ 3479 ] );
	CGMUICustomPanel::AddChild( m_pMagicResistLabel ) ;

	// Avalansa new stats add EVASION
	m_pEvasionLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pEvasionLabel->SetLeft( 305 );
	m_pEvasionLabel->SetTop( 300 );
	m_pEvasionLabel->SetFontColor (0xff2ECCFA);
	m_pEvasionLabel->SetAutoSize( TRUE ) ;
	m_pEvasionLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3476 ], 3476, TOOLTIP_PANEL ) ;
	//m_pEvasionLabel->SetCaption( CRYLStringTable::m_strString[ 3476 ] );
	CGMUICustomPanel::AddChild( m_pEvasionLabel ) ;

	// Avalansa new stats add ACCURACY
	m_pHitRationLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pHitRationLabel->SetLeft( 305 );
	m_pHitRationLabel->SetTop( 320 );
	m_pHitRationLabel->SetFontColor (0xff2ECCFA);
	m_pHitRationLabel->SetAutoSize( TRUE ) ;
	m_pHitRationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3473 ], 3473, TOOLTIP_PANEL ) ;
	//m_pHitRationLabel->SetCaption( CRYLStringTable::m_strString[ 3473 ] );
	CGMUICustomPanel::AddChild( m_pHitRationLabel ) ;

	// Avalansa new stats add CRITICAL
	m_pCriticalLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCriticalLabel->SetLeft( 305 );
	m_pCriticalLabel->SetTop( 340 );
	m_pCriticalLabel->SetFontColor (0xff2ECCFA);
	m_pCriticalLabel->SetAutoSize( TRUE ) ;
	m_pCriticalLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3474 ], 3474, TOOLTIP_PANEL ) ;
	//m_pCriticalLabel->SetCaption( CRYLStringTable::m_strString[ 3474 ] );
	CGMUICustomPanel::AddChild( m_pCriticalLabel ) ;

	// Avalansa new stats add BLOCK
	m_pBlockPowerLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pBlockPowerLabel->SetLeft( 305 );
	m_pBlockPowerLabel->SetTop( 360 );
	m_pBlockPowerLabel->SetFontColor (0xff2ECCFA);
	m_pBlockPowerLabel->SetAutoSize( TRUE ) ;
	m_pBlockPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 3477 ], 3477, TOOLTIP_PANEL ) ;
	//m_pBlockPowerLabel->SetCaption( CRYLStringTable::m_strString[ 3477 ] );
	CGMUICustomPanel::AddChild( m_pBlockPowerLabel ) ;

	// Avalansa new stats add HP-REC
	m_pHPRECLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pHPRECLabel->SetLeft( 305 ); 
	m_pHPRECLabel->SetTop( 380 );
	m_pHPRECLabel->SetFontColor (0xff2ECCFA);
	m_pHPRECLabel->SetAutoSize( TRUE ) ;
	m_pHPRECLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 4706 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pHPRECLabel ) ;

	// Avalansa new stats add MP-REC
	m_pMPRECLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMPRECLabel->SetLeft( 305 );
	m_pMPRECLabel->SetTop( 400 );
	m_pMPRECLabel->SetFontColor (0xff2ECCFA);
	m_pMPRECLabel->SetAutoSize( TRUE ) ;
	m_pMPRECLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 4706 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pMPRECLabel ) ;

	// Avalansa new stats add SPEED
	m_pSPEEDLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSPEEDLabel->SetLeft( 305 );
	m_pSPEEDLabel->SetTop( 420 );
	m_pSPEEDLabel->SetFontColor (0xff2ECCFA);
	m_pSPEEDLabel->SetAutoSize( TRUE ) ;
	m_pSPEEDLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 4706 ], 1861, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pSPEEDLabel ) ;

	// BONUS POINTS
	m_pBonusPointLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pBonusPointLabel->SetLeft( 300 ); 
	m_pBonusPointLabel->SetTop( 139 );
	m_pBonusPointLabel->SetFontColor (0xffffffff);
	m_pBonusPointLabel->SetAutoSize( TRUE ) ;
	m_pBonusPointLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1811 ], 1811, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pBonusPointLabel ) ;

	// NAME INFO
	m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel->SetLeft( 280 ); 
	m_pNameLabel->SetTop( 12 );
	m_pNameLabel->SetFontColor (0xffffffff);
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1792 ], 1792, TOOLTIP_PANEL ) ;	
	CGMUICustomPanel::AddChild( m_pNameLabel ) ;

}

VOID	CRYLAkhanInventoryPanel::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
//	GM_DELETE( m_pWeaponButton[ 0 ] ) ;
//	GM_DELETE( m_pWeaponButton[ 1 ] ) ;
	GM_DELETE( m_pInvenButton[ 0 ] ) ;
	GM_DELETE( m_pInvenButton[ 1 ] ) ;
	GM_DELETE( m_pInvenButton[ 2 ] ) ;
	GM_DELETE( m_pInvenButton[ 3 ] ) ;

	GM_DELETE( m_pStreetStallButton ) ;
	GM_DELETE( m_pCurrentGoldLabel ) ;
	
	GM_DELETE( m_pSTRLabel );
	GM_DELETE( m_pDEXLabel );
	GM_DELETE( m_pCONLabel );
	GM_DELETE( m_pINTLabel );
	GM_DELETE( m_pWISLabel );
	GM_DELETE( m_pHPRECLabel );
	GM_DELETE( m_pMPRECLabel );
	GM_DELETE( m_pSPEEDLabel );
	GM_DELETE( m_pKillStreak );
	GM_DELETE( m_pFAMELabel );
	GM_DELETE( m_pMEDALSLabel );
	GM_DELETE( m_pATTLabel );
	GM_DELETE( m_pDEFLabel );
	GM_DELETE( m_pMagicPowerLabel );
	GM_DELETE( m_pMagicResistLabel );

	GM_DELETE( m_pEvasionLabel );
	GM_DELETE( m_pHitRationLabel );
	GM_DELETE( m_pCriticalLabel );
	GM_DELETE( m_pBlockPowerLabel );

	GM_DELETE( m_pStrPointIncBtn );
    GM_DELETE( m_pDexPointIncBtn );
    GM_DELETE( m_pConPointIncBtn );
    GM_DELETE( m_pIntPointIncBtn );
    GM_DELETE( m_pWisPointIncBtn );

	GM_DELETE( m_pBonusPointLabel );
//  GM_DELETE( m_pBonusPointValueLabel );

	GM_DELETE( m_pNameLabel );

//	GM_DELETE( m_pExperienceLabel );
//	GM_DELETE( m_pExperienceBalueLabel );

//	GM_DELETE( m_pGuildLabel );
//  GM_DELETE( m_pClassLabel );
//  GM_DELETE( m_pLevelLabel );
//  GM_DELETE( m_pNationLabel );

}


HRESULT		CRYLAkhanInventoryPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	if ( !GetVisible() || g_MessageBoxManager.GetIsExistModal() ) 
	{
		return E_FAIL ;
	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();

	//By Avalansa
	static char strString[MAX_PATH];
	//By Avalansa

	CRYLGameData*	 pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

		// killstrike start
	bool found = false;
	static char First[50];
	switch ( pGame->m_csStatus.m_Killstreak )
	{
	case 0:
		sprintf( strString, "None" ); 
		sprintf( First, "" ); 
		found=true;
		break;

	case 5:
		sprintf( strString, "Dominating!( x5 )" ); 
		sprintf( First, "Dominating!" ); 
		found=true;
		break;

	case 8:
		sprintf( strString, "Godlike!( x8 )" ); 
		sprintf( First, "Godlike!" ); 
		found=true;
		break;

	case 12:
		sprintf( strString, "Rampage!( x12 )" ); 
		sprintf( First, "Rampage!" ); 
		found=true;
		break;

	case 18:
		sprintf( strString, "Unstopable!( x18 )" ); 
		sprintf( First, "Unstopable!" ); 
		found=true;
		break;

	case 24:
		sprintf( strString, "Holy Shit!( x24 )" ); 
		sprintf( First, "Holy Shit!" ); 
		found=true;
		break;

	case 30:
		sprintf( strString, "Wicked Sick!( x30 )" ); 
		sprintf( First, "Wicked Sick!" ); 
		found=true;
		break;

	case 40:
		sprintf( strString, "Combo Whore!( x40 )" ); 
		sprintf( First, "Combo Whore!" ); 
		found=true;
		break;

	default:
		break;
	}
	int ks = pGame->m_csStatus.m_Killstreak;
	if(!found)
	{
		sprintf( strString, "%s %2d Kills ",First,ks ); 
	}
	m_pKillStreak->SetCaption ( strString );
	//kill striker end

	unsigned long dwMousePosX = ptMousePos->x - ( m_pGMUIParentDlg->GetLeft() + 38 ) ;
	unsigned long dwMousePosY = ptMousePos->y - ( m_pGMUIParentDlg->GetTop() + 290 ) ;

	list<CItemInstance *>::iterator it;

	if (true == ProcessMessageBoxResult())
	{
		return S_OK;
	}

	if ( pGame->m_lpInterface->GetIsTrade() )
	{
		// 장비 수리
		if ( pGame->m_dwKindCursor == CURSOR_REPAIR && g_DeviceInput.GetIsLeftMouseDown() )
		{
			if(CRYLGameData::Instance()->m_bGuildSafeResult)
				return S_OK;

			// 장비창
			for (int i = 0; i < Item::EquipmentPos::MAX_EQUPMENT_POS; ++i)
			{
				if ( pGame->m_csStatus.m_lstEquipment[ i ] )
				{
					if ( pGame->m_csStatus.m_lstEquipment[ i ]->IsOverMouse() && 
						 pGame->m_csStatus.m_lstEquipment[ i ]->GetIsEnableRepair() &&
						 pGame->m_csStatus.m_lstEquipment[ i ]->GetNowDurability() < pGame->m_csStatus.m_lstEquipment[ i ]->GetMaxDurability() )
					{
						unsigned long dwRepairGold = pGame->m_csStatus.m_lstEquipment[i]->m_lpItemBase->GetRepairPrice();

						// 길드 요새 상점의 세율 적용 (요새 상점창이 떠 있을때만 적용됨)
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLCampShopDlg* pDlg = ( CRYLCampShopDlg * )pScene->GetCampShopDlg();
						unsigned long dwCampShopTaxGold = dwRepairGold * (pDlg->GetTax() / 100.0f);

						// 성에 속한 NPC 라면 수리 세율 적용
						unsigned long dwCastleRepairTaxGold = 0;
						NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
                        if ( pNpcNode && pNpcNode->m_bBelongToCastle )
						{
							CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
							if ( lpCastle )
							{
								dwCastleRepairTaxGold = static_cast<unsigned long>( dwRepairGold * ( lpCastle->GetTax(Castle::REPAIR_TAX) / 100.f ) );
							}
						}

						// 최종 가격
						dwRepairGold = dwRepairGold + dwCampShopTaxGold + dwCastleRepairTaxGold ;

						if ( pGame->m_csStatus.m_Info.Gold > dwRepairGold )
						{
							char strString[MAX_PATH];
							if (TRUE == pDlg->GetVisible())
							{
								sprintf( strString, CRYLStringTable::m_strString[ 2999 ], 
									pDlg->GetOwnerName(), CRYLStringTable::MakePrintGold( dwRepairGold ) ) ;
							}
							else
							{
								sprintf( strString, CRYLStringTable::m_strString[ 161 ], CRYLStringTable::MakePrintGold( dwRepairGold ) ) ;
							}

							CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
							lpMessageBox->Create( strString, MB_YES | MB_NO | MB_EXIT ) ;
							lpMessageBox->SetResult( &m_dwRepairResult ) ;
							m_lpRepairItem = pGame->m_csStatus.m_lstEquipment[ i ] ;
							m_dwRepairResult = 0 ;
						} 
						else
						{
							CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
							lpMessageBox->Create( CRYLStringTable::m_strString[ 162 ] ) ;
						}

						return S_OK ;
					}
				}
			}

			// 인벤토리
			for ( it = pGame->m_csStatus.m_lstInventory.begin(); it != pGame->m_csStatus.m_lstInventory.end(); ++it )
			{
				if ( ( *it )->m_lpItemBase->GetPos().GetZIndex() != m_dwBackInven )
				{
					continue ;
				}

				if ( ( *it )->GetIsEnableRepair() &&							// 장비이고
					( *it )->IsOverMouse() &&									// 마우스로 선택
					( *it )->GetNowDurability() < (*it)->GetMaxDurability())	// 내구도가 적을 때
				{
					unsigned long dwRepairGold = (*it)->m_lpItemBase->GetRepairPrice();

					// 길드 요새 상점의 세율 적용 (요새 상점창이 떠 있을때만 적용됨)
					CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLCampShopDlg* pDlg = ( CRYLCampShopDlg * )pScene->GetCampShopDlg();
					unsigned long dwCampShopTaxGold = dwRepairGold * (pDlg->GetTax() / 100.0f);

					// 성에 속한 NPC 라면 수리 세율 적용
					unsigned long dwCastleRepairTaxGold = 0;
					NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
					if ( pNpcNode && pNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
						if ( lpCastle )
						{
							dwCastleRepairTaxGold = static_cast<unsigned long>( dwRepairGold * ( lpCastle->GetTax(Castle::REPAIR_TAX) / 100.f ) );
						}
					}

					// 최종 가격
					dwRepairGold = dwRepairGold + dwCampShopTaxGold + dwCastleRepairTaxGold ;

					if ( pGame->m_csStatus.m_Info.Gold > dwRepairGold )
					{
						char strString[MAX_PATH];
						if (TRUE == pDlg->GetVisible())
						{
							sprintf( strString, CRYLStringTable::m_strString[ 2999 ], 
								pDlg->GetOwnerName(), CRYLStringTable::MakePrintGold( dwRepairGold ) ) ;
						}
						else
						{
							sprintf( strString, CRYLStringTable::m_strString[ 161 ], CRYLStringTable::MakePrintGold( dwRepairGold ) ) ;
						}

						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(strString, MB_YES | MB_NO | MB_EXIT);
						lpMessageBox->SetResult(&m_dwRepairResult);
						m_lpRepairItem = (*it);
						m_dwRepairResult = 0;
					} 
					else
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[162]);
					}

					return S_OK ;
				}
			}
		}
	} 
	else
	{
		if ( g_DeviceInput.GetIsRightMouseDown())
		{
            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if( pSelfCreature && !pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
			{
				for (it = pGame->m_csStatus.m_lstInventory.begin(); it != pGame->m_csStatus.m_lstInventory.end(); it++)
				{
					if ((*it)->m_lpItemBase->GetPos().GetZIndex() != m_dwBackInven)
					{
						continue ;
					}

					if ( ( *it )->IsOverMouse() )
					{
						UseItem(*it);
						return S_OK;
					}
				}
			}
		}
	}

	// 장비 및 인벤토리 아이템 선택
	if ( SelectItem() )  
		return S_OK ;

	if (g_DeviceInput.GetIsLeftMouseDown())
	{
		// 손에 있는 아이템 처리
		if ( pGame->m_lpPickItem &&
			pGame->m_lpPickItem->GetItemPos() != TakeType::TS_STALL &&
			pGame->m_lpPickItem->GetItemPos() != TakeType::TS_QSLOT &&
			pGame->m_lpPickItem->GetItemPos() != TakeType::TS_SSLOT)		
		{
			unsigned long dwInvenX = dwMousePosX / 26;
			unsigned long dwInvenY = dwMousePosY / 26;

			__int64 ddItem;

			if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 160, m_pGMUIParentDlg->GetTop() + 77, 
				m_pGMUIParentDlg->GetLeft() + 184, m_pGMUIParentDlg->GetTop() + 101))					
			{
				if (true == SetEquip(Item::EquipmentPos::RINGL))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 212, m_pGMUIParentDlg->GetTop() + 77, 
				m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 101))						
			{
				if (true == SetEquip(Item::EquipmentPos::RINGR))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 90, m_pGMUIParentDlg->GetTop() + 51, 
				m_pGMUIParentDlg->GetLeft() + 140, m_pGMUIParentDlg->GetTop() + 101))			
			{
				if (true == SetEquip(Item::EquipmentPos::HEAD))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 77, 
				m_pGMUIParentDlg->GetLeft() + 210, m_pGMUIParentDlg->GetTop() + 101))	
			{
				if (true == SetEquip(Item::EquipmentPos::NECKLACE))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 20, m_pGMUIParentDlg->GetTop() + 177, 
				m_pGMUIParentDlg->GetLeft() + 70, m_pGMUIParentDlg->GetTop() + 279))			
			{
				if (true == SetEquip(Item::EquipmentPos::WEAPON))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 90, m_pGMUIParentDlg->GetTop() + 114, 
				m_pGMUIParentDlg->GetLeft() + 166, m_pGMUIParentDlg->GetTop() + 216))		
			{
				if (true == SetEquip(Item::EquipmentPos::BODY))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 114, 
				m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 216))		
			{
				if (true == SetEquip(Item::EquipmentPos::SKILL_ARM))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 20, m_pGMUIParentDlg->GetTop() + 52, 
				m_pGMUIParentDlg->GetLeft() + 70, m_pGMUIParentDlg->GetTop() + 154))			
			{
				if (true == SetEquip(Item::EquipmentPos::PROTECT_ARM))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 229, 
				m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 279))		
			{
				if (true == SetEquip(Item::EquipmentPos::AVATA))
				{
					return S_OK;
				}
			} 
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 104, m_pGMUIParentDlg->GetTop() + 229, 
				m_pGMUIParentDlg->GetLeft() + 154, m_pGMUIParentDlg->GetTop() + 279))		
			{
				if (true == SetEquip(Item::EquipmentPos::PELVIS))
				{
					return S_OK;
				}
			} 
			// 인벤
		//	else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 38, m_pGMUIParentDlg->GetTop() + 290, m_pGMUIParentDlg->GetLeft() + 193, m_pGMUIParentDlg->GetTop() + 445))		
			else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 38, m_pGMUIParentDlg->GetTop() + 290, m_pGMUIParentDlg->GetLeft() + /*193*/245, m_pGMUIParentDlg->GetTop() + /*445*/627)) // inventory mod !!		
			{
				// 리네임 아이템이 설정중이면 아이템을 선택하지 못한다.
				if(pGame->m_lpRenameWarrantItem)
					return S_OK;

				ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_INVEN, dwInvenX, dwInvenY, m_dwBackInven);

				if (ddItem == ITEM_QUEST_TYPE)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3575 ]);
					return S_OK;
				}

				if (ddItem == ITEM_NOT_QUEST_TYPE)
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3576 ]);
					return S_OK;
				}

				if (ddItem != ITEM_FAILED)
				{
					Item::ItemPos pItemIndex;
					pItemIndex.SetPos(dwInvenX, dwInvenY, m_dwBackInven);
					pItemIndex.m_cPos = TakeType::TS_INVEN;

					if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
					{
						CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

						if (lpExistItem)
						{
							if (pGame->m_lpPickItem->GetIsEnableStack())
							{
								unsigned long dwDurability = lpExistItem->GetNowDurability() + pGame->m_lpPickItem->GetNowDurability();
								lpExistItem->SetNowDurability(dwDurability);
								pGame->m_csStatus.SetQuickSlot(lpExistItem);
								delete pGame->m_lpPickItem;
								pGame->SetPickItem(NULL);
							}
						} 
						else
						{
							pGame->m_csStatus.AddInventoryItem(pGame->m_lpPickItem);
							pGame->SetPickItem(NULL);
						}

						return S_OK ;
					}

					if (ddItem == ITEM_EMPTY)
					{
						if (pGame->m_lpPickItem->GetIsEnableStack())
						{
							CItemInstance *lpExistItem = pGame->m_csStatus.GetItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
							if (NULL != lpExistItem && lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID())
							{
								// 기존의 자리에 아이템이 남아있으면 아이템이 나눠진걸로 생각, 아이템을 스택한다.
								pNetwork->SendSplitItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
							} 
							else
							{
								pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
							}
						} 
						else
						{
							// 아이템 이동
							pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
						}
					} 
					else
					{
						if (true == ProcessItemOverlapEvent(ddItem, pItemIndex))
						{
							return S_OK ;
						}
					}

					return S_OK ;
				}
			}
		}
	}

	m_pCurrentGoldLabel->SetCaption( CRYLStringTable::MakePrintGold( pGame->m_csStatus.m_Info.Gold) ) ;

			//STR

		sprintf(strString, "%d", pGame->m_csStatus.GetSTR());
	m_pSTRLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetSTR() < pGame->m_csStatus.GetSTR() )
		m_pSTRLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetSTR() > pGame->m_csStatus.GetSTR() )
		m_pSTRLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pSTRLabel->SetFontColor( 0xffffffff ) ;

	//DEX
			sprintf(strString, "%d", pGame->m_csStatus.GetDEX());
	m_pDEXLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetDEX() < pGame->m_csStatus.GetDEX() )
		m_pDEXLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetDEX() > pGame->m_csStatus.GetDEX() )
		m_pDEXLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pDEXLabel->SetFontColor( 0xffffffff ) ;

	//CON
			sprintf(strString, "%d", pGame->m_csStatus.GetCON());
	m_pCONLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetCON() < pGame->m_csStatus.GetCON() )
		m_pCONLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetCON() > pGame->m_csStatus.GetCON() )
		m_pCONLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pCONLabel->SetFontColor( 0xffffffff ) ;

	//INT
			sprintf(strString, "%d", pGame->m_csStatus.GetINT());
	m_pINTLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetINT() < pGame->m_csStatus.GetINT() )
		m_pINTLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetINT() > pGame->m_csStatus.GetINT() )
		m_pINTLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pINTLabel->SetFontColor( 0xffffffff ) ;

	//WIS
			sprintf(strString, "%d", pGame->m_csStatus.GetWIS());
	m_pWISLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetWIS() < pGame->m_csStatus.GetWIS() )
		m_pWISLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetWIS() > pGame->m_csStatus.GetWIS() )
		m_pWISLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pWISLabel->SetFontColor( 0xffffffff ) ;

	//HP-REC
			sprintf(strString, "%d", pGame->m_csStatus.GetHPRegen());
	m_pHPRECLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetHPRegen() < pGame->m_csStatus.GetBaseHPRegen() )
		m_pHPRECLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetHPRegen() > pGame->m_csStatus.GetBaseHPRegen() )
		m_pHPRECLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pHPRECLabel->SetFontColor( 0xffffffff ) ;

	//MP-REC
			sprintf(strString, "%d", pGame->m_csStatus.GetMPRegen());
	m_pMPRECLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetMPRegen() < pGame->m_csStatus.GetBaseMPRegen() )
		m_pMPRECLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetMPRegen() > pGame->m_csStatus.GetBaseMPRegen() )
		m_pMPRECLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pMPRECLabel->SetFontColor( 0xffffffff ) ;

	//SPEED
			sprintf(strString, "%d", pGame->m_csStatus.GetSpeed());
	m_pSPEEDLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetSpeed() < pGame->m_csStatus.GetBaseSpeed() )
		m_pSPEEDLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetSpeed() > pGame->m_csStatus.GetBaseSpeed() )
		m_pSPEEDLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pSPEEDLabel->SetFontColor( 0xffffffff ) ;



	//FAME
			sprintf(strString, "%d", pGame->m_csStatus.GetFame());
	m_pFAMELabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetFame() < pGame->m_csStatus.GetFame() )
		m_pFAMELabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetFame() > pGame->m_csStatus.GetFame() )
		m_pFAMELabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pFAMELabel->SetFontColor( 0xffffffff ) ;

	//MEDALS
			sprintf(strString, "%d", pGame->m_csStatus.GetMileage());
	m_pMEDALSLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetMileage() < pGame->m_csStatus.GetMileage() )
		m_pMEDALSLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetMileage() > pGame->m_csStatus.GetMileage() )
		m_pMEDALSLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pMEDALSLabel->SetFontColor( 0xffffffff ) ;

	// DAMAGE

		sprintf(strString, "%d ~ %d", pGame->m_csStatus.GetMinDamage(), pGame->m_csStatus.GetMaxDamage());
	long lDamage = pGame->m_csStatus.GetMinDamage() + ( pGame->m_csStatus.GetMaxDamage() - pGame->m_csStatus.GetMinDamage()) / 2;
	long lRealDamage = pGame->m_csStatus.GetBaseMinDamage() + ( pGame->m_csStatus.GetBaseMaxDamage() - pGame->m_csStatus.GetBaseMinDamage()) / 2;
	    m_pATTLabel->SetCaption( strString ) ;

	if(lDamage < lRealDamage)
		m_pATTLabel->SetFontColor( 0xffff0000 ) ;
	else if(lDamage > lRealDamage)
		m_pATTLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pATTLabel->SetFontColor( 0xffffffff ) ;

	// DEFENSE

		sprintf(strString, "%d", pGame->m_csStatus.GetArmor());
	    m_pDEFLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetArmor() < pGame->m_csStatus.GetBaseArmor() )
		m_pDEFLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetArmor() > pGame->m_csStatus.GetBaseArmor() )
		m_pDEFLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pDEFLabel->SetFontColor( 0xffffffff ) ;

		// MAGIC POWER

		sprintf(strString, "%d", pGame->m_csStatus.GetMagicPower());
	    m_pMagicPowerLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetMagicPower() < pGame->m_csStatus.GetBaseMagicPower() )
		m_pMagicPowerLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetMagicPower() > pGame->m_csStatus.GetBaseMagicPower() )
		m_pMagicPowerLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pMagicPowerLabel->SetFontColor( 0xffffffff ) ;

		// MAGIC REZIST

		sprintf(strString, "%d", pGame->m_csStatus.GetMagicResist());
	    m_pMagicResistLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetMagicResist() < pGame->m_csStatus.GetBaseMagicResist() )
		m_pMagicResistLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetMagicResist() > pGame->m_csStatus.GetBaseMagicResist() )
		m_pMagicResistLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pMagicResistLabel->SetFontColor( 0xffffffff ) ;

		// EVASION

		sprintf(strString, "%d", pGame->m_csStatus.GetEvade());
	    m_pEvasionLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetEvade() < pGame->m_csStatus.GetBaseEvade() )
		m_pEvasionLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetEvade() > pGame->m_csStatus.GetBaseEvade() )
		m_pEvasionLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pEvasionLabel->SetFontColor( 0xffffffff ) ;

		// ACCURACY

		sprintf(strString, "%d", pGame->m_csStatus.GetHitRate());
	    m_pHitRationLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetHitRate() < pGame->m_csStatus.GetBaseHitRate() )
		m_pHitRationLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetHitRate() > pGame->m_csStatus.GetBaseHitRate() )
		m_pHitRationLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pHitRationLabel->SetFontColor( 0xffffffff ) ;

		// CRITICAL

		sprintf(strString, "%d", pGame->m_csStatus.GetCritical());
	    m_pCriticalLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetCritical() < pGame->m_csStatus.GetBaseCritical() )
		m_pCriticalLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetCritical() > pGame->m_csStatus.GetBaseCritical() )
		m_pCriticalLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pCriticalLabel->SetFontColor( 0xffffffff ) ;

		// BLOCK

		sprintf(strString, "%d", pGame->m_csStatus.GetBlock());
	    m_pBlockPowerLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetBlock() < pGame->m_csStatus.GetBaseBlock() )
		m_pBlockPowerLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetBlock() > pGame->m_csStatus.GetBaseBlock() )
		m_pBlockPowerLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pBlockPowerLabel->SetFontColor( 0xffffffff ) ;

	    // BONUS POINT

		sprintf( strString, "%d", pGame->m_csStatus.m_Info.IP ); // was m_csStatus.m_Info.IP
	    m_pBonusPointLabel->SetCaption( strString );

		// NAME INFO
		m_pNameLabel->SetCaption( pGame->m_csStatus.m_Info.Name ) ;

		// EXP INFO

	unsigned long dwExp, dwExpMax;
	char strExp[2], strExpMax[2];
	if(1000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000;
		strcpy(strExp, "K");
	} else if(1000000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000000;
		strcpy(strExp, "M");
	} else if(1000000000000 < pGame->m_csStatus.m_Info.Exp && pGame->m_csStatus.m_Info.Exp <= 1000000000000000)
	{
		dwExp = pGame->m_csStatus.m_Info.Exp / 1000000000;
		strcpy(strExp, "G");
	} else
	{
		dwExp = pGame->m_csStatus.m_Info.Exp;
		strcpy(strExp, "");
	}

	if(1000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000;
		strcpy(strExpMax, "K");
	} else if(1000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000;
		strcpy(strExpMax, "M");
	} else if(1000000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000000;
		strcpy(strExpMax, "G");
	} else
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax;
		strcpy(strExpMax, "");
	}
/*
// exp label
	sprintf(strString, "%8d%s / %8d%s(%4.2f%%)", dwExp, strExp, dwExpMax, strExpMax, (float)((double)pGame->m_csStatus.m_Info.Exp / (double)pGame->m_csStatus.m_llExpMax * 100.0f));

	// 경험점 값
	m_pExperienceBalueLabel->SetCaption( strString ) ;

	// CLASS 

	m_pClassLabel->SetCaption( pGame->GetClassName( pGame->m_csStatus.m_Info.Class ) ) ;
	
	// GUILD
	if ( 0 != pGame->m_csStatus.m_Info.GID )
	{
		CRYLGameScene*			pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*		pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel*		pPanel		= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
		LPGUILDLARGEINFOEX		lpGuildLargeInfo = pPanel->FindGuild( pGame->m_csStatus.m_Info.GID ) ;

		if ( lpGuildLargeInfo )	m_pGuildLabel->SetCaption( lpGuildLargeInfo->m_szName ) ;
	}

	// NATION
	if ( Creature::MERKADIA == pGame->m_cAccountNation )
	{
		m_pNationLabel->SetCaption( "MK" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2286 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::KARTERANT == pGame->m_cAccountNation )
	{
		m_pNationLabel->SetCaption( "KR" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2285 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::ALMIGHTY_PIRATE == pGame->m_cAccountNation )	// 신의 해적단
	{
		m_pNationLabel->SetCaption( "AP" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2287 ], 1812, TOOLTIP_PANEL ) ;
	}

	// LVL INFO

	sprintf( strString, "%d", pGame->m_csStatus.m_Info.Level ) ;
	m_pLevelLabel->SetCaption( strString ) ;
*/

	return S_OK ;
}

HRESULT		CRYLAkhanInventoryPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

BOOL	CRYLAkhanInventoryPanel::SelectItem()
{
	BOOL bPickItem = FALSE;
	CItemInstance* lpInstance = NULL;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;


	// 장비 아이템 선택
	for (int i = 0; i < Item::EquipmentPos::MAX_EQUPMENT_POS; i++)
	{
		if (pGame->m_csStatus.m_lstEquipment[i])
		{
			lpInstance = pGame->m_csStatus.m_lstEquipment[i];
			if (pGame->m_dwKindCursor == CURSOR_NORMAL && !pGame->m_lpPickItem && g_DeviceInput.GetIsLeftMouseDown() && lpInstance->IsOverMouse())
			{
				//--//	start..
				if (Item::EquipmentPos::RINGL == i || Item::EquipmentPos::RINGR == i ||	Item::EquipmentPos::NECKLACE == i ||
					Item::EquipmentPos::WEAPON_HAND1 == i || Item::EquipmentPos::WEAPON_HAND2 == i || Item::EquipmentPos::ARMOUR == i ||
					Item::EquipmentPos::SHIELD_HAND1 == i || Item::EquipmentPos::SHIELD_HAND2 == i)
				{
					short aryEquipmentAttr[Item::Attribute::MAX_ATTRIBUTE_NUM] = { 0, };
					Item::CEquipment* lpEquip = reinterpret_cast<Item::CEquipment*>(pGame->m_csStatus.m_lstEquipment[ i ]->m_lpItemBase);
					if (lpEquip)
					{
						lpEquip->GetAttribute(aryEquipmentAttr, Item::Attribute::MAX_ATTRIBUTE_NUM);
					}

					if (aryEquipmentAttr[Item::Attribute::SKILL_POINT] > 0)
					{
						unsigned short wDiff = pGame->m_csStatus.GetSkillPoint() - pGame->m_csStatus.m_Skill.wSkillNum;

						// 남아있는 스킬 포인트가 반지로 인해 증가한 수치보다 작을때
						if (wDiff < aryEquipmentAttr[Item::Attribute::SKILL_POINT])
						{
							char strMessage[MAX_PATH];
							unsigned short wDeleteSkillNum = aryEquipmentAttr[Item::Attribute::SKILL_POINT] - wDiff;
							sprintf(strMessage, CRYLStringTable::m_strString[ 122 ], wDeleteSkillNum);
							CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
							lpMessageBox->Create(strMessage);

							return FALSE;
						}
					}
				}

				// 5단계 스킬 각성 아이템
				if (Item::EquipmentPos::PROTECT_ARM == i)
				{
					Item::CEquipment* lpEquip = reinterpret_cast<Item::CEquipment*>(pGame->m_csStatus.m_lstEquipment[ i ]->m_lpItemBase);

					// 각성 스킬이 5단계 0스킬 보다 큰 경우 선택이 되지 않도록 막는다.
					switch(lpEquip->GetPrototypeID())
					{
						case 5351 : if(false == IsSelectableFifthItem(0x9302)) { return false; } break;
						case 5352 : if(false == IsSelectableFifthItem(0x9402)) { return false; } break;
						case 5353 : if(false == IsSelectableFifthItem(0x9502)) { return false; } break;
						case 5354 : if(false == IsSelectableFifthItem(0x9606)) { return false; } break;
						case 5355 : if(false == IsSelectableFifthItem(0x9603)) { return false; } break;
						case 5356 : if(false == IsSelectableFifthItem(0x9706)) { return false; } break;
						case 5357 : if(false == IsSelectableFifthItem(0x9804)) { return false; } break;
						default	 : break;
					}
				}

				pGame->SetPickItem(lpInstance);
				pGame->m_csStatus.m_lstEquipment[i] = NULL;
				pNetwork->SendCharShape();
				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
				bPickItem = TRUE;

				if (i == Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos ||
					i == Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos)
				{
					g_CharacterData.m_bAutoTargetAttack = false;
					g_CharacterData.m_dwAutoTargetID = 0xFFFFFFFF;
				}

				Item::ItemPos pItemIndex;
				pItemIndex.m_cPos = TakeType::TS_TEMP;
				pItemIndex.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
				pNetwork->SendTakeItem(lpInstance, &pItemIndex, lpInstance->GetNowDurability());
			}
		}
	}

	// 리네임 아이템이 설정중이면 아이템을 선택하지 못한다.
	if(pGame->m_lpRenameWarrantItem)
		return FALSE;

	list<CItemInstance *>::iterator it ;
	///// 인벤 아이템 선택
	for (it = pGame->m_csStatus.m_lstInventory.begin(); it != pGame->m_csStatus.m_lstInventory.end();)
	{
		if ( ( *it )->m_lpItemBase->GetPos().GetZIndex() != m_dwBackInven )
		{
			it++ ;
			continue ;
		}

		// edith 2008.01.25 아이템선택 인벤토리에 있는 아이템을 팔기위해서 처리하는 방법
		// 여기가 이이템을 왼클릭 했을때 아이템을 마우스에 붙이는 부분이다.
		// 여기서 특정 버튼이 눌렸을때 아이템을 바로 판매하게 만든다.
		
		// 아이템을 판매할때 다음과 같은 형식으로 진행된다.
		// 1. 일반 우클릭 : 현재선택한 아이템을 1개 판매하게 한다. 팝업창 출력
		// 2. Shift 우클릭 : 현재선택한 아이템을 전부 판매하게 한다. 팝업창 출력
		// 3. Ctrl 우클릭 : 현재선택한 아이템을 1개 판매한다. 팝업창 없이

		// 트레이드 모드일때 처리한다.
		if ( pGame->m_bRYLTradeMode && pGame->m_dwKindCursor == CURSOR_NORMAL && !pGame->m_lpPickItem && 
			!pGame->m_lpTradeItem &&
			g_DeviceInput.GetIsRightMouseDown() && (*it)->IsOverMouse() )
		{
			pGame->m_lpTradeItem = ( *it );
			pGame->m_bRYLAutoTrade = TRUE;
			return TRUE;
		}


		if ( pGame->m_dwKindCursor == CURSOR_NORMAL && !pGame->m_lpPickItem && 
			g_DeviceInput.GetIsLeftMouseDown() && (*it)->IsOverMouse() )
		{
			CItemInstance *lpStallItem = pGame->m_csStatus.GetStreetTradeItem((*it)->m_dwItemUID);
			if (lpStallItem)
			{
				TakeType pTakeType;
				pTakeType.m_srcPos.m_cPos	= TakeType::TS_STALL ;
				pTakeType.m_srcPos.m_cIndex = 0 ;
				pTakeType.m_dstPos			= lpStallItem->m_lpItemBase->GetPos() ;
				
				if ( lpStallItem->GetIsEnableStack() )
				{
					pTakeType.m_cNum = lpStallItem->GetNowDurability() ;
				} 
				else
				{
					pTakeType.m_cNum = 1 ;
				}
			
				pGame->m_bStallRegFlag = TRUE;
//				g_pClientSocket->SetStatusFlag( NS_STALLREGISTERITEM ) ;
				SendPacket::CharStallRegisterItem( g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
					pTakeType, 0, PktStRI::SC_CANCEL ) ;
			}

			if ( ( *it )->GetIsEnableStack() && g_DeviceInput.KeyHold(VK_SHIFT) && ( *it )->GetNowDurability() > 1 )
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 170 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
				lpMessageBox->SetResult( &m_dwSplitResult ) ;
				lpMessageBox->SetValue( &m_dwSplitValue ) ;

				m_lpSplitItem	= ( *it ) ;
				m_dwSplitResult = 0 ;
				m_dwSplitValue	= 0 ;

				char Temp[MAX_PATH]; 
				sprintf( Temp, "%s\\Sound\\Interface\\click.mnd", g_ClientMain.m_strClientPath ) ;
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				pSound->Play(); 

				++it;
			} 
            else if (!ClientSocket::IsNSFlagChainChecked(CmdCharUseItem))
			{
				pGame->SetPickItem( ( *it ) ) ;
				pGame->m_csStatus.CheckInvenField( CCharStatus::REMOVE_FIELD, pGame->m_lpPickItem ) ;
				it = pGame->m_csStatus.m_lstInventory.erase( it ) ;
				pGame->m_csStatus.SetQuickSlot( pGame->m_lpPickItem ) ;

				bPickItem = TRUE ;
			}
            else
            {
                ++it;
            }
		} 
		else
		{
			++it ;
		}
	}

	if ( bPickItem )
	{
		return TRUE ;
	}

	return FALSE;
}


HRESULT		CRYLAkhanInventoryPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	for (int i = 0; i < 15; i++)
	{
		if (pGame->m_csStatus.m_lstEquipment[i])
		{
			if(pGame->m_csStatus.m_lstEquipment[i]->IsZeroPos())
				continue;

			switch(i)
			{
				case Item::EquipmentPos::AVATA:
					pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
					if(m_dwRideFlag == 0)
						pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 229, m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 279, 0xFF282828, 0x80);
					break;

				case Item::EquipmentPos::WEAPON_HAND1:
				case Item::EquipmentPos::SHIELD_HAND1:
					if (pGame->m_csStatus.m_cWeaponPos == 0)
					{
						if (i == Item::EquipmentPos::WEAPON_HAND1 && pGame->m_csStatus.m_lstEquipment[i]->GetTwoHandItem())
						{
							if ((pGame->m_csStatus.m_lstEquipment[i]->GetItemType() == Item::ItemType::OPP_SYTHE) && 
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
							{
								pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
							}
							else
							{
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 20 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 177 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeY()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->Render(lpD3DDevice);

								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 186 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 114 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeY()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->Render(lpD3DDevice);

								pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 114, m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 216, 0xFFFF0000, 0x80);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 20 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 177 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->m_lpSprite->GetSizeY()) / 2);
							}
						} 
						else
						{
							pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
						}
					}
					break;
/*
				case Item::EquipmentPos::WEAPON_HAND2:
				case Item::EquipmentPos::SHIELD_HAND2:
					if (pGame->m_csStatus.m_cWeaponPos == 1)
					{
						if (i == Item::EquipmentPos::WEAPON_HAND2 && pGame->m_csStatus.m_lstEquipment[i]->GetTwoHandItem())
						{
							if ((pGame->m_csStatus.m_lstEquipment[i]->GetItemType() == Item::ItemType::OPP_SYTHE) && 
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND2])
							{
								pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
							} 
							else
							{
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 20 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 177 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeY()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->Render(lpD3DDevice);

								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 186 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 114 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeY()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->Render(lpD3DDevice);

								pGame->RenderRect(lpD3DDevice, m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 114, m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 216, 0xFFFF0000, 0x80);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutX =
									m_pGMUIParentDlg->GetLeft() + 20 + ((50 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeX()) / 2);
								pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_wPutY =
									m_pGMUIParentDlg->GetTop() + 177 + ((102 - pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND2]->m_lpSprite->GetSizeY()) / 2);
							}
						} 
						else
						{
							pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
						}
					}
					break;
*/
				default:
					pGame->m_csStatus.m_lstEquipment[i]->Render(lpD3DDevice);
					break;
			}
		}
	}

	list<CItemInstance *>::iterator it;
	for (it = pGame->m_csStatus.m_lstInventory.begin(); it != pGame->m_csStatus.m_lstInventory.end(); it++)
	{
		if (m_dwBackInven == (*it)->m_lpItemBase->GetPos().GetZIndex() && !(*it)->IsZeroPos()) 
			(*it)->Render(lpD3DDevice);
	}

	if (pGame->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
	{
		if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 160, m_pGMUIParentDlg->GetTop() + 77, 
			m_pGMUIParentDlg->GetLeft() + 184, m_pGMUIParentDlg->GetTop() + 101))				
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::RINGL, 160, 77, 184, 101);
		}
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 212, m_pGMUIParentDlg->GetTop() + 77, 
			m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 101))			
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::RINGR, 212, 77, 236, 101);
		}
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 90, m_pGMUIParentDlg->GetTop() + 51, 
			m_pGMUIParentDlg->GetLeft() + 140, m_pGMUIParentDlg->GetTop() + 101))			
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::HEAD, 90, 51, 140, 101);
		}
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 77, 
			m_pGMUIParentDlg->GetLeft() + 210, m_pGMUIParentDlg->GetTop() + 101))			
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::NECKLACE, 186, 77, 210, 101);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 20, m_pGMUIParentDlg->GetTop() + 177, 
			m_pGMUIParentDlg->GetLeft() + 70, m_pGMUIParentDlg->GetTop() + 279))			
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::WEAPON, 20, 177, 70, 279);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 90, m_pGMUIParentDlg->GetTop() + 114, 
			m_pGMUIParentDlg->GetLeft() + 166, m_pGMUIParentDlg->GetTop() + 216))		
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::BODY, 90, 114, 166, 216);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 114, 
			m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 216))		
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::SKILL_ARM, 186, 114, 236, 216);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 20, m_pGMUIParentDlg->GetTop() + 52, 
			m_pGMUIParentDlg->GetLeft() + 70, m_pGMUIParentDlg->GetTop() + 154))			
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::PROTECT_ARM, 20, 52, 70, 154);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 186, m_pGMUIParentDlg->GetTop() + 229, 
			m_pGMUIParentDlg->GetLeft() + 236, m_pGMUIParentDlg->GetTop() + 279))		
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::AVATA, 186, 229, 236, 279);
		} 
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 104, m_pGMUIParentDlg->GetTop() + 229, 
			m_pGMUIParentDlg->GetLeft() + 154, m_pGMUIParentDlg->GetTop() + 279))		
		{
			RenderRectBGColor(lpD3DDevice, Item::EquipmentPos::PELVIS, 104, 229, 154, 279);
		} 
		// 인벤
		//else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 38, m_pGMUIParentDlg->GetTop() + 290, m_pGMUIParentDlg->GetLeft() + 193, m_pGMUIParentDlg->GetTop() + 445))		
		else if (g_DeviceInput.InRect(m_pGMUIParentDlg->GetLeft() + 38, m_pGMUIParentDlg->GetTop() + 290, m_pGMUIParentDlg->GetLeft() + /*193*/245, m_pGMUIParentDlg->GetTop() + /*445*/627)) // inventory mod !!		
			{
			Item::ItemPos pItemIndex;
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			pItemIndex.SetPos((ptMousePos->x - (m_pGMUIParentDlg->GetLeft() + 38)) / 26, (ptMousePos->y - (m_pGMUIParentDlg->GetTop() + 290)) / 26, m_dwBackInven);

			unsigned char res;
			int pos;	
			
			__int64 ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_INVEN, pItemIndex.GetXIndex(), pItemIndex.GetYIndex(), m_dwBackInven);
			if (ddItem != ITEM_FAILED && ddItem != ITEM_QUEST_TYPE && ddItem != ITEM_NOT_QUEST_TYPE)
			{
				CItemInstance* lpExistItem = pGame->m_csStatus.GetInventoryItem(ddItem);
				CItemInstance* lpStallItem = NULL;

				if (lpExistItem)
				{
					lpStallItem = pGame->m_csStatus.GetStreetTradeItem(lpExistItem->m_dwItemUID);
				}

				if (!lpStallItem && pGame->m_csStatus.GetIsInstallSocket(lpExistItem, pGame->m_lpPickItem))
				{
					pGame->RenderRect(lpD3DDevice,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->GetItemInfo().m_DetailData.m_cXSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->GetItemInfo().m_DetailData.m_cYSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						0xFFFFFFFF, 0x80);
				} 
				// 아이템 합성			
				else if ( pGame->m_csStatus.GetIsChemicalitem(lpExistItem, pGame->m_lpPickItem, res ) )
				{
					pGame->RenderRect(lpD3DDevice,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->GetItemInfo().m_DetailData.m_cXSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->GetItemInfo().m_DetailData.m_cYSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						0xFFFFFFFF, 0x80);
				}
				else if( pGame->m_csStatus.GetIsRuneSocket( lpExistItem, pGame->m_lpPickItem, pos ) )
				{
					pGame->RenderRect(lpD3DDevice,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						m_pGMUIParentDlg->GetLeft() + 38 + lpExistItem->GetItemInfo().m_DetailData.m_cXSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + lpExistItem->GetItemInfo().m_DetailData.m_cYSize * 26 + lpExistItem->m_lpItemBase->GetPos().GetYIndex() * 26,
						0xFFFFFFFF, 0x80);
				}
				else
				{
					pGame->RenderRect(lpD3DDevice,
						m_pGMUIParentDlg->GetLeft() + 38 + pItemIndex.GetXIndex() * 26, m_pGMUIParentDlg->GetTop() + 290 + pItemIndex.GetYIndex() * 26,
						m_pGMUIParentDlg->GetLeft() + 38 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFF0000FF, 0x80);
				}
			} 
			else
			{
				if (pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize + pItemIndex.GetXIndex() <= 13 && // inventory mod !!
					pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize + pItemIndex.GetYIndex() <= 8) // inventory mod !!
				{
					pGame->RenderRect(lpD3DDevice,
						m_pGMUIParentDlg->GetLeft() + 38 + pItemIndex.GetXIndex() * 26, m_pGMUIParentDlg->GetTop() + 290 + pItemIndex.GetYIndex() * 26,
						m_pGMUIParentDlg->GetLeft() + 38 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						m_pGMUIParentDlg->GetTop() + 290 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFFFF0000, 0x80);
				}
			}
		}
	}
	return S_OK ;
}

VOID	CRYLAkhanInventoryPanel::InventorySelectWeapon( INT iIndex )
{
	return;

	// 아칸은 아직 지원하지 않는다.
	CRYLGameData*	 pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if( !pSelfCreature )
        return;

	switch( iIndex )
	{
		case 1 :
		{
			if (pGame->m_csStatus.m_cWeaponPos != 0 && !g_CharacterData.GetAttacking())
			{
				pGame->m_csStatus.m_cWeaponPos = 0;
                pSelfCreature->m_ulAttackCombo = 0;
				pNetwork->SendCharSwitchHand();
				pNetwork->SendCharShape();
				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			}
		}
		break ;

		case 2 :
		{
			if (pGame->m_csStatus.m_cWeaponPos != 1 && !g_CharacterData.GetAttacking())
			{
				pGame->m_csStatus.m_cWeaponPos = 1;
                pSelfCreature->m_ulAttackCombo = 0;
				pNetwork->SendCharSwitchHand();
				pNetwork->SendCharShape();
				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			}
		}
		break ;
	}	
}

VOID	CRYLAkhanInventoryPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pAkhanInventoryPanel )
		return ;

	CRYLCheckBox* pCheck = NULL ;

	switch( Sender->GetInstance() )
	{
		case RYLAKHANINVENTORYPANEL::WEAPON_CHECKBOX1 :
			g_pAkhanInventoryPanel->UpdateWeaponCheckButton( 0 ) ;
			g_pAkhanInventoryPanel->InventorySelectWeapon( 1 ) ;
			break ;

		case RYLAKHANINVENTORYPANEL::WEAPON_CHECKBOX2 :
			g_pAkhanInventoryPanel->UpdateWeaponCheckButton( 0 ) ; // original line
			g_pAkhanInventoryPanel->InventorySelectWeapon( 1 ) ;  // original line
//			g_pAkhanInventoryPanel->UpdateWeaponCheckButton( 1 ) ;  /////////////
//			g_pAkhanInventoryPanel->InventorySelectWeapon( 2 ) ;  /////////////////////////
			break ;

		case RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX1 :
			g_pAkhanInventoryPanel->UpdateInvenCheckButton( 0 ) ;
			g_pAkhanInventoryPanel->SetInvenIndex( 0 ) ;
			break ;

		case RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX2 :
			g_pAkhanInventoryPanel->UpdateInvenCheckButton( 1 ) ;
			g_pAkhanInventoryPanel->SetInvenIndex( 1 ) ;
			break ;

		case RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX3 :
			g_pAkhanInventoryPanel->UpdateInvenCheckButton( 2 ) ;
			g_pAkhanInventoryPanel->SetInvenIndex( 2 ) ;
			break ;

		// 퀘스트 탭
		case RYLAKHANINVENTORYPANEL::INVEN_CHECKBOX4 :
			g_pAkhanInventoryPanel->UpdateInvenCheckButton( 3 ) ;
			g_pAkhanInventoryPanel->SetInvenIndex( 3 ) ;
			break ;

		case RYLAKHANINVENTORYPANEL::STREETSTALL_BUTTON  :
		{
			if ( !CRYLGameData::Instance()->m_lpInterface->GetBusy() )
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				
				if ( pScene->GetVaultDlg()->GetVisible() || 
					 pScene->GetBlacksmithDlg()->GetVisible() ||
					 pScene->GetExchangeDlg()->GetVisible() || 
					 pScene->GetNewTradeDlg()->GetVisible() ||
					 pScene->GetStreetStallDlg()->GetVisible() )
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[ 2085 ], MB_CONFIRM | MB_EXIT ) ;
					return ;
				}

				CRYLNetworkData::SendStallLeave();
				pScene->GUIVisible( FALSE ) ;
				CRYLGameData::Instance()->m_lpInterface->InitStreetStall( CRYLNetworkData::Instance()->m_dwMyChrID, NULL ) ;
			}
		}
		break ;

			case RYLHUMANINVENTORYPANEL::STRPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_STR);
        }
		break ;

	case RYLHUMANINVENTORYPANEL::DEXPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_DEX);
        }
		break ;

	case RYLHUMANINVENTORYPANEL::CONPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_CON);
        }
		break ;

	case RYLHUMANINVENTORYPANEL::INTPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_INT);
        }
		break ;

	case RYLHUMANINVENTORYPANEL::WISPOINTINC_BUTTON:
        {
		    SendPacket::CharIncreasePoint(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::StateType_WIS);
        }
		break ;

	}
}

VOID	CRYLAkhanInventoryPanel::UpdateRideFlag( INT iIndex )
{
	m_dwRideFlag = iIndex;
}

VOID	CRYLAkhanInventoryPanel::UpdateWeaponCheckButton( INT iIndex )
{
	CRYLCheckBox* pCheck = NULL ;

	pCheck = g_pAkhanInventoryPanel->GetWeaponCheckButton( 0 ) ;
	pCheck->SetChecked( FALSE ) ;
	pCheck = g_pAkhanInventoryPanel->GetWeaponCheckButton( 1 ) ;
	pCheck->SetChecked( FALSE ) ;
//	pCheck = g_pAkhanInventoryPanel->GetShieldCheckButton( 0 ) ;  //////////////
//	pCheck->SetChecked( FALSE ) ;   //////////////
//	pCheck = g_pAkhanInventoryPanel->GetShieldCheckButton( 1 ) ;  //////////////////
//	pCheck->SetChecked( FALSE ) ;   ////////////////

	pCheck = g_pAkhanInventoryPanel->GetWeaponCheckButton( iIndex ) ;
	pCheck->SetChecked( TRUE ) ;
//	pCheck = g_pAkhanInventoryPanel->GetShieldCheckButton( iIndex ) ;   //////////////
//	pCheck->SetChecked( TRUE ) ;   ////////////////
}


VOID	CRYLAkhanInventoryPanel::UpdateInvenCheckButton( INT iIndex )
{
	CRYLCheckBox* pCheck = NULL ;

	pCheck = g_pAkhanInventoryPanel->GetInvenCheckButton( 0 ) ;
	pCheck->SetChecked( FALSE ) ;
	pCheck = g_pAkhanInventoryPanel->GetInvenCheckButton( 1 ) ;
	pCheck->SetChecked( FALSE ) ;
	pCheck = g_pAkhanInventoryPanel->GetInvenCheckButton( 2 ) ;
	pCheck->SetChecked( FALSE ) ;
	pCheck = g_pAkhanInventoryPanel->GetInvenCheckButton( 3 ) ;
	pCheck->SetChecked( FALSE ) ;

	pCheck = g_pAkhanInventoryPanel->GetInvenCheckButton( iIndex ) ;
	pCheck->SetChecked( TRUE ) ;
}


ULONG		CRYLAkhanInventoryPanel::GetAllRepairMoney()
{
	ULONG iMoney = 0 ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	for ( INT i = 0 ; i < 15 ; i ++ )
	{
		if ( pGame->m_csStatus.m_lstEquipment[ i ] )
		{
			if ( pGame->m_csStatus.m_lstEquipment[ i ]->GetIsEnableRepair() &&
				pGame->m_csStatus.m_lstEquipment[ i ]->GetNowDurability() < pGame->m_csStatus.m_lstEquipment[ i ]->GetMaxDurability() )
			{
				iMoney += pGame->m_csStatus.m_lstEquipment[i]->m_lpItemBase->GetRepairPrice() ;
			}
		}
	}

	return iMoney ;
}

VOID	CRYLAkhanInventoryPanel::UpdateInvenItemPos(VOID)
{

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	unsigned short wPosX[15] = { 90, 0, 0,  90, 20, 104, 186, 212, 160, 186, 0,  20, 0,	186, 0 };
	unsigned short wPosY[15] = { 51, 0, 0, 114, 52, 229,  77,  77,  77, 114, 0, 177, 0,	229, 0 };

	unsigned short wSizeX[15] = { 50, 0, 0, 76, 50, 50, 24, 24, 24, 50, 0, 50, 0, 50, 50 };
	unsigned short wSizeY[15] = { 50, 0, 0, 102, 102, 50, 24, 24, 24, 102, 0, 102, 0, 50, 50 };


	CItemInstance* lpInstance = NULL;
	// 장비 아이템 업데이트 및 선택
	for (int i = 0; i < Item::EquipmentPos::MAX_EQUPMENT_POS; i++)
	{
		if (pGame->m_csStatus.m_lstEquipment[i])
		{
			lpInstance = pGame->m_csStatus.m_lstEquipment[i];
			lpInstance->m_wPutX	= m_pGMUIParentDlg->GetLeft() + wPosX[i] + ((wSizeX[i] - lpInstance->m_lpSprite->GetSizeX()) / 2);
			lpInstance->m_wPutY	= m_pGMUIParentDlg->GetTop() + wPosY[i] + ((wSizeY[i] - lpInstance->m_lpSprite->GetSizeY()) / 2);
		}
	}

	list<CItemInstance *>::iterator it ;
	const Item::ItemInfo* lpItemInfo = NULL;
	///// 인벤 아이템 업데이트 및 선택
	for (it = pGame->m_csStatus.m_lstInventory.begin(); it != pGame->m_csStatus.m_lstInventory.end(); it++)
	{
		lpItemInfo = &(*it)->GetItemInfo();

		( *it )->m_wPutX = m_pGMUIParentDlg->GetLeft() + 38 + 26 * ( *it )->m_lpItemBase->GetPos().GetXIndex() +  
						   ( ( ( lpItemInfo->m_DetailData.m_cXSize * 26 - 1 ) - ( *it )->m_lpSprite->GetSizeX() ) / 2 ) ;
		( *it )->m_wPutY = m_pGMUIParentDlg->GetTop() + 290 + 26 * ( *it )->m_lpItemBase->GetPos().GetYIndex() +  
						   ( ( ( lpItemInfo->m_DetailData.m_cYSize * 26 - 1 ) - ( *it )->m_lpSprite->GetSizeY() ) / 2 ) ;
	}
}


// by avalansa
//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusInfoPanel::SetTipString()
VOID CRYLAkhanInventoryPanel::SetTipString()
{
    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
    if( !pSelfData )
        return;

    static string strTipString1;
    static string strTipString2;

    switch( pSelfData->m_usClass )
	{
		case 1:							// 파이터
            {
                strTipString1 = "STR + 1";
                strTipString2 = "CON + 1";
            }
			break;

		case 2:							// 로그
            {
                strTipString1 = "STR + 1";
                strTipString2 = "DEX + 1";
            }
			break;

		case 3:							// 메이지
            {
                strTipString1 = "INT + 1";
                strTipString2 = "DEX + 1";
            }
			break;

		case 4:							// 어콜라이트
            {
                strTipString1 = "WIS + 1";
                strTipString2 = "CON + 1";
            }
			break;

		case 5:							// 디펜더
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "STR + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 6:							// 워리어
            {
                strTipString1 = "STR + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 7:							// 어쌔신
            {
                strTipString1 = "DEX + 1";
                strTipString2 = "STR + 1";
            }
			break;

		case 8:							// 아쳐
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 1";
                    strTipString2 = "DEX + 2";
                }
                else
                {
                    strTipString1 = "STR + 2";
                    strTipString2 = "DEX+ 1";
                }
            }
			break;

		case 9:							// 소서러
            {
                strTipString1 = "INT + 2";
                strTipString2 = "DEX+ 1";
            }
			break;

		case 10:						// 인챈터
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "INT + 2";
                    strTipString2 = "DEX + 1";
                }
                else
                {
                    strTipString1 = "INT + 1";
                    strTipString2 = "DEX + 2";
                }
            }
			break;

		case 11:						// 프리스트
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "WIS + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "WIS + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 12:						// 클레릭
            {
                strTipString1 = "WIS + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 17:						// 컴배턴트
            {
                strTipString1 = "";
                strTipString2 = "CON + 1";
            }
			break;

		case 18:						// 오피세이터
            {
                strTipString1 = "";
                strTipString2 = "DEX + 1";
            }
			break;

		case 19:						// 템플러
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "STR + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "STR + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 20:						// 어태커
            {
                strTipString1 = "STR + 2";
                strTipString2 = "CON + 1";
            }
			break;

		case 21:						// 거너
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "DEX + 2";
                    strTipString2 = "CON + 1";
                }
                else
                {
                    strTipString1 = "DEX + 1";
                    strTipString2 = "CON + 2";
                }
            }
			break;

		case 22:						// 룬 오프
            {
                strTipString1 = "INT + 2";
                strTipString2 = "DEX + 1";
            }
			break;

		case 23:						// 라이프 오프
            {
                strTipString1 = "WIS + 2";
                strTipString2 = "DEX + 1";
            }
			break;

		case 24:						// 쉐도우 오프
            {
                if( pSelfData->m_cLevel % 2 )
                {                                             
                    strTipString1 = "DEX + 2";
                    strTipString2 = "STR + 1";
                }
                else
                {
                    strTipString1 = "DEX + 1";
                    strTipString2 = "STR+ 2";
                }
            }
			break;
	}

    m_pTipString2Label->SetCaption( const_cast<LPSTR>( strTipString1.c_str() ) );
    m_pTipString3Label->SetCaption( const_cast<LPSTR>( strTipString2.c_str() ) );
}



