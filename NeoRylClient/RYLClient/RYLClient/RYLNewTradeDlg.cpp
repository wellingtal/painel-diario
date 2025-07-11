
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "NPC/NPCList.h"
#include <Creature/Character/CharacterClass.h>
#include <Network/ClientSocket/Send/SendItem.h>

#include "..\\GUITooltipItem.h"
#include "..\\GUITextEdit.h"

#include "WinInput.h"
#include "RenderOption.h"
#include "BaseGraphicsLayer.h"
#include "BaseDataDefine.h"

#include "GMImageDib.h"
#include "GMUIObject.h"

#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLRaceBaseDefine.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLHumanInventoryFrameDlg.h"
#include "RYLHumanInventoryPanel.h"
#include "RYLAkhanInventoryPanel.h"
#include "RYLSkillPanel.h"
#include "RYLCampShopDlg.h"
#include "RYLChattingDlg.h"
#include "RYLNewTradeDlg.h"
#include "GMMemory.h"

CRYLNewTradeDlg*	g_pNewTradeDlg = NULL;

CRYLNewTradeDlg::CRYLNewTradeDlg() 
{

	m_dwTickCount = 0;

	m_pTabButton	= NULL;
    	
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;
	int iSiegePosY = 0 ;	// 성의 세율 표시로 인해 증가해야할 창의 Y 값
// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		iSiegePosY = 20 ;
//	}

	CRYLSpriteList::Instance()->CreateGMImage( 269, 15, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 0, 100, 15 );
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 0, 113, 15 );
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 0, 256, 15 );
	pImageDIB->DrawRect( &rt, 113, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	AttachCaptionTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

	CRYLSpriteList::Instance()->CreateGMImage( 269, 397	+ 27 + iSiegePosY, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 15, 100, 37 );
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 15, 113, 37 );
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 15, 256, 37 );
	pImageDIB->DrawRect( &rt, 113, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );

	for ( int i = 0; i < (74 + 6 + (iSiegePosY / 5)); i ++ )
	{
		SetRect( &rt, 0, 38, 100, 43 );
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
		SetRect( &rt, 100, 38, 113, 43 );
		pImageDIB->DrawRect( &rt, 100, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
		SetRect( &rt, 100, 38, 256, 43 );
		pImageDIB->DrawRect( &rt, 113, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	}

	SetRect( &rt, 0, 42, 100, 52 );
	pImageDIB->DrawRect( &rt, 0, 414 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 42, 113, 52 );
	pImageDIB->DrawRect( &rt, 100, 414 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	SetRect( &rt, 100, 42, 256, 52 );
	pImageDIB->DrawRect( &rt, 113, 414 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	
	for ( int ver = 0; ver < 12 ; ver ++ )
	{
		for ( int hor = 0; hor < 8 ; hor ++ )
		{
			pImageDIB->MakeFrameRect( 30 + ( hor * 26 ), 36 + (ver * 26 ) + iSiegePosY, 27, 28, 0xff6e6857, 0xc0000000,  1 );		
		}
	}

	pImageDIB->DrawFrameRect( 30,  36 + iSiegePosY, 209, 1, 0xffB99E6B );
	pImageDIB->DrawFrameRect( 30,  36 + iSiegePosY, 1, 313, 0xffB99E6B );
	pImageDIB->DrawFrameRect( 30, 349 + iSiegePosY, 209, 1, 0xffB99E6B );
	pImageDIB->DrawFrameRect( 237, 36 + iSiegePosY, 1, 313, 0xffB99E6B );

	pImageDIB->MakeFrameRect( 149, 370 - 15 + iSiegePosY, 68, 18, 0xff959595, 0xc0000000, 1 );			// 페이즈 카운트 박스

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	// 세율 박스
//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		pImageDIB->MakeFrameRect(  97, 45 - 15, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//		pImageDIB->MakeFrameRect( 201, 45 - 15, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//	}

	m_pImage1 = CRYLImageManager::Instance()->CreateRYLImage();
	m_pImage1->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage1 );
	
	AttachWindowTexture( ( CGMTexture*)m_pImage1 );
	GM_DELETE( pImageDIB );

    //------------------------------------------------------------------------------------------------------
	CRYLSpriteList::Instance()->CreateGMImage( 269, 397 + iSiegePosY, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 100, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 113, 37 ) ;
	pImageDIB->DrawRect( &rt, 100, 0 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 113, 0 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	for ( int i = 0 ; i < 74 + (iSiegePosY / 5) ; i ++ )
	{
		SetRect( &rt, 0, 38, 100, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 113, 43 ) ;
		pImageDIB->DrawRect( &rt, 100, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 113, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 100, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 387 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 113, 52 ) ;
	pImageDIB->DrawRect( &rt, 100, 387 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 113, 387 + iSiegePosY, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	for ( int ver = 0 ; ver < 12 ; ver ++ )
	{
		for ( int hor = 0 ; hor < 8 ; hor ++ )
		{
			pImageDIB->MakeFrameRect( 30 + ( hor * 26 ), 36 + (ver * 26 ) + iSiegePosY, 27, 28, 0xff6e6857, 0xc0000000,  1 ) ;		
		}
	}

	pImageDIB->DrawFrameRect( 30,  36 + iSiegePosY, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30,  36 + iSiegePosY, 1, 313, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30, 349 + iSiegePosY, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 237, 36 + iSiegePosY, 1, 313, 0xffB99E6B ) ;

	pImageDIB->MakeFrameRect( 135, 362 + iSiegePosY, 103, 16, 0xff959595, 0x80000000, 1 ) ;

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	// 세율 박스
//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		pImageDIB->MakeFrameRect(  97, 45 - 15, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//		pImageDIB->MakeFrameRect( 201, 45 - 15, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//	}

	m_pImage2 = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pImage2->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage2 ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 269 );
	SetHeight( 439 + iSiegePosY );

	SetTop( 135 ) ;
	SetLeft( g_ClientMain.m_iScreenWidth - (256+269) );

	m_ptStartPos.x = GetLeft();
	m_ptStartPos.y = GetTop();

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON );
                                       
    m_pOptionRefreshButton      = NULL;
	m_pRepairButton				= NULL;
	m_pAllRepairButton			= NULL;
	m_pServiceMedalLabel		= NULL;	
	m_pServiceMedalValueLabel	= NULL;	
	m_pPrevButton				= NULL;							// 페이즈 이동 버튼
	m_pNextButton				= NULL;							// 페이즈 이동 버튼
	m_pPageLabel				= NULL;

	// 성이 들어갈 경우 성의 세율 표시
	m_pTradeTax					= NULL ;	// 거래 세율
	m_pRepairTax				= NULL ;	// 수리 세율
	m_pTradeTaxValue			= NULL ;	// 거래 세율 값
	m_pRepairTaxValue			= NULL ;	// 수리 세율 값

#ifdef __NEWNPCSCRIPT
	m_lpContainer = NULL;
	m_lpTradeWindow = NULL;
#endif

	m_dwAllRepairResult		    = 0;
	m_dwBuyResult			    = 0;
	m_dwSellResult			    = 0;
	m_dwAutoSellResult			= 0;
	m_dwKindTitle			    = 0;
	m_dwTradeTabOrder			= 0;
	m_lpBuyItem				    = NULL;
	m_dwItemNumber			    = 0;

	m_bBlackMarket			    = FALSE;
	m_bMedalMarket			    = FALSE;
	m_bSkillCouponMarket	    = FALSE;
	m_bRandomOptionMarket	    = FALSE;

	m_dwNPCID				    = 0;

    m_ucCurTradeDlgIndex        = 0;

#ifndef __NEWNPCSCRIPT
	// 마법사 트레이너
	strcpy( m_TabTitle[ 0 ][ 0 ], CRYLStringTable::m_strString[ 2095 ] );
	strcpy( m_TabTitle[ 0 ][ 1 ], CRYLStringTable::m_strString[ 2096 ] );
	strcpy( m_TabTitle[ 0 ][ 2 ], "" );
	strcpy( m_TabTitle[ 0 ][ 3 ], "" );

	// 성직자 트레이너
	strcpy( m_TabTitle[ 1 ][ 0 ], CRYLStringTable::m_strString[ 2097 ] );
	strcpy( m_TabTitle[ 1 ][ 1 ], CRYLStringTable::m_strString[ 2098 ] );
	strcpy( m_TabTitle[ 1 ][ 2 ], "" );
	strcpy( m_TabTitle[ 1 ][ 3 ], "" );

	// 로그 트레이너
	strcpy( m_TabTitle[ 2 ][ 0 ], CRYLStringTable::m_strString[ 2099 ] );
	strcpy( m_TabTitle[ 2 ][ 1 ], CRYLStringTable::m_strString[ 2100 ] );
	strcpy( m_TabTitle[ 2 ][ 2 ], "" );
	strcpy( m_TabTitle[ 2 ][ 3 ], "" );

	// 전사 트레이너
	strcpy( m_TabTitle[ 3 ][ 0 ], CRYLStringTable::m_strString[ 2101 ] );
	strcpy( m_TabTitle[ 3 ][ 1 ], CRYLStringTable::m_strString[ 2102 ] );
	strcpy( m_TabTitle[ 3 ][ 2 ], "" );
	strcpy( m_TabTitle[ 3 ][ 3 ], "" );

	// 모험가 트레이너
	strcpy( m_TabTitle[ 4 ][ 0 ], CRYLStringTable::m_strString[ 2103 ] );
	strcpy( m_TabTitle[ 4 ][ 1 ], "" );
	strcpy( m_TabTitle[ 4 ][ 2 ], "" );
	strcpy( m_TabTitle[ 4 ][ 3 ], "" );

	// 잡화상점 
	strcpy( m_TabTitle[ 5 ][ 0 ], CRYLStringTable::m_strString[ 2104 ] );
	strcpy( m_TabTitle[ 5 ][ 1 ], CRYLStringTable::m_strString[ 2105 ] );
	strcpy( m_TabTitle[ 5 ][ 2 ], CRYLStringTable::m_strString[ 2106 ] );
	strcpy( m_TabTitle[ 5 ][ 3 ], "" );

	// 인간 방어구상
	strcpy( m_TabTitle[ 6 ][ 0 ], CRYLStringTable::m_strString[ 2107 ] );
	strcpy( m_TabTitle[ 6 ][ 1 ], CRYLStringTable::m_strString[ 2108 ] );
	strcpy( m_TabTitle[ 6 ][ 2 ], CRYLStringTable::m_strString[ 2109 ] );
	strcpy( m_TabTitle[ 6 ][ 3 ], CRYLStringTable::m_strString[ 2110 ] );

	// 인간 무기상
	strcpy( m_TabTitle[ 7 ][ 0 ], CRYLStringTable::m_strString[ 2111 ] );
	strcpy( m_TabTitle[ 7 ][ 1 ], CRYLStringTable::m_strString[ 2112 ] );
	strcpy( m_TabTitle[ 7 ][ 2 ], CRYLStringTable::m_strString[ 2113 ] );
	strcpy( m_TabTitle[ 7 ][ 3 ], CRYLStringTable::m_strString[ 2114 ] );

	// 메달 상점
	strcpy( m_TabTitle[ 8 ][ 0 ], CRYLStringTable::m_strString[ 2115 ] );
	strcpy( m_TabTitle[ 8 ][ 1 ], "" );
	strcpy( m_TabTitle[ 8 ][ 2 ], "" );
	strcpy( m_TabTitle[ 8 ][ 3 ], "" );

	// 아칸 무기상
	strcpy( m_TabTitle[ 9 ][ 0 ], CRYLStringTable::m_strString[ 2116 ] );		
	strcpy( m_TabTitle[ 9 ][ 1 ], CRYLStringTable::m_strString[ 2117 ] );	
	strcpy( m_TabTitle[ 9 ][ 2 ], CRYLStringTable::m_strString[ 2118 ] );
	strcpy( m_TabTitle[ 9 ][ 3 ], CRYLStringTable::m_strString[ 2119 ] );		

	// 아칸 방아구상
	strcpy( m_TabTitle[ 10 ][ 0 ], CRYLStringTable::m_strString[ 2127 ] );
	strcpy( m_TabTitle[ 10 ][ 1 ], CRYLStringTable::m_strString[ 2128 ] );
	strcpy( m_TabTitle[ 10 ][ 2 ], CRYLStringTable::m_strString[ 2129 ] );
	strcpy( m_TabTitle[ 10 ][ 3 ], CRYLStringTable::m_strString[ 2130 ] );

	// 비기너 트레이너
	strcpy( m_TabTitle[ 11 ][ 0 ], CRYLStringTable::m_strString[ 2120 ] );
	strcpy( m_TabTitle[ 11 ][ 1 ], "" );
	strcpy( m_TabTitle[ 11 ][ 2 ], "" );
	strcpy( m_TabTitle[ 11 ][ 3 ], "" );

	// 컴베턴트 트레이너
	strcpy( m_TabTitle[ 12 ][ 0 ], CRYLStringTable::m_strString[ 2121 ] );
	strcpy( m_TabTitle[ 12 ][ 1 ], CRYLStringTable::m_strString[ 2122 ] );
	strcpy( m_TabTitle[ 12 ][ 2 ], CRYLStringTable::m_strString[ 2123 ] );
	strcpy( m_TabTitle[ 12 ][ 3 ], "" );

	// 오피세이터 트레이너
	strcpy( m_TabTitle[ 13 ][ 0 ], CRYLStringTable::m_strString[ 2124 ] );
	strcpy( m_TabTitle[ 13 ][ 1 ], CRYLStringTable::m_strString[ 2125 ] );
	strcpy( m_TabTitle[ 13 ][ 2 ], CRYLStringTable::m_strString[ 2126 ] );
	strcpy( m_TabTitle[ 13 ][ 3 ], "" );

    // '판매 목록'
    strcpy( m_TabTitle[ 14 ][ 0 ], CRYLStringTable::m_strString[ 2104 ] );
	strcpy( m_TabTitle[ 14 ][ 1 ], "" );
	strcpy( m_TabTitle[ 14 ][ 2 ], "" );
	strcpy( m_TabTitle[ 14 ][ 3 ], "" );

	// 아칸 잡화상점 
	strcpy( m_TabTitle[ 15 ][ 0 ], CRYLStringTable::m_strString[ 2104 ] );
	strcpy( m_TabTitle[ 15 ][ 1 ], "" );
	strcpy( m_TabTitle[ 15 ][ 2 ], "" );
	strcpy( m_TabTitle[ 15 ][ 3 ], "" );

#endif

	g_pNewTradeDlg = this ;
	InitCtrl();
}

CRYLNewTradeDlg::~CRYLNewTradeDlg() 
{
	FinalCtrl();

	vector< CItemInstance * >::iterator it;
	CItemInstance *lpDelete ;

	for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); )
	{
		lpDelete = (*it);
		it = m_lstTradeItem.erase( it );
		GM_DELETE( lpDelete );
	}
	m_lstTradeItem.clear();
}

VOID	CRYLNewTradeDlg::InitValue()
{
	m_dwBuyResult	= 0;
	m_dwSellResult	= 0;
	m_dwAutoSellResult	= 0;
	m_dwTradeTabOrder	= 0;
	m_lpBuyItem		= NULL;
	m_dwItemNumber	= 0;
	m_dwAllRepairResult = 0;
#ifdef __NEWNPCSCRIPT
	m_lpContainer = NULL;
	m_lpTradeWindow = NULL;
#endif
}

VOID	CRYLNewTradeDlg::InitCtrl() 
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 );
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance();

	int iSiegePosY = 0 ;	// 성의 세율 표시로 인해 증가해야할 창의 Y 값

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		iSiegePosY = 20 ;
//	}

	m_pTabButton = new CRYLTabButton( this );
	m_pTabButton->SetLeft( 1 );
	m_pTabButton->SetTop( 15 );
	m_pTabButton->SetWidth( 252 );
	m_pTabButton->SetHeight( 31 );
	m_pTabButton->SetInstance( RYLNEWTRADEDLG::TAB_BUTTON );
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( "", 4 );
	m_pTabButton->AttachTabButton( "", 4 );
	m_pTabButton->AttachTabButton( "", 4 );
	m_pTabButton->AttachTabButton( "", 4 );
	AttachGMUIControl( m_pTabButton );

    m_pOptionRefreshButton = new CRYLButton( this );
	m_pOptionRefreshButton->SetLeft( 28 );
	m_pOptionRefreshButton->SetTop( 397 + iSiegePosY );
	m_pOptionRefreshButton->SetCaption( CRYLStringTable::m_strString[ 3163 ] );
	m_pOptionRefreshButton->SetFontColor( 0xffffffff);
	m_pOptionRefreshButton->SetColor( 0xffffffff);
	m_pOptionRefreshButton->OnClickEvent = DlgClick ;
    m_pOptionRefreshButton->SetInstance( RYLNEWTRADEDLG::OPTION_REFRESH );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pOptionRefreshButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 3164 ], 3164 );
	AttachGMUIControl( m_pOptionRefreshButton );

	m_pRepairButton = new CRYLButton( this );
	m_pRepairButton->SetLeft( 102 );
	m_pRepairButton->SetTop( 397 + iSiegePosY );
	m_pRepairButton->SetCaption( CRYLStringTable::m_strString[ 2087 ] );
	m_pRepairButton->SetFontColor( 0xffffffff);
	m_pRepairButton->SetColor( 0xffffffff);
	m_pRepairButton->OnClickEvent = DlgClick ;
	m_pRepairButton->SetInstance( RYLNEWTRADEDLG::REPAIR_BUTTON );
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1890 ], 1890 );
	AttachGMUIControl( m_pRepairButton );

	m_pAllRepairButton = new CRYLButton( this );
	m_pAllRepairButton->SetLeft( 176 );
	m_pAllRepairButton->SetTop( 397 + iSiegePosY );
	m_pAllRepairButton->SetCaption( CRYLStringTable::m_strString[ 2088 ] );
	m_pAllRepairButton->SetFontColor( 0xffffffff);
	m_pAllRepairButton->SetColor( 0xffffffff);
	m_pAllRepairButton->OnClickEvent = DlgClick ;
	m_pAllRepairButton->SetInstance( RYLNEWTRADEDLG::ALLREPAIR_BUTTON );
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	m_pAllRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1891 ], 1891 );
	AttachGMUIControl( m_pAllRepairButton );

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 );
	RECT rt ;

    /*m_pOptionRefreshButton = new CRYLButton( this );
	m_pOptionRefreshButton->SetLeft( 134 );
	m_pOptionRefreshButton->SetTop( 373 );
	m_pOptionRefreshButton->OnClickEvent = DlgClick ;
	m_pOptionRefreshButton->SetInstance( RYLNEWTRADEDLG::PREV_BUTTON );
	SetRect( &rt,  80, 163, 100, 183 );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 120, 163, 140, 183 );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 100, 163, 120, 183 );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt,  80, 163, 100, 183 );
	m_pOptionRefreshButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pOptionRefreshButton );*/

	m_pPrevButton = new CRYLButton( this );
	m_pPrevButton->SetLeft( 134 );
	m_pPrevButton->SetTop( 373 + iSiegePosY );
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLNEWTRADEDLG::PREV_BUTTON );
	SetRect( &rt, 217, 215, 230, 227 );
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 217, 243, 230, 255 );
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 217, 229, 230, 241 );
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 217, 215, 230, 227 );
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pPrevButton );

	m_pNextButton = new CRYLButton( this );
	m_pNextButton->SetLeft( 219 );
	m_pNextButton->SetTop( 373 + iSiegePosY );
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLNEWTRADEDLG::NEXT_BUTTON );
	SetRect( &rt, 232, 215, 245, 227 );
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	SetRect( &rt, 232, 243, 245, 255 );
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN );
	SetRect( &rt, 232, 229, 245, 241 );
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP );
	SetRect( &rt, 232, 215, 245, 227 );
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pNextButton );

	m_pPageLabel = new CRYLLabel( this );		
	m_pPageLabel->SetLeft( 158 );
	m_pPageLabel->SetTop( 374 + iSiegePosY );
	m_pPageLabel->SetFontColor (0xffffffff);
	m_pPageLabel->SetAutoSize( TRUE );
	m_pPageLabel->SetCaption( "" );
	AttachGMUIControl( m_pPageLabel );

	m_pServiceMedalLabel = new CRYLLabel( this );		
	m_pServiceMedalLabel->SetLeft( 35 );
	m_pServiceMedalLabel->SetTop( 379 + iSiegePosY );
	m_pServiceMedalLabel->SetFontColor (0xffffffff);
	m_pServiceMedalLabel->SetAutoSize( TRUE );
	m_pServiceMedalLabel->SetCaption( CRYLStringTable::m_strString[ 2150 ] );
	AttachGMUIControl( m_pServiceMedalLabel );

	m_pServiceMedalValueLabel = new CRYLLabel( this );
	m_pServiceMedalValueLabel->SetFontColor( 0xffffffff );
	SetRect( &rt, 135, 379 + iSiegePosY, 239, 395 + iSiegePosY );
	m_pServiceMedalValueLabel->SetAlignRect( rt );
	m_pServiceMedalValueLabel->SetAlign( CENTER );
	m_pServiceMedalValueLabel->SetCaption( "" );
	m_pServiceMedalValueLabel->SetAutoSize( TRUE );
	AttachGMUIControl( m_pServiceMedalValueLabel );

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		// 거래 세율
//		m_pTradeTax = new CRYLLabel( this ) ;
//		m_pTradeTax->SetLeft( 40 ) ;
//		m_pTradeTax->SetTop( 50 ) ;
//		m_pTradeTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pTradeTax->SetAutoSize( TRUE ) ;
//		m_pTradeTax->SetCaption( CRYLStringTable::m_strString[ 3869 ] ) ;
//		AttachGMUIControl( m_pTradeTax ) ;
//
//		// 수리 세율
//		m_pRepairTax = new CRYLLabel( this ) ;
//		m_pRepairTax->SetLeft( 144 ) ;
//		m_pRepairTax->SetTop( 50 ) ;
//		m_pRepairTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pRepairTax->SetAutoSize( TRUE ) ;
//		m_pRepairTax->SetCaption( CRYLStringTable::m_strString[ 3868 ] ) ;
//		AttachGMUIControl( m_pRepairTax ) ;
//
//		// 거래 세율 값
//		m_pTradeTaxValue = new CRYLLabel( this ) ;
//		SetRect( &rt, 99, 50, 134, 60 ) ;
//		m_pTradeTaxValue->SetAlignRect( rt ) ;
//		m_pTradeTaxValue->SetAlign( CENTER ) ;
//		m_pTradeTaxValue->SetCaption( "---" ) ;
//		m_pTradeTaxValue->SetFontColor( 0xFFFFFFFF ) ;
//		m_pTradeTaxValue->SetAutoSize( TRUE ) ;
//		AttachGMUIControl( m_pTradeTaxValue ) ;
//
//		// 수리 세율 값
//		m_pRepairTaxValue = new CRYLLabel( this ) ;
//		SetRect( &rt, 203, 50, 238, 60 ) ;
//		m_pRepairTaxValue->SetAlignRect( rt ) ;
//		m_pRepairTaxValue->SetAlign( CENTER ) ;
//		m_pRepairTaxValue->SetCaption( "---" ) ;
//		m_pRepairTaxValue->SetFontColor( 0xFFFFFFFF ) ;
//		m_pRepairTaxValue->SetAutoSize( TRUE ) ;
//		AttachGMUIControl( m_pRepairTaxValue ) ;
//	}
}

