
#include "RYLRaceBaseDefine.h"
#include "RYLClientMain.h"
#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLSceneManager.h"
// WORK_LIST 2.3 계정 국적 변경 UI 추가
#include "RYLCharacterSelectScene.h"

#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Creature/Character/CharacterClass.h>
#include <RenderOption.h>

#include "GMUIScreen.h"
#include "FrameTimer.h"
#include "BaseDataDefine.h"
#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"
#include "RYLButton.h"
#include "RYLSpriteList.h"
#include "RYLSpriteEx.h"
#include "RYLLabel.h"

#include "RYLCharacterDeleteDlg.h"
#include "RYLChattingDlg.h"
#include "RYLCharacterCreationDlg.h"
#include "RYLCharacterInfoDlg.h"
#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLCharacterInfoDlg*	g_pCharacterInfoDlg = NULL ;

CRYLCharacterInfoDlg::CRYLCharacterInfoDlg () : CRYLDialog()
{
	m_pExitBtn					= NULL ;
	m_pCharNameLabel			= NULL ;	
	m_pCharNameValueLabel		= NULL ;
	m_pCharLevelLabel			= NULL ;
	m_pCharLevelValueLabel		= NULL ;
	m_pCharFameLabel			= NULL ;
	m_pCharFameValueLabel		= NULL ;
	m_pCharClassLabel			= NULL ;
	m_pCharClassValueLabel		= NULL ;
	m_pCharNationLabel			= NULL ;
	m_pCharNationValueLabel		= NULL ;
	m_pCharGuildNameLabel		= NULL ;
	m_pCharGuildNameValueLabel	= NULL ;

	m_dwOverChrID = 0xFFFFFFFF;
	m_dwEndTimer = 0xFFFFFFFF;

	sprintf( m_szNationName[ 0 ], CRYLStringTable::m_strString[2308] ) ;
	sprintf( m_szNationName[ 1 ], CRYLStringTable::m_strString[2233] ) ;
	sprintf( m_szNationName[ 2 ], CRYLStringTable::m_strString[2264] ) ;
	sprintf( m_szNationName[ 3 ], CRYLStringTable::m_strString[2268] ) ;
		
	CRYLGameData::Instance()->m_bSelectChar = FALSE ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 170, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	int i = 0;
	for (  ; i < 33 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ((i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	g_pCharacterInfoDlg = this ;	
	
	SetWidth( 256 ) ;
	SetHeight( 185 ) ;

	SetLeft(g_ClientMain.m_iScreenWidth-GetWidth());
	SetTop(g_ClientMain.m_iScreenHeight-GetHeight());

	
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;

	m_dMsgBox = FALSE ;
}

CRYLCharacterInfoDlg::~CRYLCharacterInfoDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLCharacterInfoDlg::EnableButton( BOOL bEnable ) 
{
	m_pStartBtn->SetVisible( bEnable ) ;
	m_pDeleteBtn->SetVisible( bEnable ) ;
	m_pCancelBtn->SetVisible( bEnable ) ;
	m_pExitBtn->SetVisible( !bEnable ) ;
}

VOID	CRYLCharacterInfoDlg::InitValue()
{
	m_dwOverChrID = 0xFFFFFFFF;
	m_dwEndTimer = 0xFFFFFFFF;
	CRYLGameData::Instance()->m_bSelectChar = FALSE;
}

VOID	CRYLCharacterInfoDlg::InitDeleteValue() 
{
	m_dwEndTimer		= 0xFFFFFFFF;
	CRYLGameData::Instance()->m_bDeleteChar = FALSE;
}

VOID	CRYLCharacterInfoDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pCharInfoLabel = new CRYLLabel( this ) ;
	m_pCharInfoLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 18, 121, 29 ) ;
	m_pCharInfoLabel->SetAlignRect( rt ) ;
	m_pCharInfoLabel->SetAlign( LEFT ) ;
	m_pCharInfoLabel->SetCaption( CRYLStringTable::m_strString[ 1911 ] ) ;
	m_pCharInfoLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharInfoLabel ) ;

	m_pCharNameLabel = new CRYLLabel( this ) ;
	m_pCharNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 48, 121, 60 ) ;
	m_pCharNameLabel->SetAlignRect( rt ) ;
	m_pCharNameLabel->SetAlign( RIGHT ) ;
	m_pCharNameLabel->SetCaption( CRYLStringTable::m_strString[ 1912 ] ) ;
	m_pCharNameLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharNameLabel ) ;

	m_pCharNameValueLabel = new CRYLLabel( this ) ;
	m_pCharNameValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 48, 248, 60 ) ;
	m_pCharNameValueLabel->SetAlignRect( rt ) ;
	m_pCharNameValueLabel->SetAlign( LEFT ) ;
	m_pCharNameValueLabel->SetCaption( "" ) ;
	m_pCharNameValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharNameValueLabel ) ;

	m_pCharLevelLabel = new CRYLLabel( this ) ;
	m_pCharLevelLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 64, 121, 75 ) ;
	m_pCharLevelLabel->SetAlignRect( rt ) ;
	m_pCharLevelLabel->SetAlign( RIGHT ) ;
	m_pCharLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1913 ] ) ;
	m_pCharLevelLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharLevelLabel ) ;

	m_pCharLevelValueLabel = new CRYLLabel( this ) ;
	m_pCharLevelValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 64, 248, 75 ) ;
	m_pCharLevelValueLabel->SetAlignRect( rt ) ;
	m_pCharLevelValueLabel->SetAlign( LEFT ) ;
	m_pCharLevelValueLabel->SetCaption( "" ) ;
	m_pCharLevelValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharLevelValueLabel ) ;

	m_pCharClassLabel = new CRYLLabel( this ) ;
	m_pCharClassLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 80, 121, 91 ) ;
	m_pCharClassLabel->SetAlignRect( rt ) ;
	m_pCharClassLabel->SetAlign( RIGHT ) ;
	m_pCharClassLabel->SetCaption( CRYLStringTable::m_strString[ 1915 ] ) ;
	m_pCharClassLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharClassLabel ) ;

	m_pCharClassValueLabel = new CRYLLabel( this ) ;
	m_pCharClassValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 80, 248, 91 ) ;
	m_pCharClassValueLabel->SetAlignRect( rt ) ;
	m_pCharClassValueLabel->SetAlign( LEFT ) ;
	m_pCharClassValueLabel->SetCaption( "" ) ;
	m_pCharClassValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharClassValueLabel ) ;

	m_pCharFameLabel = new CRYLLabel( this ) ;
	m_pCharFameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 96, 121, 107 ) ;
	m_pCharFameLabel->SetAlignRect( rt ) ;
	m_pCharFameLabel->SetAlign( RIGHT ) ;
	m_pCharFameLabel->SetCaption( CRYLStringTable::m_strString[ 1914 ] ) ;
	m_pCharFameLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharFameLabel ) ;

	m_pCharFameValueLabel = new CRYLLabel( this ) ;
	m_pCharFameValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 96, 248, 107 ) ;
	m_pCharFameValueLabel->SetAlignRect( rt ) ;
	m_pCharFameValueLabel->SetAlign( LEFT ) ;
	m_pCharFameValueLabel->SetCaption( "" ) ;
	m_pCharFameValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharFameValueLabel ) ;

	m_pCharGuildNameLabel = new CRYLLabel( this ) ;
	m_pCharGuildNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 112, 121, 123 ) ;
	m_pCharGuildNameLabel->SetAlignRect( rt ) ;
	m_pCharGuildNameLabel->SetAlign( RIGHT ) ;
	m_pCharGuildNameLabel->SetCaption( CRYLStringTable::m_strString[ 2786 ] ) ;
	m_pCharGuildNameLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharGuildNameLabel ) ;

	m_pCharGuildNameValueLabel = new CRYLLabel( this ) ;
	m_pCharGuildNameValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 112, 248, 123 ) ;
	m_pCharGuildNameValueLabel->SetAlignRect( rt ) ;
	m_pCharGuildNameValueLabel->SetAlign( LEFT ) ;
	m_pCharGuildNameValueLabel->SetCaption( "" ) ;
	m_pCharGuildNameValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharGuildNameValueLabel ) ;

	m_pCharNationLabel = new CRYLLabel( this ) ;
	m_pCharNationLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 128, 121, 139 ) ;
	m_pCharNationLabel->SetAlignRect( rt ) ;
	m_pCharNationLabel->SetAlign( RIGHT ) ;
	m_pCharNationLabel->SetCaption( CRYLStringTable::m_strString[ 2787 ] ) ;
	m_pCharNationLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharNationLabel ) ;

	m_pCharNationValueLabel = new CRYLLabel( this ) ;
	m_pCharNationValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 129, 128, 248, 139 ) ;
	m_pCharNationValueLabel->SetAlignRect( rt ) ;
	m_pCharNationValueLabel->SetAlign( LEFT ) ;
	m_pCharNationValueLabel->SetCaption( "" ) ;
	m_pCharNationValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharNationValueLabel ) ;

	m_pStartBtn = new CRYLButton( this ) ;
	m_pStartBtn->SetLeft( 16 ) ;
	m_pStartBtn->SetTop( 149 ) ;
	m_pStartBtn->SetCaption( CRYLStringTable::m_strString[ 1916 ] ) ;
	m_pStartBtn->SetFontColor( 0xffffffff) ;
	m_pStartBtn->SetColor( 0xffffffff) ;
	m_pStartBtn->OnClickEvent = DlgClick ;
	m_pStartBtn->SetInstance( RYLCHARACTERINFODLG::START_BUTTON ) ;
	m_pStartBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pStartBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pStartBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pStartBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pStartBtn->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pStartBtn ) ;

	m_pDeleteBtn = new CRYLButton( this ) ;
	m_pDeleteBtn->SetLeft( 95 ) ;
	m_pDeleteBtn->SetTop( 149 ) ;
	m_pDeleteBtn->SetCaption( CRYLStringTable::m_strString[ 1917 ] ) ;
	m_pDeleteBtn->SetFontColor( 0xffffffff) ;
	m_pDeleteBtn->SetColor( 0xffffffff) ;
	m_pDeleteBtn->OnClickEvent = DlgClick ;
	m_pDeleteBtn->SetInstance( RYLCHARACTERINFODLG::DELETE_BUTTON ) ;
	m_pDeleteBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDeleteBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDeleteBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDeleteBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDeleteBtn->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pDeleteBtn ) ;

	m_pCancelBtn = new CRYLButton( this ) ;
	m_pCancelBtn->SetLeft( 174 ) ;
	m_pCancelBtn->SetTop( 149 ) ;
	m_pCancelBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelBtn->SetFontColor( 0xffffffff) ;
	m_pCancelBtn->SetColor( 0xffffffff) ;
	m_pCancelBtn->OnClickEvent = DlgClick ;
	m_pCancelBtn->SetInstance( RYLCHARACTERINFODLG::CANCEL_BUTTON ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCancelBtn->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pCancelBtn ) ;

	m_pExitBtn = new CRYLButton( this ) ;
	m_pExitBtn->SetLeft( 179 ) ;
	m_pExitBtn->SetTop( 149 ) ;
	m_pExitBtn->SetCaption( CRYLStringTable::m_strString[ 1918 ] ) ;
	m_pExitBtn->SetFontColor( 0xffffffff) ;
	m_pExitBtn->SetColor( 0xffffffff) ;
	m_pExitBtn->OnClickEvent = DlgClick ;
	m_pExitBtn->SetInstance( RYLCHARACTERINFODLG::EXIT_BUTTON ) ;
	m_pExitBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pExitBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pExitBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pExitBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pExitBtn->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pExitBtn ) ;

	m_pMarkSprite = new CRYLSpriteEx( this ) ;
	m_pMarkSprite->SetLeft( 150 ) ;
	m_pMarkSprite->SetTop( 76 ) ;
	m_pMarkSprite->SetWidth( 12 ) ;
	m_pMarkSprite->SetHeight( 12 ) ;
	AttachGMUIControl( m_pMarkSprite ) ;
}

