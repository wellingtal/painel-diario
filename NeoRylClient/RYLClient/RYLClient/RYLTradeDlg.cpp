
/*
#include <winsock2.h>

#include <WinInput.h>
#include <RenderOption.h>

#include <Item/ItemMgr.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Creature/Character/CharacterClass.h>
#include <Creature/NPC/NPCList.h>

#include "..\\GUITooltipItem.h"
#include "..\\GUITextEdit.h"

#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"
#include "GMUIObject.h"
#include "GMImageDib.h"

#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLRaceBaseDefine.h"
#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLChattingDlg.h"
#include "RYLTradeDlg.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLHumanInventoryPanel.h"
#include "RYLAkhanInventoryPanel.h"
#include "RYLSkillPanel.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"

CRYLTradeDlg*	g_pTradeDlg = NULL ;

CRYLTradeDlg::CRYLTradeDlg() 
{
	m_pTabButton	= NULL ;
	
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 269, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 100, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 0, 113, 15 ) ;
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 113, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 269, 397	+27, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 100, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 113, 37 ) ;
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 113, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	for ( int i = 0 ; i < (74 + 6); i ++ )
	{
		SetRect( &rt, 0, 38, 100, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 113, 43 ) ;
		pImageDIB->DrawRect( &rt, 100, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 113, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}

	SetRect( &rt, 0, 42, 100, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 414, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 113, 52 ) ;
	pImageDIB->DrawRect( &rt, 100, 414, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 113, 414, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	for ( int ver = 0 ; ver < 12 ; ver ++ )
	{
		for ( int hor = 0 ; hor < 8 ; hor ++ )
		{
			pImageDIB->MakeFrameRect( 30 + ( hor * 26 ), 36 + (ver * 26 ), 27, 28, 0xff6e6857, 0xc0000000,  1 ) ;		
		}
	}

	pImageDIB->DrawFrameRect( 30, 36, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30, 36, 1, 313, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30, 349, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 237, 36, 1, 313, 0xffB99E6B ) ;

	pImageDIB->MakeFrameRect( 149, 370-15, 68, 18, 0xff959595, 0xc0000000, 1 ) ;			// 페이즈 카운트 박스

	m_pImage1 = new CRYLImage() ;
	m_pImage1->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage1 ) ;
	
	AttachWindowTexture( ( CGMTexture*)m_pImage1 ) ;
	GM_DELETE( pImageDIB ) ;

    //------------------------------------------------------------------------------------------------------
	CRYLSpriteList::Instance()->CreateGMImage( 269, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 100, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 113, 37 ) ;
	pImageDIB->DrawRect( &rt, 100, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 113, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	for ( int i = 0 ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 100, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 113, 43 ) ;
		pImageDIB->DrawRect( &rt, 100, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		SetRect( &rt, 100, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 113, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 100, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 113, 52 ) ;
	pImageDIB->DrawRect( &rt, 100, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 100, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 113, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	for ( int ver = 0 ; ver < 12 ; ver ++ )
	{
		for ( int hor = 0 ; hor < 8 ; hor ++ )
		{
			pImageDIB->MakeFrameRect( 30 + ( hor * 26 ), 36 + (ver * 26 ), 27, 28, 0xff6e6857, 0xc0000000,  1 ) ;		
		}
	}

	pImageDIB->DrawFrameRect( 30, 36, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30, 36, 1, 313, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 30, 349, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 237, 36, 1, 313, 0xffB99E6B ) ;

	pImageDIB->MakeFrameRect( 135, 362, 103, 16, 0xff959595, 0x80000000, 1 ) ;

	m_pImage2 = new CRYLImage() ;
	m_pImage2->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pImage2 ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 269 ) ;
	SetHeight( 439 ) ;

	SetLeft( g_ClientMain.m_iScreenWidth  - 525 );
	SetTop( 0 );

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	m_pRepairButton				= NULL ;
	m_pAllRepairButton			= NULL ;
	m_pServiceMedalLabel		= NULL ;	
	m_pServiceMedalValueLabel	= NULL ;	
	m_pPrevButton				= NULL ;							// 페이즈 이동 버튼
	m_pNextButton				= NULL ;							// 페이즈 이동 버튼
	m_pPhaseLabel				= NULL ;
	memset( m_aryCurPhase, 0, sizeof(int) * 4 );
	memset( m_aryMaxPhase, 0, sizeof(int) * 4 );

	m_dwAllRepairResult		= 0 ;
	m_dwBuyResult			= 0 ;
	m_dwSellResult			= 0 ;
	m_dwKindTitle			= 0 ;
	m_dwTradeTab			= 0 ;
	m_lpBuyItem				= NULL ;
	m_dwItemNumber			= 0 ;

	m_bBlackMarket			= FALSE ;
	m_bMedalMarket			= FALSE ;
	m_bSkillCouponMarket	= FALSE ;
	m_bRandomOptionMarket	= FALSE ;

	m_cRace					= 0 ;
	m_dwNPCID				= 0;
	memset( m_aryIsEquipItemChanged, 0, sizeof( BOOL ) * 4 * 500 );
	memset( m_aryIsEquipItemInitialized, 0, sizeof( BOOL ) * 8 );

	// 마법사 트레이너
	strcpy( m_TabTitle[ 0 ][ 0 ], CRYLStringTable::m_strString[ 2095 ] ) ;
	strcpy( m_TabTitle[ 0 ][ 1 ], CRYLStringTable::m_strString[ 2096 ] ) ;
	strcpy( m_TabTitle[ 0 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 0 ][ 3 ], "" ) ;

	// 성직자 트레이너
	strcpy( m_TabTitle[ 1 ][ 0 ], CRYLStringTable::m_strString[ 2097 ] ) ;
	strcpy( m_TabTitle[ 1 ][ 1 ], CRYLStringTable::m_strString[ 2098 ] ) ;
	strcpy( m_TabTitle[ 1 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 1 ][ 3 ], "" ) ;

	// 로그 트레이너
	strcpy( m_TabTitle[ 2 ][ 0 ], CRYLStringTable::m_strString[ 2099 ] ) ;
	strcpy( m_TabTitle[ 2 ][ 1 ], CRYLStringTable::m_strString[ 2100 ] ) ;
	strcpy( m_TabTitle[ 2 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 2 ][ 3 ], "" ) ;

	// 전사 트레이너
	strcpy( m_TabTitle[ 3 ][ 0 ], CRYLStringTable::m_strString[ 2101 ] ) ;
	strcpy( m_TabTitle[ 3 ][ 1 ], CRYLStringTable::m_strString[ 2102 ] ) ;
	strcpy( m_TabTitle[ 3 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 3 ][ 3 ], "" ) ;

	// 모험가 트레이너
	strcpy( m_TabTitle[ 4 ][ 0 ], CRYLStringTable::m_strString[ 2103 ] ) ;
	strcpy( m_TabTitle[ 4 ][ 1 ], "" ) ;
	strcpy( m_TabTitle[ 4 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 4 ][ 3 ], "" ) ;

	// 잡화상점 
	strcpy( m_TabTitle[ 5 ][ 0 ], CRYLStringTable::m_strString[ 2104 ] ) ;
	strcpy( m_TabTitle[ 5 ][ 1 ], CRYLStringTable::m_strString[ 2105 ] ) ;
	strcpy( m_TabTitle[ 5 ][ 2 ], CRYLStringTable::m_strString[ 2106 ] ) ;
	strcpy( m_TabTitle[ 5 ][ 3 ], "" ) ;

	// 인간 방어구상
	strcpy( m_TabTitle[ 6 ][ 0 ], CRYLStringTable::m_strString[ 2107 ] ) ;
	strcpy( m_TabTitle[ 6 ][ 1 ], CRYLStringTable::m_strString[ 2108 ] ) ;
	strcpy( m_TabTitle[ 6 ][ 2 ], CRYLStringTable::m_strString[ 2109 ] ) ;
	strcpy( m_TabTitle[ 6 ][ 3 ], CRYLStringTable::m_strString[ 2110 ] ) ;

	// 인간 무기상
	strcpy( m_TabTitle[ 7 ][ 0 ], CRYLStringTable::m_strString[ 2111 ] ) ;
	strcpy( m_TabTitle[ 7 ][ 1 ], CRYLStringTable::m_strString[ 2112 ] ) ;
	strcpy( m_TabTitle[ 7 ][ 2 ], CRYLStringTable::m_strString[ 2113 ] ) ;
	strcpy( m_TabTitle[ 7 ][ 3 ], CRYLStringTable::m_strString[ 2114 ] ) ;

	// 메달 상점
	strcpy( m_TabTitle[ 8 ][ 0 ], CRYLStringTable::m_strString[ 2115 ] ) ;
	strcpy( m_TabTitle[ 8 ][ 1 ], "" ) ;
	strcpy( m_TabTitle[ 8 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 8 ][ 3 ], "" ) ;

	// 아칸 무기상
	strcpy( m_TabTitle[ 9 ][ 0 ], CRYLStringTable::m_strString[ 2116 ] ) ;		
	strcpy( m_TabTitle[ 9 ][ 1 ], CRYLStringTable::m_strString[ 2117 ] ) ;	
	strcpy( m_TabTitle[ 9 ][ 2 ], CRYLStringTable::m_strString[ 2118 ] ) ;
	strcpy( m_TabTitle[ 9 ][ 3 ], CRYLStringTable::m_strString[ 2119 ] ) ;		

	// 아칸 방아구상
	strcpy( m_TabTitle[ 10 ][ 0 ], CRYLStringTable::m_strString[ 2107 ] ) ;
	strcpy( m_TabTitle[ 10 ][ 1 ], CRYLStringTable::m_strString[ 2108 ] ) ;
	strcpy( m_TabTitle[ 10 ][ 2 ], CRYLStringTable::m_strString[ 2109 ] ) ;
	strcpy( m_TabTitle[ 10 ][ 3 ], CRYLStringTable::m_strString[ 2110 ] ) ;

	// 비기너 트레이너
	strcpy( m_TabTitle[ 11 ][ 0 ], CRYLStringTable::m_strString[ 2120 ] ) ;
	strcpy( m_TabTitle[ 11 ][ 1 ], "" ) ;
	strcpy( m_TabTitle[ 11 ][ 2 ], "" ) ;
	strcpy( m_TabTitle[ 11 ][ 3 ], "" ) ;

	// 컴베턴트 트레이너
	strcpy( m_TabTitle[ 12 ][ 0 ], CRYLStringTable::m_strString[ 2121 ] ) ;
	strcpy( m_TabTitle[ 12 ][ 1 ], CRYLStringTable::m_strString[ 2122 ] ) ;
	strcpy( m_TabTitle[ 12 ][ 2 ], CRYLStringTable::m_strString[ 2123 ] ) ;
	strcpy( m_TabTitle[ 12 ][ 3 ], "" ) ;

	// 오피세이터 트레이너
	strcpy( m_TabTitle[ 13 ][ 0 ], CRYLStringTable::m_strString[ 2124 ] ) ;
	strcpy( m_TabTitle[ 13 ][ 1 ], CRYLStringTable::m_strString[ 2125 ] ) ;
	strcpy( m_TabTitle[ 13 ][ 2 ], CRYLStringTable::m_strString[ 2126 ] ) ;
	strcpy( m_TabTitle[ 13 ][ 3 ], "" ) ;

	g_pTradeDlg = this ;
	InitCtrl() ;
}

CRYLTradeDlg::~CRYLTradeDlg() 
{
	FinalCtrl() ;

	vector< CItemInstance * >::iterator it;
	CItemInstance *lpDelete ;

	for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); )
	{
		lpDelete = ( *it ) ;
		it = m_lstTradeItem.erase( it ) ;
		GM_DELETE( lpDelete ) ;
	}
	m_lstTradeItem.clear() ;
}

VOID	CRYLTradeDlg::InitValue()
{
	m_dwBuyResult	= 0 ;
	m_dwSellResult	= 0 ;
	m_dwTradeTab	= 0 ;
	m_lpBuyItem		= NULL ;
	m_dwItemNumber	= 0 ;
	m_dwAllRepairResult = 0 ;
}

VOID	CRYLTradeDlg::InitCtrl() 
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLTRADEDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( "", 4 ) ;
	m_pTabButton->AttachTabButton( "", 4 ) ;
	m_pTabButton->AttachTabButton( "", 4 ) ;
	m_pTabButton->AttachTabButton( "", 4 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	m_pRepairButton = new CRYLButton( this ) ;
	m_pRepairButton->SetLeft( 92 ) ;
	m_pRepairButton->SetTop( 397 ) ;
	m_pRepairButton->SetCaption( CRYLStringTable::m_strString[ 2087 ] ) ;
	m_pRepairButton->SetFontColor( 0xffffffff) ;
	m_pRepairButton->SetColor( 0xffffffff) ;
	m_pRepairButton->OnClickEvent = DlgClick ;
	m_pRepairButton->SetInstance( RYLTRADEDLG::REPAIR_BUTTON ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1890 ], 1890 ) ;
	AttachGMUIControl( m_pRepairButton ) ;

	m_pAllRepairButton = new CRYLButton( this ) ;
	m_pAllRepairButton->SetLeft( 166 ) ;
	m_pAllRepairButton->SetTop( 397 ) ;
	m_pAllRepairButton->SetCaption( CRYLStringTable::m_strString[ 2088 ] ) ;
	m_pAllRepairButton->SetFontColor( 0xffffffff) ;
	m_pAllRepairButton->SetColor( 0xffffffff) ;
	m_pAllRepairButton->OnClickEvent = DlgClick ;
	m_pAllRepairButton->SetInstance( RYLTRADEDLG::ALLREPAIR_BUTTON ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pAllRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1891 ], 1891 ) ;
	AttachGMUIControl( m_pAllRepairButton ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	RECT rt ;

	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 134 ) ;
	m_pPrevButton->SetTop( 373 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLTRADEDLG::PREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPrevButton ) ;

	m_pNextButton = new CRYLButton( this ) ;
	m_pNextButton->SetLeft( 219 ) ;
	m_pNextButton->SetTop( 373 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLTRADEDLG::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;

	m_pPhaseLabel = new CRYLLabel( this ) ;		
	m_pPhaseLabel->SetLeft( 158 ) ;
	m_pPhaseLabel->SetTop( 374 ) ;
	m_pPhaseLabel->SetFontColor (0xffffffff);
	m_pPhaseLabel->SetAutoSize( TRUE ) ;
	m_pPhaseLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pPhaseLabel ) ;

	m_pServiceMedalLabel = new CRYLLabel( this ) ;		
	m_pServiceMedalLabel->SetLeft( 35 ) ;
	m_pServiceMedalLabel->SetTop( 379 ) ;
	m_pServiceMedalLabel->SetFontColor (0xffffffff);
	m_pServiceMedalLabel->SetAutoSize( TRUE ) ;
	m_pServiceMedalLabel->SetCaption( CRYLStringTable::m_strString[ 2150 ] ) ;
	AttachGMUIControl( m_pServiceMedalLabel ) ;

	m_pServiceMedalValueLabel = new CRYLLabel( this ) ;		
	m_pServiceMedalValueLabel->SetLeft( 195 ) ;
	m_pServiceMedalValueLabel->SetTop( 379 ) ;
	m_pServiceMedalValueLabel->SetFontColor (0xffffffff);
	m_pServiceMedalValueLabel->SetAutoSize( TRUE ) ;
	m_pServiceMedalValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pServiceMedalValueLabel ) ;
}

VOID	CRYLTradeDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pRepairButton ) ;					
	GM_DELETE( m_pAllRepairButton ) ;			
	GM_DELETE( m_pServiceMedalLabel ) ;			
	GM_DELETE( m_pServiceMedalValueLabel ) ;	
	GM_DELETE( m_pPrevButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pNextButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pPhaseLabel );
}

VOID	CRYLTradeDlg::DeleteTradeItem()
{
	vector<CItemInstance *>::iterator it ;
	CItemInstance *lpDelete ;
	
	for ( it = m_lstTradeItem.begin() ; it != m_lstTradeItem.end() ; )
	{
		lpDelete = ( *it ) ;
		it = m_lstTradeItem.erase( it ) ;
		GM_DELETE( lpDelete ) ;
	}
	m_lstTradeItem.clear() ;
}

VOID	CRYLTradeDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
	pInventoryDlg->SetVisible( TRUE ) ;
	pInventoryDlg->ShowInventory() ;
	
	m_pTabButton->SetFocusTabButton( 0 ) ;

	m_pRepairButton->SetVisible( FALSE ) ;
	m_pAllRepairButton->SetVisible( FALSE ) ;
	m_pServiceMedalLabel->SetVisible( FALSE ) ;
	m_pServiceMedalValueLabel->SetVisible( FALSE ) ;
	CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL ;

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;
}

VOID	CRYLTradeDlg::Hide( CGMUIObject* Sender ) 
{
	m_bBlackMarket = FALSE;
	m_bMedalMarket = FALSE;
	m_bSkillCouponMarket = FALSE;

	m_pRepairButton->SetVisible( FALSE ) ;
	m_pAllRepairButton->SetVisible( FALSE ) ;

	CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL ;

	vector<CItemInstance *>::iterator it ;
	CItemInstance* lpDelete ;
	
	for ( it = m_lstTradeItem.begin() ; it != m_lstTradeItem.end() ; )
	{
		lpDelete = ( *it ) ;
		it = m_lstTradeItem.erase( it ) ;
		GM_DELETE( lpDelete ) ;
	}
	m_lstTradeItem.clear() ;

	m_ptStartPos.x = GetLeft() ;
	m_ptStartPos.y = GetTop() ;
}

VOID	CRYLTradeDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	vector<CItemInstance *>::iterator it;
	int phaseCnt = m_aryCurPhase[m_dwTradeTab] * 4 ;

	for (it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); it++)
	{
		( *it )->m_wPutX = ( *it )->m_wStartX - ( m_ptStartPos.x - GetLeft() ) ;
		( *it )->m_wPutY = ( *it )->m_wStartY - ( m_ptStartPos.y - GetTop() ) ;
		
		if ( ( *it )->m_lpItemBase->GetPos().GetZIndex() != (m_dwTradeTab + phaseCnt) )
		{
			continue ;
		}
		
		( *it )->Render( lpD3DDevice ) ;
	}
}

HRESULT		CRYLTradeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLTradeDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	
CRYLTradeDlg::SetBuyFlag( CItemInstance * pItemInstance )
{
	int index = 0 ;
	
	NPCNode* lpNpcNode = g_NPCList.GetNPCNode( m_dwNPCID ) ;
	vector<Item::CItem*> *plstItem;
	plstItem = &(lpNpcNode->m_lstEquipItem[ m_cRace*4 + m_dwTradeTab][ m_aryCurPhase[ m_dwTradeTab] ]);
	
	for ( vector<Item::CItem*>::iterator it = plstItem->begin() ; it != plstItem->end() ; ++it, ++index )
	{
		if ( (*it)->GetPrototypeID() == pItemInstance->m_lpItemBase->GetPrototypeID() )
		{
			// 상점창을 열 때마다 랜덤 옵션을 바꿔줍니다.
			Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>((*it)->GetItemInfo().m_DetailData.m_cItemType);
			Item::EquipType::Grade eGrade = lpNpcNode->GetRandomGrade(eItemType);
			ChangeOption( *it, eGrade, lpNpcNode->GetBaseNum(eItemType, eGrade), pItemInstance );
			break;
		}

	}
}

HRESULT		CRYLTradeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if ( g_MessageBoxManager.GetIsExistModal() )
		return E_FAIL ;

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

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;

	if ( m_dwSellResult & MB_YES )
	{
		// 판다. 패킷 전송
		if (NULL != pGame->m_lpPickItem)
		{
			DWORD		dwNum ;
			
			dwNum = ( pGame->m_lpPickItem->GetIsEnableStack() ) ? pGame->m_lpPickItem->GetNowDurability() : 1 ;

			Item::ItemPos	pPos ;
			pNetwork->SendTradeItem( PktTr::TRC_SELL, pGame->m_dwTradingNpcID, 0, 
				TakeType(pGame->m_lpPickItem->m_lpItemBase->GetPos(), pPos, dwNum), Item::ItemPos(), NULL ) ;
		}

		m_dwSellResult = 0 ;
		return S_OK ;
	} 
	else if ( m_dwSellResult & MB_NO || m_dwSellResult & MB_EXIT )
	{
		m_dwSellResult = 0 ;
		return S_OK ;
	}

	if ( m_dwBuyResult & MB_YES )
	{
		// 산다. 패킷 전송
		Item::ItemPos		pItemIndex ;
		FieldInstance		pTradeItem ;
		pTradeItem.m_dwInstanceID	= 0 ;
		pTradeItem.m_nOID			= 0 ;
		pTradeItem.m_dwTypeID		= m_lpBuyItem->GetProtoTypeID() ;
		
		if ( m_lpBuyItem->GetIsEnableStack() )
		{
			if ( m_dwItemNumber > m_lpBuyItem->GetMaxDurability() )
			{
				m_dwItemNumber = m_lpBuyItem->GetMaxDurability() ;
			}

			pTradeItem.m_cNum = m_dwItemNumber ;
		}
		else
		{
			pTradeItem.m_cNum = 1 ;
		}

		if ( pTradeItem.m_cNum )
		{
			BOOL bPass = FALSE ;

			if ( m_bBlackMarket )
			{
				unsigned long dwBuyBlackPrice = m_lpBuyItem->m_lpItemBase->GetBuyBlackPrice() ;
				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
					if ( lpCastle )
					{
						dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax() / 100.f ) );
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
				unsigned long dwBuyPrice = m_lpBuyItem->m_lpItemBase->GetBuyPrice() ;
				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax() / 100.f ) );
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
					lpMessageBox->Create( CRYLStringTable::m_strString[ 258 ] ) ;
				} 
				else if ( m_bSkillCouponMarket )
				{
					lpMessageBox->Create( CRYLStringTable::m_strString[ 2793 ] );
				}
				else
				{
					lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] ) ;
				}

				m_dwBuyResult	= 0 ;
				m_lpBuyItem		= NULL ;

				return S_OK ;
			}

			unsigned long dwNum = pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) ;
			
			if ( dwNum )
			{
				Item::ItemPos	pPos ;
				
				if ( m_lpBuyItem->m_bMedalBuyItem )
				{
					pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_MILEAGE, pGame->m_dwTradingNpcID, 
											 m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, dwNum), Item::ItemPos(), NULL ) ;
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
							m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, dwNum), lpCoupon->m_lpItemBase->GetPos(), NULL );
					}
				}
				else
				{
					if ( IsRandomMarket() )
					{
						if ( true == m_lpBuyItem->GetIsEnableAddOption() )
						{
							pNetwork->SendTradeItem( PktTr::TRC_BUY_EQUIP, pGame->m_dwTradingNpcID, 
								m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, dwNum), Item::ItemPos(), m_lpBuyItem->m_lpItemBase ) ;
							SetBuyFlag( m_lpBuyItem );
						}
						else
						{
							pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_GOLD, pGame->m_dwTradingNpcID, 
								m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, dwNum), Item::ItemPos(), NULL) ;
						}
					}
					else
					{
						pNetwork->SendTradeItem( PktTr::TRC_BUY_WITH_GOLD, pGame->m_dwTradingNpcID, 
							m_lpBuyItem->GetProtoTypeID(), TakeType(pPos, pItemIndex, dwNum), Item::ItemPos(), NULL) ;
					}
				}
			}	
		}

		m_dwItemNumber	= 0 ;
		m_dwBuyResult	= 0 ;
		m_lpBuyItem		= NULL ;

		return S_OK ;
	} 
	else if ( m_dwBuyResult & MB_NO || m_dwBuyResult & MB_EXIT )
	{
		m_dwItemNumber	= 0 ;
		m_dwBuyResult	= 0 ;
		m_lpBuyItem		= NULL ;
		return S_OK ;
	}

	UpdateAllRepair( pGame ) ;

	if ( g_DeviceInput.InRect( GetLeft() + 30, GetTop() + 52, GetLeft() + 237, GetTop() + 364 ) && g_DeviceInput.GetIsLeftMouseUp() && pGame->m_lpPickItem )
	{
		if ( pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT )
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 257 ], pNetwork->m_dwMyChrID, 129 ) ;
		} 
		else if ( pGame->m_lpPickItem->GetProtoTypeID() & 0x8000 ) // 스킬이라면
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 2755 ], pNetwork->m_dwMyChrID, 129 ) ;
		}
		else
		{
			CRYLMessageBox *lpMessageBox ;
			CHAR	strMessage[ MAX_PATH ] ;

			if ( pGame->m_lpPickItem->GetIsEnableStack() )
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], 
					     pGame->m_lpPickItem->m_lpItemBase->GetSellPrice() * pGame->m_lpPickItem->GetNowDurability() ) ;
			} 
			else
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 253 ], 
						 pGame->m_lpPickItem->m_lpItemBase->GetSellPrice() ) ;
			}
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwSellResult ) ;

			m_dwSellResult = 0 ;
		}
		return S_OK ;
	}


	if ( NS_NOWAITING == g_pClientSocket->GetStatusFlag() )
	{
		vector<CItemInstance *>::iterator it;
		int phaseCnt = m_aryCurPhase[m_dwTradeTab] * 4 ;
		for ( it = m_lstTradeItem.begin(); it != m_lstTradeItem.end(); it ++ )
		{
			if ( ( *it )->m_lpItemBase->GetPos().GetZIndex() != (m_dwTradeTab+phaseCnt) )
			{
				continue ;
			}

			if ( !pGame->m_lpPickItem && (g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsRightMouseDown()) && (*it)->IsOverMouse() )
			{
				BOOL bPass = FALSE ;
				if ( m_bBlackMarket )
				{
					unsigned long dwBuyBlackPrice = (*it)->m_lpItemBase->GetBuyBlackPrice() ;
					NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
					if ( pNpcNode && pNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
						if ( lpCastle )
						{
							dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax() / 100.f ) );
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
					unsigned long dwBuyPrice = (*it)->m_lpItemBase->GetBuyPrice() ;
					NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
					if ( pNpcNode && pNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
						if ( lpCastle )
						{
							dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax() / 100.f ) );
						}
					}

					if ( pGame->m_csStatus.m_Info.Gold >= dwBuyPrice ) 
						bPass = TRUE ;
				}

				if ( bPass )
				{
					unsigned long	dwNum ;
					Item::ItemPos	pItemIndex ;
					FieldInstance	pTradeItem ;
					pTradeItem.m_dwInstanceID	= 0 ;
					pTradeItem.m_nOID			= 0 ;
					pTradeItem.m_dwTypeID		= ( *it )->GetProtoTypeID();
					pTradeItem.m_cNum			= 1 ;
					dwNum = pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) ;
					m_dwBuyResult				= 0 ;
					if ( dwNum )
					{
						if ( g_DeviceInput.GetIsLeftMouseUp() )
						{
							CRYLMessageBox* lpMessageBox ;
							CHAR			strMessage[ MAX_PATH ] ;
							lpMessageBox = new CRYLMessageBox ;
							m_dwBuyResult	= 0 ;
							m_lpBuyItem		= ( *it ) ;
							m_dwItemNumber	= 0 ;

							if ( m_lpBuyItem->GetIsEnableStack() )
							{
								strcpy( strMessage, CRYLStringTable::m_strString[ 254 ] ) ;
								lpMessageBox->Create( strMessage, MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
								lpMessageBox->SetValue (&m_dwItemNumber ) ; 
							} 
							else
							{
								if ( m_bBlackMarket )
								{
									unsigned long dwBuyBlackPrice = ( *it )->m_lpItemBase->GetBuyBlackPrice() ;
									NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
									if ( pNpcNode && pNpcNode->m_bBelongToCastle )
									{
										CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
										if ( lpCastle )
										{
											dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax() / 100.f ) );
										}
									}

									sprintf( strMessage, CRYLStringTable::m_strString[ 255 ], dwBuyBlackPrice ) ;
								} 
								else if ( m_bMedalMarket )
								{
									sprintf( strMessage, CRYLStringTable::m_strString[ 255 ], (*it)->m_lpItemBase->GetBuyMedalPrice() ) ;
								} 
								else if ( m_bSkillCouponMarket )
								{
									sprintf( strMessage, CRYLStringTable::m_strString[ 3167 ], 
										(*it)->m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount + 1 );
								}
								else
								{
									unsigned long dwBuyPrice = (*it)->m_lpItemBase->GetBuyPrice() ;
									NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID ) ;
									if ( pNpcNode && pNpcNode->m_bBelongToCastle )
									{
										CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( pNpcNode->m_dwTownID ) ;
										if ( lpCastle )
										{
											dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax() / 100.f ) );
										}
									}

									sprintf(strMessage, CRYLStringTable::m_strString[ 255 ], dwBuyPrice ) ;
								}
								lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT ) ;
							}
							lpMessageBox->SetResult( &m_dwBuyResult ) ;
						} 
						else if ( g_DeviceInput.GetIsRightMouseDown() )
						{
							m_dwBuyResult	= MB_YES ;
							m_lpBuyItem		= ( *it ) ;
							if ( m_lpBuyItem->GetIsEnableStack() )
							{
								m_dwItemNumber = m_lpBuyItem->GetMaxDurability() ;
							}
						}
					} 
					else
					{
						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 256 ] ) ;
					}
				} 
				else
				{
					CRYLMessageBox *lpMessageBox ;
					lpMessageBox = new CRYLMessageBox ;
					if ( m_bMedalMarket )
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 258 ] ) ;
					} 
					else if ( m_bSkillCouponMarket )
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 2793 ] );
					}
					else
					{
						lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] ) ;
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

	// 공헌 메달 값
	char strString[MAX_PATH];
	sprintf( strString, "%d", pGame->m_csStatus.m_Info.Mileage ) ;
	m_pServiceMedalValueLabel->SetCaption( strString ) ;

	return S_OK ;
}

VOID	CRYLTradeDlg::UpdateAllRepair( CRYLGameData* pGame ) 
{
	// 모두 수리
	if ( m_dwAllRepairResult & MB_YES )
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;		
		CRYLHumanInventoryPanel* pHPanel = ( CRYLHumanInventoryPanel* )pInventoryDlg->GetHumanInvenPanel() ;
		CRYLAkhanInventoryPanel* pAPanel = ( CRYLAkhanInventoryPanel* )pInventoryDlg->GetAkhanInvenPanel() ;

		ULONG uiMoney = 0 ;
		
		switch ( pGame->m_dwClientRace )
		{
			case CClass::RaceType::HUMAN:
				uiMoney = pHPanel->GetAllRepairMoney() ;
				break ;
			case CClass::RaceType::AKHAN:
				uiMoney = pAPanel->GetAllRepairMoney() ;
				break ;
		}

		SendPacket::CharRepairAllItem( g_GameSession, pGame->m_dwTradingNpcID, uiMoney ) ;
		m_dwAllRepairResult = 0 ;
	}
	else if ( m_dwAllRepairResult & MB_NO || m_dwAllRepairResult & MB_EXIT )
	{
		m_dwAllRepairResult = 0 ;	
	}
}

VOID	CRYLTradeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pTradeDlg )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch( Sender->GetInstance() )
	{
	case RYLTRADEDLG::TAB_BUTTON :
		{
			g_pTradeDlg->SetTradeTab( g_pTradeDlg->GetTabButton()->GetFocusTabButton() ) ;
		}
		break ;
	case RYLTRADEDLG::REPAIR_BUTTON :
		{
			if ( pGame->m_dwKindCursor != CURSOR_REPAIR)
				pGame->m_dwKindCursor = CURSOR_REPAIR;
			else
				pGame->m_dwKindCursor = CURSOR_NORMAL;
		}
		break;
	case RYLTRADEDLG::ALLREPAIR_BUTTON :
		{
			g_pTradeDlg->AllRepair() ;
		}
		break ;
	case RYLTRADEDLG::NEXT_BUTTON :
		{
			g_pTradeDlg->NextPhase() ;
		}
		break;
	case RYLTRADEDLG::PREV_BUTTON :
		{
			g_pTradeDlg->PrevPhase() ;
		}
		break;
	}
}

VOID	CRYLTradeDlg::NextPhase()
{
	m_aryCurPhase[ m_dwTradeTab ] = min( m_aryCurPhase[ m_dwTradeTab ]+1, (m_aryMaxPhase[ m_dwTradeTab ] -1 ) );
	SetPhaseLabel();
}

VOID	CRYLTradeDlg::PrevPhase()
{
	m_aryCurPhase[ m_dwTradeTab ] = max( 0, m_aryCurPhase[ m_dwTradeTab ] -1 );
	SetPhaseLabel();
}


VOID	CRYLTradeDlg::AllRepair() 
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

	if ( pGame->m_csStatus.m_Info.Gold > uiMoney )
	{
		char strString[MAX_PATH];
		sprintf( strString, CRYLStringTable::m_strString[ 161 ], CRYLStringTable::MakePrintGold( uiMoney ) );

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( strString, MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwAllRepairResult ) ;
		m_dwAllRepairResult = 0 ;
	} 
	else
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 162 ] ) ;
	}
}

// 아이템 변경 여부를 전부 TRUE 로
VOID	CRYLTradeDlg::SetInitialRandomOpition()
{
	for ( int j = 0; j < 4; j++ )
		for ( int i = 0; i < 500; i++ )
			m_aryIsEquipItemChanged[j][i] = TRUE;
}


VOID	CRYLTradeDlg::ChangeOption(Item::CItem* pBaseItem, Item::EquipType::Grade eGrade, unsigned char cBaseNum, 
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
	lpEquip->AddRandomOption( eGrade, cBaseNum );
	lpEquip->SetNewEquip();
	lpEquip->SetNumOrDurability( lpEquip->GetMaxNumOrDurability() );
	pBaseItem->SerializeOut( Buffer, dwItemSize );
	pItemInstance->m_lpItemBase->SerializeIn( Buffer, dwItemSize );
	pItemInstance->m_lpItemBase->SetPos( ucX, ucY, ucZ );
}

VOID	CRYLTradeDlg::AddTradeEquipItemWindow( NPCNode* lpNpcNode, unsigned char cRace, unsigned char cTab )
{
	DWORD		i, j, cx, cy, phase;
	CHAR		arySpace[ 10 ][ 12 ][ 8 ] ;
	USHORT		wOldTab = 0xFFFF ;
	BOOL		bAble ;
	int			idx = cRace*4 + cTab;
	BOOL		bBreak = FALSE;

	for ( phase = 0; phase < 10 ; phase++ )
	{
		for ( j = 0 ; j < 12 ; j ++ )
		{
			for ( i = 0 ; i < 8 ; i ++ )
			{
				arySpace[phase][ j ][ i ] = 0 ;
			}
		}
	}

	int	index = 0;
	Item::CEquipment* lpEquip = NULL;
	m_aryMaxPhase[ cTab ] = lpNpcNode->m_aryPhaseMax[ cTab + (cRace*4) ];

	if ( m_aryIsEquipItemInitialized[ cTab + cRace*4 ] )			
	{
		// 초기화 되었다면...
		for ( phase = 0 ; phase < lpNpcNode->m_aryPhaseMax[ cTab + (cRace*4) ] ; phase++ )
		{
			for ( vector<Item::CItem*>::iterator it = lpNpcNode->m_lstEquipItem[idx][phase].begin() ; 
				it != lpNpcNode->m_lstEquipItem[idx][phase].end() ; it ++, index++ )
			{
				bBreak = FALSE;
				const Item::ItemInfo* lpProtoType = &( (*it)->GetItemInfo() );

				if ( NULL == lpProtoType )						{ continue; }
				if ( !m_aryIsEquipItemChanged[cTab][index] )	{ continue; }

				for (vector< CItemInstance * >::iterator item = m_lstTradeItem.begin(); item != m_lstTradeItem.end(); item++)
				{
					if ( ( *item )->m_lpItemBase->GetPrototypeID() == (*it)->GetPrototypeID() )
					{
						Item::EquipType::Grade eGrade = lpNpcNode->GetItemGradeforID( cTab, (*it)->GetPrototypeID() );
						Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>((*it)->GetItemInfo().m_DetailData.m_cItemType);
						ChangeOption( *it, eGrade, lpNpcNode->GetBaseNum(eItemType, eGrade), (*item) );
						m_aryIsEquipItemChanged[cTab][index] = FALSE;
					}
				}
			}
		}
	}
	else
	{
		for ( phase = 0; phase < lpNpcNode->m_aryPhaseMax[ cTab+ (cRace*4) ] ; phase++ )
		{
			for ( vector<Item::CItem*>::iterator it = lpNpcNode->m_lstEquipItem[idx][phase].begin() ; it != lpNpcNode->m_lstEquipItem[idx][phase].end() ; it ++, index++ )
			{
				bBreak = FALSE;

				const Item::ItemInfo* lpProtoType = &( (*it)->GetItemInfo() );
				if ( NULL == lpProtoType )
					continue;

				DWORD dwSizeX = lpProtoType->m_DetailData.m_cXSize ;
				DWORD dwSizeY = lpProtoType->m_DetailData.m_cYSize ;

				for ( j = 0 ; j <= 12 - dwSizeY ; j ++ )
				{
					for ( i = 0 ; i <= 8 - dwSizeX ; i ++ )
					{
						bAble = TRUE ;

						for ( cy = j ; cy < j + dwSizeY ; cy ++ )
						{
							for ( cx = i ; cx < i + dwSizeX ; cx ++ )
							{
								if ( arySpace[phase][ cy ][ cx ] )
								{
									bAble = FALSE ;
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

							pItemIndex.SetZIndex( cTab + phase*4 ) ;
							pItemIndex.m_cPos = 0 ;

							lpItem->SetItemInfofromItem(  *it , pItemIndex ) ;
							m_lstTradeItem.push_back( lpItem ) ;

							lpItem->m_wPutX = GetLeft() + 30 + 26 * i + 
								(((lpItem->GetItemInfo().m_DetailData.m_cXSize * 26 - 1) - lpItem->m_lpSprite->GetSizeX()) / 2 ) ;
							lpItem->m_wPutY = GetTop()  + 52 + 26 * j + 
								(((lpItem->GetItemInfo().m_DetailData.m_cYSize * 26 - 1) - lpItem->m_lpSprite->GetSizeY()) / 2 ) ;

							lpItem->m_wStartX = lpItem->m_wPutX ;
							lpItem->m_wStartY = lpItem->m_wPutY ;

							for ( cy = j ; cy < j + dwSizeY ; cy ++ )
							{
								for ( cx = i ; cx < i + dwSizeX ; cx ++ )
								{
									arySpace[phase][ cy ][ cx ] = 1 ;
								}
							}

							bBreak = TRUE;
							m_aryIsEquipItemChanged[cTab][index] = FALSE;
						}
						if ( bBreak )
							break;
					}
					if ( bBreak )
						break;
				}
			}

		}
		m_aryIsEquipItemInitialized[ cTab + cRace*4  ] = TRUE;
	}
}

VOID	CRYLTradeDlg::SetPhaseLabel()
{
	char strString[MAX_PATH];
	int nViewPhase = min(m_aryCurPhase[m_dwTradeTab] + 1, m_aryMaxPhase[m_dwTradeTab]);
	sprintf( strString, "%3d / %3d", nViewPhase, m_aryMaxPhase[m_dwTradeTab] ) ;
	m_pPhaseLabel->SetCaption( strString ) ;
}

VOID	CRYLTradeDlg::InitTradeWindow( NPCNode* lpNpcNode, unsigned char cRace )
{
	if ( GetVisible() )
	{
		DeleteTradeItem() ;
	}

	if ( CClass::RaceType::HUMAN != cRace && CClass::RaceType::AKHAN != cRace )
	{
		cRace = CClass::RaceType::HUMAN;
	}

	DWORD		i, j, phase, cx, cy;
	CHAR		arySpace[ 10 ][ 12 ][ 8 ] ;
	USHORT		wOldTab = 0xFFFF ;
	BOOL		bAble ;

	for ( vector<ITEMNode *>::iterator it = lpNpcNode->m_lstItem.begin() ; it != lpNpcNode->m_lstItem.end() ; it ++ )
	{
		if ( cRace != ( *it )->m_cMenuIndex && 2 != cRace ) continue;	// 2는 예전에 남아있던 NATION_UNKNONE 이다.

		const Item::ItemInfo* lpProtoType = g_ItemMgr.GetItemInfo( ( *it )->m_wKindItem ) ;
		if (NULL != lpProtoType)
		{
			DWORD dwSizeX = lpProtoType->m_DetailData.m_cXSize ;
			DWORD dwSizeY = lpProtoType->m_DetailData.m_cYSize ;

			if ( wOldTab != (*it)->m_wTab )
			{
				for ( phase = 0 ; phase < 10; phase++ )
				{
					for ( j = 0 ; j < 12 ; j ++ )
					{
						for ( i = 0 ; i < 8 ; i ++ )
						{
							arySpace[phase][ j ][ i ] = 0 ;
						}
					}
				}
				
				wOldTab = ( *it )->m_wTab ;
				m_aryMaxPhase[ wOldTab ] = lpNpcNode->m_aryPhaseMax[ wOldTab + (cRace*4) ];
			}

			for ( phase = 0; phase < lpNpcNode->m_aryPhaseMax[ wOldTab + (cRace*4) ] ; phase++ )
			{
				if ( phase != ( *it )->m_cPage )
					continue;

				for ( j = 0 ; j <= 12 - dwSizeY ; j ++ )
				{
					for ( i = 0 ; i <= 8 - dwSizeX ; i ++ )
					{
						bAble = TRUE ;
						for ( cy = j ; cy < j + dwSizeY ; cy ++ )
						{
							for ( cx = i ; cx < i + dwSizeX ; cx ++ )
							{
								if ( arySpace[phase][ cy ][ cx ] )
								{
									bAble = FALSE ;
								}
							}
						}

						if ( bAble )
						{
							Item::ItemPos pItemIndex ;
							CItemInstance* lpItem = new CItemInstance ;
							lpItem->m_bTradeItem = TRUE ;

							if ( m_bMedalMarket ) 
							{
								lpItem->m_bMedalBuyItem = TRUE ;
							}
							else if ( m_bSkillCouponMarket )
							{
								lpItem->m_bSkillCouponBuyItem = TRUE ;
							}

							pItemIndex.SetZIndex( ( *it )->m_wTab + phase*4 ) ;
							pItemIndex.m_cPos = 0 ;

							lpItem->SetItemInfofromID( ( *it )->m_wKindItem, pItemIndex ) ;
							m_lstTradeItem.push_back( lpItem ) ;

							Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem->m_lpItemBase);
							if (NULL != lpEquip)
							{
								lpEquip->SetNewEquip();
							}

							lpItem->m_wPutX = GetLeft() + 30 + 26 * i + 
								(((lpItem->GetItemInfo().m_DetailData.m_cXSize * 26 - 1) - lpItem->m_lpSprite->GetSizeX()) / 2 ) ;
							lpItem->m_wPutY = GetTop()  + 52 + 26 * j + 
								(((lpItem->GetItemInfo().m_DetailData.m_cYSize * 26 - 1) - lpItem->m_lpSprite->GetSizeY()) / 2 ) ;

							lpItem->m_wStartX = lpItem->m_wPutX ;
							lpItem->m_wStartY = lpItem->m_wPutY ;

							for ( cy = j ; cy < j + dwSizeY ; cy ++ )
							{
								for ( cx = i ; cx < i + dwSizeX ; cx ++ )
								{
									arySpace[phase][ cy ][ cx ] = 1 ;
								}
							}

							goto LoopEnd ;
						}
					}
				}
			}
		}
LoopEnd:
		;
	}
}

VOID	CRYLTradeDlg::SetTradeTab( INT iTab )
{ 
	m_dwTradeTab = iTab ; 
	SetPhaseLabel();
	if ( IsRandomMarket() )
	{
//		// 상점의 그레이드 정보를 요청합니다.
//		SendPacket::CharEquipShopInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, CRYLGameData::Instance()->m_dwTradingNpcID, m_cRace, iTab );
	}

}

VOID	CRYLTradeDlg::ChangeTabTitle( DWORD dwNPCID )
{

	LPTABBUTTONINFO		lpTabInfo = NULL ;
	switch( dwNPCID )
	{
		// 무기 상점
		case POPUP_WEAPONMERCHANT :
		case POPUP_DARKWEAPONMERCHANT:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 7 ][ i ], TRUE ) ;
			}
		}
		break;

		// 방어구 상점
		case POPUP_ARMOURMERCHANT:
		case POPUP_DARKARMOURMERCHANT:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 6 ][ i ], TRUE ) ;
			}
		}
		break;

		// 잡화 상점
		case POPUP_ETCMERCHANT:
		case POPUP_DARKETCMERCHANT:
		case POPUP_A_ETCMERCHANT:
		case POPUP_A_DARKETCMERCHANT :
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 5 ][ i ], TRUE ) ;
			}
		}
		break;

		// 모험가 트레이너
		case POPUP_ADVENTURETRAINER:
		case POPUP_ADVANCEDADVENTURE:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 4 ][ i ], TRUE ) ;
			}
		}
		break;

		// 전사 트레이너
		case POPUP_FIGHTERTRAINER:
		case POPUP_ADVANCEDFIGHTER:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 3 ][ i ], TRUE ) ;
			}
		}
		break;

		// 로그 트레이너
		case POPUP_ROGUETRAINER:
		case POPUP_ADVANCEDROGUE:		
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 2 ][ i ], TRUE ) ;
			}
		}
		break;

		// 마법사 트레이너
		case POPUP_MAGETRAINER:
		case POPUP_ADVANCEDMAGE:	
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 0 ][ i ], TRUE ) ;
			}
		}
		break;

		// 성직자 트레이너
		case POPUP_ACOLYTETRAINER:
		case POPUP_ADVANCEDACOLYTEE:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 1 ][ i ], TRUE ) ;
			}
		}
		break;

		// 메달 상점
		case POPUP_MEDALSHOP:
		case POPUP_A_MEDALSHOP :
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 8 ][ i ], TRUE ) ;
			}
		}
		break;

		// 아칸 무기상
		case POPUP_A_WEAPONMERCHANT:
		case POPUP_A_DARKWEAPONMERCHANT :
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 9 ][ i ], TRUE ) ;
			}
		}
		break ;
		
		// 아칸 방어구상
		case POPUP_A_ARMOURMERCHANT:
		case POPUP_A_DARKARMOURMERCHANT :
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 10 ][ i ], TRUE ) ;
			}
		}
		break ;

		// 비기너 트레이너
		case POPUP_A_BEGINNERTRAINER:
		case POPUP_ADVANCEDBEGINNER:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 11 ][ i ], TRUE ) ;
			}
		}
		break ;

		// 컴배턴트 트레이너
		case POPUP_A_COMBATANTTRAINER:
		case POPUP_ADVANCEDCOMBATANT:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 12 ][ i ], TRUE ) ;
			}
		}
		break ;

		// 오피세이터 트레이너
		case POPUP_A_OPPICATERTRAINER:
		case POPUP_ADVANCEDOPPICATER:
		{
			for ( int i = 0 ; i < 4 ; i ++ )
			{
				lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
				lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ 13 ][ i ], TRUE ) ;
			}
		}
		break ;
	}
}

VOID	CRYLTradeDlg::InitTrade( DWORD dwNPCID, unsigned char cMenuIndex, BOOL bRandomOption )
{
	InitValue() ;
	m_dwNPCID = dwNPCID;
	
	CRYLGameData::Instance()->m_dwTradingNpcID = dwNPCID ;

	NPCNode* lpNpcNode		= g_NPCList.GetNPCNode( dwNPCID ) ;

	m_dwTradeTab			= 0 ;
	m_lpBuyItem				= NULL ;

	m_bBlackMarket			= FALSE ;
	m_bMedalMarket			= FALSE ;
	m_bSkillCouponMarket	= FALSE ;

	m_cRace					= cMenuIndex ;

	ChangeTabTitle( lpNpcNode->m_dwJob ) ;

	AttachWindowTexture( ( CGMTexture*)m_pImage1 ) ;
	for ( int i = 0 ; i < 8; i++ )
	{
		m_aryIsEquipItemInitialized[ i ] = FALSE;
	}
	m_bRandomOptionMarket = FALSE;

	switch( lpNpcNode->m_dwJob )
	{
		case POPUP_WEAPONMERCHANT :
		case POPUP_A_WEAPONMERCHANT:
			m_dwKindTitle = 0;
			m_pRepairButton->SetVisible( TRUE ) ;
			m_pAllRepairButton->SetVisible( TRUE ) ;
			m_bRandomOptionMarket = TRUE;
			break;

		case POPUP_ARMOURMERCHANT:
		case POPUP_A_ARMOURMERCHANT:
			m_dwKindTitle = 1;
			m_pRepairButton->SetVisible( TRUE ) ;
			m_pAllRepairButton->SetVisible( TRUE ) ;
			m_bRandomOptionMarket = TRUE;
			break;

		case POPUP_ETCMERCHANT:
		case POPUP_A_ETCMERCHANT:
			m_dwKindTitle = 2;
			m_pRepairButton->SetVisible( TRUE ) ;
			m_pAllRepairButton->SetVisible( TRUE ) ;
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
		case POPUP_A_MEDALSHOP:
			m_dwKindTitle = 9;
			m_bMedalMarket = TRUE;
            m_pPhaseLabel->SetVisible( FALSE );
            m_pPrevButton->SetVisible( FALSE );
            m_pNextButton->SetVisible( FALSE );
			m_pServiceMedalLabel->SetVisible( TRUE );
			m_pServiceMedalValueLabel->SetVisible( TRUE );
			AttachWindowTexture( ( CGMTexture*)m_pImage2 ) ;
			break;

		case POPUP_SKILLMERCHANT:
			m_dwKindTitle = 10;
			break;
	}

	memset( m_aryCurPhase, 0, sizeof(int) * 4 );
	memset( m_aryMaxPhase, 0, sizeof(int) * 4 );

	if ( IsRandomMarket() )
	{
		DeleteTradeItem() ;
		SetInitialRandomOpition();
		lpNpcNode->ChangeGradeOption();
        //lpNpcNode->ChangeEquipItemGradeOption();
		AddTradeEquipItemWindow( lpNpcNode, cMenuIndex, 0 );
		AddTradeEquipItemWindow( lpNpcNode, cMenuIndex, 1 );
		AddTradeEquipItemWindow( lpNpcNode, cMenuIndex, 2 );
		AddTradeEquipItemWindow( lpNpcNode, cMenuIndex, 3 );
		SetPhaseLabel();
//		SendPacket::CharEquipShopInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwNPCID, cMenuIndex, 0 );
		return;
	}
	InitTradeWindow( lpNpcNode, cMenuIndex ) ;
	SetPhaseLabel();
}
*/