VOID	CRYLNewTradeDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton );
    GM_DELETE( m_pOptionRefreshButton );
	GM_DELETE( m_pRepairButton );					
	GM_DELETE( m_pAllRepairButton );			
	GM_DELETE( m_pServiceMedalLabel );			
	GM_DELETE( m_pServiceMedalValueLabel );	
	GM_DELETE( m_pPrevButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pNextButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pPageLabel );

	// 성이 들어갈 경우 성의 세율 표시
	GM_DELETE( m_pTradeTax ) ;							// 거래 세율
	GM_DELETE( m_pRepairTax ) ;							// 수리 세율
	GM_DELETE( m_pTradeTaxValue ) ;						// 거래 세율 값
	GM_DELETE( m_pRepairTaxValue ) ;					// 수리 세율 값
}

VOID	CRYLNewTradeDlg::DeleteTradeItem()
{
	vector<CItemInstance *>::iterator it ;
	CItemInstance *lpDelete ;
	
	for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); )
	{
		lpDelete = (*it);
		it = m_lstTradeItem.erase( it );
		GM_DELETE( lpDelete );
	}
	m_lstTradeItem.clear();
}

VOID	CRYLNewTradeDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
	pInventoryDlg->SetVisible( TRUE );
	pInventoryDlg->ShowInventory();
	
	m_pTabButton->SetFocusTabButton( 0 );

    m_pPageLabel->SetVisible( TRUE );
    m_pPrevButton->SetVisible( TRUE );
    m_pNextButton->SetVisible( TRUE );
	m_pRepairButton->SetVisible( FALSE );
	m_pAllRepairButton->SetVisible( FALSE );
	m_pServiceMedalLabel->SetVisible( FALSE );
	m_pServiceMedalValueLabel->SetVisible( FALSE );

	CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL ;
	CRYLGameData::Instance()->m_bRYLTradeMode = TRUE;
	CRYLGameData::Instance()->m_bRYLAutoTrade = FALSE;


	m_ptStartPos.x = GetLeft();
	m_ptStartPos.y = GetTop();
}

