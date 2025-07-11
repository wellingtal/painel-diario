#include <winsock2.h>
#include "GMUICustomDlg.h"
#include "RYLLargeMapDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLPicture.h"
#include "WinInput.h"
#include "RYLSpriteEx.h"
#include "RYLSprite.h"
#include "Texture.h"

#include "BaseDataDefine.h"
#include "RYLGameData.h"	
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"

#include "GMImageDib.h"
#include "RYLImage.h"	

#include "RYLClientMain.h"
#include "RYLMiniMapDlg.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLBattleGroundDlg.h"
#include "..\\CharStatus.h"

#include "BaseGraphicsLayer.h"
#include "..\\GUITextEdit.h"

#include "GMFont.h"
#include "GMFontTexture.h"
#include "RYLStringTable.h"

#include "RYLCamp.h"
#include "RYLCampManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h" 


#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendBGServer.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Creature/Character/CharacterClass.h>
#include <RenderOption.h>
#include "GMMemory.h"

CRYLLargeMapDlg*	g_pLargeMapDlg = NULL ;

CRYLLargeMapDlg::CRYLLargeMapDlg( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{

	m_pCampDistributeSprite[ 0 ] = NULL ;
	m_pCampDistributeSprite[ 1 ] = NULL ;
	m_pCampDistributeSprite[ 2 ] = NULL ;

	m_pCampAttackedSprite = NULL ;

	m_pGMUIParentDlg		= pParentDlg;
	m_bShowLargeMap			= FALSE ;
	m_bBattleRespawn		= FALSE ;
	m_SpawnNumber			= 3 ;
	m_WaitNum_Out[ 0 ]		= 0 ;
	m_WaitNum_Out[ 1 ]		= 0 ;
	m_WaitNum_Out[ 2 ]		= 0 ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	/*
	CRYLSpriteList::Instance()->CreateGMImage( 512, 13, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 50, 13 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;			
	SetRect( &rt, 50, 0, 150, 13 ) ;
	pImageDIB->DrawRect( &rt, 50,  0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;			
	pImageDIB->DrawRect( &rt, 150, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 250, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 350, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 194, 0, 256, 13 ) ;
	pImageDIB->DrawRect( &rt, 450, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
//	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	*/


	CRYLSpriteList::Instance()->CreateGMImage( 512, 512, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

    //-- 2004. 3. 12. Zergra From. --//
    CRYLSpriteList::Instance()->CreateGMImage( 29, 29, &pImageDIB );
    pImageDIB->ClearbyColor( 0x00000000 );
    SetRect( &rt, 72, 29,101, 58 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STONE_ICON_246x64 ) );
    m_pHumanStoneIcon = CRYLImageManager::Instance()->CreateRYLImage();
    m_pHumanStoneIcon->LoadFromImage( pImageDIB );
    GM_DELETE( pImageDIB );

    CRYLSpriteList::Instance()->CreateGMImage( 29, 29, &pImageDIB );
    pImageDIB->ClearbyColor( 0x00000000 );
    SetRect( &rt, 72,  0,101, 29 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STONE_ICON_246x64 ) );
    m_pAkhanStoneIcon = CRYLImageManager::Instance()->CreateRYLImage();
    m_pAkhanStoneIcon->LoadFromImage( pImageDIB );
    GM_DELETE( pImageDIB );

    CRYLSpriteList::Instance()->CreateGMImage( 29, 29, &pImageDIB );
    pImageDIB->ClearbyColor( 0x00000000 );
    SetRect( &rt,101,  0,130, 29 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STONE_ICON_246x64 ) );
    m_pNuetralStoneIcon = CRYLImageManager::Instance()->CreateRYLImage();
    m_pNuetralStoneIcon->LoadFromImage( pImageDIB );
    GM_DELETE( pImageDIB );

    CRYLSpriteList::Instance()->CreateGMImage( 72, 58, &pImageDIB );
    pImageDIB->ClearbyColor( 0x00000000 );
    SetRect( &rt,  0,  0, 72, 58 );    
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STONE_ICON_246x64 ) );
    m_pDangerIcon = CRYLImageManager::Instance()->CreateRYLImage();
    m_pDangerIcon->LoadFromImage( pImageDIB );
    GM_DELETE( pImageDIB );
    //-- Zergra To. --//   
	
	SetWidth( 512 ) ;
	SetHeight( 512 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
//SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pLargeMapDlg = this ;	
	InitCtrl() ;

	CGMFont::Instance()->CreateFontTexture( &m_pFontTexture ) ;	
}

CRYLLargeMapDlg::~CRYLLargeMapDlg ()
{
	GM_DELETE( m_pFontTexture ) ;
	FinalCtrl() ;
}

