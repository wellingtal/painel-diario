
#include <winsock2.h>

#include "RYLIntegServerSelCharDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLComboBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLGameData.h"
#include "RYLImageManager.h"
#include "RYLNetworkData.h"
#include "RYLLoginScene.h"
#include "RYLSceneManager.h"
#include "RYLMessageBox.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLSelectedConfirmDlg.h"
#include "RYLCharacterSelectScene.h"
#include "RYLNationSelectDlg.h"
#include "RYLStringTable.h"

#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Creature/Character/CharacterClass.h>
#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLIntegServerSelCharDlg*	g_pRYLIntegServerSelCharDlg = NULL ;

CRYLIntegServerSelCharDlg::CRYLIntegServerSelCharDlg () : CRYLDialog()
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 545, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 108 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 164, 28, 86, 19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 4, 49, 248, 194, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 7, 52, 83, 187, 0xff959595, 0xc0000000,  1 ) ;
	for ( i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
		pImageDIB->MakeFrameRect( 92, 52 + ( i * 38 ), 157, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 4, 270, 248, 195, 0xff946b42, 0x80000000,  1 ) ;		
	for ( i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
		pImageDIB->MakeFrameRect( 7, 274 + ( i * 38 ), 242, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 120, 514, 43, 19, 0xff959595, 0xc0000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 557 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pRYLIntegServerSelCharDlg = this ;	
	InitCtrl() ;
	
	m_sUnifiedNation = 0 ;
	m_cRemainCharTransferCount = 0 ;
	m_cSelMaxCharCount = 0 ;
}

CRYLIntegServerSelCharDlg::~CRYLIntegServerSelCharDlg ()
{
	FinalCtrl() ;
}

VOID CRYLIntegServerSelCharDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pServerList = new CRYLListBox( this, FALSE ) ;
	m_pServerList->SetLeft( 14 ) ;
	m_pServerList->SetTop( 74 ) ;
	m_pServerList->SetWidth( 75 ) ;
	m_pServerList->SetHeight( 179 ) ;
	m_pServerList->SetItemHeight( 16 ) ;
	m_pServerList->OnClickEvent = DlgClick ;
	m_pServerList->SetInstance( RYLINTEGSERVERSELCHAR::SEVER_LIST ) ;
	m_pServerList->SetFontColor( 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3382 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3383 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3384 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3385 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3386 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3387 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->AddItem( CRYLStringTable::m_strString[ 3486 ], 0xffffffff, 0xffffff00, 0xffffffff ) ;
	m_pServerList->SelectItemIndex( 0 ) ;
	AttachGMUIControl( m_pServerList ) ;

	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		int iJumpHeight = i * 38 ;
		m_OldCharSlot[ i ].pSelCharButton = new CRYLButton( this ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetLeft( 200 ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetTop( 72 + iJumpHeight ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetCaption( CRYLStringTable::m_strString[ 3389 ] ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetFontColor( 0xffffffff) ;
		m_OldCharSlot[ i ].pSelCharButton->SetColor( 0xffffffff) ;
		m_OldCharSlot[ i ].pSelCharButton->OnClickEvent = DlgClick ;
		m_OldCharSlot[ i ].pSelCharButton->SetInstance( RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON + i ) ;
		m_OldCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_OldCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_OldCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_OldCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetEnable( FALSE ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetVisible( FALSE ) ;
		AttachGMUIControl( m_OldCharSlot[ i ].pSelCharButton ) ;

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

		m_SelCharSlot[ i ].pSelCharButton = new CRYLButton( this ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetLeft( 200 ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetTop( 294 + iJumpHeight ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetCaption( CRYLStringTable::m_strString[ 126 ] ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetFontColor( 0xffffffff) ;
		m_SelCharSlot[ i ].pSelCharButton->SetColor( 0xffffffff) ;
		m_SelCharSlot[ i ].pSelCharButton->OnClickEvent = DlgClick ;
		m_SelCharSlot[ i ].pSelCharButton->SetInstance( RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON + i ) ;
		m_SelCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_SelCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_SelCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_SelCharSlot[ i ].pSelCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetEnable( FALSE ) ;
		m_SelCharSlot[ i ].pSelCharButton->SetVisible( FALSE ) ;
		AttachGMUIControl( m_SelCharSlot[ i ].pSelCharButton ) ;

		m_SelCharSlot[ i ].pCharNameLabel = new CRYLLabel( this ) ;
		m_SelCharSlot[ i ].pCharNameLabel->SetLeft( 15 ) ;
		m_SelCharSlot[ i ].pCharNameLabel->SetTop( 294 + iJumpHeight ) ;
		m_SelCharSlot[ i ].pCharNameLabel->SetFontColor (0xffffffff);
		m_SelCharSlot[ i ].pCharNameLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_SelCharSlot[ i ].pCharNameLabel ) ;

		m_SelCharSlot[ i ].pCharClassLabel = new CRYLLabel( this ) ;
		m_SelCharSlot[ i ].pCharClassLabel->SetLeft( 15 ) ;
		m_SelCharSlot[ i ].pCharClassLabel->SetTop( 309 + iJumpHeight ) ;
		m_SelCharSlot[ i ].pCharClassLabel->SetFontColor (0xffffffb2);
		m_SelCharSlot[ i ].pCharClassLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_SelCharSlot[ i ].pCharClassLabel ) ;  

		m_SelCharSlot[ i ].pCharLevelLabel = new CRYLLabel( this ) ;
		m_SelCharSlot[ i ].pCharLevelLabel->SetLeft( 163 ) ;
		m_SelCharSlot[ i ].pCharLevelLabel->SetTop( 309 + iJumpHeight ) ;
		m_SelCharSlot[ i ].pCharLevelLabel->SetFontColor (0xffffffb2);
		m_SelCharSlot[ i ].pCharLevelLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_SelCharSlot[ i ].pCharLevelLabel ) ;  
	}

	m_pNextStepButton = new CRYLButton( this ) ;
	m_pNextStepButton->SetLeft( 170 ) ;
	m_pNextStepButton->SetTop( 526 ) ;
	m_pNextStepButton->SetCaption( CRYLStringTable::m_strString[ 3487 ] ) ;
	m_pNextStepButton->SetFontColor( 0xffffffff) ;
	m_pNextStepButton->SetColor( 0xffffffff) ;
	m_pNextStepButton->OnClickEvent = DlgClick ;
	m_pNextStepButton->SetInstance( RYLINTEGSERVERSELCHAR::NEXTSTEP_BUTTON ) ;
	m_pNextStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pNextStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pNextStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pNextStepButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextStepButton ) ;

	// 통합서버 캐릭터선택
	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 7 ) ;
	m_pTitleLabel->SetTop( 19 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 3488 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;  

	// 기존 캐릭터선택
	m_pOldCharLabel = new CRYLLabel( this ) ;
	m_pOldCharLabel->SetLeft( 8 ) ;
	m_pOldCharLabel->SetTop( 47 ) ;
	m_pOldCharLabel->SetFontColor (0xffffffff);
	m_pOldCharLabel->SetAutoSize( TRUE ) ;
	m_pOldCharLabel->SetCaption( CRYLStringTable::m_strString[ 3489 ] ) ;
	AttachGMUIControl( m_pOldCharLabel ) ;  

	// 종족 구분
	m_pRaceLabel = new CRYLLabel( this ) ;
	m_pRaceLabel->SetLeft( 112 ) ;
	m_pRaceLabel->SetTop( 47 ) ;
	m_pRaceLabel->SetFontColor (0xffffffff);
	m_pRaceLabel->SetAutoSize( TRUE ) ;
	m_pRaceLabel->SetCaption( CRYLStringTable::m_strString[ 3490 ] ) ;
	AttachGMUIControl( m_pRaceLabel ) ;  

	// 선택된 통합서버용 캐릭터
	m_pIntegCharLabel = new CRYLLabel( this ) ;
	m_pIntegCharLabel->SetLeft( 9 ) ;
	m_pIntegCharLabel->SetTop( 269 ) ;
	m_pIntegCharLabel->SetFontColor (0xffffffff);
	m_pIntegCharLabel->SetAutoSize( TRUE ) ;
	m_pIntegCharLabel->SetCaption( CRYLStringTable::m_strString[ 3491 ] ) ;
	AttachGMUIControl( m_pIntegCharLabel ) ;  

	// *하나의 계정에서 휴먼캐릭터와 아칸 캐릭터를
	m_pDesc1Label = new CRYLLabel( this ) ;
	m_pDesc1Label->SetLeft( 7 ) ;
	m_pDesc1Label->SetTop( 492 ) ;
	m_pDesc1Label->SetFontColor (0xffffffff);
	m_pDesc1Label->SetAutoSize( TRUE ) ;
	m_pDesc1Label->SetCaption( CRYLStringTable::m_strString[ 3492 ] ) ;
	AttachGMUIControl( m_pDesc1Label ) ;  

	// 동시에 선택하실 수 없습니다.
	m_pDesc2Label = new CRYLLabel( this ) ;
	m_pDesc2Label->SetLeft( 13 ) ;
	m_pDesc2Label->SetTop( 507 ) ;
	m_pDesc2Label->SetFontColor (0xffffffff);
	m_pDesc2Label->SetAutoSize( TRUE ) ;
	m_pDesc2Label->SetCaption( CRYLStringTable::m_strString[ 3493 ] ) ;
	AttachGMUIControl( m_pDesc2Label ) ;  

	// 캐릭터선택횟수
	m_pSelectCharcLabel = new CRYLLabel( this ) ;
	m_pSelectCharcLabel->SetLeft( 30 ) ;
	m_pSelectCharcLabel->SetTop( 534 ) ;
	m_pSelectCharcLabel->SetFontColor (0xffffffff);
	m_pSelectCharcLabel->SetAutoSize( TRUE ) ;
	m_pSelectCharcLabel->SetCaption( CRYLStringTable::m_strString[ 3494 ] ) ;
	AttachGMUIControl( m_pSelectCharcLabel ) ;  

	// 캐릭터선택횟수 값
	m_pRemainCharTransferCountLabel = new CRYLLabel( this ) ;
	m_pRemainCharTransferCountLabel->SetLeft( 126 ) ;
	m_pRemainCharTransferCountLabel->SetTop( 534 ) ;
	m_pRemainCharTransferCountLabel->SetFontColor (0xffffffff);
	m_pRemainCharTransferCountLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pRemainCharTransferCountLabel ) ;  

	// 국가 
	m_pNationLabel = new CRYLLabel( this ) ;
	m_pNationLabel->SetLeft( 170 ) ;
	m_pNationLabel->SetTop( 47 ) ;
	m_pNationLabel->SetFontColor (0xffffffff);
	m_pNationLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pNationLabel ) ;  
		
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// 종족 콤보박스
	m_pRaceCBBox = new CRYLComboBox( this ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pRaceCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 201, 225, 213 ) ;
	m_pRaceCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 201, 239, 213 ) ;
	m_pRaceCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pRaceCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

	m_pRaceCBBox->AddItem( CRYLStringTable::m_strString[ 2233 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
	m_pRaceCBBox->AddItem( CRYLStringTable::m_strString[ 2264 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
	m_pRaceCBBox->SetColor( 0xff000000 ) ;
	m_pRaceCBBox->OnClickEvent = DlgClick ;
	m_pRaceCBBox->SetLeft( 170 ) ;
	m_pRaceCBBox->SetTop( 44 ) ;
	m_pRaceCBBox->SetWidth( 77 ) ;
	m_pRaceCBBox->SetHeight( 17 ) ;
	m_pRaceCBBox->SetButtonPos() ;

	m_pRaceCBBox->SetFontColor( 0xffffffff ) ;
	m_pRaceCBBox->SelectMenuData( 0 ) ;
	
	m_pRaceCBBox->GetButton()->SetInstance( RYLINTEGSERVERSELCHAR::RACE_COMBOBOX_BTN ) ;
	m_pRaceCBBox->GetButton()->OnClickEvent = DlgClick ;
	m_pRaceCBBox->GetMenu()->SetInstance( RYLINTEGSERVERSELCHAR::RACE_COMBOBOX_MENU ) ;
	m_pRaceCBBox->GetMenu()->OnClickEvent = DlgClick ;
	m_pRaceCBBox->GetMenu()->SetItemIndex( 0 ) ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 77, 36, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	
	pImageDIB->MakeFrameRect( 1, 1, 76, 35, 0xff959595, 0xff000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pRaceCBBox->GetMenu()->AttachMenuImage( pImage ) ;
	AttachGMUIControl( m_pRaceCBBox ) ;	
}

VOID CRYLIntegServerSelCharDlg::FinalCtrl ()
{
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		GM_DELETE( m_OldCharSlot[ i ].pCharLevelLabel ) ;
		GM_DELETE( m_OldCharSlot[ i ].pCharClassLabel ) ;
		GM_DELETE( m_OldCharSlot[ i ].pCharNameLabel ) ;
		GM_DELETE( m_OldCharSlot[ i ].pSelCharButton ) ;

		GM_DELETE( m_SelCharSlot[ i ].pCharLevelLabel ) ;
		GM_DELETE( m_SelCharSlot[ i ].pCharClassLabel ) ;
		GM_DELETE( m_SelCharSlot[ i ].pCharNameLabel ) ;
		GM_DELETE( m_SelCharSlot[ i ].pSelCharButton ) ;
	}

	GM_DELETE( m_pTitleLabel ) ;	
	GM_DELETE( m_pOldCharLabel ) ;	
	GM_DELETE( m_pRaceLabel ) ;		
	GM_DELETE( m_pIntegCharLabel ) ;
	GM_DELETE( m_pDesc1Label ) ;
	GM_DELETE( m_pDesc2Label ) ;
	GM_DELETE( m_pServerList ) ;
	GM_DELETE( m_pRaceCBBox ) ;
	GM_DELETE( m_pNextStepButton ) ;
	GM_DELETE( m_pNationLabel ) ;
	GM_DELETE( m_pSelectCharcLabel ) ;
	GM_DELETE( m_pRemainCharTransferCountLabel ) ;
}

bool CRYLIntegServerSelCharDlg::UpdateRemainCharTransferCount() 
{
	bool bEnable = true ;

	if ( m_cRemainCharTransferCount < 0 )
	{
		m_cRemainCharTransferCount = 0 ;
		CRYLMessageBox* lpMessageBox = NULL;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 3495 ] ) ;

		bEnable = false ;
	}

	char szBuff[ 32 ] ;
	sprintf( szBuff, "%d/%d", m_cRemainCharTransferCount, m_cSelMaxCharCount ) ;
	m_pRemainCharTransferCountLabel->SetCaption( szBuff ) ;

	return bEnable ;
}

void CRYLIntegServerSelCharDlg::SetCurrentNation() 
{
	for ( int nCount = 0 ; nCount < SELECT_CHAR_SLOT ; nCount ++ )
	{
		if ( m_SelectChar[ nCount ].dwCID )
		{
			unsigned short sClass = m_SelectChar[ nCount ].sClass ;
			
			if ( sClass >= 1 && sClass <=12 )
			{
				m_sUnifiedNation = 0 ;
			}
			else if ( sClass >= 17 && sClass <=24 )
			{
				m_sUnifiedNation = 1 ;
			}
		}
	}
}

VOID CRYLIntegServerSelCharDlg::Show( CGMUIObject* Sender )
{
	SetCurrentNation() ;
	m_pRaceCBBox->GetMenu()->SelectItemIndex( m_sUnifiedNation ) ;
	m_pRaceCBBox->SelectMenuData( m_sUnifiedNation ) ;
	m_pServerList->SelectItemIndex( 0 ) ;
	ResetServerList() ;
	SelectServer() ;
}

VOID CRYLIntegServerSelCharDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLIntegServerSelCharDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLIntegServerSelCharDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID CRYLIntegServerSelCharDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pRYLIntegServerSelCharDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLINTEGSERVERSELCHAR::RACE_COMBOBOX_BTN :	
		break ;
	case RYLINTEGSERVERSELCHAR::RACE_COMBOBOX_MENU :	
		g_pRYLIntegServerSelCharDlg->ResetServerList() ;
		g_pRYLIntegServerSelCharDlg->SelectServer() ;
		break ;
	case RYLINTEGSERVERSELCHAR::SEVER_LIST :	
		{
			g_pRYLIntegServerSelCharDlg->SelectServer() ;
		}
		break ;
	case RYLINTEGSERVERSELCHAR::NEXTSTEP_BUTTON :
		{
			if ( g_pRYLIntegServerSelCharDlg->GetFirstLoginEnable() && g_pRYLIntegServerSelCharDlg->GetEnableCharSlot() )
			{
				g_pRYLIntegServerSelCharDlg->SetVisible( FALSE ) ;
				CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ) ;
				CRYLNationSelectDlg* pDlg = static_cast<CRYLNationSelectDlg*>( pScene->GetNationSelectDlg() ) ;

				g_pRYLIntegServerSelCharDlg->SetCurrentNation() ;
				pDlg->SetUnifiedEnable( true ) ;
				pDlg->InitValue( g_pRYLIntegServerSelCharDlg->m_sUnifiedNation, 
								 g_pRYLIntegServerSelCharDlg->m_sUnifiedNation + 1 ) ;
				pDlg->SetVisible( TRUE ) ;
			}
			else
			{
				CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
				g_pRYLIntegServerSelCharDlg->SetVisible( FALSE ) ;
				
				CRYLIntegServerSelVaultDlg* pVaultDlg = static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() ) ;
				CRYLSelectedConfirmDlg* pConfirmDlg = static_cast< CRYLSelectedConfirmDlg* >( pScene->GetSelectedConfirmDlg() ) ;
				if ( pVaultDlg->GetVaultEnable() )
				{
					pScene->GetSelectedConfirmDlg()->SetVisible( TRUE ) ;
					pConfirmDlg->m_pVaultNameLabel->SetCaption( CRYLStringTable::m_strString[ 3496 ] ) ;
				}
				else
					pScene->GetIntegServerSelVaultDlg()->SetVisible( TRUE ) ;
			}
			
		}
		break ;
	case RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON	:
	case RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON	+ 1 :
	case RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON	+ 2 :
	case RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON	+ 3 :
	case RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON	+ 4 :
		{
			g_pRYLIntegServerSelCharDlg->SelectChar( Sender->GetInstance() ) ;
		}
		break ;
	case RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON	:
	case RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON	+ 1 :
	case RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON	+ 2 :
	case RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON	+ 3 :
	case RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON	+ 4 :
		{
			g_pRYLIntegServerSelCharDlg->CancelChar( Sender->GetInstance() ) ;
			g_pRYLIntegServerSelCharDlg->UpdateRemainCharTransferCount() ;
		}
		break ;
	}
}

void CRYLIntegServerSelCharDlg::SelectServer() 
{
	int iIdx = m_pServerList->GetItemIndex() ;
	
	if ( iIdx == -1 )
		return ;
	
	ResetServerCharInfo() ;

	if ( m_pRaceCBBox->GetVisible() )
		m_sUnifiedNation = m_pRaceCBBox->GetMenu()->GetItemIndex() ;
	
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i++ )
	{
		if ( m_sUnifiedNation == 0 )
		{
			if ( m_SortData[ iIdx ][ i ].sClass >= 17 && m_SortData[ iIdx ][ i ].sClass <=24 )
				continue ;
		}
		else if ( m_sUnifiedNation == 1 )
		{
			if ( m_SortData[ iIdx ][ i ].sClass >= 1 && m_SortData[ iIdx ][ i ].sClass <=12 )
				continue ;
		}

		if ( strcmp( m_SortData[ iIdx ][ i ].szName, "" ) )
		{
			if ( m_SortData[ iIdx ][ i ].bEnable )
			{
				m_OldCharSlot[ i ].pSelCharButton->SetEnable( TRUE ) ;
				m_OldCharSlot[ i ].pCharNameLabel->SetFontColor( 0xffffffff ) ;
				m_OldCharSlot[ i ].pCharClassLabel->SetFontColor( 0xffffffb2 ) ;
				m_OldCharSlot[ i ].pCharLevelLabel->SetFontColor( 0xffffffb2 ) ;				
			}
			else
			{
				m_OldCharSlot[ i ].pSelCharButton->SetEnable( FALSE ) ;
				m_OldCharSlot[ i ].pCharNameLabel->SetFontColor( 0xff808080 ) ;
				m_OldCharSlot[ i ].pCharClassLabel->SetFontColor( 0xff808080 ) ;
				m_OldCharSlot[ i ].pCharLevelLabel->SetFontColor( 0xff808080 ) ;
			}
			m_OldCharSlot[ i ].pSelCharButton->SetVisible( TRUE ) ;
			m_OldCharSlot[ i ].pCharNameLabel->SetCaption( m_SortData[ iIdx ][ i ].szName ) ;
			m_OldCharSlot[ i ].pCharClassLabel->SetCaption(m_SortData[ iIdx ][ i ].szClass ) ;
			m_OldCharSlot[ i ].pCharLevelLabel->SetCaption(m_SortData[ iIdx ][ i ].szLevel ) ;

            if ( m_SortData[ iIdx ][ i ].cOldServerGroupID == UnifiedConst::Part1Unified &&
                 m_SortData[ iIdx ][ i ].cLevel <= m_cRestrictedPart1ToPart2Level )
			{
				m_OldCharSlot[ i ].pSelCharButton->SetEnable( FALSE ) ;
				m_OldCharSlot[ i ].pCharNameLabel->SetFontColor( 0xff808080 ) ;
				m_OldCharSlot[ i ].pCharClassLabel->SetFontColor( 0xff808080 ) ;
				m_OldCharSlot[ i ].pCharLevelLabel->SetFontColor( 0xff808080 ) ;
			}
		}
	}
}