VOID	CRYLNewTradeDlg::Hide( CGMUIObject* Sender ) 
{
	m_bBlackMarket = FALSE;
	m_bMedalMarket = FALSE;
	m_bSkillCouponMarket = FALSE;

	m_pRepairButton->SetVisible( FALSE );
	m_pAllRepairButton->SetVisible( FALSE );

	CRYLGameData::Instance()->m_bRYLTradeMode = FALSE;
	CRYLGameData::Instance()->m_bRYLAutoTrade = FALSE;
	CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL ;

	vector<CItemInstance *>::iterator it ;
	CItemInstance* lpDelete ;
	
	for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); )
	{
		lpDelete = (*it);
		it = m_lstTradeItem.erase( it );
		GM_DELETE( lpDelete );
	}
	m_lstTradeItem.clear();

#ifdef __NEWNPCSCRIPT
	DestroyTradeWindow();
#endif

	m_ptStartPos.x = GetLeft();
	m_ptStartPos.y = GetTop();

	if ( m_pTradeTaxValue ) m_pTradeTaxValue->SetCaption( "---" ) ;
	if ( m_pRepairTaxValue ) m_pRepairTaxValue->SetCaption( "---" ) ;
}

VOID	CRYLNewTradeDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice();
	CRYLDialog::GMUIPaint( xx, yy );

	vector<CItemInstance*>::iterator it;
	for ( it = m_lstTradeItem.begin() ; it != m_lstTradeItem.end() ; ++it )
	{
		(*it)->m_wPutX = (*it)->m_wStartX - ( m_ptStartPos.x - GetLeft() );
		(*it)->m_wPutY = (*it)->m_wStartY - ( m_ptStartPos.y - GetTop() );
		
		if ( (*it)->m_lpItemBase->GetPos().GetZIndex() != ( m_dwTradeTabOrder + m_nCurPage[ m_dwTradeTabOrder ] * 4 ) )
			continue;
		
		(*it)->Render( lpD3DDevice );
	}

}

HRESULT		CRYLNewTradeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLNewTradeDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	
CRYLNewTradeDlg::SetBuyFlag( CItemInstance * pItemInstance )
{
    NPCNode* lpNpcNode = g_NPCList.GetNPCNode( m_dwNPCID );
#ifdef __NEWNPCSCRIPT
    SingleTradePage* pTradePage = 
		m_lpTradeWindow->m_vecTradeTab[ m_dwTradeTabOrder ]->m_vecTradePage[ m_nCurPage[ m_dwTradeTabOrder ] ];

    for ( vector<Item::CItem*>::iterator iterItem = pTradePage->m_vecEquipmentItem.begin() ;
         iterItem != pTradePage->m_vecEquipmentItem.end() ;
         ++iterItem )
    {
		if ( (*iterItem)->GetPrototypeID() == pItemInstance->m_lpItemBase->GetPrototypeID() )
		{
			// 상점창을 열 때마다 랜덤 옵션을 바꿔줍니다.
			Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>((*iterItem)->GetItemInfo().m_DetailData.m_cItemType);
			Item::EquipType::Grade eGrade = lpNpcNode->GetRandomGrade(eItemType);
            ChangeOption( (*iterItem), eGrade, lpNpcNode->GetBaseNum(eItemType, eGrade), pItemInstance );
            break;
		}
	}
#else
    SingleTradePage* pTradePage = 
		lpNpcNode->m_vecTradeWindow[ m_dwTradeWindowOrder ]->m_vecTradeTab[ m_dwTradeTabOrder ]->m_vecTradePage[ m_nCurPage[ m_dwTradeTabOrder ] ];

    for ( vector<Item::CItem*>::iterator iterItem = pTradePage->m_vecEquipmentItem.begin() ;
         iterItem != pTradePage->m_vecEquipmentItem.end() ;
         ++iterItem )
    {
		if ( (*iterItem)->GetPrototypeID() == pItemInstance->m_lpItemBase->GetPrototypeID() )
		{
			// 상점창을 열 때마다 랜덤 옵션을 바꿔줍니다.
			Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>((*iterItem)->GetItemInfo().m_DetailData.m_cItemType);
			Item::EquipType::Grade eGrade = lpNpcNode->GetRandomGrade(eItemType);
            ChangeOption( (*iterItem), eGrade, lpNpcNode->GetBaseNum(eItemType, eGrade), pItemInstance );
            break;
		}
	}
#endif
}

HRESULT		CRYLNewTradeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{   
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if ( g_MessageBoxManager.GetIsExistModal() )
	{
		return E_FAIL ;
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

	CRYLGameData* pGame			= CRYLGameData::Instance();
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance();

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	// 성에 속한 NPC 일 경우 세율 표시
//	if ( true == CRYLNetworkData::Instance()->UseContents( GameRYL::SIEGE ) )
//	{
//		NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
//		if ( pNpcNode && pNpcNode->m_bBelongToCastle )
//		{
//			CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
//			if ( lpCastle )
//			{
//				char szBuff[ MAX_PATH ] ;
//				sprintf( szBuff, "%d%%", lpCastle->GetTax( Castle::TRADE_TAX ) ) ;
//				m_pTradeTaxValue->SetCaption( szBuff ) ;
//				sprintf( szBuff, "%d%%", lpCastle->GetTax( Castle::REPAIR_TAX ) ) ;
//				m_pRepairTaxValue->SetCaption( szBuff ) ;
//			}
//		}
//	}

	// edith 2008.01.25 아이템 판매 추가작업
	if ( m_dwAutoSellResult & MB_YES )
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			pGame->m_iRYLAutoTradeNum = 0;
			pGame->m_lpTradeItem = NULL;
			m_dwAutoSellResult = 0;
			return S_OK;
		}

		// 판다. 패킷 전송
		if (NULL != pGame->m_lpTradeItem)
		{
			DWORD		dwNum ;
			
			dwNum = pGame->m_iRYLAutoTradeNum;

			Item::ItemPos	pPos ;
			pNetwork->SendTradeItem( PktTr::TRC_SELL, pGame->m_dwTradingNpcID, 0, 
				TakeType(pGame->m_lpTradeItem->m_lpItemBase->GetPos(), pPos, dwNum), Item::ItemPos(), NULL );
		}

		pGame->m_iRYLAutoTradeNum = 0;
		pGame->m_lpTradeItem = NULL;
		m_dwAutoSellResult = 0;
		return S_OK ;
	} 
	else if ( m_dwAutoSellResult & MB_NO || m_dwAutoSellResult & MB_EXIT )
	{
		m_dwAutoSellResult = 0;
		pGame->m_lpTradeItem = NULL;
		return S_OK ;
	}

	if ( m_dwSellResult & MB_YES )
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			m_dwSellResult = 0;
			return S_OK;
		}

		// 판다. 패킷 전송
		if (NULL != pGame->m_lpPickItem)
		{
			DWORD		dwNum ;
			
			dwNum = ( pGame->m_lpPickItem->GetIsEnableStack() ) ? pGame->m_lpPickItem->GetNowDurability() : 1 ;

			Item::ItemPos	pPos ;
			pNetwork->SendTradeItem( PktTr::TRC_SELL, pGame->m_dwTradingNpcID, 0, 
				TakeType(pGame->m_lpPickItem->m_lpItemBase->GetPos(), pPos, dwNum), Item::ItemPos(), NULL );
		}

		m_dwSellResult = 0;
		return S_OK ;
	} 
	else if ( m_dwSellResult & MB_NO || m_dwSellResult & MB_EXIT )
	{
		m_dwSellResult = 0;
		return S_OK ;
	}

	if ( m_dwBuyResult & MB_YES )
	{
		// 산다. 패킷 전송
		Item::ItemPos		pItemIndex ;
		FieldInstance		pTradeItem ;
		pTradeItem.m_dwInstanceID	= 0;
		pTradeItem.m_nOID			= 0;
		pTradeItem.m_dwTypeID		= m_lpBuyItem->GetProtoTypeID();
		
		if ( m_lpBuyItem->GetIsEnableStack() )
		{
			if ( m_dwItemNumber > m_lpBuyItem->GetMaxDurability() )
			{
				m_dwItemNumber = m_lpBuyItem->GetMaxDurability();
			}

			pTradeItem.m_cNum = m_dwItemNumber ;
		}
		else
		{
			pTradeItem.m_cNum = 1 ;
		}

		if ( pTradeItem.m_cNum )
		{
			BOOL bPass = FALSE;

			if ( m_bBlackMarket )
			{
				unsigned long dwBuyBlackPrice = m_lpBuyItem->m_lpItemBase->GetBuyBlackPrice();
				NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				if ( pGame->m_csStatus.m_Info.Gold < ( pTradeItem.m_cNum * dwBuyBlackPrice ) ) 
					bPass = TRUE ;
			} 
			else if ( m_bMedalMarket )
			{               
				if ( pGame->m_csStatus.m_Info.Mileage < ( pTradeItem.m_cNum * m_lpBuyItem->m_lpItemBase->GetBuyMedalPrice() ) ) 
					bPass = TRUE ;
			} 
			else if ( m_bSkillCouponMarket )
			{
				unsigned short wCouponID = 0;
				switch ( m_lpBuyItem->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount )
				{
					case 2:		wCouponID = Item::EtcItemID::SKILL_COUPON_3;	break;
					case 3:		wCouponID = Item::EtcItemID::SKILL_COUPON_4;	break;
				}

				CItemInstance* lpCoupon = pGame->m_csStatus.GetInventoryItemfromID( wCouponID );
				if ( NULL != lpCoupon && lpCoupon->GetNowDurability() < pTradeItem.m_cNum )
				{
					bPass = TRUE ;
				}
			}
			else
			{
				unsigned long dwBuyPrice = m_lpBuyItem->m_lpItemBase->GetBuyPrice();
				NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				if ( pGame->m_csStatus.m_Info.Gold < ( pTradeItem.m_cNum * dwBuyPrice ) ) 
					bPass = TRUE ;
			}

			if ( bPass )
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				
				if ( m_bMedalMarket )
				{
					lpMessageBox->Create( CRYLStringTable::m_strString[ 258 ] );
				} 
				else if ( m_bSkillCouponMarket )
				{       
                    lpMessageBox->Create( CRYLStringTable::m_strString[ 2793 ] );
				}
				else
				{
					lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] );
				}

				m_dwBuyResult	= 0;
				m_lpBuyItem		= NULL;

				return S_OK ;
			}