VOID	CRYLCharacterInfoDlg::FinalCtrl ()
{
	GM_DELETE( m_pExitBtn ) ;
	GM_DELETE( m_pCharNameLabel ) ;
	GM_DELETE( m_pCharInfoLabel ) ;
	GM_DELETE( m_pCharNameValueLabel ) ;
	GM_DELETE( m_pCharLevelLabel ) ;
	GM_DELETE( m_pCharLevelValueLabel ) ;
	GM_DELETE( m_pCharFameLabel ) ;
	GM_DELETE( m_pCharFameValueLabel ) ;
	GM_DELETE( m_pCharClassLabel ) ;
	GM_DELETE( m_pCharClassValueLabel ) ;
	GM_DELETE( m_pCharNationLabel ) ;
	GM_DELETE( m_pCharNationValueLabel ) ;
	GM_DELETE( m_pCharGuildNameLabel ) ;
	GM_DELETE( m_pCharGuildNameValueLabel ) ;

	GM_DELETE( m_pStartBtn ) ;
	GM_DELETE( m_pDeleteBtn ) ;
	GM_DELETE( m_pCancelBtn ) ;
	GM_DELETE( m_pMarkSprite ) ;
}

VOID	CRYLCharacterInfoDlg::Show( CGMUIObject* Sender )
{
	m_pMarkSprite->SetVisible( FALSE ) ;
}