bool CRYLIntegServerSelCharDlg::GetEnableCharSlot() 
{
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		if ( m_SelCharSlot[ i ].bEnable )
			return true ;
	}
	
	return false ;
}

void CRYLIntegServerSelCharDlg::CancelChar( int iIndex ) 
{
	iIndex -= RYLINTEGSERVERSELCHAR::NEW_SLOT_BUTTON ;
	m_SortData[ m_SelectChar[ iIndex ].x ][ m_SelectChar[ iIndex ].y ].bEnable = true ;
	SelectServer() ;

	m_SelectChar[ iIndex ].bEnable = false ;
	m_SelectChar[ iIndex ].dwCID   = 0 ;
	m_SelectChar[ iIndex ].x       = 0 ;
	m_SelectChar[ iIndex ].y       = 0 ;
	m_SelectChar[ iIndex ].sClass  = 0 ;
	m_SelectChar[ iIndex ].cLevel  = 0 ;
	m_SelectChar[ iIndex ].szName[0] = '\0' ;
	m_SelectChar[ iIndex ].szClass[0] = '\0' ;
	m_SelectChar[ iIndex ].szLevel[0] = '\0' ;

	m_SelCharSlot[ iIndex ].bEnable = false ;
	m_SelCharSlot[ iIndex ].pSelCharButton->SetVisible( FALSE ) ;
	m_SelCharSlot[ iIndex ].pCharNameLabel->SetCaption( "" ) ;
	m_SelCharSlot[ iIndex ].pCharClassLabel->SetCaption( "" ) ;
	m_SelCharSlot[ iIndex ].pCharLevelLabel->SetCaption( "" ) ;

	++ m_cRemainCharTransferCount ;
}