// 서버에서 빈 공간을 체크하도록 루틴 변경 (2005-04-28 by 로딘)		            
//			if ( pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) )
//			{
				Item::ItemPos	pPos ;
				// Minotaurs 아이템 구입시 0.5초의 딜레이

				DWORD dwTickCount = ::GetTickCount();
				if(dwTickCount - m_dwTickCount > 500)
				{
					m_dwTickCount = dwTickCount;

					if(CRYLGameData::Instance()->m_bGuildSafeResult)
					{
						m_dwItemNumber	= 0;
						m_dwBuyResult	= 0;
						m_lpBuyItem		= NULL;

						return S_OK;
					}

					if ( m_lpBuyItem->m_bMedalBuyItem )
					{
						pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_MILEAGE, pGame->m_dwTradingNpcID, 
												m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, pTradeItem.m_cNum), Item::ItemPos(), NULL );
					} 
					else if ( m_lpBuyItem->m_bSkillCouponBuyItem )
					{
						unsigned short wCouponID = 0;
						switch ( m_lpBuyItem->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount )
						{
							case 2:		wCouponID = Item::EtcItemID::SKILL_COUPON_3;	break;
							case 3:		wCouponID = Item::EtcItemID::SKILL_COUPON_4;	break;
						}

						CItemInstance* lpCoupon = pGame->m_csStatus.GetInventoryItemfromID( wCouponID );	
						if (NULL != lpCoupon)
						{
							pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_SKILL_COUPON, pGame->m_dwTradingNpcID, 
								m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, pTradeItem.m_cNum), 
								lpCoupon->m_lpItemBase->GetPos(), NULL );
						}
					}
					else
					{

						if ( IsRandomMarket() )
						{
							if ( true == m_lpBuyItem->GetIsEnableAddOption() )
							{
								pNetwork->SendTradeItem( PktTr::TRC_BUY_EQUIP, pGame->m_dwTradingNpcID, 
									m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, pTradeItem.m_cNum), 
									Item::ItemPos(), m_lpBuyItem->m_lpItemBase );
								SetBuyFlag( m_lpBuyItem );
							}
							else
							{
								pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_GOLD, pGame->m_dwTradingNpcID, 
									m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, pTradeItem.m_cNum), 
									Item::ItemPos(), NULL);
							}
						}
						else
						{
							pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_GOLD, pGame->m_dwTradingNpcID, 
								m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, pTradeItem.m_cNum), 
								Item::ItemPos(), NULL);
						}
					}
				}
//			}	
		}

		m_dwItemNumber	= 0;
		m_dwBuyResult	= 0;
		m_lpBuyItem		= NULL;

		return S_OK ;
	} 
	else if ( m_dwBuyResult & MB_NO || m_dwBuyResult & MB_EXIT )
	{
		m_dwItemNumber	= 0;
		m_dwBuyResult	= 0;
		m_lpBuyItem		= NULL;
		return S_OK ;
	}

	UpdateAllRepair( pGame );

	// edith 2008.01.25 오토판매 모드이고 아이템이 선택되어있으면 자동으로 판매하는 로직이다.
	if( pGame->m_bRYLTradeMode && pGame->m_bRYLAutoTrade && pGame->m_lpTradeItem )
	{
		pGame->m_bRYLAutoTrade = FALSE;

		if(CRYLGameData::Instance()->m_bGuildSafeResult)
			return S_OK;

		DWORD		dwNum = 1;

		if(g_DeviceInput.KeyHold( VK_CONTROL ))
		{
			// 1개판매 팝업창 출력 X

			if (false == pGame->m_lpTradeItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bSell)
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 135 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			}
			else if ( pGame->m_lpTradeItem->GetItemPos() == TakeType::TS_QSLOT )
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 257 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			} 
			else if ( pGame->m_lpTradeItem->m_bSkill )		// 스킬이라면...
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 2755 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			}
			else if(pGame->m_lpTradeItem)
			{
				// edith 2009.08.19 B그레이드이상의 장비템이면 팔때 물어본다.
				BOOL bAutoDrop;
				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(pGame->m_lpTradeItem->m_lpItemBase);
				if(lpEquip && 
					(lpEquip->GetItemGrade().m_eItemGrade <= Item::EquipType::B_GRADE 
					|| lpEquip->GetMaxSocketNum() > 0 )	)
					bAutoDrop = FALSE;
				else
					bAutoDrop = TRUE;

				if(bAutoDrop == FALSE)
				{
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->SetWordBreak(TRUE);
					lpMessageBox->Create(CRYLStringTable::m_strString[ 259 ], MB_YES | MB_NO | MB_EXIT);
					lpMessageBox->SetEnableEnter(FALSE);
					lpMessageBox->SetResult(&pGame->m_dwTradePickResult);
					pGame->m_dwTradePickResult = 0;
				}
				else 
				{
					Item::ItemPos	pPos ;
					pNetwork->SendTradeItem( PktTr::TRC_SELL, pGame->m_dwTradingNpcID, 0, 
						TakeType(pGame->m_lpTradeItem->m_lpItemBase->GetPos(), pPos, dwNum), Item::ItemPos(), NULL );
				}
			}
		}
		else
		{
			// 기본은 아이템 1개판매
			// 전부판매 팝업창 출력
			if(g_DeviceInput.KeyHold( VK_SHIFT ) )
				dwNum = ( pGame->m_lpTradeItem->GetIsEnableStack() ) ? pGame->m_lpTradeItem->GetNowDurability() : 1 ;

			if (false == pGame->m_lpTradeItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bSell)
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 135 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			}
			else if ( pGame->m_lpTradeItem->GetItemPos() == TakeType::TS_QSLOT )
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 257 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			} 
			else if ( pGame->m_lpTradeItem->m_bSkill )		// 스킬이라면...
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 2755 ], pNetwork->m_dwMyChrID, 129 );
				pGame->m_lpTradeItem = NULL;
			}
			else
			{
				CHAR	strMessage[ MAX_PATH ] ;
				unsigned long dwSellPrice = 0 ;

				if ( pGame->m_lpTradeItem->GetIsEnableStack() )
				{
					dwSellPrice = pGame->m_lpTradeItem->m_lpItemBase->GetSellPrice() * dwNum;
					sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], CRYLStringTable::MakePrintGold( dwSellPrice ) );
				} 
				else
				{
					dwSellPrice = pGame->m_lpTradeItem->m_lpItemBase->GetSellPrice() ;
					sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], CRYLStringTable::MakePrintGold( dwSellPrice ) );
				}

				m_dwAutoSellResult = 0;
				pGame->m_iRYLAutoTradeNum = dwNum;

				// 팔아서 들어오는 골드와 현재 소지금의 합이 최대 소지금을 넘어설 경우
				if ( pGame->m_csStatus.m_Info.Gold > ULONG_MAX - dwSellPrice )
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( CRYLStringTable::m_strString[ 217 ] ) ;
					pGame->m_lpTradeItem = NULL;
				}
				else
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT );
					lpMessageBox->SetResult( &m_dwAutoSellResult );
				}
			}

		}
		return S_OK;
	}


	if ( g_DeviceInput.InRect( GetLeft() + 30, GetTop() + 52, GetLeft() + 237, GetTop() + 364 ) && g_DeviceInput.GetIsLeftMouseUp() && pGame->m_lpPickItem )
	{
		if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bSell)
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 135 ], pNetwork->m_dwMyChrID, 129 );
		}
		else if ( pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT )
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 257 ], pNetwork->m_dwMyChrID, 129 );
		} 
		else if ( pGame->m_lpPickItem->m_bSkill )		// 스킬이라면...
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 2755 ], pNetwork->m_dwMyChrID, 129 );
		}
		else
		{
			CHAR	strMessage[ MAX_PATH ] ;
			unsigned long dwSellPrice = 0 ;

			if ( pGame->m_lpPickItem->GetIsEnableStack() )
			{
				dwSellPrice = pGame->m_lpPickItem->m_lpItemBase->GetSellPrice() * pGame->m_lpPickItem->GetNowDurability() ;
				sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], CRYLStringTable::MakePrintGold( dwSellPrice ) );
			} 
			else
			{
				dwSellPrice = pGame->m_lpPickItem->m_lpItemBase->GetSellPrice() ;
				sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], CRYLStringTable::MakePrintGold( dwSellPrice ) );
			}

			// 팔아서 들어오는 골드와 현재 소지금의 합이 최대 소지금을 넘어설 경우
			if ( pGame->m_csStatus.m_Info.Gold > ULONG_MAX - dwSellPrice )
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 217 ] ) ;
			}
			else
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT );
				lpMessageBox->SetResult( &m_dwSellResult );

				m_dwSellResult = 0;
			}
		}

		return S_OK ;
	}

	if ( NS_NOWAITING == g_pClientSocket->GetStatusFlag() )
	{
		vector<CItemInstance *>::iterator it;
		for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); it ++ )
		{
			if ( (*it)->m_lpItemBase->GetPos().GetZIndex() != ( m_dwTradeTabOrder + m_nCurPage[ m_dwTradeTabOrder ] * 4 ) )
				continue;

			if ( !pGame->m_lpPickItem && (g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsRightMouseDown()) && (*it)->IsOverMouse() )
			{
				BOOL bPass = FALSE;
				if ( m_bBlackMarket )
				{
					unsigned long dwBuyBlackPrice = (*it)->m_lpItemBase->GetBuyBlackPrice();
					NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
					if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
						if ( lpCastle )
						{
							dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
						}
					}

					if ( pGame->m_csStatus.m_Info.Gold >= dwBuyBlackPrice ) 
					{
						bPass = TRUE ;
					}
				} 
				else if ( m_bMedalMarket )
				{
					if ( pGame->m_csStatus.m_Info.Mileage >= (*it)->m_lpItemBase->GetBuyMedalPrice() ) 
					{
						bPass = TRUE ;
					}
				} 
				else if ( m_bSkillCouponMarket )
				{
					unsigned short wCouponID = 0;
					switch ( (*it)->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount )
					{
						case 2:		wCouponID = Item::EtcItemID::SKILL_COUPON_3;	break;
						case 3:		wCouponID = Item::EtcItemID::SKILL_COUPON_4;	break;
					}

					if ( NULL != pGame->m_csStatus.GetInventoryItemfromID( wCouponID ) )
					{
						bPass = TRUE ;
					}
				}
				else
				{
					unsigned long dwBuyPrice = (*it)->m_lpItemBase->GetBuyPrice();
					NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
					if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
						if ( lpCastle )
						{
							dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
						}
					}

					if ( pGame->m_csStatus.m_Info.Gold >= dwBuyPrice ) 
						bPass = TRUE ;
				}

				if ( bPass )
				{
					Item::ItemPos	pItemIndex ;
					FieldInstance	pTradeItem ;
					pTradeItem.m_dwInstanceID	= 0;
					pTradeItem.m_nOID			= 0;
					pTradeItem.m_dwTypeID		= (*it)->GetProtoTypeID();
					pTradeItem.m_cNum			= 1 ;
                    m_dwBuyResult				= 0;

// 서버에서 빈 공간을 체크하도록 루틴 변경 (2005-04-28 by 로딘)
//					if ( pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) )
//					{
						if ( g_DeviceInput.GetIsLeftMouseUp() )
						{
							CRYLMessageBox* lpMessageBox ;
							CHAR			strMessage[ MAX_PATH ] ;
							lpMessageBox = new CRYLMessageBox ;
							m_dwBuyResult	= 0;
							m_lpBuyItem		= (*it);
							m_dwItemNumber	= 0;

							if ( m_lpBuyItem->GetIsEnableStack() )
							{
								strcpy( strMessage, CRYLStringTable::m_strString[ 254 ] );
								lpMessageBox->Create( strMessage, MB_NUMBER | MB_YES | MB_NO | MB_EXIT );
								lpMessageBox->SetValue (&m_dwItemNumber ); 
							} 
							else
							{
								if ( m_bBlackMarket )
								{
									unsigned long dwBuyBlackPrice = (*it)->m_lpItemBase->GetBuyBlackPrice();
									NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
									if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
									{
										CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
										if ( lpCastle )
										{
											dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
										}
									}

									sprintf( strMessage, CRYLStringTable::m_strString[ 255 ], 
										CRYLStringTable::MakePrintGold(dwBuyBlackPrice) );
								} 
								else if ( m_bMedalMarket )
								{
									sprintf( strMessage, CRYLStringTable::m_strString[ 256 ], 
										CRYLStringTable::MakePrintGold((*it)->m_lpItemBase->GetBuyMedalPrice()) );
								} 
								else if ( m_bSkillCouponMarket )
								{
									sprintf( strMessage, CRYLStringTable::m_strString[ 3167 ], 
										(*it)->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount + 1 );
								}
								else
								{
									unsigned long dwBuyPrice = (*it)->m_lpItemBase->GetBuyPrice();
									NPCNode* lpNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
									if ( lpNpcNode && lpNpcNode->m_bBelongToCastle )
									{
										CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( lpNpcNode->m_dwTownOrNameID );
										if ( lpCastle )
										{
											dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
										}
									}

									sprintf(strMessage, CRYLStringTable::m_strString[ 255 ], 
										CRYLStringTable::MakePrintGold(dwBuyPrice) );
								}
								lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT );
							}
							lpMessageBox->SetResult( &m_dwBuyResult );
						} 
						else if ( g_DeviceInput.GetIsRightMouseDown() )
						{
							// edith 2008.01.25 일반 우클릭시 아이템을 구매해기 위한 값이다.
							m_dwBuyResult	= MB_YES ;
							m_lpBuyItem		= (*it);
							// 기본 1개를 구매하게 한다.
							m_dwItemNumber = 1;

							// edith 2008.01.25 아이템구매 SHIFT가 눌려있으면 개수만큼 구매한다.
							if (g_DeviceInput.KeyHold( VK_SHIFT ) && m_lpBuyItem->GetIsEnableStack() )
							{
								m_dwItemNumber = m_lpBuyItem->GetMaxDurability();
							}
						}