VOID	CRYLLargeMapDlg::InitCtrl ()
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 베틀그라운드 종료버튼
	m_pBattleExitButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pBattleExitButton->SetLeft( 430 ) ;
	m_pBattleExitButton->SetTop( 17 ) ;
	m_pBattleExitButton->SetCaption( CRYLStringTable::m_strString[ 1957 ] ) ;
	m_pBattleExitButton->SetFontColor( 0xffffffff) ;
	m_pBattleExitButton->SetColor( 0xffffffff) ;
	m_pBattleExitButton->OnClickEvent = DlgClick ;
	m_pBattleExitButton->SetInstance( RYLLARGEMAPDLG::BATTLEEXIT_BUTTON ) ;
	m_pBattleExitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pBattleExitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pBattleExitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pBattleExitButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pBattleExitButton->SetVisible( FALSE ) ;
	CGMUICustomPanel::AddChild( m_pBattleExitButton ) ;

	// -13

	//----------------------------------------------------------------------------------------
	m_HumanRespawn[ 0 ].iPutX			= 147 ;		 	
	m_HumanRespawn[ 0 ].iPutY			= 58 -13 ;
	m_HumanRespawn[ 0 ].v3Point.x		= 868.65f ;
	m_HumanRespawn[ 0 ].v3Point.y		= 57.28f ;
	m_HumanRespawn[ 0 ].v3Point.z		= 1397.06f ;
	
	m_HumanRespawn[ 1 ].iPutX			= 310 ;		 	
	m_HumanRespawn[ 1 ].iPutY			= 140 -13 ;
	m_HumanRespawn[ 1 ].v3Point.x		= 1217.44f ;
	m_HumanRespawn[ 1 ].v3Point.y		= 48.62f ;
	m_HumanRespawn[ 1 ].v3Point.z		= 1827.82f ;

	m_HumanRespawn[ 2 ].iPutX			= 117 ;		 	
	m_HumanRespawn[ 2 ].iPutY			= 247 -13 ;
	m_HumanRespawn[ 2 ].v3Point.x		= 1526.42f ;
	m_HumanRespawn[ 2 ].v3Point.y		= 33.20f ;
	m_HumanRespawn[ 2 ].v3Point.z		= 1685.55f ;

	//==========================================================
	m_AkhanRespawn[ 0 ].iPutX			= 392 ;		 	
	m_AkhanRespawn[ 0 ].iPutY			= 240 -13 ;
	m_AkhanRespawn[ 0 ].v3Point.x		= 1310.52f ;
	m_AkhanRespawn[ 0 ].v3Point.y		= 28.53f ;
	m_AkhanRespawn[ 0 ].v3Point.z		= 1168.81f ;

	m_AkhanRespawn[ 1 ].iPutX			= 195 ;		 	
	m_AkhanRespawn[ 1 ].iPutY			= 358 -13 ;
	m_AkhanRespawn[ 1 ].v3Point.x		= 1619.65f ;
	m_AkhanRespawn[ 1 ].v3Point.y		= 49.31f ;
	m_AkhanRespawn[ 1 ].v3Point.z		= 998.32f ;

	m_AkhanRespawn[ 2 ].iPutX			= 348 ;		 	
	m_AkhanRespawn[ 2 ].iPutY			= 445 -13 ;
	m_AkhanRespawn[ 2 ].v3Point.x		= 1976.27f ;
	m_AkhanRespawn[ 2 ].v3Point.y		= 53.56f ;
	m_AkhanRespawn[ 2 ].v3Point.z		= 1464.75f ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	for ( INT i = 0 ; i < 3 ; i ++ )
	{
		m_pHumanSpawnButton[ i ] = new CRYLButton( m_pGMUIParentDlg ) ;		// 휴먼 리스폰 버튼
		m_pHumanSpawnButton[ i ]->SetLeft( m_HumanRespawn[ i ].iPutX ) ;
		m_pHumanSpawnButton[ i ]->SetTop( m_HumanRespawn[ i ].iPutY ) ;
		m_pHumanSpawnButton[ i ]->OnClickEvent = DlgClick ;
		m_pHumanSpawnButton[ i ]->SetInstance( RYLLARGEMAPDLG::HUMANRESPAWNPOS1_BUTTON + ( i * 1 ) ) ;
		SetRect( &rt, 80, 84, 93, 97) ;
		m_pHumanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pHumanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pHumanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pHumanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		m_pHumanSpawnButton[ i ]->SetVisible( FALSE ) ;
		CGMUICustomPanel::AddChild( m_pHumanSpawnButton[ i ] ) ;

		// 휴먼 리스폰 스프라이트
		m_pHumanSpawnSprite[ i ] = new CRYLPicture( m_pGMUIParentDlg ) ;
		m_pHumanSpawnSprite[ i ]->SetLeft( m_HumanRespawn[ i ].iPutX - 7 ) ;
		m_pHumanSpawnSprite[ i ]->SetTop( m_HumanRespawn[ i ].iPutY - 18 ) ;
		// Bounus Str Image
		CRYLSpriteList::Instance()->CreateGMImage( 26, 24, &pImageDIB ) ;
		pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 79 + ( i * 24 ), 26, 103 + ( i * 24 ) ) ;
		pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
		m_pHumanSpawnSprite[ i ]->AttachImageTexture( (CGMTexture* )pImage ) ;
		GM_DELETE( pImageDIB ) ;
		m_pHumanSpawnSprite[ i ]->SetVisible( FALSE ) ;
		CGMUICustomPanel::AddChild( m_pHumanSpawnSprite[ i ] ) ;
	}
	
	
	// 아칸 리스폰 버튼
	for ( INT i = 0 ; i < 3 ; i ++ )
	{
		m_pAkhanSpawnButton[ i ] = new CRYLButton( m_pGMUIParentDlg ) ;		// 휴먼 리스폰 버튼
		m_pAkhanSpawnButton[ i ]->SetLeft( m_AkhanRespawn[ i ].iPutX ) ;
		m_pAkhanSpawnButton[ i ]->SetTop( m_AkhanRespawn[ i ].iPutY ) ;
		m_pAkhanSpawnButton[ i ]->OnClickEvent = DlgClick ;
		m_pAkhanSpawnButton[ i ]->SetInstance( RYLLARGEMAPDLG::AKHANRESPAWNPOS1_BUTTON + ( i * 1 ) ) ;
		SetRect( &rt, 93, 84, 106, 97) ;
		m_pAkhanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pAkhanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pAkhanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pAkhanSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		m_pAkhanSpawnButton[ i ]->SetVisible( FALSE ) ;
		CGMUICustomPanel::AddChild( m_pAkhanSpawnButton[ i ] ) ;

		// 아칸 리스폰 스프라이트
		m_pAkhanSpawnSprite[ i ] = new CRYLPicture( m_pGMUIParentDlg ) ;
		m_pAkhanSpawnSprite[ i ]->SetLeft( m_AkhanRespawn[ i ].iPutX - 7 ) ;
		m_pAkhanSpawnSprite[ i ]->SetTop( m_AkhanRespawn[ i ].iPutY - 18 ) ;
		// Bounus Str Image
		CRYLSpriteList::Instance()->CreateGMImage( 26, 24, &pImageDIB ) ;
		pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 79 + ( i * 24 ), 26, 103 + ( i * 24 ) ) ;
		pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
		pImage->LoadFromImage( pImageDIB ) ;
		CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
		m_pAkhanSpawnSprite[ i ]->AttachImageTexture( (CGMTexture* )pImage ) ;
		GM_DELETE( pImageDIB ) ;
		m_pAkhanSpawnSprite[ i ]->SetVisible( FALSE ) ;
		CGMUICustomPanel::AddChild( m_pAkhanSpawnSprite[ i ] ) ;
	}


	INT iSize;
	DWORD* pBits = NULL ;
	// 카르테란트 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_KARTERANT ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[0] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[0]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[0]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[0]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[0]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[0]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[0]->m_pTexture ) ;

	// 메르카디아 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_MERKADIA ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[1] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[1]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[1]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[1]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[1]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[1]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[1]->m_pTexture ) ;

	// 제삼세력 길드 요새 영역 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_THIRD ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampDistributeSprite[2] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampDistributeSprite[2]->m_pSprite = new CRYLSprite ;
	m_pCampDistributeSprite[2]->m_pTexture = new CTexture ;
	m_pCampDistributeSprite[2]->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampDistributeSprite[2]->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampDistributeSprite[2]->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampDistributeSprite[2]->m_pTexture ) ;

	// 길드 요새 공격 받을 시 스프라이트
	pImageDIB = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CAMP_ATTACKED ) ;
	pImageDIB->GetBits( (VOID**)&pBits, iSize ) ;

	m_pCampAttackedSprite = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pCampAttackedSprite->m_pSprite = new CRYLSprite ;
	m_pCampAttackedSprite->m_pTexture = new CTexture ;
	m_pCampAttackedSprite->m_pTexture->CreateEmptyTexture( 128, 128, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED ) ;
	m_pCampAttackedSprite->m_pTexture->SetBitTexture32( 94, 94, ( DWORD* )pBits ) ;
	m_pCampAttackedSprite->m_pSprite->Create( 0, 0, 0, 0, 94, 94, m_pCampAttackedSprite->m_pTexture ) ;


	
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	
	m_pLargeMapSprite[ 0 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pLargeMapSprite[ 0 ]->m_pSprite = new CRYLSprite ;
	m_pLargeMapSprite[ 0 ]->m_pTexture = new CTexture ;
	m_pLargeMapSprite[ 0 ]->SetLeft( 0 ) ;
	m_pLargeMapSprite[ 0 ]->SetTop( 0 ) ;
	m_pLargeMapSprite[ 0 ]->SetWidth( 256 ) ;
	m_pLargeMapSprite[ 0 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pLargeMapSprite[ 0 ] ) ;

/*	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	
	m_pLargeMapSprite[ 0 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pLargeMapSprite[ 0 ]->m_pSprite = new CRYLSprite ;
	m_pLargeMapSprite[ 0 ]->m_pTexture = new CTexture ;
	m_pLargeMapSprite[ 0 ]->SetLeft( 0 ) ;
	m_pLargeMapSprite[ 0 ]->SetTop( 0 ) ;
	m_pLargeMapSprite[ 0 ]->SetWidth( 402 ) ;
	m_pLargeMapSprite[ 0 ]->SetHeight( 497 ) ;
	CGMUICustomPanel::AddChild( m_pLargeMapSprite[ 0 ] ) ;//guigui*/


	m_pLargeMapSprite[ 1 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pLargeMapSprite[ 1 ]->m_pSprite = new CRYLSprite ;
	m_pLargeMapSprite[ 1 ]->m_pTexture = new CTexture ;
	m_pLargeMapSprite[ 1 ]->SetLeft( 256 ) ;
	m_pLargeMapSprite[ 1 ]->SetTop( 0 ) ;
	m_pLargeMapSprite[ 1 ]->SetWidth( 256 ) ;
	m_pLargeMapSprite[ 1 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pLargeMapSprite[ 1 ] ) ;

	m_pLargeMapSprite[ 2 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pLargeMapSprite[ 2 ]->m_pSprite = new CRYLSprite ;
	m_pLargeMapSprite[ 2 ]->m_pTexture = new CTexture ;
	m_pLargeMapSprite[ 2 ]->SetLeft( 0 ) ;
	m_pLargeMapSprite[ 2 ]->SetTop( 256 ) ;
	m_pLargeMapSprite[ 2 ]->SetWidth( 256 ) ;
	m_pLargeMapSprite[ 2 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pLargeMapSprite[ 2 ] ) ;

	m_pLargeMapSprite[ 3 ] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pLargeMapSprite[ 3 ]->m_pSprite = new CRYLSprite ;
	m_pLargeMapSprite[ 3 ]->m_pTexture = new CTexture ;
	m_pLargeMapSprite[ 3 ]->SetLeft( 256 ) ;
	m_pLargeMapSprite[ 3 ]->SetTop( 256 ) ;
	m_pLargeMapSprite[ 3 ]->SetWidth( 256 ) ;
	m_pLargeMapSprite[ 3 ]->SetHeight( 256 ) ;
	CGMUICustomPanel::AddChild( m_pLargeMapSprite[ 3 ] ) ;
}

VOID	CRYLLargeMapDlg::FinalCtrl ()
{
	INT i = 0 ;

	GM_DELETE( m_pBattleExitButton ) ;	

	for ( i = 0 ; i < 3 ; i ++ )
	{
		GM_DELETE( m_pHumanSpawnButton[ i ] ) ;
		GM_DELETE( m_pHumanSpawnSprite[ i ] ) ;
		GM_DELETE( m_pAkhanSpawnButton[ i ] ) ;
		GM_DELETE( m_pAkhanSpawnSprite[ i ] ) ;
	}

	for ( i = 0 ; i < 4 ; i ++ )
		GM_DELETE( m_pLargeMapSprite[ i ] ) ;	

	GM_DELETE( m_pCampDistributeSprite[0] ) ;
	GM_DELETE( m_pCampDistributeSprite[1] ) ;
	GM_DELETE( m_pCampDistributeSprite[2] ) ;

	GM_DELETE( m_pCampAttackedSprite ) ;
}

VOID	CRYLLargeMapDlg::Show( CGMUIObject* Sender )
{
	
}	
VOID	CRYLLargeMapDlg::Hide( CGMUIObject* Sender )
{
}

VOID	CRYLLargeMapDlg::GMUIPaint( INT xx, INT yy )
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
	
	lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;

	CRYLGameData*	    pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance() ;

	INT left = m_pGMUIParentDlg->GetLeft() + GetLeft();
	INT top  = m_pGMUIParentDlg->GetTop() + GetTop();

    if ( pNetwork->m_dwPresentZone == 13 )//was 13 hayzohar
    {
        POINT   ptStonePos[ 3 ] = { { left + 348, top + 15 + 333 - 13 }, 
                                    { left + 246, top + 15 + 234 - 13 },
                                    { left + 144, top + 15 + 131 - 13 } };

        for ( int i = 0 ; i < 3 ; ++i )
        {
            switch ( m_StoneState[ i ].wKID )
            {
				case 1040:
				case 1041:
                {
                    // Human
                    m_pHumanStoneIcon->Draw( ptStonePos[ i ].x, ptStonePos[ i ].y );
                }
                break;

				case 1042:
				case 1043:
                {
                    // Akhan
                    m_pAkhanStoneIcon->Draw( ptStonePos[ i ].x, ptStonePos[ i ].y );
                }
                break;

	            case 1039://was 1039 hayzohar
                {
                    // Nuetral 
                    m_pNuetralStoneIcon->Draw( ptStonePos[ i ].x, ptStonePos[ i ].y );
                }
                break;
            }

            if ( m_StoneState[ i ].cState == 2 )
            {
                m_pDangerIcon->Draw( ptStonePos[ i ].x + 15, ptStonePos[ i ].y - 48 );
            }
        }
    }	


	// Minotaurs 길드요새 Draw
	DrawCampArea( xx, yy ) ;
	DrawGuildInfo( xx, yy ) ;
}

HRESULT		CRYLLargeMapDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	return S_OK ;
}