bool CRYLIntegServerSelCharDlg::CheckNation( int iIndex ) 
{
	bool bNationEnable = true, bSlotEnable = false ;
	unsigned short sClass ; //= m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].sClass ;

	for ( int nCount = 0 ; nCount < SELECT_CHAR_SLOT ; nCount ++ )
	{
		if ( m_SelectChar[ nCount ].dwCID )
		{
			bSlotEnable = true ;
			
			sClass = m_SelectChar[ nCount ].sClass ;
			
			if ( sClass >= 1 && sClass <=12 )
			{
				m_sUnifiedNation = 0 ;
			}
			else if ( sClass >= 17 && sClass <=24 )
			{
				m_sUnifiedNation = 1 ;
			}
		}
	}
	
	if ( !bSlotEnable )
		return bNationEnable ;
	
	sClass = m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].sClass ;

	// 휴먼
	if ( m_sUnifiedNation == 0 )
	{
		if ( sClass >= 17 && sClass <=24 )
			bNationEnable = false ;
	}
	else // 아칸
	{
		if ( sClass >= 1 && sClass <=12 )
			bNationEnable = false ;
	}

	return bNationEnable ;
}

void CRYLIntegServerSelCharDlg::SelectChar( int iIndex ) 
{
	iIndex -= RYLINTEGSERVERSELCHAR::OLD_SLOT_BUTTON ;
	-- m_cRemainCharTransferCount ;
	
	if ( UpdateRemainCharTransferCount() == false )
	{
		return ;
	}

	if ( !CheckNation( iIndex ) )
	{
		char buff[ 512 ] ;
		sprintf( buff, "%s%s",
			CRYLStringTable::m_strString[ 3497 ], CRYLStringTable::m_strString[ 3498 ] ) ;

		CRYLMessageBox* lpMessageBox = NULL;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( buff ) ;
		return ;
	}

	bool bAddEnable = false ;
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		if ( !m_SelCharSlot[ i ].bEnable )
			bAddEnable = true ;
	}
	if ( !bAddEnable )
	{
		CRYLMessageBox* lpMessageBox = NULL;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 3499 ] ) ;
		return ;
	}

	char szBuff[ 64 ] ;
	char* lpszName =  m_OldCharSlot[ iIndex ].pCharNameLabel->GetCaption() ;
	char* lpszClass = m_OldCharSlot[ iIndex ].pCharClassLabel->GetCaption() ;
	char* lpszLevel = m_OldCharSlot[ iIndex ].pCharLevelLabel->GetCaption() ;
    char* lpszServer = (m_pServerList->GetItemIndex() + 1 == 7) 
        ? "PartI통합" : CRYLNetworkData::Instance()->GetOldServerName( m_pServerList->GetItemIndex() + 1 ) ;

	m_OldCharSlot[ iIndex ].pSelCharButton->SetEnable( FALSE ) ;

	m_OldCharSlot[ iIndex ].pCharNameLabel->SetFontColor( 0xff808080 ) ;
	m_OldCharSlot[ iIndex ].pCharClassLabel->SetFontColor( 0xff808080 ) ;
	m_OldCharSlot[ iIndex ].pCharLevelLabel->SetFontColor( 0xff808080 ) ;
	m_OldCharSlot[ iIndex ].bEnable = false ;
	m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].bEnable = false ;

	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		if ( !strcmp( m_SelCharSlot[ i ].pCharNameLabel->GetCaption(), "" ) )
		{
			m_SelectChar[ i ].bEnable = false ;
			m_SelectChar[ i ].x       = m_pServerList->GetItemIndex() ;
			m_SelectChar[ i ].y       = iIndex ;
			m_SelectChar[ i ].dwCID   = m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].dwCID ;
			m_SelectChar[ i ].sClass  = m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].sClass ;
			m_SelectChar[ i ].cLevel  = m_SortData[ m_pServerList->GetItemIndex() ][ iIndex ].cLevel ;
			strcpy( m_SelectChar[ i ].szName, lpszName ) ;
			strcpy( m_SelectChar[ i ].szClass, lpszClass ) ;
			strcpy( m_SelectChar[ i ].szLevel, lpszLevel ) ;
			
			m_SelCharSlot[ i ].bEnable = true ;
			m_SelCharSlot[ i ].pSelCharButton->SetVisible( TRUE ) ;
			m_SelCharSlot[ i ].pSelCharButton->SetEnable( TRUE ) ;
			m_SelCharSlot[ i ].pCharNameLabel->SetCaption( lpszName ) ;
			sprintf( szBuff, "%s - %s", lpszServer, lpszClass ) ;
			m_SelCharSlot[ i ].pCharClassLabel->SetCaption( szBuff ) ;
			m_SelCharSlot[ i ].pCharLevelLabel->SetCaption( lpszLevel ) ;
			break ;
		}
	}
}