//					} 
//					else
//					{
//						CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
//						lpMessageBox->Create( CRYLStringTable::m_strString[ 13 ] );
//					}
				} 
				else
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
					if ( m_bMedalMarket )
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 258 ] );
					} 
					else if ( m_bSkillCouponMarket )
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 2793 ] );
					}
					else
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] );
					}
				}

				return S_OK ;
			}
		}
	}

	if ( g_DeviceInput.GetIsRightMouseDown() && pGame->m_dwKindCursor != CURSOR_NORMAL )
	{
		pGame->m_dwKindCursor = CURSOR_NORMAL;
	}

	// 단축키
	if( g_DeviceInput.KeyDown('R') )
	{
		g_pNewTradeDlg->RefreshRandomOption();
	}

	// 공헌 메달 값
	char strString[MAX_PATH];
	sprintf( strString, "%d", pGame->m_csStatus.m_Info.Mileage );
	m_pServiceMedalValueLabel->SetCaption( strString );

	return S_OK ;
}

VOID	CRYLNewTradeDlg::UpdateAllRepair( CRYLGameData* pGame ) 
{
	if(CRYLGameData::Instance()->m_bGuildSafeResult)
	{
		m_dwAllRepairResult = 0;
		return;
	}

	// 모두 수리
	if ( m_dwAllRepairResult & MB_YES )
	{
		SendPacket::CharRepairAllItem( g_GameSession, pGame->m_dwTradingNpcID );
		m_dwAllRepairResult = 0;
	}
	else if ( m_dwAllRepairResult & MB_NO || m_dwAllRepairResult & MB_EXIT )
	{
		m_dwAllRepairResult = 0;	
	}
}

VOID	CRYLNewTradeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pNewTradeDlg )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance();

	switch( Sender->GetInstance() )
	{
	case RYLNEWTRADEDLG::TAB_BUTTON :
		{
            LPTABBUTTONINFO lpTabInfo = g_pNewTradeDlg->GetTabButton()->GetTabButtonInfo( g_pNewTradeDlg->GetTabButton()->GetFocusTabButton() );			

            if ( strcmp( lpTabInfo->m_pTabButton->GetCaption(), "" ) )
            {
			    g_pNewTradeDlg->SetTradeTab( g_pNewTradeDlg->GetTabButton()->GetFocusTabButton() );
            }
            else
            {
                g_pNewTradeDlg->SetTradeTab( 0 );
                g_pNewTradeDlg->GetTabButton()->SetFocusTabButton( 0 );
            }
		}
		break ;
	case RYLNEWTRADEDLG::REPAIR_BUTTON :
		{
			if ( pGame->m_dwKindCursor != CURSOR_REPAIR)
			{
				if(CRYLGameData::Instance()->m_bGuildSafeResult)
				{
					pGame->m_dwKindCursor = CURSOR_NORMAL;
					return;
				}

				pGame->m_dwKindCursor = CURSOR_REPAIR;
			}
			else
				pGame->m_dwKindCursor = CURSOR_NORMAL;
		}
		break;
	case RYLNEWTRADEDLG::ALLREPAIR_BUTTON :
		{
			g_pNewTradeDlg->AllRepair();
		}
		break ;

    case RYLNEWTRADEDLG::OPTION_REFRESH:
        {
            g_pNewTradeDlg->RefreshRandomOption();
        }
        break;

	case RYLNEWTRADEDLG::NEXT_BUTTON :
		{
			g_pNewTradeDlg->NextPage();
		}
		break;
	case RYLNEWTRADEDLG::PREV_BUTTON :
		{
			g_pNewTradeDlg->PrevPage();
		}
		break;
	}
}

VOID	CRYLNewTradeDlg::NextPage()
{
    NPCNode* lpNpcNode = g_NPCList.GetNPCNode( m_dwNPCID );
#ifdef __NEWNPCSCRIPT
    SingleTradeTab* pTradeTab = m_lpTradeWindow->m_vecTradeTab[ m_dwTradeTabOrder ];

    m_nCurPage[ m_dwTradeTabOrder ] = ( m_nCurPage[ m_dwTradeTabOrder ] + 1 + 1 > static_cast<int>( pTradeTab->m_vecTradePage.size() ) ) ? 0 : m_nCurPage[ m_dwTradeTabOrder ] + 1;
	SetPageLabel();
#else
    SingleTradeTab* pTradeTab = lpNpcNode->m_vecTradeWindow[ m_dwTradeWindowOrder ]->m_vecTradeTab[ m_dwTradeTabOrder ];

    m_nCurPage[ m_dwTradeTabOrder ] = ( m_nCurPage[ m_dwTradeTabOrder ] + 1 + 1 > static_cast<int>( pTradeTab->m_vecTradePage.size() ) ) ? 0 : m_nCurPage[ m_dwTradeTabOrder ] + 1;
	SetPageLabel();
#endif
}

VOID	CRYLNewTradeDlg::PrevPage()
{
	NPCNode* lpNpcNode = g_NPCList.GetNPCNode( m_dwNPCID );
#ifdef __NEWNPCSCRIPT
    SingleTradeTab* pTradeTab = m_lpTradeWindow->m_vecTradeTab[ m_dwTradeTabOrder ];

    m_nCurPage[ m_dwTradeTabOrder ] = ( m_nCurPage[ m_dwTradeTabOrder ] - 1 < 0 ) ? pTradeTab->m_vecTradePage.size() - 1 : m_nCurPage[ m_dwTradeTabOrder ] - 1;
	SetPageLabel();
#else
    SingleTradeTab* pTradeTab = lpNpcNode->m_vecTradeWindow[ m_dwTradeWindowOrder ]->m_vecTradeTab[ m_dwTradeTabOrder ];

    m_nCurPage[ m_dwTradeTabOrder ] = ( m_nCurPage[ m_dwTradeTabOrder ] - 1 < 0 ) ? pTradeTab->m_vecTradePage.size() - 1 : m_nCurPage[ m_dwTradeTabOrder ] - 1;
	SetPageLabel();
#endif
}