VOID	CRYLCharacterInfoDlg::Hide( CGMUIObject* Sender )
{
	m_pMarkSprite->SetVisible( FALSE ) ;
}

VOID	CRYLCharacterInfoDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT CRYLCharacterInfoDlg::CharacterSelctUpdate()
{
	POINT *ptMousePos	= g_DeviceInput.GetMouseLocal() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	BOOL bIsExist		= FALSE ;
	m_dwOverChrID		= pGame->GetScreenPosChr( *ptMousePos, TARGET_SELECTCHAR, 0 ) ;
	m_dwOverChr			= 0xFFFFFFFF ;

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* ) CRYLSceneManager::Instance()->GetCurrentScene() ;

	if (NULL != GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
	}

	// 캐릭터가 선택되지않은 상태에선 일단 각 정보를 초기화합니다.
	if ( pGame->m_dwSelectChr == 0xFFFFFFFF && m_dwOverChrID == 0xFFFFFFFF)
	{
		m_pCharNameValueLabel->SetCaption( " " ) ;
		m_pCharLevelValueLabel->SetCaption( " " ) ;
		m_pCharFameValueLabel->SetCaption( " " ) ;
		m_pCharClassValueLabel->SetCaption( " " ) ;
		m_pCharNationValueLabel->SetCaption( " " ) ;
		m_pCharGuildNameValueLabel->SetCaption( " " ) ;
		m_pMarkSprite->SetVisible( FALSE ) ;
	}

	if ( m_dwOverChrID != 0xFFFFFFFF )
	{
        for ( int i = 0 ; i < MAX_CHAR_SLOT ; ++i )
        {
            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( i );

            if( m_dwOverChrID == pTempCreature->GetCharID() )
            {
                if ( pGame->m_scLoginChar[ i ].CID ) 
				{
					bIsExist = TRUE ;
				}
				m_dwOverChr = i ;
				break;
			}
		}
	}

	if ( !bIsExist ) 
		m_dwOverChrID = 0xFFFFFFFF ;

	if ( m_dwEndTimer != 0xFFFFFFFF )
	{
		if ( pGame->m_bSelectChar ) 
			return S_OK ;

		if ( CFrameTimer::GetUpdateTimer( m_dwEndTimer ) >= 1.0f )
		{
			if ( pGame->m_scLoginChar[ pGame->m_dwSelectChr].CID )
				pGame->m_dwClientState = CHRSELECT_INITSTART ;
			else
				pGame->m_dwClientState = CHRSELECT_INITCREATE ;

			return S_OK ;
		}

		// WORK_LIST 2.3 계정 국적 변경 UI 추가
		//if ( g_DeviceInput.GetIsLeftMouseDBClick() )
		if ( g_DeviceInput.GetIsLeftMouseDBClick() && !pScene->GetNationSelectDlg()->GetVisible() )
		{
			if ( pGame->m_scLoginChar[ pGame->m_dwSelectChr ].CID )
			{
// WORK_LIST 1.1 종족 선택 화면 다시 추가
//				pGame->m_dwClientRace = pGame->m_scLoginChar[ pGame->m_dwSelectChr ].Race ;
				SendPacket::CharSelect( g_AuthSession, CRYLNetworkData::Instance()->m_dwUserID, 
					                    pGame->m_scLoginChar[ pGame->m_dwSelectChr ].CID ) ;
			}
		}

		return S_OK ;
	}

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	//if ( m_dwOverChr != 0xFFFFFFFF && g_DeviceInput.GetIsLeftMouseDown() )
	if ( m_dwOverChr != 0xFFFFFFFF && g_DeviceInput.GetIsLeftMouseDown() && !pScene->GetNationSelectDlg()->GetVisible() )
	{
		CHAR	strCameraFileName[ MAX_PATH ] ;

		m_dwEndTimer = CFrameTimer::Regist(1.0f);

		pGame->m_dwSelectChr = m_dwOverChr;

		switch ( m_dwOverChr )
		{
			case 0:	strcpy( strCameraFileName, "CharacterSelect1.csf" ) ;   break ;
			case 1:	strcpy( strCameraFileName, "CharacterSelect2.csf" ) ;   break ;
			case 2:	strcpy( strCameraFileName, "CharacterSelect3.csf" ) ;   break ;
			case 3:	strcpy( strCameraFileName, "CharacterSelect4.csf" ) ;   break ;
			case 4:	strcpy( strCameraFileName, "CharacterSelect5.csf" ) ;   break ;
		}

// WORK_LIST 1.1 종족 선택 화면 다시 추가
	

//		pGame->m_dwClientRace = pGame->m_scLoginChar[ pGame->m_dwSelectChr ].Race ;
		pGame->m_lpSceneManager->m_pCameraScript->InitCameraScript( true, 
																	D3DXVECTOR3(138646.98f, 1200.16f, 144921.97f),
																	D3DXVECTOR3(138647.324161f, 1200.136657f, 144922.908621f) ) ;
		
		pGame->m_lpSceneManager->m_pCameraScript->LoadScript( strCameraFileName ) ;
//		pGame->m_lpSceneManager->m_pCameraScript->SetPlay( true, false, true ) ;
		pGame->m_lpSceneManager->m_pCameraScript->SetVecTakePos();          //좌표 보정 추가

		// WORK_LIST 2.3 계정 국적 변경 UI 추가
		CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::GetCurrentScene() ;
		pScene->HideNationDlg() ;
	}

	PrintInfoLabel( m_dwOverChr ) ;

	return S_OK ;
}