void CRYLIntegServerSelCharDlg::ResetServerList() 
{
	if ( m_pRaceCBBox->GetVisible() )
		m_sUnifiedNation = m_pRaceCBBox->GetMenu()->GetItemIndex() ;
	
	for ( int nCount = 0 ; nCount < m_pServerList->Count() ; nCount ++ )
	{
		m_pServerList->SetItemColor( nCount, 0xffffffff, 0xff00ffff, 0xffffffff ) ;
		if ( m_sUnifiedNation == 0 )
		{
			if ( ( m_SortData[ nCount ][ 0 ].sClass >= 17 && m_SortData[ nCount ][ 0 ].sClass <= 24 ) ||
				 ( m_SortData[ nCount ][ 0 ].sClass == 0 ) )
			{
				m_pServerList->SetItemColor( nCount, 0xff7b7b7b, 0xff00ffff, 0xffffffff ) ;			
			}
		}
		else if ( m_sUnifiedNation == 1 )
		{
			if ( ( m_SortData[ nCount ][ 0 ].sClass >= 1 && m_SortData[ nCount ][ 0 ].sClass <= 12 ) ||
				 ( m_SortData[ nCount ][ 0 ].sClass == 0 ) )
			{
				m_pServerList->SetItemColor( nCount, 0xff7b7b7b, 0xff00ffff, 0xffffffff ) ;			
	        }
		}
	}
}