VOID	CRYLNewTradeDlg::AllRepair() 
{
	CRYLGameData*		pGame	= CRYLGameData::Instance();
	CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );

	CRYLHumanInventoryFrameDlg*		pInventoryDlg	= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();		
	CRYLHumanInventoryPanel*		pHPanel			= ( CRYLHumanInventoryPanel* )pInventoryDlg->GetHumanInvenPanel();
	CRYLAkhanInventoryPanel*		pAPanel			= ( CRYLAkhanInventoryPanel* )pInventoryDlg->GetAkhanInvenPanel();

	ULONG uiMoney = 0;
	
	switch ( pGame->m_dwClientRace )
	{
		case CClass::RaceType::HUMAN:	uiMoney = pHPanel->GetAllRepairMoney();		break ;
		case CClass::RaceType::AKHAN:	uiMoney = pAPanel->GetAllRepairMoney();		break ;
	}
	
	if ( uiMoney == 0 )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 2151 ] );
		return ;
	}

	// 길드 요새 상점의 세율 적용 (요새 상점창이 떠 있을때만 적용됨)
	CRYLCampShopDlg* pDlg = ( CRYLCampShopDlg * )pScene->GetCampShopDlg();
	unsigned long dwCampShopTaxMoney = uiMoney * (pDlg->GetTax() / 100.0f);
	
	// 성에 속한 NPC 라면 수리 세율 적용
	unsigned long dwCastleRepairTaxMoney = 0;
	NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
	if ( pNpcNode && pNpcNode->m_bBelongToCastle )
	{
		CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
		if ( lpCastle )
		{
			dwCastleRepairTaxMoney = static_cast<unsigned long>( uiMoney * ( lpCastle->GetTax(Castle::REPAIR_TAX) / 100.f ) );
		}
	}

	// 최종 가격
	uiMoney = uiMoney + dwCampShopTaxMoney + dwCastleRepairTaxMoney ;

	if ( pGame->m_csStatus.m_Info.Gold > uiMoney )
	{
		char strString[MAX_PATH];
		if (TRUE == pDlg->GetVisible())
		{
			sprintf( strString, CRYLStringTable::m_strString[ 2999 ], 
				pDlg->GetOwnerName(), CRYLStringTable::MakePrintGold( uiMoney ) ) ;
		}
		else
		{
			sprintf( strString, CRYLStringTable::m_strString[ 161 ], CRYLStringTable::MakePrintGold( uiMoney ) ) ;
		}

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( strString, MB_YES | MB_NO | MB_EXIT );
		lpMessageBox->SetResult( &m_dwAllRepairResult );
		m_dwAllRepairResult = 0;
	} 
	else
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 162 ] );
	}
}

VOID	CRYLNewTradeDlg::ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, 
									  CItemInstance * pItemInstance)
{
	char Buffer[Item::MAX_ITEM_SIZE];
	Item::CEquipment *lpEquip ;
	unsigned int dwItemSize = Item::MAX_ITEM_SIZE;
	unsigned char ucX, ucY, ucZ;
	ucX = pItemInstance->m_lpItemBase->GetPos().GetXIndex();
	ucY = pItemInstance->m_lpItemBase->GetPos().GetYIndex();
	ucZ = pItemInstance->m_lpItemBase->GetPos().GetZIndex();
	

	lpEquip = Item::CEquipment::DowncastToEquipment( pBaseItem );
	lpEquip->InitializeAttribute();
	lpEquip->AddRandomOption( eGrade, cBaseNum, 0);
	lpEquip->SetNewEquip();
	lpEquip->SetNumOrDurability( lpEquip->GetMaxNumOrDurability() );
	pBaseItem->SerializeOut( Buffer, dwItemSize );
	pItemInstance->m_lpItemBase->SerializeIn( Buffer, dwItemSize );
	pItemInstance->m_lpItemBase->SetPos( ucX, ucY, ucZ );
}

#ifdef __NEWNPCSCRIPT
VOID	CRYLNewTradeDlg::AddTradeEquipItemWindow(void)
{   
// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	int iSiegePosY = 0 ;	// 성의 세율 표시로 인해 증가해야할 창의 Y 값
//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		iSiegePosY = 20 ;
//	}

    SingleTradeTab*     pTab =  NULL;   
    SingleTradePage*    pPage = NULL;

    for ( unsigned int unTab = 0 ; unTab < m_lpTradeWindow->m_vecTradeTab.size() ; ++unTab )
    {
        pTab = m_lpTradeWindow->m_vecTradeTab[ unTab ];

        for ( unsigned int unPage = 0 ; unPage < pTab->m_vecTradePage.size() ; ++unPage )
        {
            pPage = pTab->m_vecTradePage[ unPage ];

            for ( int i = 0 ; i < 12 ; ++i )
            {
                for ( int j = 0 ; j < 8 ; ++j )
                {
                    pPage->m_bSpaceStatus[ i ][ j ] = false;
                }
            }
        }
    }

    DWORD i, j, cx, cy;

    bool bAble = false;
    bool bBreak = false;

    for ( unsigned int unTab = 0 ; unTab < m_lpTradeWindow->m_vecTradeTab.size() ; ++unTab )
    {
        pTab = m_lpTradeWindow->m_vecTradeTab[ unTab ];

        for ( unsigned int unPage = 0 ; unPage < pTab->m_vecTradePage.size() ; ++unPage )
        {
            pPage = pTab->m_vecTradePage[ unPage ];

            for ( vector<Item::CItem*>::iterator iterItem = pPage->m_vecEquipmentItem.begin() ;
                 iterItem != pPage->m_vecEquipmentItem.end() ;
                 ++iterItem )
            {
    			bBreak = false;

				const Item::ItemInfo* lpProtoType = &( (*iterItem)->GetItemInfo() );
				if ( NULL == lpProtoType )
					continue;

				DWORD dwSizeX = lpProtoType->m_DetailData.m_cXSize ;
				DWORD dwSizeY = lpProtoType->m_DetailData.m_cYSize ;

				for ( j = 0; j <= 12 - dwSizeY ; j ++ )
				{
					for ( i = 0; i <= 8 - dwSizeX ; i ++ )
					{
						bAble = true;

						for ( cy = j ; cy < j + dwSizeY ; cy ++ )
						{
							for ( cx = i ; cx < i + dwSizeX ; cx ++ )
							{
                                if ( pPage->m_bSpaceStatus[ cy ][ cx ] )
								{
									bAble = false;
								}
							}
						}

						if ( bAble )
						{
							Item::ItemPos	pItemIndex ;
							CItemInstance*	lpItem ;
							lpItem = new CItemInstance ;
							lpItem->m_bTradeItem = TRUE ;

							if ( m_bMedalMarket ) 
							{
								lpItem->m_bMedalBuyItem = TRUE ;
							}
							else if ( m_bSkillCouponMarket )
							{
								lpItem->m_bSkillCouponBuyItem = TRUE ;
							}

							pItemIndex.SetZIndex( unTab + unPage * 4 );
							pItemIndex.m_cPos = 0;

							lpItem->SetItemInfofromItem(  *iterItem , pItemIndex );
							m_lstTradeItem.push_back( lpItem );

							lpItem->m_wPutX = GetLeft() + 30 + 26 * i + 
								( ( ( lpItem->GetItemInfo().m_DetailData.m_cXSize * 26 - 1 ) - lpItem->m_lpSprite->GetSizeX() ) / 2 );

							lpItem->m_wPutY = GetTop()  + 52 + 26 * j + iSiegePosY +
								( ( ( lpItem->GetItemInfo().m_DetailData.m_cYSize * 26 - 1) - lpItem->m_lpSprite->GetSizeY() ) / 2 );

							lpItem->m_wStartX = lpItem->m_wPutX ;
							lpItem->m_wStartY = lpItem->m_wPutY ;

							for ( cy = j ; cy < j + dwSizeY ; cy ++ )
							{
								for ( cx = i ; cx < i + dwSizeX ; cx ++ )
								{
                                    pPage->m_bSpaceStatus[ cy ][ cx ] = true;
								}
							}

							bBreak = true;
						}

						if ( bBreak )
							break;
					}

					if ( bBreak )
						break;
                }
            }
        }
    }
}
#else
VOID	CRYLNewTradeDlg::AddTradeEquipItemWindow( NPCNode* lpNpcNode, unsigned char ucTradeDlgIndex )
{   
// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	int iSiegePosY = 0 ;	// 성의 세율 표시로 인해 증가해야할 창의 Y 값
//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		iSiegePosY = 20 ;
//	}

	SingleTradeWindow*  pWindow = NULL;
    SingleTradeTab*     pTab =  NULL;   
    SingleTradePage*    pPage = NULL;

    pWindow = lpNpcNode->m_vecTradeWindow[ ucTradeDlgIndex ];

    for ( unsigned int unTab = 0 ; unTab < pWindow->m_vecTradeTab.size() ; ++unTab )
    {
        pTab = pWindow->m_vecTradeTab[ unTab ];

        for ( unsigned int unPage = 0 ; unPage < pTab->m_vecTradePage.size() ; ++unPage )
        {
            pPage = pTab->m_vecTradePage[ unPage ];

            for ( int i = 0 ; i < 10 ; ++i ) // was 12
            {
                for ( int j = 0 ; j < 8 ; ++j )
                {
                    pPage->m_bSpaceStatus[ i ][ j ] = false;
                }
            }
        }
    }

    DWORD i, j, cx, cy;

    bool bAble = false;
    bool bBreak = false;

    for ( unsigned int unTab = 0 ; unTab < pWindow->m_vecTradeTab.size() ; ++unTab )
    {
        pTab = pWindow->m_vecTradeTab[ unTab ];

        for ( unsigned int unPage = 0 ; unPage < pTab->m_vecTradePage.size() ; ++unPage )
        {
            pPage = pTab->m_vecTradePage[ unPage ];

            for ( vector<Item::CItem*>::iterator iterItem = pPage->m_vecEquipmentItem.begin() ;
                 iterItem != pPage->m_vecEquipmentItem.end() ;
                 ++iterItem )
            {
    			bBreak = false;

				const Item::ItemInfo* lpProtoType = &( (*iterItem)->GetItemInfo() );
				if ( NULL == lpProtoType )
					continue;

				DWORD dwSizeX = lpProtoType->m_DetailData.m_cXSize ;
				DWORD dwSizeY = lpProtoType->m_DetailData.m_cYSize ;

				for ( j = 0; j <= 12 - dwSizeY ; j ++ )
				{
					for ( i = 0; i <= 8 - dwSizeX ; i ++ )
					{
						bAble = true;

						for ( cy = j ; cy < j + dwSizeY ; cy ++ )
						{
							for ( cx = i ; cx < i + dwSizeX ; cx ++ )
							{
                                if ( pPage->m_bSpaceStatus[ cy ][ cx ] )
								{
									bAble = false;
								}
							}
						}

						if ( bAble )
						{
							Item::ItemPos	pItemIndex ;
							CItemInstance*	lpItem ;
							lpItem = new CItemInstance ;
							lpItem->m_bTradeItem = TRUE ;

							if ( m_bMedalMarket ) 
							{
								lpItem->m_bMedalBuyItem = TRUE ;
							}
							else if ( m_bSkillCouponMarket )
							{
								lpItem->m_bSkillCouponBuyItem = TRUE ;
							}

							pItemIndex.SetZIndex( unTab + unPage * 4 );
							pItemIndex.m_cPos = 0;

							lpItem->SetItemInfofromItem(  *iterItem , pItemIndex );
							m_lstTradeItem.push_back( lpItem );

							lpItem->m_wPutX = GetLeft() + 30 + 26 * i + 
								( ( ( lpItem->GetItemInfo().m_DetailData.m_cXSize * 26 - 1 ) - lpItem->m_lpSprite->GetSizeX() ) / 2 );

							lpItem->m_wPutY = GetTop()  + 52 + 26 * j + iSiegePosY +
								( ( ( lpItem->GetItemInfo().m_DetailData.m_cYSize * 26 - 1) - lpItem->m_lpSprite->GetSizeY() ) / 2 );

							lpItem->m_wStartX = lpItem->m_wPutX ;
							lpItem->m_wStartY = lpItem->m_wPutY ;

							for ( cy = j ; cy < j + dwSizeY ; cy ++ )
							{
								for ( cx = i ; cx < i + dwSizeX ; cx ++ )
								{
                                    pPage->m_bSpaceStatus[ cy ][ cx ] = true;
								}
							}

							bBreak = true;
						}

						if ( bBreak )
							break;
					}

					if ( bBreak )
						break;
                }
            }
        }
    }
}
#endif

