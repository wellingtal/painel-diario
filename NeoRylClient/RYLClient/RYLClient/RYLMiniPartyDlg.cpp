#include <winsock2.h>

#include "RYLMiniPartyDlg.h"
#include "GMUICustomDlg.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLPicture.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"
#include "WinInput.h"
#include <RenderOption.h>

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLEnchantDlg.h"

#include <Skill/SkillMgr.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "GMMemory.h"

CRYLMiniPartyMemberSlot* g_pMiniPartyMember = NULL ;

CRYLMiniPartyMemberSlot::CRYLMiniPartyMemberSlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pMemberLabel			= NULL ;			// 파티멤버 이름
	m_pHPProgrss			= NULL ;			// 체력 프로그래스바
	m_pMPProgrss			= NULL ;			// 마법 프로그래스바
	m_iMemberSlotIndex		= 0 ;				// 멤버슬롯 인덱스
	m_pNormalLargeImage			= NULL ;			// 일반 멤버
	m_pLeaderLargeImage			= NULL ;			// 리더 멤버
	m_pDamageLargeImage			= NULL ;			// 데미지 입은 멤버
	m_pNormalSmallImage			= NULL ;			// 일반 멤버
	m_pLeaderSmallImage			= NULL ;			// 리더 멤버
	m_pDamageSmallImage			= NULL ;			// 데미지 입은 멤버
    m_pOtherZoneImage       = NULL ;            // 다른 존 표시 이미지

	m_bShowShort = FALSE;

	m_pGMUIParentDlg   = pParentDlg ;
	g_pMiniPartyMember = this ;
}

CRYLMiniPartyMemberSlot::~CRYLMiniPartyMemberSlot() 
{
	GM_DELETE( m_pMemberLabel ) ;
	GM_DELETE( m_pHPProgrss ) ;
	GM_DELETE( m_pMPProgrss	) ;
    GM_DELETE( m_pOtherZoneImage );
}