void CRYLIntegServerSelCharDlg::ResetServerCharInfo() 
{
	for ( int i = 0 ; i < SELECT_CHAR_SLOT ; i ++ )
	{
		m_OldCharSlot[ i ].pSelCharButton->SetVisible( FALSE ) ;
		m_OldCharSlot[ i ].pSelCharButton->SetEnable( FALSE ) ;
		m_OldCharSlot[ i ].pCharNameLabel->SetCaption( "" ) ;
		m_OldCharSlot[ i ].pCharClassLabel->SetCaption( "" ) ;
		m_OldCharSlot[ i ].pCharLevelLabel->SetCaption( "" ) ;
	}
}

void CRYLIntegServerSelCharDlg::SetCharInfo( USER_INFO* UserInfo, UnifiedCharData* CharData, 
											 unsigned long dwCharDataNum_Out, unsigned char cRestrictedPart1ToPart2Level ) 
{
	m_UserInfo = *UserInfo ;
    
    memcpy(m_UnifiedCharData, CharData, sizeof(UnifiedCharData) * min(int(PktUnifiedCharInfo::MAX_CHAR_DATA), int(dwCharDataNum_Out)));
    
	m_dwCharDataNum_Out = dwCharDataNum_Out ;
	m_cRestrictedPart1ToPart2Level = cRestrictedPart1ToPart2Level ;
}