VOID	CRYLNewTradeDlg::SetPageLabel()
{
    NPCNode* lpNpcNode = g_NPCList.GetNPCNode( m_dwNPCID );
#ifdef __NEWNPCSCRIPT
	SingleTradeTab* pTradeTab = m_lpTradeWindow->m_vecTradeTab[ m_dwTradeTabOrder ];

    char strString[ MAX_PATH ];
    sprintf( strString, "%3d / %3d", m_nCurPage[ m_dwTradeTabOrder ] + 1, pTradeTab->m_vecTradePage.size() );
    m_pPageLabel->SetCaption( strString );
#else
	SingleTradeTab* pTradeTab = lpNpcNode->m_vecTradeWindow[ m_dwTradeWindowOrder ]->m_vecTradeTab[ m_dwTradeTabOrder ];

    char strString[ MAX_PATH ];
    sprintf( strString, "%3d / %3d", m_nCurPage[ m_dwTradeTabOrder ] + 1, pTradeTab->m_vecTradePage.size() );
    m_pPageLabel->SetCaption( strString );
#endif
}

VOID	CRYLNewTradeDlg::SetTradeTab( INT iTab )
{ 
    LPTABBUTTONINFO lpTabInfo = m_pTabButton->GetTabButtonInfo( iTab );

    if ( strcmp( lpTabInfo->m_pTabButton->GetCaption(), "" ) )
    {
	    m_dwTradeTabOrder = iTab ; 
	    SetPageLabel();
	    if ( IsRandomMarket() )
	    {
    //		// 상점의 그레이드 정보를 요청합니다.
    //		SendPacket::CharEquipShopInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, CRYLGameData::Instance()->m_dwTradingNpcID, m_cRace, iTab );
	    }
    }
}

#ifdef __NEWNPCSCRIPT
#else
VOID	CRYLNewTradeDlg::ChangeTabTitle( unsigned short usPopupCapacity )
{

	LPTABBUTTONINFO		lpTabInfo = NULL;
	switch( usPopupCapacity )
	{
		// 무기 상점
		case POPUP_MEDALSHOP_WEAPON :
		case POPUP_WEAPONMERCHANT :
		case POPUP_DARKWEAPONMERCHANT:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 7 ][ i ], TRUE );
			}
		}
		break;

		// 방어구 상점
		case POPUP_MEDALSHOP_ARMOUR:
		case POPUP_ARMOURMERCHANT:
		case POPUP_DARKARMOURMERCHANT:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 6 ][ i ], TRUE );
			}
		}
		break;

		// 잡화 상점
		case POPUP_ETCMERCHANT:
		case POPUP_DARKETCMERCHANT:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 5 ][ i ], TRUE );
			}
		}
		break;

		case POPUP_A_ETCMERCHANT:
		case POPUP_A_DARKETCMERCHANT :
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 15 ][ i ], TRUE );
			}
		}
		break;

		// 모험가 트레이너
		case POPUP_ADVENTURETRAINER:
		case POPUP_ADVANCEDADVENTURE:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 4 ][ i ], TRUE );
			}
		}
		break;

		// 전사 트레이너
		case POPUP_FIGHTERTRAINER:
		case POPUP_ADVANCEDFIGHTER:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 3 ][ i ], TRUE );
			}
		}
		break;

		// 로그 트레이너
		case POPUP_ROGUETRAINER:
		case POPUP_ADVANCEDROGUE:		
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 2 ][ i ], TRUE );
			}
		}
		break;

		// 마법사 트레이너
		case POPUP_MAGETRAINER:
		case POPUP_ADVANCEDMAGE:	
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 0 ][ i ], TRUE );
			}
		}
		break;

		// 성직자 트레이너
		case POPUP_ACOLYTETRAINER:
		case POPUP_ADVANCEDACOLYTEE:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 1 ][ i ], TRUE );
			}
		}
		break;

		// 메달 상점
		case POPUP_MEDALSHOP:
		case POPUP_A_MEDALSHOP :
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 8 ][ i ], TRUE );
			}
		}
		break;

		// 아칸 무기상
		case POPUP_A_MEDALSHOP_WEAPON:
		case POPUP_A_WEAPONMERCHANT:
		case POPUP_A_DARKWEAPONMERCHANT :
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 9 ][ i ], TRUE );
			}
		}
		break ;
		
		// 아칸 방어구상
		case POPUP_A_MEDALSHOP_ARMOUR:
		case POPUP_A_ARMOURMERCHANT:
		case POPUP_A_DARKARMOURMERCHANT :
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 10 ][ i ], TRUE );
			}
		}
		break ;

		// 비기너 트레이너
		case POPUP_A_BEGINNERTRAINER:
		case POPUP_ADVANCEDBEGINNER:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 11 ][ i ], TRUE );
			}
		}
		break ;

		// 컴배턴트 트레이너
		case POPUP_A_COMBATANTTRAINER:
		case POPUP_ADVANCEDCOMBATANT:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 12 ][ i ], TRUE );
			}
		}
		break ;

		// 오피세이터 트레이너
		case POPUP_A_OPPICATERTRAINER:
		case POPUP_ADVANCEDOPPICATER:
		{
			for ( int i = 0; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 13 ][ i ], TRUE );
			}
		}
		break ;

        case POPUP_SELLINGLIST:
        {
            for ( int i = 0 ; i < 4 ; ++i )
            {
                lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 14 ][ i ], TRUE );
            }
        }
        break;
	}
}
#endif

#ifdef __NEWNPCSCRIPT
VOID	CRYLNewTradeDlg::InitTrade( DWORD dwNPCID, unsigned long dwContainerID )
{
	InitValue();
	m_dwNPCID = dwNPCID;

	CRYLGameData::Instance()->m_dwTradingNpcID = dwNPCID ;

	LPContainerNode lpContainer = g_NPCList.GetContainer(dwContainerID);

	if(lpContainer)
	{
		m_lpContainer = lpContainer;

		m_bBlackMarket = m_lpContainer->m_bBlackMarket;
		m_bMedalMarket = m_lpContainer->m_bMedalMarket;
		m_bSkillCouponMarket = m_lpContainer->m_bSkillCouponMarket;
		m_bRandomOptionMarket = m_lpContainer->m_bRandomOptionMarket;

		if(m_lpContainer->m_bEnableRepair)
		{
			m_pRepairButton->SetVisible( TRUE );
			m_pAllRepairButton->SetVisible( TRUE );
		}

		if(m_bMedalMarket)
		{
			m_pServiceMedalLabel->SetVisible( TRUE );
			m_pServiceMedalValueLabel->SetVisible( TRUE );
			AttachWindowTexture( ( CGMTexture*)m_pImage2 );
		}

		NPCNode* lpNpcNode		= g_NPCList.GetNPCNode( dwNPCID );

		m_lpBuyItem				= NULL;

		LPTABBUTTONINFO		lpTabInfo = NULL;
		unsigned long i;
		for ( i = 0; i < 4 ; i ++ )
		{
			lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
			lpTabInfo->m_pTabButton->SetCaption( "", TRUE );
		}
		for ( i = 0; i < lpContainer->m_dwNumTab ; i ++ )
		{
			lpTabInfo = m_pTabButton->GetTabButtonInfo( i );
			lpTabInfo->m_pTabButton->SetCaption( const_cast<LPSTR>( lpContainer->m_lstTabNode[i]->m_strTitle.c_str() ), TRUE );
		}

		AttachWindowTexture( ( CGMTexture*)m_pImage1 );

		memset( m_nCurPage, 0, sizeof(int) * 4 );

		DeleteTradeItem();

		InitTradeWindow();

		SetItemToTradeWindow(lpNpcNode);

		if ( IsRandomMarket() )
		{
			ChangeEquipItemGradeOption(lpNpcNode);
		}

		AddTradeEquipItemWindow();
		SetPageLabel();

		m_pOptionRefreshButton->SetVisible( IsRandomMarket() );
	}
}
#else
VOID	CRYLNewTradeDlg::InitTrade( DWORD dwNPCID, unsigned char ucTradeDlgIndex, unsigned short usPopupCapacity )
{
	InitValue();
	m_dwNPCID = dwNPCID;
    m_ucCurTradeDlgIndex = ucTradeDlgIndex;

	CRYLGameData::Instance()->m_dwTradingNpcID = dwNPCID ;

	m_dwTradeTabOrder		= 0;

	m_bBlackMarket			= FALSE;
	m_bMedalMarket			= FALSE;
	m_bSkillCouponMarket	= FALSE;

	NPCNode* lpNpcNode		= g_NPCList.GetNPCNode( dwNPCID );

	m_lpBuyItem				= NULL;

	m_dwTradeWindowOrder    = ucTradeDlgIndex;

    ChangeTabTitle( usPopupCapacity );

	m_bRandomOptionMarket = FALSE;

	AttachWindowTexture( ( CGMTexture*)m_pImage1 );

	switch( usPopupCapacity )
	{
		case POPUP_WEAPONMERCHANT :
		case POPUP_A_WEAPONMERCHANT:
			m_dwKindTitle = 0;
			m_pRepairButton->SetVisible( TRUE );
			m_pAllRepairButton->SetVisible( TRUE );
			m_bRandomOptionMarket = FALSE;//was TRUE
			break;

		case POPUP_ARMOURMERCHANT:
		case POPUP_A_ARMOURMERCHANT:
			m_dwKindTitle = 1;
			m_pRepairButton->SetVisible( TRUE );
			m_pAllRepairButton->SetVisible( TRUE );
			m_bRandomOptionMarket = FALSE;//was TRUE
			break;

		case POPUP_ETCMERCHANT:
		case POPUP_A_ETCMERCHANT:
			m_dwKindTitle = 2;
			m_pRepairButton->SetVisible( TRUE );
			m_pAllRepairButton->SetVisible( TRUE );
			break;

        case POPUP_SELLINGLIST:
			m_dwKindTitle = 2;
			break;

		case POPUP_ADVENTURETRAINER:
		case POPUP_A_BEGINNERTRAINER:
			m_dwKindTitle = 3;
			break;

		case POPUP_ADVANCEDADVENTURE:
		case POPUP_ADVANCEDBEGINNER:
			m_dwKindTitle = 3;
			m_bSkillCouponMarket = TRUE;
			break;

		case POPUP_FIGHTERTRAINER:
		case POPUP_A_COMBATANTTRAINER:
			m_dwKindTitle = 4;
			break;

		case POPUP_ADVANCEDFIGHTER:
		case POPUP_ADVANCEDCOMBATANT:
			m_dwKindTitle = 4;
			m_bSkillCouponMarket = TRUE;
			break;

		case POPUP_ROGUETRAINER:
		case POPUP_A_OPPICATERTRAINER:
			m_dwKindTitle = 5;
			break;

		case POPUP_ADVANCEDROGUE:		
		case POPUP_ADVANCEDOPPICATER:
			m_dwKindTitle = 5;
			m_bSkillCouponMarket = TRUE;
			break;

		case POPUP_MAGETRAINER:
			m_dwKindTitle = 6;
			break;

		case POPUP_ADVANCEDMAGE:	
			m_dwKindTitle = 6;
			m_bSkillCouponMarket = TRUE;
			break;

		case POPUP_ACOLYTETRAINER:
			m_dwKindTitle = 7;
			break;

		case POPUP_ADVANCEDACOLYTEE:
			m_dwKindTitle = 7;
			m_bSkillCouponMarket = TRUE;
			break;

		case POPUP_A_DARKWEAPONMERCHANT :
		case POPUP_A_DARKARMOURMERCHANT :
		case POPUP_A_DARKETCMERCHANT :
			m_dwKindTitle = 6;
			m_bBlackMarket = TRUE;
			break ;

		case POPUP_DARKWEAPONMERCHANT:
		case POPUP_DARKARMOURMERCHANT:
		case POPUP_DARKETCMERCHANT:
			m_dwKindTitle = 8;
			m_bBlackMarket = TRUE;
			break;

		case POPUP_MEDALSHOP:
		case POPUP_MEDALSHOP_WEAPON:
		case POPUP_MEDALSHOP_ARMOUR:
		case POPUP_A_MEDALSHOP:
		case POPUP_A_MEDALSHOP_WEAPON:
		case POPUP_A_MEDALSHOP_ARMOUR:
			m_dwKindTitle = 9;
			m_bMedalMarket = TRUE;
            m_pPageLabel->SetVisible( FALSE );
            m_pPrevButton->SetVisible( FALSE );
            m_pNextButton->SetVisible( FALSE );
			m_pServiceMedalLabel->SetVisible( TRUE );
			m_pServiceMedalValueLabel->SetVisible( TRUE );
			AttachWindowTexture( ( CGMTexture*)m_pImage2 );
			break;

		case POPUP_SKILLMERCHANT:
			m_dwKindTitle = 10;
			break;
	}

	memset( m_nCurPage, 0, sizeof(int) * 4 );

    DeleteTradeItem();
    lpNpcNode->SetItemToTradeWindow();

    if ( IsRandomMarket() )
	{
        lpNpcNode->ChangeEquipItemGradeOption();
	}

    AddTradeEquipItemWindow( lpNpcNode, ucTradeDlgIndex );
	SetPageLabel();

    m_pOptionRefreshButton->SetVisible( IsRandomMarket() );
}
#endif