VOID	CRYLLargeMapDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pLargeMapDlg )
		return ;

	POS		Pos ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	CRYLCommunityData* pCommunity = CRYLCommunityData::Instance() ;

	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLBattleGroundDlg* pBattleDlg = ( CRYLBattleGroundDlg* )pScene->GetBattleGroundDlg() ;
	
	switch ( Sender->GetInstance() )
	{
	case RYLLARGEMAPDLG::HUMANRESPAWNPOS1_BUTTON :
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn( g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Human2, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::HUMANRESPAWNPOS2_BUTTON :	
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn(g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Human3, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::HUMANRESPAWNPOS3_BUTTON :
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn(g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Human1, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::AKHANRESPAWNPOS1_BUTTON :	
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn(g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Akhan3, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::AKHANRESPAWNPOS2_BUTTON :
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn(g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Akhan1, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::AKHANRESPAWNPOS3_BUTTON :	
		{
			CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			SendPacket::CharRespawn(g_GameSession,  pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_Akhan2, Pos, 0 ) ;
			pCommunity->SetStoneStatueRespawn( FALSE ) ;
			g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
			g_pLargeMapDlg->GetParentDlg()->SetVisible( FALSE ) ;
			pBattleDlg->SetRespawnWait( TRUE ) ;
			pBattleDlg->SetWarQuit( FALSE ) ;
		}
		break ;
	case RYLLARGEMAPDLG::BATTLEEXIT_BUTTON		 :
		{
			if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || 
				CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE )
            {
                SendPacket::CharBGServerMoveZone( g_GameSession, 0, 0 );
            }
            else
            {
			    POS pPos = { 0.0f, 0.0f, 0.0f };
			    if ( pGame->m_dwClientRace == CClass::RaceType::HUMAN )
			    {
				    pPos.fPointX = 1403.06f;
				    pPos.fPointY = 18.25f;
				    pPos.fPointZ = 1958.41f;
			    } else if ( pGame->m_dwClientRace == CClass::RaceType::AKHAN )
			    {
				    pPos.fPointX = 2014.29f;
				    pPos.fPointY = 63.50f;
				    pPos.fPointZ = 1648.47f;
			    }

				CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ;
			    SendPacket::CharRespawn(g_GameSession, pNetwork->m_dwMyChrID, ClientConstants::RespawnCmd_BG_Exit, pPos, 0);
			    g_pLargeMapDlg->SetShowLargeMap( FALSE ) ;
            }
		}
		break ;	
	}
}

VOID	CRYLLargeMapDlg::LoadLargeMapTexture()
{
	CTexture::SetPath(INTERFACETEXTUREPATH);
	CHAR	strFile[ MAX_PATH ] ;

	unsigned char cLargeMapZone = CRYLNetworkData::Instance()->m_dwPresentZone;
	switch (cLargeMapZone)
	{
		case SERVER_ID::PRISON:				cLargeMapZone = 100;					break;
	}

	for ( unsigned long i = 0 ; i < 4 ; i ++ )
	{
        sprintf( strFile, "Zone%d\\LargeMap%02d.dds", cLargeMapZone, i + 1 ) ;

		if ( m_pLargeMapSprite[ i ]->m_pTexture->GetTexture()) 
		{
			m_pLargeMapSprite[ i ]->m_pTexture->Unload() ;
		}

		m_pLargeMapSprite[ i ]->m_pTexture->Load(strFile);
		m_pLargeMapSprite[ i ]->m_pSprite->Create( 0, 0, 0, 0, 256, 256, m_pLargeMapSprite[ i ]->m_pTexture ) ;
	}
}


VOID	CRYLLargeMapDlg::SetShowLargeMap( BOOL bShowLargeMap )	
{
	m_bShowLargeMap = bShowLargeMap ;

	m_bBattleRespawn = FALSE;
	HideControl() ;
}

VOID	CRYLLargeMapDlg::HideControl()
{	
	m_pBattleExitButton->SetVisible( FALSE ) ;
	for ( INT i = 0 ;i < 3 ; i ++ )
	{
		switch ( CRYLGameData::Instance()->m_dwClientRace )
		{
		case CClass::RaceType::HUMAN : 
			m_pHumanSpawnButton[ i ]->SetVisible( FALSE ) ;		// 휴먼 리스폰 버튼
			m_pHumanSpawnSprite[ i ]->SetVisible( FALSE ) ;		// 휴먼 리스폰 스프라이트
			break ;
		case CClass::RaceType::AKHAN :
			m_pAkhanSpawnButton[ i ]->SetVisible( FALSE ) ;		// 아칸 리스폰 버튼
			m_pAkhanSpawnSprite[ i ]->SetVisible( FALSE ) ;		// 아칸 리스폰 스프라이트
			break ;
		}
	}	
}


VOID	CRYLLargeMapDlg::ShowLargeMap( USHORT& pWaitNum_Out ) //large map show hayzohar
{
	*m_WaitNum_Out = pWaitNum_Out ;
	m_bShowLargeMap = TRUE ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;
	
	 // hayzohar was ( ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE) &&  pSelfCreature->GetIsDead() )
	if ( ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE) &&  pSelfCreature->GetIsDead() )
	{
		m_bBattleRespawn = TRUE ;
		
		if ( !CRYLGameData::Instance()->m_bMouseMode )
		{
			CRYLGameData::Instance()->m_bShowCursor = TRUE ;
		}
	
		m_pBattleExitButton->SetVisible( TRUE ) ;
		for ( INT i = 0 ;i < 3 ; i ++ )
		{
			switch ( CRYLGameData::Instance()->m_dwClientRace )
			{
				case CClass::NationType::KR : 
					m_pHumanSpawnButton[ i ]->SetVisible( TRUE ) ;		// 휴먼 리스폰 버튼
					m_pHumanSpawnSprite[ i ]->SetVisible( TRUE ) ;		// 휴먼 리스폰 스프라이트
					break ;
				case CClass::NationType::MK :
					m_pAkhanSpawnButton[ i ]->SetVisible( TRUE ) ;		// 아칸 리스폰 버튼
					m_pAkhanSpawnSprite[ i ]->SetVisible( TRUE ) ;		// 아칸 리스폰 스프라이트
					break ;

					
			}
		}
	}
	// for Battle Ground Client
    if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE )
    {
        if ( !CRYLGameData::Instance()->m_bMouseMode )
		{
			CRYLGameData::Instance()->m_bShowCursor = TRUE ;
		}

        m_pBattleExitButton->SetVisible( TRUE ) ;
	}
}

//---------------------------------------------------------------------------------------------------------------
// CRYLLargeMapDlg::SetStoneState()
VOID CRYLLargeMapDlg::SetStoneState( unsigned char cIndex, unsigned wKID, unsigned char cState )
{
    m_StoneState[ cIndex ].wKID     = wKID;
    m_StoneState[ cIndex ].cState   = cState;
}

void	CRYLLargeMapDlg::DrawCampArea( INT xx, INT yy )
{
	std::map< unsigned long, CRYLCamp* > Node = CRYLCampManager::Instance()->GetNode() ;
	std::map< unsigned long, CRYLCamp* >::iterator iter = Node.begin() ;
	long lX, lY ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();
	INT iIndex ;
	RECT rt ;

	while( iter != Node.end() )
	{
		CRYLCamp* pCamp = iter->second ;
		if ( pCamp && 0 != pCamp->GetCampNation() )
		{
			switch(	CRYLNetworkData::Instance()->m_dwPresentZone)
			{
				case SERVER_ID::ZONE1:
				//	lX = (long)(pCamp->GetPointX() / 3465.0f * 512) - 48;
				//	lY = (long)( 512 - pCamp->GetPointZ() / 3465.0f * 512) - 46 ;
					lX = (long)( (long)(pCamp->GetPointX() - 315) * 512 / 3465.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 315) * 512 / 3465.0f ) - 47 ;
					break;

				case SERVER_ID::ZONE2:
					lX = (long)(pCamp->GetPointX() / 3422.0f * 512) - 52;
					lY = (long)( 512 - pCamp->GetPointZ() / 3438.0f * 512) + 52;
					break;

				case SERVER_ID::ZONE4:
					lX = (long)(pCamp->GetPointX() / 2463.0f * 512) - 68;
					lY = (long)( 512 - pCamp->GetPointZ() / 2383.0f * 512) + 83;
					break;

				case SERVER_ID::ZONE5:
					lX = (long)(pCamp->GetPointX() / 3120.0f * 512) - 53;
					lY = (long)( 512 - pCamp->GetPointZ() / 3104 * 512) + 55;
					break;

				case SERVER_ID::CAPITAL :
				case SERVER_ID::ZONE12 :
				{
					lX = (long)( (long)(pCamp->GetPointX() - 315) * 512 / 3465.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 315) * 512 / 3465.0f ) - 47 ;
					break;
				}
				case SERVER_ID::ZONE9 :
				{
					lX = (long)( (long)(pCamp->GetPointX() - 630) * 512 / 3150.0f ) - 47 ;
					lY = (long)( 512 - (long)(pCamp->GetPointZ() - 630) * 512 / 3150.0f ) - 47 ;
					break;
				}
			}

			SetRect( &rt, 0, 0, 94, 94 ) ;

			if ( (lX + 94) >= 512 ) rt.right -= (lX + 94) - 512 ;
			if ( (lY + 94) >= 512 ) rt.bottom -= (lY + 94) - 512 ;

			if ( lX <= 0 )
			{
				rt.left -= lX ;
				lX = 0 ;
			}
			if ( lY <= 0 ) 				
			{
				rt.top -= lY ;
				lY = 0 ;
			}

			if ( lX >= 512 || lY >= 512 ||
				 lX + rt.right <= 0 ||
				 lY + rt.bottom <= 0 )
			{
				++ iter ;
				continue;
			}

			switch ( pCamp->GetCampNation() )
			{
				case Creature::KARTERANT:			iIndex = 0 ;	break;
				case Creature::MERKADIA:			iIndex = 1 ;	break;
				case Creature::ALMIGHTY_PIRATE:		iIndex = 2 ;	break;
			}

			if ( Siege::DEVELOPING == pCamp->GetState() )
			{
				pCamp->UpdateAlpha() ;
				if ( pCamp->GetAlpha() > 0 )
				{
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutX = xx + lX ;
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutY = yy + lY ;
					m_pCampDistributeSprite[ iIndex ]->m_pSprite->Render( lpD3DDevice, pCamp->GetAlpha(), &rt ) ;
				}
			}
			else
			{
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutX = xx + lX ;
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->m_nPutY = yy + lY ;
				m_pCampDistributeSprite[ iIndex ]->m_pSprite->Render( lpD3DDevice, pCamp->GetDefaultAlpha(), &rt ) ;
			}

			if ( pCamp->GetAttacked() )
			{
				pCamp->UpdateAttackedAlpha() ;
				if ( pCamp->GetAttackedAlpha() > 0 )
				{
					m_pCampAttackedSprite->m_pSprite->m_nPutX = xx + lX ;
					m_pCampAttackedSprite->m_pSprite->m_nPutY = yy + lY ;
					m_pCampAttackedSprite->m_pSprite->Render( lpD3DDevice, pCamp->GetAttackedAlpha(), &rt ) ;
				}
			}
		}

		++ iter ;
	}
}