VOID	CRYLMiniPartyMemberSlot::MakePartyMemberSlot( LPRECT lprt ) 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	
	// Normal Image
	CRYLSpriteList::Instance()->CreateGMImage( 155, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 14, 155, 34 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pNormalLargeImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pNormalLargeImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pNormalLargeImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Leader Image
	CRYLSpriteList::Instance()->CreateGMImage( 155, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 34, 155, 54 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pLeaderLargeImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLeaderLargeImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLeaderLargeImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Damage Image
	CRYLSpriteList::Instance()->CreateGMImage( 155, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 54, 155, 74 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pDamageLargeImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pDamageLargeImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pDamageLargeImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Normal Image
	CRYLSpriteList::Instance()->CreateGMImage( 109, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 147, 189, 256, 209 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pNormalSmallImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pNormalSmallImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pNormalSmallImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Leader Image
	CRYLSpriteList::Instance()->CreateGMImage( 109, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 147, 209, 256, 229 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pLeaderSmallImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLeaderSmallImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLeaderSmallImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Damage Image
	CRYLSpriteList::Instance()->CreateGMImage( 109, 20, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 147, 229, 256, 249 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImageDIB->DrawRect( lprt, 6, 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pDamageSmallImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pDamageSmallImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pDamageSmallImage ) ;
	GM_DELETE( pImageDIB ) ;

    // 다른 존에 가 있는 이미지
    m_pOtherZoneImage = new CRYLPicture( this->GetParentDlg() ) ;
    m_pOtherZoneImage->SetLeft(117) ;
    m_pOtherZoneImage->SetTop(5) ;	
    
    CRYLSpriteList::Instance()->CreateGMImage( 25, 11, &pImageDIB ) ;
    pImageDIB->ClearbyColor( 0x00000000 ) ;
    SetRect( &rt, 220, 84, 245, 95 ) ;
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;    
    
    CRYLImage* pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
    pImage->LoadFromImage( pImageDIB ) ;
    CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
    m_pOtherZoneImage->AttachImageTexture( (CGMTexture* )pImage ) ;
    GM_DELETE( pImageDIB ) ;
    CGMUICustomPanel::AddChild( m_pOtherZoneImage ) ;

    m_pOtherZoneImage->SetVisible( FALSE );

	AttachPanelImage( ( CGMTexture*)m_pNormalLargeImage ) ;

	m_pMemberLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMemberLabel->SetLeft( 26 ) ;
	m_pMemberLabel->SetTop( 5 ) ;
	m_pMemberLabel->SetFontColor( 0xffffffff ) ;
	m_pMemberLabel->SetAutoSize( TRUE ) ;
	m_pMemberLabel->SetCaption( "" ) ;
	CGMUICustomPanel::AddChild( m_pMemberLabel ) ;
}

VOID		CRYLMiniPartyMemberSlot::SetPartyMemberName( CHAR* pszName ) 
{
	m_pMemberLabel->SetCaption( pszName ) ;
}

VOID		CRYLMiniPartyMemberSlot::SetPartyMemberState( INT iState ) 
{
	switch ( iState )
	{
	case 0 :
		if(m_bShowShort)
			AttachPanelImage( ( CGMTexture*)m_pNormalSmallImage ) ;
		else
			AttachPanelImage( ( CGMTexture*)m_pNormalLargeImage ) ;
		break ;
	case 1 :
		if(m_bShowShort)
			AttachPanelImage( ( CGMTexture*)m_pLeaderSmallImage ) ;
		else
			AttachPanelImage( ( CGMTexture*)m_pLeaderLargeImage ) ;
		break ;
	case 2 :
		if(m_bShowShort)
			AttachPanelImage( ( CGMTexture*)m_pDamageSmallImage ) ; 
		else
			AttachPanelImage( ( CGMTexture*)m_pDamageLargeImage ) ; 
		break ;
	}	
}

VOID		CRYLMiniPartyMemberSlot::SetHPProgress( UINT uiCurrentHP, UINT uiMaxHP ) 
{
    m_pOtherZoneImage->SetVisible(FALSE);

	m_pHPProgrss->SetMax( uiMaxHP ) ;
	m_pHPProgrss->SetPosition( uiCurrentHP ) ;
    m_pHPProgrss->SetVisible(TRUE);
}

VOID		CRYLMiniPartyMemberSlot::SetMPProgress( UINT uiCurrentMP, UINT uiMaxMP ) 
{
    m_pOtherZoneImage->SetVisible(FALSE);

	m_pMPProgrss->SetMax( uiMaxMP ) ;
	m_pMPProgrss->SetPosition( uiCurrentMP ) ;
    m_pMPProgrss->SetVisible(TRUE);
}

VOID        CRYLMiniPartyMemberSlot::SetOtherZone( )
{
    /*
    118, 60에 뿌림
    220, 84 ~ 245, 95
    */

    m_pHPProgrss->SetVisible(FALSE);
    m_pMPProgrss->SetVisible(FALSE);
    m_pOtherZoneImage->SetVisible(TRUE);    
}

VOID		CRYLMiniPartyMemberSlot::CreateMPProgressBar() 
{
	RECT rt ;
	CRYLImage*		pImage	  = NULL ;
	CGMImageDib*	pImageDIB = NULL ;
	m_pHPProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pHPProgrss->SetLeft( 115 ) ;
	m_pHPProgrss->SetTop( 6 ) ;
	m_pHPProgrss->SetWidth( 30 ) ;
	m_pHPProgrss->SetHeight( 5 ) ;
	m_pHPProgrss->SetMax( 10 ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 30, 5, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 155, 0, 185, 5 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pHPProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pHPProgrss->SetPosition( 10 ) ;
	
	CGMUICustomPanel::AddChild( m_pHPProgrss ) ;
	GM_DELETE( pImageDIB ) ;

	//----------------------------------------------------------------
	m_pMPProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pMPProgrss->SetLeft( 115 ) ;
	m_pMPProgrss->SetTop( 11 ) ;
	m_pMPProgrss->SetWidth( 30 ) ;
	m_pMPProgrss->SetHeight( 5 ) ;
	m_pMPProgrss->SetMax( 10 ) ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 30, 5, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 155, 5, 185, 10 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pMPProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pMPProgrss->SetPosition( 10 ) ;
	
	CGMUICustomPanel::AddChild( m_pMPProgrss ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLMiniPartyMemberSlot::SetEnableShort(BOOL bShowShort) 
{
	m_bShowShort = bShowShort;

	if(m_bShowShort)
	{
		m_pHPProgrss->SetLeft( 69 );
		m_pMPProgrss->SetLeft( 69 );
	    m_pOtherZoneImage->SetLeft(71) ;
	} else
	{
		m_pHPProgrss->SetLeft( 115 );
		m_pMPProgrss->SetLeft( 115 );
	    m_pOtherZoneImage->SetLeft(117) ;
	}
}

//--------------------------------------------------------------------------------
CRYLMiniPartyDlg*	g_pMiniPartyDlg = NULL ;

CRYLMiniPartyDlg::CRYLMiniPartyDlg() : CRYLDialog()
{
	m_iSlotYMargin = 0 ;
	m_pButtomSlotLargeImage = 0 ;
	m_pButtomSlotSmallImage = 0 ;

	m_pEnchantButton = NULL;
	m_pShortButton = NULL;

	m_bShowEnchant = TRUE;
	m_bShowShortName = FALSE;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 긴 이름판
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CRYLSpriteList::Instance()->CreateGMImage( 155, 14, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 155, 14 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pLongCaption = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLongCaption->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLongCaption ) ;
	AttachCaptionTexture( ( CGMTexture*)m_pLongCaption ) ;
	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 155, 221, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	m_pLongWindow = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pLongWindow->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pLongWindow ) ;
	AttachWindowTexture( ( CGMTexture*)m_pLongWindow ) ;
	GM_DELETE( pImageDIB ) ;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 짧은 이름판
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CRYLSpriteList::Instance()->CreateGMImage( 109, 14, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 147, 175, 256, 189 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	m_pShortCaption = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pShortCaption->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pShortCaption ) ;
	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 109, 221, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	m_pShortWindow = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pShortWindow->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pShortWindow ) ;
	GM_DELETE( pImageDIB ) ;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SetWidth( 155 ) ;
	SetHeight( 221 ) ;

	SetLeft( 0 ) ;
	SetTop( 150 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pMiniPartyDlg = this ;
}

CRYLMiniPartyDlg::~CRYLMiniPartyDlg ()
{
	FinalCtrl() ;


}

VOID	CRYLMiniPartyDlg::InitCtrl ()
{
	RECT rt[ 10 ] ;//was 10 hayzohar
	SetRect( &rt[ 0 ], 1, 84, 15, 95 ) ;
	SetRect( &rt[ 1 ], 22, 84, 36, 95 ) ;
	SetRect( &rt[ 2 ], 43, 84, 57, 95 ) ;
	SetRect( &rt[ 3 ], 64, 84, 78, 95 ) ;
	SetRect( &rt[ 4 ], 85, 84, 98, 95 ) ;
	SetRect( &rt[ 5 ], 106, 84, 120, 95 ) ;
	SetRect( &rt[ 6 ], 127, 84, 141, 95 ) ;
	SetRect( &rt[ 7 ], 148, 84, 162, 95 ) ;
	SetRect( &rt[ 8 ], 169, 84, 182, 95 ) ;
	SetRect( &rt[ 9 ], 190, 84, 209, 95 ) ;
//	SetRect( &rt[ 10 ], 211, 84, 230, 95 ) ;//added this 2 hayzohar
//	SetRect( &rt[ 11 ], 232, 84, 251, 95 ) ;
	
	for ( INT i = 0 ; i < 10 ; i ++ )//was 10 hayzohar
	{
		m_pMemberPartySlot[ i ] = new CRYLMiniPartyMemberSlot( this ) ;
		m_pMemberPartySlot[ i ]->MakePartyMemberSlot( &rt[ i ]) ;
		m_pMemberPartySlot[ i ]->CreateMPProgressBar() ;
		m_pMemberPartySlot[ i ]->SetLeft( 0 ) ;
		m_pMemberPartySlot[ i ]->SetTop( m_iSlotYMargin + 14 ) ;
		m_pMemberPartySlot[ i ]->SetVisible( FALSE ) ;
		AttachGMUIControl( m_pMemberPartySlot[ i ] ) ;
		m_iSlotYMargin += 20 ;
	}
	SetHeight( 221 ) ;
	
	
	RECT rtButtom ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pButtomSlotLargeImage =  new CGMUICustomPanel( this ) ;
	m_pButtomSlotLargeImage->SetLeft( 0 ) ;
	m_pButtomSlotLargeImage->SetTop( m_iSlotYMargin + 14 ) ;

	// Bounus Wis Image (Large)
	CRYLSpriteList::Instance()->CreateGMImage( 155, 7, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rtButtom, 0, 74, 155, 81 ) ;
	pImageDIB->DrawRect( &rtButtom, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pButtomSlotLargeImage->AttachPanelImage( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pButtomSlotLargeImage ) ;

	m_pButtomSlotSmallImage =  new CGMUICustomPanel( this ) ;
	m_pButtomSlotSmallImage->SetLeft( 0 ) ;
	m_pButtomSlotSmallImage->SetTop( m_iSlotYMargin + 14 ) ;

	// Bounus Wis Image (Small)
	CRYLSpriteList::Instance()->CreateGMImage( 109, 7, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rtButtom, 147, 249, 256, 256 ) ;
	pImageDIB->DrawRect( &rtButtom, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pButtomSlotSmallImage->AttachPanelImage( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;

	RECT rcRect ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	// 인챈트 버튼
	m_pEnchantButton = new CRYLButton( this ) ;
	m_pEnchantButton->SetLeft( 3 ) ;
	m_pEnchantButton->SetTop( 3 ) ;
	m_pEnchantButton->OnClickEvent = DlgClick ;
	m_pEnchantButton->SetInstance( RYLMINIPARTYDLG::ENCHANT_BUTTON ) ;
	//SetRect( &rcRect, 130, 214, 140, 223 ) ;//hz guigui
	SetRect( &rcRect, 4, 380, 15, 388 ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	//SetRect( &rcRect, 130, 232, 140, 241 ) ;
	SetRect( &rcRect, 4, 399, 15, 407 ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	//SetRect( &rcRect, 130, 223, 140, 232 ) ;
	SetRect( &rcRect, 4, 389, 15, 398 ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	//SetRect( &rcRect, 141, 215, 151, 224 ) ;
	SetRect( &rcRect, 15, 380, 25, 389 ) ;
	m_pEnchantButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pEnchantButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 65 ], 3000 ) ;
	AttachGMUIControl( m_pEnchantButton ) ;

	// 이름 단축 버튼
	m_pShortButton = new CRYLButton( this ) ;
	m_pShortButton->SetLeft( 131 ) ;
	m_pShortButton->SetTop( 3 ) ;
	m_pShortButton->OnClickEvent = DlgClick ;
	m_pShortButton->SetInstance( RYLMINIPARTYDLG::SHORT_BUTTON ) ;
	SetRect( &rcRect, 130, 214, 140, 223 ) ;
	m_pShortButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rcRect, 130, 232, 140, 241 ) ;
	m_pShortButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rcRect, 130, 223, 140, 232 ) ;
	m_pShortButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rcRect, 141, 215, 151, 224 ) ;
	m_pShortButton->AttachButtonImage( pSprite, &rcRect, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShortButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 66 ], 3001 ) ;
	AttachGMUIControl( m_pShortButton ) ;
}


VOID	CRYLMiniPartyDlg::FinalCtrl ()
{
	for ( INT i = 0 ; i < 10 ; i ++ )//was 10 hayzohar
	{
		GM_DELETE( m_pMemberPartySlot[ i ] ) ;
	}

	GM_DELETE( m_pButtomSlotLargeImage ) ;
	GM_DELETE( m_pButtomSlotSmallImage ) ;
	GM_DELETE( m_pEnchantButton ) ;				// 인챈트 버튼
	GM_DELETE( m_pShortButton ) ;				// 이름 단축 버튼
}

VOID	CRYLMiniPartyDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLMiniPartyDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLMiniPartyDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;

	if(m_bShowEnchant)	// 인챈트 표기
	{
		LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		CRYLGameScene *pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLEnchantDlg *pDlg = (CRYLEnchantDlg *)pScene->GetEnchantDlg();

		int iW = GetWidth()+5;
		int iH = -2;
		unsigned char i, cChantNum, j, cPartyNum = 1;
		for( i = 0; i < pGame->m_csStatus.m_PartyInfo.m_cMemberNum ; i ++ )
		{
			if ( pGame->m_csStatus.m_PartyInfo.ServerID[i] )
			{
				CCharStatus::PartyExtraInfo& partyExtraInfo = pGame->m_csStatus.m_aryPartyExtraInfo[i];

				unsigned long dwMyChrID = CRYLNetworkData::Instance()->m_dwMyChrID ;
				
				if ( 0 != ( Creature::SIEGE_OBJECT_BIT & dwMyChrID ) )
				{
					RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwMyChrID ) ;

					if ( pTempCreature )
					{
						dwMyChrID = pTempCreature->GetRiderID( 0 ) ;
					}
				}

				// edith 2008.05.07 파티원 버프 아이콘 출력
				if(pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwMyChrID)
				{	// 본인일때
					cChantNum = 0;
					for(j = 0; j < Skill::SpellID::MAX_SPELL_TYPE; j++)	// was i < Skill::SpellID::ExpOrb hayzohar fix for aura
//					for(j = 0; j < EnchantInfo::ULONG_BIT * EnchantInfo::MAX_ARRAY; j++)
					{
						if(partyExtraInfo.m_EnchantInfo.GetFlag(j) && pDlg->GetChantIcon(j))
						{
							pDlg->GetChantIcon(j)->Render( lpD3DDevice, 0xFF, iW+xx+(19 * cChantNum), iH+ yy + 19) ;
							cChantNum++;
						}
					}
				} 
				else
				{	// 본인을 제외한 파티원일때
					cChantNum = 0;
					for(j = 0; j < Skill::SpellID::MAX_SPELL_TYPE; j++)//was i < Skill::SpellID::ExpOrb hayzohar fix for aura
//					for(j = 0; j < EnchantInfo::ULONG_BIT * EnchantInfo::MAX_ARRAY; j++)
					{
						if(partyExtraInfo.m_EnchantInfo.GetFlag(j) && pDlg->GetChantIcon(j))
						{
							pDlg->GetChantIcon(j)->Render( lpD3DDevice, 0xFF, iW+xx+(19 * cChantNum), iH+ yy + 19 + (cPartyNum * 20)) ;
							cChantNum++;
						}
					}
					cPartyNum++;
				}
			}
		}
	}
}

HRESULT		CRYLMiniPartyDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE ;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE ;
		}
	}

	CHAR		strTempName[16], strDot[4] = "...";
	ULONG		lengthDot = CRYLStringTable::GetStringWidth( strDot ) ;
	INT			count = 1 ;

	for ( INT i = 0 ; i < 10 ; i ++ )//was 10 hayzohar
	{
		m_pMemberPartySlot[ i ]->SetVisible( FALSE ) ;
	}

	INT iCount = 0 ;

	INT iSlotIndex = -1;
	for (INT i = 0; i < pGame->m_csStatus.m_PartyInfo.m_cMemberNum ; i ++ )
	{
		if ( pGame->m_csStatus.m_PartyInfo.ServerID[i])
		{
			++ iSlotIndex;
			++ iCount ;

            CCharStatus::PartyExtraInfo& partyExtraInfo = pGame->m_csStatus.m_aryPartyExtraInfo[i];

            bool bAggressive = true;

            if(0 < partyExtraInfo.m_lPartyAttackTimer && partyExtraInfo.m_wAttackType & AtType::SKILL_BIT)
            {
                
                const Skill::ProtoType* lpProtoType = 
                    CSkillMgr::GetInstance().GetSkillProtoType(partyExtraInfo.m_wAttackType);

                if(0 != lpProtoType)
                {
                    switch(lpProtoType[partyExtraInfo.m_cSkillLockcount].m_eTargetType)
                    {
                    case Skill::Target::FRIEND:
                    case Skill::Target::FRIEND_EXCEPT_SELF:
                    case Skill::Target::DEAD_FRIEND:
                    case Skill::Target::PARTY:

                        bAggressive = false;
                        break;
                    }
                }
            }

			// 나 자신이 공성 병기에 타고 있는 경우
			unsigned long dwMyChrID = CRYLNetworkData::Instance()->m_dwMyChrID ;
			if ( 0 != ( Creature::SIEGE_OBJECT_BIT & dwMyChrID ) )
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwMyChrID ) ;
				if ( pTempCreature )
				{
					dwMyChrID = pTempCreature->GetRiderID( 0 ) ;
				}
			}

			if ( pGame->m_csStatus.m_PartyInfo.MemberCID[i] == dwMyChrID )
			{
				-- iSlotIndex;

				m_pMemberPartySlot[ 0 ]->SetVisible( TRUE ) ;

				if (partyExtraInfo.m_lPartyAttackTimer)
				{
                    m_pMemberPartySlot[ 0 ]->SetPartyMemberState( bAggressive ? 2 : 1) ;
				} 
				else
				{
					if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID )
						m_pMemberPartySlot[ 0 ]->SetPartyMemberState( 1 ) ;
					else
						m_pMemberPartySlot[ 0 ]->SetPartyMemberState( 0 ) ;
				}
				
				if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 90 )
				{
					int nNumUnicode = 0 ;
					for ( INT j = 0; j < 15 ; j ++ )
					{
						strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
						strTempName[ j + 1 ] = '\0' ;
						
						if ( strTempName[ j ] & 0x80 ) 
							nNumUnicode ++ ;
						
						if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 90 )
						{
							if ( nNumUnicode % 2 )
							{
								memcpy( &strTempName[ j ], strDot, 4 ) ;
							} 
							else
							{
								memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
							}
							break ;
						}
					}
					
					if( m_bShowShortName )
					{
						if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 36 )
						{
							int nNumUnicode = 0 ;
							for ( INT j = 0; j < 15 ; j ++ )
							{
								strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
								strTempName[ j + 1 ] = '\0' ;
								
								if ( strTempName[ j ] & 0x80 ) 
									nNumUnicode ++ ;
								
								if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 36 )
								{
									if ( nNumUnicode % 2 )
									{
										memcpy( &strTempName[ j ], strDot, 4 ) ;
									} 
									else
									{
										memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
									}
									break ;
								}
							}
						}
					}
					if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID )
					{
						m_pMemberPartySlot[ 0 ]->SetFontColor( 0xff8affff ) ;
						m_pMemberPartySlot[ 0 ]->SetPartyMemberName( strTempName ) ;
					} 
					else
					{
						m_pMemberPartySlot[ 0 ]->SetFontColor( 0xffffffff ) ;
						m_pMemberPartySlot[ 0 ]->SetPartyMemberName( strTempName ) ;
					}
				} 
				else
				{
					strcpy( strTempName, pGame->m_csStatus.m_PartyInfo.Name[ i ] );
					if( m_bShowShortName )
					{
						if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 36 )
						{
							int nNumUnicode = 0 ;
							for ( INT j = 0; j < 15 ; j ++ )
							{
								strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
								strTempName[ j + 1 ] = '\0' ;
								
								if ( strTempName[ j ] & 0x80 ) 
									nNumUnicode ++ ;
								
								if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 36 )
								{
									if ( nNumUnicode % 2 )
									{
										memcpy( &strTempName[ j ], strDot, 4 ) ;
									} 
									else
									{
										memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
									}
									break ;
								}
							}
						}
					}
					if (pGame->m_csStatus.m_PartyInfo.MemberCID[i] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID)
					{
						m_pMemberPartySlot[ 0 ]->SetFontColor( 0xff8affff ) ;
						m_pMemberPartySlot[ 0 ]->SetPartyMemberName( strTempName ) ;
					} 
					else
					{
						m_pMemberPartySlot[ 0 ]->SetFontColor( 0xffffffff ) ;
						m_pMemberPartySlot[ 0 ]->SetPartyMemberName( strTempName ) ;
					}
				}

				m_pMemberPartySlot[ 0 ]->SetHPProgress( pGame->m_csStatus.m_Info.HP, pGame->m_csStatus.m_MaxHP ) ;
				m_pMemberPartySlot[ 0 ]->SetMPProgress( pGame->m_csStatus.m_Info.MP, pGame->m_csStatus.m_MaxMP ) ;
			} 
			else
			{
				m_pMemberPartySlot[ iSlotIndex + 1 ]->SetVisible( TRUE ) ;

				if (partyExtraInfo.m_lPartyAttackTimer)
				{
                    m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberState( bAggressive ? 2 : 1 ) ;
				} 
				else
				{
					if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID )
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberState( 1 ) ;
					else
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberState( 0 ) ;
				}

				if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 90 )
				{
					INT nNumUnicode = 0  ;
					for ( INT j = 0; j < 15; j ++ )
					{
						strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
						strTempName[ j + 1 ] = '\0' ;
						if ( strTempName[ j ] & 0x80 )  
							nNumUnicode ++ ;
						
						if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 90 )
						{
							if ( nNumUnicode % 2 )
							{
								memcpy( &strTempName[ j ], strDot, 4 ) ;
							} 
							else
							{
								memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
							}
							break ;
						}
					}
					
					if( m_bShowShortName )
					{
						if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 36 )
						{
							int nNumUnicode = 0 ;
							for ( INT j = 0; j < 15 ; j ++ )
							{
								strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
								strTempName[ j + 1 ] = '\0' ;
								
								if ( strTempName[ j ] & 0x80 ) 
									nNumUnicode ++ ;
								
								if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 36 )
								{
									if ( nNumUnicode % 2 )
									{
										memcpy( &strTempName[ j ], strDot, 4 ) ;
									} 
									else
									{
										memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
									}
									break ;
								}
							}
						}
					}
					if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID )
					{
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetFontColor( 0xff8affff ) ;
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberName( strTempName ) ;
					} 
					else
					{
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetFontColor( 0xffffffff ) ;
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberName( strTempName ) ;
					}
				} 
				else
				{
					strcpy( strTempName, pGame->m_csStatus.m_PartyInfo.Name[ i ] );
					if( m_bShowShortName )
					{
						if ( CRYLStringTable::GetStringWidth( pGame->m_csStatus.m_PartyInfo.Name[ i ] ) > 36 )
						{
							int nNumUnicode = 0 ;
							for ( INT j = 0; j < 15 ; j ++ )
							{
								strTempName[ j ] = pGame->m_csStatus.m_PartyInfo.Name[ i ][ j ] ;
								strTempName[ j + 1 ] = '\0' ;
								
								if ( strTempName[ j ] & 0x80 ) 
									nNumUnicode ++ ;
								
								if ( CRYLStringTable::GetStringWidth( strTempName ) + lengthDot > 36 )
								{
									if ( nNumUnicode % 2 )
									{
										memcpy( &strTempName[ j ], strDot, 4 ) ;
									} 
									else
									{
										memcpy( &strTempName[ j - 1 ], strDot, 4 ) ;
									}
									break ;
								}
							}
						}
					}
					if ( pGame->m_csStatus.m_PartyInfo.MemberCID[i] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID )
					{
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetFontColor( 0xff8affff ) ;
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberName( strTempName ) ;
					} 
					else
					{
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetFontColor( 0xffffffff ) ;
						m_pMemberPartySlot[ iSlotIndex + 1 ]->SetPartyMemberName( strTempName ) ;
					}
				}
				
                unsigned long dwMemberCID       = pGame->m_csStatus.m_PartyInfo.MemberCID[ i ];

                SERVER_ID memberServerID;
                memberServerID.dwID = pGame->m_csStatus.m_PartyInfo.ServerID[ i ];
                
                if (memberServerID.GetZone() == CRYLNetworkData::Instance()->m_dwPresentZone &&
                    memberServerID.GetChannel() == CRYLNetworkData::Instance()->m_dwChannelID )
				{
					m_pMemberPartySlot[ iSlotIndex + 1 ]->SetHPProgress( partyExtraInfo.m_usCurrHP, partyExtraInfo.m_usMaxHP) ;
                    m_pMemberPartySlot[ iSlotIndex + 1 ]->SetMPProgress( partyExtraInfo.m_usCurrMP, partyExtraInfo.m_usMaxMP ) ;
				}
                else
                {
                    m_pMemberPartySlot[ iSlotIndex + 1 ]->SetOtherZone();
                }
			}
		}
	}
	
	if(m_bShowShortName)
	{
		m_pButtomSlotSmallImage->SetTop( ( iCount * 20 ) + 14 ) ;
	} else
	{
		m_pButtomSlotLargeImage->SetTop( ( iCount * 20 ) + 14 ) ;
	}
	SetHeight( ( iCount * 20 ) + 21 ) ;
	
	return S_OK ;	
}