#ifdef __NEWNPCSCRIPT
VOID CRYLNewTradeDlg::InitTradeWindow(void)
{
	if(m_lpTradeWindow == NULL)
	{
		SingleTradeTab*     pTab    = NULL;
		SingleTradePage*    pPage   = NULL;

		m_lpTradeWindow = new SingleTradeWindow;

		unsigned long i, j;
		for(i = 0; i < m_lpContainer->m_dwNumTab; ++i)
		{
			pTab = new SingleTradeTab;
			m_lpTradeWindow->m_vecTradeTab.push_back(pTab);
			for(j = 0; j < m_lpContainer->m_lstTabNode[i]->m_dwNumPage; j++)
			{
				pPage = new SingleTradePage;
				pTab->m_vecTradePage.push_back(pPage);
			}
		}
	}
}

VOID CRYLNewTradeDlg::DestroyTradeWindow(void)
{
	if(m_lpTradeWindow)
	{
		vector<SingleTradeTab*>::iterator iterTab;
		vector<SingleTradePage*>::iterator iterPage;
		vector<Item::CItem*>::iterator iterItem;
		SingleTradeTab* pTab;
		SingleTradePage* pPage;
		Item::CItem* lpItem;
		for( iterTab = m_lpTradeWindow->m_vecTradeTab.begin() ; iterTab != m_lpTradeWindow->m_vecTradeTab.end() ;)
		{
			pTab = (*iterTab);

			for( iterPage = pTab->m_vecTradePage.begin() ; iterPage != pTab->m_vecTradePage.end() ;)
			{
				pPage = (*iterPage);

				for( iterItem = pPage->m_vecEquipmentItem.begin() ; iterItem != pPage->m_vecEquipmentItem.end() ;)
				{
					lpItem = (*iterItem);
					iterItem = pPage->m_vecEquipmentItem.erase(iterItem);
					if( lpItem ) DELETE_ITEM( lpItem );
				}

				pPage->m_vecEquipmentItem.clear();
				pPage->m_mapEquipmentItemGrade.clear();

				iterPage = pTab->m_vecTradePage.erase(iterPage);
				delete pPage;
			}
			iterTab = m_lpTradeWindow->m_vecTradeTab.erase(iterTab);
			delete pTab;
		}
		delete m_lpTradeWindow;
		m_lpTradeWindow = NULL;
	}
}

void CRYLNewTradeDlg::ClearTradeWindow()
{
	// 너무 한거 아닌가 싶기도 하지만 그래도 양이 얼마 안되니까 이렇게 돌려도 몇 번 안돈다 -_-;
	// 메모리만 지우고 공간은 남겨둔다.
	if(m_lpTradeWindow)
	{
		vector<SingleTradeTab*>::iterator iterTab;
		vector<SingleTradePage*>::iterator iterPage;
		vector<Item::CItem*>::iterator iterItem;
		SingleTradeTab* pTab;
		SingleTradePage* pPage;
		Item::CItem* lpItem;
		for( iterTab = m_lpTradeWindow->m_vecTradeTab.begin() ; iterTab != m_lpTradeWindow->m_vecTradeTab.end() ; ++iterTab )
		{
			pTab = (*iterTab);

			for( iterPage = pTab->m_vecTradePage.begin() ; iterPage != pTab->m_vecTradePage.end() ; ++iterPage )
			{
				pPage = (*iterPage);

				for( iterItem = pPage->m_vecEquipmentItem.begin() ; iterItem != pPage->m_vecEquipmentItem.end() ; ++iterItem )
				{
					lpItem = (*iterItem);
					if( lpItem )
					{
						DELETE_ITEM( lpItem );
						(*iterItem) = NULL;
					}
				}

				pPage->m_vecEquipmentItem.clear();
				pPage->m_mapEquipmentItemGrade.clear();
			}
		}
	}
}

VOID CRYLNewTradeDlg::SetItemToTradeWindow(NPCNode *lpNPCNode)
{
	if(m_lpTradeWindow)
	{
		Item::EquipType::Grade	cItemGrade;
		LPItemNode pItemNode;
		Item::CItem*            lpItem;

		ClearTradeWindow();

		unsigned dwTab, dwPage;
		vector<LPItemNode>::iterator itItem;

		for(dwTab = 0; dwTab < m_lpContainer->m_dwNumTab; ++dwTab)
		{
			for(dwPage = 0; dwPage < m_lpContainer->m_lstTabNode[dwTab]->m_dwNumPage; ++dwPage)
			{
				for(itItem = m_lpContainer->m_lstTabNode[dwTab]->m_lstPageNode[dwPage]->m_lstItemNode.begin();
					itItem != m_lpContainer->m_lstTabNode[dwTab]->m_lstPageNode[dwPage]->m_lstItemNode.end(); ++itItem)
				{
					pItemNode = (*itItem);
					lpItem = Item::CItemFactory::GetInstance().CreateItem( pItemNode->m_wKindItem );

					cItemGrade = Item::EquipType::Grade( 
						lpNPCNode->GetRandomGrade(static_cast<Item::ItemType::Type>(lpItem->GetItemInfo().m_DetailData.m_cItemType)) );

					SingleTradePage* pTradePage = 
						m_lpTradeWindow->m_vecTradeTab[ dwTab ]->m_vecTradePage[ dwPage ];

					pTradePage->m_mapEquipmentItemGrade.insert( pair< unsigned short, unsigned char>( pItemNode->m_wKindItem, cItemGrade ) );
					pTradePage->m_vecEquipmentItem.push_back( lpItem );
				}
			}
		}
	}
}

VOID CRYLNewTradeDlg::ChangeEquipItemGradeOption(NPCNode *lpNPCNode)
{
	if(m_lpTradeWindow)
	{
		Item::EquipType::Grade	cItemGrade;
		Item::CItem*            lpItem;
		Item::CEquipment*       pEquip;

		for( vector<SingleTradeTab*>::iterator iterTab = m_lpTradeWindow->m_vecTradeTab.begin() ;
			iterTab != m_lpTradeWindow->m_vecTradeTab.end() ;
			++iterTab )
		{
			SingleTradeTab* pTab = (*iterTab);

			for( vector<SingleTradePage*>::iterator iterPage = pTab->m_vecTradePage.begin() ;
				iterPage != pTab->m_vecTradePage.end() ;
				++iterPage )
			{
				SingleTradePage* pPage = (*iterPage);

				for( vector<Item::CItem*>::iterator iterItem = pPage->m_vecEquipmentItem.begin() ;
					iterItem != pPage->m_vecEquipmentItem.end() ;
					++iterItem )
				{
					lpItem = (*iterItem);

					Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(lpItem->GetItemInfo().m_DetailData.m_cItemType);
					cItemGrade = lpNPCNode->GetRandomGrade(eItemType);

					pEquip = Item::CEquipment::DowncastToEquipment( lpItem );
					pEquip->AddRandomOption( cItemGrade, lpNPCNode->GetBaseNum(eItemType, cItemGrade), 0);
					pEquip->SetNewEquip();
					pEquip->SetNumOrDurability( pEquip->GetMaxNumOrDurability() );
				}
			}
		}
	}
}
#endif
//--------------------------------------------------------------------------------------------------------------------------
// CRYLNewTradeDlg::RefreshRandomOption()
VOID CRYLNewTradeDlg::RefreshRandomOption()
{
    if ( IsRandomMarket() )
	{
	    NPCNode* lpNpcNode		= g_NPCList.GetNPCNode( CRYLGameData::Instance()->m_dwTradingNpcID );

        DeleteTradeItem();
#ifdef __NEWNPCSCRIPT
		SetItemToTradeWindow(lpNpcNode);
        ChangeEquipItemGradeOption(lpNpcNode);
        AddTradeEquipItemWindow();
#else
		lpNpcNode->SetItemToTradeWindow();
        lpNpcNode->ChangeEquipItemGradeOption();
        AddTradeEquipItemWindow( lpNpcNode, m_ucCurTradeDlgIndex );
#endif
 
		m_ptStartPos.x = GetLeft();
		m_ptStartPos.y = GetTop();
   }
}