void	CRYLLargeMapDlg::DrawGuildInfo( INT xx, INT yy )
{
	std::map< unsigned long, CRYLCamp* > Node = CRYLCampManager::Instance()->GetNode() ;
	std::map< unsigned long, CRYLCamp* >::iterator iter = Node.begin() ;
	RECT rcWeapon, rcName, rcMark;
	long lX, lY ;
	unsigned long dwLength ;
	int iHalfSize ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();
	CRYLSprite* pMarkSprite = NULL ;

	while ( iter != Node.end() )
	{
		CRYLCamp* pCamp = iter->second ;
		if ( pCamp )
		{
			switch(	CRYLNetworkData::Instance()->m_dwPresentZone)
			{
				case SERVER_ID::ZONE1:
				//	lX = (long)(pCamp->GetPointX() / 3465.0f * 512) - 48;
				//	lY = (long)( 512 - pCamp->GetPointZ() / 3465.0f * 512) - 46 ;
					lX = (long)( ( (long)(pCamp->GetPointX()) - 315 ) * 512 / 3465.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 315 ) * 512 / 3465.0f ) ;
					break;

				case SERVER_ID::ZONE2:
					lX = (long)(pCamp->GetPointX() / 3422.0f * 512) - 52;
					lY = (long)( 512 - pCamp->GetPointZ() / 3438.0f * 512) + 52;
					break;

				case SERVER_ID::ZONE4:
					lX = (long)(pCamp->GetPointX() / 2463.0f * 512) - 68;
					lY = (long)( 512 - pCamp->GetPointZ() / 2383.0f * 512) + 83;
					break;

				case SERVER_ID::ZONE5:
					lX = (long)(pCamp->GetPointX() / 3120.0f * 512) - 53;
					lY = (long)( 512 - pCamp->GetPointZ() / 3104 * 512) + 55;
					break;

				case SERVER_ID::CAPITAL :
				case SERVER_ID::ZONE12 :
				{
					lX = (long)( ( (long)(pCamp->GetPointX()) - 315 ) * 512 / 3465.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 315 ) * 512 / 3465.0f ) ;
					break ;
				}
				case SERVER_ID::ZONE9 :
				{
					lX = (long)( ( (long)(pCamp->GetPointX()) - 630 ) * 512 / 3150.0f ) ;
					lY = (long)( 512 - ( (long)(pCamp->GetPointZ()) - 630 ) * 512 / 3150.0f ) ;
					break ;
				}
			}

			// 월드 웨폰은 월드 웨폰이라고 출력해준다.
			bool bWorldWeaponRender = false;
			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
			if ( pTempCreature )
			{
				if ( Siege::KARTERANT_WEAPON == pTempCreature->GetObjectType() ||
					 Siege::MERKADIA_WEAPON == pTempCreature->GetObjectType() )
				{
					dwLength = CRYLStringTable::GetStringWidth( CRYLStringTable::m_strString[ 3480 ] ) ;
					iHalfSize = dwLength / 2 ;

					// 월드 웨폰 출력
					SetRect( &rcWeapon, lX - iHalfSize, lY - 54, lX + iHalfSize, lY - 24 ) ;
					if ( rcWeapon.right > 512 )
					{
						rcWeapon.left -= rcWeapon.right - 512;
						rcWeapon.right = 512;
					}

					if ( rcWeapon.bottom > 512 )
					{
						rcWeapon.top -= rcWeapon.bottom - 512 - 15;
						rcWeapon.bottom = 512 - 15;
					}

					if ( rcWeapon.left < 0 )
					{
						rcWeapon.right += -rcWeapon.left;
						rcWeapon.left = 0;
					}

					if ( rcWeapon.top < 0 )
					{
						rcWeapon.bottom += -rcWeapon.top;
						rcWeapon.top = 0;
					}

					rcWeapon.left += xx ;
					rcWeapon.right += xx ;
					rcWeapon.top += yy ;
					rcWeapon.bottom += yy ;

					bWorldWeaponRender = true;
						
					++rcWeapon.left ;	++rcWeapon.right ;	++rcWeapon.top ;	++rcWeapon.bottom ;
					g_TextEdit.DrawText( CRYLStringTable::m_strString[ 3480 ], &rcWeapon, DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255) ) ;
					--rcWeapon.left ;	--rcWeapon.right ;	--rcWeapon.top ;	--rcWeapon.bottom ;
					g_TextEdit.DrawText( CRYLStringTable::m_strString[ 3480 ], &rcWeapon, DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255) ) ;
				}
			}

			// 길드명 && 길드 마크 출력
			const char* szGuildName = pPanel->GetGuildName( pCamp->GetGID() ) ;
			pMarkSprite = pPanel->GetGuildSprite( pCamp->GetGID() ) ;

			if ( szGuildName )
			{
				dwLength = CRYLStringTable::GetStringWidth( szGuildName ) ;
				
				if ( pMarkSprite )
				{
					iHalfSize = (dwLength + 3 + 18) / 2 ;
				}
				else
				{
					iHalfSize = dwLength / 2 ;
				}

				if ( bWorldWeaponRender )
				{
					SetRect( &rcName, lX - iHalfSize, lY - 39, lX + iHalfSize, lY - 9 ) ;

					if ( rcName.right > 512 )
					{
						rcName.left -= rcName.right - 512;
						rcName.right = 512;
					}

					if ( rcName.bottom > 512 )
					{
						rcName.top -= rcName.bottom - 512;
						rcName.bottom = 512;
					}

					if ( rcName.left < 0 )
					{
						rcName.right += -rcName.left;
						rcName.left = 0;
					}

					if ( rcName.top < 0 )
					{
						rcName.bottom += -rcName.top + 15;
						rcName.top = 15;
					}
				}
				else
				{
					SetRect( &rcName, lX - iHalfSize, lY - 39, lX + iHalfSize, lY - 9 ) ;

					if ( rcName.right > 512 )
					{
						rcName.left -= rcName.right - 512;
						rcName.right = 512;
					}

					if ( rcName.bottom > 512 )
					{
						rcName.top -= rcName.bottom - 512;
						rcName.bottom = 512;
					}

					if ( rcName.left < 0 )
					{
						rcName.right += -rcName.left;
						rcName.left = 0;
					}

					if ( rcName.top < 0 )
					{
						rcName.bottom += -rcName.top;
						rcName.top = 0;
					}
				}
				
				rcName.left += xx ;
				rcName.right += xx ;
				rcName.top += yy ;
				rcName.bottom += yy ;

				// 길드명 출력
				++rcName.left ;	++rcName.right ;	++rcName.top ;	++rcName.bottom ;
				g_TextEdit.DrawText( szGuildName, &rcName, DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255) ) ;
				--rcName.left ;	--rcName.right ;	--rcName.top ;	--rcName.bottom ;
				g_TextEdit.DrawText( szGuildName, &rcName, DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255) ) ;
				
				if ( pMarkSprite )
				{
					SetRect( &rcMark, 0, 0, 18, 12 ) ;
					pMarkSprite->SetPosition( xx + lX + iHalfSize - 18, yy + lY - 29 ) ;

					// Mark Rect Clipping
					if ( lX + iHalfSize >= 512 )
					{
						rcMark.right -= (lX + iHalfSize) - 512 ;
					}
					if ( lY - 28 + 12 >= 512 )
					{
						rcMark.bottom -= (lY - 28 + 12) - 512 ;
					}

					if ( lX + iHalfSize - 18 < 0 )
					{
						rcMark.left -= (lX + iHalfSize - 18) ;
						pMarkSprite->m_nPutX = xx ;
					}
					if ( lY - 28 < 0 )
					{
						rcMark.top -= (lY - 28) ;
						pMarkSprite->m_nPutY = yy ;
					}

					if ( rcMark.left >= 0 && rcMark.top >= 0 &&
						rcMark.right <= 512 && rcMark.bottom <= 512 )
					{
						// 마크 출력
						pMarkSprite->Render( lpD3DDevice, 0xFF, &rcMark ) ;
					}
				}
			}
		}

		++ iter ;
	}
}