VOID	CRYLMiniPartyDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pMiniPartyDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLMINIPARTYDLG::ENCHANT_BUTTON :
			g_pMiniPartyDlg->m_bShowEnchant = !g_pMiniPartyDlg->m_bShowEnchant;
			break ;

		case RYLMINIPARTYDLG::SHORT_BUTTON :
			g_pMiniPartyDlg->m_bShowShortName = !g_pMiniPartyDlg->m_bShowShortName;
			g_pMiniPartyDlg->ProcessDlgSize();
			break ;
	}
}

VOID	CRYLMiniPartyDlg::ProcessDlgSize()
{
	if(g_pMiniPartyDlg->m_bShowShortName)
	{
		AttachCaptionTexture( ( CGMTexture*)m_pShortCaption ) ;
		AttachWindowTexture( ( CGMTexture*)m_pShortWindow ) ;
		DetachGMUIControl( m_pButtomSlotLargeImage ) ;
		AttachGMUIControl( m_pButtomSlotSmallImage ) ;
		m_pShortButton->SetLeft( 85 ) ;
		GetCloseButton()->SetLeft( GetCloseButton()->GetLeft() - 46 );

		SetLeft( GetLeft() + 46 );
		SetWidth( 109 ) ;
	} else
	{
		AttachCaptionTexture( ( CGMTexture*)m_pLongCaption ) ;
		AttachWindowTexture( ( CGMTexture*)m_pLongWindow ) ;
		DetachGMUIControl( m_pButtomSlotSmallImage ) ;
		AttachGMUIControl( m_pButtomSlotLargeImage ) ;
		m_pShortButton->SetLeft( 131 ) ;
		GetCloseButton()->SetLeft( GetCloseButton()->GetLeft() + 46 );

		SetLeft( GetLeft() - 46 );
		SetWidth( 155 ) ;
	}

	for(int i = 0; i < 10; i++)//was 10 hayzohar
	{
		m_pMemberPartySlot[i]->SetEnableShort(g_pMiniPartyDlg->m_bShowShortName);
	}
}