HRESULT	CRYLCharacterInfoDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if ( CRYLSceneManager::Instance()->GetSceneID() != CHARACTERSELECT_SCENE )
	{
		return S_OK ;
	}

	if (NULL != GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
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
	
	CRYLGameData*	 pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;
	g_MessageBoxManager.Update() ;

	PrintInfoLabel( pGame->m_dwSelectChr );

	if ( pGame->m_bSelectChar ) 
		return S_OK ;

	if ( m_dwEndTimer != 0xFFFFFFFF )
	{
		if ( CFrameTimer::GetUpdateTimer( m_dwEndTimer ) >= 1.0f )
		{
			pGame->m_dwClientState = CHRSELECT_INITSELECT ;
			g_MessageBoxManager.DestroyList() ;
		}
		return S_OK ;
	}

	if ( g_pClientSocket->GetStatusFlag() != NS_NOWAITING )
		return S_OK ;

	CharacterDelete() ;

	return S_OK ;
}

VOID	CRYLCharacterInfoDlg::PrintInfoLabel( DWORD dwSelectChr )
{
	if ( dwSelectChr != 0xFFFFFFFF )
	{
		CRYLGameData*	 pGame		= CRYLGameData::Instance() ;

		CHAR szBuff[ MAX_PATH ] ;
		m_pCharNameValueLabel->SetCaption( pGame->m_scLoginChar[ dwSelectChr ].Name ) ;
		m_pCharClassValueLabel->SetCaption( pGame->GetClassName( pGame->m_scLoginChar[ dwSelectChr ].Class ) ) ;
		sprintf( szBuff, "%d", pGame->m_scLoginChar[ dwSelectChr ].Level ) ;
		m_pCharLevelValueLabel->SetCaption( szBuff ) ;
		
		// WORK_LIST 2.3 계정 국적 변경 기능 구현
		if ( pGame->m_scLoginChar[ dwSelectChr ].GID )
		{
			m_pCharNationValueLabel->SetCaption( m_szNationName[ pGame->m_scLoginCharGuildData[ dwSelectChr ].m_cNation ] ) ;
			m_pCharGuildNameValueLabel->SetCaption( pGame->m_scLoginCharGuildData[ dwSelectChr ].m_szName ) ;
		}
		else
		{
			m_pCharNationValueLabel->SetCaption( m_szNationName[ pGame->m_cAccountNation ] ) ;
			m_pCharGuildNameValueLabel->SetCaption( "" ) ;
		}
		pGame->m_dwClientSex = pGame->m_scLoginChar[ dwSelectChr ].Sex;
		// TODO : 캐릭터 선택 화면에서 국가(길드)에 대한 정보를 얻기 힘든 관계로 당분간 계급장을 출력하지 않습니다.
		//		추후 캐릭터 선택 화면 리뉴얼시에 다시 고려하도록 합시다.
/*
		unsigned long dwClass = 0;
		unsigned long dwFame  = pGame->m_scLoginChar[ dwSelectChr ].Fame ;

		if (0 < dwFame && dwFame <= 2000)				{ dwClass = 1; 	} 
		else if (2000 < dwFame && dwFame <= 10000)		{ dwClass = 2; 	} 
		else if (10000 < dwFame && dwFame <= 25000)		{ dwClass = 3; 	} 
		else if (25000 < dwFame && dwFame <= 50000)		{ dwClass = 4; 	} 
		else if (50000 < dwFame && dwFame <= 75000)		{ dwClass = 5; 	} 
		else if (75000 < dwFame && dwFame <= 100000)	{ dwClass = 6; 	} 
		else if (100000 < dwFame && dwFame <= 150000)	{ dwClass = 7; 	} 
		else if (150000 < dwFame && dwFame <= 250000)	{ dwClass = 8; 	} 
		else if (250000 < dwFame && dwFame <= 500000)	{ dwClass = 9; 	} 
		else if (500000 < dwFame)						{ dwClass = 10;	} 

		if (0 != dwClass)
		{
			if ( CClass::RaceType::AKHAN == pGame->m_dwClientRace )
			{
				dwClass += 5 ;
			}

			if ( pGame->m_lpInterface->m_lpMark[ ( dwClass - 1) ] )
			{
				m_pMarkSprite->SetVisible( TRUE ) ;
				m_pMarkSprite->AttachImageTexture( pGame->m_lpInterface->m_lpMark[ ( dwClass - 1) ] ) ;
			}
		}
		else
		{
			m_pMarkSprite->SetVisible( FALSE ) ;
		}
*/

		m_pCharFameValueLabel->SetCaption( CRYLStringTable::MakePrintGold( pGame->m_scLoginChar[ dwSelectChr ].Fame ) ) ;
	}
}