void CRYLIntegServerSelCharDlg::UpdateDlg() 
{
    UnifiedCharData* lpPos = m_UnifiedCharData;
    UnifiedCharData* lpEnd = m_UnifiedCharData + m_dwCharDataNum_Out;
	
	char szBuff[ 64 ] ;

    int iSlot[ 10 ] ;
    memset( iSlot, 0, sizeof( int ) * 10 ) ;

    for (; lpPos != lpEnd; ++lpPos)
	{
		if (UnifiedConst::ROW == lpPos->cOldServerGroupID)
		{
			continue;
		}

        int i = 0;
        for(; i < USER_INFO::MAX_CHAR_NUM; ++i)
        {
            if (lpPos->dwNewCID == m_UserInfo.CharID[i])
            {
                m_SelCharSlot[ i ].bEnable = true ;
                m_SelCharSlot[ i ].pSelCharButton->SetVisible( FALSE ) ;
                m_SelCharSlot[ i ].pCharNameLabel->SetCaption( lpPos->szName ) ;
                sprintf( szBuff, "[PartII]통합서버 - %s", CRYLGameData::Instance()->GetClassName( lpPos->sClass ) ) ;
                m_SelCharSlot[ i ].pCharClassLabel->SetCaption( szBuff ) ;
                sprintf( szBuff, "Lv %d", lpPos->cLevel ) ;
                m_SelCharSlot[ i ].pCharLevelLabel->SetCaption( szBuff ) ;
                
				if (CClass::HUMAN == CClass::GetRace(lpPos->sClass))
				{
					m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2233 ] ) ;
					m_sUnifiedNation = 0 ;
				}
				else if (CClass::AKHAN == CClass::GetRace(lpPos->sClass))
				{
					m_pNationLabel->SetCaption( CRYLStringTable::m_strString[ 2264 ] ) ;
					m_sUnifiedNation = 1 ;
				}

				m_pRaceCBBox->SetVisible( FALSE ) ;
				break;
            }
        }

        if(i == USER_INFO::MAX_CHAR_NUM)
        {
			unsigned char iIdx = (lpPos->cOldServerGroupID == UnifiedConst::Part1Unified) ?
                6 : lpPos->cOldServerGroupID - 1;

            m_SortData[ iIdx ][ iSlot[ iIdx ] ].bEnable = true ;
            strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szName, lpPos->szName ) ;
            strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szClass, CRYLGameData::Instance()->GetClassName( lpPos->sClass ) ) ;
            sprintf( szBuff, "Lv %d", lpPos->cLevel ) ;
            strcpy( m_SortData[ iIdx ][ iSlot[ iIdx ] ].szLevel, szBuff ) ;
            m_SortData[ iIdx ][ iSlot[ iIdx ] ].dwCID = lpPos->dwNewCID ;
            m_SortData[ iIdx ][ iSlot[ iIdx ] ].sClass = lpPos->sClass ;
            m_SortData[ iIdx ][ iSlot[ iIdx ] ].cLevel = lpPos->cLevel ;
            m_SortData[ iIdx ][ iSlot[ iIdx ] ].cOldServerGroupID = lpPos->cOldServerGroupID;
            ++ iSlot[ iIdx ] ;
        }
    }	
}