VOID	CRYLCharacterInfoDlg::CharacterDelete() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
    if( pGame->m_bDeleteChar )
    {
        if ( g_pClientSocket->GetStatusFlag() == NS_NOWAITING )
		{			
			if ( !pNetwork->m_dwNetworkError )
			{
				CRYLGameData* pGame = CRYLGameData::Instance() ;

                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
                if( !pTempCreature )
                    return;

                CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
                if( !pTempModel )
                    return;
                    
                for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
                     pTempModel2 ;
                     pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
                {
                    if( pTempModel2 == pTempModel )
                    {
                        RYLCreatureManager::Instance()->FreeModel( pTempModel );
						
						pTempCreature->SetModelFileName( "NPC_ChrSelect.GCMDS" ) ;
						pTempCreature->SetHairType( "HAIR01" );
						pTempCreature->SetFaceType( "FACE01" );

                        pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

						const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];
						std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

                        pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

						break;
					}				
				}			

				pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
				pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
				pTempCreature->InitWeapon( 0 ) ;

				CharacterCancelEvent() ;
			} 
			else
			{
				pGame->m_bDeleteChar = FALSE ;
			}
		}
    }
}

HRESULT	CRYLCharacterInfoDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

VOID	CRYLCharacterInfoDlg::CharacterCancelEvent() 
{
	CHAR	strCameraFileName[ MAX_PATH ] ;
	CRYLGameData* pGame = CRYLGameData::Instance();
	g_pCharacterInfoDlg->m_dwEndTimer = CFrameTimer::Regist(1.0f);

	switch ( pGame->m_dwSelectChr )
	{
		case 0:	strcpy( strCameraFileName, "Back_CharacterSelect1.csf" ) ;   break ;
		case 1:	strcpy( strCameraFileName, "Back_CharacterSelect2.csf" ) ;   break ;
		case 2:	strcpy( strCameraFileName, "Back_CharacterSelect3.csf" ) ;   break ;
		case 3:	strcpy( strCameraFileName, "Back_CharacterSelect4.csf" ) ;   break ;
		case 4:	strcpy( strCameraFileName, "Back_CharacterSelect5.csf" ) ;   break ;
	}

	pGame->m_lpSceneManager->m_pCameraScript->InitCameraScript() ;
	pGame->m_lpSceneManager->m_pCameraScript->LoadScript( strCameraFileName ) ;
//	pGame->m_lpSceneManager->m_pCameraScript->SetPlay( true, false, true ) ;
	pGame->m_lpSceneManager->m_pCameraScript->SetVecTakePos();          //좌표 보정 추가
	pGame->m_dwSelectChr = 0xFFFFFFFF;
	pGame->m_dwClientState = CHRSELECT_INITSELECT ;

	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::GetCurrentScene() ;
	pScene->ShowNationDlg() ;

	EnableButton( FALSE );
}


VOID	CRYLCharacterInfoDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pCharacterInfoDlg )//character select choose options hayzohar
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCHARACTERINFODLG::EXIT_BUTTON : 
		{
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
			break ;
		}

		case RYLCHARACTERINFODLG::START_BUTTON  :
		{
			CRYLGameData* pGame = CRYLGameData::Instance() ;
			SendPacket::CharSelect(g_AuthSession, CRYLNetworkData::Instance()->m_dwUserID, 
				pGame->m_scLoginChar[ pGame->m_dwSelectChr ].CID);
			break ;
		}

		case RYLCHARACTERINFODLG::DELETE_BUTTON :
		{
			CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ;
			CRYLCharacterDeleteDlg*	pCharacterDeleteDlg = ( CRYLCharacterDeleteDlg* )pScene->GetCharacterDeleteDlg() ;
			pCharacterDeleteDlg->SetVisible( TRUE ) ;
			break ;
		}

		case RYLCHARACTERINFODLG::CANCEL_BUTTON :
		{
			CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) ;
			CRYLCharacterDeleteDlg*	pCharacterDeleteDlg = ( CRYLCharacterDeleteDlg* )pScene->GetCharacterDeleteDlg() ;
			pCharacterDeleteDlg->SetVisible( FALSE ) ;

			g_pCharacterInfoDlg->CharacterCancelEvent() ;
			break ;
		}
	}
}
